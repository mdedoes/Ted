/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedRuler.h"
#   include	"tedDocument.h"
#   include	"tedDraw.h"
#   include	"tedCopyPaste.h"

#   include	<geoUnit.h>
#   include	<psBuildConfigFiles.h>
#   include	<sioGeneral.h>
#   include	<sioStdout.h>
#   include	<docFileExtensions.h>
#   include	<guiMenuItem.h>
#   include	"tedFileConvert.h"
#   include	"tedMenu.h"
#   include	<docParaProperties.h>
#   include	<docFrameProperties.h>
#   include	<docRowProperties.h>
#   include	<docDocumentProperties.h>
#   include	<appGuiResource.h>
#   include	<appFileMessageResources.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<tedResource.h>

#   include	<appDebugon.h>

/************************************************************************/

static AppConfigurableResource TEDApplicationResourceTable[]=
{
    /********************************************************************/
    /*  Application defaults.						*/
    /*  2)  "magnification" is duplicate. It exists for the		*/
    /*	application and for the font chooser tool.			*/
    /********************************************************************/
    APP_RESOURCE( "fileFileNotRtf",
		offsetof(TedAppResources,tarFileNotRtf),
		"File is not in Rich Text Format. Open anyway?" ),
    APP_RESOURCE( "headFootNotReachable",
	    offsetof(TedAppResources,tarHeadFootNotReachable),
	    "There are not enough pages in the section to edit "
	    "this Header/Footer." ),

    APP_RESOURCE( "cannotRecover",
	    offsetof(TedAppResources,tarCannotRecover),
	    "Can not recover from this trace file. "
	    "Try to unlock." ),

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

    /**/
    APP_RESOURCE( "documentFileName",
		offsetof(TedAppResources,tarAppHelpFileName),
		DOCUMENT_DIR "/TedDocument-en_US.rtf" ),

    /**/
    APP_RESOURCE( "pageNumberFormat",
	    offsetof(TedAppResources,tarPageNumberFormat),
	    "Page %p/%P, Section %s/%S" ),

    /**/
    APP_RESOURCE( "selectionColor",
		offsetof(TedAppResources,tarSelectionColor),
		"LightSteelBlue" ),
    APP_RESOURCE( "copiedSelectionColor",
		offsetof(TedAppResources,tarCopiedSelectionColor),
		"gray70" ),

    /**/
    APP_RESOURCE( "showTableGrid",
		offsetof(TedAppResources,tarShowTableGridString),
		"1" ),

    /**/
    APP_RESOURCE( "shadingMeshTwips",
		offsetof(TedAppResources,tarShadingMeshTwipsString),
		(char *)0 ),

    /**/
    APP_RESOURCE( "pageGapMM",
		offsetof(TedAppResources,tarPageGapMMString),
		(char *)0 ),
    /**/
    APP_RESOURCE( "lenientRtf",
		offsetof(TedAppResources,tarLenientRtfString),
		"0" ),
    APP_RESOURCE( "traceEdits",
		offsetof(TedAppResources,tarTraceEditsString),
		"1" ),
    APP_RESOURCE( "pdfOutline",
		offsetof(TedAppResources,tarPdfOutlineString),
		"1" ),
    APP_RESOURCE( "pdfMarkContent",
		offsetof(TedAppResources,tarPdfMarkContentString),
		"0" ),
    APP_RESOURCE( "pdfOmitContentMarks",
		offsetof(TedAppResources,tarPdfOmitContentMarksString),
		"0" ),
    APP_RESOURCE( "pdfDeclareUACompliant",
		offsetof(TedAppResources,tarPdfDeclareUACompliantString),
		"0" ),
    APP_RESOURCE( "pdfMarkInvisibleAsArtifact",
		offsetof(TedAppResources,tarPdfMarkInvisibleAsArtifactString),
		"0" ),
    APP_RESOURCE( "pdfFlattenPlainTables",
		offsetof(TedAppResources,tarPdfFlattenPlainTablesString),
		"0" ),
    APP_RESOURCE( "drawParticulesSeparately",
		offsetof(TedAppResources,tarDrawParticulesSeparatelyString),
		"0" ),
    APP_RESOURCE( "overridePaperSize",
		offsetof(TedAppResources,tarOverridePaperSizeString),
		"0" ),
    APP_RESOURCE( "inlineHtmlImages",
		offsetof(TedAppResources,tarInlineHtmlImagesString),
		"0" ),
    APP_RESOURCE( "honourSpecialSectBreaks",
		offsetof(TedAppResources,tarHonourSpecialSectBreaksString),
		"0" ),
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
/*  Ted, description of the application.				*/
/*									*/
/************************************************************************/

static AppSelectionTargetType TedClipboardTargets[]=
{
    { "application/rtf", (APP_ATOM)0,	tedPasteClipboardRtf,
					tedCopyClipboardRtf		},
    /* Not for Paste!
    { "text/html", (APP_ATOM)0,		(APP_PASTE_REPLY)0,
					tedCopyClipboardHtml		},
    */
    { "UTF8_STRING", (APP_ATOM)0,	tedPasteClipboardString,
					tedCopyClipboardString		},
    { "STRING",	(APP_ATOM)0,		tedPasteClipboardString,
					tedCopyClipboardString		},
    { "image/png", (APP_ATOM)0,		tedPasteClipboardPng,
					tedCopyClipboardPng		},
    { "image/jpeg", (APP_ATOM)0,	tedPasteClipboardJfif,
					tedCopyClipboardJfif		},
};

AppSelectionTargetType TedPrimaryTargets[]=
{
    { "UTF8_STRING", (APP_ATOM)0,	tedPastePrimaryString,
					tedCopyPrimaryString		},
    { "STRING",	(APP_ATOM)0,		tedPastePrimaryString,
					tedCopyPrimaryString		},
};

const int TedPrimaryTargetCount=
	    sizeof(TedPrimaryTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedClipboardTextTargets[]=
{
    { "application/rtf", (APP_ATOM)0,	tedPasteClipboardRtf,
					tedCopyClipboardRtf		},
    { "text/html", (APP_ATOM)0,		(APP_PASTE_REPLY)0,
					tedCopyClipboardHtml		},
    { "UTF8_STRING", (APP_ATOM)0,	tedPasteClipboardString,
					tedCopyClipboardString		},
    { "STRING",	(APP_ATOM)0,		tedPasteClipboardString,
					tedCopyClipboardString		},
};

const int TedClipboardTextTargetCount=
	    sizeof(TedClipboardTextTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedRulerTargets[]=
{
    { "application/rtf",	(APP_ATOM)0,	tedPasteRulerTed,
						tedCopyRulerTed		},
};

const int TedRulerTargetCount=
	    sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedFontTargets[]=
{
    { "application/rtf",	(APP_ATOM)0,	tedPasteFontTed,
						tedCopyFontTed		},
};

const int TedFontTargetCount=
	    sizeof(TedFontTargets)/sizeof(AppSelectionTargetType);

static AppSelectionType TedDocSelectionTypes[]=
{
    { "CLIPBOARD", (APP_ATOM)0,
	    TedClipboardTargets,
	    sizeof(TedClipboardTargets)/sizeof(AppSelectionTargetType),
	    tedClipboardLost },
    { "RTFFONT", (APP_ATOM)0,
	    TedFontTargets,
	    sizeof(TedFontTargets)/sizeof(AppSelectionTargetType), },
    { "RTFRULER", (APP_ATOM)0,
	    TedRulerTargets,
	    sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType), },
    { "PRIMARY", (APP_ATOM)0,
	    TedPrimaryTargets,
	    sizeof(TedPrimaryTargets)/sizeof(AppSelectionTargetType),
	    tedPrimaryLost	},
};

/************************************************************************/

static void tedMainSetPageLayout( EditDocument *		ed,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet,
				int				wholeDocument )
    {
    tedDocSetPageLayout( ed, dgSetMask, dgSet, wholeDocument,
				((TedDocument *)ed->edPrivateData)->tdTraced );
    }

/************************************************************************/

static int tedFind(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    TEDResources.tarFindPattern= (const char *)argv[0];
    TEDResources.tarFindRegex= 0;

    return 1;
    }

static int tedRegFind(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    TEDResources.tarFindPattern= (const char *)argv[0];
    TEDResources.tarFindRegex= 1;

    return 1;
    }

static int tedGSFontmapForFiles(EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    int		ret= argc;

    SimpleOutputStream *	sosOut= sioOutStdoutOpen();
    if  ( ! sosOut )
	{ XDEB(sosOut); return -1;	}

    if  ( psFontmapForFiles( sosOut, argc, (const char * const *)argv ) )
	{ SLDEB(call,argc); ret= -1;	}

    sioOutClose( sosOut );

    return ret;
    }

static int tedRtfToPs(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    fprintf( stderr, "Use Ted --printToFile -rtf- -ps-\n" );
    return -1;
    }

static int tedRtfToPsPaper(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    fprintf( stderr, "Use Ted --printToFilePaper -rtf- -ps- %s\n", argv[0] );
    return -1;
    }

static SpecialCall TedSpecialCalls[]=
{
    { "TtfToAfm",		tedTtfToAfm,			},
    { "AfmToGSFontmap",		tedAfmToGSFontmap,		},
    { "GSFontmapForFiles",	tedGSFontmapForFiles,		},
    { "AfmForFontFiles",	tedAfmForFontFiles,		},
    { "RtfToPs",		tedRtfToPs,			},
    { "RtfToPsPaper",		tedRtfToPsPaper,		},
    { "Find",			tedFind,			},
    { "RegFind",		tedRegFind,			},
    { "FontsDocuments",		tedFontsDocuments,		},
    { "Concatenate",		tedConcatenate,			},
    { "ConcatenateText",	tedConcatenateText,		},
    { "MergeJson",		tedMergeJson,			},
};

/************************************************************************/

#   if USE_FONTCONFIG
#	include	<fontconfig/fontconfig.h> /* keep for version */
#	include	<ft2build.h> /* keep for version */
#   endif

#   if USE_XFT
#	include	FT_FREETYPE_H /* keep for version */
#   endif

# define VSS(x) #x
# define VSSS(x) VSS(x)

#   ifdef XmVERSION_STRING
    # define MOTIF_VSS "\"" XmVERSION_STRING "\" "
#   else
    # define MOTIF_VSS "Motif "
#   endif

# define GTK_VSS \
	"GTK " VSSS(GTK_MAJOR_VERSION) "." \
	VSSS(GTK_MINOR_VERSION) "." \
	VSSS(GTK_MICRO_VERSION) " "

# define FT2_VSS \
    "FT2 " VSSS(FREETYPE_MAJOR) "." VSSS(FREETYPE_MINOR) " "

# define XFT_VSS \
    "XFT " VSSS(XFT_MAJOR) "." VSSS(XFT_MINOR) "." VSSS(XFT_REVISION) " "

# define FC_VSS \
    "FONTCONFIG " VSSS(FC_MAJOR) "." VSSS(FC_MINOR) "." VSSS(FC_REVISION) " "

static const char TED_ConfigString[]=

#   if USE_MOTIF
    MOTIF_VSS
#   endif

#   if USE_GTK
    GTK_VSS
#   endif

#   if USE_FONTCONFIG
    FC_VSS
#   endif

#   if USE_XFT
    FT2_VSS
#   endif

    "";

/************************************************************************/

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
    TED_ConfigString,
    "Ted 2.23nz2, July 9, 2023",
    "http://www.nllgg.nl/Ted",
    MY_PLATFORM " " MY_RELEASE,
    "\"" MY_HOST_DATE "\"",

    "tedmain",
    "tedabout",
		    /****************************************************/
		    /*  File extensions, count.				*/
		    /*  Set in main() to..				*/
		    /*  -   DOC_FileExtensions				*/
		    /*  -   DOC_FileExtensionCount			*/
		    /****************************************************/
    (struct AppFileExtension *)0, 0,
		    /****************************************************/
		    /*  Default filter for file choosers.		*/
		    /****************************************************/
    "*.rtf",
    (void *)&TEDResources,
    TEDApplicationResourceTable,
    sizeof(TEDApplicationResourceTable)/sizeof(AppConfigurableResource),
    TEDFileMessageResourceTable,
    sizeof(TEDFileMessageResourceTable)/sizeof(AppConfigurableResource),
    TedSpecialCalls, sizeof(TedSpecialCalls)/sizeof(SpecialCall),
		    /****************************************************/
		    /*  Create new document from command line?		*/
		    /****************************************************/
    1,

    1.0,

		    /****************************************************/
		    /*  Ruler Geometry.					*/
		    /****************************************************/
    4, 8, 4, 4,
    6, 6, 6, 6,
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
    &TED_AppFileMenu,
		    /****************************************************/
		    /*  Application Window Menu.			*/
		    /****************************************************/
    &TED_AppWinMenu,
		    /****************************************************/
		    /*  Document File Menu.				*/
		    /****************************************************/
    &TED_DocFileMenu,
		    /****************************************************/
		    /*  Document Edit Menu.				*/
		    /****************************************************/
    &TED_DocEditMenu,
		    /****************************************************/
		    /*  Document Window Menu.				*/
		    /****************************************************/
    &TED_DocWinMenu,
		    /****************************************************/
		    /*  Document, Application Help Menu.		*/
		    /****************************************************/
    &TED_DocHelpMenu,
    &TED_AppHelpMenu,
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
    tedDocumentFirstVisible,
    NULL,					/*  CanSave		*/
    tedSaveDocument,				/*  Save		*/
    tedFreeDocument,				/*  Free		*/
    tedSuggestPageSetup,			/*  SuggestPageSetup	*/
    tedPreparePrint,				/*  PreparePrint	*/
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
    tedMainSetPageLayout,
		    /****************************************************/
		    /*  User input on the document widget:		*/
		    /****************************************************/
    tedMouseButtonPressed,
    tedScrollEventHandler,
    tedDocGotString,
    tedDocGotKey,
    tedDocObserveFocus,

    tedDocHorizontalScrollbarCallback,
    tedDocVerticalScrollbarCallback,

    tedScrollTopRuler,
    tedSetTopRulerRange,
    NULL,				/*  ScrollLeftRuler		*/
    NULL,				/*  SetLeftRulerRange		*/
    NULL,				/*  ScrollRightRuler		*/
    NULL,				/*  SetRightRulerRange		*/
    NULL,				/*  ScrollBottomRuler		*/
    NULL,				/*  SetBottomRulerRange		*/

    tedDocSetTopRuler,			/*  eaSetTopRuler		*/
    tedDocSetLeftRuler,			/*  eaSetLeftRuler		*/
    tedDocSetRightRuler,		/*  eaSetRightRuler		*/
    tedDocSetBottomRuler,		/*  eaSetBottomRuler		*/

    tedFreeTopRuler,			/*  eaFreeTopRuler		*/
    tedFreeVerticalRuler,		/*  eaFreeLeftRuler		*/
    tedFreeVerticalRuler,		/*  eaFreeRightRuler		*/
    tedFreeBottomRuler,			/*  eaFreeBottomRuler		*/

    TedDocSelectionTypes,
    sizeof(TedDocSelectionTypes)/ sizeof(AppSelectionType),

    (AppSelectionType *)0,
    0/ sizeof(AppSelectionType),

    0,0,0,0,
    };

/************************************************************************/
/*									*/
/*  Main() of the 'Ted' Application.					*/
/*									*/
/*  1)  Sanity checks							*/
/*  2)  Initialisation of the application wide data.			*/
/*									*/
/************************************************************************/

int main(	int		argc,
		char *		argv[]	)
    {
    EditApplication *	ea= &TedApplication;

    /*  1  */
    if  ( RPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(RPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( PPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(PPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( CLprop_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(CLprop_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( DPprop_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(DPprop_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( TFPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(TFPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}

    ea->eaFileExtensions= DOC_FileExtensions;
    ea->eaFileExtensionCount= DOC_FileExtensionCount;

    /*  2  */
    TEDResources.tarShowTableGridInt= 0;
    TEDResources.tarShadingMeshPointsDouble= -1;

    TEDResources.tarFindPattern= (const char *)0;
    TEDResources.tarFindRegex= 0;

    tedGetNamedImages( &(ea->eaNamedImages), &(ea->eaNamedImageCount) );

    return appMain( ea, argc, argv );
    }

