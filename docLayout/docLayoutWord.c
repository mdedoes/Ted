/************************************************************************/
/*									*/
/*  Layout of a document. Place a contigous range of SPAN particules	*/
/*  that form a 'Word'.							*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docLayoutLine.h"
#   include	"layoutContext.h"
#   include	<docListLevel.h>
#   include	<docBorderProperties.h>
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<psFontInfo.h>
#   include	"docParticuleData.h"
#   include	<docAttributes.h>
#   include	<docParaParticules.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static void docLayoutAddSlantToPrevious(	int *			pX,
						ParticuleData *		pd,
						const AfmFontInfo *	afi,
						const TextAttribute *	ta,
						const TextAttribute *	taPr )
    {
    int		extra;

    if  ( taPr->taFontSizeHalfPoints < ta->taFontSizeHalfPoints )
	{ extra= taPr->taFontSizeHalfPoints;		}
    else{ extra= ta->taFontSizeHalfPoints;		}

    extra = (int)( -afi->afiTanItalicAngle* 10/ 2* extra );

    *pX += extra;
    docLayoutAddExtraToPrevious( pd, extra );

    return;
    }

/************************************************************************/

static int docLayoutSpanHasLeftBorder(	int *			pThick,
					LineLayoutJob *		llj,
					LineRun *		lr,
					const TextParticule *	tp,
					const TextAttribute *	ta,
					const TextAttribute *	taPr )
    {
    int				spanHasLeftBorder= 0;
    int				leftBorderThick= 0;
    int				space= 0;

    struct BufferDocument *	bd= llj->lljLayoutContext->lcDocument;
    const BorderProperties *	bp;

    bp= docGetBorderPropertiesByNumber( bd, ta->taBorderNumber );

    leftBorderThick= docBorderThick( &space, bp );

    if  ( lr->lrFattestBorder < leftBorderThick+ space )
	{ lr->lrFattestBorder=  leftBorderThick+ space;	}

    if  ( tp->tpStroff == llj->lljHeadStroff )
	{ spanHasLeftBorder= 1;		}
    else{
	if  ( taPr && taPr->taBorderNumber != ta->taBorderNumber )
	    { spanHasLeftBorder= 1;	}
	}

    *pThick= leftBorderThick;
    return spanHasLeftBorder;
    }

/************************************************************************/
/*									*/
/*  Place a single 'word'.						*/
/*									*/
/*  A 'Word' is a series of TEXT particules that does not contain any	*/
/*  white space.							*/
/*									*/
/*  1)  Check that the assumption below that the loop is entered at	*/
/*	least once is true.						*/
/*  2)  For groups with the same text attributes...			*/
/*  3)  Retrieve font information.					*/
/*  4)  Reserve space for the text border. If necessary also left of	*/
/*	the text.							*/
/*  5)  Calculate text extents.						*/
/*  6)  White space: End of word.					*/
/*  7)  Claim a little more space if the font switches from italic to	*/
/*	regular. ( ////|||| )						*/
/*  8)  Claim space for the right text border if any. As this is inside	*/
/*	the word there is no relation with line folding.		*/
/*  9)  Add the right border to the visible x1. The caller claims space	*/
/*	if the line is not folded.					*/
/*									*/
/************************************************************************/

