/************************************************************************/
/*									*/
/*  Ted: Copy/Paste related functionality.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<sioMemory.h>
#   include	<sioXprop.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Edit menu actions.							*/
/*									*/
/*  2)  If the beginning of the selection is the beginning of a		*/
/*	kyperlink, try to include the link.				*/
/*  8)  Also put a copied picture on the clipboard that 'xv' implements	*/
/*	on the root window.						*/
/*									*/
/************************************************************************/

static void tedFinishTextCopy(	TedDocument *			td,
				AppDrawingData *		add,
				const DocumentSelection *	ds )
    {
    BufferDocument *		bd= td->tdDocument;
    SelectionGeometry		sg;

    tedSelectionGeometry( &sg, ds, bd, add );

    td->tdCopiedSelectionClosed= 1;

    if  ( (unsigned)ds->dsEnd.dpStroff < ds->dsEnd.dpBi->biParaStrlen )
	{ td->tdCopiedSelectionClosed= 0;	}

    if  ( docIsParaSelection( ds )		&&
	  sg.sgBegin.pgLine == sg.sgEnd.pgLine	)
	{ td->tdCopiedSelectionClosed= 0;	}

    appCleanBitmapImage( &(td->tdCopiedImage) );
    appInitBitmapImage( &(td->tdCopiedImage) );

    return;
    }

