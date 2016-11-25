/************************************************************************/
/*									*/
/*  Print Dialog.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<appFrame.h>
#   include	"appUnit.h"
#   include	"appGeoString.h"
#   include	"appPaperChooser.h"
#   include	"appPaper.h"

#   define	DRH_MM	40

/************************************************************************/
/*									*/
/*  Represents a print dialog.						*/
/*									*/
/************************************************************************/

#   define	FILEL	400

#   define	PRINTselAll_PAGES	0
#   define	PRINTselPAGE_RANGE	1
#   define	PRINTselCURRENT_PAGE	2

#   define	PRINTselCOUNT		3

typedef struct PlacementSpecification
    {
    int		psGridRows;
    int		psGridCols;
    int		psGridHorizontal;
    int		psScalePagesToFit;
    char *	psOptionText;
    APP_WIDGET	psOptionWidget;
    } PlacementSpecification;

typedef enum PlacementSpecificationNumber
    {
    PSnum_1x1_ASIS= 0,
    PSnum_1x1_SCALED,
    PSnum_2x1_HOR,
    PSnum_1x2_HOR,
    PSnum_2x2_HOR,
    PSnum_2x2_VER,

    PSnum__COUNT
    } PlacementSpecificationNumber;

static PlacementSpecification	APP_Placements[PSnum__COUNT]=
    {
	{ 1, 1, 0, 0, "1x1 As Is"	},
	{ 1, 1, 1, 1, "1x1 Scale"	},
	{ 2, 1, 1, 1, "2x1 Hor"		},
	{ 1, 2, 1, 1, "1x2 Hor"		},
	{ 2, 2, 1, 1, "2x2 Hor"		},
	{ 2, 2, 0, 1, "2x2 Ver"		},
    };

typedef struct AppPrintDialog
    {
    AppDialog			apdDialog;

    APP_WIDGET			apdPrintButton;
    APP_WIDGET			apdCancelButton;

    AppOptionmenu		apdPrinterOptionmenu;
    AppOptionmenu		apdPlacementOptionmenu;
    AppOptionmenu		apdSelectionOptionmenu;

    APP_WIDGET			apdCustomTransformRow;
    APP_WIDGET			apdRotate90Toggle;
    APP_WIDGET			apdCenterHToggle;

    int				apdUnitType;
    PaperChooser		apdPaperChooser;

    APP_WIDGET			apdFaxNumberText;

    APP_WIDGET			apdPageDrawing;
    AppDrawingData		apdDrawingData;
    APP_FONT *			apdFontStruct;

    APP_WIDGET			apdSelectionOptions[PRINTselCOUNT];
    APP_WIDGET			apdPageFromTextWidget;
    APP_WIDGET			apdPageToTextWidget;

    int				apdPrinterCount;
    int				apdPrintOptionCount;
    int				apdOptionNrChosen;
    int				apdVisiblePrinterCount;

    int				apdActivateFaxOptions;
    int				apdToFileOptionNr;
    int				apdFaxOptionNr;
    int				apdUpOptionNr;
    int				apdFirstPrinterOptionNr;
    int				apdDownOptionNr;
    int				apdFirstVisiblePrinter;

    DocumentGeometry		apdDocumentGeometry;
    DocumentGeometry		apdPrinterGeometry;
    PlacementSpecification	apdPlacements[PSnum__COUNT];
    int				apdPlacementChosen;
    int				apdRotate90Chosen;
    int				apdCenterHChosen;

    int				apdPageCount;
    int				apdFirstSelectedPage;
    int				apdLastSelectedPage;
    int				apdSelectionChosen;
    int				apdFirstPageChosen;
    int				apdLastPageChosen;

    void *			apdTarget;

    char *			apdPrinterText;
    char *			apdNoneText;
    char *			apdCancelText;
    char *			apdPrintText;

    char *			apdFaxText;
    char *			apdPrintToFileText;
    char *			apdVisiblePrinterCountText;
    char *			apdPrintersUpText;
    char *			apdPrintersDownText;

    char *			apdPaperSizeText;
    char *			apdCustomPaperSizeText;
    char *			apdPlacementText;
    char *			apdShowPlacementDrawingString;
    int				apdShowPlacementDrawingInt;
    char *			apdRotate90Text;
    char *			apdCenterHText;

    char *			apdSelectionText;
    char *			apdAllPagesText;
    char *			apdPageRangeText;
    char *			apdCurrentPageText;
    char *			apdFromPageText;
    char *			apdToPageText;

    char *			apdDefaultPrinter;

    APP_WIDGET			apdPrinterOptions[1]; 	/* LAST! */
    } AppPrintDialog;

