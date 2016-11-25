/************************************************************************/
/*									*/
/*  Table editing.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<ctype.h>
#   include	<limits.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	"docEditImpl.h"
#   include	<docTextParticule.h>
#   include	<docNodeTree.h>
#   include	<textAttribute.h>
#   include	<docRowNodeProperties.h>
#   include	<docTreeScanner.h>
#   include	<docRowProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docBuf.h>
#   include	<utilPropMask.h>
#   include	"docEditOperation.h"
#   include	<docCellProperties.h>
#   include	<docBorderProperties.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Step in insert row actions: Copy attributes.			*/
/*									*/
/*  NOTE: Copying the text attribute numbers means that the rows MUST	*/
/*	be in the same document. The same applies for the color numbers	*/
/*	in the paragraph properties.					*/
/*									*/
/************************************************************************/

static int docCopyRowColumnAttributes(	struct BufferItem *		rowNode,
					const struct BufferItem *	refRowNode,
					const struct BufferDocument *	bd )
    {
    int		colCount;
    int		col;

    if  ( rowNode->biChildCount < refRowNode->biChildCount )
	{ colCount= rowNode->biChildCount;	}
    else{ colCount= refRowNode->biChildCount;	}

    for ( col= 0; col < colCount; col++ )
	{
	DocumentPosition	dp;
	DocumentPosition	dpRef;

	TextParticule *		tp;
	const TextParticule *	tpRef;

	PropertyMask		ppSetMask;

	if  ( docHeadPosition( &dp, rowNode->biChildren[col] ) )
	    { continue;	}
	if  ( docHeadPosition( &dpRef, refRowNode->biChildren[col] ) )
	    { continue;	}

	utilPropMaskClear( &ppSetMask );
	utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

	PROPmaskUNSET( &ppSetMask, PPpropLISTOVERRIDE );
	PROPmaskUNSET( &ppSetMask, PPpropOUTLINELEVEL );
	PROPmaskUNSET( &ppSetMask, PPpropLISTLEVEL );

	if  ( docParaNodeSetProperties( dp.dpNode,
			&ppSetMask, dpRef.dpNode->biParaProperties, bd ) )
	    { LDEB(1); return -1;	}

	tp= dp.dpNode->biParaParticules;
	tpRef= dpRef.dpNode->biParaParticules;

	tp->tpTextAttrNr= tpRef->tpTextAttrNr;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a series of rows in a table.					*/
/*									*/
/************************************************************************/

int docInsertTableRows(		DocumentSelection *	dsRows,
				EditOperation *		eo,
				struct BufferItem *		parentNode,
				const struct BufferItem *	refRowNode,
				const RowProperties *	rp,
				int			textAttributeNr,
				int			n,
				int			paraNr,
				int			rows )
    {
    int			row;
    int			row0= n;

    const int		recursively= 0;

    for ( row= 0; row < rows; n++, row++ )
	{
	struct BufferItem *		rowNode= (struct BufferItem *)0;

	const int		sectShift= 0;
	const int		stroffShift= 0;
	const int		paraShift= rp->rpCellCount;

	rowNode= docInsertRowNode( eo->eoDocument, parentNode, n, rp,
						    textAttributeNr );
	if  ( ! rowNode )
	    { XDEB(rowNode); return -1;	}

	if  ( paraNr == 1 )
	    {
	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, 0,
				    sectShift, paraShift, stroffShift );
	    }
	else{
	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr- 1, INT_MAX,
				    sectShift, paraShift, stroffShift );
	    }

	paraNr += rp->rpCellCount;

	if  ( refRowNode						&&
	      docCopyRowColumnAttributes( rowNode, refRowNode, eo->eoDocument )	)
	    { LDEB(1); return -1;	}

	docEditIncludeNodeInReformatRange( eo, rowNode );
	}

    docDelimitTables( parentNode, recursively );

    {
    const int		direction= 1;

    DocumentSelection	dsNew;
    EditRange		erRows;

    if  ( docHeadPosition( &(dsNew.dsHead), parentNode->biChildren[row0] ) )
	{ LDEB(row0);	}
    if  ( docTailPosition( &(dsNew.dsTail), parentNode->biChildren[n-1] ) )
	{ LDEB(n-1);	}

    docSetRangeSelection( dsRows, &(dsNew.dsHead), &(dsNew.dsTail), direction );

    docSetEditRange( &erRows, &dsNew );
    docUnionEditRanges( &(eo->eoAffectedRange),
					&(eo->eoAffectedRange), &erRows );
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static struct BufferItem * docInsertCellInRow(
				struct BufferDocument *	bd,
				struct BufferItem *		rowNode,
				int			col,
				int			after,
				const struct CellProperties *	cp,
				int			textAttributeNr )
    {
    struct BufferItem *	cellNode;
    struct BufferItem *	paraNode;

    cellNode= docInsertNode( bd, rowNode, col+ after, DOClevCELL );
    if  ( ! cellNode )
	{ LXDEB(col,cellNode); return (struct BufferItem *)0;	}

    if  ( docInsertColumnInRowNode( rowNode, col,
			    cp, bd, (const struct DocumentAttributeMap *)0 ) )
	{ LDEB(col); return (struct BufferItem *)0;	}

    paraNode= docAppendParagraph( bd, cellNode, textAttributeNr );
    if  ( ! paraNode )
	{ XDEB(paraNode); return (struct BufferItem *)0;	}

    docSetParaTableNesting( paraNode, bd );

    return paraNode;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

int docSplitColumnInRows(	struct BufferItem **		pNewParaNode,
				EditOperation *		eo,
				struct BufferItem *		parentNode,
				int			row0,
				int			row,
				int			row1,
				int			col,
				int			after )
    {
    int				rval= 0;
    struct BufferDocument *		bd= eo->eoDocument;

    struct BufferItem *		newParaNode= (struct BufferItem *)0;

    struct BufferItem *		rowNode;

    const int			recursively= 0;

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;

    RowProperties		newRp;

    docInitRowProperties( &newRp );

    utilPropMaskClear( &rpSetMask );
    PROPmaskADD( &rpSetMask, RPprop_CELL_PROPS );
    utilPropMaskClear( &cpSetMask );
    PROPmaskADD( &cpSetMask, CLpropWIDTH );

    if  ( after )
	{ after= 1;	}

    while( row0 <= row1 )
	{
	struct BufferItem *		cellNode;
	struct BufferItem *		paraNode;
	int			colTo;
	int			colCount;

	int			textAttributeNr;

	rowNode= parentNode->biChildren[row0];

	{
	const RowProperties *	oldRp;
	oldRp= rowNode->biRowProperties;

	if  ( docCopyRowProperties( &newRp, oldRp,
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	newRp.rpCells[col].cpWide /= 2;

	if  ( docInsertRowColumn( &newRp, col+ 1, &(newRp.rpCells[col]),
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	newRp.rpCells[col+ 1].cpWide=
		    oldRp->rpCells[col].cpWide- newRp.rpCells[col].cpWide;
	}

	cellNode= rowNode->biChildren[col];
	paraNode= cellNode->biChildren[0];
	textAttributeNr= paraNode->biParaParticules[0].tpTextAttrNr;

	paraNode= docInsertCellInRow( bd, rowNode, col, after,
				&(newRp.rpCells[col]), textAttributeNr );

	colTo= 0;
	colCount= newRp.rpCellCount;

	if  ( docChangeRowNodeProperties( (PropertyMask *)0, (PropertyMask *)0,
				    rowNode, &rpSetMask, &cpSetMask, &newRp,
				    colTo, colCount, bd,
				    (const struct DocumentAttributeMap *)0 ) )
	    { LDEB(col); rval= -1; goto ready;	}

	if  ( row0 == row )
	    { newParaNode= paraNode;	}

	{
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	sectShift= 0;

	const int	paraNr= docNumberOfParagraph( paraNode );
	const int	paraShift= 1;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
					paraNr, stroffFrom,
					sectShift, paraShift, stroffShift );
	}

	docEditIncludeNodeInReformatRange( eo, rowNode );

	row0++;
	}

    docDelimitTables( parentNode, recursively );

    *pNewParaNode= newParaNode;

  ready:

    docCleanRowProperties( &newRp );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete columns from a table.					*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRows(	EditOperation *	eo,
				struct BufferItem *	parentNode,
				int		delRow0,
				int		delRow1,
				int		delCol0,
				int		delCol1 )
    {
    int				rval= 0;
    int				count= delCol1- delCol0+ 1;
    int				row;

    const int			recursively= 0;

    for ( row= delRow0; row <= delRow1; row++ )
	{
	struct BufferItem *		rowNode= parentNode->biChildren[row];
	DocumentSelection	dsCols;

	docInitDocumentSelection( &dsCols );
	if  ( docHeadPosition( &dsCols.dsHead, rowNode->biChildren[delCol0] ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docTailPosition( &dsCols.dsTail, rowNode->biChildren[delCol1] ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docEditDeleteFieldsForBlockDelete( eo, &dsCols ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    for ( row= delRow0; row <= delRow1; row++ )
	{
	struct BufferItem *	rowNode= parentNode->biChildren[row];

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	docEditDeleteNodes( eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					rowNode, delCol0, count );

	docEditIncludeNodeInReformatRange( eo, rowNode );
	}

    docDelimitTables( parentNode, recursively );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Flatten a row in a table.						*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRefreshTableNesting(
				struct BufferItem *			node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				vbd )
    {
    const struct BufferDocument *	bd= (const struct BufferDocument *)vbd;

    if  ( docIsParaNode( node ) )
	{ docSetParaTableNesting( node, bd );	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void docFlattenRow(		struct BufferItem *		node,
				struct BufferDocument *	bd )
    {
    /* nearest row. NOT a real one! */
    node= docGetRowLevelNode( node );

    if  ( ! node )
	{ XDEB(node);	}
    else{
	docRowNodeResetRowProperties( node, bd );

	docScanTreeNode( bd, node,
		docRefreshTableNesting, (NodeVisitor)0,
		(TreeVisitor)0, (TreeVisitor)0, 
		0, (void *)bd );
	}
    }

/************************************************************************/
/*									*/
/*  Build a table to be inserted in another document.			*/
/*									*/
/************************************************************************/

int docFillTableDocument(	struct BufferDocument *	bdTable,
				struct BufferItem *	parentNode,
				int			textAttrNr,
				int			wide,
				int			rows,
				int			columns )
    {
    int				rval= 0;
    int				row;
    int				col;

    const TextAttribute *	ta;

    RowProperties		rp;
    CellProperties *		cp;
    BorderProperties		bp;
    int				borderPropertiesNumber;

    docInitRowProperties( &rp );
    docInitBorderProperties( &bp );

    bp.bpStyle= DOCbsS;
    bp.bpPenWideTwips= 15;

    borderPropertiesNumber= docBorderPropertiesNumber( bdTable, &bp );
    if  ( borderPropertiesNumber < 0 )
	{ LDEB(borderPropertiesNumber); rval= -1; goto ready;	}

    ta= docGetTextAttributeByNumber( bdTable, textAttrNr );

    if  ( docEqualWidthColumns( &rp, columns, wide, ta->taFontSizeHalfPoints ) )
	{ LDEB(columns); rval= -1; goto ready;	}

    cp= rp.rpCells;
    for ( col= 0; col < rp.rpCellCount; cp++, col++ )
	{
	cp->cpTopBorderNumber= borderPropertiesNumber;
	cp->cpLeftBorderNumber= borderPropertiesNumber;
	cp->cpRightBorderNumber= borderPropertiesNumber;
	cp->cpBottomBorderNumber= borderPropertiesNumber;
	}

    for ( row= 0; row < rows; row++ )
	{
	struct BufferItem *		rowNode= (struct BufferItem *)0;

	rowNode= docInsertRowNode( bdTable, parentNode, row, &rp, textAttrNr );
	if  ( ! rowNode )
	    { XDEB(rowNode); rval= -1; goto ready;	}
	}

  ready:

    docCleanRowProperties( &rp );
    docCleanBorderProperties( &bp );

    return rval;
    }

