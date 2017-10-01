/************************************************************************/
/*									*/
/*  The 'Tabs' page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<limits.h>

#   include	<geoString.h>
#   include	<geoUnit.h>

#   include	"tedTabsTool.h"
#   include	<tedAppFront.h>
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<appQuestion.h>
#   include	<docEditCommand.h>
#   include	<docNodeTree.h>
#   include	<docTabStop.h>
#   include	<docTabStopList.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docDocumentProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appInspector.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Tabs' page.					*/
/*									*/
/************************************************************************/

typedef struct TabsPageResources
    {
    const char *	tprDefaultTabStopsLabel;
    const char *	tprDefaultTabStopsEvery;
    const char *	tprDefaultTabStopsRevert;
    const char *	tprDefaultTabStopsApply;

    const char *	tprPositions;

    const char *	tprTabStop;

    const char *	tprPosition;

    const char *	tprAlignment;
    const char *	tprAlignmentOptionTexts[DOCta_COUNT];

    const char *	tprLeader;
    const char *	tprLeaderOptionTexts[DOCtl_COUNT];

    const char *	tprTooCloseMessage;
    } TabsPageResources;

/************************************************************************/
/*									*/
/*  A tabs tool, i.e. the 'Tabs' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TabsTool
    {
    struct EditApplication *	ttApplication;
    struct AppInspector *	ttInspector;
    const TabsPageResources *	ttPageResources;
    InspectorSubject *		ttInspectorSubject;

    int				ttInProgrammaticChange;

    int				ttUnitInt;
    unsigned char		ttCanChange;

    DocumentProperties		ttDocPropertiesSet;
    DocumentProperties		ttDocPropertiesChosen;

    TabStopList			ttTabStopsSet;
    TabStopList			ttTabStopsChosen;

    TabStop			ttTabStopValue;
    int				ttTabStopNumber;

    /**/
    APP_WIDGET			ttTabDefaultFrame;
    APP_WIDGET			ttTabDefaultPaned;
    APP_WIDGET			ttTabDefaultText;
    APP_WIDGET			ttTabDefaultRevertButton;
    APP_WIDGET			ttTabDefaultApplyButton;

    /**/
    APP_WIDGET			ttTabPositionList;

    APP_WIDGET			ttTabPropertyFrame;
    APP_WIDGET			ttTabPropertyPaned;

    APP_WIDGET			ttTabPositionText;

    APP_WIDGET			ttDeleteTabButton;
    APP_WIDGET			ttInsertTabButton;

    AppOptionmenu		ttAlignmentOptionmenu;

    AppOptionmenu		ttLeaderOptionmenu;
    } TabsTool;

static TabsPageResources		TED_TabsPageResources;
static InspectorSubjectResources	TED_TabsSubjectResources;

static void tedInitTabsTool(	TabsTool *	tt )
    {
    memset( tt, 0, sizeof(TabsTool) );
    }

/************************************************************************/
/*									*/
/*  Refresh the controls with the current tab stop.			*/
/*									*/
/************************************************************************/

