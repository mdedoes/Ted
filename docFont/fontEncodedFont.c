/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<textOfficeCharset.h>
#   include	<utilPagedList.h>
#   include	"fontEncodedFont.h"
#   include	"fontDocFontList.h"
#   include	"fontDocFontListImpl.h"
#   include	"fontDocFont.h"

#   include	<appDebugon.h>

/************************************************************************/

const char DOC_RTF_AnsiCharsetName[]= "CP1252";

/************************************************************************/

void fontInitEncodedFont(	EncodedFont *	ef )
    {
    ef->efFileFontNumber= -1;
    ef->ecBufFontNumber= -1;
    ef->ecCharset= FONTcharsetANSI;
    ef->ecCharsetIdx= -1;
    ef->ecCodepage= -1;
    }

/************************************************************************/
/*									*/
/*  Build the administration for writing encoded fonts.			*/
/*									*/
/************************************************************************/

int fontBuildEncodedFontAdmin(	DocumentFontList *	dfl,
				PagedList *		encodedFontList )
    {
    int				f;
    int				fileFontNumber= 0;

    for ( f= 0; f < dfl->dflFontCount; f++ )
	{
	DocumentFont *	df= fontFontListGetFontByNumber( dfl, f );

	if  ( ! df )
	    { continue;	}

	if  ( df->dfFontSpecificEncoding )
	    {
	    df->dfCharsetIdxUsedCount= 0;

	    if  ( ! textGetOfficeCharsetByCharset( &(df->dfCharsetIdxUsed[0]),
							FONTcharsetSYMBOL ) )
		{ LDEB(FONTcharsetSYMBOL);	}
	    else{ df->dfCharsetIdxUsedCount++;	}
	    }
	else{
	    if  ( fontFindLegacyEncodings( df ) )
		{
		LSDEB(f,utilMemoryBufferGetString(&(df->dfName)));
		return -1;
		}
	    }

	if  ( df->dfCharsetIdxUsedCount > 0 )
	    {
	    int		cs;

	    for ( cs= 0; cs < df->dfCharsetIdxUsedCount; cs++ )
		{
		EncodedFont		efIn;
		EncodedFont *		efOut;
		const OfficeCharset *	oc;
		int			key;

		oc= textGetOfficeCharsetByIndex( df->dfCharsetIdxUsed[cs] );
		if  ( ! oc )
		    { LXDEB(df->dfCharsetIdxUsed,oc); return -1;	}

		fontInitEncodedFont( &efIn );

		efIn.efFileFontNumber= fileFontNumber++;
		efIn.ecBufFontNumber= df->dfDocFontNumber;
		efIn.ecCharset= oc->ocOfficeCharset;
		efIn.ecCharsetIdx= df->dfCharsetIdxUsed[cs];

		key= CHARSETidx_COUNT* efIn.ecBufFontNumber+
						    df->dfCharsetIdxUsed[cs];

		efOut= (EncodedFont *)utilPagedListClaimItem(
						    encodedFontList, key );
		if  ( ! efOut )
		    { LXDEB(key,efOut); return -1;	}
		*efOut= efIn;
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get a font from the administration for writing the font table.	*/
/*									*/
/************************************************************************/

static const EncodedFont * fontGetEncodedFont(
				PagedList *		encodedFontList,
				const DocumentFont *	df,
				int			csIdx )
    {
    const EncodedFont *		ef;
    int				key;

    key= CHARSETidx_COUNT* df->dfDocFontNumber+ csIdx;
    ef= (const EncodedFont *)utilPagedListGetItemByNumber(
					encodedFontList, key );
    if  ( ! ef )
	{ SLLLXDEB(utilMemoryBufferGetString(&(df->dfName)),df->dfDocFontNumber,csIdx,key,ef);	}

    return ef;
    }

int fontGetFileFont(	const char **			pSuffix,
			int *				pFileFontNumber,
			int *				pOfficeCharset,
			const DocumentFont *		df,
			struct PagedList *		encodedFontList,
			int				csIdx )
    {
    const OfficeCharset *	oc;
    const EncodedFont *		ef;
    const char *		suffix= (const char *)0;

    ef= fontGetEncodedFont( encodedFontList, df, csIdx );
    if  ( ! ef )
	{ LXDEB(csIdx,ef); return -1;	}

    oc= textGetOfficeCharsetByIndex( csIdx );
    if  ( ! oc )
	{ LXDEB(csIdx,oc); return -1; }

    if  ( oc->ocEmitSuffix )
	{ suffix= oc->ocOfficeFontnameSuffix;	}

    *pSuffix= suffix;
    *pFileFontNumber= ef->efFileFontNumber;
    *pOfficeCharset= oc->ocOfficeCharset;

    return 0;
    }

int fontGetSymbolCharset(	int *			pCharset,
				struct PagedList *	encodedFontList,
				const DocumentFont *	df,
				int			symbol )
    {
    int			csIdx;
    const EncodedFont *	ef;
    int			charset= FONTcharsetDEFAULT;
    int			fontNumber= -1;

    csIdx= utilIndexMappingGet( &(df->dfUnicodeToCharset), symbol );
    if  ( csIdx < 0 )
	{ csIdx= df->dfCharsetIdxUsed[0];	}

    ef= fontGetEncodedFont( encodedFontList, df, csIdx );
    if  ( ef )
	{
	charset= ef->ecCharset;
	fontNumber= ef->efFileFontNumber;
	}
    else{ LXXDEB(csIdx,ef,symbol);		}

    *pCharset= charset;
    return fontNumber;
    }

int fontGetDefaultFileFontNumber(
				struct PagedList *	encodedFontList,
				const DocumentFont *	df )
    {
    const EncodedFont * ef;
    const int		cs= 0;

    ef= fontGetEncodedFont( encodedFontList, df, df->dfCharsetIdxUsed[cs] );
    if  ( ! ef )
	{ LXDEB(cs,ef); return -1;	}

    return ef->efFileFontNumber;
    }

/************************************************************************/

/************************************************************************/
/*									*/
/*  Add a font while reading the font table of the document.		*/
/*									*/
/************************************************************************/

int fontAddEncodedFontToList(	DocumentFontList *	dfl,
				PagedList *		encodedFontList,
				DocumentFont *		dfIn,
				EncodedFont *		efIn )
    {
    DocumentFont *		df;
    EncodedFont *		efOut;
    const OfficeCharset *	oc;

    if  ( utilMemoryBufferEqualsString( &(dfIn->dfName), "ZapfDingbats" ) )
	{
	if  ( fontSetFamilyName( dfIn, "ITC Zapf Dingbats" ) )
	    { LDEB(1); return -1;	}
	}
    if  ( utilMemoryBufferEqualsString( &(dfIn->dfName), "ZapfChancery" ) )
	{
	if  ( fontSetFamilyName( dfIn, "ITC Zapf Chancery" ) )
	    { LDEB(1); return -1;	}
	}

    fontRemoveCharsetFromFontName( dfIn, efIn->ecCharset );

    efIn->ecBufFontNumber= fontListGetFontByName(
			dfl, utilMemoryBufferGetString( &(dfIn->dfName) ) );
    if  ( efIn->ecBufFontNumber < 0 )
	{
	SLDEB(utilMemoryBufferGetString(&(dfIn->dfName)),efIn->ecBufFontNumber);
	return -1;
	}

    df= fontFontListGetFontByNumber( dfl, efIn->ecBufFontNumber );
    if  ( ! df )
	{ LXDEB(efIn->ecBufFontNumber,df); return -1; }

    if  ( fontCopyDocumentFont( df, dfIn ) )
	{ LDEB(1); return -1;	}
    df->dfDocFontNumber= efIn->ecBufFontNumber;

    efOut= (EncodedFont *)utilPagedListClaimItem(
				encodedFontList, efIn->efFileFontNumber );
    if  ( ! efOut )
	{ LXDEB(efIn->efFileFontNumber,efOut); return -1; }

    oc= textGetOfficeCharsetByCharset( &(efIn->ecCharsetIdx), efIn->ecCharset );

    if  ( ! oc )
	{ XDEB(oc);	}
    else{
	int	i;

	for ( i= 0; i < df->dfCharsetIdxUsedCount; i++ )
	    {
	    if  ( df->dfCharsetIdxUsed[i] == efIn->ecCharsetIdx )
		{ break;	}
	    }
	if  ( i >= df->dfCharsetIdxUsedCount )
	    {
	    if  ( df->dfCharsetIdxUsedCount >= CHARSETidx_COUNT )
		{ LLDEB(df->dfCharsetIdxUsedCount,CHARSETidx_COUNT);	}
	    else{
		df->dfCharsetIdxUsed[df->dfCharsetIdxUsedCount++]=
							    efIn->ecCharsetIdx;
		}
	    }
	}

    *efOut= *efIn;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Check whether a font only has default encoding.			*/
/*  [Used after reading the font table of an RTF file.]			*/
/*									*/
/************************************************************************/

void fontDetermineEncoding(	DocumentFont *	df )
    {
    int		cs;

    df->dfFontSpecificEncoding= df->dfCharsetIdxUsedCount > 0;

    for ( cs= 0; cs < df->dfCharsetIdxUsedCount; cs++ )
	{
	const OfficeCharset *	oc;

	oc= textGetOfficeCharsetByIndex( df->dfCharsetIdxUsed[cs] );
	/* ??
	if  ( oc->ocOfficeCharset != FONTcharsetDEFAULT	&&
	      oc->ocOfficeCharset != FONTcharsetSYMBOL	)
	    { df->dfFontSpecificEncoding= 0; break;	}
	*/
	if  ( oc->ocOfficeCharset != FONTcharsetSYMBOL	)
	    { df->dfFontSpecificEncoding= 0; break;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Map a file font number to a document font number while reading an	*/
/*  RTF document.							*/
/*									*/
/************************************************************************/


int fontMapFileFont(	const struct PagedList *	encodedFontList,
			int *				pDocFontNum,
			int *				pCharset,
			int				fileFontNum )
    {
    EncodedFont *	ef;

    if  ( fileFontNum < 0 )
	{ LDEB(fileFontNum); return -1;	}

    ef= (EncodedFont *)utilPagedListGetItemByNumber(
					    encodedFontList, fileFontNum );
    if  ( ! ef )
	{ return 1;	}

    if  ( ef->ecCodepage >= 0 )
	{ LDEB(ef->ecCodepage);	}

    *pDocFontNum= ef->ecBufFontNumber;
    *pCharset= ef->ecCharset;

    return 0;
    }
