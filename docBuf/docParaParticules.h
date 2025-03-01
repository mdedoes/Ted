/************************************************************************/
/*									*/
/*  Manage the particules and lines in a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PARTICULES_H
#   define	DOC_PARA_PARTICULES_H

struct DocumentPosition;
struct DocumentSelection;
struct BufferItem;
struct TextParticule;
struct TextLine;
struct BufferDocument;
struct DocumentTree;

	/**
	 * Find the first particule that contains this position
	 */
#   define PARAfindFIRST	0

	/**
	 * Find the last particule that contains this position
	 */
#   define PARAfindLAST		1

	/**
	 * Find the last particule that contains this position
	 * Or.. If it is the last position in the paragraph,
	 * return the position past the last particule.
	 */
#   define PARAfindPAST		2

/************************************************************************/

	/**
	 *  The position is at the head of the particule.
	 */
#   define POSflagPART_HEAD		0x01

	/**
	 *  The position is at the tail of the particule.
	 */
#   define POSflagPART_TAIL		0x02

	/**
	 *  The position is immediately after a line or page break.
	 */
#   define POSflagPART_AFTER_BREAK	0x04

	/**
	 *  The position is at the head of the line.
	 */
#   define POSflagLINE_HEAD		0x08

	/**
	 *  The position is at the tail of the line.
	 */
#   define POSflagLINE_TAIL		0x10

	/**
	 *  The position is at the head of the paragraph.
	 */
#   define POSflagPARA_HEAD		0x20

	/**
	 *  The position is at the tail of the paragraph.
	 */
#   define POSflagPARA_TAIL		0x40

	/**
	 *  Used when a position is compared to a selection.
	 *  The flag is set if the position is inside the selection.
	 */
#   define POSflagPARA_FOUND		0x80

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFindParticuleOfPosition(
				int *				pPart,
				int *				pFlags,
				const struct DocumentPosition *	dp,
				int				lastOne );

extern int docFindLineOfPosition( int *				pLine,
				int *				pFlags,
				const struct DocumentPosition *	dp,
				int				lastOne );

extern int docGetLineOfPosition( int *				pLine,
				const struct DocumentPosition *	dp,
				int				positionFlags );

extern void docSetLineFlags(	int *				pFlags,
				const struct BufferItem *	paraNode,
				int				stroff,
				const struct TextLine *		tl );

extern void docInitParaNode(	struct BufferItem *		paraNode );
extern void docCleanParaNode(	struct BufferDocument *		bd,
				struct DocumentTree *		dt,
				struct BufferItem *		paraNode );

extern int docParagraphIsEmpty(	const struct BufferItem *	paraNode );

extern struct TextLine * docInsertTextLine(
				struct BufferItem *		paraNode,
				int				line );

extern void docCleanParticuleObject(
				struct BufferDocument *		bd,
				struct TextParticule *		tp );

extern int docIntersectSelectionWithParagraph(
				struct DocumentSelection *	dsPara,
				int *				pPartFrom,
				int *				pPartUpto,
				int *				pHeadFlags,
				int *				pTailFlags,
				const struct BufferItem *	paraNode,
				const struct DocumentSelection * ds );

extern int docParaDelimitDirectionalRun(
				int *				pDirection,
				int *				pStroffUpto,
				int *				pPartUpto,
				const struct BufferItem *	paraNode,
				int				stroffFrom );

extern void docSetParticuleFlags( int *				pFlags,
				const struct BufferItem *	paraNode,
				int				part,
				int				stroff );

#   endif		/*  DOC_PARA_PARTICULES_H	*/