static void tedTabToolRefreshCurrentTab(	TabsTool *	tt )
    {
    const TabStop *		ts= &(tt->ttTabStopValue);

    if  ( ts->tsOffset >= 0 )
	{
	guiLengthToText( tt->ttTabPositionText,
					    ts->tsOffset, UNITtyPOINTS );
	}
    else{
	guiStringToText( tt->ttTabPositionText, "" );
	}

    guiSetOptionmenu( &(tt->ttAlignmentOptionmenu), ts->tsAlignment );

    guiSetOptionmenu( &(tt->ttLeaderOptionmenu), ts->tsLeader );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a certain tab in the list.					*/
/*									*/
/************************************************************************/

static void tedTabsToolSelectTab(	TabsTool *	tt,
					int		n )
    {
    const TabStopList *		tsl= &(tt->ttTabStopsChosen);

    if  ( n >= 0 && n >= tsl->tslTabStopCount )
	{ LLDEB(n,tsl->tslTabStopCount); n= -1;	}

    if  ( n >= 0 )
	{
	tt->ttTabStopValue= tsl->tslTabStops[n];
	tt->ttTabStopNumber= n;

	guiSelectPositionInListWidget( tt->ttTabPositionList, n );
	}
    else{
	docInitTabStop( &(tt->ttTabStopValue) );
	tt->ttTabStopValue.tsOffset= -1;
	tt->ttTabStopNumber= -1;

	guiRemoveSelectionFromListWidget( tt->ttTabPositionList );
	}

    tedTabToolRefreshCurrentTab( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the list of tabs and the currently selected tab.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTabsPage(	TabsTool *	tt )
    {
    const TabStopList *		tsl= &(tt->ttTabStopsChosen);

    int				below= -1;
    int				above= -1;
    int				da= INT_MAX;
    int				db= INT_MAX;

    char			scratch[50];

    const TabStop *		ts;
    int				i;

    InspectorSubject *		is= tt->ttInspectorSubject;

    tt->ttInProgrammaticChange++;

    guiEmptyListWidget( tt->ttTabPositionList );

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	geoLengthToString( scratch, ts->tsOffset, UNITtyPOINTS );
	guiAddValueToListWidget( tt->ttTabPositionList, -1, scratch );

	if  ( ts->tsOffset <= tt->ttTabStopValue.tsOffset )
	    {
	    db= tt->ttTabStopValue.tsOffset- ts->tsOffset;
	    below= i;
	    }
	if  ( above < 0 && ts->tsOffset >= tt->ttTabStopValue.tsOffset )
	    {
	    da= ts->tsOffset- tt->ttTabStopValue.tsOffset;
	    above= i;
	    }
	}

    if  ( tsl->tslTabStopCount == 0 )
	{ tedTabsToolSelectTab( tt, -1 );		}
    else{
	if  ( da < db )
	    { tedTabsToolSelectTab( tt, above );	}
	else{ tedTabsToolSelectTab( tt, below );	}
	}

    guiEnableWidget( tt->ttInsertTabButton, tt->ttCanChange );

    guiEnableWidget( is->isApplyButton,
			    tt->ttCanChange && tt->ttTabStopNumber >= 0 );
    guiEnableWidget( tt->ttDeleteTabButton,
			    tt->ttCanChange && tt->ttTabStopNumber >= 0 );

    tt->ttInProgrammaticChange--;

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the tabs tool to the current selection in the document.	*/
/*									*/
/************************************************************************/

static void tedRefreshTabsTool(	void *				vtt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    TabsTool *				tt= (TabsTool *)vtt;
    PropertyMask			chgMask;
    PropertyMask			updMask;

    const TabStopList *			tsl;

    struct BufferItem *			paraNode= ds->dsHead.dpNode;

    const struct DocumentAttributeMap * const	dam0=
					(const struct DocumentAttributeMap *)0;

    tsl= docParaNodeGetTabStopList( paraNode, bd );

    utilPropMaskClear( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    utilPropMaskClear( &chgMask );

    if  ( docCopyTabStopList( &(tt->ttTabStopsChosen), tsl ) )
	{ LDEB(1); return ;	}
    if  ( docCopyTabStopList( &(tt->ttTabStopsSet), tsl ) )
	{ LDEB(1); return ;	}

    /**/

    tt->ttCanChange= cmdEnabled[EDITcmdUPD_PARA_PROPS];

    guiEnableWidget( tt->ttTabPropertyFrame, tt->ttCanChange );
    guiEnableText( tt->ttTabPositionText, tt->ttCanChange );

    tedFormatToolRefreshTabsPage( tt );

    guiEnableWidget( is->isPrevButton,
			    sd->sdInContiguousParagraphs		&&
			    docPrevParagraph( paraNode ) != (struct BufferItem *)0 );
    guiEnableWidget( is->isNextButton,
			    sd->sdInContiguousParagraphs		&&
			    docNextParagraph( paraNode ) != (struct BufferItem *)0 );

    guiEnableWidget( is->isRevertButton, tt->ttCanChange );

    /**/

    utilPropMaskClear( &updMask );
    PROPmaskADD( &updMask, DPpropDEFTAB );

    utilPropMaskClear( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesChosen),
					&updMask, bd->bdProperties, dam0 ) )
	{ LDEB(1); return ;	}

    utilPropMaskClear( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesSet),
					&updMask, bd->bdProperties, dam0 ) )
	{ LDEB(1); return ;	}

    /**/

    tt->ttInProgrammaticChange++;

    guiLengthToText( tt->ttTabDefaultText,
		tt->ttDocPropertiesChosen.dpTabIntervalTwips, UNITtyPOINTS );

    guiEnableWidget( tt->ttTabDefaultFrame, cmdEnabled[EDITcmdUPD_DOC_PROPS] );
    guiEnableText( tt->ttTabDefaultText, cmdEnabled[EDITcmdUPD_DOC_PROPS] );

    tt->ttInProgrammaticChange--;

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the tabs tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatTabsRevertParaPushed, w, voidtt )
    {
    TabsTool *		tt= (TabsTool *)voidtt;

    if  ( docCopyTabStopList( &(tt->ttTabStopsChosen),
						    &(tt->ttTabStopsSet) ) )
	{ LDEB(1);	}

    tedFormatToolRefreshTabsPage( tt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatTabsRevertDocPushed, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;

    PropertyMask		chgMask;
    PropertyMask		updMask;

    const struct DocumentAttributeMap * const	dam0=
				(const struct DocumentAttributeMap *)0;

    utilPropMaskClear( &chgMask );

    utilPropMaskClear( &updMask );
    PROPmaskADD( &updMask, DPpropDEFTAB );

    docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesChosen),
				&updMask, &(tt->ttDocPropertiesSet), dam0 );

    {
    const DocumentProperties *	dp= &(tt->ttDocPropertiesChosen);

    guiLengthToText( tt->ttTabDefaultText,
				    dp->dpTabIntervalTwips, UNITtyPOINTS );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  Check whether a tab velue is too close to an existing tab value for	*/
/*  the current paragraph.						*/
/*									*/
/************************************************************************/

static int tedTabToolValueTooCloseToExisting(
				    const TabStopList *	tsl,
				    int			current,
				    int			value )
    {
    int				i;
    TabStop *			ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	if  ( i == current )
	    { continue;	}

	if  ( value >= ts->tsOffset- 200		&&
	      value <= ts->tsOffset+ 200		)
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTabPositionChanged, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    const TabStopList *		tsl= &(tt->ttTabStopsChosen);

    int				value;
    int				changed;

    if  ( tt->ttTabStopNumber < 0			||
	  tt->ttTabStopNumber >= tsl->tslTabStopCount	)
	{ value= 0;	}
    else{
	TabStop *	ts= tsl->tslTabStops+ tt->ttTabStopNumber;

	value= ts->tsOffset;
	}

    changed= 0;

    tedFormatValidateDimension( &value, &changed,
					    tt->ttTabPositionText, value );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedTabDeftabChanged, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    DocumentProperties *	dp= &(tt->ttDocPropertiesChosen);

    int				value= dp->dpTabIntervalTwips;
    int				changed= 0;

    tedFormatValidateDimension( &value, &changed, tt->ttTabDefaultText, value );

    return;
    }

/************************************************************************/
/*									*/
/*  A tab was selected in the list of positions.			*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedTabsToolTabChosen, w, voidtt, voidlcs )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    const TabStopList *		tsl= &(tt->ttTabStopsChosen);

    int				position;

    if  ( tt->ttInProgrammaticChange )
	{ return;	}

    position= guiGetPositionFromListCallback( w, voidlcs );

    /*  No: takes the role of 'Revert'
    if  ( position == tt->ttTabStopNumber )
	{ return;	}
    */

    if  ( position < 0 || position >= tsl->tslTabStopCount )
	{ LLDEB(position,tsl->tslTabStopCount); return;	}

    tt->ttTabStopValue= tsl->tslTabStops[position];
    tt->ttTabStopNumber= position;

    tedTabToolRefreshCurrentTab( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;

    tedAppSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;

    tedAppSelectWholeParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Add a tab to the list.						*/
/*									*/
/*  1)  Retrieve the new position from the text widget.			*/
/*  2)  Value not too close to an existing tab stop?			*/
/*  3)  Insert in list.							*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTabToolAddTab, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;
    TabStopList *		tsl= &(tt->ttTabStopsChosen);

    const TabsPageResources *	tpr= tt->ttPageResources;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;

    /*  1  */
    value= tt->ttTabStopValue.tsOffset; changed= 0;

    if  ( guiLengthFromText( tt->ttTabPositionText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    /*  2  */
    if  ( tedTabToolValueTooCloseToExisting( tsl, -1, value ) )
	{
	appQuestionRunErrorDialog( tt->ttApplication,
					tt->ttInspector->aiTopWidget, w,
					tpr->tprTooCloseMessage );
	return;
	}

    if  ( changed )
	{ tt->ttTabStopValue.tsOffset= value;	}

    /*  3  */
    changed= docAddTabToList( tsl, &(tt->ttTabStopValue) );
    if  ( changed < 0 )
	{ LDEB(changed); return;	}

    tedAppSetParagraphTabs( ea, tsl );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the tab that is currently selected in the list.		*/
/*  and update the selected paragraphs in the document accordingly.	*/
/*									*/
/*  2)  Retrieve the new position from the text widget.			*/
/*  3)  Value not too close to another tab stop?			*/
/*  4)  Delete the current one.						*/
/*  5)  Insert the new value in the correct position.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeParaTabs, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;
    TabStopList *		tsl= &(tt->ttTabStopsChosen);

    const TabsPageResources *	tpr= tt->ttPageResources;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;
    int				res;

    /*  2  */
    value= tt->ttTabStopValue.tsOffset; changed= 0;

    if  ( guiLengthFromText( tt->ttTabPositionText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    /*  3  */
    if  ( tedTabToolValueTooCloseToExisting( tsl, tt->ttTabStopNumber, value ) )
	{
	appQuestionRunErrorDialog( tt->ttApplication,
					tt->ttInspector->aiTopWidget,
					w, tpr->tprTooCloseMessage );
	return;
	}

    if  ( changed )
	{ tt->ttTabStopValue.tsOffset= value;	}

    /*  4  */
    if  ( tt->ttTabStopNumber >= 0			&&
	  tt->ttTabStopNumber < tsl->tslTabStopCount	)
	{ docDeleteTabFromList( tsl, tt->ttTabStopNumber );	}

    /*  5  */
    res= docAddTabToList( tsl, &(tt->ttTabStopValue) );
    if  ( res < 0 )
	{ LDEB(res); return;	}

    tedAppSetParagraphTabs( ea, tsl );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete the tab that is currently selected in the list.		*/
/*									*/
/*  1)  Has a tab been selected?					*/
/*  2)  Delete it.							*/
/*  3)  Refresh list.							*/
/*  4)  If a tab remains select the one after it, or if it was the last	*/
/*	one, select the previous one.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTabToolDeleteTab, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;
    TabStopList *		tsl= &(tt->ttTabStopsChosen);

    /*  1  */
    if  ( tt->ttTabStopNumber < 0		||
	  tt->ttTabStopNumber >= tsl->tslTabStopCount	)
	{ LLDEB(tt->ttTabStopNumber,tsl->tslTabStopCount); return;	}

    /*  2  */
    docDeleteTabFromList( tsl, tt->ttTabStopNumber );

    if  ( tt->ttTabStopNumber < tsl->tslTabStopCount )
	{ tt->ttTabStopValue= tsl->tslTabStops[tt->ttTabStopNumber];	}

    tedAppSetParagraphTabs( ea, tsl );

    return;
    }

/************************************************************************/
/*									*/
/*  2)  The 'Change' default tab distance for the document has been	*/
/*	pushed.								*/
/*									*/
/************************************************************************/

/*  2  */
static APP_BUTTON_CALLBACK_H( tedFormatTabsApplyDocPushed, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    struct EditApplication *	ea= tt->ttApplication;
    DocumentProperties *	dpSet= &(tt->ttDocPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;

    value= dpSet->dpTabIntervalTwips;

    if  ( guiLengthFromText( tt->ttTabDefaultText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    if  ( changed )
	{
	PropertyMask		dpSetMask;

	utilPropMaskClear( &dpSetMask );
	PROPmaskADD( &dpSetMask, DPpropDEFTAB );

	dpSet->dpTabIntervalTwips= value;

	tedAppSetDocumentProperties( ea, &dpSetMask, dpSet );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  An alignment has been chosen in the alignment menu in the tab	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static void tedTabAlignmentChosen(	int		alignment,
					void *		voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    TabStop *			ts= &(tt->ttTabStopValue);

    if  ( alignment < 0 || alignment >= DOCta_COUNT )
	{ LLDEB(alignment,DOCta_COUNT); return;	}

    ts->tsAlignment= alignment;

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of leader has been chosen in the leader menu in the tab	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static void tedTabLeaderChosen(		int		leader,
					void *		voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    TabStop *			ts= &(tt->ttTabStopValue);

    if  ( leader < 0 || leader >= DOCtl_COUNT )
	{ LLDEB(leader,DOCtl_COUNT); return;	}

    ts->tsLeader= leader;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the tabs page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildTabsPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr )
    {
    const TabsPageResources *	tpr= &TED_TabsPageResources;

    APP_WIDGET		everyLabel;
    APP_WIDGET		posLabel;
    APP_WIDGET		alignmentLabel;
    APP_WIDGET		leaderLabel;

    APP_WIDGET		row;

    const int		visibleItems= 5;

    const int		textColumns= 10;

    TabsTool *		tt= malloc( sizeof(TabsTool) );

    if  ( ! tt )
	{ XDEB(tt); return tt;	}

    tedInitTabsTool( tt );

    /**/
    tt->ttPageResources= tpr;
    tt->ttInspector= ai;
    tt->ttApplication= ea;
    tt->ttInspectorSubject= is;

    /**/
    docInitDocumentProperties( &(tt->ttDocPropertiesSet) );
    docInitDocumentProperties( &(tt->ttDocPropertiesChosen) );

    docInitTabStopList( &(tt->ttTabStopsSet) );
    docInitTabStopList( &(tt->ttTabStopsChosen) );

    docInitTabStop( &(tt->ttTabStopValue) );
    tt->ttTabStopValue.tsOffset= -1;
    tt->ttTabStopNumber= -1;

    tt->ttInProgrammaticChange= 0;

    /**/
    guiMakeColumnFrameInColumn( &(tt->ttTabDefaultFrame),
			    &(tt->ttTabDefaultPaned),
			    pageWidget, tpr->tprDefaultTabStopsLabel );

    guiToolMakeLabelAndTextRow( &row, &everyLabel, &(tt->ttTabDefaultText),
			    tt->ttTabDefaultPaned,
			    tpr->tprDefaultTabStopsEvery, textColumns, 1 );

    guiSetGotValueCallbackForText( tt->ttTabDefaultText,
				    tedTabDeftabChanged, (void *)tt );

    guiToolMake2BottonRow( &row, tt->ttTabDefaultPaned,
					&(tt->ttTabDefaultRevertButton),
					&(tt->ttTabDefaultApplyButton),
					tpr->tprDefaultTabStopsRevert,
					tpr->tprDefaultTabStopsApply,
					tedFormatTabsRevertDocPushed,
					tedFormatTabsApplyDocPushed, tt );
    /**/
    guiMakeListInColumn( &(tt->ttTabPositionList),
		pageWidget, visibleItems,
		tedTabsToolTabChosen, (APP_BUTTON_CALLBACK_T)0, (void *)tt );

    /**/
    guiMakeColumnFrameInColumn( &(tt->ttTabPropertyFrame),
				    &(tt->ttTabPropertyPaned),
				    pageWidget, tpr->tprTabStop );

    /**/
    guiToolMakeLabelAndTextRow( &row, &posLabel, &(tt->ttTabPositionText),
				    tt->ttTabPropertyPaned,
				    tpr->tprPosition, textColumns, 1 );

    guiSetGotValueCallbackForText( tt->ttTabPositionText,
				    tedTabPositionChanged, (void *)tt );
    /**/
    guiToolMakeLabelAndMenuRow( &row, &(tt->ttAlignmentOptionmenu),
			&alignmentLabel, tt->ttTabPropertyPaned,
			tpr->tprAlignment, tedTabAlignmentChosen, (void *)tt );

    /**/
    guiToolMakeLabelAndMenuRow( &row, &(tt->ttLeaderOptionmenu),
			&leaderLabel, tt->ttTabPropertyPaned,
			tpr->tprLeader, tedTabLeaderChosen, (void *)tt );

    guiToolMake2BottonRow( &row, pageWidget,
		&(tt->ttDeleteTabButton), &(tt->ttInsertTabButton),
		isr->isrDeleteButtonText, isr->isrInsertButtonText,
		tedTabToolDeleteTab, tedTabToolAddTab, tt );

    return (void *)tt;
    }

/************************************************************************/
/*									*/
/*  Finish the tabs page.						*/
/*									*/
/************************************************************************/

static void tedTabsToolFillChoosers(	void *		vtt )
    {
    TabsTool *			tt= (TabsTool *)vtt;
    const TabsPageResources *	tpr= tt->ttPageResources;

    appFillInspectorMenu( DOCta_COUNT, DOCtaLEFT,
		tpr->tprAlignmentOptionTexts, &(tt->ttAlignmentOptionmenu) );

    appFillInspectorMenu( DOCtl_COUNT, DOCtlNONE,
		tpr->tprLeaderOptionTexts, &(tt->ttLeaderOptionmenu) );

    guiOptionMenuEnablePosition( &(tt->ttLeaderOptionmenu), DOCtlTHICK, 0 );
    guiOptionMenuEnablePosition( &(tt->ttLeaderOptionmenu), DOCtlEQUAL, 0 );

    return;
    }

static void tedFormatFinishTabsPage(	void *	vtt )
    {
    TabsTool *			tt= (TabsTool *)vtt;

    guiOptionmenuRefreshWidth( &(tt->ttAlignmentOptionmenu) );
    guiOptionmenuRefreshWidth( &(tt->ttLeaderOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a tabs tool.				*/
/*									*/
/************************************************************************/

static void tedFormatFreeParaTabsTool(	void *	vtt )
    {
    TabsTool *			tt= (TabsTool *)vtt;

    docCleanDocumentProperties( &(tt->ttDocPropertiesSet) );
    docCleanDocumentProperties( &(tt->ttDocPropertiesChosen) );

    docCleanTabStopList( &(tt->ttTabStopsSet) );
    docCleanTabStopList( &(tt->ttTabStopsChosen) );

    free( tt );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get tabs tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTabsSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolTabs",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Tab Stops" ),
    APP_RESOURCE( "formatToolChangeTabs",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Change Tab" ),
    APP_RESOURCE( "tableToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolTabsNextPara",
	offsetof(InspectorSubjectResources,isrNextButtonText),
	"Next Paragraph" ),
    APP_RESOURCE( "formatToolTabsPreviousPara",
	offsetof(InspectorSubjectResources,isrPrevButtonText),
	"Previous Paragraph" ),

    APP_RESOURCE( "formatToolTabsAddTab",
	offsetof(InspectorSubjectResources,isrInsertButtonText),
	"Add Tab" ),
    APP_RESOURCE( "formatToolTabsDeleteTab",
	offsetof(InspectorSubjectResources,isrDeleteButtonText),
	"Delete Tab" ),
    };

static AppConfigurableResource TED_TedTabsToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolDefaultTabStopsLabel",
	offsetof(TabsPageResources,tprDefaultTabStopsLabel),
	"Default Tab Stops" ),
    APP_RESOURCE( "formatToolDefaultTabStopsEvery",
	offsetof(TabsPageResources,tprDefaultTabStopsEvery),
	"Every" ),
    APP_RESOURCE( "formatToolDefaultTabStopsRevert",
	offsetof(TabsPageResources,tprDefaultTabStopsRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolDefaultTabStopsApply",
	offsetof(TabsPageResources,tprDefaultTabStopsApply),
	"Apply to Document" ),

    /**/
    APP_RESOURCE( "formatToolTabPositions",
	offsetof(TabsPageResources,tprPositions),
	"Positions" ),

    APP_RESOURCE( "formatToolTabStop",
	offsetof(TabsPageResources,tprTabStop),
	"Tab Stop" ),

    APP_RESOURCE( "formatToolTabPosition",
	offsetof(TabsPageResources,tprPosition),
	"Position" ),

    APP_RESOURCE( "formatToolTabAlignment",
	offsetof(TabsPageResources,tprAlignment),
	"Alignment" ),
    /**/
    APP_RESOURCE( "formatToolTabAlignmentLeft",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaLEFT]),
	"Left" ),
    APP_RESOURCE( "formatToolTabAlignmentRight",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaRIGHT]),
	"Right" ),
    APP_RESOURCE( "formatToolTabAlignmentCenter",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaCENTER]),
	"Center" ),
    APP_RESOURCE( "formatToolTabAlignmentDecimal",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaDECIMAL]),
	"Decimal" ),

    /**/
    APP_RESOURCE( "formatToolTabLeader",
	offsetof(TabsPageResources,tprLeader),
	"Leader" ),
    APP_RESOURCE( "formatToolTabLeaderNone",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlNONE]),
	"None" ),
    APP_RESOURCE( "formatToolTabLeaderDots",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlDOTS]),
	"Dots" ),
    APP_RESOURCE( "formatToolTabLeaderUnderline",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlUNDERLINE]),
	"Line" ),
    APP_RESOURCE( "formatToolTabLeaderHyphens",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlHYPH]),
	"Dashes" ),

    APP_RESOURCE( "formatToolTabLeaderThick",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlTHICK]),
	"Thick Line" ),
    APP_RESOURCE( "formatToolTabLeaderEqual",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlEQUAL]),
	"Double Line" ),

    /**/
    APP_RESOURCE( "formatToolTabTooCloseToOther",
	offsetof(TabsPageResources,tprTooCloseMessage),
	"Position is too close to that of an other tab" ),
    };

static void tedFormatToolGetTabsResourceTable(
					struct EditApplication *	ea,
					TabsPageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTabsToolResourceTable,
				sizeof(TED_TedTabsToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTabsSubjectResourceTable,
				sizeof(TED_TedTabsSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedTabsToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetTabsResourceTable( ea,
				&TED_TabsPageResources,
				&TED_TabsSubjectResources );

    ist->istResources= &TED_TabsSubjectResources;

    ist->istBuildTool= tedFormatBuildTabsPage;
    ist->istFillChoosers= tedTabsToolFillChoosers;
    ist->istFinishPage= tedFormatFinishTabsPage;
    ist->istFreeTool= tedFormatFreeParaTabsTool;

    ftst->ftstRefreshPage= tedRefreshTabsTool;
    /*fts->ftsToolResetCache*/

    ist->istMoveToNext= tedFormatNextPara;
    ist->istMoveToPrev= tedFormatPrevPara;

    ist->istRevertChanges= tedFormatTabsRevertParaPushed;
    ist->istApplyChanges= tedFormatChangeParaTabs;

    return 0;
    }
