/************************************************************************/
/*									*/
/*  Private functions on a drawing surface.				*/
/*									*/
/************************************************************************/

#   include <drawDrawingSurface.h>
#   include "guiBase.h"

struct PostScriptFontList;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Utility functions.							*/
/*									*/
/************************************************************************/

#   if ! USE_HEADLESS

extern int drawFontImplementationName(
			struct MemoryBuffer *		target,
			const DrawingSurface		ds,
			int				screenFont );

extern int drawOpenDefaultFont(
			DrawingSurface			ds,
			const struct PostScriptFontList *	psfl,
			APP_WIDGET			w,
			int				fontSizeHintPixels );

#   endif
