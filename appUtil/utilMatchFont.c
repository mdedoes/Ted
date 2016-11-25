/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   include	"utilPropMask.h"
#   include	"utilDocFont.h"
#   include	"psFont.h"
#   include	"utilMatchFont.h"

/************************************************************************/
/*									*/
/*  An half hearted attempt to derive the font style from the family	*/
/*  name.								*/
/*									*/
/************************************************************************/

int utilFontFamilyStyle(	const char *	fontFamilyName )
    {
    int		l= strlen( fontFamilyName );

    while( l > 0 )
	{
	int		past= -1;
	int		i;

	if  ( l == 5 && ! strncmp( fontFamilyName, "Arial", 5 ) )
	    { return DFstyleFSWISS;	}

	if  ( l == 9 && ! strncmp( fontFamilyName, "Helvetica", 9 ) )
	    { return DFstyleFSWISS;	}

	if  ( l == 5 && ! strncmp( fontFamilyName, "Times", 5 ) )
	    { return DFstyleFROMAN;	}

	if  ( l == 8 && ! strncmp( fontFamilyName, "Palatino", 8 ) )
	    { return DFstyleFROMAN;	}

	if  ( l == 6 && ! strncmp( fontFamilyName, "Symbol", 6 ) )
	    { return DFstyleFTECH;	}

	if  ( l == 7 && ! strncmp( fontFamilyName, "Courier", 7 ) )
	    { return DFstyleFMODERN;	}

	if  ( l == 5 && ! strncmp( fontFamilyName, "Ohlfs", 5 ) )
	    { return DFstyleFMODERN;	}

	if  ( l == 9 && ! strncmp( fontFamilyName, "Wingdings", 9 ) )
	    { return DFstyleFTECH;	}
	if  ( l == 8 && ! strncmp( fontFamilyName, "Webdings", 8 ) )
	    { return DFstyleFTECH;	}

	if  ( l == 7 && ! strncmp( fontFamilyName, "Corsiva", 7 ) )
	    { return DFstyleFSCRIPT;	}

	for ( i= 0; i < l- 1; i++ )
	    {
	    if  ( fontFamilyName[i] != ' ' && fontFamilyName[i+ 1] == ' ' )
		{ past= i+ 1;	}
	    }
	if  ( past <= 0 )
	    { break;	}
	l= past;

	for ( i= 0; i < l; i++ )
	    {
	    if  ( fontFamilyName[i] != ' ' )
		{ past= i+ 1;	}
	    }
	l= past;
	}

    return DFstyleFNIL;
    }

/************************************************************************/
/*									*/
/*  1)  Give an arbitrary measure of the distance between two faces,	*/
/*	typically of the same font.					*/
/*  2)  Compare two faces for the ordering in a fonts tool.		*/
/*									*/
/************************************************************************/

void utilFontFaceDistance(	int *			pDifCount,
				double *		pDistance,
				const AppFontTypeface *	aft1,
				int			isSlanted,
				int			weight )
    {
    double	d;

    double	distance= 0.0;
    int		difCount= 0;

    if  ( aft1->aftWeight != weight )
	{
	d= weight- aft1->aftWeight;
	d /= ( FONTweight_MAX- FONTweight_MIN );

	distance += d* d;
	difCount++;
	}

    if  ( aft1->aftIsSlanted != isSlanted )
	{
	d= isSlanted- aft1->aftIsSlanted;
	/* d /= 1.0; */

	distance += d* d;
	difCount++;
	}

    distance /= 2.0;
    distance= sqrt( distance );

    *pDifCount= difCount;
    *pDistance= distance;

    return;
    }

static void utilFontCompareProps(	int *		pCmp,
					int		weight,
					int		v1,
					int		v2 )
    {
    if  ( v1 > v2 )
	{ (*pCmp) += weight;	}
    if  ( v1 < v2 )
	{ (*pCmp) -= weight;	}

    return;
    }

