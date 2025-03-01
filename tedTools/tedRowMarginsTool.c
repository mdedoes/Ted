/************************************************************************/
/*									*/
/*  The Row page of the Format Tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	<geoUnit.h>

#   include	"tedRowMarginsTool.h"
#   include	<docTreeNode.h>
#   include	<docBlockFrame.h>
#   include	<docPageGrid.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docRowProperties.h>
#   include	<docCellProperties.h>
#   include	<docStripFrame.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get the left indentation of the row from the margins tool.		*/
/*									*/
/*  1)  Refuse values that protrude outside the parent on the left	*/
/*	hand side.							*/
/*  2)  Make sure that some space is left inside the first cell of the	*/
/*	row.								*/
/*									*/
/************************************************************************/

static int tedRowToolGetLeftIndent(	int *			pValue,
					RowMarginsTool *	rmt,
					const RowProperties *	rp )
    {
    int				value;
    int				changed;

    const int			minValue= INT_MIN;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    value= rp->rpLeftIndentTwips;
    if  ( guiLengthFromText( rmt->rmtLeftIndentText,
				&value, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    /*  1 */
    if  ( value < -rmt->rmtParentLeftMargin )
	{
	value= -rmt->rmtParentLeftMargin+ 10;

	guiLengthToText( rmt->rmtLeftIndentText, value, UNITtyPOINTS );
	guiRefuseTextValue( rmt->rmtLeftIndentText );
	return 1;
	}

    /*  2 */
    if  ( value+ 2* rp->rpHalfGapWidthTwips >= rp->rpCells[0].cpWide )
	{
	value= rp->rpCells[0].cpWide- 3* rp->rpHalfGapWidthTwips;

	if  ( value < -rmt->rmtParentLeftMargin )
	    { guiRefuseTextValue( rmt->rmtLeftIndentText ); return -1;	}

	guiLengthToText( rmt->rmtLeftIndentText, value, UNITtyPOINTS );
	guiRefuseTextValue( rmt->rmtLeftIndentText );
	return 1;
	}

    *pValue= value; return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the row margins.						*/
/*									*/
/************************************************************************/

void tedRefreshRowMarginsTool(	RowMarginsTool *		rmt,
				const RowProperties *		rp,
				int				canChange )
    {
    guiEnableWidget( rmt->rmtCellMarginRow, canChange );
    guiEnableText( rmt->rmtCellMarginText, canChange );
    guiLengthToText( rmt->rmtCellMarginText,
				    rp->rpHalfGapWidthTwips, UNITtyPOINTS );

    guiEnableWidget( rmt->rmtLeftIndentRow, canChange );
    guiEnableText( rmt->rmtLeftIndentText, canChange );
    guiLengthToText( rmt->rmtLeftIndentText,
				    rp->rpLeftIndentTwips, UNITtyPOINTS );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract margins and indents.					*/
/*									*/
/************************************************************************/

int tedRowMarginToolGetValues(	RowProperties *		rp,
				RowMarginsTool *	rmt )
    {
    int			value;
    int			changed;

    int			res;

    const int		minValue= 0;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;


    value= rp->rpHalfGapWidthTwips;
    if  ( guiLengthFromText( rmt->rmtCellMarginText,
				&value, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    rp->rpHalfGapWidthTwips= value;

    res= tedRowToolGetLeftIndent( &value, rmt, rp );
    if  ( res != 0 )
	{ return -1;	}

    rp->rpLeftIndentTwips= value;

    return 0;
    }

static int tedFormatToolGetGapWidth(	const RowProperties *	rp,
					int *			pValue,
					APP_WIDGET		w )
    {
    int				value;
    int				changed;

    int				minWidth;
    const CellProperties *	cp;
    int				col;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    value= rp->rpHalfGapWidthTwips;
    if  ( guiLengthFromText( w, &value, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    cp= rp->rpCells;
    minWidth= cp->cpWide;

    for ( col= 1; col < rp->rpCellCount; cp++, col++ )
	{
	if  ( minWidth > cp->cpWide )
	    { minWidth=  cp->cpWide;	}
	}

    if  ( 2* value >= minWidth )
	{
	value= minWidth/3;

	guiLengthToText( w, value, UNITtyPOINTS );
	guiRefuseTextValue( w );
	return 1;
	}

    *pValue= value;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Check the cell margin that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

int tedMarginToolCheckCellMargin(	RowMarginsTool *	rmt,
					RowProperties *		rp )
    {
    int			value;

    if  ( ! tedFormatToolGetGapWidth( rp, &value, rmt->rmtCellMarginText ) )
	{
	guiLengthToText( rmt->rmtCellMarginText, value, UNITtyPOINTS );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Check the left indent that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

int tedMarginToolCheckLeftIndent(	RowMarginsTool *	rmt,
					RowProperties *		rp )
    {
    int			value;

    if  ( ! tedRowToolGetLeftIndent( &value, rmt, rp ) )
	{
	guiLengthToText( rmt->rmtLeftIndentText, value, UNITtyPOINTS );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build the row margins controls.					*/
/*									*/
/************************************************************************/

void tedFillRowMarginsTool(
			RowMarginsTool *		rmt,
			const RowMarginsToolResources *	rmtr,
			const int			textColumns,
			APP_TXACTIVATE_CALLBACK_T	leftIndentChanged,
			APP_TXACTIVATE_CALLBACK_T	cellMarginChanged,
			void *				through,
			APP_WIDGET			pageWidget )
    {
    /**************/
    guiToolMakeLabelAndTextRow( &(rmt->rmtLeftIndentRow),
			    &(rmt->rmtLeftIndentLabel),
			    &(rmt->rmtLeftIndentText),
			    pageWidget, rmtr->rmtrLeftIndent, textColumns, 1 );

    guiSetGotValueCallbackForText( rmt->rmtLeftIndentText,
						leftIndentChanged, through );

    /**************/
    guiToolMakeLabelAndTextRow( &(rmt->rmtCellMarginRow),
			    &(rmt->rmtCellMarginLabel),
			    &(rmt->rmtCellMarginText),
			    pageWidget, rmtr->rmtrCellMargin, textColumns, 1 );

    guiSetGotValueCallbackForText( rmt->rmtCellMarginText,
						cellMarginChanged, through );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize the tool.						*/
/*									*/
/************************************************************************/

void tedInitRowMarginsTool(	RowMarginsTool *	rmt )
    {
    rmt->rmtParentLeftMargin= 0;
    rmt->rmtParentWidth= 0;

    rmt->rmtCellMarginRow= (APP_WIDGET)0;
    rmt->rmtCellMarginLabel= (APP_WIDGET)0;
    rmt->rmtCellMarginText= (APP_WIDGET)0;

    rmt->rmtLeftIndentRow= (APP_WIDGET)0;
    rmt->rmtLeftIndentLabel= (APP_WIDGET)0;
    rmt->rmtLeftIndentText= (APP_WIDGET)0;
    }

/************************************************************************/
/*									*/
/*  Determine margins.							*/
/*									*/
/************************************************************************/

int tedRowMarginToolGetMargins(	RowMarginsTool *		rmt,
				struct BufferItem *		rowNode,
				const struct BufferDocument *	bd )
    {
    int			rval= 0;
    BlockFrame		bf;

    docLayoutInitBlockFrame( &bf );
    docBlockFrameTwips( &bf, rowNode, (struct BufferDocument *)bd,
					rowNode->biTopPosition.lpPage,
					rowNode->biTopPosition.lpColumn );

    switch( rowNode->biParent->biLevel )
	{
	case DOClevSECT:
	    {
	    const DocumentGeometry *	dg= &(bf.bfPageGeometry);

	    rmt->rmtParentLeftMargin= dg->dgMargins.roLeftOffset;
	    rmt->rmtParentWidth= dg->dgPageWideTwips+ dg->dgMargins.roRightOffset;
	    }
	    break;

	case DOClevCELL:
	    {
	    ParagraphFrame	pf;

	    docCellFrameTwips( &pf, &bf, rowNode->biParent );

	    rmt->rmtParentLeftMargin=
			    pf.pfCellContentRect.drX0- pf.pfCellRect.drX0;
	    rmt->rmtParentWidth= pf.pfCellRect.drX1;
	    }
	    break;

	default:
	    LDEB(rowNode->biParent->biLevel);
	    rval= -1; goto ready;
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

