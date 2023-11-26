/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout inside lies of text.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayoutLine.h"
#   include	"layoutContext.h"
#   include	<docPropVal.h>
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	<docParaProperties.h>
#   include	"docParticuleData.h"
#   include	<docStripFrame.h>
#   include	<docTextLine.h>
#   include	<docParaNodeProperties.h>
#   include	<docListLevel.h>
#   include	"docLayout.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>


static const int LINEDISTANCE= 115;

/************************************************************************/

static int docLayoutLineVerticalGeometry(
				DocumentRectangle *		drLine,
				const LineRun *			lr )
    {
    const struct BufferItem *	paraNode= lr->lrParaNode;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    DocumentRectangle		drTypicalLine;

    int				lineStride;

    drTypicalLine= lr->lrRectangle;
    drTypicalLine.drY0= paraNode->biParaMajorityFontAscY0;
    drTypicalLine.drY1= paraNode->biParaMajorityFontDescY1;

    geoIncludeRectangleY( drLine, &drTypicalLine );

    /*LINEDISTANCE*/
    {
    int				majorityFontSizeTwips;

    majorityFontSizeTwips= 10* paraNode->biParaMajorityFontSize;
    lineStride= ( LINEDISTANCE* majorityFontSizeTwips )/ 100;
    }

    if  ( pp->ppLineSpacingTwips < 0 )
	{
	int		fixedStride= -pp->ppLineSpacingTwips;
	int		heightLeft= fixedStride- drLine->drY1+ drLine->drY0;

	drLine->drY0 -= heightLeft;
	lineStride= fixedStride;
	}
    else{
	drLine->drY0 -= lr->lrFattestBorder;
	drLine->drY1 += lr->lrFattestBorder;
	lineStride += 2* lr->lrFattestBorder;

	if  ( drLine->drY0 < drTypicalLine.drY0 )
	    { lineStride += drTypicalLine.drY0- drLine->drY0; }
	if  ( drLine->drY1 > drTypicalLine.drY1 )
	    { lineStride += drLine->drY1- drTypicalLine.drY1; }

	if  ( pp->ppLineSpacingTwips > 0		&&
	      lineStride < pp->ppLineSpacingTwips	)
	    {
	    int		fixedStride= pp->ppLineSpacingTwips;
	    int		heightLeft= fixedStride- drLine->drY1+ drLine->drY0;

	    if  ( pp->ppLineSpacingIsMultiple )
		{ drLine->drY1 += heightLeft;	}
	    else{ drLine->drY0 -= heightLeft;	}

	    lineStride= fixedStride;
	    }
	}

    return lineStride;
    }

/************************************************************************/

static void docLeftAlignLine(	int *				pXShift,
				LineLayoutJob *			llj,
				const DocumentRectangle *	drAdvanced,
				const DocumentRectangle *	drVisible )
    {
    llj->lljParticuleData->pdFlags |= TPflagXMATCH_LEFT;

    llj->lljLineRectangle.drX0= drAdvanced->drX0;
    llj->lljLineRectangle.drX1= drVisible->drX1;

    *pXShift= 0;
    }

static void docRightAlignLine(	int *				pXShift,
				LineLayoutJob *			llj,
				const DocumentRectangle *	drAdvanced,
				const DocumentRectangle *	drVisible )
    {
    int			visibleWidth= drVisible->drX1- drVisible->drX0;

    llj->lljParticuleData[llj->lljAccepted- 1].pdFlags |= TPflagXMATCH_RIGHT;

    llj->lljLineRectangle.drX1= llj->lljX1;
    llj->lljLineRectangle.drX0= llj->lljX1- visibleWidth;

    *pXShift= llj->lljX1- drVisible->drX1;
    }

