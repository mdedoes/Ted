/************************************************************************/
/*									*/
/*  Copy part of one document to another (or the same) document.	*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	"docDocumentCopyJob.h"
#   include	<docParaRulerAdmin.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docItemShadingAdmin.h>

void docInitDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    dcj->dcjEditOperation= (EditOperation *)0;
    docInitSelectionScope( &(dcj->dcjTargetSelectionScope) );
    dcj->dcjTargetTree= (DocumentTree *)0;
    dcj->dcjSourceDocument= (BufferDocument *)0;
    dcj->dcjSourceTree= (DocumentTree *)0;
    dcj->dcjCopyFields= 0;
    dcj->dcjFieldMap= (int *)0;

    docInitDocumentAttributeMap( &(dcj->dcjAttributeMap) );
    dcj->dcjForceAttributeTo= -1;

    utilInitMemoryBuffer( &(dcj->dcjRefFileName) );

    dcj->dcjFieldStack= (FieldCopyStackLevel *)0;

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

    while( dcj->dcjFieldStack )
	{
	FieldCopyStackLevel *	prev= dcj->dcjFieldStack->fcslPrev;

	free( dcj->dcjFieldStack );

	dcj->dcjFieldStack= prev;
	}

    utilCleanIndexSet( &(dcj->dcjNoteFieldsCopied) );

    return;
    }

int docPushFieldOnCopyStack(		DocumentCopyJob *	dcj,
					DocumentField *		df )
    {
    FieldCopyStackLevel *	fcsl;

    fcsl= (FieldCopyStackLevel *)malloc( sizeof(FieldCopyStackLevel) );
    if  ( ! fcsl )
	{ XDEB(fcsl); return -1;	}

    fcsl->fcslPrev= dcj->dcjFieldStack;
    fcsl->fcslField= df;
    dcj->dcjFieldStack= fcsl;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a mapping for field numbers.				*/
/*									*/
/************************************************************************/

static int * docAllocateFieldMap(	const BufferDocument *	bdFrom )
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
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom )
    {
    int *		fontMap= (int *)0;
    int *		colorMap= (int *)0;
    int *		bmap= (int *)0;
    int *		shadingMap= (int *)0;
    int *		frameMap= (int *)0;
    int *		lsmap= (int *)0;
    int *		rulerMap= (int *)0;

    if  ( docMergeColorTables( &colorMap, bdTo, bdFrom ) )
	{ LDEB(1); return -1;	}

    if  ( docMergeBorderPropertiesLists( &bmap, colorMap,
					&(bdTo->bdBorderPropertyList),
					&(bdFrom->bdBorderPropertyList) ) )
	{ LDEB(1); return -1;	}

    if  ( docMergeItemShadingLists( &shadingMap, colorMap,
					&(bdTo->bdItemShadingList),
					&(bdFrom->bdItemShadingList) ) )
	{ LDEB(1); return -1;	}

    if  ( docMergeFramePropertyLists( &frameMap,
					&(bdTo->bdFramePropertyList),
					&(bdFrom->bdFramePropertyList) ) )
	{ LDEB(1); return -1;	}

    if  ( docMergeTabstopListLists( &rulerMap,
					&(bdTo->bdTabStopListList),
					&(bdFrom->bdTabStopListList) ) )
	{ LDEB(1); return -1;	}

    if  ( docMergeDocumentLists( &fontMap, &lsmap, bdTo, bdFrom,
						    colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    if  ( dam->damFontMap )
	{ free( dam->damFontMap );	}
    dam->damFontMap= fontMap;

    if  ( dam->damColorMap )
	{ free( dam->damColorMap );	}
    dam->damColorMap= colorMap;

    if  ( dam->damRulerMap )
	{ free( dam->damRulerMap );	}
    dam->damRulerMap= rulerMap;

    if  ( dam->damBorderMap )
	{ free( dam->damBorderMap );	}
    dam->damBorderMap= bmap;

    if  ( dam->damShadingMap )
	{ free( dam->damShadingMap );	}
    dam->damShadingMap= shadingMap;

    if  ( dam->damFrameMap )
	{ free( dam->damFrameMap );	}
    dam->damFrameMap= frameMap;

    if  ( dam->damListStyleMap )
	{ free( dam->damListStyleMap );	}
    dam->damListStyleMap= lsmap;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Build a copy job for copying within a document.			*/
/*									*/
/************************************************************************/

int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				int			copyFields )
    {
    int *	fieldMap;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= eo->eoDocument;
    dcj->dcjSourceTree= eo->eoTree;
    dcj->dcjCopyFields= copyFields;

    fieldMap= docAllocateFieldMap( dcj->dcjSourceDocument );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    if  ( eo->eoBottomField					&&
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
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
				BufferDocument *	bdFrom )
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
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
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
				BufferDocument *	bdFrom,
				DocumentTree *		eiFrom,
				const MemoryBuffer *	refFileName,
				int			forceAttributeTo )
    {
    int *		fldmap;

    BufferDocument *	bdTo= eo->eoDocument;

    dcj->dcjEditOperation= eo;
    dcj->dcjTargetSelectionScope= eo->eoSelectionScope;
    dcj->dcjTargetTree= eo->eoTree;
    dcj->dcjSourceDocument= bdFrom;
    dcj->dcjSourceTree= eiFrom;
    dcj->dcjCopyFields= 1;
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
	  docPushFieldOnCopyStack( dcj, eo->eoBottomField )	)
	{ LDEB(1); return -1;	}

    return 0;
    }
