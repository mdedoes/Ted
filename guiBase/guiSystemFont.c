/************************************************************************/
/*									*/
/*  Allocate the font of a widget in the administration of screen fonts	*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"guiBase.h"

#   if FONTS_X11

#   include	"drawDrawingSurfaceImpl.h"
#   include	"drawFontImpl.h"
#   include	"drawScreenFontImpl.h"

#   include	<appDebugon.h>

int guiSystemFont(	DrawingSurface		ds,
			APP_FONT *		xfs )
    {
    NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    DrawScreenFont *		dsf= (DrawScreenFont *)0;
    int				screenFont;

    /* HACK */
    dsf= utilPagedListClaimItemAtEnd( &screenFont, &(npl->nplPagedList) );
    if  ( ! dsf )
	{ XDEB(dsf); return -1;	}

    dsf->dsfEncodedFonts.esfFonts= malloc( sizeof(EncodedScreenFont) );
    if  ( ! dsf->dsfEncodedFonts.esfFonts )
	{ XDEB(dsf->dsfEncodedFonts.esfFonts); return -1;	}
    drawInitEncodedScreenFont( dsf->dsfEncodedFonts.esfFonts );
    dsf->dsfEncodedFonts.esfFontCount= 1;

    dsf->dsfEncodedFonts.esfFonts[0].esfFontStruct= xfs;

#   if USE_MOTIF
    dsf->dsfDisplay= ds->dsDisplay;
    dsf->dsfDrawable= ds->dsDrawable;
    dsf->dsfGc= ds->dsGc;
    dsf->dsfEncodedFonts.esfFonts[0].esfFontBorrowed= 1;
#   endif

#   if DRAW_GDK
    dsf->dsfDrawable= ds->dsDrawable;
    dsf->dsfGc= ds->dsGc;
    dsf->dsfEncodedFonts.esfFonts[0].esfFontBorrowed= 0;
#   endif

#   if DRAW_XFT
    dsf->dsfXftDrawable= ds->dsXftDrawable;
#   endif

#   if FONTS_X11
    drawX11GetFontProperties( dsf->dsfEncodedFonts.esfFonts, dsf );

    utilMemoryBufferSetString( &(dsf->dsfFontNameX11),
				dsf->dsfEncodedFonts.esfFonts->esfFontName );
#   endif

    return screenFont;
    }

#   endif
