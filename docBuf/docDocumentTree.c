/************************************************************************/
/*									*/
/*  Buffer administration for 'ExternalItems' I.E. headers/footers and	*/
/*  foonotes/endnotes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docShape.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	<docTreeType.h>
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	<docBreakKind.h>
#   include	<docSectProperties.h>
#   include	"docFields.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise an struct DocumentTree.						*/
/*									*/
/************************************************************************/

void docInitDocumentTree(	struct DocumentTree *	dt )
    {
    dt->dtRoot= (struct BufferItem *)0;
    dt->dtPageFormattedFor= -1;
    dt->dtColumnFormattedFor= -1;

    dt->dtY0UsedTwips= 0;
    dt->dtY1UsedTwips= 0;
    dt->dtY0ReservedTwips= 0;
    dt->dtY1ReservedTwips= 0;

    dt->dtPageSelectedUpon= -1;
    dt->dtColumnSelectedIn= -1;

    docInitListNumberTrees( &(dt->dtListNumberTrees) );
    docInitListNumberTree( &(dt->dtOutlineTree) );

    docInitChildFields( &(dt->dtRootFields) );
    return;
    }

void docCleanDocumentTree(	struct BufferDocument *	bd,
				struct DocumentTree *	dt )
    {
    int		updateFlags= 0;

    if  ( dt->dtRoot )
	{ docDeleteDocumentTree( bd, dt );	}

    docDeleteChildFields( &updateFlags, bd, &(dt->dtRootFields) );

    docCleanListNumberTrees( &(dt->dtListNumberTrees) );
    docCleanListNumberTree( &(dt->dtOutlineTree) );

    return;
    }

void docInvalidateTreeLayout(	struct DocumentTree *	dt )
    {
    dt->dtPageFormattedFor= -1;
    dt->dtColumnFormattedFor= -1;
    }

/************************************************************************/
/*									*/
/*  Delete the contents of a document tree.				*/
/*									*/
/*  Leave some of the geometry intact to permit the formatter to see	*/
/*  things have changed and hence cause a redraw of its original	*/
/*  rectangle.								*/
/*									*/
/************************************************************************/

void docEraseDocumentTree(	struct BufferDocument *	bd,
				struct DocumentTree *		dt )
    {
    int		resY0= dt->dtY0ReservedTwips;
    int		resY1= dt->dtY1ReservedTwips;

    docCleanDocumentTree( bd, dt );
    docInitDocumentTree( dt );

    dt->dtY0ReservedTwips= resY0;
    dt->dtY1ReservedTwips= resY1;

    return;
    }

/************************************************************************/
/*  									*/
/*  Keep track of the hierarchy of fields in this tree.			*/
/*  									*/
/************************************************************************/

