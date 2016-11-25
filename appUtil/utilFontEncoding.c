#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"utilFontEncoding.h"
#   include	<charnames.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List of more or less actively supported font encodings.		*/
/*									*/
/************************************************************************/

FontCharset PS_Encodings[ENCODINGps_COUNT]=
{
    { psIsoLatin1GlyphNames,	256,
			    "-iso1", "Enc-iso1",
			    "encodingLatin1", "Latin 1",
			    FONTcharsetANSI, FONTcodepageANSI, (const char *)0,
			    },
    { psIsoLatin2GlyphNames,	256,
			    "-iso2", "Enc-iso2",
			    "encodingLatin2", "Latin 2",
			    FONTcharsetEE, FONTcodepageEE, " CE",
			    },
    { psSymbolGlyphNames,	256,
			    "-symb", "Enc-symb",
			    "encodingSymbol", "Symbol",
			    FONTcharsetSYMBOL, -1, (const char *)0,
			    },
    { psCyrillicGlyphNames,	256,
			    "-cyr", "Enc-cyr",
			    "encodingCyrillic", "Cyrillic",
			    FONTcharsetRUSSIAN, FONTcodepageRUSSIAN, " Cyr",
			    },
    { psDingbatGlyphNames,	256,
			    "-ding", "Enc-ding",
			    "encodingDingbats", "Dingbats",
			    -1, -1, (const char *)0,
			    },
    { psIsoLatin7GlyphNames,	256,
			    "-iso7", "Enc-iso7",
			    "encodingLatin7", "Latin 7",
			    FONTcharsetGREEK, FONTcodepageGREEK, " Greek",
			    },
};

/************************************************************************/
/*									*/
/*  Return our style encoding for a windows codepage.			*/
/*									*/
/************************************************************************/

int utilEncodingFromWindowsCodepage(	int	codepage )
    {
    switch( codepage )
	{
	case FONTcodepagePC_437:	return ENCODINGpsISO_8859_1;
	case FONTcodepageTHAI:		return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageSHIFTJIS:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageGB2313:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageHANGEUL:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageCHINESEBIG5:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageEE:		return ENCODINGpsISO_8859_2;
	case FONTcodepageRUSSIAN:	return ENCODINGpsADOBE_CYRILLIC;
	case FONTcodepageANSI:		return ENCODINGpsISO_8859_1;
	case FONTcodepageGREEK:		return ENCODINGpsISO_8859_7;
	case FONTcodepageTURKISH:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageHEBREW:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageARABIC:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageBALTIC:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageVIETNAMESE:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageJOHAB:		return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageMAC_X:		return ENCODINGpsISO_8859_1;   /*!*/
	}

    LDEB(codepage); return -1;
    }

/************************************************************************/
/*									*/
/*  Return the (Windows) character set of a (Windows) codepage.		*/
/*									*/
/************************************************************************/

int utilWindowsCharsetFromCodepage(	int	codepage )
    {
    switch( codepage )
	{
	case FONTcodepagePC_437:	return FONTcharsetPC_437;
	case FONTcodepageTHAI:		return FONTcharsetTHAI;
	case FONTcodepageSHIFTJIS:	return FONTcharsetSHIFTJIS;
	case FONTcodepageGB2313:	return FONTcharsetGB2313;
	case FONTcodepageHANGEUL:	return FONTcharsetHANGEUL;
	case FONTcodepageCHINESEBIG5:	return FONTcharsetCHINESEBIG5;
	case FONTcodepageEE:		return FONTcharsetEE;
	case FONTcodepageRUSSIAN:	return FONTcharsetRUSSIAN;
	case FONTcodepageANSI:		return FONTcharsetANSI;
	case FONTcodepageGREEK:		return FONTcharsetGREEK;
	case FONTcodepageTURKISH:	return FONTcharsetTURKISH;
	case FONTcodepageHEBREW:	return FONTcharsetHEBREW;
	case FONTcodepageARABIC:	return FONTcharsetARABIC;
	case FONTcodepageBALTIC:	return FONTcharsetBALTIC;
	case FONTcodepageVIETNAMESE:	return FONTcharsetVIETNAMESE;
	case FONTcodepageJOHAB:		return FONTcharsetJOHAB;
	case FONTcodepageMAC_X:		return FONTcharsetMAC_X;
	}

    LDEB(codepage); return -1;
    }

/************************************************************************/
/*									*/
/*  Remember the character classification information for a particular	*/
/*  character set encoding.						*/
/*									*/
/************************************************************************/

void utilSetLatin1CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO1_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO1_isupper( i ) )
		{ charShifts[i]= ISO1_toupper( i );	}
	    }

	if  ( ISO1_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO1_islower( i ) )
		{ charShifts[i]= ISO1_tolower( i );	}
	    }

	if  ( ISO1_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin2CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO2_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO2_isupper( i ) )
		{ charShifts[i]= ISO2_toupper( i );	}
	    }

	if  ( ISO2_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO2_islower( i ) )
		{ charShifts[i]= ISO2_tolower( i );	}
	    }

	if  ( ISO2_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin7CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO7_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO7_isupper( i ) )
		{ charShifts[i]= ISO7_toupper( i );	}
	    }

	if  ( ISO7_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO7_islower( i ) )
		{ charShifts[i]= ISO7_tolower( i );	}
	    }

	if  ( ISO7_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetKoi8rCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( KOI8R_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! KOI8R_isupper( i ) )
		{ charShifts[i]= KOI8R_toupper( i );	}
	    }

	if  ( KOI8R_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! KOI8R_islower( i ) )
		{ charShifts[i]= KOI8R_tolower( i );	}
	    }

	if  ( KOI8R_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetSymbolCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( PSSYM_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! PSSYM_isupper( i ) )
		{ charShifts[i]= PSSYM_toupper( i );	}
	    }

	if  ( PSSYM_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! PSSYM_islower( i ) )
		{ charShifts[i]= PSSYM_tolower( i );	}
	    }

	if  ( PSSYM_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetEncodingCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256],
					int		encoding )
    {
    int		i;

    memset( charKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ charShifts[i]= i; }

    switch( encoding )
	{
	case ENCODINGpsFONTSPECIFIC:
	    /*  impossible */
	    break;

	case ENCODINGpsISO_8859_1:
	    utilSetLatin1CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_2:
	    utilSetLatin2CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_SYMBOL:
	    utilSetSymbolCharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_CYRILLIC:
	    utilSetKoi8rCharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_DINGBATS:
	    /*  no digits, no letters */
	    break;

	case ENCODINGpsISO_8859_7:
	    utilSetLatin7CharacterKinds( charKinds, charShifts );
	    break;

	default:
	    LDEB(encoding);
	    break;
	}

    return;
    }

void utilInitializeFontEncodings( void )
    {
    int		enc;

    static int	done= 0;

    if  ( done )
	{ return;	}

    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	{
	utilSetEncodingCharacterKinds( PS_Encodings[enc].fcCharKinds,
					PS_Encodings[enc].fcCharShifts, enc );
	}

    done= 1;

    return;
    }
