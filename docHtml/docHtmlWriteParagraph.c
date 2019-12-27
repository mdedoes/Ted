/************************************************************************/
/*									*/
/*  Save a paragraph to HTML.						*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docField.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docHtmlWriteParagraph.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docScanner.h>
#   include	<docTextRun.h>
#   include	<docSelect.h>
#   include	<docParaScanner.h>
#   include	<docNotes.h>
#   include	<docNotesProperties.h>
#   include	<sioGeneral.h>
#   include	"docWriteCss.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docHtmlSaveObject(	const VisitParticule *	vp,
				struct InsertedObject *	io,
				void *			vhpw )
    {
    HtmlParagraphWriter *	hpw= (HtmlParagraphWriter *)vhpw;
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    int				pictureDone= 0;

    if  ( docHtmlSaveInsertedObject( &pictureDone, hwc,
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
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

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

	case TPkindCHFTNSEP:
	case TPkindCHFTNSEPC:
	    docHtmlPutString( "<hr style=\"", hwc );
	    docCssWriteProperty( &(xw->xwColumn), sos,
			"display", "inline-block" );
	    docCssWriteDimension( &(xw->xwColumn), sos,
			"width", TWIPS_TO_PIXELS( tp->tpWide ), "px" );
	    docHtmlPutString( "\"/>", hwc );
	    docHtmlNewLine( hwc );
	    hpw->hpwAfterSpace= 0;
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

static int docHtmlSaveNoteHead(	const VisitParticule *	vp,
				const DocumentField *	df,
				HtmlParagraphWriter *	hpw )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    struct BufferDocument *	bd= hpw->hpwDocument;
    struct DocumentNote *	dn= docGetNoteOfField( df, bd );

    char			scratch[20+1];

    if  ( vp->vpParaNode->biTreeType == DOCinBODY		&&
	  docGetNotePosition( dn, bd ) == FTNplacePAGE_END	&&
	  hws->hwsInlineNotes				)
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

#   if 1
    /* Save unified note text */
    docHtmlPutString( scratch, hwc );

    if  ( hwc->hwcInHyperlink )
	{ hwc->hwcBytesInLink += strlen( scratch );	}

    docHtmlFinishField( df, hwc );

    return SCANadviceSKIP;
#   else
    /* Save original note text */
    return SCANadviceOK;
#   endif
    }

static int docHtmlSaveFieldHead( const VisitParticule *	vp,
				const DocumentField *	df,
				HtmlParagraphWriter *	hpw )

    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    if  ( fki->fkiIsFieldInRtf		&&
	  fki->fkiLevel == DOClevSPAN	)
	{
	docHtmlStartField( df, hwc, vp->vpParaNode, vp->vpTextAttributeNr );
	return SCANadviceOK;
	}

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	docHtmlStartField( df, hwc, vp->vpParaNode, vp->vpTextAttributeNr );
	return SCANadviceOK;
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{ return docHtmlSaveNoteHead( vp, df, hpw );	}

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

    if  ( ! hwc->hwcInHyperlink		||
	  ! hwc->hwcInPageref		||
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

int docHtmlEmitParagraphSelection(
					HtmlParagraphWriter *		hpw,
					const DocumentSelection *	ds )
    {
    HtmlWritingContext *	hwc= hpw->hpwHtmlWriter;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    BufferItem *		paraNode= hpw->hpwParaNode;

    const int			flags= 0;

    if  ( docScanParagraphLogicalOrder( bd, paraNode, ds, flags,
			docHtmlSaveParticule,
			docHtmlSaveField,
			docHtmlSaveRun,
			docHtmlSaveObject,
			(TabVisitor)0,
			hpw ) < 0 )
	{ LDEB(1); return -1;	}

    docHtmlChangeAttributes( hwc, -1 );

    return 0;
    }
