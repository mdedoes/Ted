#   include	"guiBaseConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"

#   include	<appDebugon.h>

#   if USE_GTK

void guiMakeSliderInRow(	APP_WIDGET *		pSlider,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through )
    {
    GtkAdjustment *	ga;
    GtkWidget *		scale;

    ga= GTK_ADJUSTMENT( gtk_adjustment_new(
				    0.0, 0.0, 100.0, 1.0, 10.0, 10.0 ) );
#   if GTK_MAJOR_VERSION < 3
    scale= gtk_hscale_new( ga );
#   else
    scale= gtk_scale_new( GTK_ORIENTATION_HORIZONTAL, ga );
#   endif
    gtk_scale_set_draw_value( GTK_SCALE( scale ), FALSE );

    if  ( callback )
	{
	g_signal_connect( G_OBJECT( ga ),
			"value_changed", (GCallback)callback, through );
	}

    guiGtkAttachChildInGrid( row, scale,
			column, colspan,
			0, 1, /* row, rowspan */
			TRUE, TRUE, /* hexpand, vexpand */
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( scale );

    *pSlider= scale;
    }

void guiSetSlider(		APP_WIDGET	slider,
				int		minVal,
				int		maxVal,
				int		curVal )
    {
    GtkAdjustment *	ga;

    ga= gtk_range_get_adjustment( GTK_RANGE( slider ) );

    gtk_adjustment_set_lower( ga, minVal );
    gtk_adjustment_set_upper( ga, maxVal );
    gtk_adjustment_set_value( ga, curVal );

    gtk_adjustment_changed( ga );
    }

void guiSetSliderValue(		APP_WIDGET	slider,
					int		curVal )
    {
    GtkAdjustment *	ga;

    ga= gtk_range_get_adjustment( GTK_RANGE( slider ) );

    gtk_adjustment_set_value( ga, curVal );

    gtk_adjustment_changed( ga );
    }

int guiGetValueFromSliderCallback(	int *		pValue,
					APP_WIDGET	w,
					void *		voidscs )
    {
    GtkAdjustment *	ga= (GtkAdjustment *)w;

    *pValue= gtk_adjustment_get_value( ga );
    return 0;
    }

#   endif

