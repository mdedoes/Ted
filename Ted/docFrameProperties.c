/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docFrameProperties.h"

/************************************************************************/
/*									*/
/*  Manage text frame properties.					*/
/*									*/
/************************************************************************/

void docInitTextFrameProperties(	TextFrameProperties *	tfp )
    {
    tfp->tfpFrameWidthTwips= 0;
    tfp->tfpFrameHeightTwips= 0;

    tfp->tfpFrameXTwips= 0;
    tfp->tfpFrameYTwips= 0;

    tfp->tfpDistanceFromTextTwips= 0;
    tfp->tfpHorDistanceFromTextTwips= 0;
    tfp->tfpVerDistanceFromTextTwips= 0;

    tfp->tfpHorizontalFrameReference= HFRphCOL;
    tfp->tfpHorizontalFrameAlignment= HFAposXL;

    tfp->tfpVerticalFrameReference= VFRpvMRG;
    tfp->tfpVerticalFrameAlignment= VFAposYIL;
    tfp->tfpFrameLockedToParagraph= 0;

    tfp->tfpFrameWrapStyle= FWSwrapWRAP;

    return;
    }

