/************************************************************************/
/*									*/
/*  Visit the intersection of a text line with the selection.		*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stdlib.h>

#   include	<docTreeNode.h>
#   include	<docSelect.h>
#   include	<docScanner.h>
#   include	<docParaScanner.h>
#   include	<docTextRun.h>
#   include	<docTextParticule.h>
#   include	<geoRectangle.h>
#   include	<docScreenLayout.h>
#   include	<layoutContext.h>
#   include	"docLineRectangles.h"
#   include	<docParaProperties.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docIntersectHiglight(	int *			pFrom,
					int *			pUpto,
					int			trFrom,
					int			trUpto,
					int			dsFrom,
					int			dsUpto )
    {
    if  ( trFrom >= dsUpto )
	{ return +1;	}
    if  ( trUpto <= dsFrom )
	{ return -1;	}

    if  ( trFrom < dsFrom )
	{ *pFrom= dsFrom;	}
    else{ *pFrom= trFrom;	}

    if  ( trUpto > dsUpto )
	{ *pUpto= dsUpto;	}
    else{ *pUpto= trUpto;	}

    return 0;
    }

static int docVisitHighlight(
		LineRectangles *		lr,
		int				x1Pixels )
    {
    int			res;
    DocumentRectangle	dr= *lr->lrDrThisLineFrame;

    dr.drX0= lr->lrHighlightX0;
    dr.drX1= x1Pixels;

    res= (*lr->lrVisitRectangle)( &dr, lr->lrParaNode, lr->lrThrough );
    if  ( res != SCANadviceOK && res != SCANadviceSTOP )
	{ LDEB(res); return -1;	}

    lr->lrInHighlight= 0;

    return res;
    }

static int docVisitParticuleHighlight(
				const VisitParticule *	vp,
				void *			vlb )
    {
    LineRectangles *		lr= (LineRectangles *)vlb;
    const LayoutContext *	lc= lr->lrLayoutContext;
    const TextParticule *	tp= vp->vpTextParticule;

    const int			tpFrom= tp->tpStroff;
    const int			tpUpto= tp->tpStroff+ tp->tpStrlen;

    int				from;
    int				upto;

    int				x0Pixels;

    x0Pixels= docLayoutXPixels( lc, lr->lrXTwips );

    if  ( docIntersectHiglight( &from, &upto, tpFrom, tpUpto,
					lr->lrDsThisLine.dsHead.dpStroff,
					lr->lrDsThisLine.dsTail.dpStroff ) )
	{
	/* leaving highlight ? */
	if  ( lr->lrInHighlight != 0 )
	    {
	    int	res= docVisitHighlight( lr, x0Pixels );
	    if  ( res != SCANadviceOK )
		{ return res;	}
	    }
	}
    else{
	/* entering highlight ? */
	if  ( lr->lrInHighlight == 0 )
	    {
	    lr->lrInHighlight= 1;
	    lr->lrHighlightX0= x0Pixels;
	    }
	}

    lr->lrXTwips += tp->tpWide;

    return SCANadviceOK;
    }

