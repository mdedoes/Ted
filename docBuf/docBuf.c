/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<utilTree.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docTreeNode.h"
#   include	<docTreeType.h>
#   include	<docTextLine.h>
#   include	<textAttributeAdmin.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docItemShadingAdmin.h>
#   include	<docParaRulerAdmin.h>
#   include	"docNodeTree.h"
#   include	<utilMatchFont.h>
#   include	<docDocumentNote.h>

/************************************************************************/

void docDeleteFieldFromDocument(	BufferDocument *	bd,
					DocumentField *		df )
    {
    if  ( docFieldHasNote( df->dfKind ) && df->dfNoteIndex >= 0 )
	{
	docCleanNote( bd, bd->bdNotesList.nlNotes+ df->dfNoteIndex );
	docInitNote( bd->bdNotesList.nlNotes+ df->dfNoteIndex );
	}

    docDeleteFieldFromList( &(bd->bdFieldList), df );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean and free a whole document.					*/
/*									*/
/************************************************************************/

void docFreeDocument( BufferDocument *	bd )
    {
    int		i;

    docCleanDocumentTree( bd, &(bd->bdBody) );

    for ( i= 0; i < bd->bdNotesList.nlNoteCount; i++ )
	{ docCleanNote( bd, &(bd->bdNotesList.nlNotes[i]) );	}

    docCleanDocumentTree( bd, &(bd->bdEiFtnsep) );
    docCleanDocumentTree( bd, &(bd->bdEiFtnsepc) );
    docCleanDocumentTree( bd, &(bd->bdEiFtncn) );

    docCleanDocumentTree( bd, &(bd->bdEiAftnsep) );
    docCleanDocumentTree( bd, &(bd->bdEiAftnsepc) );
    docCleanDocumentTree( bd, &(bd->bdEiAftncn) );

    docCleanStyleSheet( &(bd->bdStyleSheet) );
    docCleanFieldList( &(bd->bdFieldList) );
    docCleanShapeList( &(bd->bdShapeList) );
    docCleanObjectList( &(bd->bdObjectList) );

    docCleanDocumentProperties( &(bd->bdProperties) );

    if  ( bd->bdNotesList.nlNotes )
	{ free( bd->bdNotesList.nlNotes );	}

    if  ( bd->bdSeqFieldIdentifiers )
	{
	utilTreeFreeTree( bd->bdSeqFieldIdentifiers,
					    utilTreeFreeValue, (void *)0 );
	}

    utilCleanNumberedPropertiesList( &(bd->bdTextAttributeList) );
    utilCleanNumberedPropertiesList( &(bd->bdBorderPropertyList) );
    utilCleanNumberedPropertiesList( &(bd->bdItemShadingList) );
    utilCleanNumberedPropertiesList( &(bd->bdFramePropertyList) );
    utilCleanNumberedPropertiesList( &(bd->bdTabStopListList) );

    free( bd );
    }

/************************************************************************/

int docGetDefaultFont(		BufferDocument *	bd )
    {
    int				i;
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);

    if  ( bd->bdProperties.dpDefaultFont >= 0 )
	{ return bd->bdProperties.dpDefaultFont;	}

    for ( i= 0; i < dfl->dflFontCount; i++ )
	{
	if  ( docFontListGetFontByNumber( dfl, i ) )
	    {
	    bd->bdProperties.dpDefaultFont= i;
	    return bd->bdProperties.dpDefaultFont;
	    }
	}

    /*LDEB(bd->bdProperties.dpDefaultFont);*/
    return bd->bdProperties.dpDefaultFont;
    }

/************************************************************************/
/*									*/
/*  Initialise a BufferDocument.					*/
/*									*/
/************************************************************************/

static void docInitDocument(	BufferDocument *	bd )
    {
    utilInitTextAttributeList( &(bd->bdTextAttributeList) );
    docInitBorderPropertyList( &(bd->bdBorderPropertyList) );
    docInitItemShadingList( &(bd->bdItemShadingList) );
    docInitFramePropertyList( &(bd->bdFramePropertyList) );
    docInitTabStopListList( &(bd->bdTabStopListList) );

    docInitObjectList( &(bd->bdObjectList) );

    docInitDocumentProperties( &(bd->bdProperties) );

    docInitDocumentTree( &(bd->bdBody) );

    docInitStyleSheet( &(bd->bdStyleSheet) );
    docInitFieldList( &(bd->bdFieldList) );
    docInitShapeList( &(bd->bdShapeList) );

    bd->bdNotesList.nlNotes= (DocumentNote *)0;
    bd->bdNotesList.nlNoteCount= 0;

    bd->bdSeqFieldIdentifiers= (void *)0;

    docInitDocumentTree( &(bd->bdEiFtnsep) );
    docInitDocumentTree( &(bd->bdEiFtnsepc) );
    docInitDocumentTree( &(bd->bdEiFtncn) );

    docInitDocumentTree( &(bd->bdEiAftnsep) );
    docInitDocumentTree( &(bd->bdEiAftnsepc) );
    docInitDocumentTree( &(bd->bdEiAftncn) );

    bd->bdAutoHyphenate= 0;

    return;
    }