static AppConfigurableResource APP_PrintDialogresourceTable[]=
    {
	APP_RESOURCE( "printDialogPrinter",
		    offsetof(AppPrintDialog,apdPrinterText),
		    "Printer" ),
	APP_RESOURCE( "printDialogNone",
		    offsetof(AppPrintDialog,apdNoneText),
		    "None" ),
	APP_RESOURCE( "printDialogCancel",
		    offsetof(AppPrintDialog,apdCancelText),
		    "Cancel" ),
	APP_RESOURCE( "printDialogPrint",
		    offsetof(AppPrintDialog,apdPrintText),
		    "Print" ),
	APP_RESOURCE( "printDialogPrintToFile",
		    offsetof(AppPrintDialog,apdPrintToFileText),
		    "Print to File" ),

	APP_RESOURCE( "printDialogFax",
		    offsetof(AppPrintDialog,apdFaxText),
		    "Fax" ),
	APP_RESOURCE( "printDialogPaperSize",
		    offsetof(AppPrintDialog,apdPaperSizeText),
		    "Paper Size" ),
	APP_RESOURCE( "printDialogCustomPaperSize",
		    offsetof(AppPrintDialog,apdCustomPaperSizeText),
		    "Custom" ),

	/***/
	APP_RESOURCE( "printDialogPlacement",
		    offsetof(AppPrintDialog,apdPlacementText),
		    "Placement" ),

	APP_RESOURCE( "printDialogShowPlacementDrawing",
		    offsetof(AppPrintDialog,apdShowPlacementDrawingString),
		    "1" ),

	APP_RESOURCE( "printDialog_1_UpText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_1x1_ASIS].psOptionText),
		    "1 Page/Sheet" ),
	APP_RESOURCE( "printDialog_1_ScaledText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_1x1_SCALED].psOptionText),
		    "1 Page/Sheet Scale to Fit" ),
	APP_RESOURCE( "printDialog_2Row_UpText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_2x1_HOR].psOptionText),
		    "2 Pages/Sheet Vertical" ),
	APP_RESOURCE( "printDialog_2Col_UpText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_1x2_HOR].psOptionText),
		    "2 Pages/Sheet Horizontal" ),
	APP_RESOURCE( "printDialog_4H_UpText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_2x2_HOR].psOptionText),
		    "4 Pages/Sheet Horizontal" ),
	APP_RESOURCE( "printDialog_4V_UpText",
		    offsetof(AppPrintDialog,
				apdPlacements[PSnum_2x2_VER].psOptionText),
		    "4 Pages/Sheet Vertical" ),

	APP_RESOURCE( "printDialogRotate90",
		    offsetof(AppPrintDialog,apdRotate90Text),
		    "Rotate 90\260" ),
	APP_RESOURCE( "printDialogCenterH",
		    offsetof(AppPrintDialog,apdCenterHText),
		    "Center Horizontally" ),

	/***/
	APP_RESOURCE( "printDialogSelection",
		    offsetof(AppPrintDialog,apdSelectionText),
		    "Selection" ),

	APP_RESOURCE( "printDialogAllPages",
		    offsetof(AppPrintDialog,apdAllPagesText),
		    "All Pages" ),
	APP_RESOURCE( "printDialogPageRange",
		    offsetof(AppPrintDialog,apdPageRangeText),
		    "Page Range" ),
	APP_RESOURCE( "printDialogCurrentPage",
		    offsetof(AppPrintDialog,apdCurrentPageText),
		    "Current Page" ),

	APP_RESOURCE( "printDialogFromPage",
		    offsetof(AppPrintDialog,apdFromPageText),
		    "From" ),
	APP_RESOURCE( "printDialogToPage",
		    offsetof(AppPrintDialog,apdToPageText),
		    "To" ),

	APP_RESOURCE( "defaultPrinter",
		    offsetof(AppPrintDialog,apdDefaultPrinter),
		    "" ),

	APP_RESOURCE( "printDialogPrinterCount",
		    offsetof(AppPrintDialog,apdVisiblePrinterCountText),
		    "15" ),
	APP_RESOURCE( "printDialogPrintersUp",
		    offsetof(AppPrintDialog,apdPrintersUpText),
		    "%d up" ),
	APP_RESOURCE( "printDialogPrintersDown",
		    offsetof(AppPrintDialog,apdPrintersDownText),
		    "%d down" ),
    };

/************************************************************************/
/*									*/
/*  Adapt the menu with printers to the currently selected range of	*/
/*  printers.								*/
/*									*/
/*  1)  All printers can be shown.. No need to offer browsing options	*/
/*  2)  Make the browsing options visible.				*/
/*  3)  Hide all printers before the fist visible one.			*/
/*  4)  Show the range of visible printers.				*/
/*  5)  Hide all printers after the last visible one.			*/
/*  6)  Enable/Disable the browsing options depending on whether there	*/
/*	are any printers before/after the current range.		*/
/*									*/
/************************************************************************/

static void appPrintDialogMenuRange(	AppPrintDialog *	apd )
    {
    int		i;
    int		gotBefore= 0;
    int		gotAfter= 0;

    /*  1  */
    if  ( apd->apdPrinterCount <= apd->apdVisiblePrinterCount )
	{
	if  ( apd->apdUpOptionNr >= 0 )
	    {
	    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdUpOptionNr], 0 );
	    }

	if  ( apd->apdDownOptionNr >= 0 )
	    {
	    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdDownOptionNr], 0 );
	    }

	return;
	}

    /*  2  */
    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdUpOptionNr], 1 );
    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdDownOptionNr], 1 );

    /*  3  */
    for ( i= 0; i < apd->apdFirstVisiblePrinter; i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 0 );
	gotBefore= 1;
	}

    /*  4  */
    for ( i= apd->apdFirstVisiblePrinter;
	  i < apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount &&
	  i < apd->apdPrinterCount;
	  i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 1 );
	}

    /*  5  */
    for ( i= apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount;
	  i < apd->apdPrinterCount;
	  i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 0 );
	gotAfter= 1;
	}

    /*  6  */
    appGuiEnableWidget(
		    apd->apdPrinterOptions[apd->apdUpOptionNr], gotBefore );
    appGuiEnableWidget(
		    apd->apdPrinterOptions[apd->apdDownOptionNr], gotAfter );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Show a range of pages in the Print Dialog.				*/
/*									*/
/************************************************************************/

