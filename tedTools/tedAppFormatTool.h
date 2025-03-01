/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_APP_FORMAT_TOOL_H
#   define	TED_APP_FORMAT_TOOL_H

/************************************************************************/
/*									*/
/*  The different pages in a Format tool:				*/
/*									*/
/************************************************************************/

# define USE_FONT_TOOL 1
# define USE_TABS_TOOL 1
# define USE_LIST_TOOL 1
# define USE_PAGE_TOOL 1
# define USE_FIND_TOOL 1
# define USE_LINK_TOOL 1
# define USE_BOOKMARK_TOOL 1
# define USE_SPELL_TOOL 1
# define USE_LANGUAGE_TOOL 1

# define USE_FRAME_TOOL 0

typedef enum AppToolSubjectIndex
    {
#   if USE_FONT_TOOL
    TED_APPtsiFONT= 0,
#   endif
    TED_APPtsiTEXT_ORN,

    TED_APPtsiPARA_LAY,
#   if USE_TABS_TOOL
    TED_APPtsiTABS,
#   endif
    TED_APPtsiPARA_ORN,
#   if USE_LIST_TOOL
    TED_APPtsiLISTS,
#   endif
    TED_APPtsiSECT,

    TED_APPtsiTABLE,
    TED_APPtsiROW,
    TED_APPtsiCOLUMN,
    TED_APPtsiCELL,

#   if USE_PAGE_TOOL
    TED_APPtsiPAGE,
#   endif
    TED_APPtsiHEADFOOT,
    TED_APPtsiTHIS_NOTE,
    TED_APPtsiFOOT_NOTES,
    TED_APPtsiEND_NOTES,
#   if USE_LINK_TOOL
    TED_APPtsiLINK,
#   endif
#   if USE_BOOKMARK_TOOL
    TED_APPtsiBOOKMARK,
#   endif
    TED_APPtsiIMAGE,
#   if USE_FRAME_TOOL
    TED_APPtsiFRAME,
#   endif
    TED_APPtsiTOC,
    TED_APPtsiSYMBOL,

#   if USE_FIND_TOOL
    TED_APPtsiFIND,
#   endif
#   if USE_SPELL_TOOL
    TED_APPtsiSPELL,
#   endif
#   if USE_LANGUAGE_TOOL
    TED_APPtsiLANGUAGE,
#   endif

    TED_APPtsiRGB,

    TED_APPtsi_COUNT

    } AppToolSubjectIndex;

/************************************************************************/
/*									*/
/*  Represents a format tool.						*/
/*									*/
/************************************************************************/

typedef struct TedFormatTool
    {
    struct EditApplication *	tftApplication;
    struct AppInspector *	tftInspector;
    } TedFormatTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  TED_APP_FORMAT_TOOL_H */
