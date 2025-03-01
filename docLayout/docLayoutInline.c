/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout of the particules inside the	*/
/*  line.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	"docLayoutLine.h"
#   include	"layoutContext.h"

#   include	<docObjectProperties.h>
#   include	<docTextLine.h>
#   include	<docTreeNode.h>
#   include	<docShape.h>
#   include	<docObject.h>
#   include	<textAttribute.h>
#   include	<docBorderProperties.h>
#   include	<docTextParticule.h>
#   include	<psTextExtents.h>
#   include	"docParticuleData.h"
#   include	<docStripFrame.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start to insert a candidate run in the line.			*/
/*									*/
/************************************************************************/

void docInitLineRun(		LineRun *		lr )
    {
    lr->lrParaNode= (struct BufferItem *)0;

    lr->lrHeadParticule= 0;
    lr->lrTailParticule= 0;

    geoInitRectangle( &(lr->lrRectangle) );
    lr->lrVisibleX1= 0;

    lr->lrFattestBorder= 0;

    lr->lrWordCount= 0;

    lr->lrLineFlags= 0;

    lr->lrAddRightBorder= 0;
    lr->lrRightBorderThick= 0;
    }

void docLineLayoutStartRun(	LineRun *		lr,
				struct BufferItem *	paraNode,
				int			part,
				int			x0 )
    {
    lr->lrParaNode= paraNode;

    lr->lrHeadParticule= part;
    lr->lrTailParticule= part;

    lr->lrRectangle.drX0= x0;
    lr->lrRectangle.drX1= lr->lrRectangle.drX0;
    lr->lrRectangle.drY0= 0;
    lr->lrRectangle.drY1= 0;
    lr->lrVisibleX1= lr->lrRectangle.drX0;

    lr->lrFattestBorder= 0;

    lr->lrWordCount= 0;

    lr->lrLineFlags= 0;

    lr->lrAddRightBorder= 0;
    lr->lrRightBorderThick= 0;

    return;
    }

void docLineLayoutStartChildRun(	struct LineRun *	lr,
					int			part,
					const struct LineRun *	parent )
    {
    docLineLayoutStartRun( lr, parent->lrParaNode, part,
						parent->lrRectangle.drX1 );
    }

void docLineLayoutIncludeRun(	LineRun *	to,
				const LineRun *	from )
    {
    to->lrTailParticule= from->lrTailParticule;

    geoIncludeRectangleY( &(to->lrRectangle), &(from->lrRectangle) );

    to->lrRectangle.drX1= from->lrRectangle.drX1;
    if  ( from->lrVisibleX1 > from->lrRectangle.drX0 )
	{ to->lrVisibleX1= from->lrVisibleX1;	}

    if  ( to->lrFattestBorder < from->lrFattestBorder )
	{ to->lrFattestBorder=  from->lrFattestBorder;	}

    to->lrWordCount += from->lrWordCount;
    to->lrLineFlags |= from->lrLineFlags;

    return;
    }

/************************************************************************/

void docLayoutAddExtraToPrevious(	ParticuleData *	pd,
					int		extra )
    {
    if  ( pd[-1].pdTwipsWide+ extra < 0 )
	{
	LLDEB(pd[-1].pdTwipsWide,extra);
	pd[-1].pdTwipsWide= 0;
	}
    else{ pd[-1].pdTwipsWide += extra;		}

    if  ( pd[-1].pdVisibleBBox.drX1+ extra < 0 )
	{
	LLDEB(pd[-1].pdVisibleBBox.drX1,extra);
	pd[-1].pdVisibleBBox.drX1= 0;
	}
    else{ pd[-1].pdVisibleBBox.drX1 += extra;	}

    return;
    }

void docLayoutAddRightBorderToPrevious(	ParticuleData *	pd,
					int		rightBorderThick )
    {
    pd[-1].pdFlags |= TPflagRIGHT_BORDER;
    pd[-1].pdRightBorderWidth= rightBorderThick;

    docLayoutAddExtraToPrevious( pd, rightBorderThick );
    }

/************************************************************************/
/*									*/
/*  Get font information.						*/
/*									*/
/************************************************************************/

const struct AfmFontInfo * docLayoutGetAfi(
			int *				pTextAttrNr,
			const TextAttribute **		pTa,
			unsigned char *			pLineFlags,
			const LayoutContext *		lc,
			const struct BufferItem *	paraNode,
			int				part )
    {
    struct BufferDocument *	bd= lc->lcDocument;

    const struct AfmFontInfo *	afi;
    int				textAttrNr;
    const TextAttribute *	ta;

    textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    afi= docDocLayoutGetFontInfo( lc, ta );
    if  ( ! afi )
	{ LDEB(textAttrNr); return (struct AfmFontInfo *)0; }

    if  ( docBorderNumberIsBorder( bd, ta->taBorderNumber ) )
	{ *pLineFlags |= TLflagBORDER;	}

    if  ( docShadingNumberIsShading( bd, ta->taShadingNumber ) )
	{ *pLineFlags |= TLflagSHADING;	}

    *pTa= ta;
    *pTextAttrNr= textAttrNr;
    return afi;
    }

