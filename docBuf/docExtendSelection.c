/************************************************************************/
/*									*/
/*  Ted: Extend the selection.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	"docTreeNode.h"
#   include	<docParaProperties.h>
#   include	"docTreeNode.h"
#   include	"docSelect.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Extend a selection that begins outside a table cell and ends inside	*/
/*  that cell.								*/
/*									*/
/*  The speccial cases that occur when the selection begins in a table	*/
/*  are covered by tedExtendSelectionFromTable(), that has priority	*/
/*  over tedExtendSelectionIntoTable().					*/
/*									*/
/************************************************************************/

int docExtendSelectionIntoTable(	DocumentPosition *	dpEnd )
    {
    struct BufferItem *	cellNode= dpEnd->dpNode->biParent;
    struct BufferItem *	rowNode= cellNode->biParent;

    int			col1= rowNode->biChildCount -1;

    docTailPosition( dpEnd, rowNode->biChildren[col1] );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extend a selection that begins inside a table cell and ends outside	*/
/*  that cell.								*/
/*									*/
/*  1)  Not in same section.						*/
/*  2)  Look for selection end in same table.				*/
/*  3)  Found?								*/
/*									*/
/************************************************************************/

int docExtendSelectionFromTable(	DocumentPosition *	dpHead,
					DocumentPosition *	dpTail,
					int *			pCol0,
					int *			pCol1 )
    {
    struct BufferItem *	hCellNode= dpHead->dpNode->biParent;
    struct BufferItem *	hRowNode= hCellNode->biParent;
    struct BufferItem *	hParentNode= hRowNode->biParent;

    struct BufferItem *	tRowNode= dpTail->dpNode->biParent->biParent;
    struct BufferItem *	tParentNode= tRowNode->biParent;

    int			row1= hRowNode->biNumberInParent;

    int			col0= -1;
    int			col1= -1;

    /*  1  */
    if  ( tParentNode != hParentNode )
	{
	docHeadPosition( dpHead, hRowNode->biChildren[0] );

	if  ( dpTail->dpNode->biParaProperties->ppTableNesting > 0 )
	    { docExtendSelectionIntoTable( dpTail ); }

	return 0;
	}

    /*  2  */
    while( row1 < hRowNode->biRowTablePast- 1 )
	{
	if  ( tRowNode == hParentNode->biChildren[row1] )
	    { break;	}

	row1++;
	}

    if  ( tRowNode == hParentNode->biChildren[row1] )
	{
	if  ( dpTail->dpNode->biParent->biNumberInParent <
					    hCellNode->biNumberInParent )
	    {
	    col0= dpTail->dpNode->biParent->biNumberInParent;
	    col1= dpHead->dpNode->biParent->biNumberInParent;

	    docHeadPosition( dpHead, hRowNode->biChildren[col0] );
	    hRowNode= dpTail->dpNode->biParent->biParent;
	    docTailPosition( dpTail, hRowNode->biChildren[col1] );
	    }
	else{
	    docHeadPosition( dpHead, dpHead->dpNode->biParent );
	    docTailPosition( dpTail, dpTail->dpNode->biParent );
	    }

	col0= dpHead->dpNode->biParent->biNumberInParent;
	col1= dpTail->dpNode->biParent->biNumberInParent;
	}
    else{
	docHeadPosition( dpHead, hRowNode->biChildren[0] );

	if  ( dpTail->dpNode->biParaProperties->ppTableNesting > 0 )
	    { docExtendSelectionIntoTable( dpTail ); }

	return 0;
	}

    *pCol0= col0; *pCol1= col1; return 0;
    }
