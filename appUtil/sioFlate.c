/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in In/Deflate format	*/
/*  using zlib.								*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<zlib.h>

#   include	"sioFlate.h"
#   include	"sioEndian.h"
#   include	"sioGeneral.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Input steam for flate decompression using zlib.			*/
/*									*/
/************************************************************************/

#   define	FLATE_IN_LEN	512

typedef struct FlateInputStream
    {
    z_stream		fisZstream;
    SimpleInputStream *	fisSisFlate;

    unsigned char	fisInputBuffer[FLATE_IN_LEN];
    int			fisInputPosition;
    int			fisInputCapacity;
    int			fisExhausted;
    } FlateInputStream;

/************************************************************************/
/*									*/
/*  Decompress an input stream using zlib.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  The Flate decompression routine:					*/
/*									*/
/************************************************************************/

static int sioInFlateReadBytes(	void *			voidfis,
				unsigned char *		buffer,
				unsigned int		count )
    {
    FlateInputStream *		fis= (FlateInputStream *)voidfis;
    z_stream *			d_stream= &(fis->fisZstream);

    int				ret;
    int				done= 0;

    if  ( fis->fisExhausted )
	{ return 0;	}

    d_stream->next_out= buffer;
    d_stream->avail_out= (unsigned int)count;

    while( done < count )
	{
	int	todo;

	if  ( fis->fisInputPosition < fis->fisInputCapacity )
	    {
	    todo= fis->fisInputCapacity- fis->fisInputPosition;

	    d_stream->next_in= fis->fisInputBuffer+ fis->fisInputPosition;
	    d_stream->avail_in= todo;
	    }
	else{
	    todo= sioInReadBytes( fis->fisSisFlate,
					fis->fisInputBuffer, FLATE_IN_LEN );
	    if  ( todo <= 0 )
		{ fis->fisExhausted= 2; break;	}

	    fis->fisInputPosition= 0;
	    fis->fisInputCapacity= todo;

	    d_stream->next_in= fis->fisInputBuffer;
	    d_stream->avail_in= todo;
	    }

	ret= inflate( d_stream, Z_NO_FLUSH );
	if  ( ret != Z_OK )
	    {
	    if  ( ret == Z_STREAM_END )
		{
		done= count- d_stream->avail_out;
		fis->fisExhausted= 1;
		break;
		}
	    else{ LDEB(ret); return -1;		}
	    }

	fis->fisInputPosition += todo- d_stream->avail_in;
	done= count- d_stream->avail_out;
	}

    return done;
    }


/************************************************************************/
/*									*/
/*  Close a Flate Input stream.						*/
/*									*/
/*  1)  First drain it.							*/
/*									*/
/************************************************************************/

static int sioInFlateClose(	void *	voidfis )
    {
    int				rval= 0;
    int				ret;

    FlateInputStream *		fis= (FlateInputStream *)voidfis;
    z_stream *			d_stream= &(fis->fisZstream);

    /*  1 */
    while ( ! fis->fisExhausted )
	{
	unsigned char	buffer[SIOsizBUF];

	if  ( sioInFlateReadBytes( voidfis, buffer, SIOsizBUF ) < 0 )
	    { LDEB(1); rval= -1; break;	}
	}

    ret= inflateEnd( d_stream );
    if  ( ret != Z_OK )
	{ LDEB(ret); rval= -1;	}

    free( voidfis );

    return rval;
    }

SimpleInputStream * sioInFlateOpen(	SimpleInputStream *	sisFlate )
    {
    SimpleInputStream *		sis;
    FlateInputStream *		fis;
    z_stream *			d_stream;

    int				ret;

    fis= (FlateInputStream *)malloc( sizeof(FlateInputStream) );
    if  ( ! fis )
	{ XDEB(fis); return (SimpleInputStream *)0;	}

    fis->fisSisFlate= sisFlate;
    fis->fisInputPosition= 0;
    fis->fisInputCapacity= 0;
    fis->fisExhausted= 0;

    d_stream= &(fis->fisZstream);

    d_stream->zalloc = (alloc_func)0;
    d_stream->zfree = (free_func)0;
    d_stream->opaque = (voidpf)0;

    ret= inflateInit( d_stream );
    if  ( ret != Z_OK )
	{ LDEB(ret); free( fis ); return (SimpleInputStream *)0;	}

    sis= sioInOpen( (void *)fis, sioInFlateReadBytes, sioInFlateClose );

    if  ( ! sis )
	{
	XDEB(sis);
	inflateEnd( d_stream ); free( fis );
	return (SimpleInputStream *)0;
	}

    return sis;
    }

/************************************************************************/
/*									*/
/*  SimpleOutputStreams doing Flate compression.			*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Actual Output stream object.					*/
/*									*/
/************************************************************************/

typedef struct FlateOutputStream
    {
    z_stream			fosZstream;
    unsigned char		fosOutputBuffer[SIOsizBUF];
    SimpleOutputStream *	fosSosFlate;
    int				fosGzipEmbedded;

    unsigned long		fosAdlerCrc;
    unsigned long		fosUncompressedSize;
    } FlateOutputStream;

/************************************************************************/
/*									*/
/*  The Flate compression routine:					*/
/*									*/
/************************************************************************/

static int sioOutFlateFlushBytes(	FlateOutputStream *	fos,
					int			n )
    {
    z_stream *			c_stream= &(fos->fosZstream);
    const unsigned char *	b= fos->fosOutputBuffer;

    if  ( n > 0 && sioOutWriteBytes( fos->fosSosFlate, b, n ) != n )
	{ LDEB(n); return -1; }

    c_stream->next_out= fos->fosOutputBuffer;
    c_stream->avail_out= SIOsizBUF;

    return n;
    }

