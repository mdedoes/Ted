/************************************************************************/
/*									*/
/*  Table of rtf tags that are not treated as an exception.		*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<charnames.h>
#   include	<textOfficeCharset.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfParaInsertType.h"
#   include	"docRtfTagHash.h"
#   include	"docRtfTraceImpl.h"
#   include	"docRtfTagEnum.h"
#   include	"docRtfTags.h"
#   include	"docRtfTagLookup.h"
#   include	"docRtfFindProperty.h"
#   include	"docRtfMakeLookup.h"
#   include	<docTreeType.h>
#   include	<docItemShading.h>
#   include	<docDrawingObject.h>
#   include	<docTextParticule.h>
#   include	<docObjectProperties.h>
#   include	<docShape.h>
#   include	<docShapeType.h>
#   include	<psShading.h>
#   include	<psDocumentFontStyle.h>
#   include	<docFieldKind.h>
#   include	<docFieldProperty.h>
#   include	<docTreeNode.h>
#   include	<docBreakKind.h>
#   include	<docPropVal.h>
#   include	<docTextFlow.h>
#   include	<utilDateTime.h>
#   include	<docEditPosition.h>

#   include	<appDebugon.h>

#   define	BUILD_NO_HASH				/*  for sed */
#   define	BUILD_NO_LOOKUP				/*  for sed */

# if defined(BUILD_HASH) || defined(BUILD_LOOKUP)

#	define		docRtfBrdrProperty			0
#	define		docRtfBeginBorder			0
#	define		docRtfRememberProperty			0
#	define		docRtfRememberDocProperty		0
#	define		docRtfRememberStyleProperty		0
#	define		docRtfRememberSectionProperty		0
#	define		docRtfRememberRowProperty		0
#	define		docRtfCommitRowText			0
#	define		docRtfGotCellX				0
#	define		docRtfRememberCellProperty		0
#	define		docRtfRememberCellInstanceProperty	0
#	define		docRtfRememberParagraphProperty		0
#	define		docRtfRememberTextProperty		0
#	define		docRtfRememberTextDirection		0
#	define		docRtfRememberListLevelProperty		0
#	define		docRtfRememberListProperty		0
#	define		docRtfRememberListOverrideProperty	0
#	define		docRtfRememberListOverrideLevelProperty	0
#	define		docRtfRememberTabStopProperty		0
#	define		docRtfRememberParaFrameProperty		0
#	define		docRtfRememberRowFrameProperty		0
#	define		docRtfFontProperty			0
#	define		docRtfCommitFontDest			0
#	define		docRtfObjectProperty			0
#	define		docRtfPictureProperty			0
#	define		docRtfTextSpecialChar			0
#	define		docRtfTextUnicode			0
#	define		docRtfTextSpecialParticule		0
#	define		docRtfBreakParticule			0
#	define		docRtfTextSpecialToField		0
#	define		docRtfDrawingObjectProperty		0
#	define		docRtfDrawingObjectCoordinate		0
#	define		docRtfShpProperty			0
#	define		docRtfLookupWord			0
#	define		docRtfColorComp				0
#	define		docRtfColorIgnored			0
#	define		docRtfHierarchy				0
#	define		docRtfRememberFieldProperty		0
#	define		docRtfRememberNoteProperty		0
#	define		docRtfIgnoreWord			0
#	define		docRtfReadTimeField			0

#	define		docRtfFontTable				0
#	define		docRtfColorTable			0
#	define		docRtfStylesheet			0
#	define		docRtfListTable				0
#	define		docRtfCommitListLevelText		0
#	define		docRtfCommitListLevelNumbers		0
#	define		docRtfCommitListName			0
#	define		docRtfCommitListStyleName		0
#	define		docRtfRememberList			0
#	define		docRtfRememberListLevel			0
#	define		docRtfApplyDocEncodedTextGroup		0
#	define		docRtfBkmkEnd				0
#	define		docRtfBkmkStart				0
#	define		docRtfListOverrideTable			0
#	define		docRtfLookupEntry			0
#	define		docRtfReadDrawingObject			0
#	define		docRtfReadExtTree			0
#	define		docRtfReadField				0
#	define		docRtfReadFldinst			0
#	define		docRtfReadFldrslt			0
#	define		docRtfReadNote				0
#	define		docRtfReadNeXTGraphic			0
#	define		docRtfNeXTDimension			0
#	define		docRtfReadInfo				0
#	define		docRtfReadObject			0
#	define		docRtfReadPict				0
#	define		docRtfReadRowProperties			0
#	define		docRtfReadShape				0
#	define		docRtfReadShpXPict			0
#	define		docRtfReadTextField			0
#	define		docRtfRevisionTable			0
#	define		docRtfShapeProperty			0
#	define		docRtfShapePropertyName			0
#	define		docRtfShapePropertyValue		0
#	define		docRtfSkipGroup				0

#	define		docRtfCommitDocPropText			0
#	define		docRtfDocTimeGroup			0

#	define		docRtfRememberTextShadingProperty	0
#	define		docRtfRememberParaShadingProperty	0
#	define		docRtfRememberCellShadingProperty	0
#	define		docRtfRememberRowShadingProperty	0

#	define		docRtfSelectionScopeProperty		0
#	define		docRtfEditRangeProperty			0
#	define		docRtfSelectionOpen			0

#	define		docRtfReadDataField			0
#	define		docRtfReadFormField			0
#	define		docRtfRememberFormFieldProperty		0
#	define		docRtfCommitFormFieldText		0

int docRtfApplyRawBytesGroup(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    { return 0;	}

#   endif

# define	RTF_BORDER_POSITIVE(s,sc,id,sp) \
		    { \
		    s, sc, id, RTCtypeBORDER_POSITIVE, sp, 0, \
		    docRtfBeginBorder, \
		    (RtfCommitGroup)0, \
		    docRtfBrdrProperty, \
		    }

# define	RTF_BORDER_ANYWAY(s,sc,id,sp) \
		    { \
		    s, sc, id, RTCtypeBORDER_ANYWAY, sp, 0, \
		    docRtfBeginBorder, \
		    (RtfCommitGroup)0, \
		    docRtfBrdrProperty, \
		    }

/************************************************************************/
/*									*/
/*  Table of common rtf tags.						*/
/*									*/
/************************************************************************/

# define	RTF_REMEMBER(s,id) \
		{ s, RTCscopeANY, id, RTCtypeANY, docRtfRememberProperty, }


static RtfControlWord	docRtfPropertyWords[]=
{
			/*
			 *  Hierarchy/Structure tags
			 */
# define		RTF_HIERARCHY(s,id) \
			    { \
			    s, RTCscopeHIERARCHY, id, RTCtypeANY, \
			    docRtfHierarchy, \
			    }

	RTF_HIERARCHY( RTFtag_par ,	DOClevPARA ),
	RTF_HIERARCHY( RTFtag_cell,	DOClevCELL ),
	RTF_HIERARCHY( RTFtag_row,	DOClevROW ),
	RTF_HIERARCHY( RTFtag_sect,	DOClevSECT ),
	RTF_HIERARCHY( "nestcell",	DOClevNESTCELL ), /**/
	RTF_HIERARCHY( "nestrow",	DOClevNESTROW ), /**/
	RTF_HIERARCHY( "nonesttables",	DOClevNONESTTABLES ), /**/

				/****************************************/
				/*  Text properties.			*/
				/*  Character Formatting Properties	*/
				/****************************************/

# define	RTF_TEXT_DEFAULT(s) \
		    { \
		    s, RTCscopeTEXT, TAprop_NONE, RTCtypeANY, \
		    docRtfRememberTextProperty, \
		    }
# define	RTF_TEXT_NUMBER(s,id) \
		    { \
		    s, RTCscopeTEXT, id, RTCtypeNUMBER, \
		    docRtfRememberTextProperty, \
		    }
# define	RTF_TEXT_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberTextProperty, \
		    }
# define	RTF_TEXT_ENUM(s,id,v) \
		    { \
		    s, RTCscopeTEXT, id, RTCtypeENUM, \
		    docRtfRememberTextProperty, v, \
		    }
# define	RTF_TEXT_FLAG(s,id) \
		    { \
		    s, RTCscopeTEXT, id, RTCtypeFLAG, \
		    docRtfRememberTextProperty, \
		    }
# define	RTF_TEXT_FLAG_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberTextProperty, \
		    }
# define	RTF_TEXT_DIR(s,v) \
		    { \
		    s, RTCscopeTEXT_DIR, 0, RTCtypeENUM, \
		    docRtfRememberTextDirection, v \
		    }

# define	RTF_TEXT_BORDER(s,id)  \
		    RTF_BORDER_ANYWAY(s,RTCscopeTEXT,id,docRtfRememberTextProperty)

# define	RTF_TEXT_SHD_ENUM(s,id,v) \
		    { \
		    s, RTCscopeTEXT_SHADING, id, RTCtypeENUM, \
		    docRtfRememberTextShadingProperty, v \
		    }
# define	RTF_TEXT_SHD_NUMBER(s,id) \
		    { \
		    s, RTCscopeTEXT_SHADING, id, RTCtypeNUMBER, \
		    docRtfRememberTextShadingProperty, \
		    }

    RTF_TEXT_DEFAULT( RTFtag_plain ),

    RTF_TEXT_NUMBER( "f",		TApropFONT_NUMBER ),
    RTF_TEXT_NUMBER( "fs",		TApropFONTSIZE ),
    RTF_TEXT_NUMBER( "cf",		TApropTEXT_COLOR ),
    RTF_TEXT_NUMBER_X( "fc",		TApropTEXT_COLOR ), /* typo NextStep */
    RTF_TEXT_NUMBER( "cs",		TApropTEXT_STYLE ),

    RTF_TEXT_NUMBER( "up",		TApropBASELINE_SHIFT ),
    RTF_TEXT_NUMBER( "dn",		TAprop_BASELINE_SHIFT_DOWN ),

    RTF_TEXT_BORDER( "chbrdr",		TApropBORDER ),

    RTF_TEXT_NUMBER( "lang",		TApropLOCALE ),
    RTF_TEXT_FLAG(   "noproof",		TApropNOPROOF ),

    RTF_TEXT_SHD_NUMBER( "chcfpat",	ISpropFORE_COLOR ),
    RTF_TEXT_SHD_NUMBER( "chcbpat",	ISpropBACK_COLOR ),
    RTF_TEXT_SHD_NUMBER( "chshdng",	ISpropLEVEL ),
    RTF_TEXT_SHD_NUMBER( "cb",		ISpropBACK_SOLID ),

    RTF_TEXT_SHD_ENUM( "chbghoriz",	ISpropPATTERN, PSshdHORIZ ),
    RTF_TEXT_SHD_ENUM( "chbgvert",	ISpropPATTERN, PSshdVERT ),
    RTF_TEXT_SHD_ENUM( "chbgfdiag",	ISpropPATTERN, PSshdFDIAG ),
    RTF_TEXT_SHD_ENUM( "chbgbdiag",	ISpropPATTERN, PSshdBDIAG ),
    RTF_TEXT_SHD_ENUM( "chbgcross",	ISpropPATTERN, PSshdCROSS ),
    RTF_TEXT_SHD_ENUM( "chbgdcross",	ISpropPATTERN, PSshdDCROSS ),
    RTF_TEXT_SHD_ENUM( "chbgdkhoriz",	ISpropPATTERN, PSshdDKHORIZ ),
    RTF_TEXT_SHD_ENUM( "chbgdkvert",	ISpropPATTERN, PSshdDKVERT ),
    RTF_TEXT_SHD_ENUM( "chbgdkfdiag",	ISpropPATTERN, PSshdDKFDIAG ),
    RTF_TEXT_SHD_ENUM( "chbgdkbdiag",	ISpropPATTERN, PSshdDKBDIAG ),
    RTF_TEXT_SHD_ENUM( "chbgdkcross",	ISpropPATTERN, PSshdDKCROSS ),
    RTF_TEXT_SHD_ENUM( "chbgdkdcross",	ISpropPATTERN, PSshdDKDCROSS ),

    RTF_TEXT_FLAG( "b",			TApropFONTBOLD ),
    RTF_TEXT_FLAG( "i",			TApropFONTSLANTED ),

    RTF_TEXT_ENUM( "ul",		TApropTEXTUNDERLINED, 1 ),
    RTF_TEXT_ENUM( "ulnone",		TApropTEXTUNDERLINED, 0 ),
    
    RTF_TEXT_FLAG_X( "uld",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "uldash",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "uldashd",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "uldashdd",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "uldb",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulhwave",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulldash ",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulth",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulthd",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulthdash ",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulthdashd ",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulthdashdd ",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulthldash ",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ululdbwave",	TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulw",		TApropTEXTUNDERLINED ),
    RTF_TEXT_FLAG_X( "ulwave",		TApropTEXTUNDERLINED ),


    RTF_TEXT_ENUM( RTFtag_nosupersub,	TApropSUPERSUB, TEXTvaREGULAR ),
    RTF_TEXT_ENUM( RTFtag_super,	TApropSUPERSUB, TEXTvaSUPERSCRIPT ),
    RTF_TEXT_ENUM( RTFtag_sub,		TApropSUPERSUB, TEXTvaSUBSCRIPT ),

    RTF_TEXT_FLAG( "caps",		TApropCAPITALS ),
    RTF_TEXT_FLAG( "scaps",		TApropSMALLCAPS ),
    RTF_TEXT_FLAG( "strike",		TApropSTRIKETHROUGH ),
    /* Note number: striked1 or striked0 */
    RTF_TEXT_NUMBER_X( "striked",	TApropSTRIKETHROUGH ), /* HACK ! */

    RTF_TEXT_DIR( RTFtag_ltrch,		0 ),
    RTF_TEXT_DIR( RTFtag_rtlch,		1 ),

    /*
     *  HACK to render the revision control directive as strikethrough
     */
    RTF_TEXT_FLAG_X( "deleted",		TApropSTRIKETHROUGH ),

    RTF_REMEMBER( "ulc",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "embo",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "impr",		RTFid_NOT_SUPPORTED ),

				/**
				 *  Paragraph number attributes.
				 *
				 *  I think I have replaced this with 
				 *  list functionality, but to be sure, 
				 *  the tags are still skipped. (MdD jul 2015)
				 *
				 */