/************************************************************************/

int docLayoutTextExtents(	DocumentRectangle *		drText,
				const TextAttribute *		ta,
				const struct AfmFontInfo *	afi,
				const char *			from,
				int				len )
    {
    int			fontSizeTwips;
    int			width;

    fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

    if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT ||
	  ta->taSuperSub == TEXTvaSUBSCRIPT   )
	{ fontSizeTwips= SUPERSUB_SIZE( fontSizeTwips ); }

    width= psTextExtents( drText, ta, fontSizeTwips, afi, from, len );
    if  ( width < 0 )
	{ LLDEB(len,width); return -1;	}

    if  ( ta->taBaselineShiftHalfPoints > 0 )
	{ drText->drY0 -= 10* ta->taBaselineShiftHalfPoints;	}
    if  ( ta->taBaselineShiftHalfPoints < 0 )
	{ drText->drY1 -= 10* ta->taBaselineShiftHalfPoints;	}

    return width;
    }

int docLayoutTextExtentsInParticule(
				DocumentRectangle *		drText,
				const LayoutContext *		lc,
				const BufferItem *		paraNode,
				int				part,
				const char *			from,
				int				len )
    {
    int				textAttrNr;
    const TextAttribute *	ta;
    const struct AfmFontInfo *	afi;

    unsigned char		ignoredLineFlags= 0;

    afi= docLayoutGetAfi( &textAttrNr, &ta, &ignoredLineFlags,
						    lc, paraNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    return docLayoutTextExtents( drText, ta, afi, from, len );
    }

/************************************************************************/
/*									*/
/*  Get descent/ascent for the font of a special particule.		*/
/*									*/
/************************************************************************/

static int docLayoutFontAscDesc( const struct BufferItem *	paraNode,
				const TextAttribute **		pTa,
				DocumentRectangle *		drAscDesc,
				int *				pBorder,
				const LayoutContext *		lc,
				int				part )
    {
    struct BufferDocument *	bd= lc->lcDocument;
    const TextAttribute *	ta;

    int				rval;
    DocumentRectangle		drBBox;

    const struct AfmFontInfo *	afi;

    int				fontSizeTwips;
    const int			vswap= 1;

    unsigned char		lineFlags= 0;
    int				textAttrNr;

    afi= docLayoutGetAfi( &textAttrNr, &ta, &lineFlags, lc, paraNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    fontSizeTwips= TA_FONT_SIZE_TWIPS( ta );

    rval= psFontBBox( &drBBox, drAscDesc, fontSizeTwips, vswap, afi );

    if  ( pBorder && ( lineFlags & TLflagBORDER ) )
	{
	int				thick= 0;
	int				space= 0;
	const BorderProperties *	bp;

	bp= docGetBorderPropertiesByNumber( bd, ta->taBorderNumber );

	thick= docBorderThick( &space, bp );

	*pBorder= thick+ space;
	}

    *pTa= ta;
    return rval;
    }

/************************************************************************/

int docLayoutInlineObject(	LineRun *			lr,
				ParticuleData *			pd,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				InsertedObject *		io )
    {
    const BorderProperties *	bp;
    int				thick= 0;
    int				space= 0;

    int				width;
    const TextAttribute *	ta;

    /*  In shapes that do not compete with the text, do not reserve space */
    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;
	const ShapeProperties *	sp= &(ds->dsShapeProperties);
	const ShapeDrawing *	sd= &(ds->dsDrawing);

	if  ( ! sd->sd_fPseudoInline				&&
	      ( sp->spWrapStyle == SHPswtNONE		||
	        sp->spWrapStyle == SHPswtTHROUGH	)	)
	    {
	    lr->lrTailParticule= part+ 1;
	    lr->lrLineFlags= TLflagINLINECONTENT; /* WHY? */
	    return 0;
	    }
	}

    width= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100.0;

    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
    bp= docGetBorderPropertiesByNumber( bd, ta->taBorderNumber );

    if  ( DOCisBORDER( bp ) )
	{ thick= docBorderThick( &space, bp );	}

    lr->lrTailParticule= part+ 1;

    lr->lrRectangle.drX1= lr->lrRectangle.drX0+ width;
    lr->lrRectangle.drY0= -( io->ioScaleYUsed* io->ioTwipsHigh )/ 100.0;
    lr->lrRectangle.drY1= 0;
    lr->lrVisibleX1= lr->lrRectangle.drX1;

    lr->lrFattestBorder= thick+ space;
    lr->lrLineFlags= TLflagINLINECONTENT;

    if  ( ta->taBaselineShiftHalfPoints > 0 )
	{ lr->lrRectangle.drY0 -= 10* ta->taBaselineShiftHalfPoints; }
    if  ( ta->taBaselineShiftHalfPoints < 0 )
	{ lr->lrRectangle.drY1 -= 10* ta->taBaselineShiftHalfPoints; }

    /* some extra space ? Take leading! LINEDISTANCE */
    lr->lrRectangle.drY0 -= ( 150* paraNode->biParaMajorityFontSize )/ 100;

    pd->pdTwipsWide= width;
    pd->pdVisibleBBox= lr->lrRectangle;
    pd->pdVisibleBBox.drX0= 0;
    pd->pdVisibleBBox.drX1= pd->pdTwipsWide;

    return 0;
    }

int docLayoutSeparator(	struct LineRun *		lr,
			ParticuleData *			pd,
			const struct BufferItem *	paraNode,
			int				part,
			const LineLayoutJob *		llj )
    {
    int				width= 2880;
    const TextAttribute *	ta;

    DocumentRectangle		drAscDesc;

    if  ( width > llj->lljLineWidth )
	{ width=  llj->lljLineWidth;	}

    if  ( docLayoutFontAscDesc( paraNode, &ta,
			    &drAscDesc, &(lr->lrFattestBorder),
			    llj->lljLayoutContext, part ) < 0 )
	{ LDEB(1); return -1;	}

    lr->lrRectangle.drY0= drAscDesc.drY0;
    lr->lrRectangle.drY1= drAscDesc.drY1;

    if  ( ta->taBaselineShiftHalfPoints > 0 )
	{ lr->lrRectangle.drY0 -= 10* ta->taBaselineShiftHalfPoints; }
    if  ( ta->taBaselineShiftHalfPoints < 0 )
	{ lr->lrRectangle.drY1 -= 10* ta->taBaselineShiftHalfPoints; }

    lr->lrTailParticule= part+ 1;
    lr->lrRectangle.drX1= lr->lrRectangle.drX0+ width;
    lr->lrVisibleX1= lr->lrRectangle.drX1;
    lr->lrLineFlags |= TLflagINLINECONTENT;

    pd->pdTwipsWide= width;
    pd->pdVisibleBBox= lr->lrRectangle;
    pd->pdVisibleBBox.drX0= 0;
    pd->pdVisibleBBox.drX1= pd->pdTwipsWide;

    return 0;
    }

int docLayoutOptionalHyphen(
			struct LineRun *		lr,
			ParticuleData *			pd,
			const struct BufferItem *	paraNode,
			int				part,
			const LayoutContext *		lc )
    {
    int				width;
    DocumentRectangle		drAscDesc;

    int				from;
    int				len;

    from= paraNode->biParaParticules[part].tpStroff;
    len= paraNode->biParaParticules[part].tpStrlen;

    width= docLayoutTextExtentsInParticule( &drAscDesc, lc, paraNode, part,
				    docParaString( paraNode, from ), len );
    if  ( width < 0 )
	{ LLDEB(len,width); return -1;	}

    lr->lrTailParticule= part+ 1;
    lr->lrRectangle.drX1= lr->lrRectangle.drX0+ width;
    lr->lrVisibleX1= lr->lrRectangle.drX1;
    lr->lrLineFlags |= TLflagINLINECONTENT;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add an optional hyphen to a line that is full.			*/
/*									*/
/*  It is guaranteed to fit, because otherwise the line would have been	*/
/*  folded to make it fit.						*/
/*									*/
/************************************************************************/

int docLayoutShowOptionalHyphen( const LayoutContext *		lc,
				struct BufferItem *		paraNode,
				int				part,
				unsigned char *			pLineFlags,
				ParticuleData *			pd,
				int				x0 )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				textAttrNr;
    const TextAttribute *	ta;
    const struct AfmFontInfo *	afi;

    int				width;
    DocumentRectangle		drPart;

    const char *		from;
    int				len;

    unsigned char		lineFlags= 0;

    from= docParaString( paraNode, tp->tpStroff );
    len= tp->tpStrlen;

    afi= docLayoutGetAfi( &textAttrNr, &ta, &lineFlags, lc, paraNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    width= docLayoutTextExtents( &drPart, ta, afi, from, len );
    if  ( width < 0 )
	{ LLDEB(len,width); return -1;	}

    docInitParticuleData( pd );
    pd->pdAfi= afi;
    pd->pdX0= x0;
    pd->pdTwipsWide= width;
    pd->pdVisibleBBox= drPart;

    *pLineFlags |= lineFlags;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the pixel rectangle for a line of text.			*/
/*  The rectangle has the full width of the frame that holds the	*/
/*  paragraph that holds the line.					*/
/*									*/
/************************************************************************/

void docLineFrameRectanglePixels(
				DocumentRectangle *	drLineFramePixels,
				const LayoutPosition *	lpTop,
				const LayoutPosition *	lpBottom,
				const TextLine *	tl,
				const BlockOrigin *	bo,
				const ParagraphFrame *	pf,
				const LayoutContext *	lc )
    {
    docGetPixelRectForPos( drLineFramePixels, lc,
			pf->pfCellContentRect.drX0+ (bo?bo->boXShift:0),
			pf->pfCellContentRect.drX1+ (bo?bo->boXShift:0),
			lpTop, lpBottom );

    return;
    }

