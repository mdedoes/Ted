/************************************************************************/
/*									*/
/*  Print Dialog.							*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appEditApplication.h>
#   include	<appPrintDocument.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	"appPaperChooser.h"
#   include	<psNup.h>
#   include	<appPrintJob.h>
#   include	<guiToolUtil.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	"appToolDrawPage.h"
#   include	<drawDrawingSurfacePrivate.h>
#   include	<guiTextUtil.h>
#   include	<utilPropMask.h>
#   include	<utilColor.h>
#   include	<guiWidgets.h>
#   include	<appGuiResource.h>
#   include	"appPrintDialog.h"
#   include	<guiRunDialog.h>
#   include	<utilPrinter.h>

#   include	<appDebugon.h>

#   define	DRH_MM		32

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
    int			psGridRows;
    int			psGridCols;
    int			psGridHorizontal;
    int			psScalePagesToFit;
    const char *	psOptionText;
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
	/* R  C  H  F  "text"		*/
	{  1, 1, 0, 0, "1x1 As Is"	},
	{  1, 1, 1, 1, "1x1 Scale"	},
	{  2, 1, 1, 1, "2x1 Hor"	},
	{  1, 2, 1, 1, "1x2 Hor"	},
	{  2, 2, 1, 1, "2x2 Hor"	},
	{  2, 2, 0, 1, "2x2 Ver"	},
    };

typedef struct AppPrintDialogResources
    {
    char *			apdrPrinterText;
    char *			apdrNoneText;
    char *			apdrCancelText;
    char *			apdrPrintText;

    char *			apdrPrintToFileText;
    char *			apdrVisiblePrinterCountText;
    char *			apdrPrintersUpText;
    char *			apdrPrintersDownText;

    char *			apdrPaperSizeText;
    char *			apdrCustomPaperSizeText;
    char *			apdrPlacementText;
    char *			apdrShowPlacementDrawingString;
    int				apdrShowPlacementDrawingInt;
    char *			apdrRotate90Text;
    char *			apdrCenterHText;

    char *			apdrSelectionText;
    char *			apdrAllPagesText;
    char *			apdrPageRangeText;
    char *			apdrCurrentPageText;
    char *			apdrFromPageText;
    char *			apdrToPageText;

    char *			apdrDefaultPrinter;

    PlacementSpecification	apdrPlacements[PSnum__COUNT];

    char *			apdrOddFacesOnly;
    char *			apdrEvenFacesOnly;
    char *			apdrFacesForward;
    char *			apdrFacesBackward;
    } AppPrintDialogResources;