static int sioOutFlateWriteBytes(	void *			voidfos,
					const unsigned char *	buffer,
					int			count )
    {
    FlateOutputStream *		fos= (FlateOutputStream *)voidfos;
    z_stream *			c_stream= &(fos->fosZstream);

    c_stream->next_in= (unsigned char *)buffer;
    c_stream->avail_in= count;

    fos->fosUncompressedSize += count;
    if  ( fos->fosGzipEmbedded )
	{ fos->fosAdlerCrc= crc32( fos->fosAdlerCrc, buffer, count );	}

    while( c_stream->avail_in > 0 )
	{
	int		ret;

	if  ( c_stream->avail_out == 0 )
	    {
	    if  ( sioOutFlateFlushBytes( fos, SIOsizBUF ) != SIOsizBUF )
		{ LDEB(SIOsizBUF); return -1;	}
	    }

	ret= deflate( c_stream, Z_NO_FLUSH );
	if  ( ret != Z_OK )
	    { LDEB(ret); return -1;	}
	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Close a Flate output stream.					*/
/*									*/
/************************************************************************/

static int sioOutFlateClose(	void *		voidfos )
    {
    int				rval= 0;
    int				ret;

    FlateOutputStream *		fos= (FlateOutputStream *)voidfos;
    z_stream *			c_stream= &(fos->fosZstream);

    c_stream->next_in= (unsigned char *)0;
    c_stream->avail_in= 0;

    for (;;)
	{
	if  ( c_stream->avail_out == 0 )
	    {
	    if  ( sioOutFlateFlushBytes( fos, SIOsizBUF ) != SIOsizBUF )
		{ LDEB(SIOsizBUF); return -1;	}
	    }

	ret= deflate( c_stream, Z_FINISH );
	if  ( ret == Z_STREAM_END )
	    { break;	}
	if  ( ret != Z_OK )
	    { LDEB(ret); rval= -1; break;	}
	}

    if  ( c_stream->avail_out < SIOsizBUF )
	{
	int	n= SIOsizBUF- c_stream->avail_out;

	if  ( sioOutFlateFlushBytes( fos, n ) != n )
	    { LDEB(n); return -1;	}
	}

    ret= deflateEnd( c_stream );
    if  ( ret != Z_OK )
	{ LDEB(ret); rval= -1;	}

    if  ( fos->fosGzipEmbedded )
	{
	sioEndianPutLeInt32( fos->fosAdlerCrc, fos->fosSosFlate );
	sioEndianPutLeInt32( fos->fosUncompressedSize, fos->fosSosFlate );
	}

    free( fos );

    return rval;
    }

static int sioFlateWriteGzipHeader(	SimpleOutputStream *	sosFlate )
    {
    if  ( sioOutPutByte( 0x1f, sosFlate ) < 0	||	/*  magic 0	*/
	  sioOutPutByte( 0x8b, sosFlate ) < 0	||	/*  magic 1	*/
	  sioOutPutByte( Z_DEFLATED, sosFlate ) < 0 ||	/*  method	*/
	  sioOutPutByte( 0x00, sosFlate ) < 0	||	/*  flags	*/

	  sioEndianPutLeInt32( 0L, sosFlate ) < 0 ||	/*  time	*/

	  sioOutPutByte( 0x00, sosFlate ) < 0	||	/*  xfl		*/

	  sioOutPutByte( 0x03, sosFlate ) < 0	)	/*  os is unix	*/
	{ return -1;	}

    return 0;
    }

SimpleOutputStream * sioOutFlateOpen(	SimpleOutputStream *	sosFlate,
					int			gzipEmbedded )
    {
    SimpleOutputStream *	sos;
    FlateOutputStream *		fos;
    z_stream *			c_stream;

    int				ret;

    fos= (FlateOutputStream *)malloc( sizeof(FlateOutputStream) );
    if  ( ! fos )
	{ XDEB(fos); return (SimpleOutputStream *)0;	}

    fos->fosSosFlate= sosFlate;
    fos->fosGzipEmbedded= gzipEmbedded;

    c_stream= &(fos->fosZstream);

    c_stream->zalloc = (alloc_func)0;
    c_stream->zfree = (free_func)0;
    c_stream->opaque = (voidpf)0;

    fos->fosAdlerCrc= 0;
    fos->fosUncompressedSize= 0;

    if  ( gzipEmbedded )
	{
	fos->fosAdlerCrc= crc32( 0L, Z_NULL, 0 );

	ret= deflateInit2( c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			    -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY );
	if  ( ret != Z_OK )
	    { LDEB(ret); free( fos ); return (SimpleOutputStream *)0;	}
	}
    else{
	ret= deflateInit( c_stream, Z_DEFAULT_COMPRESSION );
	if  ( ret != Z_OK )
	    { LDEB(ret); free( fos ); return (SimpleOutputStream *)0;	}
	}

    c_stream->next_in= (unsigned char *)0;
    c_stream->avail_in= 0;

    c_stream->next_out= fos->fosOutputBuffer;
    c_stream->avail_out= SIOsizBUF;

    sos= sioOutOpen( (void *)fos, sioOutFlateWriteBytes, sioOutFlateClose );

    if  ( ! sos )
	{
	XDEB(sos);
	deflateEnd( c_stream ); free( fos );
	return (SimpleOutputStream *)0;
	}

    if  ( fos->fosGzipEmbedded )
	{ sioFlateWriteGzipHeader( fos->fosSosFlate );	}

    return sos;
    }

