/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8 using iconv.				*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<errno.h>

#   include	"uniUtf8.h"

#   include	"textConverter.h"
#   include	"textConverterImpl.h"
#   include	<utilIndexMapping.h>
#   include	"uniLegacyEncoding.h"
#   include	"uniLegacyMapping.h"

#   if	USE_LIBICONV
#	include	<iconv.h>
#   endif

#   include	<appDebugon.h>

#   if	USE_LIBICONV
#	define	ICONV_NONE	((struct TextConverterImpl *)(iconv_t)-1)
#   else
#	define	ICONV_NONE	((struct TextConverterImpl *)-1)
#   endif

typedef struct TextconverterCodePage
    {
    const char * const		tcpName;
    const int * const		tcpGlyphUnicodes;
    IndexMapping * const	tcpUnicodeToGlyphMapping;
    } TextconverterCodePage;

const TextconverterCodePage	TEXT_CONVERTER_FALLBACK_PAGES[] =
{
    { "SYMBOL",
	    uniSymbolGlyphUnicodes,
	    &UNI_SymbolToGlyphMapping,		},
    { "DINGBATS",
	    uniDingbatsGlyphUnicodes,
	    &UNI_DingbatsToGlyphMapping,	},
    { "CP1250",
	    uniWinCp1250GlyphUnicodes,
	    &UNI_CP1250ToGlyphMapping		},
    { "CP1251",
	    uniWinCp1251GlyphUnicodes,
	    &UNI_CP1251ToGlyphMapping		},
    { "CP1252", /* IS A MUST-SUPPORT as CP1252 is the default RTF encoding */
	    uniWinCp1252GlyphUnicodes,
	    &UNI_CP1252ToGlyphMapping		},
    { "CP1253",
	    uniWinCp1253GlyphUnicodes,
	    &UNI_CP1253ToGlyphMapping		},
    { "CP1254",
	    uniWinCp1254GlyphUnicodes,
	    &UNI_CP1254ToGlyphMapping		},
};

static const TextconverterCodePage * textConverterGetCodePageByName(
					const char *	name )
    {
    const int	n= sizeof(TEXT_CONVERTER_FALLBACK_PAGES)/
				    sizeof(TextconverterCodePage);
    int		i;

    for ( i= 0; i < n; i++ )
	{
	const	TextconverterCodePage *	tcp= TEXT_CONVERTER_FALLBACK_PAGES+ i;

	if  ( ! strcmp( tcp->tcpName, name ) )
	    { return tcp;	}
	}

    SDEB(name);
    return (TextconverterCodePage *)0;
    }

const char * textConverterGetNativeEncodingName(	TextConverter *	tc )
    {
    return tc->tcNativeEncodingName;
    }

int textConverterSetNativeEncodingName(	TextConverter *	tc,
					const char *	encodingName )
    {
    if  ( ! encodingName )
	{ XDEB(encodingName); return -1;	}

#   if ! USE_LIBICONV
    if  ( encodingName && ! strcmp( encodingName, "UTF-8" ) )
	{ encodingName= (const char *)0;	}
#   endif

    /* Shortcut that also covers the both null case */
    if  ( tc->tcNativeEncodingName == encodingName )
	{ return 0;	}

    if  ( (   tc->tcNativeEncodingName && ! encodingName )	||
	  ( ! tc->tcNativeEncodingName &&   encodingName )	||
	  strcmp( tc->tcNativeEncodingName, encodingName )	)
	{
	char *		oldName= tc->tcNativeEncodingName;

#	if USE_LIBICONV
	if  ( tc->tcIconvToUtf8 != ICONV_NONE )
	    {
	    iconv_close( (iconv_t)tc->tcIconvToUtf8 );
	    tc->tcIconvToUtf8= ICONV_NONE;
	    }

	if  ( tc->tcIconvFrUtf8 != ICONV_NONE )
	    {
	    iconv_close( (iconv_t)tc->tcIconvFrUtf8 );
	    tc->tcIconvFrUtf8= ICONV_NONE;
	    }
#	endif

	if  ( encodingName )
	    { tc->tcNativeEncodingName= strdup( encodingName );	}
	else{ tc->tcNativeEncodingName= (char *)0;		}

	if  ( oldName )
	    { free( oldName );	}
	}

    return 0;
    }

