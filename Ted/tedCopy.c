/************************************************************************/
/*									*/
/*  Ted: Copy: Producing content for a pasting application		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedSelect.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedEdit.h"
#   include	"tedApp.h"
#   include	"tedDocument.h"
#   include	<tedDocFront.h>
#   include	"tedDocMenu.h"
#   include	"tedCopyPaste.h"
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docRtfFlags.h>
#   include	<docEditCommand.h>
#   include	<sioMemory.h>
#   include	<sioXprop.h>
#   include	<sioFileio.h>
#   include	<bmio.h>
#   include	<appEditDocument.h>
#   include	<sioGeneral.h>
#   include	<docDocument.h>
#   include	<appDocFront.h>
#   include	<bitmap.h>
#   include	<docFileExtensions.h>

#   include	<appDebugon.h>

void tedDocCut(			EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ return;	}

    if  ( ! sd.sdIsTableSlice && ! sd.sdCanReplace )
	{ LLDEB(sd.sdIsTableSlice,sd.sdCanReplace); return;	}

    /*  Nothing to copy -> Nothing to cut */
    if  ( sd.sdIsIBarSelection )
	{ return;	}

    if  ( tedDocCopySelection( ed ) )
	{ LDEB(1); return;	}

    if  ( sd.sdIsTableSlice )
	{
	const TableRectangle *	tr= &(sd.sdTableRectangle);

	if  ( tr->trIsWholeTable )
	    {
	    if  ( tedDocDeleteTable( ed, td->tdTraced ) )
		{ LDEB(tr->trIsWholeTable); return;	}
	    }
	else{
	    if  ( tr->trIsRowSlice )
		{
		if  ( tedDocDeleteRows( ed, td->tdTraced ) )
		    { LDEB(tr->trIsRowSlice); return;	}
		}
	    else{
		if  ( tr->trIsColumnSlice )
		    {
		    if  ( tedDocDeleteColumns( ed, td->tdTraced ) )
			{ LDEB(tr->trIsColumnSlice); return;	}
		    }
		else{
		    LLDEB(tr->trIsRowSlice,tr->trIsColumnSlice); return;
		    }
		}
	    }
	}
    else{
	tedDocDeleteSelection( ed, EDITcmdDELETE_SELECTION, td->tdTraced );
	}

    if  ( appDocOwnSelection( ed, "CLIPBOARD",
		TedClipboardTextTargets, TedClipboardTextTargetCount ) )
	{ LDEB(1); 	}

    return;
    }

void tedDocCopy(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ return;	}

    if  ( tedDocCopySelection( ed ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 1;

    tedExposeCurrentSelection( ed );

    if  ( appDocOwnSelection( ed, "CLIPBOARD",
		TedClipboardTextTargets, TedClipboardTextTargetCount ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  We lost the ownership of the selection on the clipboard. So we are	*/
/*  no longer supposed to paste the selection. Make this visible for	*/
/*  user.								*/
/*									*/
/************************************************************************/

void tedClipboardLost(	APP_WIDGET			w,
			void *				voided,
			APP_EVENT *			event )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 0;

    if  ( sd.sdIsSet && ! sd.sdIsIBarSelection )
	{ tedExposeCurrentSelection( ed );	}
    }

void tedPrimaryLost(	APP_WIDGET			w,
			void *				voided,
			APP_EVENT *			event )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdOwnsPrimarySelection= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Return the selection to a requestor.				*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( tedCopyClipboardRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedSelection) ) )
	{ LDEB(td->tdCopiedSelection.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
				    td->tdCopiedSelection.mbSize ) <= 0 )
	{ LDEB(td->tdCopiedSelection.mbSize); sioOutClose( sos ); return; }

    sioOutClose( sos );

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	sos= sioOutFileioOpenS( "/tmp/returned.rtf" );
	if  ( sos )
	    {
	    if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
					td->tdCopiedSelection.mbSize ) <= 0 )
		{ LDEB(td->tdCopiedSelection.mbSize); }

	    sioOutClose( sos );
	    }
	}

    return;
    }

typedef struct CopyOutput
    {
    struct BufferDocument *	coDocument;
    SimpleOutputStream *	coSos;
    } CopyOutput;

static void tedInitCopyOutput(	CopyOutput *	co )
    {
    co->coDocument= (struct BufferDocument *)0;
    co->coSos= (SimpleOutputStream *)0;
    }

static void tedCleanCopyOutput(	CopyOutput *	co )
    {
    if  ( co->coDocument )
	{ docFreeDocument( co->coDocument );	}
    if  ( co->coSos )
	{ sioOutClose( co->coSos );		}
    }

static int tedOpenCopyOutput(	CopyOutput *		co,
				const MemoryBuffer *	mb,
				APP_WIDGET		w,
				APP_SELECTION_EVENT *	event )
    {
    int				rval= 0;
    SimpleInputStream *		sis= (SimpleInputStream *)0;

    const unsigned int		rtfFlags= 0;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ LDEB(mb->mbSize); rval= -1; goto ready; }

    co->coSos= sioOutOpenCopy( w, event );
    if  ( ! co->coSos )
	{ XDEB(co->coSos); rval= -1; goto ready;	}

    sis= sioInMemoryOpen( mb );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    co->coDocument= docRtfReadFile( sis, rtfFlags );
    if  ( ! co->coDocument )
	{ XDEB(co->coDocument); rval= -1; goto ready; }

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