/*  2  */
int utilFontCompareFaces(	const void *	vaft1,
				const void *	vaft2 )
    {
    const AppFontTypeface *	aft1= (AppFontTypeface *)vaft1;
    const AppFontTypeface *	aft2= (AppFontTypeface *)vaft2;

    int				cmp= 0;

    utilFontCompareProps( &cmp, 
			4, aft1->aftWidth, aft2->aftWidth );

    utilFontCompareProps( &cmp, 
			2, aft1->aftWeight, aft2->aftWeight );

    utilFontCompareProps( &cmp, 
			1, aft1->aftIsSlanted, aft2->aftIsSlanted );

    if  ( cmp > 0 )
	{ return  1;	}
    if  ( cmp < 0 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get a font.								*/
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

static int utilGetPsFamilyByText(
				int *				pFamily,
				const char *			name,
				int				encoding,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    const AppFontFamily *	psf;

    if  ( ! name )
	{ return -1;	}

    psf= psfl->psflFamilies;
    for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	{
	if  ( encoding != ENCODINGpsFONTSPECIFIC		&&
	      ! psf->affSupportedCharsets[encoding].scSupported	)
	    { continue;	}

	if  ( psf->affFontFamilyName			&&
	      ! strcmp( name, psf->affFontFamilyName )	)
	    { *pFamily= fam; return 0;	}
	}

    return -1;
    }

static int utilGetPsFamilyByName( int *				pFamily,
				const char *			name,
				int				encoding,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    const AppFontFamily *	psf;

    if  ( ! name )
	{ return -1;	}

    psf= psfl->psflFamilies;
    for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	{
	if  ( encoding != ENCODINGpsFONTSPECIFIC		&&
	      ! psf->affSupportedCharsets[encoding].scSupported	)
	    { continue;	}

	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    if  ( ! strncmp( name, "Times New Roman", 15 ) )
	{ name= "Times";	}
    if  ( ! strncmp( name, "Courier New", 11 ) )
	{ name= "Courier";	}
    if  ( ! strcmp( name, "Arial" ) )
	{ name= "Helvetica";	}
    if  ( ! strcmp( name, "Apple Chancery" ) )
	{ name= "ITC Zapf Chancery";	}
    if  ( ! strncmp( name, "Wingdings", 9 ) )
	{ name= "ITC Zapf Dingbats";	}
    if  ( ! strncmp( name, "Webdings", 8 ) )
	{ name= "ITC Zapf Dingbats";	}

    psf= psfl->psflFamilies;
    for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	{
	if  ( encoding != ENCODINGpsFONTSPECIFIC		&&
	      ! psf->affSupportedCharsets[encoding].scSupported	)
	    { continue;	}

	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    if  ( ! strncmp( name, "Wingdings", 9 ) )
	{ name= "Dingbats";	}
    if  ( ! strncmp( name, "Webdings", 8 ) )
	{ name= "Dingbats";	}

    psf= psfl->psflFamilies;
    for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	{
	if  ( encoding != ENCODINGpsFONTSPECIFIC		&&
	      ! psf->affSupportedCharsets[encoding].scSupported	)
	    { continue;	}

	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    return -1;
    }

static int utilGetPsFamilyByStyle(
				int *				pFamily,
				const DocumentFont *		df,
				int				encoding,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    const AppFontFamily *	psf;

    /*  5  */
    if  ( ! df->dfFamilyStyle			||
	  ! strcmp( df->dfFamilyStyle, "fnil" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "Helvetica", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  6  */
    if  ( ! strcmp( df->dfFamilyStyle, "fswiss" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "Helvetica", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  7  */
    if  ( ! strcmp( df->dfFamilyStyle, "froman" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "Times", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  8  */
    if  ( ! strcmp( df->dfFamilyStyle, "fmodern" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "Courier", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  9  */
    if  ( ! strcmp( df->dfFamilyStyle, "fscript" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "ITC Zapf Chancery", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  10  */
    if  ( ! strcmp( df->dfFamilyStyle, "fdecor" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "ITC Zapf Chancery", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  10  */
    if  ( ! strcmp( df->dfFamilyStyle, "ftech" ) )
	{
	psf= psfl->psflFamilies;
	for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	    {
	    if  ( encoding != ENCODINGpsFONTSPECIFIC			&&
		  ! psf->affSupportedCharsets[encoding].scSupported	)
		{ continue;	}

	    if  ( ! strcmp( "Symbol", psf->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Find the PostScript font family that best matches a document font	*/
/*									*/
/************************************************************************/

static int utilGetPsFamilyForFont(
				int *				pFamily,
				int *				pFontEncoding,
				const DocumentFont *		df,
				const PostScriptFontList *	psfl )
    {
    int		encoding;

    switch( df->dfCharset )
	{
	case FONTcharsetDEFAULT:
	    encoding= ENCODINGpsFONTSPECIFIC;
	    break;

	case FONTcharsetSYMBOL:
	    encoding= ENCODINGpsADOBE_SYMBOL;
	    break;

	case FONTcharsetOEM:
	    LDEB(df->dfCharset);
	    return -1;

	case FONTcharsetANSI:
	    encoding= ENCODINGpsISO_8859_1;
	    break;

	case FONTcharsetRUSSIAN:
	    encoding= ENCODINGpsADOBE_CYRILLIC;
	    break;

	case FONTcharsetEE:
	    encoding= ENCODINGpsISO_8859_2;
	    break;

	case FONTcharsetGREEK:
	    encoding= ENCODINGpsISO_8859_7;
	    break;

	case FONTcharsetTURKISH:
	    encoding= ENCODINGpsISO_8859_9;
	    break;

	case FONTcharsetBALTIC:
	    encoding= ENCODINGpsISO_8859_13;
	    break;

	case FONTcharsetHEBREW:
	case FONTcharsetARABIC:
	case FONTcharsetSHIFTJIS:
	case FONTcharsetHANGEUL:
	case FONTcharsetGB2313:
	case FONTcharsetCHINESEBIG5:
	default:
	    /*LDEB(df->dfCharset);*/
	    encoding= ENCODINGpsFONTSPECIFIC;
	    break;
	}

    /*  4  */
    if  ( utilGetPsFamilyByText(  pFamily, df->dfName, encoding, psfl )	&&
	  utilGetPsFamilyByName(  pFamily, df->dfName, encoding, psfl )	&&
	  utilGetPsFamilyByStyle( pFamily, df, encoding, psfl )		)
	{
	/* SSLLDEB(df->dfName,df->dfFamilyStyle,df->dfCharset,encoding);*/
	encoding= ENCODINGpsFONTSPECIFIC;
	}

    if  ( utilGetPsFamilyByText(  pFamily, df->dfName, encoding, psfl )	&&
	  utilGetPsFamilyByName(  pFamily, df->dfName, encoding, psfl )	&&
	  utilGetPsFamilyByStyle( pFamily, df, encoding, psfl )		)
	{
	SSLLDEB(df->dfName,df->dfFamilyStyle,df->dfCharset,encoding);
	/*
	{
	int		fam;

	for ( fam= 0; fam < psfl->psflFamilyCount; fam++ )
	    { LSDEB(fam,psfl->psflFamilies[fam].affFontFamilyName);	}
	}
	*/

	return -1;
	}

    if  ( encoding == ENCODINGpsFONTSPECIFIC )
	{ encoding= psfl->psflFamilies[*pFamily].affDefaultEncoding;	}

    *pFontEncoding= encoding;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Add the list of available PostScript fonts to the font list of the	*/
/*  document.								*/
/*									*/
/************************************************************************/

int utilAddPsFontsToDocList(	DocumentFontList *		dfl,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    const AppFontFamily *	psf;

    psf= psfl->psflFamilies;
    for ( fam= 0; fam < psfl->psflFamilyCount; psf++, fam++ )
	{
	int			enc;
	int			fac;
	const AppFontTypeface *	aft= psf->affFaces;
	short int		psFaceNumber[FONTface_COUNT];

	for ( fac= 0; fac < FONTface_COUNT; fac++ )
	    { psFaceNumber[fac]= -1; }

	for ( fac= 0; fac < psf->affFaceCount; aft++, fac++ )
	    {
	    psFaceNumber[FACE_INDEX(aft->aftIsSlanted,aft->aftIsBold)]= fac;
	    }

	if  ( psFaceNumber[0] < 0 )
	    {
	    for ( fac= 0; fac < FONTface_COUNT; fac++ )
		{ 
		if  ( psFaceNumber[fac] >= 0 )
		    { psFaceNumber[0]= psFaceNumber[fac]; break; }
		}
	    }
	for ( fac= 0; fac < FONTface_COUNT; fac++ )
	    { 
	    if  ( psFaceNumber[fac] < 0 )
		{ psFaceNumber[fac]= psFaceNumber[0]; }
	    }

	for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	    {
	    int				fontNumber;
	    DocumentFont *		df;
	    const char *		name= psf->affFontFamilyName;
	    DocumentFontFamily *	dff;

	    if  ( ! psf->affSupportedCharsets[enc].scSupported )
		{ continue;	}

	    fontNumber= docGetFontByName( dfl, name, enc );
	    if  ( fontNumber < 0 )
		{ SLDEB(name,fontNumber); return -1; }

	    df= dfl->dflFonts+ fontNumber;
	    if  ( df->dfPsFamilyNumber < 0 )
		{ df->dfPsFamilyNumber= fam;	}

	    dff= dfl->dflFamilies+ df->dfDocFamilyIndex;

	    if  ( df->dfEncodingSet < 0 )
		{ df->dfEncodingSet= enc; }
	    if  ( dff->dffFontForEncoding[enc] < 0 )
		{ dff->dffFontForEncoding[enc]= fontNumber;	}

	    for ( fac= 0; fac < FONTface_COUNT; fac++ )
		{
		if  ( psFaceNumber[fac] >= 0 )
		    { df->dfPsFaceNumber[fac]= psFaceNumber[fac];	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find PostScript fonts for the fonts in the font list.		*/
/*									*/
/*  1)  For all document fonts in the font list.			*/
/*  2)  If we already have a PostScript font with the font, we must be	*/
/*	happy.								*/
/*  3)  Match a PostScript family to this font. If this fails proceed	*/
/*	with the next font. Otherwise remember the match.		*/
/*  4)  If no encoding could be derived from explicit data such as \cpg	*/
/*	or \fcharset tags in the document, derive an encoding from the	*/
/*	PostScript font.						*/
/*  4a) Default encoding.						*/
/*  4b) The first encoding that is supported.				*/
/*  5)  Remember that this document font supports its encoding with the	*/
/*	document font family.						*/
/*  6)  Remember what faces are available. Derive the information from	*/
/*	the PostScript faces.						*/
/*									*/
/************************************************************************/

int utilFindPsFontsForDocFonts(	DocumentFontList *		dfl,
				const PostScriptFontList *	psfl )
    {
    int			font;
    DocumentFont *	df;

    /*  1  */
    df= dfl->dflFonts;
    for ( font= 0; font < dfl->dflFontCount; df++, font++ )
	{
	/*  2  */
	if  ( df->dfPsFamilyNumber >= 0 )
	    { continue;	}
	if  ( df->dfDocFontNumber < 0 )
	    { continue;	}
	if  ( ! df->dfName )
	    { continue;	}

	if  ( utilFindPsFontForDocFont( df, dfl, psfl ) )
	    { SDEB(df->dfName); continue;	}
	}

    return 0;
    }

int utilFindPsFontForDocFont(	DocumentFont *			df,
				DocumentFontList *		dfl,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    int				fac;
    int				encoding;

    const AppFontFamily *	psf;
    const AppFontTypeface *	aft;

    /*  3  */
    if  ( utilGetPsFamilyForFont( &fam, &encoding, df, psfl ) )
	{ SDEB(df->dfName); return -1; }

    df->dfPsFamilyNumber= fam;
    psf= psfl->psflFamilies+ fam;

    /*  4  */
    if  ( df->dfEncodingSet < 0 )
	{
	/*  4a  */
	if  ( psf->affDefaultEncoding >= 0 )
	    { df->dfEncodingSet= psf->affDefaultEncoding;	}
	else{
	    int		enc;

	    /*  4b  */
	    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
		{
		if  ( psf->affSupportedCharsets[enc].scSupported )
		    { df->dfEncodingSet= enc; break;	}
		}
	    }

	if  ( df->dfEncodingSet < 0 )
	    { SLDEB(df->dfName,df->dfEncodingSet);	}
	}
    else{
	if  ( encoding != df->dfEncodingSet )
	    { df->dfEncodingSet= encoding;	}
	}


    /*  5  */
    if  ( df->dfEncodingSet >= 0 )
	{
	DocumentFontFamily *	dff;

	dff= dfl->dflFamilies+ df->dfDocFamilyIndex;

	if  ( dff->dffFontForEncoding[df->dfEncodingSet] < 0 )
	    {
	    dff->dffFontForEncoding[df->dfEncodingSet]= df->dfDocFontNumber;
	    }
	}

    /*  6  */
    aft= psf->affFaces;
    for ( fac= 0; fac < psf->affFaceCount; aft++, fac++ )
	{
	df->dfPsFaceNumber[FACE_INDEX(aft->aftIsSlanted,aft->aftIsBold)]=
								    fac;
	}

    if  ( df->dfPsFaceNumber[0] < 0 )
	{
	for ( fac= 0; fac < FONTface_COUNT; fac++ )
	    { 
	    if  ( df->dfPsFaceNumber[fac] >= 0 )
		{ df->dfPsFaceNumber[0]= df->dfPsFaceNumber[fac]; break; }
	    }
	}
    for ( fac= 0; fac < FONTface_COUNT; fac++ )
	{ 
	if  ( df->dfPsFaceNumber[fac] < 0 )
	    { df->dfPsFaceNumber[fac]= df->dfPsFaceNumber[0]; }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find PostScript face for a text attribute.				*/
/*									*/
/************************************************************************/

AfmFontInfo * utilPsGetAfi(	int *				pEncoding,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl,
				const TextAttribute *		ta )
    {
    const AppFontFamily *		aff;
    const AppFontTypeface *		aft;
    const DocumentFont *		df;
    int					fac;

    if  ( ta->taFontNumber < 0 || ta->taFontNumber >= dfl->dflFontCount )
	{
	LLDEB(ta->taFontNumber,dfl->dflFontCount);
	return (AfmFontInfo *)0;
	}

    df= dfl->dflFonts+ ta->taFontNumber;

    if  ( df->dfPsFamilyNumber < 0			||
	  df->dfPsFamilyNumber >= psfl->psflFamilyCount	)
	{
	SLLDEB(df->dfName,df->dfPsFamilyNumber,psfl->psflFamilyCount);
	return (AfmFontInfo *)0;
	}

    aff= psfl->psflFamilies+ df->dfPsFamilyNumber;

    fac= df->dfPsFaceNumber[FACE_INDEX(ta->taFontIsSlanted,ta->taFontIsBold)];
    if  ( fac < 0 || fac >= aff->affFaceCount )
	{
	SDEB(df->dfName);
	LLDEB(ta->taFontIsSlanted,ta->taFontIsBold);
	LLDEB(fac,aff->affFaceCount);
	fac= 0;
	}

    aft= aff->affFaces+ fac;

    if  ( ! aft->aftPrintingData )
	{ XDEB(aft->aftPrintingData);	}

    *pEncoding= df->dfEncodingSet;
    return (AfmFontInfo *)aft->aftPrintingData;
    }

/************************************************************************/
/*									*/
/*  Derive a font weight from the string designation.			*/
/*									*/
/************************************************************************/

int utilFontWeightFromString(		int *		pWeight,
					int *		pStart,
					int *		pLength,
					const char *	name )
    {
    const char *	s= name;

    while( s[0] == ' ' )
	{ s++;	}

    while( *s )
	{
	if  ( ! strncmp( s, "Extra Light", 11 ) )
	    {
	    *pWeight= FONTweightEXTRA_LIGHT;
	    *pStart= s- name;
	    *pLength= 11;
	    return 1;
	    }

	if  ( ! strncmp( s, "Ultralight", 10 ) )
	    {
	    *pWeight= FONTweightEXTRA_LIGHT;
	    *pStart= s- name;
	    *pLength= 10;
	    return 1;
	    }

	if  ( ! strncmp( s, "Light", 5 ) )
	    {
	    *pWeight= FONTweightLIGHT;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }

	if  ( ! strncmp( s, "Book", 4 ) )
	    {
	    *pWeight= FONTweightSEMI_LIGHT;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Regular", 7 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 7;
	    return 1;
	    }
	if  ( ! strncmp( s, "Normal", 6 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }
	if  ( ! strncmp( s, "Medium", 6 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }
	if  ( ! strncmp( s, "Roman", 5 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }


	if  ( ! strncmp( s, "Semibold", 8 ) )
	    {
	    *pWeight= FONTweightSEMI_BOLD;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }
	if  ( ! strncmp( s, "Demibold", 8 ) )
	    {
	    *pWeight= FONTweightSEMI_BOLD;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }
	if  ( ! strncmp( s, "Demi", 4 ) )
	    {
	    *pWeight= FONTweightSEMI_BOLD;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Bold", 4 ) )
	    {
	    *pWeight= FONTweightBOLD;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Extrabold", 9 ) )
	    {
	    *pWeight= FONTweightEXTRA_BOLD;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }

	if  ( ! strncmp( s, "Heavy", 5 ) )
	    {
	    *pWeight= FONTweightEXTRA_BOLD;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }

	if  ( ! strncmp( s, "Black", 5 ) )
	    {
	    *pWeight= FONTweightULTRA_BOLD;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }


	while( s[0] && s[0] != ' ' )
	    { s++;	}
	while( s[0] == ' ' )
	    { s++;	}
	}

    return 0;
    }


int utilFontWidthFromString(		int *		pWidth,
					int *		pStart,
					int *		pLength,
					const char *	name )
    {
    const char *	s= name;

    while( s[0] == ' ' )
	{ s++;	}

    while( *s )
	{
	if  ( ! strncmp( s, "Narrow", 6 )		&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }

	if  ( ! strncmp( s, "Extra Condensed", 15 )	&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    {
	    *pWidth= FONTwidthEXTRA_CONDENSED;
	    *pStart= s- name;
	    *pLength= 15;
	    return 1;
	    }
	
	if  ( ! strncmp( s, "Condensed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	if  ( ! strncmp( s, "Compressed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	if  ( ! strncmp( s, "Compact", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	
	if  ( ! strncmp( s, "Extended", 8 )	&&
	      ( s[8] == '\0' || s[8] == ' ' )	)
	    {
	    *pWidth= FONTwidthEXPANDED;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }

	while( s[0] && s[0] != ' ' )
	    { s++;	}
	while( s[0] == ' ' )
	    { s++;	}
	}

    return 0;
    }

