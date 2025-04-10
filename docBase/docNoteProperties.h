/************************************************************************/
/*									*/
/*  Properties of the notes in a document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_NOTE_PROPERTIES_H
#   define	DOC_NOTE_PROPERTIES_H

#   include	<utilMemoryBuffer.h>

struct PropertyMask;

typedef struct NoteProperties
    {
    MemoryBuffer	npFixedText;
    unsigned char	npTreeType;
    unsigned char	npAutoNumber;
    } NoteProperties;

typedef enum NoteProperty
    {
    NOTEpropFIXED_TEXT= 0,
    NOTEpropTREE_TYPE,
    NOTEpropAUTO_NUMBER,

    NOTEprop_COUNT
    } NoteProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitNoteProperties(	NoteProperties *	np );
extern void docCleanNoteProperties(	NoteProperties *	np );

extern int docCopyNoteProperties(	NoteProperties *	to,
					const NoteProperties *	from );

extern int docNotePropertyDifference(
				struct PropertyMask *		pDifMask,
				const NoteProperties *		np1,
				const struct PropertyMask *	cmpMask,
				const NoteProperties *		np2 );

extern int docUpdNoteProperties( struct PropertyMask *		npDoneMask,
				NoteProperties *		npTo,
				const struct PropertyMask *	npSetMask,
				const NoteProperties *		npSet );

#   endif
