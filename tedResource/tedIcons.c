/************************************************************************/
/*									*/
/*  Icons and pictures for 'Ted'.					*/
/*									*/
/************************************************************************/

#   include	"tedResourceConfig.h"

#   include	<guiNamedImage.h>
#   include	"tedResource.h"
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

#   include	"tedmain_png.h"
#   include	"tedabout_png.h"
#   include	"replace_png.h"
#   include	"replace_find_png.h"
#   include	"navigate_next_png.h"
#   include	"navigate_before_png.h"

#   define	xsz( x ) x, sizeof( x )/sizeof(MemoryBuffer)

static NamedImage	TEDPictures[]=
    {
	{ "tedmain",		xsz( tedmain )	},
	{ "tedabout",		xsz( tedabout )	},
	{ "replace",		xsz( replace )	},
	{ "replace_find",	xsz( replace_find )	},
	{ "navigate_next",	xsz( navigate_next )	},
	{ "navigate_before",	xsz( navigate_before )	},
    };

void tedGetNamedImages(	NamedImage **		pPictures,
			int *			pPictureCount )
    {
    *pPictures= TEDPictures;
    *pPictureCount= sizeof(TEDPictures)/sizeof(NamedImage);
    }
