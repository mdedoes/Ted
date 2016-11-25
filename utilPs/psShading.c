/************************************************************************/
/*									*/
/*  Names of the procedures that fill a rectangle with a shading.	*/
/*									*/
/*  As the procedures operate in the device coordinate system, rather	*/
/*  than in the page coordinate system, they must be rotated if the	*/
/*  page is rotated for landscape printing.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	"psShading.h"

const char * const PsShadingNames[]=
    {
    (const char *)0,	/* solid */
    "f-horiz",
    "f-vert",
    "f-fdiag",
    "f-bdiag",
    "f-cross",
    "f-dcross",
    "f-dkhoriz",
    "f-dkvert",
    "f-dkfdiag",
    "f-dkbdiag",
    "f-dkcross",
    "f-dkdcross",
    };

const char * const PsShadingNames_90[]=
    {
    (const char *)0,	/* solid */
    "f-vert",
    "f-horiz",
    "f-bdiag",
    "f-fdiag",
    "f-cross",
    "f-dcross",
    "f-dkvert",
    "f-dkhoriz",
    "f-dkbdiag",
    "f-dkfdiag",
    "f-dkcross",
    "f-dkdcross",
    };

