/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<utilTree.h>

#   include	"docBuf.h"
#   include	"docAttributes.h"
#   include	"docDocument.h"
#   include	"docShape.h"
#   include	"docTreeNode.h"
#   include	"docNotes.h"
#   include	"docObjects.h"
#   include	<docTreeType.h>
#   include	<textAttributeAdmin.h>
#   include	"docNodeTree.h"
#   include	<fontMatchFont.h>
#   include	<docPropertiesAdmin.h>
#   include	<docDocumentProperties.h>
#   include	<utilPropMask.h>
#   include	<docSectProperties.h>
#   include	<textAttribute.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clean and free a whole document.					*/
/*									*/
/************************************************************************/

void docFreeDocument( struct BufferDocument *	bd )
    {
    int		i;

    docCleanDocumentObjects( bd );

    docCleanDocumentTree( bd, &(bd->bdBody) );

    for ( i= 0; i < bd->bdNotesList.nlNoteCount; i++ )
	{ docDeleteNote( bd, i );	}

    docCleanDocumentTree( bd, &(bd->bdFtnsep) );
    docCleanDocumentTree( bd, &(bd->bdFtnsepc) );
    docCleanDocumentTree( bd, &(bd->bdFtncn) );

    docCleanDocumentTree( bd, &(bd->bdAftnsep) );
    docCleanDocumentTree( bd, &(bd->bdAftnsepc) );
    docCleanDocumentTree( bd, &(bd->bdAftncn) );

    docCleanStyleSheet( &(bd->bdStyleSheet) );
    docCleanFieldList( &(bd->bdFieldList) );
    docCleanShapeList( &(bd->bdShapeList) );
    docCleanObjectList( &(bd->bdObjectList) );

    if  ( bd->bdProperties )
	{
	docCleanDocumentProperties( bd->bdProperties );
	free( bd->bdProperties );
	}

    if  ( bd->bdNotesList.nlNotes )
	{ free( bd->bdNotesList.nlNotes );	}

    if  ( bd->bdSeqFieldIdentifiers )
	{
	utilTreeFreeTree( bd->bdSeqFieldIdentifiers,
					    utilTreeFreeValue, (void *)0 );
	}

    if  ( bd->bdPropertyLists )
	{ docFreeDocumentPropertyLists( bd->bdPropertyLists );	}

    free( bd );
    }

/************************************************************************/
/*									*/
/*  Initialise a struct BufferDocument.					*/
/*									*/
/************************************************************************/

