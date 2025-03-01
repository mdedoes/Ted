/************************************************************************/
/*									*/
/*  The 'Bullets and Numbering' page of the format tool.		*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<limits.h>

#   include	"tedListTool.h"
#   include	<tedAppFront.h>
#   include	"tedToolUtil.h"
#   include	<docTreeType.h>
#   include	<geoUnit.h>
#   include	<guiWidgetsImpl.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	<docListLevel.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	<guiKeys.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<docListAdmin.h>
#   include	<docSelect.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

#   if USE_MOTIF
#	include <Xm/Text.h>
#	include <Xm/PushB.h>
#	include <Xm/RowColumn.h>
#   endif

#   define IS_STRING(idx)	(((idx)%2)==0)
#   define IS_NUMBER(idx)	(((idx)%2)!=0)

/************************************************************************/
/*									*/
/*  Resources for the 'Bullets and Numbers' page.			*/
/*									*/
/************************************************************************/

# define LISTTOOLcountSTYLES	8

typedef struct ListsPageResources
    {
    const char *	lprListLevel;
    const char *	lprNumberStyle;
    const char *	lprNumberFormat;

    const char *	lprStartAt;
    const char *	lprNoRestart;
    const char *	lprJustify;
    const char *	lprFollowedBy;
    const char *	lprStyleOptionTexts[LISTTOOLcountSTYLES];
    const char *	lprJustifyOptionTexts[DOClla_COUNT];
    const char *	lprFollowOptionTexts[DOCllf_COUNT];

    const char *	lprLevelFirstIndent;
    const char *	lprLevelLeftIndent;

    const char *	lprSelection;
    const char *	lprSetList;
    const char *	lprNewList;
    const char *	lprSetLevel;
    const char *	lprRemoveFromList;

    const char *	lprDeleteLevelNumber;
    const char *	lprInsertLevelNumber;
    const char *	lprEditLevelText;
    } ListsPageResources;

/************************************************************************/
/*									*/
/*  A lists tool, i.e. the 'Bullets and Numbers' page of the format	*/
/*  tool.								*/
/*									*/
/************************************************************************/

typedef struct ListTool
    {
    struct EditApplication *	ltApplication;
    struct AppInspector *	ltInspector;
    const ListsPageResources *	ltPageResources;

    int				ltUnitInt;

    int				ltInProgrammaticChange;

    DocumentList		ltListPropertiesSet;
    DocumentList		ltListPropertiesChosen;
    ListOverride		ltListOverrideSet;
    ListOverride		ltListOverrideChosen;
    int				ltCurrentLevel;
    int				ltCurrentParagraphNumber;
    int				ltTabIntervalTwips;

    int				ltHerePath[DLmaxLEVELS];
    int				ltPrevPath[DLmaxLEVELS];
    int				ltCurrPath[DLmaxLEVELS];
    int				ltFormatPath[DLmaxLEVELS];
    int				ltStartPath[DLmaxLEVELS];

    int				ltHereLevel;
    int				ltPrevLevel;
    int				ltFormatIndex0;
    int				ltFormatIndex1;
				/****************************************/
				/*  Possible values range from 0 to	*/
				/*  2* ll->dllNumberSize. Even		*/
				/*  numbers refer to the text. Odd	*/
				/*  numbers to the number. The last	*/
				/*  place is for the text after the	*/
				/*  last number. I.E: these are		*/
				/*  subsctipts in the offsets array.	*/
				/****************************************/
    int				ltFormatOffset0;
    int				ltFormatOffset1;
    int				ltNumberFormatOffsets[2* DLmaxLEVELS+ 2];

    unsigned char		ltCanUpdateSelection;
    unsigned char		ltCanUpdateList;

    /**/
    APP_WIDGET			ltListLevelList;

    APP_WIDGET			ltSelectionFrame;
    APP_WIDGET			ltSelectionPaned;

    APP_WIDGET			ltListLevelFrame;
    APP_WIDGET			ltListLevelPaned;

    AppOptionmenu		ltNumberStyleOptionmenu;

    APP_WIDGET			ltFormatLabel;
    APP_WIDGET			ltNumberFormatText;
    APP_WIDGET			ltNumberFormatMenu;
    int				ltFormatEditable;

    APP_WIDGET			ltFirstIndentText;
    APP_WIDGET			ltLeftIndentText;
    APP_WIDGET			ltStartAtText;
    APP_WIDGET			ltNoRestartToggle;

    AppOptionmenu		ltJustifyOptionmenu;

    AppOptionmenu		ltFollowOptionmenu;

    APP_WIDGET			ltSetListButton;
    APP_WIDGET			ltNewListButton;
    APP_WIDGET			ltSetLevelButton;
    APP_WIDGET			ltRemoveFromListButton;
    } ListTool;

static ListsPageResources		TED_ListsPageResources;
static InspectorSubjectResources	TED_ListsSubjectResources;

/************************************************************************/
/*									*/
/*  Supported styles:							*/
/*									*/
/************************************************************************/

static const int TED_ListToolStyles[LISTTOOLcountSTYLES]=
    {
    DOCpnDEC,
    DOCpnUCRM,
    DOCpnLCRM,
    DOCpnUCLTR,
    DOCpnLCLTR,
    DOCpn_0DEC,
    DOCpn_BULLET,
    DOCpn_NONE,
    };

static void tedListLevelRefreshFormatMenu(	ListTool *	lt );
static void tedFormatToolRefreshListPage(	ListTool *	lt );

static int tedListToolSetFormatLevel(		ListTool *	lt,
						int		number );

/************************************************************************/
/*									*/
/*  Format the level text for a certain list level.			*/
/*									*/
/************************************************************************/

static void tedListToolSetCurrentLevelPath(
				    int *			numberPath,
				    int				level,
				    ListTool *			lt )
    {
    int			l;

    if  ( lt->ltPrevLevel >= 0 )
	{
	for ( l= 0; l <= level; l++ )
	    { numberPath[l]= lt->ltPrevPath[l];	}

	if  ( level > lt->ltPrevLevel )
	    {
	    for ( l= lt->ltPrevLevel+ 1; l <= level; l++ )
		{ numberPath[l]= 0;	}
	    }
	else{
	    numberPath[level]= lt->ltPrevPath[level]+ 1;
	    }
	}
    else{
	for ( l= 0; l <= level; l++ )
	    { numberPath[l]= lt->ltHerePath[l];	}
	}

    return;
    }

static int tedListToolFormatLevelText(	
				    char *			listtext,
				    int				maxlen,
				    int *			offsets,
				    int				maxoffsets,
				    ListTool *			lt,
				    const ListLevel *		ll,
				    int				level )
    {
    int		numberPath[DLmaxLEVELS];
    int		size;

    tedListToolSetCurrentLevelPath( numberPath, level, lt );

    size= docListLevelFormatParagraphNumber( listtext, maxlen,
						offsets, maxoffsets,
						level, numberPath,
						lt->ltStartPath,
						lt->ltFormatPath, ll );
    if  ( size < 0 )
	{ listtext[0]= '\0'; return -1;	}

    return size;
    }

/************************************************************************/
/*									*/
/*  1)	Find the segment where the selection starts. Between two 	*/
/*	segments the choice depends on the end.				*/
/*  2)	NOTE the special case to make an empty free text piece editable	*/
/*  3)	Find the segment where the selection ends. Between two segments	*/
/*	segments the choice depends on the beginning. A selection of	*/
/*	the end of a number is translated to the head of the subsequent	*/
/*	string.								*/
/*  4)	Remember indexes and offsets.					*/
/*  5)	Extend selection to make sure numbers are never partially	*/
/*	selected.							*/
/*  6)	Return 1 if another range was selected.				*/
/*									*/
/************************************************************************/

