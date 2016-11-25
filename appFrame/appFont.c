/************************************************************************/
/*  Management of X11 fonts: Translation of understandable fonts to	*/
/*  X11 structures.							*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<utilPropMask.h>

#   include	<utilFontmap.h>

#   include	"appDraw.h"
#   include	<bitmap.h>

#   include	<X11/Xatom.h>

#   ifdef USE_GTK
#	include	<gdk/gdkx.h>
#   endif

/************************************************************************/
/*									*/
/*  Strings used for the analysis of X11 font names.			*/
/*									*/
/*  1)  The asterisk to insert in a font name for all properties that	*/
/*	we consider to be irrelevant.					*/
/*  2)  The templete/format to make X11 font names.			*/
/*  3)  A series of encodings with information on the X11 encoding	*/
/*	strings and on the encodings for which they can be used.	*/
/*  4)  This is the combination of character set and encoding. Given	*/
/*	a particular value, XDrawString() draws the same string with	*/
/*	the same array of characters.					*/
/*									*/
/************************************************************************/

/*  1  */
static const char *	EditFontAnyTemplate= "*";

/*  2  */
# define	EditFontFamilyTemplate \
			    "-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s"

/*  3  */
typedef struct AppFontEncoding
    {
			/*  4  */
    int			afeCharsetEncoding;

    const char *	afeRegistry;
    const char *	afeEncoding;
    AppFontFamily *	afeXfontFamilies;
    int			afeXfontFamilyCount;
    } AppFontEncoding;

static AppFontEncoding	AppFontXEncodings[]=
    {
	{
	    ENCODINGpsISO_8859_1,
	    "iso8859", "1",
	},

	{
	    ENCODINGpsISO_8859_2,
	    "iso8859", "2",
	},

	{
	    ENCODINGpsADOBE_SYMBOL,
	    "adobe", "fontspecific",
	},

	{
	    ENCODINGpsADOBE_DINGBATS,
	    "adobe", "fontspecific",
	},

	{
	    ENCODINGpsISO_8859_7,
	    "iso8859", "7",
	},

    };

/************************************************************************/
/*									*/
/*  Make a font query string.						*/
/*									*/
/************************************************************************/

static void appFontQueryString(	char *		target,
				const char *	foundry,
				const char *	family,
				const char *	weight,
				const char *	slant,
				const char *	swdth,
				const char *	pxlsz,
				const char *	registry,
				const char *	encoding )
    {
    sprintf( target, EditFontFamilyTemplate,
		foundry,
		family,
		weight,
		slant,
		swdth,			/* swdth		*/
		EditFontAnyTemplate,	/* adstyl		*/
		pxlsz,
		EditFontAnyTemplate,	/* ptsz			*/
		EditFontAnyTemplate,	/* resx			*/
		EditFontAnyTemplate,	/* resy			*/
		EditFontAnyTemplate,	/* spacing		*/
		EditFontAnyTemplate,	/* average width	*/
		registry,
		encoding );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract properties from an X font selection string.			*/
/*									*/
/************************************************************************/

static int appFontExtractString(	const char *	s,
					const char **	pPast,
					char *		target )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ *(target++)= *(s++); }
    *target= '\0';

    *pPast= s; return 0;
    }

static int appFontSkipString(	const char *	s,
				const char **	pPast )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ s++; }

    *pPast= s; return 0;
    }

