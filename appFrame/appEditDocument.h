/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_EDIT_DOCUMENT_H
#   define	APP_EDIT_DOCUMENT_H

#   include	<guiBase.h>

#   include	<geoRectangle.h>
#   include	<guiWidgets.h>
#   include	<utilColor.h>
#   include	<utilMemoryBuffer.h>

# ifdef __cplusplus
extern "C"
    {
# endif

struct EditDocument;
struct AppSelectionTargetType;

typedef struct SelectRectangle
    {
    int				srDirection;
#				define DOCselNONE	0

#				define DOCselN		1
#				define DOCselE		2
#				define DOCselS		3
#				define DOCselW		4

    DocumentRectangle		srSelected;

				/**
				 * left/top margins
				 */
    DocumentRectangle		srLTM;

				/**
				 * right/bottom margins
				 */
    DocumentRectangle		srRBM;
    } SelectRectangle;

typedef struct EditDocument
    {
    struct EditApplication *	edApplication;

    MemoryBuffer		edFilename;
    MemoryBuffer		edTitle;
    int				edFormat;
    int				edFileReadOnly;
				/**
				 *  Unique number. Can be used to distinguish
				 *  EditDocuments wilhout remembering pointers
				 */
    unsigned int		edDocumentId;

    void *			edPrivateData;
    struct DrawingSurface *	edDrawingSurface;

    unsigned char		edHasBeenChanged;
    unsigned char		edIsReadonly;
    unsigned char		edIsVisible;

#   if ! USE_HEADLESS
    AppToplevel			edToplevel;

    APP_WIDGET			edMenuBar;
    APP_WIDGET			edMainWindow;
    APP_WIDGET				edFileMenu;
    APP_WIDGET				edFileMenuButton;
    APP_WIDGET					edFileCloseOption;
    APP_WIDGET				edEditMenu;
    APP_WIDGET				edEditMenuButton;
    APP_WIDGET				edWindowMenu;
    APP_WIDGET				edWindowMenuButton;
    APP_WIDGET				edHelpMenu;
    APP_WIDGET				edHelpMenuButton;

    APP_WIDGET			edToolbar;
    APP_WIDGET			edScrolledWindow;
    APP_WIDGET				edVerticalScrollbar;
    APP_WIDGET				edHorizontalScrollbar;
    APP_WIDGET				edWorkWidget;
#   if USE_GTK
    GtkAdjustment *			edVerticalAdjustment;
    GtkAdjustment *			edHorizontalAdjustment;
#   endif
    DocumentWidget			edDocumentWidget;

    APP_WIDGET				edLeftRulerWidget;
    APP_WIDGET				edTopRulerWidget;
    APP_WIDGET				edRightRulerWidget;
    APP_WIDGET				edBottomRulerWidget;

    void *				edLeftRuler;
    void *				edTopRuler;
    void *				edRightRuler;
    void *				edBottomRuler;

    int					edLeftRulerWidePixels;
    int					edTopRulerHighPixels;
    int					edRightRulerWidePixels;
    int					edBottomRulerHighPixels;

				    /**
				     *  The (pixel) rectangle that the whole
				     *  document would occupy on screen.
				     *  I.E: On a huge screen without 
				     *  scrollbars. X0 and X0 are 0 by 
				     *  definition.
				     */
    DocumentRectangle		edFullRect;
				    /**
				     *  The (pixel) rectangle of the document
				     *  that is visible ob screen. Together 
				     *  with edFullRect, it determines the 
				     *  positions of the scrollbars.
				     */
    DocumentRectangle		edVisibleRect;

				    /**
				     *  The color to draw where the window is
				     *  not covered by the document.
				     */
    RGB8Color			edBackgroundColor;

    struct AppSelectionTargetType *
				edTargetTypes;
    int				edTargetTypeCount;

    int				edMapped;	/*  Struggle with fvwm	*/
    int				edNotYetDrawn;	/*  For FirstVisible	*/

				/**
				 *  Do not react to events caused by 
				 *  programmatic changes.
				 */
    int				edInProgrammaticChange;

    SelectRectangle		edSelectRectangle;

				/**
				 *  Set of tools to manage document properties
				 */
    struct AppInspector *	edInspector;

#   endif

    } EditDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInitSelectRectangle(	SelectRectangle *	sr );

extern void appDocumentChanged(	struct EditDocument *	ed,
				int			changed );

extern void appDocSetScrollbarValues(	struct EditDocument *	ed );

extern void appMouseWheelUp(		struct EditDocument *	ed );
extern void appMouseWheelDown(		struct EditDocument *	ed );

extern int appSetDocumentFilename(	struct EditDocument *	ed,
					const struct MemoryBuffer *	filename );

extern int appSetDocumentTitle(		struct EditDocument *	ed,
					const struct MemoryBuffer *	title );

# ifdef __cplusplus
    }
# endif

#   endif
