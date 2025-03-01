/************************************************************************/
/*									*/
/*  Manage notes in a document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_NOTES_H
#   define	DOC_NOTES_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct DocumentNote;
struct BufferDocument;
struct DocumentField;
struct DocumentSelection;
struct BufferItem;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docInsertNote(	struct DocumentNote **		pDn,
				struct BufferDocument *		bd,
				struct DocumentField *		dfNote,
				int				autoNumber );

extern void docDeleteNote(	struct BufferDocument *		bd,
				int				noteIndex );

extern struct DocumentField * docGetSelectedNote(
			struct DocumentNote **			pDn,
			int *					pSelInNote,
			struct BufferDocument *			bd,
			const struct DocumentSelection *	ds );

extern struct DocumentField * docGetFirstNoteFromColumn(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType );

extern struct DocumentField * docGetFirstNoteInColumn(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType );

extern struct DocumentField * docGetLastNoteInColumn(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType );

extern struct DocumentField * docGetFirstNoteOnPage(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				treeType );

extern struct DocumentField * docGetFirstNoteOfSection(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				sect,
				int				treeType );

extern struct DocumentField * docGetFirstNoteOfDocument(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				treeType );

extern struct DocumentField * docGetLastNoteOfDocument(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				treeType );

extern struct DocumentField * docGetNextNoteInSection(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				int				sect,
				struct DocumentField *		df,
				int				treeType );

extern struct DocumentField * docGetNextNoteInDocument(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				struct DocumentField *		df,
				int				treeType );

extern struct DocumentField * docGetPrevNoteInDocument(
				struct DocumentNote **		pDn,
				const struct BufferDocument *	bd,
				struct DocumentField *		df,
				int				treeType );

extern struct DocumentNote *	docGetNoteOfField(
				const struct DocumentField *	dfNote,
				const struct BufferDocument *	bd );

extern int docExtractFixedTextNote(
				struct DocumentNote *		dn,
				struct BufferDocument *		bd,
				struct BufferItem *		ownerNode,
				int				fixedStroff,
				int				fixedStrlen,
				const char * const		instBytes,
				const int			instSize );

extern int docNoteIncludeFixedTextInField(
				int *				pFixedStroff,
				int *				pFixedStrlen,
				struct BufferItem *		ownerNode,
				struct DocumentField *		dfNote );

extern int docCheckNoteSeparatorExistence(
				struct BufferDocument *		bd,
				int				sepTreeType );

extern int docCheckSeparatorExistenceForNoteType(
				struct BufferDocument *		bd,
				int				noteTreeType );

extern int docSetNoteLinks(	struct MemoryBuffer *		mbTarget,
				struct MemoryBuffer *		mbSource,
				struct BufferItem *		paraNode,
				const struct DocumentField *	dfChftn );

extern int docCollectNoteTitle(	struct MemoryBuffer *		mbTitle,
				const struct DocumentNote *	dn,
				struct BufferDocument *		bd );

extern int docGetNotePosition(	struct DocumentNote *		dn,
				struct BufferDocument *		bd );

extern int docGetEndnotePlacement(	struct BufferDocument *	bd );

#   endif
