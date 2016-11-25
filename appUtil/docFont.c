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

void docInitFontList(	DocumentFontList *	dfl )
    {
    utilInitializeFontEncodings();

    dfl->dflCount= 0;
    dfl->dflFonts= (DocumentFont *)0;
    }

void docCleanFontList(	DocumentFontList *	dfl )
    {
    int		i;

    for ( i = 0; i < dfl->dflCount; i++ )
	{ docCleanDocumentFont( dfl->dflFonts+ i );	}

    if  ( dfl->dflFonts )
	{ free( dfl->dflFonts );	}
    }

int docCopyFontList(	DocumentFontList *		to,
			const DocumentFontList *	from )
    {
    int			i;
    DocumentFontList	copy;

    docInitFontList( &copy );

    copy.dflFonts= malloc( from->dflCount* sizeof(DocumentFont) );
    if  ( ! copy.dflFonts )
	{ XDEB(copy.dflFonts); docCleanFontList( &copy ); return -1;	}

    for ( i = 0; i < from->dflCount; i++ )
	{ docInitDocumentFont( copy.dflFonts+ i );	}
    copy.dflCount= from->dflCount;

    for ( i = 0; i < from->dflCount; i++ )
	{
	if  ( docCopyDocumentFont( copy.dflFonts+ i, from->dflFonts+ i ) )
	    { LDEB(i); docCleanFontList( &copy ); return -1;	}
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

void docInitDocumentFont(	DocumentFont *	df	)
    {
    df->dfFamilyStyle= (char *)0;
    df->dfName= (char *)0;
    df->dfAltName= (char *)0;
    df->dfDocFamilyNumber= -1;
    df->dfPsFamilyNumber= -1;
    df->dfInstanceCount= 0;
    df->dfInstances= (DocumentFontInstance *)0;
    df->dfCharset= FONTcharsetDEFAULT;
    df->dfCodepage= -1;
    df->dfEncodingUsed= -1;
    df->dfPitch= FONTpitchDEFAULT;

    df->dfPanose[0]= '\0';
    }

void docCleanDocumentFont(	DocumentFont *	df	)
    {
    if  ( df->dfFamilyStyle )
	{ free( df->dfFamilyStyle );	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}

    if  ( df->dfInstances )
	{ free( df->dfInstances );	}
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
    copy.dfInstances= (DocumentFontInstance *)0;

    if  ( docFontSetFamilyStyleString( &copy, from->dfFamilyStyle ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetFamilyName( &copy, from->dfName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetAltName( &copy, from->dfAltName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( from->dfInstanceCount > 0 )
	{
	int	i;

	copy.dfInstances= (DocumentFontInstance *)
	    malloc( from->dfInstanceCount* sizeof( DocumentFontInstance ) );

	if  ( ! copy.dfInstances )
	    { LDEB(1); rval= -1; goto ready;	}

	for ( i= 0; i < from->dfInstanceCount; i++ )
	    { copy.dfInstances[i]= from->dfInstances[i];	}
	}

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

    if  ( n > dfl->dflCount )
	{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( n + 1 ) * sizeof( DocumentFont ) );
	}
    else{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( dfl->dflCount + 1 ) * sizeof( DocumentFont ) );
	}
    if  ( ! df )
	{ LLDEB(dfl->dflCount,df); goto failed;	}
    dfl->dflFonts= df;

    if  ( docCopyDocumentFont( &dfNew, dfSet ) )
	{ LDEB(1); goto failed;	}

    if  ( n == -1 )
	{
	df= dfl->dflFonts;
	for ( n= 0; n < dfl->dflCount; df++, n++ )
	    {
	    if  ( df->dfDocFamilyNumber < 0 )
		{ break;	}
	    }
	}
    else{
	if  ( n < dfl->dflCount && dfl->dflFonts[n].dfDocFamilyNumber >= 0 )
	    { LLDEB(n,dfl->dflFonts[n].dfDocFamilyNumber);	}

	/* No!!
	df= dfl->dflFonts;
	for ( i= dfl->dflCount; i > n; i-- )
	    { df[i]= df[i-1];	}
	*/
	}

    df= dfl->dflFonts+ dfl->dflCount;
    for ( i= dfl->dflCount; i <= n; df++, i++ )
	{ docInitDocumentFont( df );	}

    dfNew.dfDocFamilyNumber= n;

    df= dfl->dflFonts+ n;
    docCleanDocumentFont( df );

    *df= dfNew;

    if  ( n >= dfl->dflCount )
	{ dfl->dflCount= n+ 1;	}

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

    utilInitializeFontEncodings();

    if  ( encoding >= 0 )
	{
	const FontCharset *	fc= PS_Encodings+ encoding;

	if  ( PS_Encodings[encoding].fcOfficeFontnameSuffix )
	    {
	    allocated= malloc( strlen( fontFamilyName )+ 
				strlen( fc->fcOfficeFontnameSuffix )+ 1 );
	    
	    if  ( ! allocated )
		{ XDEB(allocated); return -1;	}

	    strcpy( allocated, fontFamilyName );
	    strcat( allocated, fc->fcOfficeFontnameSuffix );

	    fontFamilyName= allocated;
	    }

	charset= fc->fcOfficeCharset;
	}

    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFamilyNumber < 0	||
	      ! dfTo->dfName			)
	    { continue;	}


	if  ( ! strcmp( dfTo->dfName, fontFamilyName )	&&
	      dfTo->dfCharset == charset		)
	    { break;	}
	}

    if  ( i >= dflTo->dflCount )
	{
	int			fontFamilyStyle;
	DocumentFont		dfNew;

	fontFamilyStyle= utilFontFamilyStyle( fontFamilyName );

	docInitDocumentFont( &dfNew );
	dfNew.dfCharset= charset;

	if  ( docFontSetFamilyStyle( &dfNew, fontFamilyStyle ) )
	    { LDEB(1); return -1;	}
	if  ( docFontSetFamilyName( &dfNew, fontFamilyName ) )
	    { LDEB(1); return -1;	}

	dfTo= docInsertFont( dflTo, -1, &dfNew );
	if  ( ! dfTo )
	    { LDEB(dflTo->dflCount); return -1;	}

	docCleanDocumentFont( &dfNew );
	}

    if  ( allocated )
	{ free( allocated );	}

    return dfTo->dfDocFamilyNumber;
    }

int docMergeFontIntoFontlist(	DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom )
    {
    int			i;
    DocumentFont *	dfTo;

    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFamilyNumber < 0 )
	    { continue;	} 
	if  ( ! dfTo->dfFamilyStyle )
	    { XDEB(dfTo->dfFamilyStyle); continue;	}

	if  ( ! strcmp( dfTo->dfName, dfFrom->dfName )	&&
	      dfTo->dfCharset == dfFrom->dfCharset	)
	    { break;	}
	}

    if  ( i < dflTo->dflCount )
	{ return i;	}

    dfTo= docInsertFont( dflTo, -1, dfFrom );
    if  ( ! dfTo )
	{ XDEB(dfTo); return -1;	}

    return dfTo->dfDocFamilyNumber;
    }

