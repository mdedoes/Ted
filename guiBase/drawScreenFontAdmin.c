/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"drawImpl.h"
#   include	"drawScreenFontAdmin.h"
#   include	"drawScreenFontImpl.h"
#   include	"drawTextImpl.h"
#   include	"drawTextImplX11.h"
#   include	"drawDrawingSurfaceImpl.h"
#   include	"drawScreenFontImpl.h"
#   include	<psTextExtents.h>
#   include	<psFontInfo.h>
#   include	<utilNumberedPropertiesAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void drawStartScreenFontList(	NumberedPropertiesList *	sfl )
    {
    utilInitNumberedPropertiesList( sfl );

    utilStartNumberedPropertyList( sfl,

		    DSFkey_COUNT,
		    (NumberedPropertiesGetProperty)drawGetScreenFontKey,
		    (NumberedPropertiesCopyProperties)0,
		    (NumberedPropertiesGetNumber)0,

		    sizeof(DrawScreenFont),
		    (InitPagedListItem)drawInitScreenFont,
		    (CleanPagedListItem)drawCleanScreenFont );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

DrawScreenFont * drawGetScreenFontByNumber(
					const NumberedPropertiesList *	sfl,
					int				n )
    {
    void *	vdsf= utilPagedListGetItemByNumber( &(sfl->nplPagedList), n );

    return (DrawScreenFont *)vdsf;
    }

/************************************************************************/
/*									*/
/*  Call a function for all ScreenFont in the list.		*/
/*									*/
/************************************************************************/

void drawForAllScreenFonts(	const NumberedPropertiesList *	sfl,
				ScreenFontFunction		f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(sfl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vdsf= utilPagedListGetItemByNumber( pl, n );

	(*f)( (DrawScreenFont *)vdsf, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the slot for a particular font/attribute combination.		*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*									*/
/************************************************************************/

int drawGetScreenFont(		DrawScreenFont **		pApf,
				int *				pFresh,
				NumberedPropertiesList *	npl,
				const IndexSet *		unicodesUsed,
				const AfmFontInfo *		afi,
				int				pixelSize )
    {
    int					screenFont;

    DrawScreenFont			dsfTemplate;
    DrawScreenFont *			dsfFound;

    drawInitScreenFont( &dsfTemplate );

    if  ( pixelSize < 2 )
	{
	/* LFLDEB(twipsSize,add->addMagnifiedPixelsPerTwip,sizePixels); */
	pixelSize= 2;
	}

    /*  1  */
    drawInitScreenFont( &dsfTemplate );
    dsfTemplate.apfPsFaceNumber= afi->afiFaceNumber;
    dsfTemplate.dsfSizePixels= pixelSize;

    screenFont= utilGetPropertyNumber( npl, 0, &dsfTemplate );
    if  ( screenFont >= 0 )
	{
	dsfFound= drawGetScreenFontByNumber( npl, screenFont );
	if  ( ! dsfFound )
	    { LXDEB(screenFont,dsfFound); return -1;	}

	*pApf= dsfFound; *pFresh= 0; return screenFont;
	}

    /*  KEY  */
    dsfTemplate.apfPsFaceNumber= afi->afiFaceNumber;
    dsfTemplate.dsfSizePixels= pixelSize;

    /*  DEP  */
    dsfTemplate.dsfPsFontInfo= afi;
    dsfTemplate.dsfUnicodesUsed= unicodesUsed;

    screenFont= utilGetPropertyNumber( npl, 1, &dsfTemplate );
    if  ( screenFont >= 0 )
	{
	const int		baselinePixels= 0;

	dsfFound= drawGetScreenFontByNumber( npl, screenFont );
	if  ( ! dsfFound )
	    { LXDEB(screenFont,dsfFound); return -1;	}

	/****************************************************************/
	/*  Calculate geometry shortcuts.				*/
	/****************************************************************/

	psStrikethroughGeometry( &(dsfFound->dsfStrikethroughPositionPixels),
				&(dsfFound->dsfStrikethroughThicknessPixels),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->dsfPsFontInfo );
	if  ( dsfFound->dsfStrikethroughThicknessPixels < 1 )
	    {
	    dsfFound->dsfStrikethroughThicknessPixels= 1;
	    dsfFound->dsfStrikethroughPositionPixels--;
	    }

	psUnderlineGeometry( &(dsfFound->dsfUnderlinePositionPixels),
				&(dsfFound->dsfUnderlineThicknessPixels),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->dsfPsFontInfo );
	if  ( dsfFound->dsfUnderlineThicknessPixels < 1 )
	    {
	    dsfFound->dsfUnderlineThicknessPixels= 1;
	    dsfFound->dsfUnderlinePositionPixels--;
	    }

	psGetSuperBaseline( &(dsfFound->apfSuperBaseline),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->dsfPsFontInfo );
	psGetSubBaseline( &(dsfFound->apfSubBaseline),
				baselinePixels, dsfFound->dsfSizePixels,
				dsfFound->dsfPsFontInfo );

	*pApf= dsfFound; *pFresh= 1; return screenFont;
	}

    LDEB(screenFont); return screenFont;
    }

int drawTextExtentsImpl(	const DrawingSurface		ds,
				DocumentRectangle *		drText,
				int				x,
				int				y,
				int				screenFont,
				const char *			s,
				int				len )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    DrawScreenFont *			dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{ LXDEB(screenFont,dsf); return -1;	}

#   if DRAW_XFT
    if  ( dsf->dsfXftFont )
	{
	int			wide;

	wide= drawTextExtentsXft( drText, x, y, dsf, dsf, s, len );

	if  ( wide >= 0 )
	    { return wide;	}
	}
#   endif

#   if  DRAW_GDK || DRAW_X11
    {
    TextProgress			tp;

    tp.tpX= x;
    tp.tpY= y;

    if  ( drawHandleTextSegmentsX11( &tp, dsf, s, len,
		    drawTextSegment8LengthX11, drawTextSegment16LengthX11,
		    &(dsf->dsfEncodedFonts) ) < 0 )
	{ LDEB(len); return -1;	}

    drText->drX0= x;
    drText->drX1= tp.tpX;

    drawVerticalTextExtentsX11( drText, y, &(dsf->dsfEncodedFonts) );
    return tp.tpX- x;
    }
#   endif

#   if DRAW_CAIRO
    {
    DocumentRectangle		dr20;
    const int			withKerning= 0;
    const int			vswap= 1;

    wide= psCalculateStringExtents( &dr20, s, len, 20* dsf->dsfSizePixels,
				    withKerning, vswap, dsf->dsfPsFontInfo );
    drText->drX0= x;
    drText->drX1= x+ ( wide+ 10 )/ 20;
    drText->drY0= y+ ( dr20.drY0+ 10 )/ 20;
    drText->drY1= y+ ( dr20.drY1+ 10 )/ 20;
    return ( wide+ 10 )/ 20;
    }
#   endif

    return drText->drX1- drText->drX0+ 1;
    }
