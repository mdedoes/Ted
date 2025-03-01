/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to preserve links when the	*/
/*  PostScript is converted to PDF.					*/
/*									*/
/*  Though the Adobe Technical Note #5150: "pdfmark Reference Manual"	*/
/*  exhaustively documents the pdfmark functionality, the book:		*/
/*  Merz, Thomas, "Postscript & Acrobat/PDF", Springer-Verlag, Berlin	*/
/*  &c, 1996, ISBN 3-540-60854-0. gives a summary on page 289 that is	*/
/*  much easier to understand.						*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioFileio.h>
#   include	<psDrawMetafileProcs.h>

#   include	"docDraw.h"
#   include	<docLayout.h>
#   include	"docPsPrintImpl.h"
#   include	"docPsPrint.h"
#   include	<docTreeNode.h>
#   include	<layoutContext.h>
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	<sioGeneral.h>
#   include	<sioUtil.h>
#   include	<psFace.h>
#   include	<docBuf.h>
#   include	<psPrint.h>
#   include	<docHeaderFooterScopes.h>
#   include	<textMsLocale.h>
#   include	<docSectLayout.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

#   define	SHOW_PAGE_GRID	0

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docPsSetColorRgb(	DrawingContext *	dc,
				void *			vps,
				const RGB8Color *	rgb8 )
    {
    PrintingState *		ps= (PrintingState *)vps;

    if  ( psSetRgb8Color( ps, rgb8 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int docPsSetFont(	DrawingContext *	dc,
				void *			vps,
				int			textAttrNumber,
				const TextAttribute *	ta )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;
    PrintingState *		ps= (PrintingState *)vps;
    const struct AfmFontInfo *	afi;

    afi= docDocLayoutGetFontInfo( lc, ta );
    if  ( ! afi )
	{ LDEB(textAttrNumber); return -1; }

    psSetFont( ps, afi, ta );

    return 0;
    }

/************************************************************************/

#   if  SHOW_PAGE_GRID

static void docPsPageBoxes(	DrawingContext *		dc,
				void *				vps,
				const DocumentGeometry *	dg )
    {
    PrintingState *		ps= (PrintingState *)vps;

    DocumentRectangle		drBBox;
    DocumentRectangle		drBody;
    DocumentRectangle		drHead;
    DocumentRectangle		drFoot;

    RGB8Color			rgb8;

    rgb8.rgb8Alpha= 255;

    geoDocumentGeometryGetPageBoundingBox( &drBBox, dg, 1, 1 );
    geoDocumentGeometryGetBodyRect( &drBody, dg );
    geoDocumentGeometryGetHeaderRect( &drHead, dg );
    geoDocumentGeometryGetFooterRect( &drFoot, dg );

    utilRGB8SolidRGB( 255, 199, 199 );
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drHead.drX0, drHead.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX1, drHead.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX1, drHead.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX0, drHead.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX0, drHead.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    utilRGB8SolidRGB( 199, 199, 255 );
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drFoot.drX0, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX1, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX1, drFoot.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX0, drFoot.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX0, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    utilRGB8SolidRGB( 199, 255, 199 );
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBody.drX0, drBody.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX1, drBody.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX1, drBody.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX0, drBody.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX0, drBody.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    utilRGB8SolidRGB( 0, 255, 0 );
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBBox.drX0, drBBox.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBBox.drX1, drBBox.drY1 );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBBox.drX0, drBBox.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBBox.drX1, drBBox.drY0 );
    sioOutPrintf( ps->psSos, " stroke\n" );
    }

