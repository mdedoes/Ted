#   include	"bitmapConfig.h"

#   if USE_LIBWEBP

#   include	<stdlib.h>
#   include	<stddef.h>

#   include	"bmformats.h"
#   include	<sioFileio.h>
#   include	<sioGeneral.h>

#   include	<webp/decode.h>
#   include	<webp/encode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read WEBP image content						*/
/*									*/
/************************************************************************/

static int bmWebpReadWebp(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis,
				int *			pPrivateFormat )
    {
    int			rval= 0;
    WebPDecoderConfig	config;
    WebPIDecoder *	idec= (WebPIDecoder *)0;

    unsigned char	bytes[1000];
    int			done;
    VP8StatusCode	status;

    unsigned char *	buffer= (unsigned char *)0;

    if  ( ! WebPInitDecoderConfig(&config) )
	{ LDEB(1); rval= -1; goto ready;	}

    done= sioInReadBytes( sis, bytes, sizeof(bytes) );
    if  ( done < 0 )
	{ LDEB(done); rval= -1; goto ready;	}
    status= WebPGetFeatures( bytes, done, &(config.input) );
    if  ( status != VP8_STATUS_OK )
	{ LDEB(status); rval= -1; goto ready;	}

    if  ( config.input.has_alpha )
	{
	config.output.colorspace= MODE_RGBA;
	config.output.u.RGBA.stride= 4* config.input.width;
	config.output.u.RGBA.size=
			config.input.height* config.output.u.RGBA.stride;
	}
    else{
	config.output.colorspace= MODE_RGB;
	config.output.u.RGBA.stride= 3* config.input.width;
	config.output.u.RGBA.size=
			config.input.height* config.output.u.RGBA.stride;
	}

    buffer= malloc( config.output.u.RGBA.size );
    if  ( ! buffer )
	{ LXDEB(config.output.u.RGBA.size,buffer); rval= -1; goto ready; }

    config.output.u.RGBA.rgba= buffer;
    config.output.is_external_memory= 1;

    idec= WebPINewDecoder( &(config.output) );
    if  ( ! idec )
	{ XDEB(idec); rval= -1; goto ready;	}

    for (;;)
	{
	status= WebPIAppend( idec, bytes, done);
	if  ( status != VP8_STATUS_OK && status != VP8_STATUS_SUSPENDED )
	    { LDEB(status); rval= -1; goto ready;	}
	if  ( status == VP8_STATUS_OK )
	    { break;	}

	if  ( done < sizeof(bytes) )
	    { LLDEB(done,sizeof(bytes)); rval= -1; goto ready;	}

	done= sioInReadBytes( sis, bytes, sizeof(bytes) );
	if  ( done < 1 )
	    { LDEB(done); rval= -1; goto ready;	}
	}

    bd->bdPixelsWide= config.input.width;
    bd->bdPixelsHigh= config.input.height;
    bd->bdColorEncoding= BMcoRGB;
    bd->bdBitsPerSample= 8;

    bd->bdXResolution= 1;
    bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    bd->bdBytesPerRow= config.output.u.RGBA.stride;
    bd->bdBufferLength= config.output.u.RGBA.size;

    if  ( config.input.has_alpha )
	{
	bd->bdHasAlpha= 1;
	bd->bdSamplesPerPixel= 4;
	bd->bdBitsPerPixel= 32;
	}
    else{
	bd->bdHasAlpha= 0;
	bd->bdSamplesPerPixel= 3;
	bd->bdBitsPerPixel= 24;
	}

    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
    *pPrivateFormat= config.input.format;

  ready:

    if  ( idec )
	{ WebPIDelete( idec );	}

    WebPFreeDecBuffer(&config.output);

    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an image from a WEBP input stream.				*/
/*									*/
/************************************************************************/

int bmReadWebpFile(	const struct MemoryBuffer *	filename,
			unsigned char **		pBuffer,
			BitmapDescription *		bd,
			int *				pPrivateFormat )
    {
    SimpleInputStream *	sis;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    if  ( bmWebpReadWebp( bd, pBuffer, sis, pPrivateFormat ) )
	{ LDEB(1); sioInClose( sis ); return -1; }

    sioInClose( sis );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Can write a bitmap to a PNG file?					*/
/*  All images can be written to png.					*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int bmCanWriteWebpFile( const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdColorEncoding != BMcoRGB		||
	  bd->bdBitsPerSample != 8			)
	{ return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int bmWebpWriteWebp(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos,
				int				privateFormat )
    {
    int			rval= 0;

    size_t		size= 0;
    unsigned char *	output= (unsigned char *)0;

    if  ( bd->bdColorEncoding != BMcoRGB		||
	  bd->bdBitsPerSample != 8			)
	{
	LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
	rval= -1; goto ready;
	}

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB:
	    if  ( bd->bdHasAlpha )
		{
		switch( privateFormat )
		    {
		    case 1:
			size= WebPEncodeRGBA( buffer,
					bd->bdPixelsWide, bd->bdPixelsHigh,
					bd->bdBytesPerRow, 75.0, &output );
			if  ( size == 0 )
			    {
			    LLLDEB(bd->bdColorEncoding,privateFormat,size);
			    rval= -1; goto ready;
			    }
			break;

		    case 2:
			size= WebPEncodeLosslessRGBA( buffer,
					bd->bdPixelsWide, bd->bdPixelsHigh,
					bd->bdBytesPerRow, &output );
			if  ( size == 0 )
			    {
			    LLLDEB(bd->bdColorEncoding,privateFormat,size);
			    rval= -1; goto ready;
			    }
			break;

		    default:
			LLDEB(bd->bdColorEncoding,privateFormat);
			rval= -1; goto ready;
		    }
		}
	    else{
		switch( privateFormat )
		    {
		    case 1:
			size= WebPEncodeRGB( buffer,
					bd->bdPixelsWide, bd->bdPixelsHigh,
					bd->bdBytesPerRow, 75.0, &output );
			if  ( size == 0 )
			    {
			    LLLDEB(bd->bdColorEncoding,privateFormat,size);
			    rval= -1; goto ready;
			    }
			break;

		    case 2:
			size= WebPEncodeLosslessRGB( buffer,
					bd->bdPixelsWide, bd->bdPixelsHigh,
					bd->bdBytesPerRow, &output );
			if  ( size == 0 )
			    {
			    LLLDEB(bd->bdColorEncoding,privateFormat,size);
			    rval= -1; goto ready;
			    }
			break;

		    default:
			LLDEB(bd->bdColorEncoding,privateFormat);
			rval= -1; goto ready;
		    }
		}

	    break;

	default:
	    LDEB(bd->bdColorEncoding); rval= -1; goto ready;
	}

    if  ( sioOutWriteBytes( sos, output, size ) != size )
	{ LDEB(size); rval= 0; goto ready;	}

  ready:

    if  ( output )
	{ free( output );	}

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int bmWriteWebpFile(	const struct MemoryBuffer *	filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( bmWebpWriteWebp( bd, buffer, sos, privateFormat ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

# endif
