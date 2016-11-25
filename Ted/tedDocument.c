#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<sioStdio.h>
#   include	<appSystem.h>

#   include	<appSpellTool.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	"docLayout.h"
#   include	"docEdit.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF
#	include	<X11/cursorfont.h>
#   endif

/************************************************************************/
/*									*/
/*  Ted, callbacks for the DrawingArea.					*/
/*									*/
/************************************************************************/

void tedDrawRectangle(	APP_WIDGET		w,
			EditDocument *		ed,
			DocumentRectangle *	drClip,
			int			ox,
			int			oy )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);
    AppColors *			ac= &(ed->edColors);

    /*  2,3,4  */
    tedRedrawRectangle( w, td, drClip, add, ac, ox, oy );
    }

/************************************************************************/
/*									*/
/*  Scrolling callbacks.						*/
/*									*/
/************************************************************************/

void tedMoveObjectWindows(	EditDocument *		ed )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    InsertedObject *		io;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentPosition		dp;
    PositionGeometry		pg;

    docInitDocumentPosition( &dp );

    if  ( tedGetObjectSelection( td, &dp, &io ) )
	{ LDEB(1); return;	}

    tedPositionGeometry( &pg, &dp, bd, add );

    tedSetObjectWindows( ed, &pg, io, ox, oy );
    }

void tedDocHorizontalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    appDocHorizontalScrollbarCallback( w, voided, voidscbs );

    if  ( td->tdObjectSelected )
	{ tedMoveObjectWindows( ed );	}
    }

void tedDocVerticalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    appDocVerticalScrollbarCallback( w, voided, voidscbs );

    if  ( td->tdObjectSelected )
	{ tedMoveObjectWindows( ed );	}
    }

/************************************************************************/
/*									*/
/*  Last phase in building a document window.				*/
/*									*/
/*  1)  Set the position by hand, because the regular routine sends an	*/
/*	expose to a window that does not exist yet.			*/
/*									*/
/************************************************************************/

