/************************************************************************/
/*									*/
/*  Table of rtf tags thet are not treated as an exception.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<utilFontEncoding.h>
#   include	<utilJenkinsPerfectHash.h>

#   include	"docRtf.h"

#   include	<appDebugon.h>

#   define	BUILD_NO_HASH				/*  for sed */

#   ifdef	BUILD_HASH

#	define		docRtfBrdrProperty			0
#	define		docRtfBeginBorder			0
#	define		docRtfRememberProperty			0
#	define		docRtfRememberDocProperty		0
#	define		docRtfRememberStyleProperty		0
#	define		docRtfRememberSectionProperty		0
#	define		docRtfRememberRowProperty		0
#	define		docRtfRememberCellProperty		0
#	define		docRtfRememberParagraphProperty		0
#	define		docRtfRememberTextProperty		0
#	define		docRtfRememberListLevelProperty		0
#	define		docRtfRememberListProperty		0
#	define		docRtfRememberListOverrideProperty	0
#	define		docRtfRememberListOverrideLevelProperty	0
#	define		docRtfRememberTabStopProperty		0
#	define		docRtfRememberTextFrameProperty		0
#	define		docRtfFontProperty			0
#	define		docRtfRememberPntextProperty		0
#	define		docRtfObjectProperty			0

#   endif

/************************************************************************/
/*									*/
/*  Control words to remember border properties.			*/
/*									*/
/************************************************************************/

# define RTF_BORDER_PROP( s, id ) \
		    { s, id, DOClevANY, docRtfBrdrProperty, }
# define RTF_BORDER_STYLE(s,v) \
		    { s, BRDRpropSTYLE, DOClevANY, docRtfBrdrProperty, v, }

