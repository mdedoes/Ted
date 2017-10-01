/************************************************************************/
/*									*/
/*  A break tool: configure where a document node starts		*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<string.h>

#   include	"tedBreakTool.h"
#   include	<docBreakKind.h>
#   include	<guiWidgets.h>
#   include	<guiToolUtil.h>

#   include	<appDebugon.h>

void tedInitBreakTool(	BreakTool *	bt )
    {
    memset( bt, 0, sizeof(BreakTool) );
    }

void tedSetBreakTool(	BreakTool *	bt,
			int		breakKind )
    {
    guiSetOptionmenu( &(bt->btBreakKindOptionmenu), breakKind );
    }

void tedEnableBreakTool(	BreakTool *	bt,
				int		enabled )
    {
    guiEnableWidget( bt->btBreakKindRow, enabled );
    guiEnableOptionmenu( &(bt->btBreakKindOptionmenu), enabled );
    }

void tedEnableBreakToolOption(	BreakTool *	bt,
				int		option,
				int		enabled )
    {
    guiOptionMenuEnablePosition( &(bt->btBreakKindOptionmenu),
							option, enabled );
    }

void tedBuildBreakTool(		APP_WIDGET			parent,
				BreakTool *			bt,
				const BreakToolResources *	btr,
				OptionmenuCallback		callback,
				void *				through )
    {
    guiToolMakeLabelAndMenuRow( &(bt->btBreakKindRow),
			&(bt->btBreakKindOptionmenu),
			&(bt->btRowLabel), parent, btr->btrBreakKind,
			callback, through );
    }

void tedFillBreakToolChooser(	int				count,
				int				current,
				BreakTool *			bt,
				const BreakToolResources *	btr )
    {
    appFillInspectorMenu( count, current,
		btr->btrBreakKindMenuTexts, &(bt->btBreakKindOptionmenu) );
    }

void tedFinishBreakTool(	BreakTool *			bt )
    {
    guiOptionmenuRefreshWidth( &(bt->btBreakKindOptionmenu) );
    }
