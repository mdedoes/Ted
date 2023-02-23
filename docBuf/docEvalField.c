/************************************************************************/
/*									*/
/*  Evaluate fields+ the list of kinds of fields.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	"docField.h"
#   include	"docDocument.h"
#   include	"docParaString.h"
#   include	"docEvalField.h"
#   include	<docDocumentField.h>
#   include	"docRecalculateFields.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	"docObjects.h"
#   include	"docParaBuilderImpl.h"
#   include	"docParaBuilder.h"
#   include	"docParaParticuleAdmin.h"
#   include	<utilTree.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start a recalculate fields job.					*/
/*									*/
/************************************************************************/

void docInitRecalculateFields(   RecalculateFields *     rf )
    {
    rf->rfDocument= (struct BufferDocument *)0;
    rf->rfTree= (struct DocumentTree *)0;
    rf->rfSelectionScope= (const struct SelectionScope *)0;
    rf->rfSelectedTree= (struct DocumentTree *)0;
    rf->rfFieldsUpdated= 0;
    rf->rfUpdateFlags= 0;

    rf->rfBodySectNode= (const struct BufferItem *)0;
    rf->rfBodySectPage= -1;

    docInitEditPosition( &(rf->rfSelHead) );
    docInitEditPosition( &(rf->rfSelTail) );

    rf->rfFieldDataProvider= (FieldDataProvider)0;
    rf->rfOpenInstanceStream= 0;
    rf->rfMergeThrough= (void *)0;

    rf->rfLocale= (const struct SimpleLocale *)0;
    rf->rfIncludeDocumentCache= (void *)0;

    return;
    }

static int docRecalculateFieldsFreeCachedDocument(
					const char *		key,
					void *			vbdSource,
					void *			through )
    {
    docFreeDocument( (struct BufferDocument *)vbdSource );
    return 0;
    }

void docCleanRecalculateFields(	RecalculateFields *	rf )
    {
    if  ( rf->rfIncludeDocumentCache )
	{
	utilTreeFreeTree( rf->rfIncludeDocumentCache,
			docRecalculateFieldsFreeCachedDocument, (void *)0 );
	}
    }

InstanceStream * docRecalculateFieldsOpenInstanceStream(
				struct RecalculateFields *	rf,
				const char *			name,
				const char *			query )
    {
    InstanceStream *			rval= (InstanceStream *)0;
    InstanceStream *			is= (InstanceStream *)0;

    if  ( ! rf->rfOpenInstanceStream )
	{ XDEB(rf->rfOpenInstanceStream); goto ready;	}

    is= malloc( sizeof(InstanceStream) );
    if  ( ! is )
	{ XDEB(is); goto ready;	}
    is->isName= name;
    is->isQuery= query;
    is->isRecalculateFields= rf;

    is->isToNext= 0;
    is->isClosePrivate= 0;
    is->isPrivate= (struct InstanceStreamPrivate *)0;

    if  ( (*rf->rfOpenInstanceStream)( is, rf, name, query ) )
	{ SSDEB(name,query); goto ready;	}

    rval= is; is= (InstanceStream *)0; /* steal */

  ready:

    if  ( is )
	{ docRecalculateFieldsCloseInstanceStream( is );	}

    return rval;
    }

void docRecalculateFieldsCloseInstanceStream(	InstanceStream *	is )
    {
    if  ( is->isClosePrivate )
	{ (*is->isClosePrivate)( is );	}

    free( is );

    return;
    }

/************************************************************************/
/*									*/
/*  Replace the previous contents of a field with new contents.		*/
/*									*/
/************************************************************************/

