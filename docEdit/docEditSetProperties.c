/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<ctype.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	<docTreeType.h>
#   include	"docEditSetProperties.h"
#   include	"docEditOperation.h"
#   include	<docSelect.h>
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of a node that we encounter on the way.	*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docSetPropsEnterNode( struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				through )
    {
    SetProperties *	setProps= (SetProperties *)through;
    EditOperation *	eo= setProps->spEditOperation;

    switch( node->biLevel )
	{
	case DOClevPARA:
	    setProps->spGotPara++;

	    if  ( docEditChangeParaProperties( setProps, ds, node,
				    setProps->sp_taSet, setProps->sp_ppSet ) )
		{ LDEB(node->biLevel); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{ setProps->spGotCell++;	}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		setProps->spEnteredRow++;

		if  ( docEditSetRowNodeProperties( node, setProps ) )
		    { LDEB(node->biLevel); return -1;	}
		}
	    break;

	case DOClevSECT:
	    setProps->spGotSect++;

	    if  ( setProps->sp_spSetMask && node->biTreeType == DOCinBODY )
		{
		if  ( docEditUpdSectProperties( eo, setProps->sp_spDoneMask,
				      node, setProps->sp_spSetMask,
				      setProps->sp_spSet ) )
		    { XDEB(setProps->sp_spSetMask); return -1;	}
		}
	    break;

	case DOClevBODY:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int docChangeSelectionProperties(
			EditOperation *			eo,
			const DocumentSelection *	ds,

			PropertyMask *			selTaDoneMask,
			const PropertyMask *		taSetMask,
			const struct TextAttribute *	taSet,

			PropertyMask *			selPpDoneMask,
			const PropertyMask *		ppSetMask,
			const struct ParagraphProperties * ppSet,

			PropertyMask *			selCpDoneMask,
			const PropertyMask *		cpSetMask,
			const struct CellProperties *	cpSet,

			PropertyMask *			selRpDoneMask,
			const PropertyMask *		rpSetMask,
			const struct RowProperties *	rpSet,

			PropertyMask *			selSpDoneMask,
			const PropertyMask *		spSetMask,
			const struct SectionProperties * spSet,

			PropertyMask *			docDpDoneMask,
			const PropertyMask *		dpSetMask,
			const struct DocumentProperties * dpSet )

    {
    int			rval= 0;
    const int		flags= 0; /* NOT FLAGtsSCAN_MERGED_CELLS */
    SetProperties	setProps;

    docEditInitSetProperties( &setProps );

    if  ( ds && docIsIBarSelection( ds )		&&
	  docParaStrlen( ds->dsHead.dpNode ) > 0	)
	{ taSetMask= (const PropertyMask *)0;	}

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    setProps.spEditOperation= eo;
    setProps.spRedoParaLayout= 1;

    setProps.sp_taDoneMask= selTaDoneMask;
    setProps.sp_taSetMask= taSetMask;
    setProps.sp_taSet= taSet;

    setProps.sp_ppDoneMask= selPpDoneMask;
    setProps.sp_ppSetMask= ppSetMask;
    setProps.sp_ppSet= ppSet;

    setProps.sp_cpDoneMask= selCpDoneMask;
    setProps.sp_cpSetMask= cpSetMask;
    setProps.sp_cpSet= cpSet;

    setProps.sp_rpDoneMask= selRpDoneMask;
    setProps.sp_rpSetMask= rpSetMask;
    setProps.sp_rpSet= rpSet;

    setProps.sp_spDoneMask= selSpDoneMask;
    setProps.sp_spSetMask= spSetMask;
    setProps.sp_spSet= spSet;

    if  ( cpSetMask || rpSetMask )
	{
	if  ( docGetTableRectangle( &(setProps.spTableRectangle), ds ) )
	    { XXDEB(cpSetMask,rpSetMask); rval= -1; goto ready;	}
	}
    else{
	docInitTableRectangle( &(setProps.spTableRectangle) );
	}

    if  ( dpSetMask							&&
	  docChangeDocumentProperties( eo, docDpDoneMask,
			    dpSetMask, dpSet, setProps.spAttributeMap )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! ds )
	{
	if  ( docScanTree( eo->eoDocument, eo->eoTree,
				    docSetPropsEnterNode, (NodeVisitor)0,
				    (TreeVisitor)0, (TreeVisitor)0, 
				    flags, (void *)&setProps ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( ! docSelectionIsSet( ds ) )
	    {
	    XXDEB(ds->dsHead.dpNode,ds->dsTail.dpNode);
	    rval= -1; goto ready;
	    }

	if  ( docSelectionSingleParagraph( ds ) &&
	      ! ppSetMask			&&
	      ! cpSetMask			&&
	      ! rpSetMask			&&
	      ! spSetMask			&&
	      ! dpSetMask			)
	    {
	    const int	stroffShift= 0;

	    setProps.spRedoParaLayout= 0;

	    docSetParagraphAdjust( eo, ds->dsHead.dpNode,
					    stroffShift, ds->dsTail.dpStroff );
	    }

	if  ( docScanSelection( eo->eoDocument, ds,
				    docSetPropsEnterNode, (NodeVisitor)0,
				    (TreeVisitor)0, (TreeVisitor)0, 
				    flags, (void *)&setProps ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ( rpSetMask || cpSetMask ) && ! setProps.spEnteredRow )
	    {
	    struct BufferItem *	rowNode= docGetRowNode( ds->dsHead.dpNode );

	    if  ( ! rowNode )
		{ XDEB(rowNode); rval= -1; goto ready;	}

	    if  ( docSetPropsEnterNode( rowNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(rpSetMask); rval= -1; goto ready;	}
	    }

	if  ( cpSetMask && ! setProps.spGotCell )
	    {
	    struct BufferItem *	cellNode= docGetCellNode( ds->dsHead.dpNode );

	    if  ( ! cellNode )
		{ XDEB(cellNode); rval= -1; goto ready;	}

	    if  ( docSetPropsEnterNode( cellNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(cpSetMask); rval= -1; goto ready;	}
	    }

	if  ( spSetMask && ! setProps.spGotSect )
	    {
	    struct BufferItem *	sectNode= docGetSectNode( ds->dsHead.dpNode );

	    if  ( ! sectNode )
		{ XDEB(sectNode); rval= -1; goto ready;	}

	    if  ( docSetPropsEnterNode( sectNode, ds,
				eo->eoBodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(spSetMask); rval= -1; goto ready;	}
	    }

	if  ( spSetMask && ds->dsHead.dpNode->biTreeType != DOCinBODY )
	    {
	    struct BufferItem *	bodySectNode;

	    bodySectNode= docGetBodySectNode( ds->dsHead.dpNode,
							    eo->eoDocument );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); rval= -1; goto ready;	}

	    if  ( docSetPropsEnterNode( bodySectNode, ds,
				    bodySectNode, (void *)&setProps ) < 0 )
		{ XDEB(spSetMask); rval= -1; goto ready;	}
	    }
	}

  ready:

    docEditCleanSetProperties( &setProps );

    return rval;
    }

void docEditInitSetProperties(	SetProperties *		setProps )
    {
    setProps->spEditOperation= (struct EditOperation *)0;
    setProps->spRedoParaLayout= 0;
    setProps->spAttributeMap= (const struct DocumentAttributeMap *)0;

    docInitTableRectangle( &(setProps->spTableRectangle) );

    setProps->spGotPara= 0;
    setProps->spGotCell= 0;
    setProps->spEnteredRow= 0;
    setProps->spLeftRow= 0;
    setProps->spGotSect= 0;

    docInitRowProperties( &(setProps->spCurrentRowProperties) );

    setProps->sp_taDoneMask= (struct PropertyMask *)0;
    setProps->sp_taSetMask= (const struct PropertyMask *)0;
    setProps->sp_taSet= (const struct TextAttribute *)0;

    setProps->sp_ppDoneMask= (struct PropertyMask *)0;
    setProps->sp_ppSetMask= (const struct PropertyMask *)0;
    setProps->sp_ppSet= (const struct ParagraphProperties *)0;

    setProps->sp_cpDoneMask= (struct PropertyMask *)0;
    setProps->sp_cpSetMask= (const struct PropertyMask *)0;
    setProps->sp_cpSet= (const struct CellProperties *)0;

    setProps->sp_rpDoneMask= (struct PropertyMask *)0;
    setProps->sp_rpSetMask= (const struct PropertyMask *)0;
    setProps->sp_rpSet= (const struct RowProperties *)0;

    setProps->sp_spDoneMask= (struct PropertyMask *)0;
    setProps->sp_spSetMask= (const struct PropertyMask *)0;
    setProps->sp_spSet= (const struct SectionProperties *)0;
    }

void docEditCleanSetProperties(	SetProperties *		setProps )
    {
    docCleanRowProperties( &(setProps->spCurrentRowProperties) );
    }

