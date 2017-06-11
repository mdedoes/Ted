/************************************************************************/
/*									*/
/*  Adapt the attributes of a text box to the validity of its content	*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"guiBase.h"
#   include	"guiTextValidate.h"
#   include	<stddef.h>

#   include	<appDebugon.h>

#   if USE_GTK

int guiTextShowValidity(	APP_WIDGET	textw,
				int		given,
				int		valid )
    {
    if  ( valid )
	{
	GdkColor xc;
	gdk_color_parse( "#ddffdd", &xc );
	gtk_widget_modify_base( textw, GTK_STATE_NORMAL, &xc );
	gtk_widget_modify_base( textw, GTK_STATE_ACTIVE, &xc );
	}
    else{
	if  ( given )
	    {
	    GdkColor xc;
	    gdk_color_parse( "#ffdddd", &xc );
	    gtk_widget_modify_base( textw, GTK_STATE_NORMAL, &xc );
	    gtk_widget_modify_base( textw, GTK_STATE_ACTIVE, &xc );
	    }
	else{
	    gtk_widget_modify_base( textw, GTK_STATE_NORMAL, (GdkColor *)0 );
	    gtk_widget_modify_base( textw, GTK_STATE_ACTIVE, (GdkColor *)0 );
	    }
	}

    return 0;
    }

#   endif
