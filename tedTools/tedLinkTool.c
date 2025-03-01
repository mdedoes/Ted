/************************************************************************/
/*									*/
/*  Ted: Link page on format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<tedAppFront.h>
#   include	"tedLinkTool.h"
#   include	<docField.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	<docFieldKind.h>
#   include	"tedBookmarkList.h"
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  User data for a link dialog.					*/
/*									*/
/************************************************************************/

typedef struct LinkToolResources
    {
    const char *		ltrFileText;
    const char *		ltrMarkText;

    const char *		ltrFollowLinkButtonText;
    const char *		ltrRemoveLinkButtonText;
    } LinkToolResources;

typedef struct LinkTool
    {
    struct EditApplication *	ltApplication;
    struct AppInspector *	ltInspector;
    const LinkToolResources *	ltPageResources;
    InspectorSubject *		ltInspectorSubject;

    APP_WIDGET			ltFileTextWidget;

    BookmarkList		ltBookmarkList;

    APP_WIDGET			ltFollowLinkButton;
    APP_WIDGET			ltRemoveLinkButton;

    unsigned int		ltCurrentDocumentId;
    int				ltCanChange;
    int				ltInProgrammaticChange;
    HyperlinkField		ltLinkSet;
    HyperlinkField		ltLinkChosen;
    } LinkTool;

static LinkToolResources		TED_LinkToolResources;
static InspectorSubjectResources	TED_LinkSubjectResources;

/************************************************************************/
/*									*/
/*  Initialize/Clean a link tool					*/
/*									*/
/************************************************************************/

static void tedInitLinkTool(		LinkTool *		lt )
    {
    lt->ltCurrentDocumentId= 0;

    tedInitBookmarkList( &(lt->ltBookmarkList) );

    lt->ltFollowLinkButton= (APP_WIDGET)0;
    lt->ltRemoveLinkButton= (APP_WIDGET)0;

    docInitHyperlinkField( &(lt->ltLinkChosen) );
    docInitHyperlinkField( &(lt->ltLinkSet) );

    lt->ltInProgrammaticChange= 0;

    return;
    }

static void tedFormatFreeLinkTool(	void *	vlt )
    {
    LinkTool *			lt= (LinkTool *)vlt;

    docCleanHyperlinkField( &(lt->ltLinkChosen) );
    docCleanHyperlinkField( &(lt->ltLinkSet) );

    free( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  A)  A Bookmark was chosen in the list.				*/
/*  B)  Something was typed in the 'Destination' text widgets.		*/
/*									*/
/************************************************************************/

static void tedLinkAdaptToText(	LinkTool *		lt )
    {
    int				gotSomething;
    int				changed;
    int				canSetLink;

    BookmarkList *		bl= &(lt->ltBookmarkList);

    const HyperlinkField *	hfChosen= &(lt->ltLinkChosen);
    const HyperlinkField *	hfSet= &(lt->ltLinkSet);

    InspectorSubject *		is= lt->ltInspectorSubject;

    lt->ltInProgrammaticChange++;

    bl->blIsLocal= ! docHyperlinkFieldIsExtern( hfChosen );
    gotSomething= docHyperlinkFieldIsLink( hfChosen );

    changed= ! docEqualHyperlinkFields( hfChosen, hfSet );
    canSetLink= lt->ltCanChange && changed && gotSomething;

    guiEnableWidget( is->isApplyButton, canSetLink );
    guiEnableWidget( is->isRevertButton, changed );

    guiEnableWidget( lt->ltRemoveLinkButton,
				lt->ltCanChange && gotSomething && ! changed );

    guiEnableWidget( lt->ltFollowLinkButton, gotSomething );

    guiEnableWidget( bl->blMarkListWidget,
				    lt->ltCanChange && bl->blIsLocal );

    guiEnableText( lt->ltFileTextWidget, lt->ltCanChange );
    guiEnableText( bl->blMarkTextWidget, lt->ltCanChange );

    lt->ltInProgrammaticChange--;

    return;
    }

static void tedLinkAdaptToValues(	LinkTool *		lt,
					int			adaptFile,
					int			adaptMark,
					int			search,
					int			select )
    {
    BookmarkList *		bl= &(lt->ltBookmarkList);
    const HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

    bl->blIsLocal= ! docHyperlinkFieldIsExtern( hfChosen );

    if  ( search )
	{
	tedBookmarkFindChosen( bl, lt->ltApplication, &(hfChosen->hfBookmark) );
	}

    lt->ltInProgrammaticChange++;

    if  ( adaptFile)
	{
	guiBufferToText( lt->ltFileTextWidget, &(hfChosen->hfFile) );
	}

    if  ( adaptMark )
	{
	guiBufferToText( bl->blMarkTextWidget, &(hfChosen->hfBookmark) );
	}

    if  ( select )
	{
	tedBookmarkUpdateSelectionInList( bl, &(hfChosen->hfBookmark) );
	}

    tedLinkAdaptToText( lt );

    lt->ltInProgrammaticChange--;

    return;
    }

/*  A  */
static APP_LIST_CALLBACK_H( tedLinkBookmarkChosen, w, voidlt, voidlcs )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);

    lt->ltInProgrammaticChange++;

    tedBookmarkListToText( bl, &(hfChosen->hfBookmark), voidlcs, w );
    tedLinkAdaptToText( lt );

    lt->ltInProgrammaticChange--;

    return;
    }

