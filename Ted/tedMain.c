/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<appSystem.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	<appSymbolPicker.h>
#   include	<appSpellTool.h>

#   include	<appGeoString.h>
#   include	<appPaper.h>

#   include	<sioStdio.h>
#   include	<sioMemory.h>
#   include	<sioBase64.h>

#   include	<appDebugon.h>

static AppConfigurableResource TEDApplicationResourceTable[]=
    {
	/****************************************************************/
	/*  Application defaults.					*/
	/*  2)  "magnification" is duplicate. It exists for the		*/
	/*	application and for the font chooser tool.		*/
	/****************************************************************/
	APP_RESOURCE( "fileFileNotRtf",
		    offsetof(TedAppResources,tarFileNotRtf),
		    "File is not in Rich Text Format." ),
	APP_RESOURCE( "headFootNotReachable",
		offsetof(TedAppResources,tarHeadFootNotReachable),
		"There are not enough pages in the section to edit "
		"this Header/Footer." ),

	/********************************/
	/*  Application file menu.	*/
	/*  Application windows menu.	*/
	/********************************/
	APP_RESOURCE( "appFileMenuText",
		    offsetof(TedAppResources,tarAppFileMenuText),
		    "File" ),
	APP_RESOURCE( "appWinMenuText",
		    offsetof(TedAppResources,tarAppWinMenuText),
		    "Window" ),

	/********************************/
	/*  Document file menu.		*/
	/*  Document edit menu.		*/
	/*  Document windows menu.	*/
	/*  Document Font menu.		*/
	/*  Document Format menu.	*/
	/*  Document Table menu.	*/
	/*  Document Tool menu.		*/
	/*  Document Help menu.		*/
	/********************************/
	APP_RESOURCE( "docFileMenuText",
		    offsetof(TedAppResources,tarDocFileMenuText),
		    "File" ),
	APP_RESOURCE( "docEditMenuText",
		    offsetof(TedAppResources,tarDocEditMenuText),
		    "Edit" ),
	APP_RESOURCE( "docInsertMenuText",
		    offsetof(TedAppResources,tarDocInsertMenuText),
		    "Insert" ),
	APP_RESOURCE( "docWinMenuText",
		    offsetof(TedAppResources,tarDocWinMenuText),
		    "Window" ),
	APP_RESOURCE( "docFontMenuText",
		    offsetof(TedAppResources,tarDocFontMenuText),
		    "Font" ),
	APP_RESOURCE( "docFormatMenuText",
		    offsetof(TedAppResources,tarDocFormatMenuText),
		    "Format" ),
	APP_RESOURCE( "docTableMenuText",
		    offsetof(TedAppResources,tarDocTableMenuText),
		    "Table" ),
	APP_RESOURCE( "docToolsMenuText",
		    offsetof(TedAppResources,tarDocToolMenuText),
		    "Tools" ),
	APP_RESOURCE( "helpMenuText",
		    offsetof(TedAppResources,tarHelpMenuText),
		    "Help" ),

	/**/
#	ifdef __VMS
	APP_RESOURCE( "documentFileName",
		    offsetof(TedAppResources,tarAppHelpFileName),
		    "TED$ROOT:[Ted]TedDocument-en_US.rtf" ),
#	else
	APP_RESOURCE( "documentFileName",
		    offsetof(TedAppResources,tarAppHelpFileName),
		    DOCUMENT_DIR "TedDocument-en_US.rtf" ),
#	endif

	/**/
	APP_RESOURCE( "selectionColor",
		    offsetof(TedAppResources,tarSelectionColor),
		    "LightSteelBlue" ),
	APP_RESOURCE( "copiedSelectionColor",
		    offsetof(TedAppResources,tarCopiedSelectionColor),
		    "gray70" ),

	/**/
	APP_RESOURCE( "defaultAnsicpg",
		    offsetof(TedAppResources,tarDefaultAnsicpgString),
		    "1252" ),

	/**/
	APP_RESOURCE( "showTableGrid",
		    offsetof(TedAppResources,tarShowTableGridString),
		    "1" ),
    };


