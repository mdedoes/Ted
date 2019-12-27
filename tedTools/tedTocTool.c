/************************************************************************/
/*									*/
/*  Table of Contents page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stdlib.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	<utilTree.h>

#   include	<tedAppFront.h>
#   include	"tedTocTool.h"
#   include	<docField.h>
#   include	<guiToolUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docFieldKind.h>
#   include	<docTocField.h>
#   include	<docListDepth.h>
#   include	<docParaProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelect.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

struct SelectionGeometry;

/************************************************************************/
/*									*/
/*  Resources for the 'Table of Contents' page.				*/
/*									*/
/************************************************************************/

typedef struct TocPageResources
    {
    const char *	tprType;
    const char *	tprTypeItems[TOCtype_COUNT];
    const char *	tprLevels;

    const char *	tprFrom;
    const char *	tprFromStyles;
    const char *	tprFromOutline;
    const char *	tprFromTcEntries;

    const char *	tprSequence;
    const char *	tprNoSequences;

    const char *	tprMakeHyperlinks;
    const char *	tprIncludePageNumbers;

    const char *	tprOutlineLevel;
    const char *	tprOutlineLevels[PPoutline_COUNT+1];
    const char *	tprRevertParaLevel;
    const char *	tprApplyParaLevel;
    } TocPageResources;

/************************************************************************/
/*									*/
/*  Get Toc tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTocSubjectResourceTable[]=
    {
    APP_RESOURCE( "tocToolToc",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Table of Contents" ),

    APP_RESOURCE( "tocToolChangeToc",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply / Refresh" ),
    APP_RESOURCE( "tocToolRevertToc",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Toc */
    /*  no Prev Toc */
    /*  no Select Toc */
    APP_RESOURCE( "tocToolDeleteToc",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Contents" ),
    APP_RESOURCE( "tocToolInsertToc",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Contents" ),
    /*  no Append Toc */
    };

static AppConfigurableResource TED_TedTocToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "tocToolType",
		    offsetof(TocPageResources,tprType),
		    "Table of" ),
    APP_RESOURCE( "tocToolEntries",
		    offsetof(TocPageResources,tprTypeItems[TOCtypeTOC]),
		    "Contents" ),
    APP_RESOURCE( "tocToolCaptions",
		    offsetof(TocPageResources,tprTypeItems[TOCtypeSEQ]),
		    "Captions" ),

    APP_RESOURCE( "tocToolFrom",
		    offsetof(TocPageResources,tprFrom),
		    "From" ),
    APP_RESOURCE( "tocToolFromStyles",
		    offsetof(TocPageResources,tprFromStyles),
		    "Styles" ),
    APP_RESOURCE( "tocToolFromOutlineLevels",
		    offsetof(TocPageResources,tprFromOutline),
		    "Outline Levels" ),
    APP_RESOURCE( "tocToolFromTcEntries",
		    offsetof(TocPageResources,tprFromTcEntries),
		    "Table Entry Fields" ),

    APP_RESOURCE( "tocToolLevels",
		    offsetof(TocPageResources,tprLevels),
		    "(Style) Levels" ),

    APP_RESOURCE( "tocToolSequence",
		    offsetof(TocPageResources,tprSequence),
		    "Sequence" ),
    APP_RESOURCE( "tocToolNoSequences",
		    offsetof(TocPageResources,tprNoSequences),
		    "None Defined" ),

    APP_RESOURCE( "tocToolMakeHyperlinks",
		    offsetof(TocPageResources,tprMakeHyperlinks),
		    "Hyperlinks" ),
    APP_RESOURCE( "tocToolIncludePageNumbers",
		    offsetof(TocPageResources,tprIncludePageNumbers),
		    "Page numbers" ),

    APP_RESOURCE( "tocToolOutlineLevel",
		    offsetof(TocPageResources,tprOutlineLevel),
		    "Outline Level" ),
    APP_RESOURCE( "tocToolOutlineLevel1",
		    offsetof(TocPageResources,tprOutlineLevels[0]),
		    "1" ),
    APP_RESOURCE( "tocToolOutlineLevel2",
		    offsetof(TocPageResources,tprOutlineLevels[1]),
		    "2" ),
    APP_RESOURCE( "tocToolOutlineLevel3",
		    offsetof(TocPageResources,tprOutlineLevels[2]),
		    "3" ),
    APP_RESOURCE( "tocToolOutlineLevel4",
		    offsetof(TocPageResources,tprOutlineLevels[3]),
		    "4" ),
    APP_RESOURCE( "tocToolOutlineLevel5",
		    offsetof(TocPageResources,tprOutlineLevels[4]),
		    "5" ),
    APP_RESOURCE( "tocToolOutlineLevel6",
		    offsetof(TocPageResources,tprOutlineLevels[5]),
		    "6" ),
    APP_RESOURCE( "tocToolOutlineLevel7",
		    offsetof(TocPageResources,tprOutlineLevels[6]),
		    "7" ),
    APP_RESOURCE( "tocToolOutlineLevel8",
		    offsetof(TocPageResources,tprOutlineLevels[7]),
		    "8" ),
    APP_RESOURCE( "tocToolOutlineLevel9",
		    offsetof(TocPageResources,tprOutlineLevels[8]),
		    "9" ),
    APP_RESOURCE( "tocToolOutlineLevelBody",
		    offsetof(TocPageResources,
				    tprOutlineLevels[PPoutlineBODYTEXT]),
		    "Body Text" ),
    APP_RESOURCE( "tocToolOutlineLevelMultiple",
		    offsetof(TocPageResources,
				    tprOutlineLevels[PPoutline_COUNT]),
		    "(multiple)" ),
    APP_RESOURCE( "tocToolRevertParaLevel",
		    offsetof(TocPageResources,tprRevertParaLevel),
		    "Revert" ),
    APP_RESOURCE( "tocToolApplyParaLevel",
		    offsetof(TocPageResources,tprApplyParaLevel),
		    "Apply to Paragraph" ),
    };

