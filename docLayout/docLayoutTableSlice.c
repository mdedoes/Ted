/************************************************************************/
/*									*/
/*  Layout of a stack of contiguous table rows.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docLayoutStopCode.h"
#   include	<docTreeNode.h>
#   include	<docRowProperties.h>
#   include	<docBlockFrame.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

# define docKeepRowWithNext( node, rowN ) \
	    ( ( (rowN) < node->biRowPastHeaderRow ) || \
	      (node)->biRowProperties->rp_Keepfollow )

#    define	DEBUG_PROGRESS		0

#   if DEBUG_PROGRESS

static void logProgress(	const char *		head,
				const LayoutJob *	lj,
				const LayoutPosition *	lpTop,
				const BufferItem *	parentNode,
				int			row,
				int			upto,
				int			stayInThisColumn )
    {
    appDebug( "%s frame:%d/%d %s  ROW %3d .. %3d  sect %2d\n",
			    head,
			    lpTop->lpPage, lpTop->lpColumn,
			    stayInThisColumn?"STAY":"    ",
			    row, upto,
			    lj->ljBodySectNode->biNumberInParent );
    }

#   endif

/************************************************************************/
/*									*/
/*  Format as many rows as possible while respecting 'Keep with Next'	*/
/*  requirements. Stop when:						*/
/*									*/
/*  * the frame is full, and we were to stay on the same page		*/
/*  * everything is ready.						*/
/*									*/
/************************************************************************/

static int docLayoutRows(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				int			inNewFrame,
				int			stayInThisColumn,
				const int		honourKeep,
				struct BufferItem *	parentNode,
				int			from,
				int			upto,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int			row;
    int			next;

    LayoutPosition	lpDone= *lpTop;
    LayoutPosition	lpHere= *lpTop;

#   if DEBUG_PROGRESS
    logProgress( "CCCCCCCC", lj, lpTop, parentNode,
					from, upto, stayInThisColumn );
#   endif

    row= from;
    next= from;
    while( row < upto )
	{
	int			found= FORMATstopREADY;
	BufferItem *		rowNode= parentNode->biChildren[row];

#	if DEBUG_PROGRESS
	logProgress( "cccc....", lj, &lpHere, parentNode,
					row, upto, stayInThisColumn );
#	endif

	docLayoutStartNodeLayout( rowNode, lj, &lpHere );

	if  ( docLayoutRowNode( &found, &lpHere, &lpHere, rowNode, bf,
					    inNewFrame, stayInThisColumn, lj ) )
	    { LDEB(row); return -1;	}

	if  ( found == FORMATstopFRAME_FULL )
	    { break;	}

	if  ( stayInThisColumn						&&
	      RP_IS_ONE_PAGE( rowNode->biRowProperties )		&&
	      docCompareLayoutPositionFrames( &lpHere, &lpDone ) > 0	)
	    { *lpBelow= lpDone; return row;	}

	if  ( row < upto- 1 )
	    {
	    docLayoutFinishNodeLayout( (int *)0, rowNode, lj, &lpHere );
	    lpDone= lpHere;
	    }
	else{
	    *lpBelow= lpHere;
	    return row+ 1;
	    }

	stayInThisColumn= honourKeep && docKeepRowWithNext( rowNode, row );
	if  ( ! stayInThisColumn )
	    { next= row+ 1;	}

	row++;
	inNewFrame= 0;
	rowNode= parentNode->biChildren[row];
	}

    *lpBelow= lpDone;
    return next;
    }

/************************************************************************/
/*									*/
/*  Try to format a series of rows in a (newspaper style) column. If	*/
/*  part of them can be	placed in the column, do so, if not, make	*/
/*  successive attempts to place them in the next column etc.		*/
/*									*/
/*  Give up if the attempts fail.					*/
/*									*/
/************************************************************************/

