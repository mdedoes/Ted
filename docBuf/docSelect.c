/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docNodeTree.h"
#   include	"docDebug.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docTableRectangle.h"
#   include	<docCellProperties.h>
#   include	"docTreeNode.h"
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	"docObjects.h"
#   include	"docFields.h"
#   include	"docSelect.h"

#   include	<appDebugon.h>

/**
 *  Get the first position in the node. This is a position where it makes
 *  sense to place the I-Bar.
 */
int docGotoFirstPosition(	DocumentPosition *	dp,
				struct BufferItem *	node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaFirstStroff( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[0];
	}

    /*XDEB(node);*/ return -1;
    }

/**
 *  Get the first position in the node. Do not care whether it makes
 *  sense to place the I-Bar there.
 */
int docHeadPosition(	DocumentPosition *	dp,
			struct BufferItem *	node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, 0 );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[0];
	}

    /*XDEB(node);*/ return -1;
    }

/**
 *  Get the last position in the node. This is a position where it makes
 *  sense to place the I-Bar.
 */
int docGotoLastPosition(	DocumentPosition *	dp,
				struct BufferItem *	node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaLastStroff( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[node->biChildCount- 1];
	}

    /*XDEB(node);*/ return -1;
    }

/**
 *  Get the last position in the node. Do not care whether it makes
 *  sense to place the I-Bar there.
 */
int docTailPosition(	DocumentPosition *	dp,
			struct BufferItem *	node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaStrlen( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[node->biChildCount- 1];
	}

    /*XDEB(node);*/ return -1;
    }

int docDocumentHead(	DocumentPosition *	dp,
			struct BufferDocument *	bd )
    {
    if  ( docHeadPosition( dp, bd->bdBody.dtRoot ) )
	{ return -1;	}

    return 0;
    }

