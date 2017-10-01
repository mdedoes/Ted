/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<docField.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<psShading.h>
#   include	<docTextParticule.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docPropVal.h>
#   include	<docItemShading.h>
#   include	<utilPropMask.h>
#   include	<docScanner.h>
#   include	<docTextRun.h>
#   include	<docSelect.h>
#   include	<docParaScanner.h>
#   include	<docAttributes.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

typedef struct HtmlParagraphWriter
    {
    HtmlWritingContext *	hpwHtmlWriter;
    int				hpwNoteRefCount;
    unsigned char		hpwAfterSpace;
    } HtmlParagraphWriter;

static int docHtmlSaveObject(	const VisitParticule *	vp,
				struct InsertedObject *	io,
				void *			vhpw )
    {
    HtmlParagraphWriter *	hpw= (HtmlParagraphWriter *)vhpw;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    int				pictureDone= 0;

    if  ( docHtmlSaveImgElement( &pictureDone, hwc,
		    vp->vpTextParticule->tpObjectNumber,
		    vp->vpParaNode, io ) < 0 )
	{ LXDEB(vp->vpTextParticule->tpObjectNumber,io);	}

    if  ( ! pictureDone )
	{ docHtmlPutString( " ", hwc );	}

    hpw->hpwAfterSpace= 0;
    return SCANadviceOK;
    }

static int docHtmlSaveParticule( const VisitParticule *	vp,
				void *			vhpw )
    {
    HtmlParagraphWriter *	hpw= (HtmlParagraphWriter *)vhpw;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

    const TextParticule *	tp= vp->vpTextParticule;

    switch( tp->tpKind )
	{
	case TPkindTAB:
	    docHtmlPutString( " ", hwc );
	    hpw->hpwAfterSpace= 1;
	    break;

	case TPkindOBJECT:
	    LDEB(tp->tpKind); return -1;

	case TPkindFIELDHEAD:
	    LDEB(tp->tpKind); return -1;

	case TPkindFIELDTAIL:
	    LDEB(tp->tpKind); return -1;
	    break;

	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    docHtmlPutString( "<br/>", hwc );
	    docHtmlNewLine( hwc );
	    hpw->hpwAfterSpace= 0;
	    break;

	case TPkindOPT_HYPH:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    break;

	default:
	    LSDEB(tp->tpKind,docKindStr(tp->tpKind));
	    break;
	}

    return SCANadviceOK;
    }

static int docHtmlSaveFieldTail( const VisitParticule *	vp,
				const DocumentField *	df,
				HtmlParagraphWriter *	hpw )

    {
    HtmlWritingContext *		hwc= hpw->hpwHtmlWriter;
    const FieldKindInformation *	fki;

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( df->dfKind == DOCfkCHFTN					||
	  df->dfKind == DOCfkBOOKMARK					||
	  ( fki->fkiIsFieldInRtf && fki->fkiLevel == DOClevSPAN )	)
	{ docHtmlFinishField( df, hwc );	}

    return SCANadviceOK;
    }

static int docHtmlSaveFieldHead( const VisitParticule *	vp,
				const DocumentField *	df,
				HtmlParagraphWriter *	hpw )

    {
    HtmlWritingContext *		hwc= hpw->hpwHtmlWriter;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    if  ( fki->fkiIsFieldInRtf		&&
	  fki->fkiLevel == DOClevSPAN	)
	{
	docHtmlStartField( df, hwc, vp->vpParaNode, vp->vpTextAttributeNr );
	}

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	docHtmlStartField( df, hwc, vp->vpParaNode, vp->vpTextAttributeNr );
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	char	scratch[20+1];

	if  ( vp->vpParaNode->biTreeType == DOCinBODY	&&
	      hwc->hwcInlineNotes			)
	    {
	    if  ( utilIndexMappingAppend( &(hwc->hwcDeferredNotes),
			    (int *)0, vp->vpTextParticule->tpObjectNumber ) )
		{ LDEB(vp->vpTextParticule->tpObjectNumber);	}
	    }

	docHtmlStartField( df, hwc, vp->vpParaNode, vp->vpTextAttributeNr );

	if  ( vp->vpParaNode->biTreeType == DOCinBODY )
	    {
	    sprintf( scratch, "%d", hwc->hwcNoteRefCount+ 1 );
	    hwc->hwcNoteRefCount++;
	    }
	else{
	    sprintf( scratch, "%d", hwc->hwcNoteDefCount+ 1 );
	    hwc->hwcNoteDefCount++;
	    }

#	if 1
	/* Save unified note text */
	docHtmlPutString( scratch, hwc );

	if  ( hwc->hwcInHyperlink )
	    { hwc->hwcBytesInLink += strlen( scratch );	}

	docHtmlFinishField( df, hwc );

	return SCANadviceSKIP;
#	else
	/* Save original note text */
	return SCANadviceOK;
#	endif
	}

    return SCANadviceOK;
    }

