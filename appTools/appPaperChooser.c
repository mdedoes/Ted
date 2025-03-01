/************************************************************************/
/*									*/
/*  Choose a paper format common functionality.				*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<geoString.h>
#   include	<appPaper.h>
#   include	<utilPropMask.h>

#   include	"appPaperChooser.h"
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Turn on or off							*/
/*									*/
/************************************************************************/

void appEnablePaperChooser(	PaperChooser *			pc,
				int				enabled )
    {
    guiEnableWidget( pc->pcFrame, enabled );
    guiEnableText( pc->pcSizeText, enabled );
    }

/************************************************************************/
/*									*/
/*  Update the text widget.						*/
/*									*/
/************************************************************************/

static void appPaperChooserShowWidthHeight(
				PaperChooser *			pc,
				const DocumentGeometry *	dg )
    {
    char			scratch[50];

    geoRectangleToString( scratch,
		    dg->dgPageWideTwips, dg->dgPageHighTwips, pc->pcUnitType );

    pc->pcProgrammatic++;
    guiStringToText( pc->pcSizeText, scratch );
    pc->pcProgrammatic--;

    return;
    }

/************************************************************************/
/*									*/
/*  Extract a paper size from the Text widget.				*/
/*									*/
/************************************************************************/

int appPaperChooserGetSize(	PropertyMask *		pDoneMask,
				PaperChooser *		pc,
				DocumentGeometry *	dg )
    {
    char *		s;
    DocumentGeometry	dgSet;

    PropertyMask	setMask;
    PropertyMask	doneMask;

    dgSet= *dg;

    utilPropMaskClear( &setMask );
    utilPropMaskClear( &doneMask );

    PROPmaskADD( &setMask, DGpropPAGE_WIDTH );
    PROPmaskADD( &setMask, DGpropPAGE_HEIGHT );

    s= guiStringFromText( pc->pcSizeText );

    if  ( geoRectangleFromString( s, pc->pcUnitType,
				    &(dgSet.dgPageWideTwips),
				    &(dgSet.dgPageHighTwips) )		||
	  dgSet.dgPageWideTwips <= 0					||
	  dgSet.dgPageHighTwips <= 0					)
	{
	guiFreeStringFromTextWidget( s );

	guiRefuseTextValue( pc->pcSizeText );

	return -1;
	}

    guiFreeStringFromTextWidget( s );

    geoUpdDocumentGeometry( &doneMask, dg, &setMask, &dgSet );

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &doneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt to geometry.							*/
/*									*/
/*  1)  Show the orientation in the radiogroup.				*/
/*  2)  Derive the values to show in the radiogroup and the size menu	*/
/*	from the rectangle.						*/
/*  3)  Set the orientation buttons and the paper size menu.		*/
/*									*/
/************************************************************************/

/*  1  */
static void appPaperChooserShowOrientation(	PaperChooser *	pc )
    {
    if  ( ! pc->pcPortraitRadio || ! pc->pcLandscapeRadio )
	{ XXDEB(pc->pcPortraitRadio,pc->pcLandscapeRadio); return;	}

    pc->pcProgrammatic++;

    if  ( pc->pcLandscapeChosen )
	{
	guiSetToggleState( pc->pcPortraitRadio,  0 );
	guiSetToggleState( pc->pcLandscapeRadio, 1 );
	}
    else{
	guiSetToggleState( pc->pcPortraitRadio,  1 );
	guiSetToggleState( pc->pcLandscapeRadio, 0 );
	}

    pc->pcProgrammatic--;

    return;
    }

/*  2  */
static void appPaperChooserWidgetSettings(
				int *				pLandscape,
				int *				pSizeNr,
				int				custom,
				const DocumentGeometry *	dg )
    {
    int		sizeNr;
    int		landscape;

    if  ( dg->dgPageWideTwips > dg->dgPageHighTwips )
	{
	sizeNr= utilPaperGetNumberBySize( dg->dgPageHighTwips,
						    dg->dgPageWideTwips );

	landscape= 1;
	}
    else{
	sizeNr= utilPaperGetNumberBySize( dg->dgPageWideTwips,
						    dg->dgPageHighTwips );

	landscape= 0;
	}

    if  ( sizeNr < 0 )
	{ sizeNr= custom;	}

    *pLandscape= landscape;
    *pSizeNr= sizeNr;

    return;
    }

/*  3  */
static void appPaperChooserSetWidgets(	PaperChooser *		pc,
					int			sizeNr,
					int			landscape )
    {
    if  ( pc->pcSizeChosen != sizeNr )
	{
	pc->pcSizeChosen= sizeNr;

	pc->pcProgrammatic++;
	guiSetOptionmenu( &(pc->pcOptionmenu), pc->pcSizeChosen );
	pc->pcProgrammatic--;
	}

    if  ( pc->pcLandscapeChosen != landscape	)
	{
	pc->pcLandscapeChosen= landscape;

	if  ( pc->pcPortraitRadio	&&
	      pc->pcLandscapeRadio	)
	    { appPaperChooserShowOrientation( pc );	}
	}

    return;
    }

void appPaperChooserAdaptToGeometry(	PaperChooser *			pc,
					const DocumentGeometry *	dg )
    {
    int		sizeNr;
    int		landscape;

    pc->pcGeometryChosen= *dg;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    appPaperChooserWidgetSettings( &landscape, &sizeNr,
			    pc->pcCustomPaperSize, &(pc->pcGeometryChosen) );

    appPaperChooserSetWidgets( pc, sizeNr, landscape );

    return;
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen in the menu. As far as the paper chooser	*/
/*  is concerned no more needs to be done.				*/
/*									*/
/************************************************************************/

static void appPaperChooserSizeChosen(	int		i,
					void *		voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;

    int			changed= 0;

    int			width;
    int			height;

    if  ( pc->pcProgrammatic )
	{ return;	}

    if  ( i < 0 || i > pc->pcCustomPaperSize )
	{ LLDEB(i,pc->pcCustomPaperSize); return;	}

    pc->pcSizeChosen= i;

    if  ( i == pc->pcCustomPaperSize )
	{
	PropertyMask		updMask;
	DocumentGeometry	dgScratch;

	utilPropMaskClear( &updMask );

	if  ( appPaperChooserGetSize( &updMask, pc, &dgScratch ) )
	    { LDEB(1); return;	}

	width= dgScratch.dgPageWideTwips;
	height= dgScratch.dgPageHighTwips;

	if  ( width > height )
	    { pc->pcLandscapeChosen= 1;	}
	else{ pc->pcLandscapeChosen= 0;	}
	}
    else{
	if  ( utilPaperGetInfoByNumber( i, &width, &height, (const char **)0 ) )
	    { return;	}

	if  ( pc->pcLandscapeChosen )
	    {
	    int		swap;

	    swap= height; height= width; width= swap;
	    }
	}

    if  ( pc->pcGeometryChosen.dgPageWideTwips != width		||
	  pc->pcGeometryChosen.dgPageHighTwips != height	)
	{ changed= 1;	}

    pc->pcGeometryChosen.dgPageWideTwips= width;
    pc->pcGeometryChosen.dgPageHighTwips= height;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    if  ( changed && pc->pcCallback )
	{
	(*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the widgets for a paper chooser.				*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appPaperChooserTypingCallback, w, voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;
    char *		s;

    DocumentGeometry	dgHere;

    geoInitDocumentGeometry( &dgHere );

    if  ( pc->pcProgrammatic )
	{ return;	}

    s= guiStringFromText( pc->pcSizeText );

    if  ( ! geoRectangleFromString( s, pc->pcUnitType,
				    &(dgHere.dgPageWideTwips),
				    &(dgHere.dgPageHighTwips) )		&&
	  dgHere.dgPageWideTwips > 0					&&
	  dgHere.dgPageHighTwips > 0					)
	{
	int		sizeNr;
	int		landscape;

	appPaperChooserWidgetSettings( &landscape, &sizeNr,
					    pc->pcCustomPaperSize, &dgHere );

	appPaperChooserSetWidgets( pc, sizeNr, landscape );
	}

    guiFreeStringFromTextWidget( s );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback when the user pushes return in the edit.			*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appPaperChooserGotValueCallback, w, voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;
    PropertyMask	updMask;

    utilPropMaskClear( &updMask );

    if  ( pc->pcProgrammatic )
	{ return;	}

    if  ( appPaperChooserGetSize( &updMask, pc, &(pc->pcGeometryChosen) ) )
	{ LDEB(1); return;	}

    if  ( ! utilPropMaskIsEmpty( &updMask ) )
	{
	appPaperChooserAdaptToGeometry( pc, &(pc->pcGeometryChosen) );

	if  ( pc->pcCallback )
	    {
	    (*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  One of the orientation toggles was activated. As far as the paper	*/
/*  chooser is concerned no more needs to be done.			*/
/*									*/
/*  The contorted logic of this function is dictated by the fact that	*/
/*  GTK invokes callbacks for programmatic changes to the toggle	*/
/*  buttons. This is a curse we have to live with.			*/
/*									*/
/************************************************************************/

static void appPaperChooserOrientationChanged(	PaperChooser *	pc )
    {
    int	swap;

    swap= pc->pcGeometryChosen.dgPageWideTwips;
    pc->pcGeometryChosen.dgPageWideTwips=
				pc->pcGeometryChosen.dgPageHighTwips;
    pc->pcGeometryChosen.dgPageHighTwips= swap;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    if  ( pc->pcCallback )
	{
	(*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPaperChooserLandscapeToggled, w, voidpc, e )
    {
    PaperChooser *		pc= (PaperChooser *)voidpc;

    int				change= 0;
    int				set;

    if  ( pc->pcProgrammatic )
	{ return;	}

    set= guiGetToggleStateFromCallback( w, e );

    if  ( pc->pcLandscapeChosen )
	{
	if  ( ! set )
	    {
	    pc->pcProgrammatic++;
	    guiSetToggleState( pc->pcLandscapeRadio, 1 );
	    pc->pcProgrammatic--;
	    }
	}
    else{
	if  ( set )
	    {
	    pc->pcLandscapeChosen= 1;
	    pc->pcProgrammatic++;
	    guiSetToggleState( pc->pcPortraitRadio, 0 );
	    pc->pcProgrammatic--;
	    change= 1;
	    }
	}

    if  ( change )
	{ appPaperChooserOrientationChanged( pc );	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPaperChooserPortraitToggled, w, voidpc, e )
    {
    PaperChooser *		pc= (PaperChooser *)voidpc;

    int				change= 0;
    int				set;

    if  ( pc->pcProgrammatic )
	{ return;	}

    set= guiGetToggleStateFromCallback( w, e );

    if  ( pc->pcLandscapeChosen )
	{
	if  ( set )
	    {
	    pc->pcLandscapeChosen= 0;
	    pc->pcProgrammatic++;
	    guiSetToggleState( pc->pcLandscapeRadio, 0 );
	    pc->pcProgrammatic--;
	    change= 1;
	    }
	}
    else{
	if  ( ! set )
	    {
	    pc->pcProgrammatic++;
	    guiSetToggleState( pc->pcPortraitRadio, 1 );
	    pc->pcProgrammatic--;
	    }
	}

    if  ( change )
	{ appPaperChooserOrientationChanged( pc );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the widgets in a paper chooser and install the callback.	*/
/*									*/
/************************************************************************/

void appMakePaperChooserWidgets(	APP_WIDGET		parent,
					const char *		title,
					int			unitType,
					PaperChooser *		pc,
					PaperChooserCallback	callback,
					void *			through )
    {
    const int	textEnabled= 1;

    pc->pcUnitType= unitType;
    pc->pcCallback= callback;
    pc->pcCallbackThrough= through;

    guiMakeColumnFrameInColumn( &(pc->pcFrame),
				    &(pc->pcVerticalColumn), parent, title );


    guiMakeOptionmenuInColumn( &(pc->pcOptionmenu), pc->pcVerticalColumn,
				    appPaperChooserSizeChosen, (void *)pc );

    guiMakeTextInColumn( &(pc->pcSizeText), pc->pcVerticalColumn,
							    0, textEnabled );

    guiSetGotValueCallbackForText( pc->pcSizeText,
				appPaperChooserGotValueCallback, (void *)pc );

    guiSetTypingCallbackForText( pc->pcSizeText,
				appPaperChooserTypingCallback, (void *)pc );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Add Portrait/Landscape toggles to a paper chooser.			*/
/*									*/
/************************************************************************/

void appPaperChooserAddOrientationToggles( PaperChooser *	pc,
					const char *		portrait,
					const char *		landscape )
    {
    const int	heightResizable= 0;

    pc->pcOrientationRow= guiMakeRowInColumn( pc->pcVerticalColumn, 2,
							    heightResizable );

    pc->pcPortraitRadio= guiMakeToggleInRow( pc->pcOrientationRow,
		portrait, appPaperChooserPortraitToggled, (void *)pc, 0, 1 );

    pc->pcLandscapeRadio= guiMakeToggleInRow( pc->pcOrientationRow,
		landscape, appPaperChooserLandscapeToggled, (void *)pc, 1, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the list of paper sizes.					*/
/*									*/
/************************************************************************/

void appPaperChooserFillMenu(	PaperChooser *		pc,
				const char *		customLabel )
    {
    const char *	label;
    int			i;
    int			gotSome= 0;

    if  ( ! pc->pcSizeOptionsMade )
	{
	i= 0;
	while( ! utilPaperGetInfoByNumber( i, (int *)0, (int *)0, &label ) )
	    { i++; }

	pc->pcCustomPaperSize= i++;
	}

    guiEmptyOptionmenu( &(pc->pcOptionmenu) );

    i= 0;
    while( ! utilPaperGetInfoByNumber( i, (int *)0, (int *)0, &label ) )
	{
	guiAddItemToOptionmenu( &(pc->pcOptionmenu), label );

	if  ( i == 0 )
	    { gotSome= 1;	}

	i++;
	}

    guiAddItemToOptionmenu( &(pc->pcOptionmenu), customLabel );
    if  ( i == 0 )
	{ gotSome= 1;	}

    if  ( gotSome )
	{
	guiSetOptionmenu( &(pc->pcOptionmenu), 0 );
	pc->pcSizeChosen= 0;
	}
    else{
	guiSetOptionmenu( &(pc->pcOptionmenu), -1 );
	}

    pc->pcSizeOptionsMade= 1;
    guiOptionmenuRefreshWidth( &(pc->pcOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize a fresh PaperChooser.					*/
/*									*/
/************************************************************************/

void appCleanPaperChooser(	PaperChooser *	pc )
    {
    }

void appInitPaperChooser(	PaperChooser *	pc )
    {
    pc->pcFrame= (APP_WIDGET)0;
    pc->pcVerticalColumn= (APP_WIDGET)0;

    guiInitOptionmenu( &(pc->pcOptionmenu) );
    pc->pcSizeText= (APP_WIDGET)0;

    pc->pcOrientationRow= (APP_WIDGET)0;
    pc->pcPortraitRadio= (APP_WIDGET)0;
    pc->pcLandscapeRadio= (APP_WIDGET)0;

    pc->pcCustomPaperSize= -1;
    pc->pcSizeChosen= -1;
    pc->pcLandscapeChosen= -1;

    geoInitDocumentGeometry( &(pc->pcGeometryChosen) );

    pc->pcSizeOptionsMade= 0;

    pc->pcCallback= (PaperChooserCallback)0;
    pc->pcCallbackThrough= (void *)0;

    pc->pcProgrammatic= 0;

    return;
    }
