/************************************************************************/
/*									*/
/*  Ted: Management of the selection and the current position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<tedDocFront.h>
#   include	"tedDraw.h"
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docDescribeSetSelection.h>
#   include	<docTreeNode.h>
#   include	<docParaProperties.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	<docLayout.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docBuf.h>
#   include	<docFields.h>
#   include	<appDocFront.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Is the current selection exactly an object?				*/
/*									*/
/************************************************************************/

int tedGetObjectSelection(	const struct PositionGeometry ** pPgObj,
				int *				pPartObj,
				DocumentPosition *		dpObj,
				struct InsertedObject **	pIo,
				const EditDocument *		ed )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    const struct BufferDocument * bd= td->tdDocument;
    int				rval;

    rval= docGetObjectSelection( pPartObj, dpObj, pIo,
						    bd, &(td->tdSelection) );
    if  ( rval )
	{ return rval;	}

    if  ( pPgObj )
	{ *pPgObj= &(td->tdSelectionGeometry.sgHead);	}

    return 0;
    }

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
/*  Scroll a selection into view.					*/
/*									*/
/************************************************************************/

void tedScrollToSelection(	EditDocument *		ed,
				int *			pScrolledX,
				int *			pScrolledY )
    {
    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    tedScrollToPositions( ed, &(sgDoc.sgHead), &(sgDoc.sgTail),
					    &lc, pScrolledX, pScrolledY );

    return;
    }