int tedFinishDocumentSetup(	EditApplication *	ea,
				EditDocument *		ed )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    AppColors *			ac= &(ed->edColors);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    const TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;
    const char *		selColorName= tar->tarSelectionColor;
    const char *		xselColorName= tar->tarCopiedSelectionColor;
    /*
    char *			tableColorName= "gray80";
    */

    {
    DocumentPosition	dp;

    /*  1  */
    if  ( ! docFirstDocumentPosition( &dp, bd ) )
	{
	const TextParticule *		tp;

	docSetIBarSelection( &(td->tdDocumentSelection), &dp );

	tedSelectionGeometry( &(td->tdSelectionGeometry),
				     &(td->tdDocumentSelection), bd, add );

	tp= dp.dpBi->biParaParticules+ dp.dpParticule;

	utilGetTextAttributeByNumber( &(td->tdCurrentTextAttribute),
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
	td->tdCurrentTextAttributeNumber= tp->tpTextAttributeNumber;

	td->tdCurrentPhysicalFont= tp->tpPhysicalFont;
	}
    else{ docListItem( 0, &(bd->bdItem) );	}

    /*
    LDEB(1);
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort();	}
    */

    }

    if  ( appFinishDocumentSetup( ea, ed ) )
	{ LDEB(1); return -1;	}

    if  ( appAllocateColors( add, ac ) )
	{ LDEB(1); return -1;	}

    if  ( ed->edColors.acAllocator.caDepth < 4 )
	{
	if  ( appColorRgb( &(td->tdFieldColor), &(ed->edColors), 0, 0, 0 ) )
	    { LDEB(1); return -1;	}

	td->tdSelColor.rgb8Red= 0;
	td->tdSelColor.rgb8Green= 0;
	td->tdSelColor.rgb8Blue= 0;

	td->tdCopiedSelColor.rgb8Red= 0;
	td->tdCopiedSelColor.rgb8Green= 0;
	td->tdCopiedSelColor.rgb8Blue= 0;

	td->tdTableColor.rgb8Red= 0;
	td->tdTableColor.rgb8Green= 0;
	td->tdTableColor.rgb8Blue= 0;
	}
    else{
	APP_COLOR_RGB	acSel;
	APP_COLOR_RGB	acCopiedSel;

	if  ( appColorNamed( &acSel, &ed->edColors, selColorName ) )
	    {
	    if  ( appColorFindRgb( &acSel,
					   &(ed->edColors), 176, 196, 222 ) )
		{ SDEB(selColorName); return -1;	}
	    }

	if  ( appColorNamed( &acCopiedSel,
					    &(ed->edColors), xselColorName ) )
	    {
	    if  ( appColorFindRgb( &acCopiedSel,
					    &(ed->edColors), 176, 176, 176 ) )
		{ SDEB(xselColorName); return -1;	}
	    }

	if  ( appColorRgb( &(td->tdFieldColor), &ed->edColors, 0, 0, 200 ) )
	    {
	    if  ( appColorFindRgb( &(td->tdFieldColor),
					       &(ed->edColors), 0, 0, 255 ) )
		{ LDEB(1); return -1;	}
	    }

#	ifdef USE_MOTIF
	td->tdSelColor.rgb8Red= acSel.red/ 256;
	td->tdSelColor.rgb8Green= acSel.green/ 256;
	td->tdSelColor.rgb8Blue= acSel.blue/ 256;

	td->tdCopiedSelColor.rgb8Red= acCopiedSel.red/ 256;
	td->tdCopiedSelColor.rgb8Green= acCopiedSel.green/ 256;
	td->tdCopiedSelColor.rgb8Blue= acCopiedSel.blue/ 256;

	td->tdTableColor.rgb8Red= add->addBackColor.red/ 256;
	td->tdTableColor.rgb8Green= add->addBackColor.green/ 256;
	td->tdTableColor.rgb8Blue= add->addBackColor.blue/ 256;
#	endif

#	ifdef USE_GTK
	td->tdSelColor.rgb8Red= acSel.red/ 256;
	td->tdSelColor.rgb8Green= acSel.green/ 256;
	td->tdSelColor.rgb8Blue= acSel.blue/ 256;

	td->tdCopiedSelColor.rgb8Red= acCopiedSel.red/ 256;
	td->tdCopiedSelColor.rgb8Green= acCopiedSel.green/ 256;
	td->tdCopiedSelColor.rgb8Blue= acCopiedSel.blue/ 256;

	td->tdTableColor.rgb8Red= add->addBackColor.red/ 256;
	td->tdTableColor.rgb8Green= add->addBackColor.green/ 256;
	td->tdTableColor.rgb8Blue= add->addBackColor.blue/ 256;
#	endif
	}

    if  ( tedOpenItemObjects( &(bd->bdItem), &(ed->edColors),
						    &(ed->edDrawingData) ) )
	{ LDEB(1);	}

    td->tdDrawTableGrid= tar->tarShowTableGridInt;

    appGuiSetToggleItemState( td->tdDrawTableGridOption,
						td->tdDrawTableGrid >= 0 );

    if  ( ed->edIsReadonly )
	{ tedMakeDocumentReadonly( ed );	}

    if  ( tedHasSelection( td ) )
	{ tedAdaptToolsToSelection( ed );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the document widget and the rulers.				*/
/*									*/
/*  1)  Calculate the different width of the margins.			*/
/*  2)  Insert a Form in the scrolled window.				*/
/*  3)  Insert the top ruler in the form.				*/
/*  4)  Insert the bottom ruler in the form.				*/
/*  5)  Insert the left ruler in the form.				*/
/*  6)  Insert the right ruler in the form.				*/
/*  7)  Insert the document widget in the form.				*/
/*									*/
/************************************************************************/

void tedFreeDocument(		void *			voidtd,
				int			format,
				AppDrawingData *	add )
    {
    TedDocument *	td= (TedDocument *)voidtd;

    tedCleanCursorBlink( td );

    if  ( td->tdDocument )
	{
	BufferDocument *	bd= td->tdDocument;
	int			noteCount= 0;
	int			paragraphCount= 0;

	docCloseItemObjects( &noteCount, &paragraphCount,
					    bd, &(bd->bdItem),
					    (void *)add, tedCloseObject );

	docFreeDocument( bd );
	}

    utilCleanMemoryBuffer( &(td->tdCopiedSelection) );
    utilCleanMemoryBuffer( &(td->tdCopiedFont) );
    utilCleanMemoryBuffer( &(td->tdCopiedRuler) );

    appCleanBitmapImage( &(td->tdCopiedImage) );

    free( td );
    }

int tedMakeDocumentWidget(	EditApplication *	ea,
				EditDocument *		ed )
    {
    if  ( appMakeDocumentWidget( ea, ed ) )
	{ LDEB(1); return -1;	}

    if  ( ! ea->eaDocumentCursor )
	{
#	ifdef USE_MOTIF
	ea->eaDocumentCursor= XCreateFontCursor(
			    XtDisplay( ea->eaToplevel.atTopWidget), XC_xterm );
	if  ( ! ea->eaDocumentCursor )
	    { LDEB(ea->eaDocumentCursor);	}
#	endif

#	ifdef USE_GTK
	ea->eaDocumentCursor= gdk_cursor_new( GDK_XTERM );
	if  ( ! ea->eaDocumentCursor )
	    { XDEB(ea->eaDocumentCursor);	}
#	endif
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find out whether a document uses just one code page.		*/
/*									*/
/************************************************************************/

static int tedDetermineCodepage(	BufferDocument *	bd,
					const AppDrawingData *	add )
    {
    PostScriptFaceList		psfl;
    PostScriptFace *		psf;
    int				i;

    int				charset= -1;
    int				charsetCount= 0;

    utilInitPostScriptFaceList( &psfl );

    if  ( docPsPrintGetDocumentFonts( bd, &psfl, &(add->addPhysicalFontList) ) )
	{ LDEB(1); return -1;	}

    psf= psfl.psflFaces;
    for ( i= 0; i < psfl.psflFaceCount; psf++, i++ )
	{
	if  ( charset < 0 )
	    { charset= psf->psfEncodingUsed; charsetCount= 1;	}
	else{
	    if  ( charset != psf->psfEncodingUsed )
		{ charsetCount++; }
	    }
	}

    utilCleanPostScriptFaceList( &psfl );

    if  ( charsetCount != 1 )
	{ return -1;	}

    switch( charset )
	{
	case ENCODINGpsISO_8859_1:
	    return 1252;

	case ENCODINGpsISO_8859_2:
	    return 1250;

	default:
	    LDEB(charset); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Make a new empty document.						*/
/*									*/
/************************************************************************/

int tedNewDocument(	EditApplication *	ea,
			EditDocument *		ed,
			const char *		filename )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    PropertyMask		newMask;
    ExpandedTextAttribute	etaNew;
    TextAttribute		ta;

    time_t			now;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    int				rval= 0;

    tedDetermineDefaultSettings( tar );

    PROPmaskCLEAR( &newMask );
    docInitExpandedTextAttribute( &etaNew );

    if  ( docExpandedAttributeFromString( ea->eaDefaultFont,
							&newMask, &etaNew ) )
	{ SDEB(ea->eaDefaultFont); rval= -1; goto ready;	}

    if  ( ! PROPmaskISSET( &newMask, TApropFONTSIZE )	||
	  etaNew.etaFontSizeHalfPoints < 6		)
	{
	LDEB(etaNew.etaFontSizeHalfPoints);
	etaNew.etaFontSizeHalfPoints= 24;
	PROPmaskADD( &newMask, TApropFONTSIZE );
	}

    bd= docNewFile( &ta, etaNew.etaFontFamilyName,
				    etaNew.etaFontSizeHalfPoints,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaDefaultDocumentGeometry) );
    if  ( ! bd )
	{ XDEB(bd); rval= -1; goto ready;	}

    ed->edFormat= TEDdockindRTF; /* rtf */
    td->tdDocument= bd;
    dp= &(bd->bdProperties);

    if  ( filename )
	{
	const char *	ext= appFileExtensionOfName( filename );

	if  ( ext && ! strcmp( ext, "rtf" ) )
	    { ed->edFormat= TEDdockindRTF;	}
	if  ( ext && ! strcmp( ext, "txt" ) )
	    { ed->edFormat= TEDdockindTEXT_SAVE_FOLDED;	}
	}

    if  ( ea->eaAuthor )
	{ dp->dpAuthor= (unsigned char *)strdup( ea->eaAuthor ); }

    now= time( (time_t *)0 );
    dp->dpCreatim= *localtime( &now );

  ready:

    docCleanExpandedTextAttribute( &etaNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Perform the setup of the editing environment for a document.	*/
/*									*/
/*  8)  Determine the code page of the document.			*/
/*  9)  Do not use the 1252 codepage as an explicit setting: It is	*/
/*	almost certainly returned by a default 'Ted' installation, and	*/
/*	is almost impossible to get rid of.				*/
/*									*/
/************************************************************************/

int tedLayoutDocument(		void *				privateData,
				int				format,
				AppDrawingData *		add,
				const DocumentGeometry *	dgDef )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dgDoc= &(dp->dpGeometry);

    int				noteNumbersChanged= 0;

    RecalculateFields		rf;

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= FIELDdoDOC_INFO|FIELDdoDOC_COMPLETE|FIELDdoCHFTN;
    rf.rfFieldsUpdated= 0;

    tedScreenRectangles( add, dgDoc );

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( tedLayoutDocumentTree( td, add ) )
	{ LDEB(1); return -1;	}

    docRenumberNotes( &noteNumbersChanged, bd );

    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED;
    rf.rfFieldsUpdated= 0;

    if  ( noteNumbersChanged )
	{ rf.rfUpdateFlags |= FIELDdoCHFTN;	}

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0 && tedLayoutDocumentTree( td, add ) )
	{ LDEB(1); return -1;	}

    /*  8  */
    if  ( bd->bdProperties.dpAnsiCodepage < 0 )
	{
	bd->bdProperties.dpAnsiCodepage= tedDetermineCodepage( bd, add );

	/*  9  */
	if  ( bd->bdProperties.dpAnsiCodepage == 1252 )
	    { bd->bdProperties.dpAnsiCodepage= -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a document.							*/
/*									*/
/************************************************************************/

int tedSaveDocument(	const void *		privateData,
			int			format,
			const char *		applicationId,
			const char *		documentTitle,
			const char *		filename )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    SimpleOutputStream *	sos;

    const int			saveBookmarks= 1;

    const int			asMimeAggr= 0;

    time_t			now;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ /* SXDEB(filename,sos); */ return -1;	}

    switch( format )
	{
	case TEDdockindRTF:
	    now= time( (time_t *)0 );
	    dp->dpRevtim= *localtime( &now );

	    if  ( docRtfSaveDocument( sos, bd, (const DocumentSelection *)0,
							    saveBookmarks ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindTEXT_OPEN:
	    LDEB(format);
	    /*FALLTHROUGH*/

	case TEDdockindTEXT_SAVE_FOLDED:
	    if  ( docPlainSaveDocument( sos, bd,
					    (const DocumentSelection *)0,
					    1, 1 ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindTEXT_SAVE_WIDE:
	    if  ( docPlainSaveDocument( sos, bd,
					    (const DocumentSelection *)0,
					    0, 1 ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindHTML_FILES:
	    if  ( docHtmlSaveDocument( sos, bd, asMimeAggr,
						(const char *)0, filename ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	default:
	    LDEB(format); return -1;
	}

    sioOutClose( sos );
    
    return 0;
    }


/************************************************************************/
/*									*/
/*  Allocate private data for a document.				*/
/*									*/
/************************************************************************/

void * tedMakePrivateData()
    {
    TedDocument *	td;

    td= (TedDocument *)malloc( sizeof(TedDocument) );
    if  ( ! td )
	{ XDEB(td); return (void *)0;	}

    td->tdDocument= (BufferDocument *)0;
    utilInitTextAttribute( &(td->tdCurrentTextAttribute) );
    td->tdCurrentTextAttributeNumber= -1;
    td->tdCurrentPhysicalFont= -1;

    td->tdFormatMenu= (APP_WIDGET)0;
    td->tdFormatMenuButton= (APP_WIDGET)0;
    td->tdFormatOneParaOption= (APP_WIDGET)0;

    td->tdCopyWidget= (APP_WIDGET)0;
    td->tdCutWidget= (APP_WIDGET)0;
    td->tdPasteWidget= (APP_WIDGET)0;

    td->tdInsertMenu= (APP_WIDGET)0;
    td->tdInsertMenuButton= (APP_WIDGET)0;
    td->tdInsPictOption= (APP_WIDGET)0;
    td->tdInsFileOption= (APP_WIDGET)0;
    td->tdInsSymbolOption= (APP_WIDGET)0;
    td->tdInsHyperlinkOption= (APP_WIDGET)0;
    td->tdInsBookmarkOption= (APP_WIDGET)0;
    td->tdInsInsertFootnoteOption= (APP_WIDGET)0;
    td->tdInsInsertChftnsepOption= (APP_WIDGET)0;
    td->tdInsInsertTableOption= (APP_WIDGET)0;
    td->tdInsInsertPageNumberOption= (APP_WIDGET)0;
    td->tdInsInsertLineBreakOption= (APP_WIDGET)0;
    td->tdInsInsertPageBreakOption= (APP_WIDGET)0;
    td->tdInsInsertSectBreakOption= (APP_WIDGET)0;

    td->tdTableMenu= (APP_WIDGET)0;
    td->tdTableMenuButton= (APP_WIDGET)0;
    td->tdTabInsertTableOption= (APP_WIDGET)0;
    td->tdTabAddRowOption= (APP_WIDGET)0;
    td->tdTabAddColumnOption= (APP_WIDGET)0;

    td->tdSelectTableWidget= (APP_WIDGET)0;
    td->tdSelectRowWidget= (APP_WIDGET)0;
    td->tdSelectColumnOption= (APP_WIDGET)0;
    td->tdDrawTableGridOption= (APP_WIDGET)0;

    td->tdFontMenu= (APP_WIDGET)0;
    td->tdFontMenuButton= (APP_WIDGET)0;
    td->tdFontBoldOption= (APP_WIDGET)0;
    td->tdFontItalicOption= (APP_WIDGET)0;
    td->tdFontUnderlinedOption= (APP_WIDGET)0;
    td->tdFontSuperscriptOption= (APP_WIDGET)0;
    td->tdFontSubscriptOption= (APP_WIDGET)0;

    td->tdToolsMenu= (APP_WIDGET)0;
    td->tdToolsMenuButton= (APP_WIDGET)0;

    docInitDocumentSelection( &(td->tdDocumentSelection) );
    docInitSelectionGeometry( &(td->tdSelectionGeometry) );

    td->tdVisibleSelectionCopied= 0;
    td->tdCanReplaceSelection= 0;

    td->tdCopiedSelectionClosed= 0;
    utilInitMemoryBuffer( &(td->tdCopiedSelection) );
    utilInitMemoryBuffer( &(td->tdCopiedFont) );
    utilInitMemoryBuffer( &(td->tdCopiedRuler) );

    appInitBitmapImage( &(td->tdCopiedImage) );

#   ifdef USE_MOTIF
    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;
#   endif
#   ifdef USE_GTK
    td->tdHideIBarId= 0;
    td->tdShowIBarId= 0;
#   endif

    td->tdObjectWindow= (APP_WINDOW)0;
    td->tdObjectBottomWindow= (APP_WINDOW)0;
    td->tdObjectRightWindow= (APP_WINDOW)0;
    td->tdObjectCornerWindow= (APP_WINDOW)0;
    td->tdObjectSelected= 0;

    td->tdDrawTableGrid= 1;

    return (void *)td;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( tedObserveFocus, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    EditApplication *	ea= ed->edApplication;

    int			inout= 0;

    if  ( appDrawGetInoutFromFocusEvent( &inout, w, event ) )
	{ return;	}

    if  ( inout > 0 )
	{
	if  ( ea->eaCurrentDocument != ed )
	    {
	    appSetCurrentDocument( ea, ed );

	    if  ( ea->eaFindTool && ed->edIsReadonly )
		{ appFindToolDisableReplace( ea->eaFindTool ); }

	    if  ( ea->eaSpellTool )
		{ appEnableSpellTool( ea->eaSpellTool, ! ed->edIsReadonly ); }

	    tedAdaptPageToolToDocument( ea, ed );

	    tedAdaptFormatToolToDocument( ed );
	    }

	if  ( tedHasIBarSelection( td ) )
	    { tedStartCursorBlink( ed );	}
	}

    if  ( inout < 0 )
	{ tedStopCursorBlink( ed ); }
    }

/************************************************************************/
/*									*/
/*  Print a document to PostScript.					*/
/*									*/
/************************************************************************/

int tedPrintDocument(	SimpleOutputStream *		sos,
			const PrintJob *		pj,
			const PrintGeometry *		pg,
			int				firstPage,
			int				lastPage )
    {
    EditApplication *		ea= pj->pjApplication;
    TedDocument *		td= (TedDocument *)pj->pjPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    time_t			now;

    RecalculateFields		rf;

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)pj->pjDrawingData;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoDOC_INFO;
    rf.rfFieldsUpdated= 0;

    now= time( (time_t *)0 );
    dp->dpPrintim= *localtime( &now );

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0				&&
	  tedLayoutDocumentTree( td, pj->pjDrawingData )	)
	{ LDEB(1); return -1;	}

    if  ( docPsPrintDocument( sos, pj->pjTitle, ea->eaApplicationName,
				ea->eaReference, pj->pjDrawingData,
				td->tdDocument, pg,
				pj->pjUsePostScriptFilters,
				pj->pjUsePostScriptIndexedImages,
				firstPage, lastPage,
				tedLayoutExternalItem, tedCloseObject ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/*  1)  Open an input stream.						*/
/*  2)  Try to read as RTF.						*/
/*  3)  If this fails, try as plain text.				*/
/*									*/
/************************************************************************/

int tedOpenDocumentFile(	EditApplication *	ea,
				int *			pFormat,
				BufferDocument **	pBd,
				const char *		filename,
				APP_WIDGET		relative,
				APP_WIDGET		option )
    {
    const char *		ext;
    SimpleInputStream *		sis;

    BufferDocument *		bd;
    int				triedRtf= 0;

    int				resp;
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    int				longestPara;

    tedDetermineDefaultSettings( tar );

    ext= appFileExtensionOfName( filename );

    /*  1  */
    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{
	appQuestionRunSubjectErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNoAccess );

	return -1;
	}

    if  ( ext && ! strcmp( ext, "rtf" ) )
	{
	/*  2  */
	bd= docRtfReadFile( sis, tar->tarDefaultAnsicpgInt );

	sioInClose( sis );
	
	if  ( bd )
	    {
	    *pFormat= TEDdockindRTF; *pBd= bd;

	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }
	else{
	    resp= appQuestionRunSubjectOkCancelDialog( ea,
				    relative, option,
				    filename, tar->tarFileNotRtf,
				    (char *)0, (char *)0 );

	    if  ( resp != AQDrespOK )
		{ return -1;	}

	    triedRtf= 1;
	    }
	}

    if  ( ext && ! strcmp( ext, "txt" ) )
	{
	bd= docPlainReadFile( sis, &longestPara,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaDefaultDocumentGeometry) );

	sioInClose( sis );

	if  ( bd )
	    {
	    *pBd= bd;

	    if  ( longestPara > 76 )
		{ *pFormat= TEDdockindTEXT_SAVE_WIDE;		}
	    else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;		}

	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }
	else{ SXDEB(filename,bd); return -1;	}
	}

    /*  2  */
    if  ( ! triedRtf )
	{
	bd= docRtfReadFile( sis, tar->tarDefaultAnsicpgInt );
	if  ( bd )
	    {
	    *pFormat= TEDdockindRTF; *pBd= bd;
	    
	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }

	resp= appQuestionRunSubjectOkCancelDialog( ea,
					ea->eaToplevel.atTopWidget, option,
					filename, tar->tarFileNotRtf,
					(char *)0, (char *)0 );

	if  ( resp != AQDrespOK )
	    { return -1;	}
	}

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    bd= docPlainReadFile( sis, &longestPara,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaDefaultDocumentGeometry) );

    sioInClose( sis );

    if  ( bd )
	{
	*pBd= bd;

	if  ( longestPara > 76 )
	    { *pFormat= TEDdockindTEXT_SAVE_WIDE;		}
	else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;		}

	if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
	    { LDEB(1);	}

	return 0;
	}

    SXDEB(filename,bd); return -1;
    }

