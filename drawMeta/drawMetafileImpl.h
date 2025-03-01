/************************************************************************/
/*									*/
/*  Common purely administrative functionality.				*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_METAFILE_IMPL_H
#   define		DRAW_METAFILE_IMPL_H

struct DeviceContext;
struct LogicalBrush;
struct LogicalPen;
struct LogicalFont;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaSelectBrushObjectImpl(struct DeviceContext *	dc,
					void *			through,
					struct LogicalBrush *	lb );

extern int appMetaSelectPenObjectImpl(	struct DeviceContext *	dc,
					void *			through,
					struct LogicalPen *	lp );

extern int appMetaSelectFontObjectImpl(	struct DeviceContext *	dc,
					void *			through,
					struct LogicalFont *	lf );

extern int appMetaGetPenDots(		struct DeviceContext *	dc,
					unsigned char		dots[6],
					const struct LogicalPen *	lp );

extern int appMetaGetPenWidth(		struct DeviceContext *		dc,
					const struct LogicalPen *	lp );

#   endif	/*	DRAW_METAFILE_IMPL_H	*/
