/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_NOTE_H
#   define	DOC_DOCUMENT_NOTE_H

#   include	"docDocumentTree.h"
#   include	<docNoteProperties.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;

/************************************************************************/
/*									*/
/*  A footnote/endnote/annotation.					*/
/*									*/
/*  1)  Though the notes are stored with the section, the paragraph	*/
/*	number is relative to the body of the document as a whole.	*/
/*									*/
/************************************************************************/

typedef struct DocumentNote
    {
    DocumentTree	dnDocumentTree;
    short int		dnNoteNumber;
    short int		dnReferringPage;
    short int		dnReferringColumn;
    NoteProperties	dnNoteProperties;
    } DocumentNote;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitNote(	struct DocumentNote *		dn );

extern void docCleanNote(	struct BufferDocument *		bd,
				struct DocumentNote *		dn );

#   endif