static int tedListToolSelectNumberFormatRange(	ListTool *	lt,
						const int	off0,
						const int	off1 )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const int			level= lt->ltCurrentLevel;
    const ListLevel *		ll= &(dl->dlLevels[level]);

    const int			fieldCount= ll->llLevelNumberCount;

    int				idx0;
    int				idx1;

    /*  1  */
    for ( idx0= 0; idx0 < 2* fieldCount+ 2; idx0++ )
	{
	if  ( lt->ltNumberFormatOffsets[idx0+ 1] >= off0 )
	    { break;	}
	}
    if  ( off1 > off0 && off0 == lt->ltNumberFormatOffsets[idx0+ 1] )
	{ idx0++;	}

    /*  2  */
    if  ( off0 == off1					&&
	  IS_NUMBER( idx0 )				&&
	  idx0 < 2* fieldCount+ 1			&&
	  off0 == lt->ltNumberFormatOffsets[idx0+ 1]	&&
	  off0 == lt->ltNumberFormatOffsets[idx0+ 2]	)
	{ idx0++;	}

    /*  3  */
    for ( idx1= idx0; idx1 < 2* fieldCount+ 2; idx1++ )
	{
	if  ( lt->ltNumberFormatOffsets[idx1+ 1] >= off1 )
	    { break;	}
	}
    if  ( off0 == off1					&&
	  IS_NUMBER( idx1 )				&&
	  off1 == lt->ltNumberFormatOffsets[idx1+ 1]	)
	{ idx0= idx1= idx1+ 1;	}

    /*  4  */
    lt->ltFormatIndex0= idx0;
    lt->ltFormatIndex1= idx1;
    lt->ltFormatOffset0= off0;
    lt->ltFormatOffset1= off1;

    /*  5  */
    if  ( IS_NUMBER( idx0 ) )
	{ lt->ltFormatOffset0= lt->ltNumberFormatOffsets[idx0];		}
    if  ( IS_NUMBER( idx1 ) )
	{ lt->ltFormatOffset1= lt->ltNumberFormatOffsets[idx1+ 1];	}

    /*  6  */
    if  ( lt->ltFormatOffset0 != off0	||
	  lt->ltFormatOffset1 != off1	)
	{ return 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the controls with the current list level.			*/
/*									*/
/************************************************************************/

#   if USE_GTK
static void tedListToolFormatInsertText(GtkEditable *		w,
					gchar *			new_text,
					gint			new_text_length,
					gint *			position,
					void *			voidlt );

static void tedListToolFormatDeleteText(GtkEditable *		w,
					gint			start_pos,
					gint			end_pos,
					void *			voidlt );
#   endif

static void tedListToolSetLevelFormatText(
					ListTool *		lt,
					const char *		text )
    {

#   if USE_GTK
#   if GTK_MAJOR_VERSION >= 2
			/*  The weird construct is to make good compilers
			 *  shut up about the weird and ugly construct in GTK
			 */
    GCallback	sit= (GCallback)tedListToolFormatInsertText;
    GCallback	sdt= (GCallback)tedListToolFormatDeleteText;

    gpointer		it= (gpointer)0;
    gpointer		dt= (gpointer)0;

    if  ( sizeof(GCallback) != sizeof(gpointer) )
	{ LLDEB(sizeof(GCallback),sizeof(gpointer)); return;	}

    memcpy( &it, &sit, sizeof(gpointer) );
    memcpy( &dt, &sdt, sizeof(gpointer) );

    g_signal_handlers_block_by_func( lt->ltNumberFormatText, it, (void *)lt );
    g_signal_handlers_block_by_func( lt->ltNumberFormatText, dt, (void *)lt );
#   endif
#   endif

    guiStringToText( lt->ltNumberFormatText, (char *)text );

#   if USE_GTK
#   if GTK_MAJOR_VERSION >= 2
    g_signal_handlers_unblock_by_func( lt->ltNumberFormatText, it, (void *)lt );
    g_signal_handlers_unblock_by_func( lt->ltNumberFormatText, dt, (void *)lt );
#   endif
#   endif
    }

static void tedListToolRefreshLevelFormat(	ListTool *		lt,
						const ListLevel *	ll,
						int			level )
    {
    char		listtext[255+1];

    tedListToolFormatLevelText( listtext, sizeof(listtext)- 1,
				lt->ltNumberFormatOffsets, 2* DLmaxLEVELS+ 2,
				lt, ll, level );

    tedListToolSetLevelFormatText( lt, listtext );

    lt->ltFormatIndex0= lt->ltFormatIndex1= -1;
    lt->ltFormatOffset0= lt->ltFormatOffset1= -1;

    tedListToolSelectNumberFormatRange( lt, 0, 0 );

    guiTextSelectContents( lt->ltNumberFormatText,
				lt->ltFormatOffset0, lt->ltFormatOffset1 );

    return;
    }

/************************************************************************/
/*									*/
/*  A level to be used in the format string has been chosen.		*/
/*									*/
/************************************************************************/

# if USE_MOTIF

static void tedListFormatLevelChosen(	Widget			w,
					void *			voidlt,
					void *			pbcs )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;

    short		number= -1;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    XtVaGetValues( w,	XmNpositionIndex,	&number,
			NULL );

    if  ( number < 0 || number > level )
	{ LLDEB(number,level); return;	}

    tedListToolSetFormatLevel( lt, number );

    return;
    }

# endif

/************************************************************************/
/*									*/
/*  Display the popup that belongs to this drawn popdown.		*/
/*									*/
/************************************************************************/

#   if USE_MOTIF

static int tedListToolGetSelection(	APP_WIDGET		w,
					ListTool *		lt )
    {
    XmTextPosition		start;
    XmTextPosition		end;

    if  ( ! XmTextGetSelectionPosition( w, &start, &end ) )
	{ start= end= XmTextGetCursorPosition( w );	}

    return tedListToolSelectNumberFormatRange( lt, start, end );
    }

static void tedListToolFormatSelectionChanged(
					APP_WIDGET		w,
					void *			voidlt,
					void *			voidcbs )
    {
    XmTextVerifyCallbackStruct *	cbs;

    cbs= (XmTextVerifyCallbackStruct *)voidcbs;

    if  ( cbs->reason == XmCR_MOVING_INSERT_CURSOR )
	{
	ListTool *		lt= (ListTool *)voidlt;

	XmTextPosition		off0;
	XmTextPosition		off1;

	if  ( XmTextGetSelectionPosition( w, &off0, &off1 )	&&
	      off0 != off1					)
	    {
	    if  ( off0 != cbs->newInsert && off1 != cbs->newInsert )
		{ LLLDEB(cbs->newInsert,off0,off1); return;	}
	    }
	else{ off0= off1= cbs->newInsert;	}

	if  ( tedListToolSelectNumberFormatRange( lt, off0, off1 ) )
	    {
	    guiTextSelectContents( lt->ltNumberFormatText,
				    lt->ltFormatOffset0, lt->ltFormatOffset1 );
	    }
	}

    return;
    }

#   endif

#   if USE_GTK

static int tedListToolGetSelection(	APP_WIDGET		w,
					ListTool *		lt )
    {
    gint		start;
    gint		end;

#   if GTK_MAJOR_VERSION < 2
    start= end= gtk_editable_get_position( GTK_EDITABLE( w ) );
#   else
    if  ( ! gtk_editable_get_selection_bounds( GTK_EDITABLE( w ),
							    &start, &end ) )
	{ start= end= gtk_editable_get_position( GTK_EDITABLE( w ) );	}
#   endif

    return tedListToolSelectNumberFormatRange( lt, start, end );
    }

# if GTK_MAJOR_VERSION >= 2

