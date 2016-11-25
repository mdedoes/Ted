/************************************************************************/
/*									*/
/*  A node in the bidirectional tree.					*/
/*									*/
/************************************************************************/

#   ifndef		TEXT_BIDI_TREE_H
#   define		TEXT_BIDI_TREE_H

struct MemoryBuffer;
struct BidiScanner;


/**
 *  A run in a bidi tree. It either represents the run of a node, 
 *  or the run between two children of the node. (Or before the 
 *  first child, or after the last one.)
 */
typedef struct BidiRun
    {
			/**
			 *  String offset in bytes of the run.
			 *  Implicitly assumes UTF-8
			 */
    int			brFrom;

			/**
			 *  Past the last byte in the run.
			 */
    int			brUpto;

			/**
			 *  The embedding level of the text run.  As levels 
			 *  never exceed 125+1, an unsigned char is sufficient.
			 */
    unsigned char	brLevel;

			/**
			 *  The initiator for this level. Apart from the 
			 *  initiators in the strict sense ( LRE, RLE,
			 *  LRO, RLO, LRI, RLI, FSI ) we support L, R, ON 
			 *  for runs that originate from implicit processing.
			 *
			 *  Directional controls in the document are handled
			 *  as if they originate from LRI and RLI in the 
			 *  document. This does NOT mean that a tree builder 
			 *  must implement TR9 type explicit processing.
			 */
    unsigned char	brInitiator;
    } BidiRun;

/**
 *  A node in the Bidi tree.
 *  Nodes without children are 'Level Runs' in the sense of BD7 in the
 *  Unicode TR9 bidirectional positioning algorithm.
 *
 *  See: http://unicode.org/reports/tr9/
 *
 */
typedef struct BidiNode
    {
			/**
			 *  The base run of the node. NOTE that children 
			 *  are not contiguous, and that the initiator 
			 *  only applies for the piece before the first 
			 *  child.
			 */
    BidiRun		bnRun;

			/**
			 *  The parent node. Is NULL in the root of a tree.
			 */
    struct BidiNode *	bnParent;

			/**
			 *  The children (If any) of the node.
			 */
    struct BidiNode **	bnChildren;
			/**
			 *  The number of children of the node. 
			 */
    unsigned short	bnChildCount;
			/**
			 *  The index in the parents bnChildren array.
			 */
    unsigned short	bnNumberInParent;
    } BidiNode;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void bidiInitRun(		BidiRun *		br );

extern void bidiInitTreeNode(		BidiNode *		bn );
extern void bidiCleanTreeNode(		BidiNode *		bn );
extern void bidiNodeDeleteNode(		BidiNode *		bn );

extern BidiNode * bidiNodeAddChild(	BidiNode *		parent,
					int			initiator,
					int			level,
					int			from,
					int			upto );

extern BidiNode * bidiNodeStartTree(	int			initiator,
					int			level,
					int			from,
					int			upto );

extern int bidiStretchNode(		BidiNode *		bn,
					int			upto );

extern int bidiShiftNode(		BidiNode *		bn,
					int			from,
					int			by );

extern int bidiTraverseNodeDisplayOrder(const struct BidiNode *	root,
					const struct BidiScanner * bs,
					int			from,
					int			upto );

extern int bidiTraverseNodeLogicalOrder(const struct BidiNode *	root,
					const struct BidiScanner * bs,
					int			from,
					int			upto );

extern void bidiListNode(	const struct BidiNode *		bn,
				const struct MemoryBuffer *	symbols );

extern int bidiDelimitDirectionalRun(
				int *				pDirection,
				int *				pUpto,
				const BidiNode *		root,
				int				from );

extern struct BidiNode * bidiFindNode(
				BidiRun *			br,
				struct BidiNode *		root,
				int				offset );

#   endif	/*	TEXT_BIDI_TREE_H	*/
