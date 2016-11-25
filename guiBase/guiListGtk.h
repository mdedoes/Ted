#   include	"guiBaseConfig.h"
#   include	"guiBase.h"

#   if USE_GTK

typedef struct ListAction
    {
    GtkWidget *			laWidget;
    GtkWidget *			laParentWidget;
    GtkAdjustment *		laAdjustment;
    APP_LIST_CALLBACK_T		laListCallback;
    APP_BUTTON_CALLBACK_T	laActionCallback;
    void *			laThrough;

    int				laListHigh;
    int				laParentHigh;
    int				laItemCount;
    int				laSelectedPosition;
    } ListAction;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern ListAction * guiListGtkSetCallbacks(
					GtkWidget *		treev,
					GtkWidget *		scrolled_window,
					GtkAdjustment *		adjustment,
					APP_LIST_CALLBACK_T	listCallback,
					APP_BUTTON_CALLBACK_T	actionCallback,
					void *			through );

extern ListAction * guiListGtkSetAdmin(	GtkWidget *		combo,
					APP_LIST_CALLBACK_T	listCallback,
					void *			through );

extern ListAction * guiGtkGetListAction(
					GtkWidget *		widget );

extern void guiGtkSetListAdjustment(	ListAction *		la,
					int			position );

extern void guiGtkAddValueToList(	GtkTreeModel *		model,
					ListAction *		la,
					int			position,
					const char *		value,
					int			sensitive );

extern void guiGtkReplaceValueInList(	GtkTreeModel *		model,
					int			position,
					const char *		value );

extern void guiGtkDeleteValueFromList(	GtkTreeModel *		model,
					ListAction *		la,
					int			position );

extern void guiListGtkSetRowFlag(	GtkTreeModel *		model,
					int			position,
					int			col,
					int			flag );

#   endif
