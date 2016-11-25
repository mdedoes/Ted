/************************************************************************/
/*									*/
/*  Paragraph borders and shading tool:					*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_ORNAMENTS_TOOL_H
#   define	TED_PARA_ORNAMENTS_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Borders and Shading' page.		*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsPageResources
    {
    char *			poprShading;
    ShadingToolResources	poprShadingResources;
    BorderToolResources		poprBorderToolResources;

    char *			poprTopBorder;
    char *			poprBottomBorder;
    } ParagraphOrnamentsPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsTool
    {
    EditApplication *				potApplication;
    AppInspector *				potInspector;
    const ParagraphOrnamentsPageResources *	potPageResources;

    ParagraphProperties				potPropertiesSet;
    ParagraphProperties				potPropertiesChosen;

    BorderTool					potTopBorderTool;
    BorderTool					potBottomBorderTool;

    ShadingTool					potShadingTool;
    } ParagraphOrnamentsTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetParaOrnamentsResourceTable(
				    EditApplication *			ea,
				    ParagraphOrnamentsPageResources *	plpr,
				    InspectorSubjectResources *		isr );

extern void tedFormatFillParagraphOrnamentsChoosers(
				ParagraphOrnamentsTool *		pot,
				const ParagraphOrnamentsPageResources *	popr );

extern void tedFormatFillParagraphOrnamentsPage(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			AppInspector *				ai,
			int					subjectPage,
			InspectorSubject *			is,
			APP_WIDGET				pgW,
			const InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot,
				int *				pEnabled,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedInitParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot );
extern void tedCleanParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot );

#   endif	/*  TED_PARA_ORNAMENTS_TOOL_H */
