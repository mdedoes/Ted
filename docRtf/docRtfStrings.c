/************************************************************************/
/*									*/
/*  String constants.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"
#   include	"docRtfTags.h"

const char RTFtag__SelOpen[]= "*\\SelOpen";

const char RTFtag_FLK[]= "FLK";

const char RTFtag_OTX[]= "OTX";
const char RTFtag_NTX[]= "NTX";
const char RTFtag_NPR[]= "NPR";

const char RTFtag_SLI[]= "SLI";
const char RTFtag_SBK[]= "SBK";
const char RTFtag_DFL[]= "DFL";
const char RTFtag_UFL[]= "UFL";
const char RTFtag_ULI[]= "ULI";
const char RTFtag_NLI[]= "NLI";
const char RTFtag_NNO[]= "NNO";
const char RTFtag_UNO[]= "UNO";
const char RTFtag_UTA[]= "UTA";
const char RTFtag_UPP[]= "UPP";
const char RTFtag_UTP[]= "UTP";
const char RTFtag_URP[]= "URP";
const char RTFtag_UCP[]= "UCP";
const char RTFtag_UcP[]= "UcP";
const char RTFtag_USP[]= "USP";
const char RTFtag_USDP[]= "USDP";
const char RTFtag_UDP[]= "UDP";
const char RTFtag_REP[]= "REP";
const char RTFtag_REPF[]= "REPF";
const char RTFtag_REPB[]= "REPB";
const char RTFtag_DELS[]= "DELS";
const char RTFtag_DELF[]= "DELF";
const char RTFtag_DELB[]= "DELB";
const char RTFtag_REX[]= "REX";
const char RTFtag_MPA[]= "MPA";

const char RTFtag_DPA[]= "DPA";
const char RTFtag_DSE[]= "DSE";
const char RTFtag_DRW[]= "DRW";
const char RTFtag_DTB[]= "DTB";
const char RTFtag_DCO[]= "DCO";
const char RTFtag_DHD[]= "DHD";
const char RTFtag_DFT[]= "DFT";

const char RTFtag_IPA[]= "IPA";
const char RTFtag_APA[]= "APA";
const char RTFtag_ITB[]= "ITB";
const char RTFtag_ISE[]= "ISE";
const char RTFtag_ASE[]= "ASE";
const char RTFtag_SSE[]= "SSE";
const char RTFtag_IRW[]= "IRW";
const char RTFtag_ARW[]= "ARW";
const char RTFtag_ICO[]= "ICO";
const char RTFtag_ACO[]= "ACO";
const char RTFtag_IHD[]= "IHD";
const char RTFtag_IFT[]= "IFT";

const char RTFtag_UOB[]= "UOB";
const char RTFtag_SRWD[]= "SRWD";
const char RTFtag_SRWU[]= "SRWU";

const char RTFtag_SPR[]= "SPR";
const char RTFtag_SPL[]= "SPL";
const char RTFtag_Open[]= "Open";
const char RTFtag_Save[]= "Save";
const char RTFtag_New[]= "New";

const char RTFtag_OTT[]= "OTT";
const char RTFtag_OSN[]= "OSN";
const char RTFtag_OOSN[]= "OOSN";
const char RTFtag_OON[]= "OON";
const char RTFtag_OPG[]= "OPG";
const char RTFtag_OCO[]= "OCO";

const char RTFtag_NTT[]= "NTT";
const char RTFtag_NSN[]= "NSN";
const char RTFtag_NOSN[]= "NOSN";
const char RTFtag_NON[]= "NON";
const char RTFtag_NPG[]= "NPG";
const char RTFtag_NCO[]= "NCO";

const char RTFtag_OHC[]= "OHC";
const char RTFtag_OHP[]= "OHP";
const char RTFtag_OHS[]= "OHS";

const char RTFtag_OTC[]= "OTC";
const char RTFtag_OTP[]= "OTP";
const char RTFtag_OTS[]= "OTS";
const char RTFtag_PRL[]= "PRL";

const char RTFtag_NHP[]= "NHP";
const char RTFtag_NHS[]= "NHS";
const char RTFtag_NTP[]= "NTP";
const char RTFtag_NTS[]= "NTS";

const char RTFtag_SEH[]= "SEH";
const char RTFtag_SEN[]= "SEN";
const char RTFtag_SAL[]= "SAL";
const char RTFtag_SET[]= "SET";
const char RTFtag_SEB[]= "SEB";
const char RTFtag_SEA[]= "SEA";

/* Regular tags */

const char RTFtag_pict[]= "pict";
const char RTFtag_result[]= "result";

const char RTFtag_par[]= "par";
const char RTFtag_cell[]= "cell";
const char RTFtag_row[]= "row";
const char RTFtag_sect[]= "sect";
const char RTFtag_pard[]= "pard";
const char RTFtag_plain[]= "plain";

const char RTFtag_footnote[]= "footnote";
const char RTFtag_chftn[]= "chftn";
const char RTFtag_fet[]= "fet";

const char RTFtag_revtim[]= "revtim";
const char RTFtag_hlinkbase[]= "hlinkbase";
const char RTFtag__generator[]= "*\\generator";

const char RTFtag_nosupersub[]= "nosupersub";
const char RTFtag_super[]= "super";
const char RTFtag_sub[]= "sub";

const char RTFtag_titlepg[]= "titlepg";
const char RTFtag_endpg[]= "endpg";

const char RTFtag_tql[]= "tql";
const char RTFtag_tqr[]= "tqr";
const char RTFtag_tqc[]= "tqc";
const char RTFtag_tqdec[]= "tqdec";

