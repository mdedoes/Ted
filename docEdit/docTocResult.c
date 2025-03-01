/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<docBuf.h>
#   include	"docEdit.h"
#   include	"docIntermediaryDocument.h"
#   include	"docRecalculateTocField.h"
#   include	<docTocField.h>
#   include	"docCalculateToc.h"
#   include	<docStripFrame.h>
#   include	<docPageGrid.h>
#   include	<docTabStop.h>
#   include	<docHyperlinkField.h>
#   include	<docRefField.h>
#   include	<docPagerefField.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFieldStack.h>
#   include	<docStyle.h>
#   include	<docTabStopList.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docDocumentProperties.h>
#   include	<docNodeTree.h>
#   include	<docFields.h>
#   include	<docAttributes.h>
#   include	<docParaBuilder.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*  This is a two level process: First build the TOC from REF and	*/
/*  PAGEREF fields, then let those be evaluated by the text field	*/
/*  evaluator.								*/
/*									*/
/*  Expertimenting with MS-Word shows that the format of the table of	*/
/*  contents is controlled by the format of the first entry: the	*/
/*  paragraph in which the TOC field starts. Things are not THAT simple	*/
/*  however: To be investigated!					*/
/*									*/
/************************************************************************/

static int docParaInsertTocEntry(	CalculateToc *		ct,
					const TocEntry *	te,
					const TextAttribute *	ta,
					const SelectionScope *	ss,
					BufferItem *		paraNode )
    {
    int				rval= 0;

    DocumentField *		dfHyper= (DocumentField *)0;
    DocumentField *		dfRef= (DocumentField *)0;
    DocumentField *		dfPageref= (DocumentField *)0;

    HyperlinkField		hf;
    RefField			rf;
    PagerefField		pf;

    docInitHyperlinkField( &hf );
    docInitRefField( &rf );
    docInitPagerefField( &pf );

    int				makeHyperlinks= ct->ctTocField.tfHyperlinks;

    if  ( ! te->teMarkName						||
	  utilCopyMemoryBuffer( &(hf.hfBookmark), te->teMarkName )	||
	  utilCopyMemoryBuffer( &(rf.rfBookmark), te->teMarkName )  	||
	  utilCopyMemoryBuffer( &(pf.pfBookmark), te->teMarkName )	)
	{ XDEB(te->teMarkName); rval= -1; goto ready;	}

    docParagraphBuilderDeleteEmptySpan( ct->ctParagraphBuilder );

    /* HYPERLINK */
    if  ( makeHyperlinks )
	{
	dfHyper= docParaBuilderAppendFieldHead( ct->ctParagraphBuilder, ta,
					DOCfkHYPERLINK, (const char *)0, 0 );
	if  ( ! dfHyper )
	    { XDEB(dfHyper); rval= -1; goto ready;	}

	if  ( docSetHyperlinkField( &(dfHyper->dfInstructions), &hf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docFieldStackPushLevel( &(ct->ctFieldStack),
						dfHyper, FSpieceFLDRSLT ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /* REF */
    dfRef= docParaBuilderAppendFieldHead( ct->ctParagraphBuilder, ta,
					    DOCfkREF, (const char *)0, 0 );
    if  ( ! dfRef )
	{ XDEB(dfRef); rval= -1; goto ready;	}

    if  ( docSetRefField( dfRef, &rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFieldStackPushLevel( &(ct->ctFieldStack), dfRef, FSpieceFLDRSLT ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docParagraphBuilderAppendText( ct->ctParagraphBuilder, ta, "x", 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docParaBuilderAppendFieldTail( ct->ctParagraphBuilder, ta, dfRef ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docFieldStackPopLevel( &(ct->ctFieldStack), &(ct->ctBdToc->bdBody) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( te->teNumbered )
	{
	/* SEPARATOR */
	if  ( docParaBuilderAppendTab( ct->ctParagraphBuilder, ta ) )
	    { LDEB(1); return -1;	}

	/* PAGEREF */
	dfPageref= docParaBuilderAppendFieldHead( ct->ctParagraphBuilder, ta,
					    DOCfkPAGEREF, (const char *)0, 0 );
	if  ( ! dfPageref )
	    { XDEB(dfPageref); rval= -1; goto ready;	}

	if  ( docSetPagerefField( dfPageref, &pf ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docFieldStackPushLevel( &(ct->ctFieldStack),
						dfPageref, FSpieceFLDRSLT ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParagraphBuilderAppendText( ct->ctParagraphBuilder, ta,
								    "x", 1 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaBuilderAppendFieldTail( ct->ctParagraphBuilder,
						    ta, dfPageref ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docFieldStackPopLevel( &(ct->ctFieldStack),
						    &(ct->ctBdToc->bdBody) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( dfHyper )
	{
	if  ( docParaBuilderAppendFieldTail( ct->ctParagraphBuilder,
						    ta, dfHyper ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docFieldStackPopLevel( &(ct->ctFieldStack),
						    &(ct->ctBdToc->bdBody) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanHyperlinkField( &hf );
    docCleanRefField( &rf );
    docCleanPagerefField( &pf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Create a paragraph to hold one TOC entry.				*/
/*									*/
/************************************************************************/

static struct BufferItem * docMakeTocParagraph(
				CalculateToc *			ct,
				struct BufferItem *		refNode,
				int				level,
				int				numbered,
				const TextAttribute *		ta )

    {
    const DocumentStyle *	ds= ct->ctLevelStyles[level];
    const DocumentProperties *	dp= ct->ctBdToc->bdProperties;

    BufferItem *		paraNodeToc= (struct BufferItem *)0;

    TabStopList			tsl;
    ParagraphProperties		ppToc;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );

    docInitTabStopList( &tsl );
    docInitParagraphProperties( &ppToc );

    if  ( ds && ! utilPropMaskIsEmpty( &(ds->dsParaMask) ) )
	{
	/*  No mapping needed: The style is in the target document */
	if  ( docUpdParaProperties( &ppSetMask, &ppToc,
					&(ds->dsParaMask), &(ds->dsParaProps),
					(const DocumentAttributeMap *)0 ) )
	    { LDEB(1);		}

	ppToc.ppStyleNumber= ds->dsStyleNumber;
	PROPmaskADD( &ppSetMask, PPpropSTYLE );
	}
    else{
	PropertyMask	ppAllMask;

	utilPropMaskClear( &ppAllMask );
	utilPropMaskFill( &ppAllMask, PPprop_COUNT );

	if  ( docUpdParaProperties( &ppSetMask, &ppToc, &ppAllMask,
			    &(ct->ctRefPP), (const DocumentAttributeMap *)0 ) )
	    { LDEB(1);	}

	if  ( ct->ctTocField.tfType == TOCtypeTOC	&&
	      level >= 0				&&
	      level <= PPoutlineDEEPEST			)
	    {
	    ppToc.ppLeftIndentTwips= level* dp->dpTabIntervalTwips;
	    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
	    }
	}

    if  ( ppToc.ppTabStopListNumber > 0 )
	{
	const ParagraphFrame *	pf= &(ct->ctParagraphFrame);
	const TabStopList *	tslFound;

	tslFound= docGetTabStopListByNumber( ct->ctBdToc,
						ppToc.ppTabStopListNumber );

	if  ( ! tslFound			||
	      tslFound->tslTabStopCount == 0	)
	    { ppToc.ppTabStopListNumber= 0;	}
	else{
	    const TabStop *	ts= &(tslFound->tslTabStops[0]);
	    int			wide;

	    wide= pf->pfCellContentRect.drX1- pf->pfCellContentRect.drX0;

	    if  ( ts->tsAlignment == DOCtaRIGHT )
		{
		if  ( ts->tsOffset > wide )
		    { ppToc.ppTabStopListNumber= 0;	}
		}
	    else{
		if  ( ts->tsOffset > ( 19* wide ) / 20 )
		    { ppToc.ppTabStopListNumber= 0;	}
		}
	    }
	}

    if  ( numbered				&&
	  ppToc.ppTabStopListNumber == 0	)
	{
	TabStop			ts;
	int			n;
	const ParagraphFrame *	pf= &(ct->ctParagraphFrame);

	docInitTabStop( &ts );

	ts.tsAlignment= DOCtaRIGHT;
	ts.tsOffset= pf->pfCellContentRect.drX1- pf->pfCellContentRect.drX0;
	ts.tsLeader= DOCtlDOTS;

	if  ( docAddTabToList( &tsl, &ts ) )
	    { LDEB(1);	}
	n= docTabStopListNumber( ct->ctBdToc, &tsl );
	if  ( n < 0 )
	    { LDEB(n);	}
	else{
	    ppToc.ppTabStopListNumber= n;
	    PROPmaskADD( &ppSetMask, PPpropTAB_STOPS );
	    }
	}

    paraNodeToc= docParaBuilderStartNewParagraph( ct->ctParagraphBuilder,
					refNode, &ppToc, ta, ppToc.ppRToL );
    if  ( ! paraNodeToc )
	{ XDEB(paraNodeToc); goto ready;	}

    paraNodeToc->biParaTocLevel= level+ 1;

  ready:

    docCleanTabStopList( &tsl );
    docCleanParagraphProperties( &ppToc );

    return paraNodeToc;
    }

/************************************************************************/
/*									*/
/*  Build the document that will receive the table of contents, before	*/
/*  it is copied over to the target document.				*/
/*									*/
/*  A couple of administrations are copied over to make sure that	*/
/*  properties have the same indexes in the administrations of both	*/
/*  documents.								*/
/*									*/
/************************************************************************/

static int docSetupTocDocument(		CalculateToc *		ct )
    {
    int			level;

    ct->ctBdToc= docIntermediaryDocument( &(ct->ctSectNode), ct->ctBdDoc );
    if  ( ! ct->ctBdToc )
	{ XDEB(ct->ctBdToc); return -1;	}

    ct->ctParagraphBuilder= docOpenParagraphBuilder( ct->ctBdToc,
				    &(ct->ctSectNode->biSectSelectionScope),
				    &(ct->ctBdToc->bdBody) );

    for ( level= 0; level <= PPoutlineDEEPEST; level++ )
	{
	char			scratch[20+1];
	const DocumentStyle *	ds;

	sprintf( scratch, "toc %d", level+ 1 );
	ds= docGetStyleByName( &(ct->ctBdToc->bdStyleSheet), scratch );
	ct->ctLevelStyles[level]= ds;

	ct->ctLevelAttributes[level]= ct->ctTextAttribute;
	if  ( ds && ! utilPropMaskIsEmpty( &(ds->dsTextMask) ) )
	    {
	    PropertyMask	doneMask;

	    docPlainTextAttribute( &(ct->ctLevelAttributes[level]),
								ct->ctBdToc );

	    textUpdateTextAttribute( &doneMask, &(ct->ctLevelAttributes[level]),
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
	    }
	}

    ct->ctLevelAttributes[PPoutlineBODYTEXT]= ct->ctTextAttribute;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Derive paragraph properties from the source document.		*/
/*									*/
/************************************************************************/

static int docGetTocParaProperties(
				CalculateToc *			ct,
				const DocumentSelection *	dsInsideToc,
				int				part0 )
    {
    int			rval= 0;

    docSectionBlockFrameTwips( &(ct->ctBlockFrame),
			dsInsideToc->dsHead.dpNode,
			ct->ctBodySectNode, ct->ctBdDoc,
			dsInsideToc->dsHead.dpNode->biTopPosition.lpPage,
			dsInsideToc->dsHead.dpNode->biTopPosition.lpColumn );

    docParagraphFrameTwips( &(ct->ctParagraphFrame), &(ct->ctBlockFrame),
						dsInsideToc->dsHead.dpNode );

    if  ( docCopyParagraphProperties( &(ct->ctRefPP),
			    dsInsideToc->dsHead.dpNode->biParaProperties ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ct->ctRefPP.ppOutlineLevel= PPoutlineBODYTEXT;

    {
    int				part;
    const TextParticule *	tp= (const TextParticule *)0;

    tp= dsInsideToc->dsHead.dpNode->biParaParticules+ part0;
    ct->ctTextAttribute= *docGetTextAttributeByNumber(
					ct->ctBdDoc, tp->tpTextAttrNr );

    part= part0;
    while( part < dsInsideToc->dsHead.dpNode->biParaParticuleCount )
	{
	if  ( tp->tpKind == TPkindSPAN )
	    {
	    ct->ctTextAttribute= *docGetTextAttributeByNumber(
					    ct->ctBdDoc, tp->tpTextAttrNr );

	    break;
	    }

	part++; tp++;
	}
    }

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Recalculate TOC field.						*/
/*									*/
/************************************************************************/

static int docCalculateTocField( CalculateToc *			ct,
				const DocumentSelection *	dsInsideToc,
				int				part0,
				const DocumentField *		dfToc )
    {
    struct BufferItem *		paraNodeToc= (struct BufferItem *)0;
    struct BufferItem *		refNode= (struct BufferItem *)0;

    int				entryNr= 0;
    TocEntry *			te;

    if  ( docGetTocParaProperties( ct, dsInsideToc, part0 ) )
	{ LDEB(1); return -1;	}

    if  ( docSetupTocDocument( ct ) )
	{ LDEB(1); return -1;	}

    refNode= ct->ctSectNode;
    te= ct->ctEntries;
    for ( entryNr= 0; entryNr < ct->ctEntryCount; te++, entryNr++ )
	{
	const TextAttribute *	ta= &(ct->ctLevelAttributes[te->teLevel]);

	paraNodeToc= docMakeTocParagraph( ct, refNode,
					    te->teLevel, te->teNumbered, ta );
	if  ( ! paraNodeToc )
	    { XDEB(paraNodeToc); return -1;	}

	if  ( docParaInsertTocEntry( ct, te, ta,
				    &(dfToc->dfSelectionScope), paraNodeToc ) )
	    { LDEB(entryNr); return -1;	}

	if  ( docParaBuilderFinishParagraph( ct->ctParagraphBuilder, ta ) )
	    { LDEB(1); return -1;	}

	refNode= paraNodeToc;
	}

    if  ( ! paraNodeToc )
	{
	const int		level= 0;
	const int		numbered= 0;

	const TextAttribute *	ta= &(ct->ctTextAttribute);

	paraNodeToc= docMakeTocParagraph( ct, refNode, level, numbered, ta );
	if  ( ! paraNodeToc )
	    { XDEB(paraNodeToc); return -1;	}

	if  ( docParagraphBuilderAppendText( ct->ctParagraphBuilder, ta,
								"TOC", 3 ) )
	    { LDEB(1); return -1;	}

	if  ( docParaBuilderFinishParagraph( ct->ctParagraphBuilder, ta ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate one TOC field in a document.				*/
/*									*/
/************************************************************************/

int docRecalculateOneTocField(	struct BufferDocument *	bdDoc,
				DocumentField *		dfToc )
    {
    int				rval= 0;

    const MemoryBuffer * const	refFileName= (const MemoryBuffer *)0;
    const int			forceAttributeTo= -1;

    DocumentSelection		dsInsideToc;
    DocumentSelection		dsAroundToc;
    int				part0;
    int				part1;

    EditOperation		eo;
    DocumentCopyJob		dcj;
    CalculateToc		ct;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );
    docInitCalculateToc( &ct );

    if  ( docDelimitFieldInDoc( &dsInsideToc, &dsAroundToc,
					    &part0, &part1, bdDoc, dfToc ) )
	{ LDEB(1); rval= -1; goto ready;	}

#   if 0
    /*  MS-Word does something like this: */
    docHeadPosition( &(dsAroundToc.dsHead), dsAroundToc.dsHead.dpNode );
    docTailPosition( &(dsAroundToc.dsTail), dsAroundToc.dsTail.dpNode );
#   endif

    eo.eoBodySectNode= docGetBodySectNodeOfScope(
					&(dfToc->dfSelectionScope), bdDoc );

    ct.ctBdDoc= bdDoc;
    ct.ctBodySectNode= eo.eoBodySectNode;
    docFieldGetToc( &(ct.ctTocField), dfToc );

    if  ( docCollectTocInput( &ct ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docCalculateTocField( &ct, &dsInsideToc, part0, dfToc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docStartEditOperation( &eo, &dsInsideToc, bdDoc, dfToc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ct.ctBdToc->bdProperties->dpHasOpenEnd= 1;

    if  ( docSet2DocumentCopyJob( &dcj, &eo, ct.ctBdToc, &(ct.ctBdToc->bdBody),
					    refFileName, forceAttributeTo) )
	{ LDEB(1); rval= -1; goto ready;	}

    dcj.dcjCopyHeadParaProperties= 1;
    dcj.dcjCopyTailParaProperties= 1;

    if  ( docIncludeDocument( &dcj ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );
    docCleanCalculateToc( &ct );

    return rval;
    }
