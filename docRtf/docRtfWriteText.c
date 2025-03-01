/************************************************************************/
/*									*/
/*  Manage the actual string content while reading an RTF document.	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<fontMatchFont.h>
#   include	<uniUtf8.h>
#   include	<textConverter.h>
#   include	<textConverterImpl.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfFlags.h"
#   include	<fontEncodedFont.h>
#   include	<docBuf.h>
#   include	<sioGeneral.h>
#   include	<fontDocFont.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Emit a string to the RTF output stream.				*/
/*									*/
/************************************************************************/

static int docRtfEscapeString(	void *			vrw,
				int			produced, /* ignored */
				const char *		ss,
				int			n )
    {
    RtfWriter *			rw= (RtfWriter *)vrw;
    const unsigned char *	us= (const unsigned char *)ss;
    SimpleOutputStream *	sos= rw->rwSosOut;
    int				i;
    int				addSpace= 0;

    if  ( n == 0 )
	{ return n;	}

    switch( rw->rwAfter )
	{
	case RTFafterTAG:
	    if  ( ss[0] == ' '		||
		  ss[0] == '-'		||
		  ss[0] == '_'		||
		  isalnum( ss[0] )	)
		{ addSpace= 1;	}
	    break;

	case RTFafterARG:
	    if  ( ss[0] == ' ' || isdigit( ss[0] ) )
		{ addSpace= 1;	}
	    break;

	case RTFafterTEXT:
	    break;

	default:
	    CDEB(rw->rwAfter); return -1;
	}


    if  ( addSpace )
	{
	if  ( sioOutPutByte( ' ', rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}

	rw->rwCol += 1;
	}
    rw->rwAfter= RTFafterTEXT;

    i= 0;
    while( i < n )
	{
	int		c= *us;

	switch( c )
	    {
	    case '{': case '\\': case '}':
		if  ( sioOutPutByte( '\\', sos ) < 0 )
		    { LDEB(1); return -1;	}
		if  ( sioOutPutByte( c, sos ) < 0 )
		    { LDEB(1); return -1;	}
		rw->rwCol += 2;
		break;
	    default:
		if  ( c < 32 || c > 127 )
		    {
		    static const char	xdigs[]= "0123456789abcdef";

		    if  ( sioOutPutByte( '\\', sos ) < 0 )
			{ LDEB(1); return -1;	}
		    if  ( sioOutPutByte( '\'', sos ) < 0 )
			{ LDEB(1); return -1;	}
		    if  ( sioOutPutByte( xdigs[ ( c >> 4 ) & 0x0f ], sos ) < 0 )
			{ LDEB(1); return -1;	}
		    if  ( sioOutPutByte( xdigs[ ( c >> 0 ) & 0x0f ], sos ) < 0 )
			{ LDEB(1); return -1;	}
		    rw->rwCol += 4;
		    }
		else{
		    if  ( sioOutPutByte( c, sos ) < 0 )
			{ LDEB(1); return -1;	}
		    rw->rwCol += 1;
		    }
		break;
	    }

	i++; us++;
	}

    return n;
    }

/************************************************************************/
/*									*/
/*  Emit a unicode (UTF-16) character as a tag.				*/
/*									*/
/************************************************************************/

static int docRtfEmitUnicode(		RtfWriter *		rw,
					int			symbol )
    {
    if  ( symbol > 32767 && symbol < 65536 )
	{ symbol -= 65536;	}

    docRtfWriteArgTag( rw, "u", symbol );

    if  ( rw->rwUnicodeSubstituteCount > 0 )
	{
	if  ( symbol >= 0 && symbol < 255 )
	    {
	    char	s[2];

	    s[0]= symbol & 0xff; s[1]= '\0';

	    docRtfEscapeString( (void *)rw, 0, s, 1 );
	    }
	else{
	    docRtfEscapeString( (void *)rw, 0, "?", 1 );
	    }

	for ( int i= 1; i < rw->rwUnicodeSubstituteCount; i++ )
	    { docRtfEscapeString( (void *)rw, 0, "?", 1 );	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit an utf-8 string in some legacy encoding.			*/
/*  The string might very well contain unicodes that cannot be		*/
/*  represented in the legacy encoding. The encoder stops on those	*/
/*  bytes and we emit them in \u1234 format.				*/
/*									*/
/************************************************************************/

static int docRtfWriteEncodedString(	RtfWriter *		rw,
					TextConverter *		tc,
					int			fontEncoded,
					const char *		ss,
					int			n )
    {
    int		produced= 0;

    while( n > 0 )
	{
	int		consumed= 0;

	produced= textConverterConvertFromUtf8( tc, (void *)rw,
						&consumed, produced, ss, n );
	if  ( produced < 0 )
	    { LDEB(produced); return -1;	}

	ss += consumed; n -= consumed;

	if  ( n > 0 )
	    {
	    unsigned short	symbol;

	    consumed= uniGetUtf8( &symbol, ss );
	    if  ( consumed < 1 )
		{ LDEB(consumed); return -1;	}

	    if  ( fontEncoded )
		{
		struct BufferDocument *	bd= rw->rwDocument;
		const DocumentFont *	df;
		TextAttribute *		ta= &(rw->rwTextAttribute);

		df= docGetFontOfAttribute( bd, ta );
		if  ( df )
		    {
		    int			fontNumber;
		    int			charset= FONTcharsetDEFAULT;
		    const char *	encodingName;

		    fontNumber= docRtfWriteGetCharset( rw, &charset,
								df, symbol );
		    if  ( fontNumber >= 0 && rw->rwTextCharset != charset )
			{
			docRtfWriteArgTag( rw, "f", fontNumber );

			encodingName= fontGetEncodingName( &(df->dfName),
								    charset );
			textConverterSetNativeEncodingName(
				    rw->rwTextTextConverter, encodingName );
			rw->rwTextCharset= charset;
			continue;
			}
		    }
		}

	    docRtfEmitUnicode( rw, symbol );

	    ss += consumed; n -= consumed;
	    }
	}

    return 0;
    }

int docRtfWriteDocEncodedString(	RtfWriter *		rw,
					const char *		s,
					int			n )
    {
    const int	fontEncoded= 0;

    return docRtfWriteEncodedString( rw, rw->rwRtfTextConverter,
							fontEncoded, s, n );
    }

int docRtfWriteFontEncodedString(	RtfWriter *		rw,
					const char *		ss,
					int			n )
    {
    const int			fontEncoded= 1;
    const char *		encodingName= (const char *)0;

    if  ( rw->rwSaveFlags & RTFflagUNENCODED )
	{
	return docRtfWriteDocEncodedString( rw, ss, n );
	}

    encodingName= docGetEncodingName( rw->rwDocument,
				&(rw->rwTextAttribute), rw->rwTextCharset );
    if  ( ! encodingName )
	{
	encodingName= rw->rwRtfTextConverter->tcNativeEncodingName;
	}

    textConverterSetNativeEncodingName(
			rw->rwTextTextConverter, encodingName );

    return docRtfWriteEncodedString( rw, rw->rwTextTextConverter,
							fontEncoded, ss, n );
    }

void docRtfWriteRawBytes(	RtfWriter *		rw,
				const char *		ss,
				int			n )
    {
    docRtfEscapeString( (void *)rw, 0, ss, n );
    }


/************************************************************************/

void docRtfWriteSetupTextConverters(	RtfWriter *	rw )
    {
    textConverterSetNativeEncodingName(
			    rw->rwRtfTextConverter,
			    DOC_RTF_AnsiCharsetName );

    textConverterSetProduce( rw->rwRtfTextConverter, docRtfEscapeString );
    textConverterSetProduce( rw->rwTextTextConverter, docRtfEscapeString );
    }