static void tedListToolFormatSelectionChanged(
					GtkEntry *		w,
					GtkMovementStep		step,
					gint			arg2,
					gboolean		arg3,
					void *			voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;

    if  ( tedListToolGetSelection( lt->ltNumberFormatText, lt ) )
	{
	g_signal_stop_emission_by_name( w, "move_cursor" );

	guiTextSelectContents( lt->ltNumberFormatText,
				lt->ltFormatOffset0, lt->ltFormatOffset1 );
	}

    return;
    }

#   endif

#   endif

static void tedListToolRefreshFormat(	ListTool *	lt,
					const int	level,
					ListLevel *	ll,
					int		textAlso )
    {
    char		listtext[255+1];
    char		listScratch[30+sizeof(listtext)];

    tedListToolFormatLevelText( listtext, sizeof(listtext)- 1,
				lt->ltNumberFormatOffsets, 2* DLmaxLEVELS+ 2,
				lt, ll, level );

    if  ( textAlso )
	{ tedListToolSetLevelFormatText( lt, listtext ); }

    sprintf( (char *)listScratch, "%d: %s", level+ 1, (char *)listtext );

    guiReplaceValueInListWidget( lt->ltListLevelList, level,
							(char *)listScratch );

    return;
    }

/************************************************************************/

