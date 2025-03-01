/************************************************************************/
/*									*/
/*  Header file for geometry/formatting/drawing related functionality	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_LAYOUT_H
#   define	DOC_SCREEN_LAYOUT_H

struct LayoutContext;
struct LayoutJob;
struct DocumentTree;
struct BufferItem;
struct DocumentRectangle;
struct BufferDocument;
struct TextRun;
struct AfmFontInfo;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docOpenScreenFont(	
			const struct LayoutContext *		lc,
			int					textAttrNr );

extern int docStartScreenLayoutForTree(
				struct LayoutJob *		lj,
				struct DocumentTree *		tree,
				int				page,
				int				column );

extern int docScreenLayoutNode(	int *				pReachedBottom,
				struct BufferItem *		node,
				const struct LayoutContext *	lc,
				struct DocumentRectangle *	drChanged );

extern int docScreenLayoutDocumentBody(
				int *				pReachedBottom,
				struct BufferDocument *		bd,
				const struct LayoutContext *	lc );

extern void docSetScreenLayoutFunctions(	struct LayoutJob *	lj );

extern int docLayoutMakeOutput(
			const char **			pPrintString,
			char **				pScratchString,
			int **				pSegments,
			int *				pSegmentCount,
			int *				pFullScreenFont,
			int *				pScapsScreenFont,
			const struct AfmFontInfo **	pAfi,
			int *				pFullSizeTwips,
			int *				pScapsSizeTwips,
			const struct TextRun *		tr,
			int				mirror,
			const struct LayoutContext *	lc,
			const char *			outputString,
			int				len );

extern int docGetScreenWidth(
			const struct LayoutContext *	lc,
			int				xPixels,
			int				yPixels,
			int				fullScreenFont,
			int				scapsScreenFont,
			const char *			outputString,
			const int			outputLength,
			const int *			segments,
			int				segmentCount );

#   endif	/*  DOC_SCREEN_LAYOUT_H	*/
