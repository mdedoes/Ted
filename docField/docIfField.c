/************************************************************************/
/*									*/
/*  Calculate = (Formula) fields.					*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	"docDocumentField.h"
#   include	"docIfField.h"
#   include	"docFieldKind.h"

#   include	<appDebugon.h>

void docInitIfField(	IfField *	iff )
    {
    utilInitMemoryBuffer( &(iff->iffExpression1) );
    utilInitMemoryBuffer( &(iff->iffOperator) );
    utilInitMemoryBuffer( &(iff->iffExpression2) );
    utilInitMemoryBuffer( &(iff->iffTrueText) );
    utilInitMemoryBuffer( &(iff->iffFalseText) );
    return;
    }

void docCleanIfField(	IfField *	iff )
    {
    utilCleanMemoryBuffer( &(iff->iffExpression1) );
    utilCleanMemoryBuffer( &(iff->iffOperator) );
    utilCleanMemoryBuffer( &(iff->iffExpression2) );
    utilCleanMemoryBuffer( &(iff->iffTrueText) );
    utilCleanMemoryBuffer( &(iff->iffFalseText) );
    return;
    }

/************************************************************************/
/*									*/
/*  Evaluate = (Formula) fields.					*/
/*									*/
/************************************************************************/

typedef enum FormulaString
    {
    IF_Expression1,
    IF_Operator,
    IF_Expression2,
    IF_TrueText,
    IF_FalseText,

    IF__COUNT
    } FormulaString;

int docGetIfField(	IfField *		iff,
			const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					str= IF_Expression1;

    if  ( df->dfKind != DOCfkIF )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == IF_Expression1		)
	    {
	    if  ( utilCopyMemoryBuffer( &(iff->iffExpression1), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    str++; continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == IF_Operator		)
	    {
	    if  ( utilCopyMemoryBuffer( &(iff->iffOperator), &(ic->icBuffer) ) )
		{ LDEB(comp);	}
	    str++; continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == IF_Expression2		)
	    {
	    if  ( utilCopyMemoryBuffer( &(iff->iffExpression2), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    str++; continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == IF_TrueText		)
	    {
	    if  ( utilCopyMemoryBuffer( &(iff->iffTrueText), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    str++; continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == IF_FalseText		)
	    {
	    if  ( utilCopyMemoryBuffer( &(iff->iffFalseText), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    str++; continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