# define	RTF_PN_IGNORE(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeANY, \
		    docRtfIgnoreWord \
		    }

    RTF_PN_IGNORE( "pnstart",	LLpropSTARTAT ),

    RTF_PN_IGNORE( "pndec",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnucrm",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnlcrm",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnucltr",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnlcltr",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnord",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pncard",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnordt",	LLpropSTYLE ),

    RTF_PN_IGNORE( "pndbnum",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pndbnumd",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnaiu",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pniroha",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pndbnuml",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pndbnumt",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pndbnumk",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pncnum",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pndecd",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnaiud",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnirohad",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnganada",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pngbnum",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pngbnumd",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pngbnuml",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pngbnumk",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnzodiac",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnzodiacd",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnzodiacl",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnchosung",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnaiueo",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnaiueod",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnbidia",	LLpropSTYLE ),
    RTF_PN_IGNORE( "pnbidib",	LLpropSTYLE ),

    RTF_PN_IGNORE( "pnql",	LLpropJUSTIFY ),
    RTF_PN_IGNORE( "pnqc",	LLpropJUSTIFY ),
    RTF_PN_IGNORE( "pnqr",	LLpropJUSTIFY ),

    RTF_PN_IGNORE( "pnindent",	LLpropINDENT ),
    RTF_PN_IGNORE( "pnsp",	LLpropSPACE ),
    RTF_PN_IGNORE( "pnprev",	LLpropPREV ),

    RTF_PN_IGNORE( "pnacross",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnlvl",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnlvlblt",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnlvlbody",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnlvlcont",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnnumonce",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnrestart",	RTFid_NOT_SUPPORTED ),
    RTF_PN_IGNORE( "pnrnot",	RTFid_NOT_SUPPORTED ),

    /**/

    RTF_REMEMBER( "cgrid",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "cchs",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "outl",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revauth",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "crauth",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "crdate",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revauthdel",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revdttm",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revdttmdel",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revised",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "shad",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "fcs",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "v",			RTFidV ),
    RTF_REMEMBER( "webhidden",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "kerning",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "expnd",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "expndtw",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "charscalex",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "highlight",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "faauto",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "fahang",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "facenter",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "faroman",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "favar",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "fafixed",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Paragraph properties.		*/
				/****************************************/

# define	RTF_PARA_DEFAULT(s) \
		    { \
		    s, RTCscopePARA, PPprop_NONE, RTCtypeANY, \
		    docRtfRememberParagraphProperty, \
		    }

# define	RTF_PARA_NUMBER(s,id) \
		    { \
		    s, RTCscopePARA, id, RTCtypeNUMBER, \
		    docRtfRememberParagraphProperty, \
		    }
# define	RTF_PARA_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberParagraphProperty, \
		    }

# define	RTF_PARA_ENUM(s,id,v) \
		    { \
		    s, RTCscopePARA, id, RTCtypeENUM, \
		    docRtfRememberParagraphProperty, v, \
		    }

# define	RTF_PARA_FLAG(s,id) \
		    { \
		    s, RTCscopePARA, id, RTCtypeFLAG, \
		    docRtfRememberParagraphProperty, \
		    }
# define	RTF_PARA_FLAG_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberParagraphProperty, \
		    }

# define	RTF_PARA_BORDER(s,id) \
			RTF_BORDER_ANYWAY(s,RTCscopePARA, id,docRtfRememberParagraphProperty)

# define	RTF_PARA_SHD_NUMBER(s,id) \
		    { \
		    s, RTCscopePARA_SHADING, id, RTCtypeNUMBER, \
		    docRtfRememberParaShadingProperty, \
		    }

# define	RTF_PARA_SHD_ENUM(s,id,v) \
		    { \
		    s, RTCscopePARA_SHADING, id, RTCtypeENUM, \
		    docRtfRememberParaShadingProperty, v, \
		    }

    RTF_PARA_DEFAULT( RTFtag_pard ),

    RTF_PARA_NUMBER( "s",		PPpropSTYLE ),
    RTF_PARA_NUMBER( "ls",		PPpropLISTOVERRIDE ),

    RTF_PARA_NUMBER( "li",		PPpropLEFT_INDENT ),
    RTF_PARA_NUMBER( "fi",		PPpropFIRST_INDENT ),
    RTF_PARA_NUMBER( "ri",		PPpropRIGHT_INDENT ),

    RTF_PARA_ENUM( "ql",		PPpropALIGNMENT, DOCthaLEFT ),
    RTF_PARA_ENUM( "qr",		PPpropALIGNMENT, DOCthaRIGHT ),
    RTF_PARA_ENUM( "qj",		PPpropALIGNMENT, DOCthaJUSTIFIED ),
    RTF_PARA_ENUM( "qc",		PPpropALIGNMENT, DOCthaCENTERED ),

    RTF_PARA_ENUM( "nopagebb",		PPpropBREAK_KIND, DOCibkNONE ), /*!*/
    RTF_PARA_ENUM( "pagebb",		PPpropBREAK_KIND, DOCibkPAGE ),

    RTF_PARA_ENUM( "nowidctlpar", 	PPpropWIDCTLPAR, 0 ),
    RTF_PARA_ENUM( "widctlpar",		PPpropWIDCTLPAR, 1 ),

    RTF_PARA_FLAG( "keep",		PPpropKEEP ),
    RTF_PARA_FLAG( "keepn",		PPpropKEEPN ),

    RTF_PARA_NUMBER( "sb",		PPpropSPACE_BEFORE ),
    RTF_PARA_NUMBER( "sa",		PPpropSPACE_AFTER ),
    RTF_PARA_NUMBER( "sl",		PPpropLINE_SPACING_DIST ),

    RTF_PARA_NUMBER( "lisb",		PPprop_LISB ),
    RTF_PARA_NUMBER( "lisa",		PPprop_LISA ),

    RTF_PARA_FLAG( "slmult", 		PPpropLINE_SPACING_MULT ),

    RTF_PARA_NUMBER( "outlinelevel",	PPpropOUTLINELEVEL ),
    RTF_PARA_NUMBER( "ilvl",		PPpropLISTLEVEL ),

    RTF_PARA_FLAG( "hyphpar",		PPpropHYPHPAR ),
    RTF_PARA_FLAG( "intbl",		PPprop_IN_TABLE ),
    RTF_PARA_NUMBER( "itap",		PPpropTABLE_NESTING ),

    RTF_PARA_SHD_ENUM( "bghoriz",	ISpropPATTERN, PSshdHORIZ ),
    RTF_PARA_SHD_ENUM( "bgvert",	ISpropPATTERN, PSshdVERT ),
    RTF_PARA_SHD_ENUM( "bgfdiag",	ISpropPATTERN, PSshdFDIAG ),
    RTF_PARA_SHD_ENUM( "bgbdiag",	ISpropPATTERN, PSshdBDIAG ),
    RTF_PARA_SHD_ENUM( "bgcross",	ISpropPATTERN, PSshdCROSS ),
    RTF_PARA_SHD_ENUM( "bgdcross",	ISpropPATTERN, PSshdDCROSS ),
    RTF_PARA_SHD_ENUM( "bgdkhoriz",	ISpropPATTERN, PSshdDKHORIZ ),
    RTF_PARA_SHD_ENUM( "bgdkvert",	ISpropPATTERN, PSshdDKVERT ),
    RTF_PARA_SHD_ENUM( "bgdkfdiag",	ISpropPATTERN, PSshdDKFDIAG ),
    RTF_PARA_SHD_ENUM( "bgdkbdiag",	ISpropPATTERN, PSshdDKBDIAG ),
    RTF_PARA_SHD_ENUM( "bgdkcross",	ISpropPATTERN, PSshdDKCROSS ),
    RTF_PARA_SHD_ENUM( "bgdkdcross",	ISpropPATTERN, PSshdDKDCROSS ),

    RTF_PARA_SHD_NUMBER( "cfpat",	ISpropFORE_COLOR ),
    RTF_PARA_SHD_NUMBER( "cbpat",	ISpropBACK_COLOR ),
    RTF_PARA_SHD_NUMBER( "shading",	ISpropLEVEL ),

    RTF_PARA_BORDER( "brdrt",		PPpropTOP_BORDER ),
    RTF_PARA_BORDER( "brdrb",		PPpropBOTTOM_BORDER ),
    RTF_PARA_BORDER( "brdrl",		PPpropLEFT_BORDER ),
    RTF_PARA_BORDER( "brdrr",		PPpropRIGHT_BORDER ),
    RTF_PARA_BORDER( "brdrbtw",		PPpropBETWEEN_BORDER ),
    RTF_PARA_BORDER( "brdrbar",		PPpropBAR_BORDER ),
    RTF_PARA_BORDER( "box",		PPprop_BOX_BORDER ),

    RTF_PARA_ENUM( "ltrpar",		PPpropRTOL, 0 ),
    RTF_PARA_ENUM( "rtlpar",		PPpropRTOL, 1 ),

					/**/

    RTF_PARA_FLAG_X( "nosnaplinegrid",	PPprop_IGNORED ),
    RTF_PARA_FLAG_X( "wrapdefault",	PPprop_IGNORED ),

    RTF_PARA_FLAG_X( "adjustright",	PPprop_IGNORED ),
    RTF_PARA_FLAG_X( "adjustleft",	PPprop_IGNORED ),

    RTF_PARA_NUMBER_X( "sbauto",	PPprop_IGNORED ),
    RTF_PARA_NUMBER_X( "saauto",	PPprop_IGNORED ),

    RTF_PARA_FLAG_X( "nooverflow",	PPprop_IGNORED ),

    RTF_PARA_FLAG_X( "aspalpha",	PPprop_IGNORED ),
    RTF_PARA_FLAG_X( "aspnum",		PPprop_IGNORED ),
    RTF_PARA_FLAG_X( "contextualspace",	PPprop_IGNORED ),

			/*
			 * Positioned Objects and Frames: (Row properties.)
			 */
# define RTF_ROW_FRAME_NUMBER(s,id) \
	    { \
	    s, RTCscopeROW_FRAME, id, RTCtypeNUMBER, \
	    docRtfRememberRowFrameProperty, \
	    }
# define RTF_ROW_FRAME_NUMBER_X(s,id) \
	    { \
	    s, RTCscopeANY, id, RTCtypeNUMBER, \
	    docRtfRememberRowFrameProperty, \
	    }

# define RTF_ROW_FRAME_ENUM(s,id,v) \
	    { \
	    s, RTCscopeROW_FRAME,id, RTCtypeENUM, \
	    docRtfRememberRowFrameProperty, v, \
	    }

# define RTF_ROW_FRAME_FLAG(s,id) \
	    { \
	    s, RTCscopeROW_FRAME,id, RTCtypeFLAG, \
	    docRtfRememberRowFrameProperty, \
	    }

    /* no TFPpropABSW */
    /* no TFPpropABSH */

    /* no TFPpropLOCKED_TO_PARA */

    /* no TFPpropWRAP_STYLE, FWSwrapNOWRAP */
    /* no TFPpropWRAP_STYLE, FWSwrapOVERLAY */

    RTF_ROW_FRAME_FLAG( "tabsnoovrlp",	TFPpropNO_OVERLAP ),

    RTF_ROW_FRAME_ENUM( "tphmrg",	TFPpropHORIZONTAL_REF, FXrefMARGIN ),
    RTF_ROW_FRAME_ENUM( "tphpg",	TFPpropHORIZONTAL_REF, FXrefPAGE ),
    RTF_ROW_FRAME_ENUM( "tphcol",	TFPpropHORIZONTAL_REF, FXrefCOLUMN ),

    RTF_ROW_FRAME_NUMBER( "tposx",	TFPpropHORIZONTAL_POS ),
    RTF_ROW_FRAME_NUMBER_X( "tposnegx",	TFPpropHORIZONTAL_POS ),

    RTF_ROW_FRAME_ENUM( "tposxl",	TFPpropHORIZONTAL_ALIGN, FXposXL ),
    RTF_ROW_FRAME_ENUM( "tposxr",	TFPpropHORIZONTAL_ALIGN, FXposXR ),
    RTF_ROW_FRAME_ENUM( "tposxc",	TFPpropHORIZONTAL_ALIGN, FXposXC ),
    RTF_ROW_FRAME_ENUM( "tposxi",	TFPpropHORIZONTAL_ALIGN, FXposXI ),
    RTF_ROW_FRAME_ENUM( "tposxo",	TFPpropHORIZONTAL_ALIGN, FXposXO ),

    RTF_ROW_FRAME_ENUM( "tpvmrg",	TFPpropVERTICAL_REF, FYrefMARGIN ),
    RTF_ROW_FRAME_ENUM( "tpvpara",	TFPpropVERTICAL_REF, FYrefPARA ),
    RTF_ROW_FRAME_ENUM( "tpvpg",	TFPpropVERTICAL_REF, FYrefPAGE ),
    RTF_ROW_FRAME_ENUM( "tposyil",	TFPpropVERTICAL_REF, FYrefLINE ),

    RTF_ROW_FRAME_NUMBER( "tposy",	TFPpropVERTICAL_POS ),
    RTF_ROW_FRAME_NUMBER_X( "tposnegy",	TFPpropVERTICAL_POS ),

    RTF_ROW_FRAME_ENUM( "tposyb",	TFPpropVERTICAL_ALIGN, FYposYB ),
    RTF_ROW_FRAME_ENUM( "tposyc",	TFPpropVERTICAL_ALIGN, FYposYC ),
    RTF_ROW_FRAME_ENUM( "tposyin",	TFPpropVERTICAL_ALIGN, FYposYIN ),
    RTF_ROW_FRAME_ENUM( "tposyout",	TFPpropVERTICAL_ALIGN, FYposYOUT ),
    RTF_ROW_FRAME_ENUM( "tposyt",	TFPpropVERTICAL_ALIGN, FYposYT ),

    /*  no TFPpropDFRMTXT */
    /*  no TFPpropDFRMTXTX */
    /*  no TFPpropDFRMTXTY */

    RTF_ROW_FRAME_NUMBER( "tdfrmtxtLeft",	TFPpropDFRMTXTL ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtRight",	TFPpropDFRMTXTR ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtTop",	TFPpropDFRMTXTT ),
    RTF_ROW_FRAME_NUMBER( "tdfrmtxtBottom",	TFPpropDFRMTXTB ),

				/****************************************/
				/*  Positioned Objects and Frames:	*/
				/*  (Paragraph properties.)		*/
				/****************************************/
# define	RTF_PARA_FRAME_NUMBER(s,id) \
		    { \
		    s, RTCscopePARA_FRAME, id, RTCtypeNUMBER, \
		    docRtfRememberParaFrameProperty, \
		    }
# define	RTF_PARA_FRAME_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberParaFrameProperty, \
		    }

# define	RTF_PARA_FRAME_ENUM(s,id,v) \
		    { \
		    s, RTCscopePARA_FRAME, id, RTCtypeENUM, \
		    docRtfRememberParaFrameProperty, v, \
		    }