#   endif

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/*  1)  If anything is printed on the page, and header/footer printing	*/
/*      is postponed, print headers and footers for this page.		*/
/*  2)  If omitHeadersOnEmptyPages is set, dc->dcPostponeHeadersFooters	*/
/*	is derived from it. So any header printing is covered by (1)	*/
/*  3)  If the page is completely empty, skip it.			*/
/*  4)  if the page is empty, but had not have been so if headers and	*/
/*	footers were printed, emit it. Otherwise skip it. This sounds a	*/
/*	bit strange, but the customer is always right. Actually, it has	*/
/*	a purpose: In simple documents, you do not want to waste paper.	*/
/*	So you should not print empty pages. In more complicated	*/
/*	documents, you do not want to print pages with only a header	*/
/*	and a footer to prevent fraud. For pagination purposes, you do	*/
/*	want to emit the page however. Now in stead of make the user	*/
/*	say what she wants with a multitude of parameters, we		*/
/*	distinguish between simple and complicated documents by		*/
/*	assuming that a complicated document has headers and footers.	*/
/*									*/
/************************************************************************/

static int docPsFinishPage(	void *				vps,
				DrawingContext *		dc,
				struct BufferItem *		bodySectNode,
				int				page,
				int				asLast )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			pageWasMarked;
    int			pageIsMarked;
    int			pageHasHeader= dc->dcDocHasPageHeaders;
    int			pageHasFooter= dc->dcDocHasPageFooters;
    int			skip= 0;

    const int asArtifact= docDrawHeaderAsArtifact(
					    page, bodySectNode );
    const int postponedHeader= dc->dcPostponeHeadersFooters && asArtifact;

#   if 0
    docPsPageBoxes( dc, vps,
	&(dc->dcLayoutContext.lcDocument->bdProperties.dpGeometry) );
#   endif

    pageWasMarked= ps->psLastPageMarked >= ps->psPagesPrinted;

    /*
    sioOutPrintf( ps->psSos, "%% pageWasMarked= %d\n", pageWasMarked );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageHeaders= %d\n",
					    dc->dcDocHasPageHeaders );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageFooters= %d\n",
					    dc->dcDocHasPageFooters );
    */

    /*  1  */
    if  ( pageWasMarked && postponedHeader )
	{
	if  ( dc->dcDocHasPageHeaders				&&
	      docDrawPageHeader( bodySectNode, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageHeaders,page);	}
	}

    /*  1  */
    if  ( pageWasMarked			&&
	  dc->dcPostponeHeadersFooters	)
	{
	if  ( dc->dcDocHasPageFooters				&&
	      docDrawPageFooter( bodySectNode, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageFooters,page);	}
	}

    /*  2  */

    pageIsMarked= ps->psLastPageMarked >= ps->psPagesPrinted;
    /*
    sioOutPrintf( ps->psSos, "%% pageIsMarked= %d\n", pageIsMarked );
    */

    /*  3  */
    if  ( ! pageIsMarked && ps->psPrintGeometry.pgSkipBlankPages )
	{ skip= 1;	}

    /*  4  */
    if  ( ! pageIsMarked )
	{
	const LayoutContext *		lc= dc->dcLayoutContext;
	const struct BufferDocument *	bd= lc->lcDocument;

	struct DocumentTree *		tree;
	int				isEmpty;

	if  ( dc->dcDocHasPageHeaders )
	    {
	    tree= (struct DocumentTree *)0;
	    docDrawWhatPageHeader( &tree, &isEmpty, bodySectNode, page, bd );
	    if  ( ! tree || ! tree->dtRoot || isEmpty )
		{ pageHasHeader= 0;	}
	    }

	if  ( dc->dcDocHasPageFooters )
	    {
	    tree= (struct DocumentTree *)0;
	    docDrawWhatPageFooter( &tree, &isEmpty, bodySectNode, page, bd );
	    if  ( ! tree || ! tree->dtRoot || isEmpty )
		{ pageHasFooter= 0;	}
	    }
	}

    /*
    sioOutPrintf( ps->psSos, "%% pageHasHeader= %d\n", pageHasHeader );
    sioOutPrintf( ps->psSos, "%% pageHasFooter= %d\n", pageHasFooter );
    */

    if  ( ! pageIsMarked			&&
	  ps->psPrintGeometry.pgSkipEmptyPages	&&
	  ! pageHasHeader			&&
	  ! pageHasFooter			)
	{ skip= 1;	}


    if  ( skip )
	{ psAbortPage( ps, page, asLast );	}
    else{
	if  ( psSetPageProperty( ps, "TedEmpty", pageIsMarked?"false":"true" ) )
	    { LDEB(1); return -1;	}

	if  ( psFinishPage( ps, page, asLast ) )
	    { LDEB(page); return -1;	}
	}

    return 0;
    }