static void docJustifyLine(	int *				pXShift,
				LineLayoutJob *			llj,
				const DocumentRectangle *	drAdvanced,
				const DocumentRectangle *	drVisible )
    {
    llj->lljParticuleData->pdFlags |= TPflagXMATCH_LEFT;
    llj->lljParticuleData[llj->lljAccepted- 1].pdFlags |=
					    TPflagXMATCH_RIGHT;

    docJustifyParticules( llj->lljParaNode, llj->lljTextParticules,
		    llj->lljParticuleData, llj->lljAccepted,
		    drVisible->drX1, llj->lljX1 );

    llj->lljLineRectangle.drX0= llj->lljX0;
    llj->lljLineRectangle.drX1= llj->lljX1;

    *pXShift= 0;
    }

static void docJustifiedLine(	int *				pXShift,
				LineLayoutJob *			llj,
				const DocumentRectangle *	drAdvanced,
				const DocumentRectangle *	drVisible )
    {
    if  ( llj->lljFound == PSfoundLINEFULL )
	{ docJustifyLine( pXShift, llj, drAdvanced, drVisible );	}
    else{
	if  ( llj->lljParaNode->biParaProperties->ppRToL )
	    { docRightAlignLine( pXShift, llj, drAdvanced, drVisible );	}
	else{ docLeftAlignLine( pXShift, llj, drAdvanced, drVisible );	}
	}
    }

static void docCenterLine(	int *				pXShift,
				LineLayoutJob *			llj,
				const DocumentRectangle *	drAdvanced,
				const DocumentRectangle *	drVisible )
    {
    int				xShift;

    int				center= ( llj->lljX0+ llj->lljX1 )/ 2;
    int				hwidth= ( drVisible->drX1- drVisible->drX0 )/ 2;

    /* Is where we start drawing */
    llj->lljLineRectangle.drX0= center- hwidth-
				    ( drVisible->drX0- drAdvanced->drX0 );
    llj->lljLineRectangle.drX1= llj->lljLineRectangle.drX0+ 2* hwidth;

    xShift= llj->lljLineRectangle.drX0- llj->lljX0;

    *pXShift= xShift;
    }

/************************************************************************/
/*									*/
/*  Perform geometry calculations once the text that belongs to a line	*/
/*  is determined.							*/
/*									*/
/*  The way to determine the line stride was found out by printing text	*/
/*  on a piece of paper with a pica grid as a background.		*/
/*									*/
/************************************************************************/

int docLayoutLineGeometry(	LineLayoutJob *		llj,
				const LineRun *		lr )
    {
    DocumentRectangle		drAdvanced= lr->lrRectangle;
    DocumentRectangle		drVisible;

    int				lineStride;
    int				xShift;

    int				p;
    int				prevFlags;

    lineStride= docLayoutLineVerticalGeometry( &drAdvanced, lr );

    /*  B  */
    llj->lljLineRectangle.drY0= drAdvanced.drY0;
    llj->lljLineRectangle.drY1= drAdvanced.drY1;
    llj->lljLineStride= lineStride;

    drVisible= drAdvanced;
    {
    ParticuleData *	pdd= llj->lljParticuleData+ llj->lljAccepted- 1;

    drVisible.drX0= drAdvanced.drX0+ llj->lljParticuleData->pdVisibleBBox.drX0;
    drVisible.drX1= pdd->pdX0+ pdd->pdVisibleBBox.drX1;
    }

    switch( llj->lljParaNode->biParaProperties->ppAlignment )
	{
	case DOCthaLEFT:
	    docLeftAlignLine( &xShift, llj, &drAdvanced, &drVisible );
	    break;

	case DOCthaRIGHT:
	    docRightAlignLine( &xShift, llj, &drAdvanced, &drVisible );
	    break;

	case DOCthaCENTERED:
	    docCenterLine( &xShift, llj, &drAdvanced, &drVisible );
	    break;

	case DOCthaJUSTIFIED:
	    docJustifiedLine( &xShift, llj, &drAdvanced, &drVisible );
	    break;

	default:
	    LDEB(llj->lljParaNode->biParaProperties->ppAlignment);
	    docLeftAlignLine( &xShift, llj, &drAdvanced, &drVisible );
	    break;
	}

    if  ( xShift != 0 )
	{
	ParticuleData *	pdd= llj->lljParticuleData;

	for ( p= 0; p < llj->lljAccepted; pdd++, p++ )
	    { pdd->pdX0 += xShift;	}
	}

    prevFlags= 0x0;
    for ( p= 0; p < llj->lljAccepted; p++ )
	{
	ParticuleData *	pdd= &(llj->lljParticuleData[p]);

	if  ( prevFlags & TPflagRIGHT_BORDER )
	    { pdd->pdFlags |= TPflagXMATCH_LEFT;	}

	if  ( pdd->pdTwipsWide < 0 )
	    {
	    LDEB(pdd->pdTwipsWide);
	    llj->lljTextParticules[p].tpWide= 0;
	    }
	else{
	    llj->lljTextParticules[p].tpWide= pdd->pdTwipsWide;
	    }

	llj->lljTextParticules[p].tpFlags= pdd->pdFlags;

	prevFlags= pdd->pdFlags;
	}

    return 0;
    }

