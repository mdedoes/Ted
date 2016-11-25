/************************************************************************/
/*									*/
/*  Spell tool and spelling checking definitions.			*/
/*									*/
/************************************************************************/

#   ifndef		APP_SPELL_TOOL_H
#   define		APP_SPELL_TOOL_H

#   include	<guiBase.h>
#   include	<guiOptionmenu.h>
#   include	<utilMemoryBuffer.h>

struct EditApplication;
struct SpellToolResources;
struct SpellChecker;
struct InspectorSubject;
struct InspectorSubjectResources;
struct AppInspector;

/************************************************************************/

typedef struct SpellTool
    {
    struct EditApplication *	stApplication;
    struct AppInspector *	stInspector;

    int				stInProgrammaticChange;

    const struct SpellToolResources *	stResources;

    APP_WIDGET			stDictionaryFrame;
    AppOptionmenu		stDictionaryOptionmenu;

    APP_WIDGET			stLearnButton;
    APP_WIDGET			stForgetButton;
    APP_WIDGET			stDoNotCheckButton;
    APP_WIDGET			stIgnoreButton;
    APP_WIDGET			stFindNextButton;

    APP_WIDGET			stCorrectButton;
    APP_WIDGET			stGuessList;
    APP_WIDGET			stWordText;

    struct SpellChecker *	stSpellChecker; /* Not owned by the tool */

    int				stCurrentDictionary;

				/**
				 *  The text that was rejected the last 
				 *  time.
				 */
    MemoryBuffer		stRejectedText;
				/**
				 *  The dictionary by which the text was
				 *  rejected the last time.
				 */
    int				stRejectedDictionary;

				/**
				 *  The document is read-only, so we cannot 
				 *  correct spelling errors or exclude spans 
				 *  of text from spell checking.
				 */
    unsigned char		stReadOnly;

				/**
				 *  We have proposed an alternative 
				 *  for the word/phrase that was just 
				 *  rejected by the spell checker.
				 */
    unsigned char		stGotProposal;
				/**
				 *  The proposed alternative is in the 
				 *  current dictionary.
				 */
    unsigned char		stProposalInCurrentDict;

				/**
				 *  A stretch of text is selected in the 
				 *  document.
				 */
    unsigned char		stTextSelected;

				/**
				 *  The spell checker just rejected the
				 *  word/phrase that was selected.
				 */
    unsigned char		stTextRejected;
				/**
				 *  The rejected word/phrase is displayed
				 *  on the spell tool
				 */
    unsigned char		stTextRejectedShown;
    } SpellTool;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

extern void appEnableSpellTool(		void *		voidast,
					int		enabled );

extern void appFillSpellTool(	struct SpellChecker *		sc,
				SpellTool *			st,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				int				subjectPage,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

extern void appSpellToolGetResourceTable(
				struct EditApplication *	ea,
				struct InspectorSubjectResources * isr );

extern void appSpellToolSetReadOnly(	SpellTool *		st,
					int			readonly );

extern void appInitSpellTool(		SpellTool *		st );
extern void appCleanSpellTool(		SpellTool *		st );

extern void appSpellToolFillChoosers(	SpellTool *		st );
extern void appFinishSpellTool(		SpellTool *		st );

extern void appSpellToolSelectLocaleTag(
					SpellTool *		st,
					const char *		localeTag );

extern void appSpellToolSetTextSelected( SpellTool *		st,
					int			yes_no );

#   endif	/*	APP_SPELL_TOOL_H	*/
