/************************************************************************/
/*									*/
/*  A tool to manage the style of notes in the document.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  What property values are relevant for footnotes/endnotes?		*/
/*									*/
/************************************************************************/

static const int TED_FootnoteRestarts[]=
    {
    DPftnRST_CONTINUOUS,
    DPftnRST_PER_SECTION,
    DPftnRST_PER_PAGE,
    };

static const int TED_FootnotePositions[]=
    {
    DPftnPOS_BELOW_TEXT,
    DPftnPOS_PAGE_BOTTOM,
    };

static const int TED_EndnoteRestarts[]=
    {
    DPftnRST_CONTINUOUS,
    DPftnRST_PER_SECTION,
    };

static const int TED_EndnotePositions[]=
    {
    DPftnPOS_SECT_END,
    DPftnPOS_DOC_END,
    };

/************************************************************************/
/*									*/
/*  Refresh the 'Notes' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedSetNotesMenu(	AppOptionmenu *		aom,
				int			val,
				APP_WIDGET *		options )
    {
    int		n= 0;
    int		i;

    for ( i= 0; i < val; i++ )
	{
	if  ( options[i] )
	    { n++;	}
	}

    appSetOptionmenu( aom, n );

    return;
    }

static void tedNotesRefreshNotesStartNumber(	NotePropertiesTool *	npt,
						const NotesProperties *	np )
    {
    if  ( np->npRestart == DPftnRST_CONTINUOUS )
	{
	appIntegerToTextWidget( npt->nptStartNumberText, np->npStartNumber );
	appEnableText( npt->nptStartNumberText, 1 );
	}
    else{
	appStringToTextWidget( npt->nptStartNumberText, "" );
	appEnableText( npt->nptStartNumberText, 0 );
	}

    return;
    }

static void tedNotesRefreshNotePropertiesTool(	NotePropertiesTool *	npt,
						const NotesProperties *	np )
    {
    tedNotesRefreshNotesStartNumber( npt, np );

    tedSetNotesMenu( &(npt->nptPositionOptionmenu),
					    np->npPosition,
					    npt->nptPositionOptions );
    tedSetNotesMenu( &(npt->nptRestartOptionmenu),
					    np->npRestart,
					    npt->nptRestartOptions );
    appSetOptionmenu( &(npt->nptStyleOptionmenu),
					    np->npNumberStyle );

    return;
    }

static void tedNotesToolRefreshDocWidgets(	NotesTool *	nt )
    {
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    tedNotesRefreshNotePropertiesTool( &(nt->ntFootnotePropertiesTool),
						&(dp->dpFootnoteProperties) );

    tedNotesRefreshNotePropertiesTool( &(nt->ntEndnotePropertiesTool),
						&(dp->dpEndnoteProperties) );

    return;
    }

static void tedNotesToolRefreshNoteWidgets(	NotesTool *	nt )
    {
    int		enabled= 0;

    if  ( nt->ntNoteKindChosen == DOCinFOOTNOTE )
	{ enabled= 1;	}
    if  ( nt->ntNoteKindChosen == DOCinENDNOTE )
	{ enabled= 1;	}

    appGuiEnableWidget( nt->ntCurrentNoteFrame, enabled );

    appGuiEnableWidget( nt->ntToNoteRefButton, nt->ntInsideNote );
    appGuiEnableWidget( nt->ntToNoteButton, ! nt->ntInsideNote );

    appGuiSetToggleState( nt->ntFootnoteToggle,
				nt->ntNoteKindChosen == DOCinFOOTNOTE );

    appGuiSetToggleState( nt->ntEndnoteToggle,
				nt->ntNoteKindChosen == DOCinENDNOTE );

    return;
    }

void tedFormatToolRefreshNotesTool(
				NotesTool *			nt,
				int *				pEnabled,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				BufferDocument *		bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    PropertyMask		chgMask;
    PropertyMask		updMask;

    int				enabled= 0;
    int				noteIndex= -1;

    nt->ntInsideNote= 0;
    nt->ntNoteKindSet= DOCinBODY;

    if  ( ds )
	{
	DocumentNote *		dn;

	noteIndex= docGetSelectedNote( &dn, bd, ds );
	if  ( noteIndex >= 0 )
	    {
	    nt->ntNoteKindSet= dn->dnExternalItemKind;

	    if  ( ds->dsSelectionScope.ssInExternalItem == DOCinFOOTNOTE  ||
		  ds->dsSelectionScope.ssInExternalItem == DOCinENDNOTE   )
		{ nt->ntInsideNote= 1;	}
	    }
	}

    nt->ntNoteKindChosen=  nt->ntNoteKindSet;

    PROPmaskCLEAR( &updMask );

    PROPmaskADD( &updMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &updMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropENDNOTE_STARTNR );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesChosen),
							&updMask, dp ) )
	{ LDEB(1); return;	}
    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesSet),
							&updMask, dp ) )
	{ LDEB(1); return;	}

    tedNotesToolRefreshDocWidgets( nt );
    tedNotesToolRefreshNoteWidgets( nt );

    if  ( ds->dsSelectionScope.ssInExternalItem == DOCinBODY		||
	  noteIndex >= 0						)
	{ enabled= 1;	}

    *pEnabled= enabled;
    return;
    }

/************************************************************************/
/*									*/
/*  Jump back and forth between a note and its reference.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedNotesToolToNoteRef, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    tedAppGotoNoteRef( nt->ntApplication );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolToNote, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    tedAppEditNote( nt->ntApplication );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the document properties have been pushed.	*/
/*									*/
/************************************************************************/

