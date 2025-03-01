/************************************************************************/
/*									*/
/*  Administration of StructItems: Do not emit any PS/PDFMARKS here.	*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"psPrint.h"
#   include	<geoRectangle.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Pdfmark functionality around marked content.			*/
/*									*/
/************************************************************************/

/**
 * Return a new contentId. ISO 32000-1:2008, 14.7.4.2 requires this to
 * be unique in the content stream. As we have one content stream per
 * page, we distribute unique numbers per page.
 */
static int psNewContentId(	PrintingState *		ps )
    {
    return ps->psPageContentMarkCount++;
    }

StructItem * psPdfLeafStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine )
    {
    StructItem *	structItem= malloc( sizeof(StructItem) );
    const int		contentId= psNewContentId( ps );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName),
			"TedRo%d", ps->psDictionaryNameCount++ );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 1;
    structItem->siIsInline= inLine;

    return structItem;
    }

StructItem * psPdfAnnotatedStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine )
    {
    StructItem *	structItem= malloc( sizeof(StructItem) );
    const int		contentId= psNewContentId( ps );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName),
			"TedRo%d", ps->psDictionaryNameCount++ );
    utilMemoryBufferPrintf( &(structItem->siAnnotationDictionaryName),
			"TedAn%d", ps->psDictionaryNameCount++ );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 1;
    structItem->siIsInline= inLine;

    return structItem;
    }

StructItem * psPdfGroupStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			contentId )
    {
    const int		uniqueDictId= ps->psDictionaryNameCount++;
    StructItem *	structItem= malloc( sizeof(StructItem) );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName), "TedGr%d", uniqueDictId );
    utilMemoryBufferPrintf( &(structItem->siChildArrayName), "TedGr%dK", uniqueDictId );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 0;
    structItem->siIsInline= 0;

    return structItem;
    }

/************************************************************************/
/*									*/
/*  Manage the stack of StructItems for marked PDF only. (PDF-UA )	*/
/*									*/
/************************************************************************/

void psPdfInitStructItem(	StructItem *	structItem )
    {
    utilInitMemoryBuffer( &(structItem->siDictionaryName) );
    utilInitMemoryBuffer( &(structItem->siChildArrayName) );
    utilInitMemoryBuffer( &(structItem->siAnnotationDictionaryName) );

    structItem->siParent= (StructItem *)0;
    structItem->siStructureType= (const char *)0;
    structItem->siContentId= -1;
    structItem->siIsLeaf= 0;
    structItem->siIsInline= 0;
    structItem->siLanguageTag= (const char *)0;
    }

void psPdfCleanStructItem(	StructItem *	structItem )
    {
    utilCleanMemoryBuffer( &(structItem->siDictionaryName) );
    utilCleanMemoryBuffer( &(structItem->siChildArrayName) );
    utilCleanMemoryBuffer( &(structItem->siAnnotationDictionaryName) );
    }

void psPdfPopStructItem(	PrintingState * ps )
    {
    if  ( ! ps->psCurrentStructItem )
	{ XDEB(ps->psCurrentStructItem);	}
    else{
	StructItem *	structItem= ps->psCurrentStructItem;

	ps->psCurrentStructItem= structItem->siParent;

	psPdfCleanStructItem( structItem );
	free( structItem );
	}
    }

int psPdfPushStructItem(	PrintingState *	ps,
				StructItem *	structItem )
    {
    structItem->siParent= ps->psCurrentStructItem;
    ps->psCurrentStructItem= structItem;

    return 0;
    }

