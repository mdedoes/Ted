/************************************************************************/
/*									*/
/*  Document Rulers.							*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<docPageGrid.h>
#   include	<geoGrid.h>

#   include	<docTreeNode.h>
#   include	<docTabStopList.h>
#   include	<docRowProperties.h>
#   include	<docParaProperties.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docRulerColumnSeparator.h>
#   include	<docAttributes.h>
#   include	"docScreenRuler.h"
#   include	<docTabStop.h>
#   include	<docCellProperties.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Obtain the separator positions for a table.				*/
/*									*/
/************************************************************************/

int docRowGetRulerColumnsPixels( int *				pX0Columns,
				int *				pX1Columns,
				ColumnSeparator **		pCs,
				int *				pCsCount,
				const struct BufferItem *	rowNode,
				const LayoutContext *		lc,
				const BlockFrame *		bf )
    {
    double			xfac= lc->lcPixelsPerTwip;
    ColumnSeparator *		cs;

    const RowProperties *	rp= rowNode->biRowProperties;

    int				sep;

    ParagraphFrame		pf;


    cs= (ColumnSeparator *)malloc( ( rowNode->biChildCount+ 1 )*
						sizeof( ColumnSeparator ) );
    if  ( ! cs )
	{ LXDEB(rowNode->biChildCount,cs); return -1;	}

    docCellFrameTwips( &pf, bf, rowNode->biChildren[0] );

    if  ( rp->rpRToL )
	{
	int	x1= pf.pfParentContentRect.drX1- rp->rpLeftIndentTwips;

	x1= 2* pf.pfCellRect.drX1- pf.pfCellContentRect.drX1;

	cs[0].csX1= COORDtoGRID( xfac, x1 );
	}
    else{
	int	x0= pf.pfParentContentRect.drX0+ rp->rpLeftIndentTwips;

	x0= 2* pf.pfCellRect.drX0- pf.pfCellContentRect.drX0;

	cs[0].csX0= COORDtoGRID( xfac, x0 );
	}

    for ( sep= 0; sep < rowNode->biChildCount; sep++ )
	{
	if  ( rp->rpRToL )
	    {
	    cs[sep+0].csX0= COORDtoGRID( xfac, pf.pfCellContentRect.drX1 );
	    cs[sep+1].csX1= COORDtoGRID( xfac, pf.pfCellContentRect.drX0 );
	    }
	else{
	    cs[sep+0].csX1= COORDtoGRID( xfac, pf.pfCellContentRect.drX0 );
	    cs[sep+1].csX0= COORDtoGRID( xfac, pf.pfCellContentRect.drX1 );
	    }

	if  ( sep+ 1 >= rowNode->biChildCount )
	    { break;	}

	docCellFrameTwips( &pf, bf, rowNode->biChildren[sep+ 1] );
	}

    sep= rowNode->biChildCount;

    if  ( rp->rpRToL )
	{
	int	x0= pf.pfParentContentRect.drX0;

	x0= 2* pf.pfCellRect.drX0- pf.pfCellContentRect.drX0;

	cs[sep+0].csX0= COORDtoGRID( xfac, x0 );
	}
    else{
	int	x1= pf.pfParentContentRect.drX1;

	x1= 2* pf.pfCellRect.drX1- pf.pfCellContentRect.drX1;

	cs[sep+0].csX1= COORDtoGRID( xfac, x1 );
	}

    *pX0Columns= COORDtoGRID( xfac, bf->bfContentRect.drX0 );
    *pX1Columns= COORDtoGRID( xfac, bf->bfContentRect.drX1 );

    *pCs= cs; *pCsCount= rowNode->biChildCount+ 1;
    return 0;
    }