static RtfControlWord	docRtfBorderWords[]=
    {
	RTF_BORDER_PROP( "brdrw",		BRDRpropPEN_WIDE ),
	RTF_BORDER_PROP( "brdrcf",		BRDRpropCOLOR ),
	RTF_BORDER_PROP( "brsp",		BRDRpropSPACING ),

	/**/

	RTF_BORDER_STYLE( "brdrnone",		DOCbsNONE ),
	RTF_BORDER_STYLE( "brdrs",		DOCbsS ),
	RTF_BORDER_STYLE( "brdrth",		DOCbsTH ),
	RTF_BORDER_STYLE( "brdrsh",		DOCbsSH ),

	RTF_BORDER_STYLE( "brdrdb",		DOCbsDB ),
	RTF_BORDER_STYLE( "brdrhair",		DOCbsHAIR ),
	RTF_BORDER_STYLE( "brdrdot",		DOCbsDOT ),
	RTF_BORDER_STYLE( "brdrdash",		DOCbsDASH  ),
	RTF_BORDER_STYLE( "brdrdashd",		DOCbsDASHD ),
	RTF_BORDER_STYLE( "brdrdashdd",		DOCbsDASHDD ),
	RTF_BORDER_STYLE( "brdrtriple",		DOCbsTRIPLE ),

	RTF_BORDER_STYLE( "brdrtnthsg",		DOCbsTNTHSG ),
	RTF_BORDER_STYLE( "brdrthtnsg",		DOCbsTHTNSG ),
	RTF_BORDER_STYLE( "brdrtnthtnsg",	DOCbsTNTHTNSG ),
	RTF_BORDER_STYLE( "brdrtnthmg",		DOCbsTNTHMG ),
	RTF_BORDER_STYLE( "brdrthtnmg",		DOCbsTHTNMG ),
	RTF_BORDER_STYLE( "brdrtnthtnmg",	DOCbsTNTHTNMG ),
	RTF_BORDER_STYLE( "brdrtnthlg",		DOCbsTNTHLG ),
	RTF_BORDER_STYLE( "brdrthtnlg",		DOCbsTHTNLG ),
	RTF_BORDER_STYLE( "brdrtnthtnlg",	DOCbsTNTHTNLG ),

	RTF_BORDER_STYLE( "brdrwavy",		DOCbsWAVY ),
	RTF_BORDER_STYLE( "brdrwavydb",		DOCbsWAVYDB ),
	RTF_BORDER_STYLE( "brdrdashsm",		DOCbsDASHSM ),
	RTF_BORDER_STYLE( "brdrdashdotstr",	DOCbsDASHDOTSTR ),
	RTF_BORDER_STYLE( "brdremboss",		DOCbsEMBOSS ),
	RTF_BORDER_STYLE( "brdrengrave",	DOCbsENGRAVE ),
	RTF_BORDER_STYLE( "brdroutset",		DOCbsOUTSET ),

	/**/

	{ 0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Table of common rtf tags.						*/
/*									*/
/************************************************************************/

/* Not practical
# define rrcBd do not use
*/
# define rrcBi do not use
# define rrcInsertedObject do not use
# define rrcFieldNumber do not use

# define	RTF_REMEMBER(s,id) \
    { s,      id,         DOClevANY,  docRtfRememberProperty, }


# define	RTF_DOC_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, }
# define	RTF_DOC_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberDocProperty, v }


# define	RTF_BORDER(s,id) \
    { s,	id,	DOClevANY,  docRtfRememberProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }


# define	RTF_STYLE_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, }
# define	RTF_STYLE_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, }
# define	RTF_STYLE_VAL_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberStyleProperty, v }

# define	RTF_TEXT_DEFAULT(s) \
		{ s, TAprop_NONE, DOClevANY, docRtfRememberTextProperty, }
# define	RTF_TEXT_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, }
# define	RTF_TEXT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, v, }
# define	RTF_TEXT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberTextProperty, }

# define	RTF_PNTEXT_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, }
# define	RTF_PNTEXT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, v, }
# define	RTF_PNTEXT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberPntextProperty, }

# define	RTF_PARA_DEFAULT(s) \
		{ s, PPprop_NONE, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, v, }
# define	RTF_PARA_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, }
# define	RTF_PARA_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberParagraphProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }


# define	RTF_SECT_DEFAULT(s) \
		{ s, SPprop_NONE, DOClevANY, docRtfRememberSectionProperty, }
# define	RTF_SECT_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, }
# define	RTF_SECT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, v, }
# define	RTF_SECT_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberSectionProperty, }

# define	RTF_FRAME_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberTextFrameProperty, }
# define	RTF_FRAME_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTextFrameProperty, v, }
# define	RTF_FRAME_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberTextFrameProperty, }

# define	RTF_CELL_DEFAULT(s) \
		{ s, CLprop_NONE, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, v, }
# define	RTF_CELL_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, }
# define	RTF_CELL_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberCellProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

# define	RTF_ROW_DEFAULT(s) \
		{ s, RPprop_NONE, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, v, }
# define	RTF_ROW_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, }
# define	RTF_ROW_BORDER(s,id) \
		{ s, id, DOClevANY, docRtfRememberRowProperty, 0, \
				    docRtfBeginBorder, docRtfBorderWords, }

# define	RTF_LISTLEV_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberListLevelProperty, }

# define	RTF_LIST_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberListProperty, }
# define	RTF_LIST_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberListProperty, v, }
# define	RTF_LIST_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberListProperty, }

# define	RTF_LISTOVERR_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfRememberListOverrideProperty, }

# define	RTF_LISTOVERRLEV_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberListOverrideLevelProperty, }

# define	RTF_TAB_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfRememberTabStopProperty, v }
# define	RTF_TAB_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfRememberTabStopProperty }

# define	RTF_FONT_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfFontProperty, v }
# define	RTF_FONT_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfFontProperty, }

# define	RTF_OBJ_ENUM(s,id,v) \
		{ s, id, DOClevANY, docRtfObjectProperty, v }
# define	RTF_OBJ_VALUE(s,id) \
		{ s, id, DOClevANY, docRtfObjectProperty, }
# define	RTF_OBJ_FLAG(s,id) \
		{ s, id, DOClevANY, docRtfObjectProperty, }

RtfControlWord	docRtfPropertyWords[]=
{
				/****************************************/
				/*  Text properties.			*/
				/*  Character Formatting Properties	*/
				/****************************************/
    RTF_TEXT_DEFAULT( "plain" ),

    RTF_TEXT_VALUE( "f",		TApropFONTFAMILY ),
    RTF_TEXT_VALUE( "fs",		TApropFONTSIZE ),
    RTF_TEXT_VALUE( "cf",		TApropTEXT_COLOR ),

    RTF_TEXT_FLAG( "b",			TApropFONTBOLD ),
    RTF_TEXT_FLAG( "i",			TApropFONTSLANTED ),
    RTF_TEXT_FLAG( "ul",		TApropTEXTUNDERLINED ),

    RTF_TEXT_ENUM( "sub",		TApropSUPERSUB, DOCfontSUBSCRIPT ),
    RTF_TEXT_ENUM( "super",		TApropSUPERSUB, DOCfontSUPERSCRIPT ),
    RTF_TEXT_ENUM( "nosupersub",	TApropSUPERSUB, DOCfontREGULAR ),

    RTF_TEXT_FLAG( "caps",		TApropCAPITALS ),
    RTF_TEXT_FLAG( "scaps",		TApropSMALLCAPS ),
    RTF_TEXT_FLAG( "strike",		TApropSTRIKETHROUGH ),

    /**/

    RTF_PNTEXT_VALUE( "pnf",		TApropFONTFAMILY ),
    RTF_PNTEXT_VALUE( "pnfs",		TApropFONTSIZE ),
    RTF_PNTEXT_VALUE( "pbcf",		TApropTEXT_COLOR ),

    RTF_PNTEXT_FLAG( "pnb",		TApropFONTBOLD ),
    RTF_PNTEXT_FLAG( "pni",		TApropFONTSLANTED ),
    RTF_PNTEXT_FLAG( "pnul",		TApropTEXTUNDERLINED ),

    RTF_PNTEXT_FLAG( "pncaps",		TApropCAPITALS ),
    RTF_PNTEXT_FLAG( "pnscaps",		TApropSMALLCAPS ),
    RTF_PNTEXT_FLAG( "pnstrike",	TApropSTRIKETHROUGH ),

    /*
    RTF_REMEMBER( "pnuld",		RTFidPNULD ),
    RTF_REMEMBER( "pnuldb",		RTFidPNULDB ),
    RTF_REMEMBER( "pnulnone",		RTFidPNULNONE ),
    */

    /**/

    RTF_REMEMBER( "lang",		RTFidLANG ),
    RTF_REMEMBER( "noproof",		RTFidNOPROOF ),
    RTF_REMEMBER( "ulnone",		RTFidULNONE ),
    RTF_REMEMBER( "up",			RTFidUP ),
    RTF_REMEMBER( "cgrid",		RTFidCGRID ),

    RTF_REMEMBER( "cb",			RTFidCB ),
    RTF_REMEMBER( "cchs",		RTFidCCHS ),
    RTF_REMEMBER( "deleted",		RTFidDELETED ),
    RTF_REMEMBER( "dn",			RTFidDN ),
    RTF_REMEMBER( "ltrch",		RTFidLTRCH ),
    RTF_REMEMBER( "outl",		RTFidOUTL ),
    RTF_REMEMBER( "revauth",		RTFidREVAUTH ),
    RTF_REMEMBER( "crauth",		RTFidREVAUTH ),
    RTF_REMEMBER( "crdate",		RTFidREVAUTH ),
    RTF_REMEMBER( "revauthdel",		RTFidREVAUTHDEL ),
    RTF_REMEMBER( "revdttm",		RTFidREVDTTM ),
    RTF_REMEMBER( "revdttmdel",		RTFidREVDTTMDEL ),
    RTF_REMEMBER( "revised",		RTFidREVISED ),
    RTF_REMEMBER( "rtlch",		RTFidRTLCH ),
    RTF_REMEMBER( "shad",		RTFidSHAD ),

    RTF_REMEMBER( "uld",		RTFidULD ),
    RTF_REMEMBER( "uldb",		RTFidULDB ),
    RTF_REMEMBER( "ulw",		RTFidULW ),
    RTF_REMEMBER( "v",			RTFidV ),
    RTF_REMEMBER( "webhidden",		RTFidWEBHIDDEN ),
    RTF_REMEMBER( "kerning",		RTFidKERNING ),
    RTF_REMEMBER( "expnd",		RTFidEXPND ),
    RTF_REMEMBER( "expndtw",		RTFidEXPNDTW ),
    RTF_REMEMBER( "charscalex",		RTFidCHARSCALEX ),
    RTF_REMEMBER( "highlight",		RTFidHIGHLIGHT ),

    RTF_REMEMBER( "chbghoriz",		RTFidCHBGHORIZ ),
    RTF_REMEMBER( "chbgvert",		RTFidCHBGVERT ),
    RTF_REMEMBER( "chbgfdiag",		RTFidCHBGFDIAG ),
    RTF_REMEMBER( "chbgbdiag",		RTFidCHBGBDIAG ),
    RTF_REMEMBER( "chbgcross",		RTFidCHBGCROSS ),
    RTF_REMEMBER( "chbgdcross",		RTFidCHBGDCROSS ),
    RTF_REMEMBER( "chbgdkhoriz",	RTFidCHBGDKHORIZ ),
    RTF_REMEMBER( "chbgdkvert",		RTFidCHBGDKVERT ),
    RTF_REMEMBER( "chbgdkfdiag",	RTFidCHBGDKFDIAG ),
    RTF_REMEMBER( "chbgdkbdiag",	RTFidCHBGDKBDIAG ),
    RTF_REMEMBER( "chbgdkcross",	RTFidCHBGDKCROSS ),
    RTF_REMEMBER( "chbgdkdcross",	RTFidCHBGDKDCROSS ),

    RTF_BORDER(   "chbrdr",		RTFidCHBRDR ),

    RTF_REMEMBER( "chcbpat",		RTFidCHCBPAT ),
    RTF_REMEMBER( "chcfpat",		RTFidCHCFPAT ),
    RTF_REMEMBER( "chshdng",		RTFidCHSHDNG ),

    RTF_REMEMBER( "faauto",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "fahang",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "facenter",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "faroman",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "favar",		RTFid_FONTALIGN ),
    RTF_REMEMBER( "fafixed",		RTFid_FONTALIGN ),

				/****************************************/
				/*  Paragraph properties.		*/
				/****************************************/
    RTF_PARA_DEFAULT( "pard" ),

    RTF_PARA_VALUE( "s",		PPpropSTYLE ),
    RTF_PARA_VALUE( "ls",		PPpropLISTOVERRIDE ),

    RTF_PARA_VALUE( "li",		PPpropLEFT_INDENT ),
    RTF_PARA_VALUE( "fi",		PPpropFIRST_INDENT ),
    RTF_PARA_VALUE( "ri",		PPpropRIGHT_INDENT ),

    RTF_PARA_ENUM( "ql",		PPpropALIGNMENT, DOCiaLEFT ),
    RTF_PARA_ENUM( "qr",		PPpropALIGNMENT, DOCiaRIGHT ),
    RTF_PARA_ENUM( "qj",		PPpropALIGNMENT, DOCiaJUSTIFIED ),
    RTF_PARA_ENUM( "qc",		PPpropALIGNMENT, DOCiaCENTERED ),

    RTF_PARA_FLAG( "pagebb",		PPpropNEWPAGE ),

    RTF_PARA_ENUM( "nowidctlpar",	PPpropWIDCTLPAR, 0 ),
    RTF_PARA_ENUM( "widctlpar",		PPpropWIDCTLPAR, 1 ),

    RTF_PARA_FLAG( "keep",		PPpropKEEP ),
    RTF_PARA_FLAG( "keepn",		PPpropKEEPN ),

    RTF_PARA_VALUE( "sb",		PPpropSPACE_BEFORE ),
    RTF_PARA_VALUE( "sa",		PPpropSPACE_AFTER ),
    RTF_PARA_VALUE( "sl",		PPpropLINE_SPACING_DIST ),

    RTF_PARA_FLAG( "slmult",		PPpropLINE_SPACING_MULT ),

    RTF_PARA_VALUE( "outlinelevel",	PPpropOUTLINELEVEL ),

    RTF_PARA_FLAG( "hyphpar",		PPpropHYPHPAR ),

    RTF_PARA_VALUE( "shading",		PPpropSHADE_LEVEL ),

    RTF_PARA_ENUM( "bgbdiag",		PPpropSHADE_PATTERN, DOCspBGBDIAG ),
    RTF_PARA_ENUM( "bgcross",		PPpropSHADE_PATTERN, DOCspBGCROSS ),
    RTF_PARA_ENUM( "bgdcross",		PPpropSHADE_PATTERN, DOCspBGDCROSS ),
    RTF_PARA_ENUM( "bgdkbdiag",		PPpropSHADE_PATTERN, DOCspBGDKBDIAG ),
    RTF_PARA_ENUM( "bgdkcross",		PPpropSHADE_PATTERN, DOCspBGDKCROSS ),
    RTF_PARA_ENUM( "bgdkdcross",	PPpropSHADE_PATTERN, DOCspBGDKDCROSS),
    RTF_PARA_ENUM( "bgdkfdiag",		PPpropSHADE_PATTERN, DOCspBGDKFDIAG ),
    RTF_PARA_ENUM( "bgdkhoriz",		PPpropSHADE_PATTERN, DOCspBGDKHORIZ ),
    RTF_PARA_ENUM( "bgdkvert",		PPpropSHADE_PATTERN, DOCspBGDKVERT ),
    RTF_PARA_ENUM( "bgfdiag",		PPpropSHADE_PATTERN, DOCspBGFDIAG ),
    RTF_PARA_ENUM( "bghoriz",		PPpropSHADE_PATTERN, DOCspBGHORIZ ),
    RTF_PARA_ENUM( "bgvert",		PPpropSHADE_PATTERN, DOCspBGVERT ),

    RTF_PARA_VALUE( "cfpat",		PPpropSHADE_FORE_COLOR ),
    RTF_PARA_VALUE( "cbpat",		PPpropSHADE_BACK_COLOR ),

    RTF_PARA_BORDER( "brdrt",		PPpropTOP_BORDER ),
    RTF_PARA_BORDER( "brdrb",		PPpropBOTTOM_BORDER ),
    RTF_PARA_BORDER( "brdrl",		PPpropLEFT_BORDER ),
    RTF_PARA_BORDER( "brdrr",		PPpropRIGHT_BORDER ),
    RTF_PARA_BORDER( "box",		PPpropBOX_BORDER ),
    RTF_PARA_BORDER( "brdrbtw",		PPpropBETWEEN_BORDER ),
    RTF_PARA_BORDER( "brdrbar",		PPpropBAR_BORDER ),

					/**/

    RTF_REMEMBER( "intbl",		RTFidINTBL ),

    RTF_REMEMBER( "adjustright",	RTFidADJUSTRIGHT ),
    RTF_REMEMBER( "adjustleft",		RTFidADJUSTLEFT ),

    RTF_REMEMBER( "sbauto",		RTFidSBAUTO ),
    RTF_REMEMBER( "saauto",		RTFidSAAUTO ),

    RTF_REMEMBER( "nooverflow",		RTFidNOOVERFLOW ),

    RTF_REMEMBER( "aspalpha",		RTFidASPALPHA ),
    RTF_REMEMBER( "aspnum",		RTFidASPNUM ),

				/****************************************/
				/*  Positioned Objects and Frames:	*/
				/*  ( Paragraph properties. )		*/
				/****************************************/

    RTF_FRAME_VALUE( "absw",		TFPpropABSW ),
    RTF_FRAME_VALUE( "absh",		TFPpropABSH ),

    RTF_FRAME_ENUM( "phmrg",		TFPpropHORIZONTAL_REFERENCE, HFRphMRG ),
    RTF_FRAME_ENUM( "phpg",		TFPpropHORIZONTAL_REFERENCE, HFRphPG ),
    RTF_FRAME_ENUM( "phcol",		TFPpropHORIZONTAL_REFERENCE, HFRphCOL ),

    RTF_FRAME_VALUE( "posx",		TFPpropHORIZONTAL_POSITION ),
    RTF_FRAME_VALUE( "posnegx",		TFPpropHORIZONTAL_POSITION ),

    RTF_FRAME_ENUM( "posxl",		TFPpropHORIZONTAL_ALIGNMENT, HFAposXL ),
    RTF_FRAME_ENUM( "posxr",		TFPpropHORIZONTAL_ALIGNMENT, HFAposXR ),
    RTF_FRAME_ENUM( "posxc",		TFPpropHORIZONTAL_ALIGNMENT, HFAposXC ),
    RTF_FRAME_ENUM( "posxi",		TFPpropHORIZONTAL_ALIGNMENT, HFAposXI ),
    RTF_FRAME_ENUM( "posxo",		TFPpropHORIZONTAL_ALIGNMENT, HFAposXO ),

    RTF_FRAME_ENUM( "pvmrg",		TFPpropVERTICAL_REFERENCE, VFRpvMRG ),
    RTF_FRAME_ENUM( "pvpara",		TFPpropVERTICAL_REFERENCE, VFRpvPARA ),
    RTF_FRAME_ENUM( "pvpg",		TFPpropVERTICAL_REFERENCE, VFRpvPG ),

    RTF_FRAME_VALUE( "posy",		TFPpropVERTICAL_POSITION ),
    RTF_FRAME_VALUE( "posnegy",		TFPpropVERTICAL_POSITION ),

    RTF_FRAME_FLAG( "abslock",		TFPpropLOCKED_TO_PARA ),

    RTF_FRAME_ENUM( "posyb",		TFPpropVERTICAL_ALIGNMENT, VFAposYB ),
    RTF_FRAME_ENUM( "posyc",		TFPpropVERTICAL_ALIGNMENT, VFAposYC ),
    RTF_FRAME_ENUM( "posyil",		TFPpropVERTICAL_ALIGNMENT, VFAposYIL ),
    RTF_FRAME_ENUM( "posyt",		TFPpropVERTICAL_ALIGNMENT, VFAposYT ),

    RTF_FRAME_ENUM( "nowrap",		TFPpropWRAP_STYLE, FWSwrapNOWRAP ),
    RTF_FRAME_ENUM( "overlay",		TFPpropWRAP_STYLE, FWSwrapOVERLAY ),

    RTF_FRAME_VALUE( "dxfrtext",	TFPpropDXFRTEXT ),
    RTF_FRAME_VALUE( "dfrmtxtx",	TFPpropDFRMTXTX ),
    RTF_FRAME_VALUE( "dfrmtxty",	TFPpropDFRMTXTY ),

    /**/
    RTF_REMEMBER( "dropcapli",		RTFidDROPCAPLI ),
    RTF_REMEMBER( "dropcapt",		RTFidDROPCAPT ),
				/****************************************/
				/*  Bullets and Numbering.		*/
				/*  (No destinations)			*/
				/****************************************/
    RTF_REMEMBER( "ilvl",		RTFidILVL ),
				/****************************************/
				/*  Revision marks for paragraph	*/
				/*  numbers.				*/
				/****************************************/
    RTF_REMEMBER( "pnrauth",		RTFidPNRAUTH ),
    RTF_REMEMBER( "pnrdate",		RTFidPNR_REVPROP ),
    RTF_REMEMBER( "pnrnot",		RTFidPNR_REVPROP ),
    RTF_REMEMBER( "pnrnfc",		RTFidPNRNFC ),
    RTF_REMEMBER( "pnrpnbr",		RTFidPNRPNBR ),
    RTF_REMEMBER( "pnrrgb",		RTFidPNRRGB ),
    RTF_REMEMBER( "pnrstart",		RTFidPNRSTART ),
    RTF_REMEMBER( "pnrstop",		RTFidPNRSTOP ),
    RTF_REMEMBER( "pnrxst",		RTFidPNRXST ),
				/****************************************/
				/*  Tabs.				*/
				/****************************************/
    RTF_REMEMBER( "tx",			RTFidTX ),
    RTF_REMEMBER( "tb",			RTFidTB ),

				/****************************************/
				/*  Associated Character Properties	*/
				/****************************************/
    RTF_REMEMBER( "ab",			RTFidAB ),
    RTF_REMEMBER( "acaps",		RTFidACAPS ),
    RTF_REMEMBER( "acf",		RTFidACF ),
    RTF_REMEMBER( "adn",		RTFidADN ),
    RTF_REMEMBER( "aexpnd",		RTFidAEXPND ),
    RTF_REMEMBER( "af",			RTFidAF ),
    RTF_REMEMBER( "afs",		RTFidAFS ),
    RTF_REMEMBER( "ai",			RTFidAI ),
    RTF_REMEMBER( "alang",		RTFidALANG ),
    RTF_REMEMBER( "aoutl",		RTFidAOUTL ),
    RTF_REMEMBER( "ascaps",		RTFidASCAPS ),
    RTF_REMEMBER( "ashad",		RTFidASHAD ),
    RTF_REMEMBER( "astrike",		RTFidASTRIKE ),
    RTF_REMEMBER( "aul",		RTFidAUL ),
    RTF_REMEMBER( "auld",		RTFidAULD ),
    RTF_REMEMBER( "auldb",		RTFidAULDB ),
    RTF_REMEMBER( "aulnone",		RTFidAULNONE ),
    RTF_REMEMBER( "aulw",		RTFidAULW ),
    RTF_REMEMBER( "aup",		RTFidAUP ),
    RTF_REMEMBER( "loch",		RTFidLOCH ),
    RTF_REMEMBER( "hich",		RTFidHICH ),
    RTF_REMEMBER( "dbch",		RTFidDBCH ),

				/****************************************/
				/*  Row, Cell/Column properties.	*/
				/****************************************/

    RTF_REMEMBER( "ltrrow",		RTFidLTRROW ),
    RTF_REMEMBER( "rtlrow",		RTFidRTLROW ),

    RTF_REMEMBER( "cellx",		RTFidCELLX ),

    RTF_CELL_DEFAULT( "celld" ),

    RTF_CELL_VALUE( "clcfpat",		CLpropSHADE_FORE_COLOR ),
    RTF_CELL_VALUE( "clcbpat",		CLpropSHADE_BACK_COLOR ),

    RTF_CELL_BORDER( "clbrdrt",		CLpropTOP_BORDER ),
    RTF_CELL_BORDER( "clbrdrb",		CLpropBOTTOM_BORDER ),
    RTF_CELL_BORDER( "clbrdrl",		CLpropLEFT_BORDER ),
    RTF_CELL_BORDER( "clbrdrr",		CLpropRIGHT_BORDER ),

    RTF_CELL_VALUE( "clshdng",		CLpropSHADE_LEVEL ),

    RTF_CELL_ENUM( "clvertalt",		CLpropTEXT_VERT_ALIGN,
							DOCtvaTOP ),
    RTF_CELL_ENUM( "clvertalc",		CLpropTEXT_VERT_ALIGN,
							DOCtvaCENTERED ),
    RTF_CELL_ENUM( "clvertalb",		CLpropTEXT_VERT_ALIGN,
							DOCtvaBOTTOM ),

    RTF_CELL_ENUM( "cltxlrtb",		CLpropTEXT_FLOW, CLflowTXLRTB ),
    RTF_CELL_ENUM( "cltxtbrl",		CLpropTEXT_FLOW, CLflowTXTBRL ),
    RTF_CELL_ENUM( "cltxbtlr",		CLpropTEXT_FLOW, CLflowTXBTLR ),
    RTF_CELL_ENUM( "cltxlrtbv",		CLpropTEXT_FLOW, CLflowTXLRTBV ),
    RTF_CELL_ENUM( "cltxtbrlv",		CLpropTEXT_FLOW, CLflowTXTBRLV ),


    RTF_CELL_ENUM( "clbgbdiag",		CLpropSHADE_PATTERN, DOCspBGBDIAG ),
    RTF_CELL_ENUM( "clbgcross",		CLpropSHADE_PATTERN, DOCspBGCROSS ),
    RTF_CELL_ENUM( "clbgdcross",	CLpropSHADE_PATTERN, DOCspBGDCROSS ),
    RTF_CELL_ENUM( "clbgdkbdiag",	CLpropSHADE_PATTERN, DOCspBGDKBDIAG ),
    RTF_CELL_ENUM( "clbgdkcross",	CLpropSHADE_PATTERN, DOCspBGDKCROSS ),
    RTF_CELL_ENUM( "clbgdkdcross",	CLpropSHADE_PATTERN, DOCspBGDKDCROSS ),
    RTF_CELL_ENUM( "clbgdkfdiag",	CLpropSHADE_PATTERN, DOCspBGDKFDIAG ),
    RTF_CELL_ENUM( "clbgdkhor",		CLpropSHADE_PATTERN, DOCspBGDKHORIZ ),
    RTF_CELL_ENUM( "clbgdkvert",	CLpropSHADE_PATTERN, DOCspBGDKVERT ),
    RTF_CELL_ENUM( "clbgfdiag",		CLpropSHADE_PATTERN, DOCspBGFDIAG ),
    RTF_CELL_ENUM( "clbghoriz",		CLpropSHADE_PATTERN, DOCspBGHORIZ ),
    RTF_CELL_ENUM( "clbgvert",		CLpropSHADE_PATTERN, DOCspBGVERT ),

    RTF_CELL_FLAG( "clmgf",		CLpropLEFT_IN_MERGED ),
    RTF_CELL_FLAG( "clmrg",		CLpropMERGED_WITH_LEFT ),
    RTF_CELL_FLAG( "clvmgf",		CLpropTOP_IN_MERGED ),
    RTF_CELL_FLAG( "clvmrg",		CLpropMERGED_WITH_ABOVE ),
    RTF_CELL_FLAG( "clNoWrap",		CLpropNO_WRAP ),

    RTF_CELL_VALUE( "clwWidth",		CLpropCLW_WIDTH ),
    RTF_CELL_VALUE( "clftsWidth",	CLpropCLFTS_WIDTH ),

    RTF_CELL_VALUE( "clpadt",		CLpropTOP_PADDING ),
    RTF_CELL_VALUE( "clpadb",		CLpropBOTTOM_PADDING ),
    RTF_CELL_VALUE( "clpadl",		CLpropLEFT_PADDING ),
    RTF_CELL_VALUE( "clpadr",		CLpropRIGHT_PADDING ),

    RTF_CELL_VALUE( "clpadft",		CLpropTOP_PADDING_UNIT ),
    RTF_CELL_VALUE( "clpadfb",		CLpropBOTTOM_PADDING_UNIT ),
    RTF_CELL_VALUE( "clpadfl",		CLpropLEFT_PADDING_UNIT ),
    RTF_CELL_VALUE( "clpadfr",		CLpropRIGHT_PADDING_UNIT ),

				/****************************************/
				/*  Row properties.			*/
				/****************************************/

    RTF_ROW_DEFAULT( "trowd" ),

    RTF_ROW_VALUE( "trgaph",		RPpropGAP_WIDTH ),
    RTF_ROW_VALUE( "trleft",		RPpropLEFT_INDENT ),
    RTF_ROW_VALUE( "trrh",		RPpropHEIGHT ),

    RTF_ROW_FLAG( "trhdr",		RPpropIS_TABLE_HEADER ),
    RTF_ROW_FLAG( "trkeep",		RPpropKEEP_ON_ONE_PAGE ),
    RTF_ROW_FLAG( "trautofit",		RPpropAUTOFIT ),

    RTF_ROW_ENUM( "trql",		RPpropALIGNMENT,	DOCiaLEFT ),
    RTF_ROW_ENUM( "trqr",		RPpropALIGNMENT,	DOCiaRIGHT ),
    RTF_ROW_ENUM( "trqc",		RPpropALIGNMENT,	DOCiaCENTERED ),

    /**/
    RTF_ROW_BORDER( "trbrdrt",		RPpropTOP_BORDER ),
    RTF_ROW_BORDER( "trbrdrb",		RPpropBOTTOM_BORDER ),
    RTF_ROW_BORDER( "trbrdrl",		RPpropLEFT_BORDER ),
    RTF_ROW_BORDER( "trbrdrr",		RPpropRIGHT_BORDER ),
    RTF_ROW_BORDER( "trbrdrh",		RPpropHORIZ_BORDER ),
    RTF_ROW_BORDER( "trbrdrv",		RPpropVERT_BORDER ),

    /**/
    RTF_ROW_VALUE( "trwWidth",		RPpropTRW_WIDTH ),
    RTF_ROW_VALUE( "trftsWidth",	RPpropTRFTS_WIDTH ),

    /**/
    RTF_ROW_VALUE( "trspdl",		RPpropTRSPDL ),
    RTF_ROW_VALUE( "trspdr",		RPpropTRSPDR ),
    RTF_ROW_VALUE( "trspdt",		RPpropTRSPDT ),
    RTF_ROW_VALUE( "trspdb",		RPpropTRSPDB ),

    RTF_ROW_VALUE( "trspdfl",		RPpropTRSPDFL ),
    RTF_ROW_VALUE( "trspdfr",		RPpropTRSPDFR ),
    RTF_ROW_VALUE( "trspdft",		RPpropTRSPDFT ),
    RTF_ROW_VALUE( "trspdfb",		RPpropTRSPDFB ),

    /**/
    RTF_ROW_VALUE( "trpaddl",		RPpropTRPADDL ),
    RTF_ROW_VALUE( "trpaddr",		RPpropTRPADDR ),
    RTF_ROW_VALUE( "trpaddt",		RPpropTRPADDT ),
    RTF_ROW_VALUE( "trpaddb",		RPpropTRPADDB ),

    RTF_ROW_VALUE( "trpaddfl",		RPpropTRPADDFL ),
    RTF_ROW_VALUE( "trpaddfr",		RPpropTRPADDFR ),
    RTF_ROW_VALUE( "trpaddft",		RPpropTRPADDFT ),
    RTF_ROW_VALUE( "trpaddfb",		RPpropTRPADDFB ),

    /**/
    RTF_ROW_VALUE( "trwWidthB",		RPpropTRW_WIDTHB ),
    RTF_ROW_VALUE( "trwWidthA",		RPpropTRW_WIDTHA ),

    RTF_ROW_VALUE( "trftsWidthB",	RPpropTRFTS_WIDTHB ),
    RTF_ROW_VALUE( "trftsWidthA",	RPpropTRFTS_WIDTHA ),

				/****************************************/
				/*  Document formatting.		*/
				/****************************************/

    RTF_REMEMBER( "pgbrdrhead",		RTFidPGBRDRHEAD ),
    RTF_REMEMBER( "pgbrdrfoot",		RTFidPGBRDRFOOT ),
    RTF_REMEMBER( "pgbrdrt",		RTFidPGBRDRT ),
    RTF_REMEMBER( "pgbrdrb",		RTFidPGBRDRB ),
    RTF_REMEMBER( "pgbrdrl",		RTFidPGBRDRL ),
    RTF_REMEMBER( "pgbrdrr",		RTFidPGBRDRR ),
    RTF_REMEMBER( "brdrart",		RTFidBRDRART ),
    RTF_REMEMBER( "pgbrdropt",		RTFidPGBRDROPT ),
    RTF_REMEMBER( "pgbrdrsnap",		RTFidPGBRDRSNAP ),

    RTF_DOC_VALUE( "margl",		DGpropLEFT_MARGIN ),
    RTF_DOC_VALUE( "margr",		DGpropRIGHT_MARGIN ),
    RTF_DOC_VALUE( "margt",		DGpropTOP_MARGIN ),
    RTF_DOC_VALUE( "margb",		DGpropBOTTOM_MARGIN ),
    RTF_DOC_VALUE( "paperh",		DGpropPAGE_HEIGHT ),
    RTF_DOC_VALUE( "paperw",		DGpropPAGE_WIDTH ),
    RTF_DOC_VALUE( "gutter",		DGpropGUTTER ),

    RTF_DOC_VALUE( "widowctrl",		DPpropWIDOWCTRL ),

    RTF_REMEMBER( "psz",		RTFidPSZ ),

    RTF_REMEMBER( "jcompress",		RTFidJCOMPRESS),
    RTF_REMEMBER( "jexpand",		RTFidJEXPAND),

    RTF_REMEMBER( "horzdoc",		RTFidHORZDOC),
    RTF_REMEMBER( "dghspace",		RTFidDGHSPACE),
    RTF_REMEMBER( "dgvspace",		RTFidDGVSPACE),
    RTF_REMEMBER( "dghorigin",		RTFidDGHORIGIN),
    RTF_REMEMBER( "dgvorigin",		RTFidDGVORIGIN),
    RTF_REMEMBER( "dghshow",		RTFidDGHSHOW),
    RTF_REMEMBER( "dgvshow",		RTFidDGVSHOW),
    RTF_REMEMBER( "dgmargin",		RTFidDGMARGIN),

    RTF_DOC_VALUE( "pgnstart",		DPpropSTART_PAGE ),
    RTF_DOC_VALUE( "deff",		DPpropDEFF ),
    RTF_DOC_VALUE( "facingp",		DPpropFACING_PAGES ),
    RTF_DOC_VALUE( "deftab",		DPpropDEFTAB ),

    RTF_REMEMBER( "truncex",		RTFidTRUNCEX ),
    RTF_REMEMBER( "nolead",		RTFidNOLEAD ),
    RTF_REMEMBER( "msmcap",		RTFidMSMCAP ),
    RTF_REMEMBER( "splytwnine",		RTFidSPLYTWNINE ),
    RTF_REMEMBER( "ftnlytwnine",	RTFidFTNLYTWNINE ),
    RTF_REMEMBER( "htmautsp",		RTFidHTMAUTSP ),
    RTF_REMEMBER( "useltbaln",		RTFidUSELTBALN ),
    RTF_REMEMBER( "alntblind",		RTFidALNTBLIND ),
    RTF_REMEMBER( "lytcalctblwd",	RTFidLYTCALCTBLWD ),
    RTF_REMEMBER( "lyttblrtgr",		RTFidLYTTBLRTGR ),
    RTF_REMEMBER( "lnbrkrule",		RTFidLNBRKRULE ),

    RTF_REMEMBER( "bdrrlswsix",		RTFidBDRRLSWSIX ),
    RTF_REMEMBER( "oldas",		RTFidOLDAS ),

    RTF_REMEMBER( "sprstsm",		RTFidSPRSTSM ), /* ignore */

				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    RTF_REMEMBER( "noxlattoyen",	RTFidNOXLATTOYEN ),
    RTF_REMEMBER( "expshrtn",		RTFidEXPSHRTN ),
    RTF_REMEMBER( "noultrlspc",		RTFidNOULTRLSPC ),
    RTF_REMEMBER( "dntblnsbdb",		RTFidDNTBLNSBDB ),
    RTF_REMEMBER( "nospaceforul",	RTFidNOSPACEFORUL ),

				/****************************************/
				/*  Paragraph Numbering.		*/
				/****************************************/
    RTF_REMEMBER( "pncard",		RTFidPNCARD ),
    RTF_REMEMBER( "pndec",		RTFidPNDEC ),
    RTF_REMEMBER( "pnucltr",		RTFidPNUCLTR ),
    RTF_REMEMBER( "pnucrm",		RTFidPNUCRM ),
    RTF_REMEMBER( "pnlcltr",		RTFidPNLCLTR ),
    RTF_REMEMBER( "pnlcrm",		RTFidPNLCRM ),
    RTF_REMEMBER( "pnord",		RTFidPNORD ),
    RTF_REMEMBER( "pnordt",		RTFidPNORDT ),

    RTF_REMEMBER( "pnstart",		RTFidPNSTART ),
    RTF_REMEMBER( "pnindent",		RTFidPNINDENT ),
    RTF_REMEMBER( "pnhang",		RTFidPNHANG ),

    RTF_REMEMBER( "pnuld",		RTFidPNULD ),
    RTF_REMEMBER( "pnuldb",		RTFidPNULDB ),
    RTF_REMEMBER( "pnulnone",		RTFidPNULNONE ),

				/****************************************/
				/*  Section properties.			*/
				/****************************************/
    RTF_SECT_DEFAULT( "sectd" ),

    RTF_SECT_VALUE( "headery",		DGpropHEADER_POSITION ),
    RTF_SECT_VALUE( "footery",		DGpropFOOTER_POSITION ),
    RTF_SECT_VALUE( "pgwsxn",		DGpropPAGE_WIDTH ),
    RTF_SECT_VALUE( "pghsxn",		DGpropPAGE_HEIGHT ),
    RTF_SECT_VALUE( "marglsxn",		DGpropLEFT_MARGIN ),
    RTF_SECT_VALUE( "margtsxn",		DGpropTOP_MARGIN ),
    RTF_SECT_VALUE( "margrsxn",		DGpropRIGHT_MARGIN ),
    RTF_SECT_VALUE( "margbsxn",		DGpropBOTTOM_MARGIN ),
    RTF_SECT_VALUE( "guttersxn",	DGpropGUTTER ),

    RTF_SECT_VALUE( "ds",		SPpropSTYLE ),

    RTF_SECT_FLAG( "titlepg",		SPpropTITLEPG ),

    RTF_SECT_ENUM( "sbknone",		SPpropBREAK_KIND, DOCsbkNONE ),
    RTF_SECT_ENUM( "sbkcol",		SPpropBREAK_KIND, DOCsbkCOL ),
    RTF_SECT_ENUM( "sbkpage",		SPpropBREAK_KIND, DOCsbkPAGE ),
    RTF_SECT_ENUM( "sbkeven",		SPpropBREAK_KIND, DOCsbkEVEN ),
    RTF_SECT_ENUM( "sbkodd",		SPpropBREAK_KIND, DOCsbkODD ),

    RTF_SECT_ENUM( "pgndec",		SPpropNUMBER_STYLE, DOCpgnDEC ),
    RTF_SECT_ENUM( "pgnucrm",		SPpropNUMBER_STYLE, DOCpgnUCRM ),
    RTF_SECT_ENUM( "pgnlcrm",		SPpropNUMBER_STYLE, DOCpgnLCRM ),
    RTF_SECT_ENUM( "pgnucltr",		SPpropNUMBER_STYLE, DOCpgnUCLTR ),
    RTF_SECT_ENUM( "pgnlcltr",		SPpropNUMBER_STYLE, DOCpgnLCLTR ),

    RTF_SECT_ENUM( "pgncont",		SPpropPAGE_RESTART, 0 ),
    RTF_SECT_ENUM( "pgnrestart",	SPpropPAGE_RESTART, 1 ),

    RTF_SECT_VALUE( "pgnstarts",	SPpropSTART_PAGE ),

    RTF_SECT_VALUE( "cols",		SPpropCOLUMN_COUNT ),
    RTF_SECT_VALUE( "colsx",		SPpropCOLUMN_SPACING ),
    RTF_SECT_VALUE( "colw",		SPpropCOLUMN_WIDTH ),

    /**/

    RTF_REMEMBER( "sectdefaultcl",	RTFidSECTDEFAULTCL ),

    RTF_REMEMBER( "pgnhn",		RTFidPGNHN ),
    RTF_REMEMBER( "pgnhnsh",		RTFidPGNHNSH ),
    RTF_REMEMBER( "pgnhnsp",		RTFidPGNHNSP ),
    RTF_REMEMBER( "pgnhnsc",		RTFidPGNHNSC ),
    RTF_REMEMBER( "pgnhnsm",		RTFidPGNHNSM ),
    RTF_REMEMBER( "pgnhnsn",		RTFidPGNHNSN ),

    RTF_REMEMBER( "pgnx",		RTFidPGNX ),
    RTF_REMEMBER( "pgny",		RTFidPGNY ),

    RTF_REMEMBER( "linex",		RTFidLINEX ),

    RTF_REMEMBER( "colno",		RTFidCOLNO ),
    RTF_REMEMBER( "colsr",		RTFidCOLSR ),

    RTF_REMEMBER( "vertalb",		RTFidVERTALB ),
    RTF_REMEMBER( "vertalc",		RTFidVERTALC ),
    RTF_REMEMBER( "vertalj",		RTFidVERTALJ ),
    RTF_REMEMBER( "vertalt",		RTFidVERTALT ),

    RTF_REMEMBER( "sectlinegrid",	RTFidSECTLINEGRID ),

    RTF_REMEMBER( "endnhere",		RTFidENDNHERE ),
    RTF_REMEMBER( "binfsxn",		RTFidBINFSXN ),
    RTF_REMEMBER( "binsxn",		RTFidBINSXN ),
    RTF_REMEMBER( "linebetcol",		RTFidLINEBETCOL ),
    RTF_REMEMBER( "linecont",		RTFidLINECONT ),
    RTF_REMEMBER( "linemod",		RTFidLINEMOD ),
    RTF_REMEMBER( "lineppage",		RTFidLINEPPAGE ),
    RTF_REMEMBER( "linerestart",	RTFidLINERESTART ),
    RTF_REMEMBER( "linestarts",		RTFidLINESTARTS ),
    RTF_REMEMBER( "lndscpsxn",		RTFidLNDSCPSXN ),
    RTF_REMEMBER( "ltrsect",		RTFidLTRSECT ),
    RTF_REMEMBER( "rtlsect",		RTFidRTLSECT ),
    RTF_REMEMBER( "sectunlocked",	RTFidSECTUNLOCKED ),

				/****************************************/
				/*  Document properties: (Footnotes)	*/
				/****************************************/
    RTF_DOC_VALUE( "fet",			DPpropNOTETYPES ),

    RTF_DOC_ENUM( "endnotes",		DPpropFOOTNOTE_POSITION,
						    DPftnPOS_SECT_END ),
    RTF_DOC_ENUM( "enddoc",		DPpropFOOTNOTE_POSITION,
						    DPftnPOS_DOC_END ),
    RTF_DOC_ENUM( "ftntj",		DPpropFOOTNOTE_POSITION,
						    DPftnPOS_BELOW_TEXT ),
    RTF_DOC_ENUM( "ftnbj",		DPpropFOOTNOTE_POSITION,
						    DPftnPOS_PAGE_BOTTOM ),

    RTF_DOC_ENUM( "aendnotes",		DPpropENDNOTE_POSITION,
						    DPftnPOS_SECT_END ),
    RTF_DOC_ENUM( "aenddoc",		DPpropENDNOTE_POSITION,
						    DPftnPOS_DOC_END ),
    RTF_DOC_ENUM( "aftntj",		DPpropENDNOTE_POSITION,
						    DPftnPOS_BELOW_TEXT ),
    RTF_DOC_ENUM( "aftnbj",		DPpropENDNOTE_POSITION,
						    DPftnPOS_PAGE_BOTTOM ),

    RTF_DOC_VALUE( "ftnstart",		DPpropFOOTNOTE_STARTNR ),
    RTF_DOC_VALUE( "aftnstart",		DPpropENDNOTE_STARTNR ),

    RTF_DOC_ENUM( "ftnrstcont",		DPpropFOOTNOTE_RESTART,
						    DPftnRST_CONTINUOUS ),
    RTF_DOC_ENUM( "ftnrestart",		DPpropFOOTNOTE_RESTART,
						    DPftnRST_PER_SECTION ),
    RTF_DOC_ENUM( "ftnrstpg",		DPpropFOOTNOTE_RESTART,
						    DPftnRST_PER_PAGE ),

    RTF_DOC_ENUM( "aftnrstcont",	DPpropENDNOTE_RESTART,
						    DPftnRST_CONTINUOUS ),
    RTF_DOC_ENUM( "aftnrestart",	DPpropENDNOTE_RESTART,
						    DPftnRST_PER_SECTION ),
    RTF_DOC_ENUM( "aftnrstpg",		DPpropENDNOTE_RESTART,
						    DPftnRST_PER_PAGE ),

    RTF_DOC_ENUM( "ftnnar",		DPpropFOOTNOTE_STYLE, DPftnNAR ),
    RTF_DOC_ENUM( "ftnnalc",		DPpropFOOTNOTE_STYLE, DPftnNALC ),
    RTF_DOC_ENUM( "ftnnauc",		DPpropFOOTNOTE_STYLE, DPftnNAUC ),
    RTF_DOC_ENUM( "ftnnrlc",		DPpropFOOTNOTE_STYLE, DPftnNRLC ),
    RTF_DOC_ENUM( "ftnnruc",		DPpropFOOTNOTE_STYLE, DPftnNRUC ),
    RTF_DOC_ENUM( "ftnnchi",		DPpropFOOTNOTE_STYLE, DPftnNCHI ),

    RTF_DOC_ENUM( "aftnnar",		DPpropENDNOTE_STYLE, DPftnNAR ),
    RTF_DOC_ENUM( "aftnnalc",		DPpropENDNOTE_STYLE, DPftnNALC ),
    RTF_DOC_ENUM( "aftnnauc",		DPpropENDNOTE_STYLE, DPftnNAUC ),
    RTF_DOC_ENUM( "aftnnrlc",		DPpropENDNOTE_STYLE, DPftnNRLC ),
    RTF_DOC_ENUM( "aftnnruc",		DPpropENDNOTE_STYLE, DPftnNRUC ),
    RTF_DOC_ENUM( "aftnnchi",		DPpropENDNOTE_STYLE, DPftnNCHI ),

    RTF_REMEMBER( "ftnalt",		RTFidFTNALT ),

				/****************************************/
				/*  StyleSheet.				*/
				/****************************************/
    RTF_STYLE_FLAG( "additive",		DSpropADDITIVE ),
    RTF_STYLE_VALUE( "sbasedon",	DSpropBASED_ON ),
    RTF_STYLE_VALUE( "snext",		DSpropNEXT ),
					/*  RTFidS	*/
					/*  RTFidDS	*/
    /*
    RTF_STYLE_VAL_ENUM( "ds",		DSpropSTYLE_NUMBER, DOClevSECT ),
    RTF_STYLE_VAL_ENUM( "s",		DSpropSTYLE_NUMBER, DOClevPARA ),
    */
    RTF_STYLE_VAL_ENUM( "cs",		DSpropSTYLE_NUMBER, DOClevTEXT ),
    RTF_STYLE_FLAG( "sautoupd",		DSpropAUTO_UPD ),
    RTF_STYLE_FLAG( "shidden",		DSpropHIDDEN ),
    RTF_STYLE_FLAG( "spersonal",	DSpropPERSONAL ),

				/****************************************/
				/*  Encoding.				*/
				/****************************************/
    RTF_DOC_ENUM( "ansi",		DPpropDOC_CHARSET, DOCcharsetANSI ),
    RTF_DOC_ENUM( "mac",		DPpropDOC_CHARSET, DOCcharsetMAC ),
    RTF_DOC_ENUM( "pc",			DPpropDOC_CHARSET, DOCcharsetPC ),
    RTF_DOC_ENUM( "pca",		DPpropDOC_CHARSET, DOCcharsetPCA ),
    RTF_DOC_VALUE( "ansicpg",		DPpropANSICPG ),

				/****************************************/
				/*  Unknown				*/
				/****************************************/
    RTF_REMEMBER( "itap",		RTFidITAP ),
    RTF_REMEMBER( "lin",		RTFidLIN ),
    RTF_REMEMBER( "rin",		RTFidRIN ),
    RTF_REMEMBER( "langnp",		RTFidLANGNP ),
    RTF_REMEMBER( "langfe",		RTFidLANGFE ),
    RTF_REMEMBER( "langfenp",		RTFidLANGFENP ),
    RTF_REMEMBER( "nolnhtadjtbl",	RTFidNOLNHTADJTBL ),

				/****************************************/
				/*  Document Formatting properties.	*/
				/****************************************/
    RTF_REMEMBER( "allprot",		RTFidALLPROT ),
    RTF_REMEMBER( "annotprot",		RTFidANNOTPROT ),
    RTF_REMEMBER( "brkfrm",		RTFidBRKFRM ),
    RTF_REMEMBER( "cvmme",		RTFidCVMME ),
    RTF_REMEMBER( "defformat",		RTFidDEFFORMAT ),
    RTF_REMEMBER( "deflang",		RTFidDEFLANG ),
    RTF_REMEMBER( "doctemp",		RTFidDOCTEMP ),
    RTF_REMEMBER( "fldalt",		RTFidFLDALT ),
    RTF_REMEMBER( "formdisp",		RTFidFORMDISP ),
    RTF_REMEMBER( "formprot",		RTFidFORMPROT ),
    RTF_REMEMBER( "formshade",		RTFidFORMSHADE ),
    RTF_REMEMBER( "fracwidth",		RTFidFRACWIDTH ),
    RTF_REMEMBER( "hyphauto",		RTFidHYPHAUTO ),
    RTF_REMEMBER( "hyphcaps",		RTFidHYPHCAPS ),
    RTF_REMEMBER( "hyphconsec",		RTFidHYPHCONSEC ),
    RTF_REMEMBER( "hyphhotz",		RTFidHYPHHOTZ ),
    RTF_REMEMBER( "landscape",		RTFidLANDSCAPE ),
    RTF_REMEMBER( "linestart",		RTFidLINESTART ),
    RTF_REMEMBER( "linkstyles",		RTFidLINKSTYLES ),
    RTF_REMEMBER( "ltrdoc",		RTFidLTRDOC ),
    RTF_REMEMBER( "makebackup",		RTFidMAKEBACKUP ),
    RTF_REMEMBER( "margmirror",		RTFidMARGMIRROR ),
    RTF_REMEMBER( "nocolbal",		RTFidNOCOLBAL ),
    RTF_REMEMBER( "noextrasprl",	RTFidNOEXTRASPRL ),
    RTF_REMEMBER( "notabind",		RTFidNOTABIND ),
    RTF_REMEMBER( "otblrul",		RTFidOTBLRUL ),
    RTF_REMEMBER( "prcolbl",		RTFidPRCOLBL ),
    RTF_REMEMBER( "printdata",		RTFidPRINTDATA ),
    RTF_REMEMBER( "psover",		RTFidPSOVER ),
    RTF_REMEMBER( "revbar",		RTFidREVBAR ),
    RTF_REMEMBER( "revisions",		RTFidREVISIONS ),
    RTF_REMEMBER( "revprop",		RTFidREVPROP ),
    RTF_REMEMBER( "revprot",		RTFidREVPROT ),
    RTF_REMEMBER( "rtldoc",		RTFidRTLDOC ),
    RTF_REMEMBER( "sprslnsp",		RTFidSPRSLNSP ),
    RTF_REMEMBER( "sprsspbf",		RTFidSPRSSPBF ),
    RTF_REMEMBER( "sprstsp",		RTFidSPRSTSP ),
    RTF_REMEMBER( "subfontbysize",	RTFidSUBFONTBYSIZE ),
    RTF_REMEMBER( "swpbdr",		RTFidSWPBDR ),
    RTF_REMEMBER( "transmf",		RTFidTRANSMF ),
    RTF_REMEMBER( "ight",		RTFidIGHT ),
    RTF_REMEMBER( "wraptrsp",		RTFidWRAPTRSP ),
    RTF_REMEMBER( "viewzk",		RTFidVIEWZK ),
    RTF_REMEMBER( "viewkind",		RTFidVIEWKIND ),
    RTF_REMEMBER( "viewscale",		RTFidVIEWSCALE ),
    RTF_REMEMBER( "lytprtmet",		RTFidLYTPRTMET ),
    RTF_REMEMBER( "ogutter",		RTFidOGUTTER ),
    RTF_REMEMBER( "deflangfe",		RTFidDEFLANGFE ),
    RTF_REMEMBER( "uc",			RTFidUC ),
    RTF_REMEMBER( "level",		RTFidLEVEL ),
    RTF_REMEMBER( "ltrpar",		RTFidLTRPAR ),
    RTF_REMEMBER( "noline",		RTFidNOLINE ),
    RTF_REMEMBER( "rtlpar",		RTFidRTLPAR ),
    RTF_REMEMBER( "sbys",		RTFidSBYS ),
    RTF_REMEMBER( "subdocument",	RTFidSUBDOCUMENT ),

				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
    RTF_REMEMBER( "hr",			RTFidHR ),
    RTF_REMEMBER( "min",		RTFidMIN ),
    RTF_REMEMBER( "sec",		RTFidSEC ),
    RTF_REMEMBER( "dy",			RTFidDY ),
    RTF_REMEMBER( "mo",			RTFidMO ),
    RTF_REMEMBER( "yr",			RTFidYR ),

				/****************************************/
				/*  Unsupported W2002 revision		*/
				/*  control.				*/
				/****************************************/
    RTF_REMEMBER( "insrsid",		RTFidRSID ),
    RTF_REMEMBER( "rsidroot",		RTFidRSID ),
    RTF_REMEMBER( "delrsid",		RTFidRSID ),
    RTF_REMEMBER( "charrsid",		RTFidRSID ),
    RTF_REMEMBER( "sectrsid",		RTFidRSID ),
    RTF_REMEMBER( "pararsid",		RTFidRSID ),
    RTF_REMEMBER( "tblrsid",		RTFidRSID ),
    RTF_REMEMBER( "styrsid",		RTFidRSID ),

				/****************************************/
				/*  List table &c.			*/
				/****************************************/
    RTF_LISTLEV_VALUE( "levelstartat",		LLpropSTARTAT ),
    RTF_LISTLEV_VALUE( "levelnfcn",		LLpropSTYLE ),
    RTF_LISTLEV_VALUE( "levelnfc",		LLpropSTYLE ),		/**/
    RTF_LISTLEV_VALUE( "leveljcn",		LLpropJUSTIFY ),
    RTF_LISTLEV_VALUE( "leveljc",		LLpropJUSTIFY ),	/**/
    RTF_LISTLEV_VALUE( "levelfollow",		LLpropFOLLOW ),
    RTF_LISTLEV_VALUE( "levellegal",		LLpropLEGAL ),
    RTF_LISTLEV_VALUE( "levelnorestart",	LLpropNORESTART ),
    RTF_LISTLEV_VALUE( "levelpicture",		LLpropPICTURE ),
    RTF_LISTLEV_VALUE( "fbias",			LLpropFBIAS ),

    RTF_LISTLEV_VALUE( "leveltemplateid",	LLpropTEMPLATEID ),

    RTF_LISTLEV_VALUE( "levelold",		LLpropOLD ),		/**/
    RTF_LISTLEV_VALUE( "levelprev",		LLpropPREV ),		/**/
    RTF_LISTLEV_VALUE( "levelprevspace",	LLpropPREVSPACE ),	/**/
    RTF_LISTLEV_VALUE( "levelindent",		LLpropINDENT ),		/**/
    RTF_LISTLEV_VALUE( "levelspace",		LLpropSPACE ),		/**/

    RTF_LIST_VALUE( "listid",			DLpropLISTID ),
    RTF_LIST_VALUE( "listtemplateid",		DLpropTEMPLATEID ),
    RTF_LIST_ENUM( "listsimple",		DLpropHYBRID, 0 ),
    RTF_LIST_ENUM( "listhybrid",		DLpropHYBRID, 1 ),
    RTF_LIST_FLAG( "listrestarthdn",		DLpropRESTARTHDN ),
    RTF_LIST_VALUE( "liststyleid",		DLpropSTYLEID ),

    RTF_LISTOVERR_VALUE( "listoverridecount",	LOpropOVERRIDECOUNT ),
    /* "listid" ->	LOpropLISTID used via list */
    /* "ls" ->		LOpropINDEX  used via paragraph properties */

    RTF_LISTOVERRLEV_FLAG( "listoverridestartat",	LOLpropSTARTAT ),
    RTF_LISTOVERRLEV_FLAG( "listoverrideformat",	LOLpropFORMAT ),

				/****************************************/
				/*  Tab stops.				*/
				/*					*/
				/*  "tql" is not part of the rtf spec	*/
				/*  but it can be found in some		*/
				/*  documents.				*/
				/*					*/
				/*  "tx" is handeled differently.	*/
				/*					*/
				/****************************************/

    RTF_TAB_ENUM( "tql",	TABpropALIGN,	DOCtaLEFT ),
    RTF_TAB_ENUM( "tqr",	TABpropALIGN,	DOCtaRIGHT ),
    RTF_TAB_ENUM( "tqc",	TABpropALIGN,	DOCtaCENTRE ),
    RTF_TAB_ENUM( "tqdec",	TABpropALIGN,	DOCtaDECIMAL ),

    RTF_TAB_ENUM( "tldot",	TABpropLEADER,	DOCtlDOTS ),
    RTF_TAB_ENUM( "tlul",	TABpropLEADER,	DOCtlUNDERLINE ),
    RTF_TAB_ENUM( "tlhyph",	TABpropLEADER,	DOCtlHYPH ),
    RTF_TAB_ENUM( "tlth",	TABpropLEADER,	DOCtlTHICK ),
    RTF_TAB_ENUM( "tleq",	TABpropLEADER,	DOCtlEQUAL ),

    RTF_TAB_FLAG( "jclisttab",	TABpropFROM_STYLE ),

				/****************************************/
				/*  Font table.				*/
				/****************************************/
    RTF_FONT_ENUM( "fnil",	DFpropFAMILY_STYLE,	DFstyleFNIL ),
    RTF_FONT_ENUM( "froman",	DFpropFAMILY_STYLE,	DFstyleFROMAN ),
    RTF_FONT_ENUM( "fswiss",	DFpropFAMILY_STYLE,	DFstyleFSWISS ),
    RTF_FONT_ENUM( "fmodern",	DFpropFAMILY_STYLE,	DFstyleFMODERN ),
    RTF_FONT_ENUM( "fscript",	DFpropFAMILY_STYLE,	DFstyleFSCRIPT ),
    RTF_FONT_ENUM( "fdecor",	DFpropFAMILY_STYLE,	DFstyleFDECOR ),
    RTF_FONT_ENUM( "ftech",	DFpropFAMILY_STYLE,	DFstyleFTECH ),

    RTF_FONT_VALUE( "fcharset",	DFpropCHARSET ),
    RTF_FONT_VALUE( "cpg",	DFpropCODEPAGE ),
    RTF_FONT_VALUE( "fprq",	DFpropPITCH ),

				/****************************************/
				/*  Pictures/Objects			*/
				/****************************************/

    RTF_OBJ_ENUM( "objemb",	IOpropKIND,	DOCokOLEOBJECT ),
    RTF_OBJ_ENUM( "macpict",	IOpropKIND,	DOCokMACPICT ),
    RTF_OBJ_ENUM( "wmetafile",	IOpropKIND,	DOCokPICTWMETAFILE ),
    RTF_OBJ_ENUM( "pngblip",	IOpropKIND,	DOCokPICTPNGBLIP ),
    RTF_OBJ_ENUM( "jpegblip",	IOpropKIND,	DOCokPICTJPEGBLIP ),
    RTF_OBJ_ENUM( "emfblip",	IOpropKIND,	DOCokPICTEMFBLIP ),
    RTF_OBJ_ENUM( "dibitmap",	IOpropKIND,	DOCokDIBITMAP ),
    RTF_OBJ_ENUM( "wbitmap",	IOpropKIND,	DOCokWBITMAP ),
    RTF_OBJ_ENUM( "pmmetafile",	IOpropKIND,	DOCokPMMETAFILE ),

    RTF_OBJ_VALUE( "objw",	IOpropOBJTWIPS_WIDE ),
    RTF_OBJ_VALUE( "objh",	IOpropOBJTWIPS_HIGH ),

    RTF_OBJ_VALUE( "picwgoal",	IOpropPICTWIPS_WIDE ),
    RTF_OBJ_VALUE( "pichgoal",	IOpropPICTWIPS_HIGH ),

    RTF_OBJ_VALUE( "objscalex",	IOpropOBJSCALE_X ),
    RTF_OBJ_VALUE( "objscaley",	IOpropOBJSCALE_Y ),

    RTF_OBJ_VALUE( "picscalex",	IOpropPICSCALE_X ),
    RTF_OBJ_VALUE( "picscaley",	IOpropPICSCALE_Y ),

    RTF_OBJ_VALUE( "picw",	IOpropPICX_WIN_EXT ),
    RTF_OBJ_VALUE( "pich",	IOpropPICY_WIN_EXT ),

    RTF_OBJ_VALUE( "objcropt",	IOpropOBJCROP_TOP ),
    RTF_OBJ_VALUE( "objcropb",	IOpropOBJCROP_BOTTOM ),
    RTF_OBJ_VALUE( "objcropl",	IOpropOBJCROP_LEFT ),
    RTF_OBJ_VALUE( "objcropr",	IOpropOBJCROP_RIGHT ),

    RTF_OBJ_VALUE( "piccropt",	IOpropPICCROP_TOP ),
    RTF_OBJ_VALUE( "piccropb",	IOpropPICCROP_BOTTOM ),
    RTF_OBJ_VALUE( "piccropl",	IOpropPICCROP_LEFT ),
    RTF_OBJ_VALUE( "piccropr",	IOpropPICCROP_RIGHT ),

    RTF_OBJ_VALUE( "bliptag",	IOpropBLIPTAG ),

    RTF_OBJ_VALUE( "wbmbitspixel",	IOpropWBMBITSPIXEL ),
    RTF_OBJ_VALUE( "wbmplanes",		IOpropWBMPLANES ),
    RTF_OBJ_VALUE( "wbmwidthbytes",	IOpropWBMWIDTHBYTES ),

    RTF_OBJ_FLAG( "picbmp",		IOpropPICBMP ),
    RTF_OBJ_VALUE( "picbpp",		IOpropPICBPP ),

    RTF_OBJ_FLAG( "pictscaled",		IOprop_UNSUPPORTED ),
    RTF_OBJ_VALUE( "blipupi",		IOprop_UNSUPPORTED ),
    RTF_OBJ_VALUE( "objsetsize",	IOprop_UNSUPPORTED ),
    RTF_OBJ_VALUE( "rsltmerge",		IOprop_UNSUPPORTED ),

				/****************************************/
				/*  End.				*/
				/****************************************/
    { 0, 0, 0 }
};

static int docRtfMakeTagHash( void )
    {
    int			rval= 0;
    int			i;

    const unsigned char **	keyStrings= (const unsigned char **)0;
    int				keyStringCount;

    keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

    keyStrings= malloc( keyStringCount* sizeof(const unsigned char *) );
    if  ( ! keyStrings )
	{ LXDEB(keyStringCount,keyStrings); rval= -1; goto ready;	}

    for ( i= 0; i < keyStringCount; i++ )
	{
	keyStrings[i]= (const unsigned char *)docRtfPropertyWords[i].rcwWord;
	}

    if  ( utilJenkinsPerfectHash( keyStrings, keyStringCount,
						    "docRtfTagHash.c",
						    "\"docRtf.h\"",
						    "docRtfTagHash",
						    "docRtfTagIndex" ) )
	{ LDEB(keyStringCount); rval= 0;	}

  ready:

    if  ( keyStrings )
	{ free( keyStrings );	}

    return rval;
    }

#   ifndef	BUILD_HASH

static int docRtfCheckTagHash( void )
    {
    int			rval= 0;
    int			i;

    int			keyStringCount;

    keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

    for ( i= 0; i < keyStringCount; i++ )
	{
	const unsigned char *	key;
	int			len;
	int			idx;

	key= (const unsigned char *)docRtfPropertyWords[i].rcwWord;
	len= strlen( docRtfPropertyWords[i].rcwWord );

	idx= docRtfTagIndex( key, len );

	if  ( idx != i )
	    {
	    long hash= docRtfTagHash( key, len );

	    LSLLDEB(i,docRtfPropertyWords[i].rcwWord,idx,hash); rval= -1;
	    }
	}

    return rval;
    }

const RtfControlWord * docRtfFindPropertyWord(
					const char *	controlWord )
    {
    const RtfControlWord *	contolWords;

    static int			checked= 1;

    if  ( ! checked )
	{
	checked= 1;

	if  ( docRtfCheckTagHash() )
	    {
	    checked= -1;

	    LDEB(1);
	    if  ( docRtfMakeTagHash() )
		{ LDEB(1);	}
	    }
	}

    if  ( checked > 0 )
	{
	int			idx;
	int			keyStringCount;

	keyStringCount= sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

	idx= docRtfTagIndex( (const unsigned char *)controlWord,
							strlen( controlWord ) );

	if  ( idx >= 0 && idx < keyStringCount )
	    {
	    contolWords= docRtfPropertyWords+ idx;

	    if  ( ! strcmp( controlWord, contolWords->rcwWord ) )
		{ return contolWords;	}
	    }

	return (const RtfControlWord *)0;
	}
    else{
	contolWords= docRtfPropertyWords;

	while( contolWords->rcwWord )
	    {
	    if  ( ! strcmp( contolWords->rcwWord, controlWord ) )
		{ return contolWords;	}

	    contolWords++;
	    }

	return (const RtfControlWord *)0;
	}
    }

#   endif

#   ifdef	BUILD_HASH

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeTagHash() )
	    { LDEB(1); return 1;	}

	return 0;
	}

#   endif

