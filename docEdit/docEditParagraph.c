/************************************************************************/
/*									*/
/*  Paragraph manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<limits.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"
#   include	<docPropVal.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<utilPropMask.h>
#   include	<docFields.h>
#   include	<docParaBuilder.h>
#   include	<docParaParticuleAdmin.h>

#   include	<appDebugon.h>

#   define	DEB_PARTICULES		0
#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Replace text in a paragraph.					*/
/*									*/
/*  B)  If we are at the head of the particule and the previous one	*/
/*	has the same text attribute as the insertion source include	*/
/*	the previous particule in the reformat. This prevents the	*/
/*	creation of adjacent particules with the same text attribute.	*/
/*  C)  If only part of the head particule is covered, decide to split	*/
/*	it.								*/
/*  D)  If we are at the tail of the particule and the next one has the	*/
/*	same text attribute as the insertion source include the next	*/
/*	particule in the reformat. (Why? See B). Remember that unless	*/
/*	the length of a particule equals zero, we should never be at	*/
/*	the end.							*/
/*  E)  If only part of the head particule is covered, decide to split	*/
/*	it.								*/
/*  F)  Clean all particules affected. Field particules should have	*/
/*	been cleaned by the caller. Remember that replacements never	*/
/*	contain only part of a field or bookmark. The caller takes care	*/
/*	of this.							*/
/*  G)  Insert the byes of the string into the paragraph.		*/
/*  H)  Redivide the new bytes and the old bytes from (B,D) in		*/
/*	particules.							*/
/*  I)  Delete any particules left over. Make sure that at the		*/
/*	paragraph has at least one particule.				*/
/*  J)  Shift the offsets of any particules after the replacement.	*/
/*  K)  Return tail position and particule numbers to the caller. If	*/
/*	the head or tail particules were merged, recalculate the	*/
/*	particule numbers. As the positions are inside a series of	*/
/*	spans, the special cases for different kinds of particules do	*/
/*	not apply.							*/
/*									*/
/************************************************************************/