static AppConfigurableResource TEDFileMessageResourceTable[]=
    {
	/********************************/
	/*  Format for 'New' title	*/
	/********************************/
	APP_RESOURCE( "namelessFormat",
		    offsetof(AppFileMessageResources,afmrNamelessTitleFormat),
		    "Nameless %d" ),

	/********************************/
	/*  Really Close? Dialog.	*/
	/********************************/
	APP_RESOURCE( "reallyCloseQuestion",
		    offsetof(AppFileMessageResources,afmrReallyCloseQuestion),
		    "Do you want to save the changes you made to this text?" ),
	APP_RESOURCE( "reallyCloseSave",
		    offsetof(AppFileMessageResources,afmrReallyCloseSaveIt),
		    "Yes" ),
	APP_RESOURCE( "reallyCloseDontSave",
		    offsetof(AppFileMessageResources,afmrReallyCloseDontSave),
		    "No" ),
	APP_RESOURCE( "reallyCloseCancel",
		    offsetof(AppFileMessageResources,afmrReallyCloseCancel),
		    "Cancel" ),

	/********************************/
	/*  Really Quit? Dialog.	*/
	/********************************/
	APP_RESOURCE( "reallyQuitQuestion",
		    offsetof(AppFileMessageResources,afmrReallyQuitQuestion),
		    "Do you want to save changes to texts?" ),
	APP_RESOURCE( "reallyQuitReview",
		    offsetof(AppFileMessageResources,afmrReallyQuitReview),
		    "Yes" ),
	APP_RESOURCE( "reallyQuitQuitAnyway",
		    offsetof(AppFileMessageResources,afmrReallyQuitAnyway),
		    "No" ),
	APP_RESOURCE( "reallyQuitCancel",
		    offsetof(AppFileMessageResources,afmrReallyQuitCancel),
		    "Cancel" ),

	/********************************/
	/*  Messages about  a file.	*/
	/********************************/
	APP_RESOURCE( "fileFileNoAccess",
		    offsetof(AppFileMessageResources,afmrFileNoAccess),
		    "Cannot open file." ),
	APP_RESOURCE( "fileFileReadOnly",
		    offsetof(AppFileMessageResources,afmrFileReadOnly),
		    "File is Readonly." ),
	APP_RESOURCE( "fileNoSuchFile",
		    offsetof(AppFileMessageResources,afmrNoSuchFileMessage),
		    "This file does not exist." ),
	APP_RESOURCE( "fileNotWritable",
		    offsetof(AppFileMessageResources,afmrFileNotWritable),
		    "You cannot write to this file." ),
	APP_RESOURCE( "fileMakeIt",
		    offsetof(AppFileMessageResources,afmrMakeItQuestion),
		    "There is no such file. Make it?" ),

    };

static TedAppResources			TEDResources;

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

static int tedOpenDocument(	EditApplication *	ea,
				void *			voidtd,
				int *			pFormat,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const char *		filename	)
    {
    TedDocument *	td= (TedDocument *)voidtd;

    if  ( tedOpenDocumentFile( ea, pFormat, &(td->tdDocument),
					    filename, relative, option ) )
	{ /*SDEB(filename);*/ return -1;	}

    return 0;
    }

static void tedDocFilePrint(	APP_WIDGET	printOption,
				void *		voided,
				void *		call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    BufferDocument *	bd= td->tdDocument;

    int			pageCount= bd->bdItem.biBelowPosition.lpPage+ 1;
    int			firstPage= -1;
    int			lastPage= -1;

    DocumentSelection	ds;
    SelectionGeometry	sg;

    const DocumentGeometry *	dgDoc= &(bd->bdProperties.dpGeometry);

    if  ( ! tedGetSelection( &ds, &sg, td )	)
	{
	firstPage= sg.sgBegin.pgTopPosition.lpPage;
	lastPage= sg.sgEnd.pgTopPosition.lpPage;
	}

    appRunPrintDialog( ed, dgDoc, pageCount, firstPage, lastPage,
						    printOption, "print" );

    tedRedoDocumentLayout( ed );
    }

/************************************************************************/
/*									*/
/*  Display the online manual.						*/
/*									*/
/************************************************************************/

static APP_DESTROY_CALLBACK_H( tedManualDestroyed, w, voidtar )
    {
    TedAppResources *	tar= (TedAppResources *)voidtar;

    tar->tarManualDocument= (EditDocument *)0;
    }

static void tedManual(		APP_WIDGET		option,
				EditApplication *	ea,
				void *			voidpbcs,
				APP_WIDGET		relative )
    {
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    const int		read_only= 1;
    const char *	file= TEDResources.tarAppHelpFileName;

    if  ( tar->tarManualDocument )
	{ appMakeDocVisible( ea, tar->tarManualDocument );	}
    else{
	tar->tarManualDocument= appOpenDocument( ea, relative,
						    option, read_only, file );

	if  ( ! tar->tarManualDocument )
	    { SXDEB(file,tar->tarManualDocument); return;	}

	appSetDestroyCallback( tar->tarManualDocument->edToplevel.atTopWidget,
					tedManualDestroyed, (void *)tar );
	}

    return;
    }

static void tedDocManual(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    tedManual( option, ea, voidpbcs, ed->edToplevel.atTopWidget );
    }

static void tedAppManual(	APP_WIDGET	option,
				void *		voidea,
				void *		voidpbcs )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    tedManual( option, ea, voidpbcs, ea->eaToplevel.atTopWidget );
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

static void tedDocTableInsertTable(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;

    int			rows= 2;
    int			columns= 2;

    if  ( tedInsertTable( ed, rows, columns ) )
	{ LLDEB(rows,columns);	}

    appDocumentChanged( ed, 1 );

    return;
    }

static void tedDocTableAddRow(		APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedAppendRowToTable( ed );

    appDocumentChanged( ed, 1 );

    return;
    }

static void tedDocTableAddColumn(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( tedAppendColumnToTable( ed ) )
	{ LDEB(1);	}

    appDocumentChanged( ed, 1 );
    }

static void tedDocFormatTool(	APP_WIDGET	option,
				void *		voided,
				void *		voidcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    tedShowFormatTool( option, ea, "tedFormatTool", "tedtable" );

    tedAdaptFormatToolToDocument( ed );
    }