/************************************************************************/

static TocPageResources			TED_TocPageResources;
static InspectorSubjectResources	TED_TocSubjectResources;

/************************************************************************/
/*									*/
/*  A toc tool, i.e. the 'Table of Contents' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct TocTool
    {
    struct EditApplication *		ttApplication;
    struct AppInspector *		ttInspector;
    const TocPageResources *	ttPageResources;

    /*  TOC  */
    TocField			ttPropertiesSet;
    TocField			ttPropertiesChosen;
    unsigned char		ttHaveToc;

    APP_WIDGET			ttTocFrame;
    APP_WIDGET			ttTocPaned;
    AppOptionmenu		ttTocTypeMenu;
    AppOptionmenu		ttTocLevelsMenu;

    APP_WIDGET			ttTocEntryNameText;	/* \f		*/
    APP_WIDGET			ttTocFromTcToggle;	/* \f		*/
    APP_WIDGET			ttTocFromStylesToggle;	/* \o		*/
    APP_WIDGET			ttTocFromOutlineToggle;	/* \u		*/

    AppOptionmenu		ttTocSeqOptionMenu;	/* \c \a	*/

    APP_WIDGET			ttTocHyperlinksToggle;	/* \h		*/
    APP_WIDGET			ttTocPageNumbersToggle;	/* \n		*/

    const char **		ttSequences;
    int				ttSequenceCount;

    /*  OUTLINE  */
    int				ttOutlineLevelSet;
    int				ttOutlineLevelChosen;

    APP_WIDGET			ttOutlineFrame;
    APP_WIDGET			ttOutlinePaned;
    AppOptionmenu		ttOulineLevelMenu;
    APP_WIDGET			ttRevertParaButton;
    APP_WIDGET			ttApplyParaButton;

    APP_WIDGET			ttApplyButton;
    APP_WIDGET			ttInsertButton;
    APP_WIDGET			ttDeleteButton;
    } TocTool;