int docDocumentTail(	DocumentPosition *	dp,
			struct BufferDocument *	bd )
    {
    if  ( docTailPosition( dp, bd->bdBody.dtRoot ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Keep out the read-only field at the Head of certain kinds of	*/
/*  paragraphs.								*/
/*									*/
/************************************************************************/

void docAvoidParaHeadField(	DocumentPosition *		dp,
				int *				pPart,
				const struct BufferDocument *	bd )
    {
    struct DocumentField *	dfHead= (struct DocumentField *)0;
    DocumentSelection		dsInsideHead;
    DocumentSelection		dsAroundHead;
    int				partBegin= -1;
    int				partEnd= -1;

    int	fieldKind= docParaHeadFieldKind( dp->dpNode );
    if  ( fieldKind < 0 )
	{ return;	}

    if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, dp->dpNode, bd ) )
	{ LDEB(1);	}
    else{
	if  ( dp->dpStroff < dsAroundHead.dsTail.dpStroff )
	    { dp->dpStroff=  dsAroundHead.dsTail.dpStroff;	}

	if  ( pPart && *pPart <= partBegin )
	    { *pPart= partBegin+ 1;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

static struct BufferItem * docNextNode(	struct BufferItem *	node,
					int			level )
    {
    for (;;)
	{
	if  ( ! node->biParent )
	    { return (struct BufferItem *)0;	}

	if  ( node->biNumberInParent < node->biParent->biChildCount- 1 )
	    {
	    node= node->biParent->biChildren[node->biNumberInParent+ 1];

	    if  ( docIsMergedCell( node ) )
		{ continue;	}

	    while( node->biLevel < level	&&
		   node->biChildCount > 0	)
		{
		node= node->biChildren[0];

		if  ( docIsMergedCell( node ) )
		    { break; }
		}

	    if  ( docIsMergedCell( node ) )
		{ continue; }

	    if  ( node->biLevel == level )
		{ return node;	}
	    }
	else{ node= node->biParent;	}
	}
    }

struct BufferItem *	docNextParagraph(	struct BufferItem *	node )
    { return docNextNode( node, DOClevPARA );	}

struct BufferItem *	docNextSection(	struct BufferItem *	node )
    { return docNextNode( node, DOClevSECT );	}

static struct BufferItem * docPrevNode(	struct BufferItem *	node,
					int		level )
    {
    for (;;)
	{
	if  ( ! node->biParent )
	    { return (struct BufferItem *)0;	}

	if  ( node->biNumberInParent > 0 )
	    {
	    node= node->biParent->biChildren[node->biNumberInParent- 1];

	    if  ( docIsMergedCell( node ) )
		{ continue;	}

	    while( node->biLevel < level	&&
		   node->biChildCount > 0	)
		{
		node= node->biChildren[node->biChildCount- 1];

		if  ( docIsMergedCell( node ) )
		    { break; }
		}

	    if  ( docIsMergedCell( node ) )
		{ continue; }

	    if  ( node->biLevel == level )
		{ return node;	}
	    }
	else{ node= node->biParent;	}
	}
    }

struct BufferItem *	docPrevParagraph(	struct BufferItem *	node )
    { return docPrevNode( node, DOClevPARA );	}

struct BufferItem *	docPrevSection(		struct BufferItem *	node )
    { return docPrevNode( node, DOClevSECT );	}

/************************************************************************/
/*									*/
/*  Move to the Next/Previous word.					*/
/*									*/
/*  1)  Words start after a particule ending in a space or at the	*/
/*	beginning of a paragraph.					*/
/*									*/
/************************************************************************/

int docNextWord(	DocumentPosition *	dp )
    {
    struct BufferItem *		paraNode= dp->dpNode;
    int				stroff= dp->dpStroff;

    if  ( stroff == docParaStrlen( paraNode ) )
	{
	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { return 1;	}

	stroff= 0;
	}

    stroff= docParaNextWord( paraNode, stroff );
    docSetDocumentPosition( dp, paraNode, stroff );

    return 0;
    }

int docPrevWord(	DocumentPosition *	dp )
    {
    struct BufferItem *		paraNode= dp->dpNode;
    int				stroff= dp->dpStroff;

    if  ( stroff == 0 )
	{
	paraNode= docPrevParagraph( paraNode );
	if  ( ! paraNode )
	    { return 1;	}

	stroff= docParaStrlen( paraNode );
	}

    stroff= docParaPrevWord( paraNode, stroff );
    docSetDocumentPosition( dp, paraNode, stroff );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move one line up.							*/
/*									*/
/*  1)  Go to previous line of the paragraph or to the last line of the	*/
/*	last paragraph before this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineUp(		int *			pLine,
			DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    TextLine *		tl;
    struct BufferItem *	node= dp->dpNode;

    if  ( docGetLineOfPosition( &line, dp, positionFlags ) )
	{ LDEB(dp->dpStroff); return -1;	}

    line--;

    /*  1  */
    while( node )
	{
	if  ( node->biParaLineCount == 0 )
	    { LLDEB(docNumberOfParagraph(node),node->biParaLineCount);	}

	if  ( node->biLevel == DOClevPARA	&&
	      line < node->biParaLineCount	&& /* against crashes */
	      line >= 0				)
	    {
	    tl= node->biParaLines+ line;

	    dp->dpNode= node;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pLine )
		{ *pLine= line;	}
	    return 0;
	    }

	/*  2  */
	if  ( node->biLevel == DOClevPARA			&&
	      node->biParaProperties->ppTableNesting > 0	&&
	      node->biNumberInParent == 0			)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    struct BufferItem *	parentNode;

	    if  ( docDelimitTable( node,
				    &parentNode, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row > row0 )
		{
		struct BufferItem *	rowNode= parentNode->biChildren[row-1];

		if  ( col < rowNode->biChildCount )
		    {
		    struct BufferItem *	cellNode= rowNode->biChildren[col];

		    node= cellNode->biChildren[cellNode->biChildCount-1];
		    line= node->biParaLineCount- 1;
		    continue;
		    }
		}
	    }

	node= docPrevParagraph( node );
	if  ( ! node )
	    { break;	}
	line= node->biParaLineCount- 1;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move one line down.							*/
/*									*/
/*  1)  Go to the next line of the paragraph or to the first line of	*/
/*	the first paragraph after this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineDown(	int *			pLine,
			DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    TextLine *		tl;
    struct BufferItem *	node= dp->dpNode;

    if  ( docGetLineOfPosition( &line, dp, positionFlags ) )
	{ LDEB(dp->dpStroff); return -1;	}

    line++;

    /*  1  */
    while( node )
	{
	if  ( node->biLevel == DOClevPARA	&&
	      line < node->biParaLineCount	&&
	      line >= 0				)   /*  against crashes  */
	    {
	    tl= node->biParaLines+ line;

	    dp->dpNode= node;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pLine )
		{ *pLine= line;	}
	    return 0;
	    }

	/*  2  */
	if  ( node->biLevel == DOClevPARA				&&
	      node->biParaProperties->ppTableNesting > 0		&&
	      node->biNumberInParent == node->biParent->biChildCount- 1	)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    struct BufferItem *	parentNode;

	    if  ( docDelimitTable( node, &parentNode, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row < row1 )
		{
		struct BufferItem *	rowNode= parentNode->biChildren[row+1];

		if  ( col < rowNode->biChildCount )
		    {
		    struct BufferItem *	cellNode= rowNode->biChildren[col];

		    node= cellNode->biChildren[0];
		    line= 0;
		    continue;
		    }
		}
	    }

	node= docNextParagraph( node );
	line= 0;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move to the beginning/end of a line					*/
/*									*/
/************************************************************************/

int docLineHead(	DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    int			flags= 0;

    if  ( positionFlags & POSflagLINE_HEAD )
	{ return 0;	}

    if  ( docFindLineOfPosition( &line, &flags, dp, PARAfindFIRST ) )
	{ LDEB(dp->dpStroff); return -1;	}

    dp->dpStroff= dp->dpNode->biParaLines[line].tlStroff;

    return 0;
    }

int docLineTail(	DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    int			flags= 0;
    const TextLine *	tl;

    if  ( positionFlags & POSflagLINE_TAIL )
	{ return 0;	}

    if  ( docFindLineOfPosition( &line, &flags, dp, PARAfindLAST ) )
	{ LDEB(dp->dpStroff); return -1;	}

    tl= &(dp->dpNode->biParaLines[line]);
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get buffer positions for a text line.				*/
/*									*/
/************************************************************************/

void docTextLineSelection(	DocumentSelection *		dsLine,
				const struct BufferItem *	paraNode,
				const TextLine *		tlFirst,
				const TextLine *		tlLast )
    {
    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    const int		direction= 1;

    docInitDocumentPosition( &dpHead );
    docInitDocumentPosition( &dpTail );

    if  ( paraNode->biLevel != DOClevPARA )
	{ LLDEB(paraNode->biLevel,DOClevPARA); return;	}

    docInitDocumentSelection( dsLine );

    dpHead.dpNode= (struct BufferItem *)paraNode;
    dpHead.dpStroff= tlFirst->tlStroff;

    dpTail.dpNode= (struct BufferItem *)paraNode;
    dpTail.dpStroff= tlLast->tlStroff+ tlLast->tlStrlen;

    docSetRangeSelection( dsLine, &dpHead, &dpTail, direction );

    return;
    }


void docLineSelection(	DocumentSelection *		dsLine,
			const struct BufferItem *	paraNode,
			int				line )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ LLDEB(paraNode->biLevel,DOClevPARA); return;	}
    if  ( line < 0 || line >= paraNode->biParaLineCount )
	{ LLDEB(line,paraNode->biParaLineCount); return;	}

    docTextLineSelection( dsLine, paraNode,
			paraNode->biParaLines+ line,
			paraNode->biParaLines+ line );
    }

void docIntersectLineSelection(	DocumentSelection *		dsLine,
				struct BufferItem *		paraNode,
				const TextLine *		tl,
				const DocumentSelection * 	ds )
    {
    *dsLine= *ds;

    if  ( ds->dsHead.dpNode != paraNode )
	{
	dsLine->dsHead.dpNode= paraNode;
	dsLine->dsHead.dpStroff= tl->tlStroff;
	}
    else{
	if  ( dsLine->dsHead.dpStroff < tl->tlStroff )
	    { dsLine->dsHead.dpStroff=  tl->tlStroff;	}
	}

    if  ( ds->dsTail.dpNode != paraNode )
	{
	dsLine->dsTail.dpNode= paraNode;
	dsLine->dsTail.dpStroff= tl->tlStroff+ tl->tlStrlen;
	}
    else{
	if  ( dsLine->dsTail.dpStroff > tl->tlStroff+ tl->tlStrlen )
	    { dsLine->dsTail.dpStroff=  tl->tlStroff+ tl->tlStrlen;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delimit a 'Word' in the document.					*/
/*									*/
/*  A 'Word' is one of the following:					*/
/*  1)	A continguous stretch of text. It is not interrupted by either	*/
/*	white space or control particules. Any white space after the	*/
/*	word is included in the word.					*/
/*  2)	An object or an image.						*/
/*									*/
/************************************************************************/

void docWordSelection(	DocumentSelection *		dsWord,
			int *				pIsObject,
			int *				pAfterObject,
			const DocumentPosition *	dpAround )
    {
    TextParticule *	tp;

    struct BufferItem *	paraNode= dpAround->dpNode;
    int			part;
    const int		direction= 1;

    if  ( paraNode->biLevel != DOClevPARA )
	{ LLDEB(paraNode->biLevel,DOClevPARA); return;	}

    if  ( docFindParticuleOfPosition( &part, (int *)0,
						dpAround, PARAfindLAST ) )
	{ LDEB(dpAround->dpStroff); return;	}

    tp= dpAround->dpNode->biParaParticules+ part;

    if  ( tp->tpStroff == dpAround->dpStroff	&&
	  part > 0				&&
	  tp[-1].tpKind == TPkindOBJECT		)
	{
	if  ( dsWord )
	    {
	    docSetParaSelection( dsWord, paraNode, direction,
					tp[-1].tpStroff, tp[-1].tpStrlen );
	    }
	if  ( pIsObject )
	    { *pIsObject= 1;	}
	if  ( pAfterObject )
	    { *pAfterObject= 1;	}
	return;
	}

    if  ( tp->tpKind == TPkindOBJECT )
	{
	if  ( dsWord )
	    {
	    docSetParaSelection( dsWord, paraNode, direction,
					tp->tpStroff, tp->tpStrlen );
	    }
	if  ( pIsObject )
	    { *pIsObject= 1;	}
	if  ( pAfterObject )
	    {
	    if  ( tp->tpStroff == dpAround->dpStroff )
		{ *pAfterObject= 0;	}
	    else{
		if  ( tp->tpStroff+ tp->tpStrlen == dpAround->dpStroff )
		    { *pAfterObject= 1;	}
		else{
		    LLLDEB(dpAround->dpStroff,tp->tpStroff,tp->tpStrlen);
		    docListNode((const struct BufferDocument *)0,0,dpAround->dpNode,0);
		    *pAfterObject= 0;
		    }
		}
	    }
	return;
	}
    else{
	if  ( dsWord )
	    {
	    int	stroffHead;
	    int	stroffTail;

	    docParaHeadOfWord( &stroffHead, paraNode, part );
	    docParaTailOfWord( &stroffTail, paraNode, part );

	    docSetParaSelection( dsWord, paraNode, direction,
					stroffHead, stroffTail- stroffHead );
	    }
	if  ( pIsObject )
	    { *pIsObject= 0;	}
	if  ( pAfterObject )
	    { *pAfterObject= 0;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a TableRectangle to a DocumentSelection.			*/
/*									*/
/************************************************************************/

int docTableRectangleSelection(	DocumentSelection *	ds,
				struct BufferItem **	pParentNode,
				struct BufferDocument *	bd,
				const TableRectangle *	tr )
    {
    DocumentSelection	dsNew;

    struct BufferItem *	selSectNode;
    struct BufferItem *	selParentNode;
    struct BufferItem *	rowNode;
    struct BufferItem *	cellNode;

    docInitDocumentSelection( &dsNew );

    /*******/

    selParentNode= docGetSelectionRoot(
				(struct DocumentTree **)0,
				(struct BufferItem **)0, bd, ds );
    if  ( ! selParentNode )
	{ XDEB(selParentNode); return -1;	}

    if  ( selParentNode->biLevel == DOClevPARA )
	{ selParentNode= selParentNode->biParent;	}
    if  ( selParentNode->biLevel == DOClevCELL )
	{ selParentNode= selParentNode->biParent;	}
    if  ( selParentNode->biLevel == DOClevROW )
	{ selParentNode= selParentNode->biParent;	}

    selSectNode= docGetSectNode( selParentNode );
    if  ( ! selSectNode )
	{ XDEB(selSectNode); return -1;	}

    /*******/

    if  ( tr->trRow0 < 0					||
	  tr->trRow0 >= selParentNode->biChildCount		)
	{ LLDEB(tr->trRow0,selParentNode->biChildCount); return -1;	}
    rowNode= selParentNode->biChildren[tr->trRow0];

    if  ( tr->trCol0 < 0			||
	  tr->trCol0 >= rowNode->biChildCount	)
	{ LLDEB(tr->trCol0,rowNode->biChildCount); return -1;	}
    cellNode= rowNode->biChildren[tr->trCol0];

    if  ( docHeadPosition( &dsNew.dsHead, cellNode ) )
	{ LDEB(0); return -1;	}

    /*******/

    if  ( tr->trRow1 < 0				||
	  tr->trRow1 >= selParentNode->biChildCount	)
	{ LLDEB(tr->trRow0,selParentNode->biChildCount); return -1;	}
    rowNode= selParentNode->biChildren[tr->trRow1];

    if  ( tr->trCol1 < 0			||
	  tr->trCol1 >= rowNode->biChildCount	)
	{ LLDEB(tr->trCol1,rowNode->biChildCount); return -1;	}
    cellNode= rowNode->biChildren[tr->trCol1];

    if  ( docTailPosition( &dsNew.dsTail, cellNode ) )
	{ LDEB(0); return -1;	}

    /*******/

    dsNew.dsCol0= tr->trCol0;
    dsNew.dsCol1= tr->trCol1;
    dsNew.dsDirection= 1;
    dsNew.dsAnchor= dsNew.dsHead;

    if  ( tr->trRow1 < tr->trRow0					||
	  ( tr->trRow1 == tr->trRow0 && tr->trCol1 < tr->trCol0 )	)
	{
	dsNew.dsDirection= -1;
	dsNew.dsAnchor= dsNew.dsTail;
	}

    docSetSelectionScope( &dsNew );

    *ds= dsNew;
    *pParentNode= selParentNode;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is the selection exactly an object?					*/
/*									*/
/************************************************************************/

int docGetObjectSelection(	int *				pPart,
				DocumentPosition *		dpObject,
				struct InsertedObject **	pIo,
				const struct BufferDocument *	bd,
				const DocumentSelection *	ds )
    {
    if  ( docSelectionIsSet( ds )		&&
	  docSelectionSingleParagraph( ds )	)
	{
	struct BufferItem *	paraNode= ds->dsHead.dpNode;
	int			part;
	TextParticule *		tp;

	if  ( docFindParticuleOfPosition( &part, (int *)0,
						&(ds->dsHead), PARAfindLAST ) )
	    { LDEB(ds->dsHead.dpStroff); return -1;	}

	tp= paraNode->biParaParticules+ part;

	if  ( tp->tpKind == TPkindOBJECT			&&
	      ds->dsHead.dpStroff == tp->tpStroff		&&
	      ds->dsTail.dpStroff == tp->tpStroff+ tp->tpStrlen	)
	    {
	    *pPart= part;
	    *dpObject= ds->dsHead;
	    *pIo= docGetObject( bd, tp->tpObjectNumber );

	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Select the whole frame around a position. Make no difficulties if	*/
/*  the position is not in a frame: simply select the neghbourhood that	*/
/*  is not a frame. (Usually the whole cell.)				*/
/*									*/
/************************************************************************/

int docSelectFrameOfPosition(	DocumentSelection *		ds,
				const DocumentPosition *	dp )
    {
    struct BufferItem *	cellNode;
    int			para0;
    int			para1;
    int			frameNumber;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    const int		direction= 1;

    docInitDocumentPosition( &dpHead );
    docInitDocumentPosition( &dpTail );

    if  ( ! dp->dpNode )
	{ XDEB(dp->dpNode); return 1;	}

    frameNumber= dp->dpNode->biParaProperties->ppFrameNumber;
    cellNode= dp->dpNode->biParent;
    para0= para1= dp->dpNode->biNumberInParent;

    while( para0 > 0 )
	{
	if  ( cellNode->biChildren[para0- 1]->biParaProperties->ppFrameNumber
							    != frameNumber )
	    { break;	}
	para0--;
	}

    while( para1 < cellNode->biChildCount- 1 )
	{
	if  ( cellNode->biChildren[para1+ 1]->biParaProperties->ppFrameNumber
							    != frameNumber )
	    { break;	}
	para1++;
	}

    if  ( docHeadPosition( &dpHead, cellNode->biChildren[para0] ) )
	{ LDEB(1); return 1;	}
    if  ( docTailPosition( &dpTail, cellNode->biChildren[para1] ) )
	{ LDEB(1); return 1;	}

    docSetRangeSelection( ds, &dpHead, &dpTail, direction );

    return 0;
    }

