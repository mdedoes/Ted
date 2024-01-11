/************************************************************************/
/*									*/
/*  Buffer administration routines. Functionality related to the node	*/
/*  tree.								*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docNodeTree.h"
#   include	"docParaParticules.h"
#   include	"docSectHeadersFooters.h"
#   include	"docRowNodeProperties.h"
#   include	"docParaNodeProperties.h"
#   include	<docRowProperties.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	"docParaParticuleAdmin.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

#   define	VALIDATE_TREE	0

#if 0

Schema to illustrate the numbering of paragraphs and the 
administration that is kept for that purpose
=========================================================

node->biLeftParagraphs is the number of paragraphs to the left 
of this node in its immediate parent plus the number of paragraph
descendants. I.E: For nodes with children, the number of paragraphs 
in the children is included in node->biLeftParagraphs. A paragraph 
includes itself in its node->biLeftParagraphs.

Numbers of the paragraphs and the value of node->biLeftParagraphs:

    1     2     3     4     5     6     7     8     9    10    11    12
    +     +     +     +     +     +     +     +     +     +     +     +
    1     2     3     4     1     2     3     4     5     1     2     3
    |     |     |     |     |     |     |     |     |     |     |     |
    4-----+-----+-----+     9-----+-----+-----+-----+     12----+-----+
    |                       |                             |
    12----------------------+-----------------------------+
    |
    *

Deleting 8 yields:

    1     2     3     4     5     6     7           8     9    10    11
    +     +     +     +     +     +     +           +     +     +     +
    1     2     3     4     1     2     3           4     1     2     3
    |     |     |     |     |     |     |           |     |     |     |
    4-----+-----+-----+     8-----+-----+-----+-----+     11----+-----+
    |                       |                             |
    11----------------------+-----------------------------+
    |
    *

So after deleting/inserting paragraphs: Descend to the root of the 
tree. In every parent adapt node->biLeftParagraphs of all direct 
children to the right of the child that we come from. Set
node->biLeftParagraphs of the parent to the value in its right most 
child. Continue with the parent of the parent until we have reached 
the root.

NOTE: I am educated as a biologist. My trees have their root at the 
    bottom, not at the top like those of computer scientists that 
    turn the world upside down. (I must admit that the metaphor 
    of parents and descendants has the computer science orientation.)

#endif

/************************************************************************/
/*									*/
/*  Free a BufferItem.							*/
/*									*/
/************************************************************************/

static void docCleanNode(	struct BufferDocument *	bd,
				struct DocumentTree *	tree,
				struct BufferItem *	node )
    {
    int				i;

    for ( i= node->biChildCount- 1; i >= 0; i-- )
	{ docFreeNode( bd, tree, node->biChildren[i] ); }
    if  ( node->biChildren )
	{ free( node->biChildren );	}

    switch( node->biLevel )
	{
	case DOClevBODY:
	    break;

	case DOClevSECT:
	    {
	    SectHeadersFooters *	shf= node->biSectHeadersFooters;

	    if  ( shf )
		{
		docCleanSectHeadersFooters( bd, shf );
		free( shf );
		}

	    if  ( node->biSectProperties )
		{
		docCleanSectionProperties( node->biSectProperties );
		free( node->biSectProperties );
		}
	    }
	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    utilCleanMemoryBuffer( &(node->biRowSummary) );
	    break;

	case DOClevPARA:
	    docCleanParaNode( bd, tree, node );
	    break;

	default:
	    /*FALLTHROUGH*/
	case DOClevOUT:
	    LDEB(node->biLevel);
	    break;
	}

    node->biLevel= DOClevOUT;
    }

void docFreeNode(	struct BufferDocument *	bd,
			struct DocumentTree *	tree,
			struct BufferItem *	node )
    {
    docCleanNode( bd, tree, node );
    free( node );
    }

struct BufferItem * docMakeNode( void )
    {
    return (struct BufferItem *)malloc(sizeof(struct BufferItem));
    }

/************************************************************************/
/*									*/
/*  Initialise a BufferItem.						*/
/*									*/
/************************************************************************/

