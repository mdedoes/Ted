/************************************************************************/
/*									*/
/*  Build trees to represent the hierarchy defined by the unicode	*/
/*  bidirectionals algorithm.						*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   ifndef		BIDI_TREE_BUILDER_H
#   define		BIDI_TREE_BUILDER_H

struct BidiTreeBuilder;
struct BidiNode;
struct BidiRun;
struct BidiScanner;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern struct BidiTreeBuilder * bidiOpenTreeBuilder( void );

extern int bidiFinishTreeBuilder(
				struct BidiScanner *		bs,
				struct BidiTreeBuilder *	btb,
				int				upto );

extern void bidiCloseTreeBuilder(
				struct BidiTreeBuilder *	btb );

extern void bidiStartTreeBuilder(
				struct BidiTreeBuilder *	btb,
				int				bottomLevel );

extern int bidiTreeBuilderStartExistingTree(
				struct BidiTreeBuilder *	btb,
				struct BidiNode *		bottomNode,
				int				byteOffset );

extern int bidiTreeBuilderAddRun(
				struct BidiTreeBuilder *	btb,
				int				initiator,
				int				level,
				int				from,
				int				upto );

extern int bidiTreeBuilderHandleImplicitClass(
				const struct BidiScanner *	bs,
				struct BidiTreeBuilder * 	btb,
				int				klass,
				int				offset );

extern int bidiTreeBuilderCloseExplicitRun(
				const struct BidiScanner *	bs,
				struct BidiTreeBuilder *	btb,
				int				eor,
				int				upto );

extern int bidiTreeLevelSwitchToIsolateLevel(
				const struct BidiScanner *	bs,
				struct BidiTreeBuilder *	btb,
				int				level,
				int				offset );

extern struct BidiNode * bidiTreeBuilderGetRootNode(
				struct BidiTreeBuilder *	btb );

extern int bidiTreeBuilderGetCurrentRun(
				struct BidiRun *		br,
				struct BidiTreeBuilder *	btb );

#   endif	/*	BIDI_TREE_BUILDER_H	*/
