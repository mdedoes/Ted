/************************************************************************/
/*									*/
/*  Basic screen font management.					*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   if USE_GTK
#   define	USE_GTK_DEPRECATED
#   define	USE_GDK_DEPRECATED
#   include	"guiBase.h"
#   endif /* GTK */

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"drawImpl.h"
#   include	"drawScreenFontImpl.h"

#   include	<drawDrawingSurface.h>
#   include	<textAttribute.h>
#   include	<psDocumentFontStyle.h>
#   include	"drawDrawingSurfacePrivate.h"
#   include	"drawMatchFont.h"
#   include	<psFontInfo.h>

#   include	<appDebugon.h>

void drawCleanScreenFont(	DrawScreenFont *	dsf )
    {
#   if FONTS_XFT
    if  ( dsf->dsfXftFont )
	{ XftFontClose( dsf->dsfDisplay, dsf->dsfXftFont );	}
#   endif

#   if FONTS_X11
    utilCleanMemoryBuffer( &(dsf->dsfFontNameX11) );
    drawCleanEncodedScreenFontList( dsf, &(dsf->dsfEncodedFonts) );
#   endif

#   if FONTS_CAIRO
    if  ( dsf->dsfFT_Face )
	{ FT_Done_Face( dsf->dsfFT_Face );	}

    /* Resources are allocated for each drawing action */
#   endif

    return;
    }

void drawInitScreenFont(	DrawScreenFont *	dsf )
    {
    /*  KEY  */
    dsf->apfPsFaceNumber= -1;
    dsf->dsfSizePixels= 0;

    /*  DEP  */
    dsf->dsfPsFontInfo= (AfmFontInfo *)0;
    dsf->dsfUnderlinePositionPixels= 0;
    dsf->dsfUnderlineThicknessPixels= 0;
    dsf->dsfStrikethroughPositionPixels= 0;
    dsf->dsfStrikethroughThicknessPixels= 0;
    dsf->apfSuperBaseline= 0;
    dsf->apfSubBaseline= 0;
    dsf->apfXHeightPixels= 0;

#   if FONTS_X11
    drawInitEncodedScreenFontList( &(dsf->dsfEncodedFonts) );
    utilInitMemoryBuffer( &(dsf->dsfFontNameX11) );
#   endif

    dsf->dsfUnicodesUsed= (const IndexSet *)0;

#   if DRAW_X11 || FONTS_XFT
    dsf->dsfDisplay= (Display *)0;
#   endif

#   if DRAW_X11 || DRAW_GDK
    dsf->dsfDrawable= (APP_DRAWABLE)0;
    dsf->dsfGc= (APP_GC)0;
#   endif

#   if DRAW_XFT
    dsf->dsfXftDrawable= (XftDraw *)0;
#   endif

#   if FONTS_XFT
    dsf->dsfXftFont= (XftFont *)0;
#   endif

#   if FONTS_CAIRO
    dsf->dsfFT_Face= (FT_Face)0;
    /* Resources are allocated for each drawing action */
#   endif

    return;
    }

