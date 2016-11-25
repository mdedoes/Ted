/************************************************************************/
/*									*/
/*  A group of widgets to manipulate page margins.			*/
/*									*/
/************************************************************************/

#   ifndef APP_MARGIN_TOOL_H
#   define APP_MARGIN_TOOL_H

#   include	<guiBase.h>

struct PropertyMask;
struct DocumentGeometry;

typedef struct AppMarginTool
    {
    APP_WIDGET			amtMarginFrame;
    APP_WIDGET			amtInsideWidget;
    APP_WIDGET			amtLeftColumn;
    APP_WIDGET			amtRightColumn;

    APP_WIDGET			amtLeftMarginText;
    APP_WIDGET			amtTopMarginText;
    APP_WIDGET			amtRightMarginText;
    APP_WIDGET			amtBottomMarginText;
    } AppMarginTool;

typedef struct MarginToolResources
    {
    char *	mtrTitleText;
    char *	mtrLeftMarginText;
    char *	mtrTopMarginText;
    char *	mtrRightMarginText;
    char *	mtrBottomMarginText;
    } MarginToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMarginToolShowMargins(	AppMarginTool *			amt,
					int				unitInt,
					const struct DocumentGeometry *	dg );

extern int appMarginToolGetMargins(	struct PropertyMask *		pUpdMask,
					const struct PropertyMask *	chgMask,
					int			unitType,
					AppMarginTool *		amt,
					struct DocumentGeometry *	dg );

extern int appMarginToolCheckMargins(	const struct DocumentGeometry * dg );

extern void appMakeMarginToolWidgets(
				APP_WIDGET			parent,
				const MarginToolResources *	mtr,
				AppMarginTool *			amt,
				APP_TXACTIVATE_CALLBACK_T	gotValue,
				void *				through );

extern void appEnableMarginTool(	AppMarginTool *		amt,
					int			enabled );

#   endif /*  APP_MARGIN_TOOL_H  */
