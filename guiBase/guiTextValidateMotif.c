/************************************************************************/
/*									*/
/*  Adapt the attributes of a text box to the validity of its content	*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"guiBase.h"
#   include	"drawColors.h"
#   include	"guiTextValidate.h"
#   include	"drawUtilMotif.h"
#   include	<stddef.h>

#   include	<appDebugon.h>

#   if USE_MOTIF

int guiTextShowValidity(	APP_WIDGET	textw,
				int		given,
				int		valid )
    {
    Display *		display= XtDisplay( textw );

    if  ( valid )
	{
	XColor xc;
	drawColorRgb( &xc, drawGetColorsMotif( display ), 0xdd,0xff,0xdd );

	XtVaSetValues( textw,
				XmNbackground,	xc.pixel,
				NULL );
	}
    else{
	if  ( given )
	    {
	    XColor xc;
	    drawColorRgb( &xc, drawGetColorsMotif( display ), 0xff,0xdd,0xdd );

	    XtVaSetValues( textw,
				XmNbackground,	xc.pixel,
				NULL );
	    }
	else{
	    XtVaSetValues( textw,
		XmNbackground,	WhitePixel( display, DefaultScreen(display) ),
		NULL );
	    }
	}

    return 0;
    }

#   endif
