/************************************************************************/
/*									*/
/*  Ted: The list of bookmarks on the Bookmark and Hyperlink tools.	*/
/*									*/
/************************************************************************/

#   ifndef	TED_BOOKMARK_LIST_H
#   define	TED_BOOKMARK_LIST_H

#   include	<guiBase.h>

struct BufferDocument;
struct EditApplication;
struct MemoryBuffer;

typedef struct BookmarkList
    {
    APP_WIDGET				blMarkLabelWidget;
    APP_WIDGET				blMarkTextWidget;

    APP_WIDGET				blMarkListWidget;

    unsigned char			blIsLocal;
    unsigned char			blMarkChosenExists;
    } BookmarkList;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedInitBookmarkList(	BookmarkList *		bl );

extern void tedFillBookmarkList(	BookmarkList *		bl,
					int			includeTocMarks,
					const struct BufferDocument *	bd );

extern void tedBookmarkFindChosen(	BookmarkList *		bl,
					struct EditApplication *	ea,
					const struct MemoryBuffer *	mbChosen );

extern void tedBookmarkUpdateSelectionInList(
					BookmarkList *		bl,
					const struct MemoryBuffer *	mbChosen );

extern void tedBookmarkListToText(	BookmarkList *		bl,
					struct MemoryBuffer *		mbChosen,
					void *			voidlcs,
					APP_WIDGET		w );

#   endif	/*  TED_BOOKMARK_LIST_H  */
