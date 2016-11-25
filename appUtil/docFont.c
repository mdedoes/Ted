/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilPropMask.h"
#   include	"docFont.h"
#   include	"utilFontEncoding.h"

/************************************************************************/
/*									*/
/*  Initialise a Font List.						*/
/*  Clean a Font List.							*/
/*  Copy a Font List.							*/
/*									*/
/************************************************************************/

static void docInitFontFamily(	DocumentFontFamily *	dff )
    {
    int		i;

    dff->dffFamilyName= (char *)0;

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{ dff->dffFontForEncoding[i]= -1;	}
    }

static void docCleanFontFamily(	DocumentFontFamily *	dff )
    {
    if  ( dff->dffFamilyName )
	{ free( dff->dffFamilyName );	}
    }

void docInitFontList(	DocumentFontList *	dfl )
    {
    utilInitializeFontEncodings();

    dfl->dflFontCount= 0;
    dfl->dflFonts= (DocumentFont *)0;
    dfl->dflFamilyCount= 0;
    dfl->dflFamilies= (DocumentFontFamily *)0;
    }

void docCleanFontList(	DocumentFontList *	dfl )
    {
    int		i;

    for ( i = 0; i < dfl->dflFontCount; i++ )
	{ docCleanDocumentFont( dfl->dflFonts+ i );	}

    if  ( dfl->dflFonts )
	{ free( dfl->dflFonts );	}

    for ( i = 0; i < dfl->dflFamilyCount; i++ )
	{ docCleanFontFamily( dfl->dflFamilies+ i );	}

    if  ( dfl->dflFamilies )
	{ free( dfl->dflFamilies );	}

    return;
    }

static int docEncodingForCharset(	int		charset )
    {
    int		encoding;

    if  ( charset == -1 )
	{ return -1;	}

    for ( encoding= 0; encoding < ENCODINGps_COUNT; encoding++ )
	{
	if  ( PS_Encodings[encoding].fcOfficeCharset == charset )
	    { return encoding;	}
	}

    return -1;
    }

static int docRememberFontName(		DocumentFontList *	dfl,
					DocumentFont *		df,
					int			encoding )
    {
    int				rval= 0;
    char *			allocated= (char *)0;
    const char *		fontFamilyName= df->dfName;
    DocumentFontFamily *	dff;

    int				idx;

    if  ( encoding < 0				&&
	  df->dfCharset != FONTcharsetDEFAULT	)
	{ encoding= docEncodingForCharset( df->dfCharset );	}

    if  ( encoding >= 0 )
	{
	const FontCharset *	fc= PS_Encodings+ encoding;
	const char *		suffix= fc->fcOfficeFontnameSuffix;

	if  ( suffix )
	    {
	    int			lenN= strlen( fontFamilyName );
	    int			lenS= strlen( suffix );

	    if  ( lenN > lenS						&&
		  ! strcmp( fontFamilyName+ lenN- lenS, suffix )	)
		{
		allocated= strdup( fontFamilyName );
		if  ( ! allocated )
		    { LXDEB(lenN,allocated); rval= -1; goto ready;	}

		allocated[lenN- lenS]= '\0';
		fontFamilyName= allocated;
		}
	    }
	}

    dff= dfl->dflFamilies;
    for ( idx= 0; idx < dfl->dflFamilyCount; dff++, idx++ )
	{
	if  ( ! strcmp( dff->dffFamilyName, fontFamilyName ) )
	    { break; }
	}

    if  ( idx >= dfl->dflFamilyCount )
	{
	dff= realloc( dfl->dflFamilies,
		    ( dfl->dflFamilyCount+ 1 )* sizeof(DocumentFontFamily) );
	if  ( ! dff )
	    { LXDEB(dfl->dflFamilyCount,dff); rval= -1; goto ready;	}

	dfl->dflFamilies= dff;

	dff += dfl->dflFamilyCount;

	docInitFontFamily( dff );

	if  ( ! allocated )
	    {
	    allocated= strdup( fontFamilyName );
	    if  ( ! allocated )
		{ XDEB(allocated); rval= -1; goto ready;	}
	    }

	dff->dffFamilyName= allocated; allocated= (char *)0; /* steal */

	dfl->dflFamilyCount++;
	}

    if  ( encoding >= 0 )
	{ dff->dffFontForEncoding[encoding]= df->dfDocFontNumber; }

    df->dfDocFamilyIndex= idx;
    df->dfEncodingSet= encoding;

  ready:

    if  ( allocated )
	{ free( allocated );	}

    return rval;
    }

