/************************************************************************/
/*									*/
/*  API for the document. (Editing and navigation.)			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedApp.h"
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedSelect.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedDocument.h"
#   include	"tedAppResources.h"
#   include	<tedToolFront.h>
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	<guiDrawingWidget.h>
#   include	<docEditCommand.h>
#   include	<docCollectStatistics.h>
#   include	<docFieldKind.h>
#   include	<docExpandedTextAttribute.h>
#   include	<docPropVal.h>
#   include	<docBreakKind.h>
#   include	<docStatistics.h>
#   include	<docDocumentProperties.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<appGuiDocument.h>
#   include	<docBuf.h>
#   include	<docParaProperties.h>
#   include	<appDocFront.h>
#   include	<docFind.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a replace from one of the tools.				*/
/*  (Used from the Find and Spell tools)				*/
/*									*/
/************************************************************************/

int tedDocReplace(	EditDocument *		ed,
			const char *		word )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    if  ( ! word || ! word[0] )
	{ SDEB(word); return -1;	}

    if  ( tedHasSelection( ed )				&&
	  ! tedHasIBarSelection( ed )			&&
	  td->tdSelectionDescription.sdCanReplace	)
	{
	tedDocReplaceSelection( ed, EDITcmdREPLACE, word, strlen( word ),
							    td->tdTraced );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  'Select All' from the 'Edit' menu.					*/
/*									*/
/************************************************************************/

void tedDocSelAll(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &dsNew, &sg, &sd,
		(struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ docInitDocumentSelection( &dsNew );	}

    if  ( docSelectWholeBody( &dsNew, bd ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Navigation over columns.						*/
/*									*/
/************************************************************************/

void tedDocSelectWholeCell(	EditDocument *	ed,
				int		direction,
				int		allRows )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
	    (struct DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectWholeCell( &ds, direction, allRows ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

int tedDocSelectColumn(		EditDocument *	ed,
				int		col )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
	    (struct DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return -1;	}

    if  ( docSelectColumn( &ds, col ) )
	{ LDEB(col); return -1;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Navigation over rows.						*/
/*									*/
/************************************************************************/

void tedDocSelectRow(		EditDocument *	ed,
				int		direction,
				int		allColumns )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
		(struct DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectRow( &ds, direction, allColumns ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Select table.							*/
/*									*/
/************************************************************************/

void tedDocSelectTable(	EditDocument *		ed )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
		(struct DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectWholeTable( &ds ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

int tedDocShiftRowsInTable(	EditDocument *		ed,
				int			direction )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    return tedEditShiftRowsInTable( ed, direction, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Callback from 'Frame' tool.						*/
/*									*/
/************************************************************************/

int tedDocSelectCurrentFrame(	EditDocument *	ed )
    {
    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &dsNew, &sg, &sd,
		(struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ docInitDocumentSelection( &dsNew );	}

    if  ( docSelectFrameOfPosition( &dsNew, &(dsNew.dsHead) ) )
	{ LDEB(1); return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return 0;
    }

APP_MENU_CALLBACK_H( tedDocInsertFootnote, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedDocInsertNote( (EditDocument *)voided, DOCinFOOTNOTE, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocInsertEndnote, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedDocInsertNote( (EditDocument *)voided, DOCinENDNOTE, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableSelectTable, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocSelectTable( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectRow, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    const int			direction= 0;

    const int			allColumns= 1;

    tedDocSelectRow( ed, direction, allColumns );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectColumn, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    const int			direction= 0;

    const int			allRows= 1;

    tedDocSelectWholeCell( ed, direction, allRows );

    return;
    }

APP_MENU_CALLBACK_H( tedDocEditUndo, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocUndo( ed );
    }

APP_MENU_CALLBACK_H( tedDocEditRepeat, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocRepeat( ed );
    }

/************************************************************************/
/*									*/
/*  Callback for the 'DrawTableGrid' toggle.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocTableDrawGrid, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				set;

    set= guiGetMenuToggleStateFromCallback( option, e );

    if  ( set )
	{ td->tdDrawTableGrid=  1;	}
    else{ td->tdDrawTableGrid= -1;	}

    tedExposeWholeDocument( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteTable, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteTable( ed, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteRow, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteRows( ed, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteColumns( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Insert a line/page/column break in the document.			*/
/*  Insert a note separator.						*/
/*									*/
/*  Note that page breaks and column breaks at the head of a paragraph	*/
/*  are modified into a paragraph property by the editing code.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertLineBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed,
			TPkindLINEBREAK, EDITcmdREPLACE,
			redoLayout, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertPageBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed,
		    TPkindPAGEBREAK, EDITcmdREPLACE_BODY_LEVEL,
		    redoLayout, td->tdTraced );
    */

    tedDocSplitParagraph( ed, DOCibkPAGE,
				EDITcmdREPLACE_BODY_LEVEL, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertColumnBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocSplitParagraph( ed, DOCibkCOL,
				EDITcmdREPLACE_BODY_LEVEL, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertChftnsep, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed,
			TPkindCHFTNSEP, EDITcmdREPLACE_FTNSEP,
			redoLayout, td->tdTraced );

    return;
    }

/************************************************************************/
/*									*/
/*  Menu options to change paragraph alignment.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatAlignLeft, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedDocChangeParagraphPropertyValue( ed, PPpropALIGNMENT, DOCthaLEFT );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignRight, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedDocChangeParagraphPropertyValue( ed, PPpropALIGNMENT, DOCthaRIGHT );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignCenter, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedDocChangeParagraphPropertyValue( ed, PPpropALIGNMENT, DOCthaCENTERED );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignJustify, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedDocChangeParagraphPropertyValue( ed, PPpropALIGNMENT, DOCthaJUSTIFIED );
    }

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatIncreaseIndent, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		direction= 1;

    tedFormatShiftIndent( ed, direction, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocFormatDecreaseIndent, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		direction= -1;

    tedFormatShiftIndent( ed, direction, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Replace the current selection with a page number field.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertPageNumber, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedInsertPageNumber( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocTableInsertTable, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    int			rows= 2;
    int			columns= 2;

    if  ( tedInsertTable( ed, rows, columns, td->tdTraced ) )
	{ LLDEB(rows,columns);	}

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableAddRow, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		after= 1;

    tedDocAddRowToTable( ed, after, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableAddColumn, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		after= 1;

    tedDocAddColumnToTable( ed, after, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Font Menu.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontPaste, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "RTFFONT" );
    }

/************************************************************************/
/*									*/
/*  Insert a section break.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocMenuInsertSectBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocInsertSectBreak( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Toggle text attributes.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontBold, option, voided, e )
    {
    int	set= guiGetMenuToggleStateFromCallback( option, e );

    tedDocChangeTextAttributeValue( (EditDocument *)voided,
					    TApropFONTBOLD, set != 0 );
    }

APP_MENU_CALLBACK_H( tedDocFontItalic, option, voided, e )
    {
    int	set= guiGetMenuToggleStateFromCallback( option, e );

    tedDocChangeTextAttributeValue( (EditDocument *)voided,
					    TApropFONTSLANTED, set != 0 );
    }

APP_MENU_CALLBACK_H( tedDocFontUnderlined, option, voided, e )
    {
    int	set= guiGetMenuToggleStateFromCallback( option, e );

    tedDocChangeTextAttributeValue( (EditDocument *)voided,
					    TApropTEXTUNDERLINED, set != 0 );
    }

/************************************************************************/
/*									*/
/*  Change the base line of the current selection.			*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontSupersub, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    int			set;
    int			supersub= TEXTvaREGULAR;

    set= guiGetMenuToggleStateFromCallback( option, e );
    if  ( set )
	{
	if  ( option == td->tdFontSubscriptOption )
	    { supersub= TEXTvaSUBSCRIPT;	}

	if  ( option == td->tdFontSuperscriptOption )
	    { supersub= TEXTvaSUPERSCRIPT;	}
	}
    else{ supersub= TEXTvaREGULAR;	}

    tedDocChangeTextAttributeValue( ed, TApropSUPERSUB, supersub );
    }

/************************************************************************/
/*									*/
/*  Merge the paragraphs in the selection.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatOnePara, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedMergeParagraphsInSelection( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Show or create a find tool.						*/
/*									*/
/************************************************************************/

static void tedDocShowFindTool(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    if  ( ed->edToolbar )
	{
	tedDocBuildFormatTool( ed );

	tedDocToolShowFindTool( ed,
				&(td->tdSelectionDescription),
				&(td->tdSelection),
				td->tdDocument );

	tedAdaptFormatToolToDocument( ed, 0 );
	}
    else{
	/*  To get the correct title  */
	tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );
	tedAdaptFormatToolToDocument( ed, 0 );

	tedAppToolShowFindTool( ea,
				&(td->tdSelectionDescription),
				&(td->tdSelection),
				td->tdDocument );
	}

    return;
    }

APP_MENU_CALLBACK_H( tedDocToolFind, findOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedDocShowFindTool( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocToolFindNext, findOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td;

    if  ( ! ed )
	{ XDEB(ed); return;	}
    td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdFindProg )
	{
	tedDocShowFindTool( ed );
	return;
	}

    tedDocFindNext( ed );

    return;
    }

int tedDocFindToolSetPattern(	void *		voided,
				const char *	pattern,
				int		useRegex,
				int		asWord,
				int		caseSensitive )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tar->tarFindPattern= (char *)0;

    if  ( docFindSetPattern( &(td->tdFindProg),
				pattern, useRegex, asWord, caseSensitive ) )
	{ SDEB(pattern); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Show or create a spelling tool.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocToolSpell, spellOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowSpellPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Custom 'Paste' functionality.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocEditPaste, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;
    TedDocument *			td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); return;	}

    /*  1  */
    appDocAskForPaste( ed, "CLIPBOARD" );

    return;
    }

APP_MENU_CALLBACK_H( tedDocFormatPasteRul, fontsOption, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "RTFRULER" );
    }

APP_MENU_CALLBACK_H( tedDocFontCopy, fontsOption, voided, e )
    {
    tedCopyFont( (EditDocument *)voided );
    }

APP_MENU_CALLBACK_H( tedDocFormatCopyRul, fontsOption, voided, e )
    {
    tedCopyRuler( (EditDocument *)voided );
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the font tool was pushed.			*/
/*									*/
/************************************************************************/

int tedDocFontToolSet(		EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    DocumentProperties *	dp= bd->bdProperties;
    struct DocumentFontList *	dfl= dp->dpFontList;

    PropertyMask		doneMask;
    TextAttribute		taSet;


    textInitTextAttribute( &taSet );
    utilPropMaskClear( &doneMask );

    docIndirectTextAttribute( &doneMask, &taSet, etaSet, taSetMask,
						dfl, dp->dpColorPalette );

    tedDocChangeTextAttribute( ed, taSetMask, &taSet, td->tdTraced );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Run the 'Link Dialog' from the 'Insert Link' menu option.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertLink, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditApplication *		ea= ed->edApplication;

    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );

    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowLinkPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Run the 'Bookmark Dialog' from the 'Insert Bookmark' menu option.	*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertBookmark, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditApplication *		ea= ed->edApplication;

    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );

    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowBookmarkPage( ea );
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocShowPageTool, pageOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowPagePage( ea );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  Font Menu.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocToolFont, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowFontPage( ea );

    return;
    }

APP_MENU_CALLBACK_H( tedDocToolInsertSymbol, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedAppShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedAppFormatShowSymbolPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Show the format tool.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatTool, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    /* option == td->tdToolsFormatToolOption */

    tedAppShowFormatTool( option, ea );
    tedAdaptFormatToolToDocument( ed, 0 );
    }

/************************************************************************/
/*									*/
/*  Move the rezize handles of the image that is sellected as part of	*/
/*  the scrollbar callback.						*/
/*									*/
/************************************************************************/

APP_SCROLLBAR_CALLBACK_H( tedDocHorizontalScrollbarCallback, w, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				x= guiGetScrollbarValueFromCallback( w, e );

    appDocScrollToX( ed, x );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    return;
    }

APP_SCROLLBAR_CALLBACK_H( tedDocVerticalScrollbarCallback, w, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				y= guiGetScrollbarValueFromCallback( w, e );

    appDocScrollToY( ed, y );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( tedDocObserveFocus, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    int			inout= 0;

    if  ( guiDrawGetInoutFromFocusEvent( &inout, w, event ) )
	{ return;	}

    tedObserveFocus( ed, inout );
    }

/************************************************************************/
/*									*/
/*  Run document properties dialog.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFileProps, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentStatistics		ds;

    docCollectDocumentStatistics( &ds, bd );

    tedRunPropertyDialog( ed, bd->bdProperties, &ds, option );
    }


/************************************************************************/
/*									*/
/*  Delete the table of contents					*/
/*									*/
/************************************************************************/

void tedDocDeleteTocField(	EditDocument *		ed,
				int			traced )
    {
    tedDocFlattenTypedField( ed, DOCfkTOC,
		(const PropertyMask *)0, (const TextAttribute *)0, traced );
    }

/************************************************************************/
/*									*/
/*  Unlock/Recover a document.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFileUnlock, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocUnlock( ed );
    }

APP_MENU_CALLBACK_H( tedDocFileRecover, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocRecover( ed );
    }

/************************************************************************/
/*									*/
/*  Display the online manual.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocManual, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    tedManual( option, ea, ed->edToplevel.atTopWidget );
    }
