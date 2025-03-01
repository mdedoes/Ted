/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docRefField.h>
#   include	"docSelect.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docCalculateRefFieldValue(	MemoryBuffer *			mbResult,
				const MemoryBuffer *		markName,
				struct BufferDocument *		bd )
    {
    DocumentSelection		dsInside;

    docInitDocumentSelection( &dsInside );

    if  ( docFindBookmarkInDocument( &dsInside, bd, markName ) )
	{
	utilMemoryBufferAppendString( mbResult, "<<? " );
	utilMemoryAppendBuffer( mbResult, markName );
	utilMemoryBufferAppendString( mbResult, " ?>>" );
	}
    else{
	if  ( docCollectReference( mbResult, &dsInside, bd ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a ref field.					*/
/*									*/
/************************************************************************/

int docCalculateRefFieldString( int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;
    RefField		reff;

    docInitRefField( &reff );

    if  ( docGetRefField( &reff, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( docCalculateRefFieldValue( mbResult, &(reff.rfBookmark),
							    rf->rfDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pCalculated= 1;

  ready:

    docCleanRefField( &reff );

    return rval;
    }

