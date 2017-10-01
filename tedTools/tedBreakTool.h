/************************************************************************/
/*									*/
/*  Select a break kind on one of the tools				*/
/*									*/
/************************************************************************/

#   include	<docBreakKind.h>
#   include	<guiBase.h>
#   include	<guiOptionmenu.h>

/************************************************************************/
/*									*/
/*  Resources for a 'Break' tool.					*/
/*									*/
/************************************************************************/

typedef struct BreakToolResources
    {
    const char *	btrBreakKind;
    const char *	btrBreakKindMenuTexts[DOCibk_COUNT];
    } BreakToolResources;

/************************************************************************/
/*									*/
/*  A break tool, i.e. the controls to select the kind of break.	*/
/*									*/
/************************************************************************/

typedef struct BreakTool
    {
    APP_WIDGET				btBreakKindRow;
    APP_WIDGET				btRowLabel;
    AppOptionmenu			btBreakKindOptionmenu;
    } BreakTool;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void tedInitBreakTool(		BreakTool *	bt );

extern void tedSetBreakTool(		BreakTool *	bt,
					int		breakKind );

extern void tedEnableBreakTool(		BreakTool *	bt,
					int		enabled );

extern void tedEnableBreakToolOption(	BreakTool *	bt,
					int		option,
					int		enabled );

extern void tedBuildBreakTool(	APP_WIDGET			parent,
				BreakTool *			bt,
				const BreakToolResources *	btr,
				OptionmenuCallback		callback,
				void *				through );

extern void tedFillBreakToolChooser(
				int				count,
				int				current,
				BreakTool *			bt,
				const BreakToolResources *	btr );

extern void tedFinishBreakTool(	BreakTool *			bt );
