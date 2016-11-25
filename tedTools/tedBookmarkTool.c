/************************************************************************/
/*									*/
/*  Ted: Bookmark dialog.						*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<tedAppFront.h>
#   include	"tedBookmarkTool.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docField.h>
#   include	<docBookmarkField.h>
#   include	<docEditCommand.h>
#   include	<docFieldKind.h>
#   include	"tedBookmarkList.h"
#   include	"tedFormatToolSubject.h"
#   include	<appInspectorSubject.h>
#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<guiWidgets.h>
#   include	<appGuiResource.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  User data for a bookmark dialog.					*/
/*									*/
/************************************************************************/

typedef struct BookmarkPageResources
    {
    const char *	bprMarkText;
    const char *	bprShowTocMarks;

    const char *	bprGoToBookmarkButtonText;
    const char *	bprRemoveBookmarkButtonText;
    } BookmarkPageResources;

typedef struct BookmarkTool
    {
    struct EditApplication *		btApplication;
    struct AppInspector *		btInspector;
    const BookmarkPageResources *	btResources;
    InspectorSubject *			btInspectorSubject;

    BookmarkList			btBookmarkList;

    APP_WIDGET				btGoToBookmarkButton;
    APP_WIDGET				btRemoveBookmarkButton;

    unsigned int			btCurrentDocumentId;
    int					btCanChange;
    MemoryBuffer			btMarkSet;
    MemoryBuffer			btMarkChosen;

    int					btInProgrammaticChange;
    } BookmarkTool;

static BookmarkPageResources		TED_BookmarkPageResources;
static InspectorSubjectResources	TED_BookmarkSubjectResources;

/************************************************************************/
/*									*/
/*  Initialize/Clean a bookmark tool					*/
/*									*/
/************************************************************************/

static void tedInitBookmarkTool(		BookmarkTool *		bt )
    {
    bt->btCurrentDocumentId= 0;

    tedInitBookmarkList( &(bt->btBookmarkList) );
    bt->btBookmarkList.blIsLocal= 1;

    bt->btGoToBookmarkButton= (APP_WIDGET)0;
    bt->btRemoveBookmarkButton= (APP_WIDGET)0;

    utilInitMemoryBuffer( &(bt->btMarkSet) );
    utilInitMemoryBuffer( &(bt->btMarkChosen) );

    bt->btInProgrammaticChange= 0;

    return;
    }

static void tedFormatFreeBookmarkTool(	void *	vbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)vbt;

    utilCleanMemoryBuffer( &(bt->btMarkSet) );
    utilCleanMemoryBuffer( &(bt->btMarkChosen) );

    free( bt );

    return;
    }
/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedBookmarkAdaptToText(	const BookmarkTool *	bt,
					int			adaptString )
    {
    const BookmarkList *	bl= &(bt->btBookmarkList);

    int				gotChosen;
    int				changed;

    InspectorSubject *		is= bt->btInspectorSubject;

    changed= utilMemoryCompareBuffers( &(bt->btMarkChosen), &(bt->btMarkSet) );
    gotChosen= ! utilMemoryBufferIsEmpty( &(bt->btMarkChosen) );

    guiEnableWidget( is->isApplyButton,
		    ! bl->blMarkChosenExists		&&
		    bt->btCanChange			&&
		    changed && gotChosen		);

    guiEnableWidget( is->isRevertButton, changed );

    guiEnableWidget( bt->btRemoveBookmarkButton,
		     bt->btCanChange			&&
		     ! changed && gotChosen		);

    guiEnableWidget( bt->btGoToBookmarkButton,
		    bl->blMarkChosenExists && gotChosen );

    guiEnableWidget( bl->blMarkListWidget, bl->blIsLocal );
    guiEnableText( bl->blMarkTextWidget, bt->btCanChange );

    if  ( adaptString )
	{
	guiBufferToText( bl->blMarkTextWidget, &(bt->btMarkChosen) );
	}

    return;
    }