# define	RTF_PARA_FRAME_FLAG(s,id) \
		    { \
		    s, RTCscopePARA_FRAME, id, RTCtypeFLAG, \
		    docRtfRememberParaFrameProperty, \
		    }

    RTF_PARA_FRAME_NUMBER( "absw",	TFPpropABSW ),
    RTF_PARA_FRAME_NUMBER( "absh",	TFPpropABSH ),

    RTF_PARA_FRAME_FLAG( "abslock",	TFPpropLOCK_ANCHOR ),

    RTF_PARA_FRAME_ENUM( "nowrap",	TFPpropWRAP_STYLE, FWSwrapNOWRAP ),
    RTF_PARA_FRAME_ENUM( "overlay",	TFPpropWRAP_STYLE, FWSwrapOVERLAY ),

    RTF_PARA_FRAME_FLAG( "absnoovrlp",	TFPpropNO_OVERLAP ),

    RTF_PARA_FRAME_ENUM( "phmrg",	TFPpropHORIZONTAL_REF, FXrefMARGIN ),
    RTF_PARA_FRAME_ENUM( "phpg",	TFPpropHORIZONTAL_REF, FXrefPAGE ),
    RTF_PARA_FRAME_ENUM( "phcol",	TFPpropHORIZONTAL_REF, FXrefCOLUMN ),

    RTF_PARA_FRAME_NUMBER( "posx",	TFPpropHORIZONTAL_POS ),
    RTF_PARA_FRAME_NUMBER_X( "posnegx",	TFPpropHORIZONTAL_POS ),

    RTF_PARA_FRAME_ENUM( "posxl",	TFPpropHORIZONTAL_ALIGN, FXposXL ),
    RTF_PARA_FRAME_ENUM( "posxr",	TFPpropHORIZONTAL_ALIGN, FXposXR ),
    RTF_PARA_FRAME_ENUM( "posxc",	TFPpropHORIZONTAL_ALIGN, FXposXC ),
    RTF_PARA_FRAME_ENUM( "posxi",	TFPpropHORIZONTAL_ALIGN, FXposXI ),
    RTF_PARA_FRAME_ENUM( "posxo",	TFPpropHORIZONTAL_ALIGN, FXposXO ),

    RTF_PARA_FRAME_ENUM( "pvmrg",	TFPpropVERTICAL_REF, FYrefMARGIN ),
    RTF_PARA_FRAME_ENUM( "pvpara",	TFPpropVERTICAL_REF, FYrefPARA ),
    RTF_PARA_FRAME_ENUM( "pvpg",	TFPpropVERTICAL_REF, FYrefPAGE ),
    RTF_PARA_FRAME_ENUM( "posyil",	TFPpropVERTICAL_REF, FYrefLINE ),

    RTF_PARA_FRAME_NUMBER( "posy",	TFPpropVERTICAL_POS ),
    RTF_PARA_FRAME_NUMBER_X( "posnegy",	TFPpropVERTICAL_POS ),

    RTF_PARA_FRAME_ENUM( "posyb",	TFPpropVERTICAL_ALIGN, FYposYB ),
    RTF_PARA_FRAME_ENUM( "posyc",	TFPpropVERTICAL_ALIGN, FYposYC ),
    RTF_PARA_FRAME_ENUM( "posyin",	TFPpropVERTICAL_ALIGN, FYposYIN ),
    RTF_PARA_FRAME_ENUM( "posyout",	TFPpropVERTICAL_ALIGN, FYposYOUT ),
    RTF_PARA_FRAME_ENUM( "posyt",	TFPpropVERTICAL_ALIGN, FYposYT ),

    RTF_PARA_FRAME_NUMBER( "dxfrtext",	TFPprop_DFRMTXT ),
    RTF_PARA_FRAME_NUMBER( "dfrmtxtx",	TFPprop_DFRMTXTX ),
    RTF_PARA_FRAME_NUMBER( "dfrmtxty",	TFPprop_DFRMTXTY ),

    RTF_PARA_FRAME_ENUM( "frmtxlrtb",	TFPpropTEXT_FLOW, TXflowTXLRTB ),
    RTF_PARA_FRAME_ENUM( "frmtxtbrl",	TFPpropTEXT_FLOW, TXflowTXTBRL ),
    RTF_PARA_FRAME_ENUM( "frmtxbtlr",	TFPpropTEXT_FLOW, TXflowTXBTLR ),
    RTF_PARA_FRAME_ENUM( "frmtxlrtbv",	TFPpropTEXT_FLOW, TXflowTXLRTBV ),
    RTF_PARA_FRAME_ENUM( "frmtxtbrlv",	TFPpropTEXT_FLOW, TXflowTXTBRLV ),

    /*  no TFPpropDFRMTXTL */
    /*  no TFPpropDFRMTXTR */
    /*  no TFPpropDFRMTXTT */
    /*  no TFPpropDFRMTXTB */

    /**/
    RTF_REMEMBER( "dropcapli",		TFPprop_IGNORED ),
    RTF_REMEMBER( "dropcapt",		TFPprop_IGNORED ),
    RTF_REMEMBER( "wraparound",		TFPprop_IGNORED ),
				/****************************************/
				/*  Revision marks for paragraph	*/
				/*  numbers.				*/
				/****************************************/
    RTF_REMEMBER( "pnrauth",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrdate",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrnfc",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrpnbr",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrrgb",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrstart",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrstop",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnrxst",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Associated Character Properties	*/
				/****************************************/
    RTF_REMEMBER( "ab",			RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "acaps",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "acf",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "adn",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aexpnd",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "af",			RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "afs",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ai",			RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "alang",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aoutl",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ascaps",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ashad",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "astrike",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aul",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "auld",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "auldb",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aulnone",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aulw",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "aup",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "loch",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "hich",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "dbch",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Row, Cell/Column properties.	*/
				/****************************************/
# define	RTF_CELL_X(s) \
		    { \
		    s, RTCscopeCELL, CLpropWIDTH, RTCtypeANY, \
		    docRtfGotCellX, \
		    }

# define	RTF_CELL_DEFAULT(s) \
		    { \
		    s, RTCscopeCELL, CLprop_NONE, RTCtypeANY, \
		    docRtfRememberCellProperty, \
		    }

# define	RTF_CELL_NUMBER(s,id) \
		    { \
		    s, RTCscopeCELL, id, RTCtypeNUMBER, \
		    docRtfRememberCellProperty, \
		    }
# define	RTF_CELL_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberCellProperty, \
		    }

# define	RTF_CELL_ENUM(s,id,v) \
		    { \
		    s, RTCscopeCELL, id, RTCtypeENUM, \
		    docRtfRememberCellProperty, v, \
		    }
# define	RTF_CELL_ENUM_X(s,id,v) \
		    { \
		    s, RTCscopeANY, id, RTCtypeENUM, \
		    docRtfRememberCellProperty, v, \
		    }

# define	RTF_CELL_FLAG(s,id) \
		    { \
		    s, RTCscopeCELL, id, RTCtypeFLAG, \
		    docRtfRememberCellProperty, \
		    }

# define	RTF_CELL_BORDER(s,id) \
			RTF_BORDER_POSITIVE(s,RTCscopeCELL, id, docRtfRememberCellProperty)

# define	RTF_CELL_SHD_ENUM(s,id,v) \
		    { \
		    s, RTCscopeCELL_SHADING, id, RTCtypeENUM, \
		    docRtfRememberCellShadingProperty, v, \
		    }

# define	RTF_CELL_SHD_NUMBER(s,id) \
		    { \
		    s, RTCscopeCELL_SHADING, id, RTCtypeNUMBER, \
		    docRtfRememberCellShadingProperty, \
		    }
# define	RTF_CELL_SHD_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberCellShadingProperty, \
		    }

    RTF_CELL_X( RTFtag_cellx ),

    RTF_CELL_DEFAULT( "celld" ),

    RTF_CELL_BORDER( "clbrdrt",		CLpropTOP_BORDER ),
    RTF_CELL_BORDER( "clbrdrb",		CLpropBOTTOM_BORDER ),
    RTF_CELL_BORDER( "clbrdrl",		CLpropLEFT_BORDER ),
    RTF_CELL_BORDER( "clbrdrr",		CLpropRIGHT_BORDER ),

    RTF_CELL_BORDER( "cldglu",		CLpropLEFT_RIGHT_DIAGONAL ),
    RTF_CELL_BORDER( "cldgll",		CLpropRIGHT_LEFT_DIAGONAL ),

    RTF_CELL_SHD_ENUM( "clbghoriz",	ISpropPATTERN, PSshdHORIZ ),
    RTF_CELL_SHD_ENUM( "clbgvert",	ISpropPATTERN, PSshdVERT ),
    RTF_CELL_SHD_ENUM( "clbgfdiag",	ISpropPATTERN, PSshdFDIAG ),
    RTF_CELL_SHD_ENUM( "clbgbdiag",	ISpropPATTERN, PSshdBDIAG ),
    RTF_CELL_SHD_ENUM( "clbgcross",	ISpropPATTERN, PSshdCROSS ),
    RTF_CELL_SHD_ENUM( "clbgdcross",	ISpropPATTERN, PSshdDCROSS ),
    RTF_CELL_SHD_ENUM( "clbgdkhor",	ISpropPATTERN, PSshdDKHORIZ ),
    RTF_CELL_SHD_ENUM( "clbgdkvert",	ISpropPATTERN, PSshdDKVERT ),
    RTF_CELL_SHD_ENUM( "clbgdkfdiag",	ISpropPATTERN, PSshdDKFDIAG ),
    RTF_CELL_SHD_ENUM( "clbgdkbdiag",	ISpropPATTERN, PSshdDKBDIAG ),
    RTF_CELL_SHD_ENUM( "clbgdkcross",	ISpropPATTERN, PSshdDKCROSS ),
    RTF_CELL_SHD_ENUM( "clbgdkdcross",	ISpropPATTERN, PSshdDKDCROSS ),

    RTF_CELL_SHD_NUMBER( "clcfpat",	ISpropFORE_COLOR ),
    RTF_CELL_SHD_NUMBER( "clcbpat",	ISpropBACK_COLOR ),
    RTF_CELL_SHD_NUMBER( "clshdng",	ISpropLEVEL ),

    RTF_CELL_SHD_NUMBER_X( "clcfpatraw",	ISpropFORE_COLOR ), /*?*/
    RTF_CELL_SHD_NUMBER_X( "clcbpatraw",	ISpropBACK_COLOR ), /*?*/
    RTF_CELL_SHD_NUMBER_X( "clshdngraw",	ISpropLEVEL ), /*?*/

    RTF_CELL_ENUM( "clvertalt",		CLpropVALIGN, DOCtvaTOP ),
    RTF_CELL_ENUM( "clvertalc",		CLpropVALIGN, DOCtvaCENTERED ),
    RTF_CELL_ENUM( "clvertalb",		CLpropVALIGN, DOCtvaBOTTOM ),

					/* In table styles */
    RTF_CELL_ENUM_X( "tsvertalt",	CLpropVALIGN, DOCtvaTOP ),
    RTF_CELL_ENUM_X( "tsvertalc",	CLpropVALIGN, DOCtvaCENTERED ),
    RTF_CELL_ENUM_X( "tsvertalb",	CLpropVALIGN, DOCtvaBOTTOM ),

    RTF_CELL_ENUM( "cltxlrtb",		CLpropTEXT_FLOW, TXflowTXLRTB ),
    RTF_CELL_ENUM( "cltxtbrl",		CLpropTEXT_FLOW, TXflowTXTBRL ),
    RTF_CELL_ENUM( "cltxbtlr",		CLpropTEXT_FLOW, TXflowTXBTLR ),
    RTF_CELL_ENUM( "cltxlrtbv",		CLpropTEXT_FLOW, TXflowTXLRTBV ),
    RTF_CELL_ENUM( "cltxtbrlv",		CLpropTEXT_FLOW, TXflowTXTBRLV ),

    RTF_CELL_ENUM( "ClmNone",		CLpropHOR_MERGE, CELLmergeNONE ),
    RTF_CELL_ENUM( "clmgf",		CLpropHOR_MERGE, CELLmergeHEAD ),
    RTF_CELL_ENUM( "clmrg",		CLpropHOR_MERGE, CELLmergeFOLLOW ),

    RTF_CELL_ENUM( "ClvmNone",		CLpropVERT_MERGE, CELLmergeNONE ),
    RTF_CELL_ENUM( "clvmgf",		CLpropVERT_MERGE, CELLmergeHEAD ),
    RTF_CELL_ENUM( "clvmrg",		CLpropVERT_MERGE, CELLmergeFOLLOW ),

    RTF_CELL_FLAG( "clNoWrap",		CLpropNO_WRAP ),

    RTF_CELL_FLAG( "clhdr",		CLpropROW_HEADER ),

    RTF_CELL_FLAG( "clshdrawnil",	CLpropNO_SHADING ),

    RTF_CELL_NUMBER( "clwWidth",	CLpropCLW_WIDTH ),
    RTF_CELL_NUMBER( "clftsWidth",	CLpropCLFTS_WIDTH ),
    RTF_CELL_NUMBER_X( "tscellwidthfts",CLprop_NOT_SUPPORTED ),
    RTF_CELL_NUMBER_X( "tscellpct",	CLprop_NOT_SUPPORTED ),
    RTF_CELL_NUMBER_X( "tscbandsh",	CLprop_NOT_SUPPORTED ),
    RTF_CELL_NUMBER_X( "tscbandsv",	CLprop_NOT_SUPPORTED ),

					/********************************/
					/* NOTE: Word 2003 swaps left	*/
					/*       and top.		*/
					/********************************/
    RTF_CELL_NUMBER( "clpadt",		CLpropLEFT_PADDING ), /* !!! */
    RTF_CELL_NUMBER( "clpadb",		CLpropBOTTOM_PADDING ),
    RTF_CELL_NUMBER( "clpadl",		CLpropTOP_PADDING ), /* !!! */
    RTF_CELL_NUMBER( "clpadr",		CLpropRIGHT_PADDING ),

    RTF_CELL_NUMBER( "clpadft",		CLpropLEFT_PADDING_UNIT ), /* !!! */
    RTF_CELL_NUMBER( "clpadfb",		CLpropBOTTOM_PADDING_UNIT ),
    RTF_CELL_NUMBER( "clpadfl",		CLpropTOP_PADDING_UNIT ), /* !!! */
    RTF_CELL_NUMBER( "clpadfr",		CLpropRIGHT_PADDING_UNIT ),

				/****************************************/
				/*  Application of table styles.	*/
				/****************************************/

# define	RTF_CELLI_NUMBER(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberCellInstanceProperty, \
		    }
# define	RTF_CELLI_FLAG(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberCellInstanceProperty, \
		    }

    RTF_CELLI_NUMBER( "yts",		CIpropYTS ),

    RTF_CELLI_FLAG(   "tscfirstrow",	CIpropFIRST_ROW ),
    RTF_CELLI_FLAG(   "tsclastrow",	CIpropLAST_ROW ),
    RTF_CELLI_FLAG(   "tscfirstcol",	CIpropFIRST_COLUMN ),
    RTF_CELLI_FLAG(   "tsclastcol",	CIpropLAST_COLUMN ),

    RTF_CELLI_FLAG(   "tscbandhorzodd",	CIpropODD_ROW_BAND ),
    RTF_CELLI_FLAG(   "tscbandhorzeven",CIpropEVEN_ROW_BAND ),
    RTF_CELLI_FLAG(   "tscbandvertodd",	CIpropODD_COLUMN_BAND ),
    RTF_CELLI_FLAG(   "tscbandverteven",CIpropEVEN_COLUMN_BAND ),

    RTF_CELLI_FLAG(   "tscnwcell",	CIpropNW_CELL ),
    RTF_CELLI_FLAG(   "tscnecell",	CIpropNE_CELL ),
    RTF_CELLI_FLAG(   "tscswcell",	CIpropSW_CELL ),
    RTF_CELLI_FLAG(   "tscsecell",	CIpropSE_CELL ),

				/****************************************/
				/*  Row properties.			*/
				/****************************************/

# define	RTF_ROW_DEFAULT(s) \
		    { \
		    s, RTCscopeROW, RPprop_NONE, RTCtypeANY, \
		    docRtfRememberRowProperty, \
		    }
# define	RTF_ROW_DEFAULT_X(s) \
		    { \
		    s, RTCscopeANY, RPprop_NONE, RTCtypeANY, \
		    docRtfRememberRowProperty, \
		    }

# define	RTF_ROW_NUMBER(s,id) \
		    { \
		    s, RTCscopeROW, id, RTCtypeNUMBER, \
		    docRtfRememberRowProperty, \
		    }
# define	RTF_ROW_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberRowProperty, \
		    }

# define	RTF_ROW_ENUM(s,id,v) \
		    { \
		    s, RTCscopeROW, id, RTCtypeENUM, \
		    docRtfRememberRowProperty, v, \
		    }

# define	RTF_ROW_FLAG(s,id) \
		    { \
		    s, RTCscopeROW, id, RTCtypeFLAG, \
		    docRtfRememberRowProperty, \
		    }
# define	RTF_ROW_FLAG_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberRowProperty, \
		    }

# define	RTF_ROW_BORDER(s,id) \
		    RTF_BORDER_POSITIVE(s,RTCscopeROW,id,docRtfRememberRowProperty)
# define	RTF_ROW_BORDER_X(s,id) \
		    RTF_BORDER_POSITIVE(s,RTCscopeANY,id,docRtfRememberRowProperty)

# define	RTF_ROW_SHD_NUMBER(s,id) \
		    { \
		    s, RTCscopeROW_SHADING, id, RTCtypeNUMBER, \
		    docRtfRememberRowShadingProperty, \
		    }
# define	RTF_ROW_SHD_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberRowShadingProperty, \
		    }

# define	RTF_ROW_SHD_ENUM(s,id,v) \
		    { \
		    s, id, RTCscopeROW_SHADING, RTCtypeENUM, \
		    docRtfRememberRowShadingProperty, v, \
		    }
