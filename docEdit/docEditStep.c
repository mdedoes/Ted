/************************************************************************/
/*									*/
/*  Trace modifications to a document.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docEditStep.h"

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

    es->esSourceDocument= (BufferDocument *)0;

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
    BufferDocument *	bd= es->esSourceDocument;

    if  ( bd )
	{
	/* Shallow copies: Do not clean! */
	docInitDocumentProperties( &(bd->bdProperties) );
	utilInitNumberedPropertiesList( &(bd->bdTextAttributeList) );
	utilInitNumberedPropertiesList( &(bd->bdBorderPropertyList) );
	utilInitNumberedPropertiesList( &(bd->bdItemShadingList) );
	utilInitNumberedPropertiesList( &(bd->bdFramePropertyList) );
	utilInitNumberedPropertiesList( &(bd->bdTabStopListList) );

	docFreeDocument( es->esSourceDocument );
	}

    docCleanDocumentStyle( &(es->esNewStyle) );
    docCleanDocumentProperties( &(es->esNewDocProps) );

    /* docCleanPictureProperties( &(es->esPictureProperties) ); */

    docCleanDocumentList( &(es->esDocumentList) );

    return;
    }