int docParaReplaceText(	EditOperation *			eo,
			int				paraNr,
			DocumentPosition *		dpNewTail,
			const DocumentPosition *	dpHead,
			unsigned int			stroffReplaceTail,
			const char *			addedText,
			unsigned int			addedLength,
			int				addedAttributeNr )
    {
    int				rval= 0;

    int				partHead;
    int				partUpto;

    int				splitHead= 0;
    int				splitTail= 0;
    int				mergeHead= 0;
    int				mergeTail= 0;

    int				stroffRedivideHead;
    int				stroffRedivideTail;
    int				partsMade;

    int				stroffShift;

    DocumentPosition		dpTail= *dpHead;

    dpTail.dpStroff= stroffReplaceTail;

    /*  B,C  */
    if  ( docParaSubstituteFindHeadPositions( &partHead, &stroffRedivideHead,
					    &splitHead, &mergeHead,
					    dpHead, addedAttributeNr ) )
	{ LDEB(dpHead->dpStroff); rval= -1; goto ready;	}

    if  ( addedLength == 0 || addedAttributeNr < 0 )
	{
	addedAttributeNr=
		dpHead->dpNode->biParaParticules[partHead].tpTextAttrNr;
	}

    /*  D,E  */
    if  ( docParaSubstituteFindTailPositions( &partUpto, &stroffRedivideTail,
					    &splitTail, &mergeTail,
					    &dpTail, addedAttributeNr ) )
	{ LDEB(dpTail.dpStroff); rval= -1; goto ready;	}

    /*  C  */
    if  ( splitHead )
	{
	if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
					dpHead->dpNode, dpHead->dpStroff ) )
	    { LDEB(dpHead->dpStroff); rval= -1; goto ready;	}

	if  ( docParaBuilderSplitTextParticule( (TextParticule **)0,
					(TextParticule **)0,
					eo->eoParagraphBuilder,
					partHead, dpHead->dpStroff ) )
	    { LDEB(partHead); rval= -1; goto ready;	}

	partHead++;	/*  Move to second half		*/
	partUpto++;	/*  Split: shift number		*/

	/* redivide will take care of splitting the tail */
	if  ( dpHead->dpStroff == stroffReplaceTail )
	    { splitTail= 0;	}
	}

    /*  E  */
    if  ( splitTail )
	{
	if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
					dpHead->dpNode, stroffReplaceTail ) )
	    { LDEB(dpHead->dpStroff); rval= -1; goto ready;	}

	if  ( docParaBuilderSplitTextParticule( (TextParticule **)0,
					(TextParticule **)0,
					eo->eoParagraphBuilder,
					partUpto- 1, stroffReplaceTail ) )
	    { LDEB(partHead); rval= -1; goto ready;	}

	stroffRedivideTail= stroffReplaceTail;
	}

    if  ( dpHead->dpNode->biParaParticuleCount == 1			&&
	  dpHead->dpNode->biParaParticules[0].tpKind == TPkindSPAN	&&
	  dpHead->dpNode->biParaParticules[0].tpStrlen == 0		)
	{ partHead= 0;	}

    /*  F  */
    if  ( docEditCleanParticules( eo, dpHead->dpNode, partHead, partUpto ) )
	{ LLDEB(partHead,partUpto); rval= -1; goto ready;	}

    if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
					dpHead->dpNode, dpHead->dpStroff ) )
	{ LDEB(dpHead->dpStroff); rval= -1; goto ready;	}

    /*  G  */
    stroffShift= 0;
    partsMade= docParaBuilderSubstitute( &stroffShift, eo->eoParagraphBuilder,
			dpHead->dpStroff, stroffReplaceTail,
			stroffRedivideHead, stroffRedivideTail,
			partHead, partUpto,
			addedText, addedLength, addedAttributeNr );
    if  ( partsMade < 0 )
	{ LDEB(partsMade); rval= -1; goto ready;	}

    /*  I  */
    if  ( dpHead->dpNode->biParaParticuleCount == 0 )
	{
	TextParticule *	tp;

	if  ( partHead != 0 || docParaStrlen( dpHead->dpNode ) != 0 )
	    { LLDEB(partHead,docParaStrlen(dpHead->dpNode));	}

	tp= docInsertTextParticule( dpHead->dpNode, partHead,
				    docParaStrlen( dpHead->dpNode ), 0,
				    TPkindSPAN, addedAttributeNr );
	if  ( ! tp )
	    { XDEB(tp); rval= -1; goto ready;	}
	partsMade++;

	/* No.. Gives problems in the incremental formatter!
	docInvalidateParagraphLayout( paraNode );
	*/
	}


    /*  J  */
    if  ( docEditShiftParticuleOffsets( eo, dpHead->dpNode, paraNr,
		    partHead+ partsMade, dpHead->dpStroff, stroffShift ) )
	{ LDEB(stroffShift);	}

#   if DEB_PARTICULES
    {
    TextParticule *	tp;
    int			p;

    appDebug( "\n" );

    p= 0; tp= paraNode->biParaParticules;
    while( p < paraNode->biParaParticuleCount )
	{
	const int	indent= 0;
	docListParticule( indent, "===", p, paraNode, tp );
	p++; tp++;
	}
    }
#   endif

    /*  K  */
    docSetDocumentPosition( dpNewTail,
				dpHead->dpNode, dpHead->dpStroff+ addedLength );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete text: Replace with nothing.					*/
/*									*/
/************************************************************************/

int docParaDeleteText(		EditOperation *			eo,
				int				paraNr,
				DocumentPosition *		dpTail,
				const DocumentPosition *	dpHead,
				unsigned int			stroffTail )
    {
    return docParaReplaceText( eo, paraNr, dpTail,
					dpHead, stroffTail,
					(const char *)0, 0, -1 );
    }

/************************************************************************/
/*									*/
/*  Merge a particule from the source into a particule of the target.	*/
/*  (Both are assumed to be text spans with the same attributes)	*/
/*									*/
/*  1)  Include the text of the first paricule of the source in the	*/
/*	target.	(This may merge on the right hand side as well: see	*/
/*	below.								*/
/*  2)  Bookkeeping on how far we have advanced.			*/
/*  3)  If the end of the insertion now is in the middle of a particule:*/
/*	split it.							*/
/*  4)  Bookkeeping on how far we have advanced.			*/
/*									*/
/************************************************************************/

