/************************************************************************/
/*									*/
/*  The 'Bullets and Numbering' page of the format tool.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"docEvalField.h"
#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Supported styles:							*/
/*									*/
/************************************************************************/

static const int TED_ListToolStyles[LISTTOOLcountSTYLES]=
    {
    0,
    1,
    2,
    3,
    4,
    22,
    23,
    255,
    };

/************************************************************************/
/*									*/
/*  Refresh the controls with the current list level.			*/
/*									*/
/************************************************************************/

static void tedListToolRefreshCurrentLevel(	ListTool *	lt )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const DocumentListLevel *	dll;
    int				level= lt->ltCurrentLevel;

    char			scratch[22];

    int				i;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{
	appStringToTextWidget( lt->ltLevelText, "" );
	appStringToTextWidget( lt->ltStartAtText, "" );
	appSetOptionmenu( &(lt->ltNumberStyleOptionmenu), -1 );
	appSetOptionmenu( &(lt->ltJustifyOptionmenu), -1 );
	appSetOptionmenu( &(lt->ltFollowOptionmenu), -1 );

	return;
	}

    dll= &(dl->dlLevels[level]);

    appGeoLengthToString( scratch,
	    dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( lt->ltFirstIndentText, scratch );

    appGeoLengthToString( scratch, dll->dllLeftIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( lt->ltLeftIndentText, scratch );


    appIntegerToTextWidget( lt->ltLevelText, level+ 1 );
    appIntegerToTextWidget( lt->ltStartAtText, dll->dllStartAt );

    for ( i= 0; i < LISTTOOLcountSTYLES; i++ )
	{
	if  ( dll->dllNumberStyle == TED_ListToolStyles[i] )
	    { break;	}
	}
    if  ( i >= LISTTOOLcountSTYLES )
	{ i= -1;	}

    appSetOptionmenu( &(lt->ltNumberStyleOptionmenu), i );
    appSetOptionmenu( &(lt->ltJustifyOptionmenu), dll->dllJustification );
    appSetOptionmenu( &(lt->ltFollowOptionmenu), dll->dllFollow );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a certain tab in the list.					*/
/*									*/
/************************************************************************/

static void tedListToolSelectLevel(	ListTool *	lt,
					int		n )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);

    if  ( n >= 0 && n >= dl->dlLevelCount )
	{ LLDEB(n,dl->dlLevelCount); n= -1;	}

    if  ( n >= 0 )
	{
	lt->ltCurrentLevel= n;

	appGuiSelectPositionInListWidget( lt->ltListLevelList, n );
	}
    else{
	lt->ltCurrentLevel= -1;

	appGuiRemoveSelectionFromListWidget( lt->ltListLevelList );
	}

    tedListToolRefreshCurrentLevel( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the list of tabs and the currently selected tab.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshListPage(	ListTool *	lt )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const ListOverride *	lo= &(lt->ltListOverrideChosen);

    appGuiEmptyListWidget( lt->ltListLevelList );

    if  ( dl->dlLevelCount > 0 )
	{
	char				scratch[100];
	unsigned char			listtext[50];

	const DocumentListLevel *	dll;
	int				lev;

	docListGetFormatPath( lt->ltStartPath, lt->ltFormatPath,
					&dll, dl->dlLevelCount- 1, dl, lo );

	dll= dl->dlLevels;
	for ( lev= 0; lev < dl->dlLevelCount; dll++, lev++ )
	    {
	    int		numberPath[DLmaxLEVELS];
	    int		l;

	    if  ( lt->ltPrevLevel >= 0 )
		{
		for ( l= 0; l <= lev; l++ )
		    { numberPath[l]= lt->ltPrevPath[l];	}

		if  ( lev > lt->ltPrevLevel )
		    {
		    for ( l= lt->ltPrevLevel+ 1; l <= lev; l++ )
			{ numberPath[l]= 0;	}
		    }
		else{
		    numberPath[lev]= lt->ltPrevPath[lev]+ 1;
		    }
		}
	    else{
		for ( l= 0; l <= lev; l++ )
		    { numberPath[l]= lt->ltHerePath[l];	}
		}

	    if  ( docCalculateListtextString( listtext, sizeof(listtext)-1,
						lev, numberPath,
						lt->ltStartPath,
						lt->ltFormatPath, dll ) < 0 )
		{ listtext[0]= '\0';	}

	    sprintf( scratch, "%d: %s", lev+ 1, (char *)listtext );

	    appGuiAddValueToListWidget( lt->ltListLevelList, scratch );
	    }

	tedListToolSelectLevel( lt, lt->ltCurrentLevel );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the tabs tool to the current selection in the document.	*/
/*									*/
/************************************************************************/

static void tedFormatToolNoList(	ListTool *		lt,
					InspectorSubject *	is )
    {
    docCleanDocumentList( &(lt->ltListPropertiesSet) );
    docCleanDocumentList( &(lt->ltListPropertiesChosen) );

    docInitDocumentList( &(lt->ltListPropertiesSet) );
    docInitDocumentList( &(lt->ltListPropertiesChosen) );

    docCleanListOverride( &(lt->ltListOverrideSet) );
    docCleanListOverride( &(lt->ltListOverrideChosen) );

    docInitListOverride( &(lt->ltListOverrideSet) );
    docInitListOverride( &(lt->ltListOverrideChosen) );

    lt->ltCurrentLevel= -1;
    lt->ltHereLevel= -1;
    lt->ltPrevLevel= -1;
    lt->ltCurrentParagraphNumber= -1;

    appGuiEnableWidget( lt->ltRemoveFromListButton, 0 );
    appGuiEnableWidget( is->isApplyButton, 0 );
    appGuiEnableWidget( is->isRevertButton, 0 );

    appGuiEnableWidget( lt->ltListLevelFrame, 0 );
    appGuiEnableWidget( lt->ltSetLevelButton, 0 );

    return;
    }


static int tedFormatToolSetList(	ListTool *			lt,
					InspectorSubject *		is,
					int				paraNr,
					int				ls,
					int				level,
					ListOverride *			lo,
					DocumentList *			dl,
					ListNumberTreeNode *		root )
    {
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;

    if  ( docCopyDocumentList( &(lt->ltListPropertiesSet), dl,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyDocumentList( &(lt->ltListPropertiesChosen), dl,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}

    if  ( docCopyListOverride( &(lt->ltListOverrideSet), lo,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyListOverride( &(lt->ltListOverrideChosen), lo,
						    fontMap, colorMap ) )
	{ LDEB(1); return -1;	}

    lt->ltHereLevel= level;

    if  ( paraNr != lt->ltCurrentParagraphNumber			||
	  lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >=
			    lt->ltListPropertiesChosen.dlLevelCount	)
	{ lt->ltCurrentLevel= level;	}

    lt->ltCurrentParagraphNumber= paraNr;

    docListNumberTreeGetNumberPath( lt->ltHerePath, root, level, paraNr );

    if  ( docListNumberTreeGetPrevPath( lt->ltPrevPath, &(lt->ltPrevLevel),
							root, paraNr ) )
	{
	int i;

	for ( i= 0; i <= level; i++ )
	    { lt->ltPrevPath[i]= lt->ltHerePath[i];	}

	lt->ltPrevLevel= -1;
	}

    appGuiEnableWidget( lt->ltRemoveFromListButton, 1 );
    appGuiEnableWidget( is->isApplyButton, 1 );
    appGuiEnableWidget( is->isRevertButton, 1 );

    appGuiEnableWidget( lt->ltListLevelFrame, 1 );
    appGuiEnableWidget( lt->ltSetLevelButton, 1 );

    return 0;
    }

void tedFormatToolRefreshListTool(
				ListTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    ListOverride *			lo;
    DocumentList *			dl;
    ListNumberTreeNode *		root;

    int					i;

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{
	lt->ltPrevPath[i]= 0;
	lt->ltHerePath[i]= 0;
	}

    lt->ltPrevLevel= -1;
    lt->ltHereLevel= -1;

    if  ( ! sd->sdHasLists					||
	  sd->sdListOverride < 1				||
	  docGetListOfParagraph( &lo, &root, &dl,
				    sd->sdListOverride, bd )	)
	{ tedFormatToolNoList( lt, is );	}
    else{
	if  ( tedFormatToolSetList( lt, is, sd->sdFirstListParaNr,
			sd->sdListOverride, sd->sdListLevel, lo, dl, root ) )
	    { LDEB(sd->sdFirstListParaNr); return;	}
	}

    if  ( sd->sdListOverride > 0 && sd->sdMultiList )
	{ appGuiEnableWidget( lt->ltSetListButton, 1 );	}
    else{ appGuiEnableWidget( lt->ltSetListButton, 0 );	}

    /**/

    if  ( lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >= lt->ltListPropertiesChosen.dlLevelCount	)
	{
	if  ( lt->ltListPropertiesChosen.dlLevelCount > 0 )
	    { lt->ltCurrentLevel= 0;	}
	else{ lt->ltCurrentLevel= -1;	}
	}

    tedFormatToolRefreshListPage( lt );

    *pEnabled= ds->dsSelectionScope.ssInExternalItem == DOCinBODY;
*pEnabled= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Set List' button on the list tool has been pushed.		*/
/*  The 'Set Level' button on the list tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListSetListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;

    const ListOverride *	lo= &(lt->ltListOverrideChosen);

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );

    docInitParagraphProperties( &pp );

    pp.ppListOverride= lo->loIndex;

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListSetLevelPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    if  ( lt->ltCurrentLevel < 0 || lt->ltCurrentLevel >= dl->dlLevelCount )
	{ LLDEB(lt->ltCurrentLevel,dl->dlLevelCount); return;	}

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    pp.ppListLevel= lt->ltCurrentLevel;

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Remove from List' button on the list tool has been pushed.	*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListRemoveFromListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    if  ( tedAppChangeParagraphProperties( ea, &ppSetMask, &pp ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListNewListPushed, w, voidlt )
    {
LDEB(1); return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the list tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListRevertPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;

    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;

    if  ( docCopyDocumentList( &(lt->ltListPropertiesChosen),
			    &(lt->ltListPropertiesSet), fontMap, colorMap ) )
	{ LDEB(1); return;	}

    if  ( docCopyListOverride( &(lt->ltListOverrideChosen),
			    &(lt->ltListOverrideSet), fontMap, colorMap ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  A level was selected in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedListToolLevelChosen, w, voidlt, voidlcs )
    {
    ListTool *			lt= (ListTool *)voidlt;
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);

    int				level;

    level= appGuiGetPositionFromListCallback( w, voidlcs );

    /*  No: takes the role of 'Revert'
    if  ( level == lt->ltCurrentLevel )
	{ return;	}
    */

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount);	}

    lt->ltCurrentLevel= level;

    tedListToolRefreshCurrentLevel( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the current list in the document.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeList, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    EditApplication *		ea= lt->ltApplication;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);

    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				li;
    int				lipfi;
    int				changed;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    /**/
    lipfi= dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips;
    if  ( appGetLengthFromTextWidget( lt->ltFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    li= dll->dllLeftIndentTwips;
    if  ( appGetLengthFromTextWidget( lt->ltLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					1, 1, maxValue, adaptToMax ) )
	{ return;	}

    dll->dllLeftIndentTwips= li;
    dll->dllFirstIndentTwips= lipfi- li;

    /**/
    if  ( appGetIntegerFromTextWidget( lt->ltStartAtText,
				    &(dll->dllStartAt), 1, 0, INT_MAX, 0 ) )
	{ return;	}

    if  ( tedAppChangeCurrentList( ea, dl ) )
	{ LDEB(1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  A number style has been chosen in the menu in the list level	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedListNumberStyleChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				style;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    style= appGuiGetOptionmenuItemIndex( &(lt->ltNumberStyleOptionmenu), w );
    if  ( style < 0 || style >= LISTTOOLcountSTYLES )
	{ LLDEB(style,LISTTOOLcountSTYLES); return;	}

    dll->dllNumberStyle= TED_ListToolStyles[style];

    return;
    }

static APP_OITEM_CALLBACK_H( tedListJustifyChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				justify;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    justify= appGuiGetOptionmenuItemIndex( &(lt->ltJustifyOptionmenu), w );
    if  ( justify < 0 || justify >= DOClla_COUNT )
	{ LLDEB(justify,DOClla_COUNT); return;	}

    dll->dllJustification= justify;

    return;
    }

static APP_OITEM_CALLBACK_H( tedListFollowChosen, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				follow;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    follow= appGuiGetOptionmenuItemIndex( &(lt->ltFollowOptionmenu), w );
    if  ( follow < 0 || follow >= DOCllf_COUNT )
	{ LLDEB(follow,DOCllf_COUNT); return;	}

    dll->dllFollow= follow;

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedLevelStartAtChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    appGetIntegerFromTextWidget( lt->ltStartAtText,
				    &(dll->dllStartAt), 1, 0, INT_MAX, 0 );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelFirstIndentChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				lipfi;
    int				changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    lipfi=  dll->dllLeftIndentTwips+ dll->dllFirstIndentTwips;

    tedFormatValidateDimension( &lipfi, &changed,
					    lt->ltFirstIndentText, lipfi );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelLeftIndentChanged, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    DocumentList *		dl= &(lt->ltListPropertiesChosen);
    int				level= lt->ltCurrentLevel;
    DocumentListLevel *		dll;

    int				value;
    int				changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    dll= &(dl->dlLevels[level]);

    tedFormatValidateDimension( &value, &changed,
			    lt->ltLeftIndentText, dll->dllLeftIndentTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the tabs page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillListsPage(	ListTool *			lt,
				const ListsPageResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		levelLabel;
    APP_WIDGET		startLabel;
    APP_WIDGET		styleLabel;
    APP_WIDGET		justifyLabel;
    APP_WIDGET		followLabel;
    APP_WIDGET		firstLabel;
    APP_WIDGET		leftLabel;

    APP_WIDGET		row;

    const int		visibleItems= 9;

    const int		textColumns= 10;

    /**/
    lt->ltPageResources= lpr;

    /**/
    docInitDocumentList( &(lt->ltListPropertiesSet) );
    docInitDocumentList( &(lt->ltListPropertiesChosen) );

    docInitListOverride( &(lt->ltListOverrideSet) );
    docInitListOverride( &(lt->ltListOverrideChosen) );

    lt->ltCurrentLevel= -1;
    lt->ltCurrentParagraphNumber= -1;


    /**/
    appGuiMakeListInColumn( &(lt->ltListLevelList), pageWidget,
			    visibleItems, tedListToolLevelChosen, (void *)lt );

    /**/
    appMakeColumnFrameInColumn( &(lt->ltSelectionFrame),
				    &(lt->ltSelectionPaned),
				    pageWidget, lpr->lprSelection );

    appInspectorMakeButtonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetListButton), &(lt->ltRemoveFromListButton),
		    lpr->lprSetList, lpr->lprRemoveFromList,
		    tedFormatListSetListPushed,
		    tedFormatListRemoveFromListPushed, lt );

    appInspectorMakeButtonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetLevelButton), &(lt->ltNewListButton),
		    lpr->lprSetLevel, lpr->lprNewList,
		    tedFormatListSetLevelPushed,
		    tedFormatListNewListPushed, lt );

    /**/
    appMakeColumnFrameInColumn( &(lt->ltListLevelFrame),
				    &(lt->ltListLevelPaned),
				    pageWidget, lpr->lprListLevel );

    /**/
    appMakeLabelAndTextRow( &row, &levelLabel, &(lt->ltLevelText),
				    lt->ltListLevelPaned,
				    lpr->lprLevel, textColumns, 1 );

    appEnableText( lt->ltLevelText, 0 );

    /**/
    appMakeLabelAndTextRow( &row, &firstLabel, &(lt->ltFirstIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelFirstIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltFirstIndentText,
				    tedLevelFirstIndentChanged, (void *)lt );
    /**/
    appMakeLabelAndTextRow( &row, &leftLabel, &(lt->ltLeftIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltLeftIndentText,
				    tedLevelLeftIndentChanged, (void *)lt );

    /**/
    appMakeLabelAndTextRow( &row, &startLabel, &(lt->ltStartAtText),
				    lt->ltListLevelPaned,
				    lpr->lprStartAt, textColumns, 1 );

    appGuiSetGotValueCallbackForText( lt->ltStartAtText,
					tedLevelStartAtChanged, (void *)lt );
    /**/
    appInspectorMakeMenuRow( &row, &(lt->ltNumberStyleOptionmenu),
				    &styleLabel, lt->ltListLevelPaned,
				    lpr->lprStyle );

    appInspectorMakeMenuRow( &row, &(lt->ltJustifyOptionmenu),
				    &justifyLabel, lt->ltListLevelPaned,
				    lpr->lprJustify );

    appInspectorMakeMenuRow( &row, &(lt->ltFollowOptionmenu),
				    &followLabel, lt->ltListLevelPaned,
				    lpr->lprFollowedBy );

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatListRevertPushed, tedFormatChangeList, lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedFormatFillListChoosers(		ListTool *			lt )
    {
    const ListsPageResources *	lpr= lt->ltPageResources;

    appFillInspectorMenu( LISTTOOLcountSTYLES, 0,
			lt->ltNumberStyleItems, lpr->lprStyleOptionTexts,
			&(lt->ltNumberStyleOptionmenu),
			tedListNumberStyleChosen, (void *)lt );

    appFillInspectorMenu( DOClla_COUNT, DOCllaLEFT,
			lt->ltJustifyItems, lpr->lprJustifyOptionTexts,
			&(lt->ltJustifyOptionmenu),
			tedListJustifyChosen, (void *)lt );

    appFillInspectorMenu( DOCllf_COUNT, DOCllfTAB,
			lt->ltFollowItems, lpr->lprFollowOptionTexts,
			&(lt->ltFollowOptionmenu),
			tedListFollowChosen, (void *)lt );

    return;
    }

void tedFormatFinishListPage(		ListTool *			lt )
    {
    appOptionmenuRefreshWidth( &(lt->ltNumberStyleOptionmenu) );
    appOptionmenuRefreshWidth( &(lt->ltJustifyOptionmenu) );
    appOptionmenuRefreshWidth( &(lt->ltFollowOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a tabs tool.				*/
/*									*/
/************************************************************************/

void tedFormatCleanListsTool(	ListTool *			lt )
    {
    docCleanDocumentList( &(lt->ltListPropertiesSet) );
    docCleanDocumentList( &(lt->ltListPropertiesChosen) );
    
    docCleanListOverride( &(lt->ltListOverrideSet) );
    docCleanListOverride( &(lt->ltListOverrideChosen) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get list tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedListSubjectResourceTable[]=
    {
    APP_RESOURCE( "listToolParagraphNumber",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Number" ),
    APP_RESOURCE( "listToolChangeList",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Change List" ),
    APP_RESOURCE( "listToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    };

static AppConfigurableResource TED_TedListToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "listToolSelection",
	offsetof(ListsPageResources,lprSelection),
	"Selection" ),
    APP_RESOURCE( "listToolSetList",
	offsetof(ListsPageResources,lprSetList),
	"Set List" ),
    APP_RESOURCE( "listToolRemoveFromList",
	offsetof(ListsPageResources,lprRemoveFromList),
	"Remove from List" ),
    APP_RESOURCE( "listToolSetLevel",
	offsetof(ListsPageResources,lprSetLevel),
	"Set Level" ),
    APP_RESOURCE( "listToolNewList",
	offsetof(ListsPageResources,lprNewList),
	"New List" ),

    /**/
    APP_RESOURCE( "listToolListLevel",
	offsetof(ListsPageResources,lprListLevel),
	"List Level" ),
    APP_RESOURCE( "listToolLevelFirstIndent",
	offsetof(ListsPageResources,lprLevelFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "listToolLevelLeftIndent",
	offsetof(ListsPageResources,lprLevelLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "listToolLevel",
	offsetof(ListsPageResources,lprLevel),
	"Level" ),
    APP_RESOURCE( "listToolListLevelStartAt",
	offsetof(ListsPageResources,lprStartAt),
	"First Number" ),
    APP_RESOURCE( "listToolListLevelNoRestart",
	offsetof(ListsPageResources,lprNoRestart),
	"Continuous" ),

    /**/
    APP_RESOURCE( "listToolLevelNumberStyle",
	offsetof(ListsPageResources,lprStyle),
	"Style" ),
    /**/
    APP_RESOURCE( "listToolListLevelStyleNar",
	    offsetof(ListsPageResources,lprStyleOptionTexts[0]),
	    "1, 2, 3" ),
    APP_RESOURCE( "listToolListLevelStyleNruc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[1]),
	    "I, II, III" ),
    APP_RESOURCE( "listToolListLevelStyleNrlc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[2]),
	    "i, ii, iii" ),
    APP_RESOURCE( "listToolListLevelStyleNauc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[3]),
	    "A, B, C" ),
    APP_RESOURCE( "listToolListLevelStyleNalc",
	    offsetof(ListsPageResources,lprStyleOptionTexts[4]),
	    "a, b, c" ),
    APP_RESOURCE( "listToolListLevelStyleNar0",
	    offsetof(ListsPageResources,lprStyleOptionTexts[5]),
	    "01, 02, 10, 11" ),
    APP_RESOURCE( "listToolListLevelStyleBullet",
	    offsetof(ListsPageResources,lprStyleOptionTexts[6]),
	    "Bullet" ),
    APP_RESOURCE( "listToolListLevelStyleNone",
	    offsetof(ListsPageResources,lprStyleOptionTexts[7]),
	    "None" ),

    /**/
    APP_RESOURCE( "listToolLevelJustify",
	offsetof(ListsPageResources,lprJustify),
	"Justify" ),
    /**/
    APP_RESOURCE( "listToolListLevelJustifyLeft",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaLEFT]),
	    "Left" ),
    APP_RESOURCE( "listToolListLevelJustifyCentered",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaCENTERED]),
	    "Centered" ),
    APP_RESOURCE( "listToolListLevelJustifyRight",
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaRIGHT]),
	    "Right" ),

    /**/
    APP_RESOURCE( "listToolLevelFollowedBy",
	offsetof(ListsPageResources,lprFollowedBy),
	"Separator" ),
    /**/
    APP_RESOURCE( "listToolListLevelFollowTab",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfTAB]),
	    "Tab" ),
    APP_RESOURCE( "listToolListLevelFollowSpace",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfSPACE]),
	    "Space" ),
    APP_RESOURCE( "listToolListLevelFollowNone",
	    offsetof(ListsPageResources,lprFollowOptionTexts[DOCllfNONE]),
	    "None" ),
    };

void tedFormatToolGetListsResourceTable(EditApplication *		ea,
					ListsPageResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    appGuiGetResourceValues( ea, (void *)lpr,
				TED_TedListToolResourceTable,
				sizeof(TED_TedListToolResourceTable)/
				sizeof(AppConfigurableResource) );

    appGuiGetResourceValues( ea, (void *)isr,
				TED_TedListSubjectResourceTable,
				sizeof(TED_TedListSubjectResourceTable)/
				sizeof(AppConfigurableResource) );

    return;
    }
