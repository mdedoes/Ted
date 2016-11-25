/************************************************************************/
/*									*/
/*  Data that describes where a font belongs in the mess of font	*/
/*  encodings and character sets that we have to cope with.		*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_FONT_ENCODING_H
#   define	UTIL_FONT_ENCODING_H

/************************************************************************/
/*									*/
/*  Document character sets that are more or less actively supported	*/
/*									*/
/************************************************************************/

#   define	DOCcharsetANSI			0
#   define	DOCcharsetMAC			1
#   define	DOCcharsetPC			2
#   define	DOCcharsetPCA			3

/************************************************************************/
/*									*/
/*  Font encodings that we more or less actively support.		*/
/*									*/
/************************************************************************/

#   define	ENCODINGpsFONTSPECIFIC		-1
#   define	ENCODINGpsISO_8859_1		0
#   define	ENCODINGpsISO_8859_2		1
#   define	ENCODINGpsADOBE_SYMBOL		2
#   define	ENCODINGpsADOBE_CYRILLIC	3
#   define	ENCODINGpsADOBE_DINGBATS	4
#   define	ENCODINGpsISO_8859_7		5
#   define	ENCODINGps_COUNT		6

typedef struct FontCharset
    {
    const char * const *	fcGlyphNames;
    int				fcGlyphCount;
    const char *		fcEncodingSuffix;
    const char *		fcEncodingArray;

    char *			fcId;
    char *			fcLabel;

    int				fcOfficeCharset;
    int				fcOfficeCodepage;
    const char *		fcOfficeFontnameSuffix;

    unsigned char		fcCharKinds[256];
    unsigned char		fcCharShifts[256];
    } FontCharset;

/************************************************************************/
/*									*/
/*  Font character sets as used by windows programs.			*/
/*									*/
/************************************************************************/

#   define FONTcharsetANSI		0	/*  cpg 1252		*/
#   define FONTcharsetDEFAULT		1	/*  i.e. font specific	*/
#   define FONTcharsetSYMBOL		2
#   define FONTcharsetINVALID		3

#   define FONTcharsetMAC		77

#   define FONTcharsetSHIFTJIS		128	/*  cpg 932		*/
#   define FONTcharsetHANGEUL		129	/*  cpg 949		*/
#   define FONTcharsetJOHAB		130	/*  cpg 1361		*/
#   define FONTcharsetGB2313		134	/*  cpg 936		*/
#   define FONTcharsetCHINESEBIG5	136	/*  cpg 950		*/
#   define FONTcharsetGREEK		161	/*  cpg 1253		*/
#   define FONTcharsetTURKISH		162	/*  cpg 1254		*/
#   define FONTcharsetVIETNAMESE	163	/*  cpg	1258		*/
#   define FONTcharsetBALTIC		186	/*  cpg 1257		*/
#   define FONTcharsetHEBREW		177	/*  cpg 1255		*/
#   define FONTcharsetARABIC		178	/*  cpg 1256		*/
#   define FONTcharsetARABIC_TRADITION	179
#   define FONTcharsetARABIC_USER	180
#   define FONTcharsetHEBREW_USER	181
#   define FONTcharsetRUSSIAN		204	/*  cpg 1251		*/
#   define FONTcharsetTHAI		222	/*  cpg 874		*/
#   define FONTcharsetEE		238	/*  cpg 1250		*/

#   define FONTcharsetPC_437		254	/*  cpg 437		*/
#   define FONTcharsetOEM		255

#   define FONTcharsetMAC_X		256 	/* ?			*/

/************************************************************************/
/*									*/
/*  Font codepages sets as used by windows programs.			*/
/*									*/
/************************************************************************/

#   define FONTcodepagePC_437		437
#   define FONTcodepageTHAI		874
#   define FONTcodepageSHIFTJIS		932
#   define FONTcodepageGB2313		936
#   define FONTcodepageHANGEUL		949
#   define FONTcodepageCHINESEBIG5	950
#   define FONTcodepageEE		1250
#   define FONTcodepageRUSSIAN		1251
#   define FONTcodepageANSI		1252
#   define FONTcodepageGREEK		1253
#   define FONTcodepageTURKISH		1254
#   define FONTcodepageHEBREW		1255
#   define FONTcodepageARABIC		1256
#   define FONTcodepageBALTIC		1257
#   define FONTcodepageVIETNAMESE	1258
#   define FONTcodepageJOHAB		1361
#   define FONTcodepageMAC_X		10000

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern FontCharset PS_Encodings[ENCODINGps_COUNT];

extern const char * const	psIsoLatin1GlyphNames[256];
extern const char * const	psIsoLatin2GlyphNames[256];
extern const char * const	psIsoLatin7GlyphNames[256];
extern const char * const	psSymbolGlyphNames[256];
extern const char * const	psCyrillicGlyphNames[256];
extern const char * const	psDingbatGlyphNames[256];

extern const unsigned char	docWIN1250_to_ISO2[256];
extern const unsigned char	docISO2_to_WIN1250[256];

extern const unsigned char	docWIN1253_to_ISO7[256];
extern const unsigned char	docISO7_to_WIN1253[256];

extern const unsigned char	docWIN1251_to_KOI8R[256];
extern const unsigned char	docKOI8R_to_WIN1251[256];

extern const unsigned char	docWIN1251_to_ISO5[256];
extern const unsigned char	docISO5_to_WIN1251[256];

extern const unsigned char	docDOS437_to_ISO1[256];
extern const unsigned char	docDOS850_to_ISO1[256];
extern const unsigned char	docMAC_to_ISO1[256];

extern void utilSetEncodingCharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256],
				int			encoding );

extern void utilSetLatin1CharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256] );

extern void utilSetLatin2CharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256] );

extern void utilSetLatin7CharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256] );

extern void utilSetSymbolCharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256] );

extern void utilSetKoi8rCharacterKinds(
				unsigned char		charKinds[256],
				unsigned char		charShifts[256] );

extern void utilInitializeFontEncodings( void );

extern int utilWindowsCharsetFromCodepage(	int	codepage );
extern int utilEncodingFromWindowsCodepage(	int	codepage );

#   endif
