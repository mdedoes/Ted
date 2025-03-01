/************************************************************************/
/*									*/
/*  Collect and exchange horizontal properties between the document	*/
/*  and the (top) ruler.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_RULER_H
#   define	DOC_SCREEN_RULER_H

struct BufferItem;
struct LayoutContext;
struct BlockFrame;
struct ColumnSeparator;
struct TabStopList;
struct ParagraphProperties;
struct BufferDocument;
struct ParagraphFrame;
struct RowProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRowGetRulerColumnsPixels(
			int *					pX0Columns,
			int *					pX1Columns,
			struct ColumnSeparator **		pCs,
			int *					pCsCount,
			const struct BufferItem *		rowNode,
			const struct LayoutContext *		lc,
			const struct BlockFrame *		bf );

int docRowGetRulerColumnTwips(
			struct RowProperties *			rpSet,
			const struct LayoutContext *		lc,
			int					sep,
			const struct ColumnSeparator *		csOld,
			const struct ColumnSeparator *		csNew );

extern int docParaGetTabStopsPixels(
			struct TabStopList *			tslPixels,
			const struct ParagraphProperties *	pp,
			const struct ParagraphFrame *		pf,
			const struct LayoutContext *		lc,
			const struct TabStopList *		tslTwips );

extern int docParaGetRulerTwips(
			const struct LayoutContext *		lc,
			const struct BlockFrame *		bf,
			const struct ParagraphFrame *		pf,
			struct BufferDocument *			bd,
			struct ParagraphProperties *		ppNew,
			int					firstIndentPix,
			int					leftIndentPix,
			int					rightIndentPix,
			const struct TabStopList *		tslNew );

extern int docParaGetRulerPixels(
			int *					pParaX0Pixels,
			int *					pParaX1Pixels,
			int *					pFirstIndPixels,
			int *					pLeftIndPixels,
			int *					pRightIndPixels,
			const struct LayoutContext *		lc,
			const struct ParagraphProperties *	pp,
			const struct ParagraphFrame *		pf );

#   endif	/*  DOC_SCREEN_RULER_H  */