static int docParaMergeParticule(
				EditOperation *			eo,
				const int			paraNrTo,
				const int			textAttrTo,
				DocumentPosition *		dpTo,
				int *				pPartTo,
				DocumentPosition *		dpFrom,
				int *				pPartFrom )
    {
    struct BufferItem *		nodeFrom= dpFrom->dpNode;
    const char *		addedString;
    int				addedStrlen;
    const TextParticule *	tpTo;

    const TextParticule * const tpFrom= nodeFrom->biParaParticules+ *pPartFrom;

    /*  1  */
    addedString= docParaString( nodeFrom, dpFrom->dpStroff );
    addedStrlen= tpFrom->tpStroff+ tpFrom->tpStrlen- dpFrom->dpStroff;

    if  ( docParaReplaceText( eo, paraNrTo, dpTo, dpTo, dpTo->dpStroff,
				    addedString, addedStrlen, textAttrTo ) )
	{ LDEB(addedStrlen); return -1;	}

    /*  2:  *pPartFrom May point outside source */
    docSetDocumentPosition( dpFrom, nodeFrom,
				    tpFrom->tpStroff+ tpFrom->tpStrlen );
    (*pPartFrom)++;

    if  ( docFindParticuleOfPosition( pPartTo, (int *)0,
						    dpTo, PARAfindPAST ) )
	{ LDEB(dpTo->dpStroff); return -1;	}

    tpTo= dpTo->dpNode->biParaParticules+ *pPartTo;

    /*  3  */
    if  ( *pPartTo < dpTo->dpNode->biParaParticuleCount		&&
	  dpTo->dpStroff > tpTo->tpStroff			&&
	  dpTo->dpStroff < tpTo->tpStroff+ tpTo->tpStrlen	)
	{
	if  ( docSplitTextParticule( (TextParticule **)0, (TextParticule **)0,
				    dpTo->dpNode, *pPartTo, dpTo->dpStroff ) )
	    { LDEB(*pPartTo); return -1;	}
	}

    /*  4  */
    if  ( docFindParticuleOfPosition( pPartTo, (int *)0,
						dpTo, PARAfindPAST ) )
	{ LDEB(dpTo->dpStroff); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert/Append the tail of a paragraph into another paragraph.	*/
/*									*/
/*  A)  Never copy the head field of the source.			*/
/*  B)  If the head of the source is a span with the same text		*/
/*	attributes as the target insert it as text. This takes care of	*/
/*	the correct distribution of the text over the spans.		*/
/*  C)  Manually copy a partial particule (Not handled by B)		*/
/*	Unless the arguments are silly, this only occurs for SPAN	*/
/*	particules. (As the implementation is identical to that of (B)	*/
/*	the cases are merged.)						*/
/*  D)  If the tail of the source is a span with the same text		*/
/*	attributes as the target we will insert it as text.		*/
/*  E)  Copy particules not covered by B,C,D=F.				*/
/*  F)  Insert tail as text (D). This takes care of the correct		*/
/*	distribution of the text over the spans. There is no partial	*/
/*	span case here as we completely copy the tail of the source.	*/
/*									*/
/************************************************************************/

int docParaInsertTail(		DocumentCopyJob *		dcj,
				int				paraNrTo,
				DocumentPosition *		dpTail,
				const DocumentPosition *	dp_To,
				const DocumentPosition *	dp_From )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    DocumentPosition		dpTo= *dp_To;
    DocumentPosition		dpFrom= *dp_From;
    int				rval= 0;

    struct BufferItem *		nodeTo= dpTo.dpNode;
    const struct BufferItem *		nodeFrom= dpFrom.dpNode;

    int				textAttrTo= INT_MAX;	/*  Impossible	*/

    int				copyLastAsText= 0;
    int				particulesCopied;

    int				partTo;
    int				partFrom;

    int				splitHead= 0;
    int				mergeHead= 0;
    int				fromFlags= 0;

    if  ( docFindParticuleOfPosition( &partFrom, &fromFlags,
						    &dpFrom, PARAfindPAST ) )
	{ LDEB(1); return -1; }

    /*  A  */
    if  ( nodeFrom->biParaProperties->ppListOverride > 0 )
	{
	struct DocumentField *	dfHead= (struct DocumentField *)0;
	DocumentSelection	dsInsideHead;
	DocumentSelection	dsAroundHead;
	int			partHead= -1;
	int			partTail= -1;

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
		    &partHead, &partTail, nodeFrom, dcj->dcjSourceDocument ) )
	    { /*LDEB(nodeFrom->biParaListOverride);*/	}
	else{
	    if  ( dpFrom.dpStroff < dsAroundHead.dsTail.dpStroff )
		{ dpFrom.dpStroff=  dsAroundHead.dsTail.dpStroff;	}
	    if  ( partFrom < partTail+ 1 )
		{ partFrom=  partTail+ 1;	}
	    }
	}

    if  ( partFrom >= nodeFrom->biParaParticuleCount )
	{ goto done;	}

    {
    const TextParticule * const	tpFrom= nodeFrom->biParaParticules+ partFrom;

    if  ( tpFrom->tpKind == TPkindSPAN )
	{ textAttrTo= docMapTextAttributeNumber( dcj, tpFrom->tpTextAttrNr ); }
    else{ textAttrTo= INT_MAX; /* not an existing value */		}
    }

    /*  B,C  */
    if  ( docParaSubstituteFindHeadPositions( &partTo, (int *)0,
				&splitHead, &mergeHead, &dpTo, textAttrTo ) )
	{ LDEB(dpTo.dpStroff); rval= -1; goto ready;	}

    /*  B,C  */
    if  ( splitHead )
	{
	if  ( docSplitTextParticule( (TextParticule **)0, (TextParticule **)0,
					dpTo.dpNode, partTo, dpTo.dpStroff ) )
	    { LDEB(partTo); rval= -1; goto ready;	}

	partTo++;
	}

    if  ( mergeHead )
	{
	if  ( docParaMergeParticule( eo, paraNrTo, textAttrTo,
				    &dpTo, &partTo, &dpFrom, &partFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	const TextParticule * const tpFrom= nodeFrom->biParaParticules+ partFrom;
	if  ( tpFrom->tpKind == TPkindSPAN		&&
	      ! ( fromFlags & POSflagPARA_TAIL )	)
	    {
	    if  ( docParaMergeParticule( eo, paraNrTo, textAttrTo,
				    &dpTo, &partTo, &dpFrom, &partFrom ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}

    if  ( partFrom >= nodeFrom->biParaParticuleCount )
	{ goto done;	}

    /*  D  */
    if  ( dpTo.dpStroff < docParaStrlen( dpTo.dpNode ) )
	{
	const TextParticule * const	tpFrom=
		nodeFrom->biParaParticules+ nodeFrom->biParaParticuleCount- 1;

	const TextParticule * const	tpTo= nodeTo->biParaParticules+ partTo;

	textAttrTo= docMapTextAttributeNumber( dcj, tpFrom->tpTextAttrNr );

	copyLastAsText= 0;

	if  ( partTo < nodeTo->biParaParticuleCount	&&
	      tpFrom->tpKind == TPkindSPAN		&&
	      tpTo->tpKind == TPkindSPAN		&&
	      textAttrTo == tpTo->tpTextAttrNr		)
	    { copyLastAsText= 1;	}
	}

    /*  E  */
    particulesCopied= nodeFrom->biParaParticuleCount- partFrom- copyLastAsText;
    if  ( particulesCopied > 0 )
        {
	int		particulesInserted= 0;
	int		charactersInserted= 0;

        if  ( docCopyParticules( dcj, nodeTo, nodeFrom, partTo, partFrom,
                                    particulesCopied,
                                    &particulesInserted,
                                    &charactersInserted ) )
            { LDEB(1); rval= -1; goto ready;       }

        partTo += particulesInserted;
        dpTo.dpStroff += charactersInserted;

	/* May point outside source */
	partFrom += particulesInserted;
        dpFrom.dpStroff += charactersInserted;
        }
    else{
        if  ( particulesCopied < 0 )
            { LDEB(particulesCopied); particulesCopied= 0;      }
        }

    /*  F  */
    if  ( copyLastAsText > 0 )
	{
	const char *	addedString;
	int		addedStrlen;

	addedString= docParaString( nodeFrom, dpFrom.dpStroff );
	addedStrlen= docParaStrlen( nodeFrom )- dpFrom.dpStroff;

	if  ( docParaReplaceText( eo, paraNrTo, &dpTo,
				    &dpTo, dpTo.dpStroff,
				    addedString, addedStrlen, textAttrTo ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	/* May point outside source */
	partFrom++; dpFrom.dpStroff += addedStrlen;
	}

  done:

    *dpTail= dpTo;

  ready:

    return rval;
    }

static void docEditShiftSplitReferences(EditOperation *		eo,
					int			splitLevel )
    {
    const struct BufferItem *	oldParaNode= eo->eoTailDp.dpNode;
    const int		paraNr= docNumberOfParagraph( oldParaNode );
    const int		stroffFrom= eo->eoTailDp.dpStroff;
    const int		paraShift= 1;
    const int		stroffShift= -stroffFrom;
    const int		sectShift= splitLevel <= DOClevSECT;

    if  ( stroffFrom == 0 )
	{ eo->eoSplitAtHead=1;	}

    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );

    eo->eoSectionsAdded += sectShift;

    return;
    }

/************************************************************************/
/*									*/
/*  Upon splittong a paragraph: Move the piece after the split to the	*/
/*  new paragraph.							*/
/*									*/
/************************************************************************/

static int docMoveTailToNew(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				struct BufferItem *		newNode,
				struct BufferItem *		oldNode,
				int			part )
    {
    TextParticule *	newTp;
    int			stroff= eo->eoTailDp.dpStroff;

    int			stroffShift= 0;
    int			newStrlen= 0;
    int			partShift= 0;
    TextParticule *	tp;

    int			truncatedParticuleCount= part;

    /*  2  */
    if  ( docParaStringReplace( &stroffShift, newNode,
				    docParaStrlen( newNode ),
				    docParaStrlen( newNode ),
				    docParaString( oldNode, stroff ),
				    docParaStrlen( oldNode )- stroff ) )
	{ LLDEB(docParaStrlen(oldNode),stroff); return -1; }

    /*  4  */
    newStrlen= 0;
    tp= oldNode->biParaParticules+ part;
    while( part < oldNode->biParaParticuleCount )
	{
	switch( tp->tpKind )
	    {
	    default:
		LDEB(tp->tpKind);
		newStrlen += tp->tpStrlen; part++; tp++;
		continue;

	    case TPkindFIELDTAIL:
		/*  5  */
		break;

	    case TPkindFIELDHEAD:
		break;

	    case TPkindOBJECT:
	    case TPkindSPAN:
	    case TPkindTAB:
	    case TPkindLINEBREAK:
	    case TPkindPAGEBREAK:
	    case TPkindCOLUMNBREAK:
		break;
	    }

	if  ( docCopyParticuleAndData( &newTp, dcj, newNode, partShift++,
					newStrlen, tp->tpStrlen, tp ) )
	    { LDEB(part); return -1;	}

	newStrlen += tp->tpStrlen; part++; tp++;
	}

    oldNode->biParaParticuleCount= truncatedParticuleCount;
    docParaSetStrlen( oldNode, stroff );

    if  ( newNode->biParaParticuleCount == 0 )
	{
	tp= oldNode->biParaParticules+ oldNode->biParaParticuleCount- 1;

	newTp= docInsertTextParticule( newNode, -1, 0, 0,
				    TPkindSPAN, tp->tpTextAttrNr );
	if  ( ! newTp )
	    { LDEB(newNode->biParaParticuleCount); return -1; }
	}

    if  ( oldNode->biParaParticuleCount == 0 )
	{
	tp= oldNode->biParaParticules;

	newTp= docInsertTextParticule( oldNode, -1, 0, 0,
				    TPkindSPAN, tp->tpTextAttrNr );
	if  ( ! newTp )
	    { LDEB(oldNode->biParaParticuleCount); return -1; }
	}
    else{ docCheckNoBreakAsLast( eo, oldNode );	}


    return 0;
    }

/************************************************************************/

int docEditSplitParaParent(	EditOperation *		eo,
				struct BufferItem **		pBeforeNode,
				struct BufferItem **		pAfterNode,
				struct BufferItem *		paraNode,
				int			after,
				int			splitLevel )
    {
    struct BufferItem *	beforeNode;
    struct BufferItem *	afterNode;

    after= after != 0;

    if  ( docSplitGroupNodeAtLevel( eo->eoDocument, &beforeNode, &afterNode,
			paraNode->biParent, paraNode->biNumberInParent+ after,
			splitLevel ) )
	{ LDEB(splitLevel); return -1;	}

    if  ( afterNode && afterNode->biParent )
	{ docEditIncludeNodeInReformatRange( eo, afterNode->biParent );	}
    else{ XDEB(afterNode);						}

    if  ( pBeforeNode )
	{ *pBeforeNode= beforeNode;	}
    if  ( pAfterNode )
	{ *pAfterNode= afterNode;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  a)  Find the first particule that ends on or after the given	*/
/*	position. If several particules apply, choose the one with the	*/
/*	lowest level of field nesting.					*/
/*  1)  Insert a paragraph AFTER the current one.			*/
/*  1a) Copy indents and tab stops.					*/
/*  2)  Allocate space for the string of the successor.			*/
/*  3)  If the position is in the middle of a particule, split it.	*/
/*  4)  Move the rest of the particules to the new paragraph.		*/
/*  5)  Keep field ends at the split position in the old paragraph.	*/
/*	So do not insert field ends in the new paragraph. They are	*/
/*	reinserted at the end of the old paragraph.			*/
/*									*/
/************************************************************************/

int docSplitParaNode(		struct BufferItem **	pNewParaNode,
				EditOperation *		eo,
				int			splitLevel )
    {
    int				rval= 0;

    struct BufferItem *		oldParaNode= eo->eoTailDp.dpNode;
    struct BufferItem *		newParaNode;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    DocumentCopyJob		dcj;

    int				part;
    int				headFlags;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet1DocumentCopyJob( &dcj, eo, CFH_STEAL ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFindParticuleOfPosition( &part, &headFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(eo->eoTailDp.dpStroff); rval= -1; goto ready;	}

    if  ( ! ( headFlags & (POSflagPART_HEAD|POSflagPARA_TAIL) ) )
	{
	if  ( docSplitTextParticule( (TextParticule **)0,
				    (TextParticule **)0, eo->eoTailDp.dpNode,
				    part, eo->eoTailDp.dpStroff ) )
	    { LXDEB(part,headFlags); rval= -1; goto ready;	}

	part++;
	}

#   if SHOW_SELECTION_RANGE
    appDebug( "SPLIT     %3d(%3d)\n",
		docNumberOfParagraph( oldParaNode ), eo->eoTailDp.dpStroff );
#   endif

    /*  1  */
    newParaNode= docInsertNode( eo->eoDocument, oldParaNode->biParent,
		    oldParaNode->biNumberInParent+ 1, oldParaNode->biLevel );
    if  ( ! newParaNode )
	{ XDEB(newParaNode); rval= -1; goto ready;	}

    if  ( splitLevel < DOClevPARA )
	{
	const int	after= 1;
	struct BufferItem *	beforeNode;
	struct BufferItem *	afterNode;

	if  ( docEditSplitParaParent( eo, &beforeNode, &afterNode, oldParaNode,
							after, splitLevel ) )
	    { LDEB(splitLevel); return -1;	}

	docEditShiftSplitReferences( eo, splitLevel );
	}
    else{
	docEditShiftSplitReferences( eo, splitLevel );
	}

    /*  2  */
    if  ( docMoveTailToNew( eo, &dcj, newParaNode, oldParaNode, part ) )
	{
	LLDEB(docParaStrlen(oldParaNode),eo->eoTailDp.dpStroff);
	/* Try to undo! */
	docDeleteNodes( eo->eoDocument, eo->eoTree, oldParaNode->biParent,
					oldParaNode->biNumberInParent+ 1, 1 );
	rval= -1; goto ready;
	}

    if  ( splitLevel >= DOClevPARA )
	{
	/*  2, 4  */
	docEditIncludeNodeInReformatRange( eo, oldParaNode );
	docEditIncludeNodeInReformatRange( eo, newParaNode );
	}

    utilPropMaskClear( &ppChgMask );
    utilPropMaskClear( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );

    /*  1a  */
    if  ( docEditUpdParaProperties( eo, &ppChgMask, newParaNode, &ppUpdMask,
				oldParaNode->biParaProperties,
				&(dcj.dcjAttributeMap) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( oldParaNode->biParaProperties->ppListOverride > 0 )
	{ dcj.dcjBulletsCopied++;		}
    if  ( newParaNode->biParaProperties->ppListOverride > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    docInvalidateParagraphLayout( newParaNode );
    docInvalidateParagraphLayout( oldParaNode );

    *pNewParaNode= newParaNode;

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a tab or another kind of special particule for something	*/
/*  that takes up some space in the document.				*/
/*									*/
/*  1)  Insert a space in the text string.				*/
/*  2)  Replace the empty particule in an empty paragraph.		*/
/*  3)  Can be inserted before the current particule.			*/
/*  4)  Can be inserted after the current particule.			*/
/*  5)  First split the particule,					*/
/*  6)  Then insert between the two halves.				*/
/*  7)  Shift particule offsets in the rest of the paragraph.		*/
/*									*/
/************************************************************************/

static TextParticule * docParaSpecialParticule(
				EditOperation *			eo,
				int				kind,
				int *				pPartShift,
				int *				pStroffShift )
    {
    TextParticule *		tp;
    TextParticule *		tpRet;
    int				stroffShift= 0;

    DocumentPosition *		dp= &(eo->eoTailDp);
    int				tailPart;
    int				tailFlags= 0;
    int				paraNr= docNumberOfParagraph( dp->dpNode );

    if  ( docFindParticuleOfPosition( &tailPart, &tailFlags,
						    dp, PARAfindPAST ) )
	{ LDEB(1); return (TextParticule *)0; }

    /*  1  */
    if  ( docParaStringReplace( &stroffShift,
			dp->dpNode, dp->dpStroff, dp->dpStroff, " ", 1 ) )
	{ LDEB(dp->dpStroff); return (TextParticule *)0;	}

    if  ( tailFlags & POSflagPARA_TAIL )
	{
	tp= dp->dpNode->biParaParticules+ tailPart- 1;

	tpRet= docInsertTextParticule( dp->dpNode, tailPart, dp->dpStroff, 1,
						    kind, tp->tpTextAttrNr );
	if  ( ! tpRet )
	    { LXDEB(tailPart,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	eo->eoSelectedRange.erTail.epStroff= dp->dpStroff+ 1;

	*pPartShift= 1; *pStroffShift= stroffShift;
	return tpRet;
	}

    if  ( tailPart < 0 || tailPart >= dp->dpNode->biParaParticuleCount )
	{
	LLDEB(tailPart,dp->dpNode->biParaParticuleCount);
	LLDEB(dp->dpStroff,docParaStrlen( dp->dpNode ));
	return (TextParticule *)0;
	}

    tp= dp->dpNode->biParaParticules+ tailPart;

    /*  2  */
    if  ( dp->dpNode->biParaParticuleCount == 1			&&
	  docParaStrlen( dp->dpNode ) == 0			&&
	  dp->dpNode->biParaParticules->tpKind == TPkindSPAN	)
	{
	dp->dpNode->biParaParticules->tpKind= kind;
	dp->dpNode->biParaParticules->tpStrlen= 1;

	*pPartShift= 0; *pStroffShift= stroffShift;
	tailPart= 1;

	if  ( docEditShiftParticuleOffsets( eo, dp->dpNode, paraNr,
						tailPart, dp->dpStroff, 1 ) )
	    { LDEB(1);	}

	return dp->dpNode->biParaParticules;
	}

    /*  3  */
    if  ( tailFlags & POSflagPART_HEAD )
	{
	tpRet= docInsertTextParticule( dp->dpNode, tailPart,
				    dp->dpStroff, 1, kind, tp->tpTextAttrNr );
	if  ( ! tpRet )
	    { LXDEB(tailPart,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	*pPartShift= 1;
	tailPart++;
	}
    else{
	/*  4  */
	if  ( tailFlags & POSflagPART_TAIL )
	    {
	    tpRet= docInsertTextParticule( dp->dpNode, tailPart+ 1,
				    dp->dpStroff, 1, kind, tp->tpTextAttrNr );
	    if  ( ! tpRet )
		{ LXDEB(tailPart,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    tailPart += 2;
	    }
	else{
	    /*  5  */
	    if  ( docSplitTextParticule( &tp, (TextParticule **)0, dp->dpNode,
						    tailPart, dp->dpStroff ) )
		{ LLDEB(tailPart,dp->dpStroff); return (TextParticule *)0; }

	    /*  6  */
	    tpRet= docInsertTextParticule( dp->dpNode, tailPart+ 1,
				    dp->dpStroff, 1, kind, tp->tpTextAttrNr );
	    if  ( ! tpRet )
		{ LXDEB(tailPart+1,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    tailPart += 2;
	    }
	}

    /*  7  */
    if  ( docEditShiftParticuleOffsets( eo, dp->dpNode, paraNr, tailPart,
							    dp->dpStroff, 1 ) )
	{ LDEB(1);	}

    *pStroffShift= stroffShift;

    return tpRet;
    }

TextParticule * docEditParaSpecialParticule(
					EditOperation *		eo,
					int			kind )
    {
    TextParticule *	tp;
    int			partShift= 0;
    int			stroffShift= 0;

    tp= docParaSpecialParticule( eo, kind, &partShift, &stroffShift );

    if  ( eo->eoParaAdjustParagraphNumber >= 0 )
	{ docExtendParagraphAdjust( eo, eo->eoTailDp.dpNode, stroffShift ); }

    eo->eoTailDp.dpStroff += stroffShift;

    return tp;
    }

/************************************************************************/
/*									*/
/*  Fixup for paragraphs that start with a \page or \column:		*/
/*  Remove the particule and remember the fact as a paragraph property.	*/
/*									*/
/************************************************************************/

int docEditFixParaBreakKind(	EditOperation *		eo,
				struct DocumentTree *		dt,
				struct BufferDocument *	bd,
				int			paraNr )
    {
    struct BufferItem *	paraNode= docGetParagraphByNumber( dt, paraNr );

    int			partHead= 0;
    int			stroffHead= 0;
    int			stroffShift= 0;

    int			part;
    TextParticule *	tp;

    if  ( ! paraNode )
	{ LXDEB(paraNr,paraNode); return -1;	}

    if  ( paraNode->biParaProperties->ppListOverride > 0 )
	{
	struct DocumentField *	dfHead= (struct DocumentField *)0;
	DocumentSelection	dsInsideHead;
	DocumentSelection	dsAroundHead;
	int			partBegin= -1;
	int			partEnd= -1;

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, paraNode, bd ) )
	    { LDEB(1);		}
	else{
	    stroffHead= dsAroundHead.dsTail.dpStroff;
	    partHead= partEnd+ 1;
	    }
	}

    tp= paraNode->biParaParticules+ partHead;
    for ( part= partHead; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	const int	paraShift= 0;

	if  ( tp->tpStroff < stroffHead )
	    { LLDEB(tp->tpStroff,stroffHead); continue;	}
	if  ( tp->tpStroff > stroffHead )
	    { break;					}

	switch( tp->tpKind )
	    {
	    case TPkindPAGEBREAK:
		docParaNodeSetBreakKind( paraNode, DOCibkPAGE, bd );
		break;

	    case TPkindCOLUMNBREAK:
		docParaNodeSetBreakKind( paraNode, DOCibkCOL, bd );
		break;

	    default:
		continue;
	    }

	/* remove the space from the paragraph contents */
	docParaStringReplace( &stroffShift, paraNode,
				tp->tpStroff, tp->tpStroff+ tp->tpStrlen,
				(char *)0, 0 );

	docShiftEditRange( &(eo->eoSelectedRange),
			    paraNr, tp->tpStroff, paraShift, stroffShift );
	docShiftEditRange( &(eo->eoAffectedRange),
			    paraNr, tp->tpStroff, paraShift, stroffShift );

SDEB("!!!!!!");
	docEditShiftParticuleOffsets( eo, paraNode, paraNr, part+ 1,
				    tp->tpStroff+ tp->tpStrlen, stroffShift );

	docDeleteParticules( paraNode, part, 1 );
	break;
	}

    return 0;
    }

