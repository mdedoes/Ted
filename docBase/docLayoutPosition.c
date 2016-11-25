/************************************************************************/
/*									*/
/*  A (vertical) position in the document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docLayoutPosition.h"
#   include	<geoRectangle.h>

/************************************************************************/
/*									*/
/*  Initialise a layout position.					*/
/*									*/
/************************************************************************/

void docInitLayoutPosition(	LayoutPosition *	lp )
    {
    lp->lpPageYTwips= 0;
    lp->lpPage= 0;
    lp->lpColumn= 0;
    lp->lpAtTopOfColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Initialise a block origin.						*/
/*									*/
/************************************************************************/

void docInitBlockOrigin(	BlockOrigin *	bo )
    {
    bo->boXShift= 0;
    bo->boYShift= 0;

    bo->boOverridePage= 0;
    bo->boOverrideColumn= 0;
    bo->boOverrideFrame= 0;
    }

/************************************************************************/
/*									*/
/*  Push a position to one at least below another one. An example of	*/
/*  its use is pushing down the bottom of a row to the bottom of its	*/
/*  tallest cell.							*/
/*									*/
/************************************************************************/

void docLayoutPushBottomDown(		LayoutPosition *	lpParentBottom,
					const LayoutPosition *	lpChildBottom )
    {
    if  ( lpParentBottom->lpPage > lpChildBottom->lpPage )
	{ return;	}
    if  ( lpParentBottom->lpPage < lpChildBottom->lpPage )
	{ *lpParentBottom= *lpChildBottom; return; }

    if  ( lpParentBottom->lpColumn > lpChildBottom->lpColumn )
	{ return;	}
    if  ( lpParentBottom->lpColumn < lpChildBottom->lpColumn )
	{ *lpParentBottom= *lpChildBottom; return; }

    if  ( lpParentBottom->lpPageYTwips < lpChildBottom->lpPageYTwips )
	{ *lpParentBottom= *lpChildBottom; return; }

    return;
    }

int docCompareLayoutPositions(	const LayoutPosition *		lp1,
				const LayoutPosition *		lp2 )
    {
    if  ( lp1->lpPage > lp2->lpPage )
	{ return  1;	}
    if  ( lp1->lpPage < lp2->lpPage )
	{ return -1;	}

    if  ( lp1->lpColumn > lp2->lpColumn )
	{ return  1;	}
    if  ( lp1->lpColumn < lp2->lpColumn )
	{ return -1;	}

    if  ( lp1->lpPageYTwips > lp2->lpPageYTwips )
	{ return  1;	}
    if  ( lp1->lpPageYTwips < lp2->lpPageYTwips )
	{ return -1;	}

    return 0;
    }

int docCompareLayoutPositionFrames(
				const LayoutPosition *		lp1,
				const LayoutPosition *		lp2 )
    {
    if  ( lp1->lpPage > lp2->lpPage )
	{ return  1;	}
    if  ( lp1->lpPage < lp2->lpPage )
	{ return -1;	}

    if  ( lp1->lpColumn > lp2->lpColumn )
	{ return  1;	}
    if  ( lp1->lpColumn < lp2->lpColumn )
	{ return -1;	}

    return 0;
    }

int docCompareLayoutPositionToFrame(
				const LayoutPosition *		lp,
				int				page,
				int				column )
    {
    if  ( lp->lpPage > page )
	{ return  1;	}
    if  ( lp->lpPage < page )
	{ return -1;	}

    if  ( lp->lpColumn > column )
	{ return  1;	}
    if  ( lp->lpColumn < column )
	{ return -1;	}

    return 0;
    }

void docShiftPosition(	LayoutPosition *	to,
			const BlockOrigin *	bo,
			const LayoutPosition *	from )
    {
    *to= *from;

    if  ( bo && bo->boOverrideFrame )
	{
	to->lpPage= bo->boOverridePage;
	to->lpColumn= bo->boOverrideColumn;
	}

    to->lpPageYTwips += (bo?bo->boYShift:0);
    }

void docShiftRectangle(	DocumentRectangle *		to,
			const BlockOrigin *		bo,
			const DocumentRectangle *	from )
    {
    *to= *from;

    to->drX0 += (bo?bo->boXShift:0);
    to->drX1 += (bo?bo->boXShift:0);

    to->drY0 += (bo?bo->boYShift:0);
    to->drY1 += (bo?bo->boYShift:0);
    }