static void appPrintDialogShowFromTo(	AppPrintDialog *	apd,
					int			fromPage,
					int			toPage,
					int			enabled )
    {
    appIntegerToTextWidget( apd->apdPageFromTextWidget, fromPage );
    appIntegerToTextWidget( apd->apdPageToTextWidget, toPage );

    appEnableText( apd->apdPageFromTextWidget, enabled );
    appEnableText( apd->apdPageToTextWidget, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  Fax number changed.							*/
/*									*/
/************************************************************************/

static void appPrintCheckFaxNumber(	AppPrintDialog *	apd )
    {
    char *	faxNumber;
    char *	s;

    s= faxNumber= appGetStringFromTextWidget( apd->apdFaxNumberText );

    while( *s == ' ' )
	{ s++;	}

    appGuiEnableWidget( apd->apdPrintButton, *s != '\0' );

    appFreeStringFromTextWidget( faxNumber );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appPrintFaxChanged, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appPrintCheckFaxNumber( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  A printer was chosen.						*/
/*									*/
/************************************************************************/

static void appPrinterAdaptToDestination(	AppPrintDialog *	apd )
    {
    if  ( apd->apdOptionNrChosen == apd->apdFaxOptionNr )
	{
	if  ( apd->apdFaxNumberText )
	    {
	    appEnableText( apd->apdFaxNumberText, 1 );

	    appPrintCheckFaxNumber( apd );
	    }
	}
    else{
	if  ( apd->apdFaxNumberText )
	    {
	    appEnableText( apd->apdFaxNumberText, 0 );
	    }

	appGuiEnableWidget( apd->apdPrintButton, 1 );

	appGuiSetDefaultButtonForDialog( &(apd->apdDialog),
						    apd->apdPrintButton );
	}
    }

static APP_OITEM_CALLBACK_H( appDestinationChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    short		n= -1;

    n= appGuiGetOptionmenuItemIndex( &(apd->apdPrinterOptionmenu), w );
    if  ( n < 0 || n >= apd->apdPrintOptionCount )
	{ LLDEB(n,apd->apdPrintOptionCount); return;	}

    apd->apdOptionNrChosen= n;

    appPrinterAdaptToDestination( apd );

    return;
    }

static APP_OITEM_CALLBACK_H( appPrintersUpChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			printerChosen;

    apd->apdFirstVisiblePrinter -= apd->apdVisiblePrinterCount;
    if  ( apd->apdFirstVisiblePrinter < 0 )
	{ apd->apdFirstVisiblePrinter= 0;	}

    printerChosen= apd->apdFirstVisiblePrinter;
    apd->apdOptionNrChosen= apd->apdFirstPrinterOptionNr+ printerChosen;

    appPrintDialogMenuRange( apd );
    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );

    return;
    }

static APP_OITEM_CALLBACK_H( appPrintersDownChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			printerChosen;

    apd->apdFirstVisiblePrinter += apd->apdVisiblePrinterCount;
    if  ( apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount >
						    apd->apdPrinterCount )
	{
	apd->apdFirstVisiblePrinter=
			apd->apdPrinterCount- apd->apdVisiblePrinterCount;
	if  ( apd->apdFirstVisiblePrinter < 0 )
	    { apd->apdFirstVisiblePrinter= 0;	}
	}

    printerChosen= apd->apdFirstVisiblePrinter;
    apd->apdOptionNrChosen= apd->apdFirstPrinterOptionNr+ printerChosen;

    appPrintDialogMenuRange( apd );
    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh placement drawing.						*/
/*									*/
/************************************************************************/

static void appPrintDialogRefreshPlacementDrawing(
					    AppPrintDialog *	apd )
    {
    AppDrawingData *	add= &(apd->apdDrawingData);

    if  ( ! apd->apdPageDrawing )
	{ return;	}

    appExposeRectangle( add, 0, 0, 0, 0 );
    }

/************************************************************************/
/*									*/
/*  A placement was chosen.						*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appPrintDialogPlacementChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			i;

    i= appGuiGetOptionmenuItemIndex( &(apd->apdPlacementOptionmenu), w );
    if  ( i < 0 || i >= PSnum__COUNT )
	{ LLDEB(i,PSnum__COUNT); return;	}

    apd->apdPlacementChosen= i;

    if  ( apd->apdCustomTransformRow )
	{
	int	rotate90= apd->apdRotate90Chosen;
	int	centerHorizontally= apd->apdCenterHChosen;

	if  ( apd->apdPlacementChosen != 0 )
	    { rotate90= centerHorizontally= 0;	}

	appGuiEnableWidget( apd->apdRotate90Toggle,
				    apd->apdPlacementChosen == 0 );
	appGuiEnableWidget( apd->apdCenterHToggle,
				    apd->apdPlacementChosen == 0 );

	appGuiSetToggleState( apd->apdRotate90Toggle, rotate90 );
	appGuiSetToggleState( apd->apdCenterHToggle, centerHorizontally );
	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  A selection mode was chosen.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appSelectionChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			n;

#   ifdef USE_MOTIF
    XSetInputFocus( XtDisplay( apd->apdDialog.adTopWidget ),
				    XtWindow( apd->apdDialog.adTopWidget ),
				    RevertToNone, CurrentTime );
#   endif

    n= appGuiGetOptionmenuItemIndex( &(apd->apdSelectionOptionmenu), w );
    if  ( n < 0 || n >= PRINTselCOUNT )
	{ LLDEB(n,PRINTselCOUNT); return;	}

    switch( n )
	{
	case PRINTselAll_PAGES:
	    apd->apdFirstPageChosen= 0;
	    apd->apdLastPageChosen= apd->apdPageCount- 1;

	    appPrintDialogShowFromTo( apd, 1, apd->apdPageCount, 0 );
	    apd->apdSelectionChosen= n;
	    return;

	case PRINTselPAGE_RANGE:
	    appEnableText( apd->apdPageFromTextWidget, 1 );
	    appEnableText( apd->apdPageToTextWidget, 1 );
	    apd->apdSelectionChosen= n;

#	    ifdef USE_MOTIF
	    XmProcessTraversal( apd->apdPageFromTextWidget,
							XmTRAVERSE_CURRENT );
#	    endif
	    return;

	case PRINTselCURRENT_PAGE:
	    apd->apdFirstPageChosen= apd->apdFirstSelectedPage;
	    apd->apdLastPageChosen= apd->apdLastSelectedPage;

	    appPrintDialogShowFromTo( apd, apd->apdFirstSelectedPage+ 1,
					    apd->apdLastSelectedPage+ 1, 0 );
	    apd->apdSelectionChosen= n;
	    return;

	default:
	    LDEB(n); return;
	}
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appPrintDialogCancelPushed, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appGuiBreakDialog( &(apd->apdDialog), AQDrespCANCEL );

    return;
    }

static APP_BUTTON_CALLBACK_H( appPrintDialogPrintPushed, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    if  ( apd->apdOptionNrChosen < 0	)
	{ LDEB(apd->apdOptionNrChosen); return;	}

    if  ( apd->apdSelectionChosen == PRINTselPAGE_RANGE )
	{
	int		from;
	int		to;

	if  ( appGetIntegerFromTextWidget( apd->apdPageFromTextWidget,
					&from, 1, 0, apd->apdPageCount, 0 ) )
	    { return;	}

	if  ( appGetIntegerFromTextWidget( apd->apdPageToTextWidget,
					&to, from, 0, apd->apdPageCount, 1 ) )
	    { return;	}

	apd->apdFirstPageChosen= from- 1;
	apd->apdLastPageChosen= to- 1;
	}

    appGuiBreakDialog( &(apd->apdDialog), AQDrespOK );

    return;
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen.						*/
/*									*/
/************************************************************************/

static void appPrintDialogPaperRectChanged(
				    PaperChooser *		pc,
				    void *			voidapd,
				    const DocumentGeometry *	dg )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;

    apd->apdPrinterGeometry.dgPageWideTwips= dg->dgPageWideTwips;
    apd->apdPrinterGeometry.dgPageHighTwips= dg->dgPageHighTwips;

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  The custom placement toggles were activated.			*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogRotate90Toggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdRotate90Chosen= ( set != 0 );

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogCenterHToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdCenterHChosen= ( set != 0 );

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the page placement.			*/
/*									*/
/************************************************************************/

static int appPrintDialogPrintGeometry(
				    PrintGeometry *		pg,
				    AppPrintDialog *		apd )
    {
    const PlacementSpecification *	ps;

    PropertyMask			updMask;

    if  ( appPaperChooserGetSize( &updMask,
				    &(apd->apdPaperChooser),
				    &(apd->apdPrinterGeometry) ) )
	{ return -1;	}

    ps= apd->apdPlacements+ apd->apdPlacementChosen;

    pg->pgSheetGeometry= apd->apdPrinterGeometry;

    pg->pgRotatePage90= apd->apdRotate90Chosen;
    pg->pgCenterPageHorizontally= apd->apdCenterHChosen;

    if  ( ps->psGridRows * ps->psGridCols != 1 )
	{
	pg->pgRotatePage90= 0;
	pg->pgCenterPageHorizontally= 0;
	}

    pg->pgGridRows= ps->psGridRows;
    pg->pgGridCols= ps->psGridCols;
    pg->pgGridHorizontal= ps->psGridHorizontal;
    pg->pgScalePagesToFit= ps->psScalePagesToFit;

    return 0;
    }

static APP_EVENT_HANDLER_H( appPrintDialogDrawPage, w, voidapd, exposeEvent )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    AppDrawingData *		add= &(apd->apdDrawingData);

    PrintGeometry		pg;

    if  ( ! apd->apdPageDrawing )
	{ XDEB(apd->apdPageDrawing); return;	}

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{ LLDEB(apd->apdPlacementChosen,PSnum__COUNT); return;	}

    if  ( appPrintDialogPrintGeometry( &pg, apd ) )
	{ LDEB(1);	}

    appDrawNupDiagram( apd->apdPageDrawing, add, apd->apdFontStruct, DRH_MM,
					    &(apd->apdDocumentGeometry), &pg );
    }

/************************************************************************/
/*									*/
/*  Fill the list of printers.						*/
/*									*/
/*  1)  Empty option menu. It should already be empty.			*/
/*  2)  Insert print to file option.					*/
/*  3)  Insert fax option.						*/
/*  4)  Insert printers up option.					*/
/*  5)  Insert options for the actual printers.				*/
/*  6)  Insert printers down option.					*/
/*									*/
/************************************************************************/

static void appPrintDialogFillPrinterMenu(
					AppPrintDialog *	apd,
					int			printerCount,
					int			defaultPrinter,
					PrintDestination *	pd )
    {
    int			opt;
    int			prt;

    int			optionChosen= -1;
    int			firstPrinter= -1;

    /*  1  */
    appEmptyOptionmenu( &(apd->apdPrinterOptionmenu) );

    opt= 0;

    /*  2  */
    {
    apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apd->apdPrintToFileText,
					appDestinationChosen, (void *)apd );

    if  ( optionChosen < 0 )
	{ optionChosen= opt;	}

    apd->apdToFileOptionNr= opt++;
    }

    /*  3  */
    {
    apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apd->apdFaxText,
					appDestinationChosen, (void *)apd );

    if  ( ! apd->apdActivateFaxOptions )
	{ appGuiEnableWidget( apd->apdPrinterOptions[opt], 0 ); }

    apd->apdFaxOptionNr= opt++;
    }

    /*  4  */
    {
    int		ltxt= strlen( apd->apdPrintersUpText );
    int		lval= strlen( apd->apdVisiblePrinterCountText );
    char *	scratch;

    scratch= malloc( ltxt+ lval+ 1 );
    if  ( ! scratch )
	{
	LLXDEB(ltxt,lval,scratch);
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apd->apdPrintersUpText,
					appPrintersUpChosen, (void *)apd );
	}
    else{
	int	pos;

	for ( pos= 0; pos < ltxt; pos++ )
	    {
	    if  ( apd->apdPrintersUpText[pos+ 0] == '%'	&&
		  apd->apdPrintersUpText[pos+ 1] == 'd'	)
		{ break;	}
	    }

	strncpy( scratch, apd->apdPrintersUpText, pos );
	strncpy( scratch+ pos, apd->apdVisiblePrinterCountText, lval );

	if  ( pos < ltxt )
	    { strcpy( scratch+ pos+ lval ,apd->apdPrintersUpText+ pos+ 2 ); }
	else{ strcpy( scratch+ pos+ lval ,apd->apdPrintersUpText+ pos );    }
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					scratch,
					appPrintersUpChosen, (void *)apd );
	free( scratch );
	}

    apd->apdUpOptionNr= opt++;
    }

    /*  5  */
    apd->apdFirstPrinterOptionNr= opt;

    for ( prt= 0; prt < printerCount; pd++, opt++, prt++ )
	{
	apd->apdPrinterOptions[opt]=
			appAddItemToOptionmenu( &(apd->apdPrinterOptionmenu),
			pd->pdPrinterName, appDestinationChosen,
			(void *)apd );

	if  ( prt == defaultPrinter )
	    {
	    optionChosen= opt;
	    firstPrinter= prt;
	    }
	if  ( prt == 0 )
	    {
	    optionChosen= opt;
	    firstPrinter= prt;
	    }
	}

    /*  6  */
    {
    int		ltxt= strlen( apd->apdPrintersDownText );
    int		lval= strlen( apd->apdVisiblePrinterCountText );
    char *	scratch;

    scratch= malloc( ltxt+ lval+ 1 );
    if  ( ! scratch )
	{
	LLXDEB(ltxt,lval,scratch);
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apd->apdPrintersDownText,
					appPrintersDownChosen, (void *)apd );
	}
    else{
	int	pos;

	for ( pos= 0; pos < ltxt; pos++ )
	    {
	    if  ( apd->apdPrintersDownText[pos+ 0] == '%'	&&
		  apd->apdPrintersDownText[pos+ 1] == 'd'	)
		{ break;	}
	    }

	strncpy( scratch, apd->apdPrintersDownText, pos );
	strncpy( scratch+ pos, apd->apdVisiblePrinterCountText, lval );

	if  ( pos < ltxt )
	    { strcpy( scratch+ pos+ lval ,apd->apdPrintersDownText+ pos+ 2 ); }
	else{ strcpy( scratch+ pos+ lval ,apd->apdPrintersDownText+ pos );    }

	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					scratch,
					appPrintersDownChosen, (void *)apd );
	free( scratch );
	}

    apd->apdDownOptionNr= opt++;
    }

    apd->apdPrintOptionCount= opt;
    
    apd->apdPrinterCount= printerCount;
    apd->apdOptionNrChosen= optionChosen;
    apd->apdFirstVisiblePrinter= firstPrinter;

    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );
    appPrintDialogMenuRange( apd );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Fill the list of possible page placements.				*/
