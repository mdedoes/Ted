/************************************************************************/
/*									*/
/*  Ted: Management of the selection and the current position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedRuler.h"
#   include	"tedLayout.h"
#   include	"tedSelect.h"
#   include	<tedDocFront.h>
#   include	<tedToolFront.h>
#   include	"tedDocument.h"
#   include	<docParaParticules.h>
#   include	<docEditCommand.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<docBuf.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Is the selection a position between two chars?			*/
/*									*/
/************************************************************************/

int tedHasIBarSelection(	const EditDocument *  ed )
    {
    const TedDocument *	td= (const TedDocument *)ed->edPrivateData;

    return docIsIBarSelection( &(td->tdSelection) );
    }

int tedHasSelection(	const EditDocument *	ed )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    const DocumentSelection *	ds= &(td->tdSelection);

    return docSelectionIsSet( ds );
    }

int tedGetSelection(	DocumentSelection *		ds,
			SelectionGeometry *		sg,
			SelectionDescription *		sd,
			struct DocumentTree **		pTree,
			struct BufferItem **		pBodySectNode,
			const EditDocument *		ed )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    const DocumentSelection *	dss= &(td->tdSelection);

    struct BufferItem *		bodySectNode= (struct BufferItem *)0;
    struct DocumentTree *	tree= (struct DocumentTree *)0;

    if  ( ! docSelectionIsSet( dss ) )
	{ return 1;	}

    if  ( pTree || pBodySectNode )
	{
	if  ( docGetTreeOfNode( &tree, &bodySectNode, bd, dss->dsHead.dpNode ) )
	    { LDEB(1); return -1;	}
	}

    *ds= td->tdSelection;
    *sg= td->tdSelectionGeometry;
    *sd= td->tdSelectionDescription;
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode;	}
    if  ( pTree )
	{ *pTree= tree;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the derived properties such as line number and		*/
/*  coordinates of the current selection.				*/
/*									*/
/************************************************************************/

void tedDelimitCurrentSelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    struct BufferItem *		bodySectNode;

    int				lastLine;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
			    (struct DocumentTree **)0, &bodySectNode, ed ) )
	{ LDEB(1); return;	}

    lastLine= sdDoc.sdIsIBarSelection &&
	( td->tdSelectionGeometry.sgHead.pgPositionFlags & POSflagLINE_HEAD );

    docSelectionGeometry( &(td->tdSelectionGeometry), &dsDoc, bodySectNode,
							    lastLine, &lc );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt tools and rulers to the current position.			*/
/*									*/
/************************************************************************/

static void tedAdaptOptions(	TedDocument *			td,
				const unsigned char *		cmdEnabled,
				const SelectionDescription *	sd )
    {
    const EditTrace *	et= &(td->tdEditTrace);

    /* modifications */

    guiEnableWidget( td->tdInsHyperlinkOption,
				    cmdEnabled[EDITcmdSET_HYPERLINK] );

    guiEnableWidget( td->tdInsBookmarkOption,
				    cmdEnabled[EDITcmdSET_BOOKMARK] );

    guiEnableWidget( td->tdCutOption, cmdEnabled[EDITcmdDELETE_SELECTION] );

    guiEnableWidget( td->tdPasteOption, cmdEnabled[EDITcmdREPLACE] );

    guiEnableWidget( td->tdInsPictOption, cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsFileOption, cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsSymbolOption, cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdToolsSymbolOption, cmdEnabled[EDITcmdREPLACE] );

    guiEnableWidget( td->tdFormatOneParaOption,
					cmdEnabled[EDITcmdMERGE_PARAS] );

    guiEnableWidget( td->tdInsInsertFootnoteOption,
					cmdEnabled[EDITcmdINS_NOTE] );

    guiEnableWidget( td->tdInsInsertEndnoteOption,
					cmdEnabled[EDITcmdINS_NOTE] );

    guiEnableWidget( td->tdInsInsertChftnsepOption,
				    cmdEnabled[EDITcmdREPLACE_FTNSEP] );
    guiEnableWidget( td->tdInsInsertTableOption,
				    cmdEnabled[EDITcmdINSERT_TABLE] );
    guiEnableWidget( td->tdInsInsertPageNumberOption, 
				    cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsInsertLineBreakOption,
				    cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsInsertPageBreakOption,
				    cmdEnabled[EDITcmdREPLACE_BODY_LEVEL] );
    guiEnableWidget( td->tdInsInsertColumnBreakOption,
				    cmdEnabled[EDITcmdREPLACE_BODY_LEVEL] &&
				    sd->sdHeadInMultiColumnSection );
    guiEnableWidget( td->tdInsInsertSectBreakOption,
				    cmdEnabled[EDITcmdSPLIT_SECT] );

    guiEnableWidget( td->tdTabInsertTableOption,
				    cmdEnabled[EDITcmdINSERT_TABLE] );
    guiEnableWidget( td->tdTabAddRowOption,
				    cmdEnabled[EDITcmdAPPEND_ROW] );
    guiEnableWidget( td->tdTabAddColumnOption,
				    cmdEnabled[EDITcmdINSERT_COLUMN] );
    guiEnableWidget( td->tdTabDeleteTableOption,
				    cmdEnabled[EDITcmdDELETE_TABLE] );
    guiEnableWidget( td->tdTabDeleteRowOption,
				    cmdEnabled[EDITcmdDELETE_ROW] );
    guiEnableWidget( td->tdTabDeleteColumnOption,
				    cmdEnabled[EDITcmdDELETE_COLUMN] );

    guiEnableWidget( td->tdFontBoldOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontItalicOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontUnderlinedOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontSuperscriptOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontSubscriptOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontPasteOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );

    guiEnableWidget( td->tdFormatPasteRulerOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatIncreaseIndentOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatDecreaseIndentOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignLeftOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignRightOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignCenterOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignJustifyOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );

    {
    int			n;
    const int		direction= -1;

    n= docEditGetTraceStep( (const TraceStep **)0, (int *)0,
						direction, et, et->etIndex );

    guiEnableWidget( td->tdUndoOption, n >= 0 );
    }

    {
    const TraceStep *	ts= (const TraceStep *)0;
    int			n;
    const int		direction= 1;
    int			isRepeat;

    n= docEditGetTraceStep( &ts, &isRepeat, direction, et, et->etIndex );
    if  ( n < 0 )
	{ guiEnableWidget( td->tdRepeatOption, 0 );	}
    else{
	if  ( ! isRepeat )
	    { guiEnableWidget( td->tdRepeatOption, 1 );	}
	else{
	    int	cmd= ts->tsCommand;

	    if  ( cmd == EDITcmdEXTEND_REPLACE )
		{ cmd= EDITcmdREPLACE;	}
	    if  ( cmd < 0 || cmd >= EDITcmd_COUNT )
		{ LDEB(cmd);  guiEnableWidget( td->tdRepeatOption, 0 );	}
	    else{
		if  ( cmd == EDITcmdDEL_FIELD		||
		      cmd == EDITcmdUPD_FIELD		)
		    {
		    guiEnableWidget( td->tdRepeatOption,
				cmdEnabled[cmd]				&&
				sd->sdHeadFieldKind == ts->tsFieldKind	);
		    }
		else{
		    guiEnableWidget( td->tdRepeatOption, cmdEnabled[cmd] );
		    }
		}
	    }
	}
    }

    /* read-only */

    guiEnableWidget( td->tdCopyOption, ! sd->sdIsIBarSelection );

    guiEnableWidget( td->tdTabSelectTableOption, sd->sdHeadInTable );
    guiEnableWidget( td->tdTabSelectRowOption, sd->sdHeadInTable );
    guiEnableWidget( td->tdTabSelectColumnOption, sd->sdHeadInTable );

    return;
    }

