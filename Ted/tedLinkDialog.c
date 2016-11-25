/************************************************************************/
/*									*/
/*  Ted: Link dialog.							*/
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
/*									*/
/*  User data for a link dialog.					*/
/*									*/
/************************************************************************/

typedef struct LinkContext
    {
    AppDialog		lcDialog;
    APP_WIDGET		lcPaned;

    APP_WIDGET		lcFileTextWidget;
    APP_WIDGET		lcMarkTextWidget;
    APP_WIDGET		lcMarkListWidget;

    APP_WIDGET		lcButtonRow;
    APP_WIDGET		lcSetLinkButton;
    APP_WIDGET		lcFollowLinkButton;
    APP_WIDGET		lcRemoveLinkButton;
    APP_WIDGET		lcCancelButton;

    APP_WIDGET		lcLinkAsFrame;
    APP_WIDGET		lcLinkAsPaned;
    AppOptionmenu	lcLinkAsOptionmenu;
    APP_WIDGET		lcLinkAsItems[LINKkind_COUNT];

    char *		lcFileText;
    char *		lcMarkText;

    char *		lcLinkAsText;
    char *		lcLinkAsItemTexts[LINKkind_COUNT];

    char *		lcSetLinkText;
    char *		lcFollowLinkText;
    char *		lcRemoveLinkText;
    char *		lcCancelText;

    char *		lcSavedFile;
    char *		lcSavedMark;

    int			lcLinkKind;

    EditDocument *	lcDocument;
    } LinkContext;

/************************************************************************/
/*									*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_LinkResourceTable[]=
{
    APP_RESOURCE( "hyperlinkDocument",
		offsetof(LinkContext,lcFileText),
		"Document" ),
    APP_RESOURCE( "hyperlinkBookmark",
		offsetof(LinkContext,lcMarkText),
		"Bookmark" ),

    APP_RESOURCE( "hyperlinkLinkAs",
		offsetof(LinkContext,lcLinkAsText),
		"Link as" ),

    APP_RESOURCE( "hyperlinkLinkAsHyperlink",
		offsetof(LinkContext,lcLinkAsItemTexts[LINKkindHYPERLINK]),
		"Hyperlink" ),
    APP_RESOURCE( "hyperlinkLinkAsPageNumber",
		offsetof(LinkContext,lcLinkAsItemTexts[LINKkindPAGE_NUMBER]),
		"Page Number" ),
    APP_RESOURCE( "hyperlinkLinkAsBookmarkText",
		offsetof(LinkContext,lcLinkAsItemTexts[LINKkindBOOKMARK_TEXT]),
		"Bookmark Text" ),
    APP_RESOURCE( "hyperlinkLinkAsTextPlusPage",
		offsetof(LinkContext,lcLinkAsItemTexts[LINKkindTEXT_PLUS_PAGE]),
		"Text and Page Number" ),

    APP_RESOURCE( "hyperlinkSetLink",
		offsetof(LinkContext,lcSetLinkText),
		"Set Link" ),
    APP_RESOURCE( "hyperlinkRemoveLink",
		offsetof(LinkContext,lcRemoveLinkText),
		"Remove Link" ),
    APP_RESOURCE( "hyperlinkFollowLink",
		offsetof(LinkContext,lcFollowLinkText),
		"Follow Link" ),
    APP_RESOURCE( "hyperlinkCancel",
		offsetof(LinkContext,lcCancelText),
		"Cancel" ),
};

/************************************************************************/
/*									*/
/*  Button callbacks.							*/
/*									*/
/*  NOTE that the NO,YES responses are abused for 'Remove,Set Link'.	*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( tedLinkDialogClosed, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    appGuiBreakDialog( &(lc->lcDialog), AQDrespCANCEL );
    }

static APP_BUTTON_CALLBACK_H( tedLinkSetButtonPushed, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    appGuiBreakDialog( &(lc->lcDialog), AQDrespYES );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLinkFollowButtonPushed, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    appGuiBreakDialog( &(lc->lcDialog), AQDrespOK );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLinkRemoveButtonPushed, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    appGuiBreakDialog( &(lc->lcDialog), AQDrespNO );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLinkCancelButtonPushed, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    appGuiBreakDialog( &(lc->lcDialog), AQDrespCANCEL );

    return;
    }

/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedLinkAdaptToText(	LinkContext *		lc,
				const char *		fileName,
				const char *		markName )
    {
    EditDocument *		ed= lc->lcDocument;
    int				gotSomething;
    int				isLocal= fileName[0] == '\0';

    const char *		savedFile= lc->lcSavedFile;
    const char *		savedMark= lc->lcSavedMark;

    int				changed;

    if  ( ! savedFile )
	{ savedFile= "";	}
    if  ( ! savedMark )
	{ savedMark= "";	}

    gotSomething= markName[0] != '\0' || fileName[0] != '\0';
    changed= strcmp( fileName, savedFile ) || strcmp( markName, savedMark );

    appGuiEnableWidget( lc->lcSetLinkButton,
			! ed->edIsReadonly && changed && gotSomething );
    appGuiEnableWidget( lc->lcRemoveLinkButton, gotSomething && ! changed );

    appGuiEnableWidget( lc->lcFollowLinkButton, gotSomething );

    appGuiEnableWidget( lc->lcMarkListWidget, isLocal );

    if  ( lc->lcLinkAsItems[LINKkindHYPERLINK] )
	{
	appGuiEnableWidget( lc->lcLinkAsItems[LINKkindPAGE_NUMBER], isLocal );
	appGuiEnableWidget( lc->lcLinkAsItems[LINKkindBOOKMARK_TEXT], isLocal );
	appGuiEnableWidget( lc->lcLinkAsItems[LINKkindTEXT_PLUS_PAGE], isLocal);

	if  ( lc->lcLinkKind != LINKkindHYPERLINK && ! isLocal )
	    {
	    lc->lcLinkKind= LINKkindHYPERLINK;
	    appSetOptionmenu( &(lc->lcLinkAsOptionmenu), LINKkindHYPERLINK );
	    }
	}

    return;
    }

/*  A  */
static APP_LIST_CALLBACK_H( tedLinkBookmarkChosen, w, voidlc, voidlcs )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    char *			fileName;
    char *			markName;

    markName= appGuiGetStringFromListCallback( w, voidlcs );
    if  ( ! markName )
	{ XDEB(markName); return;	}
    fileName= appGetStringFromTextWidget( lc->lcFileTextWidget );
    if  ( ! fileName )
	{ XDEB(fileName); return;	}

    appStringToTextWidget( lc->lcMarkTextWidget, markName );

    tedLinkAdaptToText( lc, fileName, markName );

    appFreeStringFromListCallback( markName );
    appFreeStringFromTextWidget( fileName );

    return;
    }

