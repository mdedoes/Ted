/************************************************************************/
/*									*/
/*  Screen drawing specific geometry calculations.			*/
/*									*/
/*  The layout of a document is completely determined by the PostScript	*/
/*  layout process. For drawing on screen however, we have to live with	*/
/*  the fact that the fonts that we use on screen only approximately	*/
/*  match the ones that go into the PostScript/PDF files. For that	*/
/*  reason, we must apply approximations to the geometry. Those		*/
/*  approximations are made here.					*/
/*									*/
/*  In a left aligned paragraph, the first particule of a line gets a	*/
/*  fixed left position. In a right aligned paragraph the last one gets	*/
/*  a fixed right position. In a justified paragraph, the first one has	*/
/*  a fixed left position and the last one a fixed right one.		*/
/*									*/
/*  Inside a line, tabs get a pixel position that exactly matches the	*/
/*  twips position in PostScript. The same applies for the particules	*/
/*  after the vertical borders around stretches of text. The start and	*/
/*  end of shaded stretches are handled as borders.			*/
/*									*/
/*  If the text between fixed positions is left aligned:		*/
/*  -   The left hand sides of stretches have matching pixel and twips	*/
/*	positions. The rest of the stretch is placed using the pixel	*/
/*	width of the particules. (*)					*/
/*  If the text between fixed positions is right aligned:		*/
/*  -   The right hand sides of stretches have matching pixel and twips	*/
/*	positions. The rest of the stretch is placed using the pixel	*/
/*	width of the particules. (*)					*/
/*  If the text between fixed positions is justified:			*/
/*  -   The left and right hand sides of stretches have matching pixel	*/
/*	and twips positions. The particules in between are spread out	*/
/*	to fit in the space between the two positions.			*/
/*									*/
/*  (*) If more space is needed to draw in pixels than is available	*/
/*	from twips coordinates, the pixel coordinates are squeezed into	*/
/*	the twips space as if the text were justified.			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<drawDrawingSurface.h>

#   include	"docLayout.h"
#   include	"docLayoutLine.h"
#   include	<docTextRun.h>
#   include	"docScreenLayout.h"

#   include	<docTreeNode.h>
#   include	<docShape.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docObjectProperties.h>
#   include	<docObject.h>
#   include	<docPropVal.h>
#   include	<utilIndexMapping.h>
#   include	<docParaScanner.h>
#   include	<docBuf.h>
#   include	<docStripFrame.h>
#   include	<docObjects.h>
#   include	<docAttributes.h>
#   include	<textAttribute.h>
#   include	<docParaProperties.h>
#   include	<docScanner.h>
#   include	<docNodeTree.h>
#   include	"layoutContext.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	LOG_PLACEX	0

/************************************************************************/
/*									*/
/*  Open screen fonts for all text attributes in a paragraph.		*/
/*									*/
/************************************************************************/

