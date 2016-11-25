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
#   include	<appDebugon.h>

#   include	"textConverter.h"
#   include	"textConverterImpl.h"
#   include	<utilIndexMapping.h>
#   include	"uniLegacyEncoding.h"
#   include	"uniLegacyMapping.h"

void textConverterSetNativeEncodingName(	TextConverter *	tc,
						const char *	encodingName )
    {
    if  ( ! encodingName )
	{ XDEB(encodingName); return;	}

    /* Shortcut that also covers the both null case */
    if  ( tc->tcNativeEncodingName == encodingName )
	{ return;	}

    if  ( (   tc->tcNativeEncodingName && ! encodingName )	||
	  ( ! tc->tcNativeEncodingName &&   encodingName )	||
	  strcmp( tc->tcNativeEncodingName, encodingName )	)
	{
	char *		oldName= tc->tcNativeEncodingName;

	if  ( tc->tcIconvToUtf8 != (iconv_t)-1 )
	    {
	    iconv_close( tc->tcIconvToUtf8 );
	    tc->tcIconvToUtf8= (iconv_t)-1;
	    }

	if  ( tc->tcIconvFrUtf8 != (iconv_t)-1 )
	    {
	    iconv_close( tc->tcIconvFrUtf8 );
	    tc->tcIconvFrUtf8= (iconv_t)-1;
	    }

	if  ( encodingName )
	    { tc->tcNativeEncodingName= strdup( encodingName );	}
	else{ tc->tcNativeEncodingName= (char *)0;		}

	if  ( oldName )
	    { free( oldName );	}
	}

    return;
    }

void utilInitTextConverter(	TextConverter *		tc )
    {
    tc->tcNativeEncodingName= (char *)0;
    tc->tcIconvToUtf8= (iconv_t)-1;
    tc->tcIconvFrUtf8= (iconv_t)-1;
    }

void utilCleanTextConverter(	TextConverter *		tc )
    {
    if  ( tc->tcNativeEncodingName )
	{ free( tc->tcNativeEncodingName );	}

    if  ( tc->tcIconvToUtf8 != (iconv_t)-1 )
	{
	iconv_close( tc->tcIconvToUtf8 );
	tc->tcIconvToUtf8= (iconv_t)-1;
	}

    if  ( tc->tcIconvFrUtf8 != (iconv_t)-1 )
	{
	iconv_close( tc->tcIconvFrUtf8 );
	tc->tcIconvFrUtf8= (iconv_t)-1;
	}
    }

/************************************************************************/
/*									*/
/*  Hack because iconv does not support the symbol encoding.		*/
/*  Convert native bytes to UTF-8.					*/
/*									*/
/************************************************************************/

static int textConverterConvertBytesToUtf8(
					const int		unicodes[256],
					void *			through,
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    unsigned char	scratch[750];
    int			buffered= 0;
    int			shift;
    const int		scratchLen= sizeof(scratch)- 7;
    int			consumed= 0;

    while( len > 0 )
	{
	if  ( buffered >= scratchLen )
	    {
	    shift= (*produce)( through, produced, (char *)scratch, buffered );
	    if  ( shift < 0 )
		{ LLLDEB(produced,buffered,shift); return -1;	}
	    produced += shift; buffered= 0;
	    }

	if  ( unicodes[*text&0xff] < 0 )
	    {
	    shift= uniPutUtf8( scratch+ buffered, *text&0xff );	
	    }
	else{
	    shift= uniPutUtf8( scratch+ buffered, unicodes[*text&0xff] );
	    }
	if  ( shift < 1 )
	    { LDEB(shift); return -1;	}

	text++; len--; consumed++; buffered += shift;
	}

    if  ( buffered > 0 )
	{
	shift= (*produce)( through, produced, (char *)scratch, buffered );
	if  ( shift < 0 )
	    { LLLDEB(produced,buffered,shift); return -1;	}
	produced += shift; buffered= 0;
	}

    *pConsumed += consumed;
    return produced;
    }

/************************************************************************/
/*									*/
/*  Hack because iconv does not support the symbol encoding.		*/
/*  Convert UTF-8 to native.						*/
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
    int			shift;
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
	    shift= (*produce)( through, produced, (char *)scratch, done );
	    if  ( shift < 0 )
		{ LLLDEB(produced,done,shift); return -1;	}
	    produced += shift; done= 0;
	    }

	shift= uniGetUtf8( &symbol, (unsigned char *)text );
	if  ( shift < 1 )
	    { LDEB(shift); return -1;	}

	code= utilIndexMappingGetU( im, symbol );
	if  ( code < 0 )
	    { break;	}

	scratch[done++]= code;
	text += shift; len -= shift; consumed += shift;
	}

    if  ( done > 0 )
	{
	shift= (*produce)( through, produced, (char *)scratch, done );
	if  ( shift < 0 )
	    { LLLDEB(produced,done,shift); return -1;	}
	produced += shift; done= 0;
	}

    *pConsumed += consumed;
    return produced;
    }

