/************************************************************************/
/*									*/
/*  Manage the actual string content while reading an RTF document.	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<uniUtf8.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<textConverter.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docBuf.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle text..							*/
/*									*/
/*  1)  Ignore it.							*/
/*  2)  Refuse it.							*/
/*  3)  Save it for later use. (Convert it to UTF-8 on the fly).	*/
/*									*/
/************************************************************************/

/*  1  */
int docRtfIgnoreText(	RtfReader *		rr,
			const char *		text,
			int			len )
    { return 0; }

/*  2  */
int docRtfRefuseText(	RtfReader *		rr,
			const char *		text,
			int			len )
    { LDEB(1); return -1; }

/************************************************************************/
/*									*/
/*  Save text: It is not encoded.					*/
/*									*/
/************************************************************************/

int docRtfSaveRawBytes(		RtfReader *		rr,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( utilMemoryBufferAppendBytes( &(rrs->rrsSavedText),
					(const unsigned char *)text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert text from some encoding to UTF-8.				*/
/*									*/
/************************************************************************/

static int docRtfSaveBytes(	void *		vmb,
				int		offset,
				const char *	bytes,
				int		count )
    {
    MemoryBuffer *	mb= (MemoryBuffer *)vmb;

    if  ( utilMemoryBufferAppendBytes( mb,
				    (const unsigned char *)bytes, count ) )
	{ LDEB(mb->mbSize); return -1;	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Save text: It is in the document encoding. (I.E. the encoding of	*/
/*  the document file that is not necessarily identical to the encoding	*/
/*  of the current text font.						*/
/*									*/
/************************************************************************/

int docRtfSaveDocEncodedText(	RtfReader *		rr,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rr->rrState;
    int			upto;
    int			consumed= 0;

    upto= textConverterConvertToUtf8( rr->rrRtfTextConverter,
				    (void *)&(rrs->rrsSavedText),
				    &consumed,
				    rrs->rrsSavedText.mbSize, text, len );
    if  ( upto < 0 )
	{ LDEB(upto); return -1;	}

    return 0;
    }

/************************************************************************/

int docRtfReadSetupTextConverters(	RtfReader *	rr )
    {
    rr->rrRtfTextConverter= textOpenTextConverter();
    if  ( ! rr->rrRtfTextConverter )
	{ PDEB(rr->rrRtfTextConverter); return -1;	}

    textConverterSetNativeEncodingName( rr->rrRtfTextConverter,
						    DOC_RTF_AnsiCharsetName );

    textConverterSetProduce( rr->rrRtfTextConverter, docRtfSaveBytes );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Store the text bytes that we collected from the rtf file in some	*/
/*  location. Use realloc() to rezize the target location and flush	*/
/*  the collected text bytes.						*/
/*									*/
/************************************************************************/

int docRtfStoreSavedText(	char **		pTarget,
				int *		pSize,
				RtfReader *	rr,
				int		removeSemicolon )
    {
    RtfReadingState *	rrs= rr->rrState;

    char *	fresh;
    int		size;

    if  ( utilMemoryBufferIsEmpty( &(rrs->rrsSavedText) ) )
	{ *pSize= 0; return 0;	}

    size= rrs->rrsSavedText.mbSize;
    fresh= (char *)realloc( *pTarget, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh, rrs->rrsSavedText.mbBytes, size );
    fresh[size]= '\0';

    if  ( removeSemicolon		&&
	  size > 0			&&
	  fresh[size- 1] == ';'		)
	{ fresh[--size]= '\0';	}

    utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );

    *pTarget= fresh;
    *pSize= size;
    return 0;
    }

int docRtfMemoryBufferSetText(	MemoryBuffer *		mb,
				RtfReader *		rr,
				int			removeSemicolon )
    {
    int		rval= 0;
    char *	text= (char *)0;
    int		size= 0;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(size); rval= -1; goto ready;	}

  ready:
    if  ( text )
	{ free( text );		}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Append saved text to a memory buffer.				*/
/*									*/
/************************************************************************/

int docRtfMemoryBufferAppendText(	MemoryBuffer *		mb,
					RtfReader *		rr )
    {
    int		rval= 0;
    const int	removeSemicolon= 0;
    char *	text= (char *)0;
    int		size= 0;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferAppendBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:
    if  ( text )
	{ free( text );		}

    return rval;
    }

/************************************************************************/

static int docRtfReadAdaptToFontEncoding(
				RtfReader *			rr,
				RtfReadingState *		rrs )
    {
    const char *	encodingName= (const char *)0;
    RtfTreeStack *	rts= rr->rrTreeStack;

    if  ( rrs->rrsTextCharset >= 0 )
	{
	encodingName= docGetEncodingName( rr->rrDocument,
			    &(rrs->rrsTextAttribute), rrs->rrsTextCharset );
	}

    if  ( ! encodingName )
	{
	encodingName= textConverterGetNativeEncodingName(
						rr->rrRtfTextConverter );
	}

    docParagraphBuilderSetNativeEncodingName( rts->rtsParagraphBuilder,
								encodingName );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert particules from the input in the document.			*/
/*									*/
/************************************************************************/

int docRtfGotText(	RtfReader *		rr,
			const char *		text,
			int			len )
    {
    RtfReadingState *		rrs= rr->rrState;
    RtfTreeStack *		rts= rr->rrTreeStack;
    struct BufferItem *		paraNode;
    int				paraStrlen;

    if  ( rr->rrInIgnoredGroup )
	{ return 0;	}

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ XDEB(paraNode); return -1; }

    paraStrlen= docParaStrlen( paraNode );
    if  ( paraStrlen == 0				||
	  paraStrlen == rr->rrParaHeadFieldTailOffset	)
	{
	if  ( docRtfAdaptToParaProperties( rr ) )
	    { LDEB(1); return -1;	}
	/*
	 * No need to update rrParaHeadFieldTailOffset, as we append text after
	 * the head field anyway
	 */
	}

    if  ( docRtfReadAdaptToFontEncoding( rr, rrs ) )
	{ LDEB(1);	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    if  ( docParagraphBuilderAppendText( rts->rtsParagraphBuilder,
					&(rrs->rrsTextAttribute), text, len ) )
	{ LDEB(1); return -1;	}

    docRtfResetParagraphReadingState( rr );
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle an explicit unicode.						*/
/*  Special characters.							*/
/*									*/
/************************************************************************/

static int docRtfTextUnicodeValue(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;

    char		bytes[7];

    if  ( arg < 0 )
	{ arg += 65536;	}

    /* Dirty HACK: Only use low byte of characters in the unicode private range */
    if  ( arg >= 0xE000 && arg <= 0xF8FF )
	{
	bytes[0]= arg & 0xff;
	bytes[1]= '\0';

	return docRtfSaveDocEncodedText( rr, bytes, 1 );
	}

    if  ( rr->rrGotText == docRtfSaveRawBytes )
	{ XXDEB(rr->rrGotText,docRtfSaveRawBytes); return 0; }

    if  ( rr->rrGotText == docRtfGotText )
	{
	struct BufferItem *	paraNode= docRtfGetParaNode( rr );

	if  ( ! paraNode )
	    { SXDEB(rcw->rcwWord,paraNode); return -1; }

	if  ( rrs->rrsTextShadingChanged )
	    { docRtfRefreshTextShading( rr, rrs );	}

	if  ( docParagraphBuilderAppendSymbol( rts->rtsParagraphBuilder, 
					&(rrs->rrsTextAttribute), arg ) )
	    { LDEB(arg); return -1;	}
	}
    else{
	int	count;

	count= uniPutUtf8( bytes, arg );
	if  ( count < 1 )
	    { LLDEB(arg,count); return 0;	}

	if  ( utilMemoryBufferAppendBytes( &(rrs->rrsSavedText),
					(unsigned char *)bytes, count ) )
	    { LDEB(count); return -1;	}
	}

    return 0;
    }

int docRtfTextUnicode(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( docRtfTextUnicodeValue( rcw, arg, rr ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    rrs->rrsUnicodeBytesToSkip= rrs->rrsBytesPerUnicode;
    return 0;
    }

int docRtfTextSpecialChar(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    /* docRtfGotText() adjusts level */

    if  ( docRtfTextUnicodeValue( rcw, rcw->rcwID, rr ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

int docRtfTextSpecialParticule(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;
    struct BufferItem *	paraNode;

    if  ( rr->rrInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ SXDEB(rcw->rcwWord,paraNode); return -1; }

    switch( rcw->rcwID )
	{
	case TPkindTAB:
	case TPkindLINEBREAK:
	case TPkindCHFTNSEP:
	case TPkindCHFTNSEPC:
	case TPkindOPT_HYPH:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    if  ( docParaBuilderAppendSpecialParticule(
				rts->rtsParagraphBuilder,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    docRtfResetParagraphReadingState( rr );

	    break;

	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

