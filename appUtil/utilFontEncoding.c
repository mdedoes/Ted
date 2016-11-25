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
			    "iso8859", "1",
			    },
    { psIsoLatin2GlyphNames,	256,
			    "-iso2", "Enc-iso2",
			    "encodingLatin2", "Latin 2",
			    FONTcharsetEE, FONTcodepageEE, " CE",
			    "iso8859", "2",
			    },
    { psSymbolGlyphNames,	256,
			    "-symb", "Enc-symb",
			    "encodingSymbol", "Symbol",
			    FONTcharsetSYMBOL, -1, (const char *)0,
			    "adobe", "fontspecific",
			    },
    { psIsoLatin5GlyphNames,	256,
			    "-cyr", "Enc-cyr",
			    "encodingCyrillic", "Cyrillic",
			    FONTcharsetRUSSIAN, FONTcodepageRUSSIAN, " Cyr",
			    "iso8859", "5",
			    },
    { psDingbatGlyphNames,	256,
			    "-ding", "Enc-ding",
			    "encodingDingbats", "Dingbats",
			    FONTcharsetDEFAULT, -1, (const char *)0,
			    "urw", "fontspecific",
			    },
    { psIsoLatin7GlyphNames,	256,
			    "-iso7", "Enc-iso7",
			    "encodingLatin7", "Greek",
			    FONTcharsetGREEK, FONTcodepageGREEK, " Greek",
			    "iso8859", "7",
			    },
    { psIsoLatin9GlyphNames,	256,
			    "-iso9", "Enc-iso9",
			    "encodingLatin9", "Turkish",
			    FONTcharsetTURKISH, FONTcodepageTURKISH, " Tur",
			    "iso8859", "9",
			    },
    { psIsoLatin13GlyphNames,	256,
			    "-iso13", "Enc-iso13",
			    "encodingLatin13", "Baltic",
			    FONTcharsetBALTIC, FONTcodepageBALTIC, " Baltic",
			    "iso8859", "13",
			    },
};

/************************************************************************/
/*									*/
/*  Return our style encoding from X11 registry, encoding combination.	*/
/*									*/
/*  1)  Hacks for known fonts.						*/
/*  2)  Find registry and encoding in name.				*/
/*  3)  Nothing can be done about the fact that the fontspecific X11	*/
/*	encodings return silly results.					*/
/*  4)  Table lookup.							*/
/*									*/
/************************************************************************/

int utilEncodingFromX11FontName(	const char *	x11name )
    {
    int			reg_enc= -1;
    int			enc= -1;
    int			rl;
    int			j;
    int			d;
    int			encoding;

    /*  1  */

    /*  2  */
    j= 0; d= 0;
    while( x11name[j] )
	{
	if  ( x11name[j] == '-' )
	    {
	    if  ( d == 12 )
		{ reg_enc= j+ 1;	}
	    if  ( d == 13 )
		{ enc= j+ 1;	}

	    d++;
	    }

	j++;
	}

    if  ( reg_enc < 0 || enc < 0 )
	{ SDEB(x11name); return -1;	}

    /*  3 
    if  ( ! strcmp( x11name+ enc, "fontspecific" ) )
	{ return -1;	}
    */

    rl= enc- reg_enc- 1;

    /*  4  */
    for ( encoding= 0; encoding < ENCODINGps_COUNT; encoding++ )
	{
	if  ( ! PS_Encodings[encoding].fcX11Encoding )
	    { continue;	}
	if  ( ! PS_Encodings[encoding].fcX11Registry )
	    { continue;	}

	if  ( strcmp( PS_Encodings[encoding].fcX11Encoding, x11name+ enc ) )
	    { continue;	}
	if  ( strncmp( PS_Encodings[encoding].fcX11Registry,
						    x11name+ reg_enc, rl ) )
	    { continue;	}
	if  ( PS_Encodings[encoding].fcX11Registry[rl] )
	    { continue;	}

	return encoding;
	}

    return -1;
    }

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
	case FONTcodepageTURKISH:	return ENCODINGpsISO_8859_9;
	case FONTcodepageHEBREW:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageARABIC:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageBALTIC:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageVIETNAMESE:	return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageJOHAB:		return ENCODINGpsFONTSPECIFIC; /*!*/
	case FONTcodepageMAC_X:		return ENCODINGpsISO_8859_1;   /*!*/
	}

    LDEB(codepage); return -1;
    }

int utilWindowsCodepageFromEncoding(	int	encoding )
    {
    if  ( encoding >= 0 && encoding < ENCODINGps_COUNT )
	{ return PS_Encodings[encoding].fcOfficeCodepage; }

    LLDEB(encoding,ENCODINGps_COUNT); return -1;
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

void utilSetLatin5CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO5_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO5_isupper( i ) )
		{ charShifts[i]= ISO5_toupper( i );	}
	    }

	if  ( ISO5_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO5_islower( i ) )
		{ charShifts[i]= ISO5_tolower( i );	}
	    }

	if  ( ISO5_isdigit( i ) )
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

void utilSetLatin9CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO9_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO9_isupper( i ) )
		{ charShifts[i]= ISO9_toupper( i );	}
	    }

	if  ( ISO9_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO9_islower( i ) )
		{ charShifts[i]= ISO9_tolower( i );	}
	    }

	if  ( ISO9_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin13CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO13_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO13_isupper( i ) )
		{ charShifts[i]= ISO13_toupper( i );	}
	    }

	if  ( ISO13_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO13_islower( i ) )
		{ charShifts[i]= ISO13_tolower( i );	}
	    }

	if  ( ISO13_isdigit( i ) )
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

	case ENCODINGpsISO_8859_9:
	    utilSetLatin9CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_13:
	    utilSetLatin13CharacterKinds( charKinds, charShifts );
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
