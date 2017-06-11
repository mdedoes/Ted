#   include	"guiBaseConfig.h"

#   include	<string.h>

#   include	"guiBase.h"

#   if USE_MOTIF

#   include	"guiWidgets.h"
#   include	"guiImageImpl.h"
#   include	"drawUtilMotif.h"
#   include	"guiNamedImage.h"
#   include	<geoRectangle.h>

#   include	<bitmap.h>

#   include	<Xm/Label.h>

#   include	<appDebugon.h>

static int guiMotifMakeImagePixmap(	Drawable *		pPixmap,
					APP_WIDGET		parentW,
					const RasterImage *	ri )
    {
    int			rval= 0;

    Display *		display= XtDisplay( parentW );
    int			screen= DefaultScreen( display );
    int			depth= DefaultDepth( display, screen );
    GC			gc= (GC)0;
    struct AppColors *	apc= drawGetColorsMotif( display );

    int			pixelsWide= ri->riDescription.bdPixelsWide;
    int			pixelsHigh= ri->riDescription.bdPixelsHigh;
    DocumentRectangle	drSrc;

    Drawable		parentDrawable= XtWindow( parentW );
    Drawable		pixmap;
    XImage *		xim= (XImage *)0;

    drSrc.drX0= 0;
    drSrc.drY0= 0;
    drSrc.drX1= pixelsWide- 1;
    drSrc.drY1= pixelsHigh- 1;

    if  ( drawUtilMotifMakeImage( display, screen,
					    &xim, pixelsWide, pixelsHigh,
					    apc, ri, &drSrc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! parentDrawable )
	{ XDEB(parentDrawable); rval= -1; goto ready; }

    pixmap= XCreatePixmap( display, parentDrawable,
						pixelsWide, pixelsHigh, depth );

    gc= XCreateGC( display, pixmap, 0, NULL );
    if  ( ! gc )
	{ XDEB(gc); rval= -1; goto ready;	}

    XPutImage( display, pixmap, gc, xim, 0, 0, 0, 0,
						pixelsWide, pixelsHigh );

    *pPixmap= pixmap;

  ready:

    if  ( xim )
	{ XDestroyImage( xim );	}

    if  ( gc )
	{ XFreeGC( display, gc );	}

    return rval;
    }

int guiMotifGetImagePixmap(	Drawable *		pPixmap,
				APP_WIDGET		parentW,
				NamedImage *		ni )
    {
    Drawable	pixmap= (Drawable)0;

    if  ( ni->niPixmap )
	{
	*pPixmap= ni->niPixmap;
	return 0;
	}

    if  ( ! ni->niRasterImage && guiMakeImageRasterImage( ni ) )
	{ LDEB(1); return -1;	}

    if  ( guiMotifMakeImagePixmap( &pixmap, parentW, ni->niRasterImage ) )
	{ LDEB(1); return -1;	}

    ni->niDisplay= XtDisplay( parentW );
    ni->niPixmap= pixmap;

    *pPixmap= pixmap;
    return 0;
    }

void guiMakeImageInColumn(	APP_WIDGET *		pImageW,
				APP_WIDGET		column,
				struct NamedImage *	ni )
    {
    Arg		al[20];
    int		ac= 0;

    Drawable	pixmap= (Drawable)0;
    Widget	imageW= (Widget)0;

    if  ( guiMotifGetImagePixmap( &pixmap, column, ni ) )
	{ LDEB(1); goto ready;	}

    ac= 0;
    XtSetArg( al[ac], XmNlabelType,		XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNlabelPixmap,		pixmap ); ac++;

    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    imageW= XmCreateLabel( column, WIDGET_NAME, al, ac );

    XtManageChild( imageW );

    guiMotifTurnOfSashTraversal( column );

    *pImageW= imageW;

  ready:

    return;
    }

void guiMakeImageInRow(		APP_WIDGET *		pImageW,
				APP_WIDGET		row,
				struct NamedImage *	ni,
				int			position,
				int			colspan )
    {
    Arg		al[20];
    int		ac= 0;

    Drawable	pixmap= (Drawable)0;
    Widget	imageW= (Widget)0;

    if  ( guiMotifGetImagePixmap( &pixmap, row, ni ) )
	{ LDEB(1); goto ready;	}

    ac= 0;
    XtSetArg( al[ac], XmNlabelType,		XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNlabelPixmap,		pixmap ); ac++;

    ac= guiMotifSetInRowConstraints( al, ac, position, colspan );

    imageW= XmCreateLabel( row, WIDGET_NAME, al, ac );

    XtManageChild( imageW );

    *pImageW= imageW;

  ready:

    return;
    }

void guiSetWindowIcon(		APP_WIDGET		topLevel,
				struct NamedImage * 	ni )
    {
    Drawable	pixmap= (Drawable)0;

    if  ( guiMotifGetImagePixmap( &pixmap, topLevel, ni ) )
	{ LDEB(1); goto ready;	}

    XtVaSetValues( topLevel,
		XmNiconPixmap,	pixmap,
		NULL );

  ready:

    return;
    }

void guiCleanNamedImageImpl( NamedImage *	np )
    {
    if  ( np->niPixmap )
	{ XFreePixmap( np->niDisplay, np->niPixmap ); np->niPixmap= None; }
    if  ( np->niMask )
	{ XFreePixmap( np->niDisplay, np->niMask ); np->niMask= None; }
    }

# endif
