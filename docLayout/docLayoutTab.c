/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout of the tabs inside the line	*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"docLayoutLine.h"
#   include	"layoutContext.h"

#   include	<docTreeNode.h>
#   include	<textAttribute.h>
#   include	<docDocumentProperties.h>
#   include	<docTextParticule.h>
#   include	"docParticuleData.h"
#   include	<docBuf.h>
#   include	<docTabStop.h>
#   include	<docTabStopList.h>
#   include	<docListLevel.h>
#   include	<docSelect.h>
#   include	<docFields.h>
#   include	<utilLocale.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

int docLayoutTab(	LineRun *			lrTab,
			LineRun *			lrTxt,
			int *				pTabKind,
			ParticuleData *			pdTab,
			int				part,
			const struct LineLayoutJob *	llj,
			int				acceptAnyway )
    {
    const LayoutContext *	lc= llj->lljLayoutContext;
    const struct TabStopList *	tsl= llj->lljTabStopList;

    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;

    int				tab= -1;

    TabStop			ts;

    const struct AfmFontInfo *	afi;
    int				textAttrNr;
    const TextAttribute *	ta;

    const int			bulletX= llj->lljBulletTabX;

    /* Determine the next tab stop. It might be a calculated one */
    if  ( docNextTabStop( &ts, &(lrTab->lrRectangle.drX1), &tab, tsl,
		    dp->dpTabIntervalTwips, lrTab->lrRectangle.drX0 ) )
	{ LDEB(dp->dpTabIntervalTwips); return -1;	}

    /* Does the tab derived from a negative first indent apply? */
    /* Is it before the tab that we found?			*/
    if  ( bulletX > 0				&&
	  lrTab->lrRectangle.drX0 < bulletX	&&
	  bulletX < ts.tsOffset			)
	{
	docCalculatedTabStop( &ts, bulletX );

	tab= -1;
	lrTab->lrRectangle.drX1= ts.tsOffset;
	}

    if  ( ( tab < 0 || ts.tsAlignment == DOCtaLEFT )	&&
	  lrTab->lrRectangle.drX1 >= llj->lljX1		&&
	  ! acceptAnyway				)
	{ return 0;	}

    afi= docLayoutGetAfi( &textAttrNr, &ta, &(lrTab->lrLineFlags),
						lc, lrTab->lrParaNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    lrTab->lrParaNode->biParaParticules[part].tpObjectNumber= tab;
    lrTab->lrTailParticule= part+ 1;

    if  ( ts.tsAlignment == DOCtaLEFT )
	{
	lrTxt->lrRectangle.drX0= lrTab->lrRectangle.drX1;
	lrTxt->lrRectangle.drX1= lrTxt->lrRectangle.drX0;
	}
    else{
	lrTxt->lrRectangle.drX0= lrTab->lrRectangle.drX0;
	lrTxt->lrRectangle.drX1= lrTxt->lrRectangle.drX0;
	}

    pdTab->pdAfi= afi;
    pdTab->pdX0= lrTab->lrRectangle.drX0;
    pdTab->pdTwipsWide= lrTab->lrRectangle.drX1- lrTab->lrRectangle.drX0;
    pdTab->pdTabKind= ts.tsAlignment;

    *pTabKind= ts.tsAlignment;
    return 1;
    }

/************************************************************************/
/*									*/
/*  Handle the bullet of a list as if it were aligned to a tab.		*/
/*									*/
/*  Paragraph numbers are aligned as if there is a tab at the first	*/
/*  first line indent position. The kind of tab depends on the		*/
/*  alignment of the list level.					*/
/*									*/
/*  Do two things:							*/
/*  1)  Report the alignment of the bullet as the settings of a tab	*/
/*	before any text in the paragraph.				*/
/*  2)  As formatting starts at that position anyway, there is no need	*/
/*	to do anything special for left aligned bullets or paragraph	*/
/*	numbers.							*/
/*  3)  Report the end of the bullet as a temporary limit for the text	*/
/*	formatter to alow for shifting the bullet text as a piece of	*/
/*	text between tabs.						*/
/*									*/
/************************************************************************/

int docDelimitBulletAsTab(
			int *				pParticuleUpto,
			struct LineRun *		lrBul,
			struct LineRun *		lrTxt,
			int *				pTabKind,
			const struct LineLayoutJob *	llj )
    {
    struct BufferDocument *	bd= llj->lljLayoutContext->lcDocument;
    const struct BufferItem *	paraNode= llj->lljParaNode;

    int				particuleUpto= *pParticuleUpto;
    int				tabKind= *pTabKind;

    switch( llj->lljListBulletJustification )
	{
	case DOCllaLEFT:
	    /*  2  */
	    tabKind= DOCtaLEFT;
	    lrTxt->lrRectangle.drX1= llj->lljBulletTabX;
	    break;

	case DOCllaRIGHT:
	case DOCllaCENTER:
	    tabKind= DOCtaRIGHT;
	    lrBul->lrRectangle.drX0= llj->lljX0;
	    lrBul->lrRectangle.drX1= llj->lljBulletTabX;
	    break;

	default:
	    LDEB(llj->lljListBulletJustification);
	    break;
	}

	/*  2  */
    if  ( llj->lljListBulletFollow != DOCllfTAB	&&
	  tabKind != DOCtaLEFT			)
	{
	DocumentSelection		dsInsideHead;
	DocumentSelection		dsAroundHead;
	int				partBegin;
	int				partEnd;

	if  ( docDelimitParaHeadField( (struct DocumentField **)0,
				&dsInsideHead, &dsAroundHead,
				&partBegin, &partEnd, paraNode, bd ) )
	    { LDEB(1);			}
	else{ particuleUpto= partEnd+ 1;	}
	}

    *pParticuleUpto= particuleUpto;
    *pTabKind= tabKind;

    return 0;
    }

/************************************************************************/

static int docLayoutDecWidth(	LineRun *		lrTxt,
				const LayoutContext *	lc,
				int			visibleSinceTab )
    {
    BufferItem *		paraNode= lrTxt->lrParaNode;
    int				visibleToDec= visibleSinceTab;
    const SimpleLocale *	sl= lc->lcLocale;

    int				part= lrTxt->lrHeadParticule;
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				stroff0= tp->tpStroff;
    const char *		paraString= docParaString( paraNode, stroff0 );

    int				visible;

    visible= lrTxt->lrRectangle.drX0;
    for ( part= lrTxt->lrHeadParticule; part < lrTxt->lrTailParticule; part++ )
	{
	int		off;

	tp= paraNode->biParaParticules+ part;

	if  ( tp->tpKind != TPkindSPAN )
	    { visible += tp->tpWide; continue; }

	for ( off= tp->tpStroff; off < tp->tpStroff+ tp->tpStrlen; off++ )
	    {
	    const char *	partString= paraString+ tp->tpStroff- stroff0;

	    if  ( paraString[off- stroff0] == sl->slDecimalPoint )
		{
		DocumentRectangle	drText;
		int			dw;

		dw= docLayoutTextExtentsInParticule( &drText, lc,
			paraNode, part, partString, off- tp->tpStroff );
		if  ( dw < 0 )
		    { LLDEB(off- tp->tpStroff,dw); return -1;	}

		visibleToDec= visible+ dw;
		}
	    }

	visible += tp->tpWide;
	}

    return visibleToDec;
    }

/************************************************************************/
/*									*/
/*  Align a strech of text to the tab before it.			*/
/*									*/
/*  Left tabs have been taken care of by the regular line formatter.	*/
/*									*/
/*  The other tab alignments have been temporarily ignored by the line	*/
/*  formatter: The text after the tab is formatted as if it immediately	*/
/*  came from the position before the tab. Here we have the two X	*/
/*  intervals and we shift the text to make the intervals align in the	*/
/*  desired way.							*/
/*									*/
/*  1)  Negative shifts occur when the text to after the tab does not	*/
/*	does not fit in the X interval of the tab. If this is the case,	*/
/*	we ignore the tab: The text is not shifted and the tab gets	*/
/*	zero width.							*/
/*									*/
/************************************************************************/

int docLayoutAlignTextToTab(	LineRun *		lrTxt,
				const LayoutContext *	lc,
				ParticuleData *		pdTxt,
				ParticuleData *		pdTab,
				const TextParticule *	tpTab,
				int			tabKind,
				const LineRun *		lrTab )
    {
    int			visibleSinceTab= lrTxt->lrVisibleX1;
    int			shift= 0;

    int			done= lrTxt->lrTailParticule- lrTxt->lrHeadParticule;

    switch( tabKind )
	{
	case DOCtaLEFT:
	    if  ( done > 0 && tpTab && tpTab->tpKind == TPkindTAB )
		{ pdTxt[0].pdFlags |= TPflagXMATCH_LEFT;	}
	    shift= 0;
	    break;

	case DOCtaDECIMAL:
	    if  ( done > 0 && pdTab )
		{
		int			visibleToDec;

		visibleToDec= docLayoutDecWidth( lrTxt, lc, visibleSinceTab );
		if  ( visibleToDec < 0 )
		    { LDEB(visibleToDec); return -1;	}

		shift= lrTab->lrRectangle.drX1- visibleToDec;
		if  ( shift < 0 )
		    { LDEB(shift); shift= 0;	}

		pdTab->pdTwipsWide= shift;
		pdTab->pdVisibleBBox.drX0= 0;
		pdTab->pdVisibleBBox.drX1= 0;
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	case DOCtaRIGHT:

	    if  ( done > 0 && pdTab )
		{
		pdTxt[done-1].pdFlags |= TPflagXMATCH_RIGHT;

		/*  6  */
		shift= lrTab->lrRectangle.drX1- visibleSinceTab;
		if  ( shift < 0 )
		    { LDEB(shift); shift= 0;	}

		pdTab->pdTwipsWide= shift;
		pdTab->pdVisibleBBox.drX0= 0;
		pdTab->pdVisibleBBox.drX1= 0;
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	case DOCtaCENTER:
	    if  ( done > 0 && pdTab )
		{
		int	visibleX0= pdTxt[0].pdX0+ pdTxt[0].pdVisibleBBox.drX0;

		/*  6  */
		shift= lrTab->lrRectangle.drX1- 
					    visibleSinceTab/ 2- visibleX0/ 2;

		if  ( shift < 0 )
		    { LDEB(shift); shift= 0;	}
		pdTab->pdTwipsWide= shift;
		pdTab->pdVisibleBBox.drX0= 0;
		pdTab->pdVisibleBBox.drX1= 0;
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	default:
	    LDEB(tabKind);
	    shift= 0; break;
	}

    /*  1  */
    if  ( shift > 0 )
	{
	int		i;

	for ( i= 0; i < done; i++ )
	    { pdTxt[i].pdX0 += shift; }

	lrTxt->lrRectangle.drX0 += shift;
	lrTxt->lrRectangle.drX1 += shift;
	}

    return 0;
    }