const char RTFtag_cellx[]= "cellx";

const char RTFtag_ltrch[]= "ltrch";
const char RTFtag_rtlch[]= "rtlch";

const char * const DOCrtf_DocFootNotesJustificationTags[]=
    {
    "ftntj",
    "ftnbj",
    };

const int DOCrtf_DocFootNotesJustificationTagCount=
	    sizeof(DOCrtf_DocFootNotesJustificationTags)/sizeof(const char *);

const char * const DOCrtf_DocFootNotesPlacementTags[]=
    {
    (char *)0,
    "endnotes",
    "enddoc",
    };

const int DOCrtf_DocFootNotesPlacementTagCount=
	    sizeof(DOCrtf_DocFootNotesPlacementTags)/sizeof(const char *);

const char * const DOCrtf_DocEndNotesJustificationTags[]=
    {
    "aftntj",
    "aftnbj",
    };

const int DOCrtf_DocEndNotesJustificationTagCount=
	    sizeof(DOCrtf_DocEndNotesJustificationTags)/sizeof(const char *);

const char * const DOCrtf_DocEndNotesPlacementTags[]=
    {
    (char *)0,
    "aendnotes",
    "aenddoc",
    };

const int DOCrtf_DocEndNotesPlacementTagCount=
	    sizeof(DOCrtf_DocEndNotesPlacementTags)/sizeof(const char *);

const char * const DOCrtf_SectFootNotesJustificationTags[]=
    {
    "sftntj",
    "sftnbj",
    };

const int DOCrtf_SectFootNotesJustificationTagCount=
	    sizeof(DOCrtf_SectFootNotesJustificationTags)/sizeof(const char *);

const char * const DOCrtf_SectEndNotesJustificationTags[]=
    {
    "saftntj",
    "saftnbj",
    };

const int DOCrtf_SectEndNotesJustificationTagCount=
	    sizeof(DOCrtf_SectEndNotesJustificationTags)/sizeof(const char *);

const char * const DOCrtf_DocFootNotesRestartTags[]=
    {
    "ftnrstcont",
    "ftnrestart",
    "ftnrstpg",
    };

const int DOCrtf_DocFootNotesRestartTagCount=
		sizeof(DOCrtf_DocFootNotesRestartTags)/sizeof(const char *);

const char * const DOCrtf_SectFootNotesRestartTags[]=
    {
    "sftnrstcont",
    "sftnrestart",
    "sftnrstpg",
    };

const int DOCrtf_SectFootNotesRestartTagCount=
		sizeof(DOCrtf_SectFootNotesRestartTags)/sizeof(const char *);

const char * const DOCrtf_DocEndNotesRestartTags[]=
    {
    "aftnrstcont",
    "aftnrestart",
    "aftnrstpg",	/*  UNUSED: Effectively makes them footnotes	*/
    };

const int DOCrtf_DocEndNotesRestartTagCount=
		sizeof(DOCrtf_DocEndNotesRestartTags)/sizeof(const char *);

const char * const DOCrtf_SectEndNotesRestartTags[]=
    {
    "saftnrstcont",
    "saftnrestart",
    "saftnrstpg",	/*  UNUSED: Effectively makes them footnotes	*/
    };

const int DOCrtf_SectEndNotesRestartTagCount=
		sizeof(DOCrtf_SectEndNotesRestartTags)/sizeof(const char *);

const char * const DOCrtf_DocFootNotesNumberStyleTags[]=
    {
    "ftnnar",
    "ftnnalc",
    "ftnnauc",
    "ftnnrlc",
    "ftnnruc",
    "ftnnchi",
    };

const int DOCrtf_DocFootNotesNumberStyleTagCount=
		sizeof(DOCrtf_DocFootNotesNumberStyleTags)/sizeof(const char *);

const char * const DOCrtf_DocEndNotesNumberStyleTags[]=
    {
    "aftnnar",
    "aftnnalc",
    "aftnnauc",
    "aftnnrlc",
    "aftnnruc",
    "aftnnchi",
    };

const int DOCrtf_DocEndNotesNumberStyleTagCount=
		sizeof(DOCrtf_DocEndNotesNumberStyleTags)/sizeof(const char *);

const char * const DOCrtf_SectFootNotesNumberStyleTags[]=
    {
    "sftnnar",
    "sftnnalc",
    "sftnnauc",
    "sftnnrlc",
    "sftnnruc",
    "sftnnchi",
    };

const int DOCrtf_SectFootNotesNumberStyleTagCount=
	    sizeof(DOCrtf_SectFootNotesNumberStyleTags)/sizeof(const char *);

const char * const DOCrtf_SectEndNotesNumberStyleTags[]=
    {
    "saftnnar",
    "saftnnalc",
    "saftnnauc",
    "saftnnrlc",
    "saftnnruc",
    "saftnnchi",
    };

const int DOCrtf_SectEndNotesNumberStyleTagCount=
	    sizeof(DOCrtf_SectEndNotesNumberStyleTags)/sizeof(const char *);

const char * const DOCrtf_ParaBreakTags[]=
    {
	/**
	 *  NOT part of RTF. Should never show up in docs. Ted emits this 
	 *  in the undo/redo/recover trace file.
	 */
    "nopagebb",
	/**
	 *  Works as a property as this is the last paragraph property to 
	 *  write.
	 */
    "column",
    "pagebb",
    };

const int DOCrtf_ParaBreakTagCount=
		sizeof(DOCrtf_ParaBreakTags)/sizeof(const char *);

