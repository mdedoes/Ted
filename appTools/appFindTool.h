/************************************************************************/
/*									*/
/*  A find/replace tool.						*/
/*									*/
/************************************************************************/

#   ifndef		APP_FIND_TOOL_H
#   define		APP_FIND_TOOL_H

#   include	<guiBase.h>

struct EditApplication;
struct AppInspector;
struct InspectorSubject;
struct InspectorSubjectResources;

/************************************************************************/
/*									*/
/*  Callbacks from the Find Tool.					*/
/*									*/
/************************************************************************/

typedef int (*FindToolFind)(		void *			target );

typedef int (*FindToolReplace)(		void *			target,
					const char *		replacement );

typedef int (*FindToolSetPattern)(	void *			voidea,
					const char *		pattern,
					int			useRegex,
					int			asWord,
					int			caseSensitive );

/************************************************************************/
/*									*/
/*  Represents a find tool.						*/
/*									*/
/************************************************************************/

typedef struct FindToolResources
    {
    char *	ftrFindTitle;

    char *	ftrUseRegex;
    char *	ftrAsWord;
    char *	ftrCaseSensitive;

    char *	ftrReplaceTitle;
    char *	ftrReplaceFound;
    char *	ftrReplaceNext;

    char *	ftrExplicitFocus;
    } FindToolResources;

typedef struct FindTool
    {
    struct EditApplication *	ftApplication;
    struct AppInspector *	ftInspector;
    int				ftSubjectPage;

    int				ftInProgrammaticChange;

				/**
				 *  The parent of the actual find controls
				 */
    APP_WIDGET			ftFindParent;

    APP_WIDGET			ftPatternText;
    APP_WIDGET			ftFindNextButton;
    APP_WIDGET			ftFindPrevButton;

				/**
				 *  The parent of the actual replace controls
				 */
    APP_WIDGET			ftReplaceParent;
    APP_WIDGET			ftReplaceText;
    APP_WIDGET			ftReplaceButton;
    APP_WIDGET			ftReplaceNextButton;

    APP_WIDGET			ftAsWordToggle;
    APP_WIDGET			ftCaseSensitiveToggle;
    APP_WIDGET			ftRegexToggle;

    FindToolFind		ftFindNext;
    FindToolFind		ftFindPrev;
    FindToolReplace		ftReplace;
    FindToolSetPattern		ftSetPattern;
    void *			ftTarget;

    int				ftReplaceEnabled;
    int				ftLastFindResult;

    unsigned char		ftAsWord;
    unsigned char		ftCaseSensitive;
    unsigned char		ftUseRegex;

    int				ftExplicitFocus;
    } FindTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appFillVerticalFindTool(
				FindTool *			ft,
				const FindToolResources *	ftr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr,
				FindToolFind			findNext,
				FindToolFind			findPrev,
				FindToolSetPattern		setPattern,
				FindToolReplace			replaceText,
				void *				target );

extern void appFillHorizontalFindTool(
				FindTool *			ft,
				const FindToolResources *	ftr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr,
				FindToolFind			findNext,
				FindToolFind			findPrev,
				FindToolSetPattern		setPattern,
				FindToolReplace			replaceText,
				void *				target );

extern void appFindToolEnableReplace(	FindTool *	aft,
					int		enabled );

extern void appFindToolSetPattern(	FindTool *		aft,
					const char *		pattern,
					int			useRegex );

extern void appFindToolFocusToPattern(	FindTool *		ft );

extern void appFindToolGetResourceTable( struct EditApplication *	ea,
					FindToolResources *		spr,
					struct InspectorSubjectResources *	isr );

extern void appInitFindTool(		FindTool *			aft );
extern void appCleanFindTool(		FindTool *			aft );

#   endif	/*	APP_FIND_TOOL_H	*/
