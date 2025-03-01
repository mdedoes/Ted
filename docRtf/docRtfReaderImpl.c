/************************************************************************/
/*									*/
/*  Reading of Rtf files. (Various utility functions.)			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<utilDateTime.h>
#   include	<textOfficeCharset.h>
#   include	<textConverter.h>
#   include	<docShape.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reset the attributes in an RtfReader to the defaults.		*/
/*									*/
/************************************************************************/

void docRtfResetParagraphReadingState(	RtfReader *	rr )
    {
    rr->rrAfterNoteref= 0;
    rr->rrParaHeadFieldTailOffset= 0; /* Does no harm without one */
    rr->rrAfterInlineShape= 0;
    rr->rrInlineShapeObjectNumber= -1;
    }

static void docRtfInitReader(	RtfReader *	rr )
    {
    rr->rrGotText= (RtfGotText)0;
    rr->rrInputStream= (struct SimpleInputStream *)0;

    rr->rrState= (RtfReadingState *)0;
    rr->rrTreeStack= (struct RtfTreeStack *)0;
    rr->rrInIgnoredGroup= 0;

    rr->rrComplainUnknown= 1;
    rr->rrCharacterAhead= EOF;

    docRtfResetParagraphReadingState( rr );

    rr->rrReadFlags= 0;

				/*
				 *  The current position in the input.
				 */
    rr->rrBytesRead= 0;
    rr->rrCurrentLine= 1;

				/*
				 *  The current position in the document that 
				 *  we are collecting.
				 */
    rr->rrDocument= (struct BufferDocument *)0;

    docInitFrameProperties( &(rr->rrRowFrameProperties) );

    docInitCellProperties( &(rr->rrCellProperties) );
    utilPropMaskClear( &(rr->rrCellPropertyMask) );
    docInitItemShading( &(rr->rrCellShading) );

    docInitSectionProperties( &(rr->rrSectionProperties) );
    rr->rrSectionColumn= 0;
    docInitItemShading( &(rr->rrRowShading) );
    utilPropMaskClear( &(rr->rrRowPropertyMask) );

    docInitDocumentList( &(rr->rrcDocumentList) );
    docInitListOverride( &(rr->rrcListOverride) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitListOverrideLevel( &(rr->rrcListOverrideLevel) );
    utilInitMemoryBuffer( &(rr->rrcListLevelText) );
    utilInitMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docInitBorderProperties( &(rr->rrBorderProperties) );

    rr->rrDrawingShape= (struct DrawingShape *)0;
    rr->rrShapeProperty= (struct RtfControlWord *)0;
    utilInitMemoryBuffer( &(rr->rrShapePropertyName) );
    utilInitMemoryBuffer( &(rr->rrShapePropertyValue) );
    rr->rrNextObjectVertex= 0;
    docInitTabStop( &(rr->rrTabStop) );

    rr->rrColor.rgb8Red= rr->rrColor.rgb8Green= rr->rrColor.rgb8Blue= 0;
    rr->rrGotColorComponent= 0;

    utilInvalidateTime( &(rr->rrTm) );
				/*
				 *  Document properties
				 */
    fontInitDocumentFont( &(rr->rrCurrentFont) );
    utilInitPagedList( &(rr->rrEncodedFontList) );
    fontInitEncodedFont( &(rr->rrCurrentEncodedFont) );
    utilStartPagedList( &(rr->rrEncodedFontList),
				sizeof(EncodedFont),
				(InitPagedListItem)fontInitEncodedFont,
				(CleanPagedListItem)0 );

    rr->rrDefaultFont= -1;
    rr->rrDefaultFontDbch= -1;
    rr->rrDefaultFontLoch= -1;
    rr->rrDefaultFontHich= -1;
    rr->rrDefaultFontBi= -1;

    docInitDocumentStyle( &(rr->rrStyle) );
    docInitDocumentProperties( &(rr->rrDocumentProperties) );
    utilPropMaskClear( &(rr->rrDocPropertyMask) );

    rr->rrInsertedObject= (struct InsertedObject *)0;
    docInitPictureProperties( &(rr->rrPictureProperties) );
    utilPropMaskClear( &(rr->rrPicturePropMask) );

    utilInitMemoryBuffer( &(rr->rrcBookmark) );

    rr->rrParagraphBreakOverride= -1;

    rr->rrGotDocGeometry= 0;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/

    rr->rrRtfTextConverter= (struct TextConverter *)0;

    docInitNoteProperties( &(rr->rrNoteProperties) );
    utilPropMaskClear( &(rr->rrNotePropertyMask) );

    docInitFormField( &(rr->rrFormField) );

				/* No trace reader */
    rr->rrTraceReader= (struct RtfTraceReader *)0;
    }

static void docRtfCleanReader(	RtfReader *	rr )
    {
    while( rr->rrState )
	{
	XDEB(rr->rrState);
	docRtfPopReadingState( rr );
	}

    while( rr->rrTreeStack )
	{
	XDEB(rr->rrTreeStack);
	docRtfPopTreeStack( rr );
	}

    /*docCleanCellProperties( &(rr->rrCellProperties) );*/
    docCleanSectionProperties( &(rr->rrSectionProperties) );
    /*docCleanShapeProperties( &(rr->rrcShapeProperties) );*/

    if  ( rr->rrDrawingShape )
	{ docDeleteDrawingShape( rr->rrDocument, rr->rrDrawingShape ); }

    fontCleanDocumentFont( &(rr->rrCurrentFont) );
    utilCleanPagedList( &(rr->rrEncodedFontList) );

    docCleanDocumentList( &(rr->rrcDocumentList) );
    docCleanListOverride( &(rr->rrcListOverride) );
    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docCleanListOverrideLevel( &(rr->rrcListOverrideLevel) );
    utilCleanMemoryBuffer( &(rr->rrcListLevelText) );
    utilCleanMemoryBuffer( &(rr->rrcListLevelNumbers) );

    docCleanDocumentStyle( &(rr->rrStyle) );
    docCleanDocumentProperties( &(rr->rrDocumentProperties) );

    utilCleanMemoryBuffer( &(rr->rrcBookmark) );

    docCleanNoteProperties( &(rr->rrNoteProperties) );

    docCleanFormField( &(rr->rrFormField) );

    if  ( rr->rrRtfTextConverter )
	{ textCloseTextConverter( rr->rrRtfTextConverter );	}
    }

void docRtfPushReadingState(	RtfReader *		rr,
				RtfReadingState *	rrs )
    {
    struct BufferDocument *		bd= rr->rrDocument;

    docPlainTextAttribute( &(rrs->rrsTextAttribute), bd );
    rrs->rrsTextCharset= -1;
    rrs->rrsTextRToL= 0;
    docInitItemShading( &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitTabStopList( &(rrs->rrsTabStopList) );
    docInitItemShading( &(rrs->rrsParagraphShading) );
    docInitFrameProperties( &(rrs->rrsParaFrameProperties) );

    utilInitMemoryBuffer( &(rrs->rrsSavedText) );

    rrs->rrsBytesPerUnicode= 1;
    rrs->rrsUnicodeBytesToSkip= 0;

    rrs->rrsWidth= 0;
    rrs->rrsHeight= 0;

    if  ( rr->rrState )
	{
	const RtfReadingState *	above= rr->rrState;

	rrs->rrsBytesPerUnicode= above->rrsBytesPerUnicode;
	rrs->rrsUnicodeBytesToSkip= 0;

	rrs->rrsTextAttribute= above->rrsTextAttribute;
	rrs->rrsTextCharset= above->rrsTextCharset;
	rrs->rrsTextRToL= above->rrsTextRToL;

	rrs->rrsTextShading= above->rrsTextShading;
	rrs->rrsTextShadingChanged= above->rrsTextShadingChanged;

	docCopyParagraphProperties( &(rrs->rrsParagraphProperties),
				    &(above->rrsParagraphProperties) );
	docCopyTabStopList( &(rrs->rrsTabStopList), &(above->rrsTabStopList) );
	rrs->rrsParagraphShading= above->rrsParagraphShading;
	}

    rrs->rrsPrev= rr->rrState;
    rr->rrState= rrs;

    return;
    }

void docRtfPopReadingState(	RtfReader *	rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( ! rrs )
	{ XDEB(rrs); return;	}

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docCleanTabStopList( &(rrs->rrsTabStopList) );

    /*  Copy unused data in child to parent */
    if  ( rrs->rrsPrev )
	{
	if  ( utilMemoryAppendBuffer( &(rrs->rrsPrev->rrsSavedText),
						    &(rrs->rrsSavedText) ) )
	    { LDEB(rrs->rrsSavedText.mbSize);	}
	}

    utilCleanMemoryBuffer( &(rrs->rrsSavedText) );

    /**/
    rr->rrState= rrs->rrsPrev;
    if  ( rr->rrState )
	{ rr->rrState->rrsUnicodeBytesToSkip= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Open a reader.							*/
/*									*/
/************************************************************************/

RtfReader * docRtfOpenReader(	struct SimpleInputStream *	sis,
				struct BufferDocument *		bd,
				int				flags )
    {
    RtfReader *		rval= (RtfReader *)0;
    RtfReader *		rr= (RtfReader *)0;

    rr= (RtfReader *)malloc(sizeof(RtfReader) );
    if  ( ! rr )
	{ PDEB(rr); goto ready;	}

    docRtfInitReader( rr );

    rr->rrDocument= bd;
    rr->rrInputStream= sis;
    rr->rrReadFlags= flags;

    if  ( docRtfReadSetupTextConverters( rr ) )
	{ LDEB(1); goto ready;	}

    rval= rr; rr= (RtfReader *)0; /* steal */

  ready:

    if  ( rr )
	{ docRtfCloseReader( rr );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close a reader.							*/
/*									*/
/************************************************************************/

void docRtfCloseReader(		RtfReader *		rr )
    {
    docRtfCleanReader( rr );
    free( rr );

    return;
    }
