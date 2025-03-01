/************************************************************************/
/*									*/
/*  Shading tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_SHADING_TOOL_H
#   define	TED_SHADING_TOOL_H

#   include <guiBase.h>
#   include <appInspector.h>
#   include <appColorChooser.h>
#   include <psShading.h>
#   include <docItemShading.h>
#   include <utilPropMask.h>

struct ShadingTool;
struct BufferDocument;

typedef void (*TedShadingToolCallback)(
				struct ShadingTool *		st,
				void *				through,
				const PropertyMask *		bpSetMask,
				const ExpandedItemShading *	eis );

typedef struct ShadingTool
    {
    APP_WIDGET			stFrame;
    APP_WIDGET			stPaned;
    APP_WIDGET			stRow;

    APP_WIDGET			stPatternLabel;
    APP_WIDGET			stLevelLabel;
    APP_WIDGET			stBackLabel;
    APP_WIDGET			stForeLabel;

    int				stShowPattern;
    AppDrawnPulldown		stPatternPulldown;
    DrawingSurface		stPatternPixmaps[PSshd_COUNT];

    APP_WIDGET			stOnOffToggle;
    APP_WIDGET			stLevelText;
    ColorChooser		stBackColorChooser;
    ColorChooser		stForeColorChooser;

    ExpandedItemShading		stShadingChosen;
    PropertyMask		stShadingChanged;

    TedShadingToolCallback	stCallback;
    void *			stTarget;
    struct AppInspector *	stInspector;

    int				stSubjectPage;
    int				stBackWhich;
    int				stForeWhich;
    } ShadingTool;

typedef struct ShadingToolResources
    {
    char *			strPattern;
    char *			strLevel;
    char *			strBackColor;
    char *			strForeColor;

    ColorChooserResources	strBackColorChooserResources;
    ColorChooserResources	strForeColorChooserResources;
    } ShadingToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedEnableShadingTool(	ShadingTool *		st,
					int			enabled );

extern void tedSetShadingToolByNumber(	ShadingTool *			st,
					const struct BufferDocument *	bd,
					int				num );

extern void tedFormatMakeShadingTool(
				ShadingTool *			st,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const ShadingToolResources *	str,
				int				subjectPage,
				int				foreWhich,
				int				backWhich,
				TedShadingToolCallback		callback,
				void *				through );

extern void tedShadeSetExplicitColorChoice(
				ShadingTool *			st,
				int				which,
				const RGB8Color *		rgb8Set );

extern void tedInitShadingTool(		ShadingTool *		st );
extern void tedCleanShadingTool(	ShadingTool *		st );

extern void tedFinishShadingTool(ShadingTool *			st,
				const PostScriptFontList *	psfl );

extern int tedShadingToolGetShadingNumber(
					int *			pNum,
					PropertyMask *		isSetMask,
					const ShadingTool *	st,
					struct BufferDocument *	bd );

#   endif	/*  TED_SHADING_TOOL_H */