void tedAdaptToolsToSelection(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    struct DocumentTree *	tree;
    struct BufferItem *		bodySectNode;

    unsigned char		cmdEnabled[EDITcmd_COUNT];

    if  ( tedGetSelection( &ds, &sg, &sd, &tree, &bodySectNode, ed ) )
	{ LDEB(1); return;	}

    docEnableEditCommands( cmdEnabled, &sd );

    tedDocAdaptTopRuler( ed, &ds, &sg, &sd, bodySectNode );

    tedSetBottomRulerValues( ed->edBottomRuler, ed->edBottomRulerWidget,
			sd.sdTailPage,
			sd.sdDocumentPages,
			sd.sdTailSection,
			sd.sdDocumentSections );

    tedAdaptFontIndicatorsToSelection( ed );

    tedAdaptOptions( td, cmdEnabled, &(td->tdSelectionDescription) );

    if  ( ea->eaInspector )
	{
	const int	choosePage= 0;

	tedAppRefreshFormatTool( ea->eaInspector, choosePage,
					    td->tdDocument, tree,
					    &(td->tdSelection),
					    &(td->tdSelectionGeometry),
					    &(td->tdSelectionDescription),
					    cmdEnabled );
	}

    if  ( ed->edInspector )
	{
	tedDocRefreshFormatTool( ed, ed->edInspector,
					    td->tdDocument, tree,
					    &(td->tdSelection),
					    &(td->tdSelectionGeometry),
					    &(td->tdSelectionDescription),
					    cmdEnabled );
	}
    }

/************************************************************************/
/*									*/
/*  Adapt format tool to the circumstances.				*/
/*									*/
/************************************************************************/

void tedAdaptFormatToolToDocument(	EditDocument *	ed,
					int		choosePage )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    struct DocumentTree *	tree;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    unsigned char		cmdEnabled[EDITcmd_COUNT];

    if  ( ! ea->eaInspector && ! ed->edInspector )
	{ return;	}

    if  ( tedGetSelection( &ds, &sg, &sd, &tree, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    docEnableEditCommands( cmdEnabled, &sd );

    if  ( ea->eaInspector )
	{
	tedAppRefreshFormatTool( ea->eaInspector, choosePage,
			    td->tdDocument, tree, &ds, &sg, &sd, cmdEnabled );
	}

    if  ( ed->edInspector )
	{
	tedDocRefreshFormatTool( ed, ed->edInspector,
					    td->tdDocument, tree,
					    &(td->tdSelection),
					    &(td->tdSelectionGeometry),
					    &(td->tdSelectionDescription),
					    cmdEnabled );
	}

    return;
    }

