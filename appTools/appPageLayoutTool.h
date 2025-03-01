/************************************************************************/
/*									*/
/*  Page Layout page on an inspector:					*/
/*									*/
/************************************************************************/

#   ifndef	APP_PAGE_LAYOUT_TOOL_H
#   define	APP_PAGE_LAYOUT_TOOL_H

#   include	<guiBase.h>
#   include	"appPaperChooser.h"
#   include	"appMarginTool.h"
#   include	<drawDrawingSurface.h>
#   include	<utilColor.h>

struct EditApplication;
struct InspectorSubjectResources;
struct InspectorSubject;
struct AppInspector;

/************************************************************************/
/*									*/
/*  Resources for the 'Page Layout' page.				*/
/*									*/
/************************************************************************/

typedef struct PageLayoutPageResources
    {
    char *		plprUnitType;

    char *		plprPage;
    char *		plprCustomPageSizeText;
    char *		plprLandscapeText;
    char *		plprPortraitText;

    MarginToolResources	plprMarginToolResources;
    char *		plprPositionsText;
    char *		plprHeaderPositionText;
    char *		plprFooterPositionText;

    char *		plprRevertSelectionText;
    char *		plprChangeSelectionText;
    } PageLayoutPageResources;

/************************************************************************/
/*									*/
/*  A page layout tool, i.e. the 'Page Layout' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct PageLayoutTool
    {
    struct EditApplication *		pltApplication;
    struct AppInspector *		pltInspector;
    const PageLayoutPageResources *	pltPageResources;

    int					pltManageHeadersFooters;
    int					pltManageSelection;

    int					pltUnitType;
    double				pltPixelsPerTwip;
    int					pltPageHighMm;

    unsigned char			pltCanChangeSelection;
    unsigned char			pltCanChangeDocument;

    DocumentGeometry			pltGeometrySetSelection;
    DocumentGeometry			pltGeometrySetDocument;
    DocumentGeometry			pltGeometryChosen;

    APP_WIDGET				pltPageDrawing;
    PaperChooser			pltPaperChooser;
    AppMarginTool			pltMarginTool;
    DrawingSurface			pltDrawingSurface;
    RGB8Color				pltBackgroundColor;

    APP_WIDGET				pltHeaderFooterFrame;
    APP_WIDGET				pltHeaderFooterInside;
    APP_WIDGET				pltHeaderFooterLeftColumn;
    APP_WIDGET				pltHeaderFooterRightColumn;
    APP_WIDGET				pltHeaderPositionText;
    APP_WIDGET				pltFooterPositionText;

    APP_WIDGET				pltRevertSelectionWidget;
    APP_WIDGET				pltChangeSelectionWidget;
    APP_WIDGET				pltRevertDocumentWidget;
    APP_WIDGET				pltChangeDocumentWidget;
    } PageLayoutTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void appPageLayoutPageGetResourceTable(
				struct EditApplication *	ea,
				PageLayoutPageResources *	plpr,
				struct InspectorSubjectResources *	isr );

extern void appPageLayoutPageRefresh(
				PageLayoutTool *		plt,
				struct InspectorSubject *	is,
				const DocumentGeometry *	dgSelection,
				const DocumentGeometry *	dgDoc );

extern void appPageLayoutPageFillPage(
				PageLayoutTool *		plt,
				int				doHdFt,
				int				doSelection,
				const PageLayoutPageResources *	plpr,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

extern void appPageLayoutPageFillChoosers(
				PageLayoutTool *		plt,
				const PageLayoutPageResources *	plpr );

extern void appPageLayoutPageFinishPage(
				PageLayoutTool *		plt,
				const PageLayoutPageResources *	plpr );

extern void appInitPageLayoutTool(	PageLayoutTool *	plt );
extern void appCleanPageLayoutTool(	PageLayoutTool *	plt );

#   endif	/*  APP_PAGE_LAYOUT_TOOL_H */
