/************************************************************************/
/*									*/
/*  Layout of a document. Layout of a series of paragraphs in a common	*/
/*  parent.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	"docStripLayoutJob.h"
#   include	"layoutContext.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>
#   include	<docSelect.h>
#   include	<docFrameProperties.h>
#   include	<docRowProperties.h>
#   include	<docParaProperties.h>
#   include	<docBlockFrame.h>
#   include	<docAttributes.h>
#   include	<docBreakKind.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	DEBUG_PROGRESS		0

/************************************************************************/

static int docLayoutChildInStrip(
			int *				pStopCode,
			ParagraphLayoutPosition *	plp,
			BlockFrame *			bf,
			const LayoutJob *		lj,
			int				cellTopInset,
			int				isRedo,
			struct BufferItem *		childNode )
    {
    switch( childNode->biLevel )
	{
	case DOClevPARA:
	    return docLayoutParagraphInStrip( pStopCode, plp, bf, lj,
					    cellTopInset, isRedo, childNode );
	case DOClevROW:
	    return docLayoutRowInStrip( pStopCode, plp, bf, lj, childNode );
	default:
	    LDEB(childNode->biLevel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Preparation for vertical alignment of tabe cells. It is also used	*/
/*  to position the contents of a frame.				*/
/*									*/
/************************************************************************/

void docRedoParaStripLayout(	const LayoutJob *		lj,
				BlockFrame *			bf,
				const LayoutPosition *		lpFrom,
				struct BufferItem *		cellNode,
				int				childFrom,
				int				childUpto )
    {
    int				stopCode= FORMATstopREADY;
    int				child;

    ParagraphLayoutPosition	plpRedo;
    const int			isRedo= 1;

    docInitParagraphLayoutPosition( &plpRedo );

    docStripLayoutStartChild( &plpRedo, childFrom );
    plpRedo.plpPos= *lpFrom;

    for ( child= childFrom; child < childUpto; child++ )
	{
	const int	cellTopInset= 0; /* irrelevant: we shift */

	if  ( docLayoutChildInStrip( &stopCode, &plpRedo, bf, lj,
		    cellTopInset, isRedo, cellNode->biChildren[child] ) < 0 )
	    { LDEB(child); return;	}
	}

    return;
    }

/************************************************************************/

static int docLayoutGetChildFrame(
				int *				pFrameNumber,
				const FrameProperties **	pFp,
				const struct BufferDocument *	bd,
				const struct BufferItem *	childNode )
    {
    int		isFrame= 0;
    int		frameNumber;
    const FrameProperties *	fp= (const FrameProperties *)0;

    switch( childNode->biLevel )
	{
	case DOClevPARA:
	    frameNumber= childNode->biParaProperties->ppFrameNumber;
	    break;
	case DOClevROW:
	    frameNumber= childNode->biRowProperties->rpFrameNumber;
	    break;
	default:
	    LDEB(childNode->biLevel);
	    frameNumber= -1;
	    break;
	}

    if  ( frameNumber >= 0 )
	{
	fp= docGetFramePropertiesByNumber( bd, frameNumber );
	isFrame= DOCisFRAME( fp );
	}

    *pFp= fp;
    *pFrameNumber= frameNumber;
    return isFrame;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a series of paragraphs and child rows as fit on	*/
/*  the current page. (Actually the newspaper style column on the page).*/
/*									*/
/*  a)  Do not format children that belong to different frames.		*/
/*									*/
/************************************************************************/

int docLayoutStripChildren(	int *				pStopCode,
				ParagraphLayoutJob *		plj,
				BlockFrame *			bfFlow,
				struct BufferItem *		cellNode )
    {
    const LayoutJob *		lj= plj->pljLayoutJob;
    const LayoutContext *	lc= lj->ljContext;
    ParagraphLayoutPosition *	plp= &(plj->pljPos);
    const int			childFrom= plp->pspChild;
    struct BufferItem *		childNode= cellNode->biChildren[childFrom];
    const int			isRedo= 0;

    BlockFrame *		bf= bfFlow;

    const struct BufferDocument * bd= lc->lcDocument;

    const FrameProperties *	fpBegin= (const FrameProperties *)0;
    int				frameBegin;
    int				wasFrame;

    const FrameProperties *	fpRow= (const FrameProperties *)0;
    int				frameRow= -1;
    int				rowIsFrame= 0;

    struct BufferItem *		rowNode;

    if  ( ! docIsCellNode( cellNode ) )
	{ SDEB(docLevelStr(cellNode->biLevel)); return -1;	}
    if  ( childFrom >= cellNode->biChildCount )
	{ LLDEB(childFrom,cellNode->biChildCount); return -1;	}

    rowNode= docGetRowNode( cellNode );
    if  ( rowNode )
	{
	DocumentPosition	dp;

	if  ( ! docHeadPosition( &dp, rowNode ) )
	    {
	    rowIsFrame= docLayoutGetChildFrame( &frameRow, &fpRow,
							    bd, dp.dpNode );
	    }
	}

    /*  a  */
    childNode= cellNode->biChildren[plj->pljPos.pspChild];
    wasFrame= docLayoutGetChildFrame( &frameBegin, &fpBegin, bd, childNode );
    if  ( wasFrame && ( ! rowIsFrame || frameRow != frameBegin ) )
	{
	BlockFrame			bfTextFrame;
	ParagraphLayoutPosition		plpTextFrame;
	int				frameHeight= fpBegin->fpHighTwips;

	if  ( frameHeight < 0 )
	    { frameHeight= -frameHeight;	}

	plpTextFrame= *plp;
	plp= &plpTextFrame;

	docLayoutInitBlockFrame( &bfTextFrame );
	docLayoutSetTextFrame( &bfTextFrame, &(plp->plpPos),
						bf, fpBegin, frameHeight );

	plp->plpPos.lpPageYTwips= bfTextFrame.bfContentRect.drY0;
	bf= &bfTextFrame;
	}

    while( plp->pspChild < plj->pljChildUpto )
	{
	int			stopCode= FORMATstopREADY;
	int			ret;

	const FrameProperties *	fpHere= (const FrameProperties *)0;
	int			frameHere;
	int			isFrame;

	childNode= cellNode->biChildren[plp->pspChild];
	isFrame= docLayoutGetChildFrame( &frameHere, &fpHere, bd, childNode );

	/*  a  */
	if  ( isFrame != wasFrame				||
	      ( wasFrame && ( frameHere != frameBegin )	)	)
	    {
	    if  ( wasFrame )
		{
		docLayoutFinishFrame( fpBegin, bf, bfFlow, lj,
			&(plj->pljPos), cellNode, childFrom, plp->pspChild );

		plj->pljPos= *plp;
		}

	    *pStopCode= FORMATstopNEXT_FRAME;
	    break;
	    }

	ret= docLayoutChildInStrip( &stopCode, plp, bf, lj,
				cellNode->biCellTopInset, isRedo, childNode );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;		}

	switch( ret )
	    {
	    case FORMATstopBLOCK_FULL:
	    case FORMATstopPAGE_BREAK:
	    case FORMATstopCOLUMN_BREAK:
		*pStopCode= stopCode;
		break;

	    case FORMATstopPARTIAL:
	    case FORMATstopREADY:
		continue;

	    default:
		LDEB(ret); continue;
	    }

	break;
	}

    if  ( docLayoutStripDone( plp, plj ) )
	{ *pStopCode= FORMATstopREADY;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a start point for the formatter to layout lines. A certain	*/
/*  work ahead is necessary to decide whether work on line layout can	*/
/*  be used or not. docCommitStripLayout() is used to decide what	*/
/*  work is final and what possibly is to be redone.			*/
/*									*/
/*  docCommitStripLayout() is called when a column is full. In this	*/
/*  case, everything that is not final yet needs to be moved to the	*/
/*  next page.								*/
/*									*/
/*  -   Insure that the constraints comming from Widow/Orphan control	*/
/*	are been obeyed.						*/
/*  -   Insure that paragraphs that should not be divided over pages	*/
/*	are not split.							*/
/*  -   Insure that paragraphs that should be with the next one as a	*/
/*	header really are with it. (Experimenting with Word shows that	*/
/*	paragraphs that should be with the next one should be		*/
/*	completely with it.)						*/
/*									*/
/*  When any of the above is violated, find a solution that respects	*/
/*  the requirements by pushing offending paragraphs or lines down.	*/
/*									*/
/*  docCommitStripLayout() shifts 'plp0' to the last position that is	*/
/*  not below 'plp1' that is not influenced by the layout of position	*/
/*  on 'plp1' or below. Everything before 'plp1' is formatted and	*/
/*  plp1->plpPos is the position where formatting would continue if	*/
/*  docCommitStripLayout() moved 'plp0' onto 'plp1'. (As it often does)	*/
/*									*/
/*  REGULAR APPROACH							*/
/*  ================							*/
/*  a)  Avoid superfluous work and the handling of this exception	*/
/*	below: Commit an explit page break.				*/
/*  b)	If the current position is a legal position for a page break..	*/
/*	break here.							*/
/*									*/
/*  'Keep With Next' and 'Widow/Orphan Contol'				*/
/*  ==========================================				*/
/*  1)  Find the place to begin. Anything on a previous page is final:	*/
/*	It will not be shifted to the next page. (Paragraph)		*/
/*  2)	Idem: Line. The line found here might very well be irrelevant	*/
/*	because a different paragraph to commit is found below.		*/
/*  3)	Commit all paragraphs before the current one that do not have	*/
/*	the 'Keep with Next' flag.					*/
/*	flag.								*/
/*  4)	If the paragraph to be committed is before the current one	*/
/*	commit the position we have found. This can be:			*/
/*	-   The first line on the current page (from 2)			*/
/*	    [ This case is however excluded by the exception code (0)].	*/
/*	-   The head of a series of KeepWithNext paragraphs. (from 3)	*/
/*  5)	When this paragraph is to be kept together, restart from its	*/
/*	beginning.							*/
/*  6)	When Widow/Orphan control is active.. Do not commit the first	*/
/*	line before the whole paragraph is committed.			*/
/*  7)	The special cases of the last line of a paragraph with widow	*/
/*	orphan control and a paragraph with three lines are covered	*/
/*	in docLayoutStripChildren() at the end of a paragraph.		*/
/*	So no exceptions have to be made here. Also note the		*/
/*	preparations in docAdjustParaLayout().				*/
/*									*/
/*  8)	And finally when no exception applies, start from where we are	*/
/*	now.								*/
/*									*/
/************************************************************************/

/* Implement (a) */
static int docCommitStripAtExplicitPageBreak(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				const struct BufferItem *	cellNode )
    {
    if  ( plp1->pspChild < cellNode->biChildCount )
	{
	const struct BufferItem *	childNode1;

	childNode1= cellNode->biChildren[plp1->pspChild];

	if  ( childNode1->biLevel == DOClevPARA )
	    {
	    int				line1= plp1->pspLine;

	    if  ( line1 == 0						&&
		  childNode1->biParaProperties->ppBreakKind != DOCibkNONE )
		{
		*plp0= *plp1;
		return 1;
		}

	    if  ( line1- 1 >= 0					&&
		  line1- 1 < childNode1->biParaLineCount	)
		{
		const TextLine *	tl= childNode1->biParaLines+ line1- 1;

		if  ( tl->tlFlags & TLflagBLOCKBREAK )
		    {
		    *plp0= *plp1;
		    return 1;
		    }
		}
	    }
	}

    return 0;
    }

/* Implement (b) */
static int docCommitStripAtLegalPosition(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				const struct BufferItem *	cellNode )
    {
    if  ( plp1->pspChild < cellNode->biChildCount )
	{
	const struct BufferItem *	childNode1;

	childNode1= cellNode->biChildren[plp1->pspChild];

	if  ( childNode1->biLevel == DOClevPARA			&& /*!nestrow*/
	      ! PP_IS_ONE_PAGE( childNode1->biParaProperties )	&&
	      plp1->pspLine > 0					)
	    {
	    int		line= 1;

	    if  ( childNode1->biParaProperties->ppWidowControl )
		{ line++;	}

	    if  ( plp1->pspLine >= line )
		{
		*plp0= *plp1;
		return 1;
		}
	    }

	/* TODO: Ignores /trkeep,/trkeepn etc */
	if  ( childNode1->biLevel == DOClevROW ) /* nestrow */
	    {
	    plp0->pspChild= plp1->pspChild;
	    plp0->pspChildAdvanced= plp1->pspChildAdvanced;
	    return 1;
	    }
	}

    return 0;
    }

/* Implement (1) */
static const struct BufferItem * docCommitParagraphsBeforeFrame(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode )
    {
    const struct BufferItem *	paraNode0;

    paraNode0= cellNode->biChildren[plp0->pspChild];

    while( plp0->pspChild < plp1->pspChild )
	{
	if  ( docCompareLayoutPositionFrames( &(paraNode0->biBelowPosition),
							lpHere ) >= 0 )
	    { break;	}

	docStripLayoutNextChild( plp0 );
	paraNode0= cellNode->biChildren[plp0->pspChild];
	}

    return paraNode0;
    }

/* Implement (2) */
static void docCommitLinesBeforeFrame(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	paraNode0 )
    {
    int				line0;
    const TextLine *		tl;

    if  ( plp0->pspChild < plp1->pspChild )
	{ line0= paraNode0->biParaLineCount;	}
    else{ line0= plp1->pspLine;			}

    tl= paraNode0->biParaLines+ plp0->pspLine;
    while( plp0->pspLine < line0 && plp0->pspLine < paraNode0->biParaLineCount )
	{
	if  ( docCompareLayoutPositionFrames( &(tl->tlTopPosition),
								lpHere ) >= 0 )
	    { break;	}

	plp0->pspLine++; tl++;
	}
    }

/* Implement (3) */
static const struct BufferItem * docCommitParagraphsNoKeepNext(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				int				advanceAnyway,
				const struct BufferItem *	cellNode,
				const struct BufferItem *	paraNode0 )
    {
    int	para;

    for ( para= plp0->pspChild; para < plp1->pspChild; para++ )
	{
	const struct BufferItem *	childNode= cellNode->biChildren[para];

	if  ( advanceAnyway					||
	      childNode->biLevel != DOClevPARA			|| /* nestrow */
	      ! childNode->biParaProperties->ppKeepWithNext	)
	    {
	    docStripLayoutStartChild( plp0, para+ 1 );

	    if  ( plp0->pspChild < cellNode->biChildCount )
		{ paraNode0= cellNode->biChildren[plp0->pspChild];	}
	    /*  else .. return below */
	    }
	}

    return paraNode0;
    }

/* Implement (6) */
static int docCommitStripToAvoidOrphan(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				int				advanceAnyway,
				const struct BufferItem *	cellNode )
    {
    if  ( plp1->pspChild < cellNode->biChildCount )
	{
	const struct BufferItem *	childNode1;

	childNode1= cellNode->biChildren[plp1->pspChild];

	if  ( ! advanceAnyway					&&
	      childNode1->biLevel == DOClevPARA			&&
	      childNode1->biParaProperties->ppWidowControl	&&
	      plp1->pspLine == 1				&&
	      childNode1->biParaLineCount >= 1			)
	    {
	    const TextLine *		tl= childNode1->biParaLines+ 0;

	    if  ( tl->tlFirstParticule+ tl->tlParticuleCount <
				childNode1->biParaParticuleCount	)
		{
		docStripLayoutStartChild( plp0, plp0->pspChild );
		return 1;
		}
	    }
	}

    return 0;
    }

static void docCommitStripLayout_x(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				const int			advanceAnyway,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode )
    {
    const struct BufferItem *	paraNode0;

    paraNode0= cellNode->biChildren[plp0->pspChild];

    if  ( paraNode0->biLevel != DOClevPARA )
	{ LSDEB(paraNode0->biLevel,docLevelStr(paraNode0->biLevel)); return; }

    /*  a  */
    if  ( docCommitStripAtExplicitPageBreak( plp0, plp1, cellNode ) )
	{ return;	}

    /*  b  */
    if  ( docCommitStripAtLegalPosition( plp0, plp1, cellNode ) )
	{ return;	}

    /*  1  */
    paraNode0= docCommitParagraphsBeforeFrame( plp0, plp1, lpHere, cellNode );
    /*  2  */
    docCommitLinesBeforeFrame( plp0, plp1, lpHere, paraNode0 );

    /*  3  */
    paraNode0= docCommitParagraphsNoKeepNext( plp0, plp1, advanceAnyway,
						    cellNode, paraNode0 );

    /*  4  */
    if  ( plp0->pspChild < plp1->pspChild )
	{ return;	}

    if  ( plp0->pspChild != plp1->pspChild )
	{
	LLLLDEB(lpHere->lpPage,lpHere->lpColumn,plp0->pspChild,plp1->pspChild );	}

    if  ( plp0->pspChild >= cellNode->biChildCount )
	{ return;	}

    /*  5  */
    if  ( ! advanceAnyway						&&
	  ( paraNode0->biLevel != DOClevPARA			||
	    PP_IS_ONE_PAGE( paraNode0->biParaProperties )	)	)
	{
	docStripLayoutStartChild( plp0, plp0->pspChild );
	return;
	}

    /*  6  */
    if  ( docCommitStripToAvoidOrphan( plp0, plp1, advanceAnyway, cellNode ) )
	{ return;	}

    /*  7  */

    /*  8  */
    *plp0= *plp1;

    return;
    }

/************************************************************************/
/*									*/
/*  Find out whether the fact that layout has proceeded to plj->pljPos	*/
/*  makes any layout final. If so.. Move pljPos0 past what has become	*/
/*  final.								*/
/*									*/
/************************************************************************/

# define docSetLayoutProgress( plpt, plpf ) \
    { \
    (plpt)->pspChild= (plpf)->pspChild; \
    (plpt)->pspChildAdvanced= (plpf)->pspChildAdvanced; \
    (plpt)->pspPart= (plpf)->pspPart; \
    (plpt)->pspLine= (plpf)->pspLine; \
    }

void docCommitStripLayout(	int *				pAdvanced,
				const int			advanceAnyway,
				ParagraphLayoutJob *		plj,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode )
    {
    int				advanced= 0;
    ParagraphLayoutPosition	plp0;

    plp0= plj->pljPos0;

    docCommitStripLayout_x( &plp0, &(plj->pljPos),
				    advanceAnyway, lpHere, cellNode );

    advanced= ( docCompareLayoutProgress( &plp0, &(plj->pljPos0) ) < 0 );
    if  ( advanced )
	{
	docSetLayoutProgress( &(plj->pljPos0), &plp0 );
	docSetLayoutProgress( &(plj->pljPos), &plp0 );
	}
    else{
#	if 0
	int cmp= docCompareLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );
	if  ( cmp >= 0 )
	    { LDEB(cmp);	}
#	endif
	/* Prevent loops: Be sure to advance */
	if  ( advanceAnyway && ! advanced )
	    {
	    SLLDEB("#####",advanceAnyway,advanced);
	    docSetLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );
	    advanced= 1;
	    }
	}

    *pAdvanced= advanced;
    }

/************************************************************************/
/*									*/
/*  Find out where to start formatting a strip.				*/
/*  After some thought: That is the position that would be committed	*/
/*  if the strip were formatted upto the start position.		*/
/*									*/
/************************************************************************/

void docFindStripLayoutOrigin(	ParagraphLayoutJob *		plj,
				const struct LayoutPosition *	lpHere,
				const struct BufferItem *	cellNode )
    {
    int				advanceAnyway= 0;

    docCommitStripLayout_x( &(plj->pljPos0), &(plj->pljPos),
					advanceAnyway, lpHere, cellNode );

    return;
    }

/************************************************************************/

#   if DEBUG_PROGRESS

static void logLayoutProgress(  const char *			head,
				const BufferItem *		parentNode,
				const ParagraphLayoutJob *	plj )
    {
    const BufferItem *	bodySectNode= plj->pljLayoutJob->ljBodySectNode;

    appDebug( "%s P/C=%d/%d %3d .. %3d || %3d SECT %d %s\n", head,
			plj->pljPos0.plpPos.lpPage,
			plj->pljPos0.plpPos.lpColumn,
			plj->pljPos0.pspChild,
			plj->pljPos.pspChild,
			plj->pljChildUpto,
			bodySectNode->biNumberInParent,
			docTreeTypeStr( parentNode->biTreeType ) );
    }

#   endif

/************************************************************************/
/*									*/
/*  Format the lines in a series of paragraphs. On the way keep an	*/
/*  administration on where to restart formatting at a page break.	*/
/*									*/
/*  1)  Place as much as fits in the first frame (page,column).		*/
/*  2)  While unformatted paragraphs remain: place some more in		*/
/*	subsequent frames.						*/
/*  3)  First place the footnotes in the frame.				*/
/*  4)  Then find out where to restart the layout job on the next page	*/
/*	Widow/Orphan control etc can cause some of the text that we	*/
/*	already formatted to go to the next page.			*/
/*  5)  Skip to the next frame.						*/
/*  5a) If we already are at the head of the frame, it does not make	*/
/*	sense to skip to the next frame to make things fit.		*/
/*  5b) If even that does not help.. Skip the offending paragraph to	*/
/*	prevent loops.							*/
/*  6)  Determine available space in the next frame.			*/
/*  7)  Place as much as fits in the next frame.			*/
/*									*/
/************************************************************************/

int docLayoutStackedStrip(	struct BufferItem *		parentNode,
				BlockFrame *			bf,
				ParagraphLayoutJob *		plj )
    {
    LayoutPosition		lpPrevRound;

    int				stopCode= FORMATstopREADY;

    lpPrevRound= plj->pljPos.plpPos;

#   if DEBUG_PROGRESS
    logLayoutProgress( ">>>>>>>>", parentNode, plj );
#   endif

    /*  1  */
    if  ( docLayoutStripChildren( &stopCode, plj, bf, parentNode ) )
	{ LDEB(1); return -1;	}

#   if DEBUG_PROGRESS
    logLayoutProgress( ">.......", parentNode, plj );
#   endif

    /*  2  */
    while( stopCode != FORMATstopREADY		&&
	   stopCode != FORMATstopFRAME_FULL	)
	{
	int				advanced;
	const int			belowText= 0;
	LayoutPosition			lpBelowNotes;

	/*  3  */
	if  ( BF_HAS_FOOTNOTES( bf )					&&
	      ( parentNode->biTreeType == DOCinBODY		||
	        parentNode->biTreeType == DOCinENDNOTE	)	&&
	      ! bf->bfFootnotesPlaced					&&
	      docLayoutFootnotesForColumn( &lpBelowNotes,
			    &(plj->pljPos.plpPos), bf, belowText,
			    plj->pljLayoutJob )	)
	    { LDEB(1); return -1;	}

	/*  4  */
	{
	const int advanceAnyway= 0;
	docCommitStripLayout( &advanced, advanceAnyway, plj,
						&lpPrevRound, parentNode );
	}

#	if DEBUG_PROGRESS
	logLayoutProgress( "--------", parentNode, plj );
#	endif

	/*  5  */
	switch( stopCode )
	    {
	    case FORMATstopBLOCK_FULL:
		/*  5a  */
		if  ( ! advanced && lpPrevRound.lpAtTopOfColumn )
		    {
		    const int advanceAnyway= 1;
		    docCommitStripLayout( &advanced, advanceAnyway, plj,
						&lpPrevRound, parentNode );
		    /*  5b  */
		    if  ( ! advanced )
			{
			LDEB(advanced);
			LPOSDEB(&lpPrevRound);
			LPOSDEB(&(plj->pljPos0.plpPos));
			LPOSDEB(&(plj->pljPos.plpPos));
			RECTDEB(
			    &(plj->pljPos.plpParagraphFrame.pfParaContentRect));

			docStripLayoutNextChild( &(plj->pljPos) );
			docSetLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );

			if  ( docLayoutStripDone( &(plj->pljPos), plj ) )
			    { break;	}
			}
		    }

		docLayoutToNextColumn( &(plj->pljPos.plpPos),
					bf, parentNode, plj->pljLayoutJob );
		break;

	    case FORMATstopCOLUMN_BREAK:
		docLayoutToNextColumn( &(plj->pljPos.plpPos),
					bf, parentNode, plj->pljLayoutJob );
		/*Explicit breaks DO advance!*/
		advanced= 1;
		break;

	    case FORMATstopPAGE_BREAK:
		docLayoutToNextPage( &(plj->pljPos.plpPos),
					bf, parentNode, plj->pljLayoutJob );
		/*Explicit breaks DO advance!*/
		advanced= 1;
		break;

	    case FORMATstopREADY:
		break;
	    case FORMATstopFRAME_FULL:
		break;

	    default:
		LLDEB(stopCode,advanced);
		break;
	    }

	/*  6  */
	docCellStripFrame( parentNode, bf, plj );

	/*  7  */
	lpPrevRound= plj->pljPos.plpPos;

#	if DEBUG_PROGRESS
	logLayoutProgress( "++++++++", parentNode, plj );
#	endif

	if  ( docLayoutStripChildren( &stopCode, plj, bf, parentNode ) )
	    { LDEB(1); return -1;	}

#	if DEBUG_PROGRESS
	logLayoutProgress( "........", parentNode, plj );
#	endif
	}

    if  ( parentNode->biChildCount > 0 )
	{ parentNode->biTopPosition= parentNode->biChildren[0]->biTopPosition; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the formatting frame for a paragraph layout.		*/
/*									*/
/************************************************************************/

void docCellStripFrame(		struct BufferItem *		cellNode,
				const BlockFrame *		bf,
				ParagraphLayoutJob *		plj )
					
    {
    struct BufferItem *		childNode;
    ParagraphFrame *		pf= &(plj->pljPos.plpParagraphFrame);

    childNode= cellNode->biChildren[plj->pljPos.pspChild];

    if  ( childNode->biLevel == DOClevPARA )
	{ docParagraphFrameTwips( pf, bf, childNode );	}
    else{ docCellFrameTwips( pf, bf, cellNode );	}

    return;
    }

