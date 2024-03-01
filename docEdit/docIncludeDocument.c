/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>
#   include	<limits.h>

#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	"docCopyNode.h"
#   include	<docParaParticules.h>
#   include	<docField.h>
#   include	<docNodeTree.h>
#   include	<docParaProperties.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<docBuf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'TOC' )				*/
/*									*/
/*  B)  The first particule of the line was split, probably, part fits	*/
/*	on the previous line. Reformat from the previous particule.	*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*									*/
/*  1)  Replace the selection of the target with the text of those	*/
/*	particules at the beginning of the source that have the same	*/
/*	attributes.							*/
/*  2)  Insert the rest of the first paragraph of the source into the	*/
/*	target.								*/
/*  4)  If the insertion consists of more than one paragraph, split the	*/
/*	target paragraph.						*/
/*  5)  Insert the last particule of the insertion as text.		*/
/*									*/
/*  z)  Copy all paragraphs between the first and last (exclusive) of	*/
/*	the source to the target.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Delete the selected range.						*/
/*									*/
/************************************************************************/

static int docReplaceMultipleParagraphsWithSingle(
				DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsSource,
				int				partFrom,
				const int			paraNrHead )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;

    int				rval= 0;
    DocumentCopyJob		dcjTail;

    EditPosition		epSave;

    DocumentPosition		tailDp= eo->eoTailDp;

    docInitDocumentCopyJob( &dcjTail );

    if  ( eo->eoHeadDp.dpNode->biParaProperties->ppTableNesting > 0	&&
	  eo->eoTailDp.dpNode->biParaProperties->ppTableNesting == 0	)
	{ docFlattenRow( eo->eoHeadDp.dpNode, eo->eoDocument );	}

    /*  2  */
    if  ( docParaDeleteText( eo, paraNrHead, &tailDp,
		    &(eo->eoHeadDp), docParaStrlen( eo->eoHeadDp.dpNode ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docParaInsertTail( dcjInsert, paraNrHead,
				&tailDp, &tailDp,
				&(dsSource->dsHead), &(dsSource->dsTail) ) )
	{ LDEB(partFrom); rval= -1; goto ready;	}

    epSave= eo->eoAffectedRange.erTail;
    epSave.epParaNr= paraNrHead;
    epSave.epStroff= docParaStrlen( eo->eoHeadDp.dpNode );

    /*  3  */
    if  ( eo->eoTailDp.dpStroff < docParaStrlen( eo->eoTailDp.dpNode ) )
	{
	DocumentPosition	afterDp;

	if  ( docSet1DocumentCopyJob( &dcjTail, eo, CFH_STEAL ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( &dcjTail, paraNrHead,
				&afterDp, &tailDp,
				&(eo->eoTailDp), (DocumentPosition *)0 ) )
	    { LDEB(eo->eoTailDp.dpStroff); rval= -1; goto ready;	}
	}

    eo->eoTailDp= tailDp;

    eo->eoAffectedRange.erTail= epSave;
    eo->eoSelectedRange.erTail= epSave;

    /*  4  */
    if  ( docRemoveSelectionTail( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include a single paragraph in the current document. It will		*/
/*  replace the  current selection.					*/
/*									*/
/************************************************************************/

static int docInsertSingleParagraph(
				DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsSource,
				int				paraNrHead )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;

    int				rval= 0;

    const int			partFrom= 0;

    /*  1  */
    if  ( eo->eoHeadDp.dpNode != eo->eoTailDp.dpNode )
	{
	if  ( docReplaceMultipleParagraphsWithSingle( dcjInsert,
				    dsSource, partFrom, paraNrHead ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}
	}
    else{
	/*  5  */
	if  ( docParaDeleteText( eo, paraNrHead, &(eo->eoTailDp),
				    &(eo->eoHeadDp), eo->eoTailDp.dpStroff ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( dcjInsert, paraNrHead,
			&(eo->eoTailDp), &(eo->eoTailDp),
			&(dsSource->dsHead), &(dsSource->dsTail) ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}
	}

    if  ( ( dcjInsert->dcjCopyHeadParaProperties	||
	    dcjInsert->dcjCopyTailParaProperties	)		&&
	  docEditTransferParaProperties( eo, eo->eoTailDp.dpNode,
				      dsSource->dsHead.dpNode, 
				      0, &(dcjInsert->dcjAttributeMap) )	)
	{ LDEB(1); rval= -1; goto ready; }

    docEditFinishStep( eo );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Copy a node as part of an include action.				*/
/*									*/
/************************************************************************/

typedef struct TreeCopyJob
    {
					/**
					 *  The document that receives the
					 *  copied tree.
					 */
    struct BufferDocument *		tcjTargetDocument;

					/**
					 *  The selection in the source 
					 *  document that we copy.
					 */
    const struct DocumentSelection *	tcjSourceSelection;

					/**
					 *  If copying is limited to a 
					 *  selection of the source, do not 
					 *  go beyond this paragraph.
					 */
    int					tcjSourceParaNrLast;

					/**
					 *  The node whose children we are 
					 *  copying to the target.
					 */
    const struct BufferItem *		tcjSourceNode;

					/**
					 *  The node that receives the copied 
					 *  nodes.
					 */
    struct BufferItem *			tcjTargetNode;

					/**
					 *  The index in the target node where
					 *  copied children will be placed
					 */
    int					tcjTargetIndex;

    struct BufferItem *			tcjTailNode;
    } TreeCopyJob;

static int docInsertNextStep(		TreeCopyJob *		tcj )
    {
    const struct BufferItem *	srcNode= tcj->tcjSourceNode;
    const struct BufferItem *	nextParaNode;
    
    nextParaNode= docNextParagraph( (struct BufferItem *)srcNode );
    if  ( ! nextParaNode )
	{ return 1;	}
    if  ( tcj->tcjSourceSelection )
	{
	int	nextParaNumber= docNumberOfParagraph( nextParaNode );

	if  ( nextParaNumber > tcj->tcjSourceParaNrLast )
	    { return 1;	}
	}

    while( srcNode )
	{
	int	next= srcNode->biNumberInParent+ 1;

	if  ( srcNode->biParent				&&
	      next < srcNode->biParent->biChildCount	)
	    {
	    srcNode= srcNode->biParent->biChildren[next];
	    break;
	    }
	else{
	    srcNode= srcNode->biParent;
	    if  ( ! srcNode || ! srcNode->biParent )
		{ XDEB(srcNode); return -1;		}

	    if  ( ! tcj->tcjTargetNode )
		{ XDEB(tcj->tcjTargetNode); return -1;	}
	    if  ( tcj->tcjTargetIndex < tcj->tcjTargetNode->biChildCount )
		{
		struct BufferItem *	insertedNode= (struct BufferItem *)0;

		if  ( docSplitGroupNode( tcj->tcjTargetDocument,
					&insertedNode,
					tcj->tcjTargetNode,
					tcj->tcjTargetIndex ) )
		    { LDEB(1); return -1;	}

		tcj->tcjTargetIndex= insertedNode->biNumberInParent+ 1;
		tcj->tcjTargetNode= insertedNode->biParent;
		tcj->tcjTailNode= insertedNode;
		}
	    else{
		tcj->tcjTargetIndex= tcj->tcjTargetNode->biNumberInParent+ 1;
		tcj->tcjTargetNode= tcj->tcjTargetNode->biParent;
		tcj->tcjTailNode= tcj->tcjTargetNode;
		}
	    }
	}

    if  ( ! srcNode )
	{ XDEB(srcNode); return -1;	}

    tcj->tcjSourceNode= srcNode;
    return 0;
    }

static int docInsertCopyNode(	TreeCopyJob *		tcj,
				DocumentCopyJob *	dcjInsert )
    {
    struct BufferItem *	insertedNode;
    int			step;

    if  ( ! tcj->tcjTargetNode )
	{ XDEB(tcj->tcjTargetNode); return -1;	}

    insertedNode= docCopyNodeSelection( dcjInsert,
			tcj->tcjTargetNode, tcj->tcjTargetIndex,
			tcj->tcjSourceNode, tcj->tcjSourceSelection );
    if  ( ! insertedNode )
	{
	SSXDEB(docLevelStr(tcj->tcjSourceNode->biLevel),
			docLevelStr(tcj->tcjTargetNode->biLevel),insertedNode);
	return -1;
	}

    tcj->tcjTargetNode= insertedNode->biParent;
    tcj->tcjTargetIndex= insertedNode->biNumberInParent+ 1;
    tcj->tcjTailNode= insertedNode;

    /* Reached end of source? */
    if  ( tcj->tcjSourceSelection					&&
	  tcj->tcjSourceSelection->dsTail.dpNode == tcj->tcjSourceNode	)
	{ return 1;	}

    step= docInsertNextStep( tcj );
    if  ( step < 0 )
	{ LDEB(step); return step;	}

    return step;
    }

/************************************************************************/

static int docStartTreeCopyInsideTargetParagraph(
				TreeCopyJob *			tcj,
				EditOperation *			eo,
				int				headSplitLevel,
				const DocumentSelection *	dsSource )
    {
    BufferItem *		newParaNode;

    if  ( docEditSplitParaNode( &newParaNode, eo, headSplitLevel ) )
	{ LDEB(headSplitLevel); return -1;	}

    tcj->tcjTargetDocument= eo->eoDocument;
    tcj->tcjSourceSelection= dsSource;
    if  ( tcj->tcjSourceSelection )
	{
	tcj->tcjSourceParaNrLast=
		docNumberOfParagraph( tcj->tcjSourceSelection->dsTail.dpNode );
	}
    else{
	tcj->tcjSourceParaNrLast= INT_MAX;
	}

    if  ( headSplitLevel == DOClevPARA )
	{
	tcj->tcjTargetNode= newParaNode->biParent;
	tcj->tcjTargetIndex= newParaNode->biNumberInParent;
	tcj->tcjSourceNode= dsSource->dsHead.dpNode;
	tcj->tcjTailNode= eo->eoTailDp.dpNode;
	}
    else{
	struct BufferItem *	rowNode;

	rowNode= docGetRowLevelNode( eo->eoTailDp.dpNode );
	tcj->tcjTargetNode= rowNode->biParent;
	tcj->tcjTargetIndex= rowNode->biNumberInParent+ 1;
	tcj->tcjSourceNode= docGetRowNode( dsSource->dsHead.dpNode );
	tcj->tcjTailNode= rowNode;
	}

    return 0;
    }

/************************************************************************/

static int docStartTreeCopyBetweenTargetParagraphs(
				TreeCopyJob *			tcj,
				EditOperation *			eo,
				int				headSplitLevel,
				int				after,
				const DocumentSelection *	dsSource )
    {
    struct BufferItem *	rowNode;
    int			atExtremity;

    rowNode= docGetRowLevelNode( eo->eoTailDp.dpNode );

    if  ( docNodeAtExtremity( &atExtremity,
				rowNode, eo->eoTailDp.dpNode, after ) )
	{ LDEB(1); return -1;	}

    if  ( atExtremity )
	{
	tcj->tcjTargetNode= rowNode->biParent;
	tcj->tcjTargetIndex= rowNode->biNumberInParent+ after;

	tcj->tcjTargetDocument= eo->eoDocument;
	tcj->tcjSourceSelection= dsSource;
	if  ( tcj->tcjSourceSelection )
	    {
	    tcj->tcjSourceParaNrLast=
		    docNumberOfParagraph( tcj->tcjSourceSelection->dsTail.dpNode );
	    }
	else{
	    tcj->tcjSourceParaNrLast= INT_MAX;
	    }
	tcj->tcjSourceNode= docGetRowNode( dsSource->dsHead.dpNode );
	tcj->tcjTailNode= rowNode; /* Wrong for ! after, but irrelevant */

	docEditIncludeNodeInReformatRange( eo, rowNode->biParent );
	}
    else{
	struct BufferItem *		beforeNode;
	struct BufferItem *		afterNode;

	if  ( docEditSplitParaParent( eo, &beforeNode, &afterNode,
			    eo->eoTailDp.dpNode, after, headSplitLevel ) )
	    { LDEB(headSplitLevel); return -1;	}

	rowNode= docGetRowLevelNode( eo->eoTailDp.dpNode );
	tcj->tcjTargetNode= rowNode->biParent;
	tcj->tcjTargetIndex= rowNode->biNumberInParent+ after;

	tcj->tcjTargetDocument= eo->eoDocument;
	tcj->tcjSourceSelection= dsSource;
	if  ( tcj->tcjSourceSelection )
	    {
	    tcj->tcjSourceParaNrLast=
		    docNumberOfParagraph( tcj->tcjSourceSelection->dsTail.dpNode );
	    }
	else{
	    tcj->tcjSourceParaNrLast= INT_MAX;
	    }
	tcj->tcjSourceNode= docGetRowNode( dsSource->dsHead.dpNode );
	tcj->tcjTailNode= afterNode;

	if  ( after )
	    {
	    eo->eoAffectedRange.erTail.epParaNr=
			    docNumberOfParagraph( eo->eoTailDp.dpNode )+ 1;
	    eo->eoAffectedRange.erTail.epStroff= 0;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a multi paragraph document in the current document. It will	*/
/*  replace the  current selection.					*/
/*									*/
/*  1)  Delete the selection from the target document.			*/
/*  2)  Split the current paragraph upto the desired level.		*/
/*  3)  Append the first paragraph of the source to the current		*/
/*	paragraph.							*/
/*  4)  Insert the source document tree.				*/
/*	Append the items to the parents at the same level in the	*/
/*	target.								*/
/*  5)  Attach the bottom side of the inserted wedge to the document.	*/
/*	This is done by merging parents of the same level. [Table rows	*/
/*	are not merged.]						*/
/*									*/
/************************************************************************/

static int docInsertMultipleParagraphs(
				DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsSource,
				int				paraNrHead )
    {
    int				rval= 0;

    EditOperation *		eo= dcjInsert->dcjEditOperation;
    const struct BufferDocument * bdFrom= dcjInsert->dcjSourceDocument;

    DocumentCopyJob		dcjTail;

    const int			partFrom= 0;

    DocumentPosition		tailDp;
    int				sectShift;
    int				lastInserted;

    int				headSplitLevel= DOClevPARA;
    int				tailSplitLevel= DOClevPARA;

    struct BufferItem *		nextRowNode= (struct BufferItem *)0;

    TreeCopyJob			tcj;

    int				part;
    int				partFlags;

    docInitDocumentCopyJob( &dcjTail );

    /*  1  */
    if  ( docDeleteSelection( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! docSelectionInsideCell( dsSource ) )
	{
	const BufferItem *		tailNode= eo->eoTailDp.dpNode;
	const ParagraphProperties *	pp= tailNode->biParaProperties;
	int				tailTableNesting= pp->ppTableNesting;

	if  ( tailTableNesting == 0	&&
	      dsSource->dsHead.dpNode->biParaProperties->ppTableNesting > 0 )
	    { headSplitLevel= DOClevROW;	}

	if  ( tailTableNesting == 0	&&
	      dsSource->dsTail.dpNode->biParaProperties->ppTableNesting > 0 )
	    { tailSplitLevel= DOClevROW;	}
	}

    if  ( docFindParticuleOfPosition( &part, &partFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  2  */
    if  ( headSplitLevel == DOClevPARA				||
	  ! ( partFlags & (POSflagPARA_HEAD|POSflagPARA_TAIL) )	)
	{
	if  ( docStartTreeCopyInsideTargetParagraph( &tcj, eo,
						headSplitLevel, dsSource ) )
	    { LDEB(headSplitLevel); rval= -1; goto ready;	}
	}
    else{
	int	after= ( partFlags & POSflagPARA_HEAD ) == 0;

	if  ( docStartTreeCopyBetweenTargetParagraphs( &tcj, eo,
					headSplitLevel, after, dsSource ) )
	    { LDEB(headSplitLevel); rval= -1; goto ready;	}
	}

    if  ( headSplitLevel == DOClevPARA )
	{
	int	step;

	/*  3  */
	docTailPosition( &tailDp, eo->eoTailDp.dpNode );

	if  ( docParaInsertTail( dcjInsert, paraNrHead,
				&tailDp, &tailDp,
				&(dsSource->dsHead), &(dsSource->dsTail) ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}

	if  ( dcjInsert->dcjCopyHeadParaProperties )
	    {
	    if  ( docEditTransferParaProperties( eo, tailDp.dpNode,
				  dsSource->dsHead.dpNode,
				  0, &(dcjInsert->dcjAttributeMap) )	)
		{ LDEB(1); rval= -1; goto ready; }

	    tailDp.dpNode->biParaTocLevel= dsSource->dsHead.dpNode->biParaTocLevel;
	    }

	step= docInsertNextStep( &tcj );
	if  ( step < 0 )
	    { LDEB(step); rval= -1; goto ready;	}
	if  ( step > 0 )
	    { tcj.tcjSourceNode= (const struct BufferItem *)0;	}
	}

    /*  4  */
    while( tcj.tcjSourceNode )
	{
	int step= docInsertCopyNode( &tcj, dcjInsert );
	if  ( step < 0 )
	    { LDEB(step); rval= -1; goto ready;	}
	if  ( step > 0 )
	    { break;	}
	}

    if  ( docTailPosition( &tailDp, tcj.tcjTailNode ) )
	{ XDEB(tcj.tcjTargetNode); rval= -1; goto ready;	}

    if  ( tailSplitLevel == DOClevROW )
	{
	DocumentPosition	dpNext;

	dpNext= tailDp;
	if  ( ! docNextPosition( &dpNext ) )
	    { nextRowNode= docGetRowLevelNode( dpNext.dpNode );	}
	}

    lastInserted= docNumberOfParagraph( tailDp.dpNode );
    sectShift= 0;

    /*  5  */
    while( tcj.tcjTailNode->biLevel != DOClevPARA )
	{
	struct BufferItem *	parentNode= tcj.tcjTailNode->biParent;
	int			n= tcj.tcjTailNode->biNumberInParent;
	int			lastChild= tcj.tcjTailNode->biChildCount- 1;

	if  ( parentNode == nextRowNode )
	    { break;	}

	if  ( n+ 1 < parentNode->biChildCount )
	    {
	    int			sectionsDeleted;
	    struct BufferItem *	mergedNode;

	    mergedNode= parentNode->biChildren[n+ 1];

	    if  ( mergedNode == nextRowNode )
		{ break;	}

	    if  ( docMergeGroupNodes( eo->eoDocument, tcj.tcjTailNode, mergedNode ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    sectionsDeleted= 0;
	    docDeleteEmptyParents( eo, &sectionsDeleted, mergedNode );
	    sectShift -= sectionsDeleted;
	    }

	tcj.tcjTailNode= tcj.tcjTailNode->biChildren[lastChild];
	}

    if  ( sectShift != 0 )
	{
	const int	paraNr= lastInserted+ 1;
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	paraShift= 0;

	eo->eoSectionsDeleted -= sectShift;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );
	}

    if  ( bdFrom->bdProperties->dpHasOpenEnd				&&
	  tailSplitLevel == DOClevPARA					&&
	  tailDp.dpNode->biNumberInParent <
			  tailDp.dpNode->biParent->biChildCount -1	)
	{
	const int		partTailFrom= 0;
	DocumentPosition	dpTailFrom;

	DocumentPosition	dpTailTo;

	int			paraNrTail;

	EditPosition		affSav= eo->eoAffectedRange.erTail;

	eo->eoTailDp= tailDp;

	docSetEditPosition( &(eo->eoAffectedRange.erTail), &(eo->eoTailDp) );
	affSav= eo->eoAffectedRange.erTail;

	paraNrTail= docNumberOfParagraph( tailDp.dpNode );

	dpTailFrom= eo->eoTailDp;
	if  ( docNextPosition( &dpTailFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docTailPosition( &(eo->eoLastDp), dpTailFrom.dpNode ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docSet1DocumentCopyJob( &dcjTail, eo, CFH_STEAL ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( &dcjTail, paraNrTail,
				&dpTailTo, &(eo->eoTailDp),
				&dpTailFrom, (DocumentPosition *)0 ) )
	    { LDEB(partTailFrom); rval= -1; goto ready;	}

	if  ( docRemoveSelectionTail( eo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( dcjInsert->dcjCopyTailParaProperties			&&
	      docEditTransferParaProperties( eo, dpTailTo.dpNode,
				  dsSource->dsTail.dpNode,
				  0, &(dcjInsert->dcjAttributeMap) )	)
	    { LDEB(1); rval= -1; goto ready; }

	eo->eoAffectedRange.erTail= affSav;
	eo->eoSelectedRange.erTail= affSav;
	}

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include a document in the current document. It will replace the	*/
/*  current selection.							*/
/*									*/
/*  1)  Find out what to insert.					*/
/*  2)  Insert text belonging to a single paragraph.			*/
/*  3)  Insert text belonging to a multiple paragraphs.			*/
/*									*/
/************************************************************************/

int docIncludeSelection(	DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsSource )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;
    struct BufferDocument *	bdFrom= dcjInsert->dcjSourceDocument;

    DocumentSelection		dsTo;

    int				paraNrHead;

    paraNrHead= docNumberOfParagraph( eo->eoHeadDp.dpNode );

    if  ( docEditDeleteReplacedFields( eo ) )
	{ LDEB(1);	}

#   if SHOW_SELECTION_RANGE
    appDebug( "INCLUDE.. %3d:%-3d .. %3d:%-3d :: %s\n",
		    eo->eoAffectedRange.erHead.epParaNr,
		    eo->eoAffectedRange.erHead.epStroff,
		    eo->eoAffectedRange.erTail.epParaNr,
		    eo->eoAffectedRange.erTail.epStroff,
		    bdFrom->bdProperties.dpHasOpenEnd?"OPEN":"CLOSED");
#   endif

    docEditOperationGetSelection( &dsTo, eo );

    /*  2,3  */
    if  ( bdFrom->bdProperties->dpHasOpenEnd			&&
	  dsSource->dsHead.dpNode == dsSource->dsTail.dpNode	)
	{
	if  ( docInsertSingleParagraph( dcjInsert, dsSource, paraNrHead ) )
	    { LDEB(1); return -1;	}
	}
    else{
	const int	recursively= 1;

	if  ( docInsertMultipleParagraphs( dcjInsert, dsSource, paraNrHead ) )
	    { LDEB(1); return -1;	}


	docDelimitTables( eo->eoTree->dtRoot, recursively );
	}

    if ( utilIndexSetUnion( &(eo->eoNoteFieldsAdded),
				    &(eo->eoNoteFieldsAdded),
				    &(dcjInsert->dcjNoteFieldsCopied) ) > 0 )
	{ eo->eoFieldUpdate |= FIELDdoCHFTN;	}
    if  ( dcjInsert->dcjBulletsCopied > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    return 0;
    }

int docIncludeDocument(		DocumentCopyJob *		dcjInsert )
    {
    struct BufferDocument *	bdFrom= dcjInsert->dcjSourceDocument;

    DocumentSelection		dsSource;

    /*  1  */
    if  ( docHeadPosition( &(dsSource.dsHead), bdFrom->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition(  &(dsSource.dsTail), bdFrom->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}
    if  ( docSelectWholeBody( &dsSource, bdFrom ) )
	{ LDEB(1); return -1;	}

    return docIncludeSelection( dcjInsert, &dsSource );
    }

/************************************************************************/
/*									*/
/*  Wrapper around docIncludeDocument(): More relayout admin.		*/
/*									*/
/************************************************************************/

int docEditIncludeDocument(	EditOperation *			eo,
				struct DocumentCopyJob *	dcj )
    {
    int			beginStrlenOld= docParaStrlen( eo->eoHeadDp.dpNode );

    if  ( docIncludeDocument( dcj ) )
	{ LDEB(1); return -1;	}

    if  ( eo->eoParagraphsInserted == 0 )
	{
	int	beginStrlenNew= docParaStrlen( eo->eoHeadDp.dpNode );
	int	stroffTail= eo->eoSelectedRange.erTail.epStroff;
	int	stroffShift= beginStrlenNew- beginStrlenOld;

	/*  C  */
	docSetParagraphAdjust( eo, eo->eoHeadDp.dpNode,
						stroffShift, stroffTail );
	}

    return 0;
    }