int docCopyFontList(	DocumentFontList *		to,
			const DocumentFontList *	from )
    {
    int			i;
    DocumentFontList	copy;

    DocumentFont *		dfTo;
    const DocumentFont *	dfFrom;

    docInitFontList( &copy );

    copy.dflFonts= malloc( from->dflFontCount* sizeof(DocumentFont) );
    if  ( ! copy.dflFonts )
	{ XDEB(copy.dflFonts); docCleanFontList( &copy ); return -1;	}

    for ( i = 0; i < from->dflFontCount; i++ )
	{ docInitDocumentFont( copy.dflFonts+ i );	}
    copy.dflFontCount= from->dflFontCount;

    dfTo= copy.dflFonts;
    dfFrom= from->dflFonts;
    for ( i = 0; i < from->dflFontCount; dfTo++, dfFrom++, i++ )
	{
	if  ( docCopyDocumentFont( dfTo, dfFrom ) )
	    { LDEB(i); docCleanFontList( &copy ); return -1;	}

	if  ( ! dfTo->dfName )
	    { continue;	}

	if  ( docRememberFontName( &copy, dfTo, dfFrom->dfEncodingSet ) )
	    { LDEB(1); docCleanFontList( &copy ); return -1;	}
	}

    docCleanFontList( to );

    *to= copy; return 0;
    }

/************************************************************************/
/*									*/
/*  Initialise a font.							*/
/*  Clean a font.							*/
/*  Copy a font.							*/
/*									*/
/************************************************************************/

void docInitDocumentFont(	DocumentFont *	df )
    {
    int		i;

    df->dfFamilyStyle= (char *)0;
    df->dfName= (char *)0;
    df->dfAltName= (char *)0;
    df->dfDocFontNumber= -1;
    df->dfPsFamilyNumber= -1;
    df->dfDocFamilyIndex= -1;
    df->dfEncodingSet= -1;
    df->dfCharset= FONTcharsetDEFAULT;
    df->dfCodepage= -1;
    df->dfPitch= FONTpitchDEFAULT;
    df->dfUsed= 0;

    df->dfPanose[0]= '\0';

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ df->dfPsFaceNumber[i]= -1;	}
    }

void docCleanDocumentFont(	DocumentFont *	df	)
    {
    if  ( df->dfFamilyStyle )
	{ free( df->dfFamilyStyle );	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}
    }

static int docFontSetFamilyStyleString(	DocumentFont *	df,
					const char *	style )
    {
    char *	 saved= (char *)0;

    if  ( style )
	{
	saved= strdup( style );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfFamilyStyle )
	{ free( df->dfFamilyStyle );	}

    df->dfFamilyStyle= saved;

    return 0;
    }

int docFontSetFamilyName(		DocumentFont *	df,
					const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    df->dfName= saved;

    return 0;
    }

int docFontSetAltName(		DocumentFont *	df,
				const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}

    df->dfAltName= saved;

    return 0;
    }

int docFontSetFamilyStyle(	DocumentFont *	df,
				int		style )
    {
    const char *	s= (const char *)0;

    switch( style )
	{
	case DFstyleFNIL:	s= "fnil";	break;
	case DFstyleFROMAN:	s= "froman";	break;
	case DFstyleFSWISS:	s= "fswiss";	break;
	case DFstyleFMODERN:	s= "fmodern";	break;
	case DFstyleFSCRIPT:	s= "fscript";	break;
	case DFstyleFDECOR:	s= "fdecor";	break;
	case DFstyleFTECH:	s= "ftech";	break;

	default:
	    LDEB(style); return -1;
	}

    if  ( docFontSetFamilyStyleString( df, s ) )
	{ LSDEB(style,s); return -1;	}

    return 0;
    }

