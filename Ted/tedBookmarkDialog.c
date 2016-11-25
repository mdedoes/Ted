/************************************************************************/
/*									*/
/*  Ted: Bookmark dialog.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*  User data for a link dialog.					*/
/************************************************************************/

typedef struct BookmarkContext
    {
    AppDialog		bcDialog;
    APP_WIDGET		bcPaned;

    APP_WIDGET		bcDestinationLabel;
    APP_WIDGET		bcDestinationText;

    APP_WIDGET		bcMarkListWidget;

    APP_WIDGET		bcButtonRow;

    APP_WIDGET		bcSetBookmarkButton;
    APP_WIDGET		bcGoToBookmarkButton;
    APP_WIDGET		bcRemoveBookmarkButton;
    APP_WIDGET		bcCancelButton;

    APP_WIDGET		bcCopyAsFrame;
    APP_WIDGET		bcCopyAsRow;
    AppOptionmenu	bcCopyAsOptionmenu;
    APP_WIDGET		bcCopyAsItems[LINKkind_COUNT];

    APP_WIDGET		bcCopyBookmarkButton;

    char *		bcMarkText;
    char *		bcSetBookmarkText;
    char *		bcGoToBookmarkText;
    char *		bcRemoveBookmarkText;
    char *		bcCancelText;

    char *		bcCopyHeaderText;
    char *		bcCopyButtonText;
    char *		bcCopyAsItemTexts[LINKkind_COUNT];

    char *		bcSavedMark;
    int			bcCopyKind;
	
    EditDocument *	bcDocument;
    } BookmarkContext;

