/************************************************************************/
/*									*/
/*  Evaluate = (Formula) fields.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	"docCalculateFormulaValue.h"
#   include	"docSelect.h"
#   include	"docNodeTree.h"
#   include	"docTreeNode.h"
#   include	"docField.h"
#   include	"docFields.h"
#   include	<docDocumentField.h>
#   include	<docFormulaField.h>
#   include	<numbersBase26.h>
#   include	<utilLocale.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docCalculateFormula(	FormulaValue *			fv,
				int *				pField,
				const CalculateFormula *	cf,
				const InfixOperator **		pNextIo,
				int				minPriority,
				const char *			formula );

/************************************************************************/

static int docTableDirection(	DocumentSelection *		dsSlice,
				int*				pDirection,
				const CalculateFormula *	cf,
				const char *			name,
				int				length )
    {
    int			dir;
    int			direction= 0;

    static const char * DOC_TableDirectionNames[]=
	{
	"LEFT", "RIGHT", "ABOVE", "BELOW"
	};
    static const int DOC_TableDirectionNameCount=
			sizeof(DOC_TableDirectionNames)/sizeof(const char *);

    for ( dir= 0; dir < DOC_TableDirectionNameCount; dir++ )
	{
	const char *	s= DOC_TableDirectionNames[dir];
	const char *	f= name;
	int		len= 0;

	while( *s && len < length && tolower(*s) == tolower(*f) )
	    { s++; f++; len++;	}

	if  ( ! *s && len == length )
	    { break;	}
	}

    if  ( dir >= DOC_TableDirectionNameCount )
	{ return 1;	}

    if  ( docTableNesting( cf->cfHeadPosition.dpNode ) == 0 )
	{ return 1;	}

    switch( dir )
	{
	case 0:
	    if  ( docSelectTableLeft( dsSlice, cf->cfHeadPosition.dpNode ) )
		{ LDEB(dir); return -1;	}
	    direction= -1;
	    break;
	case 1:
	    if  ( docSelectTableRight( dsSlice, cf->cfHeadPosition.dpNode ) )
		{ LDEB(dir); return -1;	}
	    direction=  1;
	    break;
	case 2:
	    if  ( docSelectTableAbove( dsSlice, cf->cfHeadPosition.dpNode ) )
		{ LDEB(dir); return -1;	}
	    direction= -1;
	    break;
	case 3:
	    if  ( docSelectTableBelow( dsSlice, cf->cfHeadPosition.dpNode ) )
		{ LDEB(dir); return -1;	}
	    direction=  1;
	    break;
	default:
	    LDEB(dir); return -1;
	}

    *pDirection= direction;
    return 0;
    }

/************************************************************************/

static int docTryRectangleArgument(
				DocumentSelection *		dsSlice,
				int *				pDirection,
				DocumentPosition *		dpRelative,
				int *				pIsBookmark,
				const CalculateFormula *	cf,
				const char *			formula,
				int				arg )
    {
    int			done= 0;
    int			direction= 0;

    const char *	head= formula;
    const char *	tail= formula;
    const char *	here= formula;

    MemoryBuffer	markName;

    const struct SimpleLocale *	sl= cf->cfRecalculateFields->rfLocale;

    utilInitMemoryBuffer( &markName );

    while( isalnum( here[0] ) || here[0] == '_' )
	{ here++;	}

    tail= here;

    if  ( isalpha( head[0] ) )
	{
	int		dir= 1;

	dir= docTableDirection( dsSlice, &direction, cf, head, tail- head );
	if  ( dir < 0 )
	    { LDEB(dir); done= -1; goto ready;	}

	if  ( dir == 0 )
	    { formula += tail- head; done += tail- head;	}

	*pIsBookmark= 0;
	}

    if  ( ! docSelectionIsSet( dsSlice ) && here[0] == ':' )
	{
	int	x0, x1, y0, y1;

	here++;
	while( isalnum( here[0] ) )
	    { here++;	}
	tail= here;

	if  ( numbersBase26Rectangle( &x0, &x1, &y0, &y1,
						head, tail- head ) )
	    { SDEB(formula); done= -1; goto ready;	}

	if  ( docSelectTableRect( dsSlice, dpRelative->dpNode,
							 x0, x1, y0, y1 ) )
	    { LLLLDEB(x0,x1,y0,y1); done= -1; goto ready;	}

	formula += tail- head; done += tail- head;

	*pIsBookmark= 0;
	}

    while( here[0] == ' '	)
	{ here++;	}

    if  ( tail > head					&&
	  ! docSelectionIsSet( dsSlice )		&&
	  arg == 0 && here[0] != ')' && here[0] != sl->slListSeparator	)
	{
	int			i;
	DocumentField *		df;
	struct BufferDocument *	bd= cf->cfRecalculateFields->rfDocument;
	struct DocumentTree *	tree;

	if  ( utilMemoryBufferSetBytes( &markName,
				(unsigned char *)head, tail- head ) )
	    { LDEB(tail-head); done= -1; goto ready;	}

	/* Unused
	formula += here- head;
	*/
	done += here- head;

	i= docFindBookmarkField( &df, bd, &markName );
	if  ( i < 0 )
	    { done= 0; goto ready;	}

	if  ( docGetRootOfSelectionScope( &tree, (struct BufferItem **)0,
					    bd, &(df->dfSelectionScope) ) )
	    { LDEB(1); done= -1; goto ready;	}

	if  ( docPositionForEditPosition( dpRelative,
					    &(df->dfHeadPosition), tree ) )
	    { LDEB(df->dfHeadPosition.epParaNr); done= -1; goto ready; }

	*pIsBookmark= 1;
	}

    *pDirection= direction;

  ready:

    utilCleanMemoryBuffer( &markName );

    return done;
    }

