/************************************************************************/
/*									*/
/*  Ted: Handle user input from the keyboard thar results in changes to	*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<tedDocFront.h>
#   include	"tedEdit.h"
#   include	<docParaParticules.h>
#   include	<docEditCommand.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	<guiKeys.h>
#   include	<appEditDocument.h>
#   include	"tedKeyboard.h"
#   include	<docBreakKind.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Delete a selection as the result of a BackSpace or Delete key with	*/
/*  an IBar selection.							*/
/*									*/
/************************************************************************/

static int tedInputSetIBarDelete( 
				struct EditDocument *		ed,
				int				headAtLineHead,
				const DocumentPosition *	dpHead,
				const DocumentPosition *	dpTail )
    {
    DocumentSelection	ds;

    docSetRangeSelection( &ds, dpHead, dpTail, 1 );

    if  ( ! docSelectionSingleParagraph( &ds ) )
	{
	docTailPosition( &(ds.dsHead), ds.dsHead.dpNode );
	docHeadPosition( &(ds.dsTail), ds.dsTail.dpNode );
	}

    tedSetSelectionLow( ed, &ds, headAtLineHead, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/

static void tedEditOnTabKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    const int			redoLayout= 0;

    tedEditInsertSpecialParticule( ed,
			TPkindTAB, EDITcmdREPLACE, redoLayout, td->tdTraced );

    return;
    }

static void tedEditOnEnterKeySamePage(
				struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocSplitParagraph( ed, DOCibkNONE,
				EDITcmdREPLACE, td->tdTraced );

    return;
    }

static void tedEditOnEnterKeyNewPage(
				struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocSplitParagraph( ed, DOCibkCOL,
				EDITcmdREPLACE_BODY_LEVEL, td->tdTraced );

    return;
    }

static void tedEditOnDeleteKey(	struct EditDocument *		ed,
				int				x_keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentPosition		dpNew;
    int				headAtLineHead;
    int				deleteCommand= EDITcmdDELETE_SELECTION;

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;

    if  ( sd->sdIsIBarSelection )
	{
	dpNew= ds->dsTail;

	if  ( tedMoveRightOnKey( &dpNew, bd, state ) )
	    { return;	}

	if  ( ( sd->sdHeadInTable 				||
		dpNew.dpNode->biParaProperties->ppTableNesting > 0	) &&
	      ! docPositionsInsideCell( &(ds->dsHead), &dpNew ) )
	    { LDEB(1); return;	}

	tedInputSetIBarDelete( ed, headAtLineHead, &(ds->dsHead), &dpNew );
	deleteCommand= EDITcmdDELETE_SELECTION_DEL;
	}

    tedDocDeleteSelection( ed, deleteCommand, td->tdTraced );

    return;
    }

static void tedEditOnBackSpaceKey( struct EditDocument *	ed,
				int				x_keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentPosition		dpNew;
    int				headAtLineHead;
    int				deleteCommand= EDITcmdDELETE_SELECTION;

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;

    if  ( sd->sdIsIBarSelection )
	{
	dpNew= ds->dsHead;

	if  ( tedMoveLeftOnKey( &dpNew, bd, state ) )
	    { return;	}

	if  ( ( dpNew.dpNode->biParaProperties->ppTableNesting > 0	||
		sd->sdTailInTable ) 				&&
	      ! docPositionsInsideCell( &dpNew, &(ds->dsTail) )	)
	    { LDEB(1); return;	}

	tedInputSetIBarDelete( ed, headAtLineHead, &dpNew, &(ds->dsTail) );
	deleteCommand= EDITcmdDELETE_SELECTION_BS;
	}

    tedDocDeleteSelection( ed, deleteCommand, td->tdTraced );

    return;
    }

static void tedDeleteTableOnKey(
				struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    int				res;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    res= tedDocDeleteTable( ed, td->tdTraced );
    if  ( res )
	{ LDEB(res); return;	}

    return;
    }

static void tedDeleteRowSliceOnKey(
				struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    int				res;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    res= tedDocDeleteRows( ed, td->tdTraced );
    if  ( res )
	{ LDEB(res); return;	}

    return;
    }

static void tedDeleteColumnSliceOnKey(
				struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    int				res;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    res= tedDocDeleteColumns( ed, td->tdTraced );
    if  ( res )
	{ LDEB(res); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  2)  If it is a row slice (Whole rows) delete the rows. This covers	*/
/*	deleting the whole table as well. [ Handling it in the column	*/
/*	branch would leave empty rows.]					*/
/*  3)  If it is a column slice (Whole columns) delete the columns.	*/
/*  4)  Impossible!							*/
/*									*/
/************************************************************************/

static TedKeyHandler tedKeyGetTableSliceHandler(
			int *					pEditCommand,
			const struct SelectionDescription *	sd )
    {
    const TableRectangle *	tr= &(sd->sdTableRectangle);

    /*  2 */
    if  ( tr->trIsWholeTable )
	{
	*pEditCommand= EDITcmdDELETE_TABLE;
	return tedDeleteTableOnKey;
	}

    /*  2 */
    if  ( tr->trIsRowSlice )
	{
	*pEditCommand= EDITcmdDELETE_ROW;
	return tedDeleteRowSliceOnKey;
	}

    /*  3 */
    if  ( tr->trIsColumnSlice )
	{
	*pEditCommand= EDITcmdDELETE_COLUMN;
	return tedDeleteColumnSliceOnKey;
	}

    /*  4 */
    LLDEB(tr->trIsRowSlice,tr->trIsColumnSlice);
    return (TedKeyHandler)0;
    }

TedKeyHandler tedKeyGetEditHandler(
			int *					pEditCommand,
			struct EditDocument *			ed,
			int					keySym,
			int					state,
			const struct DocumentSelection *	ds,
			const struct SelectionDescription *	sd )
    {
    switch( keySym )
	{
	case KEY_i:
	    if  ( state != KEY_CONTROL_MASK )
		{ return (TedKeyHandler)0; }
	    /*FALLTHROUGH*/
	case KEY_Tab:
	    if  ( docSelectionIsSet( ds )		&&
		  sd->sdHeadInTable			&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{ return (TedKeyHandler)0;	}

	    if  ( state & KEY_SHIFT_MASK )
		{ return (TedKeyHandler)0;	}

	    *pEditCommand= EDITcmdREPLACE;
	    return tedEditOnTabKey;

	case KEY_j: case KEY_m:
	    if  ( state != KEY_CONTROL_MASK )
		{ return (TedKeyHandler)0;	}
	    /*FALLTHROUGH*/
	case KEY_KP_Enter:
	case KEY_Return:
	    {
	    const int	onNewPage= ( state == KEY_CONTROL_MASK  &&
				     keySym != KEY_j		&&
				     keySym != KEY_m		);

	    if  ( onNewPage )
		{
		*pEditCommand= EDITcmdREPLACE_BODY_LEVEL;
		return tedEditOnEnterKeyNewPage;
		}
	    else{
		*pEditCommand= EDITcmdREPLACE;
		return tedEditOnEnterKeySamePage;
		}
	    }

	case KEY_KP_Delete:
	case KEY_Delete:
	    if  ( sd->sdInOneTable			&&
		  sd->sdTableRectangle.trIsTableSlice	&&
		  ! sd->sdTableRectangle.trIsSingleCell )
		{ return tedKeyGetTableSliceHandler( pEditCommand, sd ); }
	    else{
		if  ( sd->sdIsIBarSelection )
		    { *pEditCommand= EDITcmdDELETE_SELECTION_DEL;	}
		else{ *pEditCommand= EDITcmdDELETE_SELECTION;		}
		return tedEditOnDeleteKey;
		}

	case KEY_BackSpace:
	    if  ( sd->sdInOneTable			&&
		  sd->sdTableRectangle.trIsTableSlice	&&
		  ! sd->sdTableRectangle.trIsSingleCell )
		{ return tedKeyGetTableSliceHandler( pEditCommand, sd ); }
	    else{
		if  ( sd->sdIsIBarSelection )
		    { *pEditCommand= EDITcmdDELETE_SELECTION_BS;	}
		else{ *pEditCommand= EDITcmdDELETE_SELECTION;		}
		return tedEditOnBackSpaceKey;
		} 
	}

    return (TedKeyHandler)0;
    }