void docInitNode(	struct BufferItem *		node,
			struct BufferItem *		parent,
			const struct BufferDocument *	bd,
			int			numberInParent,
			int			level,
			int			treeType )
    {
    node->biChildren= (struct BufferItem **)0;
    node->biChildCount= 0;
    node->biLeftParagraphs= 0;

    switch( level )
	{
	case DOClevBODY:
	    break;

	case DOClevSECT:
	    node->biSectHeadersFooters= (SectHeadersFooters *)malloc(
						sizeof(SectHeadersFooters) );
	    if  ( ! node->biSectHeadersFooters )
		{ XDEB(node->biSectHeadersFooters);			}
	    else{ docInitSectHeadersFooters( node->biSectHeadersFooters ); }

	    node->biSectProperties= malloc( sizeof(SectionProperties) );
	    if  ( ! node->biSectProperties )
		{ XDEB(node->biSectProperties);			}
	    else{
		docInitSectionProperties( node->biSectProperties );

		if  ( bd )
		    { node->biSectDocumentGeometry= bd->bdProperties->dpGeometry; }
		}

	    docInitSelectionScope( &(node->biSectSelectionScope) );

	    node->biSectSelectionScope.ssTreeType= treeType;
	    node->biSectSelectionScope.ssSectNr= numberInParent;

	    break;

	case DOClevCELL:
	    node->biCellTopInset= 0;
	    node->biCellRowspan= 1;
	    node->biCellMergedCellTopRow= -1;
	    node->biCellMergedCellTopCol= -1;

	    node->biCellProperties= (const struct CellProperties *)0;
	    node->biCellHeadX= 0;
	    node->biCellTailX= 0;
	    node->biCellColspan= 0;
	    break;

	case DOClevROW:
	    node->biRowTableFirst= -1;
	    node->biRowTablePast= -1;
	    node->biRowPastHeaderRow= -1;
	    node->biRowPrecededByHeader= 0;
	    node->biRowIsPlain= 0;
	    node->biRowForTable= 0;

	    node->biRowTopInset= 0;

	    node->BIU.biuRow.brProperties= (const struct RowProperties *)0;
	    node->BIU.biuRow.brRowPropertyNumber= 0;

	    docInitLayoutPosition( &(node->biRowBelowAllCellsPosition) );
	    docInitLayoutPosition( &(node->biRowAboveHeaderPosition) );

	    utilInitMemoryBuffer( &(node->biRowSummary) );
	    break;

	case DOClevPARA:
	    docInitParaNode( node );
	    break;

	default:
	    node->biLevel= DOClevOUT;
	    node->biParent= (struct BufferItem *)0;
	    LDEB(level); return;
	}

    node->biLevel= level;
    node->biTreeType= treeType;
    node->biParent= parent;
    node->biNumberInParent= numberInParent;

    docInitLayoutPosition( &(node->biTopPosition) );
    docInitLayoutPosition( &(node->biBelowPosition) );

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Delete a series of nodes.					*/
/*  2)  Delete a node from its parent.					*/
/*									*/
/************************************************************************/

static void docSectSetSelectionScopes(	struct BufferItem *	sectNode )
    {
    int				n= sectNode->biNumberInParent;
    SectHeadersFooters *	shf= sectNode->biSectHeadersFooters;

    sectNode->biSectSelectionScope.ssSectNr= n;

    if  ( shf )
	{
	if  ( shf->shfFirstPageHeader.dtRoot )
	    {
	    shf->shfFirstPageHeader.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfLeftPageHeader.dtRoot )
	    {
	    shf->shfLeftPageHeader.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfRightPageHeader.dtRoot )
	    {
	    shf->shfRightPageHeader.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfLastPageHeader.dtRoot )
	    {
	    shf->shfLastPageHeader.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }

	if  ( shf->shfFirstPageFooter.dtRoot )
	    {
	    shf->shfFirstPageFooter.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfLeftPageFooter.dtRoot )
	    {
	    shf->shfLeftPageFooter.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfRightPageFooter.dtRoot )
	    {
	    shf->shfRightPageFooter.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	if  ( shf->shfLastPageFooter.dtRoot )
	    {
	    shf->shfLastPageFooter.dtRoot->
				biSectSelectionScope.ssOwnerSectNr= n;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Paragraphs have been deleted from a group node.			*/
/*  Administration in the child array has been done. Fix		*/
/*  biLeftParagraphs in the node itself and its parents.		*/
/*									*/
/************************************************************************/

static void docParagraphsDeleted(	struct BufferItem *	node,
					int		paragraphsDeleted )
    {
    while( node->biParent )
	{
	int		first;
	int		f;

	first= node->biNumberInParent;
	node= node->biParent;

	for ( f= first; f < node->biChildCount; f++ )
	    { node->biChildren[f]->biLeftParagraphs -= paragraphsDeleted;	}
	}

    node->biLeftParagraphs -= paragraphsDeleted;

    return;
    }

/*  1  */
void docDeleteNodes(	struct BufferDocument *	bd,
			struct DocumentTree *	tree,
			struct BufferItem *	node,
			int			first,
			int			count )
    {
    int		n;
    int		f;
    int		c;
    int		paragraphsDeleted= 0;

#   if VALIDATE_TREE
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

    if  ( first > node->biChildCount )
	{
	LLDEB(first,node->biChildCount);
	first= node->biChildCount;
	}

    if  ( first+ count > node->biChildCount )
	{
	LLDEB(first+count,node->biChildCount);
	count= node->biChildCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}
    else{
	f= 0;
	if  ( first > 0 )
	    { f= node->biChildren[first- 1]->biLeftParagraphs;	}

	c= node->biChildren[first+ count- 1]->biLeftParagraphs;

	paragraphsDeleted= c- f;
	}

    n= first+ count- 1;
    while( n >= first )
	{ docFreeNode( bd, tree, node->biChildren[n] ); n--; }

    node->biChildCount -= count;

    f= first;
    while( f < node->biChildCount )
	{
	node->biChildren[f]= node->biChildren[f+ count];
	node->biChildren[f]->biNumberInParent= f;
	node->biChildren[f]->biLeftParagraphs -= paragraphsDeleted;

	if  ( node->biChildren[f]->biLevel == DOClevSECT )
	    { docSectSetSelectionScopes( node->biChildren[f] ); }

	f++;
	}

    docParagraphsDeleted( node, paragraphsDeleted );

#   if VALIDATE_TREE
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

    return;
    }

/*  2  */
void docDeleteNode(	struct BufferDocument *	bd,
			struct DocumentTree *	tree,
			struct BufferItem *	node )
    {
    if  ( node->biParent )
	{
	docDeleteNodes( bd, tree, node->biParent, node->biNumberInParent, 1 );
	}
    else{ docFreeNode( bd, tree, node );					}
    }

/************************************************************************/
/*									*/
/*  Delete the Document Tree Node that is the root of a document Tree	*/
/*  This is NOT the way to delete a document tree. Use			*/
/*  docEraseDocumentTree() to do so.					*/
/*									*/
/************************************************************************/

void docDeleteDocumentTree(	struct BufferDocument *	bd,
				struct DocumentTree *	tree )
    {
    docFreeNode( bd, tree, tree->dtRoot );
    tree->dtRoot= (struct BufferItem *)0;
    }

/************************************************************************/
/*									*/
/*  Paragraphs have been inserted into a group node.			*/
/*  Administration in the child array has been done. Fix		*/
/*  biLeftParagraphs in the node itself and its parents.		*/
/*									*/
/************************************************************************/

static void docParagraphsInserted(	struct BufferItem *	node,
					int		paragraphsInserted )
    {
    while( node->biParent )
	{
	int		first;
	int		f;

	first= node->biNumberInParent;
	node= node->biParent;

	for ( f= first; f < node->biChildCount; f++ )
	    { node->biChildren[f]->biLeftParagraphs += paragraphsInserted; }
	}

    node->biLeftParagraphs += paragraphsInserted;

    return;
    }

/************************************************************************/

int docValidChildLevel(		int		parentLevel,
				int		childLevel )
    {
    switch( parentLevel )
	{
	case DOClevBODY:
	    if  ( childLevel != DOClevSECT )
		{ return 0;	}
	    break;

	case DOClevSECT:
	    if  ( childLevel != DOClevROW )
		{ return 0; }
	    break;

	case DOClevROW:
	    if  ( childLevel != DOClevCELL )
		{ return 0; }
	    break;

	case DOClevCELL:
	    if  ( childLevel != DOClevPARA && childLevel != DOClevROW )
		{ return 0; }
	    break;

	default:
	    LLDEB(parentLevel,childLevel); return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Add a new child to a parent.					*/
/*									*/
/************************************************************************/

struct BufferItem * docInsertNode(
				const struct BufferDocument *	bd,
				struct BufferItem *		parent,
				int				n,
				int				level )
    {
    struct BufferItem *	rval= (struct BufferItem *)0;
    struct BufferItem *	newNode= (struct BufferItem *)0;

    int			i;

    int			newSize;

    struct BufferItem **	freshChildren;

    int			paragraphsInserted;

#   if VALIDATE_TREE
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

    if  ( ! docValidChildLevel( parent->biLevel, level ) )
	{ SSDEB(docLevelStr(parent->biLevel),docLevelStr(level)); goto ready; }

    if  ( n == -1 )
	{ n= parent->biChildCount;	}

    newSize= ( parent->biChildCount+ 1 )* sizeof(struct BufferItem *);

    freshChildren= (struct BufferItem **)realloc( parent->biChildren, newSize );
    if  ( ! freshChildren )
	{ LLXDEB(parent->biChildCount,newSize,freshChildren); goto ready; }
    parent->biChildren= freshChildren;

    newNode= (struct BufferItem *)malloc( sizeof(struct BufferItem) );
    if  ( ! newNode )
	{ XDEB(newNode); goto ready;	}

    docInitNode( newNode, parent, bd, n, level, parent->biTreeType );

    if  ( n == 0 )
	{ newNode->biTopPosition= parent->biTopPosition;		}
    else{ newNode->biTopPosition= freshChildren[n-1]->biBelowPosition;	}

    for ( i= parent->biChildCount; i > n; i-- )
	{
	freshChildren[i]= freshChildren[i-1];

	freshChildren[i]->biNumberInParent= i;

	if  ( freshChildren[i]->biLevel == DOClevSECT )
	    { docSectSetSelectionScopes( freshChildren[i] ); }
	}

    freshChildren[n]= newNode;
    parent->biChildCount++;

    rval= newNode; newNode= (struct BufferItem *)0; /* steal */

    if  ( level == DOClevPARA )
	{
	paragraphsInserted= 1;

	docSetParaTableNesting( rval, bd );

	if  ( n > 0 )
	    { rval->biLeftParagraphs= freshChildren[n-1]->biLeftParagraphs; }

	for ( i= n; i < parent->biChildCount; i++ )
	    { parent->biChildren[i]->biLeftParagraphs += paragraphsInserted; }

	docParagraphsInserted( parent, paragraphsInserted );
	}
    else{
	if  ( n > 0 )
	    { rval->biLeftParagraphs= freshChildren[n-1]->biLeftParagraphs; }
	}

#   if VALIDATE_TREE
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

  ready:

    if  ( newNode )
	{ free( newNode );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Append an empty paragraph at the end of a (sub)tree in the		*/
/*  document.								*/
/*									*/
/************************************************************************/

struct BufferItem * docAppendParagraph(
			struct BufferDocument *	bd,
			struct BufferItem *	node,
			int			textAttributeNr )
    {
    if  ( node->biLevel < DOClevSECT )
	{ LDEB(node->biLevel); return (struct BufferItem *)0;	}

    if  ( node->biLevel < DOClevROW )
	{
	node= docInsertNode( bd, node, -1, DOClevROW );
	if  ( ! node )
	    { XDEB(node); return (struct BufferItem *)0;   }
	}

    if  ( node->biLevel < DOClevCELL )
	{
	node= docInsertNode( bd, node, -1, DOClevCELL );
	if  ( ! node )
	    { XDEB(node); return (struct BufferItem *)0;   }
	}

    if  ( node->biLevel < DOClevPARA )
	{
	node= docInsertNode( bd, node, -1, DOClevPARA );
	if  ( ! node )
	    { XDEB(node); return (struct BufferItem *)0;   }
	}
    else{
	node= docInsertNode( bd, node->biParent, -1, DOClevPARA );
	if  ( ! node )
	    { XDEB(node); return (struct BufferItem *)0;   }
	}

    if  ( ! docInsertTextParticule( node, 0, 0, 0,
				    TPkindSPAN, textAttributeNr ) )
	{ LDEB(1); return (struct BufferItem *)0;	}

    return node;
    }

/************************************************************************/
/*									*/
/*  Insert a new row in a table.					*/
/*									*/
/*  8)  Force the per column administration to be redone.		*/
/*									*/
/************************************************************************/

struct BufferItem * docInsertRowNode(
			struct BufferDocument *		bd,
			struct BufferItem *		parentNode,
			int				n,
			const struct RowProperties *	rp,
			int				textAttributeNr )
    {
    int				col;

    struct BufferItem *		rval= (struct BufferItem *)0;
    struct BufferItem *		rowNode= (struct BufferItem *)0;

    rowNode= docInsertNode( bd, parentNode, n, DOClevROW );
    if  ( ! rowNode )
	{ XDEB(rowNode); goto ready;	}

    if  ( docSetRowNodeProperties( rowNode, rp, bd ) )
	{ LDEB(1); goto ready; }

    for ( col= 0; col < rp->rpCellCount; col++ )
	{
	struct BufferItem *	paraNode;

	paraNode= docAppendParagraph( bd, rowNode, textAttributeNr );
	if  ( ! paraNode )
	    { XDEB(paraNode); goto ready; }

	docSetParaTableNesting( paraNode, bd );
	}

    /*  8  */
    if  ( docSetRowNodeProperties( rowNode, rp, bd ) )
	{ LDEB(1); goto ready; }

    rval= rowNode; rowNode= (struct BufferItem *)0; /* steal */

  ready:

    if  ( rowNode )
	{ docDeleteNode( bd, (struct DocumentTree *)0, rowNode );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Split a node with childen.						*/
/*									*/
/*  1)  Insert a fresh node at the position of the node to be split.	*/
/*  2)  Copy various properties to the fresh node.			*/
/*  3)  Move the first n children to the fresh node.			*/
/*  4)  Paragraph number administration.				*/
/*  5)  Shift the children in the old node.				*/
/*									*/
/************************************************************************/

int docSplitGroupNode(			struct BufferDocument *	bd,
					struct BufferItem **	pNewNode,
					struct BufferItem *	oldNode,
					int			n )
    {
    struct BufferItem *	newNode;
    int			i;
    int			prev;

    /*  1  */
    newNode= docInsertNode( bd, oldNode->biParent,
				oldNode->biNumberInParent, oldNode->biLevel );
    if  ( ! newNode )
	{ XDEB(newNode); return -1;	}

    /*  2  */
    switch( oldNode->biLevel )
	{
	case DOClevSECT:
	    if  ( docCopySectDescription( newNode, bd, oldNode, bd ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    if  ( docCopyRowNodeProperties( newNode, oldNode, bd,
					(const struct DocumentAttributeMap *)0 ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(oldNode->biLevel); return -1;
	}

    newNode->biChildren= (struct BufferItem **)malloc( n* sizeof(struct BufferItem *) );
    if  ( ! newNode->biChildren )
	{ XDEB(newNode->biChildren); return -1;	}

    /*  3  */
    for ( i= 0; i < n; i++ )
	{
	newNode->biChildren[i]= oldNode->biChildren[i];
	newNode->biChildren[i]->biParent= newNode;
	}

    /*  4  */
    prev= 0;
    if  ( newNode->biNumberInParent > 0 )
	{
	prev= newNode->biParent->biChildren[newNode->biNumberInParent-1]->
							biLeftParagraphs;
	}

    if  ( n == 0 )
	{ newNode->biLeftParagraphs= prev; }
    else{
	newNode->biLeftParagraphs= prev+ newNode->biChildren[n-1]->biLeftParagraphs;
	}

    /*  4,5  */
    newNode->biChildCount= n;
    oldNode->biChildCount -= n;

    prev= 0;
    for ( i= 0; i < oldNode->biChildCount; i++ )
	{
	struct BufferItem *	child= oldNode->biChildren[i+ n];

	/*  5  */
	oldNode->biChildren[i]= child;
	child->biNumberInParent -= n;

	if  ( oldNode->biChildren[i]->biLevel == DOClevPARA )
	    { prev++;	}
	else{
	    if  ( child->biChildCount > 0 )
		{
		prev += child->biChildren[child->biChildCount-1]->
							    biLeftParagraphs;
		}
	    }

	oldNode->biChildren[i]->biLeftParagraphs= prev;
	}

    *pNewNode= newNode;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Split a 'group node' I.E. a node that has children rather than	*/
/*  its own text.							*/
/*									*/
/*  The goal is to make sure that the split before position n in	*/
/*  splitNode is between two nodes with level level.			*/
/*									*/
/************************************************************************/

int docSplitGroupNodeAtLevel(	struct BufferDocument *	bd,
				struct BufferItem **	pBeforeNode,
				struct BufferItem **	pAfterNode,
				struct BufferItem *	splitNode,
				int			n,
				int			level )
    {
    struct BufferItem *	beforeNode= (struct BufferItem *)0;
    struct BufferItem *	afterNode= splitNode;
    struct BufferItem *	node;

#   if VALIDATE_TREE
    SDEB(docLevelStr(bd->bdBody.dtRoot->biLevel));
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

    if  ( ! splitNode )
	{ XDEB(splitNode); return -1;	}

    node= splitNode;
    while( node && node->biLevel != level- 1 )
	{ node= node->biParent; }
    if  ( ! node )
	{
	SSXDEB(docLevelStr(splitNode->biLevel),docLevelStr(level),node);
	return -1;
	}

    if  ( n < 0 || n >= splitNode->biChildCount )
	{ LLDEB(n,splitNode->biChildCount); return -1;	}

    afterNode= splitNode;
    for (;;)
	{
	if  ( n > 0 || afterNode->biLevel == level )
	    {
	    if  ( docSplitGroupNode( bd, &beforeNode, afterNode, n ) )
		{ LDEB(n); return -1;	}

	    if  ( afterNode->biLevel == level )
		{ break;	}
	    }

	n= afterNode->biNumberInParent;
	afterNode= afterNode->biParent;
	}

#   if VALIDATE_TREE
    SDEB(docLevelStr(afterNode->biLevel));
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
#   endif

    if  ( pBeforeNode )
	{ *pBeforeNode= beforeNode;	}
    if  ( pAfterNode )
	{ *pAfterNode= afterNode;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Merge the children of two nodes into the first one.			*/
/*									*/
/************************************************************************/

int docMergeGroupNodes(		struct BufferDocument *	bd,
				struct BufferItem *	to,
				struct BufferItem *	from )
    {
    struct BufferItem **	freshChildren;
    int			f;
    int			t;
    int			left;
    int			prev;
    int			paragraphsMoved;

    if  ( to == from )
	{ XXDEB(to,from); return -1;	}
    if  ( to->biLevel != from->biLevel )
	{ LLDEB(to->biLevel,from->biLevel); return -1;	}

    if  ( from->biChildCount == 0 )
	{ return 0;	}

#   if VALIDATE_TREE
    SSDEB(docLevelStr(to->biLevel),docLevelStr(from->biLevel));
    if  ( docCheckRootNode( to, bd, 1 ) )
	{ LDEB(2); docListRootNode( bd, 0, to, 0 ); abort(); }
    if  ( docCheckRootNode( from, bd, 1 ) )
	{ LDEB(2); docListRootNode( bd, 0, from, 0 ); abort(); }
#   endif

    freshChildren= (struct BufferItem **)realloc( to->biChildren,
	    ( to->biChildCount+ from->biChildCount )* sizeof(struct BufferItem *) );
    if  ( ! freshChildren )
	{
	LLXDEB(to->biChildCount,from->biChildCount,freshChildren);
	return -1;
	}
    to->biChildren= freshChildren;

    if  ( from->biParent && from->biNumberInParent > 0 )
	{
	struct BufferItem *	prevNode;

	prevNode= from->biParent->biChildren[from->biNumberInParent- 1];
	paragraphsMoved= from->biLeftParagraphs- prevNode->biLeftParagraphs;
	}
    else{
	paragraphsMoved= from->biLeftParagraphs;
	}

    left= 0; prev= 0;
    t= to->biChildCount;
    if  ( t > 0 )
	{ left= to->biChildren[t- 1]->biLeftParagraphs; }
    for ( f= 0; f < from->biChildCount; t++, f++ )
	{
	int	count;

	freshChildren[t]= from->biChildren[f];
	freshChildren[t]->biParent= to;
	freshChildren[t]->biNumberInParent= t;

	count= freshChildren[t]->biLeftParagraphs- prev;
	prev= freshChildren[t]->biLeftParagraphs;
	left += count;
	freshChildren[t]->biLeftParagraphs= left;

	if  ( freshChildren[t]->biLevel == DOClevSECT )
	    { docSectSetSelectionScopes( freshChildren[t] ); }
	}

    to->biChildCount += from->biChildCount;
    from->biChildCount= 0;

    docParagraphsInserted( to, paragraphsMoved );
    docParagraphsDeleted( from, paragraphsMoved );

#   if VALIDATE_TREE
    SSDEB(docLevelStr(to->biLevel),docLevelStr(from->biLevel));
    if  ( docCheckRootNode( to, bd, 1 ) )
	{ LDEB(2); docListRootNode( bd, 0, to, 0 ); abort(); }
    if  ( docCheckRootNode( from, bd, 1 ) )
	{ LDEB(2); docListRootNode( bd, 0, from, 0 ); abort(); }
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the number of a paragraph.					*/
/*									*/
/************************************************************************/

int docNumberOfParagraph(	const struct BufferItem *	node )
    {
    int		n= 0;

    if  ( node->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(node->biLevel)); return -1;	}

    n= node->biLeftParagraphs;

    while( node->biParent )
	{
	node= node->biParent;

	if  ( node->biParent && node->biNumberInParent > 0 )
	    {
	    if  ( node->biNumberInParent >= node->biParent->biChildCount )
		{ LLDEB(node->biNumberInParent,node->biParent->biChildCount); }

	    n += node->biParent->biChildren[
				node->biNumberInParent- 1]->biLeftParagraphs;
	    }
	}

    return n;
    }

struct BufferItem * docGetParagraphByNumber(
				const struct DocumentTree *	tree,
				int				paraNr )
    {
    struct BufferItem *	node= tree->dtRoot;

    if  ( paraNr < 1 )
	{ LDEB(paraNr); return (struct BufferItem *)0;	}

    while( node->biChildCount > 0 )
	{
	int		i;

	for ( i= 0; i < node->biChildCount; i++ )
	    {
	    if  ( node->biChildren[i]->biLeftParagraphs >= paraNr )
		{ break;	}
	    }

	if  ( i >= node->biChildCount )
	    {
	    /* LLSDEB(paraNr,node->biChildCount,docLevelStr(node->biLevel)); */
	    return (struct BufferItem *)0;
	    }

	if  ( i > 0 )
	    { paraNr -= node->biChildren[i-1]->biLeftParagraphs;	}

	node= node->biChildren[i];
	}

    if  ( node->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(node->biLevel)); return (struct BufferItem *)0;	}

    if  ( paraNr != 1 )
	{ LDEB(paraNr); return (struct BufferItem *)0; }

    return node;
    }

struct BufferItem * docGetCommonParent(	struct BufferItem *	paraNode1,
					struct BufferItem *	paraNode2 )
    {
    int			paraNr1= docNumberOfParagraph( paraNode1 );
    int			paraNr2= docNumberOfParagraph( paraNode2 );

    struct BufferItem *	bi1= paraNode1;
    struct BufferItem *	bi2= paraNode2;

    if  ( paraNr1 < 1 || paraNr2 < paraNr1 )
	{ LLDEB(paraNr1,paraNr2); return (struct BufferItem *)0;	}

    while( bi1->biParent )
	{ bi1= bi1->biParent;	}
    while( bi2->biParent )
	{ bi2= bi2->biParent;	}
    if  ( bi1 != bi2 )
	{ XXDEB(bi1,bi2); return (struct BufferItem *)0;	}

    while( bi1->biChildCount > 0 )
	{
	int		i;

	for ( i= 0; i < bi1->biChildCount; i++ )
	    {
	    if  ( bi1->biChildren[i]->biLeftParagraphs >= paraNr1 )
		{ break;	}
	    }

	if  ( bi1->biChildren[i]->biLeftParagraphs < paraNr2 )
	    { return bi1;	}

	if  ( i >= bi1->biChildCount )
	    {
	    /* LLSDEB(paraNr,bi1->biChildCount,docLevelStr(bi1->biLevel)); */
	    return (struct BufferItem *)0;
	    }

	if  ( i > 0 )
	    {
	    paraNr1 -= bi1->biChildren[i-1]->biLeftParagraphs;
	    paraNr2 -= bi1->biChildren[i-1]->biLeftParagraphs;
	    }

	bi1= bi1->biChildren[i];
	}

    if  ( bi1->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(bi1->biLevel)); return (struct BufferItem *)0;	}

    if  ( paraNr1 != 1 || paraNr2 != 1 )
	{ LLDEB(paraNr1,paraNr2); return (struct BufferItem *)0; }

    return bi1;
    }

/************************************************************************/
/*									*/
/*  Return the nearest parent of a BufferItem that is a real row.	*/
/*  candidate row, cell.						*/
/*									*/
/************************************************************************/

struct BufferItem * docGetRowNode(		struct BufferItem *		node )
    {
    while( node && ! docIsRowNode( node ) )
	{ node= node->biParent;	}

    return node;
    }

struct BufferItem * docGetRowLevelNode(	struct BufferItem *		node )
    {
    while( node					&&
	   node->biLevel != DOClevROW		)
	{ node= node->biParent;	}

    return node;
    }

struct BufferItem * docGetCellNode(		struct BufferItem *		node )
    {
    while( node					&&
	   node->biLevel != DOClevCELL		)
	{ node= node->biParent;	}

    return node;
    }

/************************************************************************/
/*									*/
/*  Return the nearest parent of a BufferItem that is a section.	*/
/*									*/
/************************************************************************/

struct BufferItem * docGetSectNode(	struct BufferItem *		node )
    {
    while( node					&&
	   node->biLevel != DOClevSECT		)
	{ node= node->biParent;	}

    return node;
    }

/************************************************************************/
/*									*/
/*  Calculate table nesting: Follow the path to the root and count	*/
/*  the number of (table) rows that we traverse.			*/
/*									*/
/************************************************************************/

int docTableNesting(		const struct BufferItem *	node )
    {
    int			tableNesting= 0;

    while( node )
	{
	if  ( docIsRowNode( node ) )
	    { tableNesting++;	}

	node= node->biParent;
	}

    return tableNesting;
    }

int docRowNesting(		const struct BufferItem *	node )
    {
    int			rowNesting= 0;

    while( node )
	{
	if  ( node->biLevel == DOClevROW )
	    { rowNesting++;	}

	node= node->biParent;
	}

    return rowNesting;
    }

void docSetParaTableNesting(		struct BufferItem *		paraNode,
					const struct BufferDocument *	bd )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(paraNode->biLevel)); return;	}

    docParaNodeSetTableNesting( paraNode, docTableNesting( paraNode ), bd );
    }

/************************************************************************/
/*									*/
/*  Change the kind of tree for a node and all its children.		*/
/*									*/
/************************************************************************/

void docSetTreeTypeOfNode(	struct BufferItem *	node,
				int			treeType )
    {
    int		i;

    node->biTreeType= treeType;

    if  ( node->biLevel == DOClevSECT )
	{ node->biSectSelectionScope.ssTreeType= treeType; }

    for ( i= 0; i < node->biChildCount; i++ )
	{ docSetTreeTypeOfNode( node->biChildren[i], treeType );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Is a node at the extremity of a parent node.			*/
/*									*/
/************************************************************************/

int docNodeAtExtremity(	int *				pAtExtremity,
			const struct BufferItem *	parentNode,
			const struct BufferItem *	paraNode,
			int				after )
    {
    if  ( after )
	{
	DocumentPosition	dpTail;

	if  ( docTailPosition( &dpTail, (struct BufferItem *)parentNode ) )
	    { LDEB(1); return -1;	}

	*pAtExtremity= ( paraNode == dpTail.dpNode );
	}
    else{
	DocumentPosition	dpHead;

	if  ( docHeadPosition( &dpHead, (struct BufferItem *)parentNode ) )
	    { LDEB(1); return -1;	}

	*pAtExtremity= ( paraNode == dpHead.dpNode );
	}

    return 0;
    }

