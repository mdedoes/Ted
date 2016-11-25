/************************************************************************/
/*									*/
/*  List tool:								*/
/*									*/
/*  The page on the format tool to edit list properties.		*/
/*									*/
/************************************************************************/

#   ifndef	TED_LIST_TOOL_H
#   define	TED_LIST_TOOL_H

# define LISTTOOLcountSTYLES	8

/************************************************************************/
/*									*/
/*  Resources for the 'Bullets and Numbers' page.			*/
/*									*/
/************************************************************************/

typedef struct ListsPageResources
    {
    char *		lprListLevel;
    char *		lprLevel;
    char *		lprStartAt;
    char *		lprNoRestart;
    char *		lprStyle;
    char *		lprJustify;
    char *		lprFollowedBy;
    char *		lprStyleOptionTexts[LISTTOOLcountSTYLES];
    char *		lprJustifyOptionTexts[DOClla_COUNT];
    char *		lprFollowOptionTexts[DOCllf_COUNT];

    char *		lprLevelFirstIndent;
    char *		lprLevelLeftIndent;

    char *		lprSelection;
    char *		lprSetList;
    char *		lprNewList;
    char *		lprSetLevel;
    char *		lprRemoveFromList;
    } ListsPageResources;

/************************************************************************/
/*									*/
/*  A lists tool, i.e. the 'Bullets and Numbers' page of the format	*/
/*  tool.								*/
/*									*/
/************************************************************************/

typedef struct ListTool
    {
    EditApplication *		ltApplication;
    AppInspector *		ltInspector;
    const ListsPageResources *	ltPageResources;

    int				ltUnitType;

    DocumentList		ltListPropertiesSet;
    DocumentList		ltListPropertiesChosen;
    ListOverride		ltListOverrideSet;
    ListOverride		ltListOverrideChosen;
    int				ltCurrentLevel;
    int				ltCurrentParagraphNumber;

    int				ltHerePath[DLmaxLEVELS];
    int				ltPrevPath[DLmaxLEVELS];
    int				ltFormatPath[DLmaxLEVELS];
    int				ltStartPath[DLmaxLEVELS];

    int				ltHereLevel;
    int				ltPrevLevel;

    /**/
    APP_WIDGET			ltListLevelList;

    APP_WIDGET			ltSelectionFrame;
    APP_WIDGET			ltSelectionPaned;

    APP_WIDGET			ltListLevelFrame;
    APP_WIDGET			ltListLevelPaned;

    APP_WIDGET			ltLevelText;
    APP_WIDGET			ltFirstIndentText;
    APP_WIDGET			ltLeftIndentText;
    APP_WIDGET			ltStartAtText;
    APP_WIDGET			ltNoRestartToggle;

    AppOptionmenu		ltNumberStyleOptionmenu;
    APP_WIDGET			ltNumberStyleItems[LISTTOOLcountSTYLES];

    AppOptionmenu		ltJustifyOptionmenu;
    APP_WIDGET			ltJustifyItems[DOClla_COUNT];

    AppOptionmenu		ltFollowOptionmenu;
    APP_WIDGET			ltFollowItems[DOCllf_COUNT];

    APP_WIDGET			ltSetListButton;
    APP_WIDGET			ltNewListButton;
    APP_WIDGET			ltSetLevelButton;
    APP_WIDGET			ltRemoveFromListButton;
    } ListTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetListsResourceTable(
					EditApplication *		ea,
					ListsPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshListTool(
				ListTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd );

extern void tedFormatFillListsPage( ListTool *			lt,
				const ListsPageResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr );

extern void tedFormatFillListChoosers(	ListTool *			lt );

extern void tedFormatFinishListPage(	ListTool *			lt );

extern void tedFormatCleanListsTool(	ListTool *			lt );

#   endif	/*  TED_LIST_TOOL_H */