static int tedListToolModifyConstPiece( ListLevel *		ll,
					int			field,
					int			off0,
					int			off1,
					const char *		text,
					int			length )

    {
    int				size;
    char			scratch[255+1];
    int				shift= off0- off1+ length;
    int				changed= 0;

    size= docListLevelGetText( scratch, sizeof(scratch)- 1, ll, field );
    if  ( size < 0 )
	{ LLDEB(field,size); return -1; }

    memmove( scratch+ off1+ shift, scratch+ off1, size- off1 );
    if  ( length > 0 )
	{ memcpy( scratch+ off0, text, length );	}
    scratch[size+shift]= '\0';

    docListLevelSetText( &changed, ll, scratch, field );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the selection in the number format with a text string.	*/
/*									*/
/*  The alogorithm is atrocious; given the size of the strings and the	*/
/*  infrequency of te action that is not a problem.			*/
/*									*/
/*  1)  String replacements are done before the removal of the numbers	*/
/*	to avoid interference of the concatenation of the strings by	*/
/*	the removal of the numbers.					*/
/*  2)  Do the text replacement in the first string piece.		*/
/*  3)  Do the text replacement in the last and other string pieces in	*/
/*	reverse order.							*/
/*  4)  Delete any numbers.						*/
/*									*/
/************************************************************************/

static int tedListToolModifyFormatText(	ListTool *		lt,
					const char *		text,
					int			length )
    {
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const int		level= lt->ltCurrentLevel;
    ListLevel *		ll= &(dl->dlLevels[level]);

    int			idx0;
    int			idx1;
    int			off0;
    int			off1;

    /*  1  */

    /*  2  */
    idx0= lt->ltFormatIndex0;
    off0= lt->ltFormatOffset0;
    if  ( IS_NUMBER( idx0 ) )
	{
	if  ( off0 != lt->ltNumberFormatOffsets[idx0] )
	    { LLDEB(off0,lt->ltNumberFormatOffsets[idx0]);	}

	idx0++; off0= lt->ltNumberFormatOffsets[idx0];
	}

    idx1= idx0;
    off1= lt->ltFormatOffset1;
    if  ( lt->ltFormatIndex1 > idx0 )
	{ idx1++; off1= lt->ltNumberFormatOffsets[idx1];	} 

    if  ( off0 < lt->ltNumberFormatOffsets[idx0] )
	{ LLDEB(off0,lt->ltNumberFormatOffsets[idx0]); return -1; }
    if  ( off1 < lt->ltNumberFormatOffsets[idx0] )
	{ LLDEB(off1,lt->ltNumberFormatOffsets[idx0]); return -1; }

    off0 -= lt->ltNumberFormatOffsets[idx0];
    off1 -= lt->ltNumberFormatOffsets[idx0];

    if  ( tedListToolModifyConstPiece( ll, idx0/2,
						off0, off1, text, length ) )
	{ LDEB(idx0/2); return -1;	}

    /*  3  */
    off0= 0;
    idx1= lt->ltFormatIndex1;
    off1= lt->ltFormatOffset1;
    if  ( IS_NUMBER( idx1 ) )
	{ off1= lt->ltNumberFormatOffsets[idx1]; idx1--;	}

    while( idx1 > idx0 )
	{
	off1 -= lt->ltNumberFormatOffsets[idx1];

	if  ( tedListToolModifyConstPiece( ll, idx1/2, off0, off1, "", 0 ) )
	    { LDEB(idx1/2); return -1;	}

	idx1--;
	off1= lt->ltNumberFormatOffsets[idx1]; idx1--;
	}

    /*  4  */
    idx0= lt->ltFormatIndex0;
    idx1= lt->ltFormatIndex1;
    if  ( IS_STRING( idx1 ) )
	{ idx1--;	}
    while( idx1 >= idx0 )
	{ docListLevelDeleteNumber( ll, idx1/ 2 ); idx1 -= 2; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection in the level format with a number	*/
/*  placeholder.							*/
/*									*/
/*  1)  Save strings that we might need to redistribute over the result.*/
/*  2)  Remove the current selection.					*/
/*  3)  Insert a number placeholder before the current selection.	*/
/*  4)  Divide the string over the result.				*/
/*  5)  Refresh tool and select the position after the new insert.	*/
/*									*/
/************************************************************************/

static int tedListToolSetFormatLevel(	ListTool *	lt,
					int		number )
    {
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll= &(dl->dlLevels[level]);

    int			idx0= lt->ltFormatIndex0;
    int			idx1= lt->ltFormatIndex1;
    char		scratch[255+1];
    int			size;
    int			split= -1;

    tedListToolGetSelection( lt->ltNumberFormatText, lt );

    /*  1  */
    size= docListLevelGetText( scratch, sizeof(scratch)- 1, ll, idx0/ 2 );
    if  ( size < 0 )
	{ LLDEB(idx0/2,size); return -1; }

    if  ( IS_STRING( idx0 ) )
	{ split= lt->ltFormatOffset0- lt->ltNumberFormatOffsets[idx0];	}
    else{
	split= size;

	if  ( IS_STRING( idx1 ) )
	    {
	    int		off1;

	    size= docListLevelGetText( scratch+ split,
			    sizeof(scratch)- split- 1, ll, idx1/ 2 );
	    if  ( size < 0 )
		{ LLDEB(idx1/2,size); return -1; }

	    /*  +1: '\0'  */
	    off1= lt->ltFormatOffset1- lt->ltNumberFormatOffsets[idx1];
	    memmove( scratch+ split, scratch+ split+ off1, size- off1+ 1 );
	    }
	else{
	    size= docListLevelGetText( scratch+ split,
			    sizeof(scratch)- split- 1, ll, idx1/ 2+ 1 );
	    if  ( size < 0 )
		{ LLDEB(idx1/2+ 1,size); return -1; }
	    }
	}

    /*  2  */
    tedListToolModifyFormatText( lt, "", 0 );

    /*  3  */
    docListLevelInsertNumber( ll, number, idx0/ 2 );

    /*  4  */
    if  ( split >= 0 )
	{
	int	changed= 1;

	docListLevelSetText( &changed, ll, scratch+ split, idx0/ 2+ 1 );
	scratch[split]= '\0';
	docListLevelSetText( &changed, ll, scratch       , idx0/ 2 );
	}

    /*  5  */
    tedListToolRefreshFormat( lt, level, ll, 1 );

    tedListToolSelectNumberFormatRange( lt,
				    lt->ltNumberFormatOffsets[idx0+2],
				    lt->ltNumberFormatOffsets[idx0+2] );

    guiTextSelectContents( lt->ltNumberFormatText,
				    lt->ltFormatOffset0, lt->ltFormatOffset1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Editing callback:							*/
/*									*/
/*  Substitute a piece of string for the current selection. If we think	*/
/*  that our idea of what is selected differs from the callback, we	*/
/*  refresh our idea. NOTE that the refreshed idea might differ from	*/
/*  both the origibal idea and the selection in the callback.		*/
/*									*/
/************************************************************************/

static void tedListToolReplaceText(	ListTool *		lt,
					const char *		text,
					int			length )
    {
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const int		level= lt->ltCurrentLevel;
    ListLevel *		ll= &(dl->dlLevels[level]);

    tedListToolModifyFormatText( lt, text, length );

    tedListToolRefreshFormat( lt, level, ll, 0 );

    tedListToolSelectNumberFormatRange( lt, lt->ltFormatOffset0,
						lt->ltFormatOffset0+ length );

    return;
    }

#   if USE_MOTIF

static void tedListToolFormatModified(	APP_WIDGET		w,
					void *			voidlt,
					void *			voidcbs )
    {
    XmTextVerifyCallbackStruct *	cbs;

    cbs= (XmTextVerifyCallbackStruct *)voidcbs;

    if  ( cbs->event && cbs->reason == XmCR_MODIFYING_TEXT_VALUE )
	{
	ListTool *		lt= (ListTool *)voidlt;

	if  ( tedListToolSelectNumberFormatRange( lt,
						cbs->startPos, cbs->endPos ) )
	    {
	    cbs->startPos= lt->ltFormatOffset0;
	    cbs->endPos= lt->ltFormatOffset1;
	    }

	tedListToolReplaceText( lt, cbs->text->ptr, cbs->text->length );
	}

    return;
    }

#   endif

#   if USE_GTK

static void tedListToolFormatInsertText(GtkEditable *		w,
					gchar *			new_text,
					gint			new_text_length,
					gint *			position,
					void *			voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;

    if  ( tedListToolSelectNumberFormatRange( lt, *position, *position ) )
	{
LDEB(*position);
	}
    else{
	tedListToolReplaceText( lt, new_text, new_text_length );
	}

    return;
    }

static void tedListToolFormatDeleteText(GtkEditable *		w,
					gint			start_pos,
					gint			end_pos,
					void *			voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;

    if  ( tedListToolSelectNumberFormatRange( lt, start_pos, end_pos ) )
	{
#	if GTK_MAJOR_VERSION >= 2
	g_signal_stop_emission_by_name( w, "delete_text" );
#	endif

	gtk_editable_delete_text( w, lt->ltFormatOffset0, lt->ltFormatOffset1 );
	}

    tedListToolReplaceText( lt, "", 0 );

    return;
    }

#   endif

#   if USE_MOTIF

static void tedListLevelRefreshFormatMenu(	ListTool *	lt )
    {
    int				level= lt->ltCurrentLevel;

    Arg				al[20];
    int				ac= 0;
    int				acCommon;
    Widget			option;

    char			scratch[50];
    char			acci[30+1];

    int				lev;

    ac= 0;
    XtSetArg( al[ac], XmNmarginWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNborderWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNhighlightThickness,	1 ); ac++;
    XtSetArg( al[ac], XmNshadowThickness,	1 ); ac++;
    XtSetArg( al[ac], XmNfillOnArm,		True ); ac++;

    acCommon= ac;

    /**/
    guiEmptyParentWidget( lt->ltNumberFormatMenu );

    for ( lev= 0; lev <= level; lev++ )
	{
	char		texti[30+1];

	ac= acCommon;

	if  ( docListLevelFormatLevelNumber( texti, sizeof(texti)- 1,
				    lt->ltCurrPath[lev]+ lt->ltStartPath[lev],
				    lt->ltFormatPath[lev] ) < 0 )
	    {
	    LLDEB(lev,lt->ltCurrPath[lev]);
	    strcpy( (char *)texti, "?" );
	    }

	sprintf( scratch, "%d: %s", lev+ 1, texti );
	sprintf( acci, "<Key>%d", lev+ 1 );

	XtSetArg( al[ac], XmNaccelerator, acci ); ac++;

	option= XmCreatePushButton( lt->ltNumberFormatMenu, 
						    scratch, al, ac );

	XtAddCallback( option,
		XmNactivateCallback, tedListFormatLevelChosen, (void *)lt );

	XtManageChild( option );
	}

    return;
    }

#   endif

#   if USE_GTK

static void tedListLevelRefreshFormatMenu(	ListTool *	lt )
    {
    int				level= lt->ltCurrentLevel;

    int				lev;

    guiEmptyParentWidget( lt->ltNumberFormatMenu );

    for ( lev= 0; lev <= level; lev++ )
	{
	GtkWidget *	item;
	char		texti[30+1];

	if  ( docListLevelFormatLevelNumber( texti, sizeof(texti)- 1,
				    lt->ltCurrPath[lev]+ lt->ltStartPath[lev],
				    lt->ltFormatPath[lev] ) < 0 )
	    {
	    LLDEB(lev,lt->ltCurrPath[lev]);
	    strcpy( (char *)texti, "?" );
	    }

	item= gtk_menu_item_new_with_label( (gchar *)texti );
	gtk_menu_shell_append( GTK_MENU_SHELL( lt->ltNumberFormatMenu ), item );
	}
    }

#   endif

static void tedListToolSetLevelHeader(		ListTool *	lt,
						const char *	level )
    {
    const ListsPageResources *	lpr= lt->ltPageResources;
    int				levlen= strlen( level );
    int				size;
    const char *		from;
    char *			scratch;
    char *			to;

    size= 0;

    from= lpr->lprListLevel;
    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'd' )
	    { from++; size += levlen;	}
	else{ size++;			}

	from++;
	}

    scratch= to= malloc( size+ 1 );
    if  ( ! scratch || ! to )
	{ LXDEB(size,scratch); return; }

    from= lpr->lprListLevel;
    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'd' )
	    { from++; strcpy( to, level ); to += levlen;	}
	else{ *(to++)= *from;						}

	from++;
	}

    *(to)= '\0';

    guiSetFrameTitle( lt->ltListLevelFrame, scratch );

    free( scratch );

    return;
    }


static void tedListToolRefreshIndents(	ListTool *		lt,
					const ListLevel *	ll )
    {
    guiLengthToText( lt->ltFirstIndentText,
	    ll->llLeftIndentTwips+ ll->llFirstIndentTwips, UNITtyPOINTS );

    guiLengthToText( lt->ltLeftIndentText,
	    ll->llLeftIndentTwips, UNITtyPOINTS );
    }

static void tedListToolRefreshCurrentLevel(	ListTool *	lt )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    const ListLevel *		ll;
    int				level= lt->ltCurrentLevel;

    int				i;
    char			scratch[50];

    if  ( level < 0 || level >= dl->dlLevelCount )
	{
	tedListToolSetLevelHeader( lt, "" );
	guiStringToText( lt->ltStartAtText, "" );
	tedListToolSetLevelFormatText( lt, "" );

	guiSetOptionmenu( &(lt->ltNumberStyleOptionmenu), -1 );
	guiSetOptionmenu( &(lt->ltJustifyOptionmenu), -1 );
	guiSetOptionmenu( &(lt->ltFollowOptionmenu), -1 );

	return;
	}

    sprintf( scratch, "%d", level+ 1 );
    tedListToolSetLevelHeader( lt, scratch );

    tedListToolSetCurrentLevelPath( lt->ltCurrPath, lt->ltCurrentLevel, lt );

    ll= &(dl->dlLevels[level]);

    tedListToolRefreshIndents( lt, ll );

    guiIntegerToText( lt->ltStartAtText, ll->llStartAt );

    for ( i= 0; i < LISTTOOLcountSTYLES; i++ )
	{
	if  ( ll->llNumberStyle == TED_ListToolStyles[i] )
	    { break;	}
	}
    if  ( i >= LISTTOOLcountSTYLES )
	{ i= -1;	}

    guiSetOptionmenu( &(lt->ltNumberStyleOptionmenu), i );
    guiSetOptionmenu( &(lt->ltJustifyOptionmenu), ll->llJustification );
    guiSetOptionmenu( &(lt->ltFollowOptionmenu), ll->llFollow );

    /**/
    tedListToolRefreshLevelFormat( lt, ll, level );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a certain tab in the list.					*/
/*									*/
/************************************************************************/

static void tedListToolSelectLevel(	ListTool *	lt,
					int		level )
    {
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);

    if  ( level >= 0 && level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); level= -1;	}

    if  ( level >= 0 )
	{
	lt->ltCurrentLevel= level;

	guiSelectPositionInListWidget( lt->ltListLevelList, level );
	}
    else{
	lt->ltCurrentLevel= -1;

	guiRemoveSelectionFromListWidget( lt->ltListLevelList );
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

    lt->ltInProgrammaticChange++;

    guiEmptyListWidget( lt->ltListLevelList );

    if  ( dl->dlLevelCount > 0 )
	{
	char			listtext[255+1];
	char			listScratch[30+sizeof(listtext)];

	const ListLevel *	ll;
	int			level;

	docListGetFormatPath( lt->ltStartPath, lt->ltFormatPath,
					&ll, dl->dlLevelCount- 1, dl, lo );

	ll= dl->dlLevels;
	for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
	    {
	    tedListToolFormatLevelText( listtext, sizeof(listtext)- 1,
							    (int *)0, 0,
							    lt, ll, level );

	    sprintf( listScratch, "%d: %s", level+ 1, listtext );

	    guiAddValueToListWidget( lt->ltListLevelList,
							level, listScratch );
	    }

	tedListToolSelectLevel( lt, lt->ltCurrentLevel );
	}

    lt->ltInProgrammaticChange--;

    return;
    }