static void tedInitTocTool(	TocTool *	tt )
    {
    memset( tt, 0, sizeof(TocTool) );
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Table of Contents' page of the format tool.		*/
/*									*/
/************************************************************************/

static void tedTocToolRefreshFrom(	TocTool *	tt )
    {
    TocField *			tf= &(tt->ttPropertiesChosen);

    int				valid= 0;

    guiSetToggleState( tt->ttTocFromTcToggle, tf->tfUseTcEntries );
    guiSetToggleState( tt->ttTocFromStylesToggle, tf->tfUseStyleLevels );
    guiSetToggleState( tt->ttTocFromOutlineToggle, tf->tfUseOutlineLevels );

    if  ( tf->tfType == TOCtypeTOC )
	{
	if  ( tf->tfUseTcEntries )
	    { valid= 1;	}
	if  ( tf->tfUseStyleLevels )
	    { valid= 1;	}
	if  ( tf->tfUseOutlineLevels )
	    { valid= 1;	}
	}

    if  ( tf->tfType == TOCtypeSEQ )
	{
	valid= ! utilMemoryBufferIsEmpty( &(tf->tfIdentifierName) );
	}

    guiEnableWidget( tt->ttApplyButton, valid && tt->ttHaveToc );
    guiEnableWidget( tt->ttInsertButton, valid && ! tt->ttHaveToc );
    guiEnableWidget( tt->ttDeleteButton, tt->ttHaveToc );
    }

static void tedFormatAdaptTocToType(		TocTool *	tt )
    {
    TocField *	tf= &(tt->ttPropertiesChosen);

    guiEnableWidget( tt->ttTocFromTcToggle, tf->tfType == TOCtypeTOC );
    guiEnableWidget( tt->ttTocFromStylesToggle, tf->tfType == TOCtypeTOC );
    guiEnableWidget( tt->ttTocFromOutlineToggle, tf->tfType == TOCtypeTOC );

    guiEnableOptionmenu( &(tt->ttTocLevelsMenu),
			    tf->tfType == TOCtypeTOC && tf->tfUseStyleLevels );
    guiEnableOptionmenu( &(tt->ttTocSeqOptionMenu),
			    tf->tfType == TOCtypeSEQ );
    }

static void tedFormatToolRefreshTocPage(	TocTool *	tt )
    {
    int		seq= -1;
    TocField *	tf= &(tt->ttPropertiesChosen);

    guiSetOptionmenu( &(tt->ttTocTypeMenu), tf->tfType );
    tedFormatAdaptTocToType( tt );

    guiSetOptionmenu( &(tt->ttTocLevelsMenu), tf->tfStyleLevel1 );

    if  ( ! utilMemoryBufferIsEmpty( &(tf->tfIdentifierName) ) )
	{
	int	i;

	for ( i= 0; i < tt->ttSequenceCount; i++ )
	    {
	    if  ( utilMemoryBufferEqualsString( &(tf->tfIdentifierName),
							tt->ttSequences[i] ) )
		{ seq= i; break;	}
	    }
	}

    guiSetOptionmenu( &(tt->ttTocSeqOptionMenu), seq );
    guiOptionMenuEnablePosition( &(tt->ttTocTypeMenu), TOCtypeSEQ, 
					    tt->ttSequenceCount > 0 );

    guiSetOptionmenu( &(tt->ttOulineLevelMenu), tt->ttOutlineLevelChosen );
    guiEnableWidget( tt->ttApplyParaButton,
				tt->ttOutlineLevelChosen < PPoutline_COUNT );

    guiSetToggleState( tt->ttTocHyperlinksToggle, tf->tfHyperlinks );
    guiSetToggleState( tt->ttTocPageNumbersToggle,
					tf->tfNLevel0 > PPoutlineDEEPEST );

    tedTocToolRefreshFrom( tt );

    guiEnableWidget( tt->ttOutlineFrame, ! tt->ttHaveToc );
    }

/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedTocFromStylesToggled, w, voidtt, voidtbcs )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    tf->tfUseStyleLevels= ( set != 0 );

    tedTocToolRefreshFrom( tt );
    tedFormatAdaptTocToType( tt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedTocFromOutlinesToggled, w, voidtt, voidtbcs )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    tf->tfUseOutlineLevels= ( set != 0 );

    tedTocToolRefreshFrom( tt );
    tedFormatAdaptTocToType( tt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedTocFromTcToggled, w, voidtt, voidtbcs )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    tf->tfUseTcEntries= ( set != 0 );

    tedTocToolRefreshFrom( tt );
    tedFormatAdaptTocToType( tt );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedTocMakeHyperlinksToggled, w, voidtt, voidtbcs )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    tf->tfHyperlinks= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedTocIncludePageNumbersToggled, w, voidtt, voidtbcs )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    int				set;

    set= guiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	tf->tfNLevel0= PPoutlineBODYTEXT;
	tf->tfNLevel1= PPoutlineBODYTEXT;
	}
    else{
	tf->tfNLevel0= 0;
	tf->tfNLevel1= PPoutlineDEEPEST;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeTocPushed, w, voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);

    /*  All changes are made in the callbacks of the controls
    if  ( tedTocGetTocProperties( tt ) )
	{ return;	}
    */

    tedAppSetTocField( tt->ttApplication, tf );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertTocPushed, w, voidtt )
    {
    TocTool *		tt= (TocTool *)voidtt;

    if  ( docCopyTocField( &(tt->ttPropertiesChosen), &(tt->ttPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshTocPage( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTocChangeParaPushed, w, voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;

    tedAppSetParaOutlineLevel( tt->ttApplication, tt->ttOutlineLevelChosen );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTocRevertParaPushed, w, voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;

    tt->ttOutlineLevelChosen= tt->ttOutlineLevelSet;

    tedFormatToolRefreshTocPage( tt );

    return;
    }

/************************************************************************/

static void tedTocLevel1Chosen(		int		level,
					void *		voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);

    if  ( level < 0 || level >= PPoutline_COUNT )
	{ LLDEB(level,PPoutline_COUNT); return;	}

    tf->tfStyleLevel0= 0;
    tf->tfStyleLevel1= level;

    return;
    }

static void tedTocSequenceChosen(	int		seq,
					void *		voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);
    const char *		found;

    if  ( seq >= tt->ttSequenceCount )
	{ LLDEB(seq,tt->ttSequenceCount); return;	}

    found= tt->ttSequences[seq];

    if  ( utilMemoryBufferSetString( &(tf->tfIdentifierName), found ) )
	{ LDEB(seq);	}

    return;
    }

/************************************************************************/

static void tedTocUsingChosen(		int		type,
					void *		voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);

    if  ( type < 0 || type >= TOCtype_COUNT )
	{ LLDEB(type,TOCtype_COUNT); return;	}

    tf->tfType= type;
    tedFormatAdaptTocToType( tt );

    return;
    }

static void tedOutlineLevelChosen(	int		level,
					void *		voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;

    if  ( level < 0 || level >= PPoutline_COUNT )
	{ LLDEB(level,PPoutline_COUNT); return;	}

    tt->ttOutlineLevelChosen= level;

    tedFormatToolRefreshTocPage( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete/Select Toc buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTocDeleteToc, w, voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;

    tedAppDeleteTocField( tt->ttApplication );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedTocAddToc, w, voidtt )
    {
    TocTool *			tt= (TocTool *)voidtt;
    TocField *			tf= &(tt->ttPropertiesChosen);

    /*  All changes are made in the callbacks of the controls
    if  ( tedTocGetTocProperties( tt ) )
	{ return;	}
    */

    tedAppAddTocField( tt->ttApplication, tf );
    }

/************************************************************************/

static int tedCountSequence(		const char *		key,
					void *			val,
					void *			voidtt )
    {
    TocTool *	tt= (TocTool *)voidtt;

    tt->ttSequenceCount++;

    return 0;
    }

static int tedAddSequenceToMenu(	const char *		key,
					void *			val,
					void *			voidtt )
    {
    TocTool *	tt= (TocTool *)voidtt;

    guiAddItemToOptionmenu( &(tt->ttTocSeqOptionMenu), key );
    tt->ttSequences[tt->ttSequenceCount++]= key;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt the table of contents page to the current selection of this	*/
/*  document.								*/
/*									*/
/************************************************************************/

static void tedRefreshTocTool(	void *				vtt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    TocTool *				tt= (TocTool *)vtt;

    const struct DocumentField *	df;
    struct BufferItem *			paraNode;
    int					outlineLevel;

    /*  TOC  */
    tt->ttHaveToc= 0;
    df= docFindTypedFieldForPosition( (struct BufferDocument *)bd,
						&(ds->dsHead), DOCfkTOC, 0 );
    if  ( df )
	{
	const int	stopOnFailure= 0;

	if  ( docFieldGetToc( &(tt->ttPropertiesSet), df ) )
	    { LDEB(1);	}

	if  ( docCopyTocField( &(tt->ttPropertiesChosen),
						&(tt->ttPropertiesSet) ) )
	    { LDEB(1); *pEnabled= 0; return;	}

	guiEmptyOptionmenu( &(tt->ttTocSeqOptionMenu) );
	tt->ttSequenceCount= 0;
	utilTreeForAll( bd->bdSeqFieldIdentifiers, stopOnFailure,
					    tedCountSequence, (void *)tt );
	if  ( tt->ttSequenceCount > 0 )
	    {
	    tt->ttSequences= realloc( tt->ttSequences,
				    tt->ttSequenceCount * sizeof(char *) );
	    if  ( ! tt->ttSequences )
		{ PDEB(tt->ttSequences); *pEnabled= 0; return;	}

	    tt->ttSequenceCount= 0;
	    utilTreeForAll( bd->bdSeqFieldIdentifiers, stopOnFailure,
					tedAddSequenceToMenu, (void *)tt );
	    }
	else{
	    guiAddItemToOptionmenu( &(tt->ttTocSeqOptionMenu),
					tt->ttPageResources->tprNoSequences );
	    }

	tt->ttHaveToc= 1;
	*pPref += 5;
	}
    else{
	docCleanTocField( &(tt->ttPropertiesSet) );
	docInitTocField( &(tt->ttPropertiesSet) );

	docCleanTocField( &(tt->ttPropertiesChosen) );
	docInitTocField( &(tt->ttPropertiesChosen) );
	}

    /*  OUTLINE  */
    paraNode= ds->dsHead.dpNode;
    outlineLevel= paraNode->biParaProperties->ppOutlineLevel;
    while( paraNode )
	{
	if  ( paraNode->biParaProperties->ppOutlineLevel != outlineLevel )
	    { outlineLevel= PPoutline_COUNT; break;	}

	if  ( paraNode == ds->dsTail.dpNode )
	    { break;	}

	paraNode= docNextParagraph( paraNode );
	}

    tt->ttOutlineLevelChosen= tt->ttOutlineLevelSet= outlineLevel;

    tedFormatToolRefreshTocPage( tt );

    *pEnabled= ! sd->sdDocumentReadonly;
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Toc' page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildTocTool(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const TocPageResources *	tpr= &(TED_TocPageResources);

    APP_WIDGET			row= (APP_WIDGET )0;
    APP_WIDGET			rowLabel= (APP_WIDGET )0;

    TocTool *			tt= malloc( sizeof(TocTool) );

    if  ( ! tt )
	{ XDEB(tt); return tt;	}

    tedInitTocTool( tt );

    /**/
    tt->ttPageResources= tpr;
    tt->ttInspector= ai;
    tt->ttApplication= ea;

    /*  TOC  */

    docInitTocField( &(tt->ttPropertiesSet) );
    docInitTocField( &(tt->ttPropertiesChosen) );

    guiMakeColumnFrameInColumn( &(tt->ttTocFrame),
					    &(tt->ttTocPaned), pageWidget,
					    isr->isrSubjectName );

    /**************/
    guiToolMakeLabelAndMenuRow( &row, &(tt->ttTocTypeMenu),
			    &rowLabel, tt->ttTocPaned, tpr->tprType,
			    tedTocUsingChosen, (void *)tt );

    /**************/

    guiToolMakeLabelAndToggleRow( &row, tt->ttTocPaned,
	    &rowLabel, &(tt->ttTocFromStylesToggle),
	    tpr->tprFrom, tpr->tprFromStyles,
	    tedTocFromStylesToggled, (void *)tt );

    guiToolMakeLabelAndToggleRow( &row, tt->ttTocPaned,
	    &rowLabel, &(tt->ttTocFromOutlineToggle),
	    "", tpr->tprFromOutline,
	    tedTocFromOutlinesToggled, (void *)tt );

    guiToolMakeLabelAndToggleRow( &row, tt->ttTocPaned,
	    &rowLabel, &(tt->ttTocFromTcToggle),
	    "", tpr->tprFromTcEntries,
	    tedTocFromTcToggled, (void *)tt );

    /**************/
    guiToolMakeLabelAndMenuRow( &row, &(tt->ttTocLevelsMenu),
			&rowLabel, tt->ttTocPaned, tpr->tprLevels,
			tedTocLevel1Chosen, (void *)tt );

    guiToolMakeLabelAndMenuRow( &row,
			&(tt->ttTocSeqOptionMenu),
			&rowLabel, tt->ttTocPaned, tpr->tprSequence,
			tedTocSequenceChosen, (void *)tt );

    /**************/
    guiToolMake2ToggleRow( &row, tt->ttTocPaned,
	    &(tt->ttTocHyperlinksToggle), &(tt->ttTocPageNumbersToggle),
	    tpr->tprMakeHyperlinks, tpr->tprIncludePageNumbers,
	    tedTocMakeHyperlinksToggled, tedTocIncludePageNumbersToggled,
	    (void *)tt, (void *)tt );

    /**************/
    guiToolMake2BottonRow( &row, tt->ttTocPaned,
		&(is->isInsertButton), &(is->isDeleteButton),
		isr->isrInsertButtonText, isr->isrDeleteButtonText,
		tedTocAddToc, tedTocDeleteToc, (void *)tt );

    guiToolMake2BottonRow( &(is->isApplyRow), tt->ttTocPaned,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatRevertTocPushed, tedFormatChangeTocPushed,
		(void *)tt );

    tt->ttApplyButton= is->isApplyButton;
    tt->ttInsertButton= is->isInsertButton;
    tt->ttDeleteButton= is->isDeleteButton;

    /*  OUTLINE  */
    tt->ttOutlineLevelSet= PPoutlineBODYTEXT;
    tt->ttOutlineLevelChosen= PPoutlineBODYTEXT;

    guiMakeColumnFrameInColumn( &(tt->ttOutlineFrame),
					    &(tt->ttOutlinePaned), pageWidget,
					    tpr->tprOutlineLevel );

    guiToolMakeLabelAndMenuRow( &row,
			&(tt->ttOulineLevelMenu),
			&rowLabel, tt->ttOutlinePaned, tpr->tprOutlineLevel,
			tedOutlineLevelChosen, (void *)tt );

    guiToolMake2BottonRow( &row, tt->ttOutlinePaned,
		&(tt->ttRevertParaButton), &(tt->ttApplyParaButton),
		tpr->tprRevertParaLevel, tpr->tprApplyParaLevel,
		tedTocRevertParaPushed, tedTocChangeParaPushed,
		(void *)tt );

    return (void *)tt;
    }

/************************************************************************/
/*									*/
/*  Clean the toc tool.							*/
/*									*/
/************************************************************************/

static void tedFreeTocTool(	void *	vtt )
    {
    TocTool *			tt= (TocTool *)vtt;

    docCleanTocField( &(tt->ttPropertiesSet) );
    docCleanTocField( &(tt->ttPropertiesChosen) );

    if  ( tt->ttSequences )
	{ free( tt->ttSequences );	}

    free( tt );

    return;
    }

static void tedTocToolFillChoosers( void *			vtt )
    {
    TocTool *			tt= (TocTool *)vtt;
    const TocPageResources *	tpr= &TED_TocPageResources;

    static const char * const levels[]= { "1","2","3","4","5","6","7","8","9" };

    appFillInspectorMenu( TOCtype_COUNT, TOCtypeTOC,
			    tpr->tprTypeItems, &(tt->ttTocTypeMenu) );

    appFillInspectorMenu( PPoutlineDEEPEST+ 1, 2,
			    levels, &(tt->ttTocLevelsMenu) );

    appFillInspectorMenu( PPoutline_COUNT+ 1, PPoutlineBODYTEXT,
			    tpr->tprOutlineLevels, &(tt->ttOulineLevelMenu) );

#   if USE_MOTIF
    guiAddItemToOptionmenu( &(tt->ttTocSeqOptionMenu),
					tt->ttPageResources->tprNoSequences );
#   endif

    guiOptionMenuEnablePosition( &(tt->ttOulineLevelMenu), PPoutline_COUNT, 0 );
    }

static void tedFinishTocTool( 	void *	vtt )
    {
    TocTool *			tt= (TocTool *)vtt;

    guiOptionmenuRefreshWidth( &(tt->ttTocTypeMenu) );
    guiOptionmenuRefreshWidth( &(tt->ttTocLevelsMenu) );
    guiOptionmenuRefreshWidth( &(tt->ttTocSeqOptionMenu) );
    guiOptionmenuRefreshWidth( &(tt->ttOulineLevelMenu) );
    }

static void tedTocToolGetResourceTable(	struct EditApplication *	ea,
					TocPageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTocToolResourceTable,
				sizeof(TED_TedTocToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTocSubjectResourceTable,
				sizeof(TED_TedTocSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedTocToolFillSubjectTypes( struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedTocToolGetResourceTable( ea,
			&TED_TocPageResources,
			&TED_TocSubjectResources );

    ist->istResources= &TED_TocSubjectResources;

    ist->istBuildTool= tedFormatBuildTocTool;
    ist->istFillChoosers= tedTocToolFillChoosers;
    ist->istFinishPage= tedFinishTocTool;
    ist->istFreeTool= tedFreeTocTool;

    ftst->ftstRefreshPage= tedRefreshTocTool;
    /*ftst->ftstToolResetCache*/

    return 0;
    }
