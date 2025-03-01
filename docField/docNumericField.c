/************************************************************************/
/*									*/
/*  Bookkeeping for numeric fields such as section, sectionpages, page	*/
/*  fields.								*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<string.h>

#   include	"docNumericField.h"
#   include	"docDocumentField.h"
#   include	"docFieldFormat.h"

#   include	<appDebugon.h>

void docInitNumericField(	NumericField *	nf )
    {
    nf->nfNumberFormat= FIELDformatARABIC;

    return;
    }

void docCleanNumericField(	NumericField *	nf )
    {
    /* nothing */
    }

/************************************************************************/
/*									*/
/*  Evaluate numeric fields.						*/
/*									*/
/************************************************************************/

int docGetNumericField(		NumericField *		nf,
				int			expect,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != expect )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentNumberFormat( &(nf->nfNumberFormat), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LSDEB(comp,utilMemoryBufferGetString(&(ic->icBuffer)));
	}

    return 0;
    }

int docSetNumericField(		DocumentField *		df,
				int			kindInt,
				const char *		kindStr,
				const NumericField *	nf )
    {
    FieldInstructions *		fi= &(df->dfInstructions);

    df->dfKind= kindInt;

    if  ( docStartFieldInstructions( fi, kindStr, strlen(kindStr) ) )
	{ LDEB(7); return -1;	}

    if  ( docFieldInstructionsAddFormat( fi, nf->nfNumberFormat ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

