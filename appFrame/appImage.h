#   ifndef	APP_IMAGE_H
#   define	APP_IMAGE_H

#   include	<guiBase.h>

struct RasterImage;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appImgPastePixmap(		struct RasterImage *	ri,
					APP_WIDGET		w,
					APP_SELECTION_EVENT *	event );

#   if USE_MOTIF
    extern int appGetImageMotif(	struct RasterImage *	ri,
					const XImage *		xim,
					Display *		display );
#   endif

#   endif
