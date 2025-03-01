/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<docLayout.h>
#   include	<layoutContext.h>
#   include	<geoGrid.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBreakKind.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	"docSelectionGeometry.h"
#   include	<docBuf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

int docLayoutYPixels(	const LayoutContext *	lc,
			const LayoutPosition *	lp )
    {
    DocumentRectangle	drPagePixels;

    docGetPageRectPixels( &drPagePixels, lc, lp->lpPage );

    return drPagePixels.drY0+ lc->lcPixelsPerTwip* lp->lpPageYTwips;
    }

int docLayoutXPixels(	const LayoutContext *	lc,
			int			xTwips )
    {
    return lc->lcPixelsPerTwip* xTwips;
    }

/************************************************************************/
/*									*/
/*  Get the pixel rectangle for a rectangle that lives on one page.	*/
/*									*/
/************************************************************************/

void docGetPixelRect(		DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				const DocumentRectangle *	drTwips,
				int				page )
    {
    LayoutPosition		lp0;
    LayoutPosition		lp1;

    lp0.lpPage= lp1.lpPage= page;
    lp0.lpPageYTwips= drTwips->drY0;
    lp1.lpPageYTwips= drTwips->drY1;

    drPixels->drX0= docLayoutXPixels( lc, drTwips->drX0 );
    drPixels->drX1= docLayoutXPixels( lc, drTwips->drX1 );
    drPixels->drY0= docLayoutYPixels( lc, &lp0 );
    drPixels->drY1= docLayoutYPixels( lc, &lp1 );

    return;
    }


void docGetPixelRectForPos(	DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBottom )
    {
    drPixels->drX0= docLayoutXPixels( lc, x0Twips );
    drPixels->drX1= docLayoutXPixels( lc, x1Twips );
    drPixels->drY0= docLayoutYPixels( lc, lpTop );
    drPixels->drY1= docLayoutYPixels( lc, lpBottom );

    return;
    }

/************************************************************************/
/*									*/
/*  Get the pixel rectangle that corrresponds to a particular page.	*/
/*									*/
/*  1)  Find the first section that appears on the page.		*/
/*	If none can be found, the page belongs to document end notes	*/
/*	that are formatted according to the geometry of the last	*/
/*	section.							*/
/*  2)  Skip back to the last section that has its own page geometry:	*/
/*	That is either the first section of the document, or a section	*/
/*	that starts on a fresh page.					*/
/*									*/
/************************************************************************/

void docGetPixelRectangleForPages(
				DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				page0,
				int				page1 )
    {
    int			sectNr;
    const struct BufferItem *	bodyNode= lc->lcDocument->bdBody.dtRoot;
    int			first= 1;

    sectNr= docGetFirstSectionOnPage( lc->lcDocument, page1 );
    if  ( sectNr < 0 )
	{ sectNr= bodyNode->biChildCount- 1;	}

    if  ( sectNr < 0 ) /* never happens, make clang happy */
	{ LLDEB(bodyNode->biChildCount,sectNr); geoInitRectangle( drPixels );	}

    while( sectNr >= 0 )
	{
	const struct BufferItem *	sectNode= bodyNode->biChildren[sectNr];

	if  ( sectNr == 0					||
	      ( sectNode->biSectBreakKind != DOCibkNONE	&&
	        sectNode->biSectBreakKind != DOCibkCOL	)	)
	    {
	    DocumentRectangle	drOutside;
	    DocumentRectangle	drInside;
	    int			step;
	    int			topPage= sectNode->biTopPosition.lpPage;

	    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );

	    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

	    if  ( first )
		{ *drPixels= drOutside; first= 0;	}

	    if  ( page1 > topPage )
		{
		drPixels->drY1 += ( page1- topPage )* step;
		page1= topPage;
		}
	    if  ( page0 > topPage )
		{
		drPixels->drY0 += ( page0- topPage )* step;
		page0= topPage;
		}
	    else{
		if  ( drPixels->drX0 > drOutside.drX0 )
		    { drPixels->drX0=  drOutside.drX0;	}
		if  ( drPixels->drX1 < drOutside.drX1 )
		    { drPixels->drX1=  drOutside.drX1;	}
		}
	    }

	sectNr--;
	}

    return;
    }

void docGetPageRectPixels(	DocumentRectangle *		drPixels,
				const LayoutContext *		lc,
				int				page )
    {
    docGetPixelRectangleForPages( drPixels, lc, page, page );
    }

