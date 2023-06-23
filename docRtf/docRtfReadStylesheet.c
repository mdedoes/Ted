/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  struct BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfFindProperty.h"
#   include	<docStyleSheet.h>
#   include	<docBuf.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Consume the style name in the stylesheet of a document. It comes	*/
/*  last in the style, so we can save the style.			*/
/*									*/
/************************************************************************/

static int docRtfStyleName(	RtfReader *		rr,
				const char *		name,
				int			len )
    {
    RtfReadingState *		rrs= rr->rrState;

    DocumentStyle *		ds;

    int				size;
    const int			removeSemicolon= 1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( docRtfSaveDocEncodedText( rr, name, len ) )
	{ LDEB(len); return -1;	}

    ds= &(rr->rrStyle);
    switch( ds->dsLevel )
	{
	case DOClevSPAN:
	    break;

	case 0: case -1:
	    LDEB(rr->rrStyle.dsLevel);
	    ds->dsStyleNumber= 0;
	    ds->dsLevel= DOClevPARA;
	    break;

	case DOClevPARA:
	    ds->dsStyleNumber= rrs->rrsParagraphProperties.ppStyleNumber;
	    break;

	case DOClevROW:
	    /* NO! They have trowd and the sheet owns the style
	    ds->dsStyleNumber= rr->rrRowProperties.rpRowStyle;
	    */
	    break;

	case DOClevSECT:
	    ds->dsStyleNumber= rr->rrSectionProperties.spStyle;
	    break;

	default:
	    LDEB(rr->rrStyle.dsLevel); return -1;
	}

    if  ( docRtfStoreStyleProperties( rr ) )
	{ LDEB(1);	}

    if  ( docRtfStoreSavedText( &(rr->rrStyle.dsName),
					    &size, rr, removeSemicolon ) )
	{ LDEB(len); return -1;	}

    ds= docInsertStyle( &(rr->rrDocument->bdStyleSheet),
					rr->rrStyle.dsStyleNumber,
					&(rr->rrStyle), dam0 );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docCleanDocumentStyle( &(rr->rrStyle) );
    docInitDocumentStyle( &(rr->rrStyle) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rr->rrDocument );

    return 0;
    }

