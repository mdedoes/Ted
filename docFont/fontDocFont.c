/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"fontDocFont.h"
#   include	<psDocumentFontStyle.h>

/************************************************************************/
/*									*/
/*  Initialise a font.							*/
/*  Clean a font.							*/
/*  Copy a font.							*/
/*									*/
/************************************************************************/

void fontInitDocumentFont(	DocumentFont *	df )
    {
    int		i;

    utilInitMemoryBuffer( &(df->dfName) );
    utilInitMemoryBuffer( &(df->dfAltName) );
    df->dfDocFontNumber= -1;
    df->dfStyleInt= DFstyleFNIL;
    df->dfPitch= FONTpitchDEFAULT;
    df->dfPanose[0]= '\0';

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ df->dfPsFontInfo[i]= (struct AfmFontInfo *)0;	}

    utilInitIndexSet( &(df->dfUnicodesUsed) );
    utilInitIndexMapping( &(df->dfUnicodeToCharset) );

    df->dfCharsetIdxUsed[0]= -1;
    df->dfCharsetIdxUsedCount= 0;

    df->dfFontSpecificEncoding= 0;
    df->dfEmbeddedType= FONTtypeNONE;

    utilInitMemoryBuffer( &(df->dfFontBytes) );
    utilInitMemoryBuffer( &(df->dfFontFile) );

    return;
    }

void fontCleanDocumentFont(	DocumentFont *	df	)
    {
    utilCleanMemoryBuffer( &(df->dfName) );
    utilCleanMemoryBuffer( &(df->dfAltName) );

    utilCleanIndexSet( &(df->dfUnicodesUsed) );
    utilCleanIndexMapping( &(df->dfUnicodeToCharset) );

    utilCleanMemoryBuffer( &(df->dfFontBytes) );
    utilCleanMemoryBuffer( &(df->dfFontFile) );
    }

int fontSetFamilyName(		DocumentFont *	df,
				const char *	name )
    {
    if  ( name )
	{
	if  ( utilMemoryBufferSetString( &(df->dfName), name ) )
	    { SDEB(name); return -1;	}
	}
    else{ utilEmptyMemoryBuffer( &(df->dfName) );	}

    return 0;
    }

int fontSetAltName(		DocumentFont *	df,
				const char *	name )
    {
    if  ( name )
	{
	if  ( utilMemoryBufferSetString( &(df->dfAltName), name ) )
	    { SDEB(name); return -1;	}
	}
    else{ utilEmptyMemoryBuffer( &(df->dfAltName) );	}

    return 0;
    }

int fontSetFamilyStyle(	DocumentFont *	df,
				int		styleInt )
    {
    df->dfStyleInt= styleInt;
    return 0;
    }

int fontCopyDocumentFont(	DocumentFont *		to,
				const DocumentFont *	from )
    {
    int			rval= 0;
    DocumentFont	copy;
    int			i;

    fontInitDocumentFont( &copy );

    copy.dfDocFontNumber= from->dfDocFontNumber;
    copy.dfStyleInt= from->dfStyleInt;
    copy.dfPitch= from->dfPitch;
    memcpy( &copy.dfPanose, from->dfPanose, FONTlenPANOSE+ 1 );

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ copy.dfPsFontInfo[i]= from->dfPsFontInfo[i];	}

    for ( i= 0; i < from->dfCharsetIdxUsedCount; i++ )
	{ copy.dfCharsetIdxUsed[i]= from->dfCharsetIdxUsed[i];	}
    copy.dfCharsetIdxUsedCount= from->dfCharsetIdxUsedCount;

    if  ( utilCopyMemoryBuffer( &(copy.dfName), &(from->dfName) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilCopyMemoryBuffer( &(copy.dfAltName), &(from->dfAltName) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyIndexSet( &(copy.dfUnicodesUsed),
					    &(from->dfUnicodesUsed) ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyMemoryBuffer( &(copy.dfFontBytes), &(from->dfFontBytes) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilCopyMemoryBuffer( &(copy.dfFontFile), &(from->dfFontFile) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fontCleanDocumentFont( to );

    *to= copy; fontInitDocumentFont( &copy ); /* steal */

  ready:

    fontCleanDocumentFont( &copy );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Copy information on the match with the PostScript font from one	*/
/*  font to another. Usually this can be done safely as the PS list is	*/
/*  an application-wide constant.					*/
/*									*/
/************************************************************************/

void fontCopyFaceMatches(	DocumentFont *		dfTo,
				const DocumentFont *	dfFrom )
    {
    int		fac;

    for ( fac= 0; fac < FONTface_COUNT; fac++ )
	{ 
	if  ( ! dfTo->dfPsFontInfo[fac] )
	    { dfTo->dfPsFontInfo[fac]= dfFrom->dfPsFontInfo[fac]; }
	}
    }

/************************************************************************/
/*									*/
/*  Remove the character set suffix from a document font name.		*/
/*									*/
/*  In office documents, the same font in PostScript terms can occur	*/
/*  several times in the font list with a different character set and	*/
/*  with a suffix to the family name to differentiate between the	*/
/*  occurences.								*/
/*									*/
/*  1)  Find an office character set with the character set given. In	*/
/*	many cases, this does not really make sense: The font is given	*/
/*	with the 'default' character set.				*/
/*  2)  If an office character set was given, derive the family name	*/
/*	from the suffix of the character set. [I.E: remove it from the	*/
/*	name.]								*/
/*									*/
/************************************************************************/

int fontRemoveCharsetFromFontName(	DocumentFont *	df,
					int		charset )
    {
    int				charsetIdx= -1;
    const OfficeCharset *	oc= (const OfficeCharset *)0;
    const char *		suffix= (const char *)0;

    const char *		fname= utilMemoryBufferGetString( &(df->dfName) );

    /*  1  */
    oc= textGetOfficeCharsetByCharset( &charsetIdx, charset );
    if  ( ! oc )
	{
	oc= textGetOfficeCharsetBySuffix( &charsetIdx, fname );
	if  ( ! oc )
	    { return FONTcharsetDEFAULT;	}

	charset= oc->ocOfficeCharset;
	}

    /*  2  */
    suffix= oc->ocOfficeFontnameSuffix;
    if  ( suffix )
	{
	int			lenN= strlen( fname );
	int			lenS= strlen( suffix );

	if  ( lenN > lenS				&&
	      ! strcmp( fname+ lenN- lenS, suffix )	)
	    { utilMemoryBufferSetSize( &(df->dfName), lenN- lenS );	}
	}

    return charset;
    }