static int docVisitRunHighlight( const TextRun *		tr,
				void *				vlb )
    {
    int				rval= SCANadviceOK;
    LineRectangles *		lr= (LineRectangles *)vlb;
    const LayoutContext *	lc= lr->lrLayoutContext;

    const int			trFrom= tr->trStroff;
    const int			trUpto= tr->trStroff+ tr->trStrlen;

    int				from;
    int				upto;

    int				twipsWide= docTextRunTwipsWide( tr );

    int				len= trUpto- trFrom;

    const char *		printString= (const char *)0;
    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    const int			mirror= 0;
    int				fullScreenFont;
    int				scapsScreenFont;
    int				fullSizeTwips;
    int				scapsSizeTwips;
    const struct AfmFontInfo *	afi;

    int				x0Pixels;
    int				x1Pixels;

    x0Pixels= docLayoutXPixels( lc, lr->lrXTwips );
    x1Pixels= docLayoutXPixels( lc, lr->lrXTwips+ twipsWide );

    if  ( docIntersectHiglight( &from, &upto, trFrom, trUpto,
					lr->lrDsThisLine.dsHead.dpStroff,
					lr->lrDsThisLine.dsTail.dpStroff ) )
	{
	/* leaving highlight ? */
	if  ( lr->lrInHighlight != 0 )
	    {
	    int	res= docVisitHighlight( lr, x0Pixels );
	    if  ( res != SCANadviceOK )
		{ return res;	}
	    }

	lr->lrXTwips += twipsWide;
	goto ready;
	}

    if  ( docLayoutMakeOutput( &printString, &scratchString,
			&segments, &segmentCount,
			&fullScreenFont, &scapsScreenFont,
			&afi, &fullSizeTwips, &scapsSizeTwips, tr, mirror, lc,
			docParaString( lr->lrParaNode, trFrom ), len ) )
	{ LDEB(len); rval= -1; goto ready;	}

    switch( tr->trDirection )
	{
	case 0:
	    /* entering highlight ? */
	    if  ( lr->lrInHighlight == 0 )
		{
		int wide= 0;

		if  ( from > trFrom )
		    {
		    wide= docGetScreenWidth( lc, x0Pixels, lr->lrYPixels,
				    fullScreenFont, scapsScreenFont,
				    printString, from- trFrom,
				    segments, segmentCount );
		    }

		lr->lrInHighlight= 1;
		lr->lrHighlightX0= x0Pixels+ wide;
		}

	    /* leaving highlight ? */
	    if  ( lr->lrInHighlight != 0 && upto < trUpto )
		{
		int	res;
		int	wide;

		wide= docGetScreenWidth( lc, x0Pixels, lr->lrYPixels,
				    fullScreenFont, scapsScreenFont,
				    printString, upto- trFrom,
				    segments, segmentCount );

		res= docVisitHighlight( lr, x0Pixels+ wide );
		if  ( res != SCANadviceOK )
		    { return res;	}
		}
	    break;

	case 1:
	    /* entering highlight ? */
	    if  ( lr->lrInHighlight == 0 )
		{
		if  ( upto < trUpto )
		    {
		    int	wide;

		    wide= docGetScreenWidth( lc, x0Pixels, lr->lrYPixels,
				    fullScreenFont, scapsScreenFont,
				    printString, upto- trFrom,
				    segments, segmentCount );

		    lr->lrInHighlight= 1;
		    lr->lrHighlightX0= x1Pixels- wide;
		    }
		else{
		    lr->lrInHighlight= 1;
		    lr->lrHighlightX0= x0Pixels;
		    }
		}

	    /* leaving highlight ? */
	    if  ( lr->lrInHighlight != 0 && from > trFrom )
		{
		int	res;
		int	wide;

		wide= docGetScreenWidth( lc, x0Pixels, lr->lrYPixels,
				    fullScreenFont, scapsScreenFont,
				    printString, from- trFrom,
				    segments, segmentCount );

		res= docVisitHighlight( lr, x1Pixels- wide );
		if  ( res != SCANadviceOK )
		    { return res;	}
		}
	    break;

	default:
	    LDEB(tr->trDirection);
	    rval= -1; goto ready;
	}

    lr->lrXTwips += twipsWide;

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

int docForAllLineRectangles(
			LineRectangles *		lr,
			int				x0Twips,
			int				partsSeparately,
			VisitLineRectangle		visitRectangle,
			void *				through )
    {
    int			res;
    int			flags;

    lr->lrInHighlight= 0;
    lr->lrHighlightX0= -1;
    lr->lrXTwips= x0Twips;

    lr->lrVisitRectangle= visitRectangle;
    lr->lrThrough= through;

    if  ( ( lr->lrCmp_ShLh < 0 && lr->lrCmp_StLt > 0 )	||
	  lr->lrAsTableRectangle			)
	{
	res= (*lr->lrVisitRectangle)( lr->lrDrThisLineFrame,
					    lr->lrParaNode, lr->lrThrough );
	if  ( res != SCANadviceOK && res != SCANadviceSTOP )
	    { LDEB(res); return -1;	}

	return res;
	}

    if  ( lr->lrCmp_ShLh < 0 )
	{
	lr->lrHighlightX0= lr->lrDrThisLineFrame->drX0;
	lr->lrInHighlight= 1;
	}

    if  ( partsSeparately )
	{ flags= FLAGpsSCAN_RUN_SEPARATELY;	}
    else{ flags= FLAGpsSCAN_COMBINE_LINES;	}
 
    res= docScanLineDisplayOrder( lr->lrDocument,
				lr->lrParaNode,
				lr->lrTextLine,
				(struct DocumentSelection *)0, flags,
				docVisitParticuleHighlight,
				(ParaFieldVisitor)0,
				docVisitRunHighlight,
				(ObjectVisitor)0,
				(TabVisitor)0,
				(void *)lr );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    if  ( lr->lrInHighlight )
	{
	int	x1Pixels;

	if  ( lr->lrCmp_StLt > 0 )
	    { x1Pixels= lr->lrDrThisLineFrame->drX1;	}
	else{ x1Pixels= docLayoutXPixels( lr->lrLayoutContext, lr->lrXTwips ); }

	res= docVisitHighlight( lr, x1Pixels );
	if  ( res != SCANadviceOK && res != SCANadviceSTOP )
	    { return res;	}
	}

    return res;
    }

void docSetLineRectangles(	LineRectangles *		lr,
				const LayoutContext *		lc,
				const DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct TextLine *		tl,
				int				yPixels,
				const DocumentRectangle *	drLineFrame )
    {
    const ParagraphProperties *	ppHead= ds->dsHead.dpNode->biParaProperties;
    const ParagraphProperties *	ppTail= ds->dsTail.dpNode->biParaProperties;

    lr->lrLayoutContext= lc;
    lr->lrDocument= lc->lcDocument;
    lr->lrParaNode= paraNode;
    lr->lrTextLine= tl;
    lr->lrIntersectsSelection= 0;
    lr->lrYPixels= yPixels;

    if  ( ! docSelectionSameRoot( ds, paraNode ) )
	{ lr->lrIntersectsSelection= 0; return;	}

    /*  1  */
    docTextLineSelection( &(lr->lrDsThisLine), paraNode, tl, tl );
    lr->lrDrThisLineFrame= drLineFrame;

    lr->lrCmp_StLh=
	    docComparePositions( &(ds->dsTail), &(lr->lrDsThisLine.dsHead) );
    if  ( lr->lrCmp_StLh < 0 )
	{ lr->lrIntersectsSelection= 0; return;	}

    lr->lrCmp_ShLt=
	    docComparePositions( &(ds->dsHead), &(lr->lrDsThisLine.dsTail) );
    if  ( lr->lrCmp_ShLt > 0 )
	{ lr->lrIntersectsSelection= 0; return;	}

    lr->lrCmp_ShLh=
	    docComparePositions( &(ds->dsHead), &(lr->lrDsThisLine.dsHead) );
    lr->lrCmp_StLt=
	    docComparePositions( &(ds->dsTail), &(lr->lrDsThisLine.dsTail) );

    lr->lrIntersectsSelection= 1;
    docIntersectLineSelection( &(lr->lrDsThisLine), paraNode, tl, ds );

    lr->lrAsTableRectangle= 0;
    if  ( ppHead->ppTableNesting > 0		&&
	  ppTail->ppTableNesting > 0		&&
	  ! docSelectionInsideCell( ds )	)
	{ lr->lrAsTableRectangle= 1;	}
    }

void docInitCollectRectangles(	CollectRectangles *	cr )
    {
    cr->crCount= 0;
    geoInitRectangle( &(cr->crUnion) );
    }

static int docTextLineSelectedCollectRectangle(
				const DocumentRectangle *	dr,
				struct BufferItem *		paraNode,
				void *				vcr )
    {
    CollectRectangles *	cr= (CollectRectangles *)vcr;

    if  ( cr->crCount == 0 )
	{ cr->crUnion= *dr;						}
    else{ geoUnionRectangle( &(cr->crUnion), &(cr->crUnion), dr );	}

    cr->crCount++;

    return SCANadviceOK;
    }

int docTextLineSelectedCollectRectangles(
			LineRectangles *		lr,
			int				x0Twips,
			int				partsSeparately,
			CollectRectangles *		cr )
    {
    return docForAllLineRectangles( lr, x0Twips, partsSeparately,
			docTextLineSelectedCollectRectangle, (void *)cr );
    }