static int docRtfStyleGroup(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    switch( rcw->rcwID )
	{
	case DOClevSPAN:
	    rr->rrStyle.dsStyleNumber= arg;
	    rr->rrStyle.dsLevel= DOClevSPAN;
	    break;

	case DOClevSECT:
	    rr->rrSectionProperties.spStyle= arg;
	    rr->rrStyle.dsStyleNumber= arg;
	    rr->rrStyle.dsLevel= DOClevSECT;
	    break;

	case DOClevROW:
	    if  ( ! rr->rrTreeStack )
		{ XDEB(rr->rrTreeStack); return -1;	}
	    rr->rrTreeStack->rtsRowProperties.rpRowStyle= arg;
	    rr->rrStyle.dsStyleNumber= arg;
	    rr->rrStyle.dsLevel= DOClevROW;
	    break;

	case DOClevPARA:
	    rrs->rrsParagraphProperties.ppStyleNumber= arg;
	    rr->rrStyle.dsStyleNumber= arg;
	    rr->rrStyle.dsLevel= DOClevPARA;
	    break;

	default:
	    LDEB(rcw->rcwID);
	}

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
		    (RtfControlWord *)0, docRtfStyleName, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    rr->rrStyle.dsLevel= DOClevANY;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Inside a stylesheet.. These tags are handled differently.		*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfStylesheetGroups[]=
{
    RTF_DEST_XX( "cs",	RTCscopeSTYLE, DOClevSPAN,	docRtfStyleGroup ),
    RTF_DEST_XX( "ds",	RTCscopeSTYLE, DOClevSECT,	docRtfStyleGroup ),
    RTF_DEST_XX( "ts",	RTCscopeSTYLE, DOClevROW,	docRtfStyleGroup ),
    RTF_DEST_XX( "tsrowd", RTCscopeSTYLE, DOClevROW,	docRtfStyleGroup ),
    RTF_DEST_XX( "s",	RTCscopeSTYLE, DOClevPARA,	docRtfStyleGroup ),

    { (char *)0, 0, 0 }
};

/************************************************************************/
/*									*/
/*  Consume a group introduced by a control word that is not a		*/
/*  destination.							*/
/*									*/
/************************************************************************/

static int docRtfReadWordGroup(	RtfReader *		rr,
				int			gotArg,
				int			arg,
				const char *		controlWord,
				const RtfControlWord *	groupWords,
				RtfGotText		gotText )
    {
    const RtfControlWord *	rcw;

    rcw= docRtfFindPropertyWord( controlWord );
    if  ( rcw )
	{
	if  ( docRtfReadGroupX( rcw, rcw, gotArg, arg, rr, groupWords,
					gotText, (RtfCommitGroup)0 ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}
    else{
	if  ( rr->rrComplainUnknown )
	    { LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,controlWord);	}

	if  ( docRtfReadUnknownGroup( rr ) )
	    { LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,controlWord); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume the stylesheet in a document.				*/
/*									*/
/*  Because <styledef> is optional in the stylesheet. (For the first	*/
/*  style at least.) The normal mechanism of the parser does not work	*/
/*  for the stylesheet. Do things by hand.				*/
/*									*/
/************************************************************************/

int docRtfStylesheet(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrState;
    int				res;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				c;

    docCleanDocumentStyle( &(rr->rrStyle) );
    docInitDocumentStyle( &(rr->rrStyle) );

    res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    rr->rrStyle.dsLevel= DOClevPARA;
    rrs->rrsParagraphProperties.ppStyleNumber= 0;

    for (;;)
	{
	rrs= rr->rrState;

	switch( res )
	    {
	    case RTFfiCLOSE:
		break;

	    case RTFfiCTRLGROUP:
		rcw= docRtfFindWord( controlWord, docRtfStylesheetGroups );
		if  ( ! rcw )
		    {
		    if  ( docRtfReadWordGroup( rr, gotArg, arg, controlWord,
					(RtfControlWord *)0, docRtfStyleName ) )
			{ SDEB(controlWord); return -1;	}
		    }
		else{
		  groupFound:
		    res= docRtfApplyControlWord( rcw, gotArg, arg, rr );
		    if  ( res < 0 )
			{ LDEB(res); SDEB(controlWord); return -1;	}
		    }

		res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    case RTFfiSTARGROUP:
		rcw= docRtfFindWord( controlWord, docRtfStylesheetGroups );
		if  ( rcw )
		    { goto groupFound; }

		rr->rrInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( rr ) )
		    { LDEB(1); rr->rrInIgnoredGroup--; return -1;	}

		rr->rrInIgnoredGroup--;

		res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    case RTFfiTEXTGROUP:
		{
		const RtfControlWord *	rcwApplyFirst= (RtfControlWord *)0;
		int			argApplyFirst= -1;
		int			gotArgApplyFirst= 0;

		if  ( docRtfReadGroupX( (const RtfControlWord *)0,
			rcwApplyFirst, gotArgApplyFirst, argApplyFirst,
			rr, docRtfStylesheetGroups,
			docRtfStyleName, (RtfCommitGroup)0 ) )
		    { LDEB(1); return -1;	}

		res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		}
		continue;

	    default:
		LDEB(res); return -1;
	    }

	break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a style property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberStyleProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    DocumentStyle *	ds= &(rr->rrStyle);

    /* \s \cs \ds tags but \s and \ds handeled with the para/sect style */
    if  ( rcw->rcwID == DSpropSTYLE_NUMBER )
	{
	ds->dsStyleNumber= arg;
	/* Exceptional trick */
	ds->dsLevel= rcw->rcwEnumValue;
	return 0;
	}

    if  ( docSetStyleProperty( ds, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember accumulated properties of the reader on the current style.	*/
/*									*/
/************************************************************************/

int docRtfStoreStyleProperties(		RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrState;
    struct BufferDocument *		bd= rr->rrDocument;

    DocumentStyle *		ds= &(rr->rrStyle);

    /*  SECT	*/
    docCopySectionProperties( &(ds->dsSectProps),
					    &(rr->rrSectionProperties) );

    /*  ROW	*/

    /*  CELL	*/

    /*  PARA	*/
    if  ( docRtfSetParaProperties( &(ds->dsParaProps), bd, rrs ) )
	{ LDEB(1);	}

    /*  SPAN	*/
    if  ( rrs->rrsTextShadingChanged )
	{
	docRtfRefreshTextShading( rr, rrs );
	PROPmaskADD( &(rr->rrStyle.dsTextMask), TApropSHADING );
	}

    ds->dsTextAttribute= rrs->rrsTextAttribute;

    return 0;
    }

