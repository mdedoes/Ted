/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	<docParaString.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docObjectProperties.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFieldStack.h>
#   include	<docObject.h>
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docObjects.h>
#   include	<docFields.h>
#   include	<docParaParticuleAdmin.h>
#   include	<docParaBuilder.h>
#   include	<docParaParticules.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/*
# define DEB_PARTICULES
*/

/************************************************************************/
/*									*/
/*  Enter a field while copying. This assumes that copying is done in	*/
/*  document order.							*/
/*									*/
/************************************************************************/

static DocumentField * docStartFieldCopy(
				DocumentCopyJob *		dcj,
				const EditPosition *		epStart,
				int				obnrFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;
    const DocumentFieldList *	dflFrom= &(dcj->dcjSourceDocument->bdFieldList);
    const int			fieldCount= dflFrom->dflPagedList.plItemCount;

    DocumentField *		dfFrom;
    DocumentField *		dfTo;

    dfFrom= docGetFieldByNumber( dcj->dcjSourceDocument, obnrFrom );

    dfTo= docClaimFieldCopy( &(bdTo->bdFieldList), dfFrom,
				&(dcj->dcjTargetSelectionScope), epStart );
    if  ( ! dfTo )
	{ XDEB(dfTo); return (DocumentField *)0;	}

    if  ( obnrFrom < 0 || obnrFrom >= fieldCount )
	{ LDEB(obnrFrom); }
    else{
	dcj->dcjFieldMap[obnrFrom]= dfTo->dfFieldNumber;
	/*appDebug("FIELDMAP: @%3d= %3d\n", obnrFrom, dfTo->dfFieldNumber );*/
	}

    if  ( docPushFieldOnCopyStack( dcj, dfTo, FSpieceFLDRSLT ) )
	{ LDEB(1); return (DocumentField *)0;	}

    if  ( dfTo->dfKind >= DOC_FieldKindCount	)
	{ LDEB(dfTo->dfKind);	}
    else{
	eo->eoFieldUpdate |=
		DOC_FieldKinds[dfTo->dfKind].fkiCalculateWhen;
	}

    if  ( dfTo->dfKind == DOCfkHYPERLINK			&&
	  ! utilMemoryBufferIsEmpty( &(dcj->dcjRefFileName) )	)
	{
	docMakeHyperlinkRelative( dfTo, &(dcj->dcjRefFileName) );
	}

    if  ( dfTo->dfKind == DOCfkBOOKMARK )
	{
	const MemoryBuffer *	mbFrom;
	MemoryBuffer		mbTo;

	utilInitMemoryBuffer( &mbTo );

	if  ( docFieldGetBookmark( &mbFrom, dfFrom ) )
	    { utilMemoryBufferSetString( &mbTo, "x" );	}
	else{ utilCopyMemoryBuffer( &mbTo, mbFrom );	}

	if  ( docMakeBookmarkUnique( bdTo, &mbTo ) )
	    { LDEB(1); return (DocumentField *)0;	}

	if  ( docSetBookmarkField( &(dfTo->dfInstructions), &mbTo ) )
	    { LDEB(1); return (DocumentField *)0;	}

	utilCleanMemoryBuffer( &mbTo );
	}

    if  ( docFieldHasNote( dfFrom->dfKind ) && dfFrom->dfNoteIndex >= 0 )
	{
	if  ( docCopyNote( dcj, dfTo, dfFrom ) )
	    { LDEB(1); return (DocumentField *)0;	}

	utilIndexSetAdd( &(dcj->dcjNoteFieldsCopied), dfTo->dfFieldNumber );
	}

    return dfTo;
    }

/************************************************************************/
/*									*/
/*  Leave a field while copying. This assumes that copying is done in	*/
/*  document order.							*/
/*									*/
/************************************************************************/

