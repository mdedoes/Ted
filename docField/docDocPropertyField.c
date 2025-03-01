/************************************************************************/
/*									*/
/*  Bookkeeping about document property fields.				*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	"docDocumentField.h"
#   include	"docDocPropertyField.h"
#   include	"docFieldFormat.h"
#   include	"docFieldKind.h"

#   include	<appDebugon.h>

void docInitDocPropertyField(	DocPropertyField *	dpf )
    {
    utilInitMemoryBuffer( &(dpf->dpfPropertyName) );

    dpf->dpfCaseShift= FIELDformatASIS;

    return;
    }

void docCleanDocPropertyField(	DocPropertyField *	dpf )
    {
    utilCleanMemoryBuffer( &(dpf->dpfPropertyName) );
    }

int docGetDocPropertyField(	DocPropertyField *		dpf,
				const DocumentField *		df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkDOCPROPERTY )
	{ return 1;	}

    utilCopyMemoryBuffer( &(dpf->dpfPropertyName),
					&(fi->fiComponents[1].icBuffer) );

    ic= fi->fiComponents+ 2;
    for ( comp= 2; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentCaseShift( &(dpf->dpfCaseShift), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

