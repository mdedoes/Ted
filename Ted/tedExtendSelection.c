/************************************************************************/
/*									*/
/*  Ted: Extend the selection.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docParaParticules.h>
#   include	<docTreeNode.h>
#   include	<docParaProperties.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	<docLayout.h>
#   include	<docTreeNode.h>
#   include	<docFields.h>
#   include	<appDocFront.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scroll the slection into view.					*/
/*									*/
/************************************************************************/

static void tedScrollToPositions(EditDocument *			ed,
				const PositionGeometry *	pgB,
				const PositionGeometry *	pgE,
				const struct LayoutContext *	lc,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    DocumentRectangle		drPixels;

    docPixelRectangleForPositions( &drPixels, pgB, pgE, lc );

    appScrollToRectangle( pScrolledX, pScrolledY, ed, &drPixels );

    return;
    }

/************************************************************************/
/*									*/
/*  Extend the selection upon subsequent MotionNotify events.		*/
/*									*/
/*  1)  Do not update the tools and the ruler: The mouse up handler	*/
/*	will do so.							*/
/*									*/
/************************************************************************/

static void tedSetExtendedSelection(
				EditDocument *			ed,
				int				direction,
				const struct BufferItem *	bodySectNode,
				const DocumentPosition *	dpTo,
				const DocumentPosition *	dpBegin,
				const DocumentPosition *	dpEnd )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    PositionGeometry		pgTo;

    int				scrolledX= 0;
    int				scrolledY= 0;
    const int			lastLine= 0;

    LayoutContext		lc;
    int				lastOne= (direction>0)?PARAfindLAST:PARAfindFIRST;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    docPositionGeometry( &pgTo, dpTo, lastOne, &lc );
    tedScrollToPositions( ed, &pgTo, &pgTo, &lc, &scrolledX, &scrolledY );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
					(struct DocumentTree **)0,
					(struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    appDocExposeRectangle( ed, &(sgDoc.sgRectangle), scrolledX, scrolledY );

    docSetRangeSelection( &(td->tdSelection), dpBegin, dpEnd, direction );
    docSelectionGeometry( &(td->tdSelectionGeometry),
				    &(td->tdSelection), bodySectNode,
				    lastLine, &lc );
    /*  1  */
    tedDescribeSelection( ed );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
					(struct DocumentTree **)0,
					(struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    appDocExposeRectangle( ed, &(sgDoc.sgRectangle), scrolledX, scrolledY );

    tedManagePrimarySelection( ed );

    return;
    }

static void tedBalanceFieldSelection(	struct BufferDocument *	bd,
					struct DocumentTree *	tree,
					int *			pBalanced,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd )
    {
    int				beginMoved= 0;
    int				endMoved= 0;

    docBalanceFieldSelection( (struct DocumentField **)0,
				(struct DocumentField **)0,
				&beginMoved, &endMoved,
				(int *)0, (int *)0,
				dpBegin, dpEnd, tree, bd );

    if  ( beginMoved )
	{ *pBalanced= 1;	}
    if  ( endMoved )
	{ *pBalanced= 1;	}

    return;
    }

int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				col0= -1;
    int				col1= -1;

    DocumentPosition		dpFrom;
    DocumentPosition		dpTo;

    int				directionToAnchor;
    int				directionBeginAnchor;
    int				directionEndAnchor;
    int				dir;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				cellChanged= 0;
    int				balanced= 0;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    struct DocumentTree *		tree;
    struct BufferItem *		bodySectNode;

    dpFrom= *dpAnchor;
    dpTo= *dpFound;

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc, &tree, &bodySectNode, ed ) )
	{ LDEB(1); return -1;	}

    directionToAnchor= docComparePositions( &dpTo, dpAnchor );
    directionBeginAnchor= docComparePositions( &(dsDoc.dsHead), dpAnchor );
    directionEndAnchor= docComparePositions( &(dsDoc.dsTail), dpAnchor );

    /********************/
    /*  Before anchor.	*/
    /********************/
    if  ( directionToAnchor < 0 )
	{
	int		directionToBegin;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpNode->biParaProperties->ppTableNesting > 0 )
		{ col0= col1= dpFrom.dpNode->biParent->biNumberInParent; }
	    }
	else{
	    if  ( dpTo.dpNode->biParaProperties->ppTableNesting > 0 )
		{
		if  ( docExtendSelectionFromTable( &dpTo, &dpFrom,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpFrom.dpNode->biParaProperties->ppTableNesting > 0 )
		    {
		    if  ( docExtendSelectionIntoTable( &dpFrom ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( td->tdDocument, tree,
					    &balanced, &dpTo, &dpFrom );

	directionToBegin= docComparePositions( &dpTo, &(dsDoc.dsHead) );
	cellChanged= ! docPositionsInsideCell( &dpTo, &(dsDoc.dsHead) );

	/****************************************/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionEndAnchor > 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Left.	*/
	/************************/
	if  ( directionToBegin < 0 )
	    {
	    dir= -1;
	    tedSetExtendedSelection( ed, dir, bodySectNode,
						&dpTo, &dpTo, &dpFrom );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	/************************/
	/*  Shrunk Left.	*/
	/************************/
	if  ( directionToBegin > 0 )
	    {
	    dir= -1;
	    tedSetExtendedSelection( ed, dir, bodySectNode,
						&dpTo, &dpTo, &dpFrom );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  After anchor.	*/
    /********************/
    if  ( directionToAnchor > 0 )
	{
	int		directionToEnd;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpNode->biParaProperties->ppTableNesting > 0 )
		{ col0= col1= dpFrom.dpNode->biParent->biNumberInParent; }
	    }
	else{
	    if  ( dpFrom.dpNode->biParaProperties->ppTableNesting > 0 )
		{
		if  ( docExtendSelectionFromTable( &dpFrom, &dpTo,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpTo.dpNode->biParaProperties->ppTableNesting > 0 )
		    {
		    if  ( docExtendSelectionIntoTable( &dpTo ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( td->tdDocument, tree,
					    &balanced, &dpFrom, &dpTo );

	directionToEnd= docComparePositions( &dpTo, &(dsDoc.dsTail) );
	cellChanged= ! docPositionsInsideCell( &dpTo, &(dsDoc.dsTail) );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Right.	*/
	/************************/
	if  ( directionToEnd > 0 )
	    {
	    dir= 1;
	    tedSetExtendedSelection( ed, dir, bodySectNode,
						    &dpTo, &dpFrom, &dpTo );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	/************************/
	/*  Shrunk Right.	*/
	/************************/
	if  ( directionToEnd < 0 )
	    {
	    dir= 1;
	    tedSetExtendedSelection( ed, dir, bodySectNode,
						    &dpTo, &dpFrom, &dpTo );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  At anchor.	*/
    /********************/
	{
	dpFrom= *dpAnchor;
	dpTo= *dpAnchor;

	tedBalanceFieldSelection( td->tdDocument, tree, &balanced, &dpFrom, &dpTo );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      directionEndAnchor > 0	)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	dir= 0;
	tedSetExtendedSelection( ed, dir, bodySectNode,
						&dpFrom, &dpFrom, &dpTo );

	td->tdVisibleSelectionCopied= 0;
	}

    return 0;
    }