int docCopyDocumentFont(	DocumentFont *		to,
				const DocumentFont *	from )
    {
    int			rval= 0;
    DocumentFont	copy;

    copy= *from;

    copy.dfFamilyStyle= (char *)0;
    copy.dfName= (char *)0;
    copy.dfAltName= (char *)0;

    if  ( docFontSetFamilyStyleString( &copy, from->dfFamilyStyle ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetFamilyName( &copy, from->dfName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetAltName( &copy, from->dfAltName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docCleanDocumentFont( to );

    *to= copy; docInitDocumentFont( &copy ); /* steal */

  ready:

    docCleanDocumentFont( &copy );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a font.							*/
/*									*/
/************************************************************************/

DocumentFont * docInsertFont(	DocumentFontList *	dfl,
				int			n,
				const DocumentFont *	dfSet )
    {
    int			i;
    DocumentFont *	df;

    DocumentFont	dfNew;

    utilInitializeFontEncodings();

    docInitDocumentFont( &dfNew );

    if  ( n > dfl->dflFontCount )
	{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( n + 1 ) * sizeof( DocumentFont ) );
	}
    else{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( dfl->dflFontCount + 1 ) * sizeof( DocumentFont ) );
	}
    if  ( ! df )
	{ LLDEB(dfl->dflFontCount,df); goto failed;	}
    dfl->dflFonts= df;

    if  ( docCopyDocumentFont( &dfNew, dfSet ) )
	{ LDEB(1); goto failed;	}

    if  ( n == -1 )
	{
	df= dfl->dflFonts;
	for ( n= 0; n < dfl->dflFontCount; df++, n++ )
	    {
	    if  ( df->dfDocFontNumber < 0 )
		{ break;	}
	    }
	}
    else{
	if  ( n < dfl->dflFontCount && dfl->dflFonts[n].dfDocFontNumber >= 0 )
	    { LLDEB(n,dfl->dflFonts[n].dfDocFontNumber);	}

	/* No!!
	df= dfl->dflFonts;
	for ( i= dfl->dflFontCount; i > n; i-- )
	    { df[i]= df[i-1];	}
	*/
	}

    df= dfl->dflFonts+ dfl->dflFontCount;
    for ( i= dfl->dflFontCount; i <= n; df++, i++ )
	{ docInitDocumentFont( df );	}

    dfNew.dfDocFontNumber= n;

    if  ( docRememberFontName( dfl, &dfNew, -1 ) )
	{ SDEB(dfNew.dfName); goto failed;	}

    df= dfl->dflFonts+ n;
    docCleanDocumentFont( df );

    *df= dfNew;

    if  ( n >= dfl->dflFontCount )
	{ dfl->dflFontCount= n+ 1;	}

    return df;

  failed:

    docCleanDocumentFont( &dfNew );

    return (DocumentFont *)0;
    }

/************************************************************************/
/*									*/
/*  Find or insert a font in a document font list.			*/
/*									*/
/*  In office documents, the same font in PostScript terms can occur	*/
/*  several times in the font list with a different character set and	*/
/*  with a suffix to the family name to differentiate between the	*/
/*  occurences.								*/
/*									*/
/*  NOTE that the name of a font is the key field in this		*/
/*	administration. Character set and family style are dependent	*/
/*	fields.								*/
/*									*/
/************************************************************************/

int docGetFontByName(	DocumentFontList *	dflTo,
			const char *		fontFamilyName,
			int			encoding )
    {
    int			i;
    DocumentFont *	dfTo;

    char *		allocated= (char *)0;
    int			charset= FONTcharsetDEFAULT;
    int			encodingWasDefault= ( encoding < 0 );

    utilInitializeFontEncodings();

    if  ( encoding >= 0 )
	{
	const FontCharset *	fc= PS_Encodings+ encoding;
	const char *		suffix= fc->fcOfficeFontnameSuffix;

	if  ( suffix )
	    {
	    int			lenN= strlen( fontFamilyName );
	    int			lenS= strlen( suffix );

	    if  ( lenN <= lenS					||
		  strcmp( fontFamilyName+ lenN- lenS, suffix )	)
		{
		allocated= malloc( lenN+ lenS+ 1 );
		if  ( ! allocated )
		    { LLXDEB(lenN,lenS,allocated); return -1;	}

		strcpy( allocated,       fontFamilyName );
		strcpy( allocated+ lenN, suffix );

		fontFamilyName= allocated;
		}
	    }

	charset= fc->fcOfficeCharset;
	}
    else{
	int			enc;
	int			lenN= strlen( fontFamilyName );
	const FontCharset *	fc= PS_Encodings;

	for ( enc= 0; enc < ENCODINGps_COUNT; fc++, enc++ )
	    {
	    const char *	suffix= fc->fcOfficeFontnameSuffix;
	    int			lenS= suffix?strlen( suffix ):0;

	    if  ( ! suffix )
		{ suffix= "";	}

	    if  ( lenN > lenS						&&
		  ! strcmp( fontFamilyName+ lenN- lenS, suffix )	)
		{
		encoding= enc;
		charset= fc->fcOfficeCharset;
		break;
		}
	    }
	}

    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFontNumber < 0	||
	      ! dfTo->dfName		)
	    { continue;	}


	if  ( ! strcmp( dfTo->dfName, fontFamilyName )	&&
	      dfTo->dfCharset == charset		)
	    { break;	}
	}

    if  ( i >= dflTo->dflFontCount && encodingWasDefault )
	{
	dfTo= dflTo->dflFonts;
	for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	    {
	    if  ( dfTo->dfDocFontNumber < 0	||
		  ! dfTo->dfName		)
		{ continue;	}

	    if  ( ! strcmp( dfTo->dfName, fontFamilyName ) )
		{ break;	}
	    }
	}

    if  ( i >= dflTo->dflFontCount )
	{
	int			fontFamilyStyle;
	DocumentFont		dfNew;

	fontFamilyStyle= utilFontFamilyStyle( fontFamilyName );

	docInitDocumentFont( &dfNew );
	dfNew.dfCharset= charset;
	dfNew.dfEncodingSet= encoding;

	if  ( docFontSetFamilyStyle( &dfNew, fontFamilyStyle ) )
	    { LDEB(1); return -1;	}
	if  ( docFontSetFamilyName( &dfNew, fontFamilyName ) )
	    { LDEB(1); return -1;	}

	dfTo= docInsertFont( dflTo, -1, &dfNew );
	if  ( ! dfTo )
	    { LDEB(dflTo->dflFontCount); return -1;	}

	docCleanDocumentFont( &dfNew );
	}

    if  ( allocated )
	{ free( allocated );	}

    return dfTo->dfDocFontNumber;
    }

int docMergeFontIntoFontlist(	DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom )
    {
    int			i;
    DocumentFont *	dfTo;

    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFontNumber < 0 )
	    { continue;	} 
	if  ( ! dfTo->dfFamilyStyle )
	    { XDEB(dfTo->dfFamilyStyle); continue;	}

	if  ( ! strcmp( dfTo->dfName, dfFrom->dfName )	&&
	      dfTo->dfCharset == dfFrom->dfCharset	)
	    { break;	}
	}

    if  ( i < dflTo->dflFontCount )
	{ return i;	}

    dfTo= docInsertFont( dflTo, -1, dfFrom );
    if  ( ! dfTo )
	{ XDEB(dfTo); return -1;	}

    return dfTo->dfDocFontNumber;
    }

