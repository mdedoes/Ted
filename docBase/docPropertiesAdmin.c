/************************************************************************/
/*									*/
/*  Manage the lists of properties in a document.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docPropertiesAdmin.h"

#   include	"docBorderPropertyAdmin.h"
#   include	"docItemShadingAdmin.h"
#   include	"docFramePropertiesAdmin.h"
#   include	"docParaTabsAdmin.h"
#   include	"docCellPropertyAdmin.h"
#   include	"docRowPropertyAdmin.h"
#   include	"docParaPropertyAdmin.h"

#   include	<fontDocFontList.h>
#   include	<textAttributeAdmin.h>

#   include	<stdlib.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clean and free DocumentPropertyLists				*/
/*									*/
/************************************************************************/

void docFreeDocumentPropertyLists( DocumentPropertyLists * dpl )
    {
    utilCleanNumberedPropertiesList( &(dpl->dplTextAttributeList) );
    utilCleanNumberedPropertiesList( &(dpl->dplBorderPropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplItemShadingList) );
    utilCleanNumberedPropertiesList( &(dpl->dplFramePropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplTabStopListList) );
    utilCleanNumberedPropertiesList( &(dpl->dplCellPropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplRowPropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplParaPropertyList) );

    fontCleanDocFontList( &(dpl->dplFontList) );
    docCleanListAdmin( &(dpl->dplListAdmin) );
    utilCleanColorPalette( &(dpl->dplColorPalette) );

    free( dpl );
    }

/************************************************************************/
/*									*/
/*  Allocate DocumentPropertyLists					*/
/*									*/
/************************************************************************/

DocumentPropertyLists * docMakeDocumentPropertyLists( void )
    {
    DocumentPropertyLists * dpl= malloc( sizeof(DocumentPropertyLists) );
    if  ( ! dpl )
	{ XDEB(dpl); return (DocumentPropertyLists *)0;	}

    textInitTextAttributeList( &(dpl->dplTextAttributeList) );
    docInitBorderPropertyList( &(dpl->dplBorderPropertyList) );
    docInitItemShadingList( &(dpl->dplItemShadingList) );
    docInitFramePropertyList( &(dpl->dplFramePropertyList) );
    docInitTabStopListList( &(dpl->dplTabStopListList) );
    docInitCellPropertyList( &(dpl->dplCellPropertyList) );
    docInitRowPropertiesList( &(dpl->dplRowPropertyList) );
    docInitParagraphPropertyList( &(dpl->dplParaPropertyList) );

    fontInitDocFontList( &(dpl->dplFontList) );
    docInitListAdmin( &(dpl->dplListAdmin) );
    utilInitColorPalette( &(dpl->dplColorPalette) );

    return dpl;
    }