static int docLayoutRowsInBlock( LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				struct BufferItem *	parentNode,
				const int		startInThisColumn,
				int			from,
				int			upto,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int		honourKeep;

#   if DEBUG_PROGRESS
    logProgress( "BBBBBBBB", lj, lpTop, parentNode,
					from, upto, startInThisColumn );
#   endif

    for ( honourKeep= 1; honourKeep >= 0; honourKeep-- )
	{
	int		attempt= 0;
	int		inNewFrame= lpTop->lpAtTopOfColumn;

	int		row= from;
	LayoutPosition	lpHere= *lpTop;

	for (;;)
	    {
	    int		next;
	    int		startHere= startInThisColumn && honourKeep;

	    next= docLayoutRows( &lpHere, &lpHere,
				    inNewFrame, startHere, honourKeep,
				    parentNode, row, upto, bf, lj );
	    if  ( next < 0 )
		{ LDEB(next); return -1;	}
	    if  ( next > row )
		{
		if  ( next < upto ) /* FORMATstopFRAME_FULL with some done */
		    {
		    docLayoutToNextColumn( &lpHere, bf, parentNode, lj );

#		    if DEBUG_PROGRESS
		    logProgress( "<<<<++++", lj, &lpHere, parentNode,
					    next, upto, startInThisColumn );
#		    endif
		    }

		*lpBelow= lpHere;
		return next;
		}

	    /* If the slice does not even fit on a subsequent page: */
	    /* it does not make sense to try on that page */
	    if  ( lpHere.lpPage > lpTop->lpPage )
		{ break;	}

	    /* Slice does not fit on the page as a whole */
	    docLayoutToNextColumn( &lpHere, bf, parentNode, lj );
	    attempt++;
	    inNewFrame= 1;

#	    if DEBUG_PROGRESS
	    logProgress( "bbbb++++", lj, &lpHere, parentNode,
					    row, upto, startInThisColumn );
#	    endif
	    }
	}

    /*  Accept the inevitable: this row is not going to fit on one page */
    {
	int		row= from;
	int		found;

	BufferItem *	rowNode= parentNode->biChildren[row];

	const int	inNewFrame= 0;
	const int	doNotStartInThisColumn= 0;

	LayoutPosition	lpHere= *lpTop;

	docLayoutStartNodeLayout( rowNode, lj, &lpHere );

	if  ( docLayoutRowNode( &found, &lpHere, &lpHere, rowNode, bf,
					inNewFrame, doNotStartInThisColumn, lj ) )
	    { LDEB(row); return -1;	}

	if  ( found == FORMATstopFRAME_FULL )
	    { LDEB(found); return -1;	}

	*lpBelow= lpHere;
	return row+ 1;
    }
    }

/************************************************************************/
/*									*/
/*  Layout of a stack of contiguous table rows.				*/
/*									*/
/*  The caller keeps track of the bottom position of the slice: It must	*/
/*  set the bottom position of the last row.				*/
/*									*/
/************************************************************************/

int docLayoutTableSlice(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				struct BufferItem *	parentNode,
				int			from,
				int			upto,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int				row;

    LayoutPosition		lpHere= *lpTop;

    int				startInThisColumn= 1;

#   if DEBUG_PROGRESS
    logProgress( "SSSSSSSS", lj, lpTop, parentNode,
					from, upto, startInThisColumn );
#   endif

    row= from;
    while( row < upto )
	{
	int			next;

	next= docLayoutRowsInBlock( &lpHere, &lpHere, parentNode,
					startInThisColumn, row, upto, bf, lj );
	if  ( next < 0 || next == row )
	    { LLDEB(row,next); return -1;	}

#	if DEBUG_PROGRESS
	logProgress( "....ssss", lj, &lpHere, parentNode,
					row, upto, startInThisColumn );
#	endif

	row= next;
	if  ( row < upto )
	    {
	    const BufferItem *	rowNode= parentNode->biChildren[row];
	    startInThisColumn= docKeepRowWithNext( rowNode, row );
	    }
	}

    *lpBelow= lpHere;
    return 0;
    }

