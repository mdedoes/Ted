/************************************************************************/
/*									*/
/*  Implementation of a find/replace tool.				*/
/*									*/
/************************************************************************/

#   ifndef		APP_FIND_TOOL_IMPL_H
#   define		APP_FIND_TOOL_IMPL_H

#   include	<guiBase.h>
#   include	"appFindTool.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appFindToolReflectPatternText(	FindTool *	ft );

extern APP_TXTYPING_CALLBACK_H( appFindToolPatternChanged, w, voidaft );

extern APP_TOGGLE_CALLBACK_H( appFindAsWordToggled, w, voidaft, voidtbcs );

extern APP_TOGGLE_CALLBACK_H( appFindCaseSensitiveToggled, w, voidaft, voidtbcs );

extern APP_TOGGLE_CALLBACK_H( appFindRegexToggled, w, voidaft, voidtbcs );

extern APP_BUTTON_CALLBACK_H( appFindToolFindNextPushed, w, voidft );

extern APP_BUTTON_CALLBACK_H( appFindToolFindPrev, w, voidft );

extern void appFindToolReflectReplaceText(	FindTool *	ft );

extern APP_TXTYPING_CALLBACK_H( appFindToolReplacementChanged, w, voidaft );

extern APP_BUTTON_CALLBACK_H( appFindToolReplaceSelectionPushed, w, voidft );

extern APP_BUTTON_CALLBACK_H( appFindToolReplaceNext, w, voidft );

#   endif	/*	APP_FIND_TOOL_IMPL_H	*/
