/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"
#   include	"docEditOperation.h"
#   include	<docFields.h>

#   include	<appDebugon.h>

/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docEditForgetFields(	struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voideo )
    {
    if  ( node->biLevel == DOClevPARA )
	{
	int		partFrom;
	int		partUpto;
	int		headFlags;
	int		tailFlags;

	int		part;
	TextParticule *	tp;

	DocumentSelection	dsPara;

	if  ( docIntersectSelectionWithParagraph( &dsPara,
				    &partFrom, &partUpto,
				    &headFlags, &tailFlags,
				    node, ds ) < 0 )
	    { LDEB(1); return -1;	}

	if  ( headFlags & POSflagPART_TAIL )
	    { partFrom++;	}

	tp= node->biParaParticules+ partFrom;
	for ( part= partFrom; part < partUpto; tp++, part++ )
	    {
	    if  ( tp->tpKind == TPkindFIELDHEAD	||
		  tp->tpKind == TPkindFIELDTAIL	)
		{
		tp->tpObjectNumber= -1;
		}
	    }
	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int docDeleteFieldsInRange(	EditOperation *		eo,
					DocumentSelection *	dsBalanced )
    {
    int			updateFlags= 0;
    int			flags= 0;

    EditRange		erBalanced;

    if  ( ! eo->eoIsFieldBalanced )
	{
	struct DocumentField *	dfLeft;
	struct DocumentField *	dfRight;

	int			headMoved= 0;
	int			tailMoved= 0;

	int * const		pHeadPart= (int *)0;
	int * const		pTailPart= (int *)0;

	if  ( docBalanceFieldSelection( &dfLeft, &dfRight,
			    &headMoved, &tailMoved, pHeadPart, pTailPart,
			    &(dsBalanced->dsHead), &(dsBalanced->dsTail),
			    eo->eoTree, eo->eoDocument ) )
	    { LDEB(1); return -1;	}
	}

    docSetEditRange( &erBalanced, dsBalanced );

    if  ( docDeleteFieldRange( &updateFlags, eo->eoDocument,
				&erBalanced, &(eo->eoTree->dtRootFields) ) )
	{ LDEB(1);	}

    eo->eoFieldUpdate |= updateFlags;

    docScanSelection( eo->eoDocument, dsBalanced,
				    docEditForgetFields, (NodeVisitor)0,
				    (TreeVisitor)0, (TreeVisitor)0, 
				    flags, (void *)eo );

    return 0;
    }

/************************************************************************/
/*									*/
/*  A set of branches in the document tree, rather than a balanced	*/
/*  selection is deleted. Make sure that no partial fields remain.	*/
/*									*/
/*  This code relies on the fact that a field is deleted if its		*/
/*  contents are fully selected.					*/
/*									*/
/************************************************************************/

int docEditDeleteFieldsForBlockDelete(	EditOperation *			eo,
					const DocumentSelection *	ds )
    {
    int			rval= 0;
    DocumentSelection	dsBalanced= *ds;

    dsBalanced.dsHead.dpStroff= 0;
    dsBalanced.dsTail.dpStroff= docParaStrlen( dsBalanced.dsTail.dpNode );

    if  ( docDeleteFieldsInRange( eo, &dsBalanced ) )
	{ LDEB(1); return -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete all fields that are embedded in the content that we will	*/
/*  replace in this edit operation.					*/
/*									*/
/************************************************************************/

int docEditDeleteReplacedFields(	EditOperation *		eo )
    {
    DocumentSelection	ds;

    docSetRangeSelection( &ds, &(eo->eoHeadDp), &(eo->eoTailDp), 1 );

    if  ( docDeleteFieldsInRange( eo, &ds ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

