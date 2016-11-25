/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  struct BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<ctype.h>

#   include	<psDocumentFontStyle.h>
#   include	"docRtfWriterImpl.h"
#   include	<fontEncodedFont.h>
#   include	"docRtfFlags.h"
#   include	<fontDocFont.h>
#   include	<docBuf.h>
#   include	<fontDocFontList.h>
#   include	<docDocumentProperties.h>

#   include	<appDebugon.h>

/************************************************************************/

static const char * docRtfFontFamilyStyleTag(	int styleInt )
    {
    switch( styleInt )
	{
	case DFstyleFNIL:	return "fnil";
	case DFstyleFROMAN:	return "froman";
	case DFstyleFSWISS:	return "fswiss";
	case DFstyleFMODERN:	return "fmodern";
	case DFstyleFSCRIPT:	return "fscript";
	case DFstyleFDECOR:	return "fdecor";
	case DFstyleFTECH:	return "ftech";
	case DFstyleFBIDI:	return "fbidi";

	default:
	    LDEB(styleInt); return "fnil";
	}
    }

/************************************************************************/
/*									*/
/*  Write a font table.							*/
/*									*/
/************************************************************************/

static void docRtfWriteEncodedFont(	RtfWriter *		rw,
					const DocumentFont *	df,
					const char *		suffix,
					int			fontnum,
					int			charset )
    {
    docRtfWriteArgDestinationBegin( rw, "f", fontnum );

    docRtfWriteTag( rw, docRtfFontFamilyStyleTag( df->dfStyleInt ) );

    docRtfWriteArgTag( rw, "fcharset", charset );

    if  ( df->dfPitch != FONTpitchDEFAULT )
	{ docRtfWriteArgTag( rw, "fprq", df->dfPitch ); }

    if  ( df->dfPanose[0] )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedStringDestination( rw, "*\\panose",
		    df->dfPanose, strlen( df->dfPanose ), addSemicolon );
	}

    if  ( ! utilMemoryBufferIsEmpty( &(df->dfName) ) )
	{
	const char *	name= utilMemoryBufferGetString( &(df->dfName) );

	if  ( ! strcmp( name, "ITC Zapf Dingbats" ) )
	    { name= "ZapfDingbats";	}
	if  ( ! strcmp( name, "ITC Zapf Chancery" ) )
	    { name= "ZapfChancery";	}

	docRtfWriteDocEncodedString( rw, name, strlen( name ) );

	if  ( suffix )
	    { docRtfWriteDocEncodedString( rw, suffix, strlen( suffix ) ); }
	}

    if  ( ! utilMemoryBufferIsEmpty( &(df->dfAltName) ) )
	{
	const int	addSemicolon= 0;

	docRtfWriteDocEncodedBufferDestination( rw, "*\\falt",
					    &(df->dfAltName), addSemicolon );
	}

    docRtfWriteSemicolon( rw );

    docRtfWriteDestinationEnd( rw );

    return;
    }

int docRtfWriteGetCharset(	RtfWriter *		rw,
				int *			pCharset,
				const DocumentFont *	df,
				int			symbol )
    {
    return fontGetSymbolCharset( pCharset,
				    &(rw->rwEncodedFontList), df, symbol );
    }

int docRtfWriteGetDefaultFont(	RtfWriter *			rw,
				int				deff )
    {
    const struct BufferDocument *	bd= rw->rwDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentFontList *	dfl= dp->dpFontList;

    const DocumentFont *	df;

    if  ( rw->rwSaveFlags & RTFflagUNENCODED )
	{ return deff;	}

    if  ( deff < 0 || deff >= dfl->dflFontCount )
	{ return -1;	}

    df= fontFontListGetFontByNumber( dfl, deff );
    if  ( ! df						||
	  utilMemoryBufferIsEmpty( &(df->dfName) )	||
	  df->dfCharsetIdxUsedCount == 0		)
	{ return -1;	}

    return fontGetDefaultFileFontNumber( &(rw->rwEncodedFontList), df );
    }

void docRtfWriteFontTable(	RtfWriter *			rw )
    {
    const struct BufferDocument *	bd= rw->rwDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentFontList *	dfl= dp->dpFontList;

    int				fnt;

    docRtfWriteDestinationBegin( rw, "fonttbl" );
    docRtfWriteNextLine( rw );

    for ( fnt= 0; fnt < dfl->dflFontCount; fnt++ )
	{
	const DocumentFont *	df= fontFontListGetFontByNumber( dfl, fnt );
	int			cs;

	if  ( ! df || utilMemoryBufferIsEmpty( &(df->dfName) ) )
	    { continue;	}

	for ( cs= 0; cs < df->dfCharsetIdxUsedCount; cs++ )
	    {
	    const char *		suffix;
	    int				fileFontNumber;
	    int				officeCharset;

	    if  ( fontGetFileFont( &suffix, &fileFontNumber, &officeCharset,
		    df, &(rw->rwEncodedFontList), df->dfCharsetIdxUsed[cs] ) )
		{ LDEB(cs); continue;	}

	    docRtfWriteEncodedFont( rw, df,
				    suffix, fileFontNumber, officeCharset );

	    if  ( fnt+ 1 < dfl->dflFontCount		||
		  cs+ 1 < df->dfCharsetIdxUsedCount	)
		{ docRtfWriteNextLine( rw );	}
	    }
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return;
    }

/************************************************************************/

int docRtfWriteBuildFontAdmin(	RtfWriter *		rw )
    {
    struct BufferDocument *		bd= rw->rwDocument;
    DocumentProperties *	dp= bd->bdProperties;
    DocumentFontList *		dfl= dp->dpFontList;

    fontListClearCharsUsed( dfl );

    if  ( docGetCharsUsed( bd ) )
	{ LDEB(1); return -1;	}

    return fontBuildEncodedFontAdmin( dfl, &(rw->rwEncodedFontList) );
    }
