/************************************************************************/
/*									*/
/*  A spelling tool.							*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	"appSpellTool.h"
#   include	<appQuestion.h>
#   include	<guiToolUtil.h>
#   include	<spellLocaleName.h>
#   include	<indSpellScanJob.h>
#   include	<indSpellChecker.h>
#   include	"appInspector.h"
#   include	"appInspectorSubject.h"
#   include	<guiTextUtil.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	<guiTextValidate.h>

#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct SpellToolResources
    {
    char *		strDictionary;
    char *		strLearn;
    char *		strForget;
    char *		strGuesses;
    char *		strIgnore;
    char *		strFindNext;
    char *		strCorrect;
    char *		strPrivateDictionaries;
    char *		strSystemDictionaries;
    char *		strNoDicts;

    char *		strDoNotCheck;

    char *		strErrors[SCerr_COUNT];
    } SpellToolResources;

static SpellToolResources APP_SpellToolResources;

/************************************************************************/
/*									*/
/*  Adapt the color of the word depending on whether it is misspelled	*/
/*  or not.								*/
/*									*/
/************************************************************************/

static void appSpellAdaptToDiagnosis(	SpellTool *	st,
					int		size,
					int		inDict )
    {
    guiTextShowValidity( st->stWordText, size > 0, inDict );
    }

/************************************************************************/
/*									*/
/*  Adapt buttons to different situations..				*/
/*									*/
/************************************************************************/

# define appSpellToolCanCorrectText( st ) ( \
				! (st)->stReadOnly		&& \
				(st)->stGotProposal		&& \
				! (st)->stTextRejectedShown	&& \
				(st)->stTextRejected		&& \
				(st)->stTextSelected		)

# define appSpellToolCanCorrectLocale( st ) ( \
	  (st)->stGotProposal					&& \
	  (st)->stCurrentDictionary >= 0			&& \
	  (st)->stRejectedDictionary >= 0			&& \
	  (st)->stCurrentDictionary != (st)->stRejectedDictionary && \
	  (st)->stProposalInCurrentDict				)

static void appSpellToolEnableControls(	SpellTool *	st )
    {
    guiEnableWidget( st->stCorrectButton,
			    appSpellToolCanCorrectText( st )	||
			    appSpellToolCanCorrectLocale( st )	);

    guiEnableWidget( st->stIgnoreButton,
				st->stTextRejected		&&
				st->stTextRejectedShown		&&
				st->stTextSelected		);

    guiEnableWidget( st->stDoNotCheckButton,
				! st->stReadOnly		&&
				/*
				st->stTextRejectedShown		&&
				*/
				st->stTextSelected		);

    guiEnableWidget( st->stLearnButton,
				st->stGotProposal		&&
				! st->stProposalInCurrentDict	);

    guiEnableWidget( st->stForgetButton,
				st->stGotProposal		&&
				st->stProposalInCurrentDict	);

    return;
    }

void appSpellToolSetTextSelected(	SpellTool *	st,
					int		textSelected )
    {
    st->stTextSelected= textSelected;

    st->stTextRejected= 0; /* not this one */
    st->stTextRejectedShown= 0; /* not this one */
    st->stGotProposal= 0; /* not for this one */
    st->stProposalInCurrentDict= 0; /* not for this one */

    st->stInProgrammaticChange++;
    guiEmptyListWidget( st->stGuessList );
    guiStringToText( st->stWordText, "" );
    st->stInProgrammaticChange--;

    appSpellAdaptToDiagnosis( st, 0, 0 );
    appSpellToolEnableControls( st );

    return;
    }

static void appSpellToolSetTextRejected(	SpellTool *	st,
						int		textRejected,
						int		rejectedShown )
    {
    st->stTextRejected= textRejected;
    st->stTextRejectedShown= textRejected && rejectedShown;

    appSpellToolEnableControls( st );

    return;
    }

static void appSpellToolGotProposal(	SpellTool *	st,
					int		gotProposal,
					int		inCurrentDict )
    {
    st->stGotProposal= gotProposal;
    st->stProposalInCurrentDict= gotProposal && inCurrentDict;

    appSpellToolEnableControls( st );

    return;
    }

