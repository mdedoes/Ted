/************************************************************************/
/*									*/
/*  Calculate DOCPROPERTY fields.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docDocPropertyField.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the value of a DOCPROPERTY field.				*/
/*									*/
/************************************************************************/

int docCalculateDocPropertyFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;
    DocPropertyField	dpf;

    docInitDocPropertyField( &dpf );

    if  ( docGetDocPropertyField( &dpf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

#   if 0
    Calculation not yet ready yet

    if  ( utilMemoryAppendBuffer( mbResult, &(dpf.dpfPropertyName) ) )
	{ LDEB(1); return -1;	}

    *pCalculated= 1;
#   else
    *pCalculated= 0;
#   endif

  ready:

    docCleanDocPropertyField( &dpf );

    return rval;
    }
