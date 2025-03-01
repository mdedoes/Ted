#   ifndef		DOC_SELECT_H
#   define		DOC_SELECT_H

#   include		<docSelectionScope.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct TableRectangle;
struct InsertedObject;
struct TextLine;
struct EditPosition;
struct EditRange;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  A position in a document.						*/
/*									*/
/*  NOTE Geometry is derived from the position, NOT the other way round.*/
/*									*/
/************************************************************************/

typedef struct DocumentPosition
    {
    struct BufferItem *	dpNode;
    int			dpStroff;
    } DocumentPosition;

typedef struct DocumentSelection
    {
    SelectionScope	dsSelectionScope;

    DocumentPosition	dsHead;					/*  E  */
    DocumentPosition	dsTail;					/*  E  */
    DocumentPosition	dsAnchor;				/*  E  */

    int			dsCol0;
    int			dsCol1;
    signed char		dsDirection;
    } DocumentSelection;

#   define	docSelectionIsSet( ds ) \
		((ds)->dsHead.dpNode && (ds)->dsTail.dpNode)

#   define	docSamePosition(b,e) \
		((b)->dpNode == (e)->dpNode && (b)->dpStroff == (e)->dpStroff)

#   define	docPositionsInsideParagraph(b,e) \
		((b)->dpNode == (e)->dpNode)
#   define	docPositionsInsideCell(b,e) \
		((b)->dpNode->biParent == (e)->dpNode->biParent)
#   define	docPositionsInsideRow(b,e) \
		((b)->dpNode->biParent->biParent == (e)->dpNode->biParent->biParent)

#   define	docSelectionSingleParagraph(ds) \
		docPositionsInsideParagraph(&(ds)->dsHead,&(ds)->dsTail)
#   define	docSelectionInsideCell(ds) \
		docPositionsInsideCell(&(ds)->dsHead,&(ds)->dsTail)
#   define	docSelectionInsideRow(ds) \
		docPositionsInsideRow(&(ds)->dsHead,&(ds)->dsTail)

#   define	docSameSelection(a,b) \
		( docSamePosition( &((a)->dsHead), &((b)->dsHead) ) && \
		  docSamePosition( &((a)->dsTail), &((b)->dsTail) ) )

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docCollectReference(	struct MemoryBuffer *		mbResult,
				const struct DocumentSelection * ds,
				struct BufferDocument *		bd );

extern void docInitDocumentPosition(	DocumentPosition *	dp );
extern void docInitDocumentSelection(	DocumentSelection *	ds );

