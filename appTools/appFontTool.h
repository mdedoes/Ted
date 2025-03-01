/************************************************************************/
/*									*/
/*  A Font/Text attribute Selection Page on an inspector.		*/
/*									*/
/************************************************************************/

#   ifndef APP_FONT_TOOL_H
#   define APP_FONT_TOOL_H

#   include	<docExpandedTextAttribute.h>
#   include	<drawDrawingSurface.h>
#   include	<guiBase.h>
#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>
#   include	<fontDocFont.h>
#   include	<fontDocFontListImpl.h>

struct EditApplication;
struct AppInspector;
struct InspectorSubject;
struct InspectorSubjectResources;
struct PostScriptFontList;

/************************************************************************/
/*									*/
/*  Represents a font chooser.						*/
/*									*/
/************************************************************************/

typedef struct AppFontToolResources
    {
    char *			aftrFamily;
    char *			aftrFace;
    char *			aftrSize;
    char *			aftrRevert;
    char *			aftrSet;

    char *			aftrTextUnderlined;
    char *			aftrTextStrikethrough;

    char *			aftrSuperscript;
    char *			aftrSubscript;
    char *			aftrBaseline;

    char *			aftrSmallcaps;
    char *			aftrCapitals;
    char *			aftrFaces[FONTface_COUNT];
    } AppFontToolResources;

typedef int (*FontChooserSetFont)(
				void *				target,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

typedef struct AppFontChooser
    {
    struct EditApplication *	afcApplication;
    struct AppInspector *	afcInspector;
    int				afcSubjectPage;
    int				afcCurrentDocumentId;
    int				afcInProgrammaticChange;

    unsigned char		afcCanChange;

    APP_WIDGET			afcSampleDrawing;
    APP_WIDGET			afcScreenName;
    APP_WIDGET			afcFontMatch;

    APP_WIDGET			afcChooseRow;
    APP_WIDGET				afcFamilyColumn;
    APP_WIDGET					afcFamilyLabel;
    APP_WIDGET					afcFamilyList;
    APP_WIDGET				afcFaceColumn;
    APP_WIDGET					afcFaceLabel;
    APP_WIDGET					afcFaceList;
    APP_WIDGET				afcSizeColumn;
    APP_WIDGET					afcSizeLabel;
    APP_WIDGET					afcSizeList;
    APP_WIDGET					afcSizeText;

    APP_WIDGET			afcToggleRow1;
    APP_WIDGET			afcUnderlinedToggle;
    APP_WIDGET			afcSuperscriptToggle;
    APP_WIDGET			afcSmallcapsToggle;

    APP_WIDGET			afcToggleRow2;
    APP_WIDGET			afcStrikethroughToggle;
    APP_WIDGET			afcSubscriptToggle;
    APP_WIDGET			afcCapitalsToggle;

    APP_WIDGET			afcApplyRow;
    APP_WIDGET			afcRevertButton;
    APP_WIDGET			afcApplyButton;

    APP_WIDGET			afcBaselineRow;
    APP_WIDGET			afcBaselineLabel;
    APP_WIDGET			afcBaselineText;

    int				afcFontSortIndexChosen;
    int				afcFaceChosen;
    PropertyMask		afcChosenMask;
    ExpandedTextAttribute	afcTextAttributeChosen;

    PropertyMask		afcSetMask;
    ExpandedTextAttribute	afcTextAttributeSet;

    FontChooserSetFont		afcSetFont;
    
    DocumentFontList		afcDocumentFontList;

    double			afcPixelsPerTwip;
    DrawingSurface		afcDrawingSurface;
    const struct PostScriptFontList *	afcPostScriptFontList;
    int				afcScreenFont;
    char			afcChoiceText[120+1];
    MemoryBuffer		afcScreenChoiceText;
    int				afcFaceMapFwd[FONTface_COUNT];
    int				afcFaceMapBck[FONTface_COUNT];

    const AppFontToolResources * afcResources;
    } AppFontChooser;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void appFontToolFillPage(AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

extern void appFontToolGetResourceTable(struct EditApplication *	ea,
					AppFontToolResources *		aftr );

extern void appFontChooserCleanPage( AppFontChooser *	afc );

extern void appFontToolFillChoosers(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern void appFontToolFinishPage(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern int appFontToolShowCurrentFont(
				AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const TextAttribute *		taNew,
				unsigned int			documentId,
				int				canChange,
				const DocumentFontList *	dfl,
				const struct ColorPalette *	cp );

#   endif /*	APP_FONT_TOOL_H  */