static int tedDocCopySelection(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);

    BufferItem *		bi;
    TextParticule *		tp;
    DocumentPosition		dpObject;
    InsertedObject *		io;

    SimpleOutputStream *	sos;

    DocumentSelection		dsNew;
    const int			saveBookmarks= 0;

    int				startPart;
    int				endPart;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    const char *		markName= (const char *)0;
    int				markSize= 0;

    if  ( ! tedHasSelection( td ) || tedHasIBarSelection( td ) )
	{ return -1; }

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ return -1;	}

    dsNew= ds;

    /*  2  */
    if  ( ! docGetHyperlinkForPosition( bd, &(dsNew.dsBegin),
				&startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{
	bi= dsNew.dsBegin.dpBi;
	tp= bi->biParaParticules+ dsNew.dsBegin.dpParticule;

	if  ( startPart < dsNew.dsBegin.dpParticule			&&
	      bi->biParaParticules[startPart].tpStroff == tp->tpStroff	)
	    { dsNew.dsBegin.dpParticule= startPart;	}

	if  ( dsNew.dsEnd.dpBi == bi )
	    {
	    tp= bi->biParaParticules+ dsNew.dsEnd.dpParticule;

	    if  ( endPart > dsNew.dsEnd.dpParticule			&&
		  bi->biParaParticules[endPart].tpStroff == tp->tpStroff )
		{ dsNew.dsEnd.dpParticule= endPart;	}
	    }
	}

    sos= sioOutMemoryOpen( &(td->tdCopiedSelection) );
    if  ( ! sos )
	{ XDEB(sos); return -1;    }

    if  ( docRtfSaveDocument( sos, td->tdDocument, &dsNew, saveBookmarks ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return -1;	}

    tedFinishTextCopy( td, add, &dsNew );

    docInitDocumentPosition( &dpObject );

    if  ( ea->eaSupportXvCopyPaste				&&
	  ! tedGetObjectSelection( td, &dpObject, &io )		&&
	  ! tedSaveObjectPicture( &(td->tdCopiedImage), io )	)
	{
	/*  8  */
	if  ( appImgPutXvClipboard( &(td->tdCopiedImage.abiBitmap),
		td->tdCopiedImage.abiBuffer, ea->eaToplevel.atTopWidget ) )
	    { LDEB(1);	}
	}

    if  ( appDocOwnSelection( ed, "PRIMARY",
		TedPrimaryTextTargets, TedPrimaryTextTargetCount ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a part of the document, in practice, a bookmark as a link	*/
/*									*/
/************************************************************************/

int tedCopyAsLink(	EditDocument *			ed,
			const DocumentSelection *	ds,
			int				asRef,
			int				asPageref,
			const char *			fileName,
			const char *			markName,
			int				markSize )
    {
    SimpleOutputStream *	sos;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				fileSize= 0;

    if  ( fileName )
	{ fileSize= strlen( fileName );	}

    sos= sioOutMemoryOpen( &(td->tdCopiedSelection) );
    if  ( ! sos )
	{ XDEB(sos); return -1;    }

    if  ( docRtfSaveSelectionAsLink( sos, td->tdDocument, ds,
				asRef, asPageref,
				(const unsigned char *)fileName, fileSize,
				(const unsigned char *)markName, markSize ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return -1;	}

    tedFinishTextCopy( td, add, ds );

    if  ( appDocOwnSelection( ed, "PRIMARY",
		    TedPrimaryTextTargets, TedPrimaryTextTargetCount ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void tedDocCut(			EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				isTableSlice= 0;
    int				isRowSlice= 0;
    int				isColSlice= 0;
    TableRectangle		tr;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ return;	}

    if  ( ! docGetTableSliceSelection( &isRowSlice, &isColSlice, &tr, &ds ) )
	{ isTableSlice= 1;	}

    if  ( ! isTableSlice && ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    if  ( tedDocCopySelection( ed ) )
	{ LDEB(1); return;	}

    if  ( isTableSlice )
	{ tedDeleteTableSliceSelection( ed );				}
    else{ tedDocReplaceSelection( ed, (const unsigned char *)0, 0 );	}

    return;
    }

void tedDocCopy(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    const int			scrolledX= 0;
    const int			scrolledY= 0;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ return;	}

    if  ( tedDocCopySelection( ed ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 1;

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Ted, Copy/Paste functionality.					*/
/*									*/
/************************************************************************/

void tedPrimaryLost(	APP_WIDGET			w,
			void *				voided,
			APP_EVENT *			event )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 0;

    if  ( tedHasSelection( td ) && ! tedHasIBarSelection( td ) )
	{
	const int	scrolledX= 0;
	const int	scrolledY= 0;

	appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );
	}
    }

/************************************************************************/
/*									*/
/*  Return the selection to a requestor.				*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( tedCopyPrimaryRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( td->tdCopiedSelection.mbSize == 0 )
	{ LDEB(td->tdCopiedSelection.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
				    td->tdCopiedSelection.mbSize ) <= 0 )
	{ LDEB(td->tdCopiedSelection.mbSize); sioOutClose( sos ); return; }

    sioOutClose( sos );

#   if 0
    sos= sioOutStdioOpen( "/tmp/copied.rtf" );
    XDEB(sos);
    if  ( sos )
	{
	if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
				    td->tdCopiedSelection.mbSize ) <= 0 )
	    { LDEB(td->tdCopiedSelection.mbSize); }

	sioOutClose( sos );
	}
#   endif

    return;
    }

APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    BufferDocument *		bd;
    SimpleInputStream *		sis;
    int				res;

    if  ( td->tdCopiedSelection.mbSize == 0 )
	{ LDEB(td->tdCopiedSelection.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    sis= sioInMemoryOpen( &td->tdCopiedSelection );
    if  ( ! sis )
	{ XDEB(sis); sioOutClose( sos ); return;	}

    bd= docRtfReadFile( sis, td->tdDocument->bdProperties.dpAnsiCodepage );

    sioInClose( sis );

    if  ( ! bd )
	{ XDEB(bd); sioOutClose( sos ); return; }

    res= docPlainSaveDocument( sos, bd,
				    (DocumentSelection *)0,
				    0, td->tdCopiedSelectionClosed );

    docFreeDocument( bd );

    if  ( res )
	{ LDEB(res); sioOutClose( sos ); return; }

    sioOutClose( sos );

    return;
    }

APP_GIVE_COPY( tedCopyPrimaryPng, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( ! td->tdCopiedImage.abiBuffer )
	{ XDEB(td->tdCopiedImage.abiBuffer); return;	}

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( bmPngWritePng( &(td->tdCopiedImage.abiBitmap),
					td->tdCopiedImage.abiBuffer, sos ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    sioOutClose( sos );

    return;
    }

APP_GIVE_COPY( tedCopyPrimaryPixmap, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    AppDrawingData *	add= &(ed->edDrawingData);
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    APP_BITMAP_IMAGE	pixmapCopied;

    int			wide= td->tdCopiedImage.abiBitmap.bdPixelsWide;
    int			high= td->tdCopiedImage.abiBitmap.bdPixelsHigh;

    if  ( appImgMakePixmap( add,
			    &pixmapCopied, wide, high,
			    &(ed->edColors), &(td->tdCopiedImage) ) )
	{
	appCleanBitmapImage( &(td->tdCopiedImage) );
	appInitBitmapImage( &(td->tdCopiedImage) );
	return;
	}

    appCopyPixmapValue( event, pixmapCopied );

    return;
    }

APP_GIVE_COPY( tedCopyFontTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( td->tdCopiedFont.mbSize == 0 )
	{ LDEB(td->tdCopiedFont.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedFont.mbBytes,
					td->tdCopiedFont.mbSize ) <= 0 )
	{ LDEB(1); sioOutClose( sos ); return;	}

    sioOutClose( sos );

    return;
    }

APP_GIVE_COPY( tedCopyRulerTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( td->tdCopiedRuler.mbSize == 0 )
	{ LDEB(td->tdCopiedRuler.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedRuler.mbBytes,
					td->tdCopiedRuler.mbSize ) <= 0 )
	{ LDEB(1); sioOutClose( sos ); return;	}

    sioOutClose( sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish a paste action.						*/
/*									*/
/************************************************************************/

typedef BufferDocument * (*TED_READ) (
				SimpleInputStream *		sis,
				int				ansiCodepage,
				const DocumentGeometry *	dg );

typedef int	(*TED_INCLUDE) (	APP_WIDGET		w,
					EditDocument *		ed,
					BufferDocument *	bd );

static BufferDocument * tedPasteRtfReadFile(
				SimpleInputStream *		sis,
				int				ansiCodepage,
				const DocumentGeometry *	dg )
    { return docRtfReadFile( sis, ansiCodepage );	}

static BufferDocument * tedPastePlainReadFile(
				SimpleInputStream *		sis,
				int				ansiCodepage,
				const DocumentGeometry *	dg )
    {
    int		longestPara;

    return docPlainReadFile( sis, &longestPara,
					DOCcharsetANSI, ansiCodepage, dg );
    }

static void tedPastePrimaryGeneric(	APP_WIDGET		w,
					EditDocument *		ed,
					APP_SELECTION_EVENT *	event,
					TED_READ		readDoc,
					TED_INCLUDE		includeDoc )
    {
    EditApplication *		ea= ed->edApplication;

    SimpleInputStream *		sis;

    BufferDocument *		bd;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    tedDetermineDefaultSettings( tar );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); return;	}

    bd= (*readDoc)( sis, tar->tarDefaultAnsicpgInt,
					&(ea->eaDefaultDocumentGeometry) );

    sioInClose( sis );

    if  ( ! bd )
	{ XDEB( bd ); return; }

    if  ( (*includeDoc)( w, ed, bd ) )
	{ LDEB(1); docFreeDocument( bd ); return;	}

    docFreeDocument( bd );

    appDocumentChanged( ed, 1 );

    return;
    }

APP_PASTE_REPLY( tedPastePrimaryRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPastePrimaryGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedIncludeRtfDocument );
    }

APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPastePrimaryGeneric( w, ed, event,
			    tedPastePlainReadFile, tedIncludePlainDocument );
    }

APP_PASTE_REPLY( tedPastePrimaryPng, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleInputStream *	sis;

    AppBitmapImage *	abi;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
    if  ( ! abi )
	{ XDEB(abi); return;	}
    appInitBitmapImage( abi );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); return;	}

    if  ( bmPngReadPng( &(abi->abiBitmap), &(abi->abiBuffer), sis ) )
	{ LDEB(1); return;	}

    sioInClose( sis );

    if  ( tedReplaceSelectionWithBitmapImage( ed, abi ) )
	{ LDEB(1); return; }

    return;
    }

APP_PASTE_REPLY( tedPastePrimaryPixmap, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    AppBitmapImage *	abi;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
    if  ( ! abi )
	{ XDEB(abi); return;	}
    appInitBitmapImage( abi );

    if  ( appImgPastePixmap( abi, w, event ) )
	{ LDEB(1); return;	}

    if  ( tedReplaceSelectionWithBitmapImage( ed, abi ) )
	{ LDEB(1); return; }

    return;
    }

APP_PASTE_REPLY( tedPasteFontTed, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

#   define		FONTSIZ	200

    SimpleInputStream *		sis;

    char			attrString[FONTSIZ+1];
    int				l;

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); return;	}

    if  ( ! sioInGetString( attrString, FONTSIZ, sis ) )
	{ LDEB(1); sioInClose( sis ); return; }

    l= strlen( attrString );
    if  ( l > 0 && attrString[l-1] == '\n' )
	{ attrString[l-1]= '\0';	}

    if  ( tedAppChangeSelectionAttributeString( ed, attrString ) )
	{
	SDEB(attrString);
	sioInClose( sis );
	return;
	}

    sioInClose( sis );

    appDocumentChanged( ed, 1 );

    return;
    }

APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    SimpleInputStream *		sis;

    ParagraphProperties		ppNew;
    TextAttribute		taSet;

    PropertyMask		taSetMask;
    PropertyMask		ppUpdMask;
    PropertyMask		spUpdMask;

    PROPmaskCLEAR( &taSetMask );
    PROPmaskCLEAR( &spUpdMask );

    docInitParagraphProperties( &ppNew );
    utilInitTextAttribute( &taSet );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); return;	}

    if  ( docRtfReadRuler( sis, &ppNew ) )
	{ LDEB(1); sioInClose( sis ); return; }

    sioInClose( sis );

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskADD( &ppUpdMask, PPpropLEFT_INDENT );
    PROPmaskADD( &ppUpdMask, PPpropFIRST_INDENT );
    PROPmaskADD( &ppUpdMask, PPpropRIGHT_INDENT );
    PROPmaskADD( &ppUpdMask, PPpropALIGNMENT );
    PROPmaskADD( &ppUpdMask, PPpropTAB_STOPS );

    if  ( tedChangeSelectionProperties( ed,
				    &taSetMask, &taSet,
				    &ppUpdMask, &ppNew,
				    &spUpdMask, (SectionProperties *)0 ) )
	{ LDEB(1); docCleanParagraphProperties( &ppNew ); return; }

    docCleanParagraphProperties( &ppNew );

    return;
    }