/*  B  */
static APP_TXTYPING_CALLBACK_H( tedLinkFileChanged, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;

    if  ( ! lt->ltInProgrammaticChange )
	{
	HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

	const int		adaptToFile= 0;
	const int		adaptToMark= 0;
	const int		search= 1;
	const int		select= 1;

	if  ( guiBufferFromText( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );
	}

    return;
    }

static APP_TXTYPING_CALLBACK_H( tedLinkMarkChanged, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);

    if  ( ! lt->ltInProgrammaticChange )
	{
	HyperlinkField *	hfChosen= &(lt->ltLinkChosen);

	const int		adaptToFile= 0;
	const int		adaptToMark= 0;
	const int		search= 1;
	const int		select= 1;

	if  ( guiBufferFromText( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Revert' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRevertPushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;

    const int		adaptToFile= 1;
    const int		adaptToMark= 1;
    const int		search= 1;
    const int		select= 1;

    if  ( docCopyHyperlinkField( &(lt->ltLinkChosen), &(lt->ltLinkSet) ) )
	{ LDEB(1); return;	}

    tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );

    return;
    }

/************************************************************************/
/*									*/
/*  'Change' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkChangePushed, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);
    struct EditApplication *	ea= lt->ltApplication;

    if  ( guiBufferFromText( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	{ LDEB(1); return;	}
    if  ( guiBufferFromText( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    if  ( docAdaptBookmarkName( &(hfChosen->hfBookmark) ) )
	{
	lt->ltInProgrammaticChange++;

	guiBufferToText( bl->blMarkTextWidget,
					    &(hfChosen->hfBookmark) );
	guiRefuseTextValue( bl->blMarkTextWidget );

	lt->ltInProgrammaticChange--;
	return;
	}

    tedAppSetHyperlink( ea, hfChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  'Follow Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkFollowPushed, w, voidlt )
    {
    LinkTool *			lt= (LinkTool *)voidlt;
    BookmarkList *		bl= &(lt->ltBookmarkList);
    HyperlinkField *		hfChosen= &(lt->ltLinkChosen);
    struct EditApplication *		ea= lt->ltApplication;

    if  ( guiBufferFromText( &(hfChosen->hfFile),
						    lt->ltFileTextWidget ) )
	{ LDEB(1); return;	}
    if  ( guiBufferFromText( &(hfChosen->hfBookmark),
						    bl->blMarkTextWidget ) )
	{ LDEB(1); return;	}

    if  ( tedAppFollowLink( w, ea,
			    &(hfChosen->hfFile), &(hfChosen->hfBookmark) ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  'Remove Link' was pushed.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLinkRemovePushed, w, voidlt )
    {
    LinkTool *		lt= (LinkTool *)voidlt;
    struct EditApplication *	ea= lt->ltApplication;

    tedAppRemoveHyperlink( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a link tool. I.E. the 'Link' page on the format tool to manage	*/
/*  Hyperlinks.								*/
/*									*/
/************************************************************************/

static void * tedBuildLinkTool(	int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const LinkToolResources *	ltr= &TED_LinkToolResources;

    BookmarkList *	bl;
    APP_WIDGET		label;
    APP_WIDGET		row;

    const int		listVisibleItems= 5;

    LinkTool *		lt= malloc( sizeof(LinkTool) );

    if  ( ! lt )
	{ XDEB(lt); return lt;	}

    tedInitLinkTool( lt );

    bl= &(lt->ltBookmarkList);

    lt->ltPageResources= ltr;
    lt->ltInspector= ai;
    lt->ltApplication= ea;
    lt->ltInspectorSubject= is;

    /* FULL WIDTH */
    guiMakeLabelInColumn( &label, pageWidget, ltr->ltrFileText );
    guiMakeTextInColumn( &(lt->ltFileTextWidget), pageWidget, 0, 1 );
    guiMakeLabelInColumn( &(bl->blMarkLabelWidget),
				pageWidget, ltr->ltrMarkText );
    guiMakeTextInColumn( &(bl->blMarkTextWidget), pageWidget, 0, 1 );

    guiSetTypingCallbackForText( lt->ltFileTextWidget,
					    tedLinkFileChanged, (void *)lt );
    guiSetTypingCallbackForText( bl->blMarkTextWidget,
					    tedLinkMarkChanged, (void *)lt );

    guiMakeListInColumn( &(bl->blMarkListWidget),
		pageWidget, listVisibleItems,
		tedLinkBookmarkChosen, (APP_BUTTON_CALLBACK_T)0, (void *)lt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(lt->ltFollowLinkButton), &(lt->ltRemoveLinkButton),
	    ltr->ltrFollowLinkButtonText, ltr->ltrRemoveLinkButtonText,
	    tedLinkFollowPushed, tedLinkRemovePushed, (void *)lt );

    return (void *)lt;
    }

static void tedLinkToolResetCache(	void *	vlt )
    {
    LinkTool *			lt= (LinkTool *)vlt;

    lt->ltCurrentDocumentId= -1;
    }

/************************************************************************/
/*									*/
/*  Refresh the link tool.						*/
/*									*/
/************************************************************************/

static void tedRefreshLinkTool(	void *				vlt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    LinkTool *			lt= (LinkTool *)vlt;

    struct DocumentField *	dfHyperlink;

    lt->ltCanChange= cmdEnabled[EDITcmdSET_HYPERLINK];

    if  ( lt->ltCurrentDocumentId != sd->sdDocumentId )
	{
	const int	includeTocMarks= 0;

	lt->ltInProgrammaticChange++;
	tedFillBookmarkList( &(lt->ltBookmarkList), includeTocMarks, bd );
	lt->ltInProgrammaticChange--;

	lt->ltCurrentDocumentId= sd->sdDocumentId;
	}

    docCleanHyperlinkField( &(lt->ltLinkChosen) );
    docInitHyperlinkField( &(lt->ltLinkChosen) );

    docCleanHyperlinkField( &(lt->ltLinkSet) );
    docInitHyperlinkField( &(lt->ltLinkSet) );

    dfHyperlink= docFindTypedFieldForPosition( (struct BufferDocument *)bd,
					&(ds->dsHead), DOCfkHYPERLINK, 0 );
    if  ( dfHyperlink )
	{
	const int		adaptToFile= 1;
	const int		adaptToMark= 1;
	const int		search= 1;
	const int		select= 1;

	if  ( docGetHyperlinkField( &(lt->ltLinkSet), dfHyperlink ) )
	    { LDEB(1); return;	}
	if  ( docCopyHyperlinkField( &(lt->ltLinkChosen), &(lt->ltLinkSet) ) )
	    { LDEB(1); return;	}

	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );

	*pPref += 3;
	}
    else{
	const int		adaptToFile= 1;
	const int		adaptToMark= 1;
	const int		search= 0;
	const int		select= 1;

	lt->ltBookmarkList.blMarkChosenExists= 0;
	tedLinkAdaptToValues( lt, adaptToFile, adaptToMark, search, select );

	if  ( sd->sdIsIBarSelection )
	    { *pEnabled= 0;	}
	}

    guiEnableWidget( is->isApplyButton, 0 && lt->ltCanChange );
    guiEnableWidget( is->isRevertButton, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Get link tool resources.						*/
/*  Resource table for various texts.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedLinkSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolLink",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Hyperlink" ),

    APP_RESOURCE( "hyperlinkSetLink",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set Link" ),
    APP_RESOURCE( "hyperlinkRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedLinkToolResourceTable[]=
{
    APP_RESOURCE( "hyperlinkDocument",
		offsetof(LinkToolResources,ltrFileText),
		"Document" ),
    APP_RESOURCE( "hyperlinkBookmark",
		offsetof(LinkToolResources,ltrMarkText),
		"Bookmark" ),

    APP_RESOURCE( "hyperlinkFollowLink",
		offsetof(LinkToolResources,ltrFollowLinkButtonText),
		"Follow Link" ),
    APP_RESOURCE( "hyperlinkRemoveLink",
		offsetof(LinkToolResources,ltrRemoveLinkButtonText),
		"Remove Link" ),
};

static void tedLinkToolGetResourceTable(
					struct EditApplication *	ea,
					LinkToolResources *		ltr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)ltr,
				TED_TedLinkToolResourceTable,
				sizeof(TED_TedLinkToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedLinkSubjectResourceTable,
				sizeof(TED_TedLinkSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedLinkToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedLinkToolGetResourceTable( ea,
			&TED_LinkToolResources,
			&TED_LinkSubjectResources );

    ist->istResources= &TED_LinkSubjectResources;

    ist->istBuildTool= tedBuildLinkTool;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ist->istFreeTool= tedFormatFreeLinkTool;

    ftst->ftstRefreshPage= tedRefreshLinkTool;
    ftst->ftstResetCache= tedLinkToolResetCache;

    ist->istRevertChanges= tedLinkRevertPushed;
    ist->istApplyChanges= tedLinkChangePushed;

    return 0;
    }