static void docInitDocument(	struct BufferDocument *	bd )
    {
    bd->bdPropertyLists= (DocumentPropertyLists *)0;

    docInitObjectList( &(bd->bdObjectList) );

    bd->bdProperties= (struct DocumentProperties *)0;

    docInitDocumentTree( &(bd->bdBody) );

    docInitStyleSheet( &(bd->bdStyleSheet) );
    docInitFieldList( &(bd->bdFieldList) );
    docInitShapeList( &(bd->bdShapeList) );

    bd->bdNotesList.nlNotes= (struct DocumentNote **)0;
    bd->bdNotesList.nlNoteCount= 0;

    bd->bdSeqFieldIdentifiers= (void *)0;

    docInitDocumentTree( &(bd->bdFtnsep) );
    docInitDocumentTree( &(bd->bdFtnsepc) );
    docInitDocumentTree( &(bd->bdFtncn) );

    docInitDocumentTree( &(bd->bdAftnsep) );
    docInitDocumentTree( &(bd->bdAftnsepc) );
    docInitDocumentTree( &(bd->bdAftncn) );

    bd->bdLocaleId= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the body of the document tree in a fresh document.		*/
/*									*/
/************************************************************************/

static int docSetupDocumentBody(	struct BufferDocument *	bd )
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

struct BufferDocument * docNewDocument( const struct BufferDocument *	bdRef )
    {
    struct BufferDocument *	rval= (struct BufferDocument *)0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    bd= (struct BufferDocument *)malloc( sizeof(struct BufferDocument) );
    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    docInitDocument( bd );
    bd->bdProperties= malloc( sizeof(DocumentProperties) );
    if  ( ! bd->bdProperties )
	{ XDEB(bd->bdProperties); goto ready;	}

    docInitDocumentProperties( bd->bdProperties );

    if  ( bdRef )
	{ bd->bdPropertyLists= bdRef->bdPropertyLists;	}
    else{
	bd->bdPropertyLists= docMakeDocumentPropertyLists();
	if  ( ! bd->bdPropertyLists )
	    { XDEB(bd->bdPropertyLists); goto ready;	}
	}

    bd->bdProperties->dpFontList= &(bd->bdPropertyLists->dplFontList);
    bd->bdProperties->dpListAdmin= &(bd->bdPropertyLists->dplListAdmin);
    bd->bdProperties->dpColorPalette= &(bd->bdPropertyLists->dplColorPalette);

    docDefaultFootEndNotesProperties( &(bd->bdProperties->dpNotesProps) );

    if  ( docSetupDocumentBody( bd ) )
	{ LDEB(1); goto ready;	}

    rval= bd; bd= (struct BufferDocument *)0; /* steal */

  ready:

    if  ( bd )
	{
	if  ( bdRef )
	    { bd->bdPropertyLists= (DocumentPropertyLists *)0;	}

	docFreeDocument( bd );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a new document consisting of one paragraph with one empty	*/
/*  particule. This is the starting point for editing and for reading	*/
/*  plain text documents.						*/
/*									*/
/************************************************************************/

struct BufferDocument * docNewFile(
			TextAttribute *			taDefault,
			const char *			defaultFontName,
			int				defaultFontSize,
			const struct PostScriptFontList * psfl,
			const DocumentGeometry *	dg )
    {
    NumberedPropertiesList *	taList;
    struct BufferDocument *	bd;
    DocumentProperties *	dp;

    PropertyMask		taNewMask;
    TextAttribute		taNew;
    int				textAttributeNr;

    struct BufferItem *		sectNode;
    struct BufferItem *		paraNode;

    utilPropMaskClear( &taNewMask );

    bd= docNewDocument( (struct BufferDocument *)0 );
    if  ( ! bd )
	{ XDEB(bd); return (struct BufferDocument *)0;	}

    dp= bd->bdProperties;
    dp->dpGeometry= *dg;
    taList= &(bd->bdPropertyLists->dplTextAttributeList);

    /*  3  */
    if  ( psfl )
	{
	if  ( fontAddPsFontsToDocList( dp->dpFontList, psfl ) )
	    { LDEB(1); goto failed;	}
	}
    else{
	if  ( fontAddBase35FontsToDocList( dp->dpFontList ) )
	    { LDEB(35); goto failed;	}
	}

    textInitTextAttribute( &taNew );
    taNew.taFontNumber= docGetFontByName( bd, defaultFontName );
    if  ( taNew.taFontNumber < 0 )
	{ LDEB(taNew.taFontNumber); goto failed;	}
    PROPmaskADD( &taNewMask, TApropFONT_NUMBER );

    if  ( defaultFontSize < 6 || defaultFontSize > 100 )
	{ LDEB(defaultFontSize); defaultFontSize= 24;	}
    taNew.taFontSizeHalfPoints= defaultFontSize;
    PROPmaskADD( &taNewMask, TApropFONTSIZE );

    dp->dpDefaultFont= taNew.taFontNumber;

    textAttributeNr= textTextAttributeNumber( taList, &taNew );
    if  ( textAttributeNr < 0 )
	{ SLDEB(defaultFontName,textAttributeNr); goto failed;	}

    sectNode= docInsertNode( bd, bd->bdBody.dtRoot, -1, DOClevSECT );
    if  ( ! sectNode )
	{ XDEB(sectNode); goto failed;	}

    sectNode->biSectDocumentGeometry= dp->dpGeometry;

    paraNode= docAppendParagraph( bd, sectNode, textAttributeNr );
    if  ( ! paraNode )
	{ XDEB(paraNode); goto failed;	}

    *taDefault= taNew;
    return bd;

  failed:

    docFreeDocument( bd );

    return (struct BufferDocument *)0;
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

/************************************************************************/

void docOverridePaperSize(	struct BufferDocument *		bd,
				const DocumentGeometry *	dgFrom )
    {
    DocumentProperties *	dp= bd->bdProperties;
    int			sect;

    geoOverridePaperSize( &(dp->dpGeometry), dgFrom );

    for ( sect= 0; sect < bd->bdBody.dtRoot->biChildCount; sect++ )
	{
	struct BufferItem *	sectNode= bd->bdBody.dtRoot->biChildren[sect];

	geoOverridePaperSize( &(sectNode->biSectDocumentGeometry), dgFrom );
	}

    return;
    }
