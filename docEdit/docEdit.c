/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docDebug.h>
#   include	<docParaString.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	<docField.h>
#   include	<docDocumentField.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	<docObjects.h>
#   include	<docFields.h>

#   include	<appDebugon.h>

#   define	DEB_PARTICULES		0
#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Replace a selection with a piece of text.				*/
/*									*/
/*  1)  If the selection spans more than one paragraph...		*/
/*  2)  Replace the tail of the first paragraph with the new text.	*/
/*  2b) Make sure that the bullet of the last paragraph is not included	*/
/*	in the tail that is to be appended to the first paragraph.	*/
/*  3)  Merge the two paragraphs. I.E: append what remains of the last	*/
/*	paragraph to the first paragraph.				*/
/*  4)  Erase all paragraphs after the beginning of the selection	*/
/*	upto and including the end of the selection.			*/
/*  5)  Otherwise just replace the text inside the selected paragraph.	*/
/*									*/
/************************************************************************/

int docReplaceSelection(	EditOperation *			eo,
				const char *			addedString,
				int				addedStrlen,
				int				addedAttrNr )
    {
    int			rval= 0;
    DocumentCopyJob	dcjTail;
    int			paraNrBegin;

    paraNrBegin= docNumberOfParagraph( eo->eoHeadDp.dpNode );

    docInitDocumentCopyJob( &dcjTail );

    if  ( docEditDeleteReplacedFields( eo ) )
	{ LDEB(1);	}

    if  ( eo->eoHeadDp.dpStroff == 0			&&
	  eo->eoHeadDp.dpNode->biParaProperties->ppListOverride > 0	)
	{
	if  ( docRemoveParagraphFromList( eo->eoHeadDp.dpNode,
						eo->eoTree, eo->eoDocument ) )
	    { LDEB(1);	}
	}

    /*  1  */
    if  ( ! docPositionsInsideParagraph( &(eo->eoHeadDp), &(eo->eoTailDp) ) )
	{
	DocumentPosition	tailDp= eo->eoTailDp;

	if  ( eo->eoHeadDp.dpNode->biParaProperties->ppTableNesting > 0	&&
	      eo->eoTailDp.dpNode->biParaProperties->ppTableNesting == 0 )
	    { docFlattenRow( eo->eoHeadDp.dpNode, eo->eoDocument );	}

	/*  2  */
	if  ( docParaReplaceText( eo, paraNrBegin, &tailDp,
			&(eo->eoHeadDp), docParaStrlen( eo->eoHeadDp.dpNode ),
			addedString, addedStrlen, addedAttrNr ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	/*  3  */
	if  ( eo->eoTailDp.dpStroff < docParaStrlen( eo->eoTailDp.dpNode ) )
	    {
	    if  ( docSet1DocumentCopyJob( &dcjTail, eo, CFH_STEAL ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    /*  tail of EO is after insert */
	    if  ( docParaInsertTail( &dcjTail, paraNrBegin,
				&(eo->eoTailDp), &tailDp,
				&(eo->eoTailDp), (DocumentPosition *)0 ) )
		{ LDEB(eo->eoTailDp.dpStroff); rval= -1; goto ready;	}
	    }
	else{
	    eo->eoTailDp= tailDp;
	    }

	docSetEditPosition( &(eo->eoSelectedRange.erTail), &tailDp );
	docSetEditPosition( &(eo->eoAffectedRange.erTail), &tailDp );

	/*  4  */
	if  ( docRemoveSelectionTail( eo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	eo->eoTailDp= tailDp;

	docCheckNoBreakAsLast( eo, eo->eoHeadDp.dpNode );

	docEditIncludeNodeInReformatRange( eo, eo->eoHeadDp.dpNode );
	}
    else{
	int		tailStroffOld= eo->eoTailDp.dpStroff;
	int		tailStroffNew;

	/*  5  */
	if  ( docParaReplaceText( eo, paraNrBegin, &(eo->eoTailDp),
			&(eo->eoHeadDp), eo->eoTailDp.dpStroff,
			addedString, addedStrlen, addedAttrNr ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	docCheckNoBreakAsLast( eo, eo->eoHeadDp.dpNode );

	tailStroffNew= eo->eoTailDp.dpStroff;
	docSetParagraphAdjust( eo, eo->eoHeadDp.dpNode,
				tailStroffNew- tailStroffOld, tailStroffNew );
	}

    docEditFinishStep( eo );

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete the selection: Replace it with noting.			*/
/*									*/
/************************************************************************/

int docDeleteSelection(	EditOperation *			eo )
    {
    const char * const	addedString= (const char *)0;
    int			addedStrlen= 0;
    const int		addedAttrNr= -1;

    return docReplaceSelection( eo, addedString, addedStrlen, addedAttrNr );
    }

/************************************************************************/
/*									*/
/*  Clean particules that will no longer be needed.			*/
/*									*/
/************************************************************************/

int docEditCleanParticules(	EditOperation *		eo,
				struct BufferItem *	paraNode,
				int			partFrom,
				int			partUpto )
    {
    int			part;
    TextParticule *	tp;

    /*  11  */
    tp= paraNode->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
#	if DEB_PARTICULES
	const int	indent= 0;
	docListParticule( indent, "OLD", part, paraNode, tp );
#	endif

	docResetParticuleObjects( eo->eoDocument, tp, 1 );

	if  ( tp->tpKind == TPkindFIELDHEAD	||
	      tp->tpKind == TPkindFIELDTAIL	)
	    {
	    /* Not yet deleted? */
	    if  ( tp->tpObjectNumber >= 0 )
		{
		const DocumentField *	df;

		df= docGetFieldByNumber( eo->eoDocument,
						    tp->tpObjectNumber );
		if  ( df && df->dfFieldNumber >= 0 )
		    {
		    LSLDEB(part,docKindStr(tp->tpKind),df->dfFieldNumber);
		    docListFieldParticule( 0, "FLD", part, paraNode, tp, df );
		    return -1;
		    }
		}
	    }

	tp->tpObjectNumber= -1;
	tp->tpKind= TPkindUNKNOWN;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is this indentation at the head of the paragraph? If so.. skip it	*/
/*  while merging.							*/
/*									*/
/************************************************************************/

static int docIsIndentationParticule(	const struct BufferItem *	node,
					const TextParticule *	tp )
    {
    const char *	s= docParaString( node, tp->tpStroff );
    int			i;

    if  ( tp->tpKind == TPkindTAB )
	{ return 1;	}

    if  ( tp->tpKind != TPkindSPAN )
	{ return 0;	}

    for ( i= 0; i < tp->tpStrlen; i++ )
	{
	if  ( *(s++) != ' ' )
	    { return 0;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Merge the selection into one paragraph.				*/
/*									*/
/************************************************************************/

int docMergeParagraphsInSelection(	EditOperation *	eo )
    {
    int				rval= 0;
    struct BufferDocument *		bd= eo->eoDocument;

    struct BufferItem *		biTo;
    struct BufferItem *		nodeFrom;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet1DocumentCopyJob( &dcj, eo, CFH_STEAL ) )
	{ LDEB(1); rval= -1; goto ready;	}

    biTo= eo->eoHeadDp.dpNode;
    if  ( ! biTo )
	{ XDEB(biTo); rval= -1; goto ready;	}
    nodeFrom= docNextParagraph( biTo );
    if  ( ! nodeFrom )
	{ XDEB(nodeFrom); rval= -1; goto ready;	}

    for (;;)
	{
	int	partFrom= 0;

	int	particulesInserted= 0;
	int	charactersCopied= 0;

	if  ( nodeFrom->biParaProperties->ppListOverride > 0 )
	    {
	    DocumentField *	dfHead= (DocumentField *)0;
	    DocumentSelection	dsInsideHead;
	    DocumentSelection	dsAroundHead;
	    int			partBegin= -1;
	    int			partEnd= -1;

	    if  ( docDelimitParaHeadField( &dfHead,
					&dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, nodeFrom, bd ) )
		{ LDEB(1);	}
	    else{
		if  ( partFrom <= partEnd )
		    { partFrom= partEnd+ 1;	}
		}
	    }

	while( partFrom < nodeFrom->biParaParticuleCount- 1		&&
	       docIsIndentationParticule( nodeFrom,
				nodeFrom->biParaParticules+ partFrom )	)
	    { partFrom++; }

	if  ( partFrom < nodeFrom->biParaParticuleCount )
	    {
	    int			toCount= biTo->biParaParticuleCount;
	    int			toStrlen= docParaStrlen( biTo );
	    const char *	toString= docParaString( biTo, 0 ); /*SHAME*/

	    if  ( toCount > 0						&&
		  biTo->biParaParticules[toCount-1].tpKind ==
							TPkindSPAN	&&
		  toString[toStrlen-1] != ' '				)
		{
		int		stroffShift= 0;

		if  ( docParaStringReplace( &stroffShift, biTo,
					    toStrlen, toStrlen, " ", 1 ) )
		    { LDEB(toStrlen); rval= -1; goto ready;	}

		biTo->biParaParticules[toCount-1].tpStrlen++;
		}

	    if  ( docCopyParticules( &dcj, biTo, nodeFrom,
			biTo->biParaParticuleCount, partFrom,
			nodeFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
		{ LDEB(nodeFrom->biParaParticuleCount); rval= -1; goto ready; }
	    }

	if  ( nodeFrom == eo->eoTailDp.dpNode )
	    { break;	}

	nodeFrom= docNextParagraph( nodeFrom );
	if  ( ! nodeFrom )
	    { XDEB(nodeFrom); rval= -1; goto ready;	}
	}

    biTo= eo->eoHeadDp.dpNode;
    if  ( ! biTo )
	{ XDEB(biTo); rval= -1; goto ready;	}

    if  ( docTailPosition( &(eo->eoTailDp), eo->eoHeadDp.dpNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRemoveSelectionTail( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilIndexSetUnion( &(eo->eoNoteFieldsAdded), &(eo->eoNoteFieldsAdded),
					    &(dcj.dcjNoteFieldsCopied) );
    if  ( dcj.dcjBulletsCopied > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    docEditIncludeNodeInReformatRange( eo, biTo );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

