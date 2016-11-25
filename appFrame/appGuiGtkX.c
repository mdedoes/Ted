#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appSystem.h>

#   include	"drawUtilGtk.h"
#   include	"drawImpl.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Actions in the GTK interface that cannot be done without using	*/
/*  plain old X11, or some knowledge of the relationship between GTK	*/
/*  and X11.								*/
/*									*/
/*  The author is aware of the fact that he should not have done this.	*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

#   include	<gdk/gdkx.h>
#   include	<X11/Xresource.h>

#   include	"appFrame.h"

void appCopyPixmapValue(	APP_SELECTION_EVENT *	gsd,
				APP_BITMAP_IMAGE	pixmapCopied )
    {
    Pixmap	pixmap= GDK_WINDOW_XWINDOW( pixmapCopied );

    gtk_selection_data_set( gsd, gsd->target, 8* sizeof(Pixmap),
				(unsigned char *)&pixmap, sizeof(Pixmap) );

    return;
    }

void appDrawGtkSetXFillRule(	GdkGC *		gc,
				int		rule )
    {
#   if GTK_MAJOR_VERSION < 2
    GdkGCPrivate *	private= (GdkGCPrivate*)gc;
    GC			xgc= private->xgc;
#   else
    GC			xgc= gdk_x11_gc_get_xgc( gc );
#   endif


    switch( rule )
	{
	case GDK_EVEN_ODD_RULE:	rule= EvenOddRule; break;
	case GDK_WINDING_RULE:	rule= WindingRule; break;
	default:
	    LDEB(rule); rule= EvenOddRule; break;
	}

    XSetFillRule( GDK_DISPLAY(), xgc, rule );
    }

/************************************************************************/
/*									*/
/*  Resolve resource values by going to X11 with the low level XrmXXX	*/
/*  routines.								*/
/*									*/
/*  Implementation follows the schema on p307 of the O'Reilly Xt book.	*/
/*  The numbers in the routine are those in the schema.			*/
/*  !   Command line options are not handeled.				*/
/*  !   $HOME/.Xdefaults-hostname is not handeled.			*/
/*									*/
/*	NYE, Adrian & Tim O'REILLY: "X Toolkit Intrinsics Programming	*/
/*	Manual", ISBN 1-56592-013-9, O'Reilly and Associates,		*/
/*	Sebastopol CA, 1995.						*/
/*									*/
/*									*/
/************************************************************************/

#   define	NAML	500