/*  B  */
static APP_TXTYPING_CALLBACK_H( tedLinkDestinationChanged, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;
    char *			fileName;
    char *			markName;

    fileName= appGetStringFromTextWidget( lc->lcFileTextWidget );
    if  ( ! fileName )
	{ XDEB(fileName); return;	}
    markName= appGetStringFromTextWidget( lc->lcMarkTextWidget );
    if  ( ! markName )
	{ XDEB(markName); return;	}

    tedLinkAdaptToText( lc, fileName, markName );

    appFreeStringFromTextWidget( fileName );
    appFreeStringFromTextWidget( markName );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of link has been chosen.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedLinkKindChosen, w, voidlc )
    {
    LinkContext *		lc= (LinkContext *)voidlc;

    int				linkKind;

    linkKind= appGuiGetOptionmenuItemIndex( &(lc->lcLinkAsOptionmenu), w );
    if  ( linkKind < 0 || linkKind >= LINKkind_COUNT )
	{ LLDEB(linkKind,LINKkind_COUNT); return;	}

    lc->lcLinkKind= linkKind;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the strip with the buttons.					*/
/*									*/
/************************************************************************/

static void tedLinkMakeButtonForm(	LinkContext *	lc )
    {
    const int		heightResizable= 0;

    lc->lcButtonRow= appMakeRowInColumn( lc->lcPaned, 4, heightResizable );


    appMakeButtonInRow( &(lc->lcSetLinkButton), lc->lcButtonRow,
				lc->lcSetLinkText,
				tedLinkSetButtonPushed, (void *)lc, 0, 1 );

    appMakeButtonInRow( &(lc->lcFollowLinkButton), lc->lcButtonRow,
				lc->lcFollowLinkText,
				tedLinkFollowButtonPushed, (void *)lc, 1, 0 );

    appMakeButtonInRow( &(lc->lcRemoveLinkButton), lc->lcButtonRow,
				lc->lcRemoveLinkText,
				tedLinkRemoveButtonPushed, (void *)lc, 2, 0 );

    appMakeButtonInRow( &(lc->lcCancelButton), lc->lcButtonRow,
				lc->lcCancelText,
				tedLinkCancelButtonPushed, (void *)lc, 3, 0 );

    appGuiSetDefaultButtonForDialog( &(lc->lcDialog), lc->lcSetLinkButton );
    appGuiSetCancelButtonForDialog( &(lc->lcDialog), lc->lcCancelButton );

    return;
    }

static void tedLinkMakeList(	LinkContext *	lc )
    {
    APP_WIDGET		list;
    const int		visibleItems= 9;

    appGuiMakeListInColumn( &list, lc->lcPaned, visibleItems,
					tedLinkBookmarkChosen, (void *)lc );

    lc->lcMarkListWidget= list;
    
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'link' dialog.						*/
/*									*/
/************************************************************************/

static APP_DESTROY_CALLBACK_H( tedFreeLinkContext, w, voidlc )
    { free( voidlc ); return;	}

static int tedMakeLinkDialog(		LinkContext **		pLc,
					EditApplication *	ea,
					APP_WIDGET		relative )
    {
    APP_WIDGET		label;
    int			i;

    LinkContext *	lc= (LinkContext *)malloc( sizeof(LinkContext) );

    if  ( ! lc )
	{ XDEB(lc); return -1;	}

    appGuiGetResourceValues( ea, (void *)lc, TED_LinkResourceTable,
					sizeof(TED_LinkResourceTable)/
					sizeof(AppConfigurableResource) );

    for ( i= 0; i < LINKkind_COUNT; i++ )
	{ lc->lcLinkAsItems[i]= (APP_WIDGET)0; }

    lc->lcFileTextWidget= (APP_WIDGET)0;
    lc->lcMarkTextWidget= (APP_WIDGET)0;

    lc->lcSetLinkButton= (APP_WIDGET)0;
    lc->lcFollowLinkButton= (APP_WIDGET)0;
    lc->lcRemoveLinkButton= (APP_WIDGET)0;
    lc->lcCancelButton= (APP_WIDGET)0;

    lc->lcSavedFile= (char *)0;
    lc->lcSavedMark= (char *)0;

    appInitOptionmenu( &(lc->lcLinkAsOptionmenu) );

    appMakeVerticalDialog( &(lc->lcDialog), &(lc->lcPaned), ea,
						tedLinkDialogClosed,
						tedFreeLinkContext,
						(void *)lc,
						"tedHyperlink" );

    appMakeLabelInColumn( &label, lc->lcPaned, lc->lcFileText );
    appMakeTextInColumn( &(lc->lcFileTextWidget), lc->lcPaned, 0, 1 );
    appMakeLabelInColumn( &label, lc->lcPaned, lc->lcMarkText );
    appMakeTextInColumn( &(lc->lcMarkTextWidget), lc->lcPaned, 0, 1 );

    appGuiSetTypingCallbackForText( lc->lcFileTextWidget,
				tedLinkDestinationChanged, (void *)lc );
    appGuiSetTypingCallbackForText( lc->lcMarkTextWidget,
				tedLinkDestinationChanged, (void *)lc );

    tedLinkMakeList( lc );

    appMakeColumnFrameInColumn( &(lc->lcLinkAsFrame), &(lc->lcLinkAsPaned),
						    lc->lcPaned,
						    lc->lcLinkAsText );

    appMakeOptionmenuInColumn( &(lc->lcLinkAsOptionmenu),
						    lc->lcLinkAsPaned );

    tedLinkMakeButtonForm( lc );

    *pLc= lc;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the 'Destination' half of the link dialog.			*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Run the 'Link Dialog' for a certain document.			*/
/*									*/
/************************************************************************/

void tedRunLinkDialog(	EditApplication *	ea,
			EditDocument *		ed,
			APP_WIDGET		option,
			const char *		fileName,
			int			fileSize,
			const char *		markName,
			int			markSize )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentFieldList *		dfl= &(bd->bdFieldList);

    APP_WIDGET			relative= ed->edToplevel.atTopWidget;
    static LinkContext *	linkContext;
    LinkContext *		lc;

    int				justMade= 0;

    if  ( fileSize == 0 )
	{ fileName= "";	}
    if  ( markSize == 0 )
	{ markName= "";	}

    if  ( ! linkContext )
	{
	if  ( tedMakeLinkDialog( &linkContext, ea, relative ) )
	    { LDEB(1); return;	}

	justMade= 1;
	}

    lc= linkContext;

    /*******************************/

    lc->lcDocument= ed;

    if  ( lc->lcSavedFile )
	{
	free( lc->lcSavedFile );
	lc->lcSavedFile= (char *)0;
	}
    if  ( lc->lcSavedMark )
	{
	free( lc->lcSavedMark );
	lc->lcSavedMark= (char *)0;
	}

    tedFillBookmarkList( lc->lcMarkListWidget, dfl );

    if  ( fileSize+ markSize > 0 )
	{
	char *		saved;

	if  ( fileSize > 0 )
	    {
	    saved= (char *)malloc( fileSize+ 1 );

	    if  ( ! saved )
		{ XDEB(saved);	}
	    else{
		strncpy( saved, fileName, fileSize )[fileSize]= '\0';
		appStringToTextWidget( lc->lcFileTextWidget, saved );
		lc->lcSavedFile= saved;
		fileName= lc->lcSavedFile;
		}
	    }
	else{ appStringToTextWidget( lc->lcFileTextWidget, "" ); }

	if  ( markSize > 0 )
	    {
	    saved= (char *)malloc( markSize+ 1 );

	    if  ( ! saved )
		{ XDEB(saved);	}
	    else{
		strncpy( saved, markName, markSize )[markSize]= '\0';
		appStringToTextWidget( lc->lcMarkTextWidget, saved );
		lc->lcSavedMark= saved;
		markName= lc->lcSavedMark;
		}
	    }
	else{ appStringToTextWidget( lc->lcMarkTextWidget, "" ); }

	appGuiEnableWidget( lc->lcRemoveLinkButton, ! ed->edIsReadonly );
	appGuiEnableWidget( lc->lcFollowLinkButton, 1 );
	}
    else{
	appGuiEnableWidget( lc->lcRemoveLinkButton, 0 );
	appGuiEnableWidget( lc->lcFollowLinkButton, 0 );

	appStringToTextWidget( lc->lcFileTextWidget, "" );
	appStringToTextWidget( lc->lcMarkTextWidget, "" );
	}

    appGuiEnableWidget( lc->lcMarkListWidget, fileName[0] == '\0' );

    appGuiEnableWidget( lc->lcSetLinkButton, 0 );

    if  ( justMade )
	{
	appFillInspectorMenu( LINKkind_COUNT, LINKkindHYPERLINK,
				lc->lcLinkAsItems,
				lc->lcLinkAsItemTexts,
				&(lc->lcLinkAsOptionmenu),
				tedLinkKindChosen, (void *)lc );
	}

    lc->lcLinkKind= LINKkindHYPERLINK;
    appSetOptionmenu( &(lc->lcLinkAsOptionmenu), LINKkindHYPERLINK );

    appGuiEnableOptionmenu( &(lc->lcLinkAsOptionmenu),
					    fileSize+ markSize == 0 );

    tedLinkAdaptToText( lc, fileName, markName );

    appGuiShowDialog( &(lc->lcDialog), ed->edToplevel.atTopWidget );

    appSetShellTitle( lc->lcDialog.adTopWidget, 
					    option, ea->eaApplicationName );

    appOptionmenuRefreshWidth( &(lc->lcLinkAsOptionmenu) );

#   ifdef USE_MOTIF
    XmProcessTraversal( lc->lcFileTextWidget, XmTRAVERSE_CURRENT );
#   endif

    appGuiRunDialog( &(lc->lcDialog), AQDrespNONE, ea );

    appGuiHideDialog( &(lc->lcDialog) );

    switch( lc->lcDialog.adResponse )
	{
	char *		file;
	char *		mark;

	case AQDrespYES:
	    {
	    int		asRef;
	    int		asPageref;

	    file= appGetStringFromTextWidget( lc->lcFileTextWidget );
	    if  ( ! file )
		{ XDEB(file); return;	}
	    mark= appGetStringFromTextWidget( lc->lcMarkTextWidget );
	    if  ( ! mark )
		{ XDEB(mark); return;	}

	    switch( lc->lcLinkKind )
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
		    LDEB(lc->lcLinkKind);
		    asRef= asPageref= 0;
		    break;
		}

	    if  ( tedSetHyperlink( ed, file, mark, asRef, asPageref ) )
		{ SSDEB(file,mark);	}

	    appFreeStringFromTextWidget( file );
	    appFreeStringFromTextWidget( mark );
	    }
	    return;

	case AQDrespOK: /* follow */
	    {
	    file= appGetStringFromTextWidget( lc->lcFileTextWidget );
	    if  ( ! file )
		{ XDEB(file); return;	}
	    mark= appGetStringFromTextWidget( lc->lcMarkTextWidget );
	    if  ( ! mark )
		{ XDEB(mark); return;	}

	    fileSize= strlen( file );
	    markSize= strlen( mark );

	    tedFollowLink( relative, option, ed,
				    file, fileSize, mark, markSize );

	    appFreeStringFromTextWidget( file );
	    appFreeStringFromTextWidget( mark );
	    }
	    return;

	case AQDrespNO:
	    if  ( tedRemoveHyperlink( ed ) )
		{ LDEB(1);	}
	    return;
	case AQDrespCANCEL:
	    return;
	default:
	    LDEB(lc->lcDialog.adResponse); return;
	}
    }