/************************************************************************/
/*									*/
/*  Custom 'Paste' functionality.					*/
/*									*/
/*  1)  The regular X11 manner.						*/
/*  2)  Try to get a possible 'paste' from xv. This is just to make it	*/
/*	possible to cooperate with John Bradley's XV. As XV is a little	*/
/*	outdated, only support this if the appropriate resource is set.	*/
/*									*/
/************************************************************************/

void tedDocEditPaste(		APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;
    TedDocument *			td= (TedDocument *)ed->edPrivateData;

    int					res;
    BitmapDescription			bd;
    unsigned char *			buffer;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    /*  1  */
    res= appDocAskForPaste( ed, "PRIMARY" );
    if  ( ! res )
	{ return;	}

    /*  2  */
    if  ( ea->eaSupportXvCopyPaste					&&
	  ! appImgGetXvClipboard( &bd, &buffer,
					ea->eaToplevel.atTopWidget )	)
	{
	AppBitmapImage *	abi;

	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
	if  ( ! abi )
	    { XDEB(abi); return;	}
	appInitBitmapImage( abi );

	if  ( bmCopyDescription( &(abi->abiBitmap), &bd ) )
	    { bmCleanDescription( &bd ); return; }

	bmCleanDescription( &bd );
	abi->abiBuffer= buffer;

	if  ( tedReplaceSelectionWithBitmapImage( ed, abi ) )
	    { LDEB(1); return; }

	return;
	}

    return;
    }

