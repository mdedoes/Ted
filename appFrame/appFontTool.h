/************************************************************************/
/*									*/
/*  A Font/Text attribute Selection Page on an inspector.		*/
/*									*/
/************************************************************************/

#   ifndef APP_FONT_TOOL_H
#   define APP_FONT_TOOL_H

#   include	<docExpandedTextAttribute.h>
#   include	<appFrame.h>
#   include	<appEncodingMenu.h>
#   include	<appColorChooser.h>

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
    char *			aftrTextColor;

    char *			aftrTextUnderlined;
    char *			aftrTextStrikethrough;

    char *			aftrSuperscript;
    char *			aftrSubscript;

    char *			aftrSmallcaps;
    char *			aftrCapitals;

    ColorChooserResources	aftrTextColorChooserResources;

    char *			aftrEncodings[ENCODINGps_COUNT];
    } AppFontToolResources;

typedef void (*FontChooserSetFont)(
				void *				target,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

typedef struct AppFontChooser
    {
    EditApplication *		afcApplication;
    AppInspector *		afcInspector;
    int				afcSubjectPage;

    APP_WIDGET			afcFamilyList;
    APP_WIDGET			afcFaceList;
    APP_WIDGET			afcSizeList;
    APP_WIDGET			afcSizeText;

    APP_WIDGET			afcColorRow;
    APP_WIDGET			afcTextColorLabel;
    ColorChooser		afcTextColorChooser;

    APP_WIDGET			afcUnderlinedToggle;
    APP_WIDGET			afcStrikethroughToggle;

    APP_WIDGET			afcSuperscriptToggle;
    APP_WIDGET			afcSubscriptToggle;

    APP_WIDGET			afcSmallcapsToggle;
    APP_WIDGET			afcCapitalsToggle;

    APP_WIDGET			afcButtonRow;
    APP_WIDGET			afcRevertButton;
    APP_WIDGET			afcSetButton;

    APP_WIDGET			afcSampleDrawing;

    AppEncodingMenu		afcEncodingMenu;

    int				afcFontFamilyChosen;
    int				afcFaceChosen;
    PropertyMask		afcChosenMask;
    ExpandedTextAttribute	afcTextAttributeChosen;

    PropertyMask		afcSetMask;
    ExpandedTextAttribute	afcTextAttributeSet;

    void *			afcTarget;

    FontChooserSetFont		afcSetFont;
    
    AppFontFamily *		afcFontFamilies;
    int				afcFontFamilyCount;

    AppDrawingData		afcDrawingData;
    APP_FONT *			afcFont;
    char			afcChoiceText[120+1];
    } AppFontChooser;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern int appFontSetCurrentFont(
				AppFontChooser *		afc,
				const PropertyMask *		updMask,
				const ExpandedTextAttribute *	etaNew,
				const RGB8Color *		colors,
				int				colorCount );

extern void appFontToolFillPage(AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appFontToolGetResourceTable(EditApplication *		ea,
					AppFontToolResources *		aftr,
					InspectorSubjectResources *	isr );

extern void appFontChooserCleanPage( AppFontChooser *	afc );

extern void appFontToolFillChoosers(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern void appFontToolFinishPage(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

#   endif /*	APP_FONT_TOOL_H  */