/************************************************************************/

void docGetColorByNumber(	RGB8Color *		rgb8,
				const BufferDocument *	bd,
				int			colorNumber )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const ColorPalette *	cp= &(dp->dpColorPalette);

    rgb8->rgb8Red= 0;
    rgb8->rgb8Green= 0;
    rgb8->rgb8Blue= 0;
    rgb8->rgb8Alpha= 255;

    if  ( colorNumber != 0 )
	{
	if  ( colorNumber < 1			||
	      colorNumber >= cp->cpColorCount	)
	    { LLDEB(colorNumber,cp->cpColorCount);	}
	else{ *rgb8= cp->cpColors[colorNumber];		}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the body of the document tree in a fresh document.		*/
/*									*/
/************************************************************************/

static int docSetupDocumentBody(	BufferDocument *	bd )
    {
    bd->bdBody.dtRoot= docMakeNode();
    if  ( ! bd->bdBody.dtRoot )
	{ XDEB(bd->bdBody.dtRoot); return -1;	}

    docInitNode( bd->bdBody.dtRoot, (struct BufferItem *)0, bd,
						0, DOClevBODY, DOCinBODY );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Create a fresh document.						*/
/*									*/
/************************************************************************/

BufferDocument * docNewDocument( void )
    {
    BufferDocument *	bd= (BufferDocument *)malloc( sizeof(BufferDocument) );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}

    docInitDocument( bd );

    if  ( docSetupDocumentBody( bd ) )
	{ LDEB(1); docFreeDocument( bd ); return (BufferDocument *)0;	}

    return bd;
    }

/************************************************************************/
/*									*/
/*  Make a new document consisting of one paragraph with one empty	*/
/*  particule. This is the starting point for editing and for reading	*/
/*  plain text documents.						*/
/*									*/
/************************************************************************/

BufferDocument * docNewFile(	TextAttribute *			taDefault,
				const char *			defaultFontName,
				int				defaultFontSize,
				const PostScriptFontList *	psfl,
				const DocumentGeometry *	dg )
    {
    BufferDocument *		bd;
    DocumentProperties *	dp;

    PropertyMask		taNewMask;
    TextAttribute		taNew;
    int				textAttributeNumber;

    BufferItem *		bi;

    utilPropMaskClear( &taNewMask );

    bd= docNewDocument();
    if  ( ! bd )
	{ XDEB(bd); return (BufferDocument *)0;	}

    dp= &(bd->bdProperties);
    dp->dpGeometry= *dg;

    /*  3  */
    if  ( psfl )
	{
	if  ( utilAddPsFontsToDocList( &(dp->dpFontList), psfl ) )
	    { LDEB(psfl->psflFamilyCount); goto failed;	}
	}
    else{
	if  ( utilAddBase35FontsToDocList( &(dp->dpFontList) ) )
	    { LDEB(35); goto failed;	}
	}

    utilInitTextAttribute( &taNew );
    taNew.taFontNumber= docGetFontByName( &(dp->dpFontList), defaultFontName );
    if  ( taNew.taFontNumber < 0 )
	{ LDEB(taNew.taFontNumber); goto failed;	}
    PROPmaskADD( &taNewMask, TApropFONT_NUMBER );

    if  ( defaultFontSize < 6 || defaultFontSize > 100 )
	{ LDEB(defaultFontSize); defaultFontSize= 24;	}
    taNew.taFontSizeHalfPoints= defaultFontSize;
    PROPmaskADD( &taNewMask, TApropFONTSIZE );

    dp->dpDefaultFont= taNew.taFontNumber;

    textAttributeNumber= utilTextAttributeNumber(
				    &(bd->bdTextAttributeList), &taNew );
    if  ( textAttributeNumber < 0 )
	{ SLDEB(defaultFontName,textAttributeNumber); goto failed;	}

    bi= docInsertNode( bd, bd->bdBody.dtRoot, -1, DOClevSECT );
    if  ( ! bi )
	{ XDEB(bi); goto failed;	}

    bi->biSectDocumentGeometry= dp->dpGeometry;

    bi= docInsertEmptyParagraph( bd, bi, textAttributeNumber );
    if  ( ! bi )
	{ XDEB(bi); goto failed;	}

    *taDefault= taNew;
    return bd;

  failed:

    docFreeDocument( bd );

    return (BufferDocument *)0;
    }

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

static void docCollectItemStatistics(	DocumentStatistics *	ds,
					const BufferItem *	bi )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevBODY:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ docCollectItemStatistics( ds, bi->biChildren[i] ); }
	    break;

	case DOClevPARA:
	    ds->dsParagraphCount++;
	    ds->dsCharacterCount += docParaStrlen( bi );
	    ds->dsLineCount += bi->biParaLineCount;

	    for ( i= 0; i < bi->biParaLineCount; i++ )
		{ ds->dsWordCount += bi->biParaLines[i].tlWordCount;	}

	    break;

	default:
	    LDEB(bi->biLevel); return;
	}

    return;
    }

