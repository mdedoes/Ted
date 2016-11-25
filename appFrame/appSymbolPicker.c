/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appSymbolPicker.h>
#   include	<appEncodingMenu.h>
#   include	<appDraw.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the Symbol Picker.					*/
/*									*/
/************************************************************************/

typedef struct AppSymbolPickerResources
    {
    char *	asprInsert;
    char *	asprFont;
    char *	asprNone;
    char *	asprEncodings[ENCODINGps_COUNT];
    } AppSymbolPickerResources;

static AppConfigurableResource APP_SymbolPickerResourceTable[]=
    {
    APP_RESOURCE( "symbolPickerFont",
		offsetof(AppSymbolPickerResources,asprFont),
		"Font, Encoding" ),
    APP_RESOURCE( "symbolPickerNone",
		offsetof(AppSymbolPickerResources,asprNone),
		"None" ),
    APP_RESOURCE( "symbolPickerInsert",
		offsetof(AppSymbolPickerResources,asprInsert),
		"Insert" ),
    };

/************************************************************************/
/*									*/
/*  Represents a symbol picker.						*/
/*									*/
/************************************************************************/

typedef struct AppSymbolPicker
    {
    EditApplication *		aspApplication;

    AppSymbolPickerResources *	aspResources;

    APP_WIDGET			aspTopWidget;
    APP_WIDGET			aspMainWidget;

    APP_WIDGET			aspFontFrame;
    AppOptionmenu		aspFontOptionmenu;
    AppEncodingMenu		aspEncodingMenu;

    APP_WIDGET			aspSymbolDrawing;
    APP_WIDGET			aspInsertButton;

    AppToolDestroy		aspDestroy;
    SymbolPickerInsert		aspInsert;
    void *			aspTarget;

    AppFontFamily *		aspFontFamilies;
    int				aspFontFamilyCount;

    int				aspSymbolSelected;

    ExpandedTextAttribute	aspTextAttribute;
    int				aspFontFamilyChosen;

    APP_FONT *			aspFont;
    AppDrawingData		aspDrawingData;
    int				aspCellSizePixels;
    int				aspCellsWide;
    int				aspCellsHigh;

    APP_WIDGET			aspFontFamilyOptions[1]; /* LAST! */
    } AppSymbolPicker;

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font.			*/
/*									*/
/*  9)  Also remember that no symbol has been selected yet, and that	*/
/*	for that reason, no symbol can be inserted.			*/
/*									*/
/************************************************************************/

static int appSymbolAdaptDrawingToFont(		AppSymbolPicker *	asp )
    {
    AppDrawingData *		add= &(asp->aspDrawingData);
    char			scratch[120];
    APP_FONT *			xfs;
    const AppFontFamily *	aff;
    ExpandedTextAttribute *	eta= &(asp->aspTextAttribute);

    const int			twipsSize= 20* 10;

    if  ( asp->aspFontFamilyChosen < 0 )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    aff= asp->aspFontFamilies+ asp->aspFontFamilyChosen;
    if  ( appFontXFont( scratch, add, aff, eta->etaFontEncoding,
				aff->affFaces, twipsSize, DOCfontREGULAR ) )
	{ SLDEB(aff->affFontFamilyName,eta->etaFontEncoding); return- 1; }

    xfs= appDrawOpenFont( add, scratch );
    if  ( ! xfs )
	{ SXDEB(scratch,xfs); return -1;	}

    if  ( asp->aspFont )
	{ appDrawFreeFont( add, asp->aspFont );	}
    asp->aspFont= xfs;

    appDrawSetFont( add, asp->aspFont );

    appExposeRectangle( add, 0, 0, 0, 0 );

    /*  9  */
    asp->aspSymbolSelected= -1;
    appGuiEnableWidget( asp->aspInsertButton, 0 );

    return 0;
    }

