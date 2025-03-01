#   include	"guiBaseConfig.h"

#   include	<stdio.h>
#   include	<appDebugon.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"

#   if USE_GTK

/************************************************************************/
/*									*/
/*  Make a row in a column: Rows are implemented as..			*/
/*									*/
/************************************************************************/

APP_WIDGET guiMakeRowInColumn(	APP_WIDGET	column,
				int		columnCount,
				int		heightResizable )
    {
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    GtkWidget *	row= guiGtkMakeGrid( 1, columnCount, TRUE );

    if  ( heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), row, expand, fill, 0 );

    gtk_widget_show( row );

    return row;
    }

void guiMakeHBoxInRow(		APP_WIDGET *	pHbox,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    int			spacing= 0;

#   if GTK_MAJOR_VERSION < 3
    GtkWidget *		hbox= gtk_hbox_new( FALSE, spacing );
#   else
    GtkWidget *		hbox= gtk_box_new( GTK_ORIENTATION_HORIZONTAL, spacing );
#   endif

    guiGtkAttachChildInGrid( row, hbox,
			position, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( hbox );

    *pHbox= hbox;
    return;
    }

void guiMakeColumnInRow(	APP_WIDGET *	pColumn,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
#   if GTK_MAJOR_VERSION < 3
    GtkWidget *		column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );
#   else
    GtkWidget *		column= gtk_box_new( GTK_ORIENTATION_VERTICAL, COLUMN_SPACING_GTK );
#   endif

    guiGtkAttachChildInGrid( row, column,
			position, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( column );

    *pColumn= column;
    }

APP_WIDGET guiGtkMakeGrid(	int			rows,
				int			columns,
				int			homogeneous )
    {
#   if GTK_MAJOR_VERSION < 3
    return gtk_table_new( rows, columns, homogeneous );
#   else
    APP_WIDGET	grid= gtk_grid_new();
    int		pos;

    for ( pos= 0; pos < rows; pos++ )
	{
	gtk_grid_insert_row( GTK_GRID( grid ), pos );
	}

    for ( pos= 0; pos < columns; pos++ )
	{
	gtk_grid_insert_column( GTK_GRID( grid ), pos );
	}

    gtk_grid_set_row_homogeneous( GTK_GRID( grid ), FALSE );
    gtk_grid_set_column_homogeneous( GTK_GRID( grid ), homogeneous );

    return grid;
#   endif
    }

void guiGtkAttachChildInGrid(	APP_WIDGET		grid,
				APP_WIDGET		child,
				int			left,
				int			colspan,
				int			top,
				int			rowspan,
				int			hexpand,
				int			vexpand,
				int			hmargin,
				int			vmargin )
    {
#   if GTK_MAJOR_VERSION < 3
    gtk_table_attach( GTK_TABLE( grid ), child,
			left, left+ colspan,
			top, top+ rowspan,
			GTK_FILL | (hexpand?(GTK_EXPAND|GTK_SHRINK):0),
			GTK_FILL | (vexpand?(GTK_EXPAND|GTK_SHRINK):0),
			hmargin, vmargin );
#   else
    gtk_grid_attach( GTK_GRID( grid ), child,
					left, top, colspan, rowspan );
    gtk_widget_set_hexpand( child, hexpand );
    gtk_widget_set_vexpand( child, vexpand );
    gtk_widget_set_margin_left( child, hmargin );
    gtk_widget_set_margin_right( child, hmargin );
    gtk_widget_set_margin_top( child, vmargin );
    gtk_widget_set_margin_bottom( child, vmargin );
#   endif

    return;
    }

#   endif