static AppConfigurableResource APP_PrintDialogresourceTable[]=
{
    APP_RESOURCE( "printDialogPrinter",
	    offsetof(AppPrintDialogResources,apdrPrinterText),
	    "Printer" ),
    APP_RESOURCE( "printDialogNone",
	    offsetof(AppPrintDialogResources,apdrNoneText),
	    "None" ),
    APP_RESOURCE( "printDialogCancel",
	    offsetof(AppPrintDialogResources,apdrCancelText),
	    "Cancel" ),
    APP_RESOURCE( "printDialogPrint",
	    offsetof(AppPrintDialogResources,apdrPrintText),
	    "Print" ),
    APP_RESOURCE( "printDialogPrintToFile",
	    offsetof(AppPrintDialogResources,apdrPrintToFileText),
	    "Print to File" ),

    APP_RESOURCE( "printDialogPaperSize",
	    offsetof(AppPrintDialogResources,apdrPaperSizeText),
	    "Paper Size" ),
    APP_RESOURCE( "printDialogCustomPaperSize",
	    offsetof(AppPrintDialogResources,apdrCustomPaperSizeText),
	    "Custom" ),

    /***/
    APP_RESOURCE( "printDialogPlacement",
	    offsetof(AppPrintDialogResources,apdrPlacementText),
	    "Placement" ),

    APP_RESOURCE( "printDialogShowPlacementDrawing",
	    offsetof(AppPrintDialogResources,apdrShowPlacementDrawingString),
	    "1" ),

    APP_RESOURCE( "printDialog_1_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x1_ASIS].psOptionText),
	    "1 Page/Sheet" ),
    APP_RESOURCE( "printDialog_1_ScaledText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x1_SCALED].psOptionText),
	    "1 Page/Sheet Scale to Fit" ),
    APP_RESOURCE( "printDialog_2Row_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x1_HOR].psOptionText),
	    "2 Pages/Sheet Vertical" ),
    APP_RESOURCE( "printDialog_2Col_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x2_HOR].psOptionText),
	    "2 Pages/Sheet Horizontal" ),
    APP_RESOURCE( "printDialog_4H_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x2_HOR].psOptionText),
	    "4 Pages/Sheet Horizontal" ),
    APP_RESOURCE( "printDialog_4V_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x2_VER].psOptionText),
	    "4 Pages/Sheet Vertical" ),

    APP_RESOURCE( "printDialogRotate90",
	    offsetof(AppPrintDialogResources,apdrRotate90Text),
	    "Rotate 90\302\260" ), /* \302\260 is the utf8 degree sign 0xb0 */
    APP_RESOURCE( "printDialogCenterH",
	    offsetof(AppPrintDialogResources,apdrCenterHText),
	    "Center Horizontally" ),

    /***/
    APP_RESOURCE( "printDialogSelection",
	    offsetof(AppPrintDialogResources,apdrSelectionText),
	    "Selection" ),

    APP_RESOURCE( "printDialogAllPages",
	    offsetof(AppPrintDialogResources,apdrAllPagesText),
	    "All Pages" ),
    APP_RESOURCE( "printDialogPageRange",
	    offsetof(AppPrintDialogResources,apdrPageRangeText),
	    "Page Range" ),
    APP_RESOURCE( "printDialogCurrentPage",
	    offsetof(AppPrintDialogResources,apdrCurrentPageText),
	    "Current Page" ),

    APP_RESOURCE( "printDialogFromPage",
	    offsetof(AppPrintDialogResources,apdrFromPageText),
	    "From" ),
    APP_RESOURCE( "printDialogToPage",
	    offsetof(AppPrintDialogResources,apdrToPageText),
	    "To" ),

    APP_RESOURCE( "defaultPrinter",
	    offsetof(AppPrintDialogResources,apdrDefaultPrinter),
	    "" ),

    APP_RESOURCE( "printDialogPrinterCount",
	    offsetof(AppPrintDialogResources,apdrVisiblePrinterCountText),
	    "15" ),
    APP_RESOURCE( "printDialogPrintersUp",
	    offsetof(AppPrintDialogResources,apdrPrintersUpText),
	    "%d up" ),
    APP_RESOURCE( "printDialogPrintersDown",
	    offsetof(AppPrintDialogResources,apdrPrintersDownText),
	    "%d down" ),

    APP_RESOURCE( "printDialogOddFacesOnly",
	    offsetof(AppPrintDialogResources,apdrOddFacesOnly),
	    "Odd Faces Only" ),
    APP_RESOURCE( "printDialogEvenFacesOnly",
	    offsetof(AppPrintDialogResources,apdrEvenFacesOnly),
	    "Even Faces Only" ),

    APP_RESOURCE( "printDialogFacesForward",
	    offsetof(AppPrintDialogResources,apdrFacesForward),
	    "First to Last" ),
    APP_RESOURCE( "printDialogFacesBackward",
	    offsetof(AppPrintDialogResources,apdrFacesBackward),
	    "Last to First" ),
};

typedef struct AppPrintDialog
    {
    const AppPrintDialogResources *	apdResources;
    AppDialog				apdDialog;

    APP_WIDGET				apdButtonRow;
    APP_WIDGET				apdPrintButton;
    APP_WIDGET				apdCancelButton;

    APP_WIDGET				apdPrinterFrame;
    APP_WIDGET				apdSelectionFrame;

    AppOptionmenu			apdPrinterOptionmenu;
    AppOptionmenu			apdPlacementOptionmenu;
    AppOptionmenu			apdSelectionOptionmenu;

    APP_WIDGET				apdCustomTransformRow;
    APP_WIDGET				apdRotate90Toggle;
    APP_WIDGET				apdCenterHToggle;

    int					apdUnitType;
    PaperChooser			apdPaperChooser;
    double				apdPixelsPerTwip;
    int					apdPageHighMm;

    APP_WIDGET				apdPageDrawing;
    DrawingSurface			apdDrawingSurface;
    RGB8Color				apdBackgroundColor;
    int					apdScreenFont;

    APP_WIDGET				apdPageFromTextWidget;
    APP_WIDGET				apdPageToTextWidget;

    APP_WIDGET				apdFaceSelectionRow;
    APP_WIDGET				apdOddFacesOnlyToggle;
    APP_WIDGET				apdEvenFacesOnlyToggle;

    APP_WIDGET				apdFaceDirectionRow;
    APP_WIDGET				apdDirectionForwardToggle;
    APP_WIDGET				apdDirectionBackwardToggle;

    int					apdPrinterCount;
    int					apdPrintOptionCount;
    int					apdOptionNrChosen;
    int					apdVisiblePrinterCount;

    int					apdToFileOptionNr;
    int					apdFirstPrinterOptionNr;
    int					apdFirstVisiblePrinter;

    DocumentGeometry			apdDocumentGeometry;
    DocumentGeometry			apdPrinterGeometry;
    int					apdPlacementChosen;
    int					apdRotate90Chosen;
    int					apdCenterHChosen;

    int					apdPageCount;
    int					apdFirstSelectedPage;
    int					apdLastSelectedPage;
    int					apdSelectionChosen;
    int					apdFirstPageChosen;
    int					apdLastPageChosen;

    int					apdPrintOddFacesOnly;
    int					apdPrintEvenFacesOnly;
    int					apdPrintSheetsReverse;

    void *				apdTarget;
    } AppPrintDialog;

