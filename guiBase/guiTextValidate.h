/************************************************************************/
/*									*/
/*  Exchange values with a text widget.					*/
/*									*/
/************************************************************************/

#   ifndef	GUI_TEXT_VALIDATE_H
#   define	GUI_TEXT_VALIDATE_H

#   include	"guiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   if  ! USE_HEADLESS

extern int guiTextShowValidity( APP_WIDGET	text,
				int		given,
				int		valid );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif
