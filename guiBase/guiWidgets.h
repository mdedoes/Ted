/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_WIDGETS_H
#   define	GUI_WIDGETS_H

#   include	"guiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

struct MemoryBuffer;
struct NamedImage;

/************************************************************************/
/*									*/
/*  Possible responses in one of Question dialogs.			*/
/*									*/
/************************************************************************/

typedef enum DialogResponse
    {
    AQDrespFAILURE= -1,
    AQDrespNONE= 0,
    AQDrespYES,
    AQDrespNO,
    AQDrespCANCEL,
    AQDrespCLOSED,	/*  Cancel, but dialog is closed by the user */
    AQDrespOK,
    AQDresp_COUNT
    } DialogResponse;

/************************************************************************/
/*									*/
/*  Describes a document generically.					*/
/*									*/
/************************************************************************/

typedef struct DocumentWidget
    {
#   if USE_HEADLESS
    int		dwHeadless;
#   else

    APP_WIDGET			dwWidget;
    APP_INPUT_CONTEXT		dwInputContext;

				/****************************************/
				/*  Event handling.			*/
				/*  Note that this is not a full GUI	*/
				/*  toolkit!				*/
				/****************************************/
    void *			dwOwner;
    void			(*dwGotString)(	void *		owner,
						const char *	value,
						int		length );
    void			(*dwGotKey)(	void *		owner,
						int		key,
						unsigned int	state );
#   endif
    } DocumentWidget;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   if ! USE_HEADLESS
extern void guiMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		buttonRow,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault );

extern void guiMakeButtonInColumn(
				APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		buttonText,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault );

extern void guiMakeImageButtonInColumn( APP_WIDGET *	pButton,
				APP_WIDGET		column,
				struct NamedImage *	image,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through );

extern void guiMakeImageButtonInRow( APP_WIDGET *	pButton,
				APP_WIDGET		row,
				struct NamedImage *	image,
				const char *		tooltip,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan );

extern void guiMakeLabelInRow(		APP_WIDGET *		pLabel,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					const char *		labelText );

extern void guiMakeLabelInHBox(		APP_WIDGET *		pLabel,
					APP_WIDGET		hbox,
					const char *		labelText );

extern void guiMakeTextInRow(		APP_WIDGET *		pText,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					int			textColumns,
					int			textEnabled );

extern void guiMakeTextInHBox(		APP_WIDGET *		pText,
					APP_WIDGET		hbox,
					int			textColumns,
					int			textEnabled );

extern APP_WIDGET guiMakeToggleInRow(	APP_WIDGET		buttonRow,
					const char *		text,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			col,
					int			colspan );

extern void guiMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title );

extern void guiMakeLabelInColumn(	APP_WIDGET *		pLabel,
					APP_WIDGET		row,
					const char *		labelText );

extern void guiMakeTextInColumn(	APP_WIDGET *	pText,
					APP_WIDGET	column,
					int		textColumns,
					int		textEnabled );

extern void guiMakeColumnToggle(	APP_WIDGET *		pToggle,
					APP_WIDGET		column,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					const char *		labelText,
					int			set );

extern void guiMakeColumnDrawing(	APP_WIDGET *		pButton,
					APP_WIDGET		column,
					APP_DRAW_BUTTON_CALLBACK exposeCallback,
					APP_BUTTON_CALLBACK_T	pushedCallback,
					void *			through,
					int			width );

extern void guiEnableText(		APP_WIDGET		text,
					int			enabled );

extern char *	guiStringFromText(	APP_WIDGET		w );
extern void	guiFreeStringFromTextWidget(	char *		s );

extern char *	guiGetTextFromMenuOption(	APP_WIDGET	w );
extern void	guiFreeTextFromMenuOption(	char *		s );

extern void guiStringToText(		APP_WIDGET		w,
					const char *		s );

extern void guiChangeButtonText(	APP_WIDGET		button,
					const char *		label );

extern APP_WIDGET guiMakeRowInColumn(	APP_WIDGET	parent,
					int		columnCount,
					int		heightResizable );

extern void guiMakeColumnInRow(		APP_WIDGET *	pColumn,
					APP_WIDGET	row,
					int		position,
					int		colspan );

extern void guiMakeHBoxInRow(		APP_WIDGET *	pHbox,
					APP_WIDGET	row,
					int		position,
					int		colspan );

extern void guiMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					int			columnCount,
					const char *		title );

extern void guiTextSelectContents(	APP_WIDGET	w,
					int		from,
					int		upto );

extern void guiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				APP_WIDGET			column,
				int				wide,
				int				high,
				int				heightResizable,
				APP_REDRAW_HANDLER_T		redraw,
				void *				through );

extern void guiChangeLabelText( APP_WIDGET			w,
				const char *			labelt );

extern void guiDocumentSetInputContext(		APP_INPUT_METHOD	im,
						DocumentWidget *	dw );

# if USE_GTK
    extern int guiGetMenuToggleStateFromCallbackGtk(
					APP_WIDGET		toggle );
    # define guiGetMenuToggleStateFromCallback( t, e ) \
			    guiGetMenuToggleStateFromCallbackGtk( (t) )
# else
    extern int guiGetMenuToggleStateFromCallback(
					APP_WIDGET		toggle,
					void *			voidcbs );
# endif

# if USE_GTK
    extern int guiGetScrollbarValueFromCallbackGtk(
					APP_WIDGET		scrollbar );

    # define guiGetScrollbarValueFromCallback( bar, e ) \
			    guiGetScrollbarValueFromCallbackGtk( (bar) )
# else
    extern int guiGetScrollbarValueFromCallback(
					APP_WIDGET		scrollbar,
					void *			e );
# endif

/**
 *  Construct a list of values in the column
 *
 *  @param visibleItems: The number of items to display.
 *	if visibleItems > 0: Allocate sufficient space for this many items. 
 *		and use a scrollbar if there are items more in the list.
 *	if visibleItems == 0: Adapt to the available space in the parent.
 *		Use a scrollbar if there are more items in the list than fit 
 *		in the assigned space.
 *	if visibleItems < 0: Resize with the number of itemsd in the list.
 *
 */
extern void guiMakeListInColumn(	APP_WIDGET *		pList,
					APP_WIDGET		column,
					int			visibleItems,
					APP_LIST_CALLBACK_T	listCallback,
					APP_BUTTON_CALLBACK_T	actionCallback,
					void *			through );

extern void guiSetTypingCallbackForText(
					APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through );

extern void guiSetGotValueCallbackForText(
				    APP_WIDGET			text,
				    APP_TXACTIVATE_CALLBACK_T	callBack,
				    void *			through );

extern void guiGetScrollbarValues(	int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar );

extern void guiSetScrollbarValues(	APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize );

extern void guiEmptyListWidget(	APP_WIDGET		list );

extern void guiAddValueToListWidget(	APP_WIDGET		list,
					int			pos,
					const char *		value );

extern void guiReplaceValueInListWidget(
					APP_WIDGET		list,
					int			pos,
					const char *		value );

extern void guiDeletePositionFromListWidget(
					APP_WIDGET		list,
					int			pos );

extern void guiSelectPositionInListWidget(	APP_WIDGET	list,
						int		position );
extern void guiSelectValueInListWidget(	APP_WIDGET	list,
						const char *	value );

extern void guiRemoveSelectionFromListWidget(	APP_WIDGET	list );

extern void guiMakeSliderInRow(	APP_WIDGET *		pSlider,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					APP_BUTTON_CALLBACK_T	callback,
					void *			through );

extern void guiSetSlider(		APP_WIDGET		slider,
					int			minVal,
					int			maxVal,
					int			curVal );

extern void guiSetSliderValue(	APP_WIDGET		slider,
					int			curVal );

extern int guiGetValueFromSliderCallback(	int *		pValue,
						APP_WIDGET	w,
						void *		voidscs );

extern int guiGetPositionFromListCallback(
					    APP_WIDGET		list,
					    APP_LIST_CHOICE *	voidlcs );

extern char * guiGetStringFromListCallback(
					    APP_WIDGET		list,
					    APP_LIST_CHOICE *	voidlcs );

extern void guiFreeStringFromListCallback(	char *		s );

extern void guiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
						APP_WIDGET	parent );

extern void guiInsertColumnInWindow(	APP_WIDGET *		pColumn,
					APP_WIDGET		parent );

extern void guiInsertMenubarInColumn( APP_WIDGET *		pMenubar,
					APP_WIDGET		parent );

extern void guiSetToggleLabel(	APP_WIDGET		toggle,
					const char *		text );

extern void guiSetToggleState(	APP_WIDGET		toggle,
					int			set );

extern int guiGetToggleState(	APP_WIDGET		toggle );

extern void guiEnableWidget(		APP_WIDGET		w,
					int			on_off );

extern void guiSetWidgetVisible(	APP_WIDGET		w,
					int			on_off );

extern void guiHideShellWidget(		APP_WIDGET		shell );
extern void guiDestroyShellWidget(	APP_WIDGET		shell );

extern void guiRefuseTextValue(		APP_WIDGET		w );

extern void guiSetToggleItemState(	APP_WIDGET		toggle,
					int			set );

extern void guiSetToggleItemLabel(	APP_WIDGET		toggle,
					const char *		label );

extern void guiShowMenuOption(		APP_WIDGET		w,
					int			visible );

extern void guiSetDialogTitle(		APP_WIDGET		shell,
					APP_WIDGET		option,
					const char *		appname );

extern void guiSetShellTitle(		APP_WIDGET		shell,
					const struct MemoryBuffer *	title );

extern void guiSetIconTitle(		APP_WIDGET		shell,
					const struct MemoryBuffer *	title );

extern int guiSetFrameTitle(		APP_WIDGET		w,
					const char *		title );

extern void guiSetDefaultButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void guiSetCancelButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void guiSetFocusChangeHandler( APP_WIDGET		shell,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiSetDestroyCallback(	APP_WIDGET		shell,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through );

extern void guiSetCloseCallback(	APP_WIDGET		shell,
					APP_CLOSE_CALLBACK_T	closeCallback,
					void *			through );

extern void guiFocusToWidget(		APP_WIDGET		w );

extern void guiSetWindowIcon(		APP_WIDGET		topLevel,
					struct NamedImage *	image );

extern void guiMakeImageInColumn(	APP_WIDGET *		pImage,
					APP_WIDGET		column,
					struct NamedImage *	image );

extern void guiMakeImageInRow(		APP_WIDGET *		pImageW,
					APP_WIDGET		row,
					struct NamedImage *	image,
					int			position,
					int			colspan );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif
