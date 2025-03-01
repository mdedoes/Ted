/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif that use Xft.			*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<psFontName.h>
#   include	"drawImpl.h"
#   include	"drawImplXft.h"
#   include	"drawScreenFontImpl.h"
#   include	"drawColorsXft.h"
#   include	<psFontInfo.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

#   if DRAW_XFT

#   if ! FONTS_XFT
    Impossible combination of DRAW_XFT and ! FONTS_XFT
#   endif

#   include	"drawImplXft.h"

static FT_UInt * drawXftAllocateGlyphs(	int *			pCount,
					const AfmFontInfo *	afi,
					const char *		s,
					int			len )
    {
    FT_UInt *	rval= (FT_UInt *)0;
    FT_UInt *	fresh= malloc( len* sizeof(FT_UInt) );
    int		count= 0;
    int		done= 0;

    if  ( ! fresh )
	{ XDEB(fresh); goto ready;	}

    while( done < len )
	{
	unsigned short	sym;
	int		step;
	int		glyph;

	step= uniGetUtf8( &sym, s );
	if  ( step < 1 )
	    { LDEB(step); goto ready;	}

	glyph= utilIndexMappingGetU( &(afi->afiCodeToGlyphMapping), sym );
	if  ( glyph < 0 )
	    { LDEB(glyph); glyph= 0;	}

	fresh[count++]= glyph;
	done += step; s += step;
	}

    rval= fresh; *pCount= count;
    fresh= (FT_UInt *)0; /* steal */

  ready:

    if  ( fresh )
	{ free( fresh );	}

    return rval;
    }

int drawStringXft(		const DrawScreenFont *	dsf,
				AppXftColorList *	axcl,
				int			x,
				int			y,
				const char *		s,
				int			len )
    {
    int			rval= 0;
    AppXftColor *	axc= &(axcl->axclCurrentColor);
    const AfmFontInfo *	afi= dsf->dsfPsFontInfo;
    FT_UInt *		glyphs= (FT_UInt *)0;

    if  ( ! dsf->dsfXftDrawable || ! dsf->dsfXftFont )
	{ rval= -1; goto ready;	}

    if  ( ! axc->axcXftColorAllocated )
	{
	appXftAllocateColor( axc, axcl );

	if  ( ! axc->axcXftColorAllocated )
	    { rval= -1; goto ready;	}
	}

    if  ( afi->afiFontSpecificEncoding )
	{
	int	count;

	glyphs= drawXftAllocateGlyphs( &count, afi, s, len );
	if  ( ! glyphs )
	    { LXDEB(len,glyphs); rval= -1; goto ready;	}

	XftDrawGlyphs( dsf->dsfXftDrawable, &(axc->axcXftColor),
				    dsf->dsfXftFont, x, y, glyphs, count );
	}
    else{
	XftDrawStringUtf8( dsf->dsfXftDrawable, &(axc->axcXftColor),
				dsf->dsfXftFont, x, y, (FcChar8 *)s, len );
	}

  ready:

    if  ( glyphs )
	{ free( glyphs );	}

    return rval;
    }

int drawTextExtentsXft(		DocumentRectangle *	drText,
				int			x,
				int			y,
				const DrawScreenFont *	hDsf,
				const DrawScreenFont *	vDsf,
				const char *		s,
				int			len )
    {
    int			width= 0;
    XGlyphInfo		glyphInfo;
    const AfmFontInfo *	afi= hDsf->dsfPsFontInfo;
    FT_UInt *		glyphs= (FT_UInt *)0;

    if  ( ! hDsf->dsfXftFont || ! vDsf->dsfXftFont )
	{ width= -1; goto ready;	}

    if  ( afi->afiFontSpecificEncoding )
	{
	int	count;

	glyphs= drawXftAllocateGlyphs( &count, afi, s, len );
	if  ( ! glyphs )
	    { LXDEB(len,glyphs); width= -1; goto ready;	}

	XftGlyphExtents( hDsf->dsfDisplay,
			hDsf->dsfXftFont, glyphs, count, &glyphInfo );
	}
    else{
	XftTextExtentsUtf8( hDsf->dsfDisplay,
			hDsf->dsfXftFont, (FcChar8 *)s, len, &glyphInfo );
	}

    drText->drX0= x;
    drText->drX1= drText->drX0+ glyphInfo.width- 1;
    drText->drY0= y- vDsf->dsfXftFont->ascent;
    drText->drY1= y+ vDsf->dsfXftFont->descent;
    width= glyphInfo.xOff;

#   if 0
    appDebug( "\"%*.*s\" -> width=%d height=%d x=%d y=%d xOff=%d yOff=%d\n",
				len, len, s,
				glyphInfo.width, glyphInfo.height,
				glyphInfo.x, glyphInfo.y,
				glyphInfo.xOff, glyphInfo.yOff );
#   endif

  ready:

    if  ( glyphs )
	{ free( glyphs );	}

    return width;
    }

