#   include	"guiBaseConfig.h"

#   define	USE_GDK_DEPRECATED
#   include	"guiBase.h"

#   if USE_GTK

#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"guiListGtk.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List specific functionality. (Implemented with a GtkTreeView)	*/
/*									*/
/************************************************************************/

static gboolean guiGtkHandlePotentialListAction(
					GtkTreeView *		treev,
					GtkTreePath *		path,
					GtkTreeViewColumn *	column,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la && la->laActionCallback )
	{
	(*la->laActionCallback)( la->laWidget, la->laThrough );
	return TRUE;
	}

    return FALSE;
    }

static gboolean guiGtkHandlePotentialListCallback(
					GtkTreeSelection *	selection,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la && la->laListCallback )
	{
	int	position;

	position= guiGetPositionFromListCallback( la->laWidget, selection );

	guiGtkSetListAdjustment( voidla, position );

	(*la->laListCallback)( la->laWidget, selection, la->laThrough );
	return TRUE;
	}

    return FALSE;
    }

static gboolean guiGtkListCursorChanged(
					GtkTreeView *		treev,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la )
	{
	GtkTreeSelection *	selection;
	int			position;

	selection= gtk_tree_view_get_selection( treev );
	if  ( ! selection )
	    { return FALSE;	}

	position= guiGetPositionFromListCallback(
					    GTK_WIDGET( treev ), selection );
	guiGtkSetListAdjustment( voidla, position );

	return TRUE;
	}

    return FALSE;
    }

static GtkWidget * appListGtkMakeParent(
				    GtkAdjustment **	pVAdjustment,
				    GtkWidget *		column )
    {
    GtkWidget *		scrolled_window;
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
    gtk_adjustment_set_value( vadjust, 0 );

    gtk_container_set_focus_vadjustment(
				    GTK_CONTAINER( scrolled_window ), vadjust );

    gtk_widget_show( scrolled_window );

    *pVAdjustment= vadjust;
    return scrolled_window;
    }

static void appListGtkSetVisibleSize(	GtkWidget *		list,
					GtkWidget *		scrolled_window,
					int			visibleItems )
    {
    int		pixelSize= 9;

#   if  GTK_MAJOR_VERSION <= 2
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
#   endif

#   if  GTK_MAJOR_VERSION >= 3
    GtkStyleContext *	gs= gtk_widget_get_style_context( list );
    const PangoFontDescription * fontDescription= xxxxx;
#   endif

#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_usize( scrolled_window, -1,
				( 110* visibleItems* pixelSize )/ 100 );
#   else
    gtk_widget_set_size_request( scrolled_window, 0,
				( 110* visibleItems* pixelSize )/ 100 );
#   endif
    }

/************************************************************************/
/*									*/
/*  Build a list widget.						*/
/*									*/
/*  1)  Reserve a (currently) unused boolean to keep track of whether	*/
/*	the row is sensitive. This makes it possible to share code with	*/
/*	the optionmenu.							*/
/*									*/
/************************************************************************/

void guiMakeListInColumn(	APP_WIDGET *		pList,
				APP_WIDGET		column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	listCallback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    GtkListStore *	store;
    APP_WIDGET		treev;

    GtkAdjustment *	vadjustment= (GtkAdjustment *)0;
    APP_WIDGET		scrolled_window= (APP_WIDGET)0;

    GtkTreeSelection *	selection;
    void *		voidla;

    /*  1  */
    store= gtk_list_store_new( 2, G_TYPE_STRING, G_TYPE_BOOLEAN );

    treev= gtk_tree_view_new_with_model( GTK_TREE_MODEL( store ) );
    gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( treev ), FALSE );

    if  ( visibleItems >= 0 )
	{
	scrolled_window= appListGtkMakeParent( &vadjustment, column );

	gtk_scrolled_window_add_with_viewport(
			    GTK_SCROLLED_WINDOW( scrolled_window ), treev );

	/*
	gtk_scrollable_set_vadjustment( GTK_SCROLLABLE( treev ), vadjustment );
	gtk_tree_view_set_vadjustment( GTK_TREE_VIEW( treev ), vadjustment );
	*/
	}
    else{
	gboolean	fill= TRUE;
	gboolean	expand= TRUE;

	gtk_box_pack_start( GTK_BOX( column ), treev, expand, fill, 0 );
	}

    gtk_widget_show( treev );

    gtk_tree_view_insert_column_with_attributes(
				GTK_TREE_VIEW( treev ),
				-1,
				"col",
				gtk_cell_renderer_text_new(),
				"text", 0, NULL );

    voidla= guiListGtkSetCallbacks( treev, scrolled_window, vadjustment,
				    listCallback, actionCallback, through );
    if  ( ! voidla )
	{ XDEB(voidla); return;	}

    selection= gtk_tree_view_get_selection( GTK_TREE_VIEW( treev ) );
    gtk_tree_selection_set_mode( selection, GTK_SELECTION_SINGLE );

    if  ( listCallback )
	{
	g_signal_connect( G_OBJECT( selection ), "changed",
		(GCallback)guiGtkHandlePotentialListCallback, voidla );
	}

    if  ( actionCallback )
	{
	g_signal_connect( G_OBJECT( treev ), "row-activated",
		(GCallback)guiGtkHandlePotentialListAction, voidla );
	}

    g_signal_connect( G_OBJECT( treev ), "cursor-changed",
		(GCallback)guiGtkListCursorChanged, voidla );

    if  ( visibleItems > 0 )
	{
	appListGtkSetVisibleSize( treev, scrolled_window, visibleItems );
	}

    *pList= treev;
    }