/************************************************************************/
/*									*/
/*  Convert bytes using iconv. This is used in both directions.		*/
/*									*/
/************************************************************************/

static int textConverterConvertIconv(
					iconv_t			ico,
					void *			through,
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		arg_ibuf,
					size_t			ileft )
    {
    char		scratch[750];
    char *		obuf= scratch;
    size_t		oleft= sizeof(scratch);
    int			consumed= 0;

#   ifdef __GNUC__
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

	    (void)iconv( ico, &ibuf, &ileft, &obuf, &oleft );

	    if  ( ibuf == iibuf )
		{ /*XXDEB(ibuf,iibuf);*/ break;	}

	    consumed += ibuf- iibuf;
	    iibuf= ibuf;
	    }

	if  ( obuf > scratch )
	    {
	    int	shift;

	    shift= (*produce)( through, produced, scratch, obuf- scratch );
	    if  ( shift < 0 )
		{ LLLDEB(produced,obuf- scratch,shift); return -1;	}

	    produced += shift;
	    obuf= scratch;
	    oleft= sizeof(scratch);
	    }

	if  ( ileft > 0 && errno == EILSEQ )
	    { /*LSCDEB(errno,strerror(errno),*ibuf);*/ break;	}
	if  ( ileft > 0 && errno != E2BIG )
	    { LSDEB(errno,strerror(errno)); return -1;	}
	}

    *pConsumed += consumed;
    return produced;
    }

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
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    if  ( tc				&&
	  tc->tcNativeEncodingName	&&
	  tc->tcNativeEncodingName[0]	)
	{
	if  ( tc->tcIconvToUtf8 == (iconv_t)-1 )
	    {
	    tc->tcIconvToUtf8= iconv_open( "UTF-8", tc->tcNativeEncodingName );

	    if  ( tc->tcIconvToUtf8 == (iconv_t)-1			&&
		  ! strcmp( tc->tcNativeEncodingName, "SYMBOL" )	)
		{
		return textConverterConvertBytesToUtf8(
						uniSymbolGlyphUnicodes,
						through, produce, pConsumed,
						produced, text, len );
		}

	    if  ( tc->tcIconvToUtf8 == (iconv_t)-1			&&
		  ! strcmp( tc->tcNativeEncodingName, "DINGBATS" )	)
		{
		return textConverterConvertBytesToUtf8(
						uniDingbatsGlyphUnicodes,
						through, produce, pConsumed,
						produced, text, len );
		}

	    if  ( tc->tcIconvToUtf8 == (iconv_t)-1 )
		{
		SXDEB(tc->tcNativeEncodingName,tc->tcIconvToUtf8);
		return -1;
		}
	    }

	produced= textConverterConvertIconv( tc->tcIconvToUtf8,
						through, produce, pConsumed,
						produced, (char *)text, len );
	if  ( produced < 0 )
	    { LDEB(produced); return -1;	}
	}
    else{
	int	shift;

	shift= (*produce)( through, produced, text, len );
	if  ( shift < 0 )
	    { LLLDEB(produced,len,shift); return -1;	}

	produced += shift;
	*pConsumed += len;
	}

    return produced;
    }

/************************************************************************/
/*									*/
/*  Convert the UTF-8 input to native bytes.				*/
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
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len )
    {
    if  ( tc				&&
	  tc->tcNativeEncodingName	&&
	  tc->tcNativeEncodingName[0]	)
	{
	if  ( tc->tcIconvFrUtf8 == (iconv_t)-1 )
	    {
	    tc->tcIconvFrUtf8= iconv_open( tc->tcNativeEncodingName, "UTF-8" );

	    if  ( tc->tcIconvFrUtf8 == (iconv_t)-1			&&
		  ! strcmp( tc->tcNativeEncodingName, "SYMBOL" )	)
		{
		return textConverterConvertBytesFromUtf8(
						&UNI_SymbolToGlyphMapping,
						uniSymbolGlyphUnicodes,
						through, produce, pConsumed,
						produced, text, len );
		}

	    if  ( tc->tcIconvFrUtf8 == (iconv_t)-1			&&
		  ! strcmp( tc->tcNativeEncodingName, "DINGBATS" )	)
		{
		return textConverterConvertBytesFromUtf8(
						&UNI_DingbatsToGlyphMapping,
						uniDingbatsGlyphUnicodes,
						through, produce, pConsumed,
						produced, text, len );
		}

	    if  ( tc->tcIconvFrUtf8 == (iconv_t)-1 )
		{
		SXDEB(tc->tcNativeEncodingName,tc->tcIconvFrUtf8);
		return -1;
		}
	    }

	produced= textConverterConvertIconv( tc->tcIconvFrUtf8,
						through, produce, pConsumed,
						produced, (char *)text, len );
	if  ( produced < 0 )
	    { LDEB(produced); return -1;	}
	}
    else{
	int	shift;

	shift= (*produce)( through, produced, text, len );
	if  ( shift < 0 )
	    { LLLDEB(produced,len,shift); return -1;	}

	produced += shift;
	*pConsumed += len;
	}

    return produced;
    }