# define	RTF_ROW_SHD_ENUM_X(s,id,v) \
		    { \
		    s, id, RTCscopeANY, RTCtypeENUM, \
		    docRtfRememberRowShadingProperty, v, \
		    }

    RTF_ROW_DEFAULT( "trowd" ),
					/* In table styles */
    RTF_ROW_DEFAULT_X( "tsrowd" ),

    RTF_ROW_NUMBER( "trgaph",		RPpropGAP_WIDTH ),
    RTF_ROW_NUMBER( "trleft",		RPpropLEFT_INDENT ),
    RTF_ROW_NUMBER( "trrh",		RPpropHEIGHT ),

    RTF_ROW_FLAG( "trhdr",		RPpropIS_TABLE_HEADER ),
    RTF_ROW_FLAG( "trkeep",		RPpropKEEP_ON_ONE_PAGE ),
    RTF_ROW_FLAG( "trkeepfollow",	RPprop_KEEPFOLLOW ),
    RTF_ROW_FLAG( "trautofit",		RPpropAUTOFIT ),

    RTF_ROW_ENUM( "trql",		RPpropALIGNMENT, DOCthaLEFT ),
    RTF_ROW_ENUM( "trqr",		RPpropALIGNMENT, DOCthaRIGHT ),
    RTF_ROW_ENUM( "trqc",		RPpropALIGNMENT, DOCthaCENTERED ),

    RTF_ROW_ENUM( "rtlrow",		RPpropRTOL, 1 ),
    RTF_ROW_ENUM( "ltrrow",		RPpropRTOL, 0 ),

    /**/
    RTF_ROW_BORDER( "trbrdrt",		RPpropTOP_BORDER ),
    RTF_ROW_BORDER( "trbrdrb",		RPpropBOTTOM_BORDER ),
    RTF_ROW_BORDER( "trbrdrl",		RPpropLEFT_BORDER ),
    RTF_ROW_BORDER( "trbrdrr",		RPpropRIGHT_BORDER ),
    RTF_ROW_BORDER( "trbrdrh",		RPpropHORIZ_BORDER ),
    RTF_ROW_BORDER( "trbrdrv",		RPpropVERT_BORDER ),

					/* In table styles */
    RTF_ROW_BORDER_X( "tsbrdrt",	RPpropTOP_BORDER ),
    RTF_ROW_BORDER_X( "tsbrdrb",	RPpropBOTTOM_BORDER ),
    RTF_ROW_BORDER_X( "tsbrdrl",	RPpropLEFT_BORDER ),
    RTF_ROW_BORDER_X( "tsbrdrr",	RPpropRIGHT_BORDER ),
    RTF_ROW_BORDER_X( "tsbrdrh",	RPpropHORIZ_BORDER ),
    RTF_ROW_BORDER_X( "tsbrdrv",	RPpropVERT_BORDER ),

    RTF_ROW_BORDER_X( "tsbrdrdgl",	RPprop_IGNORED ),
    RTF_ROW_BORDER_X( "tsbrdrdgr",	RPprop_IGNORED ),

    /**/
    RTF_ROW_NUMBER( "trwWidth",		RPpropTRW_WIDTH ),
    RTF_ROW_NUMBER( "trftsWidth",	RPpropTRFTS_WIDTH ),

    /**/
    RTF_ROW_NUMBER( "trspdl",		RPpropTRSPDL ),
    RTF_ROW_NUMBER( "trspdr",		RPpropTRSPDR ),
    RTF_ROW_NUMBER( "trspdt",		RPpropTRSPDT ),
    RTF_ROW_NUMBER( "trspdb",		RPpropTRSPDB ),

    RTF_ROW_NUMBER( "trspdfl",		RPpropTRSPDFL ),
    RTF_ROW_NUMBER( "trspdfr",		RPpropTRSPDFR ),
    RTF_ROW_NUMBER( "trspdft",		RPpropTRSPDFT ),
    RTF_ROW_NUMBER( "trspdfb",		RPpropTRSPDFB ),

    /**/
    RTF_ROW_NUMBER( "trpaddl",		RPpropTRPADDL ),
    RTF_ROW_NUMBER( "trpaddr",		RPpropTRPADDR ),
    RTF_ROW_NUMBER( "trpaddt",		RPpropTRPADDT ),
    RTF_ROW_NUMBER( "trpaddb",		RPpropTRPADDB ),

    RTF_ROW_NUMBER( "trpaddfl",		RPpropTRPADDFL ),
    RTF_ROW_NUMBER( "trpaddfr",		RPpropTRPADDFR ),
    RTF_ROW_NUMBER( "trpaddft",		RPpropTRPADDFT ),
    RTF_ROW_NUMBER( "trpaddfb",		RPpropTRPADDFB ),

					/* In table styles */
    RTF_ROW_NUMBER_X( "tscellpaddl",	RPpropTRPADDL ),
    RTF_ROW_NUMBER_X( "tscellpaddr",	RPpropTRPADDR ),
    RTF_ROW_NUMBER_X( "tscellpaddt",	RPpropTRPADDT ),
    RTF_ROW_NUMBER_X( "tscellpaddb",	RPpropTRPADDB ),

					/* In table styles */
    RTF_ROW_NUMBER_X( "tscellpaddfl",	RPpropTRPADDFL ),
    RTF_ROW_NUMBER_X( "tscellpaddfr",	RPpropTRPADDFR ),
    RTF_ROW_NUMBER_X( "tscellpaddft",	RPpropTRPADDFT ),
    RTF_ROW_NUMBER_X( "tscellpaddfb",	RPpropTRPADDFB ),

    /**/
    RTF_ROW_NUMBER( "trwWidthB",	RPpropTRW_WIDTHB ),
    RTF_ROW_NUMBER( "trwWidthA",	RPpropTRW_WIDTHA ),

    RTF_ROW_NUMBER( "trftsWidthB",	RPpropTRFTS_WIDTHB ),
    RTF_ROW_NUMBER( "trftsWidthA",	RPpropTRFTS_WIDTHA ),

    /**/
    RTF_ROW_SHD_ENUM( "trbghoriz",	ISpropPATTERN, PSshdHORIZ ),
    RTF_ROW_SHD_ENUM( "trbgvert",	ISpropPATTERN, PSshdVERT ),
    RTF_ROW_SHD_ENUM( "trbgfdiag",	ISpropPATTERN, PSshdFDIAG ),
    RTF_ROW_SHD_ENUM( "trbgbdiag",	ISpropPATTERN, PSshdBDIAG ),
    RTF_ROW_SHD_ENUM( "trbgcross",	ISpropPATTERN, PSshdCROSS ),
    RTF_ROW_SHD_ENUM( "trbgdcross",	ISpropPATTERN, PSshdDCROSS ),
    RTF_ROW_SHD_ENUM( "trbgdkhor",	ISpropPATTERN, PSshdDKHORIZ ),
    RTF_ROW_SHD_ENUM( "trbgdkvert",	ISpropPATTERN, PSshdDKVERT ),
    RTF_ROW_SHD_ENUM( "trbgdkfdiag",	ISpropPATTERN, PSshdDKFDIAG ),
    RTF_ROW_SHD_ENUM( "trbgdkbdiag",	ISpropPATTERN, PSshdDKBDIAG ),
    RTF_ROW_SHD_ENUM( "trbgdkcross",	ISpropPATTERN, PSshdDKCROSS ),
    RTF_ROW_SHD_ENUM( "trbgdkdcross",	ISpropPATTERN, PSshdDKDCROSS ),

    RTF_ROW_SHD_NUMBER( "trcfpat",	ISpropFORE_COLOR ),
    RTF_ROW_SHD_NUMBER( "trcbpat",	ISpropBACK_COLOR ),
    RTF_ROW_SHD_NUMBER( "trshdng",	ISpropLEVEL ),

					/* In table styles */
    RTF_ROW_SHD_ENUM_X( "tsbghoriz",	ISpropPATTERN, PSshdHORIZ ),
    RTF_ROW_SHD_ENUM_X( "tsbgvert",	ISpropPATTERN, PSshdVERT ),
    RTF_ROW_SHD_ENUM_X( "tsbgfdiag",	ISpropPATTERN, PSshdFDIAG ),
    RTF_ROW_SHD_ENUM_X( "tsbgbdiag",	ISpropPATTERN, PSshdBDIAG ),
    RTF_ROW_SHD_ENUM_X( "tsbgcross",	ISpropPATTERN, PSshdCROSS ),
    RTF_ROW_SHD_ENUM_X( "tsbgdcross",	ISpropPATTERN, PSshdDCROSS ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkhor",	ISpropPATTERN, PSshdDKHORIZ ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkvert",	ISpropPATTERN, PSshdDKVERT ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkfdiag",	ISpropPATTERN, PSshdDKFDIAG ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkbdiag",	ISpropPATTERN, PSshdDKBDIAG ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkcross",	ISpropPATTERN, PSshdDKCROSS ),
    RTF_ROW_SHD_ENUM_X( "tsbgdkdcross",	ISpropPATTERN, PSshdDKDCROSS ),

					/* In table styles */
    RTF_ROW_SHD_NUMBER_X( "tscellcfpat",	ISpropFORE_COLOR ),
    RTF_ROW_SHD_NUMBER_X( "tscellcbpat",	ISpropBACK_COLOR ),

    RTF_ROW_FLAG( "tbllkborder",	RPpropAUTOFORMAT_BORDERS ),
    RTF_ROW_FLAG( "tbllkshading",	RPpropAUTOFORMAT_SHADING ),
    RTF_ROW_FLAG( "tbllkfont",		RPpropAUTOFORMAT_FONT ),
    RTF_ROW_FLAG( "tbllkcolor",		RPpropAUTOFORMAT_COLOR ),
    RTF_ROW_FLAG( "tbllkbestfit",	RPpropAUTOFORMAT_APPLY_BEST_FIT ),
    RTF_ROW_FLAG( "tbllkhdrrows",	RPpropAUTOFORMAT_FIRST_ROW ),
    RTF_ROW_FLAG( "tbllklastrow",	RPpropAUTOFORMAT_LAST_ROW ),
    RTF_ROW_FLAG( "tbllkhdrcols",	RPpropAUTOFORMAT_FIRST_COLUMN ),
    RTF_ROW_FLAG( "tbllklastcol",	RPpropAUTOFORMAT_LAST_COLUMN ),

    RTF_ROW_NUMBER( "irow",		RPpropROW_NUMBER ),
    RTF_ROW_NUMBER( "irowband",		RPpropROW_BAND_NUMBER ),
    RTF_ROW_NUMBER( "ts",		RPpropROW_STYLE ),

    RTF_ROW_FLAG( "lastrow",		RPpropIS_LAST_ROW ),
    RTF_ROW_FLAG( "trauth",		RPpropTRAUTH ),
    RTF_ROW_FLAG( "trflat",		RPpropTRFLAT ),

    RTF_ROW_FLAG_X( "tbllknocolband",	RPprop_IGNORED ),
    RTF_ROW_NUMBER_X( "tblind",		RPprop_IGNORED ),
    RTF_ROW_NUMBER_X( "tblindtype",	RPprop_IGNORED ),

    RTF_TEXT_GROUP( "trsummary",RTCscopeROW,
				RPprop_SUBJECT, docRtfCommitRowText ),

				/****************************************/
				/*  Document formatting.		*/
				/****************************************/

# define	RTF_DOC_NUMBER(s,id) \
		    { \
		    s, RTCscopeDOC, id, RTCtypeNUMBER, \
		    docRtfRememberDocProperty, \
		    }
# define	RTF_DOC_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberDocProperty, \
		    }

# define	RTF_DOC_FLAG(s,id) \
		    { \
		    s, RTCscopeDOC, id, RTCtypeFLAG, \
		    docRtfRememberDocProperty, \
		    }
# define	RTF_DOC_FLAG_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberDocProperty, \
		    }

# define	RTF_DOC_ENUM(s,id,v) \
		    { \
		    s, RTCscopeDOC, id, RTCtypeENUM, \
		    docRtfRememberDocProperty, v \
		    }

