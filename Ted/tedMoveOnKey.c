/************************************************************************/
/*									*/
/*  Ted: Handle user input from the keyboard, that results in navigation*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docSelectLayout.h>
#   include	<docTreeType.h>
#   include	<docParaParticules.h>
#   include	<docParaProperties.h>
#   include	<docNodeTree.h>
#   include	<docTreeNode.h>
#   include	<guiKeys.h>
#   include	<appEditDocument.h>
#   include	"tedKeyboard.h"
#   include	"tedSelect.h"
#   include	<docBuf.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/

static void tedMoveOnTabKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    DocumentPosition		dpNew;
    const int			lastLine= 0;

    if  ( docGotoLastPosition( &dpNew, ds->dsHead.dpNode->biParent )	||
	  docGotoNextPosition( &dpNew )					)
	{ return;	}

    tedInputSetSelectedPosition( ed, &dpNew, lastLine );

    return;
    }

static void tedMoveOnBackTabKey( struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    DocumentPosition		dpNew;
    const int			lastLine= 0;

    if  ( docGotoFirstPosition( &dpNew, ds->dsHead.dpNode->biParent )	||
	  docGotoPrevPosition( &dpNew )					)
	{ return;	}

    tedInputSetSelectedPosition( ed, &dpNew, lastLine );

    return;
    }

static void tedMoveOnHomeKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    DocumentPosition		dpNew;
    const int			lastLine= 1;

    if  ( ( state & KEY_CONTROL_MASK ) )
	{
	if  ( docGotoFirstPosition( &dpNew, bd->bdBody.dtRoot ) )
	    { return;	}
	}
    else{
	if  ( ( state & KEY_SHIFT_MASK ) && ds->dsDirection >= 0 )
	    {
	    dpNew= ds->dsTail;

	    if  ( docLineHead( &dpNew, sg->sgTail.pgPositionFlags ) )
		{ return;	}
	    }
	else{
	    dpNew= ds->dsHead;

	    if  ( docLineHead( &dpNew, sg->sgHead.pgPositionFlags ) )
		{ return;	}
	    }
	}

    docAvoidParaHeadField( &dpNew, (int *)0, bd );

    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );

    return;
    }

static void tedMoveOnEndKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    const int			lastLine= 0;
    DocumentPosition		dpNew;

    if  ( ( state & KEY_CONTROL_MASK ) )
	{
	if  ( docGotoLastPosition( &dpNew, bd->bdBody.dtRoot ) )
	    { return;	}
	}
    else{
	if  ( ! ( state & KEY_SHIFT_MASK ) || ds->dsDirection >= 0 )
	    {
	    dpNew= ds->dsTail;

	    if  ( docLineTail( &dpNew, sg->sgHead.pgPositionFlags ) )
		{ return;	}
	    }
	else{
	    dpNew= ds->dsHead;

	    if  ( docLineTail( &dpNew, sg->sgTail.pgPositionFlags ) )
		{ return;	}
	    }
	}

    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );

    return;
    }

static void tedMoveOnLeftKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    const int			lastLine= 1;
    DocumentPosition		dpNew;

    int				headAtLineHead;
    int				headAtLineTail;
    int				headAfterBreak;

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    headAtLineTail= ( sg->sgHead.pgPositionFlags & POSflagLINE_TAIL ) != 0;
    headAfterBreak= ( sg->sgHead.pgPositionFlags & POSflagPART_AFTER_BREAK ) != 0;
    if  ( ! state			&&
	  sd->sdIsIBarSelection		&&
	  ds->dsHead.dpStroff > 0	&&
	  headAtLineHead		&&
	  ! headAfterBreak		&&
	  ! headAtLineTail		)
	{
	const int	noLastLine= 0;

	/*  To same position on previous line */
	tedInputChangeSelection( ed, state, ds, &(ds->dsHead), noLastLine );
	return;
	}

    if  ( ( state & KEY_SHIFT_MASK ) && ds->dsDirection >= 0 )
	{ dpNew= ds->dsTail;	}
    else{ dpNew= ds->dsHead;	}

    if  ( state != 0		||
	  ! sd->sdIsListBullet	||
	  sd->sdIsIBarSelection	)
	{
	if  ( tedMoveLeftOnKey( &dpNew, bd, state ) )
	    { return;	}
	}

    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );

    return;
    }