int docRowGetRulerColumnTwips(	RowProperties *		rpSet,
				const LayoutContext *	lc,
				int			sep,
				const ColumnSeparator *	csOld,
				const ColumnSeparator *	csNew )
    {
    double		xfac= lc->lcPixelsPerTwip;

    if  ( sep == 0 )
	{
	int	leftPix;
	int	leftTwips;

	leftPix= COORDtoGRID( xfac, rpSet->rpLeftIndentTwips );

	leftPix += ( csNew[0].csX0- csOld[0].csX0 );
	leftTwips= GRIDtoCOORD( leftPix, xfac );

	rpSet->rpCells[0].cpWide -= ( leftTwips- rpSet->rpLeftIndentTwips );
	rpSet->rpLeftIndentTwips= leftTwips;
	}
    else{
	int	oldRightTwips;
	int	rightPix;
	int	rightTwips;
	int	dTwips;

	if  ( docRowGetColumnX( (int *)0, &oldRightTwips, (int *)0, (int *)0,
							    rpSet, sep- 1 ) )
	    { LDEB(sep-1); return -1;	}

	rightPix= COORDtoGRID( xfac, oldRightTwips );

	rightPix += ( csNew[sep].csX0- csOld[sep].csX0 );
	rightTwips= GRIDtoCOORD( rightPix, xfac );
	dTwips= rightTwips- oldRightTwips;

	rpSet->rpCells[sep-1].cpWide += dTwips;
	}

    return 0;
    }

/************************************************************************/

static void docRulerTabPixels(	TabStop *			tsPixels,
				const ParagraphProperties *	pp,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				const TabStop *			tsTwips )
    {
    double	xfac= lc->lcPixelsPerTwip;

    *tsPixels= *tsTwips;

    if  ( pp->ppRToL )
	{
	int	pfX1Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX1 );

	tsPixels->tsOffset= pfX1Pixels- COORDtoGRID( xfac, tsTwips->tsOffset );
	}
    else{
	int	pfX0Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX0 );

	tsPixels->tsOffset= pfX0Pixels+ COORDtoGRID( xfac, tsTwips->tsOffset );
	}

    return;
    }

static void docRulerTabTwips(	TabStop *			tsTwips,
				const ParagraphProperties *	pp,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				const TabStop *			tsPixels )
    {
    double	xfac= lc->lcPixelsPerTwip;

    *tsTwips= *tsTwips;

    if  ( pp->ppRToL )
	{
	int	pfX1Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX1 );

	tsTwips->tsOffset= GRIDtoCOORD( pfX1Pixels- tsPixels->tsOffset, xfac );
	}
    else{
	int	pfX0Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX0 );

	tsTwips->tsOffset= GRIDtoCOORD( tsPixels->tsOffset- pfX0Pixels, xfac );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A new series of tab stops is returned from the ruler. It is the	*/
/*  result of some user action on the current ruler. Merge the changes	*/
/*  into the current ruler.						*/
/*									*/
/*  In all practical situaltions, exactly one tab is moved, OR exactly	*/
/*  one tab is added OR exactly one tab is deleted. This routine is	*/
/*  written to handle any set of changes, but it tries to change as few	*/
/*  positions in twips as possible: Pixels are less precise and		*/
/*  documents would change all the time because tabs have moved a few	*/
/*  invisible twips that originate from rounding to pixels on the ruler.*/
/*									*/
/*  NOTE that the pixel coordinates are geometric coordinates and that	*/
/*  the twips coordinates are offsets from the paragraph frame. This	*/
/*  means that in right-to-left paragraphs the two tab stop lists have	*/
/*  opposite directions.						*/
/*									*/
/************************************************************************/