/************************************************************************/
/*									*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_BookmarkResourceTable[]=
    {
    APP_RESOURCE( "bookmarkBookmark",
	    offsetof(BookmarkContext,bcMarkText),
	    "Bookmark" ),

    APP_RESOURCE( "bookmarkCopyHeader",
	    offsetof(BookmarkContext,bcCopyHeaderText),
	    "Copy Bookmark" ),

    APP_RESOURCE( "bookmarkCopyButtom",
	    offsetof(BookmarkContext,bcCopyButtonText),
	    "Copy" ),

    APP_RESOURCE( "bookmarkCopyAsHyperlink",
	    offsetof(BookmarkContext,bcCopyAsItemTexts[LINKkindHYPERLINK]),
	    "As Hyperlink" ),
    APP_RESOURCE( "bookmarkCopyAsPageNumber",
	    offsetof(BookmarkContext,bcCopyAsItemTexts[LINKkindPAGE_NUMBER]),
	    "As Page Number" ),
    APP_RESOURCE( "bookmarkCopyAsBookmarkText",
	    offsetof(BookmarkContext,bcCopyAsItemTexts[LINKkindBOOKMARK_TEXT]),
	    "As Bookmark Text" ),
    APP_RESOURCE( "bookmarkCopyAsTextPlusPage",
	    offsetof(BookmarkContext,bcCopyAsItemTexts[LINKkindTEXT_PLUS_PAGE]),
	    "As Text and Page Number" ),


    APP_RESOURCE( "bookmarkSetBookmark",
	    offsetof(BookmarkContext,bcSetBookmarkText),
	    "Set Bookmark" ),
    APP_RESOURCE( "bookmarkGoToBookmark",
	    offsetof(BookmarkContext,bcGoToBookmarkText),
	    "Go To Bookmark" ),
    APP_RESOURCE( "bookmarkRemoveBookmark",
	    offsetof(BookmarkContext,bcRemoveBookmarkText),
	    "Remove Bookmark" ),
    APP_RESOURCE( "bookmarkCancel",
	    offsetof(BookmarkContext,bcCancelText),
	    "Cancel" ),
    };

/************************************************************************/
/*									*/
/*  Generalised button callback.					*/
/*									*/
/*  NOTE that the NO,YES responses are abused for 'Remove',		*/
/*  'Set Bookmark'.							*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( tedBookmarkDialogClosed, w, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    appGuiBreakDialog( &(bc->bcDialog), AQDrespCANCEL ); return;
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkCancelPushed, button, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    appGuiBreakDialog( &(bc->bcDialog), AQDrespCANCEL ); return;
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkSetPushed, button, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;
    char *			markName;

    markName= appGetStringFromTextWidget( bc->bcDestinationText );
    if  ( ! markName )
	{
	XDEB(markName);
	appGuiBreakDialog( &(bc->bcDialog), AQDrespFAILURE ); return;	}

    if  ( tedSetBookmark( bc->bcDocument, markName ) )
	{
	SDEB(markName);
	appGuiBreakDialog( &(bc->bcDialog), AQDrespFAILURE ); return;	}

    appFreeStringFromTextWidget( markName );

    appGuiBreakDialog( &(bc->bcDialog), AQDrespOK ); return;
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkGotoPushed, button, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;
    char *			markName;

    markName= appGetStringFromTextWidget( bc->bcDestinationText );
    if  ( ! markName )
	{
	XDEB(markName);
	appGuiBreakDialog( &(bc->bcDialog), AQDrespFAILURE ); return;
	}

    if  ( markName[0] )
	{ tedGoToBookmark( bc->bcDocument, markName, strlen( markName ) ); }

    appFreeStringFromTextWidget( markName );

    appGuiBreakDialog( &(bc->bcDialog), AQDrespOK ); return;
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkRemovePushed, w, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    if  ( tedRemoveBookmark( bc->bcDocument ) )
	{ LDEB(1);	}

    appGuiBreakDialog( &(bc->bcDialog), AQDrespOK ); return;
    }

static APP_BUTTON_CALLBACK_H( tedBookmarkCopyPushed, w, voidbc )
    {
    BookmarkContext *			bc= (BookmarkContext *)voidbc;
    char *				markName;

    int					asRef= 0;
    int					asPageref= 0;

    switch( bc->bcCopyKind )
	{
	case LINKkindHYPERLINK:
	    asRef= asPageref= 0;
	    break;

	case LINKkindPAGE_NUMBER:
	    asRef= 0; asPageref= 1;
	    break;

	case LINKkindBOOKMARK_TEXT:
	    asRef= 1; asPageref= 0;
	    break;

	case LINKkindTEXT_PLUS_PAGE:
	    asRef= asPageref= 1;
	    break;

	default:
	    LDEB(bc->bcCopyKind);
	    asRef= asPageref= 0;
	    break;
	}

    markName= appGetStringFromTextWidget( bc->bcDestinationText );
    if  ( ! markName )
	{
	XDEB(markName);
	appGuiBreakDialog( &(bc->bcDialog), AQDrespFAILURE ); return;
	}

    if  ( markName[0] )
	{
	tedCopyBookmarkAsLink( bc->bcDocument, asRef, asPageref, markName );
	}

    appFreeStringFromTextWidget( markName );

    appGuiBreakDialog( &(bc->bcDialog), AQDrespOK ); return;
    }

/************************************************************************/
/*									*/
/*  Typing happened in the 'Bookmark' text widget.			*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedBookmarkDestinationChanged, w, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    char *			markName;
    int				markSize;

    appGuiEnableWidget( bc->bcRemoveBookmarkButton, 0 );

    markName= appGetStringFromTextWidget( bc->bcDestinationText );
    markSize= strlen( markName );

    if  ( markSize > 0 )
	{
	EditDocument *		ed= bc->bcDocument;
	TedDocument *		td= (TedDocument *)ed->edPrivateData;
	BufferDocument *	bd= td->tdDocument;
	DocumentFieldList *	dfl= &(bd->bdFieldList);

	DocumentField *		df;

	if  ( docGetBookmarkField( &df, markName, markSize, dfl ) < 0 )
	    {
	    appGuiEnableWidget( bc->bcGoToBookmarkButton, 0 );
	    appGuiEnableWidget( bc->bcCopyBookmarkButton, 0 );
	    appGuiEnableWidget( bc->bcSetBookmarkButton, ! ed->edIsReadonly );
	    }
	else{
	    appGuiEnableWidget( bc->bcGoToBookmarkButton, 1 );
	    appGuiEnableWidget( bc->bcCopyBookmarkButton, 1 );
	    appGuiEnableWidget( bc->bcSetBookmarkButton, 0 );
	    }
	}
    else{
	appGuiEnableWidget( bc->bcGoToBookmarkButton, 0 );
	appGuiEnableWidget( bc->bcCopyBookmarkButton, 0 );
	appGuiEnableWidget( bc->bcSetBookmarkButton, 0 );
	}

    appFreeStringFromTextWidget( markName );

    return;
    }

/************************************************************************/
/*									*/
/*  A Bookmark was chosen in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedBookmarkChosen, w, voidbc, voidlcs )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    char *			text;

    text= appGuiGetStringFromListCallback( w, voidlcs );
    if  ( ! text )
	{ XDEB(text); return;	}

    appStringToTextWidget( bc->bcDestinationText, text );

    appFreeStringFromListCallback( text );

    appGuiEnableWidget( bc->bcGoToBookmarkButton, 1 );
    appGuiEnableWidget( bc->bcCopyBookmarkButton, 1 );
    appGuiEnableWidget( bc->bcSetBookmarkButton, 0 );
    appGuiEnableWidget( bc->bcRemoveBookmarkButton, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of link has been chosen.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedBookmarkCopyKindKindChosen, w, voidbc )
    {
    BookmarkContext *		bc= (BookmarkContext *)voidbc;

    int				linkKind;

    linkKind= appGuiGetOptionmenuItemIndex( &(bc->bcCopyAsOptionmenu), w );
    if  ( linkKind < 0 || linkKind >= LINKkind_COUNT )
	{ LLDEB(linkKind,LINKkind_COUNT); return;	}

    bc->bcCopyKind= linkKind;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Destination' half of the bookmark dialog.			*/
