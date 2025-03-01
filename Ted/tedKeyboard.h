/************************************************************************/
/*									*/
/*  Ted: Handling keyboard input from the user.				*/
/*									*/
/************************************************************************/

#   ifndef		TED_KEYBOARD_H
#   define		TED_KEYBOARD_H

struct EditDocument;
struct DocumentSelection;
struct SelectionDescription;
struct SelectionGeometry;
struct DocumentPosition;
struct BufferDocument;

typedef void (*TedKeyHandler)(	struct EditDocument *			ed,
				int					keySym,
				int					state,
				const struct DocumentSelection *	ds,
				const struct SelectionDescription *	sd,
				const struct SelectionGeometry *	sg );

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern TedKeyHandler tedKeyGetMoveHandler(
			struct EditDocument *			ed,
			int					keySym,
			int					state,
			const struct DocumentSelection *	ds,
			const struct SelectionDescription *	sd );

extern TedKeyHandler tedKeyGetEditHandler(
			int *					pEditCommand,
			struct EditDocument *			ed,
			int					keySym,
			int					state,
			const struct DocumentSelection *	ds,
			const struct SelectionDescription *	sd );

extern void tedInputSetSelectedPosition(
			struct EditDocument *			ed,
			const struct DocumentPosition *		dp,
			int					lastLine );


extern void tedInputChangeSelection(
			struct EditDocument *			ed,
			unsigned int				keyState,
			const struct DocumentSelection *	dsOrig,
			const struct DocumentPosition *		dpSet,
			int					lastLine );

int tedMoveLeftOnKey(	struct DocumentPosition *		dpNew,
			struct BufferDocument *			bd,
			int					keyState );

int tedMoveRightOnKey(	struct DocumentPosition *		dpNew,
			struct BufferDocument *			bd,
			int					keyState );

#   endif	/*	TED_KEYBOARD_H	*/