void appSpellToolSetReadOnly(	SpellTool *		st,
				int			readOnly )
    {
    st->stReadOnly= readOnly;

    appSpellToolEnableControls( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Complain								*/
/*									*/
/************************************************************************/

static void appSpellToolComplain(	int			error,
					const char *		subject,
					void *			voidast )
    {
    SpellTool *			st= (SpellTool *)voidast;
    const char *		message= "?";
    const SpellToolResources *	str= st->stResources;
    APP_WIDGET			topWidget= st->stInspector->aiTopWidget;

    if  ( error > 0 && error < SCerr_COUNT )
	{ message= str->strErrors[error]; }

    if  ( subject )
	{
	appQuestionRunSubjectErrorDialog( st->stApplication,
				topWidget, (APP_WIDGET)0, subject, message );
	}
    else{
	appQuestionRunErrorDialog( st->stApplication,
				topWidget, (APP_WIDGET)0, message );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Guess what was intended for word.					*/
/*									*/
/************************************************************************/

static void appSpellMakeGuesses(	SpellTool *		st,
					const MemoryBuffer *	word )
    {
    SpellChecker *		sc;
    SpellDictionary *		sd;

    int				count;
    int				i;
    IndGuessList		igl;

    indInitGuessList( &igl );

    if  ( st->stCurrentDictionary < 0 )
	{ LDEB(st->stCurrentDictionary); goto ready;	}
    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); goto ready;	}

    sd= sc->scDictionaries+ st->stCurrentDictionary;

    count= spellGuessWord( &igl, sd, appSpellToolComplain, (void *)st,
				    sc, utilMemoryBufferGetString( word ) );
    if  ( count < 0 )
	{ LDEB(count); goto ready;	}

    guiEmptyListWidget( st->stGuessList );

    for ( i= 0; i < count; i++ )
	{
	guiAddValueToListWidget( st->stGuessList, -1,
					(char *)igl.iglGuesses[i].igsWord );
	}

  ready:

    indCleanGuessList( &igl );

    return;
    }

void appSpellToolSelectLocaleTag(	SpellTool *	st,
					const char *	localeTag )
    {
    SpellChecker *			sc;
    int					dict;

    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}

    dict= spellFindDictionary( sc->scDictionaries, sc->scDictionaryCount,
								localeTag );
    if  ( dict >= 0 )
	{
	st->stCurrentDictionary= dict;
	guiSetOptionmenu( &(st->stDictionaryOptionmenu), dict );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Look for the next unknown word.					*/
/*									*/
/************************************************************************/

static void appSpellToolFindNext(	SpellTool *	st )
    {
    SpellChecker *		sc;
    SpellDictionary *		sd;
    SpellScanJob		ssj;

    indInitSpellScanJob( &ssj );

    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); goto ready;	}

    if  ( st->stCurrentDictionary < 0				||
	  st->stCurrentDictionary >= sc->scDictionaryCount	)
	{ LLDEB(st->stCurrentDictionary,sc->scDictionaryCount); goto ready; }

    sd= sc->scDictionaries+ st->stCurrentDictionary;

    ssj.ssjChecker= sc;
    ssj.ssjDictionary= sd;
    ssj.ssjComplain= appSpellToolComplain;
    ssj.ssjThrough= (void *)st;

    utilEmptyMemoryBuffer( &(st->stRejectedText) );

    if  ( ! (*sc->scFindNext)( st->stApplication,
					&(st->stRejectedText), &ssj ) )
	{
	if  ( ssj.ssjOverrideLocaleTag )
	    {
	    appSpellToolSelectLocaleTag( st, ssj.ssjOverrideLocaleTag );
	    }

	st->stRejectedDictionary= st->stCurrentDictionary;

	appSpellToolSetTextRejected( st, 1, 1 );
	appSpellMakeGuesses( st, &(st->stRejectedText) );

	st->stInProgrammaticChange++;
	guiBufferToText( st->stWordText, &(st->stRejectedText) );
	st->stInProgrammaticChange--;

	appSpellToolGotProposal( st, 1, 0 );
	appSpellAdaptToDiagnosis( st, st->stRejectedText.mbSize, 0 );
	}
    else{
	st->stRejectedDictionary= -1;

	appSpellToolSetTextRejected( st, 0, 0 );
	appSpellToolGotProposal( st, 0, 0 );
	}

  ready:

    indCleanSpellScanJob( &ssj );

    return;
    }

/************************************************************************/
/*									*/
/*  'Learn/Forget/Ignore' button has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolLearnPushed, w, voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;
    SpellChecker *	sc;
    SpellDictionary *	sd;
    char *		word;

    if  ( st->stCurrentDictionary < 0 )
	{ LDEB(st->stCurrentDictionary); return;	}
    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}

    sd= sc->scDictionaries+ st->stCurrentDictionary;

    word= guiStringFromText( st->stWordText );

    spellLearnWord( sd, appSpellToolComplain, (void *)st, sc, word );

    guiFreeStringFromTextWidget( word );

    appSpellAdaptToDiagnosis( st, strlen( word ), 1 );
    appSpellToolFindNext( st );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSpellToolForgetPushed, w, voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;
    SpellChecker *	sc;
    SpellDictionary *	sd;
    char *		word;

    if  ( st->stCurrentDictionary < 0 )
	{ LDEB(st->stCurrentDictionary); return;	}
    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}

    sd= sc->scDictionaries+ st->stCurrentDictionary;

    word= guiStringFromText( st->stWordText );

    spellForgetWord( sd, appSpellToolComplain, (void *)st, sc, word );

    guiFreeStringFromTextWidget( word );

    appSpellAdaptToDiagnosis( st, strlen( word ), 0 );
    guiEnableWidget( st->stForgetButton, 0 );
    guiEnableWidget( st->stLearnButton, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSpellToolIgnorePushed, w, voidast )
    {
    SpellTool *			st= (SpellTool *)voidast;
    SpellChecker *		sc;
    SpellDictionary *		sd;
    const char *		word;

    if  ( st->stCurrentDictionary < 0 )
	{ LDEB(st->stCurrentDictionary); return;	}
    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}

    sd= sc->scDictionaries+ st->stCurrentDictionary;

    word= utilMemoryBufferGetString( &(st->stRejectedText) );
    if  ( ! word[0] )
	{ SDEB(word);	}
    else{
	spellIgnoreWord( sd, appSpellToolComplain, (void *)st, sc, word );
	}

    appSpellToolFindNext( st );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find Next' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolFindNextPushed, w, voidast )
    {
    appSpellToolFindNext( (SpellTool *)voidast );

    return;
    }

/************************************************************************/

static void appSpellToolAdaptToWordAnDictionary( SpellTool *	st )
    {
    SpellChecker *	sc;

    SpellScanJob	ssj;

    MemoryBuffer	mbTyped;
    int			differ;
    int			inDict= 0;

    utilInitMemoryBuffer( &mbTyped );
    indInitSpellScanJob( &ssj );

    guiBufferFromText( &mbTyped, st->stWordText );

    differ= ! utilEqualMemoryBuffer( &mbTyped, &(st->stRejectedText) );

    if  ( mbTyped.mbSize >= ( st->stRejectedText.mbSize/ 2 )+ 1 )
	{ appSpellMakeGuesses( st, &mbTyped );		}
    else{ guiEmptyListWidget( st->stGuessList );	}

    if  ( mbTyped.mbSize > 0					&&
	  st->stCurrentDictionary >= 0				&&
	  ( sc= st->stSpellChecker )				&&
	  st->stCurrentDictionary < sc->scDictionaryCount	)
	{
	SpellDictionary *		sd;
	int				start= 0;
	int				length= 0;

	sd= sc->scDictionaries+ st->stCurrentDictionary;

	inDict= ! (*sd->sdCheckWord)( &start, &length, &ssj, sd,
				utilMemoryBufferGetString( &mbTyped ), 0,
				mbTyped.mbSize, mbTyped.mbSize, 1 );
	}

    appSpellToolSetTextRejected( st, st->stTextRejected, ! differ );
    appSpellToolGotProposal( st, mbTyped.mbSize > 0, inDict );

    appSpellAdaptToDiagnosis( st, mbTyped.mbSize, inDict );

    utilCleanMemoryBuffer( &mbTyped );
    indCleanSpellScanJob( &ssj );

    return;
    }

/************************************************************************/
/*									*/
/*  The user typed something in the Coreection, Turn on the the		*/
/*  'Correct' button.							*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appSpellCorrectionTyped, w, voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;

    if  ( st->stInProgrammaticChange )
	{ return;	}

    appSpellToolAdaptToWordAnDictionary( st );

    return;
    }

/************************************************************************/
/*									*/
/*  'Correct' button has been pushed.					*/
/*  Or a double click on the listbox with guesses.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolCorrect, w, voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;
    SpellChecker *	sc;
    const char *	localeId= (const char *)0;
    char *		correction= (char *)0;

    if  ( st->stReadOnly )
	{ return;	}

    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}
    if  ( ! sc->scCorrect )
	{ XLDEB(sc->scCorrect,st->stReadOnly); return;	}

    if  ( appSpellToolCanCorrectLocale( st )		&&
	  sc->scSetLocale				)
	{ localeId= sc->scDictionaries[st->stCurrentDictionary].sdLocaleTag; }

    if  ( appSpellToolCanCorrectText( st )	&&
	  sc->scCorrect				)
	{ correction= guiStringFromText( st->stWordText ); }

    if  ( localeId )
	{
	(*sc->scSetLocale)( st->stApplication, localeId );
	}

    if  ( correction )
	{
	if  ( (*sc->scCorrect)( st->stApplication, correction ) )
	    { SDEB(correction);	}

	guiFreeStringFromTextWidget( correction );
	}

    appSpellToolFindNext( st );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Do Not Check' button has been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellDoNotCheckButtonPushed, w, voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;
    SpellChecker *	sc;

    if  ( st->stReadOnly )
	{ return;	}
    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}
    if  ( ! sc->scDoNotCheck || st->stReadOnly )
	{ XLDEB(sc->scDoNotCheck,st->stReadOnly); return;	}

    (*sc->scDoNotCheck)( st->stApplication );

    appSpellToolFindNext( st );

    return;
    }

/************************************************************************/
/*									*/
/*  A guess in the list has been selected.				*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appSpellGuessChosen, w, voidast, voidlcs )
    {
    SpellTool *		st= (SpellTool *)voidast;

    char *		text;
    int			inDict= 0;

    text= guiGetStringFromListCallback( w, voidlcs );
    if  ( text )
	{
	st->stInProgrammaticChange++;
	guiStringToText( st->stWordText, text );
	st->stInProgrammaticChange--;

	inDict= 1;
	}

    appSpellToolSetTextRejected( st, st->stTextRejected, 0 );
    appSpellToolGotProposal( st, text && text[0] != '\0', inDict  );
    appSpellAdaptToDiagnosis( st, text?strlen( text ):0, inDict );

    if  ( text )
	{
	guiFreeStringFromListCallback( text );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A dictionary has been selected.					*/
/*									*/
/************************************************************************/

static void appSpellDictionaryChosen(	int		dict,
					void *		voidast )
    {
    SpellTool *		st= (SpellTool *)voidast;
    SpellChecker *	sc;

    MemoryBuffer	mbGuess;

    utilInitMemoryBuffer( &mbGuess );

    if  ( st->stInProgrammaticChange )
	{ return;	}

    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); goto ready;	}

    if  ( dict < 0 || dict >= sc->scDictionaryCount )
	{ LLDEB(dict,sc->scDictionaryCount); goto ready;	}

    st->stCurrentDictionary= dict;

    appSpellToolAdaptToWordAnDictionary( st );

  ready:

    utilCleanMemoryBuffer( &mbGuess );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the dictionary part of the spelling tool.			*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellMakeDictionaryFrame(
					APP_WIDGET			parent,
					SpellTool *			st )
    {
    const SpellToolResources * const	str= st->stResources;

    APP_WIDGET		frame;
    APP_WIDGET		paned;

    APP_WIDGET		buttonRow;

    guiMakeColumnFrameInColumn( &frame, &paned,
					    parent, str->strDictionary );

    guiMakeOptionmenuInColumn( &(st->stDictionaryOptionmenu), paned,
				    appSpellDictionaryChosen, (void *)st );

    guiToolMake2BottonRow( &buttonRow, paned,
		&(st->stLearnButton), &(st->stForgetButton),
		str->strLearn, str->strForget,
		appSpellToolLearnPushed, appSpellToolForgetPushed,
		(void *)st );

    return frame;
    }

