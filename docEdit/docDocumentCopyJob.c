/************************************************************************/
/*									*/
/*  Copy part of one document to another (or the same) document.	*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<docPropertiesAdmin.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docParaTabsAdmin.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docItemShadingAdmin.h>
#   include	<docFieldStack.h>
#   include	<docFramePropertiesAdmin.h>

void docInitDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    dcj->dcjEditOperation= (EditOperation *)0;
    docInitSelectionScope( &(dcj->dcjTargetSelectionScope) );
    dcj->dcjTargetTree= (struct DocumentTree *)0;
    dcj->dcjSourceDocument= (struct BufferDocument *)0;
    dcj->dcjSourceTree= (struct DocumentTree *)0;
    dcj->dcjCopyFields= 0;
    dcj->dcjStealFields= 0;
    dcj->dcjFieldMap= (int *)0;

    docInitDocumentAttributeMap( &(dcj->dcjAttributeMap) );
    dcj->dcjForceAttributeTo= -1;

    utilInitMemoryBuffer( &(dcj->dcjRefFileName) );

    dcj->dcjFieldStack= (struct FieldStackLevel *)0;

    dcj->dcjInExternalTree= 0;

    dcj->dcjCurrentTextAttributeNumberFrom= -1;
    dcj->dcjCurrentTextAttributeNumberTo= -1;

    utilInitIndexSet( &(dcj->dcjNoteFieldsCopied) );
    dcj->dcjBulletsCopied= 0;

    dcj->dcjCopyHeadParaProperties= 0;
    dcj->dcjCopyTailParaProperties= 0;

    return;
    }

void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    docCleanDocumentAttributeMap( &(dcj->dcjAttributeMap) );

    utilCleanMemoryBuffer( &(dcj->dcjRefFileName) );

    docCleanFieldStack( dcj->dcjFieldStack );

    utilCleanIndexSet( &(dcj->dcjNoteFieldsCopied) );

    return;
    }

int docPushFieldOnCopyStack(		DocumentCopyJob *	dcj,
					struct DocumentField *	df,
					int			piece )
    {
    if  ( docFieldStackPushLevel( &(dcj->dcjFieldStack), df, piece ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a mapping for field numbers.				*/
/*									*/
/************************************************************************/

static int * docAllocateFieldMap(	const struct BufferDocument *	bdFrom )
    {
    int *	fieldMap;
    int		i;
    const int	fieldCount= bdFrom->bdFieldList.dflPagedList.plItemCount;

    fieldMap= (int *)malloc( fieldCount* sizeof(int) );
    if  ( ! fieldMap )
	{ LXDEB(fieldCount,fieldMap); return (int *)0; }
    for ( i= 0; i < fieldCount; i++ )
	{ fieldMap[i]= -1;	}

    return fieldMap;
    }

/************************************************************************/

