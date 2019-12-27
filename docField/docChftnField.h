/************************************************************************/
/*									*/
/*  Note field Field							*/
/*									*/
/************************************************************************/

#   ifndef DOC_CHFTN_FIELD_H
#   define DOC_CHFTN_FIELD_H

struct MemoryBuffer;
struct FootEndNotesProperties;
struct NoteProperties;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docFormatChftnField(
			struct MemoryBuffer *			mbResult,
			const struct FootEndNotesProperties *	fep,
			const struct NoteProperties *		np,
			int					noteNumber );

#   endif /* DOC_CHFTN_FIELD_H */
