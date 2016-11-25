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
#   include	"docFont.h"
#   include	"psFont.h"

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

	if  ( l == 5 && ! strncmp( fontFamilyName, "Wingdings", 9 ) )
	    { return DFstyleFTECH;	}

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
				const AppFontTypeface *	aft2 )
    {
    double	d;

    double	distance= 0.0;
    int		difCount= 0;

    if  ( aft1->aftWidth != aft2->aftWidth )
	{
	d= aft2->aftWidth- aft1->aftWidth;
	d /= ( FONTwidth_MAX- FONTwidth_MIN );

	distance += d* d;
	difCount++;
	}

    if  ( aft1->aftWeight != aft2->aftWeight )
	{
	d= aft2->aftWeight- aft1->aftWeight;
	d /= ( FONTweight_MAX- FONTweight_MIN );

	distance += d* d;
	difCount++;
	}

    if  ( aft1->aftIsSlanted != aft2->aftIsSlanted )
	{
	d= aft2->aftIsSlanted- aft1->aftIsSlanted;
	/* d /= 1.0; */

	distance += d* d;
	difCount++;
	}

    distance /= 3.0;
    distance= sqrt( distance );

    *pDifCount= difCount;
    *pDistance= distance;

    return;
    }

static void utilFontCompareProps(	int *		pDifCount1,
					int *		pDifCount2,
					int *		pCmp,
					int		weight,
					int		vnormal,
					int		v1,
					int		v2 )
    {
    if  ( v1 != vnormal )
	{ (*pDifCount1)++;	}
    if  ( v2 != vnormal )
	{ (*pDifCount2)++;	}

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

    int				difs1= 0;
    int				difs2= 0;
    int				cmp= 0;

    int				weight_range;

    weight_range= ( FONTweight_MAX- FONTweight_MIN );

    utilFontCompareProps( &difs1, &difs2, &cmp, 
			4* ( weight_range+ 1 ), FONTwidthMEDIUM,
			aft1->aftWidth, aft2->aftWidth );

    utilFontCompareProps( &difs1, &difs2, &cmp, 
			2, FONTweightMEDIUM,
			aft1->aftWeight, aft2->aftWeight );

    utilFontCompareProps( &difs1, &difs2, &cmp, 
			1, 0,
			aft1->aftIsSlanted, aft2->aftIsSlanted );

    if  ( difs1 > difs2 )
	{ return  1;	}
    if  ( difs1 < difs2 )
	{ return -1;	}

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

static int utilGetPsFamilyByText(	int *			pFamily,
					const char *		name,
					const AppFontFamily *	psf,
					int			psfCount )
    {
    int			fam;

    if  ( ! name )
	{ return -1;	}

    for ( fam= 0; fam < psfCount; psf++, fam++ )
	{
	if  ( psf->affFontFamilyText				&&
	      ! strcmp( name, psf->affFontFamilyText )	)
	    { *pFamily= fam; return 0;	}
	}

    return -1;
    }

static int utilGetPsFamilyByName(	int *			pFamily,
					const char *		name,
					const AppFontFamily *	psfList,
					int			psfCount )
    {
    int				fam;
    const AppFontFamily *	psf;

    if  ( ! name )
	{ return -1;	}

    psf= psfList;
    for ( fam= 0; fam < psfCount; psf++, fam++ )
	{
	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    if  ( ! strncmp( name, "Times New Roman", 15 ) )
	{ name= "Times";	}
    if  ( ! strncmp( name, "Courier New", 11 ) )
	{ name= "Courier";	}

    if  ( ! strncmp( name, "Wingdings", 9 ) )
	{ name= "ITC Zapf Dingbats";	}

    psf= psfList;
    for ( fam= 0; fam < psfCount; psf++, fam++ )
	{
	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    if  ( ! strcmp( name, "Times New Roman" ) )
	{ name= "Times";	}
    if  ( ! strcmp( name, "Courier New" ) )
	{ name= "Courier";	}
    if  ( ! strcmp( name, "Apple Chancery" ) )
	{ name= "ITC Zapf Chancery";	}

    psf= psfList;
    for ( fam= 0; fam < psfCount; psf++, fam++ )
	{
	if  ( ! strcmp( name, psf->affFontFamilyName ) )
	    { *pFamily= fam; return 0;	}
	}

    return -1;
    }

static int utilGetPsFamilyByStyle(	int *			pFamily,
					const DocumentFont *	df,
					const AppFontFamily *	psf,
					int			psfCount )
    {
    int				fam;
    const AppFontFamily *	aff;

    /*  5  */
    if  ( ! df->dfFamilyStyle			||
	  ! strcmp( df->dfFamilyStyle, "fnil" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "Helvetica", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  6  */
    if  ( ! strcmp( df->dfFamilyStyle, "fswiss" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "Helvetica", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  7  */
    if  ( ! strcmp( df->dfFamilyStyle, "froman" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "Times", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  8  */
    if  ( ! strcmp( df->dfFamilyStyle, "fmodern" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "Courier", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  9  */
    if  ( ! strcmp( df->dfFamilyStyle, "fscript" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "ITC Zapf Chancery", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    /*  10  */
    if  ( ! strcmp( df->dfFamilyStyle, "fdecor" ) )
	{
	aff= psf;
	for ( fam= 0; fam < psfCount; aff++, fam++ )
	    {
	    if  ( ! strcmp( "ITC Zapf Dingbats", aff->affFontFamilyName ) )
		{ *pFamily= fam; return 0;	}
	    }
	}

    return -1;
    }

int utilFontGetPsFont(	int *			pFamily,
			int *			pFace,
			AppFontFamily **	pAff,
			AppFontTypeface **	pAft,
			int *			pFontEncoding,
			const char *		afmDirectory,
			const DocumentFont *	df,
			const TextAttribute *	ta )
    {
    int			fam;
    int			face;
    int			turn;

    AppFontFamily *	aff;
    int			affCount;

    AppFontFamily *	psf;
    AppFontTypeface *	pst;

    AppFontTypeface	aft;

    int			encoding;

    /*  3  */
    if  ( psFontCatalog( afmDirectory, &aff, &affCount ) )
	{ SDEB(afmDirectory); return -1;	}

    /*  4  */
    if  ( utilGetPsFamilyByText(  &fam, df->dfName, aff, affCount )	&&
	  utilGetPsFamilyByName(  &fam, df->dfName, aff, affCount )	&&
	  utilGetPsFamilyByStyle( &fam, df, aff, affCount )		)
	{
	SSDEB(df->dfName,df->dfFamilyStyle);

	for ( fam= 0; fam < affCount; fam++ )
	    { LSDEB(fam,aff[fam].affFontFamilyName);	}

	return -1;
	}

    docInitFontTypeface( &aft );
    
    if  ( ta->taFontIsBold )
	{
	aft.aftIsBold= 1;
	aft.aftWeight= FONTweightBOLD;
	}

    if  ( ta->taFontIsSlanted )
	{ aft.aftIsSlanted= 1;	}

    psf= aff+ fam;

    switch( df->dfCharset )
	{
	case FONTcharsetDEFAULT:
	    if  ( psf->affDefaultEncoding >= 0 )
		{ encoding= psf->affDefaultEncoding;	}
	    else{ encoding= ENCODINGpsISO_8859_1;	}
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
	    LDEB(df->dfCharset);
	    return -1;

	case FONTcharsetEE:
	    encoding= ENCODINGpsISO_8859_2;
	    break;

	case FONTcharsetGREEK:
	case FONTcharsetTURKISH:
	case FONTcharsetBALTIC:
	case FONTcharsetHEBREW:
	case FONTcharsetARABIC:
	case FONTcharsetSHIFTJIS:
	case FONTcharsetHANGEUL:
	case FONTcharsetGB2313:
	case FONTcharsetCHINESEBIG5:
	default:
	    LDEB(df->dfCharset);
	    if  ( psf->affDefaultEncoding >= 0 )
		{ encoding= psf->affDefaultEncoding;	}
	    else{ encoding= ENCODINGpsISO_8859_1;	}
	    break;
	}

    for ( turn= 0; turn < 2; turn++ )
	{
	int		faceFound= psf->affFaceCount;
	int		difCountFound= 999;
	double		distanceFound= 999;

	pst= psf->affFaces;
	for ( face= 0; face < psf->affFaceCount; pst++, face++ )
	    {
	    int		difCount;
	    double	distance;

	    AfmFontInfo *	afi= (AfmFontInfo *)pst->aftPrintingData;

	    if  ( ! afi->afiSupportedCharsets[encoding].scSupported )
		{ continue;	}

	    utilFontFaceDistance( &difCount, &distance, pst, &aft );

	    if  ( difCount < difCountFound		&&
		  distance < distanceFound		)
		{
		faceFound= face;
		difCountFound= difCount;
		distanceFound= distance;
		}
	    }

	if  ( turn == 0				&&
	      face >= psf->affFaceCount		&&
	      psf->affDefaultEncoding >= 0	)
	    {
	    encoding= psf->affDefaultEncoding;
	    continue;
	    }

	face= faceFound;
	pst= psf->affFaces+ face;

	if  ( face >= psf->affFaceCount )
	    { face= 0; pst= psf->affFaces;	}

	break;
	}

    if  ( face >= psf->affFaceCount )
	{ LLDEB(ta->taFontIsBold,ta->taFontIsSlanted); return -1;	}

    *pFamily= fam;
    *pFace= face;
    *pAff= psf;
    *pAft= pst;
    *pFontEncoding= encoding;

    return 0;
    }