/*									*/
/************************************************************************/

static void appPrintDialogFillPlacementMenu( AppPrintDialog *	apd )
    {
    int				i;

    PlacementSpecification *	ps;

    appEmptyOptionmenu( &(apd->apdPlacementOptionmenu) );

    ps= apd->apdPlacements;
    for ( i= 0; i < PSnum__COUNT; ps++, i++ )
	{
	ps->psOptionWidget= appAddItemToOptionmenu(
				&(apd->apdPlacementOptionmenu),
				ps->psOptionText,
				appPrintDialogPlacementChosen, (void *)apd );
	}

    apd->apdPlacementChosen= 0;
    apd->apdRotate90Chosen= 0;
    apd->apdCenterHChosen= 0;

    appSetOptionmenu( &(apd->apdPlacementOptionmenu), 0 );

    appOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Fill the list of selection options.					*/
/*									*/
/************************************************************************/

#   define	PRINTselAll_PAGES	0
#   define	PRINTselPAGE_RANGE	1
#   define	PRINTselCURRENT_PAGE	2

static void appPrintDialogFillSelectionMenu( AppPrintDialog *	apd )
    {
    appEmptyOptionmenu( &(apd->apdSelectionOptionmenu) );

    apd->apdSelectionOptions[PRINTselAll_PAGES]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apd->apdAllPagesText,
					    appSelectionChosen, (void *)apd );
    apd->apdSelectionOptions[PRINTselPAGE_RANGE]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apd->apdPageRangeText,
					    appSelectionChosen, (void *)apd );
    apd->apdSelectionOptions[PRINTselCURRENT_PAGE]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apd->apdCurrentPageText,
					    appSelectionChosen, (void *)apd );

    apd->apdSelectionChosen= PRINTselAll_PAGES;

    appSetOptionmenu( &(apd->apdSelectionOptionmenu),
						    apd->apdSelectionChosen );

    appOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Protocol callback: The print dialog is closed through the window	*/