/************************************************************************/

static void tedListEnableListLevel(	ListTool *	lt,
					int		enabled )
    {
    guiEnableWidget( lt->ltListLevelFrame, enabled );

    guiEnableText( lt->ltNumberFormatText, enabled );
    guiEnableText( lt->ltFirstIndentText, enabled );
    guiEnableText( lt->ltLeftIndentText, enabled );
    guiEnableText( lt->ltStartAtText, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the lists tool to the current selection in the document.	*/
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
    lt->ltFormatIndex0= -1;
    lt->ltFormatIndex1= -1;
    lt->ltFormatOffset0= -1;
    lt->ltFormatOffset1= -1;
    lt->ltCurrentParagraphNumber= -1;

    guiEnableWidget( lt->ltRemoveFromListButton, 0 );
    guiEnableWidget( is->isApplyButton, 0 );
    guiEnableWidget( is->isRevertButton, 0 );

    tedListEnableListLevel( lt, 0 );
    guiEnableWidget( lt->ltSetLevelButton, 0 );

    return;
    }

static int tedFormatToolSetList(	ListTool *			lt,
					InspectorSubject *		is,
					ListNumberTrees *		lnt,
					int				ls,
					int				level,
					int				paraNr,
					ListOverride *			lo,
					DocumentList *			dl )
    {
    if  ( docCopyDocumentListSameDocument( &(lt->ltListPropertiesSet), dl ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyDocumentListSameDocument( &(lt->ltListPropertiesChosen), dl ) )
	{ LDEB(1); return -1;	}

    if  ( docCopyListOverrideSameDocument( &(lt->ltListOverrideSet), lo ) )
	{ LDEB(1); return -1;	}
    if  ( docCopyListOverrideSameDocument( &(lt->ltListOverrideChosen), lo ) )
	{ LDEB(1); return -1;	}

    lt->ltHereLevel= level;
    lt->ltFormatIndex0= -1;
    lt->ltFormatIndex1= -1;
    lt->ltFormatOffset0= -1;
    lt->ltFormatOffset1= -1;

    if  ( paraNr != lt->ltCurrentParagraphNumber			||
	  lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >=
			    lt->ltListPropertiesChosen.dlLevelCount	)
	{ lt->ltCurrentLevel= level;	}

    lt->ltCurrentParagraphNumber= paraNr;

    docListNumberTreesGetNumberPath( lt->ltHerePath, lnt, ls, level, paraNr );

    if  ( docListNumberTreesPrevNumberPath( lt->ltPrevPath, &(lt->ltPrevLevel),
							    lnt, ls, paraNr ) )
	{
	int i;

	for ( i= 0; i <= level; i++ )
	    { lt->ltPrevPath[i]= lt->ltHerePath[i];	}

	lt->ltPrevLevel= -1;
	}

    guiEnableWidget( lt->ltRemoveFromListButton, lt->ltCanUpdateSelection );

    guiEnableWidget( is->isApplyButton, lt->ltCanUpdateList );
    guiEnableWidget( is->isRevertButton, lt->ltCanUpdateList );

    tedListEnableListLevel( lt, lt->ltCanUpdateList );

    guiEnableWidget( lt->ltSetLevelButton, lt->ltCanUpdateSelection );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the list tool. I.E: Display the current selection in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

static void tedRefreshListTool(	void *				vlt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    ListTool *				lt= (ListTool *)vlt;
    ListOverride *			lo;
    DocumentList *			dl;
    const DocumentProperties *		dp= bd->bdProperties;

    int					i;

    lt->ltInProgrammaticChange++;

    for ( i= 0; i < DLmaxLEVELS; i++ )
	{
	lt->ltPrevPath[i]= 0;
	lt->ltHerePath[i]= 0;
	lt->ltCurrPath[i]= 0;
	}

    lt->ltPrevLevel= -1;
    lt->ltHereLevel= -1;
    lt->ltFormatIndex0= -1;
    lt->ltFormatIndex1= -1;
    lt->ltFormatOffset0= -1;
    lt->ltFormatOffset1= -1;

    lt->ltCanUpdateSelection= cmdEnabled[EDITcmdUPD_PARA_PROPS];
    lt->ltCanUpdateList= cmdEnabled[EDITcmdUPD_LIST];

    if  ( ds->dsSelectionScope.ssTreeType != DOCinBODY		||
	  ! sd->sdHasLists					||
	  sd->sdListOverride < 1				||
	  docGetListOfParagraph( &lo, &dl,
				    sd->sdListOverride, bd )	)
	{ tedFormatToolNoList( lt, is );	}
    else{
	/* Input was const. We are not going to update this. */
	struct BufferDocument *		xbd= (struct BufferDocument *)bd;

	if  ( tedFormatToolSetList( lt, is, &(xbd->bdBody.dtListNumberTrees),
			sd->sdListOverride, sd->sdListLevel, sd->sdFirstListParaNr,
			lo, dl ) )
	    { LDEB(sd->sdFirstListParaNr); goto ready;	}

	lt->ltTabIntervalTwips= dp->dpTabIntervalTwips;
	}

    /**/
    if  ( lt->ltCurrentLevel < 0					||
	  lt->ltCurrentLevel >= lt->ltListPropertiesChosen.dlLevelCount	)
	{
	if  ( lt->ltListPropertiesChosen.dlLevelCount > 0 )
	    { lt->ltCurrentLevel= 0;	}
	else{ lt->ltCurrentLevel= -1;	}
	}

    if  ( lt->ltCanUpdateSelection		&&
	  sd->sdListOverride > 0		&&
	  sd->sdMultiList			)
	{ guiEnableWidget( lt->ltSetListButton, 1 );	}
    else{ guiEnableWidget( lt->ltSetListButton, 0 );	}

    guiEnableWidget( lt->ltSelectionFrame, lt->ltCanUpdateSelection );

    guiEnableWidget( lt->ltRemoveFromListButton,
				    lt->ltCanUpdateSelection	&&
				    sd->sdListOverride > 0	);
    guiEnableWidget( lt->ltSetLevelButton,
				    lt->ltCanUpdateSelection	&&
				    sd->sdListOverride > 0	);

    tedListEnableListLevel( lt, lt->ltCanUpdateList );
    guiEnableWidget( lt->ltNewListButton, cmdEnabled[EDITcmdSET_NEW_LIST] );

    tedFormatToolRefreshListPage( lt );

    *pEnabled= sd->sdInDocumentBody;

  ready:

    lt->ltInProgrammaticChange--;

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
    struct EditApplication *		ea= lt->ltApplication;

    const ListOverride *	lo= &(lt->ltListOverrideChosen);

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );

    docInitParagraphProperties( &pp );

    pp.ppListOverride= lo->loIndex;

    tedAppChangeParagraphProperties( ea, &ppSetMask, &pp );

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListSetLevelPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    const DocumentList *	dl= &(lt->ltListPropertiesChosen);
    struct EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    if  ( lt->ltCurrentLevel < 0 || lt->ltCurrentLevel >= dl->dlLevelCount )
	{ LLDEB(lt->ltCurrentLevel,dl->dlLevelCount); return;	}

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    pp.ppListLevel= lt->ltCurrentLevel;

    tedAppChangeParagraphProperties( ea, &ppSetMask, &pp );

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
    struct EditApplication *		ea= lt->ltApplication;

    ParagraphProperties		pp;
    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

    docInitParagraphProperties( &pp );

    tedAppChangeParagraphProperties( ea, &ppSetMask, &pp );

    docCleanParagraphProperties( &pp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatListNewListPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;
    struct EditApplication *		ea= lt->ltApplication;

    tedAppSetNewList( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the list tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatListRevertPushed, w, voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;

    if  ( docCopyDocumentListSameDocument( &(lt->ltListPropertiesChosen),
						&(lt->ltListPropertiesSet) ) )
	{ LDEB(1); return;	}

    if  ( docCopyListOverrideSameDocument( &(lt->ltListOverrideChosen),
						&(lt->ltListOverrideSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

/************************************************************************/
/*									*/
/*  Try to save level attributes on the current list level.		*/
/*									*/
/************************************************************************/

static int tedListToolGetLevelAttributes(	ListTool *	lt,
						ListLevel *	ll )
    {
    int				li;
    int				lipfi;
    int				changed;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    /**/
    lipfi= ll->llLeftIndentTwips+ ll->llFirstIndentTwips;
    if  ( guiLengthFromText( lt->ltFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return -1;	}

    li= ll->llLeftIndentTwips;
    if  ( guiLengthFromText( lt->ltLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					0, 0, maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( ll->llLeftIndentTwips != li )
	{
	ll->llLeftIndentTwips= li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( ll->llFirstIndentTwips != lipfi- li )
	{
	ll->llFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropFIRST_INDENT );
	}


    /**/
    if  ( guiIntegerFromText( lt->ltStartAtText,
				    &(ll->llStartAt), 1, 0, INT_MAX, 0 ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  A level was selected in the list.					*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedListToolLevelChosen, w, voidlt, voidlcs )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);

    int			level;

    if  ( lt->ltInProgrammaticChange )
	{ return;	}

    level= guiGetPositionFromListCallback( w, voidlcs );

    if  ( level == lt->ltCurrentLevel )
	{ return;	}
    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    if  ( lt->ltCurrentLevel >= 0 && lt->ltCurrentLevel < dl->dlLevelCount )
	{
	tedListToolGetLevelAttributes( lt,
				    &(dl->dlLevels[lt->ltCurrentLevel]) );
	}

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
    ListTool *		lt= (ListTool *)voidlt;
    struct EditApplication *	ea= lt->ltApplication;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);

    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    if  ( tedListToolGetLevelAttributes( lt, ll ) )
	{ return;	}

    tedAppChangeCurrentList( ea, dl );

    return;
    }

/************************************************************************/
/*									*/
/*  A number style has been chosen in the menu in the list level	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static void tedListNumberStyleChosen(	int		style,
					void *		voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    if  ( style < 0 || style >= LISTTOOLcountSTYLES )
	{ LLDEB(style,LISTTOOLcountSTYLES); return;	}

    ll->llNumberStyle= TED_ListToolStyles[style];

    tedFormatToolRefreshListPage( lt );

    return;
    }

static void tedListJustifyChosen(	int		justify,
					void *		voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    if  ( justify < 0 || justify >= DOClla_COUNT )
	{ LLDEB(justify,DOClla_COUNT); return;	}

    ll->llJustification= justify;

    if  ( ll->llJustification == DOCllaCENTER		||
	  ll->llJustification == DOCllaRIGHT		)
	{
	int		refresh= 0;

	if  ( ll->llLeftIndentTwips < 20 )
	    {
	    ll->llLeftIndentTwips= lt->ltTabIntervalTwips;
	    PROPmaskADD( &(ll->llParaPropertyMask), PPpropLEFT_INDENT );
	    refresh= 1;
	    }

	if  ( ll->llLeftIndentTwips+ ll->llFirstIndentTwips < 20	&&
	      ll->llLeftIndentTwips- lt->ltTabIntervalTwips/ 2 >= 20	)
	    {
	    ll->llFirstIndentTwips= -lt->ltTabIntervalTwips/ 2;
	    PROPmaskADD( &(ll->llParaPropertyMask), PPpropFIRST_INDENT );
	    refresh= 1;
	    }

	if  ( refresh )
	    { tedListToolRefreshIndents( lt, ll );	}
	}

    return;
    }

static void tedListFollowChosen(	int		follow,
					void *		voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    if  ( follow < 0 || follow >= DOCllf_COUNT )
	{ LLDEB(follow,DOCllf_COUNT); return;	}

    ll->llFollow= follow;

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedLevelStartAtChanged, w, voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    if  ( guiIntegerFromText( lt->ltStartAtText,
				    &(ll->llStartAt), 1, 0, INT_MAX, 0 ) )
	{ return;	}

    tedFormatToolRefreshListPage( lt );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelFirstIndentChanged, w, voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    int				li;
    int				lipfi;
    int				changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    li= ll->llLeftIndentTwips;
    lipfi=  ll->llLeftIndentTwips+ ll->llFirstIndentTwips;

    if  ( tedFormatValidateDimension( &lipfi, &changed,
					    lt->ltFirstIndentText, lipfi ) )
	{ return;	}

    if  ( ll->llLeftIndentTwips != li )
	{
	ll->llLeftIndentTwips= li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( ll->llFirstIndentTwips != lipfi- li )
	{
	ll->llFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropFIRST_INDENT );
	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedLevelLeftIndentChanged, w, voidlt )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;
    ListLevel *		ll;

    int			li;
    int			lipfi;
    int			changed;

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

    ll= &(dl->dlLevels[level]);

    li= ll->llLeftIndentTwips;
    lipfi= ll->llLeftIndentTwips+ ll->llFirstIndentTwips;

    if  ( tedFormatValidateDimension( &li, &changed,
						lt->ltLeftIndentText, li ) )
	{ return;	}

    if  ( ll->llLeftIndentTwips != li )
	{
	ll->llLeftIndentTwips= li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropLEFT_INDENT );
	}
    if  ( ll->llFirstIndentTwips != lipfi- li )
	{
	ll->llFirstIndentTwips= lipfi- li;
	PROPmaskADD( &(ll->llParaPropertyMask), PPpropFIRST_INDENT );
	}

    return;
    }



/************************************************************************/

static APP_EVENT_HANDLER_H( tedListToolNumberKeyPress, w, voidlt, event )
    {
    ListTool *		lt= (ListTool *)voidlt;
    DocumentList *	dl= &(lt->ltListPropertiesChosen);
    int			level= lt->ltCurrentLevel;

    APP_KEY_VALUE	keySym;
    unsigned int	state;

#   if USE_MOTIF
    int		refused= *pRefused;
#   endif
#   if USE_GTK
    int		refused= 1;
#   endif

    if  ( level < 0 || level >= dl->dlLevelCount )
	{ LLDEB(level,dl->dlLevelCount); return;	}

#   if USE_MOTIF
    {
    XKeyPressedEvent *	keyEvent= &(event->xkey);
    char		scratch[40];

    XLookupString( keyEvent, scratch, sizeof(scratch)- 1,
					    &keySym, (XComposeStatus *)0 );
    state= keyEvent->state;
    }
#   endif

#   if USE_GTK
    {
    GdkEventKey *	keyEvent= &(event->key);

    keySym= keyEvent->keyval;
    state= keyEvent->state;

    tedListToolGetSelection( w, lt );
    }
#   endif

    switch( keySym )
	{
	case KEY_KP_Insert:
	case KEY_Insert:
	case KEY_KP_Enter:
	case KEY_Return:
	    tedListLevelRefreshFormatMenu( lt );

#	    if USE_MOTIF
	    {
	    Position	textX= 0;
	    Position	textY= 0;
	    Position	screenX;
	    Position	screenY;

	    int		pos= lt->ltNumberFormatOffsets[lt->ltFormatIndex0];

	    XmTextPosToXY( lt->ltNumberFormatText, pos, &textX, &textY );

	    XtTranslateCoords( lt->ltNumberFormatText, textX, textY,
						    &screenX, &screenY );

	    XtVaSetValues( lt->ltNumberFormatMenu,
				    XmNx,	screenX,
				    XmNy,	screenY,
				    NULL );

	    XtManageChild( lt->ltNumberFormatMenu );

	    refused= 0;
	    }
#	    endif

#	    if USE_GTK

LDEB(1);
	    gtk_menu_popup( GTK_MENU( lt->ltNumberFormatMenu ),
					NULL, NULL, NULL, NULL,
					1, ((GdkEventKey *)event)->time );

	    refused= 0;

#	    endif

	    break;

	case KEY_1:
	case KEY_KP_1:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		tedListToolSetFormatLevel( lt, 0 );

		refused= 0;
		}
	    break;
	case KEY_2:
	case KEY_KP_2:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 1 )
		    { tedListToolSetFormatLevel( lt, 1 );	}

		refused= 0;
		}
	    break;
	case KEY_3:
	case KEY_KP_3:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 2 )
		    { tedListToolSetFormatLevel( lt, 2 );	}

		refused= 0;
		}
	    break;
	case KEY_4:
	case KEY_KP_4:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 3 )
		    { tedListToolSetFormatLevel( lt, 3 );	}

		refused= 0;
		}
	    break;
	case KEY_5:
	case KEY_KP_5:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 4 )
		    { tedListToolSetFormatLevel( lt, 4 );	}

		refused= 0;
		}
	    break;
	case KEY_6:
	case KEY_KP_6:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 5 )
		    { tedListToolSetFormatLevel( lt, 5 );	}

		refused= 0;
		}
	    break;
	case KEY_7:
	case KEY_KP_7:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 6 )
		    { tedListToolSetFormatLevel( lt, 6 );	}

		refused= 0;
		}
	    break;
	case KEY_8:
	case KEY_KP_8:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 7 )
		    { tedListToolSetFormatLevel( lt, 7 );	}

		refused= 0;
		}
	    break;
	case KEY_9:
	case KEY_KP_9:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( level >= 8 )
		    { tedListToolSetFormatLevel( lt, 8 );	}

		refused= 0;
		}
	    break;

	default:
	    break;
	}

