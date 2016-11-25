/************************************************************************/
/*									*/
/*  Build trees to represent the hierarchy defined by the unicode	*/
/*  bidirectionals algorithm.						*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   ifndef		BIDI_TREE_BUILDER_IMPL_H
#   define		BIDI_TREE_BUILDER_IMPL_H

struct BidiNode;
struct ExplicitLevel;

typedef struct BidiTreeBuilder
    {
			/**
			 *  The root (bottom node) of the tree.
			 */
    struct BidiNode *	btbBottomNode;

			/**
			 *  The current node.
			 *  (Initially, this is the root node.)
			 */
    struct BidiNode *	btbCurrentNode;

			/**
			 *  The insertion point in the paragraph. (Like a C 
			 *  string offset, but as we implicitly use UTF-8 
			 *  encoding, this actually is a byte offset.)
			 */
    int			btbOffset;

			/**
			 *  The higest level run found on the way.
			 */
    int			btbHighestLevel;

			/**
			 *  The top of a stack of explicit levels
			 */

    struct ExplicitLevel * btbExplicitLevel;
    } BidiTreeBuilder;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

#   endif	/*	BIDI_TREE_BUILDER_IMPL_H	*/