static int docRulerMergePixelTabs(
				TabStopList *			tslTwips,
				const ParagraphProperties *	pp,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				const TabStopList *		tslPixels )
    {
    int			rval= 0;
    TabStop *		fresh= (TabStop *)0;

    if  ( tslPixels->tslTabStopCount > 0 )
	{
	int		pix= 0;
	int		twi;
	int		tstep;

	if  ( pp->ppRToL )
	    {
	    twi= tslTwips->tslTabStopCount- 1;
	    tstep= -1;
	    }
	else{
	    twi= 0;
	    tstep= +1;
	    }

	fresh= (TabStop *)malloc( tslPixels->tslTabStopCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(tslPixels->tslTabStopCount,fresh); rval= -1; goto ready; }

	while( pix < tslPixels->tslTabStopCount )
	    {
	    /*
	     *  Keep twips tab stops that result in identical pixel values.
	     */
	    while( pix < tslPixels->tslTabStopCount		&&
		   twi < tslTwips->tslTabStopCount		&&
		   twi >= 0					)
		{
		TabStop	tsTwipsToPixels;

		docRulerTabPixels( &tsTwipsToPixels, pp, pf, lc,
					&(tslTwips->tslTabStops[twi]) );

		if  ( tsTwipsToPixels.tsOffset !=
					tslPixels->tslTabStops[pix].tsOffset )
		    { break;	}

		fresh[pix]= tslTwips->tslTabStops[twi];
		pix++; twi += tstep;
		}

	    /*
	     *  Skip twips tab stops result in lower pixel values.
	     *  (They will be removed.)
	     */
	    while( pix < tslPixels->tslTabStopCount		&&
		   twi < tslTwips->tslTabStopCount		&&
		   twi >= 0					)
		{
		TabStop	tsTwipsToPixels;

		docRulerTabPixels( &tsTwipsToPixels, pp, pf, lc,
					&(tslTwips->tslTabStops[twi]) );

		if  ( tsTwipsToPixels.tsOffset >=
					tslPixels->tslTabStops[pix].tsOffset )
		    { break;	}

		twi += tstep;
		}

	    /*
	     *  Copy (I.E insert) pixel tab stops new with lower pixel values 
	     *  than the result of the current twips tab stop.
	     *  (Or at tail)
	     */
	    while( pix < tslPixels->tslTabStopCount )
		{
		if  ( twi < tslTwips->tslTabStopCount	&&
		      twi >= 0				)
		    {
		    TabStop	tsTwipsToPixels;

		    docRulerTabPixels( &tsTwipsToPixels, pp, pf, lc,
					    &(tslTwips->tslTabStops[twi]) );

		    if  ( tsTwipsToPixels.tsOffset <=
					tslPixels->tslTabStops[pix].tsOffset )
			{ break;	}
		    }

		docRulerTabTwips( &(fresh[pix]), pp, pf, lc,
					&(tslPixels->tslTabStops[pix]) );
		pix++;
		}
	    }

	if  ( pp->ppRToL )
	    {
	    int	fr;
	    int	to= tslPixels->tslTabStopCount- 1;

	    for ( fr= 0; fr < tslPixels->tslTabStopCount/ 2; fr++, to-- )
		{
		TabStop	sw= fresh[fr];
				fresh[fr]= fresh[to];
					   fresh[to]= sw;
		}
	    }

	}

    /*  steal by swapping */
    {
    TabStop * sw= tslTwips->tslTabStops;
		  tslTwips->tslTabStops= fresh;
		                         fresh= sw;
    }

    tslTwips->tslTabStopCount= tslPixels->tslTabStopCount;

  ready:

    if  ( fresh )
	{ free( fresh );	}

    return rval;
    }