#   if USE_MOTIF
    *pRefused= refused;
#   endif

#   if USE_GTK
    if  ( ! refused )
	{
#	if GTK_MAJOR_VERSION >= 2
	g_signal_stop_emission_by_name( lt->ltNumberFormatText,
							"key_press_event" );
#	endif
	}
#   endif

    return;
    }

#   if USE_GTK

static void tedListToolFormatMouseUp(	APP_WIDGET		w,
					APP_EVENT *		event,
					void *			voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;

    if  ( tedListToolGetSelection( w, lt ) )
	{
#	if GTK_MAJOR_VERSION >= 2
	g_signal_stop_emission_by_name( w, "button_release_event" );
#	endif

	guiTextSelectContents( lt->ltNumberFormatText,
				lt->ltFormatOffset0, lt->ltFormatOffset1 );
	}
    }

static void tedListToolFormatMouseMove(	APP_WIDGET		w,
					APP_EVENT *		event,
					void *			voidlt )
    {
    ListTool *			lt= (ListTool *)voidlt;

    if  ( ! ( event->motion.state & GDK_BUTTON1_MASK ) )
	{ return;	}

    if  ( tedListToolGetSelection( w, lt ) )
	{
#	if GTK_MAJOR_VERSION >= 2
	g_signal_stop_emission_by_name( w, "motion_notify_event" );
#	endif

	guiTextSelectContents( lt->ltNumberFormatText,
				lt->ltFormatOffset0, lt->ltFormatOffset1 );
	}
    }