static int docHtmlSaveField(	const VisitParticule *	vp,
				DocumentField *		df,
				void *			vhpw )
    {
    if  ( vp->vpTextParticule->tpKind == TPkindFIELDHEAD )
	{
	return docHtmlSaveFieldHead( vp, df, (HtmlParagraphWriter *)vhpw );
	}

    if  ( vp->vpTextParticule->tpKind == TPkindFIELDTAIL )
	{
	return docHtmlSaveFieldTail( vp, df, (HtmlParagraphWriter *)vhpw );
	}

    LDEB(vp->vpTextParticule->tpKind); return -1;
    }

static int docHtmlSaveRun(	const TextRun *			tr,
				void *				vhpw )
    {
    HtmlParagraphWriter *	hpw= (HtmlParagraphWriter *)vhpw;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);

    const BufferItem *		paraNode= tr->trParaNode;
    const char *		s= docParaString( paraNode, tr->trStroff );

    docHtmlChangeAttributes( hwc, tr->trTextAttributeNr );

    if  ( hpw->hpwAfterSpace && xw->xwColumn+ tr->trStrlen > 76 )
	{ docHtmlNewLine( hwc );	}

    if  ( ! hwc->hwcInHyperlink	||
	  ! hwc->hwcInPageref	||
	  hwc->hwcBytesInLink == 0	)
	{
	docHtmlEscapeCharacters( hwc, s, tr->trStrlen );

	if  ( hwc->hwcInHyperlink )
	    { hwc->hwcBytesInLink += tr->trStrlen;	}
	}

    hpw->hpwAfterSpace= 0;
    if  ( docParaPastLastNonBlank( paraNode, tr->trStroff,
					    tr->trStroff+ tr->trStrlen ) <
					    tr->trStroff+ tr->trStrlen )
	{ hpw->hpwAfterSpace= 1;	}

    return SCANadviceOK;
    }

