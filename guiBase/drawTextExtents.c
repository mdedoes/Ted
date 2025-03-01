/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "guiBaseConfig.h"

#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawScreenFontAdmin.h"
#   include <appDebugon.h>

int drawGetUnderlineGeometry(		int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pThick= dsf->dsfUnderlineThicknessPixels;
    *pY= baselinePixels+ dsf->dsfUnderlinePositionPixels;

    return 0;
    }

int drawGetStrikethroughGeometry(	int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pThick= dsf->dsfStrikethroughThicknessPixels;
    *pY= baselinePixels+ dsf->dsfStrikethroughPositionPixels;

    return 0;
    }

int drawGetSuperBaseline(	int *			pSuperBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pSuperBaseline= baselinePixels+ dsf->apfSuperBaseline;

    return 0;
    }

int drawGetSubBaseline(		int *			pSubBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pSubBaseline= baselinePixels+ dsf->apfSubBaseline;
    return 0;
    }

int drawGetTextExtents(	struct DocumentRectangle *	drText,
			const DrawingSurface		ds,
			int				x0,
			int				y0,
			int				screenFont,
			const char *			s,
			int				len )
    {
    return drawTextExtentsImpl( ds, drText, x0, y0, screenFont, s, len );
    }