# define	RTF_DOC_BORDER(s,id) \
		    RTF_BORDER_ANYWAY(s,RTCscopeDOC,id,docRtfRememberDocProperty)

    RTF_DOC_NUMBER( "margl",		DGpropLEFT_MARGIN ),
    RTF_DOC_NUMBER( "margr",		DGpropRIGHT_MARGIN ),
    RTF_DOC_NUMBER( "margt",		DGpropTOP_MARGIN ),
    RTF_DOC_NUMBER( "margb",		DGpropBOTTOM_MARGIN ),
    RTF_DOC_NUMBER( "paperh",		DGpropPAGE_HEIGHT ),
    RTF_DOC_NUMBER( "paperw",		DGpropPAGE_WIDTH ),
    RTF_DOC_NUMBER( "gutter",		DGpropGUTTER ),
    RTF_DOC_FLAG( "margmirror",		DGpropMARGMIR ),

    RTF_DOC_FLAG( "facingp",		DPpropFACING_PAGES ),
    RTF_DOC_FLAG( "gutterprl",		DPpropGUTTER_HORIZONTAL ),
    RTF_DOC_FLAG( "widowctrl",		DPpropWIDOWCTRL ),
    RTF_DOC_FLAG( "twoonone",		DPpropTWO_ON_ONE ),
    RTF_DOC_FLAG( "doctemp",		DPpropDOCTEMP ),

    RTF_DOC_ENUM( "rtldoc",		DPpropRTOL, 1 ),
    RTF_DOC_ENUM( "ltrdoc",		DPpropRTOL, 0 ),

    RTF_REMEMBER( "psz",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "jcompress",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "jexpand",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "noqfpromote",	RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "trackmoves",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "trackformatting",	RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "relyonvml",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "viewbksp",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "remdttm",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "utinl",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "readonlyrecommended",RTFid_NOT_SUPPORTED),

    RTF_REMEMBER( "horzdoc",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dghspace",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dgvspace",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dghorigin",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dgvorigin",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dghshow",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dgvshow",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "dgmargin",		RTFid_NOT_SUPPORTED),

    RTF_REMEMBER( "hres",		RTFid_NOT_SUPPORTED),
    RTF_REMEMBER( "chhres",		RTFid_NOT_SUPPORTED),

    RTF_DOC_NUMBER( "pgnstart",		DPpropSTART_PAGE ),
    RTF_DOC_NUMBER( "deftab",		DPpropDEFTAB ),

    RTF_DOC_NUMBER( "deff",		DPpropDEFF ),
    RTF_DOC_NUMBER( "stshfdbch",	DPpropSTSHFDBCH ),
    RTF_DOC_NUMBER( "stshfloch",	DPpropSTSHFLOCH ),
    RTF_DOC_NUMBER( "stshfhich",	DPpropSTSHFHICH ),
    RTF_DOC_NUMBER( "stshfbi",		DPpropSTSHFBI ),

    RTF_DOC_BORDER( "pgbrdrt",		DPpropTOP_BORDER ),
    RTF_DOC_BORDER( "pgbrdrb",		DPpropBOTTOM_BORDER ),
    RTF_DOC_BORDER( "pgbrdrl",		DPpropLEFT_BORDER ),
    RTF_DOC_BORDER( "pgbrdrr",		DPpropRIGHT_BORDER ),
    RTF_DOC_BORDER( "pgbrdrhead",	DPpropHEAD_BORDER ),
    RTF_DOC_BORDER( "pgbrdrfoot",	DPpropFOOT_BORDER ),

    RTF_REMEMBER( "truncex",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "nolead",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "msmcap",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "splytwnine",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ftnlytwnine",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "htmautsp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "useltbaln",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "alntblind",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lytcalctblwd",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lyttblrtgr",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lnbrkrule",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "bdrrlswsix",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "oldas",		RTFid_NOT_SUPPORTED ),

    RTF_DOC_FLAG_X( "sprstsm",		DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "pgbrdropt",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "pgbrdrsnap",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "allowfieldendsel",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nobrkwrptbl",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "snaptogridincell",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "wrppunct",		DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "asianbrkrule",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "newtblstyruls",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nogrowautofit",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "rempersonalinfo",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nojkernpunct",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "doctype",		DPprop_IGNORED ),

    RTF_DOC_FLAG_X( "usenormstyforlist",DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "noindnmbrts",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "felnbrelev",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nocxsptable",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "indrlsweleven",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "noafcnsttbl",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "afelev",		DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "hwelev",		DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "spltpgpar",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "notcvasp",		DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "notbrkcnstfrctbl",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "notvatxbx",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "krnprsnet",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "cachedcolbal",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nouicompat",	DPprop_IGNORED ),
    RTF_DOC_FLAG_X( "nofeaturethrottle",DPprop_IGNORED ),

    RTF_DOC_NUMBER_X( "adeflang",	DPprop_IGNORED ),
    RTF_DOC_NUMBER_X( "adeff",		DPprop_IGNORED ),

				/****************************************/
				/*  Additional properties for lookup	*/
				/*  entries.				*/
				/****************************************/

# define	RTF_LOOKUP_FIELD(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeANY, \
		    docRtfLookupWord, \
		    }
# define	RTF_LOOKUP_FLAG(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfLookupWord, \
		    }

    RTF_LOOKUP_FIELD( "tc",		RTFlookupTC ),
    RTF_LOOKUP_FIELD( "tcn",		RTFlookupTCN ),

    RTF_LOOKUP_FLAG( "xef",		RTFlookupXEF ),
    RTF_LOOKUP_FLAG( "bxe",		RTFlookupBXE ),
    RTF_LOOKUP_FLAG( "ixe",		RTFlookupIXE ),

    RTF_LOOKUP_FLAG( "tcf",		RTFlookupTCF ),
    RTF_LOOKUP_FLAG( "tcl",		RTFlookupTCL ),

				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    RTF_REMEMBER( "noxlattoyen",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "expshrtn",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "noultrlspc",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "dntblnsbdb",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "nospaceforul",	RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Paragraph Numbering.		*/
				/****************************************/
    RTF_REMEMBER( "pnhang",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "pnuld",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnuldb",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pnulnone",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Section properties.			*/
				/****************************************/
# define	RTF_SECT_DEFAULT(s) \
		    { \
		    s, RTCscopeSECT, SPprop_NONE, RTCtypeANY, \
		    docRtfRememberSectionProperty, \
		    }
# define	RTF_SECT_NUMBER(s,id) \
		    { \
		    s, RTCscopeSECT, id, RTCtypeNUMBER, \
		    docRtfRememberSectionProperty, \
		    }
# define	RTF_SECT_ENUM(s,id,v) \
		    { \
		    s, RTCscopeSECT, id, RTCtypeENUM, \
		    docRtfRememberSectionProperty, v, \
		    }
# define	RTF_SECT_FLAG(s,id) \
		    { \
		    s, RTCscopeSECT, id, RTCtypeFLAG, \
		    docRtfRememberSectionProperty, \
		    }

    RTF_SECT_DEFAULT( "sectd" ),

    RTF_SECT_NUMBER( "headery",		DGpropHEADER_POSITION ),
    RTF_SECT_NUMBER( "footery",		DGpropFOOTER_POSITION ),
    RTF_SECT_NUMBER( "pgwsxn",		DGpropPAGE_WIDTH ),
    RTF_SECT_NUMBER( "pghsxn",		DGpropPAGE_HEIGHT ),
    RTF_SECT_NUMBER( "marglsxn",	DGpropLEFT_MARGIN ),
    RTF_SECT_NUMBER( "margtsxn",	DGpropTOP_MARGIN ),
    RTF_SECT_NUMBER( "margrsxn",	DGpropRIGHT_MARGIN ),
    RTF_SECT_NUMBER( "margbsxn",	DGpropBOTTOM_MARGIN ),
    RTF_SECT_NUMBER( "guttersxn",	DGpropGUTTER ),
    RTF_SECT_FLAG( "margmirsxn",	DGpropMARGMIR ),

    RTF_SECT_NUMBER( "ds",		SPpropSTYLE ),

    RTF_SECT_FLAG( RTFtag_titlepg,	SPpropTITLEPG ),
    RTF_SECT_FLAG( RTFtag_endpg,	SPpropENDPG ),

    RTF_SECT_FLAG( "sfsthdnopdfart",	SPpropFST_HEADER_NO_PDF_ARTIFACT ),
    RTF_SECT_FLAG( "sfstftnopdfart",	SPpropFST_FOOTER_NO_PDF_ARTIFACT ),
    RTF_SECT_FLAG( "slsthdnopdfart",	SPpropLST_HEADER_NO_PDF_ARTIFACT ),
    RTF_SECT_FLAG( "slsftdnopdfart",	SPpropLST_FOOTER_NO_PDF_ARTIFACT ),

    RTF_SECT_ENUM( "sbknone",		SPpropBREAK_KIND, DOCibkNONE ),
    RTF_SECT_ENUM( "sbkcol",		SPpropBREAK_KIND, DOCibkCOL ),
    RTF_SECT_ENUM( "sbkpage",		SPpropBREAK_KIND, DOCibkPAGE ),
    RTF_SECT_ENUM( "sbkeven",		SPpropBREAK_KIND, DOCibkEVEN ),
    RTF_SECT_ENUM( "sbkodd",		SPpropBREAK_KIND, DOCibkODD ),

    RTF_SECT_ENUM( "pgndec",		SPpropNUMBER_STYLE, DOCpgnDEC ),
    RTF_SECT_ENUM( "pgnucrm",		SPpropNUMBER_STYLE, DOCpgnUCRM ),
    RTF_SECT_ENUM( "pgnlcrm",		SPpropNUMBER_STYLE, DOCpgnLCRM ),
    RTF_SECT_ENUM( "pgnucltr",		SPpropNUMBER_STYLE, DOCpgnUCLTR ),
    RTF_SECT_ENUM( "pgnlcltr",		SPpropNUMBER_STYLE, DOCpgnLCLTR ),

    RTF_SECT_ENUM( "pgncont",		SPpropPAGE_RESTART, 0 ),
    RTF_SECT_ENUM( "pgnrestart",	SPpropPAGE_RESTART, 1 ),

    RTF_SECT_ENUM( "rtlsect",		SPpropRTOL, 1 ),
    RTF_SECT_ENUM( "ltrsect",		SPpropRTOL, 0 ),

					/*  Count from 1! */
    RTF_SECT_NUMBER( "pgnstarts",	SPpropSTART_PAGE ),

    RTF_SECT_NUMBER( "cols",		SPpropCOLUMN_COUNT ),
    RTF_SECT_NUMBER( "colsx",		SPpropCOLUMN_SPACING ),

    RTF_SECT_NUMBER( "colno",		SPprop_COLUMN_NUMBER ),
    RTF_SECT_NUMBER( "colsr",		SPprop_COLUMN_RIGHT ),
    RTF_SECT_NUMBER( "colw",		SPprop_COLUMN_WIDTH ),

    RTF_SECT_FLAG(   "linebetcol",	SPpropLINEBETCOL ),

    RTF_SECT_ENUM( "pgnhnsh",		SPpropNUMBER_HYPHEN, DOCpgnhPGNHNSH ),
    RTF_SECT_ENUM( "pgnhnsp",		SPpropNUMBER_HYPHEN, DOCpgnhPGNHNSP ),
    RTF_SECT_ENUM( "pgnhnsc",		SPpropNUMBER_HYPHEN, DOCpgnhPGNHNSC ),
    RTF_SECT_ENUM( "pgnhnsm",		SPpropNUMBER_HYPHEN, DOCpgnhPGNHNSM ),
    RTF_SECT_ENUM( "pgnhnsn",		SPpropNUMBER_HYPHEN, DOCpgnhPGNHNSN ),

    /**/

    RTF_REMEMBER( "sectdefaultcl",	RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "pgnhn",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "pgnx",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pgny",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "linex",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "vertalb",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "vertalc",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "vertalj",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "vertalt",		RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "sectlinegrid",	RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "endnhere",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "binfsxn",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "binsxn",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linecont",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linemod",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lineppage",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linerestart",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linestarts",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lndscpsxn",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sectunlocked",	RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Section properties: (Notes)		*/
				/****************************************/

    RTF_SECT_ENUM( "sftntj",		SPpropFOOTNOTE_JUSTIFICATION,
						    FTNjustifyBELOW_TEXT ),
    RTF_SECT_ENUM( "sftnbj",		SPpropFOOTNOTE_JUSTIFICATION,
						    FTNjustifyPAGE_BOTTOM ),

    RTF_SECT_NUMBER( "sftnstart",	SPpropFOOTNOTE_STARTNR ),
    RTF_SECT_NUMBER( "saftnstart",	SPpropENDNOTE_STARTNR ),

    RTF_SECT_ENUM( "sftnrstpg",		SPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_PAGE ),
    RTF_SECT_ENUM( "sftnrestart",	SPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_SECTION ),
    RTF_SECT_ENUM( "sftnrstcont",	SPpropFOOTNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),

    RTF_SECT_ENUM( "saftnrstcont",	SPpropENDNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),
    RTF_SECT_ENUM( "saftnrestart",	SPpropENDNOTE_RESTART,
						    FTN_RST_PER_SECTION ),

    RTF_SECT_ENUM( "sftnnar",		SPpropFOOTNOTE_STYLE, FTNstyleNAR ),
    RTF_SECT_ENUM( "sftnnalc",		SPpropFOOTNOTE_STYLE, FTNstyleNALC ),
    RTF_SECT_ENUM( "sftnnauc",		SPpropFOOTNOTE_STYLE, FTNstyleNAUC ),
    RTF_SECT_ENUM( "sftnnrlc",		SPpropFOOTNOTE_STYLE, FTNstyleNRLC ),
    RTF_SECT_ENUM( "sftnnruc",		SPpropFOOTNOTE_STYLE, FTNstyleNRUC ),
    RTF_SECT_ENUM( "sftnnchi",		SPpropFOOTNOTE_STYLE, FTNstyleNCHI ),

    RTF_SECT_ENUM( "saftnnar",		SPpropENDNOTE_STYLE, FTNstyleNAR ),
    RTF_SECT_ENUM( "saftnnalc",		SPpropENDNOTE_STYLE, FTNstyleNALC ),
    RTF_SECT_ENUM( "saftnnauc",		SPpropENDNOTE_STYLE, FTNstyleNAUC ),
    RTF_SECT_ENUM( "saftnnrlc",		SPpropENDNOTE_STYLE, FTNstyleNRLC ),
    RTF_SECT_ENUM( "saftnnruc",		SPpropENDNOTE_STYLE, FTNstyleNRUC ),
    RTF_SECT_ENUM( "saftnnchi",		SPpropENDNOTE_STYLE, FTNstyleNCHI ),

				/****************************************/
				/*  Document properties: (Notes)	*/
				/****************************************/

    RTF_DOC_NUMBER( RTFtag_fet,		DPpropNOTETYPES ),

    RTF_DOC_ENUM( "endnotes",		DPpropFOOTNOTE_PLACEMENT,
						    FTNplaceSECT_END ),
    RTF_DOC_ENUM( "enddoc",		DPpropFOOTNOTE_PLACEMENT,
						    FTNplaceDOC_END ),
    RTF_DOC_ENUM( "ftntj",		DPpropFOOTNOTE_JUSTIFICATION,
						    FTNjustifyBELOW_TEXT ),
    RTF_DOC_ENUM( "ftnbj",		DPpropFOOTNOTE_JUSTIFICATION,
						    FTNjustifyPAGE_BOTTOM ),

    RTF_DOC_ENUM( "aendnotes",		DPpropENDNOTE_PLACEMENT,
						    FTNplaceSECT_END ),
    RTF_DOC_ENUM( "aenddoc",		DPpropENDNOTE_PLACEMENT,
						    FTNplaceDOC_END ),
    RTF_DOC_ENUM( "aftntj",		DPpropENDNOTE_JUSTIFICATION,
						    FTNjustifyBELOW_TEXT ),
    RTF_DOC_ENUM( "aftnbj",		DPpropENDNOTE_JUSTIFICATION,
						    FTNjustifyPAGE_BOTTOM ),

    RTF_DOC_NUMBER( "ftnstart",		DPpropFOOTNOTE_STARTNR ),
    RTF_DOC_NUMBER( "aftnstart",	DPpropENDNOTE_STARTNR ),

    RTF_DOC_ENUM( "ftnrstpg",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_PAGE ),
    RTF_DOC_ENUM( "ftnrestart",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_PER_SECTION ),
    RTF_DOC_ENUM( "ftnrstcont",		DPpropFOOTNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),

    RTF_DOC_ENUM( "aftnrstcont",	DPpropENDNOTE_RESTART,
						    FTN_RST_CONTINUOUS ),
    RTF_DOC_ENUM( "aftnrestart",	DPpropENDNOTE_RESTART,
						    FTN_RST_PER_SECTION ),

    RTF_DOC_ENUM( "ftnnar",		DPpropFOOTNOTE_STYLE, FTNstyleNAR ),
    RTF_DOC_ENUM( "ftnnalc",		DPpropFOOTNOTE_STYLE, FTNstyleNALC ),
    RTF_DOC_ENUM( "ftnnauc",		DPpropFOOTNOTE_STYLE, FTNstyleNAUC ),
    RTF_DOC_ENUM( "ftnnrlc",		DPpropFOOTNOTE_STYLE, FTNstyleNRLC ),
    RTF_DOC_ENUM( "ftnnruc",		DPpropFOOTNOTE_STYLE, FTNstyleNRUC ),
    RTF_DOC_ENUM( "ftnnchi",		DPpropFOOTNOTE_STYLE, FTNstyleNCHI ),

    RTF_DOC_ENUM( "aftnnar",		DPpropENDNOTE_STYLE, FTNstyleNAR ),
    RTF_DOC_ENUM( "aftnnalc",		DPpropENDNOTE_STYLE, FTNstyleNALC ),
    RTF_DOC_ENUM( "aftnnauc",		DPpropENDNOTE_STYLE, FTNstyleNAUC ),
    RTF_DOC_ENUM( "aftnnrlc",		DPpropENDNOTE_STYLE, FTNstyleNRLC ),
    RTF_DOC_ENUM( "aftnnruc",		DPpropENDNOTE_STYLE, FTNstyleNRUC ),
    RTF_DOC_ENUM( "aftnnchi",		DPpropENDNOTE_STYLE, FTNstyleNCHI ),

				/****************************************/
				/*  Footnotes/Endnotes.			*/
				/****************************************/

# define	RTF_NOTE_ENUM(s,id,v) \
		    { \
		    s, RTCscopeNOTE, id, RTCtypeENUM, \
		    docRtfRememberNoteProperty, v, \
		    }

# define	RTF_NOTE_FLAG(s,id) \
		    { \
		    s, RTCscopeNOTE, id, RTCtypeFLAG, \
		    docRtfRememberNoteProperty, \
		    }

    RTF_NOTE_ENUM( "ftnalt",	NOTEpropTREE_TYPE,	DOCinENDNOTE ),
						/**
						 * For undo/redo only:
						 * Cause the masks to be set.
						 */
    RTF_NOTE_ENUM( "NoFtnalt",		NOTEpropTREE_TYPE,	DOCinFOOTNOTE ),
    RTF_NOTE_FLAG( "FtnAuto",		NOTEpropAUTO_NUMBER ),

				/****************************************/
				/*  StyleSheet.				*/
				/****************************************/
# define	RTF_STYLE_FLAG(s,id) \
		    { \
		    s, RTCscopeSTYLE, id, RTCtypeFLAG, \
		    docRtfRememberStyleProperty, \
		    }

# define	RTF_STYLE_NUMBER(s,id) \
		    { \
		    s, RTCscopeSTYLE, id, RTCtypeNUMBER, \
		    docRtfRememberStyleProperty, \
		    }
# define	RTF_STYLE_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberStyleProperty, \
		    }

    RTF_STYLE_FLAG( "additive",		DSpropADDITIVE ),
    RTF_STYLE_NUMBER( "sbasedon",	DSpropBASED_ON ),
    RTF_STYLE_NUMBER( "snext",		DSpropNEXT ),
    RTF_STYLE_NUMBER( "slink",		DSpropLINK ),

    /*

    Handeled for the style where the reference to the style is handeled.

    RTF_STYLE_VAL_ENUM( "ds",		DSpropSTYLE_NUMBER, DOClevSECT ),
    RTF_STYLE_VAL_ENUM( "ts",		DSpropSTYLE_NUMBER, DOClevROW ),
    RTF_STYLE_VAL_ENUM( "s",		DSpropSTYLE_NUMBER, DOClevPARA ),
    RTF_STYLE_VAL_ENUM( "cs",		DSpropSTYLE_NUMBER, DOClevSPAN ),
    */

    RTF_STYLE_FLAG( "sautoupd",		DSpropAUTO_UPD ),
    RTF_STYLE_FLAG( "shidden",		DSpropHIDDEN ),
    RTF_STYLE_FLAG( "slocked",		DSpropLOCKED ),
    RTF_STYLE_FLAG( "ssemihidden",	DSpropSEMI_HIDDEN ),
    RTF_STYLE_FLAG( "spersonal",	DSpropPERSONAL ),

    RTF_STYLE_NUMBER_X( "spriority",	DSpropIGNORED ),
    RTF_STYLE_NUMBER_X( "sqformat",	DSpropIGNORED ),
    RTF_STYLE_NUMBER_X( "sunhideused",	DSpropIGNORED ),

				/*
				 *  Encoding.
				 */
    RTF_DOC_ENUM( "ansi",		DPpropDOC_CHARSET, DOCcharsetANSI ),
    RTF_DOC_ENUM( "mac",		DPpropDOC_CHARSET, DOCcharsetMAC ),
    RTF_DOC_ENUM( "pc",			DPpropDOC_CHARSET, DOCcharsetPC ),
    RTF_DOC_ENUM( "pca",		DPpropDOC_CHARSET, DOCcharsetPCA ),
    RTF_DOC_NUMBER( "ansicpg",		DPpropANSICPG ),

    RTF_DOC_NUMBER( "deflang",		DPpropDEFLANG ),

				/****************************************/
				/*  Colors in the color table.		*/
				/****************************************/
# define	RTF_COLOR_NUMBER(s,id) \
		    { \
		    s, RTCscopeCOLOR, id, RTCtypeNUMBER, \
		    docRtfColorComp, \
		    }

# define	RTF_COLOR_IGNORED(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeANY, \
		    docRtfColorIgnored, \
		    }

    RTF_COLOR_NUMBER( "red",		RGBAcompRED ),
    RTF_COLOR_NUMBER( "green",		RGBAcompGREEN ),
    RTF_COLOR_NUMBER( "blue",		RGBAcompBLUE ),

    RTF_COLOR_IGNORED( "caccentone",		1 ),
    RTF_COLOR_IGNORED( "caccenttwo",		1 ),
    RTF_COLOR_IGNORED( "caccentthree",		1 ),
    RTF_COLOR_IGNORED( "caccentfour",		1 ),
    RTF_COLOR_IGNORED( "caccentfive",		1 ),
    RTF_COLOR_IGNORED( "caccentsix",		1 ),
    RTF_COLOR_IGNORED( "chyperlink",		1 ),
    RTF_COLOR_IGNORED( "cfollowedhyperlink",	1 ),
    RTF_COLOR_IGNORED( "cbackgroundone",	1 ),
    RTF_COLOR_IGNORED( "ctextone",		1 ),
    RTF_COLOR_IGNORED( "cbackgroundtwo",	1 ),
    RTF_COLOR_IGNORED( "ctexttwo",		1 ),
    RTF_COLOR_IGNORED( "ctint",			1 ),
    RTF_COLOR_IGNORED( "cshade",		1 ),

				/****************************************/
				/*  Fields.				*/
				/****************************************/

# define	RTF_FIELD_FLAG(s,id) \
		    { \
		    s, RTCscopeFIELD, id, RTCtypeFLAG, \
		    docRtfRememberFieldProperty, \
		    }

# define	RTF_FIELD_NUMBER(s,id) \
		    { \
		    s, RTCscopeFIELD, id, RTCtypeNUMBER, \
		    docRtfRememberFieldProperty, \
		    }

    RTF_FIELD_FLAG( "flddirty",		FPpropDIRTY ),
    RTF_FIELD_FLAG( "fldedit",		FPpropEDITED ),
    RTF_FIELD_FLAG( "fldlock",		FPpropLOCKED ),
    RTF_FIELD_FLAG( "fldpriv",		FPpropPRIVATE ),

    RTF_FIELD_NUMBER( "bkmkcolf",	FPpropFIRST_COLUMN ),
    RTF_FIELD_NUMBER( "bkmkcoll",	FPpropLAST_COLUMN ),

				/****************************************/
				/*  Unknown				*/
				/****************************************/
    RTF_REMEMBER( "lin",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "rin",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "langnp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "langfe",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "langfenp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "langf",		RTFid_NOT_SUPPORTED ), /* ? */
    RTF_REMEMBER( "nopro",		RTFid_NOT_SUPPORTED ), /* ? */
    RTF_REMEMBER( "nolnhtadjtbl",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "themelang",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "themelangfe",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "themelangcs",	RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Document Formatting properties.	*/
				/****************************************/
    RTF_REMEMBER( "allprot",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "annotprot",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "brkfrm",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "cvmme",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "defformat",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "fldalt",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "formdisp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "formprot",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "formshade",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "fracwidth",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "hyphauto",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "hyphcaps",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "hyphconsec",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "hyphhotz",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "landscape",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linestart",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "linkstyles",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "makebackup",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "nocolbal",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "noextrasprl",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "notabind",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "otblrul",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "prcolbl",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "printdata",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "psover",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revbar",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revisions",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revprop",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "revprot",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sprslnsp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sprsspbf",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sprstsp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "subfontbysize",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "swpbdr",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "transmf",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ight",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "wraptrsp",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "viewzk",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "viewkind",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "viewscale",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "lytprtmet",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ogutter",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "rtlgutter",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "deflangfe",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "uc",			RTFidUC ),
    RTF_REMEMBER( "level",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "noline",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sbys",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "subdocument",	RTFid_NOT_SUPPORTED ),

    RTF_REMEMBER( "donotembedsysfont",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "donotembedlingdata",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "grfdocevents",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "validatexml",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "showplaceholdtext",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ignoremixedcontent",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "saveinvalidxml",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "showxmlerrors",	RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "ilfomacatclnup",	RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
# define	RTF_TIME_NUMBER(s,id) \
		    { \
		    s, RTCscopeTIME, id, RTCtypeNUMBER, \
		    docRtfReadTimeField, \
		    }

    RTF_TIME_NUMBER( "hr",		DTfieldHR ),
    RTF_TIME_NUMBER( "min",		DTfieldMIN ),
    RTF_TIME_NUMBER( "sec",		DTfieldSEC ),
    RTF_TIME_NUMBER( "dy",		DTfieldDY ),
    RTF_TIME_NUMBER( "mo",		DTfieldMO ),
    RTF_TIME_NUMBER( "yr",		DTfieldYR ),

				/****************************************/
				/*  Unsupported W2002 revision		*/
				/*  control.				*/
				/****************************************/
    RTF_REMEMBER( "insrsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "rsidroot",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "delrsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "charrsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "sectrsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "pararsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "tblrsid",		RTFid_NOT_SUPPORTED ),
    RTF_REMEMBER( "styrsid",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  List table &c.			*/
				/****************************************/

# define	RTF_LISTLEV_NUMBER(s,id) \
		    { \
		    s, RTCscopeLIST_LEVEL, id, RTCtypeNUMBER, \
		    docRtfRememberListLevelProperty, \
		    }
# define	RTF_LISTLEV_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfRememberListLevelProperty, \
		    }

# define	RTF_LIST_NUMBER(s,id) \
		    { \
		    s, RTCscopeLIST, id, RTCtypeNUMBER, \
		    docRtfRememberListProperty, \
		    }

# define	RTF_LIST_ENUM(s,id,v) \
		    { \
		    s, RTCscopeLIST, id, RTCtypeENUM, \
		    docRtfRememberListProperty, v, \
		    }

# define	RTF_LIST_FLAG(s,id) \
		    { \
		    s, RTCscopeLIST, id, RTCtypeFLAG, \
		    docRtfRememberListProperty, \
		    }
# define	RTF_LIST_FLAG_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfRememberListProperty, \
		    }

# define	RTF_LISTOVERR_NUMBER(s,id) \
		    { \
		    s, RTCscopeLISTOVERRIDE, id, RTCtypeNUMBER, \
		    docRtfRememberListOverrideProperty, \
		    }

# define	RTF_LISTOVERRLEV_FLAG(s,id) \
		    { \
		    s, RTCscopeLISTOVERRIDE_LEVEL, id, RTCtypeFLAG, \
		    docRtfRememberListOverrideLevelProperty, \
		    }

    RTF_DEST_XX( "list", RTCscopeLIST_TABLE, DPpropLISTTABLE,
					    docRtfRememberList ),
    RTF_DEST_XX( "listlevel", RTCscopeLIST, DLpropLEVELS,
					    docRtfRememberListLevel ),

    RTF_TEXT_GROUP( "leveltext", RTCscopeLIST_LEVEL, LLpropTEXT,
					    docRtfCommitListLevelText ),
    RTF_BYTE_GROUP( "levelnumbers", RTCscopeLIST_LEVEL,	LLpropNUMBERS,
					    docRtfCommitListLevelNumbers ),

    RTF_TEXT_GROUP( "listname", RTCscopeLIST, DLpropNAME,
					    docRtfCommitListName ),
    RTF_TEXT_GROUP( "liststylename", RTCscopeLIST, DLpropSTYLENAME,
					    docRtfCommitListStyleName ),

    RTF_LISTLEV_NUMBER( "levelstartat",		LLpropSTARTAT ),
    RTF_LISTLEV_NUMBER( "levelnfcn",		LLpropSTYLE ),
    RTF_LISTLEV_NUMBER_X( "levelnfc",		LLpropSTYLE ),		/**/
    RTF_LISTLEV_NUMBER( "leveljcn",		LLpropJUSTIFY ),
    RTF_LISTLEV_NUMBER_X( "leveljc",		LLpropJUSTIFY ),	/**/
    RTF_LISTLEV_NUMBER( "levelfollow",		LLpropFOLLOW ),
    RTF_LISTLEV_NUMBER( "levellegal",		LLpropLEGAL ),
    RTF_LISTLEV_NUMBER( "levelnorestart",	LLpropNORESTART ),
    RTF_LISTLEV_NUMBER( "levelpicture",		LLpropPICTURE ),
    RTF_LISTLEV_NUMBER( "fbias",		LLpropFBIAS ),

    RTF_LIST_FLAG_X( "lvltentative",		RTFid_NOT_SUPPORTED ),

    RTF_LISTLEV_NUMBER( "leveltemplateid",	LLpropTEMPLATEID ),

    RTF_LISTLEV_NUMBER_X( "levelold",		LLpropOLD ),		/**/
    RTF_LISTLEV_NUMBER_X( "levelprev",		LLpropPREV ),		/**/
    RTF_LISTLEV_NUMBER_X( "levelprevspace",	LLpropPREVSPACE ),	/**/
    RTF_LISTLEV_NUMBER_X( "levelindent",	LLpropINDENT ),		/**/
    RTF_LISTLEV_NUMBER_X( "levelspace",		LLpropSPACE ),		/**/

    RTF_LIST_NUMBER( "listid",			DLpropLISTID ),
    RTF_LIST_NUMBER( "listtemplateid",		DLpropTEMPLATEID ),
    RTF_LIST_ENUM( "listsimple",		DLpropHYBRID, 0 ),
    RTF_LIST_ENUM( "listhybrid",		DLpropHYBRID, 1 ),
    RTF_LIST_FLAG( "listrestarthdn",		DLpropRESTARTHDN ),
    RTF_LIST_NUMBER( "liststyleid",		DLpropSTYLEID ),

    RTF_LISTOVERR_NUMBER( "listoverridecount",	LOpropOVERRIDECOUNT ),
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
				/****************************************/

# define	RTF_TAB_ENUM(s,id,v) \
		    { \
		    s, RTCscopeTAB, id, RTCtypeENUM, \
		    docRtfRememberTabStopProperty, v \
		    }

# define	RTF_TAB_FLAG(s,id) \
		    { \
		    s, RTCscopeTAB, id, RTCtypeFLAG, \
		    docRtfRememberTabStopProperty \
		    }

# define	RTF_TAB_NUMBER(s,id) \
		    { \
		    s, RTCscopeTAB, id, RTCtypeNUMBER, \
		    docRtfRememberTabStopProperty, \
		    }

    RTF_TAB_ENUM( RTFtag_tql,	TABpropALIGN,	DOCtaLEFT ),
    RTF_TAB_ENUM( RTFtag_tqr,	TABpropALIGN,	DOCtaRIGHT ),
    RTF_TAB_ENUM( RTFtag_tqc,	TABpropALIGN,	DOCtaCENTER ),
    RTF_TAB_ENUM( RTFtag_tqdec,	TABpropALIGN,	DOCtaDECIMAL ),

    RTF_TAB_ENUM( "tldot",	TABpropLEADER,	DOCtlDOTS ),
    RTF_TAB_ENUM( "tlul",	TABpropLEADER,	DOCtlUNDERLINE ),
    RTF_TAB_ENUM( "tlhyph",	TABpropLEADER,	DOCtlHYPH ),
    RTF_TAB_ENUM( "tlth",	TABpropLEADER,	DOCtlTHICK ),
    RTF_TAB_ENUM( "tleq",	TABpropLEADER,	DOCtlEQUAL ),

    RTF_TAB_FLAG( "jclisttab",	TABpropFROM_STYLE ),

    RTF_TAB_NUMBER( "tx",	TABpropOFFSET ),

    RTF_REMEMBER( "tb",		RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Font table.				*/
				/****************************************/
# define	RTF_FONT_ENUM(s,id,v) \
		    { \
		    s, RTCscopeFONT, id, RTCtypeENUM, \
		    docRtfFontProperty, v \
		    }

# define	RTF_FONT_NUMBER(s,id) \
		    { \
		    s, RTCscopeFONT, id, RTCtypeNUMBER, \
		    docRtfFontProperty, \
		    }

    RTF_FONT_ENUM( "fnil",	DFpropFAMILY_STYLE,	DFstyleFNIL ),
    RTF_FONT_ENUM( "froman",	DFpropFAMILY_STYLE,	DFstyleFROMAN ),
    RTF_FONT_ENUM( "fswiss",	DFpropFAMILY_STYLE,	DFstyleFSWISS ),
    RTF_FONT_ENUM( "fmodern",	DFpropFAMILY_STYLE,	DFstyleFMODERN ),
    RTF_FONT_ENUM( "fscript",	DFpropFAMILY_STYLE,	DFstyleFSCRIPT ),
    RTF_FONT_ENUM( "fdecor",	DFpropFAMILY_STYLE,	DFstyleFDECOR ),
    RTF_FONT_ENUM( "ftech",	DFpropFAMILY_STYLE,	DFstyleFTECH ),
    RTF_FONT_ENUM( "fbidi",	DFpropFAMILY_STYLE,	DFstyleFBIDI ),

    RTF_FONT_NUMBER( "fcharset", DFpropCHARSET ),
    RTF_FONT_NUMBER( "cpg",	DFpropCODEPAGE ),
    RTF_FONT_NUMBER( "fprq",	DFpropPITCH ),

    RTF_FONT_ENUM( "ftnil",	DFpropFONT_TYPE,	FONTtypeNIL ),
    RTF_FONT_ENUM( "ftruetype", DFpropFONT_TYPE,	FONTtypeTTF ), /* ! */

    RTF_TEXT_GROUP( "fontemb",	RTCscopeFONT,
				DFpropFONT_BYTES, docRtfCommitFontDest ),

    RTF_TEXT_GROUP( "fontfile",	RTCscopeFONT,
				DFpropFONT_FILE, docRtfCommitFontDest ),
    RTF_TEXT_GROUP( "panose",	RTCscopeFONT,
				DFpropPANOSE,	docRtfCommitFontDest ),
    RTF_TEXT_GROUP( "falt",	RTCscopeFONT,
				DFpropALT_NAME,	docRtfCommitFontDest ),

				/****************************************/
				/*  Pictures/Objects			*/
				/****************************************/

# define	RTF_PICT_TYPE(s,id,v) \
		    { \
		    s, RTCscopePICT, id, RTCtypeANY, \
		    docRtfPictureProperty, v \
		    }

# define	RTF_PICT_NUMBER(s,id) \
		    { \
		    s, RTCscopePICT, id, RTCtypeNUMBER, \
		    docRtfPictureProperty, \
		    }

# define	RTF_PICT_FLAG(s,id) \
		    { \
		    s, RTCscopePICT, id, RTCtypeFLAG, \
		    docRtfPictureProperty, \
		    }

    RTF_PICT_TYPE( "macpict",	PIPpropTYPE,	DOCokMACPICT ),
    RTF_PICT_TYPE( "wmetafile",	PIPpropTYPE,	DOCokPICTWMETAFILE ),
    RTF_PICT_TYPE( "pngblip",	PIPpropTYPE,	DOCokPICTPNGBLIP ),
    RTF_PICT_TYPE( "jpegblip",	PIPpropTYPE,	DOCokPICTJPEGBLIP ),
    RTF_PICT_TYPE( "emfblip",	PIPpropTYPE,	DOCokPICTEMFBLIP ),
    RTF_PICT_TYPE( "dibitmap",	PIPpropTYPE,	DOCokDIBITMAP ),
    RTF_PICT_TYPE( "wbitmap",	PIPpropTYPE,	DOCokWBITMAP ),
    RTF_PICT_TYPE( "pmmetafile",PIPpropTYPE,	DOCokPMMETAFILE ),

    RTF_PICT_NUMBER( "wbmbitspixel",	PIPpropWBMBITSPIXEL ),
    RTF_PICT_NUMBER( "wbmplanes",	PIPpropWBMPLANES ),
    RTF_PICT_NUMBER( "wbmwidthbytes",	PIPpropWBMWIDTHBYTES ),

    RTF_PICT_NUMBER( "picw",		PIPpropPICX_WIN_EXT ),
    RTF_PICT_NUMBER( "pich",		PIPpropPICY_WIN_EXT ),

    RTF_PICT_NUMBER( "picwgoal",	PIPpropPICTWIPS_WIDE ),
    RTF_PICT_NUMBER( "pichgoal",	PIPpropPICTWIPS_HIGH ),

    RTF_PICT_NUMBER( "picscalex",	PIPpropPICSCALE_X ),
    RTF_PICT_NUMBER( "picscaley",	PIPpropPICSCALE_Y ),

    RTF_PICT_NUMBER( "piccropt",	PIPpropPICCROP_TOP ),
    RTF_PICT_NUMBER( "piccropb",	PIPpropPICCROP_BOTTOM ),
    RTF_PICT_NUMBER( "piccropl",	PIPpropPICCROP_LEFT ),
    RTF_PICT_NUMBER( "piccropr",	PIPpropPICCROP_RIGHT ),

    RTF_PICT_FLAG( "defshp",		PIPpropDEFSHP ),

    RTF_PICT_FLAG( "picscaled",		PIPpropPICSCALED ),

    RTF_PICT_NUMBER( "bliptag",		PIPpropBLIPTAG ),

    RTF_PICT_FLAG( "picbmp",		PIPpropPICBMP ),
    RTF_PICT_NUMBER( "picbpp",		PIPpropPICBPP ),
    RTF_PICT_NUMBER( "blipupi",		PIPpropBLIPUPI ),

# define	RTF_OBJ_ENUM(s,id,v) \
		    { \
		    s, RTCscopeOBJ, id, RTCtypeENUM, \
		    docRtfObjectProperty, v \
		    }

# define	RTF_OBJ_NUMBER(s,id) \
		    { \
		    s, RTCscopeOBJ, id, RTCtypeNUMBER, \
		    docRtfObjectProperty, \
		    }
# define	RTF_OBJ_NUMBER_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfObjectProperty, \
		    }

# define	RTF_OBJ_FLAG(s,id) \
		    { \
		    s, RTCscopeOBJ, id, RTCtypeFLAG, \
		    docRtfObjectProperty, \
		    }

    RTF_OBJ_ENUM( "rsltrtf",	IOpropRESULT_KIND,	RESULTkindRTF ),
    RTF_OBJ_ENUM( "rslttxt",	IOpropRESULT_KIND,	RESULTkindTXT ),
    RTF_OBJ_ENUM( "rsltpict",	IOpropRESULT_KIND,	RESULTkindPICT ),
    RTF_OBJ_ENUM( "rsltbmp",	IOpropRESULT_KIND,	RESULTkindBMP ),
    RTF_OBJ_ENUM( "rslthtml",	IOpropRESULT_KIND,	RESULTkindHTML ),

    RTF_OBJ_ENUM( "objemb",	IOpropEMBED_KIND, EMBEDkindOBJEMB ),
    RTF_OBJ_ENUM( "objlink",	IOpropEMBED_KIND, EMBEDkindOBJLINK ),
    RTF_OBJ_ENUM( "objautlink",	IOpropEMBED_KIND, EMBEDkindOBJAUTLINK ),
    RTF_OBJ_ENUM( "objsub",	IOpropEMBED_KIND, EMBEDkindOBJSUB ),
    RTF_OBJ_ENUM( "objpub",	IOpropEMBED_KIND, EMBEDkindOBJPUB ),
    RTF_OBJ_ENUM( "objicemb",	IOpropEMBED_KIND, EMBEDkindOBJICEMB ),
    RTF_OBJ_ENUM( "objhtml",	IOpropEMBED_KIND, EMBEDkindOBJHTML ),
    RTF_OBJ_ENUM( "objocx",	IOpropEMBED_KIND, EMBEDkindOBJOCX ),

    RTF_OBJ_NUMBER( "objw",		IOpropOBJTWIPS_WIDE ),
    RTF_OBJ_NUMBER( "objh",		IOpropOBJTWIPS_HIGH ),

    RTF_OBJ_NUMBER( "objscalex",	IOpropOBJSCALE_X ),
    RTF_OBJ_NUMBER( "objscaley",	IOpropOBJSCALE_Y ),

    RTF_OBJ_NUMBER( "objcropt",		IOpropOBJCROP_TOP ),
    RTF_OBJ_NUMBER( "objcropb",		IOpropOBJCROP_BOTTOM ),
    RTF_OBJ_NUMBER( "objcropl",		IOpropOBJCROP_LEFT ),
    RTF_OBJ_NUMBER( "objcropr",		IOpropOBJCROP_RIGHT ),

    RTF_OBJ_NUMBER_X( "objsetsize",	IOprop_UNSUPPORTED ),
    RTF_OBJ_NUMBER_X( "rsltmerge",	IOprop_UNSUPPORTED ),

				/****************************************/
				/*  Special characters (mostly ignored)	*/
				/****************************************/
# define	RTF_SPECIAL_CHAR(s,id) \
		    { \
		    s, RTCscopeSPECIAL_CHAR, id, RTCtypeSYMBOL, \
		    docRtfTextSpecialChar, \
		    }
# define	RTF_SPECIAL_CHAR_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeSYMBOL, \
		    docRtfTextSpecialChar, \
		    }

    RTF_SPECIAL_CHAR( "bullet",		0x2022 ),
    RTF_SPECIAL_CHAR( "emdash",		0x2014 ),
    RTF_SPECIAL_CHAR( "emspace",	ISO1_space ),
    RTF_SPECIAL_CHAR_X( "endash",	0x2013 ),
    RTF_SPECIAL_CHAR_X( "enspace",	ISO1_space ),
    RTF_SPECIAL_CHAR( "ldblquote",	ISO1_quotedbl ),
    RTF_SPECIAL_CHAR( "lquote",		ASCII_quoteleft ),
    RTF_SPECIAL_CHAR_X( "rdblquote",	ISO1_quotedbl ),
    RTF_SPECIAL_CHAR( "rquote",		ISO1_quotesingle ),

    RTF_SPECIAL_CHAR_X( "softcol",	ISO1_space ),
    RTF_SPECIAL_CHAR_X( "softlheight",	ISO1_space ),
    RTF_SPECIAL_CHAR_X( "softline",	ISO1_space ),
    RTF_SPECIAL_CHAR_X( "softpage",	ISO1_space ),
    RTF_SPECIAL_CHAR_X( "zwj",		ISO1_space ),
    RTF_SPECIAL_CHAR_X( "zwnj",		ISO1_space ),

# define	RTF_UNICODE(s,id) \
		{ \
		s, RTCscopeSPECIAL_CHAR, id, RTCtypeNUMBER, \
		docRtfTextUnicode, \
		}

    RTF_UNICODE( "u",			ISO1_u ),

# define	RTF_SPECIAL_PART(s,id) \
		    { \
		    s, RTCscopeSPECIAL_PARTICULE, id, RTCtypeANY, \
		    docRtfTextSpecialParticule, \
		    }
# define	RTF_SPECIAL_PART_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeANY, \
		    docRtfTextSpecialParticule, \
		    }

    RTF_SPECIAL_PART( "-",		TPkindOPT_HYPH ),
    RTF_SPECIAL_PART( "tab",		TPkindTAB ),
    RTF_SPECIAL_PART( "line",		TPkindLINEBREAK ),
    RTF_SPECIAL_PART_X( "lbr",		TPkindLINEBREAK ),
    RTF_SPECIAL_PART( "chftnsep",	TPkindCHFTNSEP ),
    RTF_SPECIAL_PART( "chftnsepc",	TPkindCHFTNSEPC ),
    RTF_SPECIAL_PART( "ltrmark",	TPkindLTR_MARK ),
    RTF_SPECIAL_PART( "rtlmark",	TPkindRTL_MARK ),

# define	RTF_BREAK_PART(s,id) \
		    { \
		    s, RTCscopeSPECIAL_PARTICULE, id, RTCtypeANY, \
		    docRtfBreakParticule, \
		    }

    RTF_BREAK_PART(   "page",		TPkindPAGEBREAK ),
    RTF_BREAK_PART(   "column",		TPkindCOLUMNBREAK ),

				/****************************************/
				/*  Word 95 drawing objects.		*/
				/****************************************/
# define	RTF_DO_FLAG(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeFLAG, \
		    docRtfDrawingObjectProperty \
		    }

# define	RTF_DO_ENUM(s,id,v) \
		    { \
		    s, RTCscopeANY, id, RTCtypeENUM, \
		    docRtfDrawingObjectProperty, v \
		    }

# define	RTF_DO_NUMBER(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfDrawingObjectProperty \
		    }