static void tedDocShowPageTool(		APP_WIDGET	pageOption,
					void *		voided,
					void *		voidcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

#   if 0

    Use the page layout page of the format tool instead

    if  ( ! ea->eaPageTool )
	{
	char *			pixmapName= "page";

	ea->eaPageTool= appMakePageTool( ea, pageOption, pixmapName );
	}
    else{ appShowPageTool( ea ); }

    tedAdaptPageToolToDocument( ea, ed );
#   else

    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedDocFormatTool( td->tdToolsFormatToolOption, voided, voidcbs );

    tedFormatShowPagePage( ea );
    }

#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Font Menu.								*/
/*									*/
/************************************************************************/

static void tedDocFontPaste(	APP_WIDGET	fontsOption,
				void *		voided,
				void *		voidpbcbs	 )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "FONT" );
    }

static void tedDocToolFont(	APP_WIDGET	fontsOption,
				void *		voided,
				void *		voidcbs	 )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedDocFormatTool( td->tdToolsFormatToolOption, voided, voidcbs );

    tedFormatShowFontPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Run document properties dialog.					*/
/*									*/
/************************************************************************/

static void tedDocFileProps(	APP_WIDGET	option,
				void *		voided,
				void *		call_data	 )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentStatistics		ds;

    docCollectDocumentStatistics( &ds, bd );

    tedRunPropertyDialog( ed, &ds, option, "props" );
    }

/************************************************************************/
/*									*/
/*  Mail a document.							*/
/*									*/
/************************************************************************/

static void tedDocFileMail(	APP_WIDGET	mailOption,
				void *		voided,
				void *		call_data	 )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    const unsigned char *	subject= dp->dpSubject;

    if  ( ! subject )
	{ subject= dp->dpTitle;	}

    appRunMailDialog( ea, ed, mailOption, "mail", subject, (void *)bd );
    }

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
    "appFileMenuMiniText",	"Minimize",
    "appFileMenuMiniKey",	"Ctrl <Key>m",
    "appFileMenuMiniKeyText",	"^M",		ITEMtyOPTION, appAppFileMini,
    },
    {
    "appFileMenuHideText",	"Hide",
    "appFileMenuHideKey",	"Ctrl <Key>h",
    "appFileMenuHideKeyText",	"^H",		ITEMtyOPTION, appAppFileHide,
    },
    {
    "appFileMenuQuitText",	"Quit",
    "appFileMenuQuitKey",	(char *)0,
    "appFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appAppFileQuit,
    },
};

