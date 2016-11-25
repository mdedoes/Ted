#   include	"guiBaseConfig.h"
#   include	"guiBase.h"

#   if USE_GTK

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"guiListGtk.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

static const char	GUIGTK_ListAction[]= "ListAction";

static void guiGtkFreeListAction( void * voidla )
    { free( voidla );	}

static void guiListGtkResized(	GtkWidget *		w,
				GdkRectangle *		allocation,
				gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    la->laListHigh= allocation->height;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	guiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    /*  NO gtk_adjustment_set_upper( la->laAdjustment ): crashes Gtk ) */
    }

static void guiListGtkParentResized(	GtkWidget *		w,
					GdkRectangle *		allocation,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    la->laParentHigh= allocation->height;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	guiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    /*  NO gtk_adjustment_set_page_size( la->laAdjustment ): crashes Gtk ) */
    }

static void guiListGtkSetFocusChild(	GtkTreeView *		parent,
					GtkWidget *		w,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	guiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    }

ListAction * guiListGtkSetAdmin(	GtkWidget *		widget,
					APP_LIST_CALLBACK_T	listCallback,
					void *			through )
    {
    ListAction *	la= malloc( sizeof(ListAction) );

    if  ( ! la )
	{ XDEB(la); return (void *)0;	}

    la->laWidget= widget;
    la->laParentWidget= (GtkWidget *)0;
    la->laAdjustment= (GtkAdjustment *)0;
    la->laThrough= through;
    la->laActionCallback= (APP_BUTTON_CALLBACK_T)0;
    la->laListCallback= listCallback;

    la->laListHigh= 0;
    la->laParentHigh= 0;
    la->laItemCount= 0;
    la->laSelectedPosition= -1;

    g_object_set_data_full( G_OBJECT(widget), GUIGTK_ListAction, la,
						    guiGtkFreeListAction );

    return la;
    }

ListAction * guiListGtkSetCallbacks(
				GtkWidget *		widget,
				GtkWidget *		scrolled_window,
				GtkAdjustment *		adjustment,
				APP_LIST_CALLBACK_T	listCallback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    ListAction *	la;

    la= guiListGtkSetAdmin( widget, listCallback, through );
    if  ( ! la )
	{ XDEB(la); return (void *)0;	}

    la->laParentWidget= scrolled_window;
    la->laAdjustment= adjustment;
    la->laActionCallback= actionCallback;

    g_signal_connect( G_OBJECT( la->laWidget ), "size-allocate",
			    (GCallback)guiListGtkResized, (void *)la );
    if  ( la->laParentWidget )
	{
	g_signal_connect( G_OBJECT( la->laParentWidget ), "size-allocate",
			    (GCallback)guiListGtkParentResized, (void *)la );
	}

    g_signal_connect( G_OBJECT( la->laParentWidget ), "set-focus-child",
			    (GCallback)guiListGtkSetFocusChild, (void *)la );

    return la;
    }

ListAction * guiGtkGetListAction(
				GtkWidget *		widget )
    {
    return (ListAction *)g_object_get_data(
			    G_OBJECT(widget), GUIGTK_ListAction );
    }

void guiGtkSetListAdjustment(		ListAction *	la,
						int		position )
    {
    int	increment;
    int	old_value;
    int	new_value;

    la->laSelectedPosition= position;

    if  ( la->laItemCount == 0 )
	{ /*LLDEB(position,la->laItemCount);*/ return; }

    increment= la->laListHigh/ la->laItemCount;
    if  ( increment == 0 )
	{ /*LDEB(increment);*/ return;	}

    old_value= gtk_adjustment_get_value( la->laAdjustment );
    new_value= position* increment;

    if  ( new_value < old_value )
	{
	gtk_adjustment_set_value( la->laAdjustment, new_value );
	}
    else{
	int	pagesize= la->laParentHigh;

	if  ( new_value+ increment > old_value+ pagesize )
	    {
	    int	upper= la->laListHigh;
	    int	visible_increments= la->laParentHigh/ increment;

	    new_value -= ( visible_increments- 1 )* increment;

	    if  ( new_value > upper- pagesize )
		{ new_value= upper- pagesize;	}

	    gtk_adjustment_set_value( la->laAdjustment, new_value );
	    }
	}

    return;
    }

void guiGtkAddValueToList(	GtkTreeModel *		model,
				ListAction *		la,
				int			position,
				const char *		value,
				int			sensitive )
    {
    const int		vacol= 0;
    const int		secol= 1;

    GtkListStore *	store= GTK_LIST_STORE( model );

    GtkTreePath *	ppath= (GtkTreePath *)0;
    GtkTreeIter		iter;

    if  ( position >= 0 && position < la->laItemCount )
	{
	GtkTreeIter	positer;

	ppath= gtk_tree_path_new_from_indices( position, -1 );
	gtk_tree_model_get_iter( model, &positer, ppath );

	gtk_list_store_insert_before( store, &iter, &positer );
	}
    else{
	gtk_list_store_append( store, &iter );
	}

    if  ( la )
	{ la->laItemCount++;	}

    gtk_list_store_set( store, &iter,
				vacol, value,
				secol, sensitive != 0,
				-1 );

    if  ( ppath )
	{ gtk_tree_path_free( ppath );	}

    return;
    }

void guiGtkReplaceValueInList(	GtkTreeModel *	model,
				int		position,
				const char *	value )
    {
    const int		vacol= 0;

    GtkListStore *	store;

    GtkTreePath *	path;
    GtkTreeIter		iter;

    store= GTK_LIST_STORE( model );

    path= gtk_tree_path_new_from_indices( position, -1 );
    gtk_tree_model_get_iter( model, &iter, path );

    gtk_list_store_set( store, &iter, vacol, value, -1 );

    gtk_tree_path_free( path );
    }

void guiGtkDeleteValueFromList(	GtkTreeModel *	model,
					ListAction *	la,
					int		position )
    {
    GtkListStore *	store= GTK_LIST_STORE( model );
    GtkTreePath *	path= gtk_tree_path_new_from_indices( position, -1 );
    GtkTreeIter		iter;

    if  ( position == la->laSelectedPosition )
	{
	la->laSelectedPosition= -1;
	}

    gtk_tree_model_get_iter( model, &iter, path );
    gtk_list_store_remove( store, &iter );
    gtk_tree_path_free( path );

    la->laItemCount--;
    }

void guiListGtkSetRowFlag(		GtkTreeModel *		model,
				int			position,
				int			col,
				int			flag )
    {
    GtkListStore *	store= GTK_LIST_STORE( model );
    GtkTreePath *	path= gtk_tree_path_new_from_indices( position, -1 );
    GtkTreeIter		iter;

    gtk_tree_model_get_iter( model, &iter, path );

    gtk_list_store_set( store, &iter, col, flag != 0, -1 );

    gtk_tree_path_free( path );

    return;
    }

#   endif