#   endif

/************************************************************************/
/*									*/
/*  Make the tabs page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildListsPage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const ListsPageResources *	lpr= &TED_ListsPageResources;

    APP_WIDGET		startLabel;
    APP_WIDGET		styleLabel;
    APP_WIDGET		justifyLabel;
    APP_WIDGET		followLabel;
    APP_WIDGET		firstLabel;
    APP_WIDGET		leftLabel;

    APP_WIDGET		row;

    const int		visibleItems= 4;
    const int		textColumns= 10;

    ListTool *		lt= malloc( sizeof(ListTool) );

    if  ( ! lt )
	{ XDEB(lt); return lt;	}

    /**/
    lt->ltPageResources= lpr;
    lt->ltInspector= ai;
    lt->ltApplication= ea;

    lt->ltInProgrammaticChange= 0;

    /**/
    docInitDocumentList( &(lt->ltListPropertiesSet) );
    docInitDocumentList( &(lt->ltListPropertiesChosen) );

    docInitListOverride( &(lt->ltListOverrideSet) );
    docInitListOverride( &(lt->ltListOverrideChosen) );

    lt->ltCurrentLevel= -1;
    lt->ltCurrentParagraphNumber= -1;


    /**/
    guiMakeListInColumn( &(lt->ltListLevelList),
		pageWidget, visibleItems,
		tedListToolLevelChosen, (APP_BUTTON_CALLBACK_T)0, (void *)lt );

    /**/
    guiMakeColumnFrameInColumn( &(lt->ltSelectionFrame),
				    &(lt->ltSelectionPaned),
				    pageWidget, lpr->lprSelection );

    guiToolMake2BottonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetListButton), &(lt->ltRemoveFromListButton),
		    lpr->lprSetList, lpr->lprRemoveFromList,
		    tedFormatListSetListPushed,
		    tedFormatListRemoveFromListPushed, lt );

    guiToolMake2BottonRow( &row, lt->ltSelectionPaned,
		    &(lt->ltSetLevelButton), &(lt->ltNewListButton),
		    lpr->lprSetLevel, lpr->lprNewList,
		    tedFormatListSetLevelPushed,
		    tedFormatListNewListPushed, lt );

    /**/
    guiMakeColumnFrameInColumn( &(lt->ltListLevelFrame),
				    &(lt->ltListLevelPaned),
				    pageWidget, lpr->lprListLevel );

    /**/
    guiToolMakeLabelAndMenuRow( &row, &(lt->ltNumberStyleOptionmenu),
				    &styleLabel, lt->ltListLevelPaned,
				    lpr->lprNumberStyle,
				    tedListNumberStyleChosen, (void *)lt );

    /**/
    guiToolMakeLabelAndTextRow( &row, &(lt->ltFormatLabel),
				    &(lt->ltNumberFormatText),
				    lt->ltListLevelPaned,
				    lpr->lprNumberFormat, textColumns, 1 );

