/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXTERNAL_ITEM_H
#   define	DOC_EXTERNAL_ITEM_H

#   include	<docListNumberTree.h>
#   include	<docChildFields.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentField;

/************************************************************************/
/*									*/
/*  A structure describing a tree in the document.			*/
/*									*/
/************************************************************************/

typedef struct DocumentTree
    {
    struct BufferItem *		dtRoot;
    int				dtPageFormattedFor;
    int				dtColumnFormattedFor;
    int				dtY0UsedTwips;
    int				dtY1UsedTwips;
				/**
				 *  The top position that is reserved for the 
				 *  tree. For the page footers, this also 
				 *  determines the page bottom for the body 
				 *  text and the notes.
				 */
    int				dtY0ReservedTwips;
    int				dtY1ReservedTwips;

    int				dtPageSelectedUpon;
    int				dtColumnSelectedIn;

    ListNumberTrees		dtListNumberTrees;
    ListNumberTreeNode		dtOutlineTree;

    ChildFields			dtRootFields;
    } DocumentTree;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentTree(	DocumentTree *		dt );

extern void docCleanDocumentTree(	struct BufferDocument *	bd,
					DocumentTree *		dt );

extern void docInvalidateTreeLayout(	DocumentTree *		dt );

extern int docAddRootFieldToTree(	DocumentTree *		dt,
					struct DocumentField *	dfCh );

#   endif
