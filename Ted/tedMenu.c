/************************************************************************/
/*									*/
/*  Ted, menus.								*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>

#   include	"tedDocMenu.h"
#   include	"tedDocument.h"

#   include	<guiMenuItem.h>
#   include	"tedMenu.h"
#   include	<appEditApplication.h>
#   include	<appGuiApplication.h>
#   include	<appEditDocument.h>
#   include	<appAppFront.h>
#   include	<appDocFront.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Application file menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppFileMenuItems[]=
{
    {
    "appFileMenuNewText",	"New",
    "appFileMenuNewKey",	"Ctrl <Key>n",
    "appFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appAppFileNew,
    },
    {
    "appFileMenuOpenText",	"Open",
    "appFileMenuOpenKey",	"Ctrl <Key>o",
    "appFileMenuOpenKeyText",	"^O",		ITEMtyOPTION, appAppFileOpen,
    },
    {
    "appFileMenuQuitText",	"Quit",
    "appFileMenuQuitKey",	(char *)0,
    "appFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appAppFileQuit,
    },
};

AppMenu TED_AppFileMenu=
    {
    "File",
    "appFileMenuText",

    "F",
    "appFileMenuMnemonic",

    0,

    TED_AppFileMenuItems,
    sizeof(TED_AppFileMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Application windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppWinMenuItems[]=
{
    {
    "appFileMenuNewText",	"New",
    "appFileMenuNewKey",	"Ctrl <Key>n",
    "appFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appAppFileNew,
    },
};

AppMenu TED_AppWinMenu=
    {
    "Window",
    "appWinMenuText",

    "W",
    "appWinMenuMnemonic",

    0,

    TED_AppWinMenuItems,
    sizeof(TED_AppWinMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Application Help Menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppHelpMenuItems[]=
{
    {
    "docHelpMenuDocumentText",	"Document",
    "docHelpMenuDocumentKey",	(char *)0,
    "docHelpMenuDocumentKeyText", (char *)0,
				ITEMtyOPTION, tedAppManual,
    },
};

AppMenu TED_AppHelpMenu=
    {
    "Help",
    "helpMenuText",

    "H",
    "helpMenuMnemonic",

    1,

    TED_AppHelpMenuItems,
    sizeof(TED_AppHelpMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document file menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFileMenuItems[]=
{
#   define	TEDmiDocFileNew			0
    {
    "docFileMenuNewText",	"New",
    "docFileMenuNewKey",	"Ctrl <Key>n",
    "docFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appDocFileNew,
    },
#   define	TEDmiDocFileOpen		1
    {
    "docFileMenuOpenText",	"Open",
    "docFileMenuOpenKey",	"Ctrl <Key>o",
    "docFileMenuOpenKeyText",	"^O",		ITEMtyOPTION, appDocFileOpen,
    },
#   define	TEDmiDocFileSave		2
    {
    "docFileMenuSaveText",	"Save",
    "docFileMenuSaveKey",	"Ctrl <Key>s",
    "docFileMenuSaveKeyText",	"^S",		ITEMtyOPTION, appDocFileSave,
    },
#   define	TEDmiDocFileSaveAs		3
    {
    "docFileMenuSaveAsText",	"Save As ...",
    "docFileMenuSaveAsKey",	(char *)0,
    "docFileMenuSaveAsKeyText",	(char *)0,	ITEMtyOPTION, appDocFileSaveAs,
    },
#   define	TEDmiDocFilePrint		4
    {
    "docFileMenuPrintText",	"Print ...",
    "docFileMenuPrintKey",	"Ctrl <Key>p",
    "docFileMenuPrintKeyText",	"^P",		ITEMtyOPTION, tedDocFilePrint,
    },
#   define	TEDmiDocFileProperties		5
    {
    "docFileMenuPropsText",	"Properties ...",
    "docFileMenuPropsKey",	(char *)0,
    "docFileMenuPropsKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileProps,
    },
#   define	TEDmiDocFileUnlock		6
    {
    "docFileMenuUnlockText",	"Unlock",
    "docFileMenuUnlockKey",	(char *)0,
    "docFileMenuUnlockKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileUnlock,
    },
#   define	TEDmiDocFileRecover		7
    {
    "docFileMenuRecoverText",	"Recover",
    "docFileMenuRecoverKey",	(char *)0,
    "docFileMenuRecoverKeyText",(char *)0,	ITEMtyOPTION, tedDocFileRecover,
    },
#   define	TEDmiDocFileClose		8
    {
    "docFileMenuCloseText",	"Close",
    "docFileMenuCloseKey",	(char *)0,
    "docFileMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },

#   define	TEDmiDocFileQuitSeparator	9
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFileQuit		10
    {
    "docFileMenuQuitText",	"Quit",
    "docFileMenuQuitKey",	(char *)0,
    "docFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appDocFileQuit,
    },
};

AppMenu TED_DocFileMenu=
    {
    "File",
    "docFileMenuText",

    "F",
    "docFileMenuMnemonic",

    0,

    TED_DocFileMenuItems,
    sizeof(TED_DocFileMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document edit menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocEditMenuItems[]=
{
#   define	TEDmiDocEditUndo	0
    {
    "docEditMenuUndoText",	"Undo",
    "docEditMenuUndoKey",	"Ctrl <Key>z",
    "docEditMenuUndoKeyText",	"^C",		ITEMtyOPTION, tedDocEditUndo,
    },
#   define	TEDmiDocEditRepeat	1
    {
    "docEditMenuRepeatText",	"Repeat",
    "docEditMenuRepeatKey",	"Ctrl <Key>y",
    "docEditMenuRepeatKeyText",	"^Y",		ITEMtyOPTION, tedDocEditRepeat,
    },
#   define	TEDmiDocEditCopy	2
    {
    "docEditMenuCopyText",	"Copy",
    "docEditMenuCopyKey",	"Ctrl <Key>c",
    "docEditMenuCopyKeyText",	"^C",		ITEMtyOPTION, appDocEditCopy,
    },
#   define	TEDmiDocEditCut		3
    {
    "docEditMenuCutText",	"Cut",
    "docEditMenuCutKey",	"Ctrl <Key>x",
    "docEditMenuCutKeyText",	"^X",		ITEMtyOPTION, appDocEditCut,
    },
#   define	TEDmiDocEditPaste	4
    {
    "docEditMenuPasteText",	"Paste",
    "docEditMenuPasteKey",	"Ctrl <Key>v",
    "docEditMenuPasteKeyText",	"^V",		ITEMtyOPTION, tedDocEditPaste,
    },
#   define	TEDmiDocEditSelectAll	5
    {
    "docEditMenuSelAllText",	"Select All",
    "docEditMenuSelAllKey",	"Ctrl <Key>a",
    "docEditMenuSelAllKeyText",	"^A",	ITEMtyOPTION, appDocEditSelAll,
    },

				    /*  6  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocEditFind	7
    {
    "docToolMenuFindText",		"Find",
    "docToolMenuFindKey",		"Ctrl <Key>f",
    "docToolMenuFindKeyText",		"^F",
					ITEMtyOPTION, tedDocToolFind,
    },
#   define	TEDmiDocEditFindNext	8
    {
    "docToolMenuFindNextText",		"Find Next",
    "docToolMenuFindNextKey",		"<Key>F3",
    "docToolMenuFindNextKeyText",	"F3",
					ITEMtyOPTION, tedDocToolFindNext,
    },
};

AppMenu TED_DocEditMenu=
    {
    "Edit",
    "docEditMenuText",

    "E",
    "docEditMenuMnemonic",

    0,

    TED_DocEditMenuItems,
    sizeof(TED_DocEditMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document edit menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocInsertMenuItems[]=
{
#   define	TEDmiDocInsertInsPict		0
    {
    "docInsertMenuInsPictText",		"Image ...",
    "docInsertMenuInsPictKey",		(char *)0,
    "docInsertMenuInsPictKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertImage,
    },
#   define	TEDmiDocInsertInsSymbol		1
    {
    "docInsertMenuInsertSymbolText",	"Symbol",
    "docInsertMenuInsertSymbolKey",	(char *)0,
    "docInsertMenuInsertSymbolKeyText",	(char *)0,
					ITEMtyOPTION, tedDocToolInsertSymbol,
    },
#   define	TEDmiDocInsertInsHyperlink	2
    {
    "docInsertMenuHyperlinkText",	"Hyperlink",
    "docInsertMenuHyperlinkKey",	"Ctrl <Key>k",
    "docInsertMenuHyperlinkKeyText",	"^K",
					ITEMtyOPTION, tedDocInsertLink,
    },
#   define	TEDmiDocInsertInsBookmark	3
    {
    "docInsertMenuBookmarkText",	"Bookmark",
    "docInsertMenuBookmarkKey",		(char *)0,
    "docInsertMenuBookmarkKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertBookmark,
    },
#   define	TEDmiDocInsertInsFootnote	4
    {
    "docInsertMenuFootnoteText",	"Footnote",
    "docInsertMenuFootnoteKey",		(char *)0,
    "docInsertMenuFootnoteKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertFootnote,
    },
#   define	TEDmiDocInsertInsEndnote	5
    {
    "docInsertMenuEndnoteText",		"Endnote",
    "docInsertMenuEndnoteKey",		(char *)0,
    "docInsertMenuEndnoteKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertEndnote,
    },
#   define	TEDmiDocInsertInsChftnsep	6
    {
    "docInsertMenuChftnsepText",	"Footnote Separator",
    "docInsertMenuChftnsepKey",		(char *)0,
    "docInsertMenuChftnsepKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertChftnsep,
    },
#   define	TEDmiDocInsertInsFile		7
    {
    "docInsertMenuInsertFileText",	"File ...",
    "docInsertMenuInsertFileKey",	(char *)0,
    "docInsertMenuInsertFileKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertFile,
    },
#   define	TEDmiDocInsertInsTable		8
    {
    "docInsertMenuInsertTableText",	"Table",
    "docInsertMenuInsertTableKey",	(char *)0,
    "docInsertMenuInsertTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableInsertTable,
    },
#   define	TEDmiDocInsertInsPageNumber	9
    {
    "docInsertMenuInsertPageNumberText",	"Page Number",
    "docInsertMenuInsertPageNumberKey",		(char *)0,
    "docInsertMenuInsertPageNumberKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageNumber,
    },

    /*  10  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocInsertInsLineBreak	11
    {
    "docInsertMenuInsertLineBreakText",	"Line Break",
    "docInsertMenuInsertLineBreakKey",		(char *)0,
    "docInsertMenuInsertLineBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertLineBreak,
    },
#   define	TEDmiDocInsertInsPageBreak	12
    {
    "docInsertMenuInsertPageBreakText",	"Page Break",
    "docInsertMenuInsertPageBreakKey",		(char *)0,
    "docInsertMenuInsertPageBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageBreak,
    },
#   define	TEDmiDocInsertInsColumnBreak	13
    {
    "docInsertMenuInsertColumnBreakText",	"Column Break",
    "docInsertMenuInsertColumnBreakKey",	(char *)0,
    "docInsertMenuInsertColumnBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertColumnBreak,
    },
#   define	TEDmiDocInsertInsSectBreak	14
    {
    "docInsertMenuInsertSectBreakText",	"Section Break",
    "docInsertMenuInsertSectBreakKey",		(char *)0,
    "docInsertMenuInsertSectBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocMenuInsertSectBreak,
    },

};

AppMenu TED_DocInsertMenu=
    {
    "Insert",
    "docInsertMenuText",

    "I",
    "docInsertMenuMnemonic",

    0,

    TED_DocInsertMenuItems,
    sizeof(TED_DocInsertMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocWinMenuItems[]=
{
    {
    "docWinMenuCloseText",	"Close",
    "docWinMenuCloseKey",	(char *)0,
    "docWinMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },
};

AppMenu TED_DocWinMenu=
    {
    "Window",
    "docWinMenuText",

    "W",
    "docWinMenuMnemonic",

    0,

    TED_DocWinMenuItems,
    sizeof(TED_DocWinMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document font menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFontMenuItems[]=
{
#   define	TEDmiDocFontBold	0
    {
    "docFontMenuBoldText",	"Bold",
    "docFontMenuBoldKey",	"Ctrl <Key>b",
    "docFontMenuBoldKeyText",	"^B",
				ITEMtyTOGGLE_OFF, tedDocFontBold,
    },
#   define	TEDmiDocFontItalic	1
    {
    "docFontMenuItalicText",	"Italic",
    "docFontMenuItalicKey",	"Ctrl <Key>i",
    "docFontMenuItalicKeyText",	"^I",
				ITEMtyTOGGLE_OFF, tedDocFontItalic,
    },
#   define	TEDmiDocFontUnderlined	2
    {
    "docFontMenuUnderlText",	"Underlined",
    "docFontMenuUnderlKey",	"Ctrl <Key>u",
    "docFontMenuUnderlKeyText",	"^U",
				ITEMtyTOGGLE_OFF, tedDocFontUnderlined,
    },
#   define	TEDmiDocFontSuperscript	3
    {
    "docFontMenuSuperscriptText",	"Superscript",
    "docFontMenuSuperscriptKey",	(char *)0,
    "docFontMenuSuperscriptKeyText",	(char *)0,
				ITEMtyTOGGLE_OFF, tedDocFontSupersub,
    },
#   define	TEDmiDocFontSubscript	4
    {
    "docFontMenuSubscriptText",		"Subscript",
    "docFontMenuSubscriptKey",		(char *)0,
    "docFontMenuSubscriptKeyText",	(char *)0,
				ITEMtyTOGGLE_OFF, tedDocFontSupersub,
    },

#   define	TEDmiDocFontToggleSeparator	5
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  6  */
    {
    "docFontMenuCopyText",	"Copy Font",
    "docFontMenuCopyKey",	(char *)0,
    "docFontMenuCopyKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontCopy,
    },