static void textInitTextConverter(	TextConverter *		tc )
    {
    tc->tcNativeEncodingName= (char *)0;
    tc->tcIconvToUtf8= ICONV_NONE;
    tc->tcIconvFrUtf8= ICONV_NONE;

    tc->tcProduce= (TextConverterProduce)0;
    }

TextConverter * textOpenTextConverter( void )
    {
    TextConverter *	tc= malloc( sizeof(TextConverter) );

    if  ( ! tc )
	{ PDEB(tc); return (TextConverter *)0;	}

    textInitTextConverter( tc );

    return tc;
    }

static void textCleanTextConverter(	TextConverter *		tc )
    {
    if  ( tc->tcNativeEncodingName )
	{ free( tc->tcNativeEncodingName );	}

#   if USE_LIBICONV
    if  ( tc->tcIconvToUtf8 != ICONV_NONE )
	{
	iconv_close( (iconv_t)tc->tcIconvToUtf8 );
	tc->tcIconvToUtf8= ICONV_NONE;
	}

    if  ( tc->tcIconvFrUtf8 != ICONV_NONE )
	{
	iconv_close( (iconv_t)tc->tcIconvFrUtf8 );
	tc->tcIconvFrUtf8= ICONV_NONE;
	}
#   endif
    }

void textCloseTextConverter(	struct TextConverter *	tc )
    {
    textCleanTextConverter( tc );
    free( tc );
    return;
    }

/************************************************************************/

static int textConverterProduce(	TextConverter *		tc,
					void *			through,
					int			produced,
					const char *		text,
					int			len )
    {
    int		step;

    if  ( ! tc->tcProduce )
	{ XDEB(tc->tcProduce); return -1;	}

    step= (*tc->tcProduce)( through, produced, text, len );
    if  ( step < 0 )
	{ LLLDEB(produced,len,step); return -1;	}

    return step;
    }

/************************************************************************/
/*									*/
/*  Hack because iconv does not support the symbol encoding.		*/
/*  Convert legacy bytes to UTF-8.					*/
/*									*/
/************************************************************************/

static int textConverterConvertBytesToUtf8(
					TextConverter *		tc,
					const int		unicodes[256],
					void *			through,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    char		scratch[750];
    int			buffered= 0;
    int			step;
    const int		scratchLen= sizeof(scratch)- 7;
    int			consumed= 0;

    while( len > 0 )
	{
	int	unicode;

	if  ( buffered >= scratchLen )
	    {
	    step= textConverterProduce( tc, through, produced, scratch, buffered );
	    if  ( step < 0 )
		{ LLLDEB(produced,buffered,step); return -1;	}
	    produced += step; buffered= 0;
	    }

	unicode= unicodes[*text&0xff];
	if  ( unicode < 0 )
	    {
	    if  ( *text == '\0' )
		{
		scratch[buffered]= '\0';
		step= 1;
		}
	    else{
		step= uniPutUtf8( scratch+ buffered, *text&0xff );	
		if  ( step < 1 )
		    { XLDEB(*text,step); return -1;	}
		}
	    }
	else{
	    step= uniPutUtf8( scratch+ buffered, unicodes[*text&0xff] );
	    if  ( step < 1 )
		{ XXLDEB(*text,unicodes[*text&0xff],step); return -1;	}
	    }

	text++; len--; consumed++; buffered += step;
	}

    if  ( buffered > 0 )
	{
	step= textConverterProduce( tc, through, produced, scratch, buffered );
	if  ( step < 0 )
	    { LLLDEB(produced,buffered,step); return -1;	}
	produced += step; buffered= 0;
	}

    *pConsumed += consumed;
    return produced;
    }

/************************************************************************/
/*									*/
/*  Hack because iconv does not support the symbol encoding.		*/
/*  Convert UTF-8 to legacy.						*/
/*									*/
/************************************************************************/

static int textConverterConvertBytesFromUtf8(
					IndexMapping *		im,
					const int		unicodes[256],
					void *			through,
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    unsigned char	scratch[750];
    int			done= 0;
    int			step;
    const int		scratchLen= sizeof(scratch);
    int			consumed= 0;

    if  ( utilIndexMappingIsEmpty( im )				&&
	  unicodes						&&
	  utilIndexMappingBuildBackward( im, unicodes, 256 )	)
	{ XDEB(unicodes); return -1;	}

    while( len > 0 )
	{
	unsigned short			symbol;
	int				code;

	if  ( done >= scratchLen )
	    {
	    step= (*produce)( through, produced, (char *)scratch, done );
	    if  ( step < 0 )
		{ LLLDEB(produced,done,step); return -1;	}
	    produced += step; done= 0;
	    }

	step= uniGetUtf8( &symbol, text );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	code= utilIndexMappingGetU( im, symbol );
	if  ( code < 0 )
	    { break;	}

	scratch[done++]= code;
	text += step; len -= step; consumed += step;
	}

    if  ( done > 0 )
	{
	step= (*produce)( through, produced, (char *)scratch, done );
	if  ( step < 0 )
	    { LLLDEB(produced,done,step); return -1;	}
	produced += step; done= 0;
	}

    *pConsumed += consumed;
    return produced;
    }