/*  manager. Interpret this as 'Cancel'					*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( appClosePrintDialog, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appGuiBreakDialog( &(apd->apdDialog), AQDrespCANCEL );

    return;
    }

static APP_DESTROY_CALLBACK_H( appDestroyPrintDialog, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appCleanPaperChooser( &(apd->apdPaperChooser) );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame for selecting a printer.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakePrinterFrame( APP_WIDGET	parent,
					AppPrintDialog *	apd )
    {
    APP_WIDGET	printerFrame;
    APP_WIDGET	frameColumn;

    APP_WIDGET	row;

    const int	heightResizable= 0;
    const int	textEnabled= 0;
    const int	textColumn= 1;
    const int	textColspan= 1;

    appMakeColumnFrameInColumn( &printerFrame, &frameColumn,
						parent, apd->apdPrinterText );

    row= appMakeRowInColumn( frameColumn, 2, heightResizable );

    appMakeOptionmenuInRow( &(apd->apdPrinterOptionmenu), row, 0, 1 );

    appMakeTextInRow( &(apd->apdFaxNumberText), row, textColumn,
					    textColspan, 15, textEnabled );

    appGuiSetTypingCallbackForText( apd->apdFaxNumberText,
					    appPrintFaxChanged, (void *)apd );

    return printerFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame for giving page placement.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakePlacementFrame( APP_WIDGET	parent,
					EditApplication *	ea,
					AppPrintDialog *	apd )
    {
    APP_WIDGET		placementFrame;
    APP_WIDGET		frameColumn;

    const int		heightResizable= 0;

    appMakeColumnFrameInColumn( &placementFrame, &frameColumn, parent,
						    apd->apdPlacementText );

    appMakeOptionmenuInColumn( &(apd->apdPlacementOptionmenu), frameColumn );

    apd->apdCustomTransformRow=
			appMakeRowInColumn( frameColumn, 2, heightResizable );
    apd->apdRotate90Toggle= appMakeToggleInRow( apd->apdCustomTransformRow,
				apd->apdRotate90Text,
				appPrintDialogRotate90Toggled, (void *)apd, 0 );
    apd->apdCenterHToggle= appMakeToggleInRow( apd->apdCustomTransformRow,
				apd->apdCenterHText,
				appPrintDialogCenterHToggled, (void *)apd, 1 );

    apd->apdPageDrawing= (APP_WIDGET)0;
    if  ( apd->apdShowPlacementDrawingInt >= 0 )
	{
	apd->apdPageDrawing= appMakePageDrawing( parent, ea, DRH_MM,
					appPrintDialogDrawPage, (void *)apd );
	}

    return placementFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame to select what pages are to be printed.		*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeSelectionFrame( APP_WIDGET	parent,
					AppPrintDialog *	apd )
    {
    APP_WIDGET		selectionFrame;
    APP_WIDGET		frameColumn;

    APP_WIDGET		pageSelectionRow;
    APP_WIDGET		row;
    APP_WIDGET		leftColumn;
    APP_WIDGET		rightColumn;
    APP_WIDGET		fromLabel;
    APP_WIDGET		toLabel;

    const int		heightResizable= 0;
    const int		textEnabled= 0;
    const int		colspan= 1;

    appMakeColumnFrameInColumn( &selectionFrame, &frameColumn, parent,
						    apd->apdSelectionText );

    appMakeOptionmenuInColumn( &(apd->apdSelectionOptionmenu), frameColumn );

    pageSelectionRow= appMakeRowInColumn( frameColumn, 2, heightResizable );

    appMakeColumnInRow( &leftColumn, pageSelectionRow, 0, colspan );
    appMakeColumnInRow( &rightColumn, pageSelectionRow, 1, colspan );

    appMakeLabelAndTextRow( &row, &fromLabel, &(apd->apdPageFromTextWidget),
			    leftColumn, apd->apdFromPageText, 8, textEnabled );

    appMakeLabelAndTextRow( &row, &toLabel, &(apd->apdPageToTextWidget),
			    rightColumn, apd->apdToPageText, 8, textEnabled );

    return selectionFrame;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeButtonRow(	APP_WIDGET		parent,
						AppPrintDialog *	apd )
    {
    APP_WIDGET		row;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(apd->apdPrintButton), row, apd->apdPrintText,
			appPrintDialogPrintPushed, (void *)apd, 0, 1 );

    appMakeButtonInRow( &(apd->apdCancelButton), row, apd->apdCancelText,
			appPrintDialogCancelPushed, (void *)apd, 1, 0 );

    appGuiSetCancelButtonForDialog( &(apd->apdDialog),
						    apd->apdCancelButton );
    return row;
    }

/************************************************************************/
/*									*/
/*  make a page tool.							*/
/*									*/
/************************************************************************/

