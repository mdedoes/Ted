/************************************************************************/
/*									*/
/*  Calculations of the value of = (Formula) and IF fields.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<math.h>
#   include	<string.h>

#   include	"docCalculateFormulaValue.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  INFIX OPERATORS.							*/
/*									*/
/************************************************************************/

static int doc_IO_Plus(		const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );

    if  ( docFormulaGetNumber( &fv1, top, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv2, rhs, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}

    top->fvDouble= fv1.fvDouble+ fv2.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

static int doc_IO_Minus(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );

    if  ( docFormulaGetNumber( &fv1, top, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv2, rhs, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}

    top->fvDouble= fv1.fvDouble- fv2.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

static int doc_IO_Multiply(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );

    if  ( docFormulaGetNumber( &fv1, top, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv2, rhs, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}

    top->fvDouble= fv1.fvDouble* fv2.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

static int doc_IO_Divide(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );

    if  ( docFormulaGetNumber( &fv1, top, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv2, rhs, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}

    if  ( fv2.fvDouble == 0.0 )
	{ SFDEB(io->ioSymbol,fv2.fvDouble); rval= -1; goto ready;	}

    top->fvDouble= fv1.fvDouble/ fv2.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

static int doc_IO_Power(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );

    if  ( docFormulaGetNumber( &fv1, top, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv2, rhs, cf ) )
	{ SDEB(io->ioSymbol); rval= -1; goto ready;	}

    top->fvDouble= pow( fv1.fvDouble, fv2.fvDouble );
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

static int doc_IO_Compare(	int *				pCmp,
				const CalculateFormula *	cf,
				const FormulaValue *		top,
				const FormulaValue *		rhs )
    {
    int			rval= 0;
    FormulaValue	fv1, fv2;
    FormulaValue	fv3, fv4;

    docFormulaInitValue( &fv1 );
    docFormulaInitValue( &fv2 );
    docFormulaInitValue( &fv3 );
    docFormulaInitValue( &fv4 );

    if  ( docFormulaGetString( &fv1, top, cf ) )
	{ LDEB(top->fvType); rval= -1; goto ready;	}
    if  ( docFormulaGetString( &fv2, rhs, cf ) )
	{ LDEB(rhs->fvType); rval= -1; goto ready;	}

    if  ( ! docFormulaGetStringAsNumber( &fv3, &(fv1.fvBuffer) )	&&
	  ! docFormulaGetStringAsNumber( &fv4, &(fv2.fvBuffer) )	)
	{
	*pCmp= ( fv3.fvDouble > fv4.fvDouble ) -
	       ( fv3.fvDouble < fv4.fvDouble );
	}
    else{
	*pCmp= strcmp( utilMemoryBufferGetString( &(fv3.fvBuffer) ),
	               utilMemoryBufferGetString( &(fv4.fvBuffer) ) );
	}

  ready:

    docFormulaCleanValue( &fv1 );
    docFormulaCleanValue( &fv2 );
    docFormulaCleanValue( &fv3 );
    docFormulaCleanValue( &fv4 );

    return rval;
    }

static int doc_IO_Equal(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp == 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_IO_Less(		const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp < 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_IO_LessEqual(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp <= 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_IO_Greater(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp > 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_IO_GreaterEqual(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp >= 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_IO_NotEqual(	const InfixOperator *		io,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		rhs )
    {
    int		cmp= 0;

    if  ( doc_IO_Compare( &cmp, cf, top, rhs ) )
	{ SDEB(io->ioSymbol); return -1;	}

    top->fvDouble= cmp != 0;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

/************************************************************************/
/*									*/
/*  The infix operators. The primitive search algorithm requires that	*/
/*  no operator symbol is a prefix of a subsequent one.			*/
/*									*/
/************************************************************************/

static const InfixOperator DOC_InfixOperators[]=
{
			/*
			 * arithmetic
			 */
    {	"+",	5,	doc_IO_Plus,		},
    {	"-",	5,	doc_IO_Minus,		},
    {	"*",	6,	doc_IO_Multiply,	},
    {	"/",	6,	doc_IO_Divide,		},
    {	"^",	7,	doc_IO_Power,		},
			/*
			 * comparison
			 */
    {	"=",	4,	doc_IO_Equal,		},
    {	"<=",	4,	doc_IO_LessEqual,	},
    {	">=",	4,	doc_IO_GreaterEqual,	},
    {	"<>",	4,	doc_IO_NotEqual,	},
    {	">",	4,	doc_IO_Greater,		},
    {	"<",	4,	doc_IO_Less,		},
};

static const int DOC_InfixOperatorCount=
		    sizeof(DOC_InfixOperators)/sizeof(InfixOperator);

const InfixOperator * docFormulaGetInfixOperator(
						int *		pLen,
						const char *	formula )
    {
    int		op;

    const InfixOperator * io= DOC_InfixOperators;
    for ( op= 0; op < DOC_InfixOperatorCount; io++, op++ )
	{
	const char *	s= io->ioSymbol;
	const char *	f= formula;
	int		len= 0;

	while( *s && *s == *f )
	    { s++; f++; len++;	}

	if  ( ! *s )
	    { *pLen= len; return io;	}
	}

    return (InfixOperator *)0;
    } 

/************************************************************************/
/*									*/
/*  PREFIX OPERATORS.							*/
/*									*/
/************************************************************************/

static int doc_PO_Plus(		const PrefixOperator *		po,
				const CalculateFormula *	cf,
				FormulaValue *			top )
    {
    int			rval= 0;
    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, top, cf ) )
	{ LDEB(po->poSymbol); rval= -1; goto ready;	}

    top->fvDouble= fv.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_PO_Minus(	const PrefixOperator *		po,
				const CalculateFormula *	cf,
				FormulaValue *			top )
    {
    int			rval= 0;
    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, top, cf ) )
	{ LDEB(po->poSymbol); rval= -1; goto ready;	}

    top->fvDouble= -fv.fvDouble;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

/************************************************************************/
/*									*/
/*  The prefix operators. The primitive search algorithm requires that	*/
/*  no operator symbol is a prefix of a subsequent one.			*/
/*									*/
/************************************************************************/

static const PrefixOperator DOC_PrefixOperators[]=
{
    {	"+",	doc_PO_Plus,		},
    {	"-",	doc_PO_Minus,		},
};

static const int DOC_PrefixOperatorCount=
		    sizeof(DOC_PrefixOperators)/sizeof(PrefixOperator);

const PrefixOperator * docFormulaGetPrefixOperator(
						int *		pLen,
						const char *	formula )
    {
    int		op;

    const PrefixOperator * po= DOC_PrefixOperators;
    for ( op= 0; op < DOC_PrefixOperatorCount; po++, op++ )
	{
	const char *	s= po->poSymbol;
	const char *	f= formula;
	int		len= 0;

	while( *s && *s == *f )
	    { s++; f++; len++;	}

	if  ( ! *s )
	    { *pLen= len; return po;	}
	}

    return (PrefixOperator *)0;
    } 
