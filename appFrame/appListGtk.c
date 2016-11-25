#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK
#   define	USE_GTK_LIST
#   endif

#   ifdef USE_GTK_TREE

Code still is very immature and experimental

void appGuiMakeListInColumn(	APP_WIDGET *		pList,
				APP_WIDGET		column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	callback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    gboolean		expand= TRUE;
    gboolean		fill= TRUE;

    GtkListStore *	store= gtk_list_store_new( 1, G_TYPE_STRING );
    APP_WIDGET		tree;

    tree= gtk_tree_view_new_with_model( GTK_TREE_MODEL( store ) );
    gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( tree ), 0 );

    gtk_box_pack_start( GTK_BOX( column ), tree, expand, fill, 0 );

    gtk_widget_show( tree );

    gtk_tree_view_insert_column_with_attributes(
				GTK_TREE_VIEW( tree ),
				-1,
				"col",
				gtk_cell_renderer_text_new(),
				"text", 0, NULL );

    if  ( callback )
	{
	GtkTreeSelection *	select;

	select= gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );
	gtk_tree_selection_set_mode( select, GTK_SELECTION_SINGLE );

	g_signal_connect( G_OBJECT( select ), "changed",
					(GtkSignalFunc)callback, through );
	}

    if  ( actionCallback )
	{
	gtk_signal_connect( GTK_OBJECT( tree ), "row-activated",
				    (GtkSignalFunc)actionCallback, through );
	}

    *pList= tree;
    }

void appGuiAddValueToListWidget(	APP_WIDGET		tree,
					int			pos,
					const char *		value )
    {
    const int		col= 0;

    GtkTreeView *	treev;
    GtkListStore *	store;

    GtkTreeIter		iter;

    treev= GTK_TREE_VIEW( tree );
    store= GTK_LIST_STORE( gtk_tree_view_get_model( treev ) );

    gtk_list_store_append( store, &iter );
    gtk_list_store_set( store, &iter, col, value, -1 );
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	list,
						int		position )
    {
LDEB(position); return;
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list )
    {
LDEB(0); return;
    }

void appGuiEmptyListWidget(	APP_WIDGET		tree )
    {
    GtkTreeView *	treev;
    GtkListStore *	store;

    treev= GTK_TREE_VIEW( tree );
    store= GTK_LIST_STORE( gtk_tree_view_get_model( treev ) );

    gtk_list_store_clear( store );
    }

int appGuiGetPositionFromListCallback(		APP_WIDGET	list,
						void *		voiditem )
    {
LDEB(1); return -1;
    }

void appFreeStringFromListCallback(	char *		s )
    { return;	}

char * appGuiGetStringFromListCallback(	APP_WIDGET		list,
					APP_LIST_CHOICE *	item )
    {
LDEB(5); return (char *)0;
    }

void appGuiReplaceValueInListWidget(	APP_WIDGET	list,
					int		pos,
					const char *	value )
    {
LDEB(pos); return;
    }

#   endif

#   ifdef USE_GTK_LIST

/************************************************************************/
/*									*/
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

typedef struct ListAction
    {
    APP_WIDGET			laList;
    APP_BUTTON_CALLBACK_T	laActionCallback;
    void *			laThrough;
    } ListAction;

static const char	APP_ListAction[]= "ListAction";

static void appGuiGtkFreeListAction( void * voidla )
    { free( voidla );	}

static gboolean appGuiGtkHandlePotentialListAction(
					GtkWidget *		item,
					GdkEventButton *	e,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la && e->type == GDK_2BUTTON_PRESS )
	{
	(*la->laActionCallback)( la->laList, la->laThrough );
	return TRUE;
	}

    return FALSE;
    }

