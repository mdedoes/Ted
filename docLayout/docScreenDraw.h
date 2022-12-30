/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  guiExposeDrawingWidget().						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_DRAW_H
#   define	DOC_SCREEN_DRAW_H

#   include	<drawDrawingSurface.h>
#   include	<utilColor.h>
#   include	<psShading.h>

struct InsertedObject;
struct BlockOrnaments;
struct DrawTextLine;
struct LayoutPosition;
struct TextRun;
struct DrawingContext;
struct DocumentRectangle;
struct AffineTransform2D;
struct DrawingShape;
struct LayoutContext;
struct TextAttribute;
struct DocumentSelection;
struct SelectionGeometry;
struct TextLine;
struct PositionGeometry;

typedef struct ScreenDrawingData
    {
    RGB8Color				sddForeColor;
    RGB8Color				sddBehindColor;
    RGB8Color				sddHighlightColor;
    RGB8Color				sddGridColor;

    struct DocumentTree *		sddSelRootTree;
    struct BufferItem *			sddSelRootNode;
    struct BufferItem *			sddSelRootBodySectNode;
    int					sddBoxFirstPage;
    int					sddBoxLastPage;

    const struct DocumentRectangle *	sddFullRect;
    } ScreenDrawingData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docScreenDrawDrawingShape(
			const struct DocumentRectangle *	drOutside,
			const struct AffineTransform2D *	atOutside,
			int					page,
			struct DrawingShape *			ds,
			struct DrawingContext *			dc,
			void *					vsdd );

extern int docScreenDrawStartTextLine(
			struct DrawTextLine *			dtl,
			int					x0Twips );

extern int docScreenDrawInlineObject(
			const struct DrawTextLine *		dtl,
			int					part,
			struct InsertedObject *			io,
			const struct DocumentRectangle *	drTwips,
			const struct LayoutPosition *		baseLine );

extern void docScreenDrawShadedRectangle(
			const struct LayoutContext *		lc,
			DrawingSurface				shadingPixmaps[PSshd_COUNT],
			int					pattern,
			const struct DocumentRectangle *	drShade );

extern int docScreenDrawTab(
			const struct DrawTextLine *		dtl,
			int					part,
			int					textAttrNr,
			const struct TextAttribute *		ta,
			int					leader,
			int					x0Twips,
			int					x1Twips,
			const struct LayoutPosition *		baseLine );

extern int docScreenDrawFtnsep( const struct DrawTextLine *	dtl,
			int				part,
			int				textAttrNr,
			const struct TextAttribute *		ta,
			int				x0Twips,
			int				x1Twips,
			const struct LayoutPosition *		baseLine );

extern int docScreenDrawTextRun( const struct TextRun *		tr,
			int					x0Twips,
			int					x1Twips,
			const struct DrawTextLine *		dtl,
			const struct LayoutPosition *		baseLine,
			const char *				outputString );

extern int docScreenDrawRunUnderline(
			const struct DrawTextLine *		dtl,
			int					part,
			int					upto,
			int					direction,
			int					textAttrNr,
			const struct TextAttribute *		ta,
			int					x0Twips,
			int					x1Twips,
			const struct LayoutPosition *		baseLine );

extern int docScreenDrawRunStrikethrough(
			const struct DrawTextLine *		dtl,
			int					part,
			int					upto,
			int					direction,
			int					textAttrNr,
			const struct TextAttribute *		ta,
			int					x0Twips,
			int					x1Twips,
			const struct LayoutPosition *		baseLine );

extern int docScreenDrawOrnaments(
			const struct BlockOrnaments *		bo,
			int					page,
			const struct DocumentRectangle *	drOutside,
			const struct DocumentRectangle *	drInside,
			void *					through,
			struct DrawingContext *			dc );

extern int docScreenDrawSetColorRgb(
			struct DrawingContext *			dc,
			void *					vsdd,
			const struct RGB8Color *		rgb8 );

extern int docScreenDrawSetFont(
			struct DrawingContext *			dc,
			void *					vsdd,
			int					textAttrNr,
			const struct TextAttribute *		ta );

extern void docOriginalClipping(
			struct DrawingContext *			dc,
			struct ScreenDrawingData *		sdd );

extern int docScreenDrawSelection(
			struct BufferItem *			selRootNode,
			const struct BufferItem *		bodySectNode,
			ScreenDrawingData *			sdd,
			struct DrawingContext *			dc,
			const struct DocumentSelection *	ds,
			int					headPosFlags,
			int					tailPosFlags,
			int					tableRect );

extern void docScreenDrawPageRect(
			struct ScreenDrawingData *		sdd,
			struct DrawingContext *			dc,
			int					page );

extern void docScreenGetIBarRect(
			struct DocumentRectangle *		drPixels,
			const struct PositionGeometry *		pg,
			const struct LayoutContext *		lc );

extern int docScreenDrawIBar(
			const struct DocumentRectangle *	drPixels,
			const struct LayoutContext *		lc );

extern void docScreenDrawObjectBlocks(
			const struct DocumentRectangle *	drObj,
			const struct Point2DI *			xp,
			struct DrawingContext *			dc,
			struct ScreenDrawingData *		sdd );

extern int docScreenDrawBoxAroundTree(
			const struct BufferItem *		bodySectNode,
			const struct DocumentTree *		tree,
			struct ScreenDrawingData *		sdd,
			struct DrawingContext *			dc );

extern int docScreenDrawCheckPageOfSelectedTree(
			struct SelectionGeometry *		sg,
			struct BufferItem **			pBodySectNode,
			const struct DocumentSelection *	ds,
			struct DocumentTree *			selRootTree,
			const struct LayoutContext *		lc );

extern int docScreenDrawParticulesSeparately(
			const struct BufferItem *		paraNode,
			const struct TextLine *			tl );

extern void docScreenDrawSetFunctions(
			struct DrawingContext *			dc );

extern void docScreenDrawBackground(	
			struct DrawingContext *			dc,
			struct ScreenDrawingData *		sdd,
			const struct RGB8Color *		bgColor );

#   endif	/*  DOC_SCREEN_DRAW_H	*/