int docLayoutWord(	LineLayoutJob *			llj,
			LineRun *			lr,
			ParticuleData *			pd,
			int				particuleUpto )
    {
    const TextAttribute *	taPr;
    const AfmFontInfo *		afi= (const AfmFontInfo *)0;
    int				leftBorderThick= 0;
    int				haveBorder= 0;

    int				accepted;
    int				width;

    int				len= 0;
    int				x= lr->lrRectangle.drX0;
    int				visibleX1= lr->lrRectangle.drX0;

    const struct BufferItem *	paraNode= llj->lljParaNode;
    struct BufferDocument *	bd= llj->lljLayoutContext->lcDocument;

    int				part= lr->lrHeadParticule;
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    const char *		from= docParaString( paraNode, tp->tpStroff );
    unsigned char		lineFlags= 0;

    taPr= (const TextAttribute *)0;

    if  ( part > 0 )
	{ docGetEffectiveTextAttributes( &taPr, bd, paraNode, part- 1 ); }

    /*  1  */
    if  ( part >= particuleUpto || tp->tpKind != TPkindSPAN )
	{ LLLDEB(part,particuleUpto,tp->tpKind); return -1; }

    /*  2  */
    accepted= 0;
    while( part < particuleUpto && tp->tpKind == TPkindSPAN )
	{
	int			x0Span= x;
	int			textAttrNr0;
	int			textAttrNr;
	const TextAttribute *	ta;

	int			firstInSpan= 1;
	int			spanHasLeftBorder= 0;
	unsigned char		spanFlags= 0;

	int			spanPartUpto= particuleUpto;

	leftBorderThick= 0;
	haveBorder= 0;

	if  ( part+ 1 < particuleUpto )
	    {
	    int			direction;
	    int			spanStroffUpto;
	    int			res;

	    res= docParaDelimitDirectionalRun( &direction, &spanStroffUpto,
				    &spanPartUpto, paraNode, tp->tpStroff );
	    if  ( res )
		{
		LLLLDEB(part,particuleUpto,tp->tpStroff,res);
		docListNode(bd,0,paraNode,0);
		if  ( res < 0 )
		    { return -1;	}
		direction = 0;
		spanStroffUpto= tp->tpStroff;
		spanPartUpto= particuleUpto;
		}

	    if  ( spanPartUpto > particuleUpto )
		{ spanPartUpto= particuleUpto;	}
	    if  ( spanPartUpto <= part )
		{
		LLLDEB(part,spanPartUpto,particuleUpto);
		spanPartUpto= particuleUpto;
		}
	    }

	/*  3  */
	afi= docLayoutGetAfi( &textAttrNr0, &ta, &spanFlags,
				    llj->lljLayoutContext, paraNode, part );
	if  ( ! afi )
	    {
	    LXDEB(tp->tpStroff,afi);
	    docListNode(bd,0,paraNode,0);
	    return -1;
	    }

	textAttrNr= textAttrNr0;
	lineFlags |= spanFlags;

	/*  4  */
	if  ( spanFlags & TLflagBORDER )
	    {
	    haveBorder= 1;

	    spanHasLeftBorder= docLayoutSpanHasLeftBorder(
				&leftBorderThick, llj, lr, tp, ta, taPr );
	    }

	/*  5  */
	while( part < spanPartUpto		&&
	       textAttrNr == textAttrNr0	&&
	       tp->tpKind == TPkindSPAN		)
	    {
	    DocumentRectangle		drPart;
	    const TextAttribute *	taTmp;

	    len += tp->tpStrlen;

	    width= docLayoutTextExtents( &drPart, ta, afi, from, len );
	    if  ( width < 0 )
		{ LLDEB(len,width); return -1;	}

	    if  ( spanHasLeftBorder )
		{
		width += leftBorderThick;
		drPart.drX1 += leftBorderThick;
		}

	    visibleX1= x0Span+ drPart.drX1;

	    docInitParticuleData( pd );
	    pd->pdAfi= afi;
	    pd->pdX0= x;
	    pd->pdTwipsWide= x0Span+ width- x;
	    pd->pdVisibleBBox= drPart;
	    if  ( ! firstInSpan )
		{
		/*  No need to fix Y's: Are always used for whole word.	*/
		pd->pdVisibleBBox.drX0= 0;
		pd->pdVisibleBBox.drX1= x0Span+ pd->pdVisibleBBox.drX1- x;
		if  ( pd->pdVisibleBBox.drX1 < 0 )
		    {
		    /*
		    This is a normal situation if the particule is all-blank:
		    It does not contribute to the visible rectangle.

		    int	oldLen= len- tp->tpStrlen;

		    LDEB(pd->pdVisibleBBox.drX1);
		    appDebug( "x=%d w=%d %d+%d=%d bytes: \"%*.*s|%*.*s\"\n",
				    x, width,
				    oldLen, tp->tpStrlen, len,
				    oldLen, oldLen, from,
				    tp->tpStrlen, tp->tpStrlen, from+ oldLen );
		    */

		    pd->pdVisibleBBox.drX1= 0;
		    }
		}

	    if  ( firstInSpan && spanHasLeftBorder )
		{
		pd->pdLeftBorderWidth= leftBorderThick;
		pd->pdFlags |= (TPflagLEFT_BORDER|TPflagXMATCH_LEFT);
		}

	    x= x0Span+ width;

	    geoIncludeRectangleY( &(lr->lrRectangle), &drPart );

	    accepted++; part++; tp++; pd++;

	    /*  6  */
	    if  ( ( len > 0 && from[len- 1] == ' ' )	||
		  part >= spanPartUpto			)
		{ break;	}

	    firstInSpan= 0;
	    textAttrNr= docGetEffectiveTextAttributes( &taTmp,
							bd, paraNode, part );
	    }

	taPr= ta;

	/*  6  */
	if  ( ( len > 0 && from[len- 1] == ' ' )	||
	      part >= particuleUpto			)
	    { break;	}

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	/*  7  */
	if  ( ! ta->taFontIsSlanted && taPr->taFontIsSlanted )
	    {
	    docLayoutAddSlantToPrevious( &x, pd, afi, ta, taPr );
	    }

	/*  8  */
	if  ( haveBorder && ta->taBorderNumber != taPr->taBorderNumber )
	    {
	    x += leftBorderThick;
	    docLayoutAddRightBorderToPrevious( pd, leftBorderThick );
	    }

	from= docParaString( paraNode, tp->tpStroff );
	x0Span= x;
	len= 0;
	}

    if  ( part < paraNode->biParaParticuleCount )
	{
	const TextAttribute *		ta;

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	/*  7  */
	if  ( afi			&&
	      part < particuleUpto	&&
	      ! ta->taFontIsSlanted	&&
	      taPr->taFontIsSlanted	)
	    { docLayoutAddSlantToPrevious( &x, pd, afi, ta, taPr ); }

	/*  9  */
	if  ( haveBorder )
	    {
	    visibleX1 += leftBorderThick;

	    if  ( ta->taBorderNumber != taPr->taBorderNumber )
		{ lr->lrAddRightBorder= 1;	}
	    }
	}
    else{
	/*  9  */
	if  ( haveBorder )
	    { lr->lrAddRightBorder= 1;	}
	}

    lr->lrTailParticule= lr->lrHeadParticule+ accepted;

    lr->lrRectangle.drX1= x;
    lr->lrVisibleX1= visibleX1;
    lr->lrRightBorderThick= leftBorderThick;
    lr->lrLineFlags= lineFlags;
    lr->lrLineFlags |= TLflagINLINECONTENT;
    lr->lrWordCount= 1;

    return accepted;
    }

