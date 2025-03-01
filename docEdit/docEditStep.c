/************************************************************************/
/*									*/
/*  Trace modifications to a document.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<ctype.h>

#   include	"docEditStep.h"
#   include	"docIntermediaryDocument.h"
#   include	<docTreeNode.h>
#   include	<docBuf.h>
#   include	<docDocument.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Book keeping.							*/
/*									*/
/************************************************************************/

void docInitEditStep(	EditStep *	es )
    {
    es->esCommand= -1;
    es->esPropLevel= DOClevOUT;
    es->esFieldKind= -1;

    docInitSelectionScope( &(es->esOldSelectionScope) );

    docInitEditRange( &(es->esOldEditRange) );
    es->esOldCol0= -1;
    es->esOldCol1= -1;
    es->esOldPage= -1;
    es->esOldColumn= -1;

    docInitSelectionScope( &(es->esNewSelectionScope) );

    docInitEditRange( &(es->esNewEditRange) );
    es->esNewCol0= -1;
    es->esNewCol1= -1;
    es->esNewPage= -1;
    es->esNewColumn= -1;

    es->esSelectionPosition= SELposTAIL;

    es->esSourceDocument= (struct BufferDocument *)0;
    es->esSourceIsIntermediary= 0;

    docInitDocumentStyle( &(es->esNewStyle) );
    utilPropMaskClear( &(es->esNewDocPropMask) );
    docInitDocumentProperties( &(es->esNewDocProps) );

    utilPropMaskClear( &(es->esPicturePropMask) );

    utilPropMaskClear( &(es->esNotePropMask) );

    docInitDocumentList( &(es->esDocumentList) );
    return;
    }

void docCleanEditStep(	EditStep *	es )
    {
    if  ( es->esSourceDocument )
	{
	if  ( es->esSourceIsIntermediary )
	    {
	    /* Shallow copies: Do not clean! */
	    docInitDocumentProperties( es->esSourceDocument->bdProperties );

	    docFreeIntermediaryDocument( es->esSourceDocument );
	    }
	else{
	    docFreeDocument( es->esSourceDocument );
	    }
	}

    docCleanDocumentStyle( &(es->esNewStyle) );
    docCleanDocumentProperties( &(es->esNewDocProps) );

    /* docCleanPictureProperties( &(es->esPictureProperties) ); */

    docCleanDocumentList( &(es->esDocumentList) );

    return;
    }

struct BufferDocument * docEditStepMakeSourceDocument(	EditStep *		es,
						const struct BufferDocument *	bdRef )
    {
    struct BufferDocument *	bd;

    if  ( es->esSourceDocument )
	{ XDEB(es->esSourceDocument); return (struct BufferDocument *)0;	}

    bd= docNewDocument( bdRef );
    if  ( ! bd )
	{ XDEB(bd); return (struct BufferDocument *)0;	}

    es->esSourceDocument= bd;
    es->esSourceIsIntermediary= ( bdRef != (struct BufferDocument *)0 );

    return es->esSourceDocument;
    }
