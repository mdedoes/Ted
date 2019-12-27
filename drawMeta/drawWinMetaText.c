#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"drawWinMetaImpl.h"
#   include	<sioEndian.h>
#   include	<uniUtf8.h>
#   include	<textConverter.h>
#   include	<sioGeneral.h>
#   include	<textOfficeCharset.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

int appWinMetaReadUtf16Text(	DeviceContext *		dc,
				int			expectChars,
				SimpleInputStream *	sis )
    {
    int		nchars;
    int		nbytes;
    int		done= 0;

    utilEmptyMemoryBuffer( &(dc->dcCollectedText) );

    nbytes= 0;
    for ( nchars= 0; nchars < expectChars; nchars++ )
	{
	int		step;
	char		scratch[6+1];
	int		c= sioEndianGetLeUint16( sis ); done += 2;

	if  ( c == 0 )
	    { nchars++; break;	}

	step= uniPutUtf8( scratch, c );
	if  ( step < 1 || step > 3 )
	    { XLDEB(c,step); break;	}

	if  ( utilMemoryBufferAppendString( &(dc->dcCollectedText), scratch ) )
	    { LDEB(step); return -1;	}

	nbytes += step;
	}

    while( nchars < expectChars )
	{ sioEndianGetLeInt16( sis ); done += 2; nchars++;	}

    return done;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int appWinMetaCollectTextUtf8(	void *		voiddc,
					int		produced,
					const char *	bytes,
					int		count )
    {
    DeviceContext *	dc= (DeviceContext *)voiddc;

    if  ( utilMemoryBufferAppendBytes( &(dc->dcCollectedText),
					    (unsigned char *)bytes, count ) )
	{ LDEB(count); return -1;	}

    return count;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int appWinMetaReadLegacyText(	DeviceContext *		dc,
				int			count,
				int			expectBytes,
				SimpleInputStream *	sis )
    {
    int				rval= 0;
    int				nbytes;
    int				done= 0;
    char *			buf= (char *)0;
    int				idx;

    const char *		encodingName= "CP1252";

    utilEmptyMemoryBuffer( &(dc->dcCollectedText) );

    {
    const OfficeCharset *	oc;

    oc= textGetOfficeCharsetByCharset( &idx, dc->dcFont.lfCharSet );

    if  ( oc && oc->ocEncodingName )
	{ encodingName= oc->ocEncodingName;	}
    else{
	if  ( dc->dcFont.lfCharSet != FONTcharsetDEFAULT )
	    { LPDEB(dc->dcFont.lfCharSet,oc);	}
	}
    }

    {
    int	consumed= 0;
    int	produced= 0;

    /* Do at setup time! */
    if  ( ! dc->dcTextConverter )
	{
	dc->dcTextConverter= textOpenTextConverter();
	if  ( ! dc->dcTextConverter )
	    { XDEB(dc->dcTextConverter);	}
	else{
	    textConverterSetProduce( dc->dcTextConverter,
						appWinMetaCollectTextUtf8 );
	    }
	}

    textConverterSetNativeEncodingName( dc->dcTextConverter, encodingName );

    buf= (char *)malloc( count+ 1 );
    if  ( ! buf )
	{ LPDEB(count,buf); rval= -1; goto ready;	}

    if  ( sioInReadBytes( sis, (unsigned char *)buf, count ) != count )
	{ LDEB(count); rval= -1; goto ready;	}
    nbytes= count; done += count;
    buf[count]= '\0';

    if  ( ! dc->dcTextConverter )
	{ XDEB(dc->dcTextConverter);	}
    else{
	textConverterConvertToUtf8( dc->dcTextConverter, (void *)dc,
					    &consumed, produced, buf, count );
	if  ( consumed != count )
	    { SLLDEB(encodingName,consumed,count);	}
	}
    }

    while( nbytes < expectBytes )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	done += 1; nbytes++;
	}

    rval= done;

  ready:

    if  ( buf )
	{ free( buf );	}

    return rval;
    }