int docParaGetTabStopsPixels(	TabStopList *			tslPixels,
				const ParagraphProperties *	pp,
				const ParagraphFrame *		pf,
				const LayoutContext *		lc,
				const TabStopList *		tslTwips )
    {
    int			rval= 0;
    TabStop *		fresh= (TabStop *)0;

    if  ( tslTwips->tslTabStopCount > 0 )
	{
	int		twi= 0;
	int		pix;
	int		pstep;

	if  ( pp->ppRToL )
	    {
	    pix= tslTwips->tslTabStopCount- 1;
	    pstep= -1;
	    }
	else{
	    pix= 0;
	    pstep= +1;
	    }

	fresh= (TabStop *)malloc( tslTwips->tslTabStopCount* sizeof(TabStop) );
	if  ( ! fresh )
	    { LXDEB(tslTwips->tslTabStopCount,fresh); rval= -1; goto ready; }

	while( twi < tslTwips->tslTabStopCount )
	    {
	    docRulerTabPixels( &(fresh[pix]), pp, pf, lc,
					    &(tslTwips->tslTabStops[twi]) );
	    twi++; pix += pstep;
	    }
	}

    /*  steal by swapping */
    {
    TabStop * sw= tslPixels->tslTabStops;
		  tslPixels->tslTabStops= fresh;
		                          fresh= sw;
    }

    tslPixels->tslTabStopCount= tslTwips->tslTabStopCount;

  ready:

    if  ( fresh )
	{ free( fresh );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  The user wants something with the mouse on the horizontal ruler	*/
/*									*/
/************************************************************************/

int docParaGetRulerTwips(
			const LayoutContext *		lc,
			const BlockFrame *		bf,
			const ParagraphFrame *		pf,
			struct BufferDocument *		bd,
			ParagraphProperties *		ppNew,
			int				firstIndentPixels,
			int				leftIndentPixels,
			int				rightIndentPixels,
			const TabStopList *		tslPixels )
    {
    int				rval= 0;
    double			xfac= lc->lcPixelsPerTwip;

    int				pfX0Pixels;
    int				pfX1Pixels;

    TabStopList			tslTwips;

    docInitTabStopList( &tslTwips );

    pfX0Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX0 );
    pfX1Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX1 );

    leftIndentPixels -= pfX0Pixels;
    ppNew->ppLeftIndentTwips= GRIDtoCOORD( leftIndentPixels, xfac );

    rightIndentPixels= pfX1Pixels- rightIndentPixels;
    ppNew->ppRightIndentTwips= GRIDtoCOORD( rightIndentPixels, xfac );

    if  ( ppNew->ppRToL )
	{
	firstIndentPixels= pfX1Pixels- firstIndentPixels;
	firstIndentPixels -= rightIndentPixels;
	ppNew->ppFirstIndentTwips= GRIDtoCOORD( firstIndentPixels, xfac );
	}
    else{
	firstIndentPixels -= pfX0Pixels;
	firstIndentPixels -= leftIndentPixels;
	ppNew->ppFirstIndentTwips= GRIDtoCOORD( firstIndentPixels, xfac );
	}

    if  ( docCopyTabStopList( &tslTwips,
	    docGetTabStopListByNumber( bd, ppNew->ppTabStopListNumber ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRulerMergePixelTabs( &tslTwips, ppNew, pf, lc, tslPixels ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ppNew->ppTabStopListNumber= docTabStopListNumber( bd, &tslTwips );

  ready:

    docCleanTabStopList( &tslTwips );

    return rval;
    }

int docParaGetRulerPixels(
			int *				pParaX0Pixels,
			int *				pParaX1Pixels,
			int *				pFirstIndentPixels,
			int *				pLeftIndentPixels,
			int *				pRightIndentPixels,
			const LayoutContext *		lc,
			const ParagraphProperties *	pp,
			const ParagraphFrame *		pf )
    {
    double		xfac= lc->lcPixelsPerTwip;

    int			leftIndentPixels;
    int			firstIndentPixels;
    int			rightIndentPixels;

    int			paraX0Pixels;
    int			paraX1Pixels;

    leftIndentPixels= COORDtoGRID( xfac, pf->pfParaContentRect.drX0 );
    rightIndentPixels= COORDtoGRID( xfac, pf->pfParaContentRect.drX1 );

    if  ( pp->ppRToL )
	{
	firstIndentPixels= COORDtoGRID( xfac, pf->pfParaContentRect.drX1-
						    pp->ppFirstIndentTwips );
	}
    else{
	firstIndentPixels= COORDtoGRID( xfac, pf->pfParaContentRect.drX0+
						    pp->ppFirstIndentTwips );
	}

    paraX0Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX0 );
    paraX1Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX1 );

    if  ( pParaX0Pixels )
	{ *pParaX0Pixels= paraX0Pixels;	}
    if  ( pParaX1Pixels )
	{ *pParaX1Pixels= paraX1Pixels;	}

    if  ( pFirstIndentPixels )
	{ *pFirstIndentPixels= firstIndentPixels;	}
    if  ( pLeftIndentPixels )
	{ *pLeftIndentPixels= leftIndentPixels;	}
    if  ( pRightIndentPixels )
	{ *pRightIndentPixels= rightIndentPixels;	}

    return 0;
    }