static void docHtmlStartParagraphBody(
				const struct BufferItem * const paraNode,
				int *				pBulletTail,
				int *				pBodyHead,
				const char *			tag,
				int				fontHeight,
				HtmlWritingContext *		hwc )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const struct BufferDocument *	bd= hwc->hwcDocument;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    const ItemShading *		is;

    int				bulletTail= -1;
    int				bodyHead= -1;

    is= docGetItemShadingByNumber( bd, pp->ppShadingNumber );

    if  ( hwc->hwcSupportsBullets		&&
	  pp->ppLeftIndentTwips >=  100		&&
	  pp->ppFirstIndentTwips <= 100		)
	{
	int			x;
	int			part= 0;
	const TextParticule *	tp;

	x= 0;
	part= 0;
	tp= paraNode->biParaParticules;
	for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	    {
	    x += tp->tpWide;

	    if  ( tp->tpKind == TPkindTAB )
		{
		if  ( x <= pp->ppLeftIndentTwips )
		    {
		    bulletTail= tp->tpStroff;
		    bodyHead= tp->tpStroff+ tp->tpStrlen;
		    }
		break;
		}
	    }
	}

    docHtmlPutString( "<", hwc );
    docHtmlPutString( tag, hwc );
    docHtmlPutString( " style=\"", hwc );

    if  ( pp->ppLeftIndentTwips <= -100	||
	  pp->ppLeftIndentTwips >=  100	)
	{
	docHtmlPutString( "padding-left:", hwc );
	sioOutPrintf( sos, "%dpt;",
		    ( pp->ppLeftIndentTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}

    if  ( ( pp->ppFirstIndentTwips <= -100	||
	    pp->ppFirstIndentTwips >=  100	)	)
	{
	docHtmlPutString( "text-indent:", hwc );
	sioOutPrintf( sos, "%dpt;", pp->ppFirstIndentTwips/ 20 );
	xw->xwColumn += 6;
	}

    if  ( is->isPattern == DOCspSOLID )
	{ docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, is ); }

    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-top", pp->ppTopBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-left", pp->ppLeftBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-right", pp->ppRightBorderNumber );
    docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
		    "border-bottom", pp->ppBottomBorderNumber );

    if  ( pp->ppSpaceBeforeTwips > 0 )
	{
	docHtmlPutString( "margin-top:", hwc );
	sioOutPrintf( sos, "%dpt;",
			( pp->ppSpaceBeforeTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}
    if  ( pp->ppSpaceAfterTwips > 0 )
	{
	docHtmlPutString( "margin-bottom:", hwc );
	sioOutPrintf( sos, "%dpt;",
			( pp->ppSpaceAfterTwips+ 10 )/ 20 );
	xw->xwColumn += 6;
	}

    switch( pp->ppAlignment )
	{
	case DOCthaLEFT:
	    break;
	case DOCthaRIGHT:
	    docHtmlPutString( " text-align:right;", hwc );
	    break;

	case DOCthaCENTERED:
	    docHtmlPutString( " text-align:center;", hwc );
	    break;

	case DOCthaJUSTIFIED:
	    docHtmlPutString( " text-align:justify;", hwc );
	    break;

	default:
	    LDEB(pp->ppAlignment);
	    break;
	}

    docHtmlPutString( "\"", hwc );
    docHtmlPutString( ">", hwc );

    if  ( pp->ppSpaceBeforeTwips > fontHeight/ 2 )
	{
	docHtmlPutString( "&#160;<br/>", hwc );
	docHtmlNewLine( hwc );
	}

    *pBulletTail= bulletTail;
    *pBodyHead= bodyHead;

    return;
    }

/************************************************************************/
/*									*/
/*  Save a 'Paragraph'							*/
/*									*/
/*  But not as a <P>...</P>.						*/
/*									*/
/************************************************************************/

int docHtmlSaveParaNode( HtmlWritingContext *			hwc,
			struct BufferItem *			paraNode,
			const struct BufferItem *		bodySectNode,
			const struct DocumentSelection *	ds )
    {
    const struct TextAttribute *	ta;
    int					fontHeight;

    PropertyMask			ppSetMask;
    int					bulletTail= -1;
    int					bodyHead= -1;

    struct BufferDocument *		bd= hwc->hwcDocument;

    HtmlParagraphWriter			hpw;
    DocumentSelection			dsPara;
    const int				flags= 0;

    docInitDocumentSelection( &dsPara );

    hpw.hpwHtmlWriter= hwc;
    hpw.hpwNoteRefCount= 0;
    hpw.hpwAfterSpace= 0;

    if  ( paraNode->biParaParticuleCount == 0		||
	  docParaStrlen( paraNode ) == 0		)
	{
	docHtmlPutString( "<div>&#160;</div>", hwc );
	docHtmlNewLine( hwc );
	return 0;
	}

    docGetEffectiveTextAttributes( &ta, bd, paraNode, 0 );

    fontHeight= TA_FONT_SIZE_TWIPS( ta );

    docHtmlStartParagraphBody( paraNode, &bulletTail, &bodyHead,
						    "div", fontHeight, hwc );

    if  ( bulletTail >= 0 && bodyHead >= 0 )
	{
	const int	direction= 1;

	docSetParaSelection( &dsPara, paraNode, direction, 0, bulletTail );

	docHtmlPutString( "<div", hwc );
	docHtmlWriteStringAttribute( hwc, "style", "float: left;" );
	docHtmlPutString( ">", hwc );

	if  ( docScanParagraphLogicalOrder( bd, paraNode, &dsPara, flags,
			    docHtmlSaveParticule,
			    docHtmlSaveField,
			    docHtmlSaveRun,
			    docHtmlSaveObject,
			    (TabVisitor)0,
			    &hpw ) < 0 )
	    { LLDEB(bulletTail,bodyHead); return -1;	}

	docHtmlChangeAttributes( hwc, -1 );

	docHtmlPutString( "</div>", hwc );		
	docHtmlPutString( "<div", hwc );
	docHtmlWriteStringAttribute( hwc, "style", "text-indent: 0;" );
	docHtmlPutString( ">", hwc );

	docSetParaSelection( &dsPara, paraNode, direction,
					bodyHead, docParaStrlen( paraNode ) );
	ds= &dsPara;
	}

    if  ( docScanParagraphLogicalOrder( bd, paraNode, ds, flags,
			    docHtmlSaveParticule,
			    docHtmlSaveField,
			    docHtmlSaveRun,
			    docHtmlSaveObject,
			    (TabVisitor)0,
			    &hpw ) < 0 )
	{ LDEB(1); return -1;	}

    docHtmlChangeAttributes( hwc, -1 );

    if  ( hwc->hwcInlineNotes		&&
	  hpw.hpwNoteRefCount > 0	&&
	  hwc->hwcTableNesting == 0	)
	{
	if  ( docHtmlSaveDeferredNotes( hwc ) )
	    { LDEB(hpw.hpwNoteRefCount); return -1;	}
	}

    if  ( bulletTail >= 0 && bodyHead >= 0 )
	{ docHtmlPutString( "</div>", hwc );		}

    if  ( paraNode->biParaProperties->ppSpaceAfterTwips > fontHeight/ 2 )
	{ docHtmlPutString( "<br/>&#160;</div>", hwc );	}
    else{
	docHtmlPutString( "</div>", hwc );		
	}
    docHtmlNewLine( hwc );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
			    &(hwc->hwcParagraphProperties),
			    &ppSetMask, paraNode->biParaProperties,
			    (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1);	}

    return 0;
    }