static int docScreenLayoutOpenParaFonts(
				const LayoutContext *	lc,
				struct BufferItem *	paraNode )
    {
    int			part;
    int			textAttrNr0= -1;
    struct BufferDocument *	bd= lc->lcDocument;

    for ( part= 0; part < paraNode->biParaParticuleCount; part++ )
	{
	const TextAttribute *	ta;
	int			textAttrNr;

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	if  ( textAttrNr == textAttrNr0 )
	    { continue;	}

	textAttrNr0= textAttrNr;

	if  ( utilIndexMappingGet( lc->lcAttributeToScreenFont,
							textAttrNr0 ) >= 0 )
	    { continue;	}

	if  ( docOpenScreenFont( lc, textAttrNr0 ) < 0 )
	    {
	    LLDEB(part,textAttrNr0);
	    docListNode(bd,0,paraNode,0);
	    return -1;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docScreenLayoutStartScreenPara(
				struct BufferItem *	paraNode,
				const ParagraphFrame *	pf,
				const LayoutContext *	lc )
    {
    int				part;
    TextParticule *		tp;

    double			xfac= lc->lcPixelsPerTwip;
    struct BufferDocument *	bd= lc->lcDocument;

    if  ( docScreenLayoutOpenParaFonts( lc, paraNode ) )
	{ LDEB(1); return -1;	}

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != TPkindOBJECT )
	    { continue;	}

	io= docGetObject( bd, tp->tpObjectNumber );
	if  ( ! io )
	    { LPDEB(tp->tpObjectNumber,io); return -1;	}

	docObjectSetPixelSize( io, xfac );
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Various layout routines.						*/
/*									*/
/************************************************************************/

int docScreenLayoutNode(	int *			pReachedBottom,
				struct BufferItem *	node,
				const LayoutContext *	lc,
				DocumentRectangle *	drChanged )
    {
    LayoutJob			lj;

    docInitLayoutJob( &lj );

    if  ( lc->lcDrawingSurface )
	{ docSetScreenLayoutFunctions( &lj );	}

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljContext= lc;
    lj.ljChangedNode= node;

    if  ( docLayoutNodeAndParents( node, &lj ) )
	{ LDEB(1); return -1;	}

    if  ( pReachedBottom && lj.ljReachedDocumentBottom )
	{ *pReachedBottom= 1;	}

    docCleanLayoutJob( &lj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit tables in the shape texts.					*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docDelimitTablesInShapeText(	int		n,
					void *		vio,
					void *		vdps )
    {
    InsertedObject *		io= (InsertedObject *)vio;
    DrawingShape *		ds;
    const int			recursively= 1;

    if  ( io->ioKind != DOCokDRAWING_SHAPE )
	{ return 0;	}

    ds= io->ioDrawingShape;

    if  ( ds->dsDocumentTree.dtRoot )
	{ docDelimitTables( ds->dsDocumentTree.dtRoot, recursively );	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  (re)Calculate the layout of a whole document.			*/
/*									*/
/************************************************************************/

int docScreenLayoutDocumentBody( int *				pReachedBottom,
				struct BufferDocument *		bd,
				const LayoutContext *		lc )
    {
    DocumentRectangle		drChanged;

    geoInitRectangle( &drChanged );

    /*  Why?
    DocumentProperties *	dp= bd->bdProperties;
    if  ( dp->dpFontList->dflFontCount == 0 )
	{ LDEB(dp->dpFontList->dflFontCount); return -1;	}
    */

    if  ( docScreenLayoutNode( pReachedBottom,
					bd->bdBody.dtRoot, lc, &drChanged ) )
	{ LDEB(1); return -1;	}

    if  ( utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
				docDelimitTablesInShapeText, (void *)0 ) < 0 )
	{ LDEB(1); return -1;	}

    /* LDEB(1);docListNode(bd,0,bd->bdBody.dtRoot); */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a page header.				*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docStartScreenLayoutForTree(
				LayoutJob *			lj,
				struct DocumentTree *		tree,
				int				page,
				int				column )
    {
    const LayoutContext *	lc= lj->ljContext;

    if  ( lc->lcDrawingSurface )
	{ docSetScreenLayoutFunctions( lj );	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Determine the screen font number of the attribute belonging to the	*/
/*  scaps font size. Note that this is used for the shifted lower case	*/
/*  letters only.							*/
/*									*/
/************************************************************************/

int docLayoutScapsScreenFont(	const LayoutContext *	lc,
				const TextAttribute *	ta )
    {
    int		scapsAttrNr= docScapsAttributeNumber( lc->lcDocument, ta );
    int		scapsScreenFont;

    scapsScreenFont= docOpenScreenFont(	lc, scapsAttrNr );
    if  ( scapsScreenFont < 0 )
	{ LLDEB(scapsAttrNr,scapsScreenFont);	}

    return scapsScreenFont;
    }

/************************************************************************/

typedef struct LineScreenLayout
    {
    LineLayoutJob *		lslLineLayoutJob;
    const LayoutContext *	lslLayoutContext;

    int				lslScanFlags;
    DocumentRectangle		lslLineRectangle;

    int				lslXTwips;
    int				lslXPixels;
    } LineScreenLayout;

/************************************************************************/

static int docPlaceWidthInSegment(
				int *				pPixelsAdvanced,
				int *				pPixelsVisible,
				const struct LineScreenLayout *	lsl,
				int				screenFont,
				const char *			segmentString,
				const int			len )
    {
    DocumentRectangle		drPixels;
    int				pixelsAdvanced;

    const LayoutContext *	lc= lsl->lslLayoutContext;
    const int			yPixels= 0;

    int				x0Pixels= lsl->lslXPixels+ *pPixelsAdvanced;

    pixelsAdvanced= drawGetTextExtents( &drPixels, lc->lcDrawingSurface,
				x0Pixels, yPixels,
				screenFont, segmentString, len );

    if  ( drPixels.drX1 > drPixels.drX0 )
	{ *pPixelsVisible= drPixels.drX1- lsl->lslXPixels;	}

    *pPixelsAdvanced += pixelsAdvanced;

    return SCANadviceOK;
    }

static int docPlaceWidthInSegments(
			int *				pPixelsAdvanced,
			int *				pPixelsVisible,
			const struct LineScreenLayout *	lsl,
			int				fullScreenFont,
			int				scapsScreenFont,
			const char *			outputString,
			const int *			segments,
			int				segmentCount )
    {
    int				seg;
    int				res;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
#	    if LOG_PLACEX
	    appDebug( "%3d: FULL \"%*.*s\"\n", seg,
			segments[2* seg+ 0], segments[2* seg+ 0],
			outputString );
#	    endif

	    res= docPlaceWidthInSegment( pPixelsAdvanced, pPixelsVisible,
					lsl, fullScreenFont,
					outputString, segments[2* seg+ 0] );
	    if  ( res < 0 )
		{ LDEB(res); return res;	}
	    if  ( res > 0 )
		{ return res;	}

	    outputString += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
#	    if LOG_PLACEX
	    appDebug( "%3d: SCAP \"%*.*s\"\n", seg,
			segments[2* seg+ 1], segments[2* seg+ 1],
			outputString );
#	    endif

	    res= docPlaceWidthInSegment( pPixelsAdvanced, pPixelsVisible,
					lsl, scapsScreenFont,
					outputString, segments[2* seg+ 1] );
	    if  ( res < 0 )
		{ LDEB(res); return res;	}
	    if  ( res > 0 )
		{ return res;	}

	    outputString += segments[2* seg+ 1];
	    }
	}

    return SCANadviceOK;
    }

/************************************************************************/
/*									*/
/*  Place the particules in a text run.					*/
/*  We have three tasks:						*/
/*  -   Detect situations where the pixel width from the screen font	*/
/*	significantly differs from the pixel width calculated from the	*/
/*	twips width. [ -> Draw separately.]				*/
/*  -   Detect situations where the pixel width from the screen font	*/
/*	exceeds the space available for the run. This can be because	*/
/*	the the screen text protrudes beyond the right margin, or	*/
/*	because the screen text overlaps with subsequent particules on	*/
/*	the same line. [ -> Draw separately and squeeze.]		*/
/*  -   Detect situations where the pixel width from the screen font	*/
/*	differs from the calculated width and the text is drawn right	*/
/*	aligned. [ -> Draw separately and shift.]			*/
/*									*/
/************************************************************************/

static int docPlaceLineRun(	const TextRun *			tr,
				void *				vlsl )
    {
    int				rval= SCANadviceOK;
    LineScreenLayout *		lsl= (LineScreenLayout *)vlsl;
    const LayoutContext *	lc= lsl->lslLayoutContext;

    int				twipsWide= docTextRunTwipsWide( tr );

    const BufferItem *		paraNode= tr->trParaNode;
    int				len= tr->trStrlen;

    const char *		printString= (const char *)0;
    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    const int			mirror= 0;
    int				fullScreenFont;
    int				scapsScreenFont;
    int				fullSizeTwips;
    int				scapsSizeTwips;
    const struct AfmFontInfo *	afi;

    int				pixelsAdvanced= 0;
    int				pixelsVisible= 0;
    int				x1Twips;
    int				x1Pixels;
    int				x1PixelsT;

    if  ( docLayoutMakeOutput( &printString, &scratchString,
			&segments, &segmentCount,
			&fullScreenFont, &scapsScreenFont,
			&afi, &fullSizeTwips, &scapsSizeTwips, tr, mirror, lc,
			docParaString( paraNode, tr->trStroff ), len ) )
	{ LDEB(len); rval= -1; goto ready;	}

    if  ( segmentCount > 0 )
	{
	rval= docPlaceWidthInSegments( &pixelsAdvanced, &pixelsVisible, lsl,
					fullScreenFont, scapsScreenFont,
					printString, segments, segmentCount );
	}
    else{
#	if LOG_PLACEX
	appDebug( "%3d: ---- \"%*.*s\"\n", 0, len, len, printString );
#	endif

	rval= docPlaceWidthInSegment( &pixelsAdvanced, &pixelsVisible, lsl,
					fullScreenFont, printString, len );
	}

    lsl->lslXPixels= docLayoutXPixels( lc, lsl->lslXTwips );

    x1Twips= lsl->lslXTwips+ twipsWide;
    x1PixelsT= docLayoutXPixels( lc, x1Twips );
    x1Pixels= lsl->lslXPixels+ pixelsAdvanced;

#   if LOG_PLACEX
    appDebug( "          %4d..%4d= %3d..%3d :: %3d..%3d\n",
					    lsl->lslXTwips, x1Twips,
					    lsl->lslXPixels, x1PixelsT,
					    lsl->lslXPixels, x1Pixels );
#   endif

    lsl->lslXTwips= x1Twips;
    lsl->lslXPixels= x1Pixels;

    lsl->lslLineLayoutJob->lljPixelShift= x1PixelsT- x1Pixels;

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Place a particule.							*/
/*									*/
/************************************************************************/

static int docPlaceLineParticule( const VisitParticule *	vp,
				void *				vlsl )
    {
    int				res= SCANadviceOK;
    LineScreenLayout *		lsl= (LineScreenLayout *)vlsl;

    lsl->lslXTwips += vp->vpTextParticule->tpWide;

    if  ( vp->vpTextParticule->tpWide > 0 )
	{ lsl->lslLineLayoutJob->lljPixelShift= 0;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Perform a Screen Layout of a line of text that was already		*/
/*  formatted in 'twips' terms.						*/
/*									*/
/*  As the screen font that we matched to the actual document font	*/
/*  potentially has different horizontal metrics, we try to compensate	*/
/*  for that for two reasons:						*/
/*  -   We look for a different x0 position of right-aligned text. (To	*/
/*	a tab or to the right hand side margin)				*/
/*  -   If necessary, we squeeze the particules of the line between the	*/
/*	margins.							*/
/*  We compensate by calculating deviations from the pixel position	*/
/*  of the text particules as they are derived from the twips position.	*/
/*									*/
/************************************************************************/

static int docScreenLayoutLine(	TextLine *			tl,
				LineLayoutJob *			llj )
    {
    const LayoutContext *	lc= llj->lljLayoutContext;
    BufferItem *		paraNode= llj->lljParaNode;
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphFrame *	pf= llj->lljParagraphFrame;

    const int			isLast= tl- paraNode->biParaLines < paraNode->biParaLineCount- 1;

    int				res;

    LayoutPosition		lpTop;
    LayoutPosition		lpBelowLine;

    LineScreenLayout		lsl;

    const struct BlockOrigin * const bo= (const struct BlockOrigin *)0;

    llj->lljPixelShift= 0;

    lsl.lslLineLayoutJob= llj;
    lsl.lslLayoutContext= llj->lljLayoutContext;

    docTextLineGetShiftedTop( &lpTop, bo, tl );
    docTextLineGetShiftedBelowLine( &lpBelowLine, bo, tl );

    docLineTextRectangleTwips( &(lsl.lslLineRectangle),
						&lpTop, &lpBelowLine, tl, pf );

    if  ( pp->ppAlignment == DOCthaJUSTIFIED && ! isLast )
	{ lsl.lslScanFlags= FLAGpsSCAN_RUN_SEPARATELY;	}
    else{ lsl.lslScanFlags= FLAGpsSCAN_COMBINE_LINES;	}

    lsl.lslXTwips= lsl.lslLineRectangle.drX0;
    lsl.lslXPixels= docLayoutXPixels( lc, lsl.lslXTwips );

    res= docScanLineDisplayOrder( lc->lcDocument,
				paraNode, tl,
				(struct DocumentSelection *)0, lsl.lslScanFlags,
				docPlaceLineParticule,
				(ParaFieldVisitor)0,
				docPlaceLineRun,
				(ObjectVisitor)0,
				(TabVisitor)0,
				(void *)&lsl );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    return tl->tlParticuleCount;
    }

/************************************************************************/
/*									*/
/*  Initialize screen layout.						*/
/*									*/
/************************************************************************/

void docSetScreenLayoutFunctions(	LayoutJob *	lj )
    {
    lj->ljStartScreenParagraph= docScreenLayoutStartScreenPara;
    lj->ljScreenLayoutLine= docScreenLayoutLine;

    return;
    }