/************************************************************************/

void docLineTextRectangleTwips(	struct DocumentRectangle *	drLineTwips,
				const struct LayoutPosition *	lpTop,
				const struct LayoutPosition *	lpBottom,
				const struct TextLine *		tl,
				const struct ParagraphFrame *	pf )
    {
    drLineTwips->drY0= lpTop->lpPageYTwips;
    drLineTwips->drY1= lpBottom->lpPageYTwips;

    drLineTwips->drX0= pf->pfCellContentRect.drX0+ tl->tlRectangle.drX0;
    drLineTwips->drX1= pf->pfCellContentRect.drX0+ tl->tlRectangle.drX1;

    return;
    }

/************************************************************************/

void docLayoutLineJobGeometry(	LineLayoutJob *			llj,
				int				firstLine,
				const ParagraphFrame *		pf )
    {
    struct BufferDocument *	bd= llj->lljLayoutContext->lcDocument;
    const BufferItem *		paraNode= llj->lljParaNode;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    llj->lljParagraphFrame= pf;

    llj->lljX1= pf->pfParaContentRect.drX1- pf->pfCellContentRect.drX0;
    llj->lljLineWidth= PARA_FRAME_WIDTH( pf );

    llj->lljBulletTabX= 0;
    if  ( pp->ppFirstIndentTwips < 0 )
	{ llj->lljBulletTabX= pp->ppLeftIndentTwips;	}

    geoInitRectangle( &(llj->lljLineRectangle) );
    llj->lljLineStride= 0;

    if  ( firstLine )
	{
	if  ( pp->ppListOverride > 0 )
	    {
	    struct ListOverride *	lo;
	    struct DocumentList *	dl;
	    const ListLevel *		ll;

	    int * const			startPath= (int *)0;
	    int * const			formatPath= (int *)0;

	    if  ( docGetListLevelOfParagraph( startPath, formatPath,
						&lo, &dl, &ll, paraNode, bd ) )
		{ LLDEB(pp->ppListOverride,pp->ppListLevel);	}

	    llj->lljListBulletJustification= ll->llJustification;
	    llj->lljListBulletFollow= ll->llFollow;
	    }

	if  ( pp->ppRToL )
	    {
	    llj->lljX0= pp->ppRightIndentTwips- pp->ppFirstIndentTwips;

	    llj->lljContentX0= pf->pfParaContentRect.drX0;
	    /*
	    llj->lljContentX1= pf->pfParaContentRect.drX1-
						pp->ppFirstIndentTwips;
	    */
	    }
	else{
	    llj->lljX0= pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips;

	    llj->lljContentX0= pf->pfParaContentRect.drX0+ 
						pp->ppFirstIndentTwips;
	    /*
	    llj->lljContentX1= pf->pfParaContentRect.drX1;
	    */
	    }
	}
    else{
        llj->lljX0= pp->ppLeftIndentTwips;

	llj->lljContentX0= pf->pfParaContentRect.drX0;
	/*
	llj->lljContentX1= pf->pfParaContentRect.drX1;
	*/
	}

    return;
    }
