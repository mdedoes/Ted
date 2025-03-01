/************************************************************************/
/*									*/
/*  Translate back and forth between text offset in a line and pixel	*/
/*  X coordinate.							*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stdlib.h>

#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docSelect.h>
#   include	<layoutContext.h>
#   include	<docStripFrame.h>
#   include	<docLayout.h>
#   include	<docScreenLayout.h>
#   include	<docScanner.h>
#   include	<docTextRun.h>
#   include	<docParaScanner.h>
#   include	<drawDrawingSurface.h>
#   include	"docScreenDraw.h"
#   include	<psTextExtents.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the X coordinate (in pixels) of an offset in a line.		*/
/*									*/
/************************************************************************/

typedef struct OffsetX
    {
				/**
				 *  The position that we are finding the 
				 *  coordinate for.
				 */
    const DocumentPosition *	oxPosition;

				/**
				 *  Layout context for geometry calculations
				 */
    const LayoutContext *	oxLayoutContext;

				/**
				 *  Flags to be passed to the paragraph scanner
				 */
    int				oxFlags;

				/**
				 *  The accumulated width in display order
				 *  (in twips) of the particules before 
				 *  the position that we are looking for.
				 */
    int				oxXTwips;

				/**
				 *  The result of the calculation. It is 
				 *  derived from the cumulated twips width 
				 *  upto the current particule/run and 
				 *  the actual screen metrics inside the 
				 *  particule run that holds the position.
				 */
    int				oxXPixels;

				/**
				 *  A realistic Y value for the calculations
				 */
    int				oxYPixels;
    } OffsetX;

/************************************************************************/

static int docCalculateParticuleX(
				const VisitParticule *	vp,
				void *			vox )
    {
    OffsetX *			ox= (OffsetX *)vox;
    const LayoutContext *	lc= ox->oxLayoutContext;

    const TextParticule *	tp= vp->vpTextParticule;

    switch( tp->tpKind )
	{
	case TPkindOBJECT:
	case TPkindTAB:
	case TPkindCHFTNSEP:
	case TPkindCHFTNSEPC:
	    /*  Before or after this particule */
	    if  ( tp->tpStroff > ox->oxPosition->dpStroff		||
		  tp->tpStroff+ tp->tpStrlen < ox->oxPosition->dpStroff )
		{
		ox->oxXTwips += tp->tpWide;
		return SCANadviceOK;
		}

	    /*  At the head */
	    if  ( tp->tpStroff == ox->oxPosition->dpStroff )
		{
		ox->oxXPixels= docLayoutXPixels( lc, ox->oxXTwips );
		return SCANadviceSTOP;
		}
	    /*  At the tail */
	    if  ( tp->tpStroff+ tp->tpStrlen == ox->oxPosition->dpStroff )
		{
		ox->oxXTwips += tp->tpWide;
		ox->oxXPixels= docLayoutXPixels( lc, ox->oxXTwips );
		return SCANadviceSTOP;
		}

	    /*  inside an atomic particule */
	    LLLDEB(ox->oxPosition->dpStroff,tp->tpStroff,tp->tpStrlen);
	    ox->oxXTwips += tp->tpWide; /* MUST be irrelevant */
	    return SCANadviceOK;

	case TPkindFIELDHEAD:
	case TPkindFIELDTAIL:
	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	case TPkindOPT_HYPH:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    if  ( tp->tpWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpWide);	}
	    ox->oxXTwips += tp->tpWide;
	    return SCANadviceOK;

	default:
	    if  ( tp->tpWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpWide);	}
	    else{ LDEB(tp->tpKind);			}
	    ox->oxXTwips += tp->tpWide;
	    return SCANadviceOK;
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the width of the run.					*/
/*									*/
/************************************************************************/

