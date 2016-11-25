/************************************************************************/
/*									*/
/*  Drawing of metafiles (WMF,PICT)					*/
/*									*/
/************************************************************************/

#   include	<appGuiBase.h>

#   include	<bitmap.h>
#   include	<docFont.h>
#   include	<utilPostscriptFace.h>
#   include	<appDraw.h>
#   include	<appImage.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetafileDrawRoundRectX11(	AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h,
					int			fill,
					int			borders,
					APP_COLOR_RGB *		fillColor,
					APP_COLOR_RGB *		borderColor );

extern void appMetafilePolygonPathPs(	SimpleOutputStream *	sos,
					const APP_POINT *	points,
					int			count );

extern void appMetafileRoundRectPathPs(	SimpleOutputStream *	sos,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h );

extern void appMetafileRectPathPs(	SimpleOutputStream *	sos,
					int			x0,
					int			y0,
					int			x1,
					int			y1 );

extern int appMetafileStartPatternFillPs(	SimpleOutputStream *	sos,
						const AppBitmapImage *	abi );