/************************************************************************/
/*									*/
/*  Evaluate a field that is embedded in the field instructions of the	*/
/*  formula field.							*/
/*									*/
/************************************************************************/

static int docCalculateInstructionsField(
					FormulaValue *			top,
					int *				pField,
					const CalculateFormula *	cf,
					const DocumentField *		iField )
    {
    const FieldKindInformation *	fki;
    int					calculated;

    fki= DOC_FieldKinds+ iField->dfKind;

    if  ( fki->fkiLevel != DOClevSPAN		||
	  ! fki->fkiCalculateTextString		)
	{ LXDEB(fki->fkiLevel,fki->fkiCalculateTextString); return -1;	}

    if  ( (*fki->fkiCalculateTextString)( &calculated, &(top->fvBuffer),
					iField, cf->cfRecalculateFields) )
	{ LDEB(1); return -1;	}
    if  ( ! calculated )
	{ LDEB(calculated); return -1;	}
    top->fvType= FVtypeSTRING;

    (*pField)++;
    return iField->dfTailPosition.epStroff- iField->dfHeadPosition.epStroff;
    }

/************************************************************************/
/*									*/
/*  Evaluate the arguments to a function invocation and then the	*/
/*  function return value itself.					*/
/*									*/
/*  1)  Skip initial blanks after the opening parantheses.		*/
/*  2)  Evaluate the function arguments.				*/
/*  3)  Check for an (initial) bookmark, and for a potential table	*/
/*	reference.							*/
/*  4)  Skip blanks after the argument					*/
/*  5)  After a comma, skip to the next argument.			*/
/*									*/
/************************************************************************/