static int docPsPrintStartPage(	void *				vps,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				const char *			why,
				int				page )
    {
    PrintingState *	ps= (PrintingState *)vps;

    docResetDrawingContextState( dc );

    psRefreshNupSchema( ps, dgPage );

    if  ( psStartPage( ps, page ) )
	{ LDEB(page); return -1;	}

    if  ( psSetPageProperty( ps, "TedWhy", why ) )
	{ LDEB(1); return -1;	}

#   if SHOW_PAGE_GRID
    docPsPageBoxes( dc, vps,
	&(dc->dcLayoutContext.lcDocument->bdProperties.dpGeometry) );
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a range of pages in a document.				*/
/*									*/
/************************************************************************/

static int docPsPrintPageRange(	PrintingState *		ps,
				DrawingContext *	dc,
				struct BufferItem *	bodyNode,
				int			firstPage,
				int			lastPage,
				int			asLast )
    {
    LayoutPosition		lpBelow;
    struct BufferItem *		bodySectNode;

    docInitLayoutPosition( &lpBelow );

    bodySectNode= docFindFirstSectionOnPage( bodyNode, firstPage );
    if  ( ! bodySectNode )
	{ LXDEB(firstPage,bodySectNode); return -1;	}

    if  ( docPsPrintStartPage( (void *)ps, &(bodySectNode->biSectDocumentGeometry),
						    dc, "start", firstPage ) )
	{ LDEB(firstPage); return -1;	}

    {
    const int asArtifact= docDrawHeaderAsArtifact(
					    firstPage, bodySectNode );
    const int postponeHeader= dc->dcPostponeHeadersFooters && asArtifact;

    if  ( ! postponeHeader )
	{ docDrawPageHeader( bodySectNode, (void *)ps, dc, firstPage );	}
    }

    if  ( docDrawShapesForPage( (void *)ps, dc, 1, firstPage ) )
	{ LDEB(firstPage);	}

    docDrawNode( &lpBelow, bodyNode, (void *)ps, dc );

    if  ( lastPage < 0 )
	{ lastPage= bodyNode->biBelowPosition.lpPage;	}

    bodySectNode= docFindLastSectionOnPage( bodyNode, lastPage );
    if  ( ! bodySectNode )
	{ LXDEB(lastPage,bodySectNode); return -1;	}

    if  ( docDrawShapesForPage( (void *)ps, dc, 0, lastPage ) )
	{ LDEB(lastPage);	}

    if  ( ! dc->dcPostponeHeadersFooters )
	{ docDrawPageFooter( bodySectNode, (void *)ps, dc, lastPage );	}

    docPsFinishPage( (void *)ps, dc, bodySectNode, lastPage, asLast );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/************************************************************************/

int docPsPrintDocument(	struct SimpleOutputStream *	sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const MemoryBuffer *		fontDirectory,
			double				shadingMesh,
			int				emitOutline,
			int				markContent,
			int				omitContentMarks,
			int				declareUACompliant,
			int				markInvisibleAsArtifact,
			int				flattenPlainTables,
			int				drawParticulesSeparately,
			const LayoutContext *		lc,
			const PrintGeometry *		pg )
    {
    int				rval= 0;

    struct BufferDocument *	bd= lc->lcDocument;
    DocumentProperties *	dp= bd->bdProperties;
    DocumentGeometry *		dg= &(dp->dpGeometry);
    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;

    PostScriptTypeList		pstl;

    DrawingContext		dc;
    PrintingState		ps;

    int				firstPage= pg->pgFirstPage;
    int				lastPage= pg->pgLastPage;

    const START_TREE_LAYOUT	startTreeLayout= (START_TREE_LAYOUT)0;

    psInitPostScriptFaceList( &pstl );
    if  ( utilCopyMemoryBuffer( &(pstl.pstlFontDirectory), fontDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( firstPage >= 0						||
	  lastPage >= 0							||
	  docListNumberTreeIsEmpty( &(bd->bdBody.dtOutlineTree) )	)
	{ emitOutline= 0;	}

    docInitDrawingContext( &dc );
    psInitPrintingState( &ps );

    dc.dcDocument= lc->lcDocument;
    dc.dcDrawParticulesSeparately= drawParticulesSeparately;

    dc.dcSetColorRgb= docPsSetColorRgb;
    dc.dcSetFont= docPsSetFont;
    dc.dcDrawShape= docPsPrintDrawDrawingShape;
    dc.dcDrawInlineObject= docPsPrintInlineObject;
    dc.dcStartField= docPsPrintStartField;
    dc.dcFinishField= docPsPrintFinishField;
    dc.dcDrawTab= docPsPrintTab;
    dc.dcDrawFtnsep= docPsPrintFtnsep;
    dc.dcDrawTextRun= docPsPrintTextRun;
    dc.dcDrawUnderline= docPsPrintRunUnderline;
    dc.dcDrawStrikethrough= docPsPrintRunStrikethrough;

    dc.dcStartTextLine= docPsPrintStartTextLine;
    dc.dcFinishTextLine= docPsPrintFinishTextLine;
    dc.dcDrawOrnaments= docPsPrintOrnaments;
    dc.dcFinishPage= docPsFinishPage;
    dc.dcStartPage= docPsPrintStartPage;
    dc.dcStartTreeLayout= startTreeLayout;

    if  ( markContent )
	{
	dc.dcStartNode= docPsPrintStartNode;
	dc.dcFinishNode= docPsPrintFinishNode;
	dc.dcStartLines= docPsPrintStartLines;
	dc.dcFinishLines= docPsPrintFinishLines;
	dc.dcStartTree= docPsPrintStartTree;
	dc.dcFinishTree= docPsPrintFinishTree;
	ps.psTagDocumentStructure= 1;

	ps.psMarkInvisibleAsArtifact= markInvisibleAsArtifact != 0;
	ps.psFlattenPlainTables= flattenPlainTables != 0;
	ps.psOmitContentMarks= omitContentMarks != 0;
	ps.psDeclareUACompliant= declareUACompliant != 0;
	}

    dc.dcLayoutContext= lc;

    dc.dcFirstPage= pg->pgFirstPage;
    dc.dcLastPage= pg->pgLastPage;
    dc.dcDrawOtherTrees= 1;
    dc.dcPostponeHeadersFooters= 0;

    if  ( pg->pgOmitHeadersOnEmptyPages )
	{ dc.dcPostponeHeadersFooters= 1;	}

    ps.psSos= sos;
    ps.psUsePostScriptFilters= pg->pgUsePostScriptFilters;
    ps.psUsePostScriptIndexedImages= pg->pgUsePostScriptIndexedImages;

    ps.psPrintGeometry.pgEmbedFonts= pg->pgEmbedFonts;
    ps.psPrintGeometry.pgSkipEmptyPages= pg->pgSkipEmptyPages;
    ps.psPrintGeometry.pgSkipBlankPages= pg->pgSkipBlankPages;
    ps.psPrintGeometry.pgOmitHeadersOnEmptyPages=
					    pg->pgOmitHeadersOnEmptyPages;

    {
    int	h, f;

    docInquireHeadersFooters( &h, &f, bd );

    dc.dcDocHasPageHeaders= h;
    dc.dcDocHasPageFooters= f;
    }

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{ title= utilMemoryBufferGetString( &(dp->dpTitle) );	}

    if  ( psSetNupSchema( &ps, dg, pg, dc.dcDocHasPageHeaders,
						    dc.dcDocHasPageFooters ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docPsPrintGetDocumentFonts( &pstl, lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psStartDSCDocument( &ps, &pstl,
			    title, applicationName, applicationReference );

    sioOutPrintf( sos, "%%%%BeginProlog\n" );

    psSetDefinePageFontImplementation( sos );
    psSetUtf8ShowImplementation( sos );
    psSetMvsImplementation( sos );

    docPsSaveTabLeaderProcedures( sos );
    psDefineBorderProcs( sos );

    psSetPdfmarkEmulation( sos );
    psSetRectfillEmulation( sos );
    psSetSelectfontEmulation( sos );

    psDefineEpsProcs( sos );

    if  ( pg->pgEmbedFonts )
	{ psIncludeFonts( sos, &pstl );	}
# if 0
    /* First fix definition of fonts in images */
    psSelectFontProcedures( sos, &pstl, /*allFonts=*/ 0 );
# else
    psSelectFontProcedures( sos, &pstl, /*allFonts=*/ 1 );
# endif

    psMetaDefineProcset( sos );
    psSetPatternImplementation( sos, shadingMesh );

    sioOutPrintf( sos, "%%%%EndProlog\n" );
    sioOutPrintf( sos, "%%%%BeginSetup\n" );

#   if 1
    sioOutPrintf( sos, "<< /PageSize [%d %d] >> setpagedevice\n",
	    ( ps.psPrintGeometry.pgSheetGeometry.dgPageWideTwips+ 19 )/ 20, 
	    ( ps.psPrintGeometry.pgSheetGeometry.dgPageHighTwips+ 19 )/ 20 );
#   endif

    if  ( ps.psUsePostScriptFilters )
	{ psImageQualityDistillerparams( sos );	}

    if  ( pg->pgCustomPsSetupFilename )
	{
	struct SimpleInputStream *	sis;

	sis= sioInFileioOpenS( pg->pgCustomPsSetupFilename );
	if  ( ! sis )
	    { SXDEB(pg->pgCustomPsSetupFilename,sis);	}
	else{
	    if  ( sioCopyStream( sos, sis ) )
		{ LDEB(1);	}

	    sioInClose( sis );
	    }
	}

    if  ( ps.psTagDocumentStructure )
	{
	if  ( psPdfmarkMarkedDocumentSetup( &ps,
			    textGetMsLocaleTagById( dp->dpDefaultLocaleId ) ) )
	    { LDEB(dp->dpDefaultLocaleId);	}
	}

    if  ( docHasDocumentInfo( dp ) )
	{
	docPsDocinfoPdfmark( &ps, applicationName, applicationReference, dp );
	}

    if  ( emitOutline )
	{
	docPsOutlinePdfmarks( &ps, bd );
	}

    sioOutPrintf( sos, "%%%%EndSetup\n" );

    if  ( firstPage < 0 )
	{ firstPage= 0;	}

    if  ( pg->pgPrintOddSides		&&
	  pg->pgPrintEvenSides		&&
	  ! pg->pgPrintSheetsReverse	&&
	  ! pg->pgPrintBookletOrder	)
	{
	if  ( docPsPrintPageRange( &ps, &dc, bodyNode,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); rval= -1; goto ready;	}
	}
    else{
	if  ( pg->pgPrintBookletOrder )
	    { LDEB(pg->pgPrintBookletOrder); }

	if  ( docPsPrintPageRange( &ps, &dc, bodyNode,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); rval= -1; goto ready;	}
	}

  ready:

    psCleanPrintingState( &ps );

    psCleanPostScriptFaceList( &pstl );

    docResetExternalTreeLayout( bd );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Booklet printing.							*/
/*									*/
/*  Document has n pages.						*/
/*  On the front (odd side) of sheet s left,right: page n-2s-1, 2s	*/
/*  On the back (even side) of sheet s left,right: page 2s-1, n-2s-2	*/
/*									*/
/************************************************************************/