static int tedNotesToolGetStartNumber(	NotePropertiesTool *	npt,
					NotesProperties *	np )
    {
    if  ( np->npRestart == DPftnRST_CONTINUOUS )
	{
	if  ( appGetIntegerFromTextWidget( npt->nptStartNumberText,
				    &(np->npStartNumber), 1, 0, INT_MAX, 0 ) )
	    { return -1;	}
	}

    return 0;
    }

static APP_BUTTON_CALLBACK_H( tedChangeNotesPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    PropertyMask		updMask;

    PROPmaskCLEAR( &updMask );

    PROPmaskADD( &updMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &updMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropENDNOTE_STARTNR );

    tedNotesToolGetStartNumber( &(nt->ntFootnotePropertiesTool),
						&(dp->dpFootnoteProperties) );

    tedNotesToolGetStartNumber( &(nt->ntEndnotePropertiesTool),
						&(dp->dpEndnoteProperties) );

    tedSetDocumentProperties( nt->ntApplication, dp, &updMask );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedRevertNotesPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    PropertyMask	chgMask;
    PropertyMask	updMask;

    PROPmaskCLEAR( &updMask );

    PROPmaskADD( &updMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &updMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropENDNOTE_STARTNR );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesChosen),
					&updMask, &(nt->ntPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedNotesToolRefreshDocWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the current note have been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedChangeNotePushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    tedChangeCurrentNote( nt->ntApplication, nt->ntNoteKindChosen );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedRevertNotePushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNoteKindChosen= nt->ntNoteKindSet;

    tedNotesToolRefreshNoteWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the toggles to decide whether the current note is a footnote	*/
/*  or endnote has been toggled.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFootnoteToggled, w, voidnt, voidtbcs )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{ nt->ntNoteKindChosen= DOCinFOOTNOTE;	}

    tedNotesToolRefreshNoteWidgets( nt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedEndnoteToggled, w, voidnt, voidtbcs )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{ nt->ntNoteKindChosen= DOCinENDNOTE;	}

    tedNotesToolRefreshNoteWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Notes' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedNotesMakeNotePropertiesTool(
				APP_WIDGET			pageWidget,
				const char *			title,
				NotePropertiesTool *		npt,
				const NotesPageResources *	npr )
    {
    APP_WIDGET	rowLabel;
    APP_WIDGET	row= (APP_WIDGET )0;
    const int	textColumns= 6;


    appMakeColumnFrameInColumn( &(npt->nptFrame),
				    &(npt->nptPaned), pageWidget, title );

    appInspectorMakeMenuRow( &row, &(npt->nptPositionOptionmenu),
			    &rowLabel, npt->nptPaned, npr->nprPositionText );

    appInspectorMakeMenuRow( &row, &(npt->nptRestartOptionmenu),
			    &rowLabel, npt->nptPaned, npr->nprRestartText );

    appInspectorMakeMenuRow( &row, &(npt->nptStyleOptionmenu),
				&rowLabel, npt->nptPaned, npr->nprStyleText );

    appMakeLabelAndTextRow( &row, &rowLabel, &(npt->nptStartNumberText),
			npt->nptPaned, npr->nprFirstNumberText,
			textColumns, 1 );

    return;
    }

void tedFormatFillNotesPage(	NotesTool *			nt,
				const NotesPageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    /**/
    nt->ntPageResources= npr;

    /**/
    docInitDocumentProperties( &(nt->ntPropertiesSet) );
    docInitDocumentProperties( &(nt->ntPropertiesChosen) );

    /**/
    appMakeColumnFrameInColumn( &(nt->ntCurrentNoteFrame),
			    &(nt->ntCurrentNotePaned),
			    pageWidget, npr->nprCurrentNoteText );

    appInspectorMakeToggleRow( &row, nt->ntCurrentNotePaned,
		    &(nt->ntFootnoteToggle), &(nt->ntEndnoteToggle),
		    npr->nprFootnoteText, npr->nprEndnoteText,
		    tedFootnoteToggled,
		    tedEndnoteToggled, (void *)nt );

    appInspectorMakeButtonRow( &row, nt->ntCurrentNotePaned,
		&(nt->ntToNoteRefButton), &(nt->ntToNoteButton),
		npr->nprToNoteRefText, npr->nprToNoteText,
		tedNotesToolToNoteRef, tedNotesToolToNote,
		(void *)nt );

    appInspectorMakeButtonRow( &row, nt->ntCurrentNotePaned,
		&(nt->ntRevertNoteButton), &(nt->ntChangeNoteButton),
		npr->nprRevertNoteText, npr->nprChangeNoteText,
		tedRevertNotePushed, tedChangeNotePushed,
		(void *)nt );

    /**************/

    tedNotesMakeNotePropertiesTool( pageWidget, npr->nprFootnotesText,
					&(nt->ntFootnotePropertiesTool), npr );
    tedNotesMakeNotePropertiesTool( pageWidget, npr->nprEndnotesText,
					&(nt->ntEndnotePropertiesTool), npr );

    /**************/

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedRevertNotesPushed, tedChangeNotesPushed,
		(void *)nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the Notes page.						*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedFootnoteStyleChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptStyleOptionmenu), w );
    if  ( n < 0 || n >= DPftn_NCOUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnoteStyleChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptStyleOptionmenu), w );
    if  ( n < 0 || n >= DPftn_NCOUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static APP_OITEM_CALLBACK_H( tedFootnoteRestartChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptRestartOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_FootnoteRestarts)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_FootnoteRestarts)/sizeof(int)); return;	}

    np->npRestart= TED_FootnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( npt, np );

    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnoteRestartChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptRestartOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_EndnoteRestarts)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_EndnoteRestarts)/sizeof(int)); return;	}

    np->npRestart= TED_EndnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( npt, np );

    return;
    }

