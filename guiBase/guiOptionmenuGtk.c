#   include	"guiBaseConfig.h"


#   if USE_GTK

#   include	<stdio.h>

#   include	"guiOptionmenu.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiListGtk.h"

#   include	<appDebugon.h>

static int GTKcomboSetting= 0;

/************************************************************************/
/*									*/
/*  The callback that is called when the selection changes.		*/
/*									*/
/*  1)  It is also called initially when no row is active yet.		*/
/*									*/
/************************************************************************/

static void guiComboBoxGtkCallback(	GtkComboBox *	combo,
					void *		voidaom )
    {
    AppOptionmenu *	aom= (AppOptionmenu *)voidaom;
    int			idx;

    if  ( GTKcomboSetting )
	{ return;	}

    idx= gtk_combo_box_get_active( combo );
    /*  1  */
    if  ( idx < 0 )
	{ /*LDEB(idx);*/ return;	}

    if  ( aom->aomCallback )
	{ (*aom->aomCallback)( idx, aom->aomTarget ); }
    }

void guiOptionMenuEnablePosition( AppOptionmenu *	aom,
				int			position,
				int			enabled )
    {
    const int		secol= 1;
    GtkTreeModel *	model;

    model= gtk_combo_box_get_model( GTK_COMBO_BOX( aom->aomInplace ) );

    guiListGtkSetRowFlag( model, position, secol, enabled != 0 );
    }

/************************************************************************/
/*									*/
/*  Build an optionmenu.						*/
/*									*/
/*  1)  Reserve a boolean to keep track of whether the row is		*/
/*	sensitive. This code is shared with the list.			*/
/*									*/
/************************************************************************/

void guiMakeOptionmenuGtk(	AppOptionmenu *		aom,
				OptionmenuCallback	callBack,
				void *			target )
    {
    APP_WIDGET		pulldown= (APP_WIDGET)0;
    APP_WIDGET		inplace= (APP_WIDGET)0;

    GtkListStore *	store= (GtkListStore *)0;
    void *		voidla= (void *)0;
    GtkCellRenderer *	cellR= gtk_cell_renderer_text_new();

    store= gtk_list_store_new( 2, G_TYPE_STRING, G_TYPE_BOOLEAN );
    inplace= gtk_combo_box_new_with_model( GTK_TREE_MODEL( store ) );

    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( inplace ), cellR, FALSE );
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( inplace ), cellR,
							    "text", 0,
							    "sensitive", 1,
							    NULL );
    gtk_widget_set_name( inplace, "tedRowOptMenu");

    g_signal_connect( G_OBJECT( inplace ), "changed",
				(GCallback)guiComboBoxGtkCallback, aom );

    voidla= guiListGtkSetAdmin( inplace, (APP_LIST_CALLBACK_T)0, aom );
    if  ( ! voidla )
	{ XDEB(voidla); return;	}

    aom->aomPulldown= pulldown;
    aom->aomInplace= inplace;
    aom->aomCallback= callBack;
    aom->aomTarget= target;

    return;
    }

/************************************************************************/
/*									*/
/*  Used to get around Motif's ridiculous resize behavior. Probably	*/
/*  not needed for GTK.							*/
/*									*/
/************************************************************************/

void guiOptionmenuRefreshWidth(	AppOptionmenu *		aom )
    { return;	}

void guiMakeOptionmenuInRow(		AppOptionmenu *		aom,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					OptionmenuCallback	callBack,
					void *			target )
    {
    guiMakeOptionmenuGtk( aom, callBack, target );

    guiGtkAttachChildInGrid( row, aom->aomInplace,
			column, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( aom->aomInplace );

    return;
    }

void guiMakeOptionmenuInColumn(		AppOptionmenu *		aom,
					APP_WIDGET		column,
					OptionmenuCallback	callBack,
					void *			target )
    {
    guiMakeOptionmenuGtk( aom, callBack, target );

    gtk_box_pack_start( GTK_BOX( column ), aom->aomInplace, FALSE, TRUE, 0 );

    gtk_widget_show( aom->aomInplace );

    return;
    }

/************************************************************************/
/*									*/
/*  Add an option to a pulldown.					*/
/*									*/
/************************************************************************/

void guiAddItemToOptionmenu(	AppOptionmenu *		aom,
				const char *		label )
    {
    const int		position= -1;
    APP_WIDGET		option;

    GtkComboBox *	combo= GTK_COMBO_BOX( aom->aomInplace );
    GtkTreeModel *	model= gtk_combo_box_get_model( combo );
    ListAction *	la= guiGtkGetListAction( aom->aomInplace );

    const int		sensitive= TRUE;

    guiGtkAddValueToList( model, la, position, label, sensitive );

    option= gtk_menu_item_new_with_label( label );
    gtk_widget_set_name( GTK_WIDGET(option), "tedOptMenuItem");

    return;
    }

void guiDeleteItemFromOptionmenu(	AppOptionmenu *		aom,
					int			pos )
    {
    GtkComboBox *	combo= GTK_COMBO_BOX( aom->aomInplace );
    GtkTreeModel *	model= gtk_combo_box_get_model( combo );
    ListAction *	la= guiGtkGetListAction( aom->aomInplace );

    guiGtkDeleteValueFromList( model, la, pos );
    }

void guiReplaceValueInOptionmenu(	AppOptionmenu *	aom,
					int		position,
					const char *	value )
    {
    guiGtkReplaceValueInList( gtk_combo_box_get_model(
			GTK_COMBO_BOX( aom->aomInplace ) ), position, value );
    }

/************************************************************************/
/*									*/
/*  Select a particular option in an option menu.			*/
/*									*/
/************************************************************************/

void guiSetOptionmenu(	AppOptionmenu *	aom,
			int		num )
    {
    GTKcomboSetting++;
    gtk_combo_box_set_active( GTK_COMBO_BOX( aom->aomInplace ), num );
    GTKcomboSetting--;

    return;
    }

void guiEmptyOptionmenu(	AppOptionmenu *		aom )
    {
    GtkComboBox *	combo= GTK_COMBO_BOX( aom->aomInplace );
    GtkListStore *	store;
    ListAction *	la= guiGtkGetListAction( aom->aomInplace );

    if  ( la )
	{
	la->laItemCount= 0;
	la->laSelectedPosition= -1;
	}

    store= GTK_LIST_STORE( gtk_combo_box_get_model( combo ) );

    gtk_list_store_clear( store );
    }

void guiEnableOptionmenu(	AppOptionmenu *		aom,
				int			sensitive )
    {
    gtk_widget_set_sensitive( aom->aomInplace, sensitive != 0 );
    }

void guiInitOptionmenu(		AppOptionmenu *		aom )
    {
    aom->aomPulldown= (APP_WIDGET)0;
    aom->aomInplace= (APP_WIDGET)0;

    aom->aomCallback= (OptionmenuCallback)0;
    aom->aomTarget= (void *)0;
    }

#   endif