extern void docSetIBarSelection(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern int docIsIBarSelection( const DocumentSelection *		ds );
extern int docIsParaSelection( const DocumentSelection *		ds );

extern int docGetObjectSelection(
				int *				pPart,
				DocumentPosition *		dpObject,
				struct InsertedObject **	pIo,
				const struct BufferDocument *	bd,
				const DocumentSelection *	ds );

extern int docSetRangeSelection(
				DocumentSelection *		ds,
				const DocumentPosition *	dpHead,
				const DocumentPosition *	dpTail,
				int				direction );

extern int docLineHead(		DocumentPosition *		dp,
				int				posFlags );
extern int docLineTail(		DocumentPosition *		dp,
				int				posFlags );

extern int docSelectFrameOfPosition(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern void docGetSelectionScope(	SelectionScope *		ss,
					const struct BufferItem *	node );

extern void docSetSelectionScope(	DocumentSelection *		ds );

extern int docSelectionSameInstance(
				const struct DocumentTree *	tree,
				int				page,
				int				column );

extern int docSelectionSameRoot(
				const DocumentSelection *	dsFrom,
				const struct BufferItem *	biTo );

extern void docSetEditPosition(	struct EditPosition *		ep,
				const DocumentPosition *	dp );

extern void docSetEditRange(	struct EditRange *		er,
				const DocumentSelection *	ds );

extern void docSetIBarRange(		struct EditRange *		er,
					const DocumentPosition *	dp );

extern int docGetTableRectangle(	struct TableRectangle *		tr,
					const DocumentSelection *	ds );

extern int docSetNodeSelection(		DocumentSelection *		ds,
					struct BufferItem *		node );

extern void docAvoidMergedCellTail(	DocumentSelection *		dsNew,
					struct EditRange *		er );

extern int docDelimitTable(	const struct BufferItem *	paraNode,
				struct BufferItem **		pParentNode,
				int *				pCol,
				int *				pRow0,
				int *				pRow,
				int *				pRow1 );

extern int docSelectTableCell(	DocumentSelection *		dsCell,
				struct BufferItem *		childNode,
				int				col,
				int				row );

extern int docSelectTableRect(	DocumentSelection *		dsRect,
				struct BufferItem *		childNode,
				int				col0,
				int				col1,
				int				row0,
				int				row1 );

extern int docSelectTableLeft(	DocumentSelection *		dsCell,
				struct BufferItem *		childNode );
extern int docSelectTableRight(	DocumentSelection *		dsCell,
				struct BufferItem *		childNode );
extern int docSelectTableAbove(	DocumentSelection *		dsCell,
				struct BufferItem *		childNode );
extern int docSelectTableBelow(	DocumentSelection *		dsCell,
				struct BufferItem *		childNode );

extern int docSetDocumentPosition(
				struct DocumentPosition *	dp,
				struct BufferItem *		node,
				int				stroff );

extern int docNextPosition(	struct DocumentPosition *	dp );	/* technical */
extern int docPrevPosition(	struct DocumentPosition *	dp );	/* technical */

extern int docGotoNextPosition(	struct DocumentPosition *	dp );	/* navigation */
extern int docGotoPrevPosition(	struct DocumentPosition *	dp );	/* navigation */

extern int docNextWord(		struct DocumentPosition *	dp );
extern int docPrevWord(		struct DocumentPosition *	dp );

extern int docLineDown(		int *				pLine,
				struct DocumentPosition *	dp,
				int				positionFlags );

extern int docLineUp(		int *				pLine,
				struct DocumentPosition *	dp,
				int				positionFlags );

extern int docGotoFirstPosition( struct DocumentPosition *	dp,	/* navigation */
				struct BufferItem *		node );
extern int docGotoLastPosition(	struct DocumentPosition *	dp,	/* navigation */
				struct BufferItem *		node );
extern int docHeadPosition(	struct DocumentPosition *	dp,	/* technical */
				struct BufferItem *		node );
extern int docTailPosition(	struct DocumentPosition *	dp,	/* technical */
				struct BufferItem *		node );

extern int docDocumentHead(	struct DocumentPosition *	dp,
				struct BufferDocument *		bd );

extern int docDocumentTail(	struct DocumentPosition *	dp,
				struct BufferDocument *		bd );

extern int docSelectWholeSection(	struct DocumentSelection *	ds,
					int			direction );
extern int docSelectWholeTable(		struct DocumentSelection *	ds );
extern int docSelectRow(		struct DocumentSelection *	ds,
					int			direction,
					int			allColumns );
extern int docSelectWholeCell(		struct DocumentSelection *	ds,
					int			direction,
					int			allRows );
extern int docSelectColumn(		struct DocumentSelection *	ds,
					int			col );
extern int docSelectWholeParagraph(	struct DocumentSelection *	ds,
					int			direction );

extern int docSelectWholeBody(	struct DocumentSelection *	ds,
				struct BufferDocument *		bd );

extern void docLineSelection(	struct DocumentSelection *	dsLine,
				const struct BufferItem *	node,
				int			line );

extern void docTextLineSelection( struct DocumentSelection *	dsLine,
				const struct BufferItem *	node,
				const struct TextLine *		tlFirst,
				const struct TextLine *		tlLast );

extern void docIntersectLineSelection(
				struct DocumentSelection *	dsLine,
				struct BufferItem *		paraNode,
				const struct TextLine *		tl,
				const struct DocumentSelection * ds );

extern void docWordSelection(
			struct DocumentSelection *	dsWord,
			int *				pIsObject,
			int *				pAfterObject,
			const struct DocumentPosition *	dpAround );

extern int docTableRectangleSelection(	struct DocumentSelection *	ds,
					struct BufferItem **	pParentNode,
					struct BufferDocument *	bd,
					const struct TableRectangle *	tr );

extern int docParaNextWord(	const struct BufferItem *	paraNode,
				int			stroff );

extern int docParaPrevWord(	const struct BufferItem *	paraNode,
				int			stroff );

extern int docParaHeadOfWord(	int *				pStroff,
				const struct BufferItem *	paraNode,
				int				part );

extern int docParaTailOfWord(	int *				pStroff,
				const struct BufferItem *	paraNode,
				int				part );

extern int docParaPastLastNonBlank(
				const struct BufferItem *	paraNode,
				int				from,
				int				upto );

extern void docSetParaSelection( struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				int				direction,
				int				start,
				int				length );

extern void docAvoidParaHeadField(
				struct DocumentPosition *	dp,
				int *				pPart,
				const struct BufferDocument *	bd );

extern int docComparePositions(	const struct DocumentPosition *	dp1,
				const struct DocumentPosition *	dp2 );

extern int docCompareNodePositionToSelection(
				const struct BufferItem *	node,
				const struct DocumentSelection *	ds );

extern int docCompareCellPositionToSelection(
				int *				pSelected,
				const struct BufferItem *	cellNode,
				const struct DocumentSelection *	ds );

extern void docConstrainSelectionToOneParagraph(
				int *				pHeadMoved,
				int *				pTailMoved,
				struct DocumentSelection *	ds );

extern int docPositionForEditPosition(
				struct DocumentPosition *	dpNew,
				const struct EditPosition *	ep,
				const struct DocumentTree *	tree );

extern int docSelectionForEditPositionsInDoc(
				struct DocumentSelection *	ds,
				const struct BufferDocument *	bd,
				const struct SelectionScope *	ss,
				const struct EditPosition *	epHead,
				const struct EditPosition *	epTail );

extern int docSelectionForEditPositionsInTree(
				struct DocumentSelection *	ds,
				const struct DocumentTree *	tree,
				const struct EditPosition *	epHead,
				const struct EditPosition *	epTail );

extern int docExtendSelectionFromTable(	struct DocumentPosition *	dpHead,
					struct DocumentPosition *	dpTail,
					int *				pCol0,
					int *				pCol1 );

extern int docExtendSelectionIntoTable(	struct DocumentPosition *	dpEnd );

#   endif	/*	DOC_SELECT_H	*/