void appGuiMakeListInColumn(	APP_WIDGET *		pList,
				APP_WIDGET		column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	callback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    APP_WIDGET		list;
    APP_WIDGET		scrolled_window;
    GtkAdjustment *	vadjust;

    gboolean		expand= TRUE;
    gboolean		fill= TRUE;

    scrolled_window= gtk_scrolled_window_new(
				(GtkAdjustment *)0, (GtkAdjustment *)0 );

    gtk_box_pack_start( GTK_BOX( column ), scrolled_window, expand, fill, 0 );

    gtk_scrolled_window_set_placement( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_CORNER_BOTTOM_LEFT );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_POLICY_NEVER,
						    GTK_POLICY_AUTOMATIC );

    vadjust= gtk_scrolled_window_get_vadjustment(
				    GTK_SCROLLED_WINDOW( scrolled_window ) );
    gtk_container_set_focus_vadjustment(
				    GTK_CONTAINER( scrolled_window ), vadjust );

    gtk_widget_show( scrolled_window );

    list= gtk_list_new();

    gtk_scrolled_window_add_with_viewport(
			    GTK_SCROLLED_WINDOW( scrolled_window ), list );

    gtk_list_set_selection_mode( GTK_LIST( list ), GTK_SELECTION_SINGLE );

    gtk_container_set_focus_vadjustment( GTK_CONTAINER( list ),
			    gtk_scrolled_window_get_vadjustment(
				    GTK_SCROLLED_WINDOW( scrolled_window ) ) );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( list ), "select-child",
					    (GtkSignalFunc)callback, through );
	}

    if  ( actionCallback )
	{
	ListAction *	la= malloc( sizeof(ListAction) );

	if  ( la )
	    {
	    la->laList= list;
	    la->laThrough= through;
	    la->laActionCallback= actionCallback;

	    gtk_object_set_data_full( GTK_OBJECT(list), APP_ListAction, la,
						    appGuiGtkFreeListAction );
	    }
	}

    gtk_widget_show( list );

    if  ( visibleItems > 0 )
	{
	int		pixelSize= 9;
	GtkStyle *	gs= gtk_widget_get_style( list );

	if  ( gs->font_desc )
	    {
	    if  ( pango_font_description_get_size_is_absolute( gs->font_desc ) )
		{
		int		sz;

		sz= pango_font_description_get_size( gs->font_desc );
		if  ( sz > 0 )
		    { pixelSize= sz;	}
		}
	    }
	else{
	    GdkFont *	gf;

#	    if GTK_MAJOR_VERSION < 2
	    gf= gs->font;
#	    else
	    gf= gtk_style_get_font( gs );
#	    endif

	    pixelSize= gf->ascent+ gf->descent;
	    }

#	if GTK_MAJOR_VERSION < 2
	gtk_widget_set_usize( scrolled_window, -1,
				    ( 110* visibleItems* pixelSize )/ 100 );
#	else
	gtk_widget_set_size_request( scrolled_window, 0,
				    ( 110* visibleItems* pixelSize )/ 100 );
#	endif
	}

    *pList= list; return;
    }