static void tedBalanceFieldSelection(	struct BufferDocument *	bd,
					struct DocumentTree *	tree,
					int *			pBalanced,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd )
    {
    struct DocumentField *	dfLeft;
    struct DocumentField *	dfRight;
    int				beginMoved= 0;
    int				endMoved= 0;
    int				headPart= -1;
    int				tailPart= -1;

    docBalanceFieldSelection( &dfLeft, &dfRight, &beginMoved, &endMoved,
						    &headPart, &tailPart,
						    dpBegin, dpEnd, tree, bd );

    if  ( beginMoved )
	{ *pBalanced= 1;	}
    if  ( endMoved )
	{ *pBalanced= 1;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

static int tedArrowFindPositionInLine(	DocumentPosition *	dp,
					const LayoutContext *	lc,
					int			line,
					int			docXPixels )
    {
    TextLine *		tl= dp->dpNode->biParaLines+ line;
    PositionGeometry	pg;

    docInitPositionGeometry( &pg );

    if  ( docFindPositionInLine( dp, &pg, lc, dp->dpNode, tl, docXPixels ) )
	{ LDEB(docXPixels); return -1;	}

    return 0;
    }

int tedArrowUp(		DocumentPosition *		dp,
			const PositionGeometry *	pg,
			const LayoutContext *		lc )
    {
    int			line= 0;

    if  ( docLineUp( &line, dp, pg->pgPositionFlags ) )
	{ return -1;	}

    if  ( tedArrowFindPositionInLine( dp, lc, line, pg->pgXPixels ) )
	{ LDEB(pg->pgXPixels); return -1;	}

    return 0;
    }

int tedArrowDown(	DocumentPosition *		dp,
			const PositionGeometry *	pg,
			const LayoutContext *		lc )
    {
    int			line= 0;

    if  ( docLineDown( &line, dp, pg->pgPositionFlags ) )
	{ return -1;	}

    if  ( tedArrowFindPositionInLine( dp, lc, line, pg->pgXPixels ) )
	{ LDEB(pg->pgXPixels); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  A selection was made, do bookkeeping and provoke drawing.		*/
/*									*/
/*  0)  If an object (Picture) was selected, remove the special window.	*/
/*  1)  Provoke the old selection to be redrawn. (Before scrolling)	*/
/*  2)  Stop Cursor blinking.						*/
/*  3)  Scroll to the new selection.					*/
/*  4)  Provoke the old selection to be redrawn. (It might have moved)	*/
/*  5)  Bookkeeping.							*/
/*  6)  Provoke the new selection to be redrawn.			*/
/*									*/
/*  NOTE:	As scrolling to the new selection can provoke a redraw	*/
/*		the selection needs to be set before the scrollbar is	*/
/*		adapted. Otherwise the selection administration can	*/
/*		still refer to deleted text.				*/
/*									*/
/*  A)  If the selection is partially inside and partially outside a	*/
/*	text level field, force it to contain the whole field. This is	*/
/*	to enforce proper text field nesting.				*/
/*									*/
/************************************************************************/

void tedSetSelectionLow(	EditDocument *			ed,
				const DocumentSelection *	dsSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    int				hadSelection;

    DocumentSelection		dsOld;
    SelectionGeometry		sgOld;
    SelectionDescription	sdOld;

    DocumentSelection		dsNew;
    SelectionGeometry		sgNew;

    int				balanced= 0;

    DocumentRectangle		drExpose;
    struct DocumentTree *		treeSet= (struct DocumentTree *)0;
    struct BufferItem *		bodySectNodeSet= (struct BufferItem *)0;

    DocumentRectangle		drExternalSet;
    int				bodySectNr= -1;

    int				redrawTreeOld= 0;
    int				redrawTreeSet= 0;

    int				scrolledX= 0;
    int				scrolledY= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    td->tdEditTrace.etTyping= TYPING_NO;

    if  ( tedGetSelection( &dsOld, &sgOld, &sdOld,
				(struct DocumentTree **)0,
				(struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  0  */
    if  ( sdOld.sdIsObjectSelection )
	{ tedHideObjectWindows( ed );	}

    bodySectNr= docDescribeSetSelection( &treeSet, &bodySectNodeSet,
			    &drExternalSet,
			    &redrawTreeOld, &redrawTreeSet,
			    &lc, bd, &dsOld, dsSet );
    if  ( bodySectNr < 0 )
	{ LDEB(bodySectNr); return;	}

    /*  A  */
    dsNew= *dsSet;

    if  ( td->tdOverstrike			&&
	  docIsIBarSelection( &dsNew )		)
	{
	DocumentPosition	dp= dsNew.dsHead;

	if  ( ! docGotoNextPosition( &dp )	&&
	      dp.dpNode == dsNew.dsHead.dpNode	)
	    {
	    const int	direction= 1;

	    docSetRangeSelection( &dsNew, &(dsSet->dsHead), &dp, direction );
	    }
	}

    tedBalanceFieldSelection( td->tdDocument, treeSet, &balanced,
					    &(dsNew.dsHead), &(dsNew.dsTail) );

    hadSelection= tedHasSelection( ed );
    if  ( hadSelection )
	{
	/*  1  */
	drExpose= sgOld.sgRectangle;

	if  ( redrawTreeOld )
	    { geoUnionRectangle( &drExpose, &drExpose, &(ed->edFullRect) ); }

	/*  2  */
	if  ( sdOld.sdIsIBarSelection )
	    { tedStopCursorBlink( ed ); }

	appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );
	}

    /*  5  */
    if  ( dsNew.dsHead.dpNode->biParaProperties->ppTableNesting > 0	&&
	  dsNew.dsTail.dpNode->biParaProperties->ppTableNesting > 0	&&
	  dsNew.dsHead.dpNode->biParent->biParent ==
			dsNew.dsTail.dpNode->biParent->biParent	)
	{
	dsNew.dsCol0= dsNew.dsHead.dpNode->biParent->biNumberInParent;
	dsNew.dsCol1= dsNew.dsTail.dpNode->biParent->biNumberInParent;
	}

    /*  5  */
    if  ( ! bodySectNodeSet )
	{ XDEB(bodySectNodeSet);	}

    docSelectionGeometry( &sgNew, &dsNew, bodySectNodeSet, lastLine, &lc );
    td->tdVisibleSelectionCopied= 0;
    td->tdSelection= dsNew;
    td->tdSelectionGeometry= sgNew;

    treeSet->dtPageSelectedUpon= dsNew.dsHead.dpNode->biTopPosition.lpPage;
    treeSet->dtColumnSelectedIn= dsNew.dsHead.dpNode->biTopPosition.lpColumn;

    /*  3  */
    tedScrollToSelection( ed, &scrolledX, &scrolledY );

    if  ( hadSelection )
	{
	/*  4  */
	drExpose= sgOld.sgRectangle;

	if  ( redrawTreeOld )
	    { geoUnionRectangle( &drExpose, &drExpose, &(ed->edFullRect) ); }

	appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );
	}

    /*  6  */
    drExpose= sgNew.sgRectangle;

    if  ( redrawTreeSet )
	{ geoUnionRectangle( &drExpose, &drExpose, &drExternalSet ); }

    appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );

    tedDescribeSelection( ed );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    td->tdBodySectionNumber= bodySectNr;

    tedManagePrimarySelection( ed );

    if  ( pScrolledX )
	{ *pScrolledX= scrolledX;	}
    if  ( pScrolledY )
	{ *pScrolledY= scrolledY;	}

    return;
    }

void tedSetSelection(	EditDocument *			ed,
			const DocumentSelection *	dsSet,
			int				lastLine,
			int *				pScrolledX,
			int *				pScrolledY )
    {
    tedSetSelectionLow( ed, dsSet, lastLine, pScrolledX, pScrolledY );

    /* in tedSetSelectionLow(): tedDescribeSelection() */
    tedAdaptToolsToSelection( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Collect a description of the current selection.			*/
/*									*/
/*  Remember the text attribute of the beginning of the selection.	*/
/*									*/
/************************************************************************/

void tedDescribeSelection(	EditDocument *			ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    const IndexMapping *	a2s= &(td->tdAttributeToScreenFont);
    SelectionDescription *	sd= &(td->tdSelectionDescription);

    docDescribeSelection( sd, &(td->tdSelection), bd,
			td->tdSelectionGeometry.sgHead.pgTopPosition.lpPage,
			td->tdSelectionGeometry.sgTail.pgTopPosition.lpPage,
			ed->edDocumentId, ed->edIsReadonly );

    td->tdCurrentScreenFont= utilIndexMappingGet( a2s,
					    sd->sdTextAttributeNumber );

    if  ( td->tdCurrentScreenFont < 0 )
	{ LLDEB(sd->sdTextAttributeNumber,td->tdCurrentScreenFont); }

    {
    int				fontSize= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( docLayoutParagraphLineExtents( &fontSize, &lc,
					    td->tdSelection.dsHead.dpNode ) )
	{ LDEB(1); return;	}

    sd->sdMajorityFontSize= fontSize;
    }

    }

void tedSetSelectedPosition(	EditDocument *			ed,
				const DocumentPosition *	dpSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    DocumentSelection		dsNew;

    docInitDocumentSelection( &dsNew );

    docSetIBarSelection( &dsNew, dpSet );

    tedSetSelection( ed, &dsNew, lastLine, pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Set an I Bar selection and cause the I Bar to be drawn.		*/
/*									*/
/************************************************************************/

int tedSetIBarSelection(		EditDocument *		ed,
					const DocumentPosition * dp,
					int			lastLine,
					int *			pScrolledX,
					int *			pScrolledY )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    td->tdVisibleSelectionCopied= 0;

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedHideObjectWindows( ed );	}

    tedSetSelectedPosition( ed, dp, lastLine, pScrolledX, pScrolledY );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole paragraphs.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedDocSelectWholeParagraph(	EditDocument *		ed,
				int			direction )
    {
    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    const int			lastLine= 0;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedGetSelection( &dsNew, &sg, &sd,
				(struct DocumentTree **)0,
				(struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    ret= docSelectWholeParagraph( &dsNew, direction );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedDocSelectWholeSection(	EditDocument *		ed,
				int			direction )
    {
    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    int				scrolledX= 0;
    int				scrolledY= 0;
    const int			lastLine= 0;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedGetSelection( &dsNew, &sg, &sd,
				(struct DocumentTree **)0,
				(struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    ret= docSelectWholeSection( &dsNew, direction );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

    return 0;
    }

