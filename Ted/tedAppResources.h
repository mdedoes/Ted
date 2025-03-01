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

    char *			tarPdfOmitContentMarksString;
    int				tarPdfOmitContentMarksInt;

				/**
				 * In combination with tarPdfMarkContent=1,
				 * modify XMP metadata to state that the 
				 * PDF is PDF/UA compliant. This uses a GhostScript
				 * specific pdfmark. Documents that are emitted with
				 * pdfOmitContentMarks are NOT PDF/UA compliant.
				 * Do not combine the two flags
				 */
    char *			tarPdfDeclareUACompliantString;
    int				tarPdfDeclareUACompliantInt;

				/**
				 * In combination with tarPdfMarkContent=1,
				 * we mark invisible text as an artifact.
				 */
    char *			tarPdfMarkInvisibleAsArtifactString;
    int				tarPdfMarkInvisibleAsArtifactInt;

				/**
				 * In combination with tarPdfMarkContent=1,
				 * Tables without any borders or shading are not marked
				 * as tbles in the PDF output.
				 */
    char *			tarPdfFlattenPlainTablesString;
    int				tarPdfFlattenPlainTablesInt;

				/**
				 * Draw text particules with the same attributes
				 * separately. See docDraw.h for some more details.
				 */
    char *			tarDrawParticulesSeparatelyString;
    int				tarDrawParticulesSeparatelyInt;

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