void appGuiGetResourceValuesGtkX(
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount )
    {
    static int			initialized= 0;
    static XrmDatabase		dbX= (XrmDatabase)0;

    char			name[NAML+1];
    int				appl= strlen( ea->eaApplicationName );
    int				hl;

    static const char		xdefs[]= ".Xdefaults";
    /*
    const int			xdefl= sizeof( xdefs )- 1;
    */

    static const char		xlibs[]= "/usr/lib/X11";
    const int			xlibl= sizeof( xlibs )- 1;

    static const char		adefs[]= "app-defaults";
    const int			adefl= sizeof( adefs )- 1;

    MemoryBuffer		home;

    utilInitMemoryBuffer( &home );

    if  ( ! initialized )
	{
	char *		rms;
	char *		lang;
	XrmDatabase	db;

	XrmInitialize();

	/*  8: Done by caller  */

	/*  7  */
	db= (XrmDatabase)0;
	lang= getenv( "LANG" );
	if  ( lang && xlibl+ 1+ strlen( lang )+ 1+ adefl+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s/%s",
				xlibs,
				lang,
				adefs,
				ea->eaApplicationName );

	    db= XrmGetFileDatabase( name );
	    }
	if  ( ! db && xlibl+ 1+ adefl+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s",
				xlibs,
				adefs,
				ea->eaApplicationName );
	    db= XrmGetFileDatabase( name );
	    }
	if  ( db )
	    { XrmMergeDatabases( db, &dbX );	}

	/*  6: PATH.. TODO  */
	db= (XrmDatabase)0;

	/*  5: PATH.. TODO  */
	db= (XrmDatabase)0;
	rms= getenv( "XAPPLRESDIR" );
	if  ( lang && rms && strlen( rms )+ 1+ strlen( lang )+ 1+ appl <= NAML )
	    {
	    sprintf( name, "%s/%s/%s", rms, lang, ea->eaApplicationName );
	    db= XrmGetFileDatabase( name );
	    }

	if  ( ! db )
	    {
	    appHomeDirectory( &home );
	    hl= home.mbSize;
	    if  ( hl > 0 )
		{
		strcpy( name, utilMemoryBufferGetString( &home ) );
		name[hl]= '/'; hl++;
		strcpy( name+ hl, ea->eaApplicationName );

		db= XrmGetFileDatabase( name );
		}
	    }

	if  ( db )
	    { XrmMergeDatabases( db, &dbX );	}

	/*  4  */
	db= (XrmDatabase)0;
	rms= XResourceManagerString( GDK_DISPLAY() );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }
	else{
	    appHomeDirectory( &home );
	    hl= home.mbSize;
	    if  ( hl > 0 )
		{
		name[hl]= '/'; hl++;
		strcpy( name+ hl, xdefs );

		db= XrmGetFileDatabase( name );
		if  ( db )
		    { XrmMergeDatabases( db, &dbX );	}
		}
	    }

	/*  3  */
	db= (XrmDatabase)0;
	rms= XScreenResourceString( DefaultScreenOfDisplay( GDK_DISPLAY() ) );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }

	/*  2: FILE TODO  */
	db= (XrmDatabase)0;
	rms= getenv( "XENVIRONMENT" );
	if  ( rms )
	    {
	    db= XrmGetStringDatabase( rms );
	    if  ( db )
		{ XrmMergeDatabases( db, &dbX );	}
	    }

	/*  1: ARGS TODO  */
	db= (XrmDatabase)0;

	initialized= 1;
	}

    if  ( dbX )
	{
	int		i;
	char *		values= (char *)pValues;

	for ( i= 0; i < acrCount; acr++, i++ )
	    {
	    int		l;
	    Bool	found;
	    XrmValue	xrmv;
	    char *	type_return= (char *)0;

	    l= strlen( acr->acrResourceName );
	    if  ( appl+ 1+ l > NAML )
		{ SDEB(acr->acrResourceName); continue;	}

	    sprintf( name, "%s.%s",
				ea->eaApplicationName, acr->acrResourceName );

	    found= XrmGetResource( dbX, name, name, &type_return, &xrmv );

	    if  ( found )
		{
		*((const char **)(values+acr->acrStructOffset))=
							(char *)xrmv.addr;
		}
	    }
	}

    utilCleanMemoryBuffer( &home );

    return;
    }

# ifdef USE_XFT

XftDraw * appGtkXftDrawCreate(	GdkDrawable *		drawable,
				AppXftColorList *	axcl )
    {
    GdkVisual *		gdk_vis= gdk_visual_get_system();
    GdkColormap *	gdk_cmap= gdk_colormap_get_system();

    Drawable		x_drawable;
    XftDraw *		xftDraw;

    if  ( ! drawable )
	{ XDEB(drawable); return (XftDraw *)0;	}

    axcl->axclDisplay= GDK_WINDOW_XDISPLAY( drawable );

#   if GTK_MAJOR_VERSION >= 2

    x_drawable= GDK_WINDOW_XID( drawable );
    axcl->axclVisual= gdk_x11_visual_get_xvisual( gdk_vis );
    axcl->axclColormap= gdk_x11_colormap_get_xcolormap( gdk_cmap );

#   else

    x_drawable= GDK_WINDOW_XWINDOW( drawable );
    axcl->axclVisual= GDK_VISUAL_XVISUAL( gdk_vis );
    axcl->axclColormap= GDK_COLORMAP_XCOLORMAP( gdk_cmap );

#   endif

    xftDraw= XftDrawCreate( axcl->axclDisplay, x_drawable,
				    axcl->axclVisual, axcl->axclColormap );

    if  ( ! xftDraw )
	{ XDEB(xftDraw);	}

    return xftDraw;
    }

# endif

#   endif