static int docCalculateFormulaFunction(
				    FormulaValue *		top,
				    int *			pField,
				    const CalculateFormula *	cf,
				    const FunctionCalculator *	fc,
				    const char *		formula )
    {
    int			done= 0;
    int			arg;

    FormulaValue *	argv= (FormulaValue *)0;
    int			argc= 0;

    DocumentPosition	dpRelative= cf->cfHeadPosition;

    const struct SimpleLocale *	sl= cf->cfRecalculateFields->rfLocale;

    /*  1  */
    while( formula[0] == ' '	)
	{ formula++; done++;	}

    /*  2  */
    while( formula[0] != ')' )
	{
	const int		minPriority= 0;
	FormulaValue *		nx;
	int			step;

	DocumentSelection	dsSlice;
	int			direction= 0;

	docInitDocumentSelection( &dsSlice );

	/*  3  */
	if  ( fc->fcTakesRectangle )
	    {
	    int		rectStep;
	    int		isBookmark= 0;

	    rectStep= docTryRectangleArgument( &dsSlice, &direction,
						&dpRelative, &isBookmark,
						cf, formula, argc );
	    if  ( rectStep < 0 )
		{ SLDEB(formula,rectStep); done= -1; goto ready;	}

	    formula += rectStep; done += rectStep;

	    if  ( isBookmark )
		{ continue;	}
	    }

	if  ( fc->fcArgc >= 0 && argc >= fc->fcArgc )
	    { LLDEB(fc->fcArgc,argc); done= -1; goto ready;	}

	nx= realloc( argv, ( argc+ 1 )* sizeof( FormulaValue ) );
	if  ( ! nx )
	    { LXDEB(argc,nx); done= -1; goto ready;	}
	argv= nx;
	docFormulaInitValue( &(argv[argc++]) );

	if  ( docSelectionIsSet( &dsSlice ) )
	    {
	    argv[argc-1].fvSelection= dsSlice;
	    argv[argc-1].fvType= FVtypeSELECTION;
	    argv[argc-1].fvDirection= direction;
	    }
	else{
	    step= docCalculateFormula( &(argv[argc-1]), pField, cf,
			    (const InfixOperator **)0, minPriority, formula );
	    if  ( step < 0 )
		{ SLDEB(formula,step); done= -1; goto ready;	}

	    formula += step; done += step;
	    }

	/*  4  */
	while( formula[0] == ' '	)
	    { formula++; done++;	}

	/*  5  */
	if  ( formula[0] == sl->slListSeparator )
	    {
	    formula++; done++;
	    while( formula[0] == ' '	)
		{ formula++; done++;	}

	    continue;
	    }
	}

    if  ( formula[0] != ')' )
	{ SSDEB(formula,fc->fcName); done= -1; goto ready;	}
    formula++; done++;

    if  ( fc->fcArgc >= 0 && argc < fc->fcArgc )
	{ LLDEB(fc->fcArgc,argc); done= -1; goto ready;	}

    if  ( (*fc->fcCalculate)( fc, cf, top, argv, argc ) )
	{ SDEB(fc->fcName); done= -1; goto ready;	}

  ready:

    for ( arg= 0; arg < argc; arg++ )
	{ docFormulaCleanValue( &(argv[arg]) );	}
    if  ( argv )
	{ free( argv );	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Calculate an operand in a formula.					*/
/*									*/
/*  We rely on the fact that tokens do not protrude beyond upto.	*/
/*									*/
/*  1)  Do not skip initial blanks: Blanks are skipped at the head of	*/
/*	the formula and AFTER every operand and AFTER every operator.	*/
/*  2)  Found a number: Evaluate and return.				*/
/*  3)  Found a string value: Evaluate and return.			*/
/*  4)  Found an expression in parantheses: Evaluate and return.	*/
/*  5)  A name or a function invocation.				*/
/*									*/
/************************************************************************/

static int docCalculateFormulaOperand(
				FormulaValue *			fv,
				int *				pField,
				const CalculateFormula *	cf,
				const char *			formula )
    {
    int		done= 0;

    /*  1  */

    /*  2  */
    if  ( isdigit( formula[0] ) )
	{
	char *	e= (char *)formula;

	fv->fvDouble= strtod( formula, &e );
	if  ( e == formula )
	    { SDEB(formula); return -1;	}

	done += (int)( e- formula );
	formula += (int)( e- formula );

	while( formula[0] == ' '	)
	    { formula++; done++;	}

	if  ( formula[0] == '%' )
	    {
	    fv->fvDouble /= 100.0;
	    formula++; done++;
	    }

	fv->fvType= FVtypeNUMBER;
	return done;
	}

    /*  3  */
    if  ( formula[0] == '"' )
	{
	formula++; done++;

	while( formula[0] )
	    {
	    const char *	here= formula;

	    while( formula[0] && formula[0] != '"' )
		{ formula++; done++;	}

	    if  ( ! formula[0] )
		{ SDEB(formula); return -1;	}

	    /* Not at the end of the string and no double quote */
	    if  ( formula[1]		&&
		  formula[1] != '"'	)
		{
		if  ( utilMemoryBufferAppendBytes( &(fv->fvBuffer),
				    (unsigned char *)here, formula- here ) )
		    { SDEB(formula); return -1;	}

		formula++; done++; break;
		}
	    else{
		formula++; done++;

		if  ( utilMemoryBufferAppendBytes( &(fv->fvBuffer),
				    (unsigned char *)here, formula- here ) )
		    { SDEB(formula); return -1;	}

		formula++; done++;
		}
	    }

	fv->fvType= FVtypeSTRING;
	return done;
	}

    /*  4  */
    if  ( formula[0] == '(' )
	{
	const int	minPriority= 0;
	int		step;

	formula++; done++;
	while( formula[0] == ' '	)
	    { formula++; done++;	}

	step= docCalculateFormula( fv, pField, cf,
			    (const InfixOperator **)0, minPriority, formula );
	if  ( step < 0 )
	    { SLDEB(formula,step); return -1;	}
	
	done += step; formula += step;

	if  ( formula[0] != ')' )
	    { SDEB(formula); return -1;	}

	done++;
	return done;
	}

    /*  5  */
    if  ( isalpha( formula[0] ) )
	{
	const char *	head= formula;
	const char *	tail= formula;

	formula++; done++;

	while( isalnum( formula[0] ) || formula[0] == '_' )
	    { formula++; done++;	}

	tail= formula;

	while( formula[0] == ' '	)
	    { formula++; done++;	}

	if  ( formula[0] == '(' )
	    {
	    int				step;
	    const FunctionCalculator *	fc;

	    formula++; done++;

	    fc= docFormulaGetFunctionCalculator( head, tail- head );
	    if  ( ! fc )
		{ SXDEB(head,fc); return -1;	}

	    step= docCalculateFormulaFunction( fv, pField, cf, fc, formula );
	    if  ( step < 0 )
		{ SSDEB(formula,fc->fcName); return -1;	}

	    formula += step; done += step;
	    return done;
	    }
	else{
	    if  ( utilMemoryBufferSetBytes( &(fv->fvBuffer),
				(unsigned char *)head, tail- head ) )
		{ LDEB(tail-head); return -1;	}

	    fv->fvType= FVtypeNAME;
	    return done;
	    }
	}

    SDEB(formula); return -1;
    }

/************************************************************************/
/*									*/
/*  Calculate one operand in a formula and look for a subsequent infix	*/
/*  operator.								*/
/*									*/
/*  1)  Scan and evaluate the operand.					*/
/*  2)  Skip space upto the operator (Or the end of the expression)	*/
/*  3)  If we are not at the end of the expression, find the next	*/
/*	operator.							*/
/*  4)  Skip space after the operator					*/
/*									*/
/************************************************************************/

static int docCalculateOperand(	FormulaValue *			top,
				int *				pField,
				const InfixOperator **		pNextIo,
				const CalculateFormula *	cf,
				const char *			formula )
    {
    int				done= 0;
    int				step;
    const InfixOperator *	nextIo= (const InfixOperator *)0;
    const PrefixOperator *	po= (const PrefixOperator *)0;
    
    const DocumentField *	iField= (const DocumentField *)0;

    const struct SimpleLocale *	sl= cf->cfRecalculateFields->rfLocale;

    po= docFormulaGetPrefixOperator( &step, formula );
    if  ( po )
	{
	done += step; formula += step;

	while( formula[0] == ' '	)
	    { done++; formula++; 	}
	}

    if  ( *pField < cf->cfField->dfInstructionFields.cfChildCount )
	{
	const DocumentField *	df;

	df= cf->cfField->dfInstructionFields.cfChildren[*pField];

	if  ( df->dfHeadPosition.epStroff == formula- cf->cfFormulaString )
	    { iField= df;	}
	}

    /*  1  */
    if  ( iField )
	{
	step= docCalculateInstructionsField( top, pField, cf, iField );
	if  ( step < 0 )
	    { SLDEB(formula,step); return -1;	}
	}
    else{
	step= docCalculateFormulaOperand( top, pField, cf, formula );
	if  ( step < 0 )
	    { SLDEB(formula,step); return -1;	}
	}

    done += step; formula += step;

    if  ( po && po->poOperation( po, cf, top ) )
	{ SDEB(po->poSymbol); return -1;	}
    
    /*  2  */
    while( formula[0] == ' '	)
	{ done++; formula++; 	}

    /*  3  */
    if  ( *formula && *formula != ')' && *formula != sl->slListSeparator )
	{
	nextIo= docFormulaGetInfixOperator( &step, formula );
	if  ( ! nextIo )
	    { SXDEB(formula,nextIo); return -1;	}

	formula += step; done += step;

	/*  4  */
	while( formula[0] == ' '	)
	    { done++; formula++; 	}
	}

    *pNextIo= nextIo;
    return done;
    }

/************************************************************************/
/*									*/
/*  Calculate a formula							*/
/*									*/
/*  1)  Calculate the first operand in the formula.			*/
/*  2)  Consume infix operators and right hand side operands until we	*/
/*	reach the end of the formula or we find an operator with lower	*/
/*	priority.							*/
/*									*/
/************************************************************************/

static int docCalculateFormula(	FormulaValue *			top,
				int *				pField,
				const CalculateFormula *	cf,
				const InfixOperator **		pNextIo,
				int				minPriority,
				const char *			formula )
    {
    int				done= 0;
    int				step;
    const InfixOperator *	io= (const InfixOperator *)0;

    FormulaValue		nextFv;

    docFormulaInitValue( &nextFv );

    /*  1  */
    step= docCalculateOperand( top, pField, &io, cf, formula );
    if  ( step < 0 )
	{ SLDEB(formula,step); done= -1; goto ready;	}

    done += step; formula += step;

    /*  2  */
    while( io && io->ioPriority >= minPriority )
	{
	const InfixOperator *	nextIo= (InfixOperator *)0;

	step= docCalculateFormula( &nextFv, pField, cf, &nextIo,
						io->ioPriority+ 1, formula );
	if  ( step < 0 )
	    { SLDEB(formula,step); done= -1; goto ready;	}

	done += step; formula += step;

	if  ( io->ioOperation( io, cf, top, &nextFv ) )
	    { SDEB(io->ioSymbol); done= -1; goto ready;	}

	docFormulaCleanValue( &nextFv );
	docFormulaInitValue( &nextFv );

	io= nextIo;
	}

  ready:

    docFormulaCleanValue( &nextFv );

    if  ( pNextIo )
	{ *pNextIo= io;	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Return the value of the formula.					*/
/*									*/
/*  1)  Skip initial blanks. Everywhere else, we skip trailing blanks.	*/
/*	This is to minimize the amount of blank-skipping.		*/
/*									*/
/************************************************************************/

int docCalculateFormulaFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int				rval= 0;
    int				step;
    const int			minPriority= 0;
    const char *		formula;
    int				field= 0;

    CalculateFormula		cf;
    FormulaField		ff;
    FormulaValue		fv;
    FormulaValue		fv2;

    docFormulaInitValue( &fv );
    docFormulaInitValue( &fv2 );
    docInitFormulaField( &ff );

    if  ( docGetFormulaField( &ff, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    cf.cfFormulaField= &ff;
    cf.cfField= df;
    cf.cfRecalculateFields= rf;

    if  ( docPositionForEditPosition( &(cf.cfHeadPosition),
					&(df->dfHeadPosition), rf->rfTree ) )
	{ LDEB(df->dfHeadPosition.epParaNr); /* rval= -1; */ goto ready; }

    cf.cfRowNode= docGetRowNode( cf.cfHeadPosition.dpNode );

    cf.cfFormulaString= utilMemoryBufferGetString( ff.ffFormula );
    formula= cf.cfFormulaString;

    /*  1  */
    while( formula[0] == ' '	)
	{ formula++; 	}
    if  ( ! formula[0] )
	{ SDEB(formula); /*rval= -1;*/ goto ready;	}

    step= docCalculateFormula( &fv, &field, &cf,
			    (const InfixOperator **)0, minPriority, formula );
    if  ( step < 0 )
	{ SLDEB(formula,step); /* rval= -1; */ goto ready;	}

    if  ( ff.ffPicture )
	{
	if  ( docFormulaGetNumber( &fv2, &fv, &cf ) )
	    { LDEB(fv.fvType); /* rval= -1; */ goto ready;	}

	if  ( docFieldFormatNumber( mbResult, ff.ffPicture, fv2.fvDouble,
								rf->rfLocale ) )
	    { LDEB(1); /* rval= -1; */ goto ready;	}
	}
    else{
	if  ( docFormulaGetString( &fv2, &fv, &cf ) )
	    { LDEB(fv.fvType); /* rval= -1; */ goto ready;	}

	if  ( utilCopyMemoryBuffer( mbResult, &(fv2.fvBuffer) ) )
	    { LDEB(1); /* rval= -1; */ goto ready;	}
	}

    *pCalculated= 1;

  ready:

    docCleanFormulaField( &ff );
    docFormulaCleanValue( &fv );
    docFormulaCleanValue( &fv2 );

    return rval;
    }

int docCalculateIfFieldString(	int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    if  ( utilMemoryBufferSetString( mbResult, "IF" ) )
	{ LDEB(1); return -1;	}

    *pCalculated= 1; return 0;
    }

