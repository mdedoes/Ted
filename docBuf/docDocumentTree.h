/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_TREE_H
#   define	DOC_DOCUMENT_TREE_H

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
				    /**
				     *  The document node that is the root of
				     *  this tree.
				     */
    struct BufferItem *		dtRoot;

				    /**
				     *  Header and footer elements are repeated
				     *  on multiple pages. As positions of the 
				     *  content of the tree might depend on the
				     *  actual page (even/odd) we keep track of the 
				     *  page: We must reformat on a new page.
				     */
    int				dtPageFormattedFor;
    int				dtPageSelectedUpon;

				    /**
				     *  See comment on dtPageFormattedFor. Column
				     *  width may vary between pages. So the (newspaper
				     *  style) column determines the layout.
				     */
    int				dtColumnFormattedFor;
    int				dtColumnSelectedIn;

				    /**
				     *  The actual top and bottom positions that we find
				     *  to be used after formatting the tree.
				     *  (Might be copied to the Reserved equivalents if 
				     *  we use more than reserved)
				     */
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

    ListNumberTrees		dtListNumberTrees;
    ListNumberTree		dtOutlineTree;

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
					struct DocumentField *	df );

#   endif
