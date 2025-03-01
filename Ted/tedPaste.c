/************************************************************************/
/*									*/
/*  Ted: Paste: Consuming content from a copying application		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedAppResources.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedEdit.h"
#   include	"tedDocument.h"
#   include	"tedDocMenu.h"
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<sioXprop.h>
#   include	<bmio.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<sioGeneral.h>
#   include	<docDocument.h>
#   include	<bitmap.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Finish a paste action.						*/
/*									*/
/************************************************************************/

typedef struct BufferDocument * (*TED_READ) (
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg );

typedef int	(*TED_INCLUDE) (	EditDocument *		ed,
					struct BufferDocument *	bd,
					int			traced );

static struct BufferDocument * tedPasteRtfReadFile(
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg )
    {
    const unsigned int	rtfFlags= 0;

    return docRtfReadFile( sis, rtfFlags );
    }

static struct BufferDocument * tedPastePlainReadFile(
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg )
    {
    int		longestPara;

    return docPlainReadFile( sis, &longestPara, dg );
    }

static int tedPasteClipboardGeneric(	APP_WIDGET		w,
					EditDocument *		ed,
					APP_SELECTION_EVENT *	event,
					TED_READ		readDoc,
					TED_INCLUDE		includeDoc )
    {
    int				rval= 0;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleInputStream *		sis= (SimpleInputStream *)0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    tedDetermineDefaultSettings( tar );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    bd= (*readDoc)( sis, ea, &(ea->eaDefaultDocumentGeometry) );

    if  ( ! bd )
	{ XDEB( bd ); rval= -1; goto ready; }

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	const int			rtfFlags= 0;
	const DocumentSelection * const	ds= (const DocumentSelection *)0;

	tedSaveSelectionToFile( bd, ds, rtfFlags, "/tmp/included.rtf" );
	}

    if  ( (*includeDoc)( ed, bd, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}
    if  ( sis )
	{ sioInClose( sis );		}

    return rval;
    }

APP_PASTE_REPLY( tedPasteClipboardRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedIncludeRtfDocument );

    }

APP_PASTE_REPLY( tedPasteClipboardString, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPastePlainReadFile, tedIncludePlainDocument );
    }

APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPastePlainReadFile, tedIncludePlainDocument );
    }

static int tedPasteImageFile(	APP_WIDGET		w,
				APP_SELECTION_EVENT *	event,
				EditDocument *		ed,
				bmReadBitmap		readBitmap,
				const char *		nameHint )
    {
    int			rval= 0;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleInputStream *	sis= (SimpleInputStream *)0;
    RasterImage *	ri= (RasterImage *)0;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &filename, nameHint ) )
	{ SDEB(nameHint); rval= -1; goto ready;	}

    ri= (RasterImage *)malloc( sizeof(RasterImage) );
    if  ( ! ri )
	{ XDEB(ri); rval= -1; goto ready;	}
    bmInitRasterImage( ri );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    if  ( (*readBitmap)( &(ri->riDescription), &(ri->riBytes), sis ) )
	{ LDEB(1); rval= -1; goto ready;	}
    ri->riFormat= bmSuggestFormat( &(filename),
					ri->riFormat, &(ri->riDescription) );

    if  ( tedReplaceSelectionWithRasterImage( ed, ri, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready; }

    ri= (RasterImage *)0; /* stolen */

  ready:

    utilCleanMemoryBuffer( &filename );

    if  ( ri )
	{ bmFreeRasterImage( ri );	}
    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

APP_PASTE_REPLY( tedPasteClipboardPng, w, event, voided )
    {
    if  ( tedPasteImageFile( w, event, (EditDocument *)voided,
						bmPngReadPng, "some.png" ) )
	{ LDEB(1);	}

    return;
    }

APP_PASTE_REPLY( tedPasteClipboardJfif, w, event, voided )
    {
    if  ( tedPasteImageFile( w, event, (EditDocument *)voided,
						bmJpegReadJfif, "some.jpeg" ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/

APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedApplyPastedRuler );
    return;
    }

/************************************************************************/

APP_PASTE_REPLY( tedPasteFontTed, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedApplyPastedFont );
    return;
    }
