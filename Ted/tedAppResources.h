#   ifndef	TED_APP_RESOURCES_H
#   define	TED_APP_RESOURCES_H

struct AppFileExtension;
struct EditDocument;

/************************************************************************/
/*									*/
/*  Ted specific resources.						*/
/*									*/
/*  Also: Some global data items.					*/
/*									*/
/************************************************************************/

typedef struct TedAppResources
    {
				/****************************************/
				/*  Texts.				*/
				/****************************************/

    char *			tarFileNotRtf;
    char *			tarHeadFootNotReachable;
    char *			tarCannotRecover;

    char *			tarAppHelpFileName;

    char *			tarSelectionColor;
    char *			tarCopiedSelectionColor;
    char *			tarPageNumberFormat;

				/****************************************/
				/*  Defaults.				*/
				/****************************************/
    char *			tarShowTableGridString;
    int				tarShowTableGridInt;

    char *			tarShadingMeshTwipsString;
    double			tarShadingMeshPointsDouble;

    char *			tarPageGapMMString;
    int				tarPageGapMM;

    char *			tarLenientRtfString;
    int				tarLenientRtfInt;

    char *			tarTraceEditsString;
    int				tarTraceEditsInt;

    char *			tarPdfOutlineString;
    int				tarPdfOutlineInt;

    char *			tarOverridePaperSizeString;
    int				tarOverridePaperSizeInt;

    char *			tarInlineHtmlImagesString;
    int				tarInlineHtmlImagesInt;

				/****************************************/
				/*  Global data.			*/
				/****************************************/

    struct EditDocument *	tarManualDocument;

				/****************************************/
				/*  To open images:			*/
				/*  From a file chooser.		*/
				/****************************************/
    struct AppFileExtension *	tarOpenImageExtensions;
    int				tarOpenImageExtensionCount;

				/****************************************/
				/*  For Ted --Find and Ted --RegFind	*/
				/****************************************/

    const char *		tarFindPattern;
    int				tarFindRegex;
    } TedAppResources;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedDetermineDefaultSettings(	TedAppResources *	tar );

#   endif	/*  TED_APP_RESOURCES_H	*/