# if USE_MOTIF
{
    Arg			al[20];
    int			ac= 0;

    XtAddCallback( lt->ltNumberFormatText,
			XmNmotionVerifyCallback,
			tedListToolFormatSelectionChanged, (void *)lt );

    XtAddCallback( lt->ltNumberFormatText,
			XmNmodifyVerifyCallback,
			tedListToolFormatModified, (void *)lt );

    XtInsertEventHandler( lt->ltNumberFormatText,
			KeyPressMask, False,
			tedListToolNumberKeyPress, (void *)lt, XtListHead );

    ac= 0;
    lt->ltNumberFormatMenu= XmCreatePopupMenu(
					    lt->ltNumberFormatText,
					    WIDGET_NAME,
					    al, ac );

    lt->ltFormatEditable= 0;
}
# endif

# if USE_GTK

#   if GTK_MAJOR_VERSION >= 2
    g_signal_connect_after( G_OBJECT( lt->ltNumberFormatText ),
	    "move_cursor",
	    (GCallback)tedListToolFormatSelectionChanged, (void *)lt );
#   endif

    g_signal_connect( G_OBJECT( lt->ltNumberFormatText ),
	    "insert_text",
	    (GCallback)tedListToolFormatInsertText, (void *)lt );

    g_signal_connect( G_OBJECT( lt->ltNumberFormatText ),
	    "delete_text",
	    (GCallback)tedListToolFormatDeleteText, (void *)lt );

    g_signal_connect_after( G_OBJECT( lt->ltNumberFormatText ),
	    "key_press_event",
	    (GCallback)tedListToolNumberKeyPress, (void *)lt );

    g_signal_connect_after( G_OBJECT( lt->ltNumberFormatText ),
	    "button_release_event",
	    (GCallback)tedListToolFormatMouseUp, (void *)lt );

    g_signal_connect_after( G_OBJECT( lt->ltNumberFormatText ),
	    "motion_notify_event",
	    (GCallback)tedListToolFormatMouseMove, (void *)lt );

    lt->ltNumberFormatMenu= gtk_menu_new();

# endif

    /**/
    guiToolMakeLabelAndTextRow( &row, &firstLabel, &(lt->ltFirstIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelFirstIndent, textColumns, 1 );

    guiSetGotValueCallbackForText( lt->ltFirstIndentText,
				    tedLevelFirstIndentChanged, (void *)lt );
    /**/
    guiToolMakeLabelAndTextRow( &row, &leftLabel, &(lt->ltLeftIndentText),
				    lt->ltListLevelPaned,
				    lpr->lprLevelLeftIndent, textColumns, 1 );

    guiSetGotValueCallbackForText( lt->ltLeftIndentText,
				    tedLevelLeftIndentChanged, (void *)lt );

    /**/
    guiToolMakeLabelAndTextRow( &row, &startLabel, &(lt->ltStartAtText),
				    lt->ltListLevelPaned,
				    lpr->lprStartAt, textColumns, 1 );

    guiSetGotValueCallbackForText( lt->ltStartAtText,
					tedLevelStartAtChanged, (void *)lt );
    /**/
    guiToolMakeLabelAndMenuRow( &row, &(lt->ltJustifyOptionmenu),
				    &justifyLabel, lt->ltListLevelPaned,
				    lpr->lprJustify,
				    tedListJustifyChosen, (void *)lt );

    guiToolMakeLabelAndMenuRow( &row, &(lt->ltFollowOptionmenu),
				    &followLabel, lt->ltListLevelPaned,
				    lpr->lprFollowedBy,
				    tedListFollowChosen, (void *)lt );

    return (void *)lt;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

static void tedFormatFillListChoosers(	void *		vlt )
    {
    ListTool *			lt= (ListTool *)vlt;
    const ListsPageResources *	lpr= &TED_ListsPageResources;

    appFillInspectorMenu( LISTTOOLcountSTYLES, 0,
		    lpr->lprStyleOptionTexts, &(lt->ltNumberStyleOptionmenu) );

    appFillInspectorMenu( DOClla_COUNT, DOCllaLEFT,
		    lpr->lprJustifyOptionTexts, &(lt->ltJustifyOptionmenu) );

    appFillInspectorMenu( DOCllf_COUNT, DOCllfTAB,
		    lpr->lprFollowOptionTexts, &(lt->ltFollowOptionmenu) );

    return;
    }

static void tedFormatFinishListPage(	void *	vlt )
    {
    ListTool *	lt= (ListTool *)vlt;

    guiOptionmenuRefreshWidth( &(lt->ltNumberStyleOptionmenu) );
    guiOptionmenuRefreshWidth( &(lt->ltJustifyOptionmenu) );
    guiOptionmenuRefreshWidth( &(lt->ltFollowOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a tabs tool.				*/
/*									*/
/************************************************************************/

static void tedFormatFreeListsTool(	void *	vlt )
    {
    ListTool *	lt= (ListTool *)vlt;

    docCleanDocumentList( &(lt->ltListPropertiesSet) );
    docCleanDocumentList( &(lt->ltListPropertiesChosen) );
    
    docCleanListOverride( &(lt->ltListOverrideSet) );
    docCleanListOverride( &(lt->ltListOverrideChosen) );

    free( lt );
    
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
	"Bullets and Numbering" ),
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
	"Level %d" ),

    APP_RESOURCE( "listToolLevelFirstIndent",
	offsetof(ListsPageResources,lprLevelFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "listToolLevelLeftIndent",
	offsetof(ListsPageResources,lprLevelLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "listToolListLevelStartAt",
	offsetof(ListsPageResources,lprStartAt),
	"First Number" ),
    APP_RESOURCE( "listToolListLevelNoRestart",
	offsetof(ListsPageResources,lprNoRestart),
	"Continuous" ),

    /**/
    APP_RESOURCE( "listToolLevelNumberStyle",
	offsetof(ListsPageResources,lprNumberStyle),
	"Style" ),
    APP_RESOURCE( "listToolLevelNumberFormat",
	offsetof(ListsPageResources,lprNumberFormat),
	"Format" ),

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
	    offsetof(ListsPageResources,lprJustifyOptionTexts[DOCllaCENTER]),
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

    /**/
    APP_RESOURCE( "listToolDeleteLevelNumber",
	    offsetof(ListsPageResources,lprDeleteLevelNumber),
	    "Delete" ),
    APP_RESOURCE( "listToolInsertLevelNumber",
	    offsetof(ListsPageResources,lprInsertLevelNumber),
	    "Insert" ),
    APP_RESOURCE( "listToolEditLevelText",
	    offsetof(ListsPageResources,lprEditLevelText),
	    "Edit" ),
    };

static void tedFormatToolGetListsResourceTable(
					struct EditApplication *		ea,
					ListsPageResources *		lpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)lpr,
				TED_TedListToolResourceTable,
				sizeof(TED_TedListToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedListSubjectResourceTable,
				sizeof(TED_TedListSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }


int tedListToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFormatToolGetListsResourceTable( ea,
				&TED_ListsPageResources,
				&TED_ListsSubjectResources );

    ist->istResources= &TED_ListsSubjectResources;

    ist->istBuildTool= tedFormatBuildListsPage;
    ist->istFillChoosers= tedFormatFillListChoosers;
    ist->istFinishPage= tedFormatFinishListPage;
    ftst->ftstRefreshPage= tedRefreshListTool;
    ist->istFreeTool= tedFormatFreeListsTool;
    /*fts->ftsToolResetCache*/

    ist->istRevertChanges= tedFormatListRevertPushed;
    ist->istApplyChanges= tedFormatChangeList;

    return 0;
    }