static int docFinishFieldCopy(	DocumentCopyJob *		dcj,
				const EditPosition *		epTail,
				int				obnrFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;
    const DocumentFieldList *	dflFrom= &(dcj->dcjSourceDocument->bdFieldList);
    const int			fieldCount= dflFrom->dflPagedList.plItemCount;

    DocumentField *		dfTo;
    int				obnrTo;

    int				res;

    if  ( obnrFrom < 0			||
	  obnrFrom >= fieldCount	)
	{ LLDEB(obnrFrom,fieldCount); return -1;	}

    /*appDebug("FIELDMAP: @%3d? %3d\n", obnrFrom, dcj->dcjFieldMap[obnrFrom] );*/

    if  ( dcj->dcjFieldMap[obnrFrom] < 0 )
	{
	/*  Moving tail of field around */
	if  ( dcj->dcjSourceDocument == bdTo )
	    { return obnrFrom;	}

	LLDEB(obnrFrom,fieldCount);
	SDEB(docFieldKindStr(docGetFieldKindByNumber(dflFrom,obnrFrom)));
	return -1;
	}

    obnrTo= dcj->dcjFieldMap[obnrFrom];
    dfTo= docGetFieldByNumber( bdTo, obnrTo );
    docSetFieldTail( dfTo, epTail );

    res= docFieldStackPopLevel( &(dcj->dcjFieldStack), dcj->dcjTargetTree );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ LDEB(res);		}

    /*  Head and tail copied */
    dcj->dcjFieldMap[obnrFrom]= -2;
    /*appDebug("FIELDMAP: @%3d# %3d\n", obnrFrom, dcj->dcjFieldMap[obnrFrom] );*/

    return obnrTo;
    }

/************************************************************************/
/*									*/
/*  Copy particule data from one paragraph to another.			*/
/*									*/
/*  9)  Not needed, the document is marked as changed, so this can wait	*/
/*	until it is saved.						*/
/*									*/
/************************************************************************/