static void tedMoveOnRightKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    const int			noLastLine= 0;
    DocumentPosition		dpNew;

    int				headAtLineHead;
    int				headAtLineTail;

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    headAtLineTail= ( sg->sgHead.pgPositionFlags & POSflagLINE_TAIL ) != 0;

    if  ( ! state						&&
	  sd->sdIsIBarSelection					&&
	  ds->dsHead.dpStroff < docParaStrlen( ds->dsHead.dpNode ) &&
	  headAtLineTail				&&
	  ! headAtLineHead				)
	{
	const int	lastLine= 1;

	/*  To same position on next line */
	tedInputChangeSelection( ed, state, ds, &(ds->dsHead), lastLine );
	return;
	}

    if  ( ! ( state & KEY_SHIFT_MASK ) || ds->dsDirection >= 0 )
	{ dpNew= ds->dsTail;	}
    else{ dpNew= ds->dsHead;	}

    if  ( state != 0 || sd->sdIsIBarSelection )
	{
	if  ( tedMoveRightOnKey( &dpNew, bd, state ) )
	    { return;	}
	}

    tedInputChangeSelection( ed, state, ds, &dpNew, noLastLine );

    return;
    }

static void tedMoveOnUpKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    DocumentPosition		dpNew;

    const PositionGeometry *	pgRef;
    int				headAtLineHead;
    int				lastLine;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    lastLine= headAtLineHead;

    if  ( ! ( state & KEY_SHIFT_MASK ) || ds->dsDirection >= 0 )
	{ pgRef= &(sg->sgTail); dpNew= ds->dsTail;	}
    else{ pgRef= &(sg->sgHead); dpNew= ds->dsHead;	}

    if  ( state & KEY_CONTROL_MASK )
	{
	if  ( dpNew.dpStroff == 0 )
	    {
	    dpNew.dpNode= docPrevParagraph( dpNew.dpNode );
	    if  ( ! dpNew.dpNode )
		{ return;	}

	    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
		{ return;	}
	    }
	else{
	    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
		{ return;	}
	    }
	}
    else{
	if  ( tedArrowUp( &dpNew, pgRef, &lc ) )
	    { return;	}
	}

    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );

    return;
    }

static void tedMoveOnDownKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    DocumentPosition		dpNew;

    const PositionGeometry *	pgRef;
    int				headAtLineHead;
    int				lastLine;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    lastLine= headAtLineHead;

    if  ( ! ( state & KEY_SHIFT_MASK ) || ds->dsDirection >= 0 )
	{ pgRef= &(sg->sgTail); dpNew= ds->dsTail;	}
    else{ pgRef= &(sg->sgHead); dpNew= ds->dsHead;	}

    if  ( state & KEY_CONTROL_MASK )
	{
	if  ( dpNew.dpStroff < docParaStrlen( dpNew.dpNode )	&&
	      ( state & KEY_SHIFT_MASK )			)
	    {
	    if  ( docGotoLastPosition( &dpNew, dpNew.dpNode ) )
		{ return;	}
	    }
	else{
	    dpNew.dpNode= docNextParagraph( dpNew.dpNode );
	    if  ( ! dpNew.dpNode )
		{ return;	}

	    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
		{ return;	}
	    }
	}
    else{
	if  ( tedArrowDown( &dpNew, pgRef, &lc ) )
	    { return;	}
	}

    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );
    return;
    }

