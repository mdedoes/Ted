/************************************************************************/
/*									*/
/*  Document Extensions that we support in one way or another.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FILEEXTENSIONS_H
#   define	DOC_FILEEXTENSIONS_H

struct AppFileExtension;

extern struct AppFileExtension * const DOC_FileExtensions;
extern const int DOC_FileExtensionCount;

/************************************************************************/
/*									*/
/*  Supported document formats.						*/
/*  Match the array with extensions in docFileExtensions.c.		*/
/*									*/
/************************************************************************/

typedef enum DocumentKind
    {
    TEDdockindRTF= 0,
    TEDdockindTEXT_OPEN,
    TEDdockindTEXT_SAVE_WIDE,
    TEDdockindTEXT_SAVE_FOLDED,
    TEDdockindMARKDOWN,
    TEDdockindHTML_FILES,
    TEDdockindEML,
    TEDdockindEPUB,
    TEDdockindPS,
    TEDdockindSVG,
    TEDdockindPDF,
    TEDdockindTRACE,
    TEDdockindALL_FILES,

    TEDdockind_COUNT
    } DocumentKind;

#   endif
