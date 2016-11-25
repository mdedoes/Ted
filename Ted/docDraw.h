#   ifndef	DOC_DRAW_H
#   define	DOC_DRAW_H

#   include	"docBuf.h"
#   include	"docLayout.h"
#   include	<appDraw.h>

/************************************************************************/
/*									*/
/*  The different resources needed for drawing.				*/
/*									*/
/************************************************************************/

struct DrawingContext;

typedef void (*PARA_SIDES_PIXELS)( struct ParagraphFrame *	pf,
				const AppDrawingData *		add,
				const BufferItem *		bi );

typedef int (*DRAW_TEXT_LINE)(	const BufferItem *		bi,
				int				line,
				const struct ParagraphFrame *	pf,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_TOP)(	const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpTop,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_BOTTOM)( const BorderProperties *	bpBottom,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpBelow,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_SHADE)(	const ParagraphProperties *	pp,
				void *				through,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_TOP)(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				through,
				struct DrawingContext *		dc,
				const LayoutPosition *		lpTop );

typedef int (*DRAW_CELL_BOTTOM)( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				through,
				struct DrawingContext *		dc,
				const LayoutPosition *		lpBottom );

typedef int (*DRAW_CELL_LEFT)(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				through,
				struct DrawingContext *		dc,
				int				x0Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_RIGHT)(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				through,
				struct DrawingContext *		dc,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_SHADE)(	const CellProperties *		cp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				void *				through,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*FINISH_PAGE)(	void *				through,
				struct DrawingContext *		dc,
				int				page,
				int				asLast );

typedef int (*START_PAGE)(	void *				through,
				const DocumentGeometry *	dg,
				struct DrawingContext *		dc,
				int				page );

typedef int (*SET_COLOR_RGB)(	struct DrawingContext *		dc,
				void *				through,
				int				r,
				int				g,
				int				b );

typedef int (*SET_FONT)(	struct DrawingContext *		dc,
				void *				through,
				const TextAttribute *		ta,
				int				physicalFont );

typedef struct DrawingContext
    {
    int				dcCurrentTextAttributeSet;
    TextAttribute		dcCurrentTextAttribute;
    int				dcCurrentColorSet;
    RGB8Color			dcCurrentColor;

    AppDrawingData *		dcDrawingData;
    BufferDocument *		dcDocument;
    const DocumentRectangle *	dcClipRect;
    const DocumentSelection *	dcDocumentSelection;
    const SelectionGeometry *	dcSelectionGeometry;
    int				dcFirstPage;
    int				dcLastPage;
    int				dcDrawHeadersFooters;

    int				dcDrawTableGrid;

    SET_COLOR_RGB		dcSetColorRgb;
    SET_FONT			dcSetFont;

    DRAW_TEXT_LINE		dcDrawTextLine;

    DRAW_PARA_TOP		dcDrawParaTop;
    DRAW_PARA_BOTTOM		dcDrawParaBottom;
    DRAW_PARA_SHADE		dcDrawParaShade;
    DRAW_CELL_TOP 		dcDrawCellTop;
    DRAW_CELL_BOTTOM 		dcDrawCellBottom;
    DRAW_CELL_LEFT		dcDrawCellLeft;
    DRAW_CELL_RIGHT		dcDrawCellRight;
    DRAW_CELL_SHADE		dcDrawCellShade;
    FINISH_PAGE			dcFinishPage;
    START_PAGE			dcStartPage;
    PARA_SIDES_PIXELS		dcParaFramePixels;
    LAYOUT_EXTERNAL		dcLayoutExternal;
    DOC_CLOSE_OBJECT		dcCloseObject;
    } DrawingContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDrawingContext(	DrawingContext *	dc );

extern int docDrawItem(	BufferItem *			bi,
			void *				through,
			DrawingContext *		dc );

extern int docDrawPageHeader(	BufferItem *			sectBi,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawPageFooter(	BufferItem *			sectBi,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawFootnotesForColumn(
				int				page,
				void *				through,
				DrawingContext *		dc );

extern int docDrawEndnotesForSection(	int			sect,
					void *			through,
					DrawingContext *	dc );

extern int docDrawEndnotesForDocument(	void *			through,
					DrawingContext *	dc );

extern int docDrawToPageOfItem(		BufferItem *		prevBodyBi,
					BufferItem *		thisBodyBi,
					BufferItem *		thisBi,
					void *			through,
					LayoutPosition *	lpHere,
					DrawingContext *	dc );

extern void docDrawSetColorRgb(		DrawingContext *	dc,
					void *			through,
					int			r,
					int			g,
					int			b );

extern void docDrawSetColorNumber(	DrawingContext *	dc,
					void *			through,
					int			colorNumber );

extern void docDrawDelimitString(	int *			pLen,
					int *			pDrawn,
					TextAttribute *		ta,
					const BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			count,
					int			separate );

extern void docDrawSetFont(	DrawingContext *	dc,
				void *			through,
				const TextAttribute *	ta,
				int			physicalFont );

#   endif
