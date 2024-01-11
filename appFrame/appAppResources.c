/************************************************************************/
/*									*/
/*  Application, Main module.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"appEditApplication.h"
#   include	<geoUnit.h>
#   include	"appPaper.h"
#   include	<geoString.h>
#   include	<utilArgToX.h>
#   include	"appGuiResource.h"

#   include	<appDebugon.h>


/************************************************************************/

int appDetermineBoolean(	int *			pIval,
				const char *		sVal )
    {
    if  ( sVal		&&
	  *pIval == 0	)
	{
	if  ( ! strcmp( sVal, "0" ) )
	    { *pIval= -1;	}
	if  ( ! strcmp( sVal, "1" ) )
	    { *pIval=  1;	}

	if  ( *pIval == 0 )
	    { SDEB(sVal);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Generic editor type application main().				*/
/*									*/
/*  1)  Make an application shell.					*/
/*  1b) Obtain resource values.						*/
/*  2)  Remember X11 properties.					*/
/*  3)  Remember Window manager properties.				*/
/*									*/
/************************************************************************/

static AppConfigurableResource	APP_ApplicationResourceTable[]=
{
    APP_RESOURCE( "unit",
		offsetof(EditApplication,eaUnitString),
		"inch" ),
    APP_RESOURCE( "paper",
		offsetof(EditApplication,eaPaperString),
		"letter" ),
    APP_RESOURCE( "leftMargin",
		offsetof(EditApplication,eaLeftMarginString),
		"90pt" ),
    APP_RESOURCE( "rightMargin",
		offsetof(EditApplication,eaRightMarginString),
		"90pt" ),
    APP_RESOURCE( "topMargin",
		offsetof(EditApplication,eaTopMarginString),
		"72pt" ),
    APP_RESOURCE( "bottomMargin",
		offsetof(EditApplication,eaBottomMarginString),
		"72pt" ),
    APP_RESOURCE( "magnification",
		offsetof(EditApplication,eaMagnificationString),
		"100%" ),

    APP_RESOURCE( "usePostScriptFilters",
		offsetof(EditApplication,eaUsePostScriptFiltersString),
		"1" ),
    APP_RESOURCE( "usePostScriptIndexedImages",
		offsetof(EditApplication,eaUsePostScriptIndexedImagesString),
		"1" ),

    APP_RESOURCE( "skipEmptyPages",
		offsetof(EditApplication,eaSkipEmptyPagesString),
		"0" ),
    APP_RESOURCE( "skipBlankPages",
		offsetof(EditApplication,eaSkipBlankPagesString),
		"0" ),
    APP_RESOURCE( "omitHeadersOnEmptyPages",
		offsetof(EditApplication,eaOmitHeadersOnEmptyPagesString),
		"0" ),
    APP_RESOURCE( "customPsSetupFilename",
		offsetof(EditApplication,eaCustomPsSetupFilename),
		(char *)0 ),

    APP_RESOURCE( "avoidFontconfig",
		offsetof(EditApplication,eaAvoidFontconfigString),
		"0" ),
    APP_RESOURCE( "preferBase35Fonts",
		offsetof(EditApplication,eaPreferBase35FontsString),
		"0" ),
    APP_RESOURCE( "embedFonts",
		offsetof(EditApplication,eaEmbedFontsString),
		"1" ),
    APP_RESOURCE( "useKerning",
		offsetof(EditApplication,eaUseKerningString),
		"1" ),
    APP_RESOURCE( "styleTool",
		offsetof(EditApplication,eaStyleToolString),
		"1" ),

    APP_RESOURCE( "leftRulerWidthMM",
		offsetof(EditApplication,eaLeftRulerWidthMMString),
		(char *)0 ),
    APP_RESOURCE( "topRulerHeightMM",
		offsetof(EditApplication,eaTopRulerHeightMMString),
		(char *)0 ),
    APP_RESOURCE( "rightRulerWidthMM",
		offsetof(EditApplication,eaRightRulerWidthMMString),
		(char *)0 ),
    APP_RESOURCE( "bottomRulerHeightMM",
		offsetof(EditApplication,eaBottomRulerHeightMMString),
		(char *)0 ),

    APP_RESOURCE( "author",
		offsetof(EditApplication,eaAuthor),
		(char *)0 ),

    APP_RESOURCE( "afmDirectory",
		offsetof(EditApplication,eaAfmDirectory),
		AFMDIR ),
    APP_RESOURCE( "fontDirectory",
		offsetof(EditApplication,eaFontDirectory),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontmap",
		offsetof(EditApplication,eaGhostscriptFontmap),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontToXmapping",
		offsetof(EditApplication,eaGhostscriptFontToXmapping),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand",
		offsetof(EditApplication,eaCustomPrintCommand),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName",
		offsetof(EditApplication,eaCustomPrinterName),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand2",
		offsetof(EditApplication,eaCustomPrintCommand2),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName2",
		offsetof(EditApplication,eaCustomPrinterName2),
		(char *)0 ),
};

/************************************************************************/
/*									*/
/*  Initialize geometry related settings for the application.		*/
/*									*/
/************************************************************************/

static void appDetermineGeometrySettings(	EditApplication *	ea )
    {
    int				paperFormat;

    ea->eaUnitInt= geoUnitTypeInt( ea->eaUnitString );
    if  ( ea->eaUnitInt < 0 )
	{ SDEB(ea->eaUnitString); ea->eaUnitInt= UNITtyINCH; }

    ea->eaMagnification= 1.2;
    if  ( ea->eaMagnificationString )
	{
	double	mag= 0.0;
	char	c;
	int	res;

	res= sscanf( ea->eaMagnificationString, "%lg%c", &mag, &c );
	if  ( res == 1 )
	    { ea->eaMagnification= mag;		}
	if  ( res == 2 && c == '%' )
	    { ea->eaMagnification= mag/100.0;	}

	if  ( mag < 0.1				||
	      ea->eaMagnification < 0.1		||
	      ea->eaMagnification > 10.0	)
	    {
	    SFDEB(ea->eaMagnificationString,mag);
	    ea->eaMagnification= 1.2;
	    }
	}

    if  ( utilPaperSizeFromString( &paperFormat,
			    &(ea->eaDefaultDocumentGeometry.dgPageWideTwips),
			    &(ea->eaDefaultDocumentGeometry.dgPageHighTwips),
			    ea->eaUnitInt, ea->eaPaperString ) < 0 )
	{ SDEB(ea->eaPaperString);	}

    if  ( ea->eaLeftMarginString )
	{
	if  ( geoLengthFromString( ea->eaLeftMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgMargins.roLeftOffset) ) )
	    { SDEB(ea->eaLeftMarginString);	}
	}

    if  ( ea->eaRightMarginString )
	{
	if  ( geoLengthFromString( ea->eaRightMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgMargins.roRightOffset) ) )
	    { SDEB(ea->eaRightMarginString);	}
	}

    if  ( ea->eaTopMarginString )
	{
	if  ( geoLengthFromString( ea->eaTopMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgMargins.roTopOffset) ) )
	    { SDEB(ea->eaTopMarginString);	}
	}

    if  ( ea->eaBottomMarginString )
	{
	if  ( geoLengthFromString( ea->eaBottomMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgMargins.roBottomOffset) ) )
	    { SDEB(ea->eaBottomMarginString);	}
	}

    if  ( ea->eaLeftRulerWidthMMString )
	{
	utilArgToInt( &(ea->eaLeftRulerWidthMM),
					ea->eaLeftRulerWidthMMString );
	}
    if  ( ea->eaRightRulerWidthMMString )
	{
	utilArgToInt( &(ea->eaRightRulerWidthMM),
					ea->eaRightRulerWidthMMString );
	}
    if  ( ea->eaTopRulerHeightMMString )
	{
	utilArgToInt( &(ea->eaTopRulerHeightMM),
					ea->eaTopRulerHeightMMString );
	}
    if  ( ea->eaBottomRulerHeightMMString )
	{
	utilArgToInt( &(ea->eaBottomRulerHeightMM),
					ea->eaBottomRulerHeightMMString );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resolve application resources.					*/
/*									*/
/*  9)  Theoretically a program could have more than one application	*/
/*	object. This has never beem tested. The use of a single		*/
/*	table and different flags to check for reuse here is one of the	*/
/*	things to fix.							*/
/*									*/
/************************************************************************/

void appGetApplicationResourceValues(	EditApplication *	ea )
    {
    /*  b  */
    if  ( ! ea->eaGotResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotResourceTable), ea,
					    ea->eaResourceData,
					    ea->eaResourceTable,
					    ea->eaResourceCount );
	}

    if  ( ! ea->eaGotFileMessageResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotFileMessageResourceTable), ea,
					&(ea->eaFileMessageResources),
					ea->eaFileMessageResourceTable,
					ea->eaFileMessageResourceCount );
	}

    /*  9  */
    if  ( ! ea->eaGotApplicationResources )
	{
	appGuiGetResourceValues( &(ea->eaGotApplicationResources), ea,
				(void *)ea,
				APP_ApplicationResourceTable,
				sizeof(APP_ApplicationResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    appDetermineBoolean( &(ea->eaUsePostScriptFiltersInt),
				    ea->eaUsePostScriptFiltersString );
    appDetermineBoolean( &(ea->eaUsePostScriptIndexedImagesInt),
				    ea->eaUsePostScriptIndexedImagesString );
    appDetermineBoolean( &(ea->eaSkipEmptyPagesInt),
				    ea->eaSkipEmptyPagesString );
    appDetermineBoolean( &(ea->eaSkipBlankPagesInt),
				    ea->eaSkipBlankPagesString );
    appDetermineBoolean( &(ea->eaOmitHeadersOnEmptyPagesInt),
				    ea->eaOmitHeadersOnEmptyPagesString );
    appDetermineBoolean( &(ea->eaAvoidFontconfigInt),
				    ea->eaAvoidFontconfigString );
    appDetermineBoolean( &(ea->eaPreferBase35FontsInt),
				    ea->eaPreferBase35FontsString );
    appDetermineBoolean( &(ea->eaEmbedFontsInt),
				    ea->eaEmbedFontsString );
    appDetermineBoolean( &(ea->eaUseKerningInt),
				    ea->eaUseKerningString );
    appDetermineBoolean( &(ea->eaStyleToolInt),
				    ea->eaStyleToolString );

    appDetermineGeometrySettings( ea );

    return;
    }

