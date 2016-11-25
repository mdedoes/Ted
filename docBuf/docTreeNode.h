/************************************************************************/
/*									*/
/*  A node in the document tree.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TREE_NODE_H
#   define	DOC_TREE_NODE_H

#   include	"docParaNode.h"
#   include	"docCellNode.h"
#   include	"docRowNode.h"
#   include	"docSectNode.h"

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct TextLine;
struct TextParticule;

/************************************************************************/
/*									*/
/*  A BufferItem.							*/
/*									*/
/*  A BufferItem is a node in what we fashionably call the document	*/
/*  object model. In a typical XML type DOM, the buffer items would be	*/
/*  element nodes. An item at level DOClevPARA is very much comparable	*/
/*  to a <div> in HTML terms. The HTML equivalent of a <span> does not	*/
/*  exist inside the Ted source code. Spans are implemented by a series	*/
/*  of text particules of kind DOCkindSPAN with equal text attributes.	*/
/*									*/
/*  1)  Fields for managing the hierarchy.				*/
/*  2)  Geometry information that is set by the formatter.		*/
/*	The insets are an amount of space that the item claims for	*/
/*	itself. The content of the item (children, or lines) appears	*/
/*	between the insets. Typically, the insets are used for spacing	*/
/*	or borders.							*/
/*  3)  Information that is specific to the kind of item.		*/
/*									*/
/************************************************************************/

typedef struct BufferItem
    {
				/*  1  */
    unsigned char		biLevel;
    unsigned char		biTreeType;
    struct BufferItem *		biParent;
    struct BufferItem **	biChildren;
				/**
				 *  The number of children of the node. 
				 *  Contrary to expexctations, real world 
				 *  figures do not fit in a 'short int'.
				 */
    int				biChildCount;
    int				biNumberInParent;
    int				biLeftParagraphs;

				/*  2  */
    LayoutPosition		biTopPosition;
    LayoutPosition		biBelowPosition;

				/*  3  */
    union			BufferItemUnion
	{
	ParaNode		biuPara;
	CellNode		biuCell;
	SectNode		biuSect;
	RowNode			biuRow;
	} BIU;

    } BufferItem;

# define docIsCellNode( bi )	( (bi)->biLevel == DOClevCELL )
# define docIsParaNode( bi )	( (bi)->biLevel == DOClevPARA )
# define docIsSectNode( bi )	( (bi)->biLevel == DOClevSECT )
# define docIsRowNode( bi )	( (bi)->biLevel == DOClevROW && \
				  (bi)->biRowCellCount > 0 )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
