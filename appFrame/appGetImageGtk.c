/************************************************************************/
/*									*/
/*  Paste images.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if USE_GTK

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"appImage.h"

#   include	<appDebugon.h>

struct RasterImage;

int appImgPastePixmap(		struct RasterImage *	ri,
				APP_WIDGET		w,
				APP_SELECTION_EVENT *	event )
    {
LDEB(1); return -1;
    }

#   endif
