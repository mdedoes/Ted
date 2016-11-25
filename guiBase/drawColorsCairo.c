/************************************************************************/
/*  Color mangement on X11: Try to get around the limitations.		*/
/************************************************************************/

#   include	"guiBaseConfig.h"
#   include	"guiBase.h"

#   if		DRAW_CAIRO

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"drawUtilGdk.h"
#   include	"drawColors.h"

int drawColorRgb(	APP_COLOR_RGB *		xc,
			struct AppColors *	acSys,
			unsigned int		r,
			unsigned int		g,
			unsigned int		b )
    {
    return 0;
    }

#   endif	/*  DRAW_CAIRO	*/