/************************************************************************/
/*									*/
/*  Fill the list of dictionaries.					*/
/*									*/
/************************************************************************/

static void appSpellFillDictionaryMenu(	SpellTool *		st )
    {
    const SpellToolResources * const	str= st->stResources;

    SpellChecker *			sc;
    SpellDictionary *			sd;
    int					dict;
    int					defaultDict= -1;

    if  ( ! ( sc= st->stSpellChecker ) )
	{ XDEB(st->stSpellChecker); return;	}

    guiEmptyOptionmenu( &(st->stDictionaryOptionmenu) );

    sd= sc->scDictionaries;
    for ( dict= 0; dict < sc->scDictionaryCount; sd++, dict++ )
	{
	guiAddItemToOptionmenu(
		    &(st->stDictionaryOptionmenu), sd->sdLocaleLabel );

	if  ( st->stApplication						&&
	      st->stApplication->eaLocaleTag				&&
	      ! strcmp( sd->sdLocaleTag, st->stApplication->eaLocaleTag ) )
	    { defaultDict= dict;	}
	}

    if  ( sc->scDictionaryCount == 0 )
	{
	guiAddItemToOptionmenu(
		    &(st->stDictionaryOptionmenu), str->strNoDicts );
	}

    if  ( defaultDict < 0 )
	{ defaultDict= 0;	}

    guiSetOptionmenu( &(st->stDictionaryOptionmenu), defaultDict );
    st->stCurrentDictionary= defaultDict;

    guiOptionmenuRefreshWidth( &(st->stDictionaryOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Make the listbox with guesses.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellGuessList(	APP_WIDGET			parent,
					SpellTool *			st )
    {
    const SpellToolResources * const	str= st->stResources;
    APP_WIDGET				label;
    APP_WIDGET				list;
    const int				visibleItems= 6;

    guiMakeLabelInColumn( &label, parent, str->strGuesses );

    guiMakeListInColumn( &list, parent, visibleItems,
		    appSpellGuessChosen, appSpellToolCorrect, (void *)st );

    return list;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the spelling Tool.				*/
/*									*/
/************************************************************************/

static void appSpellMakeButtonRows(
				APP_WIDGET			spellForm,
				SpellTool *			st )
    {
    const SpellToolResources * const	str= st->stResources;
    APP_WIDGET				buttonRow;

    guiToolMake2BottonRow( &buttonRow, spellForm,
		&(st->stIgnoreButton), &(st->stFindNextButton),
		str->strIgnore, str->strFindNext,
		appSpellToolIgnorePushed, appSpellToolFindNextPushed,
		(void *)st );

    guiToolMake2BottonRow( &buttonRow, spellForm,
		&(st->stDoNotCheckButton), &(st->stCorrectButton),
		str->strDoNotCheck, str->strCorrect,
		appSpellDoNotCheckButtonPushed, appSpellToolCorrect,
		(void *)st );

    return;
    }

/************************************************************************/

void appInitSpellTool(	SpellTool *	st )
    {
    memset( st, 0, sizeof(SpellTool) );
    }

void appCleanSpellTool(	SpellTool *	st )
    {
    utilCleanMemoryBuffer( &(st->stRejectedText) );

    /* st->stSpellChecker is not owned bt the tool */
    return;
    }

/************************************************************************/
/*									*/
/*  make a spell tool.							*/
/*									*/
/************************************************************************/

void appFillSpellTool(	SpellChecker *			sc,
			SpellTool *			st,
			struct AppInspector *		ai,
			struct EditApplication *	ea,
			InspectorSubject *		is,
			int				subjectPage,
			APP_WIDGET			pageWidget,
			const InspectorSubjectResources * isr )
    {
    st->stResources= &APP_SpellToolResources;

    st->stSpellChecker= sc;
    st->stInspector= ai;
    st->stApplication= ea;

    st->stCurrentDictionary= -1;
    if  ( utilMemoryBufferSetString( &(sc->scPrivateDirectory),
				st->stResources->strPrivateDictionaries ) )
	{ LDEB(1);	}
    if  ( utilMemoryBufferSetString( &(sc->scSystemDirectory),
				st->stResources->strSystemDictionaries ) )
	{ LDEB(1);	}

    guiInitOptionmenu( &(st->stDictionaryOptionmenu) );

    (*sc->scSetup)( sc, appSpellToolComplain, st );

    st->stDictionaryFrame= appSpellMakeDictionaryFrame( pageWidget, st );
    st->stGuessList= appSpellGuessList( pageWidget, st );

    guiMakeTextInColumn( &(st->stWordText), pageWidget, 0, 1 );

    guiSetTypingCallbackForText( st->stWordText,
				    appSpellCorrectionTyped, (void *)st );

    appSpellMakeButtonRows( pageWidget, st );

    appSpellToolSetTextRejected( st, 0, 0 );
    appSpellToolSetTextSelected( st, 0 );
    appSpellToolGotProposal( st, 0, 0 );

    return;
    }

void appSpellToolFillChoosers(	SpellTool *			st )
    {
    if  ( appSpellGetLocaleNames( st->stApplication, st->stSpellChecker ) )
	{ LDEB(1);	}

    appSpellFillDictionaryMenu( st );

    return;
    }

void appFinishSpellTool(		SpellTool *			st )
    {
    SpellChecker *		sc;
    int				hasDict= 1;

    guiOptionmenuRefreshWidth( &(st->stDictionaryOptionmenu) );

    if  ( ! ( sc= st->stSpellChecker ) )
	{ hasDict= 0;	}
    else{
	if  ( st->stCurrentDictionary < 0				||
	      st->stCurrentDictionary >= sc->scDictionaryCount	)
	    { hasDict= 0;	}
	}

    guiEnableWidget( st->stFindNextButton, hasDict );
    }

static AppConfigurableResource APP_SpellToolResourceTable[]=
    {
    /************************/
    /*  Spell Tool.		*/
    /************************/
    APP_RESOURCE( "spellToolDictTitle",
		offsetof(SpellToolResources,strDictionary),
		"Dictionary" ),
    APP_RESOURCE( "spellToolLearn",
		offsetof(SpellToolResources,strLearn),
		"Learn" ),
    APP_RESOURCE( "spellToolForget",
		offsetof(SpellToolResources,strForget),
		"Forget" ),
    APP_RESOURCE( "spellToolGuesses",
		offsetof(SpellToolResources,strGuesses),
		"Guesses" ),
    APP_RESOURCE( "spellToolIgnore",
		offsetof(SpellToolResources,strIgnore),
		"Ignore" ),
    APP_RESOURCE( "spellToolFindNext",
		offsetof(SpellToolResources,strFindNext),
		"Find Next" ),
    APP_RESOURCE( "spellToolDoNotCheck",
		offsetof(SpellToolResources,strDoNotCheck),
		"Do Not Check" ),
    APP_RESOURCE( "spellToolCorrect",
		offsetof(SpellToolResources,strCorrect),
		"Correct" ),
    APP_RESOURCE( "spellToolNoDicts",
		offsetof(SpellToolResources,strNoDicts),
		"None" ),

    APP_RESOURCE( "spellToolPrivateDicts",
		offsetof(SpellToolResources,strPrivateDictionaries),
		".Dictionaries" ),

    APP_RESOURCE( "spellToolSystemDicts",
		offsetof(SpellToolResources,strSystemDictionaries),
		INDDIR ),

    APP_RESOURCE( "spellToolDirNoAccess",
	    offsetof(SpellToolResources,strErrors[SCerrDIR_NO_ACCESS]),
	    "Could not access directory for dictionaries." ),
    APP_RESOURCE( "spellToolDirNoSuchDir",
	    offsetof(SpellToolResources,strErrors[SCerrDIR_DOES_NOT_EXIST]),
	    "This directory could not be found." ),
    APP_RESOURCE( "spellToolDirNoDicts",
	    offsetof(SpellToolResources,strErrors[SCerrDIR_HAS_NO_DICTS]),
	    "No dictionaries were found in this directory." ),
    APP_RESOURCE( "spellToolSysDictNoAccess",
	    offsetof(SpellToolResources,strErrors[SCerrDICT_NO_ACCESS]),
	    "Could not read system dictionary." ),
    APP_RESOURCE( "spellToolPrivDictDirNotMade",
	    offsetof(SpellToolResources,strErrors[SCerrPRIVATE_DIR_NOT_MADE]),
	    "Could not make private directory." ),
    APP_RESOURCE( "spellToolPrivDictNoAccess",
	    offsetof(SpellToolResources,strErrors[SCerrPRIVATE_DICT_NO_ACCESS]),
	    "Could not read private dictionary." ),
    APP_RESOURCE( "spellToolPrivDictWrongFormat",
	    offsetof(SpellToolResources,strErrors[SCerrPRIVATE_DICT_WRONG_FORMAT]),
	    "Private dictionary has an illegal format." ),
    };

static AppConfigurableResource APP_SpellToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "docToolMenuSpellText",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Spelling" ),
    };

void appSpellToolGetResourceTable(	struct EditApplication *	ea,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea,
					(void *)&APP_SpellToolResources,
					APP_SpellToolResourceTable,
					sizeof(APP_SpellToolResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_SpellToolSubjectResourceTable,
				sizeof(APP_SpellToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
