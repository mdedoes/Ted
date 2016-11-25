#   ifndef		DOC_SELECT_H
#   define		DOC_SELECT_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentField;

/************************************************************************/
/*									*/
/*  Used to layout lines of text.					*/
/*									*/
/*  A0 paper is 4* 30 cm high: 28.3465* 120* 20= 68031.6 Twips.		*/
/*  So an unsigned short limits us to A1 paper.				*/
/*									*/
/************************************************************************/

typedef struct LayoutPosition
    {
    unsigned short	lpPageYTwips;
    unsigned short	lpPage;
    unsigned char	lpColumn;
    unsigned char	lpAtTopOfColumn;
    } LayoutPosition;

/************************************************************************/
/*									*/
/*  A position in a document.						*/
/*									*/
/*  1)  Indication for the kind of root item for the selection.		*/
/*  2)  Geometry is derived from the position, NOT the other way round.	*/
/*									*/
/*  A)  Indication for the kind of root item for the selection.		*/
/*  B)  When the selection is not in the body of the document, the	*/
/*	number of the section that the external item belongs to.	*/
/*  D)  If the external item is a footnote/endnote: The index in the	*/
/*	array of notes in the section.					*/
/*									*/
/*  E)  The beginning, end and anchor of the selection. The order is in	*/
/*	the reding direction. NOT in the order that the selection was	*/
/*	made. USE EXTREME CARE WHEN YOU REFER TO THE OLD POSITION IN	*/
/*	EDITING CODE.							*/
/*									*/
/************************************************************************/

typedef struct DocumentPosition
    {
    struct BufferItem *	dpBi;
    int			dpParticule;
    int			dpStroff;
    } DocumentPosition;

typedef struct SelectionScope
    {
    int			ssInExternalItem;			/*  A  */
    int			ssSectNrExternalTo;			/*  B  */
    int			ssNoteArrayIndex;			/*  D  */
    } SelectionScope;

typedef struct DocumentSelection
    {
    SelectionScope	dsSelectionScope;

    DocumentPosition	dsBegin;				/*  E  */
    DocumentPosition	dsEnd;					/*  E  */
    DocumentPosition	dsAnchor;				/*  E  */

    int			dsCol0;
    int			dsCol1;
    int			dsDirection;
    } DocumentSelection;

/*  2  */
typedef struct PositionGeometry
    {
    int			pgLine;

    int			pgXPixels;
    LayoutPosition	pgTopPosition;
    int			pgY1Pixels;
    int			pgBaselinePixels;
    } PositionGeometry;

typedef struct SelectionGeometry
    {
    DocumentRectangle	sgRectangle;

    PositionGeometry	sgBegin;				/*  E  */
    PositionGeometry	sgEnd;					/*  E  */
    PositionGeometry	sgAnchor;				/*  E  */
    } SelectionGeometry;

typedef struct TableRectangle
    {
    int		trCol0;
    int		trCol1;
    int		trCol11;

    int		trRow00;
    int		trRow0;
    int		trRow1;
    int		trRow11;

    int		trCellColspan;
    int		trCellRowspan;
    } TableRectangle;

#   define	docPositionsInsideCell(b,e) \
		((b)->dpBi->biParent == (e)->dpBi->biParent)
#   define	docPositionsInsideRow(b,e) \
		((b)->dpBi->biParent->biParent == (e)->dpBi->biParent->biParent)

#   define	docSelectionInsideCell(ds) \
		docPositionsInsideCell(&(ds)->dsBegin,&(ds)->dsEnd)
#   define	docSelectionInsideRow(ds) \
		docPositionsInsideRow(&(ds)->dsBegin,&(ds)->dsEnd)

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentPosition(	DocumentPosition *	dp );
extern void docInitDocumentSelection(	DocumentSelection *	ds );
extern void docInitPositionGeometry(	PositionGeometry *	pg );
extern void docInitSelectionGeometry(	SelectionGeometry *	sg );

extern void docSetIBarSelection(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern int docIsIBarSelection( const DocumentSelection *		ds );
extern int docIsParaSelection( const DocumentSelection *		ds );

extern int docGetObjectSelection(	DocumentSelection *	ds,
					DocumentPosition *	dpObject,
					InsertedObject **	pIo );

extern void docSetRangeSelection(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const DocumentPosition *	dpTo,
				int				direction,
				int				col0,
				int				col1 );

extern int docBeginOfLine(	DocumentPosition *		dp );
extern int docEndOfLine(	DocumentPosition *		dp );

extern int docGetTableRectangle(	TableRectangle *		tr,
					const DocumentSelection *	ds );

extern int docGetTableSliceSelection(
				int *				pIsRowSlice,
				int *				pIsColSlice,
				TableRectangle *		tr,
				const DocumentSelection *	ds );

#   endif	/*	DOC_SELECT_H	*/
