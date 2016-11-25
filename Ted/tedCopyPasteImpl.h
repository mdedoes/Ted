/************************************************************************/
/*									*/
/*  Implementation of copy/paste related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_COPY_PASTE_IMPL_H
#   define	TED_COPY_PASTE_IMPL_H

struct EditDocument;
struct BufferDocument;
struct DocumentSelection;
struct MemoryBuffer;
struct SelectionDescription;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedSaveSelectionTxtToFile(
				struct BufferDocument *		bd,
				const char *			filename );

extern void tedSaveSelectionToFile(
				struct BufferDocument *		bd,
				const struct DocumentSelection *	ds,
				int				rtfFlags,
				const char *			filename );

extern int tedDocSaveSelectionRtf(	struct MemoryBuffer *		mb,
					struct DocumentSelection *	ds,
					struct SelectionDescription *	sd,
					struct EditDocument *	ed );

extern int tedDocCopySelection(		struct EditDocument *	ed );

extern int tedApplyPastedRuler(		struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

extern int tedApplyPastedFont(		struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

extern void tedCopyFont( 		struct EditDocument *	ed );

extern void tedCopyRuler( 		struct EditDocument *	ed );

#   endif	/*  TED_COPY_PASTE_IMPL_H	*/
