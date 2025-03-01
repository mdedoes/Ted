#   include	"docBaseConfig.h"

#   include	"docBlockOrnaments.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize BlockOrnaments						*/
/*									*/
/************************************************************************/

void docInitBlockOrnaments(	BlockOrnaments *	bo )
    {
    utilPropMaskClear( &(bo->boPropMask) );

    bo->boTopBorderNumber= -1;
    bo->boLeftBorderNumber= -1;
    bo->boRightBorderNumber= -1;
    bo->boBottomBorderNumber= -1;

    bo->boTopBorder= (const struct BorderProperties *)0;
    bo->boLeftBorder= (const struct BorderProperties *)0;
    bo->boRightBorder= (const struct BorderProperties *)0;
    bo->boBottomBorder= (const struct BorderProperties *)0;

    bo->boShading= (const struct ItemShading *)0;
    }