static int docCopyParticuleData(	DocumentCopyJob *	dcj,
					struct BufferItem *	nodeTo,
					TextParticule *		tpTo,
					TextParticule *		tpFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;
    int				paraNr= docNumberOfParagraph( nodeTo );

    switch( tpFrom->tpKind )
	{
	case TPkindSPAN:
	case TPkindTAB:
	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    break;

	case TPkindOBJECT:
	    {
	    const InsertedObject *	ioFrom;
	    InsertedObject *		ioTo;
	    int				nr;

	    ioFrom= docGetObject( dcj->dcjSourceDocument,
						    tpFrom->tpObjectNumber );
	    if  ( ! ioFrom )
		{ LPDEB(tpFrom->tpObjectNumber,ioFrom); return -1;	}

	    ioTo= docClaimObjectCopy( bdTo, &nr, ioFrom );
	    if  ( ! ioTo )
		{ XDEB(ioTo); return -1;	}
	    tpTo->tpObjectNumber= nr;

	    ioFrom= docGetObject( dcj->dcjSourceDocument,
						    tpFrom->tpObjectNumber );
	    if  ( ! ioFrom )
		{ LPDEB(tpFrom->tpObjectNumber,ioFrom); return -1;	}

	    if  ( ioFrom->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( ! ioFrom->ioDrawingShape )
		    { XDEB(ioFrom->ioDrawingShape);	}
		else{
		    ioTo->ioDrawingShape= docCopyDrawingShape( dcj,
						    ioFrom->ioDrawingShape );
		    if  ( ! ioTo->ioDrawingShape )
			{ XDEB(ioTo->ioDrawingShape);	}
		    }
		}

	    /*  9
	    if  ( ioTo->ioBliptag == 0 )
		{ ioTo->ioBliptag= appGetTimestamp();	}
	    */
	    }
	    break;

	case TPkindFIELDTAIL:
	    {
	    EditPosition		epTail;

	    epTail.epParaNr= paraNr;
	    epTail.epStroff= tpTo->tpStroff;

	    if  ( dcj->dcjCopyFields )
		{
		tpTo->tpObjectNumber= docFinishFieldCopy( dcj, &epTail,
						    tpFrom->tpObjectNumber );
		if  ( tpTo->tpObjectNumber < 0 )
		    { LDEB(tpTo->tpObjectNumber); return -1;	}
		}
	    else{
		tpTo->tpObjectNumber= tpFrom->tpObjectNumber;
		if  ( tpTo->tpObjectNumber < 0 )
		    { LDEB(tpTo->tpObjectNumber);	}

		if  ( dcj->dcjStealFields )
		    {
		    DocumentField *	dfTo;

		    tpFrom->tpObjectNumber= -1;

		    dfTo= docGetFieldByNumber( bdTo, tpTo->tpObjectNumber );
		    if  ( ! dfTo )
			{ LXDEB(tpTo->tpObjectNumber,dfTo);	}
		    else{ docSetFieldTail( dfTo, &epTail );	}
		    }
		}
	    }
	    break;

	case TPkindFIELDHEAD:
	    {
	    EditPosition	epHead;

	    epHead.epParaNr= paraNr;
	    epHead.epStroff= tpTo->tpStroff;

	    if  ( dcj->dcjCopyFields )
		{
		DocumentField *		dfTo;

		dfTo= docStartFieldCopy( dcj, &epHead, tpFrom->tpObjectNumber );
		if  ( ! dfTo )
		    { XDEB(dfTo); return -1;	}
		tpTo->tpObjectNumber= dfTo->dfFieldNumber;
		}
	    else{
		tpTo->tpObjectNumber= tpFrom->tpObjectNumber;
		if  ( tpTo->tpObjectNumber < 0 )
		    { LDEB(tpTo->tpObjectNumber);	}

		if  ( dcj->dcjStealFields )
		    {
		    DocumentField *		dfTo;

		    tpFrom->tpObjectNumber= -1;

		    dfTo= docGetFieldByNumber( bdTo, tpTo->tpObjectNumber );
		    if  ( ! dfTo )
			{ LXDEB(tpTo->tpObjectNumber,dfTo);	}
		    else{ docSetFieldHead( dfTo, &epHead );	}
		    }
		}
	    }
	    break;

	default:
	    LDEB(tpFrom->tpKind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy particules plus contents from one paragraph to another.	*/
/*									*/
/*  This might be a copy to a different document, so font numbers and	*/
/*  attribute numbers need to be recalculated.				*/
/*									*/
/*  1)  Find out where to insert.					*/
/*	(At the end of the paragraph or before particule[partTo].	*/
/*  2)  Find position and length of the source string.			*/
/*  3)  Find out whether to replace the empty particule that we use to	*/
/*	have at least one particule in a paragraph.			*/
/*  4)  Insert the desired number of particules in the correct		*/
/*	position. Note that that is one less in a paragraph that was	*/
/*	originally empty.						*/
/*  5)  Insert the new text into the paragraph string.			*/
/*  6)  Shift the particules in the tail of the target paragraph to	*/
/*	make place for the new text string bytes. The possible		*/
/*	overwritten 'empty' particule should not be shifted.		*/
/*  7)  Patch the particules in the hole created above to correct	*/
/*	values derived from the source particules.			*/
/*									*/
/************************************************************************/

int docCopyParticules(	DocumentCopyJob *		dcj,
			struct BufferItem *		nodeTo,
			const struct BufferItem *	nodeFrom,
			int				partTo,
			int				partFrom,
			int				countFrom,
			int *				pParticulesInserted,
			int *				pCharactersCopied )
    {
    EditOperation *	eo= dcj->dcjEditOperation;

    TextParticule *	tpTo;
    TextParticule *	tpFrom;

    int			stroffTo;
    int			stroffShift;

    int			i;

    int			replaceEmpty= 0;

    int			stroffFrom;
    int			strlenFrom;

    int			paraNrTo= docNumberOfParagraph( nodeTo );

    /*  1  */
    if  ( partTo > nodeTo->biParaParticuleCount )
	{
	LLDEB(partTo,nodeTo->biParaParticuleCount);
	partTo= nodeTo->biParaParticuleCount;
	}

    if  ( partTo == nodeTo->biParaParticuleCount )
	{ stroffTo= docParaStrlen( nodeTo );			}
    else{ stroffTo= nodeTo->biParaParticules[partTo].tpStroff;	}

    /*  2  */
    tpFrom= nodeFrom->biParaParticules+ partFrom;
    stroffFrom= tpFrom->tpStroff;
    strlenFrom= tpFrom[countFrom- 1].tpStroff+ tpFrom[countFrom- 1].tpStrlen-
							    tpFrom->tpStroff;

    /*  3  */
    if  ( docParaStrlen( nodeTo ) == 0 && nodeTo->biParaParticuleCount == 1 )
	{
	if  ( partTo < 0 || partTo > 1 )
	    { LDEB(partTo);	}
	if  ( partTo != 0 )
	    { partTo= 0;	}

	replaceEmpty= 1;
	}

    /*  4  */
    tpTo= docInsertParticules( nodeTo, partTo, countFrom- replaceEmpty );
    if  ( ! tpTo )
	{ XDEB(tpTo); return -1;	}

    /*  5  */
    if  ( docParaStringReplace( &stroffShift, nodeTo, stroffTo, stroffTo,
			docParaString( nodeFrom, stroffFrom ), strlenFrom ) )
	{
	LDEB(strlenFrom);
	docDeleteParticules( nodeTo, partTo, countFrom- replaceEmpty );
	return -1;
	}

    /*  6  */
    if  ( docEditShiftParticuleOffsets( eo, nodeTo, paraNrTo,
						partTo+ countFrom,
						stroffTo, stroffShift ) )
	{ LDEB(stroffShift);	}

    /*  7  */
    tpTo= nodeTo->biParaParticules+ partTo;
    for ( i= 0; i < countFrom; tpTo++, tpFrom++, i++ )
	{
	int		textAttributeNumberTo;

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
							tpFrom->tpTextAttrNr );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); return -1;	}

	tpTo->tpStroff= stroffTo;
	tpTo->tpStrlen= tpFrom->tpStrlen;
	tpTo->tpKind= tpFrom->tpKind;
	tpTo->tpTextAttrNr= textAttributeNumberTo;

	if  ( docCopyParticuleData( dcj, nodeTo, tpTo, tpFrom ) )
	    { LLLDEB(partTo,i,countFrom); return -1;	}

	stroffTo += tpTo->tpStrlen;
	}

    *pParticulesInserted += countFrom- replaceEmpty;
    *pCharactersCopied += stroffShift;

    return 0;
    }