static int docCalculateRunX(	const TextRun *			tr,
				void *				vox )
    {
    int				rval= SCANadviceOK;
    OffsetX *			ox= (OffsetX *)vox;
    const LayoutContext *	lc= ox->oxLayoutContext;

    const BufferItem *		paraNode= tr->trParaNode;

    int				len;
    int				x0Twips;
    int				x0Pixels;

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

    DocumentRectangle		drTwips;

    int				pixelsWide= 0;
    int				twipsWide= 0;

    /* No intersection: count full width */
    if  ( ox->oxPosition->dpStroff < tr->trStroff 		||
	  ox->oxPosition->dpStroff > tr->trStroff+ tr->trStrlen	)
	{
	ox->oxXTwips += docTextRunTwipsWide( tr );
	rval= SCANadviceOK; goto ready;
	}

    /*  inside the run */
    len= ox->oxPosition->dpStroff- tr->trStroff;
    x0Twips= ox->oxXTwips;
    x0Pixels= docLayoutXPixels( lc, x0Twips );

    if  ( docLayoutMakeOutput( &printString, &scratchString,
			&segments, &segmentCount,
			&fullScreenFont, &scapsScreenFont,
			&afi, &fullSizeTwips, &scapsSizeTwips, tr, mirror, lc,
			docParaString( paraNode, tr->trStroff ), len ) )
	{ LDEB(len); rval= -1; goto ready;	}

    pixelsWide= docGetScreenWidth( lc, x0Pixels, ox->oxYPixels,
					fullScreenFont, scapsScreenFont,
					printString, len,
					segments, segmentCount );

    twipsWide= psSegmentedTextExtents( &drTwips, printString, len,
					segments, segmentCount,
					fullSizeTwips, afi );
    switch( tr->trDirection )
	{
	case 0:
	    ox->oxXPixels= x0Pixels+ pixelsWide;
	    ox->oxXTwips= x0Twips+ twipsWide;
	    break;

	case 1:
	    {
	    int			x1Twips;
	    int			x1Pixels;

	    x1Twips= x0Twips+ docTextRunTwipsWide( tr );
	    x1Pixels= docLayoutXPixels( lc, x1Twips );

	    ox->oxXPixels= x1Pixels- pixelsWide;
	    ox->oxXTwips= x1Twips- twipsWide;
	    }
	break;

	default:
	    LDEB(tr->trDirection);
	    rval= -1; goto ready;
	}

    rval= SCANadviceSTOP;

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

int docLayoutLineXOfPosition(	int *				pDocXPixels,
				int *				pDocXTwips,
				const DocumentPosition *	dp,
				const LayoutContext *		lc,
				const ParagraphFrame *		pf,
				struct BufferItem *		paraNode,
				TextLine *			tl )
    {
    OffsetX			ox;
    int				res;

    LayoutPosition		lpTop;
    LayoutPosition		lpBaseline;
    DocumentRectangle		drLineFramePixels;
    DocumentRectangle		drLineTwips;

    const struct BlockOrigin * const bo= (const struct BlockOrigin *)0;

    docTextLineGetShiftedTop( &lpTop, bo, tl );
    docTextLineGetShiftedBaseline( &lpBaseline, bo, tl );
    docLineTextRectangleTwips( &drLineTwips, &lpTop, &lpBaseline, tl, pf );
    docLineFrameRectanglePixels( &drLineFramePixels, &lpTop, &lpBaseline,
							tl, bo, pf, lc );

    ox.oxPosition= dp;
    ox.oxLayoutContext= lc;
    ox.oxXTwips= drLineTwips.drX0;
    ox.oxXPixels= drLineFramePixels.drX0;
    ox.oxYPixels= docLayoutYPixels( lc, &lpBaseline );

    if  ( docScreenDrawParticulesSeparately( paraNode, tl ) )
	{ ox.oxFlags= FLAGpsSCAN_RUN_SEPARATELY;		}
    else{ ox.oxFlags= FLAGpsSCAN_COMBINE_LINES;			}

    res= docScanLineDisplayOrder( lc->lcDocument, dp->dpNode, tl,
				(struct DocumentSelection *)0, ox.oxFlags,
				docCalculateParticuleX,
				(ParaFieldVisitor)0,
				docCalculateRunX,
				(ObjectVisitor)0,
				(TabVisitor)0,
				(void *)&ox );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res != SCANadviceSTOP )
	{ ox.oxXPixels= docLayoutXPixels( lc, ox.oxXTwips );	}

    *pDocXPixels= ox.oxXPixels;
    *pDocXTwips= ox.oxXTwips;

    return 0;
    }