/************************************************************************/
/*									*/
/*  Show the page numbers of the range of pages to print in the Print	*/
/*  Dialog.								*/
/*									*/
/************************************************************************/

static void appPrintDialogShowFromTo(	AppPrintDialog *	apd,
					int			fromPage,
					int			toPage,
					int			enabled )
    {
    guiIntegerToText( apd->apdPageFromTextWidget, fromPage );
    guiIntegerToText( apd->apdPageToTextWidget, toPage );

    guiEnableText( apd->apdPageFromTextWidget, enabled );
    guiEnableText( apd->apdPageToTextWidget, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  A printer was chosen.						*/
/*									*/
/************************************************************************/

static void appPrinterAdaptToDestination(	AppPrintDialog *	apd )
    {
    guiEnableWidget( apd->apdPrintButton, 1 );

    guiSetDefaultButtonForDialog( &(apd->apdDialog), apd->apdPrintButton );
    }


static void appDestinationChosen(	int		n,
					void *		voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    if  ( n < 0 || n >= apd->apdPrintOptionCount )
	{ LLDEB(n,apd->apdPrintOptionCount); return;	}

    apd->apdOptionNrChosen= n;

    appPrinterAdaptToDestination( apd );

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
    if  ( ! apd->apdPageDrawing )
	{ return;	}

    guiExposeDrawingWidget( apd->apdPageDrawing );
    }

/************************************************************************/
/*									*/
/*  Retrieve the print geometry that is currently set.			*/
/*									*/
/************************************************************************/

static int appPrintDialogPrintGeometry(
				    int *			pCustRelevant,
				    PrintGeometry *		pg,
				    AppPrintDialog *		apd )
    {
    int					customTransformRelevant= 1;

    const AppPrintDialogResources *	apdr= apd->apdResources;
    const PlacementSpecification *	ps;

    PropertyMask			updMask;

    if  ( appPaperChooserGetSize( &updMask,
				    &(apd->apdPaperChooser),
				    &(apd->apdPrinterGeometry) ) )
	{ return -1;	}

    ps= apdr->apdrPlacements+ apd->apdPlacementChosen;

    pg->pgSheetGeometry= apd->apdPrinterGeometry;

    pg->pgRotatePage90= apd->apdRotate90Chosen;
    pg->pgCenterPageHorizontally= apd->apdCenterHChosen;

    if  ( ps->psGridRows * ps->psGridCols != 1 || ps->psScalePagesToFit )
	{
	customTransformRelevant= 0;
	pg->pgRotatePage90= 0;
	pg->pgCenterPageHorizontally= 0;
	}

    pg->pgGridRows= ps->psGridRows;
    pg->pgGridCols= ps->psGridCols;
    pg->pgGridHorizontal= ps->psGridHorizontal;
    pg->pgScalePagesToFit= ps->psScalePagesToFit;

    pg->pgPrintOddSides= ! apd->apdPrintEvenFacesOnly;
    pg->pgPrintEvenSides= ! apd->apdPrintOddFacesOnly;
    pg->pgPrintSheetsReverse= apd->apdPrintSheetsReverse;
    pg->pgPrintBookletOrder= 0;

    *pCustRelevant= customTransformRelevant;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt Custom Transform Row to other settings.			*/
/*									*/
/************************************************************************/

static void appPrintDialogRefreshCustomTransform(
				    AppPrintDialog *	apd )
    {
    int			customTransformRelevant= 1;
    PrintGeometry	pg;

    int			fitsWithRot= 0;
    int			fitsWithoutRot= 0;

    if  ( ! apd->apdCustomTransformRow )
	{ XDEB(apd->apdCustomTransformRow); return;	}

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, &pg, apd ) )
	{ LDEB(1);	}

    psNupFitPagesToSheet( &fitsWithoutRot, &fitsWithRot,
					    &pg, &(apd->apdDocumentGeometry) );

    if  ( ! customTransformRelevant )
	{
	guiEnableWidget( apd->apdRotate90Toggle, 0 );
	guiEnableWidget( apd->apdCenterHToggle, 0 );

	guiSetToggleState( apd->apdRotate90Toggle, 0 );
	guiSetToggleState( apd->apdCenterHToggle, 0 );
	}
    else{
	int	fitsCentered;

	if  ( apd->apdRotate90Chosen )
	    { fitsCentered= fitsWithRot;	}
	else{ fitsCentered= fitsWithoutRot;	}

	guiEnableWidget( apd->apdRotate90Toggle, fitsWithRot );
	guiEnableWidget( apd->apdCenterHToggle, fitsCentered );

	guiSetToggleState( apd->apdRotate90Toggle,
				    fitsWithRot && apd->apdRotate90Chosen );
	guiSetToggleState( apd->apdCenterHToggle,
				    fitsCentered && apd->apdCenterHChosen );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A placement was chosen.						*/
/*									*/
/************************************************************************/

static void appPrintDialogPlacementChosen(	int		i,
						void *		voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    if  ( i < 0 || i >= PSnum__COUNT )
	{ LLDEB(i,PSnum__COUNT); return;	}

    apd->apdPlacementChosen= i;

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  A selection mode was chosen.					*/
/*									*/
/************************************************************************/

static void appSelectionChosen(		int		n,
					void *		voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

#   if USE_MOTIF
    guiMotifSetFocusToWindow( apd->apdDialog.adTopWidget );
#   endif

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
	    guiEnableText( apd->apdPageFromTextWidget, 1 );
	    guiEnableText( apd->apdPageToTextWidget, 1 );
	    apd->apdSelectionChosen= n;

	    guiFocusToWidget( apd->apdPageFromTextWidget );
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

    guiBreakDialog( &(apd->apdDialog), AQDrespCANCEL );

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

	if  ( guiIntegerFromText( apd->apdPageFromTextWidget,
					&from, 1, 0, apd->apdPageCount, 0 ) )
	    { return;	}

	if  ( guiIntegerFromText( apd->apdPageToTextWidget,
					&to, from, 0, apd->apdPageCount, 1 ) )
	    { return;	}

	apd->apdFirstPageChosen= from- 1;
	apd->apdLastPageChosen= to- 1;
	}

    guiBreakDialog( &(apd->apdDialog), AQDrespOK );

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

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

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

    set= guiGetToggleStateFromCallback( w, e );

    apd->apdRotate90Chosen= ( set != 0 );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogCenterHToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= guiGetToggleStateFromCallback( w, e );

    apd->apdCenterHChosen= ( set != 0 );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  The Odd/Even pages only toggles have been activated.		*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogOddOnlyToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= guiGetToggleStateFromCallback( w, e );

    apd->apdPrintOddFacesOnly= ( set != 0 );

    if  ( set )
	{
	apd->apdPrintEvenFacesOnly= 0;
	guiSetToggleState( apd->apdEvenFacesOnlyToggle,
						apd->apdPrintEvenFacesOnly );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogEvenOnlyToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= guiGetToggleStateFromCallback( w, e );

    apd->apdPrintEvenFacesOnly= ( set != 0 );

    if  ( set )
	{
	apd->apdPrintOddFacesOnly= 0;
	guiSetToggleState( apd->apdOddFacesOnlyToggle,
						apd->apdPrintOddFacesOnly );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  The Forward/Backward pages only toggles have been activated.	*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogForwardToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= guiGetToggleStateFromCallback( w, e );

    if  ( apd->apdPrintSheetsReverse != ( set == 0 ) )
	{
	apd->apdPrintSheetsReverse= ( set == 0 );

	guiSetToggleState( apd->apdDirectionBackwardToggle,
						apd->apdPrintSheetsReverse );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogBackwardToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= guiGetToggleStateFromCallback( w, e );

    if  ( apd->apdPrintSheetsReverse != ( set != 0 ) )
	{
	apd->apdPrintSheetsReverse= ( set != 0 );

	guiSetToggleState( apd->apdDirectionForwardToggle,
						! apd->apdPrintSheetsReverse );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the page placement.			*/
/*									*/
/************************************************************************/

static APP_REDRAW_HANDLER_H( appPrintDialogDrawPage, w, voidapd, exposeEvent )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;

    int				customTransformRelevant= 1;
    PrintGeometry		pg;

    if  ( ! apd->apdPageDrawing )
	{ XDEB(apd->apdPageDrawing); return;	}

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{ LLDEB(apd->apdPlacementChosen,PSnum__COUNT); return;	}

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, &pg, apd ) )
	{ LDEB(1);	}

    appDrawNupDiagram( apd->apdPageDrawing, apd->apdDrawingSurface,
				&(apd->apdBackgroundColor),
				apd->apdScreenFont,
				apd->apdPixelsPerTwip,
				&(apd->apdDocumentGeometry), &pg );
    }

/************************************************************************/
/*									*/
/*  Fill the list of printers.						*/
/*									*/
/*  1)  Empty option menu. It should already be empty.			*/
/*  2)  Insert print to file option.					*/
/*  5)  Insert options for the actual printers.				*/
/*									*/
/************************************************************************/

static int appPrintDialogFillPrinterMenu(
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd,
				int				printerCount,
				int				defaultPrinter,
				PrintDestination *		pd )
    {
    int			opt;
    int			prt;

    int			optionChosen= -1;
    int			firstPrinter= -1;

    /*  1  */
    guiEmptyOptionmenu( &(apd->apdPrinterOptionmenu) );

    opt= 0;

    /*  2  */
    {
    guiAddItemToOptionmenu( &(apd->apdPrinterOptionmenu),
						apdr->apdrPrintToFileText );

    if  ( optionChosen < 0 )
	{ optionChosen= opt;	}

    apd->apdToFileOptionNr= opt++;
    }

    /*  5  */
    apd->apdFirstPrinterOptionNr= opt;

    for ( prt= 0; prt < printerCount; pd++, opt++, prt++ )
	{
	guiAddItemToOptionmenu(
			    &(apd->apdPrinterOptionmenu), pd->pdPrinterName );

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

    apd->apdPrintOptionCount= opt;
    
    apd->apdPrinterCount= printerCount;
    apd->apdOptionNrChosen= optionChosen;
    apd->apdFirstVisiblePrinter= firstPrinter;

    guiSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );

    guiOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the list of possible page placements.				*/
/*									*/
/************************************************************************/

static void appPrintDialogFillPlacementMenu( AppPrintDialog *	apd )
    {
    const AppPrintDialogResources *	apdr= apd->apdResources;
    int					i;

    const PlacementSpecification *	ps;

    guiEmptyOptionmenu( &(apd->apdPlacementOptionmenu) );

    ps= apdr->apdrPlacements;
    for ( i= 0; i < PSnum__COUNT; ps++, i++ )
	{
	guiAddItemToOptionmenu(
			&(apd->apdPlacementOptionmenu), ps->psOptionText );
	}

    apd->apdPlacementChosen= 0;
    apd->apdRotate90Chosen= 0;
    apd->apdCenterHChosen= 0;

    guiSetOptionmenu( &(apd->apdPlacementOptionmenu), 0 );

    guiOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
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
    const AppPrintDialogResources *	apdr= apd->apdResources;

    guiEmptyOptionmenu( &(apd->apdSelectionOptionmenu) );

    guiAddItemToOptionmenu( &(apd->apdSelectionOptionmenu),
					    apdr->apdrAllPagesText );
    guiAddItemToOptionmenu( &(apd->apdSelectionOptionmenu),
					    apdr->apdrPageRangeText );
    guiAddItemToOptionmenu( &(apd->apdSelectionOptionmenu),
					    apdr->apdrCurrentPageText );

    apd->apdSelectionChosen= PRINTselAll_PAGES;

    guiSetOptionmenu( &(apd->apdSelectionOptionmenu),
						    apd->apdSelectionChosen );

    guiOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );
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

    guiBreakDialog( &(apd->apdDialog), AQDrespCLOSED );

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

static APP_WIDGET appPrintDialogMakePrinterFrame(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET	printerFrame;
    APP_WIDGET	frameColumn;

    APP_WIDGET	row;

    const int	heightResizable= 0;

    const int	column= 0;
    const int	colspan= 2;

    guiMakeColumnFrameInColumn( &printerFrame, &frameColumn,
						parent, apdr->apdrPrinterText );

    row= guiMakeRowInColumn( frameColumn, 2, heightResizable );

    guiMakeOptionmenuInRow( &(apd->apdPrinterOptionmenu), row, column, colspan,
					appDestinationChosen, (void *)apd );

    return printerFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame for giving page placement.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakePlacementFrame(
				APP_WIDGET			parent,
				EditApplication *		ea,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET		placementFrame;
    APP_WIDGET		frameColumn;

    const int		heightResizable= 0;

    guiMakeColumnFrameInColumn( &placementFrame, &frameColumn, parent,
						    apdr->apdrPlacementText );

    guiMakeOptionmenuInColumn( &(apd->apdPlacementOptionmenu), frameColumn,
				appPrintDialogPlacementChosen, (void *)apd );

    apd->apdCustomTransformRow=
			guiMakeRowInColumn( frameColumn, 2, heightResizable );
    apd->apdRotate90Toggle= guiMakeToggleInRow( apd->apdCustomTransformRow,
			    apdr->apdrRotate90Text,
			    appPrintDialogRotate90Toggled, (void *)apd, 0, 1 );
    apd->apdCenterHToggle= guiMakeToggleInRow( apd->apdCustomTransformRow,
			    apdr->apdrCenterHText,
			    appPrintDialogCenterHToggled, (void *)apd, 1, 1 );

    apd->apdPageDrawing= (APP_WIDGET)0;

    apd->apdPageHighMm= DRH_MM;
    apd->apdPixelsPerTwip= ( apd->apdPageHighMm* ea->eaPixelsPerTwip )/
								A3_MM_HIGH;

    if  ( apdr->apdrShowPlacementDrawingInt >= 0 )
	{
	apd->apdPageDrawing= appMakePageDrawing( parent,
				ea->eaPixelsPerTwip, apd->apdPageHighMm,
				appPrintDialogDrawPage, (void *)apd );
	}

    return placementFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame to select what pages are to be printed.		*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeSelectionFrame(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
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

    const int		makeCollationWidgets= 0;

    guiMakeColumnFrameInColumn( &selectionFrame, &frameColumn, parent,
						    apdr->apdrSelectionText );

    guiMakeOptionmenuInColumn( &(apd->apdSelectionOptionmenu), frameColumn,
					    appSelectionChosen, (void *)apd );

    /**/
    pageSelectionRow= guiMakeRowInColumn( frameColumn, 2, heightResizable );

    guiMakeColumnInRow( &leftColumn, pageSelectionRow, 0, colspan );
    guiMakeColumnInRow( &rightColumn, pageSelectionRow, 1, colspan );

    guiToolMakeLabelAndTextRow( &row, &fromLabel, &(apd->apdPageFromTextWidget),
			leftColumn, apdr->apdrFromPageText, 8, textEnabled );

    guiToolMakeLabelAndTextRow( &row, &toLabel, &(apd->apdPageToTextWidget),
			rightColumn, apdr->apdrToPageText, 8, textEnabled );

    /**/
    apd->apdFaceSelectionRow= (APP_WIDGET)0;
    apd->apdOddFacesOnlyToggle= (APP_WIDGET)0;
    apd->apdEvenFacesOnlyToggle= (APP_WIDGET)0;

    if  ( makeCollationWidgets )
	{
	apd->apdFaceSelectionRow=
			guiMakeRowInColumn( frameColumn, 2, heightResizable );
	apd->apdOddFacesOnlyToggle= guiMakeToggleInRow(
			    apd->apdFaceSelectionRow,
			    apdr->apdrOddFacesOnly,
			    appPrintDialogOddOnlyToggled, (void *)apd, 0, 1 );
	apd->apdEvenFacesOnlyToggle= guiMakeToggleInRow(
			    apd->apdFaceSelectionRow,
			    apdr->apdrEvenFacesOnly,
			    appPrintDialogEvenOnlyToggled, (void *)apd, 1, 1 );
	}

    /**/
    apd->apdFaceDirectionRow= (APP_WIDGET)0;
    apd->apdDirectionForwardToggle= (APP_WIDGET)0;
    apd->apdDirectionBackwardToggle= (APP_WIDGET)0;

    if  ( makeCollationWidgets )
	{
	apd->apdFaceDirectionRow=
			guiMakeRowInColumn( frameColumn, 2, heightResizable );
	apd->apdDirectionForwardToggle= guiMakeToggleInRow(
			    apd->apdFaceDirectionRow,
			    apdr->apdrFacesForward,
			    appPrintDialogForwardToggled, (void *)apd, 0, 1 );
	apd->apdDirectionBackwardToggle= guiMakeToggleInRow(
			    apd->apdFaceDirectionRow,
			    apdr->apdrFacesBackward,
			    appPrintDialogBackwardToggled, (void *)apd, 1, 1 );
	}

    return selectionFrame;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeButtonRow(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET		row;
    const int		heightResizable= 0;
    const int		colspan= 1;

    row= guiMakeRowInColumn( parent, 2, heightResizable );

    guiMakeButtonInRow( &(apd->apdPrintButton), row, apdr->apdrPrintText,
		    appPrintDialogPrintPushed, (void *)apd, 0, colspan, 1 );

    guiMakeButtonInRow( &(apd->apdCancelButton), row, apdr->apdrCancelText,
		    appPrintDialogCancelPushed, (void *)apd, 1, colspan, 0 );

    guiSetCancelButtonForDialog( &(apd->apdDialog),
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
    AppPrintDialog *		apd;
    
    APP_WIDGET			paned;

    int				i;

    int				high;
    int				wide;

    static int				gotResources;
    static AppPrintDialogResources	apdr;

    if  ( ! ea->eaPrintDestinationsCollected	&&
	  appGetPrintDestinations( ea )		)
	{ LDEB(1); return (AppPrintDialog *)0;	}

    apd= (AppPrintDialog *)malloc(
		    sizeof(AppPrintDialog)+
		    ( ea->eaPrintDestinationCount+ 4 )* sizeof(APP_WIDGET) );
    if  ( ! apd )
	{
	LXDEB(ea->eaPrintDestinationCount,apd);
	return (AppPrintDialog *)0;
	}

    guiInitOptionmenu( &(apd->apdPrinterOptionmenu) );
    guiInitOptionmenu( &(apd->apdPlacementOptionmenu) );
    guiInitOptionmenu( &(apd->apdSelectionOptionmenu) );

    apd->apdCustomTransformRow= (APP_WIDGET)0;
    apd->apdRotate90Toggle= (APP_WIDGET)0;
    apd->apdCenterHToggle= (APP_WIDGET)0;
    apd->apdResources= &apdr;

    apd->apdDrawingSurface= (DrawingSurface)0;

    if  ( ! gotResources )
	{
	for ( i= 0; i < PSnum__COUNT; i++ )
	    { apdr.apdrPlacements[i]= APP_Placements[i];	}

	appGuiGetResourceValues( &gotResources, ea, &apdr,
				    APP_PrintDialogresourceTable,
				    sizeof(APP_PrintDialogresourceTable)/
				    sizeof(AppConfigurableResource) );

	apdr.apdrShowPlacementDrawingInt= 0;
	if  ( apdr.apdrShowPlacementDrawingString )
	    {
	    if  ( ! strcmp( apdr.apdrShowPlacementDrawingString, "1" ) )
		{ apdr.apdrShowPlacementDrawingInt=  1;	}
	    if  ( ! strcmp( apdr.apdrShowPlacementDrawingString, "0" ) )
		{ apdr.apdrShowPlacementDrawingInt= -1;	}

	    if  ( apdr.apdrShowPlacementDrawingInt == 0 )
		{ SDEB(apdr.apdrShowPlacementDrawingString);	}
	    }
	}

    apd->apdVisiblePrinterCount= 0;
    if  ( apdr.apdrVisiblePrinterCountText )
	{
	char *	past= apdr.apdrVisiblePrinterCountText;

	apd->apdVisiblePrinterCount=
		strtol( apdr.apdrVisiblePrinterCountText, &past, 0 );

	while( *past == ' ' )
	    { past++;	}
	if  ( *past )
	    {
	    SDEB(apdr.apdrVisiblePrinterCountText);
	    apd->apdVisiblePrinterCount= 0;
	    }
	}
    if  ( apd->apdVisiblePrinterCount < 1 )
	{ apd->apdVisiblePrinterCount= 15;	}

    if  ( apdr.apdrDefaultPrinter && apdr.apdrDefaultPrinter[0] )
	{
	PrintDestination *	pd= ea->eaPrintDestinations;

	for ( i= 0; i < ea->eaPrintDestinationCount; pd++, i++ )
	    {
	    if  ( ! strcmp( pd->pdPrinterName, apdr.apdrDefaultPrinter ) )
		{ ea->eaDefaultPrintDestination= i; break;	}
	    }
	}

    apd->apdTarget= (void *)ea;
    apd->apdPrinterCount= 0;
    apd->apdPrintOptionCount= 0;
    apd->apdToFileOptionNr= -1;
    apd->apdUnitType= ea->eaUnitInt;

    appInitPaperChooser( &(apd->apdPaperChooser) );

    guiMakeVerticalDialog( &(apd->apdDialog), &paned,
						ea->eaToplevel.atTopWidget,
						appClosePrintDialog,
						appDestroyPrintDialog,
						(void *)apd );

    guiSetDialogTitle( apd->apdDialog.adTopWidget,
					printOption, ea->eaApplicationName );

    apd->apdPrinterFrame= appPrintDialogMakePrinterFrame( paned, &apdr, apd );

    appMakePaperChooserWidgets( paned, apdr.apdrPaperSizeText,
					    apd->apdUnitType,
					    &(apd->apdPaperChooser),
					    appPrintDialogPaperRectChanged,
					    (void *)apd );

    appPrintDialogMakePlacementFrame( paned, ea, &apdr, apd );
    apd->apdSelectionFrame= appPrintDialogMakeSelectionFrame( paned, &apdr, apd );
    apd->apdButtonRow= appPrintDialogMakeButtonRow( paned, &apdr, apd );

    appPrintDialogFillPrinterMenu( &apdr, apd,
				    ea->eaPrintDestinationCount,
				    ea->eaDefaultPrintDestination,
				    ea->eaPrintDestinations );

    appPrintDialogFillPlacementMenu( apd );
    appPrintDialogFillSelectionMenu( apd );

    appPaperChooserFillMenu( &(apd->apdPaperChooser),
					    apdr.apdrCustomPaperSizeText );

    guiShowDialog( ea->eaContext, &(apd->apdDialog), ed->edToplevel.atTopWidget );

    appPaperChooserRefreshMenuWidth( &(apd->apdPaperChooser) );

    guiOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );
    guiOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
    guiOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );

    if  ( apd->apdPageDrawing )
	{
	apd->apdDrawingSurface= guiDrawingSurfaceForNativeWidget(
			    apd->apdPageDrawing,
			    ea->eaPostScriptFontList.psflAvoidFontconfig );

	guiDrawGetSizeOfWidget( &wide, &high, apd->apdPageDrawing );

	apd->apdScreenFont= drawOpenDefaultFont( apd->apdDrawingSurface,
					    &(ea->eaPostScriptFontList),
					    apd->apdPageDrawing, high/ 12 );
	if  ( apd->apdScreenFont < 0 )
	    { LLDEB(high/12,apd->apdScreenFont); }

	guiGetBackgroundColor( &(apd->apdBackgroundColor),
							apd->apdPageDrawing );
	}

    return apd;
    }

/************************************************************************/
/*									*/
/*  Actually execute the print command.					*/
/*									*/
/************************************************************************/

static void appPrintDialogPrint(	EditDocument *		ed,
					APP_WIDGET		printOption,
					PrintJob *		pj,
					PrintGeometry *		pg,
					AppPrintDialog *	apd )
    {
    int					customTransformRelevant= 1;

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{ LLDEB(apd->apdPlacementChosen,PSnum__COUNT); return; }

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, pg, apd ) )
	{ return; }

    pg->pgFirstPage= apd->apdFirstPageChosen;
    pg->pgLastPage= apd->apdLastPageChosen;

    if  ( apd->apdOptionNrChosen == apd->apdToFileOptionNr )
	{
	appDocPrintToFile( printOption, apd->apdDialog.adTopWidget, ed, pg );
	}
    else{
	int		printer;

	printer= apd->apdOptionNrChosen- apd->apdFirstPrinterOptionNr;

	if  ( appPrintDocument( printer, pj ) )
	    { LDEB(apd->apdOptionNrChosen); }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Show the 'Print...' dialog.						*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Default destination is set by appMakePrintDialog().		*/
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
    EditApplication *			ea= ed->edApplication;
    AppPrintDialog *			apd= (AppPrintDialog *)ea->eaPrintDialog;
    const AppPrintDialogResources *	apdr;

    PrintJob				pj;
    PrintGeometry			pg;

    int					i;

    const int				sheetSize= 0;

    psInitPrintGeometry( &pg );

    appPrintJobForEditDocument( &pj, ed, &pg );

    /*  1  */
    if  ( ! apd )
	{
	apd= appMakePrintDialog( ea, ed, printOption, pixmapName );

	if  ( ! apd )
	    { XDEB(apd); return;	}

	ea->eaPrintDialog= (void *)apd;
	}
    else{
	guiSetDialogTitle( apd->apdDialog.adTopWidget, printOption,
						    ea->eaApplicationName );

	guiShowDialog( ea->eaContext, &(apd->apdDialog), ed->edToplevel.atTopWidget );
	}

    apdr= apd->apdResources;

    apd->apdPrinterGeometry= ea->eaDefaultDocumentGeometry;
    apd->apdDocumentGeometry= *dgDocument;

    apd->apdPageCount= pageCount;
    apd->apdFirstSelectedPage= firstSelected;
    apd->apdLastSelectedPage= lastSelected;
    apd->apdFirstPageChosen= 0;
    apd->apdLastPageChosen= apd->apdPageCount- 1;
    apd->apdPrintOddFacesOnly= 0;
    apd->apdPrintEvenFacesOnly= 0;
    apd->apdPrintSheetsReverse= 0;

    psInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );
    pg.pgSheetGeometry= apd->apdPrinterGeometry;

    if  ( ea->eaSuggestPageSetup )
	{ (*ea->eaSuggestPageSetup)( &pg, ed->edPrivateData, sheetSize ); }

    for ( i= 0; i < PSnum__COUNT; i++ )
	{
	if  ( apdr->apdrPlacements[i].psGridRows == pg.pgGridRows	&&
	      apdr->apdrPlacements[i].psGridCols == pg.pgGridCols	&&
	      apdr->apdrPlacements[i].psGridHorizontal ==
						pg.pgGridHorizontal	&&
	      apdr->apdrPlacements[i].psScalePagesToFit ==
						pg.pgScalePagesToFit	)
	    { apd->apdPlacementChosen= i; break; }
	}

    guiSetOptionmenu( &(apd->apdPlacementOptionmenu),
						apd->apdPlacementChosen );

    apd->apdSelectionChosen= PRINTselAll_PAGES;
    guiSetOptionmenu( &(apd->apdSelectionOptionmenu),
						apd->apdSelectionChosen );
    appPrintDialogShowFromTo( apd, 1, apd->apdPageCount, 0 );

    guiOptionMenuEnablePosition( &(apd->apdSelectionOptionmenu),
			    PRINTselCURRENT_PAGE,
			    ( apd->apdFirstSelectedPage >= 0	&&
			      apd->apdLastSelectedPage >= 0	)	);

    guiSetOptionmenu( &(apd->apdPrinterOptionmenu),
						apd->apdOptionNrChosen );

    if  ( apd->apdOddFacesOnlyToggle )
	{
	guiSetToggleState( apd->apdOddFacesOnlyToggle,
				    apd->apdPrintOddFacesOnly );
	}
    if  ( apd->apdEvenFacesOnlyToggle )
	{
	guiSetToggleState( apd->apdEvenFacesOnlyToggle,
				    apd->apdPrintEvenFacesOnly );
	}

    if  ( apd->apdDirectionBackwardToggle )
	{
	guiSetToggleState( apd->apdDirectionBackwardToggle,
						apd->apdPrintSheetsReverse );
	}
    if  ( apd->apdDirectionForwardToggle )
	{
	guiSetToggleState( apd->apdDirectionForwardToggle,
						! apd->apdPrintSheetsReverse );
	}

    appPrinterAdaptToDestination( apd );

    appPaperChooserAdaptToGeometry( &(apd->apdPaperChooser),
						&(apd->apdPrinterGeometry) );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    guiRunDialog( &(apd->apdDialog), AQDrespNONE, ea->eaContext );

    switch( apd->apdDialog.adResponse )
	{
	case AQDrespOK:
	    appPrintDialogPrint( ed, printOption, &pj, &pg, apd );
	    guiHideDialog( &(apd->apdDialog) );
	    goto ready;

	default:
	    LDEB(apd->apdDialog.adResponse);
	    goto error;

	case AQDrespCLOSED:
	    goto ready;

	case AQDrespCANCEL:
	error:
	    guiHideDialog( &(apd->apdDialog) );
	    goto ready;
	}

  ready:
    psCleanPrintGeometry( &pg );
    return;
    }