void docPixelRectangleForPositions(
				DocumentRectangle *		drPixels,
				const PositionGeometry *	pgB,
				const PositionGeometry *	pgE,
				const LayoutContext *		lc )
    {
    DocumentRectangle		drPixelsB;
    DocumentRectangle		drPixelsE;

    docGetPixelRectForPos( &drPixelsB, lc,
			    pgB->pgXTwips, pgB->pgXTwips,
			    &(pgB->pgTopPosition),
			    &(pgB->pgNextLinePosition) );

    /* Do not trust XTwips yet */
    drPixelsB.drX0= drPixelsB.drX1= pgB->pgXPixels;


    docGetPixelRectForPos( &drPixelsE, lc,
			    pgE->pgXTwips, pgE->pgXTwips,
			    &(pgE->pgTopPosition),
			    &(pgE->pgNextLinePosition) );

    /* Do not trust XTwips yet */
    drPixelsE.drX0= drPixelsE.drX1= pgE->pgXPixels;

    geoUnionRectangle( drPixels, &drPixelsB, &drPixelsE );

    return;
    }

/************************************************************************/
/*									*/
/*  Derive private rectangles from document properties.			*/
/*									*/
/************************************************************************/

void docPageRectsPixels(	DocumentRectangle *		drOut,
				DocumentRectangle *		drIn,
				double				xfac,
				const struct BufferItem *	bodySectNode,
				const struct BufferDocument *	bd )
    {
    const DocumentGeometry *	dg= &(bd->bdProperties->dpGeometry);
    DocumentRectangle		drPage;

    if  ( bodySectNode )
	{
	if  ( bodySectNode->biLevel != DOClevSECT )
	    { SDEB(docLevelStr(bodySectNode->biLevel));	}
	if  ( bodySectNode->biTreeType != DOCinBODY )
	    { SDEB(docTreeTypeStr(bodySectNode->biTreeType));	}

	dg= &(bodySectNode->biSectDocumentGeometry);
	}

    geoDocumentGeometryGetPageRect( &drPage, dg );

    drPage.drX0= COORDtoGRID( xfac, drPage.drX0 );
    drPage.drY0= COORDtoGRID( xfac, drPage.drY0 );
    drPage.drX1= COORDtoGRID( xfac, drPage.drX1 );
    drPage.drY1= COORDtoGRID( xfac, drPage.drY1 );

    if  ( drOut )
	{ *drOut= drPage;	}

    if  ( drIn )
	{
	DocumentRectangle		drBody;

	geoDocumentGeometryGetBodyRect( &drBody, dg );

	drIn->drX0= COORDtoGRID( xfac, drBody.drX0 );
	drIn->drY0= COORDtoGRID( xfac, drBody.drY0 );
	drIn->drX1= COORDtoGRID( xfac, drBody.drX1 );
	drIn->drY1= COORDtoGRID( xfac, drBody.drY1 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the page for a particular Y cordinate.				*/
/*									*/
/************************************************************************/

int docGetPageForYPixels(	const LayoutContext *		lc,
				int				yPixels )
    {
    int			sectNr;
    const struct BufferItem *	bodyNode= lc->lcDocument->bdBody.dtRoot;
    int			candidate= 0;

    DocumentRectangle	drOutside;
    DocumentRectangle	drInside;
    int			step;

    int			pagesBefore= 0;
    int			heightBefore= 0;

    const struct BufferItem *	sectNode= bodyNode->biChildren[0];

    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );
    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

    candidate= pagesBefore+ ( yPixels- heightBefore )/ step;

    for ( sectNr= 1; sectNr < bodyNode->biChildCount; sectNr++ )
	{
	sectNode= bodyNode->biChildren[sectNr];

	if  ( sectNode->biSectBreakKind != DOCibkNONE	&&
	      sectNode->biSectBreakKind != DOCibkCOL	)
	    {
	    int			topPage= sectNode->biTopPosition.lpPage;

	    heightBefore += ( topPage- pagesBefore )* step;

	    if  ( heightBefore >= yPixels )
		{ return candidate;	}

	    docPageRectsPixels( &drOutside, &drInside, lc->lcPixelsPerTwip,
						    sectNode, lc->lcDocument );
	    step= drOutside.drY1- drOutside.drY0+ 1+ lc->lcPageGapPixels;

	    pagesBefore= topPage;
	    candidate= pagesBefore+ ( yPixels- heightBefore )/ step;
	    }
	}

    return candidate;
    }