/************************************************************************/
/*									*/
/*  Application windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppWinMenuItems[]=
{
    {
    "appWinMenuMiniText",	"Minimize",
    "appWinMenuMiniKey",	"Ctrl <Key>m",
    "appWinMenuMiniKeyText",	"^M",		ITEMtyOPTION, appAppFileMini,
    },
    {
    "appWinMenuHideText",	"Hide",
    "appWinMenuHideKey",	"Ctrl <Key>h",
    "appWinMenuHideKeyText",	"^H",		ITEMtyOPTION, appAppFileHide,
    },
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
#   define	TEDmiDocFileMail		5
    {
    "docFileMenuMailText",	"Mail ...",
    "docFileMenuMailKey",	(char *)0,
    "docFileMenuMailKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileMail,
    },
#   define	TEDmiDocFileProperties		6
    {
    "docFileMenuPropsText",	"Properties ...",
    "docFileMenuPropsKey",	(char *)0,
    "docFileMenuPropsKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileProps,
    },
#   define	TEDmiDocFileMinimize		7
    {
    "docFileMenuMiniText",	"Minimize",
    "docFileMenuMiniKey",	"Ctrl <Key>m",
    "docFileMenuMiniKeyText",	"^M",		ITEMtyOPTION, appDocFileMini,
    },
#   define	TEDmiDocFileHide		8
    {
    "docFileMenuHideText",	"Hide",
    "docFileMenuHideKey",	"Ctrl <Key>h",
    "docFileMenuHideKeyText",	"^H",		ITEMtyOPTION, appDocFileHide,
    },
#   define	TEDmiDocFileClose		9
    {
    "docFileMenuCloseText",	"Close",
    "docFileMenuCloseKey",	(char *)0,
    "docFileMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },

#   define	TEDmiDocFileQuitSeparator	10
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFileQuit		11
    {
    "docFileMenuQuitText",	"Quit",
    "docFileMenuQuitKey",	(char *)0,
    "docFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appDocFileQuit,
    },
};

/************************************************************************/
/*									*/
/*  Document edit menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocEditMenuItems[]=
{
#   define	TEDmiDocEditCopy	0
    {
    "docEditMenuCopyText",	"Copy",
    "docEditMenuCopyKey",	"Ctrl <Key>c",
    "docEditMenuCopyKeyText",	"^C",		ITEMtyOPTION, appDocEditCopy,
    },
#   define	TEDmiDocEditCut		1
    {
    "docEditMenuCutText",	"Cut",
    "docEditMenuCutKey",	"Ctrl <Key>x",
    "docEditMenuCutKeyText",	"^X",		ITEMtyOPTION, appDocEditCut,
    },
#   define	TEDmiDocEditPaste	2
    {
    "docEditMenuPasteText",	"Paste",
    "docEditMenuPasteKey",	"Ctrl <Key>v",
    "docEditMenuPasteKeyText",	"^V",		ITEMtyOPTION, tedDocEditPaste,
    },
#   define	TEDmiDocEditSelectAll	3
    {
    "docEditMenuSelAllText",	"Select All",
    "docEditMenuSelAllKey",	"Ctrl <Key>a",
    "docEditMenuSelAllKeyText",	"^A",	ITEMtyOPTION, appDocEditSelAll,
    },

    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocEditFind	5
    {
    "docToolMenuFindText",		"Find",
    "docToolMenuFindKey",		"Ctrl <Key>f",
    "docToolMenuFindKeyText",		"^F",
					ITEMtyOPTION, tedDocToolFind,
    },
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
    "docInsertMenuInsPictText",		"Picture ...",
    "docInsertMenuInsPictKey",		(char *)0,
    "docInsertMenuInsPictKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertPicture,
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
    "docInsertMenuHyperlinkText",	"Hyperlink ...",
    "docInsertMenuHyperlinkKey",	"Ctrl <Key>k",
    "docInsertMenuHyperlinkKeyText",	"^K",
					ITEMtyOPTION, tedDocInsertLink,
    },
#   define	TEDmiDocInsertInsBookmark	3
    {
    "docInsertMenuBookmarkText",	"Bookmark ...",
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
#   define	TEDmiDocInsertInsChftnsep	5
    {
    "docInsertMenuChftnsepText",	"Footnote Separator",
    "docInsertMenuChftnsepKey",		(char *)0,
    "docInsertMenuChftnsepKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertChftnsep,
    },
#   define	TEDmiDocInsertInsFile		6
    {
    "docInsertMenuInsertFileText",	"File ...",
    "docInsertMenuInsertFileKey",	(char *)0,
    "docInsertMenuInsertFileKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertFile,
    },
#   define	TEDmiDocInsertInsTable		7
    {
    "docInsertMenuInsertTableText",	"Table",
    "docInsertMenuInsertTableKey",	(char *)0,
    "docInsertMenuInsertTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableInsertTable,
    },
#   define	TEDmiDocInsertInsPageNumber	8
    {
    "docInsertMenuInsertPageNumberText",	"Page Number",
    "docInsertMenuInsertPageNumberKey",		(char *)0,
    "docInsertMenuInsertPageNumberKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageNumber,
    },

    /*  9  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocInsertInsLineBreak	10
    {
    "docInsertMenuInsertLineBreakText",	"Line Break",
    "docInsertMenuInsertLineBreakKey",		(char *)0,
    "docInsertMenuInsertLineBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertLineBreak,
    },
#   define	TEDmiDocInsertInsPageBreak	11
    {
    "docInsertMenuInsertPageBreakText",	"Page Break",
    "docInsertMenuInsertPageBreakKey",		(char *)0,
    "docInsertMenuInsertPageBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageBreak,
    },
#   define	TEDmiDocInsertInsSectBreak	12
    {
    "docInsertMenuInsertSectBreakText",	"Section Break",
    "docInsertMenuInsertSectBreakKey",		(char *)0,
    "docInsertMenuInsertSectBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertSectBreak,
    },

};

/************************************************************************/
/*									*/
/*  Document windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocWinMenuItems[]=
{
    {
    "docWinMenuMiniText",	"Minimize",
    "docWinMenuMiniKey",	"Ctrl <Key>m",
    "docWinMenuMiniKeyText",	"^M",		ITEMtyOPTION, appDocFileMini,
    },
    {
    "docWinMenuHideText",	"Hide",
    "docWinMenuHideKey",	"Ctrl <Key>h",
    "docWinMenuHideKeyText",	"^H",		ITEMtyOPTION, appDocFileHide,
    },
    {
    "docWinMenuCloseText",	"Close",
    "docWinMenuCloseKey",	(char *)0,
    "docWinMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },
};

/************************************************************************/
/*  Document font menu.							*/
/************************************************************************/

static void tedDocFontCopy(	APP_WIDGET	fontsOption,
				void *		voided,
				void *		voidpbcbs );

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

    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    {
    "docFontMenuCopyText",	"Copy Font",
    "docFontMenuCopyKey",	(char *)0,
    "docFontMenuCopyKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontCopy,
    },
    {
    "docFontMenuPasteText",	"Paste Font",
    "docFontMenuPasteKey",	(char *)0,
    "docFontMenuPasteKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontPaste,
    },

    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    {
    "docFontMenuToolText",	"Font Tool",
    "docFontMenuToolKey",	(char *)0,
    "docFontMenuToolKeyText",	(char *)0,	ITEMtyOPTION, tedDocToolFont,
    },
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

    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableDrawGrid		8
    {
    "docTableDrawGridText",		"Draw Table Grid",
    "docTableDrawGridKey",		(char *)0,
    "docTableDrawGridKeyText",		(char *)0,
					ITEMtyTOGGLE_ON, tedDocTableDrawGrid,
    },
};

