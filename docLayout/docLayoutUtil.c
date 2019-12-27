/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<limits.h>
#   include	<stdio.h>

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	"layoutContext.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	<docSelect.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>
#   include	<docEditRange.h>
#   include	<docDocumentProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize a layout job.						*/
/*									*/
/************************************************************************/

void docInitLayoutJob(	LayoutJob *	lj )
    {
    lj->ljChangedRectanglePixels= (struct DocumentRectangle *)0;
    lj->ljContext= (const struct LayoutContext *)0;
    lj->ljChangedNode= (struct BufferItem *)0;
    lj->ljReachedDocumentBottom= 0;

    lj->ljBalancePage= -1;
    lj->ljBalanceY1= 0;

    lj->ljBodySectNode= (const struct BufferItem *)0;

    lj->ljStartScreenParagraph= (START_SCREEN_PARAGRAPH)0;
    lj->ljScreenLayoutLine= (SCREEN_LAYOUT_LINE)0;

    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

void docCleanLayoutJob(	LayoutJob *	lj )
    {
    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Invalidate the layout of all paragraphs in the modified range.	*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docInvalidateChangedLayout(
				struct BufferItem *		node,
				const struct DocumentSelection * ds,
				const struct BufferItem *	bodySectNode,
				void *				through )
    {
    if  ( node->biLevel == DOClevPARA )
	{ docInvalidateParagraphLayout( node );	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Redo layout of the relevant part of the document after editing.	*/
/*									*/
/************************************************************************/

int docLayoutInvalidateRange(	struct DocumentSelection *	dsLayout,
				struct BufferDocument *		bd,
				const struct DocumentTree *	tree,
				EditRange *			er )
    {
    const int		direction= 1;
    const int		flags= 0;
    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( er->erHead.epParaNr == 0 )
	{
	LDEB(er->erHead.epParaNr);
	er->erHead.epParaNr= 1;
	}

    {
    struct BufferItem *	headNode;

    headNode= docGetParagraphByNumber( tree, er->erHead.epParaNr );

    if  ( headNode )
	{
	if  ( docHeadPosition( &dpHead, headNode ) )
	    { LDEB(er->erHead.epParaNr); return -1;	}
	}
    else{
	/*LXDEB(er->erHead.epParaNr,headNode);*/
	if  ( docHeadPosition( &dpHead, tree->dtRoot ) )
	    { LDEB(er->erHead.epParaNr); return -1;	}
	}
    }

    {
    struct BufferItem *	tailNode;

    tailNode= docGetParagraphByNumber( tree, er->erTail.epParaNr );

    if  ( tailNode )
	{
	if  ( docTailPosition( &dpTail, tailNode ) )
	    { LDEB(er->erTail.epParaNr); return -1;	}
	}
    else{
	/* LXDEB(er->erTail.epParaNr,tailNode); */
	if  ( docTailPosition( &dpTail, tree->dtRoot ) )
	    { LDEB(er->erTail.epParaNr); return -1;		}
	}
    }

    docInitDocumentSelection( dsLayout );
    docSetRangeSelection( dsLayout, &dpHead, &dpTail, direction );

    if  ( docScanSelection( bd, dsLayout,
			    docInvalidateChangedLayout, (NodeVisitor)0,
			    (TreeVisitor)0, (TreeVisitor)0, 
			    flags, (void *)0 ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the frame when a formatting task is (re) started for a	*/
/*  buffer item.							*/
/*									*/
/*  3)  If the preceding paragraph ends on the same page where this	*/
/*	nodes begins, reserve space for the footnotes upto the		*/
/*	beginning of this block and subtract the footnote height from	*/
/*	the buttom of the frame.					*/
/*									*/
/************************************************************************/

static int docLayoutGetInitialFrameImpl(
					BlockFrame *		bf,
					const LayoutJob *	lj,
					const LayoutPosition *	lpHere,
					struct BufferItem *	node )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const struct BufferItem *	prevParaNode= (const struct BufferItem *)0;

    /*  3  */
    if  ( node->biTreeType == DOCinBODY )
	{ prevParaNode= docPrevParagraph( node );	}

    if  ( prevParaNode							&&
	  prevParaNode->biBelowPosition.lpPage >= lpHere->lpPage	)
	{
	DocumentPosition		dpHere;
	int				partHere;

	if  ( docHeadPosition( &dpHere, node ) )
	    { LDEB(1); return -1;	}
	partHere= 0;

	if  ( docCollectFootnotesFromColumn( bf, &dpHere, partHere, bd,
					lpHere->lpPage, lpHere->lpColumn ) )
	    { LDEB(lpHere->lpPage); return -1;	}
	}

    return 0;
    }

int docLayoutGetInitialFrame(		BlockFrame *		bf,
					const LayoutJob *	lj,
					const LayoutPosition *	lpHere,
					struct BufferItem *	node )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;

    /*  2  */
    if  ( lj->ljBodySectNode )
	{
	docSectionBlockFrameTwips( bf, node, lj->ljBodySectNode,
				    bd, lpHere->lpPage, lpHere->lpColumn );
	}
    else{
	XDEB(lj->ljBodySectNode);
	docBlockFrameTwips( bf, node, bd, lpHere->lpPage, lpHere->lpColumn );
	}

    return docLayoutGetInitialFrameImpl( bf, lj, lpHere, node );
    }

/************************************************************************/
/*									*/
/*  Adjust the bottom of a node to changes inside.			*/
/*									*/
/************************************************************************/

void docLayoutSetBottomPosition( int *				pChanged,
				LayoutPosition *		lpBelow,
				const LayoutPosition *		lp,
				const LayoutContext *		lc,
				DocumentRectangle *		drChanged )
    {
    int			changed= 0;

    int			oldY1Pixels;
    int			newY1Pixels;

    oldY1Pixels= docLayoutYPixels( lc, lpBelow );

    if  ( ! DOC_SAME_POSITION( lpBelow, lp ) )
	{ *lpBelow= *lp; changed= 1; }

    newY1Pixels= docLayoutYPixels( lc, lp );

    if  ( oldY1Pixels < newY1Pixels )
	{
	if  ( drChanged					&&
	      drChanged->drY1 < newY1Pixels -1	)
	    { drChanged->drY1=  newY1Pixels -1;	}
	}

    if  ( oldY1Pixels > newY1Pixels )
	{
	if  ( drChanged							&&
	      drChanged->drY1 < oldY1Pixels- 1	)
	    { drChanged->drY1=  oldY1Pixels- 1;	}
	}

    /* or */
    if  ( pChanged && changed )
	{ *pChanged= changed;	}

    return;
    }

void docLayoutSetNodeBottom(	int *			pChanged,
				struct BufferItem *	node,
				const LayoutPosition *	lp,
				const LayoutContext *	lc,
				DocumentRectangle *	drChanged )
    {
    docLayoutSetBottomPosition( pChanged, &(node->biBelowPosition),
							lp, lc, drChanged );
    }

/************************************************************************/
/*									*/
/*  Start/Finish the layout of items: Is done around the node type	*/
/*  specific layout procedures.						*/
/*									*/
/************************************************************************/

void docLayoutStartNodeLayout(	struct BufferItem *	node,
				const LayoutJob *	lj,
				const LayoutPosition *	lpHere )
    {
    const LayoutContext *	lc= lj->ljContext;
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    int				y0;
    int				y1;

    y0= docLayoutYPixels( lc, &(node->biTopPosition) );
    y1= docLayoutYPixels( lc, &(node->biBelowPosition) )- 1;

    if  ( drChanged		&&
	  drChanged->drY0 > y0	)
	{ drChanged->drY0=  y0;	}

    node->biTopPosition= *lpHere;

    if  ( drChanged )
	{
	y0= docLayoutYPixels( lc, &(node->biTopPosition) );

	if  ( drChanged->drY0 > y0 )
	    { drChanged->drY0=  y0;			}
	if  ( drChanged->drY1 < y1 )
	    { drChanged->drY1=  y1;	}
	}

    return;
    }

void docLayoutFinishNodeLayout(	int *			pChanged,
				struct BufferItem *	node,
				const LayoutJob *	lj,
				const LayoutPosition *	lpHere )
    {
    docLayoutSetBottomPosition( pChanged, &(node->biBelowPosition),
		    lpHere, lj->ljContext, lj->ljChangedRectanglePixels );

    return;
    }

/************************************************************************/
/*									*/
/*  Find the first section on a particular page.			*/
/*									*/
/************************************************************************/

int docGetFirstSectionOnPage(		struct BufferDocument *	bd,
					int			page )
    {
    struct BufferItem *		bodyNode= bd->bdBody.dtRoot;
    int				sectNr;

    for ( sectNr= 0; sectNr < bodyNode->biChildCount; sectNr++ )
	{
	const struct BufferItem *	bodySectNode= bodyNode->biChildren[sectNr];

	if  ( bodySectNode->biTopPosition.lpPage <= page	&&
	      bodySectNode->biBelowPosition.lpPage >= page	)
	    { return sectNr;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Adapt the bottom of a block frame to avoid page breaks in trees	*/
/*  that can not hold one.						*/
/*									*/
/************************************************************************/

void docLayoutAdjustFrame(	BlockFrame *			bf,
				const struct BufferItem *	node )
    {
    /*  4  */
    if  ( node->biTreeType == DOCinFOOTNOTE		||
	  docIsSeparatorType( node->biTreeType )	||
	  docIsHeaderType( node->biTreeType )		||
	  docIsFooterType( node->biTreeType )		)
	{
	bf->bfContentRect.drY1= INT_MAX;
	bf->bfFlowRect.drY1= INT_MAX;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the font information for a text attribute value.		*/
/*									*/
/************************************************************************/

const struct AfmFontInfo * docDocLayoutGetFontInfo(
				const LayoutContext *		lc,
				const struct TextAttribute *	ta )
    {
    struct BufferDocument *	bd= lc->lcDocument;
    struct DocumentFontList *	dfl= bd->bdProperties->dpFontList;
    const struct AfmFontInfo *	afi;
    const struct IndexSet *	unicodesWanted;

    const struct PostScriptFontList *	psfl= lc->lcPostScriptFontList;

    afi= (*lc->lcGetFontForAttribute)( &unicodesWanted, ta, dfl, psfl );
    if  ( ! afi )
	{ XDEB(afi);	}

    return afi;
    }

/************************************************************************/
/*									*/
/*  String representation of stop codes.				*/
/*									*/
/************************************************************************/

const char *	docLayoutStopCodeStr(	int	stopCode )
    {
    static char	scratch[20];

    switch( stopCode )
	{
	case FORMATstopREADY:		return "READY";
	case FORMATstopNEXT_FRAME:	return "NEXT_FRAME";
	case FORMATstopFRAME_FULL:	return "FRAME_FULL";
	case FORMATstopBLOCK_FULL:	return "BLOCK_FULL";
	case FORMATstopCOLUMN_BREAK:	return "COLUMN_BREAK";
	case FORMATstopPAGE_BREAK:	return "PAGE_BREAK";
	case FORMATstopPARTIAL:		return "PARTIAL";

	default:
	    sprintf( scratch, "%d", stopCode );
	    return scratch;
	}
    }