int drawGetScreenFontKey(		const DrawScreenFont *	dsf,
					int			prop )
    {
    switch( prop )
	{
	case DSFkeyFACE_NUMBER:
	    return dsf->apfPsFaceNumber;
	case DSFkeySIZE_PIXELS:
	    return dsf->dsfSizePixels;
	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Obtain the system resources for a screen font.			*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*									*/
/************************************************************************/

int drawFontOpenScreenFont(	DrawScreenFont *	dsf,
				int			avoidFontconfig )
    {
    int				openFullFont= 1;

    if  ( dsf->apfPsFaceNumber < 0 || ! dsf->dsfPsFontInfo )
	{ LXDEB(dsf->apfPsFaceNumber,dsf->dsfPsFontInfo); return -1;	}

    /********************************************************************/
    /*  Open freetype fonts (if configured)				*/
    /********************************************************************/
#   if FONTS_XFT
    if  ( ! avoidFontconfig )
	{
	if  ( openFullFont )
	    {
	    dsf->dsfXftFont= drawOpenXftFont( dsf );
	    if  ( dsf->dsfXftFont )
		{ openFullFont= 0; }
	    }
	}
#   endif

#   if FONTS_X11
    /********************************************************************/
    /*  Open X11 fonts (if freetype is not configured, or it failed)	*/
    /********************************************************************/
    if  ( openFullFont )
	{
	if  ( ! appFontOpenX11Fonts( dsf ) )
	    { openFullFont= 0; }
	}
#   endif

#   if FONTS_CAIRO
    /* Resources are allocated for each drawing action */
    dsf->dsfFT_Face= drawFcGetFace( dsf->dsfPsFontInfo );
    if  ( ! dsf->dsfFT_Face )
	{ SXDEB(dsf->dsfPsFontInfo->afiFontName,dsf->dsfFT_Face); }
    else{ openFullFont= 0;	}
#   endif

    if  ( openFullFont )
	{
	SLDEB(dsf->dsfPsFontInfo->afiFontName,openFullFont);
	return -1;
	}

    return 0;
    }

#   if	USE_GTK

/************************************************************************/
/*									*/
/*  Open the default font belonging to a widget. (Or the system)	*/
/*									*/
/************************************************************************/

int drawOpenDefaultFont( DrawingSurface				ds,
			const struct PostScriptFontList *	psfl,
			APP_WIDGET				w,
			int					fontSizeHintPixels )
    {
    int			rval= 0;
    int			pixelSize= fontSizeHintPixels;
    AfmFontInfo *	afi;

    TextAttribute	ta;
    MemoryBuffer	fontFamilyName;

    textInitTextAttribute( &ta );
    utilInitMemoryBuffer( &fontFamilyName );

    if  ( utilMemoryBufferSetString( &fontFamilyName, "Sans" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( w )
	{
	GtkStyle *	gs= gtk_widget_get_style( w );

	if  ( gs )
	    {
	    if  ( gs->font_desc )
		{
		const char *	fam;
		int		style;
		int		weight;

		fam= pango_font_description_get_family( gs->font_desc );
		if  ( fam )
		    {
		    if  ( utilMemoryBufferSetString( &fontFamilyName, fam ) )
			{ LDEB(1); rval= -1; goto ready;	}
		    }

		if  ( fontSizeHintPixels <= 0				&&
		      pango_font_description_get_size_is_absolute(
							gs->font_desc )	)
		    {
		    int		sz;

		    sz= pango_font_description_get_size( gs->font_desc );
		    if  ( sz > 0 )
			{ pixelSize= sz;	}
		    }

		style= pango_font_description_get_style( gs->font_desc );
		if  ( style == PANGO_STYLE_OBLIQUE	||
		      style == PANGO_STYLE_ITALIC	)
		    { ta.taFontIsSlanted= 1;	}

		weight= pango_font_description_get_weight( gs->font_desc );
		if  ( weight >=
			( PANGO_WEIGHT_NORMAL+ PANGO_WEIGHT_SEMIBOLD ) / 2 )
		    { ta.taFontIsBold= 1;	}
		}
#	    if DRAW_GDK
	    else{
		GdkFont *	gf;

#		if GTK_MAJOR_VERSION < 2
		gf= gs->font;
#		else
		gf= gtk_style_get_font( gs );
#		endif

		gdk_font_ref( gf );
		rval= guiSystemFont( ds, gf );
		goto ready;
		}
#	    endif
	    }
	}

    if  ( pixelSize < 6 )
	{ pixelSize= 10;	}

    afi= drawGetFontInfo( &fontFamilyName, DFstyleFNIL,
				    (const struct IndexSet *)0, &ta, psfl );
    if  ( ! afi )
	{
	SXDEB(utilMemoryBufferGetString(&fontFamilyName),afi);
	rval= -1; goto ready;
	}

    rval= drawOpenScreenFont( ds, afi, pixelSize, (IndexSet *)0 );

  ready:

    utilCleanMemoryBuffer( &fontFamilyName );

    return rval;
    }

#   endif

#   if FONTS_X11

#   endif