/************************************************************************/
/*									*/
/*  Document format menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFormatMenuItems[]=
{
#   define	TEDmiDocFormatOnePara		0
    {
    "docFormatMenuOneParaText",		"Make One Paragraph",
    "docFormatMenuOneParaKey",		(char *)0,
    "docFormatMenuOneParaKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatOnePara,
    },

    /*  1  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  2  */
    {
    "docFormatMenuCopyRulText",		"Copy ruler",
    "docFormatMenuCopyRulKey",		(char *)0,
    "docFormatMenuCopyRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatCopyRul,
    },
    /*  3  */
    {
    "docFormatMenuPasteRulText",	"Paste ruler",
    "docFormatMenuPasteRulKey",		(char *)0,
    "docFormatMenuPasteRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatPasteRul,
    },

    /*  4  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFormatTool		5
    {
    "docToolMenuFormatText",		"Format Tool",
    "docToolMenuFormatKey",		(char *)0,
    "docToolMenuFormatKeyText",		(char *)0,
					ITEMtyOPTION, tedDocFormatTool,
    }

};

/************************************************************************/
/*									*/
/*  Document tools menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocToolMenuItems[]=
{
#   define	TEDmiDocToolsFontTool		0
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

static AppMenuItem TED_AppHelpMenuItems[]=
{
    {
    "docHelpMenuDocumentText",	"Document",
    "docHelpMenuDocumentKey",	(char *)0,
    "docHelpMenuDocumentKeyText", (char *)0,
				ITEMtyOPTION, tedAppManual,
    },
};

/************************************************************************/
/*									*/
/*  Make a document read_only.						*/
/*									*/
/************************************************************************/

void tedMakeDocumentReadonly(	EditDocument *	ed )
    {
#   ifdef USE_MOTIF
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    WidgetList		children;
    Cardinal		childCount= 0;

    XtVaGetValues( ed->edFileMenu,
			    XmNchildren,		&children,
			    XmNnumChildren,		&childCount,
			    NULL );

    XtUnmanageChild( children[TEDmiDocFileNew] );
    XtUnmanageChild( children[TEDmiDocFileOpen] );
    XtUnmanageChild( children[TEDmiDocFileSave] );
    XtUnmanageChild( children[TEDmiDocFileSaveAs] );
    XtUnmanageChild( children[TEDmiDocFileMail] );
    XtUnmanageChild( children[TEDmiDocFileProperties] );
    XtUnmanageChild( children[TEDmiDocFileQuitSeparator] );
    XtUnmanageChild( children[TEDmiDocFileQuit] );

    XtVaGetValues( ed->edEditMenu,
			    XmNchildren,		&children,
			    XmNnumChildren,		&childCount,
			    NULL );

    XtUnmanageChild( children[TEDmiDocEditCut] );
    XtUnmanageChild( children[TEDmiDocEditPaste] );

    XtUnmanageChild( td->tdInsPictOption );
    XtUnmanageChild( td->tdInsSymbolOption );
    XtUnmanageChild( td->tdInsFileOption );
    XtUnmanageChild( td->tdInsInsertFootnoteOption );
    XtUnmanageChild( td->tdInsInsertChftnsepOption );
    XtUnmanageChild( td->tdInsInsertTableOption );
    XtUnmanageChild( td->tdInsInsertPageNumberOption );

    XtUnmanageChild( td->tdInsInsertLineBreakOption );
    XtUnmanageChild( td->tdInsInsertPageBreakOption );
    XtUnmanageChild( td->tdInsInsertSectBreakOption );

    XtUnmanageChild( td->tdFontMenuButton );
    XtUnmanageChild( td->tdTableMenuButton );
    XtUnmanageChild( td->tdFormatMenuButton );

    XtVaGetValues( td->tdToolsMenu,
			    XmNchildren,		&children,
			    XmNnumChildren,		&childCount,
			    NULL );

    XtUnmanageChild( children[TEDmiDocToolsFontTool] );
    XtUnmanageChild( children[TEDmiDocToolsSpelling] );
    XtUnmanageChild( children[TEDmiDocToolsPageLayout] );
    XtUnmanageChild( children[TEDmiDocToolsInsertSymbol] );
    XtUnmanageChild( children[TEDmiDocToolsFormatTool] );

    XtUnmanageChild( ed->edHelpMenuButton );

    td->tdDrawTableGrid= 0;
#   endif
    }

/************************************************************************/
/*									*/
/*  Make private menus per document.					*/
/*									*/
/*  Remember Widget pointers of the menu options to be able to turn	*/
/*  on and off when appropriate.					*/
/*									*/
/************************************************************************/

