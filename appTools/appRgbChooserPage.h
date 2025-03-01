/************************************************************************/
/*									*/
/*  RGB chooser page on an inspector.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_RGB_CHOOSER_PAGE_H
#   define	APP_RGB_CHOOSER_PAGE_H

#   include	<guiBase.h>
#   include	"appRgbCube.h"

struct EditApplication;

/************************************************************************/
/*									*/
/*  Resources for the RGB chooser page.					*/
/*									*/
/************************************************************************/

typedef struct RgbChooserPageResources
    {
    char *		rcprCancelText;

    char *		rcprRedText;
    char *		rcprGreenText;
    char *		rcprBlueText;
    } RgbChooserPageResources;

/************************************************************************/
/*									*/
/*  A RGB chooser tool. Select RGB colors by typing or clicking.	*/
/*									*/
/************************************************************************/

struct RgbChooser;
struct InspectorSubject;
struct InspectorSubjectResources;
struct AppInspector;

typedef void (*RgbChooserCallback)(	struct RgbChooser *	rcp,
					const RGB8Color *	rgb8New,
					void *			through );

typedef struct RgbChooser
    {
    struct EditApplication *		rcpApplication;
    struct AppInspector *		rcpInspector;
    const RgbChooserPageResources *	rcpPageResources;

    RGB8Color				rcpRGBSet;
    RGB8Color				rcpRGBChosen;

    int					rcpSubject;
    int					rcpProperty;

    APP_WIDGET				rcpDrawing;
    DrawingSurface			rcpDrawingSurface;

    APP_WIDGET				rcpRedToggle;
    APP_WIDGET				rcpGreenToggle;
    APP_WIDGET				rcpBlueToggle;

    APP_WIDGET				rcpRedText;
    APP_WIDGET				rcpGreenText;
    APP_WIDGET				rcpBlueText;

    APP_WIDGET				rcpCancelButton;

    RgbCube				rcpRgbCube;
    } RgbChooser;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void appInitRgbChooser(	RgbChooser *			rcp );

extern void appRgbChooserPageFinishPage(
				RgbChooser *			rcp );

extern void appRgbChooserRefresh(
				RgbChooser *			rcp,
				int *				pEnabled,
				struct InspectorSubject *	is,
				const RGB8Color *		rgbSet );

extern void appRgbChooserPageFillPage(
				RgbChooser *			rcp,
				const RgbChooserPageResources *	plpr,
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

extern void appRgbChooserPageGetResourceTable(
				struct EditApplication *	ea,
				RgbChooserPageResources *	rcpr,
				struct InspectorSubjectResources *	isr );

extern void appRgbChooserPageCleanPage(
				RgbChooser *			rcp );

extern void appRgbChooserSetContext(	RgbChooser *		rcp,
					int			subjectPage,
					int			property );

#   endif	/*  APP_RGB_CHOOSER_PAGE_H */