int docFieldReplaceContents(
			int *				pStroff,
			int *				pStroffShift,
			int *				pTextAttrNr,
			BufferItem *			paraNode,
			int				part,
			int				partCount,
			int				stroffShift,
			const char *			addedString,
			int				addedStrlen,
			const RecalculateFields *	rf )
    {
    TextParticule *	tp= paraNode->biParaParticules+ part;

    int			textAttrNr= tp[1].tpTextAttrNr;
    int			past= tp[1+partCount].tpStroff+ stroffShift;
    int			stroff= tp[1].tpStroff+ stroffShift;

    int			d= 0;

    if  ( docParaStringReplace( &d, paraNode, stroff, past,
					    addedString, addedStrlen ) )
	{ LDEB(addedStrlen); return -1;	}

    if  ( partCount > 0 )
	{
	docResetParticuleObjects( rf->rfDocument,
			    paraNode->biParaParticules+ part+ 1, partCount );
	docDeleteParticules( paraNode, part+ 1, partCount );
	}

    *pStroff= stroff;
    *pStroffShift= d;
    *pTextAttrNr= textAttrNr;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Redivide the new contents of the field into text particules.	*/
/*									*/
/*  1)  A singe particule with the same contents: No need to do		*/
/*	anything.							*/
/*									*/
/************************************************************************/

int docRecalculateFieldParticulesFromString(
				int *				pCalculated,
				int *				pStroffShift,
				ParagraphBuilder *		pb,
				int				partHead,
				int				partCount,
				const MemoryBuffer *		mbResult,
				const RecalculateFields *	rf )
    {
    int			stroffShift;

    BufferItem *	paraNode= pb->pbParaNode;
    TextParticule *	tp= paraNode->biParaParticules+ partHead;
    int			textAttrNr;

    int			past;
    int			stroff;
    int			partsMade;

    /*  1  */
    if  ( tp[1].tpStrlen == mbResult->mbSize				&&
	  ! memcmp( docParaString( paraNode, tp[1].tpStroff+ *pStroffShift ),
				    mbResult->mbBytes, mbResult->mbSize ) )
	{
	*pCalculated= 0;
	*pStroffShift= 0;
	return partHead+ partCount;
	}

    docResetParticuleObjects( rf->rfDocument,
			paraNode->biParaParticules+ partHead+ 1, partCount );

    textAttrNr= tp[1].tpTextAttrNr;
    past= tp[1+partCount].tpStroff+ *pStroffShift;
    stroff= tp[1].tpStroff+ *pStroffShift;

    partsMade= docParaBuilderSubstitute( &stroffShift, pb,
				    stroff, past,
				    stroff, past,
				    partHead+ 1, partHead+ 1+ partCount,
				    (char *)mbResult->mbBytes, mbResult->mbSize, textAttrNr );
    if  ( partsMade < 0 )
	{ LDEB(partsMade); return -1;	}

    *pCalculated= 1;
    *pStroffShift= stroffShift;

    return partHead+ partsMade;
    }

/************************************************************************/
/*									*/
/*  Substitute an individual text field in a paragraph.			*/
/*									*/
/*  NOTE: This routine is not recursive. Nested fields are handeled	*/
/*	by the caller.							*/
/*									*/
/************************************************************************/

int docRecalculateParaStringTextParticules(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				DocumentField *			df,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int			partTail= partHead+ partCount;
    MemoryBuffer	mbResult;
    int			calculated= 0;

    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    utilInitMemoryBuffer( &mbResult );

    if  ( (*fki->fkiCalculateTextString)( &calculated, &mbResult, df, rf ) )
	{ SDEB(fki->fkiLabel); partTail= -1; goto ready;	}

    if  ( ! calculated )
	{
	*pCalculated= 0;
	*pStroffShift= 0;
	goto ready;
	}

    partTail= docRecalculateFieldParticulesFromString(
				    pCalculated, pStroffShift,
				    pb, partHead, partCount, &mbResult, rf );
    if  ( partTail <= partHead )
	{
	SLLDEB(fki->fkiLabel,partHead,partTail);
	partTail= -1; goto ready;
	}

  ready:

    utilCleanMemoryBuffer( &mbResult );

    return partTail;
    }