static APP_OITEM_CALLBACK_H( tedFootnotePositionChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptPositionOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_FootnotePositions)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_FootnotePositions)/sizeof(int)); return;	}

    np->npPosition= TED_FootnotePositions[n];

    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnotePositionChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptPositionOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_EndnotePositions)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_EndnotePositions)/sizeof(int)); return;	}

    np->npPosition= TED_EndnotePositions[n];
    if  ( n < 0 || n >= DPftnPOS__COUNT )
	{ LDEB(n); return;	}

    return;
    }

static void tedNotePropertiesToolFillStyleChooser(
				NotesTool *			nt,
				APP_OITEM_CALLBACK_T		callBack,
				NotePropertiesTool *		npt,
				const NotesPageResources *	npr )
    {
    int		i;

    appFillInspectorMenu( DPftn_NCOUNT, DPftnNAR,
		    npt->nptStyleOptions, npr->nprNumberStyleMenuTexts,
		    &(npt->nptStyleOptionmenu),
		    callBack, (void *)nt );

    for ( i= 0; i < DPftnPOS__COUNT; i++ )
	{ npt->nptPositionOptions[i]= (APP_WIDGET)0;	}

    for ( i= 0; i < DPftnRST__COUNT; i++ )
	{ npt->nptRestartOptions[i]= (APP_WIDGET)0;	}

    return;
    }