void guiAddValueToListWidget(	APP_WIDGET		tree,
				int			position,
				const char *		value )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreeModel *	model= gtk_tree_view_get_model( treev );
    ListAction *	la= guiGtkGetListAction( tree );

    const int		sensitive= TRUE;

    guiGtkAddValueToList( model, la, position, value, sensitive );
    }

void guiSelectPositionInListWidget(		APP_WIDGET	tree,
						int		position )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreePath *	path;
    GtkTreeSelection *	selection;
    ListAction *	la= guiGtkGetListAction( tree );

    selection= gtk_tree_view_get_selection( treev );

    path= gtk_tree_path_new_from_indices( position, -1 );
    gtk_tree_selection_select_path( selection, path );
    gtk_tree_path_free( path );

    if  ( la && la->laAdjustment && la->laItemCount > 0 )
	{ guiGtkSetListAdjustment( la, position );	}
    }

void guiRemoveSelectionFromListWidget(	APP_WIDGET	tree )
    {
    GtkTreeSelection *	selection;
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    ListAction *	la= guiGtkGetListAction( tree );

    if  ( la )
	{
	la->laSelectedPosition= -1;
	}

    selection= gtk_tree_view_get_selection( GTK_TREE_VIEW( tree ) );

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	gtk_tree_selection_unselect_iter( selection, &iter );
	}

    gtk_tree_selection_unselect_all( selection );
    }

void guiEmptyListWidget(	APP_WIDGET		tree )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkListStore *	store;
    ListAction *	la= guiGtkGetListAction( tree );

    if  ( la )
	{
	la->laItemCount= 0;
	la->laSelectedPosition= -1;
	}

    store= GTK_LIST_STORE( gtk_tree_view_get_model( treev ) );

    gtk_list_store_clear( store );
    }

int guiGetPositionFromListCallback(	APP_WIDGET		tree,
					GtkTreeSelection *	selection )
    {
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	GtkTreePath *	path= gtk_tree_model_get_path( model, &iter );
	int		idx;

	idx= gtk_tree_path_get_indices( path )[0];

	gtk_tree_path_free( path );
	     
	return idx;
	}

    return -1;
    }

void guiFreeStringFromListCallback(	char *		s )
    { g_free( s );	}

char * guiGetStringFromListCallback(	APP_WIDGET		tree,
					GtkTreeSelection *	selection )
    {
    GtkTreeModel *	model;
    GtkTreeIter		iter;

    if  ( gtk_tree_selection_get_selected( selection, &model, &iter ) )
        {
	gchar *		name;

	gtk_tree_model_get( model, &iter, 0, &name, -1 );
	return name;
	}

    return (char *)0;
    }

void guiReplaceValueInListWidget(	APP_WIDGET	tree,
					int		position,
					const char *	value )
    {
    guiGtkReplaceValueInList( gtk_tree_view_get_model( GTK_TREE_VIEW( tree ) ),
							    position, value );
    }

typedef struct FindValue
    {
    gboolean		fvFound;
    GtkTreeIter		fvIter;
    const char *	fvValue;
    } FindValue;

static gboolean guiGtkFindValue(		GtkTreeModel *	model,
						GtkTreePath *	path,
						GtkTreeIter *	iter,
						void *		vfv )
    {
    FindValue *	fv= (FindValue *)vfv;
    gchar *	value;

    gtk_tree_model_get( model, iter, 0, &value, -1);
    fv->fvFound= ! strcmp( fv->fvValue, value );
    if  ( fv->fvFound )
	{ fv->fvIter= *iter;	}
    g_free( value );

    return fv->fvFound;
    }

void guiSelectValueInListWidget(		APP_WIDGET	tree,
						const char *	value )
    {
    GtkTreeView *	treev= GTK_TREE_VIEW( tree );
    GtkTreeModel *	model= gtk_tree_view_get_model( treev );
    GtkTreeSelection *	selection= gtk_tree_view_get_selection( treev );
    ListAction *	la= guiGtkGetListAction( tree );

    FindValue		fv;

    fv.fvFound= FALSE;
    fv.fvValue= value;

    gtk_tree_model_foreach( model, guiGtkFindValue, (void *)&fv );

    if  ( fv.fvFound )
	{
	gtk_tree_selection_select_iter( selection, &(fv.fvIter) );

	if  ( la && la->laAdjustment && la->laItemCount > 0 )
	    {
	    GtkTreePath *	path;
	    int			position;

	    path= gtk_tree_model_get_path( model, &(fv.fvIter) );
	    position= gtk_tree_path_get_indices( path )[0];

	    guiGtkSetListAdjustment( la, position );

	    gtk_tree_path_free( path );
	    }
	}
    }

#   endif