int docCopyParticuleAndData(		TextParticule **	pTpTo,
					DocumentCopyJob *	dcj,
					struct BufferItem *	tgtParaNode,
					int			partTo,
					int			stroffTo,
					int			strlenTo,
					const TextParticule *	tpFrom )
    {
    TextParticule	tpSaved;
    int			textAttributeNumberTo;

    TextParticule *	tpTo;

    tpSaved= *tpFrom;

    textAttributeNumberTo= docMapTextAttributeNumber( dcj,
						    tpSaved.tpTextAttrNr );

    if  ( textAttributeNumberTo < 0 )
	{ LDEB(textAttributeNumberTo); return -1;	}

    if  ( partTo < 0 )
	{ partTo= tgtParaNode->biParaParticuleCount;	}

    tpTo= docInsertTextParticule( tgtParaNode, partTo,
					    stroffTo, strlenTo,
					    tpSaved.tpKind,
					    textAttributeNumberTo );
    if  ( ! tpTo )
	{ LXDEB(partTo,tpTo); return -1;	}

    if  ( docCopyParticuleData( dcj, tgtParaNode, tpTo, &tpSaved ) )
	{
	docDeleteParticules( tgtParaNode, partTo, 1 );
	LDEB(partTo); return -1;
	}

    *pTpTo= tpTo;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Make sure the a paragraph does not end in an explicit break.	*/
/*									*/
/************************************************************************/

int docCheckNoBreakAsLast(	EditOperation *		eo,
				struct BufferItem *		paraNode )
    {
    const int			part= paraNode->biParaParticuleCount;
    const int			stroff= docParaStrlen( paraNode );
    const int			len= 0;

    const TextParticule *	tp= paraNode->biParaParticules+ part- 1;

    if  ( tp->tpKind != TPkindLINEBREAK	&&
	  tp->tpKind != TPkindPAGEBREAK	&&
	  tp->tpKind != TPkindCOLUMNBREAK	)
	{ return 0;	}

    tp= docInsertTextParticule( paraNode, part, stroff, len,
				    TPkindSPAN, tp->tpTextAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    docEditIncludeNodeInReformatRange( eo, paraNode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a paragraph to make a new one.					*/
/*									*/
/*  1)  Copy paragraph item.						*/
/*  2)  Open a hole in references to the item to possibly receive some	*/
/*	to the fresh item.						*/
/*  3)  Do not copy a possible list text field from the original.	*/
/*  4)  Copy the contents of the original or insert a dummy particule.	*/
/*  5)  Finally adapt the properties of the target paragraph to those	*/
/*	of the original.						*/
/*									*/
/************************************************************************/

struct BufferItem * docCopyParaNode(
				DocumentCopyJob *		dcj,
				const SelectionScope *		ssRoot,
				struct BufferItem *		biCellTo,
				int				n,
				const struct BufferItem *	srcParaNode,
				const DocumentSelection *	ds )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *	bdTo= eo->eoDocument;
    struct BufferItem *		tgtParaNode= (struct BufferItem *)0;
    struct BufferItem *		rval= (struct BufferItem *)0;

    const int			partTo= 0;
    int				partFrom= 0;
    int				partUpto= srcParaNode->biParaParticuleCount;

    if  ( ds )
	{
	if  ( ds->dsHead.dpNode == srcParaNode	&&
	      ds->dsHead.dpStroff > 0		)
	    {
	    int	flags= 0;

	    if  ( docFindParticuleOfPosition( &partFrom, &flags,
						&(ds->dsHead), PARAfindLAST ) )
		{ LDEB(ds->dsHead.dpStroff); goto ready;	}

	    if  ( ! ( flags & POSflagPART_HEAD ) )
		{
		LLDEB(ds->dsHead.dpStroff,docParaStrlen(srcParaNode));
		XDEB(flags);
		}
	    }

	if  ( ds->dsTail.dpNode == srcParaNode			&&
	      ds->dsTail.dpStroff < docParaStrlen( srcParaNode )	)
	    {
	    int	flags= 0;

	    if  ( docFindParticuleOfPosition( &partUpto, &flags,
						&(ds->dsTail), PARAfindLAST ) )
		{ LDEB(ds->dsTail.dpStroff); goto ready;	}
	    if  ( ! ( flags & POSflagPART_HEAD ) )
		{
		LLDEB(ds->dsTail.dpStroff,docParaStrlen(srcParaNode));
		XDEB(flags);
		}
	    }

	}

    /*  1  */
    tgtParaNode= docInsertNode( bdTo, biCellTo, n, DOClevPARA );
    if  ( ! tgtParaNode )
	{ XDEB(tgtParaNode); goto ready;	}

    docSetParaTableNesting( tgtParaNode, bdTo );

    if  ( ! eo->eoParagraphBuilder )
	{
	eo->eoParagraphBuilder= docOpenParagraphBuilder( eo->eoDocument,
				    &(eo->eoSelectionScope), eo->eoTree );
	}

    if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
					tgtParaNode, 0 ) )
	{ LDEB(0); goto ready;	}

    /*  2  */
    {
    int		paraNr;
    const int	stroff= 0;
    const int	sectShift= 0;
    const int	paraShift= 1;

    paraNr= docNumberOfParagraph( tgtParaNode );

    docEditShiftReferences( eo, ssRoot, paraNr, stroff,
					sectShift, paraShift, -stroff );
    }

    /*  3  */
    if  ( srcParaNode->biParaProperties->ppListOverride > 0 )
	{
	DocumentField *		dfHead= (DocumentField *)0;
	DocumentSelection	dsInsideHead;
	DocumentSelection	dsAroundHead;
	int			bulletPartHead= -1;
	int			bulletPartTail= -1;

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&bulletPartHead, &bulletPartTail,
					srcParaNode, dcj->dcjSourceDocument ) )
	    { LDEB(1);	}

	if  ( partFrom <= bulletPartTail )
	    { partFrom= bulletPartTail+ 1;	}
	}

    /*  4  */
    if  ( partFrom < partUpto )
	{
	int		particulesInserted= 0;
	int		charactersCopied= 0;

	if  ( docCopyParticules( dcj, tgtParaNode, srcParaNode, partTo,
			partFrom, partUpto- partFrom,
			&particulesInserted, &charactersCopied ) )
	    { LLDEB(partUpto,partFrom); goto ready;	}
	}
    else{
	int			textAttributeNumberTo;
	const TextParticule *	tpFrom;

	if  ( partUpto == 0 )
	    { tpFrom= srcParaNode->biParaParticules;			}
	else{ tpFrom= srcParaNode->biParaParticules+ partUpto- 1;	}

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
							tpFrom->tpTextAttrNr );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); goto ready; }

	if  ( ! docInsertTextParticule( tgtParaNode, 0, 0, 0,
					TPkindSPAN, textAttributeNumberTo ) )
	    { LDEB(1); goto ready;	}
	}

    /*  5  */
    if  ( docEditTransferParaProperties( eo, tgtParaNode,
				srcParaNode, 0, &(dcj->dcjAttributeMap) ) )
	{ LDEB(1); goto ready;	}

    tgtParaNode->biParaTocLevel= srcParaNode->biParaTocLevel;

    eo->eoParagraphsInserted++;
    if  ( tgtParaNode->biParaProperties->ppListOverride > 0 )
	{ dcj->dcjBulletsCopied++;	}

    rval= tgtParaNode; tgtParaNode= (BufferItem *)0; /* steal */

  ready:

    if  ( tgtParaNode )
	{ docDeleteNode( eo->eoDocument, eo->eoTree, tgtParaNode );	}

    return rval;
    }

