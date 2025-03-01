/************************************************************************/
/*									*/
/*  Ted: The list of bookmarks on the Bookmark and Hyperlink tools.	*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stdlib.h>

#   include	"tedBookmarkList.h"
#   include	<tedAppFront.h>
#   include	<guiTextUtil.h>
#   include	<docBuf.h>
#   include	<guiWidgets.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/

void tedInitBookmarkList(	BookmarkList *		bl )
    {
    bl->blMarkLabelWidget= (APP_WIDGET)0;
    bl->blMarkTextWidget= (APP_WIDGET)0;

    bl->blMarkListWidget= (APP_WIDGET)0;

    bl->blIsLocal= 0;
    bl->blMarkChosenExists= 0;
    }

/************************************************************************/
/*									*/
/*  Fill a listbox with the bookmarks in a document.			*/
/*									*/
/*  1)  Empty the list widget.						*/
/*  2)  Collect bookmarks.						*/
/*  3)  Sort them.							*/
/*  4)  And insert them in the list.					*/
/*									*/
/************************************************************************/

void tedFillBookmarkList(	BookmarkList *		bl,
				int			includeTocMarks,
				const struct BufferDocument *	bd )
    {
    int				i;

    int				bookmarkCount= 0;
    char **			bookmarks= (char **)0;
    const DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;

    APP_WIDGET			list= bl->blMarkListWidget;

    /*  1  */
    guiEmptyListWidget( list );

    if  ( fieldCount == 0 )
	{ goto ready;	}

    /*  2,3  */
    if  ( docMakeBookmarkList( &bookmarks, &bookmarkCount,
				    includeTocMarks, &(bd->bdFieldList) ) )
	{ goto ready;	}

    /*  4  */
    for ( i= 0; i < bookmarkCount; i++ )
	{ guiAddValueToListWidget( list, -1, bookmarks[i] );	}

  ready:

    if  ( bookmarks )
	{
	for ( i= 0; i < bookmarkCount; i++ )
	    {
	    if  ( bookmarks[i] )
		{ free( bookmarks[i] );	}
	    }

	free( bookmarks );
	}

    return;
    }

void tedBookmarkUpdateSelectionInList(	BookmarkList *		bl,
					const MemoryBuffer *	mbChosen )
    {
    if  ( bl->blIsLocal && bl->blMarkChosenExists )
	{
	guiSelectValueInListWidget( bl->blMarkListWidget,
				    utilMemoryBufferGetString( mbChosen ) );
	}
    else{
	guiRemoveSelectionFromListWidget( bl->blMarkListWidget );
	}
    }

void tedBookmarkFindChosen(	BookmarkList *			bl,
				struct EditApplication *	ea,
				const MemoryBuffer *		mbChosen )
    {
    if  ( mbChosen && ! utilMemoryBufferIsEmpty( mbChosen ) )
	{
	struct DocumentField *		df;

	if  ( tedAppFindBookmarkField( &df, ea, mbChosen ) < 0 )
	    { bl->blMarkChosenExists= 0;	}
	else{ bl->blMarkChosenExists= 1;	}
	}
    else{ bl->blMarkChosenExists= 0;		}

    return;
    }

/************************************************************************/
/*									*/
/*  A Bookmark was chosen in the list.					*/
/*									*/
/************************************************************************/

void tedBookmarkListToText(	BookmarkList *		bl,
				MemoryBuffer *		mbChosen,
				void *			voidlcs,
				APP_WIDGET		w )
    {
    char *			markName;

    markName= guiGetStringFromListCallback( w, voidlcs );
    if  ( ! markName )
	{ XDEB(markName); return;	}
    guiStringToText( bl->blMarkTextWidget, markName );

    guiFreeStringFromListCallback( markName );

    if  ( guiBufferFromText( mbChosen, bl->blMarkTextWidget ) )
	{ LDEB(1);			}
    else{ bl->blMarkChosenExists= 1;	}

    return;
    }

