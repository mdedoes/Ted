/************************************************************************/
/*									*/
/*  Calculations of the value of = (Formula) and IF fields.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<math.h>
#   include	<stdlib.h>
#   include	<float.h>

#   include	"docBuf.h"
#   include	"docCalculateFormulaValue.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"

#   include	<appDebugon.h>

/************************************************************************/

static int doc_FC_Abs(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;

    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    if  ( fv.fvDouble >= 0 )
	{ top->fvDouble=  fv.fvDouble;	}
    else{ top->fvDouble= -fv.fvDouble;	}
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_FC_And(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;
    FormulaValue	fv0, fv1;

    docFormulaInitValue( &fv0 );
    docFormulaInitValue( &fv1 );

    if  ( docFormulaGetNumber( &fv0, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv1, &(argv[1]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    top->fvDouble= fv0.fvDouble != 0.0 && fv1.fvDouble != 0.0;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv0 );
    docFormulaCleanValue( &fv1 );

    return rval;
    }

static int docCollectNumbersFromSelection(
				int *				pCount,
				double *			pSum,
				double *			pProd,
				double *			pMin,
				double *			pMax,
				const CalculateFormula *	cf,
				const DocumentSelection *	ds,
				int				direction )
    {
    double *		numbers= (double *)0;
    int			numberCount;

    numberCount= docCollectNumbers( &numbers, ds, direction,
				cf->cfRecalculateFields->rfDocument );
    if  ( numberCount < 0 )
	{ LDEB(numberCount); return -1;	}
    while( numberCount > 0 )
	{
	numberCount--;
	(*pCount)++;
	*pSum  += numbers[numberCount];
	*pProd *= numbers[numberCount];
	if  ( *pMin > numbers[numberCount] )
	    { *pMin=  numbers[numberCount];	}
	if  ( *pMax < numbers[numberCount] )
	    { *pMax=  numbers[numberCount];	}
	}

    if  ( numbers )
	{ free( numbers ); numbers= (double *)0; }

    return 0;
    }

static int doc_FC_All(		const CalculateFormula *	cf,
				int *				pCount,
				double *			pSum,
				double *			pProd,
				double *			pMin,
				double *			pMax,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;

    double		sum= 0.0;
    double		prod= 1.0;
    double		min= DBL_MAX;
    double		max= DBL_MIN;
    int			count= 0;

    int			arg;

    DocumentSelection	ds;
    FormulaValue	fv;

    docFormulaInitValue( &fv );

    for ( arg= 0; arg < argc; arg++ )
	{
	switch( argv[arg].fvType )
	    {
	    case FVtypeNUMBER:
		count++;
		sum  += argv[arg].fvDouble;
		prod *= argv[arg].fvDouble;
		if  ( min > argv[arg].fvDouble )
		    { min=  argv[arg].fvDouble;	}
		if  ( max < argv[arg].fvDouble )
		    { max=  argv[arg].fvDouble;	}
		break;

	    case FVtypeSTRING:
		if  ( docFormulaGetStringAsNumber( &fv,
						&(argv[arg].fvBuffer) ) )
		    { LDEB(arg); rval= -1; goto ready;	}

		count++;
		sum  += fv.fvDouble;
		prod *= fv.fvDouble;
		if  ( min > fv.fvDouble )
		    { min=  fv.fvDouble;	}
		if  ( max < fv.fvDouble )
		    { max=  fv.fvDouble;	}
		break;

	    case FVtypeNAME:
		docInitDocumentSelection( &ds );
		if  ( docFormulaGetNameAsSelection( &ds, &(argv[arg]), cf ) )
		    { LDEB(arg); rval= -1; goto ready;	}
		if  ( docCollectNumbersFromSelection( &count, &sum,
					    &prod, &min, &max, cf, &ds, 0 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		break;

	    case FVtypeSELECTION:
		if  ( docCollectNumbersFromSelection( &count, &sum,
					    &prod, &min, &max, cf,
					    &(argv[arg].fvSelection),
					    argv[arg].fvDirection ) )
		    { LDEB(1); rval= -1; goto ready;	}
		break;

	    default:
		LLDEB(arg,argv[arg].fvType); rval= -1; goto ready;
	    }
	}

    if  ( pCount )
	{ *pCount= count;	}
    if  ( pSum )
	{ *pSum= sum;	}
    if  ( pProd )
	{ *pProd= prod;	}
    if  ( pMin )
	{ *pMin= min;	}
    if  ( pMax )
	{ *pMax= max;	}

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_FC_Average(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int		count;
    double	sum;

    if  ( doc_FC_All( cf, &count, &sum,
			    (double *)0, (double *)0, (double *)0,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    if  ( count == 0 )
	{ LDEB(count); return -1;	}
    else{
	top->fvDouble= sum/ count;
	top->fvType= FVtypeNUMBER;
	}

    return 0;
    }

static int doc_FC_Count(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int	count;

    if  ( doc_FC_All( cf, &count, (double *)0,
			    (double *)0, (double *)0, (double *)0,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    top->fvDouble= count;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_FC_Defined(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    DocumentSelection		ds;

    switch( argv[0].fvType )
	{
	case FVtypeNUMBER:
	case FVtypeSTRING:
	    top->fvType= FVtypeNUMBER;
	    top->fvDouble= 1;
	    return 0;

	case FVtypeNAME:
	    top->fvType= FVtypeNUMBER;
	    top->fvDouble= ( docFindBookmarkInDocument( &ds,
					cf->cfRecalculateFields->rfDocument,
					&(argv[0].fvBuffer) ) == 0 );
	    return 0;

	default:
	    SDEB(fc->fcName); return -1;
	}
    }

static int doc_FC_False(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    SDEB(fc->fcName); return -1;
    }

static int doc_FC_Int(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;

    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    /* Observed behavior of MS-Word: operates on the absolute value */
    if  ( fv.fvDouble >= 0 )
	{ top->fvDouble= floor( fv.fvDouble );	}
    else{ top->fvDouble=  ceil( fv.fvDouble );	}
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_FC_Min(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int		count;
    double	min;

    if  ( doc_FC_All( cf, &count, (double *)0,
			    (double *)0, &min, (double *)0,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    if  ( count == 0 )
	{ LDEB(count); return -1;	}
    else{
	top->fvDouble= min;
	top->fvType= FVtypeNUMBER;
	}

    return 0;
    }

static int doc_FC_Max(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int		count;
    double	max;

    if  ( doc_FC_All( cf, &count, (double *)0,
			    (double *)0, (double *)0, &max,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    if  ( count == 0 )
	{ LDEB(count); return -1;	}
    else{
	top->fvDouble= max;
	top->fvType= FVtypeNUMBER;
	}

    return 0;
    }

static int doc_FC_Mod(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;
    FormulaValue	fv0, fv1;

    docFormulaInitValue( &fv0 );
    docFormulaInitValue( &fv1 );

    if  ( docFormulaGetNumber( &fv0, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv1, &(argv[1]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    if  ( fv1.fvDouble == 0 )
	{ SFDEB(fc->fcName,fv1.fvDouble); rval= -1; goto ready;	}

    top->fvDouble= fmod( fv0.fvDouble, fv1.fvDouble );
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv0 );
    docFormulaCleanValue( &fv1 );

    return rval;
    }

static int doc_FC_Not(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;

    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    top->fvType= FVtypeNUMBER;
    top->fvDouble= fv.fvDouble == 0.0;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_FC_Or(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;
    FormulaValue	fv0, fv1;

    docFormulaInitValue( &fv0 );
    docFormulaInitValue( &fv1 );

    if  ( docFormulaGetNumber( &fv0, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv1, &(argv[1]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    top->fvDouble= fv0.fvDouble != 0.0 || fv1.fvDouble != 0.0;
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv0 );
    docFormulaCleanValue( &fv1 );

    return rval;
    }

static int doc_FC_Product(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int		count;
    double	prod;

    if  ( doc_FC_All( cf, &count, (double *)0,
			    &prod, (double *)0, (double *)0,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    if  ( count == 0 )
	{ LDEB(count); return -1;	}
    else{
	top->fvDouble= prod;
	top->fvType= FVtypeNUMBER;
	}

    return 0;
    }

static int doc_FC_Round(	const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;
    FormulaValue	fv0, fv1;
    double		mod;

    docFormulaInitValue( &fv0 );
    docFormulaInitValue( &fv1 );

    if  ( docFormulaGetNumber( &fv0, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}
    if  ( docFormulaGetNumber( &fv1, &(argv[1]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    if  ( fv1.fvDouble >= 0 )
	{ mod= pow( 10.0, floor( -fv1.fvDouble ) );	}
    else{ mod= pow( 10.0,  ceil( -fv1.fvDouble ) );	}

    if  ( fv0.fvDouble >= 0 )
	{ top->fvDouble= mod* floor( fv0.fvDouble/ mod+ 0.5 );	}
    else{ top->fvDouble= mod*  ceil( fv0.fvDouble/ mod- 0.5 );	}
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv0 );
    docFormulaCleanValue( &fv1 );

    return rval;
    }

static int doc_FC_Sign(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    int			rval= 0;

    FormulaValue	fv;

    docFormulaInitValue( &fv );

    if  ( docFormulaGetNumber( &fv, &(argv[0]), cf ) )
	{ SDEB(fc->fcName); rval= -1; goto ready;	}

    if  ( fv.fvDouble == 0 )
	{ top->fvDouble= 0;	}
    else{
	if  ( fv.fvDouble > 0 )
	    { top->fvDouble=  1;	}
	else{ top->fvDouble= -1;	}
	}
    top->fvType= FVtypeNUMBER;

  ready:

    docFormulaCleanValue( &fv );

    return rval;
    }

static int doc_FC_Sum(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    double	sum;

    if  ( doc_FC_All( cf, (int *)0, &sum,
			    (double *)0, (double *)0, (double *)0,
			    argv, argc ) )
	{ SDEB(fc->fcName); return -1;	}
    
    top->fvDouble= sum;
    top->fvType= FVtypeNUMBER;

    return 0;
    }

static int doc_FC_True(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    SDEB(fc->fcName); return -1;
    }

static int doc_FC_If(		const FunctionCalculator *	fc,
				const CalculateFormula *	cf,
				FormulaValue *			top,
				const FormulaValue *		argv,
				int				argc )
    {
    SDEB(fc->fcName); return -1;
    }

/************************************************************************/
/*									*/
/*  The function calculators.						*/
/*									*/
/************************************************************************/

static const FunctionCalculator DOC_FunctionCalculators[]=
{
    {	"ABS",		0,  1, doc_FC_Abs },
    {	"AND",		0,  2, doc_FC_And },
    {	"AVERAGE",	1, -1, doc_FC_Average },
    {	"COUNT",	1, -1, doc_FC_Count },
    {	"DEFINED",	0,  1, doc_FC_Defined },
    {	"FALSE",	0,  0, doc_FC_False },
    {	"INT",		0,  1, doc_FC_Int },
    {	"MIN",		1, -1, doc_FC_Min },
    {	"MAX",		1, -1, doc_FC_Max },
    {	"MOD",		0,  2, doc_FC_Mod },
    {	"NOT",		0,  1, doc_FC_Not },
    {	"OR",		0,  2, doc_FC_Or },
    {	"PRODUCT",	1, -1, doc_FC_Product },
    {	"ROUND",	0,  2, doc_FC_Round },
    {	"SIGN",		0,  1, doc_FC_Sign },
    {	"SUM",		1, -1, doc_FC_Sum },
    {	"TRUE",		0,  0, doc_FC_True },

    {	"IF",		0,  3, doc_FC_If },
};

static const int DOC_FunctionCalculatorCount=
		    sizeof(DOC_FunctionCalculators)/sizeof(FunctionCalculator);

const FunctionCalculator * docFormulaGetFunctionCalculator(
						const char *	name,
						int		length )
    {
    int		fun;

    const FunctionCalculator * fc= DOC_FunctionCalculators;
    for ( fun= 0; fun < DOC_FunctionCalculatorCount; fc++, fun++ )
	{
	const char *	s= fc->fcName;
	const char *	f= name;
	int		len= 0;

	while( *s && len < length && tolower(*s) == tolower(*f) )
	    { s++; f++; len++;	}

	if  ( ! *s && len == length )
	    { return fc;	}
	}

    SDEB(name); return (FunctionCalculator *)0;
    } 

