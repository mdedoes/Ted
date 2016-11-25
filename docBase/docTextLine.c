/************************************************************************/
/*									*/
/*  Manage lines in a paragraph.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docTextLine.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set a text line to sensible defaults.				*/
/*									*/
/************************************************************************/

void docInitTextLine(		TextLine *	tl )
    {
    tl->tlStroff= 0;
    tl->tlFirstParticule= 0;
    tl->tlStrlen= 0;
    tl->tlParticuleCount= 0;
    tl->tlWordCount= 0;

    docInitLayoutPosition( &(tl->tlTopPosition) );

    geoInitRectangle( &(tl->tlRectangle) );
    tl->tlLineStride= 0;

    tl->tlFlags= 0;

    tl->tlFrameWidthTwips= 0;

    tl->tlPixelShift= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Get the baseline position of the line. As tl->tlRectangle.drY0 is	*/
/*  negative, the baseline is below the top.				*/
/*									*/
/************************************************************************/

void docTextLineGetBaseline(	LayoutPosition *	lp,
				const TextLine *	tl )
    {
    *lp= tl->tlTopPosition;
    lp->lpPageYTwips += tl->tlLineStride- tl->tlRectangle.drY1;
    lp->lpAtTopOfColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Get the position immediately below the line. Leading is NOT		*/
/*  included.								*/
/*									*/
/************************************************************************/

void docTextLineGetBelowLine(	LayoutPosition *	lp,
				const TextLine *	tl )
    {
    *lp= tl->tlTopPosition;
    lp->lpPageYTwips += tl->tlRectangle.drY1- tl->tlRectangle.drY0;
    lp->lpAtTopOfColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Get the position where the top of the next line would be placed on	*/
/*  the same page. (Actually, in the same newspaper style column of the	*/
/*  section. Here leading IS included.					*/
/*									*/
/************************************************************************/

void docTextLineGetNextLineTop(	LayoutPosition *	lp,
				const TextLine *	tl )
    {
    *lp= tl->tlTopPosition;
    lp->lpPageYTwips += tl->tlLineStride;
    lp->lpAtTopOfColumn= 0;
    }

void docTextLineGetShiftedTop(		LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl )
    {
    docShiftPosition( lp, bo, &(tl->tlTopPosition) );
    }

void docTextLineGetShiftedBaseline(	LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl )
    {
    LayoutPosition	llp;

    docTextLineGetBaseline( &llp, tl );
    docShiftPosition( lp, bo, &llp );
    }

void docTextLineGetShiftedBelowLine(	LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl )
    {
    LayoutPosition	llp;

    docTextLineGetBelowLine( &llp, tl );
    docShiftPosition( lp, bo, &llp );
    }

void docTextLineGetShiftedNextLineTop(	LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl )
    {
    LayoutPosition	llp;

    docTextLineGetNextLineTop( &llp, tl );
    docShiftPosition( lp, bo, &llp );
    }

