/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TRACE_H
#   define	DOC_RTF_TRACE_H

struct tm;

struct PictureProperties;
struct DocumentField;
struct RowProperties;
struct CellProperties;
struct ParagraphProperties;
struct SectionProperties;
struct DocumentProperties;
struct TextAttribute;
struct EditStep;
struct EditOperation;
struct DocumentSelection;
struct SelectionScope;
struct DocumentList;
struct NoteProperties;
struct EditTrace;
struct PropertyMask;
struct DocumentTree;
struct BufferDocument;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfTraceOldContents(
				struct DocumentSelection *	ds,
				const struct EditOperation *	eo,
				int				level,
				unsigned int			flags );

extern int docRtfTraceOldContentsLow(
				const struct EditOperation *	eo,
				const struct DocumentSelection *ds,
				unsigned int			flags );

extern int docRtfTraceOldProperties(
				struct DocumentSelection *	ds,
				struct EditOperation *		eo,
				int				level,
				const struct PropertyMask *	taSetMask,
				const struct PropertyMask *	ppSetMask,
				const struct PropertyMask *	cpSetMask,
				const struct PropertyMask *	rpSetMask,
				const struct PropertyMask *	spSetMask,
				const struct PropertyMask *	dpSetMask );

extern int docRtfTraceNewProperties(
				struct EditOperation *		eo,

				const struct PropertyMask *	taSetMask,
				const struct TextAttribute *	taSet,

				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,

				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,

				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet,

				const struct PropertyMask *	spSetMask,
				const struct SectionProperties * spSet,

				const struct PropertyMask *	dpSetMask,
				const struct DocumentProperties * dpSet );

extern int docRtfTraceHeaderFooter(
				struct EditOperation *		eo,
				struct DocumentTree *		tree );

extern int docRtfTraceNewContents(
				struct EditOperation *		eo,
				int				posWhere );

extern int docRtfTraceNewPosition(
				struct EditOperation *		eo,
				const struct SelectionScope *	ssNew,
				int				posWhere );

extern int docTraceStartStep(	struct EditOperation *		eo,
				struct EditTrace *		et,
				int				command,
				int				fieldKind );

extern int docTraceStartReplace( struct DocumentSelection *	dsTraced,
				struct EditOperation *		eo,
				struct EditTrace *		et,
				int				command,
				int				level,
				unsigned int			flags );

extern int docTraceExtendReplace( struct EditOperation *	eo,
				struct EditTrace *		et,
				int				command );

extern int docRtfTraceFieldKind(	struct EditOperation *	eo,
					int			kind );

extern int docRtfTraceOldField(	struct EditOperation *		eo,
				const struct DocumentField *	df );
extern int docRtfTraceNewField(	struct EditOperation *		eo,
				const struct DocumentField *	df );

extern int docEditReadTraceStep( struct EditStep *		es,
				int *				pIsRepeat,
				int				direction,
				const struct EditTrace *	et,
				const struct BufferDocument *	bd );

extern int docRtfTraceCloseTrace( struct EditOperation *	eo,
				struct EditTrace *		et );

extern int docRtfTraceOldImageProperties(
				struct EditOperation *		eo,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipSet );

extern int docRtfTraceNewImageProperties(
				struct EditOperation *		eo,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipSet );

extern int docRtfTraceOldNoteProperties(
				struct EditOperation *		eo,
				const struct PropertyMask *	npSetMask,
				const struct NoteProperties *	npSet );

extern int docRtfTraceNewNoteProperties(
				struct EditOperation *		eo,
				const struct PropertyMask *	npSetMask,
				const struct NoteProperties *	npSet );

extern int docRtfTraceOldList(	struct EditOperation *		eo,
				const struct DocumentList *	dl );

extern int docRtfTraceNewList(	struct EditOperation *		eo,
				const struct DocumentList *	dl );

extern int docRtfTraceVersion(	int				command,
				const struct MemoryBuffer *	filename,
				const char *			digest64,
				const struct tm *		revtim,
				struct EditTrace *		et,
				struct BufferDocument *		bd );

#    endif	/*  DOC_RTF_TRACE_H	*/
