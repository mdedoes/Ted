/************************************************************************/
/*									*/
/*  Ted: Copy/Paste implementation.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedSelect.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedEditOperation.h"
#   include	"tedDocument.h"
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docRtfFlags.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>
#   include	<docObject.h>
#   include	<docParaProperties.h>
#   include	<docDocumentCopyJob.h>
#   include	<sioFileio.h>
#   include	<sioMemory.h>
#   include	<appEditDocument.h>
#   include	<sioGeneral.h>
#   include	<docBuf.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save the picture of an object for Copy/Paste.			*/
/*									*/
/************************************************************************/

static int tedSaveObjectPicture(	RasterImage **		riTo,
					const InsertedObject *	io )
    {
    if  ( ! io->ioRasterImage.riBytes )
	{ LXDEB(io->ioKind,io->ioRasterImage.riBytes); return -1;	}

    if  ( ! * riTo )
	{
	*riTo= malloc( sizeof(RasterImage) );
	if  ( ! * riTo )
	    { XDEB(*riTo); return -1;	}
	bmInitRasterImage( *riTo );
	}

    if  ( bmCopyRasterImage( *riTo, &(io->ioRasterImage) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write copy/paste to file for debugging purposes.			*/
/*									*/
/************************************************************************/

void tedSaveSelectionToFile(	struct BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				rtfFlags,
				const char *			filename )
    {
    SimpleOutputStream *	sos;

    SXDEB(filename,rtfFlags);
    sos= sioOutFileioOpenS( filename );
    if  ( ! sos )
	{ SPDEB( filename, sos );	}
    else{
	if  ( docRtfSaveDocument( sos, bd, ds, rtfFlags ) )
	    { LDEB(1); 	}
	if  ( sioOutClose( sos ) )
	    { LDEB(1);	}
	}

    return;
    }

void tedSaveSelectionTxtToFile(	struct BufferDocument *		bd,
				const char *			filename )
    {
    SimpleOutputStream *	sos;

    SDEB(filename);
    sos= sioOutFileioOpenS( filename );
    if  ( ! sos )
	{ SPDEB( filename, sos );	}
    else{
	if  ( docPlainSaveDocument( sos, bd, (DocumentSelection *)0, 0 ) )
	    { LDEB(1); 	}
	if  ( sioOutClose( sos ) )
	    { LDEB(1);	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save the current selection in rtf format.				*/
/*									*/
/************************************************************************/

int tedDocSaveSelectionRtf(	MemoryBuffer *		mb,
				DocumentSelection *	ds,
				SelectionDescription *	sd,
				EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    SelectionGeometry		sg;

    const int			rtfFlags= RTFflagNO_BOOKMARKS;

    if  ( ! tedHasSelection( ed ) || tedHasIBarSelection( ed ) )
	{ return -1; }

    if  ( tedGetSelection( ds, &sg, sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ return -1;	}

    sos= sioOutMemoryOpen( mb );
    if  ( ! sos )
	{ XDEB(sos); return -1;    }

    if  ( docRtfSaveDocument( sos, td->tdDocument, ds, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return -1;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, ds, rtfFlags,
							"/tmp/saved.rtf" );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the selection in order to make it available if someone asks	*/
/*  for a paste.							*/
/*									*/
/************************************************************************/

int tedDocCopySelection(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentPosition		dpObject;
    int				partObject;
    InsertedObject *		io;

    DocumentSelection		ds;
    SelectionDescription	sd;

    if  ( tedDocSaveSelectionRtf( &(td->tdCopiedSelection), &ds, &sd, ed ) )
	{ LDEB(1); return -1;	}

    if  ( td->tdCopiedImage )
	{
	bmFreeRasterImage( td->tdCopiedImage );
	td->tdCopiedImage= (struct RasterImage *)0;
	}

    docInitDocumentPosition( &dpObject );

    if  ( sd.sdIsObjectSelection					&&
	  ! docGetObjectSelection( &partObject, &dpObject, &io,
						td->tdDocument, &ds )	)
	{
	if  ( tedSaveObjectPicture( &(td->tdCopiedImage), io )	)
	    { LDEB(1);	}
	}

    return 0;
    }

/************************************************************************/

static int tedGetRulerFromPaste(	ParagraphProperties *	ppSet,
					PropertyMask *		ppSetMask,
					EditOperation *		eo,
					const MemoryBuffer *	filename,
					struct BufferDocument *	bdFrom )
    {
    int				rval= 0;

    const int			forceAttributeTo= -1;
    DocumentPosition		dp;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
					filename, forceAttributeTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDocumentHead( &dp, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilPropMaskClear( ppSetMask );
    PROPmaskADD( ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( ppSetMask, PPpropFIRST_INDENT );
    PROPmaskADD( ppSetMask, PPpropRIGHT_INDENT );
    PROPmaskADD( ppSetMask, PPpropALIGNMENT );
    PROPmaskADD( ppSetMask, PPpropTAB_STOPS );

    PROPmaskADD( ppSetMask, PPpropLISTOVERRIDE );
    PROPmaskADD( ppSetMask, PPpropOUTLINELEVEL );
    PROPmaskADD( ppSetMask, PPpropLISTLEVEL );

    if  ( docUpdParaProperties( (PropertyMask *)0, ppSet, ppSetMask,
			    dp.dpNode->biParaProperties,
			    &(dcj.dcjAttributeMap) ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the Ruler of the first paragraph in the pasted document to	*/
/*  the current selection.						*/
/*									*/
/************************************************************************/

int tedApplyPastedRuler(		EditDocument *		ed,
					struct BufferDocument *	bdFrom,
					int			traced )
    {
    int				rval= 0;

    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    docInitParagraphProperties( &ppSet );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedGetRulerFromPaste( &ppSet, &ppSetMask,
					    eo, &(ed->edFilename), bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		DOClevPARA, EDITcmdUPD_PARA_PROPS,
		(const PropertyMask *)0, (const TextAttribute *)0,
		&ppSetMask, &ppSet,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0 ))
	{ LDEB(1); rval= -1; goto ready; }

    /* tedEditChangeSelectionProperties() finishes the TedEditOperation */

  ready:

    tedCleanEditOperation( &teo );
    docCleanParagraphProperties( &ppSet );

    return rval;
    }

/************************************************************************/

static int tedGetAttributesFromPaste(	TextAttribute *		taSetTo,
					PropertyMask *		taSetMask,
					EditOperation *		eo,
					const MemoryBuffer *	filename,
					struct BufferDocument *	bdFrom )
    {
    int				rval= 0;

    const int			forceAttributeTo= -1;
    PropertyMask		taOnlyMask;

    DocumentSelection		dsAll;

    TextAttribute		taSetFrom;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
					filename, forceAttributeTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docInitDocumentSelection( &dsAll );
    if  ( docSelectWholeBody( &dsAll, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilPropMaskClear( taSetMask );

    textInitTextAttribute( &taSetFrom );
    textInitTextAttribute( taSetTo );

    docGetSelectionAttributes( &taSetFrom, taSetMask, bdFrom, &dsAll );
    docMapTextAttribute( taSetTo, &taSetFrom, &dcj );

    utilPropMaskClear( &taOnlyMask );

    PROPmaskADD( &taOnlyMask, TApropFONT_NUMBER );
    PROPmaskADD( &taOnlyMask, TApropFONTSIZE );
    PROPmaskADD( &taOnlyMask, TApropFONTBOLD );
    PROPmaskADD( &taOnlyMask, TApropFONTSLANTED );
    PROPmaskADD( &taOnlyMask, TApropTEXTUNDERLINED );
    PROPmaskADD( &taOnlyMask, TApropSTRIKETHROUGH );

    /* PROPmaskADD( &taOnlyMask, TApropSUPERSUB ); */
    /* PROPmaskADD( &taOnlyMask, TApropSMALLCAPS ); */
    /* PROPmaskADD( &taOnlyMask, TApropCAPITALS ); */

    PROPmaskADD( &taOnlyMask, TApropTEXT_COLOR );
    PROPmaskADD( &taOnlyMask, TApropTEXT_STYLE );
    PROPmaskADD( &taOnlyMask, TApropBORDER );
    PROPmaskADD( &taOnlyMask, TApropSHADING );

    utilPropMaskAnd( taSetMask, taSetMask, &taOnlyMask );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the text attributes of the first position in the pasted	*/
/*  document to the current selection.					*/
/*									*/
/************************************************************************/

int tedApplyPastedFont(		EditDocument *		ed,
				struct BufferDocument *	bdFrom,
				int			traced )
    {
    int				rval= 0;

    DocumentSelection		dsAll;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    docInitDocumentSelection( &dsAll );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedGetAttributesFromPaste( &taSet, &taSetMask,
					    eo, &(ed->edFilename), bdFrom ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		DOClevSPAN, EDITcmdUPD_SPAN_PROPS,
		&taSetMask, &taSet,
		(const PropertyMask *)0, (const ParagraphProperties *)0,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0 ))
	{ LDEB(1); rval= -1; goto ready; }

    /* tedEditChangeSelectionProperties() finishes the TedEditOperation */

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

