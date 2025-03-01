/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<ctype.h>

#   include	"docEditSetProperties.h"
#   include	"docEditOperation.h"
#   include	<docRowNodeProperties.h>
#   include	<docTreeNode.h>
#   include	<docRowProperties.h>
#   include	<utilPropMask.h>
#   include	<docCellProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of a row that we encounter on the way.	*/
/*									*/
/************************************************************************/

int docEditChangeRowProperties(	SetProperties *			setProps,
				struct BufferItem *		rowNode,
				const PropertyMask *		rpSetMask )
    {
    struct BufferDocument *	bd= setProps->spEditOperation->eoDocument;

    PropertyMask	rpDoneMask;

    const RowProperties *	rpSet= &(setProps->spCurrentRowProperties);
    int				colTo= 0;
    int				colCount= rpSet->rpCellCount;

    const struct DocumentAttributeMap * dam0= (const struct DocumentAttributeMap *)0;

    /* prevent crashes, and other inconsistencies */
    if  ( PROPmaskISSET( rpSetMask, RPprop_CELL_COUNT ) )
	{ LDEB(RPprop_CELL_COUNT); return -1;	}

    utilPropMaskClear( &rpDoneMask );

    /* Do not map attributes here: Are in 'to' format */
    if  ( docChangeRowNodeProperties( &rpDoneMask, (PropertyMask *)0, rowNode,
			    rpSetMask, setProps->sp_cpSetMask, rpSet,
			    colTo, colCount, bd, dam0 ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilPropMaskIsEmpty( &rpDoneMask ) )
	{
	if  ( setProps->sp_rpDoneMask )
	    {
	    utilPropMaskOr( setProps->sp_rpDoneMask,
				setProps->sp_rpDoneMask, &rpDoneMask );
	    }

	docEditIncludeNodeInReformatRange( setProps->spEditOperation,
								rowNode );

	if  ( PROPmaskISSET( &rpDoneMask, RPpropIS_TABLE_HEADER ) )
	    {
	    BufferItem *	lastRowNode;

	    lastRowNode= rowNode->biParent->biChildren[
						rowNode->biRowTablePast- 1];

	    if  ( docCompareLayoutPositionFrames(
					&(lastRowNode->biBelowPosition), 
					&(rowNode->biTopPosition) ) > 0 )
		{
		docEditIncludeNodeInReformatRange( setProps->spEditOperation,
								lastRowNode );
		}

	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Apply row-and cell properties.					*/
/*									*/
/************************************************************************/

int docEditSetRowNodeProperties(	struct BufferItem *	rowNode,
					SetProperties *		setProps )
    {
    const RowProperties *	oldRp= rowNode->biRowProperties;
    const struct DocumentAttributeMap * dam0= (const struct DocumentAttributeMap *)0;

    int	colTo= setProps->spTableRectangle.trCol0;
    int	colCount= setProps->spTableRectangle.trCol1- colTo+ 1;

    PropertyMask		rpSetMask;

    utilPropMaskClear( &rpSetMask );
    if  ( setProps->sp_rpSetMask )
	{ rpSetMask= *setProps->sp_rpSetMask;	}

    /* Do not map attributes here: Are in 'to' format */
    if  ( docCopyRowProperties( &(setProps->spCurrentRowProperties),
							    oldRp, dam0 ) )
	{ LDEB(oldRp->rpCellCount); return -1;	}

    /* Map attributes from 'from' to 'to' format */
    if  ( setProps->sp_rpSetMask )
	{
	if  ( ! setProps->sp_rpSet )
	    { XDEB(setProps->sp_rpSet); return -1;	}

	if  ( docUpdRowProperties(
			(struct PropertyMask *)0, (struct PropertyMask *)0,
			&(setProps->spCurrentRowProperties),
			setProps->sp_rpSetMask, setProps->sp_cpSetMask,
			setProps->sp_rpSet, colTo, colCount,
			setProps->spAttributeMap ) )
	    { LDEB(oldRp->rpCellCount); return -1;	}
	}

    if  ( setProps->sp_cpSetMask )
	{
	int			col;

	int			h0= -1;
	int			h1= -1;
	int			h= -1;
	int			v0= -1;
	int			v1= -1;
	int			v= -1;

	CellProperties *	cpTo;

	if  ( ! setProps->sp_cpSet )
	    { XDEB(setProps->sp_cpSet); return -1;	}

	PROPmaskADD( &rpSetMask, RPprop_CELL_PROPS );

	if  ( PROPmaskISSET( setProps->sp_cpSetMask, CLpropHOR_MERGE ) )
	    {
	    switch( setProps->sp_cpSet->cpHorizontalMerge )
		{
		case CELLmergeHEAD:
		    h0= CELLmergeHEAD;
		    h1= CELLmergeFOLLOW;
		    break;
		case CELLmergeNONE:
		    h0= CELLmergeNONE;
		    h1= CELLmergeNONE;
		    break;
		default:
		    LDEB(setProps->sp_cpSet->cpHorizontalMerge);
		    break;
		}
	    }

	if  ( PROPmaskISSET( setProps->sp_cpSetMask, CLpropVERT_MERGE ) )
	    {
	    switch( setProps->sp_cpSet->cpVerticalMerge )
		{
		case CELLmergeHEAD:
		    v0= CELLmergeHEAD;
		    v1= CELLmergeFOLLOW;
		    break;
		case CELLmergeNONE:
		    v0= CELLmergeNONE;
		    v1= CELLmergeNONE;
		    break;
		default:
		    LDEB(setProps->sp_cpSet->cpVerticalMerge);
		    break;
		}
	    }

	if  ( rowNode->biNumberInParent == setProps->spTableRectangle.trRow0 )
	    { v= v0;	}
	else{ v= v1;	}

	h= h0;

	cpTo= &(setProps->spCurrentRowProperties.rpCells[colTo]);
	for ( col= colTo; col < colTo+ colCount; cpTo++, col++ )
	    {
	    /* Map attributes from 'from' to 'to' format */
	    if  ( docUpdCellProperties( (struct PropertyMask *)0, cpTo,
			setProps->sp_cpSetMask, setProps->sp_cpSet,
			setProps->spAttributeMap ) )
		{ LDEB(1); return -1;	}

	    if  ( v >= 0 )
		{ cpTo->cpVerticalMerge= v;	}
	    if  ( h >= 0 )
		{ cpTo->cpHorizontalMerge= h;	}

	    h= h1;
	    }
	}

    /* prevent crashes, and other inconsistencies */
    if  ( PROPmaskISSET( &rpSetMask, RPprop_CELL_COUNT ) )
	{
	LDEB(RPprop_CELL_COUNT);
	PROPmaskUNSET( &rpSetMask, RPprop_CELL_COUNT );
	}

    if  ( docEditChangeRowProperties( setProps, rowNode, &rpSetMask ) )
	{ LDEB(rowNode->biLevel); return -1;	}

    return 0;
    }