/************************************************************************/
/*									*/
/*  Convert bytes using iconv. This is used in both directions.		*/
/*									*/
/*  Note that GNU iconv() expects a 'char **' as its second argument	*/
/*  rather than a 'const char **' as documented in the single UNIX spec.*/
/*  See: http://www.opengroup.org/pubs/online/7908799/xsh/iconv.html.	*/
/*									*/
/************************************************************************/

# if USE_LIBICONV

static int textConverterConvertIconv(	TextConverter *		tc,
					struct TextConverterImpl *	tci,
					void *			through,
					int *			pConsumed,
					int			produced,
					const char *		arg_ibuf,
					size_t			ileft )
    {
    char		scratch[750];
    char *		obuf= scratch;
    size_t		oleft= sizeof(scratch);
    int			consumed= 0;

    iconv_t		ico= (iconv_t)tci;

#   if defined(__GNUC__) && ! defined(iconv)
    char *		ibuf= (char *)arg_ibuf;
#   else
    const char *	ibuf= arg_ibuf;
#   endif

    while( ileft > 0 )
	{
	const char *	iibuf= ibuf;

	while( oleft > 0 && ileft > 0 )
	    {
	    /*  return value is irrelevant: Just tells that the	*/
	    /*  conversion was incomplete. So does ileft > 0.	*/
	    iconv( ico, &ibuf, &ileft, &obuf, &oleft );

	    if  ( ibuf == iibuf )
		{ /*XXDEB(ibuf,iibuf);*/ break;	}

	    consumed += ibuf- iibuf;
	    iibuf= ibuf;
	    }

	if  ( obuf > scratch )
	    {
	    int	step;

	    step= textConverterProduce( tc, through, produced, 
						    scratch, obuf- scratch );
	    if  ( step < 0 )
		{ LLLDEB(produced,obuf- scratch,step); return -1;	}

	    produced += step;
	    obuf= scratch;
	    oleft= sizeof(scratch);
	    }

	if  ( ileft > 0 && errno == EILSEQ )
	    { /*LSCDEB(errno,strerror(errno),*ibuf);*/ break;	}
	if  ( ileft > 0 && errno != E2BIG )
	    { LSDEB(errno,strerror(errno)); return -1;	}
	}

    /* flush: E.G: CP1255 is stateful */
    ileft= 0;
    obuf= scratch;
    oleft= sizeof(scratch);
    iconv( ico, NULL, &ileft, &obuf, &oleft );
    if  ( obuf > scratch )
	{
	int	step;

	step= textConverterProduce( tc, through, produced, 
						scratch, obuf- scratch );
	if  ( step < 0 )
	    { LLLDEB(produced,obuf- scratch,step); return -1;	}

	produced += step;
	}

    *pConsumed += consumed;
    return produced;
    }
# endif

/************************************************************************/
/*									*/
/*  Convert the input to UTF-8 bytes.					*/
/*									*/
/*  Note that GNU iconv() expects a 'char **' as its second argument	*/
/*  rather than a 'const char **' as documented in the single UNIX spec.*/
/*  See: http://www.opengroup.org/pubs/online/7908799/xsh/iconv.html.	*/
/*									*/
/*  Also note that iconv_open() returns an (iconv_t)-1 rather than	*/
/*  (iconv_t)0 on failure as is to be expected for a call that returns	*/
/*  a (void *) pointer.							*/
/*									*/
/************************************************************************/

