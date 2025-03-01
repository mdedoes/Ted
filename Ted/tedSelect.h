
#   ifndef	TED_SELECT_H
#   define	TED_SELECT_H

struct EditDocument;
struct InsertedObject;
struct SelectionDescription;
struct SelectionGeometry;
struct PositionGeometry;
struct DocumentPosition;
struct DocumentTree;
struct DocumentSelection;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedExtendSelectionToPosition(
				struct EditDocument *		ed,
				const struct DocumentPosition *	dpAnchor,
				const struct DocumentPosition *	dpFound );

extern void tedSetSelectionLow(	struct EditDocument *		ed,
				const struct DocumentSelection * dsSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY );

extern int tedHasSelection(	const struct EditDocument *	ed );
extern int tedHasIBarSelection(	const struct EditDocument *	ed );

extern void tedDelimitCurrentSelection(	struct EditDocument *		ed );

extern int tedGetObjectSelection(
				const struct PositionGeometry ** pPgObj,
				int *				pPartObj,
				struct DocumentPosition *	dpObj,
				struct InsertedObject **	pIo,
				const struct EditDocument *	ed );

extern void tedExposeCurrentSelection(
				const struct EditDocument *	ed );

extern void tedExposeWholeDocument(
				struct EditDocument *		ed );

extern int tedGetSelection(	struct DocumentSelection *	ds,
				struct SelectionGeometry *	sg,
				struct SelectionDescription *	sd,
				struct DocumentTree **		pTree,
				struct BufferItem **		pBodySectNode,
				const struct EditDocument *	ed );

extern int tedFindPositionForCoordinates(
				struct DocumentPosition *	dpFound,
				struct PositionGeometry *	pgFound,
				struct DocumentTree **		pTree,
				struct BufferItem **		pSelSectNode,
				struct BufferItem **		pBodySectNode,
				struct EditDocument *		ed,
				int				docX,
				int				docY );

extern void tedSetSelection(	struct EditDocument *		ed,
				const struct DocumentSelection * dsSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedSetSelectedPosition(
				struct EditDocument *		ed,
				const struct DocumentPosition *	dp,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedDescribeSelection(	struct EditDocument *	ed );

extern int tedSetIBarSelection(		struct EditDocument *		ed,
					const struct DocumentPosition *	dp,
					int			lastLine,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedScrollToSelection(	struct EditDocument *	ed,
					int *			pScrolledX,
					int *			pScrolledY );

#   endif	/*  TED_SELECT_H	*/