static int appFontSplitXFont(	const char *	fontname,
				char *		foundry,
				char *		family,
				char *		weight,
				char *		slant,
				char *		swdth,
				char *		pxlszStr,
				char *		spacing,
				char *		avgwdth,
				char *		registry,
				char *		encoding )
    {
    const char *	s;

    s= fontname;

    if  ( appFontExtractString( s, &s, foundry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, family ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, weight ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, slant ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, swdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* adstyl */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, pxlszStr ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* ptsz */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resx */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resy */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, spacing ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, avgwdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, registry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, encoding ) )
	{ SDEB(fontname); return -1;	}

    if  ( s[0] )
	{ SSDEB(fontname,s); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of X11 fonts.					*/
/*									*/
/************************************************************************/

static int appFontGetWeight(	int *			pWeight,
				const char *		weight )
    {
    if  ( ! strcmp( weight, "thin" ) )
	{ *pWeight= FONTweightEXTRA_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "light" ) )
	{ *pWeight= FONTweightLIGHT; return 0;	}

    if  ( ! strcmp( weight, "demilight" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "semilight" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "book" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "roman" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "medium" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "normal" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "regular" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}

    if  ( ! strcmp( weight, "semi bold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demi bold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "semibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demi" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}

    if  ( ! strcmp( weight, "bold" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}
    if  ( ! strcmp( weight, "heavy" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}

    if  ( ! strcmp( weight, "ultrablack" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "ultrabold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extrablack" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extrabold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extra bold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}

    if  ( ! strcmp( weight, "black" ) )
	{ *pWeight= FONTweightULTRA_BOLD; return 0;	}

    SDEB(weight); return -1;
    }

/************************************************************************/
/*									*/
/*  Try to make something of the font set width field.			*/
/*									*/
/*  As the content of the field in not covered by a standard, we have	*/
/*  to live with what font designerd and font servers choose to reoport.*/
/*									*/
/************************************************************************/

static int appFontGetWidth(	int *			pWidth,
				const char *		swdth )
    {
    if  ( ! strcmp( swdth, "narrow" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "extra condensed" ) )
	{ *pWidth= FONTwidthEXTRA_CONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "condensed" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "semi condensed" ) )
	{ *pWidth= FONTwidthSEMI_CONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "semicondensed" ) )
	{ *pWidth= FONTwidthSEMI_CONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "normal" ) )
	{ *pWidth= FONTwidthMEDIUM; return 0;	}

    if  ( ! strcmp( swdth, "semi extended" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semi expanded" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semiexpanded" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}

    if  ( ! strcmp( swdth, "extended" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "expanded" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}

    SDEB(swdth); return -1;
    }

static int appFontXCatalog(	AppDrawingData *	add,
				AppFontEncoding *	afe )
    {
    char			familyFormat[120];
	
    char **			fontList;
    int				fontCount;

    int				i;
    int				j;

    AppFontFamily *		aff;
    AppFontTypeface *		aft;

#   ifdef USE_MOTIF
    Display *			display= add->addDisplay;
#   endif

#   ifdef USE_GTK
    Display *			display= GDK_DISPLAY();
#   endif

    if  ( afe->afeXfontFamilyCount > 0 )
	{ return 0;	}

    appFontQueryString( familyFormat,
		EditFontAnyTemplate,	/* foundry,		*/
		EditFontAnyTemplate,	/* family,		*/
		EditFontAnyTemplate,	/* weight,		*/
		EditFontAnyTemplate,	/* slant,		*/
		EditFontAnyTemplate,	/* swdth,		*/
		EditFontAnyTemplate,	/* pxlsz,		*/
		afe->afeRegistry,
		afe->afeEncoding );

    fontList= XListFonts( display, familyFormat, 4000, &fontCount );

    if  ( fontCount == 4000 )
	{ LLDEB(fontCount,4000);	}

    for ( i= 0; i < fontCount; i++ )
	{
	char			foundry[40];
	char			family[40];
	char			familyName[60];

	char			weight[40];
	char			slant[40];
	char			swdth[40];
	char			faceName[60];

	char			ignore[40];
	char			spacing[40];
	char			avgwdth[40];

	char			pxlszStr[40];
	int *			sizes;
	int				pxlsz;

	if  ( appFontSplitXFont( fontList[i],
					foundry,
					family,
					weight,
					slant,
					swdth,
					pxlszStr,
					spacing,
					avgwdth,
					ignore,		/*  registry  */
					ignore ) )	/*  encoding  */
	    { LSDEB(i,fontList[i]); continue;	}

	if  ( strcmp( spacing, "m" ) && strcmp( spacing, "p" ) )
	    {
	    if  ( strcmp( spacing, "c" ) )
		{ SDEB(spacing);	}

	    continue;
	    }

	sprintf( familyName, "%s-%s", foundry, family );
	sprintf( faceName, "%s-%s-%s", weight, slant, swdth );

	aff= afe->afeXfontFamilies;
	for ( j= 0; j < afe->afeXfontFamilyCount; aff++, j++ )
	    {
	    if  ( ! strcmp( aff->affFontFamilyName, familyName ) )
		{ break;	}
	    }

	if  ( j >= afe->afeXfontFamilyCount )
	    {
	    aff= (AppFontFamily *)realloc( afe->afeXfontFamilies,
					(j+1)* sizeof(AppFontFamily) );
	    if  ( ! aff )
		{ LXDEB(j,aff); return -1;	}
	    afe->afeXfontFamilies= aff;

	    aff += j;

	    docInitFontFamily( aff );

	    aff->affFontFamilyName= strdup( familyName );
	    if  ( ! aff->affFontFamilyName )
		{ XDEB(aff->affFontFamilyName); return -1;	}

	    afe->afeXfontFamilyCount= j+ 1;
	    }

	aft= aff->affFaces;
	for ( j= 0; j < aff->affFaceCount; aft++, j++ )
	    {
	    if  ( ! strcmp( aft->aftFaceName, faceName )	)
		{ break;	}
	    }

	if  ( j >= aff->affFaceCount )
	    {
	    aft= (AppFontTypeface *)realloc( aff->affFaces,
					(j+1)* sizeof(AppFontTypeface) );
	    if  ( ! aft )
		{ LXDEB(j,aft); return -1;	}
	    aff->affFaces= aft;

	    aft += j;

	    docInitFontTypeface( aft );

	    aft->aftFaceName= strdup( faceName );
	    if  ( ! aft->aftFaceName )
		{ XDEB(aft->aftFaceName); return -1;	}

	    if  ( appFontGetWeight( &(aft->aftWeight), weight ) )
		{ SSDEB(aff->affFontFamilyName,weight);	}

	    if  ( appFontGetWidth( &(aft->aftWidth), swdth ) )
		{ SSDEB(aff->affFontFamilyName,swdth);	}

	    if  ( aft->aftWeight > FONTweightMEDIUM )
		{ aft->aftIsBold= 1;	}
	    else{ aft->aftIsBold= 0;	}

	    aft->aftIsSlanted= 0;
	    if  ( strcmp( slant, "r" )	)
		{
		if  ( ! strcmp( slant, "i" )	||
		      ! strcmp( slant, "o" )	)
		    { aft->aftIsSlanted= 1;	}
		else{ SDEB(slant);		}
		}

	    aft->aftIsScalable= 0;
	    aft->aftIsFixedWidth= ! strcmp( spacing, "m" );

	    if  ( aft->aftIsFixedWidth )
		{
		if  ( aff->affHasProportionalWidth )
		    { SDEB(fontList[i]);	}
		aff->affHasFixedWidth= 1;
		}
	    else{
		if  ( aff->affHasFixedWidth )
		    { SDEB(fontList[i]);	}
		aff->affHasProportionalWidth= 1;
		}

	    aff->affFaceCount= j+ 1;
	    }

	pxlsz= atoi( pxlszStr );
	if  ( pxlsz == 0 )
	    { aft->aftIsScalable= 1;	}
	else{
	    if  ( strcmp( avgwdth, "0" ) )
		{
		sizes= aft->aftSizes;
		for ( j= 0; j < aft->aftSizeCount; sizes++, j++ )
		    {
		    if  ( *sizes == pxlsz )
			{ break;	}
		    }

		if  ( j == aft->aftSizeCount )
		    {
		    sizes= (int *)realloc(
				    aft->aftSizes, (j+1)* sizeof(int) );
		    if  ( ! sizes )
			{ LXDEB(j,sizes); return -1;	}
		    aft->aftSizes= sizes;

		    sizes += j;

		    *sizes= pxlsz;

		    aft->aftSizeCount= j+ 1;
		    }
		}
	    }
	}

    XFreeFontNames( fontList );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get additional font properties.					*/
/*									*/
/************************************************************************/

static void appXFontProperties(	AppPhysicalFont *	apf )
    {
    XFontStruct *	xfs;

#   ifdef USE_MOTIF
    xfs= apf->apfFontStruct;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( apf->apfFontStruct );
#   endif

    if  ( ! XGetFontProperty( xfs,
		XA_UNDERLINE_THICKNESS, &apf->apfUnderlineThicknessPixels ) )
	{
	apf->apfUnderlineThicknessPixels= ( xfs->ascent+ xfs->descent )/12;

	if  ( apf->apfUnderlineThicknessPixels == 0 )
	    { apf->apfUnderlineThicknessPixels= 1;	}
	}

    if  ( ! XGetFontProperty( xfs,
		XA_UNDERLINE_POSITION, &apf->apfUnderLinePositionPixels ) )
	{
	apf->apfUnderLinePositionPixels= ( xfs->descent/ 2 );
	}

    if  ( ! XGetFontProperty( xfs, XA_X_HEIGHT, &apf->apfXHeightPixels ) )
	{ apf->apfXHeightPixels= ( xfs->ascent+ xfs->descent )/2;	}

    apf->apfAscentPixels= apf->apfFontStruct->ascent;
    apf->apfDescentPixels= apf->apfFontStruct->descent;
    }

/************************************************************************/
/*									*/
/*  Format the current font name in X11 terms.				*/
/*									*/
/************************************************************************/

static void appFontXFormatCurrent(	char *			target,
					const AppFontEncoding *	afe,
					const AppFontFamily *	aff,
					const AppFontTypeface *	aft,
					int			pxlsz )
    {
    char		family[40];
    char		foundry[40];
    char		weight[40];
    char		slant[40];
    char		swdth[40];

    const char *	registry= EditFontAnyTemplate;
    const char *	encoding= EditFontAnyTemplate;

    char		pxlszStr[40];

    strcpy( family, EditFontAnyTemplate );
    strcpy( foundry, EditFontAnyTemplate );
    strcpy( weight, EditFontAnyTemplate );
    strcpy( slant, EditFontAnyTemplate );
    strcpy( swdth, EditFontAnyTemplate );
    strcpy( pxlszStr, EditFontAnyTemplate );

    if  ( afe )
	{
	registry= afe->afeRegistry;
	encoding= afe->afeEncoding;
	}

    if  ( aff )
	{
	if  ( sscanf( aff->affFontFamilyName, "%[^-]-%[^-]",
						    foundry, family ) != 2 )
	    { SDEB(aff->affFontFamilyName); }
	}

    if  ( aft )
	{
	if  ( sscanf( aft->aftFaceName, "%[^-]-%[^-]-%[^-]",
						weight, slant, swdth ) != 3 )
	    { SDEB(aft->aftFaceName); }
	}

    if  ( pxlsz > 0 )
	{ sprintf( pxlszStr, "%d", pxlsz ); }

    appFontQueryString( target,
			    foundry, family, weight, slant, swdth,
			    pxlszStr, registry, encoding );

    return;
    }

/************************************************************************/
/*									*/
/*  Find an X11 font with a postscript style font.			*/
/*									*/
/*  1)  Obtain a list of the available X11 fonts.			*/
/*  2)  Try to find an almost exact match on the family name.		*/
/*	Have a preference for fonts with a foundry.			*/
/*  3)  Do not rely too much on scalable fonts. They look ugly and do	*/
/*	not always work transparently with remote displays.		*/
/*									*/
/************************************************************************/

static int appFontXFontUsingQuery(	char *			target,
					const AppFontTypeface *	aft,
					int			pixelSize )
    {
    char *	before= aft->aftXQueryFormat;
    char *	after;
    int		size;
    int		i;

    if  ( *before != '-' )
	{ SDEB(aft->aftXQueryFormat); return -1; }

    for ( i= 0; i < 6; i++ )
	{
	before= strchr( before+ 1, '-' );
	if  ( ! before )
	    { SDEB(aft->aftXQueryFormat); return -1;	}
	}

    after= strchr( before+ 1, '-' );
    if  ( ! after )
	{ SDEB(aft->aftXQueryFormat); return -1;	}

    size= before- aft->aftXQueryFormat+ 1;

    memcpy( target, aft->aftXQueryFormat, size );
    sprintf( target+ size, "%d", pixelSize );
    strcpy( target+ strlen( target ), after );

    return 0;
    }

int appFontXFont(	char *			target,
			AppDrawingData *	add,
			const AppFontFamily *	aff,
			int			encoding,
			AppFontTypeface *	aft,
			int			twipsSize,
			int			variant )
    {
    const AppFontFamily *	xaf;
    const AppFontTypeface *	xt;

    int				fam;
    int				face;

    int				pixelSize;
    int				turn;

    int				enc;
    AppFontEncoding *		afe;

    AppPhysicalFontList *	apfl= &(add->addPhysicalFontList);

    if  ( ! aff || ! aft )
	{ XXDEB(aff,aft); return -1;	}

    if  ( variant == DOCfontSUPERSCRIPT		||
	  variant == DOCfontSUBSCRIPT		)
	{
	pixelSize= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						SUPERSUB_SIZE( twipsSize ) );
	}
    else{
	pixelSize= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip, twipsSize );
	}

    if  ( apfl->apflGhostscriptFontmap )
	{
	if  ( utilFontmapReadMap( apfl->apflGhostscriptFontmap ) )
	    { SDEB(apfl->apflGhostscriptFontmap);	}
	}

    if  ( apfl->apflGhostscriptFontToXmapping )
	{
	if  ( utilFontmapReadT1Map( apfl->apflGhostscriptFontToXmapping ) )
	    { SDEB(apfl->apflGhostscriptFontToXmapping);	}
	}

    if  ( aft->aftXQueryFormat )
	{
	if  ( appFontXFontUsingQuery( target, aft, pixelSize ) )
	    { SDEB(aft->aftXQueryFormat); return -1;	}

	return 0;
	}

    afe= AppFontXEncodings;
    for ( enc= 0;
	  enc < sizeof(AppFontXEncodings)/sizeof(AppFontEncoding);
	  afe++, enc++ )
	{
	if  ( afe->afeCharsetEncoding == encoding )
	    { break;	}
	}

    if  ( enc >= sizeof(AppFontXEncodings)/sizeof(AppFontEncoding) )
	{
	SLDEB(aff->affFontFamilyName,enc);
	LLDEB(enc,sizeof(AppFontXEncodings)/sizeof(AppFontEncoding));
	return -1;
	}

    /*  1  */
    appFontXCatalog( add, afe );

    /*  2  */
    for ( turn= 0; turn < 2; turn++ )
	{
	xaf= afe->afeXfontFamilies;
	for ( fam= 0; fam < afe->afeXfontFamilyCount; xaf++, fam++ )
	    {
	    char *	xf= strchr( xaf->affFontFamilyName, '-' );
	    char *	psf= aff->affFontFamilyName;

	    if  ( turn == 0 && xf == xaf->affFontFamilyName )
		{ continue;	}

	    if  ( ! xf )
		{ SDEB(xaf->affFontFamilyName); continue;	}
	    xf++;

	    if  ( ! strcmp( xf, aff->affFontFamilyName ) )
		{ break;	}

	    while( ( *xf && *psf )					     &&
		   ( *xf == *psf					||
		     ( isupper( *psf ) && tolower( *psf ) == *xf )	)    )
		{ xf++; psf++;	}

	    if  ( ! *xf && ! * psf )
		{ break;	}
	    }

	if  ( fam < afe->afeXfontFamilyCount )
	    { break;	}
	}
    
    if  ( fam >= afe->afeXfontFamilyCount )
	{
	AfmFontInfo *	afi= (AfmFontInfo *)aft->aftPrintingData;
	const char *	entry;

	entry= utilFontmapGetEntry( afi->afiFontName );
	if  ( entry )
	    {
	    const char *	t1entry;

	    t1entry= utilFontmapGetT1Entry( entry );

	    if  ( t1entry )
		{
		aft->aftXQueryFormat= strdup( t1entry );
		if  ( ! aft->aftXQueryFormat )
		    { XDEB(aft->aftXQueryFormat); return -1;	}

		if  ( appFontXFontUsingQuery( target, aft, pixelSize ) )
		    { SDEB(aft->aftXQueryFormat); return -1;	}

		return 0;
		}
	    }

	SLDEB(aff->affFontFamilyName,enc); return -1;
	}

    {
    int		faceFound= xaf->affFaceCount;
    int		difCountFound= 999;
    double	distanceFound= 999;

    xt= xaf->affFaces;
    for ( face= 0; face < xaf->affFaceCount; xt++, face++ )
	{
	int		difCount;
	double		distance;

	utilFontFaceDistance( &difCount, &distance, xt, aft );

	if  ( difCount < difCountFound		&&
	      difCount < 2			&&
	      distance < distanceFound		&&
	      distance < 0.3			)
	    {
	    faceFound= face;
	    difCountFound= difCount;
	    distanceFound= distance;
	    }
	}

    face= faceFound;
    xt= xaf->affFaces+ face;

    if  ( face >= xaf->affFaceCount )
	{ SDEB(aft->aftFaceName); return -1;	}
    }

    /*  8  */
    if  ( /* ! xt->aftIsScalable && */ xt->aftSizeCount > 0 )
	{
	int		i;

	for ( i= 0; i < xt->aftSizeCount; i++ )
	    {
	    if  ( xt->aftSizes[i] >= pixelSize )
		{ break;	}
	    }

	if  ( i >= xt->aftSizeCount )
	    { i= xt->aftSizeCount-1;	}

	if  ( i > 0				&&
	      xt->aftSizes[i] > pixelSize	)
	    {
	    int		bigger= 0;

	    if  ( pixelSize < 8 )
		{ bigger= 1;	}

	    if  ( xt->aftSizes[i]- ( pixelSize+ bigger )	>=
		  ( pixelSize+ bigger )- xt->aftSizes[i- 1]	)
		{ i--; }
	    }

	if  ( ! xt->aftIsScalable )
	    { pixelSize= xt->aftSizes[i];	}
	else{
	    if  ( xt->aftSizes[i] > pixelSize				&&
		  10* ( xt->aftSizes[i]- pixelSize ) <= pixelSize	)
		{ pixelSize= xt->aftSizes[i];	}
	    else{
		if  ( xt->aftSizes[i] < pixelSize			&&
		      10* ( pixelSize- xt->aftSizes[i] ) <= pixelSize	)
		    { pixelSize= xt->aftSizes[i];	}
		}
	    }
	}

    appFontXFormatCurrent( target, afe, xaf, xt, pixelSize );

    return 0;
    }
	    

/************************************************************************/
/*									*/
/*  Get and possibly open a font.					*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*  4)  Try to find an exact match based on the name.			*/
/*  4a) Try the first word.						*/
/*  5)  Try to map 'fnil' to 'Helvetica'.				*/
/*  6)  Try to map 'fswiss' to 'Helvetica'.				*/
/*  7)  Try to map 'froman' to 'Times'.					*/
/*  8)  Chose the first family that has a fixed width font for 'fmodern'*/
/*  9)  Try to map 'fscript' to 'ITC Zapf Chancery'.			*/
/*									*/
/************************************************************************/

static int appGetPhysicalFont(	AppPhysicalFont **	pAsf,
				AppFontFamily **	pAff,
				AppFontTypeface **	pAft,
				AppPhysicalFontList *	apfl,
				DocumentFont *		df,
				const TextAttribute *	ta )
    {
    int			fam;
    int			face;
    int			i;

    AppPhysicalFont *	apf;

    AppFontFamily *	psf;
    AppFontTypeface *	pst;

    /*  1  */
    apf= apfl->apflFonts;
    for ( i= 0; i < apfl->apflCount; apf++, i++ )
	{
	TextAttribute	taAsf= apf->apfAttribute;

	if  ( docEqualFont( &taAsf, ta ) )
	    { *pAsf= apf; return i; }
	}

    /*  2  */
    apf= (AppPhysicalFont *)realloc( apfl->apflFonts,
				(apfl->apflCount+ 1)* sizeof(AppPhysicalFont) );
    if  ( ! apf )
	{ XDEB(apf); return -1;	}
    apfl->apflFonts= apf;
    apf += apfl->apflCount;

    apf->apfAttribute= *ta;
    apf->apfDocFamilyNumber= -1;

    apf->apfPsFamilyNumber= -1;
    apf->apfFontEncoding= -1;
    apf->apfPsFaceNumber= -1;
    apf->apfFontStruct= (APP_FONT *)0;
    apf->apfScapsFontStruct= (APP_FONT *)0;
    apf->apfUnderLinePositionPixels= 0;
    apf->apfUnderlineThicknessPixels= 0;
    apf->apfAscentPixels= 0;
    apf->apfDescentPixels= 0;
    apf->apfFullSizePixels= 0;
    apf->apfXHeightPixels= 0;

    if  ( utilFontGetPsFont( &fam, &face, &psf, &pst, &(apf->apfFontEncoding),
					    apfl->apflAfmDirectory, df, ta ) )
	{ LDEB(1); return -1;	}

    apf->apfPsFamilyNumber= fam;
    apf->apfPsFaceNumber= face;

    if  ( df->dfEncodingUsed < 0 )
	{ df->dfEncodingUsed= apf->apfFontEncoding;	}
    else{
	if  ( df->dfEncodingUsed != apf->apfFontEncoding )
	    { LLDEB(df->dfEncodingUsed,apf->apfFontEncoding);	}
	}

    *pAsf= apf;
    *pAff= psf;
    *pAft= pst;

    return 0;
    }

static int appFontOpenXFont(	AppDrawingData *	add,
				AppPhysicalFont *	apf,
				const AppFontFamily *	psf,
				AppFontTypeface *	pst,
				const TextAttribute	ta )
    {
    char		scratch[120];

    apf->apfFullSizePixels= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
					    10* ta.taFontSizeHalfPoints );

    if  ( appFontXFont( scratch, add, psf, apf->apfFontEncoding, pst,
				10* ta.taFontSizeHalfPoints, ta.taSuperSub ) )
	{ LDEB(1); return -1;	}

#   ifdef USE_MOTIF
    apf->apfFontStruct= XLoadQueryFont( add->addDisplay, scratch );
    if  ( ! apf->apfFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

#   ifdef USE_GTK
    apf->apfFontStruct= gdk_font_load( scratch );
    if  ( ! apf->apfFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

    appXFontProperties( apf );

    if  ( appFontXFont( scratch, add, psf, apf->apfFontEncoding, pst,
				8* ta.taFontSizeHalfPoints, ta.taSuperSub ) )
	{ LDEB(1); return -1;	}

#   ifdef USE_MOTIF
    apf->apfScapsFontStruct= XLoadQueryFont( add->addDisplay, scratch );
    if  ( ! apf->apfScapsFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

#   ifdef USE_GTK
    apf->apfScapsFontStruct= gdk_font_load( scratch );
    if  ( ! apf->apfScapsFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Open an instance of a font for a document.				*/
/*									*/
/*  1)  Was a sensible font set in the document? If not just take the	*/
/*	first one that is available.					*/
/*  2)  To make users of SGML tools not too unhappy, just complain once.*/
/*									*/
/************************************************************************/

static int appGetFontInstance(	DocumentFont **			pDf,
				DocumentFontInstance **		pDfi,
				const DocumentFontList *	dfl,
				TextAttribute			ta )
    {
    int				i;
    DocumentFont *		df;
    DocumentFontInstance *	dfi;

    /*  1  */
    if  ( ta.taFontNumber < 0 || ta.taFontNumber >= dfl->dflCount )
	{
	static int	complained;

	/*  2  */
	if  ( ! complained )
	    { LLDEB(ta.taFontNumber,dfl->dflCount); complained= 1;	}

	for ( i= 0; i < dfl->dflCount; i++ )
	    {
	    if  ( dfl->dflFonts[i].dfDocFamilyNumber >= 0 )
		{ ta.taFontNumber= i; break;	}
	    }

	if  ( ta.taFontNumber < 0 || ta.taFontNumber >= dfl->dflCount )
	    { LLDEB(ta.taFontNumber,dfl->dflCount); return -1; }
	}

    df= dfl->dflFonts+ ta.taFontNumber;

    dfi= df->dfInstances;
    for ( i= 0; i < df->dfInstanceCount; dfi++, i++ )
	{
	if  ( docEqualFont( &(dfi->dfiAttribute), &ta ) )
	    { break;	}
	}

    if  ( i == df->dfInstanceCount )
	{
	dfi= (DocumentFontInstance *)realloc( df->dfInstances,
		( df->dfInstanceCount+ 1 )* sizeof(DocumentFontInstance) );
	if  ( ! dfi )
	    { LXDEB(df->dfInstanceCount+ 1,dfi); return -1; }

	df->dfInstances= dfi;

	dfi += df->dfInstanceCount++;

	dfi->dfiAttribute= ta;
	dfi->dfiPhysicalFont= -1;
	dfi->dfiPsFaceNumber= -1;
	}

    *pDf= df; *pDfi= dfi; return 0;
    }

int appOpenDocumentFont(	AppDrawingData *		add,
				const DocumentFontList *	dfl,
				TextAttribute			ta )
    {
    DocumentFont *		df;
    DocumentFontInstance *	dfi;

    if  ( appGetFontInstance( &df, &dfi, dfl, ta ) )
	{ LDEB(1); return -1;	}

    if  ( dfi->dfiPhysicalFont < 0 )
	{
	AppPhysicalFont *	apf;

	AppFontFamily *		psf;
	AppFontTypeface *	pst;

	if  ( appGetPhysicalFont( &apf, &psf, &pst,
		    &(add->addPhysicalFontList), df, &(dfi->dfiAttribute) ) )
	    { SLDEB(df->dfName,df->dfDocFamilyNumber); return -1;	}

	dfi->dfiPsFaceNumber= apf->apfPsFaceNumber;
	df->dfPsFamilyNumber= apf->apfPsFamilyNumber;

	dfi->dfiPhysicalFont= add->addPhysicalFontList.apflCount++;

#	ifdef USE_MOTIF
	if  ( add->addDisplay && appFontOpenXFont( add, apf, psf, pst, ta ) )
	    { LDEB(1); return -1;	}
#	endif

#	ifdef USE_GTK
	if  ( add->addForScreenDrawing				&&
	      appFontOpenXFont( add, apf, psf, pst, ta )	)
	    { LDEB(1); return -1;	}
#	endif
	}

    return dfi->dfiPhysicalFont;
    }

/************************************************************************/
/*									*/
/*  Format the current font name.					*/
/*									*/
/************************************************************************/

void appFontFormatCurrent(	char *			target,
				AppFontFamily *		aff,
				AppFontTypeface *	aft,
				int			sizeHalfPoints )
    {
    const char *	familyText= "*";
    const char *	faceName= "*";
    char		sizeStr[40];

    sizeStr[0]= '*';
    sizeStr[1]= '\0';

    if  ( aff )
	{
	if  ( aff->affFontFamilyText )
	    { familyText= aff->affFontFamilyText;	}
	else{ familyText= aff->affFontFamilyName;	}
	}

    if  ( aft )
	{ faceName= aft->aftFaceName;	}

    if  ( sizeHalfPoints > 0 )
	{
	if  ( sizeHalfPoints % 2 )
	    { sprintf( sizeStr, "%d.5", sizeHalfPoints/ 2 );	}
	else{ sprintf( sizeStr, "%d", sizeHalfPoints/ 2 );	}
	}

    sprintf( target, "%s,%s,%s", familyText, faceName, sizeStr );

    return;
    }

void appInitFontList(	AppPhysicalFontList *	apfl )
    {
    apfl->apflAfmDirectory= (char *)0;
    apfl->apflGhostscriptFontmap= (char *)0;
    apfl->apflGhostscriptFontToXmapping= (char *)0;

    apfl->apflCount= 0;
    apfl->apflFonts= (AppPhysicalFont *)0;

    return;
    }

void appCleanFontList(	AppDrawingData *	add,
			AppPhysicalFontList *	apfl )
    {
    int		i;

    for ( i= 0; i < apfl->apflCount; i++ )
	{
	if  ( apfl->apflFonts[i].apfFontStruct )
	    {
#	    ifdef USE_MOTIF
	    XFreeFont( add->addDisplay, apfl->apflFonts[i].apfFontStruct );
#	    endif

#	    ifdef USE_GTK
	    gdk_font_unref( apfl->apflFonts[i].apfFontStruct );
#	    endif
	    }

	if  ( apfl->apflFonts[i].apfScapsFontStruct )
	    {
#	    ifdef USE_MOTIF
	    XFreeFont( add->addDisplay, apfl->apflFonts[i].apfScapsFontStruct );
#	    endif

#	    ifdef USE_GTK
	    gdk_font_unref( apfl->apflFonts[i].apfScapsFontStruct );
#	    endif
	    }
	}

    if  ( apfl->apflFonts )
	{ free( apfl->apflFonts );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Does a character exist in a font?					*/
/*									*/
/*  1)  Do not trust 'xfs->all_chars_exist' by itself it is often	*/
/*	wrong. But when the other mechanism is bound to fail, we have	*/
/*	to use this unreliable struct member anyway.			*/
/*									*/
/************************************************************************/

int appCharExistsInFont(	const APP_FONT *	fs,
				int			ch )
    {
    const XFontStruct *	xfs;

#   ifdef USE_MOTIF
    xfs= fs;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( (APP_FONT *)fs );
#   endif

    if  ( xfs->min_char_or_byte2 >= xfs->max_char_or_byte2	&&
	  xfs->all_chars_exist					)
	{ return 1;	}
    else{
	if  ( ! xfs->per_char )
	    {
	    if  ( ch >= xfs->min_char_or_byte2	&&
		  ch <= xfs->max_char_or_byte2	)
		{ return 1;	}

	    return 0;
	    }
	else{
	    if  ( ch >= xfs->min_char_or_byte2			&&
		  ch <= xfs->max_char_or_byte2			&&
		  xfs->per_char[ch- xfs->min_char_or_byte2].width != 0 )
		{ return 1;	}

	    return 0;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Allocate resources for drawing vertical text.			*/
/*									*/
/************************************************************************/

void appInitVerticalXFont(	VerticalXFont *	vxf )
    {
    int		i;

    vxf->vxfScratchBitmapGc= None;
    vxf->vxfPixmapGc= None;
    vxf->vxfFontStruct= (APP_FONT *)0;
    vxf->vxfScratchBitmap= None;
    vxf->vxfDirection= 0;

    for ( i= 0; i < 256; i++ )
	{ vxf->vxfCharacters[i].vcPixmap= (APP_BITMAP_IMAGE)0;	}
    }

void appCleanVerticalXFont(	VerticalXFont *		vxf,
				AppDrawingData *	add )
    {
    int		i;

#   ifdef USE_MOTIF
    if  ( vxf->vxfScratchBitmapGc )
	{ XFreeGC( add->addDisplay, vxf->vxfScratchBitmapGc );	}
    if  ( vxf->vxfPixmapGc )
	{ XFreeGC( add->addDisplay, vxf->vxfPixmapGc );	}
#   endif

#   ifdef USE_GTK
    if  ( vxf->vxfScratchBitmapGc )
	{ gdk_gc_unref( vxf->vxfScratchBitmapGc );	}
    if  ( vxf->vxfPixmapGc )
	{ gdk_gc_unref( vxf->vxfPixmapGc );		}
#   endif

    if  ( vxf->vxfFontStruct )
	{ appDrawFreeFont( add, vxf->vxfFontStruct );	}

    if  ( vxf->vxfScratchBitmap )
	{ appDrawFreePixmap( add, vxf->vxfScratchBitmap );	}

    for ( i= 0; i < 256; i++ )
	{
	if  ( vxf->vxfCharacters[i].vcPixmap )
	    {
	    appDrawFreePixmap( add, vxf->vxfCharacters[i].vcPixmap );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Open a font and initialize the structures to use it to draw		*/
/*  vertical text.							*/
/*									*/
/************************************************************************/

int appOpenVerticalXFont(	VerticalXFont *		vxf,
				AppDrawingData *	add,
				const char *		name,
				int			direction,
				APP_COLOR_RGB *		background )
    {
    XFontStruct *	xfs;

    if  ( direction == 0 )
	{ LDEB(direction); return -1;	}

    if  ( direction > 0 )
	{ vxf->vxfDirection=  1;	}
    if  ( direction < 0 )
	{ vxf->vxfDirection= -1;	}

    vxf->vxfFontStruct= appDrawOpenFont( add, name );
    if  ( ! vxf->vxfFontStruct )
	{ SXDEB(name,vxf->vxfFontStruct); return -1;	}

#   ifdef USE_MOTIF
    xfs= vxf->vxfFontStruct;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( vxf->vxfFontStruct );
#   endif

    vxf->vxfScratchBitmap= appMakeBitmap( add,
					    xfs->max_bounds.width,
					    xfs->max_bounds.ascent+
					    xfs->max_bounds.descent );

    if  ( ! vxf->vxfScratchBitmap )
	{ XDEB(vxf->vxfScratchBitmap); return -1;	}

#   ifdef USE_MOTIF
    vxf->vxfScratchBitmapGc= XCreateGC( add->addDisplay,
				vxf->vxfScratchBitmap, 0, (XGCValues *)0 );
    if  ( ! vxf->vxfScratchBitmapGc )
	{ XDEB(vxf->vxfScratchBitmapGc); return -1;	}
    vxf->vxfPixmapGc= XCreateGC( add->addDisplay, add->addDrawable,
							0, (XGCValues *)0 );
    if  ( ! vxf->vxfPixmapGc )
	{ XDEB(vxf->vxfPixmapGc); return -1;	}

    XSetFont( add->addDisplay, vxf->vxfScratchBitmapGc,
						vxf->vxfFontStruct->fid );

    XSetBackground( add->addDisplay, vxf->vxfPixmapGc,
			    background->pixel );
    XSetForeground( add->addDisplay, vxf->vxfPixmapGc,
			    BlackPixel( add->addDisplay, add->addScreen ) );
#   endif

#   ifdef USE_GTK
    vxf->vxfScratchBitmapGc= gdk_gc_new( vxf->vxfScratchBitmap );
    if  ( ! vxf->vxfScratchBitmapGc )
	{ XDEB(vxf->vxfScratchBitmapGc); return -1;	}
    vxf->vxfPixmapGc= gdk_gc_new( add->addDrawable );
    if  ( ! vxf->vxfPixmapGc )
	{ XDEB(vxf->vxfPixmapGc); return -1;	}

    gdk_gc_set_font( vxf->vxfScratchBitmapGc, vxf->vxfFontStruct );

    gdk_gc_set_background( vxf->vxfPixmapGc, background );
    /*
    gdk_gc_set_foreground( vxf->vxfPixmapGc, appDrawBlackColor( add ) );
    */
#   endif

    return 0;
    }

int appDrawVerticalString(	VerticalXFont *		vxf,
				AppDrawingData *	add,
				int			x,
				int			y,
				char *			string,
				int			length )
    {
    int			angle;

#   ifdef USE_MOTIF
    Display *			display= add->addDisplay;
    XFontStruct*		xfs= vxf->vxfFontStruct;
#   endif

#   ifdef USE_GTK
    XFontStruct*		xfs= GDK_FONT_XFONT( vxf->vxfFontStruct );
#   endif

    if  ( vxf->vxfDirection < 0 )
	{ angle=  90; x -= xfs->ascent;		}
    else{ angle= 270; x -= xfs->descent;	}

    while( length > 0 )
	{
	int		width;
	int		ch= string[0]&0xff;

	if  ( ! vxf->vxfCharacters[ch].vcPixmap )
	    {
	    XCharStruct *	xcs;
#	    ifdef USE_MOTIF
	    int			screen= DefaultScreen( display );
	    Visual *		visual= DefaultVisual( display, screen);
#	    endif
	    int			depth= 1;

	    APP_IMAGE *		xim;

	    unsigned char *	bufferIn;
	    unsigned char *	bufferOut= (unsigned char *)0;

	    BitmapDescription	bdIn;
	    BitmapDescription	bdOut;

	    bmInitDescription( &bdIn );
	    bmInitDescription( &bdOut );

	    if  ( ! xfs->per_char )
		{ xcs= &(xfs->max_bounds);	}
	    else{
		if  ( ch < xfs->min_char_or_byte2	||
		      ch > xfs->max_char_or_byte2	)
		    { length--; string++; continue;	}

		xcs= xfs->per_char+ ch- xfs->min_char_or_byte2;
		}

#	    ifdef USE_GTK
	    LDEB(1); return -1;
#	    endif

#	    ifdef USE_MOTIF
	    XSetForeground( display, vxf->vxfScratchBitmapGc, 0 );
	    XFillRectangle( display, vxf->vxfScratchBitmap,
			    vxf->vxfScratchBitmapGc,
			    0, 0, xcs->width,
			    xfs->max_bounds.ascent+ xfs->max_bounds.descent );

	    XSetForeground( display, vxf->vxfScratchBitmapGc, 1 );
	    XDrawString( display, vxf->vxfScratchBitmap,
					vxf->vxfScratchBitmapGc,
					0, xfs->max_bounds.ascent,
					string, 1 );

	    xim= XGetImage( display, vxf->vxfScratchBitmap,
			    0, 0, xcs->width,
			    xfs->max_bounds.ascent+ xfs->max_bounds.descent,
			    AllPlanes, XYPixmap );

	    if  ( ! xim )
		{ XDEB(xim); return -1;	}

	    if  ( xim->depth != 1 )
		{ LDEB(xim->depth); return -1;	}
#	    endif

	    bdIn.bdPixelsWide= xim->width;
	    bdIn.bdPixelsHigh= xim->height;
#	    ifdef USE_MOTIF
	    bdIn.bdBytesPerRow= xim->bytes_per_line;
#	    endif
#	    ifdef USE_GTK
	    bdIn.bdBytesPerRow= xim->bpl;
#	    endif
	    bdIn.bdBufferLength= bdIn.bdPixelsHigh* bdIn.bdBytesPerRow;

	    bdIn.bdBitsPerSample= 1;
	    bdIn.bdSamplesPerPixel= 1;
	    bdIn.bdBitsPerPixel= 1;

	    bdIn.bdXResolution= 1;
	    bdIn.bdYResolution= 1;
	    bdIn.bdUnit= BMunPIXEL;
	    bdIn.bdColorEncoding= BMcoBLACKWHITE;
	    bdIn.bdHasAlpha= 0;

#	    ifdef USE_MOTIF
	    if  ( xim->bitmap_bit_order == LSBFirst )
		{
		bmFlipBytes( (unsigned char *)xim->data,
				    bdIn.bdBufferLength, bdIn.bdBitsPerPixel );
		}
#	    endif

#	    ifdef USE_MOTIF
	    bufferIn= (unsigned char *)xim->data;
#	    endif
#	    ifdef USE_GTK
	    bufferIn= (unsigned char *)xim->mem;
#	    endif

	    /*
	    bmWrite( "/tmp/in.tiff",
				bufferIn, &bdIn, 0, 1.0 );
	    */

	    if  ( bmRotate90( &bdOut, &bdIn,
				&bufferOut, bufferIn, angle ) )
		{ LDEB(angle); return -1;	}

	    /* bmWrite( "/tmp/out.tiff", bufferOut, &bdOut, 0, 1.0 ); */

#	    ifdef USE_MOTIF
	    XDestroyImage( xim );
#	    endif
#	    ifdef USE_GTK
	    gdk_image_destroy( xim );
#	    endif

	    bmCleanDescription( &bdIn );

	    depth= 1;

#	    ifdef USE_MOTIF
	    vxf->vxfCharacters[ch].vcPixmap= XCreatePixmap(
				add->addDisplay, add->addDrawable,
				bdOut.bdPixelsWide, bdOut.bdPixelsHigh, depth );

	    if  ( ! vxf->vxfCharacters[ch].vcPixmap )
		{ XDEB(vxf->vxfCharacters[ch].vcPixmap); return -1;	}

	    xim= XCreateImage( add->addDisplay, visual, 1, XYBitmap,
				    0, (char *)bufferOut,
				    bdOut.bdPixelsWide, bdOut.bdPixelsHigh,
				    8, bdOut.bdBytesPerRow );
	    if  ( ! xim )
		{ XDEB(xim); return -1;	}

	    if  ( xim->bitmap_bit_order == LSBFirst )
		{
		bmFlipBytes( (unsigned char *)xim->data,
				bdOut.bdBufferLength, bdOut.bdBitsPerPixel );
		}

	    XSetBackground( add->addDisplay, vxf->vxfScratchBitmapGc, 0 );
	    XSetForeground( add->addDisplay, vxf->vxfScratchBitmapGc, 1 );

	    XPutImage( add->addDisplay, vxf->vxfCharacters[ch].vcPixmap,
				    vxf->vxfScratchBitmapGc, xim, 0, 0, 0, 0,
				    bdOut.bdPixelsWide, bdOut.bdPixelsHigh );
#	    endif

#	    ifdef USE_MOTIF
	    xim->data= (char *)0;
	    XDestroyImage( xim );
#	    endif
#	    ifdef USE_GTK
	    xim->mem= (char *)0;
	    gdk_image_destroy( xim );
#	    endif

	    bmCleanDescription( &bdOut );
	    free( bufferOut );

	    vxf->vxfCharacters[ch].vcWidth= xcs->width;
	    }

	width= vxf->vxfCharacters[ch].vcWidth;

	if  ( vxf->vxfDirection < 0 )
	    { y -= width;	}

#	ifdef USE_MOTIF
	XSetClipMask( add->addDisplay, add->addGc,
					    vxf->vxfCharacters[ch].vcPixmap );
	XSetClipOrigin( add->addDisplay, add->addGc, x, y );

	XFillRectangle( add->addDisplay, add->addDrawable, add->addGc, x, y,
		    xfs->max_bounds.ascent+ xfs->max_bounds.descent,
		    width );

	XSetClipMask( add->addDisplay, add->addGc, None );
#	endif

	if  ( vxf->vxfDirection > 0 )
	    { y += width;	}

	length--; string++;
	}

    return 0;
    }
