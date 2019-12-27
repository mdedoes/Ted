/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  struct BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	<fontEncodedFont.h>
#   include	"docRtfFlags.h"
#   include	<fontDocFont.h>
#   include	<docBuf.h>
#   include	<fontDocFontList.h>
#   include	<fontDocFontListImpl.h>
#   include	<docDocumentProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write a font table.							*/
/*									*/
/************************************************************************/

static int docRtfWriteEncodedFont(	RtfWriter *		rw,
					const DocumentFont *	df,
					const char *		suffix,
					int			fontnum,
					int			charset )
    {
    const int	scope= RTCscopeFONT;

    docRtfWriteArgDestinationBegin( rw, "f", fontnum );

    if  ( docRtfWriteProperty( rw, scope,
				DFpropFAMILY_STYLE, df->dfStyleInt ) )
	{ LLLDEB(scope,DFpropFAMILY_STYLE,df->dfStyleInt); return -1; }

    if  ( docRtfWriteProperty( rw, scope, DFpropCHARSET, charset ) )
	{ LLLDEB(scope,DFpropCHARSET,charset); return -1; }

    if  ( df->dfPitch != FONTpitchDEFAULT )
	{
	if  ( docRtfWriteProperty( rw, scope, DFpropPITCH, df->dfPitch ) )
	    { LLLDEB(scope,DFpropPITCH,df->dfPitch); return -1; }
	}

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

    return 0;
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