# define	RTF_DO_COORD(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeNUMBER, \
		    docRtfDrawingObjectCoordinate \
		    }

    RTF_DO_FLAG( "dolock",	DOpropANCHOR_LOCKED ),

    RTF_DO_ENUM( "dobxpage",	DOpropX_ATTACH,		FXrefPAGE ),
    RTF_DO_ENUM( "dobxcolumn",	DOpropX_ATTACH,		FXrefCOLUMN ),
    RTF_DO_ENUM( "dobxmargin",	DOpropX_ATTACH,		FXrefMARGIN ),

    RTF_DO_ENUM( "dobypage",	DOpropY_ATTACH,		FYrefPAGE ),
    RTF_DO_ENUM( "dobypara",	DOpropY_ATTACH,		FYrefPARA ),
    RTF_DO_ENUM( "dobymargin",	DOpropY_ATTACH,		FYrefMARGIN ),

    RTF_DO_ENUM( "dparc",	DOpropKIND, SHPtyARC ),
    RTF_DO_ENUM( "dpcallout",	DOpropKIND, SHPtyCALLOUT_1 ),
    RTF_DO_ENUM( "dpellipse",	DOpropKIND, SHPtyELLIPSE ),
    RTF_DO_ENUM( "dpline",	DOpropKIND, SHPtyLINE ),
    RTF_DO_ENUM( "dppolygon",	DOpropKIND, SHPtyFREEFORM_OR_NON_AUTOSHAPE ),
    RTF_DO_ENUM( "dppolyline",	DOpropKIND, SHPtyFREEFORM_OR_NON_AUTOSHAPE ),
    RTF_DO_ENUM( "dprect",	DOpropKIND, SHPtyRECTANGLE ),
    RTF_DO_ENUM( "dptxbx",	DOpropKIND, SHPtyTEXT_BOX ),

    RTF_DO_ENUM( "dplinesolid",	DOpropLINE_STYLE,	DSdashSOLID ),
    RTF_DO_ENUM( "dplinehollow",DOpropLINE_STYLE,	DSdashHOLLOW ),
    RTF_DO_ENUM( "dplinedash",	DOpropLINE_STYLE,	DSdashDASHED ),
    RTF_DO_ENUM( "dplinedot",	DOpropLINE_STYLE,	DSdashDOT ),
    RTF_DO_ENUM( "dplinedado",	DOpropLINE_STYLE,	DSdashDASHDOT ),
    RTF_DO_ENUM( "dplinedadodo",DOpropLINE_STYLE,	DSdashDASHDOTDOT ),

    RTF_DO_ENUM( "dpaendhol",	DOpropEND_ARROW_HEAD,	DSarrowOPEN_ARROW ),
    RTF_DO_ENUM( "dpaendsol",	DOpropEND_ARROW_HEAD,	DSarrowARROW ),
    RTF_DO_ENUM( "dpastarthol",	DOpropSTART_ARROW_HEAD,	DSarrowOPEN_ARROW ),
    RTF_DO_ENUM( "dpastartsol",	DOpropSTART_ARROW_HEAD,	DSarrowARROW ),

    RTF_DO_NUMBER( "dpaendl",	DOpropEND_ARROW_LENGTH ),
    RTF_DO_NUMBER( "dpastartl",	DOpropSTART_ARROW_LENGTH ),
    RTF_DO_NUMBER( "dpaendw",	DOpropEND_ARROW_WIDTH ),
    RTF_DO_NUMBER( "dpastartw",	DOpropSTART_ARROW_WIDTH ),

    RTF_DO_NUMBER( "dpfillpat",	DOpropFILL_PATTERN ),

    RTF_DO_FLAG( "dparcflipx",	DOpropARC_FLIP_X ),
    RTF_DO_FLAG( "dparcflipy",	DOpropARC_FLIP_Y ),

    RTF_DO_NUMBER( "dpx",	DOpropX ),
    RTF_DO_NUMBER( "dpy",	DOpropY ),
    RTF_DO_NUMBER( "dodhgt",	DOpropZ ),
    RTF_DO_NUMBER( "dpxsize",	DOpropWIDE ),
    RTF_DO_NUMBER( "dpysize",	DOpropHIGH ),

    RTF_DO_NUMBER( "dptxbxmar",	DOpropTEXT_BOX_MARGIN ),

    RTF_DO_NUMBER( "dplinew",	DOpropLINE_WIDTH ),
    RTF_DO_NUMBER( "dppolycount",DOpropPOINT_COUNT ),

    RTF_DO_NUMBER( "dplinecor",	DOpropLINE_RED ),
    RTF_DO_NUMBER( "dplinecog",	DOpropLINE_GREEN ),
    RTF_DO_NUMBER( "dplinecob",	DOpropLINE_BLUE ),
    RTF_DO_NUMBER( "dplinegray",	DOpropLINE_GRAY ),

    RTF_DO_NUMBER( "dpfillfgcr",	DOpropFILL_FORE_RED ),
    RTF_DO_NUMBER( "dpfillfgcg",	DOpropFILL_FORE_GREEN ),
    RTF_DO_NUMBER( "dpfillfgcb",	DOpropFILL_FORE_BLUE ),
    RTF_DO_NUMBER( "dpfillfggray",	DOpropFILL_FORE_GRAY ),

    RTF_DO_NUMBER( "dpfillbgcr",	DOpropFILL_BACK_RED ),
    RTF_DO_NUMBER( "dpfillbgcg",	DOpropFILL_BACK_GREEN ),
    RTF_DO_NUMBER( "dpfillbgcb",	DOpropFILL_BACK_BLUE ),
    RTF_DO_NUMBER( "dpfillbggray",	DOpropFILL_BACK_GRAY ),

    RTF_DO_COORD( "dpptx",	DOpropX ),
    RTF_DO_COORD( "dppty",	DOpropY ),

    RTF_DO_ENUM( "dptxlrtb",	DOpropTEXT_FLOW, TXflowTXLRTB ),
    RTF_DO_ENUM( "dptxtbrl",	DOpropTEXT_FLOW, TXflowTXTBRL ),
    RTF_DO_ENUM( "dptxbtlr",	DOpropTEXT_FLOW, TXflowTXBTLR ),
    RTF_DO_ENUM( "dptxlrtbv",	DOpropTEXT_FLOW, TXflowTXLRTBV ),
    RTF_DO_ENUM( "dptxtbrlv",	DOpropTEXT_FLOW, TXflowTXTBRLV ),

				/****************************************/
				/*  Shapes.				*/
				/****************************************/

