/************************************************************************/
/*									*/
/*  Color mangement with GDK: Try to get around the limitations with a	*/
/*  limited number of colors.						*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   if	USE_GTK

#   define	USE_GDK_DEPRECATED
#   include	"guiBase.h"

#   if		DRAW_GDK

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<bmcolor.h>

#   include	"drawUtilGdk.h"
#   include	"drawColors.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Allocate approximate colors.					*/
/*									*/
/************************************************************************/

/***/

static int appColorAllocSystemColor(	AllocatorColor *	acRes,
					const AllocatorColor *	acScratch,
					AppColors *		acSys )
    {
    APP_COLOR_RGB		xc;

    xc.red= acScratch->acRed;
    xc.green= acScratch->acGreen;
    xc.blue= acScratch->acBlue;

    if  ( ! gdk_color_alloc( acSys->acColormap, &xc ) )
	{ LDEB(1); return -1;	}

    acRes->acAllocated= AC_ALLOCATED;
    acRes->acRed= xc.red;
    acRes->acGreen= xc.green;
    acRes->acBlue= xc.blue;
    acRes->acColorNumber= xc.pixel;

    return 0;
    }

static int drawColorRgb111(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->ca222Colors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    { LDEB(acScratch.acColorNumber); return -1;	}
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int drawColorRgb222(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->ca222Colors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    { LDEB(acScratch.acColorNumber); return -1;	}
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int drawColorRgb555(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->caColors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    {
	    if  ( bmFindNearestColorRgb( ac, ca, r, g, b ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int drawColorRgb332(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->caColors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    {
	    if  ( bmFindNearestColorRgb( ac, ca, r, g, b ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

int drawColorRgb(	APP_COLOR_RGB *		xc,
			AppColors *		acSys,
			unsigned int		r,
			unsigned int		g,
			unsigned int		b )
    {
    AllocatorColor	ac;
    ColorAllocator *	ca= &(acSys->acAllocator);

    if  ( acSys->acVisualClass == GDK_VISUAL_TRUE_COLOR		||
	  acSys->acVisualClass == GDK_VISUAL_DIRECT_COLOR	)
	{
	bmColorRgbDirect( &ac, ca, r, g, b );
	}
    else{
	if  ( ! ca->caSystemAllocator )
	    { XDEB(ca->caSystemAllocator); return -1;	}

	if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
	    { LLLDEB(r,g,b); return -1;	}
	}

    xc->red= ac.acRed;
    xc->green= ac.acGreen;
    xc->blue= ac.acBlue;
    xc->pixel= ac.acColorNumber;

    return 0;
    }

int drawColorNamed(	APP_COLOR_RGB *		xc,
			AppColors *		acSys,
			const char *		name )
    {
    APP_COLOR_RGB	exact;

    if  ( ! gdk_color_parse( name, &exact ) )
	{ SDEB(name); return -1;	}

    return drawColorRgb( xc, acSys,
			exact.red/257, exact.green/257, exact.blue/257 );
    }

static int drawAllocateColors(	AppColors *		acSys )
    {
    GdkVisual *		vis= gdk_visual_get_system();
    GdkColormap *	cmap= gdk_colormap_get_system();

    int			count;

    ColorAllocator *	ca= &(acSys->acAllocator);

    acSys->acColormap= cmap;
    acSys->acVisualClass= gdk_visual_get_visual_type( vis );

    ca->caSystemPrivate= acSys;
    ca->caDepth= gdk_visual_get_depth( vis );

    switch( acSys->acVisualClass )
	{
	case GDK_VISUAL_STATIC_GRAY:
	case GDK_VISUAL_GRAYSCALE:
	    ca->caAllocationType= CA_INDEX_ALLOC;
	    switch( ca->caDepth )
		{
		case 1: case 2: case 4: case 8:
		    break;
		default:
		    LLDEB(acSys->acVisualClass,ca->caDepth);
		}
	    break;

	case GDK_VISUAL_STATIC_COLOR:
	    ca->caAllocationType= CA_INDEX_ALLOC;
	    break;

	case GDK_VISUAL_PSEUDO_COLOR:
	    ca->caAllocationType= CA_INDEX_ALLOC;
	    break;

	case GDK_VISUAL_TRUE_COLOR:
	case GDK_VISUAL_DIRECT_COLOR:
	    ca->caAllocationType= CA_CALCULATED;

	    {
	    guint32	red_mask;
	    guint32	green_mask;
	    guint32	blue_mask;

#	    if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 22

	    red_mask= vis->red_mask;
	    green_mask= vis->green_mask;
	    blue_mask= vis->blue_mask;

#	    else

	    gdk_visual_get_red_pixel_details( vis, &red_mask, NULL, NULL );
	    gdk_visual_get_green_pixel_details( vis, &green_mask, NULL, NULL );
	    gdk_visual_get_blue_pixel_details( vis, &blue_mask, NULL, NULL );

#	    endif

	    bmSetCalculatedShifts( ca, red_mask, green_mask, blue_mask );
	    }

	    return 0;
	}

    switch( ca->caDepth )
	{
	case 1:
	    count= 2;
	    ca->caSystemAllocator= drawColorRgb111;
	    bmSetCalculatedShifts( ca, 0x0004, 0x0002, 0x0001 );
	    break;

	case 2:
	    count= 4;
	    ca->caSystemAllocator= drawColorRgb111;
	    bmSetCalculatedShifts( ca, 0x0004, 0x0002, 0x0001 );
	    break;

	case 4:
	    count= 16;
	    ca->caSystemAllocator= drawColorRgb222;
	    bmSetCalculatedShifts( ca, 0x0030, 0x000c, 0x0003 );
	    break;

	case 8:
	    count= 256;
	    ca->caSystemAllocator= drawColorRgb332;
	    bmSetCalculatedShifts( ca, 0x00e0, 0x001c, 0x0003 );
	    break;

	case 16:
	    count= 256* 256;
	    ca->caSystemAllocator= drawColorRgb555;
	    bmSetCalculatedShifts( ca, 0x7c00, 0x03e0, 0x001f );
	    break;

	case 24:
	case 32:
	    return 0;

	default:
	    LDEB(ca->caDepth); return -1;
	}

    if  ( bmAllocateAllocatorColors( ca, count ) )
	{ LDEB(count); return -1;	}

    return 0;
    }

void drawInitColors(	AppColors *	acSys )
    {
    ColorAllocator *	ca= &(acSys->acAllocator);

    bmInitColorAllocator( ca );

    acSys->acColormap= (GdkColormap *)0;
    }

int drawColorFindRgb(	APP_COLOR_RGB *	xc,
			AppColors *	acSys,
			unsigned int	r,
			unsigned int	g,
			unsigned int	b )
    {
    ColorAllocator *	ca= &(acSys->acAllocator);
    AllocatorColor	acRet;

    if  ( bmFindNearestColorRgb( &acRet, ca, r, g, b ) )
	{ return -1;	}

    xc->red= acRet.acRed;
    xc->green= acRet.acGreen;
    xc->blue= acRet.acBlue;
    xc->pixel= acRet.acColorNumber;

    return 0;
    }

static AppColors * APP_TheColors= (AppColors *)0;

AppColors * drawGetColorsGdk( void )
    {
    if  ( ! APP_TheColors )
	{
	APP_TheColors= malloc( sizeof(AppColors) );
	if  ( ! APP_TheColors )
	    { XDEB(APP_TheColors); return (AppColors *)0;	}

	drawInitColors( APP_TheColors );
	drawAllocateColors( APP_TheColors );
	}

    return APP_TheColors;
    }

#   endif	/*  DRAW_GDK	*/
#   endif	/*  USE_GTK	*/