static GtkWidget * appGuiGetListChild(		APP_WIDGET	list,
						int *		pCount,
						int		position )
    {
    GList *		glf;
    GList *		gl;

    int		n= 0;
    GtkWidget *	item= (GtkWidget *)0;

    gl= glf= gtk_container_children( GTK_CONTAINER( list ) );

    while( gl )
	{
	if  ( n == position )
	    {
	    item= (GtkWidget *)gl->data;
	    if  ( ! pCount )
		{ break;	}
	    }
	n++; gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    if  ( pCount )
	{ *pCount= n;	}
    return item;
    }

void appGuiEmptyListWidget(	APP_WIDGET		list )
    {
#   define	GTK_LIST_BUG 1
#   if		GTK_LIST_BUG

    GList *		glist= (GList *)0;
    GtkWidget *		item= gtk_list_item_new_with_label( "Gtk bug" );

    glist= g_list_append( glist, item );

    gtk_list_append_items( GTK_LIST( list ), glist );

    /*  No:
    g_list_free( glist );
    */

#   endif

    gtk_list_unselect_all( GTK_LIST( list ) );
    gtk_list_clear_items( GTK_LIST( list ), 0, -1 );

    return;
    }

void appGuiDeletePositionFromListWidget(	APP_WIDGET	list,
						int		pos )
    {
    gtk_list_clear_items( GTK_LIST( list ), pos, pos+ 1 );
    }

void appGuiReplaceValueInListWidget(	APP_WIDGET	list,
					int		pos,
					const char *	value )
    {
    GtkWidget *	item= appGuiGetListChild( list, (int *)0, pos );

    if  ( item )
	{
#	if GTK_MAJOR_VERSION >= 2
	GtkWidget *	labelw= gtk_bin_get_child( GTK_BIN( item ) );
#	else
	GtkWidget *	labelw= GTK_BIN( item )->child;
#	endif

	gtk_label_set_text( GTK_LABEL( labelw ), value );
	}

    return;
    }

void appGuiAddValueToListWidget(	APP_WIDGET		list,
					int			pos,
					const char *		value )
    {
    GList *		glist= (GList *)0;
    GtkWidget *		item= gtk_list_item_new_with_label( value );
    ListAction *	la;

    glist= g_list_append( glist, item );

    if  ( pos < 0 )
	{
	gtk_list_append_items( GTK_LIST( list ), glist );
	}
    else{
	gtk_list_insert_items( GTK_LIST( list ), glist, pos );
	}

    gtk_widget_show( item );

#   if GTK_MAJOR_VERSION >= 2
    la= (ListAction *)gtk_object_get_data( GTK_OBJECT( list ), APP_ListAction );
    if  ( la )
	{
	gtk_signal_connect( GTK_OBJECT(item),
		     "button-press-event",
		      G_CALLBACK( appGuiGtkHandlePotentialListAction ), la );
	}
#   endif

    /*  No:
    g_list_free( glist );
    */

    return;
    }

static void appGuiGtkScrollToListItem(		APP_WIDGET	list,
						GtkWidget *	item )
    {
    GtkWidget *		scrolled_window;

    scrolled_window= gtk_widget_get_ancestor( list, GTK_TYPE_SCROLLED_WINDOW );

    if  ( ! scrolled_window )
	{ XDEB(scrolled_window); return;	}

    gtk_container_set_focus_child( GTK_CONTAINER( list ), item );
    gtk_container_set_focus_child( GTK_CONTAINER( scrolled_window ), item );

    return;
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	list,
						int		position )
    {
    int			count= 1;
    GtkWidget *		item= appGuiGetListChild( list, &count, position );

    gtk_list_select_item( GTK_LIST( list ), position );

    if  ( count > 0 )
	{
	appGuiGtkScrollToListItem( list, item );
	}

    return;
    }

void appGuiSelectValueInListWidget(		APP_WIDGET	list,
						const char *	value )
    {
    GList *	children= gtk_container_get_children( GTK_CONTAINER( list ) );

    if  ( children )
	{
	int		position= 0;
	GList *		gl= children;

	while( gl )
	    {
	    GtkWidget *	item= (GtkWidget *)gl->data;
	    char *	label;

	    if  ( appGuiGtkGetChildLabel( &label, item )	&&
		  ! strcmp( label, value )			)
		{
		gtk_list_select_item( GTK_LIST( list ), position );
		appGuiGtkScrollToListItem( list, item );
		break;
		}

	    position++; gl= gl->next;
	    }

	g_list_free( children );
	}
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list )
    {
    gtk_list_unselect_all( GTK_LIST( list ) );

    return;
    }

int appGuiGetPositionFromListCallback(		APP_WIDGET	list,
						void *		voiditem )
    {
    GtkWidget *	item= (GtkWidget *)voiditem;


    return gtk_list_child_position( GTK_LIST( list ), item );
    }

char * appGuiGetStringFromListCallback(	APP_WIDGET		list,
					APP_LIST_CHOICE *	item )
    {
    static char	slabel[]= "<< ? >>";
    char *	label= slabel;

    if  ( ! appGuiGtkGetChildLabel( &label, item ) )
	{ SDEB(label);	}

    return label;
    }

void appFreeStringFromListCallback(	char *		s )
    { return;	}

#   endif
