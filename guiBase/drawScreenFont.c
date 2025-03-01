/************************************************************************/
/*									*/
/*  Basic screen font management.					*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"drawImpl.h"
#   include	"drawScreenFontAdmin.h"
#   include	"drawScreenFontImpl.h"
#   include	<utilMemoryBuffer.h>
#   include	<psFontInfo.h>

#   include	<appDebugon.h>

int drawFontImplementationNameImpl(
			MemoryBuffer *			target,
			const struct NumberedPropertiesList *	npl,
			int				screenFont )
    {
#   if ! USE_HEADLESS

    const AfmFontInfo *			afi;
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{ LXDEB(screenFont,dsf); return -1;	}
    afi= dsf->dsfPsFontInfo;

#   if FONTS_XFT
    if  ( dsf->dsfXftFont )
	{
	return utilCopyMemoryBuffer( target, &(afi->afiFontFileName) );
	}
#   endif

#   if	FONTS_X11
    if  ( dsf->dsfEncodedFonts.esfFontCount > 0 )
	{
	return utilCopyMemoryBuffer( target, &(dsf->dsfFontNameX11) );
	}
#   endif

#   if FONTS_CAIRO
    return utilCopyMemoryBuffer( target, &(afi->afiFontFileName) );
#   endif

#   endif

    return -1;
    }