int docAddRootFieldToTree(	struct DocumentTree *	dt,
				DocumentField *		dfCh )
    {
    if  ( docInsertChildField( (DocumentField *)0,
					    &(dt->dtRootFields), dfCh ) )
	{ LDEB(1); return -1;	}

    dfCh->dfParent= (DocumentField *)0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a new 'Document Tree' I.E. A tree in the document that is not	*/
/*  part of the regular flow of the document. Examples are:		*/
/*  - (Section) page headers and footers.				*/
/*  - Endnotes and Footnotes						*/
/*  - Note separators.							*/
/*  - The text inside shapes.						*/
/*									*/
/************************************************************************/

int docMakeDocumentTree(	struct BufferDocument *			bd,
				struct DocumentTree *			tree,
				const SelectionScope *			ss,
				const struct SectionProperties *	sp )
    {
    struct BufferItem *	node= (struct BufferItem *)0;
    const int		numberInParent= 0;

    node= (struct BufferItem *)malloc( sizeof(struct BufferItem) );
    if  ( ! node )
	{ XDEB(node); return -1;	}

    docInitNode( node, (struct BufferItem *)0, bd, numberInParent,
						DOClevSECT, ss->ssTreeType );

    if  ( docCopySectionProperties( node->biSectProperties, sp ) )
	{ LDEB(1); docFreeNode( bd, tree, node ); return -1; }

    node->biSectBreakKind= DOCibkNONE;
    node->biSectSelectionScope= *ss;
    tree->dtRoot= node;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a document tree and insert an empty paragraph into it.		*/
/*									*/
/************************************************************************/

struct BufferItem * docStartDocumentTree(
			struct BufferDocument *		bd,
			struct DocumentTree *		tree,
			int				treeType,
			const struct BufferItem *	bodyNode,
			int				ownerNumber,
			int				txtAttrNr )
    {
    struct BufferItem *		paraNode= (struct BufferItem *)0;
    struct BufferItem *		sectNode= (struct BufferItem *)0;

    sectNode= docMakeTreeRoot( bd, tree, bodyNode, ownerNumber, treeType );
    if  ( ! sectNode )
	{ XDEB(sectNode); return (struct BufferItem *)0;	}

    paraNode= docAppendParagraph( bd, tree->dtRoot, txtAttrNr );
    if  ( ! paraNode )
	{
	XDEB(paraNode);
	docCleanDocumentTree( bd, tree );
	docInitDocumentTree( tree );
	}

    return paraNode;
    }

struct BufferItem * docMakeTreeRoot(
			struct BufferDocument *		bd,
			struct DocumentTree *		dt,
			const struct BufferItem *	bodyNode,
			int				ownerNumber,
			int				treeType )
    {
    SectionProperties		spDef;

    int				numberInParent= -1;
    SelectionScope		ss;

    docInitSelectionScope( &ss );
    docInitSectionProperties( &spDef );

    switch( treeType )
	{
	case DOCinBODY:
	    LDEB(treeType); goto ready;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    bodyNode= docGetSectNode( (struct BufferItem *)bodyNode );
	    if  ( ! bodyNode )
		{ XDEB(bodyNode); goto ready;	}

	    numberInParent= bodyNode->biNumberInParent;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    bodyNode= docGetSectNode( (struct BufferItem *)bodyNode );
	    if  ( ! bodyNode )
		{ XDEB(bodyNode); goto ready;	}

	    numberInParent= bodyNode->biNumberInParent;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:

	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    break;

	default:
	    LDEB(treeType); goto ready;
	}


    ss.ssTreeType= treeType;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= numberInParent;
    ss.ssOwnerNumber= ownerNumber;

    if  ( docMakeDocumentTree( bd, dt, &ss, &spDef ) )
	{ XDEB(dt->dtRoot); goto ready;	}

    docInvalidateTreeLayout( dt );

    dt->dtPageSelectedUpon= -1;
    dt->dtColumnSelectedIn= -1;

  ready:

    docCleanSectionProperties( &spDef );

    return dt->dtRoot;
    }

/************************************************************************/
/*									*/
/*  Get the tree in the document that a node is contained in.		*/
/*									*/
/************************************************************************/

int docGetRootOfSelectionScope(	struct DocumentTree **		pTree,
				struct BufferItem **		pBodySectNode,
				struct BufferDocument *		bd,
				const SelectionScope *		ss )
    {
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;
    struct DocumentTree *	dt;

    DocumentField *		dfNote;
    DocumentNote *		dn;

    switch( ss->ssTreeType )
	{
	case DOCinBODY:
	    if  ( pBodySectNode )
		{ bodySectNode= docGetBodySectNodeOfScope( ss, bd );	}
	    dt= &(bd->bdBody);
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}

	    dt= docSectionHeaderFooter( bodySectNode, (unsigned char *)0,
					bd->bdProperties, ss->ssTreeType );
	    if  ( ! dt )
		{ LXDEB(ss->ssTreeType,dt); return -1;	}

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    dfNote= docGetFieldByNumber( bd, ss->ssOwnerNumber );
	    if  ( ! dfNote )
		{ LXDEB(ss->ssOwnerNumber,dfNote); return -1;	}
	    dn= docGetNoteOfField( dfNote, bd );
	    if  ( ! dn )
		{ XDEB(dn); return -1;	}
	    dt= &(dn->dnDocumentTree);

	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    dt= docDocumentNoteSeparator( bd, ss->ssTreeType );
	    if  ( ! dt )
		{ LXDEB(ss->ssTreeType,dt); return -1;	}
	    bodySectNode= (struct BufferItem *)0;
	    /* HACK */
	    if  ( bd->bdBody.dtRoot->biChildCount > 0 )
		{ bodySectNode= bd->bdBody.dtRoot->biChildren[0];	}
	    break;

	case DOCinSHPTXT:
	    bodySectNode= docGetBodySectNodeOfScope( ss, bd );
	    if  ( ! bodySectNode )
		{ XDEB(bodySectNode); return -1;	}
	    {
	    DrawingShape *	ds;

	    ds= docGetShapeByNumber( &(bd->bdShapeList), ss->ssOwnerNumber );
	    if  ( ! ds )
		{ LPDEB(ss->ssOwnerNumber,ds); return -1;	}
	    dt= &(ds->dsDocumentTree);
	    }
	    break;

	default:
	    LDEB(ss->ssTreeType);
	    return -1;
	}

    if  ( pTree )
	{ *pTree= dt;		}
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode; }

    return 0;
    }