int textConverterConvertToUtf8(	TextConverter *		tc,
				void *			through,
				int *			pConsumed,
				int			produced,
				const char *		text,
				int			len )
    {
    if  ( ! tc )
	{ XDEB(tc); return -1;	}

    if  ( tc->tcNativeEncodingName	&&
	  tc->tcNativeEncodingName[0]	)
	{
	if  ( tc->tcIconvToUtf8 == ICONV_NONE )
	    {
#	    if  USE_LIBICONV
	    tc->tcIconvToUtf8= (struct TextConverterImpl *)
			iconv_open( "UTF-8", tc->tcNativeEncodingName );
#	    endif

	    if  ( tc->tcIconvToUtf8 == ICONV_NONE )
		{
		const TextconverterCodePage * const	tcp=
		    textConverterGetCodePageByName( tc->tcNativeEncodingName );

		if  ( tcp )
		    {
		    return textConverterConvertBytesToUtf8( tc,
					    tcp->tcpGlyphUnicodes,
					    through, pConsumed,
					    produced, text, len );
		    }

		SXDEB(tc->tcNativeEncodingName,tc->tcIconvToUtf8);
		return -1;
		}
	    }

#	if USE_LIBICONV
	produced= textConverterConvertIconv( tc, tc->tcIconvToUtf8,
					    through, pConsumed,
					    produced, (char *)text, len );
	if  ( produced < 0 )
	    { LDEB(produced); return -1;	}
#	else
	SDEB(tc->tcNativeEncodingName); return -1;
#	endif
	}
    else{
	int	step;

	step= textConverterProduce( tc, through, produced, text, len );
	if  ( step < 0 )
	    { LLLDEB(produced,len,step); return -1;	}

	produced += step;
	*pConsumed += len;
	}

    return produced;
    }

/************************************************************************/
/*									*/
/*  Convert the UTF-8 input to legacy bytes.				*/
/*									*/
/*  Note that GNU iconv() expects a 'char **' as its second argument	*/
/*  rather than a 'const char **' as documented in the single UNIX spec.*/
/*  See: http://www.opengroup.org/pubs/online/7908799/xsh/iconv.html.	*/
/*									*/
/*  Also note that iconv_open() returns an (iconv_t)-1 rather than	*/
/*  (iconv_t)0 on failure as is to be expected for a call that returns	*/
/*  a (void *) pointer.							*/
/*									*/
/************************************************************************/

int textConverterConvertFromUtf8(	TextConverter *		tc,
					void *			through,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    if  ( ! tc )
	{ XDEB(tc); return -1;	}

    if  ( tc->tcNativeEncodingName	&&
	  tc->tcNativeEncodingName[0]	)
	{
	if  ( tc->tcIconvFrUtf8 == ICONV_NONE )
	    {
#	    if  USE_LIBICONV
	    tc->tcIconvFrUtf8= (struct TextConverterImpl *)
			    iconv_open( tc->tcNativeEncodingName, "UTF-8" );
#	    endif

	    if  ( tc->tcIconvFrUtf8 == ICONV_NONE			&&
		  ! strcmp( tc->tcNativeEncodingName, "DINGBATS" )	)
		{
#		if 1
		/*  Emit the character as a \u12345 unicode */
		return 0;
#		else
		return textConverterConvertBytesFromUtf8(
					    &UNI_DingbatsToGlyphMapping,
					    uniDingbatsGlyphUnicodes,
					    through, tc->tcProduce, pConsumed,
					    produced, text, len );
#		endif
		}

	    if  ( tc->tcIconvFrUtf8 == ICONV_NONE )
		{
		const TextconverterCodePage * const	tcp=
		    textConverterGetCodePageByName( tc->tcNativeEncodingName );

		if  ( tcp )
		    {
		    return textConverterConvertBytesFromUtf8(
					    tcp->tcpUnicodeToGlyphMapping,
					    tcp->tcpGlyphUnicodes,
					    through, tc->tcProduce, pConsumed,
					    produced, text, len );
		    }

		SXDEB(tc->tcNativeEncodingName,tc->tcIconvFrUtf8);
		return -1;
		}
	    }

#	if USE_LIBICONV
	produced= textConverterConvertIconv( tc, tc->tcIconvFrUtf8,
					    through, pConsumed,
					    produced, text, len );
	if  ( produced < 0 )
	    { LDEB(produced); return -1;	}
#	else
	SDEB(tc->tcNativeEncodingName); return -1;
#	endif
	}
    else{
	int	step;

	step= textConverterProduce( tc, through, produced, text, len );
	if  ( step < 0 )
	    { LLLDEB(produced,len,step); return -1;	}

	produced += step;
	*pConsumed += len;
	}

    return produced;
    }

void textConverterSetProduce(	struct TextConverter *	tc,
				TextConverterProduce	produce )
    {
    tc->tcProduce= produce;
    }