static AppPrintDialog * appMakePrintDialog( EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    AppPrintDialog *	apd;
    
    APP_WIDGET		printerFrame;
    APP_WIDGET		selectionFrame;
    APP_WIDGET		paned;
    APP_WIDGET		buttonRow;

    int			i;

    int			high;
    int			wide;
    int			d;

    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;
    
    char		name[128];

    const double	magnification= 1.0;

    if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (AppPrintDialog *)0;	}

    if  ( ea->eaPrintDestinationCount == 0				&&
	  utilPrinterGetPrinters( &(ea->eaPrintDestinationCount),
				    &(ea->eaDefaultPrintDestination),
				    &(ea->eaPrintDestinations),
				    ea->eaCustomPrintCommand,
				    ea->eaCustomPrinterName )		)
	{ LDEB(1); return (AppPrintDialog *)0;	}

    apd= (AppPrintDialog *)malloc(
		    sizeof(AppPrintDialog)+
		    ( ea->eaPrintDestinationCount+ 4 )* sizeof(APP_WIDGET) );
    if  ( ! apd )
	{
	LXDEB(ea->eaPrintDestinationCount,apd);
	return (AppPrintDialog *)0;
	}

    apd->apdCustomTransformRow= (APP_WIDGET)0;
    apd->apdRotate90Toggle= (APP_WIDGET)0;
    apd->apdCenterHToggle= (APP_WIDGET)0;

    appInitDrawingData( &(apd->apdDrawingData) );

    for ( i= 0; i < PSnum__COUNT; i++ )
	{ apd->apdPlacements[i]= APP_Placements[i];	}

    appGuiGetResourceValues( ea, apd,
				    APP_PrintDialogresourceTable,
				    sizeof(APP_PrintDialogresourceTable)/
				    sizeof(AppConfigurableResource) );

    apd->apdShowPlacementDrawingInt= 0;
    if  ( apd->apdShowPlacementDrawingString )
	{
	if  ( ! strcmp( apd->apdShowPlacementDrawingString, "1" ) )
	    { apd->apdShowPlacementDrawingInt=  1;	}
	if  ( ! strcmp( apd->apdShowPlacementDrawingString, "0" ) )
	    { apd->apdShowPlacementDrawingInt= -1;	}

	if  ( apd->apdShowPlacementDrawingInt == 0 )
	    { SDEB(apd->apdShowPlacementDrawingString);	}
	}

    apd->apdVisiblePrinterCount= 0;
    if  ( apd->apdVisiblePrinterCountText )
	{
	char *	past= apd->apdVisiblePrinterCountText;

	apd->apdVisiblePrinterCount=
		strtol( apd->apdVisiblePrinterCountText, &past, 0 );

	while( *past == ' ' )
	    { past++;	}
	if  ( *past )
	    {
	    SDEB(apd->apdVisiblePrinterCountText);
	    apd->apdVisiblePrinterCount= 0;
	    }
	}
    if  ( apd->apdVisiblePrinterCount < 1 )
	{ apd->apdVisiblePrinterCount= 15;	}

    if  ( ea->eaFaxCommand && ea->eaFaxCommand[0] )
	{ apd->apdActivateFaxOptions= 1;	}
    else{ apd->apdActivateFaxOptions= 0;	}

    if  ( apd->apdDefaultPrinter && apd->apdDefaultPrinter[0] )
	{
	PrintDestination *	pd= ea->eaPrintDestinations;

	for ( i= 0; i < ea->eaPrintDestinationCount; pd++, i++ )
	    {
	    if  ( ! strcmp( pd->pdPrinterName, apd->apdDefaultPrinter ) )
		{ ea->eaDefaultPrintDestination= i; break;	}
	    }
	}

    apd->apdTarget= (void *)ea;
    apd->apdPrinterCount= 0;
    apd->apdPrintOptionCount= 0;
    apd->apdToFileOptionNr= -1;
    apd->apdFaxOptionNr= -1;
    apd->apdUnitType= ea->eaUnitInt;

    appInitPaperChooser( &(apd->apdPaperChooser) );

    appMakeVerticalDialog( &(apd->apdDialog), &paned, ea,
						appClosePrintDialog,
						appDestroyPrintDialog,
						(void *)apd,
						ea->eaPrintDialogName );

    appSetShellTitle( apd->apdDialog.adTopWidget,
					printOption, ea->eaApplicationName );

    printerFrame= appPrintDialogMakePrinterFrame( paned, apd );

    appMakePaperChooserWidgets( paned, apd->apdPaperSizeText,
					    apd->apdUnitType,
					    &(apd->apdPaperChooser),
					    appPrintDialogPaperRectChanged,
					    (void *)apd );

    appPrintDialogMakePlacementFrame( paned, ea, apd );
    selectionFrame= appPrintDialogMakeSelectionFrame( paned, apd );
    buttonRow= appPrintDialogMakeButtonRow( paned, apd );

    appPrintDialogFillPrinterMenu( apd,
				    ea->eaPrintDestinationCount,
				    ea->eaDefaultPrintDestination,
				    ea->eaPrintDestinations );

    appPrintDialogFillPlacementMenu( apd );
    appPrintDialogFillSelectionMenu( apd );

    appPaperChooserFillMenu( &(apd->apdPaperChooser),
					    apd->apdCustomPaperSizeText );

    appGuiShowDialog( &(apd->apdDialog), ed->edToplevel.atTopWidget );

    appPaperChooserRetreshMenuWidth( &(apd->apdPaperChooser) );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );
    appOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
    appOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );

    if  ( apd->apdPageDrawing )
	{
	AppDrawingData *	add= &(apd->apdDrawingData);

	if  ( appSetDrawingDataForWidget( apd->apdPageDrawing,
							magnification, add ) )
	    { LDEB(1);	}

	appDrawGetSizeOfWidget( &wide, &high, apd->apdPageDrawing );

	d= high/ 24;

	for ( i= 0; i <= d; i++ )
	    {
	    /**/
	    sprintf( name, ea->eaPrintDialogFont, high/ 12- i );
	    apd->apdFontStruct= appDrawOpenFont( add, name );

	    if  ( apd->apdFontStruct )
		{ appDrawSetFont( add, apd->apdFontStruct ); break; }

	    /**/
	    sprintf( name, ea->eaPrintDialogFont, high/ 12+ i );
	    apd->apdFontStruct= appDrawOpenFont( add, name );

	    if  ( apd->apdFontStruct )
		{ appDrawSetFont( add, apd->apdFontStruct ); break; }
	    }

	if  ( ! apd->apdFontStruct )
	    { SLXDEB(ea->eaPrintDialogFont,high/12,apd->apdFontStruct);	}
	}

    return apd;
    }

