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

				/**
				 * Use PDFmarks to make the PostScript
				 * output close to the PDF-UA standard 
				 * when it is converted to PDF with a 
				 * recent version of GhostScript (10+)
				 */
    char *			tarPdfMarkContentString;
    int				tarPdfMarkContentInt;

				/**
				 * In combination with tarPdfMarkContent=1,
				 * we mark invisible text as an artifact.
				 */
    char *			tarPdfMarkInvisibleAsArtifactString;
    int				tarPdfMarkInvisibleAsArtifactInt;

    char *			tarOverridePaperSizeString;
    int				tarOverridePaperSizeInt;

    char *			tarInlineHtmlImagesString;
    int				tarInlineHtmlImagesInt;

    char *			tarHonourSpecialSectBreaksString;
    int				tarHonourSpecialSectBreaksInt;

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