static int appSymbolAdaptToFamily(  AppSymbolPicker *	asp )
    {
    if  ( asp->aspFontFamilyChosen < 0 )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    appEncodingMenuAdaptToFamilyEncodings( &(asp->aspEncodingMenu),
			    asp->aspFontFamilies+ asp->aspFontFamilyChosen );

    if  ( appSymbolAdaptDrawingToFont( asp ) )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  'Insert' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appSymbolPickerInsertSymbol(	AppSymbolPicker *	asp )
    {
    PropertyMask	setMask;

    PROPmaskCLEAR( &setMask );

    if  ( asp->aspSymbolSelected < 0 || ! asp->aspInsert )
	{ LXDEB(asp->aspSymbolSelected,asp->aspInsert); return;	}

    if  ( asp->aspFontFamilyChosen >= 0 )
	{ PROPmaskADD( &setMask, TApropFONTFAMILY );	}

    (*asp->aspInsert)( asp->aspTarget, asp->aspSymbolSelected,
				    &(asp->aspTextAttribute), &setMask );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolInsertPushed, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appSymbolPickerInsertSymbol( asp );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the symbol part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolDrawSymbol(	AppSymbolPicker *	asp,
					int			sym,
					int			x0,
					int			y0 )
    {
    AppDrawingData *			add= &(asp->aspDrawingData);
    char				scratch[2];

    int					wide;
    int					fontAscent;
    int					fontDescent;

    int					x;
    int					y;

    scratch[0]= sym;
    scratch[1]= '\0';

    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
					    add, asp->aspFont, scratch, 1 );

    y= ( asp->aspCellSizePixels- fontAscent- fontDescent+ 1 )/ 2;
    y= y0+ y+ fontAscent;
    x= x0+ asp->aspCellSizePixels/ 2- wide/2;

    appDrawDrawString( add, x, y, scratch, 1 );

    return;
    }

static void appSymbolRectangle(		DocumentRectangle *	drSym,
					const AppSymbolPicker *	asp,
					int			row,
					int			col )
    {
    drSym->drX0= col* ( asp->aspCellSizePixels+ 1 )+ 1;
    drSym->drX1= drSym->drX0+ asp->aspCellSizePixels- 1;

    drSym->drY0= row* ( asp->aspCellSizePixels+ 1 )+ 1;
    drSym->drY1= drSym->drY0+ asp->aspCellSizePixels- 1;

    return;
    }

static APP_EVENT_HANDLER_H( appSymbolRedraw, w, voidasp, exposeEvent )
    {
    AppSymbolPicker *			asp= (AppSymbolPicker *)voidasp;
    AppDrawingData *			add= &(asp->aspDrawingData);

    DocumentRectangle			drClip;
    DocumentRectangle			drSym;
    DocumentRectangle			drIgn;

    int					col;
    int					row;
    int					sym;

    int					x0;
    int					y0;
    int					wide;
    int					high;

    const int				ox= 0;
    const int				oy= 0;

    if  ( ! asp->aspFont )
	{ XDEB(asp->aspFont); return;	}

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundColor( add, &(add->addBackColor) );
    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    appDrawSetForegroundBlack( add );

    y0= 0;
    wide= 1;
    high= asp->aspCellsHigh* ( asp->aspCellSizePixels+ 1 )+ 1;

    for ( col= 0; col <= asp->aspCellsWide; col++ )
	{
	x0= col* ( asp->aspCellSizePixels+ 1 );

	appDrawFillRectangle( add, x0, y0, wide, high );
	}

    x0= 0;
    wide= asp->aspCellsWide* ( asp->aspCellSizePixels+ 1 )+ 1;
    high= 1;

    for ( row= 0; row <= asp->aspCellsHigh; row++ )
	{
	y0= row* ( asp->aspCellSizePixels+ 1 );

	appDrawFillRectangle( add, x0, y0, wide, high );
	}

    appDrawSetForegroundWhite( add );

    wide= high= asp->aspCellSizePixels;
    for ( row= 0; row < asp->aspCellsHigh; row++ )
	{
	for ( col= 0; col < asp->aspCellsWide; col++ )
	    {
	    appSymbolRectangle( &drSym, asp, row, col );

	    sym= row* asp->aspCellsWide+ col;

	    if  ( ! docIntersectRectangle( &drSym, &drSym, &drClip )	||
		  ! appCharExistsInFont( asp->aspFont, sym )		||
		  sym == asp->aspSymbolSelected				)
		{ continue;	}

	    appDrawFillRectangle( add, drSym.drX0, drSym.drY0,
					    drSym.drX1- drSym.drX0+ 1,
					    drSym.drY1- drSym.drY0+ 1 );
	    }
	}

    appDrawSetForegroundBlack( add );

    wide= high= asp->aspCellSizePixels;
    for ( row= 0; row < asp->aspCellsHigh; row++ )
	{
	for ( col= 0; col < asp->aspCellsWide; col++ )
	    {
	    appSymbolRectangle( &drSym, asp, row, col );

	    sym= row* asp->aspCellsWide+ col;

	    if  ( ! docIntersectRectangle( &drIgn, &drSym, &drClip )	||
		  ! appCharExistsInFont( asp->aspFont, sym )		||
		  sym == asp->aspSymbolSelected				)
		{ continue;	}

	    appSymbolDrawSymbol( asp, sym, drSym.drX0, drSym.drY0 );
	    }
	}

    sym= asp->aspSymbolSelected;
    if  ( sym >= 0						&&
	  appCharExistsInFont( asp->aspFont, sym )		)
	{
	row= sym/ asp->aspCellsWide;
	col= sym% asp->aspCellsWide;

	appSymbolRectangle( &drSym, asp, row, col );

	if  ( docIntersectRectangle( &drIgn, &drSym, &drClip ) )
	    {
	    appDrawSetForegroundBlack( add );

	    appDrawFillRectangle( add, drSym.drX0, drSym.drY0,
					    drSym.drX1- drSym.drX0+ 1,
					    drSym.drY1- drSym.drY0+ 1 );

	    appDrawSetForegroundWhite( add );

	    appSymbolDrawSymbol( asp, sym, drSym.drX0, drSym.drY0 );
	    }
	}

    appDrawNoClipping( add );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appSymbolMousePress, w, voidasp, downEvent )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    int			x;
    int			y;
    int			row;
    int			col;
    int			sym;
    int			sameAsPrevious;

    int			oldRow= -1;
    int			oldCol= -1;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    DocumentRectangle	drExpose;

    sym= asp->aspSymbolSelected;
    if  ( sym >= 0 )
	{
	oldRow= sym/ asp->aspCellsWide;
	oldCol= sym% asp->aspCellsWide;
	}

    if  ( appGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 )
	{ return;	}

    row= y/ ( asp->aspCellSizePixels+ 1 );
    col= x/ ( asp->aspCellSizePixels+ 1 );
    sym= row* asp->aspCellsWide+ col;

    if  ( ! appCharExistsInFont( asp->aspFont, sym ) )
	{ return;	}

    sameAsPrevious= asp->aspSymbolSelected == sym;
    asp->aspSymbolSelected= sym;

    appGuiEnableWidget( asp->aspInsertButton, 1 );

    if  ( oldRow >= 0 && oldCol >= 0 )
	{
	appSymbolRectangle( &drExpose, asp, oldRow, oldCol );

	appExposeRectangle( &(asp->aspDrawingData),
					    drExpose.drX0,
					    drExpose.drY0,
					    drExpose.drX1- drExpose.drX0+ 1,
					    drExpose.drY1- drExpose.drY0+ 1 );
	}

    appSymbolRectangle( &drExpose, asp, row, col );

    appExposeRectangle( &(asp->aspDrawingData),
					    drExpose.drX0,
					    drExpose.drY0,
					    drExpose.drX1- drExpose.drX0+ 1,
					    drExpose.drY1- drExpose.drY0+ 1 );

    if  ( sameAsPrevious && seq == 2 )
	{ appSymbolPickerInsertSymbol( asp );	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  A font family was selected by the user.				*/
/*  2)  A font encoding was selected by the user.			*/
/*									*/
/************************************************************************/

/*  1  */
static APP_OITEM_CALLBACK_H( appSymbolFontFamilyChosen, w, voidasp )
    {
    int				i;
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    int				changed= 0;
    const AppFontFamily *	aff;

    i= appGuiGetOptionmenuItemIndex( &(asp->aspFontOptionmenu), w );
    if  ( i < 0 || i >= asp->aspFontFamilyCount )
	{ LLDEB(i,asp->aspFontFamilyCount); return;	}

    aff= asp->aspFontFamilies+ i;
    if  ( docExpandedTextAttributeSetFamilyAndEncoding(
				    &(asp->aspTextAttribute),
				    &changed, aff,
				    asp->aspTextAttribute.etaFontEncoding ) )
	{ LDEB(i); return;	}

    if  ( changed )
	{
	AppEncodingMenu *		aem= &(asp->aspEncodingMenu);

	asp->aspFontFamilyChosen= i;

	appEncodingMenuSetEncoding( aem, aff,
				    asp->aspTextAttribute.etaFontEncoding );

	if  ( appSymbolAdaptToFamily( asp ) )
	    { LDEB(i);	}
	}

    return;
    }

/*  2  */
static APP_OITEM_CALLBACK_H( appSymbolFontEncodingChosen, w, voidasp )
    {
    int				i;
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    int				changed= 0;
    const AppFontFamily *	aff;

    i= appGuiGetOptionmenuItemIndex(
			&(asp->aspEncodingMenu.aemEncodingOptionmenu), w );
    if  ( i < 0 || i >= ENCODINGps_COUNT )
	{ LLDEB(i,ENCODINGps_COUNT); return;	}

    if  ( asp->aspFontFamilyChosen >= 0 )
	{
	aff= asp->aspFontFamilies+ asp->aspFontFamilyChosen;

	if  ( docExpandedTextAttributeSetFamilyAndEncoding(
						&(asp->aspTextAttribute),
						&changed, aff, i ) )
	    { LDEB(i); return;	}

	if  ( changed )
	    {
	    AppEncodingMenu *		aem= &(asp->aspEncodingMenu);

	    if  ( appSymbolAdaptDrawingToFont( asp ) )
		{ LDEB(asp->aspFontFamilyChosen);	}

	    appEncodingMenuSetEncoding( aem, aff,
				    asp->aspTextAttribute.etaFontEncoding );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the font/encoding part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolMakeFontPart(	APP_WIDGET			parent,
					AppSymbolPickerResources *	aspr,
					AppSymbolPicker *		asp )
    {
    APP_WIDGET	row;

    const int	fontColumn= 0;
    const int	fontColspan= 1;
    const int	encodingColumn= fontColumn+ fontColspan;
    const int	encodingColspan= 1;
    const int	columnCount= encodingColumn+ encodingColspan;

    appMakeRowFrameInColumn( &(asp->aspFontFrame),
					    &row, parent,
					    columnCount, aspr->asprFont );

    appMakeOptionmenuInRow( &(asp->aspFontOptionmenu), row,
					    fontColumn, fontColspan );

    appMakeOptionmenuInRow( &(asp->aspEncodingMenu.aemEncodingOptionmenu),
					row, encodingColumn, encodingColspan );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the symbol part of the symbol picker.				*/
/*									*/
/************************************************************************/

static void appSymbolMakeSymbolPart(	APP_WIDGET		parent,
					double			xfac,
					AppSymbolPickerResources * aspr,
					AppSymbolPicker *	asp )
    {
    int				wide;
    int				high;

    const int		twipsSize= 20* 10;
    const int		heightResizable= 0;

    asp->aspCellSizePixels= 1.5* ( xfac* twipsSize )+ 0.5;
    asp->aspCellsWide= 32;
    asp->aspCellsHigh= 8;

    wide= asp->aspCellsWide* ( asp->aspCellSizePixels+ 1 )+ 1;
    high= asp->aspCellsHigh* ( asp->aspCellSizePixels+ 1 )+ 1;

    appGuiMakeDrawingAreaInColumn( &(asp->aspSymbolDrawing), parent,
		wide, high, heightResizable, appSymbolRedraw, (void *)asp );

    appDrawSetButtonPressHandler( asp->aspSymbolDrawing,
				    appSymbolMousePress, (void *)asp );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the symbol picker.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appSymbolMakeButtonRow( APP_WIDGET		parent,
					    AppSymbolPickerResources * aspr,
					    AppSymbolPicker *	asp )
    {
    APP_WIDGET		row;
    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    row= appMakeRowInColumn( parent, 1, heightResizable );

    appMakeButtonInRow( &(asp->aspInsertButton), row,
			    aspr->asprInsert, appSymbolInsertPushed,
			    (void *)asp, 0, showAsDefault );

    return row;
    }

/************************************************************************/
/*									*/
/*  Fill the list of font families.					*/
/*									*/
/************************************************************************/

/*  1  */
static void appSymbolFillFontMenu(	AppSymbolPickerResources *	aspr,
					AppSymbolPicker *		asp )
    {
    int			i;
    AppFontFamily *	aff;

    appEmptyOptionmenu( &(asp->aspFontOptionmenu) );

    aff= asp->aspFontFamilies;
    for ( i= 0; i < asp->aspFontFamilyCount; aff++, i++ )
	{
	char *		labelText;

	if  ( aff->affFontFamilyText )
	    { labelText= aff->affFontFamilyText;	}
	else{ labelText= aff->affFontFamilyName;	}

	asp->aspFontFamilyOptions[i]= appAddItemToOptionmenu(
				    &(asp->aspFontOptionmenu), labelText,
				    appSymbolFontFamilyChosen, (void *)asp );
	}

    if  ( asp->aspFontFamilyCount == 0 )
	{
	asp->aspFontFamilyOptions[0]= appAddItemToOptionmenu(
				&(asp->aspFontOptionmenu), aspr->asprNone,
				appSymbolFontFamilyChosen, (void *)asp );

	appGuiEnableWidget( asp->aspTopWidget, 0 );
	}

    appSetOptionmenu( &(asp->aspFontOptionmenu), 0 );
    asp->aspFontFamilyChosen= 0;

    appOptionmenuRefreshWidth( &(asp->aspFontOptionmenu) );

    return;
    }


/************************************************************************/
/*									*/
/*  A symbol picker must be destroyed.					*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( appCloseSymbolPicker, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    if  ( asp->aspDestroy )
	{ (*asp->aspDestroy)( asp->aspTarget );	}

    if  ( asp->aspFont )
	{ appDrawFreeFont( &(asp->aspDrawingData), asp->aspFont );	}

    docCleanExpandedTextAttribute( &(asp->aspTextAttribute) );

    appCleanDrawingData( &(asp->aspDrawingData) );

    free( voidasp );

    appDestroyShellWidget( w );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a Symbol Picker Tool.						*/
/*									*/
/************************************************************************/

void * appMakeSymbolPicker(	APP_WIDGET		symbolOption,
				EditApplication *	ea,
				const char *		widgetName,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				SymbolPickerInsert	insert,
				AppToolDestroy		destroy,
				void *			target )
    {
    AppSymbolPicker *			asp;
    
    APP_WIDGET				buttonForm;

    const int				userResizable= 0;

    double				horPixPerMM;
    double				verPixPerMM;
    double				xfac;
    double				yfac;

    AppFontFamily *			fontFamilies;
    int					fontFamilyCount;

    static AppSymbolPickerResources	aspr;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appEncodingMenuGetOptionTexts( aspr.asprEncodings, ea );

	appGuiGetResourceValues( ea, (void *)&aspr,
					APP_SymbolPickerResourceTable,
					sizeof(APP_SymbolPickerResourceTable)/
					sizeof(AppConfigurableResource) );

	gotResources= 1;
	}

    if  ( psFontCatalog( ea->eaAfmDirectory,
				    &fontFamilies, &fontFamilyCount ) )
	{
	SDEB(ea->eaAfmDirectory);
	fontFamilies= (AppFontFamily *)0;
	fontFamilyCount= 0;
	}

    appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );

    asp= (AppSymbolPicker *)malloc( sizeof(AppSymbolPicker)+
					fontFamilyCount* sizeof(APP_WIDGET) );
    if  ( ! asp )
	{ XDEB(asp); return (void *)0;	}

    asp->aspApplication= ea;
    asp->aspFontFamilies= fontFamilies;
    asp->aspFontFamilyCount= fontFamilyCount;

    asp->aspResources= &aspr;

    asp->aspTopWidget= (APP_WIDGET)0;
    asp->aspSymbolDrawing= (APP_WIDGET)0;
    asp->aspInsertButton= (APP_WIDGET)0;

    asp->aspDestroy= destroy;
    asp->aspInsert= insert;
    asp->aspTarget= target;

    asp->aspSymbolSelected= -1;
    asp->aspFontFamilyChosen= -1;

    asp->aspFont= (APP_FONT *)0;
    appInitDrawingData( &(asp->aspDrawingData) );

    docInitExpandedTextAttribute( &(asp->aspTextAttribute) );

    appMakeVerticalTool( &(asp->aspTopWidget), &(asp->aspMainWidget), ea,
			    iconPixmap, iconMask, widgetName, userResizable,
			    symbolOption, appCloseSymbolPicker, (void *)asp );

    appSymbolMakeFontPart( asp->aspMainWidget, &aspr, asp );

    appSymbolMakeSymbolPart( asp->aspMainWidget, xfac, &aspr, asp );
    buttonForm= appSymbolMakeButtonRow( asp->aspMainWidget, &aspr, asp );

    appSymbolFillFontMenu( &aspr, asp );
    appEncodingMenuFillOptionmenu( aspr.asprEncodings,
				    appSymbolFontEncodingChosen, (void *)asp,
				    &(asp->aspEncodingMenu) );


#   ifdef USE_MOTIF
    XtRealizeWidget( asp->aspTopWidget );
#   endif

#   ifdef USE_GTK
    gtk_widget_realize( asp->aspSymbolDrawing );
#   endif

    appOptionmenuRefreshWidth( &(asp->aspFontOptionmenu) );
    appOptionmenuRefreshWidth( &(asp->aspEncodingMenu.aemEncodingOptionmenu) );

    appSetDrawingDataForWidget( asp->aspSymbolDrawing,
				ea->eaMagnification, &(asp->aspDrawingData) );

    appShowShellWidget( asp->aspTopWidget );

    return (void *)asp;
    }

/************************************************************************/
/*									*/
/*  Draw a symbol picker to front.					*/
/*									*/
/************************************************************************/

void appShowSymbolPicker(	void *	voidasp	)
    {
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;

    appShowShellWidget( asp->aspTopWidget );
    }

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font family.			*/
/*									*/
/************************************************************************/

int appAdaptSymbolPickerToFontFamily(
				void *				voidasp,
				const ExpandedTextAttribute *	etaNew,
				const PropertyMask *		newMask )
    {
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    AppEncodingMenu *		aem= &(asp->aspEncodingMenu);
    int				rval= 0;

    PropertyMask		doneMask;

    PROPmaskCLEAR( &doneMask );
    docUpdateExpandedTextAttribute( &doneMask, &(asp->aspTextAttribute),
							    etaNew, newMask );

    if  ( PROPmaskISSET( &doneMask, TApropFONTFAMILY )	&&
	  etaNew->etaFontFamilyName			)
	{
	const AppFontFamily *	aff;
	int			i;

	aff= asp->aspFontFamilies;
	for ( i= 0; i < asp->aspFontFamilyCount; aff++, i++ )
	    {
	    if  ( ! strcmp( etaNew->etaFontFamilyName,
						aff->affFontFamilyName ) )
		{ asp->aspFontFamilyChosen= i; break;	}
	    }
	}

    if  ( asp->aspFontFamilyChosen < 0 )
	{
	int		changed= 0;

	asp->aspFontFamilyChosen=  0; 

	docExpandedTextAttributeSetFamilyAndEncoding(
				&(asp->aspTextAttribute), &changed,
				asp->aspFontFamilies,
				asp->aspFontFamilies->affDefaultEncoding );
	}

    if  ( appSymbolAdaptToFamily( asp ) )
	{ LDEB(asp->aspFontFamilyChosen); rval= -1;	}

    appSetOptionmenu( &(asp->aspFontOptionmenu),
					    asp->aspFontFamilyChosen );

    appEncodingMenuSetEncoding( aem,
			    asp->aspFontFamilies+ asp->aspFontFamilyChosen,
			    asp->aspTextAttribute.etaFontEncoding );

    return rval;
    }

void appEnableSymbolPicker(	void *	voidasp,
				int	enabled )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appGuiEnableWidget( asp->aspMainWidget, enabled != 0 );

    return;
    }
