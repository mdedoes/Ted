#   ifndef		DOC_TREE_SCANNER_H
#   define		DOC_TREE_SCANNER_H

struct BufferDocument;
struct DocumentTree;
struct DocumentSelection;
struct BufferItem;

/**
 *  Called for every node that we encounter along the way
 */
typedef int (*NodeVisitor)(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					through );

/**
 *  Only called for trees that belong to a certain node in 
 *  the tree. The flags decide whether we make an excursion into.
 *  them or not. Tree visitors are not invoked for the main tree of the scan.
 */
typedef int (*TreeVisitor)(
			struct DocumentTree *			tree,
			const struct BufferItem *		bodySectNode,
			void *					through );

#   define FLAGtsSCAN_MERGED_CELLS		0x01
#   define FLAGtsSCAN_SECTION_HEADERS_FOOTERS	0x02
#   define FLAGtsSCAN_BODY_SEPARATORS		0x04
#   define FLAGtsSCAN_FOOT_END_NOTES		0x08

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docScanTree(	struct BufferDocument *		bd,
			struct DocumentTree *		tree,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			TreeVisitor			enterTree,
			TreeVisitor			leaveTree,
			int				flags,
			void *				through );

extern int docScanSelection(
			struct BufferDocument *		bd,
			const struct DocumentSelection * ds,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			TreeVisitor			enterTree,
			TreeVisitor			leaveTree,
			int				flags,
			void *				through );

extern int docScanTreeNode(
			struct BufferDocument *		bd,
			struct BufferItem *		node,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			TreeVisitor			enterTree,
			TreeVisitor			leaveTree,
			int				flags,
			void *				through );

#   endif	/*	DOC_TREE_SCANNER_H	*/
