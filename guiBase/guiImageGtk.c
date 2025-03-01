#   include	"guiBaseConfig.h"

#   include	"guiBase.h"

#   if USE_GTK

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiImageImpl.h"
#   include	"drawUtilGdk.h"
#   include	"guiNamedImage.h"
#   include	<geoRectangle.h>
#   include	<bmFlipBits.h>

#   include	<bitmap.h>

#   include	<appDebugon.h>

static int guiGdkMakeImage(	APP_IMAGE **		pXim,
				const struct RasterImage * ri )
    {
    int			rval= 0;

    struct AppColors *	ac= drawGetColorsGdk();

    int			wide= ri->riDescription.bdPixelsWide;
    int			high= ri->riDescription.bdPixelsHigh;

    DocumentRectangle	drSrc;

    GdkImage *		xim= (GdkImage *)0;

    drSrc.drX0= 0;
    drSrc.drY0= 0;
    drSrc.drX1= wide- 1;
    drSrc.drY1= high- 1;

    if  ( drawUtilGdkMakeImage( &xim, wide, high, ac, ri, &drSrc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pXim= xim; xim= (GdkImage *)0; /* steal */

  ready:

    if  ( xim )
	{ gdk_image_destroy( xim );	}

    return rval;
    }

static int guiGdkMakePixmap(	APP_BITMAP_IMAGE *	pPixmap,
				APP_BITMAP_MASK *	pMask,
				const struct RasterImage * ri )
    {
    int			rval= 0;

    GdkVisual *		vis= gdk_visual_get_system();
    int			depth= gdk_visual_get_depth( vis );
    GdkGC *		gc= (GdkGC *)0;

    int			wide= ri->riDescription.bdPixelsWide;
    int			high= ri->riDescription.bdPixelsHigh;

    GdkImage *		xim= (GdkImage *)0;
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

    RasterImage		riMask;

    bmInitRasterImage( &riMask );

    if  ( guiGdkMakeImage( &xim, ri ) )
	{ LDEB(1); rval= -1; goto ready;	}

    pixmap= gdk_pixmap_new( NULL, wide, high, depth );
    gc= gdk_gc_new( pixmap );
    gdk_draw_image( pixmap, gc, xim, 0, 0, 0, 0, wide, high );

    if  ( ri->riDescription.bdHasAlpha )
	{
	int		ignoredInt= 0;

	if  ( bmGetAlphaMask( &riMask, ri, ignoredInt ) )
	    { LDEB(ri->riDescription.bdHasAlpha); rval= -1; goto ready;	}

	bmFlipBits( riMask.riBytes, riMask.riDescription.bdBufferLength );

	mask= gdk_bitmap_create_from_data( pixmap,
				(const gchar *)riMask.riBytes, wide, high );
	}

    *pPixmap= pixmap;
    *pMask= mask;

  ready:

    bmCleanRasterImage( &riMask );

    if  ( xim )
	{ gdk_image_destroy( xim );	}

    if  ( gc )
	{ gdk_gc_unref( gc );	}

    return rval;
    }

static int guiGdkGetPixmap(	APP_BITMAP_IMAGE *	pPixmap,
				APP_BITMAP_MASK *	pMask,
				struct NamedImage *	ni )
    {
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

    if  ( ni->niPixmap )
	{
	*pPixmap= ni->niPixmap;
	*pMask= ni->niMask;
	return 0;
	}

    if  ( ! ni->niRasterImage && guiMakeImageRasterImage( ni ) )
	{ LDEB(1); return -1;	}

    if  ( guiGdkMakePixmap( &pixmap, &mask, ni->niRasterImage ) )
	{ LDEB(1); return -1;	}

    ni->niPixmap= pixmap;
    ni->niMask= mask;

    *pPixmap= ni->niPixmap;
    *pMask= ni->niMask;
    return 0;
    }

# if GTK_MAJOR_VERSION < 3

void guiSetWindowIcon(		APP_WIDGET		topLevel,
				struct NamedImage * 	ni )
    {
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

    GdkWindow *		win= gtk_widget_get_window( topLevel );

    if  ( guiGdkGetPixmap( &pixmap, &mask, ni ) )
	{ LDEB(1); return;	}

    gdk_window_set_icon( win, win, pixmap, mask );

    return;
    }

#   endif

# if GTK_MAJOR_VERSION >= 3

void guiSetWindowIcon(		APP_WIDGET		topLevel,
				struct NamedImage *	ni )
    {
    APP_IMAGE *	image= (APP_IMAGE *)0;

    if  ( ! ni->niRasterImage && guiMakeImageRasterImage( ni ) )
	{ LDEB(1); return;	}

    if  ( guiGdkMakeImage( &image, ni->niRasterImage ) )
	{ LDEB(1); return;	}

    gtk_window_set_icon( GTK_WINDOW( topLevel ), image );

    return;
    }

#   endif

/************************************************************************/
/*									*/
/*  Insert an image in a column. The image is to be a row itself.	*/
/*									*/
/************************************************************************/

#   if GTK_MAJOR_VERSION < 3

int guiGtkMakeImageWidget(	APP_WIDGET *		pImageW,
				NamedImage *		ni )
    {
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

    GtkWidget *		imageW;

    if  ( guiGdkGetPixmap( &pixmap, &mask, ni ) )
	{ LDEB(1); return -1;	}

    imageW= gtk_pixmap_new( pixmap, mask );
    *pImageW= imageW; return 0;
    }

#   endif

#   if GTK_MAJOR_VERSION >= 3

int guiGtkMakeImageWidget(	APP_WIDGET *		pImageW,
				NamedImage *		ni )
    {
    APP_IMAGE *	pixbuf= (APP_IMAGE *)0;

    GtkWidget *		imageW;

    if  ( guiGdkMakeImage( &pixbuf, ni->niRasterImage ) )
	{ LDEB(1); return;	}

    imageW= gtk_image_new_from_pixbuf( pixbuf );

    *pImageW= imageW; return 0;
    }

#   endif

void guiMakeImageInColumn(	APP_WIDGET *		pImageW,
				APP_WIDGET		column,
				struct NamedImage *	ni )
    {
    GtkWidget *		imageW;

    if  ( guiGtkMakeImageWidget( &imageW, ni ) )
	{ LDEB(1); return;	}

    gtk_box_pack_start( GTK_BOX( column ), imageW, FALSE, TRUE, 0 );

    gtk_widget_show( imageW );

    *pImageW= imageW;
    }

void guiMakeImageInRow(		APP_WIDGET *		pImageW,
				APP_WIDGET		row,
				struct NamedImage *	ni,
				int			position,
				int			colspan )
    {
    GtkWidget *		imageW;

    if  ( guiGtkMakeImageWidget( &imageW, ni ) )
	{ LDEB(1); return;	}

    guiGtkAttachChildInGrid( row, imageW,
		    position, colspan,
		    0, 1, /* row, rowspan */
		    TRUE, TRUE, /* hexpand, vexpand */
		    ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( imageW );

    *pImageW= imageW;
    }

#   if  GTK_MAJOR_VERSION < 3

void guiCleanNamedImageImpl( NamedImage *	np )
    {
    if  ( np->niPixmap )
	{ gdk_pixmap_unref( np->niPixmap ); np->niPixmap= NULL; }
    if  ( np->niMask )
	{ gdk_pixmap_unref( np->niMask ); np->niMask= NULL; }
    }

#   else

void guiCleanNamedImageImpl( NamedImage *	np )
    {
    if  ( np->niImage )
	{ g_object_unref( np->niImage ); np->niImage= NULL; }
    }

#   endif

# endif

