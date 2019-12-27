/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TRACE_H
#   define	DOC_TRACE_H

struct PictureProperties;
struct EditOperation;
struct EditStep;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docReinsertNodes(	struct EditOperation *		eo,
				struct BufferItem *		parentTo,
				const struct BufferItem *	parentFrom,
				int				to,
				const struct EditStep *		es );

extern const struct DocumentField * docTraceGetFromField(
				const struct EditStep * 	es );

extern const struct PictureProperties * docTraceGetFromPictureProps(
				const struct EditStep * 	es );

extern int docUndoDeleteColumn(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoInsertColumn(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoDeleteHeaderFooter(	struct EditOperation *	eo,
					const struct EditStep *	es );

extern int docUndoShiftIndent(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoDeletePara(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoDeleteRow(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoUpdProps(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoDeleteSect(	struct EditOperation *		eo,
				const struct EditStep *		es );

extern int docUndoReplace(	struct EditOperation *		eo,
				const struct EditStep *		es,
				int			copyTailParaProps );

#    endif	/*  DOC_TRACE_H	*/