static void tedBookmarkAdaptToBookmark(	BookmarkTool *		bt,
					int			adaptString,
					int			select )
    {
    tedBookmarkFindChosen( &(bt->btBookmarkList), bt->btApplication,
							&(bt->btMarkChosen) );

    if  ( select )
	{
	tedBookmarkUpdateSelectionInList( &(bt->btBookmarkList),
							&(bt->btMarkChosen) );
	}

    tedBookmarkAdaptToText( bt, adaptString );
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkRevertPushed, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const int			adaptString= 1;
    const int			select= 1;

    utilCopyMemoryBuffer( &(bt->btMarkChosen), &(bt->btMarkSet) );

    bt->btInProgrammaticChange++;

    tedBookmarkAdaptToBookmark( bt, adaptString, select );

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Reset the document id to force the tool to refresh.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkChangePushed, button, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    if  ( guiBufferFromText( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}
    else{
	if  ( docAdaptBookmarkName( &(bt->btMarkChosen) ) )
	    {
	    bt->btInProgrammaticChange++;

	    guiBufferToText( bl->blMarkTextWidget,
						    &(bt->btMarkChosen) );
	    guiRefuseTextValue( bl->blMarkTextWidget );

	    bt->btInProgrammaticChange--;
	    return;
	    }

	/*  1  */
	bt->btCurrentDocumentId= 0;

	tedAppSetBookmark( bt->btApplication, &(bt->btMarkChosen) );
	}
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkGotoPushed, button, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    if  ( guiBufferFromText( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}
    else{
	if  ( tedAppGoToBookmark( bt->btApplication, &(bt->btMarkChosen) ) )
	    { LDEB(1);	}
	}
    }

/************************************************************************/
/*									*/
/*  Remove the selected bookmark.					*/
/*									*/
/*  1)  Reset the document id to force the tool to refresh.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedBookmarkRemovePushed, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;

    /*  1  */
    bt->btCurrentDocumentId= 0;

    tedAppRemoveBookmark( bt->btApplication );

    return;
    }

/************************************************************************/
/*									*/
/*  Typing happened in the 'Bookmark' text widget.			*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedBookmarkDestinationChanged, w, voidbt )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    const BookmarkList *	bl= &(bt->btBookmarkList);

    const int			adaptString= 0;
    const int			select= 1;

    guiEnableWidget( bt->btRemoveBookmarkButton, 0 );

    if  ( guiBufferFromText( &(bt->btMarkChosen),
						bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    tedBookmarkAdaptToBookmark( bt, adaptString, select );

    return;
    }

/************************************************************************/
/*									*/
/*  A Bookmark was chosen in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedBookmarkChosen, w, voidbt, voidlcs )
    {
    BookmarkTool *		bt= (BookmarkTool *)voidbt;
    BookmarkList *		bl= &(bt->btBookmarkList);
    const int			adaptString= 0;

    char *			markName= (char *)0;

    if  ( bt->btInProgrammaticChange )
	{ return;	}

    bt->btInProgrammaticChange++;

    if  ( bt->btCanChange )
	{
	tedBookmarkListToText( bl, &(bt->btMarkChosen), voidlcs, w );
	tedBookmarkAdaptToText( bt, adaptString );
	}
    else{
	markName= guiGetStringFromListCallback( w, voidlcs );
	if  ( ! markName )
	    { XDEB(markName); goto ready;	}

	if  ( utilMemoryBufferSetString( &(bt->btMarkChosen), markName ) )
	    { SDEB(markName); goto ready;	}

	if  ( tedAppGoToBookmark( bt->btApplication, &(bt->btMarkChosen) ) )
	    { LDEB(1); goto ready;	}
	}

  ready:

    if  ( markName )
	{ guiFreeStringFromListCallback( markName );	}

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a bookmark tool. I.E. the 'Bookmark' page on the format tool	*/
/*  to manage Hyperlinks.						*/
/*									*/
/************************************************************************/

static void * tedBuildBookmarkTool(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const BookmarkPageResources *	bpr= &TED_BookmarkPageResources;

    BookmarkList *	bl;
    APP_WIDGET		row;
    const int		listVisibleItems= 5;

    BookmarkTool *	bt= malloc( sizeof(BookmarkTool) );

    if  ( ! bt )
	{ XDEB(bt); return bt;	}

    tedInitBookmarkTool( bt );

    bl= &(bt->btBookmarkList);

    bt->btResources= bpr;
    bt->btInspector= ai;
    bt->btApplication= ea;
    bt->btInspectorSubject= is;

    /* FULL WIDTH */
    guiMakeLabelInColumn( &(bl->blMarkLabelWidget),
				pageWidget, bpr->bprMarkText );
    guiMakeTextInColumn( &(bl->blMarkTextWidget), pageWidget, 0, 1 );

    guiSetTypingCallbackForText( bl->blMarkTextWidget,
				tedBookmarkDestinationChanged, (void *)bt );

    guiMakeListInColumn( &(bl->blMarkListWidget),
		pageWidget, listVisibleItems,
		tedBookmarkChosen, (APP_BUTTON_CALLBACK_T)0, (void *)bt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(bt->btGoToBookmarkButton), &(bt->btRemoveBookmarkButton),
	    bpr->bprGoToBookmarkButtonText, bpr->bprRemoveBookmarkButtonText,
	    tedBookmarkGotoPushed, tedBookmarkRemovePushed, (void *)bt );

    return (void *)bt;
    }


static void tedBookmarkToolResetCache(	void *	vbt )
    {
    BookmarkTool *			bt= (BookmarkTool *)vbt;

    bt->btCurrentDocumentId= -1;
    }

/************************************************************************/
/*									*/
/*  Refresh the bookmark tool.						*/
/*									*/
/************************************************************************/

static void tedRefreshBookmarkTool(
				void *				vbt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    BookmarkTool *		bt= (BookmarkTool *)vbt;
    const int			adaptString= 1;

    bt->btInProgrammaticChange++;

    if  ( bt->btCurrentDocumentId != sd->sdDocumentId )
	{
	const BookmarkPageResources *	bpr= bt->btResources;
	int				includeTocMarks= 0;

	if  ( bpr->bprShowTocMarks && ! strcmp( bpr->bprShowTocMarks, "1" ) )
	    { includeTocMarks= 1;	}

	tedFillBookmarkList( &(bt->btBookmarkList), includeTocMarks, bd );

	bt->btCurrentDocumentId= sd->sdDocumentId;
	}

    bt->btCanChange= cmdEnabled[EDITcmdSET_BOOKMARK];

    utilCleanMemoryBuffer( &(bt->btMarkSet) );
    utilInitMemoryBuffer( &(bt->btMarkSet) );
    utilCleanMemoryBuffer( &(bt->btMarkChosen) );
    utilInitMemoryBuffer( &(bt->btMarkChosen) );

    if  ( sd->sdHeadInBookmark )
	{
	struct DocumentField *		dfBookmark;

	const MemoryBuffer *	markName= (const MemoryBuffer *)0;

	dfBookmark= docFindTypedFieldForPosition(
				    (struct BufferDocument *)bd, &(ds->dsHead),
				    DOCfkBOOKMARK, 0 );
	if  ( docFieldGetBookmark( &markName, dfBookmark ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(bt->btMarkSet), markName ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(bt->btMarkChosen), &(bt->btMarkSet) ) )
	    { LDEB(1); *pEnabled= 0; goto ready;	}

	bt->btBookmarkList.blMarkChosenExists= 1;
	tedBookmarkAdaptToText( bt, adaptString );

	if  ( ! docIsTocBookmark( (long *)0, dfBookmark ) )
	    {
	    *pPref += 3;

	    tedBookmarkUpdateSelectionInList( &(bt->btBookmarkList),
							&(bt->btMarkChosen) );
	    }
	}
    else{
	const int	select= 1;

	if  ( ! sd->sdIsIBarSelection )
	    { docSuggestNewBookmarkName( &(bt->btMarkChosen), bd, ds );	}

	tedBookmarkAdaptToBookmark( bt, adaptString, select );
	}

  ready:

    bt->btInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedBookmarkSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolBookmark",
	    offsetof(InspectorSubjectResources,isrSubjectName),
	    "Bookmark" ),

    APP_RESOURCE( "bookmarkSetBookmark",
	    offsetof(InspectorSubjectResources,isrApplyToSubject),
	    "Set Bookmark" ),
    APP_RESOURCE( "bookmarkRevert",
	    offsetof(InspectorSubjectResources,isrRevert),
	    "Revert" ),
    };