void docCollectDocumentStatistics(	DocumentStatistics *	ds,
					const BufferDocument *	bd )
    {
    docInitDocumentStatistics( ds );

    docCollectItemStatistics( ds, bd->bdBody.dtRoot );

    ds->dsPageCount= bd->bdBody.dtRoot->biBelowPosition.lpPage+ 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine a text attribute number for a particule.			*/
/*									*/
/************************************************************************/

int docTextAttributeNumber(	BufferDocument *	bd,
				const TextAttribute *	ta )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    int				textAttributeNumber;

    TextAttribute		taCopy= *ta;

    if  ( taCopy.taFontNumber < 0			||
	  taCopy.taFontNumber >= dfl->dflFontCount	)
	{
	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLDEB(taCopy.taFontNumber,dfl->dflFontCount);	}

	taCopy.taFontNumber= docGetDefaultFont( bd );

	if  ( taCopy.taFontNumber < 0 )
	    { taCopy.taFontNumber= 0;	}

	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLDEB(taCopy.taFontNumber,dfl->dflFontCount);	}
	}

    textAttributeNumber= utilTextAttributeNumber(
				    &(bd->bdTextAttributeList), &taCopy );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    return textAttributeNumber;
    }

const char * docGetEncodingName(
				BufferDocument *		bd,
				TextAttribute *			ta,
				int				charset )
    {
    const char *		encodingName= (const char *)0;

    const DocumentFont *	df;

    df= docRtfGetCurrentFont( bd, ta );
    if  ( df )
	{ encodingName= utilGetEncodingName( df->dfName, charset );	}

    return encodingName;
    }

/************************************************************************/
/*									*/
/*  Determine the plain text attribute for this document.		*/
/*									*/
/************************************************************************/

void docPlainTextAttribute(		TextAttribute *		ta,
					BufferDocument *	bd )
    {
    utilInitTextAttribute( ta );
    if  ( bd )
	{ ta->taFontNumber= docGetDefaultFont( bd );	}
    ta->taFontSizeHalfPoints= 24;

    return;
    }

/************************************************************************/

const DocumentFont * docRtfGetCurrentFont(
				BufferDocument *		bd,
				TextAttribute *			ta )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);
    const DocumentFont *	df;

    if  ( ta->taFontNumber < 0 )
	{
	/*LDEB(ta->taFontNumber);*/
	ta->taFontNumber= docGetDefaultFont( bd );
	}

    df= docFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{
	if  ( ta->taFontNumber >= 0 )
	    { LXDEB(ta->taFontNumber,df);	}
	else{
	    ta->taFontNumber= docGetDefaultFont( bd );
	    df= docFontListGetFontByNumber( dfl, ta->taFontNumber );
	    }
	}

    return df;
    }

/************************************************************************/
/*									*/
/*  Determine the number of the attribute belonging to the scaps	*/
/*  font size. Note that this is used for the shifted lower case	*/
/*  letters only.							*/
/*									*/
/************************************************************************/

int docScapsAttributeNumber(		BufferDocument *	bd,
					const TextAttribute *	ta )
    {
    TextAttribute	taScaps= *ta;

    taScaps.taFontSizeHalfPoints= SCAPS_SIZE( ta->taFontSizeHalfPoints );
    taScaps.taSmallCaps= 0;

    return utilTextAttributeNumber( &(bd->bdTextAttributeList), &taScaps );
    }

/************************************************************************/

void docDeleteDrawingShape(	struct BufferDocument *	bd,
				DrawingShape *		ds )
    {
    int		i;

    for ( i= 0; i < ds->dsChildCount; i++ )
	{
	if  ( ds->dsChildren[i] )
	    {
	    docDeleteDrawingShape( bd, ds->dsChildren[i] );
	    ds->dsChildren[i]= (DrawingShape *)0;
	    }
	}

    docEraseDocumentTree( bd, &(ds->dsDocumentTree) );

    /* Done from the paged list: docCleanDrawingShape( bd, ds ); */
    docDeleteShapeFromList( &(bd->bdShapeList), ds );
    }

