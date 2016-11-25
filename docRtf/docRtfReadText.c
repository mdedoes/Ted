/************************************************************************/
/*									*/
/*  Manage the actual string content while reading an RTF document.	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<uniUtf8.h>
#   include	<ucd.h>

#   include	"docRtfReaderImpl.h"
#   include	<docParaString.h>
#   include	<docParaParticules.h>

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
int docRtfIgnoreText(	RtfReader *	rrc,
			const char *		text,
			int			len )
    { return 0; }

/*  2  */
int docRtfRefuseText(	RtfReader *	rrc,
			const char *		text,
			int			len )
    { LDEB(1); return -1; }

/************************************************************************/
/*									*/
/*  Save text: It is not encoded.					*/
/*									*/
/************************************************************************/

int docRtfSaveRawBytes(		RtfReader *	rrc,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rrc->rrcState;

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

    if  ( utilMemoryBufferAppendBytes( mb, (const unsigned char *)bytes, count ) )
	{ LDEB(mb->mbSize); return -1;	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Save text: It is in the document encoding.				*/
/*									*/
/************************************************************************/

int docRtfSaveDocEncodedText(	RtfReader *	rrc,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    int			upto;
    int			consumed= 0;

    upto= textConverterConvertToUtf8(
				    &(rrc->rrcTextConverters.rtcRtfConverter),
				    (void *)&(rrs->rrsSavedText),
				    docRtfSaveBytes, &consumed,
				    rrs->rrsSavedText.mbSize, text, len );
    if  ( upto < 0 )
	{ LDEB(upto); return -1;	}

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
				RtfReader *	rrc,
				int		removeSemicolon )
    {
    RtfReadingState *	rrs= rrc->rrcState;

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
				RtfReader *		rrc,
				int			removeSemicolon )
    {
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    if  ( utilMemoryBufferSetBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(1); return -1;	}

    if  ( text )
	{ free( text );		}

    return 0;
    }

int docRtfMemoryBufferAppendText(	MemoryBuffer *		mb,
					RtfReader *		rrc )
    {
    const int	removeSemicolon= 0;
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    if  ( utilMemoryBufferAppendBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(1); return -1;	}

    if  ( text )
	{ free( text );		}

    return 0;
    }

/************************************************************************/

static int docRtfReadAdaptToFontEncoding(
				RtfReader *			rrc,
				RtfReadingState *		rrs )
    {
    const char *		encodingName= (const char *)0;

    if  ( rrs->rrsTextCharset >= 0 )
	{
	encodingName= docGetEncodingName( rrc->rrcDocument,
			    &(rrs->rrsTextAttribute), rrs->rrsTextCharset );
	}

    if  ( ! encodingName )
	{ encodingName= docRtfReadGetRtfEncodingName( rrc );	}

    docRtfReadSetTextEncodingName( rrc, encodingName );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert particules from the input in the document.			*/
/*									*/
/************************************************************************/

int docRtfTextParticule(	RtfReader *		rrc,
				const char *		text,
				int			len )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    BufferDocument *		bd= rrc->rrcDocument;
    BufferItem *		paraNode;

    if  ( rrc->rrcInIgnoredGroup )
	{ return 0;	}

    paraNode= docRtfGetParaNode( rrc );
    if  ( ! paraNode )
	{ XDEB(paraNode); return -1; }

    if  ( rrc->rrcAtParaHead )
	{
	int		mindTable= 1;

	if  ( docRtfSetParaProperties(  &(paraNode->biParaProperties),
							bd, mindTable, rrs ) )
	    { LDEB(1); return -1;	}

	rrc->rrcAtParaHead= 0;
	}

    if  ( docRtfReadAdaptToFontEncoding( rrc, rrs ) )
	{ LDEB(1);	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rrc, rrs );	}

    if  ( docSaveParticules( bd, paraNode, &(rrs->rrsTextAttribute),
		    &(rrc->rrcTextConverters.rtcTextConverter), text, len ) )
	{ LDEB(1); return -1;	}

    rrc->rrcAfterNoteref= 0;
    rrc->rrcAtParaHead= 0;
    
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
					RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    unsigned char	bytes[7];
    int			count;

    if  ( arg < 0 )
	{ arg += 65536;	}

    /* Dirty HACK */
    if  ( ucdIsPrivate( arg ) )
	{
	bytes[0]= arg & 0xff;
	bytes[1]= '\0';

	return docRtfSaveDocEncodedText( rrc, (char *)bytes, 1 );
	}

    count= uniPutUtf8( bytes, arg );
    if  ( count < 1 )
	{ LDEB(count); return 0;	}

    if  ( rrc->rrcAddParticule == docRtfSaveRawBytes )
	{ XXDEB(rrc->rrcAddParticule,docRtfSaveRawBytes); return 0; }

    if  ( rrc->rrcAddParticule == docRtfTextParticule )
	{
	int			stroffShift= 0;
	int			stroff;
	BufferItem *		paraNode= docRtfGetParaNode( rrc );
	BufferDocument *	bd= rrc->rrcDocument;
	int			textAttributeNumber;

	if  ( ! paraNode )
	    { SXDEB(rcw->rcwWord,paraNode); return -1; }

	if  ( rrs->rrsTextShadingChanged )
	    { docRtfRefreshTextShading( rrc, rrs );	}

	textAttributeNumber= docTextAttributeNumber( bd,
						&(rrs->rrsTextAttribute) );
	if  ( textAttributeNumber < 0 )
	    { LDEB(textAttributeNumber); return -1;	}

	stroff= docParaStrlen( paraNode );

	if  ( docParaStringReplace( &stroffShift, paraNode, stroff, stroff,
						    (char *)bytes, count ) )
	    { LDEB(docParaStrlen(paraNode)); return -1;	}

	if  ( docParaDivideAppendedText( paraNode, textAttributeNumber,
						    stroff, stroff+ count ) )
	    { LLDEB(count,paraNode->biParaParticuleCount); return -1; }
	}
    else{
	if  ( utilMemoryBufferAppendBytes( &(rrs->rrsSavedText), bytes, count ) )
	    { LDEB(count); return -1;	}
	}

    return 0;
    }

int docRtfTextUnicode(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( docRtfTextUnicodeValue( rcw, arg, rrc ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    rrs->rrsUnicodeBytesToSkip= rrs->rrsBytesPerUnicode;
    return 0;
    }

int docRtfTextSpecialChar(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    /* docRtfTextParticule() adjusts level */

    if  ( docRtfTextUnicodeValue( rcw, rcw->rcwID, rrc ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

int docRtfTextSpecialParticule(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    BufferItem *	paraNode;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rrc, rrs );	}

    paraNode= docRtfGetParaNode( rrc );
    if  ( ! paraNode )
	{ SXDEB(rcw->rcwWord,paraNode); return -1; }

    switch( rcw->rcwID )
	{
	case DOCkindTAB:
	case DOCkindLINEBREAK:
	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    if  ( docSaveSpecialParticule( rrc->rrcDocument, paraNode,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;
	    rrc->rrcAtParaHead= 0;

	    break;

	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    {
	    int				done= 0;

	    if  ( rrc->rrcAtParaHead && rcw->rcwID == DOCkindPAGEBREAK )
		{ rrs->rrsParagraphBreakOverride= DOCibkPAGE; done= 1;	}

	    if  ( rrc->rrcAtParaHead && rcw->rcwID == DOCkindCOLUMNBREAK )
		{ rrs->rrsParagraphBreakOverride= DOCibkCOL; done= 1;	}

	    if  ( ! done						&&
		  docSaveSpecialParticule( rrc->rrcDocument, paraNode,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;
	    rrc->rrcAtParaHead= 0;

	    break;
	    }

	case DOCkindOPT_HYPH:
	    if  ( docSaveSpecialParticule( rrc->rrcDocument, paraNode,
				&(rrs->rrsTextAttribute), DOCkindOPT_HYPH ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;
	    rrc->rrcAtParaHead= 0;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