static void tedCopyString(	const MemoryBuffer *	mb,
				APP_WIDGET		w,
				APP_SELECTION_EVENT *	event )
    {
    CopyOutput			co;

    tedInitCopyOutput( &co );

    if  ( tedOpenCopyOutput( &co, mb, w, event ) )
	{ LDEB(1); goto ready;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{ tedSaveSelectionTxtToFile( co.coDocument, "/tmp/returned.txt" ); }

    if  ( docPlainSaveDocument( co.coSos, co.coDocument,
						(DocumentSelection *)0, 0 ) )
	{ LDEB(1);	}

  ready:

    tedCleanCopyOutput( &co );

    return;
    }

APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;

    DocumentSelection		ds;
    SelectionDescription	sd;

    MemoryBuffer		mb;

    utilInitMemoryBuffer( &mb );

    if  ( ! tedDocSaveSelectionRtf( &mb, &ds, &sd, ed ) )
	{ tedCopyString( &mb, w, event );	}

    utilCleanMemoryBuffer( &mb );

    return;
    }

APP_GIVE_COPY( tedCopyClipboardString, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedCopyString( &(td->tdCopiedSelection), w, event );
    return;
    }

APP_GIVE_COPY( tedCopyClipboardHtml, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    struct EditApplication *	ea= ed->edApplication;

    CopyOutput			co;

    TedDocument *		tdIn= ed->edPrivateData;
    TedDocument *		tdOut= tedMakePrivateData( ea );

    const int			format= TEDdockindHTML_FILES;

    tedInitCopyOutput( &co );

    if  ( tedOpenCopyOutput( &co, &(tdIn->tdCopiedSelection), w, event ) )
	{ LDEB(1); goto ready;	}

    if  ( ! tdOut )
	{ XDEB(tdOut); goto ready;	}

    tdOut->tdDocument= co.coDocument;
    co.coDocument= (struct BufferDocument *)0; /* steal */

    if  ( tedSaveDocumentToStream( ea, (DrawingSurface)0, co.coSos,
					tdOut, format,
					(const struct MemoryBuffer *)0,
					(const struct MemoryBuffer *)0 ) )
	{ LDEB(1);	}

  ready:

    if  ( tdOut )
	{ tedFreeDocument( tdOut, format );	}

    tedCleanCopyOutput( &co );

    return;
    }

static int tedCopyImageFile(	APP_WIDGET		w,
				APP_SELECTION_EVENT *	event,
				void *			voided,
				bmWriteBitmap		writeBitmap )
    {
    int				rval= 0;
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    if  ( ! td->tdCopiedImage )
	{ XDEB(td->tdCopiedImage); rval= -1; goto ready;	}

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( (*writeBitmap)( &(td->tdCopiedImage->riDescription),
					td->tdCopiedImage->riBytes, sos ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:
    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

APP_GIVE_COPY( tedCopyClipboardPng, w, event, voided )
    {
    if  ( tedCopyImageFile( w, event, voided, bmPngWritePng ) )
	{ LDEB(1);	}

    return;
    }

APP_GIVE_COPY( tedCopyClipboardJfif, w, event, voided )
    {
    if  ( tedCopyImageFile( w, event, voided, bmJpegWriteJfif ) )
	{ LDEB(1);	}

    return;
    }

APP_GIVE_COPY( tedCopyFontTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedFont) ) )
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

/************************************************************************/
/*									*/
/*  'Copy Font' menu option.						*/
/*									*/
/************************************************************************/

void tedCopyFont( EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			rtfFlags= RTFflagNO_BOOKMARKS|RTFflagSAVE_SOMETHING;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  2  */
    sos= sioOutMemoryOpen( &(td->tdCopiedFont) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( docRtfSaveDocument( sos, td->tdDocument, &ds, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, &ds,
					rtfFlags, "/tmp/savedfont.rtf" );
	}

    appDocOwnSelection( ed, "RTFFONT", TedFontTargets, TedFontTargetCount );
    }

/************************************************************************/
/*									*/
/*  Copy the ruler as it was saved on the document struct when the	*/
/*  menu option was clicked.						*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( tedCopyRulerTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedRuler) ) )
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
/*  Callbacks for the Copy/Paste ruler menu options.			*/
/*									*/
/*  1)  The ruler paste is a selection paste with the selection		*/
/*	constrained to the last position of the first paragraph in the	*/
/*	selection.							*/
/*  2)  Remember the bytes expecting that someone will ask for them.	*/
/*  3)  Tell that we have a ruler paste available.			*/
/*									*/
/************************************************************************/

void tedCopyRuler( EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsPara;

    const int			rtfFlags= RTFflagNO_BOOKMARKS|RTFflagSAVE_SOMETHING;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  1  */
    dsPara= ds;
    docTailPosition( &(dsPara.dsHead), dsPara.dsHead.dpNode );
    docSetIBarSelection( &dsPara, &(dsPara.dsHead) );

    /*  2  */
    sos= sioOutMemoryOpen( &(td->tdCopiedRuler) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( docRtfSaveDocument( sos, td->tdDocument, &dsPara, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, &dsPara,
					rtfFlags, "/tmp/savedruler.rtf" );
	}

    /*  2  */
    appDocOwnSelection( ed, "RTFRULER", TedRulerTargets, TedRulerTargetCount );
    }

/************************************************************************/

void tedManagePrimarySelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! tedHasIBarSelection( ed ) )
	{
	if  ( appDocOwnSelection( ed, "PRIMARY",
					    TedPrimaryTargets,
					    TedPrimaryTargetCount ) )
	    { LDEB(1);				}
	else{ td->tdOwnsPrimarySelection= 1;	}
	}
    else{
	if  ( td->tdOwnsPrimarySelection )
	    {
	    appDocReleaseSelection( ed, "PRIMARY" );
	    td->tdOwnsPrimarySelection= 0;
	    }
	}

    return;
    }