static void tedMoveOnPageUpKey(	struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentPosition		dpNew;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( ( state & KEY_SHIFT_MASK ) && ds->dsDirection >= 0 )
	{ dpNew= ds->dsTail;	}
    else{ dpNew= ds->dsHead;	}

    if  ( dpNew.dpNode->biTreeType != DOCinBODY )
	{
	struct DocumentTree *	dtPrev;

	if  ( ! docPrevSimilarTree( &dpNew, &dtPrev, bd ) )
	    {
	    const int		lastLine= 0;

# if 0
	    if  ( dtPrev->dtPageSelectedUpon < 0 && page >= 0 )
		{
		dtPrev->dtPageSelectedUpon= page;
		dtPrev->dtColumnSelectedIn= column;
		}
# endif

	    docAvoidParaHeadField( &dpNew, (int *)0, bd );

	    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
	    }

	return;
	}

    if  ( docGotoPrevPosition( &dpNew )				&&
	  docGotoFirstPosition( &dpNew, bd->bdBody.dtRoot )	)
	{ return;	}

    {
	PositionGeometry	pg;
	const int		lastLine= 1;
	int			partNew;
	int			lineNew;

	int			page;

	docPositionGeometry( &pg, &dpNew, PARAfindFIRST, &lc );

	page= pg.pgTopPosition.lpPage;
	while( page >= 0 )
	    {
	    int		res;

	    res= docGetTopOfColumn( &dpNew, &lineNew, &partNew, bd, page, 0 );
	    if  ( res < 0 )
		{ LDEB(res); return;	}
	    if  ( res == 0 )
		{
		tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );
		return;
		}

	    page--;
	    }
    }

    return;
    }

static void tedMoveOnPageDownKey( struct EditDocument *		ed,
				int				keySym,
				int				state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentPosition		dpNew;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( ! ( state & KEY_SHIFT_MASK ) || ds->dsDirection >= 0 )
	{ dpNew= ds->dsTail;	}
    else{ dpNew= ds->dsHead;	}

    if  ( dpNew.dpNode->biTreeType != DOCinBODY )
	{
	struct DocumentTree *	dtNext;

	if  ( ! docNextSimilarTree( &dpNew, &dtNext, bd ) )
	    {
	    const int		lastLine= 0;

# if 0
	    if  ( dtNext->dtPageSelectedUpon < 0 && page >= 0 )
		{
		dtNext->dtPageSelectedUpon= page;
		dtNext->dtColumnSelectedIn= column;
		}
# endif

	    docAvoidParaHeadField( &dpNew, (int *)0, bd );

	    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
	    }

	return;
	}

    {
	PositionGeometry	pg;
	const int		lastLine= 1;
	int			partNew;
	int			lineNew;
	int			res;

	int			page;
	struct BufferItem *	bodyNode= bd->bdBody.dtRoot;

	docPositionGeometry( &pg, &dpNew, PARAfindLAST, &lc );

	page= pg.pgTopPosition.lpPage+ 1;
	while( page <= bodyNode->biBelowPosition.lpPage )
	    {
	    int		topRes;

	    topRes= docGetTopOfColumn( &dpNew, &lineNew, &partNew,
							bd, page, 0 );
	    if  ( topRes < 0 )
		{ LDEB(topRes); return;	}
	    if  ( topRes == 0 )
		{
		tedInputChangeSelection( ed, state, ds,
						    &dpNew, lastLine );
		break;
		}

	    page++;
	    }

	page= bodyNode->biBelowPosition.lpPage;
	res= docGetBottomOfColumn( &dpNew, &partNew, bd, page, 0 );
	if  ( res == 0 )
	    {
	    tedInputChangeSelection( ed, state, ds, &dpNew, lastLine );
	    return;
	    }
    }

    return;
    }

