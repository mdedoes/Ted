/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docField.h"
#   include	"docEvalField.h"
#   include	"docParaBuilderImpl.h"
#   include	"docRecalculateFields.h"
#   include	<docMergeField.h>
#   include	<docDocumentField.h>
#   include	<uniUtf8.h>
#   include	<charnames.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate fields to merge in external data.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Calculate the value for a database merge field.			*/
/*									*/
/************************************************************************/

static int docCalculateMergefieldFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				int				anyway,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;
    MergeField		mf;

    docInitMergeField( &mf );

    if  ( docGetMergeField( &mf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( rf->rfFieldDataProvider )
	{
	int		ret= 0;

	ret= (*rf->rfFieldDataProvider)( pCalculated, mbResult,
					    &mf, rf, rf->rfMergeThrough );
	if  ( ret )
	    { LDEB(ret); *pCalculated= 0;	}
	}
    else{ *pCalculated= 0;	}

    if  ( ! *pCalculated && anyway )
	{
	char	scratch[4];
	int	step;

	step= uniPutUtf8( scratch, ISO1_guillemotleft );
	utilMemoryBufferAppendBytes( mbResult,
				    (unsigned char *)scratch, step );

	utilMemoryAppendBuffer( mbResult, &(mf.mfFieldName) );

	step= uniPutUtf8( scratch, ISO1_guillemotright );
	utilMemoryBufferAppendBytes( mbResult,
				    (unsigned char *)scratch, step );

	*pCalculated= 1;
	}

  ready:

    docCleanMergeField( &mf );

    return rval;
    }

int docRecalculateMergeField(	int *				pCalculated,
				int *				pStroffShift,
				ParagraphBuilder *		pb,
				DocumentField *			df,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int				partTail= partHead+ partCount;
    MemoryBuffer		mbResult;
    int				calculated= 0;

    const FieldKindInformation * fki= DOC_FieldKinds+ df->dfKind;

    utilInitMemoryBuffer( &mbResult );

    if  ( docCalculateMergefieldFieldString( &calculated, &mbResult,
						partCount == 0, df, rf ) )
	{ SDEB(fki->fkiLabel); partTail= -1; goto ready;	}

    if  ( ! calculated )
	{
	*pCalculated= 0;
	*pStroffShift= 0;
	goto ready;
	}

    *pCalculated= 1;

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