/************************************************************************/
/*									*/
/*  Show the 'Print...' dialog.						*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Default destination is set by appMakePrintDialog().		*/
/*  3)	Make Fax View by Martin Vermeer happy.				*/
/*									*/
/************************************************************************/

void appRunPrintDialog(			EditDocument *		ed,
					const DocumentGeometry * dgDocument,
					int			pageCount,
					int			firstSelected,
					int			lastSelected,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    EditApplication *		ea= ed->edApplication;
    AppPrintDialog *		apd= (AppPrintDialog *)ea->eaPrintDialog;

    PrintJob			pj;
    PrintGeometry		pg;

    appPrintJobForEditDocument( &pj, ed );

    /*  1  */
    if  ( ! apd )
	{
	apd= appMakePrintDialog( ea, ed, printOption, pixmapName );

	if  ( ! apd )
	    { XDEB(apd); return;	}

	ea->eaPrintDialog= (void *)apd;

	/*  2  */

	/*  3  */
	if  ( apd->apdActivateFaxOptions )
	    {
	    const char *	faxNumber= getenv( "APP_FAX_TO" );

	    if  ( faxNumber )
		{
		appStringToTextWidget( apd->apdFaxNumberText, faxNumber );
		apd->apdOptionNrChosen= apd->apdFaxOptionNr;
		}
	    }
	}
    else{
	appSetShellTitle( apd->apdDialog.adTopWidget, printOption,
						    ea->eaApplicationName );

	appGuiShowDialog( &(apd->apdDialog), ed->edToplevel.atTopWidget );
	}

    apd->apdPrinterGeometry= ea->eaDefaultDocumentGeometry;
    apd->apdDocumentGeometry= *dgDocument;

    apd->apdPageCount= pageCount;
    apd->apdFirstSelectedPage= firstSelected;
    apd->apdLastSelectedPage= lastSelected;
    apd->apdFirstPageChosen= 0;
    apd->apdLastPageChosen= apd->apdPageCount- 1;

    apd->apdSelectionChosen= PRINTselAll_PAGES;
    appSetOptionmenu( &(apd->apdSelectionOptionmenu),
						apd->apdSelectionChosen );
    appPrintDialogShowFromTo( apd, 1, apd->apdPageCount, 0 );

    appGuiEnableWidget( apd->apdSelectionOptions[PRINTselCURRENT_PAGE],
			    ( apd->apdFirstSelectedPage >= 0	&&
			      apd->apdLastSelectedPage >= 0	)	);

    appSetOptionmenu( &(apd->apdPrinterOptionmenu),
						apd->apdOptionNrChosen );
    appPrinterAdaptToDestination( apd );

    appPaperChooserAdaptToGeometry( &(apd->apdPaperChooser),
						&(apd->apdPrinterGeometry) );

    appGuiRunDialog( &(apd->apdDialog), AQDrespNONE, ea );

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{
	LLDEB(apd->apdPlacementChosen,PSnum__COUNT);
	appGuiHideDialog( &(apd->apdDialog) );
	return;
	}

    if  ( appPrintDialogPrintGeometry( &pg, apd ) )
	{ appGuiHideDialog( &(apd->apdDialog) ); return; }

    switch( apd->apdDialog.adResponse )
	{
	case AQDrespOK:
	    if  ( apd->apdOptionNrChosen == apd->apdToFileOptionNr )
		{
		appDocPrintToFile( printOption, apd->apdDialog.adTopWidget,
						ed, &pg,
						apd->apdFirstPageChosen,
						apd->apdLastPageChosen );
		}
	    else{
		if  ( apd->apdOptionNrChosen == apd->apdFaxOptionNr )
		    {
		    char *	faxNumber;
		    char *	s;

		    s= faxNumber=
			appGetStringFromTextWidget( apd->apdFaxNumberText );

		    while( *s == ' ' )
			{ s++;	}

		    if  ( ! *s )
			{ SDEB(faxNumber);	}
		    else{
			if  ( appFaxDocument( ed, s, &pg,
						apd->apdFirstPageChosen,
						apd->apdLastPageChosen ) )
			    { SDEB(faxNumber);	}
			}

		    appFreeStringFromTextWidget( faxNumber );
		    }
		else{
		    int		printer;

		    printer=
			apd->apdOptionNrChosen- apd->apdFirstPrinterOptionNr;

		    if  ( appPrintDocument( printer, &pj, &pg,
						    apd->apdFirstPageChosen,
						    apd->apdLastPageChosen ) )
			{ LDEB(apd->apdOptionNrChosen); }
		    }
		}

	    appGuiHideDialog( &(apd->apdDialog) );
	    return;

	default:
	    LDEB(apd->apdDialog.adResponse);
	    goto error;

	case AQDrespCANCEL:
	error:
	    appGuiHideDialog( &(apd->apdDialog) );
	    return;
	}

    }