# define	RTF_SHAPE_NUMBER(s,id) \
		    { \
		    s, RTCscopeROOT_SHAPE, id, RTCtypeNUMBER, \
		    docRtfShpProperty, \
		    }

# define	RTF_SHAPE_FLAG(s,id) \
		    { \
		    s, RTCscopeROOT_SHAPE, id, RTCtypeFLAG, \
		    docRtfShpProperty, \
		    }

# define	RTF_SHAPE_ENUM(s,id,v) \
		    { \
		    s, RTCscopeROOT_SHAPE, id, RTCtypeENUM, \
		    docRtfShpProperty, v, \
		    }

    RTF_SHAPE_NUMBER( "shpleft",	SHPpropLEFT ),
    RTF_SHAPE_NUMBER( "shptop",		SHPpropTOP ),
    RTF_SHAPE_NUMBER( "shpbottom",	SHPpropBOTTOM ),
    RTF_SHAPE_NUMBER( "shpright",	SHPpropRIGHT ),

    RTF_SHAPE_NUMBER( "shplid",		SHPpropLID ),
    RTF_SHAPE_NUMBER( "shpz",		SHPpropZ ),

    RTF_SHAPE_FLAG( "shpfhdr",		SHPpropFHDR ),

    RTF_SHAPE_ENUM( "shpbxmargin",	SHPpropBX,	FXrefMARGIN ),
    RTF_SHAPE_ENUM( "shpbxpage",	SHPpropBX,	FXrefPAGE ),
    RTF_SHAPE_ENUM( "shpbxcolumn",	SHPpropBX,	FXrefCOLUMN ),
    /*RTF_SHAPE_ENUM( "shpbx?",		SHPpropBX,	FXrefCHARACTER ),*/
    RTF_SHAPE_ENUM( "shpbxignore",	SHPpropBX,	FXrefIGNORE ),

    RTF_SHAPE_ENUM( "shpbymargin",	SHPpropBY,	FYrefMARGIN ),
    RTF_SHAPE_ENUM( "shpbypage",	SHPpropBY,	FYrefPAGE ),
    RTF_SHAPE_ENUM( "shpbypara",	SHPpropBY,	FYrefPARA ),
    /*RTF_SHAPE_ENUM( "shpby?",		SHPpropBY,	FYrefLINE ),*/
    RTF_SHAPE_ENUM( "shpbyignore",	SHPpropBY,	FYrefIGNORE ),

    RTF_SHAPE_NUMBER( "shpwr",		SHPpropWR ),
    RTF_SHAPE_NUMBER( "shpwrk",		SHPpropWRK ),
    RTF_SHAPE_NUMBER( "shpfblwtxt",	SHPpropFBLWTXT ),

    RTF_SHAPE_FLAG( "shplockanchor",	SHPpropLOCKANCHOR ),

				/****************************************/
				/*  Silently ignore \upr.		*/
				/****************************************/
    RTF_REMEMBER( "upr",	RTFid_NOT_SUPPORTED ),

				/****************************************/
				/*  Various destinations without a	*/
				/*  dedicated scope.			*/
				/****************************************/

    RTF_DEST_XX( "fonttbl",	RTCscopeDOC,
				DPpropFONTTABLE,	docRtfFontTable ),
    RTF_DEST_XX( "colortbl",	RTCscopeDOC,
				DPpropCOLORTABLE,	docRtfColorTable ),
    RTF_DEST_XX( "stylesheet",	RTCscopeDOC,
				DPpropSTYLESHEET,	docRtfStylesheet ),
    RTF_DEST_XX( "listtable",	RTCscopeDOC,
				DPpropLISTTABLE,	docRtfListTable ),
    RTF_DEST_XX( "listoverridetable", RTCscopeDOC,
				DPpropLISTOVERRIDETABLE,
						docRtfListOverrideTable ),
    /* Not ready yet
    RTF_DEST_XX( "userprops",	RTCscopeDOC,
				DPpropUSERPROPS,	docRtfUserProperties ),
    */
    RTF_DEST_XX( "info",	RTCscopeDOC,
				DPprop_INFO,	docRtfReadInfo ),
    RTF_DEST_XX( "revtbl",	RTCscopeDOC,
				DPprop_REVTBL,	docRtfSkipGroup ),

    RTF_DEST_XX( "pn",		RTCscopeANY,
				0, docRtfSkipGroup ),

    RTF_DEST_XX( RTFtag_pict,	RTCscopePARA_INSERT,
				PARAinsertPICT,	docRtfReadPict ),
    RTF_DEST_XX( "do",		RTCscopePARA_INSERT,
				PARAinsertDO,	docRtfReadDrawingObject ),
    RTF_DEST_XX( "nonshppict",	RTCscopePARA_INSERT,
				PARAinsertNONSHPPICT, docRtfReadShpXPict ),
    RTF_DEST_XX( "shppict",	RTCscopePARA_INSERT,
				PARAinsertSHPPICT, docRtfReadShpXPict ),
    RTF_DEST_XX( "object",	RTCscopePARA_INSERT,
				PARAinsertOBJECT, docRtfReadObject ),
    RTF_DEST_XX( "shp",		RTCscopePARA_SHAPE,
				SHPtyUNKNOWN,	docRtfReadShape ),
    RTF_DEST_XX( "shpgrp",	RTCscopePARA_SHAPE,
				SHPtyGROUP,	docRtfReadShape ),

    /*
    RTF_DEST_XX( "nonesttables", 1,		docRtfSkipGroup ),
    */

# define	RTF_FF_NUMBER(s,id) \
		    { \
		    s, RTCscopeFORM_FIELD, id, RTCtypeNUMBER, \
		    docRtfRememberFormFieldProperty, \
		    }

# define	RTF_FF_FLAG(s,id) \
		    { \
		    s, id, RTCscopeFORM_FIELD, RTCtypeFLAG, \
		    docRtfRememberFormFieldProperty, \
		    }

    RTF_DEST_XX( "field",	RTCscopePARA_FIELD,
				DOCfkUNKNOWN, docRtfReadField ),
    RTF_DEST_XX( "fldinst",	RTCscopeFIELD,
				FPpropFLDINST, docRtfReadFldinst ),
    RTF_DEST_XX( "fldrslt",	RTCscopeFIELD,
				FPpropFLDRSLT, docRtfReadFldrslt ),
    RTF_DEST_XX( "datafield",	RTCscopeFIELD,
				FPprop_DATAFIELD, docRtfReadDataField ),
    RTF_DEST_XX( "formfield",	RTCscopeFIELD,
				FPprop_FORMFIELD, docRtfReadFormField ),

    RTF_FF_NUMBER( "fftype",		FFpropTYPE ),
    RTF_FF_NUMBER( "ffownhelp",		FFpropOWN_HELP ),
    RTF_FF_NUMBER( "ffownstat",		FFpropOWN_STAT ),
    RTF_FF_NUMBER( "ffprot",		FFpropPROT ),
    RTF_FF_NUMBER( "ffsize",		FFpropSIZE ),
    RTF_FF_NUMBER( "fftypetxt",		FFpropTYPE_TXT ),
    RTF_FF_NUMBER( "ffrecalc",		FFpropRECALC ),
    RTF_FF_NUMBER( "ffhaslistbox",	FFpropHAS_LISTBOX ),
    RTF_FF_NUMBER( "ffmaxlen",		FFpropMAX_LEN ),
    RTF_FF_NUMBER( "ffhps",		FFpropCHECKBOX_SIZE ),
    RTF_FF_NUMBER( "ffdefres",		FFpropDEF_INDEX ),

    RTF_TEXT_GROUP( "ffname",		RTCscopeFORM_FIELD, FFpropNAME,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffdeftext",	RTCscopeFORM_FIELD, FFpropDEF_TEXT,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffformat",		RTCscopeFORM_FIELD, FFpropFORMAT,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffhelptext",	RTCscopeFORM_FIELD, FFpropHELP_TEXT,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffstattext",	RTCscopeFORM_FIELD, FFpropSTAT_TEXT,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffentrymcr",	RTCscopeFORM_FIELD, FFpropENTRY_MACRO,
					    docRtfCommitFormFieldText ),
    RTF_TEXT_GROUP( "ffexitmcr",	RTCscopeFORM_FIELD, FFpropEXIT_MACRO,
					    docRtfCommitFormFieldText ),

    RTF_DEST_XX( "listtext",	RTCscopePARA_FIELD,
				DOCfkLISTTEXT,	docRtfReadTextField ),
    RTF_DEST_XX( "pntext",	RTCscopeANY,
				DOCfkLISTTEXT,	docRtfReadTextField ),

    RTF_DEST_XX( "xe",		RTCscopePARA_FIELD,
				DOCfkXE,	docRtfLookupEntry ),
    /*
    RTF_DEST_XX( "tc",		RTCscopePARA_FIELD,
				DOCfkTC,	docRtfLookupEntry ),
    RTF_DEST_XX( "tcn",		RTCscopePARA_FIELD,
				DOCfkTCN,	docRtfLookupEntry ),
    */

# define	RTF_SPECIAL_FIELD(s,id) \
		    { \
		    s, RTCscopePARA_FIELD, id, RTCtypeANY, \
		    docRtfTextSpecialToField, \
		    }
# define	RTF_SPECIAL_FIELD_X(s,id) \
		    { \
		    s, RTCscopeANY, id, RTCtypeANY, \
		    docRtfTextSpecialToField, \
		    }

    RTF_SPECIAL_FIELD( "chdate",	DOCfkDATE ),
    RTF_SPECIAL_FIELD( "chpgn",		DOCfkPAGE ),
    RTF_SPECIAL_FIELD( "chtime",	DOCfkTIME ),
    RTF_SPECIAL_FIELD( RTFtag_chftn,	DOCfkCHFTN ),
    RTF_SPECIAL_FIELD( "chatn",		DOCfkCHATN ),
    RTF_SPECIAL_FIELD( "sectnum",	DOCfkSECTION ),

    RTF_SPECIAL_FIELD_X( "chdpa",		DOCfkDATE ),
    RTF_SPECIAL_FIELD_X( "chdpl",		DOCfkDATE ),

    RTF_DEST_XX( "NeXTGraphic",	RTCscopePARA_FIELD,
				DOCfkNEXTGRAPHIC, docRtfReadNeXTGraphic ),