static AppConfigurableResource TED_TedBookmarkToolResourceTable[]=
    {
    APP_RESOURCE( "bookmarkBookmark",
	    offsetof(BookmarkPageResources,bprMarkText),
	    "Bookmark" ),
    APP_RESOURCE( "bookmarkShowTocMarks",
	    offsetof(BookmarkPageResources,bprShowTocMarks),
	    "0" ),

    APP_RESOURCE( "bookmarkGoToBookmark",
	    offsetof(BookmarkPageResources,bprGoToBookmarkButtonText),
	    "Go To Bookmark" ),
    APP_RESOURCE( "bookmarkRemoveBookmark",
	    offsetof(BookmarkPageResources,bprRemoveBookmarkButtonText),
	    "Remove Bookmark" ),
    };

static void tedBookmarkToolGetResourceTable(
					struct EditApplication *	ea,
					BookmarkPageResources *		bpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)bpr,
				TED_TedBookmarkToolResourceTable,
				sizeof(TED_TedBookmarkToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedBookmarkSubjectResourceTable,
				sizeof(TED_TedBookmarkSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedBookmarkToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedBookmarkToolGetResourceTable( ea,
			    &TED_BookmarkPageResources,
			    &TED_BookmarkSubjectResources );

    ist->istResources= &TED_BookmarkSubjectResources;

    ist->istBuildTool= tedBuildBookmarkTool;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ist->istFreeTool= tedFormatFreeBookmarkTool;

    ftst->ftstRefreshPage= tedRefreshBookmarkTool;
    ftst->ftstResetCache= tedBookmarkToolResetCache;

    ist->istRevertChanges= tedBookmarkRevertPushed;
    ist->istApplyChanges= tedBookmarkChangePushed;

    return 0;
    }