static void tedNotePropertiesToolFillChoosers(
			NotesTool *			nt,
			NotePropertiesTool *		npt,
			const NotesPageResources *	npr,
			const int *			restarts,
			int				restartCount,
			APP_OITEM_CALLBACK_T		restartChosen,
			const int *			positions,
			int				positionCount,
			APP_OITEM_CALLBACK_T		positionChosen )
    {
    int		i;

    for ( i= 0; i < restartCount; i++ )
	{
	npt->nptRestartOptions[i]=
		appAddItemToOptionmenu( &(npt->nptRestartOptionmenu),
			npr->nprRestartMenuTexts[restarts[i]],
			restartChosen, (void *)nt );
	}

    for ( i= 0; i < positionCount; i++ )
	{
	npt->nptPositionOptions[i]=
		appAddItemToOptionmenu( &(npt->nptPositionOptionmenu),
			npr->nprPositionMenuTexts[positions[i]],
			positionChosen, (void *)nt );
	}

    return;
    }

void tedFormatFillNotesChoosers(	NotesTool *			nt,
					const NotesPageResources *	npr )
    {
    tedNotePropertiesToolFillStyleChooser( nt, tedFootnoteStyleChosen,
				    &(nt->ntFootnotePropertiesTool), npr );

    tedNotePropertiesToolFillStyleChooser( nt, tedEndnoteStyleChosen,
				    &(nt->ntEndnotePropertiesTool), npr );

    tedNotePropertiesToolFillChoosers( nt,
				&(nt->ntFootnotePropertiesTool), npr,

				TED_FootnoteRestarts,
				sizeof(TED_FootnoteRestarts)/sizeof(int),
				tedFootnoteRestartChosen,

				TED_FootnotePositions,
				sizeof(TED_FootnotePositions)/sizeof(int),
				tedFootnotePositionChosen );

    tedNotePropertiesToolFillChoosers( nt,
				&(nt->ntEndnotePropertiesTool), npr,

				TED_EndnoteRestarts,
				sizeof(TED_EndnoteRestarts)/sizeof(int),
				tedEndnoteRestartChosen,

				TED_EndnotePositions,
				sizeof(TED_EndnotePositions)/sizeof(int),
				tedEndnotePositionChosen );

    }

static void tedFinishNotePropertiesTool(
				NotePropertiesTool *		npt )
    {
    appOptionmenuRefreshWidth( &(npt->nptPositionOptionmenu) );
    appOptionmenuRefreshWidth( &(npt->nptRestartOptionmenu) );
    appOptionmenuRefreshWidth( &(npt->nptStyleOptionmenu) );

    return;
    }

void tedFormatFinishNotesPage(		NotesTool *			nt,
					TedFormatTool *			tft,
					const NotesPageResources *	npr )
    {
    tedFinishNotePropertiesTool( &(nt->ntFootnotePropertiesTool) );
    tedFinishNotePropertiesTool( &(nt->ntEndnotePropertiesTool) );
    }