# define	RTF_NEXT_NUMBER(s,id) \
		    { \
		    s, RTCscopeNEXT_GRAPHIC, id, RTCtypeNUMBER, \
		    docRtfNeXTDimension, \
		    }

    RTF_NEXT_NUMBER( "width",	PIPpropPICTWIPS_WIDE ),
    RTF_NEXT_NUMBER( "height",	PIPpropPICTWIPS_HIGH ),


			    /*
			     *  Headers and Footers. Note that 
			     *  'header' and 'footer' are compatibility 
			     *  tags.
			     */
    RTF_DEST_XX( "footer",	RTCscopeANY,
				DOCinRIGHT_FOOTER, docRtfReadExtTree ),
    RTF_DEST_XX( "footerf",	RTCscopeEXT_TREE,
				DOCinFIRST_FOOTER, docRtfReadExtTree ),
    RTF_DEST_XX( "footerl",	RTCscopeEXT_TREE,
				DOCinLEFT_FOOTER,  docRtfReadExtTree ),
    RTF_DEST_XX( "footerr",	RTCscopeEXT_TREE,
				DOCinRIGHT_FOOTER, docRtfReadExtTree ),
    RTF_DEST_XX( "footere",	RTCscopeEXT_TREE,
				DOCinLAST_FOOTER,  docRtfReadExtTree ),

    RTF_DEST_XX( "header",	RTCscopeANY,
				DOCinRIGHT_HEADER, docRtfReadExtTree ),
    RTF_DEST_XX( "headerf",	RTCscopeEXT_TREE,
				DOCinFIRST_HEADER, docRtfReadExtTree ),
    RTF_DEST_XX( "headerl",	RTCscopeEXT_TREE,
				DOCinLEFT_HEADER,  docRtfReadExtTree ),
    RTF_DEST_XX( "headerr",	RTCscopeEXT_TREE,
				DOCinRIGHT_HEADER, docRtfReadExtTree ),
    RTF_DEST_XX( "headere",	RTCscopeEXT_TREE,
				DOCinLAST_HEADER,  docRtfReadExtTree ),

    RTF_DEST_XX( "ftnsep",	RTCscopeEXT_TREE,
				DOCinFTNSEP,	docRtfReadExtTree ),
    RTF_DEST_XX( "ftnsepc",	RTCscopeEXT_TREE,
				DOCinFTNSEPC,	docRtfReadExtTree ),
    RTF_DEST_XX( "ftncn",	RTCscopeEXT_TREE,
				DOCinFTNCN,	docRtfReadExtTree ),

    RTF_DEST_XX( "aftnsep",	RTCscopeEXT_TREE,
				DOCinAFTNSEP,	docRtfReadExtTree ),
    RTF_DEST_XX( "aftnsepc",	RTCscopeEXT_TREE,
				DOCinAFTNSEPC,	docRtfReadExtTree ),
    RTF_DEST_XX( "aftncn",	RTCscopeEXT_TREE,
				DOCinAFTNCN,	docRtfReadExtTree ),

			    /*
			     *  Notes and Annotations
			     */
    RTF_DEST_XX( RTFtag_footnote, RTCscopeEXT_TREE,
				DOCinFOOTNOTE, docRtfReadNote ),
    RTF_DEST_XX( "annotation",	RTCscopeEXT_TREE,
				DOCinANNOTATION, docRtfReadNote ),

			    /****************************************/
			    /*  Multilevel Lists.			*/
			    /****************************************/
    RTF_DEST_XX( "pnseclvl",	RTCscopeANY, 0,	docRtfSkipGroup ),

			    /*
			     *  Bookmarks
			     */
    RTF_DEST_XX( "bkmkstart",	RTCscopePARA_INSERT,
				PARAinsertBKMKSTART, docRtfBkmkStart ),
    RTF_DEST_XX( "bkmkend",	RTCscopePARA_INSERT,
				PARAinsertBKMKEND, docRtfBkmkEnd ),

			    /*
			     *  Shape Properties
			     */
    RTF_DEST_XX( "sp",	RTCscopeREAD_SHAPE, 1,	docRtfShapeProperty ),
    RTF_DEST_XX( "sn",	RTCscopeREAD_SHAPE, 2,	docRtfShapePropertyName ),
    RTF_DEST_XX( "sv",	RTCscopeREAD_SHAPE, 3,	docRtfShapePropertyValue ),

			    /****************************************/
			    /*  To deal with faulty documents.		*/
			    /****************************************/
    /*
    RTF_DEST_XX( "trowd",	RTCscopeROW,
				DOClevROW,	docRtfReadRowProperties ),
    */
    RTF_DEST_XX( "nesttableprops", RTCscopeANY,
				DOClevROW,	docRtfReadRowProperties ),

				/****************************************/
				/*  Inside the \info destination.	*/
				/****************************************/
    RTF_TEXT_GROUP( "title",	RTCscopeDOC,
				DPpropTITLE,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "author",	RTCscopeDOC,
				DPpropAUTHOR,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "company",	RTCscopeDOC,
				DPpropCOMPANY,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "subject",	RTCscopeDOC,
				DPpropSUBJECT,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "keywords",	RTCscopeDOC,
				DPpropKEYWORDS,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "comment",	RTCscopeANY,
				DPpropDOCCOMM,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( "doccomm",	RTCscopeDOC,
				DPpropDOCCOMM,	docRtfCommitDocPropText ),
    RTF_TEXT_GROUP( RTFtag_hlinkbase, RTCscopeDOC,
				DPpropHLINKBASE,docRtfCommitDocPropText ),

				/****************************************/
				/*  Outside the \info destination.	*/
				/****************************************/
    RTF_TEXT_GROUP( "generator",RTCscopeDOC,
				DPpropGENERATOR,docRtfCommitDocPropText ),


    RTF_DEST_XX( "creatim",	RTCscopeDOC,
				DPpropCREATIM,	docRtfDocTimeGroup ),
    RTF_DEST_XX( RTFtag_revtim,	RTCscopeDOC,
				DPpropREVTIM,	docRtfDocTimeGroup ),
    RTF_DEST_XX( "printim",	RTCscopeDOC,
				DPpropPRINTIM,	docRtfDocTimeGroup ),

    RTF_DEST_XX( "operator",	RTCscopeDOC,
				DPprop_OPERATOR,	docRtfSkipGroup ),
    RTF_DEST_XX( "version",	RTCscopeDOC,
				DPprop_VERSION,		docRtfSkipGroup ),
    RTF_DEST_XX( "edmins",	RTCscopeDOC,
				DPprop_EDMINS,		docRtfSkipGroup ),
    RTF_DEST_XX( "nofpages",	RTCscopeDOC,
				DPprop_NOFPAGES,	docRtfSkipGroup ),
    RTF_DEST_XX( "nofwords",	RTCscopeDOC,
				DPprop_NOFWORDS,	docRtfSkipGroup ),
    RTF_DEST_XX( "nofchars",	RTCscopeDOC,
				DPprop_NOFCHARS,	docRtfSkipGroup ),
    RTF_DEST_XX( "nofcharsws",RTCscopeDOC,
				DPprop_NOFCHARSWS,	docRtfSkipGroup ),
    RTF_DEST_XX( "vern",	RTCscopeDOC,
				DPprop_VERN,		docRtfSkipGroup ),

				/****************************************/
				/*  The Undo/Redo/Repeat trace.		*/
				/****************************************/
# define	RTF_OLD_SCOPE_NUMBER(s,id) \
		{ \
		s, RTCscopeTRACE_SCOPE, id, RTCtypeNUMBER, \
		docRtfSelectionScopeProperty, \
		}

# define	RTF_NEW_SCOPE_NUMBER(s,id) \
		{ \
		s, RTCscopeTRACE_SCOPE, SSprop_COUNT+ id, RTCtypeNUMBER, \
		docRtfSelectionScopeProperty, \
		}

# define	RTF_RANGE_NUMBER(s,id) \
		{ \
		s, RTCscopeTRACE_RANGE, id, RTCtypeNUMBER, \
		docRtfEditRangeProperty, \
		}

# define	RTF_RANGE_ENUM(s,id,v) \
		{ \
		s, RTCscopeTRACE_RANGE, id, RTCtypeENUM, \
		docRtfEditRangeProperty, v, \
		}

    RTF_OLD_SCOPE_NUMBER( RTFtag_OTT, SSpropTREE_TYPE ),
    RTF_OLD_SCOPE_NUMBER( RTFtag_OSN, SSpropSECT_NR ),
    RTF_OLD_SCOPE_NUMBER( RTFtag_OOSN, SSpropOWNER_SECT_NR ),
    RTF_OLD_SCOPE_NUMBER( RTFtag_OON, SSpropOWNER_NR ),
    RTF_OLD_SCOPE_NUMBER( RTFtag_OPG, SSpropPAGE ),
    RTF_OLD_SCOPE_NUMBER( RTFtag_OCO, SSpropCOLUMN ),

    RTF_NEW_SCOPE_NUMBER( RTFtag_NTT, SSpropTREE_TYPE ),
    RTF_NEW_SCOPE_NUMBER( RTFtag_NSN, SSpropSECT_NR ),
    RTF_NEW_SCOPE_NUMBER( RTFtag_NOSN, SSpropOWNER_SECT_NR ),
    RTF_NEW_SCOPE_NUMBER( RTFtag_NON, SSpropOWNER_NR ),
    RTF_NEW_SCOPE_NUMBER( RTFtag_NPG, SSpropPAGE ),
    RTF_NEW_SCOPE_NUMBER( RTFtag_NCO, SSpropCOLUMN ),

    RTF_RANGE_NUMBER( RTFtag_OHC, TRACEposOLD_HEAD_COL ),
    RTF_RANGE_NUMBER( RTFtag_OHP, TRACEposOLD_HEAD_PARA ),
    RTF_RANGE_NUMBER( RTFtag_OHS, TRACEposOLD_HEAD_STROFF ),
    RTF_RANGE_NUMBER( RTFtag_OTC, TRACEposOLD_TAIL_COL ),
    RTF_RANGE_NUMBER( RTFtag_OTP, TRACEposOLD_TAIL_PARA ),
    RTF_RANGE_NUMBER( RTFtag_OTS, TRACEposOLD_TAIL_STROFF ),

    RTF_RANGE_NUMBER( RTFtag_NHP, TRACEposNEW_HEAD_PARA ),
    RTF_RANGE_NUMBER( RTFtag_NHS, TRACEposNEW_HEAD_STROFF ),
    RTF_RANGE_NUMBER( RTFtag_NTP, TRACEposNEW_TAIL_PARA ),
    RTF_RANGE_NUMBER( RTFtag_NTS, TRACEposNEW_TAIL_STROFF ),

    RTF_RANGE_ENUM( RTFtag_SEH, TRACEposSELECTED, SELposHEAD ),
    RTF_RANGE_ENUM( RTFtag_SEN, TRACEposSELECTED, SELposNEXT ),
    RTF_RANGE_ENUM( RTFtag_SAL, TRACEposSELECTED, SELposALL ),
    RTF_RANGE_ENUM( RTFtag_SET, TRACEposSELECTED, SELposTAIL ),
    RTF_RANGE_ENUM( RTFtag_SEB, TRACEposSELECTED, SELposBEFORE ),
    RTF_RANGE_ENUM( RTFtag_SEA, TRACEposSELECTED, SELposAFTER ),

    RTF_RANGE_NUMBER( RTFtag_FLK, TRACEposFIELD_KIND ),

    RTF_RANGE_NUMBER( RTFtag_PRL, TRACEposPROP_LEVEL ),

				/****************************************/
    RTF_DEST_XX( RTFtag__SelOpen+ 2,	RTCscopeANY,
					1, docRtfSelectionOpen ),

				/****************************************/
				/*					*/
				/*  Control words to remember border	*/
				/*  properties.				*/
				/*  An ad-hoc construction requires them*/
				/*  to have the same handler.		*/
				/*					*/
				/****************************************/

# define RTF_BORDER_PROP( s, id ) \
		{ \
		s, RTCscopeBORDER, id, RTCtypeNUMBER, \
		docRtfBrdrProperty, \
		}

# define RTF_BORDER_STYLE(s,v) \
		{ \
		s, RTCscopeBORDER, BRDRpropSTYLE, RTCtypeENUM, \
		docRtfBrdrProperty, v, \
		}

    RTF_BORDER_PROP( "brdrw",		BRDRpropPEN_WIDE ),
    RTF_BORDER_PROP( "brdrcf",		BRDRpropCOLOR ),
    RTF_BORDER_PROP( "brsp",		BRDRpropSPACING ),
    RTF_BORDER_PROP( "brdrart",		BRDRpropART ),

    /**/

    RTF_BORDER_STYLE( "brdrnone",	DOCbsNONE ),
    RTF_BORDER_STYLE( "brdrs",		DOCbsS ),
    RTF_BORDER_STYLE( "brdrth",		DOCbsTH ),
    RTF_BORDER_STYLE( "brdrsh",		DOCbsSH ),

    RTF_BORDER_STYLE( "brdrdb",		DOCbsDB ),
    RTF_BORDER_STYLE( "brdrhair",	DOCbsHAIR ),
    RTF_BORDER_STYLE( "brdrdot",	DOCbsDOT ),
    RTF_BORDER_STYLE( "brdrdash",	DOCbsDASH  ),
    RTF_BORDER_STYLE( "brdrdashd",	DOCbsDASHD ),
    RTF_BORDER_STYLE( "brdrdashdd",	DOCbsDASHDD ),
    RTF_BORDER_STYLE( "brdrtriple",	DOCbsTRIPLE ),

    RTF_BORDER_STYLE( "brdrtnthsg",	DOCbsTNTHSG ),
    RTF_BORDER_STYLE( "brdrthtnsg",	DOCbsTHTNSG ),
    RTF_BORDER_STYLE( "brdrtnthtnsg",	DOCbsTNTHTNSG ),
    RTF_BORDER_STYLE( "brdrtnthmg",	DOCbsTNTHMG ),
    RTF_BORDER_STYLE( "brdrthtnmg",	DOCbsTHTNMG ),
    RTF_BORDER_STYLE( "brdrtnthtnmg",	DOCbsTNTHTNMG ),
    RTF_BORDER_STYLE( "brdrtnthlg",	DOCbsTNTHLG ),
    RTF_BORDER_STYLE( "brdrthtnlg",	DOCbsTHTNLG ),
    RTF_BORDER_STYLE( "brdrtnthtnlg",	DOCbsTNTHTNLG ),

    RTF_BORDER_STYLE( "brdrwavy",	DOCbsWAVY ),
    RTF_BORDER_STYLE( "brdrwavydb",	DOCbsWAVYDB ),
    RTF_BORDER_STYLE( "brdrdashsm",	DOCbsDASHSM ),
    RTF_BORDER_STYLE( "brdrdashdotstr",	DOCbsDASHDOTSTR ),
    RTF_BORDER_STYLE( "brdremboss",	DOCbsEMBOSS ),
    RTF_BORDER_STYLE( "brdrengrave",	DOCbsENGRAVE ),
    RTF_BORDER_STYLE( "brdrframe",	DOCbsFRAME ),
    RTF_BORDER_STYLE( "brdroutset",	DOCbsOUTSET ),
    RTF_BORDER_STYLE( "brdrinset",	DOCbsINSET ),

    RTF_BORDER_STYLE( "brdrtbl",	DOCbsTBL ),
    RTF_BORDER_STYLE( "brdrnil",	DOCbsNIL ),

				/****************************************/
				/*  End.				*/
				/****************************************/
    { (char *)0, 0, 0 }
};

static const int docRtfPropertyWordCount=
			sizeof(docRtfPropertyWords)/sizeof(RtfControlWord)- 1;

#   if ! defined(BUILD_HASH) && ! defined(BUILD_LOOKUP)

const RtfControlWord * docRtfFindPropertyWord(
					const char *	controlWord )
    {
    static int			checked= 1;

    if  ( ! checked )
	{
	if  ( docRtfCheckHash( docRtfPropertyWords,
			    docRtfPropertyWordCount, docRtfTagIndex ) )
	    { checked= -1;	}
	else{ checked=  1;	}
	}

    if  ( checked > 0 )
	{
	return docRtfFindWordInTab(
			    docRtfPropertyWords, docRtfPropertyWordCount,
			    controlWord, docRtfTagIndex );
	}
    else{
	return docRtfFindWordInTab(
			    docRtfPropertyWords, docRtfPropertyWordCount,
			    controlWord, (DOC_RTF_HASH_WORD)0 );
	}
    }

const RtfControlWord * docRtfFindProperty(
					int			scope,
					int			prop,
					int			value )
    {
    static int		checked= 1;

    if  ( ! checked )
	{
	if  ( docRtfCheckLookup( docRtfPropertyWords,
				docRtfPropertyWordCount,
				docRtfTagLookup, docRtfTagLookupCount ) )
	    { checked= -1;	}
	else{ checked=  1;	}
	}

    return docRtfLookupProperty(
		docRtfPropertyWords, docRtfTagLookup, docRtfTagLookupCount,
		scope, prop, value );
    }

#   endif

#   ifdef	BUILD_HASH

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeHash( docRtfPropertyWords, docRtfPropertyWordCount,
						"docRtfTagHash.c",
						"\"docRtfTagHash.h\"",
						"docRtfTagHash",
						"docRtfTagIndex" ) )
	    { LDEB(docRtfPropertyWordCount); return 1;	}

	return 0;
	}

#   endif

#   ifdef	BUILD_LOOKUP

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeLookup( docRtfPropertyWords, docRtfPropertyWordCount,
						"docRtfTagLookup.c",
						"\"docRtfTagLookup.h\"",
						"docRtfTagLookup",
						"docRtfTagLookupCount" ) )
	    { LDEB(docRtfPropertyWordCount); return 1;	}

	return 0;
	}

#   endif

