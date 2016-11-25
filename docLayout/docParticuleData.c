/************************************************************************/
/*									*/
/*  Additional information for the layout of a line of text. Some of	*/
/*  the fields are used for scratch in the process.			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docParticuleData.h"

#   include	<appDebugon.h>

void docInitParticuleData(		ParticuleData *	pd )
    {
    pd->pdX0= 0;
    pd->pdTwipsWide= 0;

    geoInitRectangle( &(pd->pdVisibleBBox) );

    pd->pdLeftBorderWidth= 0;
    pd->pdRightBorderWidth= 0;

    pd->pdTabKind= -1;
    pd->pdAfi= (const struct AfmFontInfo *)0;
    pd->pdFlags= 0x0;

    pd->pdWhiteUnits= 0;
    pd->pdCorrectBy= 0;
    }
