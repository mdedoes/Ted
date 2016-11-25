/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_WIDGETS_GTK_H
#   define	GUI_WIDGETS_GTK_H

#   include	"guiBase.h"

#   if	USE_GTK

#   include	"guiOptionmenu.h"

# ifdef __cplusplus
extern "C"
    {
# endif

struct NamedImage;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int guiGtkGetChildLabel(		const char **		pLabel,
					APP_WIDGET		w );

extern int guiGtkSetChildLabel(		APP_WIDGET		w,
					const char *		s );

extern void guiGtkListTree(		int			indent,
					APP_WIDGET		w );

extern void guiMakeOptionmenuGtk(	AppOptionmenu *		aom,
					OptionmenuCallback	callBack,
					void *			target );

extern APP_WIDGET guiGtkMakeGrid(	int			rows,
					int			columns,
					int			homogeneous );

extern void guiGtkAttachChildInGrid(	APP_WIDGET		grid,
					APP_WIDGET		child,
					int			left,
					int			colspan,
					int			top,
					int			rowspan,
					int			hexpand,
					int			vexpand,
					int			hmargin,
					int			vmargin );

extern int guiGtkMakeImageWidget(	APP_WIDGET *		pImageW,
					struct NamedImage *	ni );

# ifdef __cplusplus
    }
# endif

#   endif
#   endif
