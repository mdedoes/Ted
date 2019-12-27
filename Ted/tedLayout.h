/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_LAYOUT_H
#   define		TED_LAYOUT_H

#   include	<drawDrawingSurface.h>

struct EditDocument;
struct TedDocument;
struct InsertedObject;
struct RasterImage;
struct PostScriptFontList;
struct SelectionDescription;
struct DocumentPosition;
struct DocumentSelection;
struct PositionGeometry;
struct LayoutContext;
struct SelectionGeometry;
struct BufferItem;
struct Point2DI;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern void tedSetScreenLayoutContext(	struct LayoutContext *	lc,
					struct EditDocument *	ed );

extern void tedSetDocumentLayoutContext(
		struct LayoutContext *			lc,
		DrawingSurface				ds,
		const struct PostScriptFontList * 	psfl,
		int					specialSectBreaks,
		struct TedDocument *			td );

extern int tedFindPosition(	struct DocumentPosition *	dp,
				struct BufferItem *		selRootNode,
				const struct BufferItem *	bodySectNode,
				const struct LayoutContext *	lc,
				int				docXPixels,
				int				docY );

extern int tedArrowDown(	struct DocumentPosition *	bp,
				const struct PositionGeometry *	pg,
				const struct LayoutContext *	lc );

extern int tedArrowUp(		struct DocumentPosition *	bp,
				const struct PositionGeometry *	pg,
				const struct LayoutContext *	lc );

extern int tedLayoutDocumentBody( int *				pReachedBottom,
				const struct LayoutContext *	lc );

extern void tedGetObjectRectangle(
				struct DocumentRectangle *	drObject,
				struct Point2DI *		xp,
				const struct InsertedObject *	io,
				const struct PositionGeometry *	pg,
				const struct LayoutContext *	lc,
				int				afterObject,
				const struct TedDocument *	td );

extern void tedSetObjectWindows(struct EditDocument *		ed,
				const struct PositionGeometry *	pg,
				const struct InsertedObject *	io,
				const struct LayoutContext *	lc );

extern struct InsertedObject * tedObjectMakeRasterObject(
				int *				pObjectNumber,
				struct EditDocument *		ed,
				struct BufferItem *		node,
				const struct LayoutContext *	lc,
				struct RasterImage *		ri );

extern void tedDocAdaptTopRuler( struct EditDocument *		ed,
				const struct DocumentSelection * ds,
				const struct SelectionGeometry * sg,
				const struct SelectionDescription *	sd,
				const struct BufferItem *	bodySectNode );

#   endif	/*	TED_LAYOUT_H	*/