int drawFillRectangleXft(	XftDraw *			draw,
				AppXftColorList *		axcl,
				const DocumentRectangle *	dr )
    {
    AppXftColor *	axc= &(axcl->axclCurrentColor);

    if  ( ! draw )
	{ return -1;	}

    if  ( ! axc->axcXftColorAllocated )
	{
	appXftAllocateColor( axc, axcl );

	if  ( ! axc->axcXftColorAllocated )
	    { return -1;	}
	}

    XftDrawRect( draw, &(axc->axcXftColor),
			dr->drX0, dr->drY0,
			dr->drX1- dr->drX0+ 1, dr->drY1- dr->drY0+ 1 );
    return 0;
    }

/************************************************************************/

static int appFontAddOneCharToSet(	int		val,
					void *		voidfcs )
    {
    FcCharSetAddChar((FcCharSet *)voidfcs, val );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Open an XFT font corresponding to a (PostScript) font face in the	*/
/*  font catalog.							*/
/*									*/
/************************************************************************/

XftFont * drawOpenXftFont(	DrawScreenFont *	dsf )
    {
    const AfmFontInfo *	afi= dsf->dsfPsFontInfo;
    XftFont *		rval= (XftFont *)0;

    int			screen;

    const char *	fc_charset= (const char *)0;
    FcCharSet *		fcCharset= (FcCharSet *)0;
    int			typeCharset= FcTypeVoid;

    int			width= FONTwidthNORMAL;
    char		scratch[200+1];

    if  ( psRemoveWidthFromName( scratch, sizeof(scratch)- 1, &width,
						    afi->afiFamilyName ) < 0 )
	{ SDEB(afi->afiFamilyName); goto ready;	}

    if  ( ! afi->afiFontSpecificEncoding && dsf->dsfUnicodesUsed )
	{
	fcCharset= FcCharSetCreate();
	fc_charset= FC_CHARSET;
	typeCharset= FcTypeCharSet;

	/* IndexSet -> FcCharSet */
	utilIndexSetForAll( dsf->dsfUnicodesUsed,
				appFontAddOneCharToSet, (void *)fcCharset );
 	}

    if  ( ! dsf->dsfDisplay )
	{ XDEB(dsf->dsfDisplay); return (XftFont *)0;	}
    screen= DefaultScreen( dsf->dsfDisplay );

    /*  2  */
    rval= XftFontOpen( dsf->dsfDisplay, screen,
	    FC_FAMILY, FcTypeString,
			scratch,
	    FC_WIDTH, FcTypeInteger,
			afi->afiWidthInt,
	    FC_PIXEL_SIZE, FcTypeDouble,
			(double)dsf->dsfSizePixels,
	    FC_SLANT, FcTypeInteger,
			(FONT_IS_SLANTED(afi)?FC_SLANT_ITALIC:FC_SLANT_ROMAN),
	    FC_WEIGHT, FcTypeInteger,
			(FONT_IS_BOLD(afi)?FC_WEIGHT_BOLD:FC_WEIGHT_REGULAR),
	    fc_charset, typeCharset,
			fcCharset,
	    NULL );

  ready:

    if  ( fcCharset )
	{ FcCharSetDestroy( fcCharset );	}

    return rval;
    }

#   if USE_MOTIF

/************************************************************************/
/*									*/
/*  Create an XftDraw for the X11 window of an XmDrawingArea		*/
/*									*/
/************************************************************************/

XftDraw * appMotifXftDrawCreate(	Display *		display,
					int			screen,
					Drawable		drawable,
					AppXftColorList *	xftColorList )
    {
    Colormap		cmap= DefaultColormap( display, screen );
    Visual *		vis= DefaultVisual( display, screen );
    XftDraw *		xftDraw;

    xftColorList->axclDisplay= display;
    xftColorList->axclVisual= vis;
    xftColorList->axclColormap= cmap;

    xftDraw= XftDrawCreate( xftColorList->axclDisplay, drawable,
						xftColorList->axclVisual,
						xftColorList->axclColormap );

    return xftDraw;
    }

#   endif

#   endif