/************************************************************************/
/*									*/
/*  Clean the notes tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanNotesTool(	NotesTool *	nt )
    {
    docCleanDocumentProperties( &(nt->ntPropertiesSet) );
    docCleanDocumentProperties( &(nt->ntPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get notes tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedNotesSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolNotes",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Notes" ),
    APP_RESOURCE( "formatToolChangeNotes",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedNotesToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolThisNote",
		offsetof(NotesPageResources,nprCurrentNoteText),
		"This Note" ),
    APP_RESOURCE( "formatToolFootnotes",
		offsetof(NotesPageResources,nprFootnotesText),
		"Footnotes" ),
    APP_RESOURCE( "formatToolEndnotes",
		offsetof(NotesPageResources,nprEndnotesText),
		"Endnotes" ),

    /**/
    APP_RESOURCE( "formatToolNoteIsFootnote",
		offsetof(NotesPageResources,nprFootnoteText),
		"Footnote" ),
    APP_RESOURCE( "formatToolNoteIsEndnote",
		offsetof(NotesPageResources,nprEndnoteText),
		"Endnote" ),

    APP_RESOURCE( "formatToolToNoteRef",
		offsetof(NotesPageResources,nprToNoteRefText),
		"Find Note" ),
    APP_RESOURCE( "formatToolToNote",
		offsetof(NotesPageResources,nprToNoteText),
		"Edit Note" ),

    APP_RESOURCE( "formatToolRevertNote",
		offsetof(NotesPageResources,nprRevertNoteText),
		"Revert" ),
    APP_RESOURCE( "formatToolChangeNote",
		offsetof(NotesPageResources,nprChangeNoteText),
		"Apply to Note" ),

    /**/
    APP_RESOURCE( "formatToolNotesFirstNumber",
		offsetof(NotesPageResources,nprFirstNumberText),
		"First Number" ),
    APP_RESOURCE( "formatToolNotesPosition",
		offsetof(NotesPageResources,nprPositionText),
		"Position" ),
    APP_RESOURCE( "formatToolNotesRestart",
		offsetof(NotesPageResources,nprRestartText),
		"Numbering" ),
    APP_RESOURCE( "formatToolNotesNumberStyle",
		offsetof(NotesPageResources,nprStyleText),
		"Number Format" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberStyleNar",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNAR]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNalc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNALC]),
	    "a, b, c" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNauc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNAUC]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNrlc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNRLC]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNruc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNRUC]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNchi",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[DPftnNCHI]),
	    "*, +, #, $" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberingContinuous",
	offsetof(NotesPageResources,nprRestartMenuTexts[DPftnRST_CONTINUOUS]),
	"Continuous" ),
    APP_RESOURCE( "formatToolNoteNumberingPerSection",
	offsetof(NotesPageResources,nprRestartMenuTexts[DPftnRST_PER_SECTION]),
	"Per Section" ),
    APP_RESOURCE( "formatToolNoteNumberingPerPage",
	offsetof(NotesPageResources,nprRestartMenuTexts[DPftnRST_PER_PAGE]),
	"Per Page" ),
    /**/
    APP_RESOURCE( "formatToolNotePositionSectEnd",
	offsetof(NotesPageResources,nprPositionMenuTexts[DPftnPOS_SECT_END]),
	"End of Section" ),
    APP_RESOURCE( "formatToolNotePositionDocEnd",
	offsetof(NotesPageResources,nprPositionMenuTexts[DPftnPOS_DOC_END]),
	"End of Document" ),
    APP_RESOURCE( "formatToolNotePositionBelowText",
	offsetof(NotesPageResources,nprPositionMenuTexts[DPftnPOS_BELOW_TEXT]),
	"Below Text" ),
    APP_RESOURCE( "formatToolNotePositionPageBottom",
	offsetof(NotesPageResources,nprPositionMenuTexts[DPftnPOS_PAGE_BOTTOM]),
	"Page Bottom" ),
    };

void tedFormatToolGetNotesResourceTable( EditApplication *		ea,
					NotesPageResources *		npr,
					InspectorSubjectResources *	isr )
    {
    appGuiGetResourceValues( ea, (void *)npr,
				TED_TedNotesToolResourceTable,
				sizeof(TED_TedNotesToolResourceTable)/
				sizeof(AppConfigurableResource) );

    appGuiGetResourceValues( ea, (void *)isr,
				TED_TedNotesSubjectResourceTable,
				sizeof(TED_TedNotesSubjectResourceTable)/
				sizeof(AppConfigurableResource) );

    return;
    }