static int docSetAttributeMap(	DocumentAttributeMap *		dam,
				struct BufferDocument *		bdTo,
				struct BufferDocument *		bdFrom )
    {
    int		rval= 0;

    int *	colorMap= (int *)0;
    int *	borderMap= (int *)0;
    int *	shadingMap= (int *)0;
    int *	frameMap= (int *)0;
    int *	rulerMap= (int *)0;
    int *	cellMap= (int *)0;
    int *	rowMap= (int *)0;
    int *	fontMap= (int *)0;
    int *	lsMap= (int *)0;

    const DocumentPropertyLists *	dplFrom= bdFrom->bdPropertyLists;
    DocumentPropertyLists *		dplTo= bdTo->bdPropertyLists;

    if  ( docMergeColorTables( &colorMap, bdTo, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeBorderPropertiesLists( &borderMap, colorMap,
					&(dplTo->dplBorderPropertyList),
					&(dplFrom->dplBorderPropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeItemShadingLists( &shadingMap, colorMap,
					&(dplTo->dplItemShadingList),
					&(dplFrom->dplItemShadingList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeFramePropertyLists( &frameMap,
					&(dplTo->dplFramePropertyList),
					&(dplFrom->dplFramePropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docMergeTabstopListLists( &rulerMap,
					&(dplTo->dplTabStopListList),
					&(dplFrom->dplTabStopListList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  Not needed: We never refer to cells by property number
    if  ( docMergeCellPropertiesLists( &cellMap, borderMap, shadingMap,
					&(dplTo->dplCellPropertyList),
					&(dplFrom->dplCellPropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    */

    /*  Not needed: We never refer to rows by property number
    if  ( docMergeRowPropertiesLists( &rowMap, borderMap, shadingMap,
					&(dplTo->dplRowPropertyList),
					&(dplFrom->dplRowPropertyList) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    */

    if  ( docMergeDocumentLists( &fontMap, &lsMap, bdTo, bdFrom,
						    colorMap, rulerMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dam->damColorMap )
	{ free( dam->damColorMap );	}
    dam->damColorMap= colorMap; colorMap= (int *)0; /* steal */

    if  ( dam->damBorderMap )
	{ free( dam->damBorderMap );	}
    dam->damBorderMap= borderMap; borderMap= (int *)0; /* steal */

    if  ( dam->damShadingMap )
	{ free( dam->damShadingMap );	}
    dam->damShadingMap= shadingMap; shadingMap= (int *)0; /* steal */

    if  ( dam->damFrameMap )
	{ free( dam->damFrameMap );	}
    dam->damFrameMap= frameMap; frameMap= (int *)0; /* steal */

    if  ( dam->damRulerMap )
	{ free( dam->damRulerMap );	}
    dam->damRulerMap= rulerMap; rulerMap= (int *)0; /* steal */

    if  ( dam->damCellMap )
	{ free( dam->damCellMap );	}
    dam->damCellMap= cellMap; cellMap= (int *)0; /* steal */

    if  ( dam->damRowMap )
	{ free( dam->damRowMap );	}
    dam->damRowMap= rowMap; cellMap= (int *)0; /* steal */

    if  ( dam->damFontMap )
	{ free( dam->damFontMap );	}
    dam->damFontMap= fontMap; fontMap= (int *)0; /* steal */

    if  ( dam->damListStyleMap )
	{ free( dam->damListStyleMap );	}
    dam->damListStyleMap= lsMap; lsMap= (int *)0;/* steal */

  ready:

    if  ( colorMap )
	{ free( colorMap );	}
    if  ( borderMap )
	{ free( borderMap );	}
    if  ( shadingMap )
	{ free( shadingMap );	}
    if  ( frameMap )
	{ free( frameMap );	}
    if  ( rulerMap )
	{ free( rulerMap );	}
    if  ( cellMap )
	{ free( cellMap );	}
    if  ( rowMap )
	{ free( rowMap );	}
    if  ( fontMap )
	{ free( fontMap );	}
    if  ( lsMap )
	{ free( lsMap );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying within a document.			*/
/*									*/
/************************************************************************/

int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				int			fieldHandling )
    {
    int *	fieldMap;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= eo->eoDocument;
    dcj->dcjSourceTree= eo->eoTree;
    dcj->dcjCopyFields= fieldHandling == CFH_COPY;
    dcj->dcjStealFields= fieldHandling == CFH_STEAL;

    fieldMap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField, FSpieceFLDRSLT ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying within a document.			*/
/*									*/
/************************************************************************/

int docSetTraceDocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				struct BufferDocument *	bdFrom )
    {
    int *	fieldMap= (int *)0;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= bdFrom;
    dcj->dcjSourceTree= &(bdFrom->bdBody);
    dcj->dcjCopyFields= 1;

    fieldMap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField, FSpieceFLDRSLT ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying from one document to another.		*/
/*									*/
/************************************************************************/

int docSet2DocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				struct BufferDocument *	bdFrom,
				struct DocumentTree *	sourceTree,
				const MemoryBuffer *	refFileName,
				int			forceAttributeTo )
    {
    int *		fldmap;

    struct BufferDocument *	bdTo= eo->eoDocument;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= bdFrom;
    dcj->dcjSourceTree= sourceTree;
    dcj->dcjCopyFields= 1;
    dcj->dcjStealFields= 0;
    dcj->dcjForceAttributeTo= forceAttributeTo;

    fldmap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fldmap )
	{ XDEB(fldmap); return -1;	}
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}
    dcj->dcjFieldMap= fldmap;

    if  ( docSetAttributeMap( &(dcj->dcjAttributeMap), bdTo, bdFrom ) )
	{ LDEB(1); return -1;	}

    if  ( refFileName )
	{
	if  ( utilCopyMemoryBuffer( &(dcj->dcjRefFileName), refFileName ) )
	    { LDEB(1); return -1;	}
	}
    else{
	utilEmptyMemoryBuffer( &(dcj->dcjRefFileName) );
	}

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField, FSpieceFLDRSLT ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