#   define	TEDmiDocFontPaste	7
    {
    "docFontMenuPasteText",	"Paste Font",
    "docFontMenuPasteKey",	(char *)0,
    "docFontMenuPasteKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontPaste,
    },

    /*  8  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  9  */
    {
    "docFontMenuToolText",	"Font Tool",
    "docFontMenuToolKey",	(char *)0,
    "docFontMenuToolKeyText",	(char *)0,	ITEMtyOPTION, tedDocToolFont,
    },
};

AppMenu TED_FontMenu=
    {
    "Font",
    "docFontMenuText",

    "n",
    "docFontMenuMnemonic",

    0,

    TED_DocFontMenuItems,
    sizeof(TED_DocFontMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document 'Table' menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocTableMenuItems[]=
{
#   define	TEDmiDocTableInsertTable	0
    {
    "docTableInsertTableText",		"Insert Table",
    "docTableInsertTableKey",		(char *)0,
    "docTableInsertTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableInsertTable,
    },
#   define	TEDmiDocTableAddRow		1
    {
    "docTableAddRowText",		"Add Row",
    "docTableAddRowKey",		(char *)0,
    "docTableAddRowKeyText",		(char *)0,
					ITEMtyOPTION, tedDocTableAddRow,
    },
#   define	TEDmiDocTableAddColumn		2
    {
    "docTableAddColumnText",		"Add Column",
    "docTableAddColumnKey",		(char *)0,
    "docTableAddColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableAddColumn,
    },

#   define	TEDmiDocTableInsertSeparator	3
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableSelectTable	4
    {
    "docTableSelectTableText",		"Select Table",
    "docTableSelectTableKey",		(char *)0,
    "docTableSelectTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectTable,
    },
#   define	TEDmiDocTableSelectRow		5
    {
    "docTableSelectRowText",		"Select Row",
    "docTableSelectRowKey",		(char *)0,
    "docTableSelectRowKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectRow,
    },
#   define	TEDmiDocTableSelectColumn	6
    {
    "docTableSelectColumnText",		"Select Column",
    "docTableSelectColumnKey",		(char *)0,
    "docTableSelectColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectColumn,
    },

#   define	TEDmiDocTableSelectSeparator	7
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableDeleteTable	8
    {
    "docTableDeleteTableText",		"Delete Table",
    "docTableDeleteTableKey",		(char *)0,
    "docTableDeleteTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteTable,
    },
#   define	TEDmiDocTableDeleteRow		9
    {
    "docTableDeleteRowText",		"Delete Row",
    "docTableDeleteRowKey",		(char *)0,
    "docTableDeleteRowKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteRow,
    },
#   define	TEDmiDocTableDeleteColumn	10
    {
    "docTableDeleteColumnText",		"Delete Column",
    "docTableDeleteColumnKey",		(char *)0,
    "docTableDeleteColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteColumn,
    },

#   define	TEDmiDocTableDeleteSeparator	11
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableDrawGrid		12
    {
    "docTableDrawGridText",		"Draw Table Grid",
    "docTableDrawGridKey",		(char *)0,
    "docTableDrawGridKeyText",		(char *)0,
					ITEMtyTOGGLE_ON, tedDocTableDrawGrid,
    },
};

AppMenu TED_TableMenu=
    {
    "Table",
    "docTableMenuText",

    "a",
    "docTableMenuMnemonic",

    0,

    TED_DocTableMenuItems,
    sizeof(TED_DocTableMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document format menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFormatMenuItems[]=
{
#   define	TEDmiDocFormatOnePara			0
    {
    "docFormatMenuOneParaText",		"Make One Paragraph",
    "docFormatMenuOneParaKey",		(char *)0,
    "docFormatMenuOneParaKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatOnePara,
    },

    /*  1  */
#   define	TEDmiDocFormatOneParaSep		1
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  2  */
    {
    "docFormatMenuCopyRulText",		"Copy Ruler",
    "docFormatMenuCopyRulKey",		(char *)0,
    "docFormatMenuCopyRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatCopyRul,
    },

#   define	TEDmiDocFormatPasteRuler		3
    {
    "docFormatMenuPasteRulText",	"Paste Ruler",
    "docFormatMenuPasteRulKey",		(char *)0,
    "docFormatMenuPasteRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatPasteRul,
    },

#   define	TEDmiDocFormatIncIndent		4
    {
    "docFormatMenuIncreaseIndentText",	"Increase Indent",
    "docFormatMenuIncreaseIndentKey",		(char *)0,
    "docFormatMenuIncreaseIndentKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocFormatIncreaseIndent,
    },

#   define	TEDmiDocFormatDecIndent		5
    {
    "docFormatMenuDecreaseIndentText",	"Decrease Indent",
    "docFormatMenuDecreaseIndentKey",		(char *)0,
    "docFormatMenuDecreaseIndentKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocFormatDecreaseIndent,
    },

#   define	TEDmiDocFormatRulerSep		6
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFormatAlignLeft			7
    {
    "docFormatMenuAlignLeftText",	"Align Left",
    "docFormatMenuAlignLeftKey",	"Ctrl <Key>l",
    "docFormatMenuAlignLeftKeyText",	"^L",
				    ITEMtyOPTION, tedDocFormatAlignLeft,
    },

#   define	TEDmiDocFormatAlignRight		8
    {
    "docFormatMenuAlignRightText",	"Align Right",
    "docFormatMenuAlignRightKey",	"Ctrl <Key>r",
    "docFormatMenuAlignRightKeyText",	"^R",
				    ITEMtyOPTION, tedDocFormatAlignRight,
    },

#   define	TEDmiDocFormatAlignCenter		9
    {
    "docFormatMenuAlignCenterText",	"Center Text",
    "docFormatMenuAlignCenterKey",	"Ctrl <Key>e",
    "docFormatMenuAlignCenterKeyText",	"^E",
				    ITEMtyOPTION, tedDocFormatAlignCenter,
    },

#   define	TEDmiDocFormatAlignJustify		10
    {
    "docFormatMenuAlignJustifyText",	"Justify Text",
    "docFormatMenuAlignJustifyKey",	"Ctrl <Key>j",
    "docFormatMenuAlignJustifyKeyText",	"^J",
				    ITEMtyOPTION, tedDocFormatAlignJustify,
    },

#   define	TEDmiDocFormatAlignSep			11
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFormatTool			12
    {
    "docToolMenuFormatText",		"Format Tool",
    "docToolMenuFormatKey",		(char *)0,
    "docToolMenuFormatKeyText",		(char *)0,
					ITEMtyOPTION, tedDocFormatTool,
    }

};

AppMenu TED_FormatMenu=
    {
    "Format",
    "docFormatMenuText",

    "r",
    "docFormatMenuMnemonic",

    0,

    TED_DocFormatMenuItems,
    sizeof(TED_DocFormatMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document tools menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocToolsMenuItems[]=
{
#   define	TEDmiDocToolsFont		0
    {
    "docToolMenuFontText",		"Font Tool",
    "docToolMenuFontKey",		(char *)0,
    "docToolMenuFontKeyText",		(char *)0,
					ITEMtyOPTION, tedDocToolFont,
    },
#   define	TEDmiDocToolsFind		1
    {
    "docToolMenuFindText",		"Find",
    "docToolMenuFindKey",		"Ctrl <Key>f",
    "docToolMenuFindKeyText",		"^F",
					ITEMtyOPTION, tedDocToolFind,
    },
#   define	TEDmiDocToolsSpelling		2
    {
    "docToolMenuSpellText",		"Spelling",
    "docToolMenuSpellKey",		"<Key>F7",
    "docToolMenuSpellKeyText",		"F7",
					ITEMtyOPTION, tedDocToolSpell,
    },
#   define	TEDmiDocToolsPageLayout		3
    {
    "docToolMenuPageLayoutText",	"Page Layout",
    "docToolMenuPageLayoutKey",		(char *)0,
    "docToolMenuPageLayoutKeyText",	(char *)0,
					ITEMtyOPTION, tedDocShowPageTool,
    },
#   define	TEDmiDocToolsInsertSymbol	4
    {
    "docToolMenuInsertSymbolText",	"Insert Symbol",
    "docToolMenuInsertSymbolKey",	(char *)0,
    "docToolMenuInsertSymbolKeyText",	(char *)0,
					ITEMtyOPTION, tedDocToolInsertSymbol,
    },
#   define	TEDmiDocToolsFormatTool		5
    {
    "docToolMenuFormatText",		"Format Tool",
    "docToolMenuFormatKey",		(char *)0,
    "docToolMenuFormatKeyText",		(char *)0,
					ITEMtyOPTION, tedDocFormatTool,
    },
};

AppMenu TED_ToolsMenu=
    {
    "Tools",
    "docToolsMenuText",

    "T",
    "docToolsMenuMnemonic",

    0,

    TED_DocToolsMenuItems,
    sizeof(TED_DocToolsMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Document, Application help menu.					*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocHelpMenuItems[]=
{
    {
    "docWinMenuAboutText",	"About Ted",
    "docWinMenuAboutKey",	(char *)0,
    "docWinMenuAboutKeyText",	(char *)0,
				ITEMtyOPTION, appDocAbout,
    },
    {
    "docHelpMenuDocumentText",	"Document",
    "docHelpMenuDocumentKey",	(char *)0,
    "docHelpMenuDocumentKeyText", (char *)0,
				ITEMtyOPTION, tedDocManual,
    },
};

AppMenu TED_DocHelpMenu=
    {
    "Help",
    "helpMenuText",

    "H",
    "helpMenuMnemonic",

    1,

    TED_DocHelpMenuItems,
    sizeof(TED_DocHelpMenuItems)/sizeof(AppMenuItem),
    };

/************************************************************************/
/*									*/
/*  Make private menus per document.					*/
/*									*/
/*  Remember Widget pointers of the menu options to be able to turn	*/
/*  on and off when appropriate.					*/
/*									*/
/*  In this files because it uses the TEDmiDocFileNew &c defines.	*/
/*									*/
/************************************************************************/

void tedMakePrivateDocumentMenus(	EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		menubar )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    td->tdFileOpenOption=
	    TED_DocFileMenuItems[TEDmiDocFileOpen].amiOptionWidget;
    td->tdFileSaveOption=
	    TED_DocFileMenuItems[TEDmiDocFileSave].amiOptionWidget;
    td->tdFileSaveAsOption=
	    TED_DocFileMenuItems[TEDmiDocFileSaveAs].amiOptionWidget;
    td->tdFileUnlockOption=
	    TED_DocFileMenuItems[TEDmiDocFileUnlock].amiOptionWidget;
    td->tdFileRecoverOption=
	    TED_DocFileMenuItems[TEDmiDocFileRecover].amiOptionWidget;
    td->tdFilePropertiesOption=
	    TED_DocFileMenuItems[TEDmiDocFileProperties].amiOptionWidget;
    td->tdFileCloseOption=
	    TED_DocFileMenuItems[TEDmiDocFileClose].amiOptionWidget;
    td->tdFileQuitSeparator=
	    TED_DocFileMenuItems[TEDmiDocFileQuitSeparator].amiOptionWidget;
    td->tdFileQuitOption=
	    TED_DocFileMenuItems[TEDmiDocFileQuit].amiOptionWidget;

    td->tdUndoOption=
	    TED_DocEditMenuItems[TEDmiDocEditUndo].amiOptionWidget;
    td->tdRepeatOption=
	    TED_DocEditMenuItems[TEDmiDocEditRepeat].amiOptionWidget;

    td->tdCopyOption=
	    TED_DocEditMenuItems[TEDmiDocEditCopy].amiOptionWidget;
    td->tdCutOption=
	    TED_DocEditMenuItems[TEDmiDocEditCut].amiOptionWidget;
    td->tdPasteOption=
	    TED_DocEditMenuItems[TEDmiDocEditPaste].amiOptionWidget;

    td->tdInsertMenu= appMakeMenu( &(td->tdInsertMenuButton),
			&(ed->edToplevel), ea, menubar,
			&TED_DocInsertMenu, (void *)ed );

    td->tdInsPictOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPict].amiOptionWidget;
    td->tdInsFileOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsFile].amiOptionWidget;
    td->tdInsSymbolOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsSymbol].amiOptionWidget;
    td->tdInsHyperlinkOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsHyperlink].amiOptionWidget;
    td->tdInsBookmarkOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsBookmark].amiOptionWidget;
    td->tdInsInsertFootnoteOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsFootnote].amiOptionWidget;
    td->tdInsInsertEndnoteOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsEndnote].amiOptionWidget;
    td->tdInsInsertChftnsepOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsChftnsep].amiOptionWidget;
    td->tdInsInsertTableOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsTable].amiOptionWidget;
    td->tdInsInsertPageNumberOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPageNumber].amiOptionWidget;
    td->tdInsInsertLineBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsLineBreak].amiOptionWidget;
    td->tdInsInsertPageBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPageBreak].amiOptionWidget;
    td->tdInsInsertColumnBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsColumnBreak].amiOptionWidget;
    td->tdInsInsertSectBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsSectBreak].amiOptionWidget;

    td->tdFontMenu= appMakeMenu( &(td->tdFontMenuButton),
			&(ed->edToplevel), ea, menubar,
			&TED_FontMenu, (void *)ed );

    td->tdFontBoldOption=
	    TED_DocFontMenuItems[TEDmiDocFontBold].amiOptionWidget;
    td->tdFontItalicOption=
	    TED_DocFontMenuItems[TEDmiDocFontItalic].amiOptionWidget;
    td->tdFontUnderlinedOption=
	    TED_DocFontMenuItems[TEDmiDocFontUnderlined].amiOptionWidget;
    td->tdFontSuperscriptOption=
	    TED_DocFontMenuItems[TEDmiDocFontSuperscript].amiOptionWidget;
    td->tdFontSubscriptOption=
	    TED_DocFontMenuItems[TEDmiDocFontSubscript].amiOptionWidget;
    td->tdFontToggleSeparator=
	    TED_DocFontMenuItems[TEDmiDocFontToggleSeparator].amiOptionWidget;
    td->tdFontPasteOption=
	    TED_DocFontMenuItems[TEDmiDocFontPaste].amiOptionWidget;

    td->tdTableMenu= appMakeMenu( &(td->tdTableMenuButton),
			&(ed->edToplevel), ea, menubar,
			&TED_TableMenu, (void *)ed );

    td->tdTabInsertTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableInsertTable].amiOptionWidget;
    td->tdTabAddRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddRow].amiOptionWidget;
    td->tdTabAddColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddColumn].amiOptionWidget;
    td->tdTabInsertSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableInsertSeparator].amiOptionWidget;

    td->tdTabSelectTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectTable].amiOptionWidget;
    td->tdTabSelectRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectRow].amiOptionWidget;
    td->tdTabSelectColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectColumn].amiOptionWidget;
    td->tdTabSelectSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableSelectSeparator].amiOptionWidget;

    td->tdTabDeleteTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteTable].amiOptionWidget;
    td->tdTabDeleteRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteRow].amiOptionWidget;
    td->tdTabDeleteColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteColumn].amiOptionWidget;
    td->tdTabDeleteSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteSeparator].amiOptionWidget;

    td->tdDrawTableGridOption=
	    TED_DocTableMenuItems[TEDmiDocTableDrawGrid].amiOptionWidget;

    td->tdFormatMenu= appMakeMenu( &(td->tdFormatMenuButton),
			&(ed->edToplevel), ea, menubar,
			&TED_FormatMenu, (void *)ed );

    td->tdFormatOneParaOption=
	    TED_DocFormatMenuItems[TEDmiDocFormatOnePara].amiOptionWidget;
    td->tdFormatOneParaSeparator=
	    TED_DocFormatMenuItems[TEDmiDocFormatOneParaSep].amiOptionWidget;
    td->tdFormatPasteRulerOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatPasteRuler].amiOptionWidget;
    td->tdFormatIncreaseIndentOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatIncIndent].amiOptionWidget;
    td->tdFormatDecreaseIndentOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatDecIndent].amiOptionWidget;
    td->tdFormatRulerSeparator= 
	    TED_DocFormatMenuItems[TEDmiDocFormatRulerSep].amiOptionWidget;

    td->tdFormatAlignLeftOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignLeft].amiOptionWidget;
    td->tdFormatAlignRightOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignRight].amiOptionWidget;
    td->tdFormatAlignCenterOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignCenter].amiOptionWidget;
    td->tdFormatAlignJustifyOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignJustify].amiOptionWidget;
    td->tdFormatAlignSeparator= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignSep].amiOptionWidget;

    td->tdToolsMenu= appMakeMenu( &(td->tdToolsMenuButton),
			&(ed->edToplevel), ea, menubar,
			&TED_ToolsMenu, (void *)ed );

    td->tdToolsFontOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsFont].amiOptionWidget;
    td->tdToolsFindOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsFind].amiOptionWidget;
    td->tdToolsFormatToolOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsFormatTool].amiOptionWidget;
    td->tdToolsSpellingOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsSpelling].amiOptionWidget;
    td->tdToolsPageLayoutOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsPageLayout].amiOptionWidget;
    td->tdToolsSymbolOption=
	    TED_DocToolsMenuItems[TEDmiDocToolsInsertSymbol].amiOptionWidget;

    ed->edFileCloseOption= td->tdFileCloseOption;

    return;
    }