static void tedMoveOnInsertKey( struct EditDocument *		ed,
				int				x_keySym,
				int				x_state,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				headAtLineHead;

    headAtLineHead= ( sg->sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;

    td->tdOverstrike= ! td->tdOverstrike;

    if  ( docIsIBarSelection( ds ) )
	{
	tedSetSelectionLow( ed, ds, headAtLineHead, (int *)0, (int *)0 );
	}

    return;
    }

TedKeyHandler tedKeyGetMoveHandler(
			struct EditDocument *			ed,
			int					keySym,
			int					state,
			const struct DocumentSelection *	ds,
			const struct SelectionDescription *	sd )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    switch( keySym )
	{
#	ifdef KEY_ISO_Left_Tab
	case  KEY_ISO_Left_Tab:
	    if  ( docSelectionIsSet( ds )		&&
		  sd->sdHeadInTable			&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{ return tedMoveOnBackTabKey;	}
	    else{ return (TedKeyHandler)0;	}
#	endif
	case KEY_i:
	    if  ( state != KEY_CONTROL_MASK )
		{ return (TedKeyHandler)0; }
	    /*FALLTHROUGH*/
	case KEY_Tab:
	    if  ( docSelectionIsSet( ds )		&&
		  sd->sdHeadInTable			&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{
		if  ( state & KEY_SHIFT_MASK )
		    { return tedMoveOnBackTabKey;	}
		else{ return tedMoveOnTabKey;		}
		}
	    else{ return (TedKeyHandler)0;	}
	    break;

	case KEY_KP_Home:
	case KEY_Home:
	    return tedMoveOnHomeKey;

	case KEY_KP_End:
	case KEY_End:
	    return tedMoveOnEndKey;

	case KEY_KP_Left:
	case KEY_Left:
	    return tedMoveOnLeftKey;

	case KEY_KP_Right:
	case KEY_Right:
	    return tedMoveOnRightKey;

	case KEY_KP_Up:
	case KEY_Up:
	    return tedMoveOnUpKey;

	case KEY_KP_Down:
	case KEY_Down:
	    return tedMoveOnDownKey;

	case KEY_KP_Prior:
	case KEY_Prior:
	    return tedMoveOnPageUpKey;

	case KEY_KP_Next:
	case KEY_Next:
	    return tedMoveOnPageDownKey;

	case KEY_KP_Delete:
	case KEY_Delete:
	    if  ( sd->sdInOneTable && sd->sdTableRectangle.trIsTableSlice )
		{ return (TedKeyHandler)0;	}
	    else{
		if  ( sd->sdIsIBarSelection )
		    {
		    DocumentPosition	dpNew= ds->dsTail;

		    if  ( tedMoveRightOnKey( &dpNew, bd, state ) )
			{ return (TedKeyHandler)0;	}

		    if  ( ( sd->sdHeadInTable 				||
			    dpNew.dpNode->biParaProperties->ppTableNesting > 0	) &&
			  ! docPositionsInsideCell( &(ds->dsHead), &dpNew ) )
			{ return tedMoveOnRightKey;	}
		    else{ return (TedKeyHandler)0;	}
		    } 
		} 
	    return (TedKeyHandler)0;

	case KEY_BackSpace:
	    if  ( sd->sdInOneTable && sd->sdTableRectangle.trIsTableSlice )
		{ return (TedKeyHandler)0;	}
	    else{
		if  ( sd->sdIsIBarSelection )
		    {
		    DocumentPosition	dpNew= ds->dsHead;

		    if  ( tedMoveLeftOnKey( &dpNew, bd, state ) )
			{ return (TedKeyHandler)0;	}

		    if  ( ( dpNew.dpNode->biParaProperties->ppTableNesting > 0	||
			    sd->sdTailInTable ) 			&&
			  ! docPositionsInsideCell( &dpNew, &(ds->dsTail) ) )
			{ return tedMoveOnLeftKey;	}
		    else{ return (TedKeyHandler)0;	}
		    }
		} 
	    return (TedKeyHandler)0;

	case KEY_Insert:
	case KEY_KP_Insert:
	    return tedMoveOnInsertKey;
	}

   return (TedKeyHandler)0;
   }