static void tedMakePrivateDocumentMenus( EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		menubar )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    td->tdCopyWidget=
	    TED_DocEditMenuItems[TEDmiDocEditCopy].amiOptionWidget;
    td->tdCutWidget=
	    TED_DocEditMenuItems[TEDmiDocEditCut].amiOptionWidget;
    td->tdPasteWidget=
	    TED_DocEditMenuItems[TEDmiDocEditPaste].amiOptionWidget;

    td->tdInsertMenu= appMakeMenu( &(td->tdInsertMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocInsertMenuText, 0,
			TED_DocInsertMenuItems,
			sizeof(TED_DocInsertMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

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
    td->tdInsInsertSectBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsSectBreak].amiOptionWidget;

    td->tdFontMenu= appMakeMenu( &(td->tdFontMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocFontMenuText, 0,
			TED_DocFontMenuItems,
			sizeof(TED_DocFontMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

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

    td->tdTableMenu= appMakeMenu( &(td->tdTableMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocTableMenuText, 0,
			TED_DocTableMenuItems,
			sizeof(TED_DocTableMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdTabInsertTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableInsertTable].amiOptionWidget;
    td->tdTabAddRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddRow].amiOptionWidget;
    td->tdTabAddColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddColumn].amiOptionWidget;

    td->tdSelectTableWidget=
	    TED_DocTableMenuItems[TEDmiDocTableSelectTable].amiOptionWidget;
    td->tdSelectRowWidget=
	    TED_DocTableMenuItems[TEDmiDocTableSelectRow].amiOptionWidget;
    td->tdSelectColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectColumn].amiOptionWidget;
    td->tdDrawTableGridOption=
	    TED_DocTableMenuItems[TEDmiDocTableDrawGrid].amiOptionWidget;

    td->tdFormatMenu= appMakeMenu( &(td->tdFormatMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocFormatMenuText, 0,
			TED_DocFormatMenuItems,
			sizeof(TED_DocFormatMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdFormatOneParaOption=
	    TED_DocFormatMenuItems[TEDmiDocFormatOnePara].amiOptionWidget;

    td->tdToolsMenu= appMakeMenu( &(td->tdToolsMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocToolMenuText, 0,
			TED_DocToolMenuItems,
			sizeof(TED_DocToolMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdToolsFormatToolOption=
	    TED_DocToolMenuItems[TEDmiDocToolsFormatTool].amiOptionWidget;

    return;
    }

/************************************************************************/
/*									*/
/*  Turn tools on/off depending on the number of visible documents.	*/
/*									*/
/************************************************************************/

static void tedVisibleDocumentCountChanged(	EditApplication *	ea,
						int			from,
						int			to )
    {
    if  ( from == 0 && to > 0 )
	{
	/* No !
	if  ( tar->tarInspector )
	    { appEnableInspector( tar->tarInspector, 1 ); }
	*/

	if  ( ea->eaSymbolPicker )
	    { appEnableSymbolPicker( ea->eaSymbolPicker, 1 ); }
	if  ( ea->eaSpellTool )
	    { appEnableSpellTool( ea->eaSpellTool, 1 ); }
	if  ( ea->eaFindTool )
	    { appEnableFindTool( ea->eaFindTool, 1 ); }
	if  ( ea->eaPageTool )
	    { appEnablePageTool( ea->eaPageTool, 1 ); }
	}

    if  ( from > 0 && to == 0 )
	{
	TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

	if  ( tar->tarInspector )
	    { appEnableInspector( tar->tarInspector, 0 ); }

	if  ( ea->eaSymbolPicker )
	    { appEnableSymbolPicker( ea->eaSymbolPicker, 0 ); }
	if  ( ea->eaSpellTool )
	    { appEnableSpellTool( ea->eaSpellTool, 0 ); }
	if  ( ea->eaFindTool )
	    { appEnableFindTool( ea->eaFindTool, 0 ); }
	if  ( ea->eaPageTool )
	    { appEnablePageTool( ea->eaPageTool, 0 ); }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Ted, description of the application.				*/
/*									*/
/************************************************************************/

static AppFileExtension	TedFileExtensions[]=
{
    { "rtfFiles",	"*.rtf",
			"Rich Text Files ( *.rtf )",
			"rtf"	 ,
			( APPFILE_CAN_OPEN|APPFILE_CAN_SAVE ),
			},
    { "txtFiles",	"*.txt",
			"Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_OPEN ),
			},
    { "txtFilesFolded",	"*.txt",
			"Folded Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "txtFilesWide",	"*.txt",
			"Wide Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "htmlFiles",	"*.html",
			"HTML Files ( *.html )",
			"html",
			( APPFILE_CAN_SAVE ),
    },
    { "allFiles",	"*",
			"All Files ( *.* )",
			(char *)0,
			( APPFILE_CAN_OPEN ),
    },
};

static int tedWriteRtfMail(	SimpleOutputStream *		sos,
				const char *			mimeBoundary,
				void *				voidbd )
    {
    SimpleOutputStream *	sosBody;
    const int			saveBookmarks= 1;
    int				rval= 0;

    BufferDocument *		bd= (BufferDocument *)voidbd;
    const DocumentProperties *	dp= &(bd->bdProperties);

    const char *		file= (const char *)dp->dpFilename;

    if  ( file )
	{
	const char *		relative;

	relative= strrchr( file, '/' );
	if  ( relative )
	    { file= relative+ 1;	}
	}

    if  ( ! file || ! file[0] )
	{ file= "file.rtf";	}

    sosBody= sos;

    if  ( mimeBoundary )
	{
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Type: text/plain", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Transfer-Encoding: 7bit", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Type: ", sos );
	if  ( file && file[0] )
	    {
	    sioOutPutString( "application/rtf; name=\"", sos );
	    sioOutPutString( file, sos );
	    sioOutPutString( "\"", sos );
	    }
	else{
	    sioOutPutString( "application/rtf", sos );
	    }
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Transfer-Encoding: ", sos );
	sioOutPutString( "base64", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sosBody= sioOutBase64Open( sos );
	if  ( ! sosBody )
	    { XDEB(sosBody); return -1;	}
	}

    rval= docRtfSaveDocument( sosBody, bd, (const DocumentSelection *)0,
							saveBookmarks );

    if  ( mimeBoundary )
	{
	sioOutClose( sosBody );

	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	}

    return rval;
    }

static int tedWriteHtmlMail(	SimpleOutputStream *		sos,
				const char *			mimeBoundary,
				void *				voidbd )
    {
    int		asMimeAggr= mimeBoundary != (const char *)0;

    return docHtmlSaveDocument( sos, (BufferDocument *)voidbd,
					asMimeAggr, mimeBoundary, (char *)0 );
    }

static int tedWritePlainMail(	SimpleOutputStream *		sos,
				const char *			mimeBoundary,
				void *				voidbd )
    {
    return docPlainSaveDocument( sos,
				(BufferDocument *)voidbd,
				(const DocumentSelection *)0,
				1, 1 );
    }

static MailContent	TedMailContents[]=
    {
	{ "mailPlain",	"text/plain",
			"text/plain",
			0,
			tedWritePlainMail
			},

	{ "mailRtf",	"application/rtf",
			"multipart/mixed",
			1,
			tedWriteRtfMail
			},

	{ "mailHtml",	"text/html",
			"multipart/related",
			1,
			tedWriteHtmlMail
			},
    };

static AppSelectionTargetType TedPrimaryTargets[]=
    {
	{ "RTF",	(APP_ATOM)0,	tedPastePrimaryRtf,
					tedCopyPrimaryRtf		},
	{ "STRING",	(APP_ATOM)0,	tedPastePrimaryString,
					tedCopyPrimaryString		},
	{ "PNG",	(APP_ATOM)0,	tedPastePrimaryPng,
					tedCopyPrimaryPng		},
	{ "PIXMAP",	(APP_ATOM)0,	tedPastePrimaryPixmap,
					tedCopyPrimaryPixmap		},
	{ "BITMAP",	(APP_ATOM)0,	tedPastePrimaryPixmap,
					tedCopyPrimaryPixmap		},
    };

AppSelectionTargetType TedPrimaryTextTargets[]=
    {
	{ "RTF",	(APP_ATOM)0,	tedPastePrimaryRtf,
					tedCopyPrimaryRtf		},
	{ "STRING",	(APP_ATOM)0,	tedPastePrimaryString,
					tedCopyPrimaryString		},
    };

const int TedPrimaryTextTargetCount=
	    sizeof(TedPrimaryTextTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedPrimaryPictureTargets[]=
    {
	{ "RTF",	(APP_ATOM)0,	tedPastePrimaryRtf,
					tedCopyPrimaryRtf		},
	{ "PNG",	(APP_ATOM)0,	tedPastePrimaryPng,
					tedCopyPrimaryPng		},
	{ "PIXMAP",	(APP_ATOM)0,	tedPastePrimaryPixmap,
					tedCopyPrimaryPixmap		},
	{ "BITMAP",	(APP_ATOM)0,	tedPastePrimaryPixmap,
					tedCopyPrimaryPixmap		},
    };

const int TedPrimaryPictureTargetCount=
	    sizeof(TedPrimaryPictureTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedRulerTargets[]=
    {
	{ "TED",	(APP_ATOM)0,	tedPasteRulerTed,
					tedCopyRulerTed		},
    };

const int TedRulerTargetCount=
	    sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType);

static AppSelectionTargetType TedFontTargets[]=
    {
	{ "TED",	(APP_ATOM)0,	tedPasteFontTed,
					tedCopyFontTed		},
    };

static AppSelectionType TedDocSelectionTypes[]=
    {
	{ "PRIMARY", (APP_ATOM)0,
		TedPrimaryTargets,
		sizeof(TedPrimaryTargets)/sizeof(AppSelectionTargetType),
		tedPrimaryLost	},
	{ "FONT", (APP_ATOM)0,
		TedFontTargets,
		sizeof(TedFontTargets)/sizeof(AppSelectionTargetType), },
	{ "RULER", (APP_ATOM)0,
		TedRulerTargets,
		sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType), },
    };

/************************************************************************/
/*									*/
/*  'Copy Font' menu option.						*/
/*									*/
/************************************************************************/

static void tedDocFontCopy(	APP_WIDGET	fontsOption,
				void *		voided,
				void *		voidpbcbs )
    {
    EditDocument *			ed= (EditDocument *)voided;
    TedDocument *			td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *		sos;

    char				scratch[120];

    if  ( ! tedHasSelection( td ) )
	{ LDEB(1); return;	}

    if  ( tedGetDocumentAttributeString( scratch, td ) )
	{ LDEB(1); return;	}

    sos= sioOutMemoryOpen( &(td->tdCopiedFont) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    sioOutPutString( scratch, sos );

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    appDocOwnSelection( ed, "FONT",
		    TedFontTargets,
		    sizeof(TedFontTargets)/sizeof(AppSelectionTargetType) );
    }

static EditApplication	TedApplication=
    {
		    /****************************************************/
		    /*  Ted Application name for Xt.			*/
		    /*  Version information for application window.	*/
		    /*  Reference to site for application window.	*/
		    /*  Document shell name for Xt.			*/
		    /*  Page Tool shell name for Xt.			*/
		    /*  Print Dialog shell name for Xt.			*/
		    /*  Mail Dialog shell name for Xt.			*/
		    /*  Message Dialog shell name for Xt.		*/
		    /*  Name of the icon for the application window.	*/
		    /*  Name of the picture for the application window.	*/
		    /****************************************************/
    "Ted",
    "Ted, Version 2.13, Mar 15, 2003",
    "http://www.nllgg.nl/Ted",


    "tedDocument",
    "tedPageTool",
    "tedPrintDialog",
    "tedMailDialog",
    "tedMessageDialog",
    "tedmain",
    "tedabout",
		    /****************************************************/
		    /*  File extensions, count.				*/
		    /****************************************************/
    TedFileExtensions, sizeof(TedFileExtensions)/sizeof(AppFileExtension),
		    /****************************************************/
		    /*  Default filter for file choosers.		*/
		    /****************************************************/
    "*.rtf",
    "teddoc",
    (void *)&TEDResources,
    TEDApplicationResourceTable,
    sizeof(TEDApplicationResourceTable)/sizeof(AppConfigurableResource),
    TEDFileMessageResourceTable,
    sizeof(TEDFileMessageResourceTable)/sizeof(AppConfigurableResource),
		    /****************************************************/
		    /*  Create new document from command line?		*/
		    /****************************************************/
    1,

    0,
    0,
    0.0,

		    /****************************************************/
		    /*  Ruler Geometry.					*/
		    /****************************************************/
    6, 12, 6, 6,
    6, 6, 6, 6,
		    /****************************************************/
		    /*  Page Gap (mm)					*/
		    /****************************************************/
    4,
		    /****************************************************/
		    /*  Default Geometry resources.			*/
		    /****************************************************/
    "inch",
    "letter",
    "90pt",
    "72pt",
    "90pt",
    "72pt",
    UNITtyINCH,
	{ 12240, 15840, },
		    /****************************************************/
		    /*  Application File Menu.				*/
		    /****************************************************/
    &(TEDResources.tarAppFileMenuText),
    TED_AppFileMenuItems,
    sizeof(TED_AppFileMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Application Window Menu.			*/
		    /****************************************************/
    &(TEDResources.tarAppWinMenuText),
    TED_AppWinMenuItems,
    sizeof(TED_AppWinMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document File Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocFileMenuText),
    TED_DocFileMenuItems,
    sizeof(TED_DocFileMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Edit Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocEditMenuText),
    TED_DocEditMenuItems,
    sizeof(TED_DocEditMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Window Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocWinMenuText),
    TED_DocWinMenuItems,
    sizeof(TED_DocWinMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document, Application Help Menu.		*/
		    /****************************************************/
    &(TEDResources.tarHelpMenuText),
    TED_DocHelpMenuItems,
    sizeof(TED_DocHelpMenuItems)/sizeof(AppMenuItem),

    &(TEDResources.tarHelpMenuText),
    TED_AppHelpMenuItems,
    sizeof(TED_AppHelpMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Widgets.				*/
		    /*  Their initialisation.				*/
		    /*  How to save them.				*/
		    /****************************************************/
    tedMakePrivateData,
    tedMakeDocumentWidget,
    tedOpenDocument,
    tedNewDocument,
    tedLayoutDocument,
    tedFinishDocumentSetup,
    NULL,					/*  CanSave		*/
    tedSaveDocument,				/*  Save		*/
    tedFreeDocument,				/*  Free		*/
    tedPrintDocument,				/*  PrintDocument	*/
    tedDrawRectangle,
		    /****************************************************/
		    /*  Track document count.				*/
		    /****************************************************/
    tedVisibleDocumentCountChanged,
		    /****************************************************/
		    /*  Private application,document menus.		*/
		    /****************************************************/
    NULL,
    tedMakePrivateDocumentMenus,
		    /****************************************************/
		    /*  Cut and paste functions. (Document Edit Menu)	*/
		    /****************************************************/
    tedDocCopy,
    tedDocCut,
    tedDocSelAll,
		    /****************************************************/
		    /*							*/
		    /*  Set page layout.				*/
		    /*							*/
		    /****************************************************/
    tedSetPageLayout,
		    /****************************************************/
		    /*  User input on the document widget:		*/
		    /****************************************************/
    tedMouseButtonPressed,
    tedKeyPressed,
    tedObserveFocus,

    tedDocHorizontalScrollbarCallback,
    tedDocVerticalScrollbarCallback,

    tedScrollHorizontalRuler,
    tedSetHorizontalRulerRange,
    tedScrollLeftRuler,			/*  ScrollVerticalRuler		*/
    tedSetLeftRulerRange,		/*  SetVerticalRulerRange	*/

    tedDocSetTopRuler,
    tedDocSetLeftRuler,

    tedFreeVerticalRuler,
    tedFreeHorizontalRuler,
    tedFreeVerticalRuler,
    tedFreeHorizontalRuler,

    TedDocSelectionTypes,
    sizeof(TedDocSelectionTypes)/ sizeof(AppSelectionType),

    (AppSelectionType *)0,
    0/ sizeof(AppSelectionType),

    0,0,0,

    TedMailContents,
    sizeof(TedMailContents)/sizeof(MailContent),
    0,
    };

/************************************************************************/
/*									*/
/*  Main() of the 'Ted' Application.					*/
/*									*/
/************************************************************************/

int main(	int		argc,
		char *		argv[]	)
    {
    EditApplication *	ea= &TedApplication;

    TEDResources.tarDefaultAnsicpgInt= -1;
    TEDResources.tarShowTableGridInt= 0;

    tedGetNamedPictures( ea );

    return appMain( ea, argc, argv );
    }