/*									*/
/************************************************************************/

static void tedBookmarkMakeDestination(	BookmarkContext *	bc,
					const char *		labelText )
    {
    appMakeLabelInColumn( &(bc->bcDestinationLabel), bc->bcPaned, labelText );
    appMakeTextInColumn( &(bc->bcDestinationText), bc->bcPaned, 0, 1 );

    appGuiSetTypingCallbackForText( bc->bcDestinationText,
				tedBookmarkDestinationChanged, (void *)bc );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the row of buttons.						*/
/*									*/
/************************************************************************/

static void tedBookmarkMakeButtonForm(	BookmarkContext *	bc )
    {
    const int		heightResizable= 0;

    bc->bcButtonRow= appMakeRowInColumn( bc->bcPaned, 4, heightResizable );

    appMakeButtonInRow( &(bc->bcSetBookmarkButton), bc->bcButtonRow,
			    bc->bcSetBookmarkText, tedBookmarkSetPushed,
			    (void *)bc, 0, 1 );

    appMakeButtonInRow( &(bc->bcGoToBookmarkButton), bc->bcButtonRow,
			    bc->bcGoToBookmarkText, tedBookmarkGotoPushed,
			    (void *)bc, 1, 0 );

    appMakeButtonInRow( &(bc->bcRemoveBookmarkButton), bc->bcButtonRow,
			    bc->bcRemoveBookmarkText, tedBookmarkRemovePushed,
			    (void *)bc, 2, 0 );

    appMakeButtonInRow( &(bc->bcCancelButton), bc->bcButtonRow,
			    bc->bcCancelText, tedBookmarkCancelPushed,
			    (void *)bc, 3, 0 );

    appGuiSetDefaultButtonForDialog( &(bc->bcDialog),
					    bc->bcSetBookmarkButton );
    appGuiSetCancelButtonForDialog( &(bc->bcDialog),
					    bc->bcCancelButton );
    }

static void tedBookmarkMakeList(	BookmarkContext *	bc )
    {
    APP_WIDGET		list;
    const int		visibleItems= 12;

    appGuiMakeListInColumn( &list, bc->bcPaned, visibleItems,
					    tedBookmarkChosen, (void *)bc );

    bc->bcMarkListWidget= list;
    
    return;
    }

/************************************************************************/
/*									*/
/*  Make the framework of a Bookmark Dialog.				*/
/*									*/
/************************************************************************/

static APP_DESTROY_CALLBACK_H( tedFreeBookmarkContext, w, voidbc )
    { free( voidbc ); }

static int tedMakeBookmarkDialog(	BookmarkContext **	pBc,
					EditApplication *	ea,
					APP_WIDGET		relative )
    {
    BookmarkContext *	bc;

    bc= (BookmarkContext *)malloc( sizeof(BookmarkContext) );
    if  ( ! bc )
	{ XDEB(bc); return -1;	}

    appGuiGetResourceValues( ea, (void *)bc, TED_BookmarkResourceTable,
					sizeof(TED_BookmarkResourceTable)/
					sizeof(AppConfigurableResource) );

    bc->bcDestinationText= (APP_WIDGET)0;
    bc->bcMarkListWidget= (APP_WIDGET)0;

    bc->bcSetBookmarkButton= (APP_WIDGET)0;
    bc->bcGoToBookmarkButton= (APP_WIDGET)0;
    bc->bcRemoveBookmarkButton= (APP_WIDGET)0;
    bc->bcCopyBookmarkButton= (APP_WIDGET)0;
    bc->bcCancelButton= (APP_WIDGET)0;

    bc->bcSavedMark= (char *)0;

    appMakeVerticalDialog( &(bc->bcDialog), &(bc->bcPaned), ea,
			    tedBookmarkDialogClosed,
			    tedFreeBookmarkContext,
			    (void *)bc,
			    "tedBookmark" );

    tedBookmarkMakeDestination( bc, bc->bcMarkText );

    tedBookmarkMakeList( bc );

    appMakeRowFrameInColumn( &(bc->bcCopyAsFrame), &(bc->bcCopyAsRow),
				    bc->bcPaned, 4, bc->bcCopyHeaderText );

    appMakeButtonInRow( &(bc->bcCopyBookmarkButton), bc->bcCopyAsRow,
			    bc->bcCopyButtonText, tedBookmarkCopyPushed,
			    (void *)bc, 0, 0 );

    appMakeOptionmenuInRow( &(bc->bcCopyAsOptionmenu),
						    bc->bcCopyAsRow, 1, 3 );


    tedBookmarkMakeButtonForm( bc );

    *pBc= bc;
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Run the 'Bookmark Dialog' for a certain document.			*/
/*									*/
/************************************************************************/

void tedRunBookmarkDialog(	EditApplication *	ea,
				EditDocument *		ed,
				APP_WIDGET		option,
				int			nameProposed,
				const char *		markName,
				int			markSize )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentFieldList *		dfl= &(bd->bdFieldList);

    APP_WIDGET			relative= ed->edToplevel.atTopWidget;

    static BookmarkContext *	bookmarkContext;
    BookmarkContext *		bc;

    int				justMade= 0;

    if  ( ! bookmarkContext )
	{
	if  ( tedMakeBookmarkDialog( &bookmarkContext, ea, relative ) )
	    { LDEB(1); return;	}

	justMade= 1;
	}

    bc= bookmarkContext;

    bc->bcDocument= ed;

    if  ( bc->bcSavedMark )
	{
	free( bc->bcSavedMark );
	bc->bcSavedMark= (char *)0;
	}

    if  ( markSize > 0 )
	{
	DocumentField *		df;
	char *			saved;

	saved= (char *)malloc( markSize+ 1 );

	if  ( ! saved )
	    { XDEB(saved);	}
	else{
	    strncpy( saved, markName, markSize )[markSize]= '\0';
	    appStringToTextWidget( bc->bcDestinationText, saved );
	    bc->bcSavedMark= saved;
	    }

	appGuiEnableWidget( bc->bcRemoveBookmarkButton,
				    ! ed->edIsReadonly && ! nameProposed );

	if  ( docGetBookmarkField( &df, markName, markSize, dfl ) < 0 )
	    {
	    appGuiEnableWidget( bc->bcGoToBookmarkButton, 0 );
	    appGuiEnableWidget( bc->bcCopyBookmarkButton, 0 );
	    }
	else{
	    appGuiEnableWidget( bc->bcGoToBookmarkButton, 1 );
	    appGuiEnableWidget( bc->bcCopyBookmarkButton, 1 );
	    }
	}
    else{
	appGuiEnableWidget( bc->bcRemoveBookmarkButton, 0 );
	appGuiEnableWidget( bc->bcGoToBookmarkButton, 0 );
	appGuiEnableWidget( bc->bcCopyBookmarkButton, 0 );

	appStringToTextWidget( bc->bcDestinationText, "" );
	}

    if  ( ! nameProposed )
	{ appGuiEnableWidget( bc->bcSetBookmarkButton, 0 ); }

    tedFillBookmarkList( bc->bcMarkListWidget, dfl );

    if  ( justMade )
	{
	appFillInspectorMenu( LINKkind_COUNT, LINKkindHYPERLINK,
				bc->bcCopyAsItems,
				bc->bcCopyAsItemTexts,
				&(bc->bcCopyAsOptionmenu),
				tedBookmarkCopyKindKindChosen, (void *)bc );
	}

    bc->bcCopyKind= LINKkindHYPERLINK;
    appSetOptionmenu( &(bc->bcCopyAsOptionmenu), LINKkindHYPERLINK );

    appGuiShowDialog( &(bc->bcDialog), ed->edToplevel.atTopWidget );

    appSetShellTitle( bc->bcDialog.adTopWidget,
					    option, ea->eaApplicationName );

    appOptionmenuRefreshWidth( &(bc->bcCopyAsOptionmenu) );

#   ifdef USE_MOTIF
    XmProcessTraversal( bc->bcDestinationText, XmTRAVERSE_CURRENT );
#   endif

    appGuiRunDialog( &(bc->bcDialog), AQDrespNONE, ea );

    appGuiHideDialog( &(bc->bcDialog) );

    return;
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

static int tedCompareBookmarkStrings(	const void *	pv1,
					const void *	pv2 )
    {
    const char * const *	ps1= (const char * const *)pv1;
    const char * const *	ps2= (const char * const *)pv2;

    return strcmp( *ps1, *ps2 );
    }

void tedFillBookmarkList(	APP_WIDGET		list,
				DocumentFieldList *	dfl )
    {
    DocumentField *	df;
    int			i;

    int			bookmarkCount= 0;
    char **		bookmarks= (char **)0;

    /*  1  */
    appGuiEmptyListWidget( list );

    if  ( dfl->dflFieldCount == 0 )
	{ goto ready;	}

    /*  2  */
    bookmarks= (char **)malloc( dfl->dflFieldCount* sizeof(char *) );
    if  ( ! bookmarks )
	{ LXDEB(dfl->dflFieldCount,bookmarks); goto ready;	}

    df= dfl->dflFields;
    for ( i= 0; i < dfl->dflFieldCount; df++, i++ )
	{
	const char *	name;
	int		size;

	if  ( df->dfKind != DOCfkBOOKMARK		||
	      docFieldGetBookmark( df, &name, &size )	)
	    { continue;		}

	if  ( size == 0			||
	      name[0] == '_'		||
	      size >= DOCmaxBOOKMARK	)
	    { continue;	}

	bookmarks[bookmarkCount]= malloc( size+ 1 );
	if  ( ! bookmarks[bookmarkCount] )
	    { LXDEB(size,bookmarks[bookmarkCount]); goto ready; }

	strncpy( bookmarks[bookmarkCount], name, size )[size]= '\0';
	bookmarkCount++;
	}

    /*  3  */
    qsort( bookmarks, bookmarkCount, sizeof(char *),
					    tedCompareBookmarkStrings );

    /*  4  */
    for ( i= 0; i < bookmarkCount; i++ )
	{ appGuiAddValueToListWidget( list, bookmarks[i] );	}

  ready:

    for ( i= 0; i < bookmarkCount; i++ )
	{
	if  ( bookmarks[i] )
	    { free( bookmarks[i] );	}
	}

    if  ( bookmarks )
	{ free( bookmarks );	}

    return;
    }
