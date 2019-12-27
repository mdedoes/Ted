#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>
#   include	<string.h>

#   include	"jsonPathImpl.h"

#   include	"jsonPath.h"

#   include	<appDebugon.h>

static const FilterOperator JsonFilterOperators[]=
{
    { "==",  2, FO_EQ		},
    { "!=",  2, FO_NE		},
    { "<=",  2, FO_LE		},
    { ">=",  2, FO_GE		},
    { "<",   1, FO_LT		},
    { ">",   1, FO_GT		},
#   if USE_LIBPCRE2
    { "=~",  2, FO_MATCHES	},
#   endif
};
static const int JsonFilterOperatorCount=
		    sizeof(JsonFilterOperators)/ sizeof(FilterOperator);

static int jsonTestDirectionalCompare(
			json_t *			v0,
			json_t *			v1,
			int				oper )
    {
    int		cmp= 0;
    int		supported= 0;

    if  ( json_is_string( v0 ) && json_is_string( v1 ) )
	{
	cmp= strcmp( json_string_value( v0 ), json_string_value( v1 ) );

	cmp= ( ( cmp > 0 ) - ( cmp < 0 ) );
	supported= 1;
	}

    if  ( json_is_integer( v0 ) && json_is_integer( v1 ) )
	{
	json_int_t i0= json_integer_value( v0 );
	json_int_t i1= json_integer_value( v1 );

	cmp= ( ( i0 > i1 ) - ( i1 > i0 ) );
	supported= 1;
	}

    if  ( json_is_number( v0 ) && json_is_number( v1 ) )
	{
	double d0= json_number_value( v0 );
	double d1= json_number_value( v1 );

	cmp= ( ( d0 > d1 ) - ( d1 > d0 ) );
	supported= 1;
	}

    switch( oper )
	{
	case FO_GE:
	    return supported && cmp >= 0;
	case FO_LE:
	    return supported && cmp <= 0;
	case FO_GT:
	    return supported && cmp > 0;
	case FO_LT:
	    return supported && cmp < 0;
	default:
	    LDEB(oper);
	    return -1;
	}
    }

static int jsonValuesEqual(
			json_t *			v0,
			json_t *			v1 )
    {
    if  ( json_typeof( v0 ) != json_typeof( v1 )	&&
	  json_is_number( v0 )				&&
	  json_is_number( v1 )				)
	{
	return json_number_value( v0 ) == json_number_value( v1 );
	}
    else{ return json_equal( v0, v1 );	}
    }

static int jsonTestCompare(
			json_t *			v0,
			json_t *			v1,
			int				oper )
    {
    switch( oper )
	{
	case FO_EQ:
	    return jsonValuesEqual( v0, v1 );
	case FO_NE:
	    return ! jsonValuesEqual( v0, v1 );

	case FO_GE:
	case FO_LE:
	case FO_GT:
	case FO_LT:
	    return jsonTestDirectionalCompare( v0, v1, oper );
	case FO_EXISTS:
	default:
	    LDEB(oper);
	    return -1;
	}
    }

static int jsonTestFilter(	json_t *			into,
				const JsonPathComponent *	jpc )
    {
    int		rval= -1;
    json_t *	eval0= (json_t *)0;
    json_t *	eval1= (json_t *)0;

    json_t *	v0= (json_t *)0;
    json_t *	v1= (json_t *)0;

    switch( jpc->jpcFilterTerm0.jftType )
	{
	case JFT_EXPRESSION:
	    eval0= jsonPathFindByPattern(
				jpc->jpcFilterTerm0.jftExpression, into );
	    if  ( eval0 && json_array_size( eval0 ) == 1 )
		{
		v0= json_array_get( eval0, 0 );

		/* Does not need or expect a second operand */
		if  ( jpc->jpcFilterOperator == FO_EXISTS )
		    { rval= 1; goto ready;	}
		}
	    else{ rval= 0; goto ready;	}
	    break;

	case JFT_CONSTANT:
	    v0= jpc->jpcFilterTerm0.jftConstant;
	    break;

	default:
	    LDEB(jpc->jpcFilterTerm0.jftType);
	    goto ready;
	}

    switch( jpc->jpcFilterTerm1.jftType )
	{
	case JFT_EXPRESSION:
	    eval1= jsonPathFindByPattern(
				jpc->jpcFilterTerm1.jftExpression, into );
	    if  ( eval1 && json_array_size( eval1 ) == 1 )
		{ v1= json_array_get( eval1, 0 );	}
	    else{ rval= 0; goto ready;			}
	    break;

	case JFT_CONSTANT:
	    v1= jpc->jpcFilterTerm1.jftConstant;
	    break;

#	if USE_LIBPCRE2
	case JFT_PATTERN:
	    rval= jsonTestMatch( v0, jpc->jpcFilterTerm1.jftPattern, jpc );
	    if  ( rval < 0 )
		{ LDEB(rval);	}
	    goto ready;
	    break;
#	endif

	default:
	    LDEB(jpc->jpcFilterTerm1.jftType);
	    goto ready;
	}

    rval= jsonTestCompare( v0, v1, jpc->jpcFilterOperator );
    if  ( rval < 0 )
	{ LDEB(rval);	}

  ready:

    if  ( eval0 )
	{ json_decref( eval0 );	}
    if  ( eval1 )
	{ json_decref( eval1 );	}

    return rval;
    }

static int jsonParseFilterTerm(	JsonFilterTerm *	jft,
				const char *		from,
				const char **		pUpto )
    {
    const char *	upto= from;

    switch( upto[0] )
	{
	case '@':
	    upto++;
	    jft->jftExpression= malloc( sizeof(JsonPath) );
	    if  ( ! jft->jftExpression )
		{ XDEB(jft->jftExpression); return -1;	}
	    jsonInitPath( jft->jftExpression );

	    if  ( jsonPathParseComponents( jft->jftExpression, upto, &upto ) )
		{ SDEB(upto); return -1;	}

	    jft->jftType= JFT_EXPRESSION;
	    break;

	case '\'':
	    {
	    char * constant= jsonPathParseQuotedString( upto, &upto );
	    if  ( ! constant )
		{ SXDEB(from,constant); return -1;	}
	    jft->jftConstant= json_string( constant );
	    if  ( ! jft->jftConstant )
		{
		SXDEB(constant,jft->jftConstant);
		free( constant );
		return -1;
		}
	    free( constant );
	    jft->jftType= JFT_CONSTANT;
	    }
	    break;

	case '-':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':

	case 't': case 'f': case 'n':
	case '"': case '{': case '[':

	    jft->jftConstant= jsonPathParseConstant( upto, &upto );
	    if  ( ! jft->jftConstant )
		{ SXDEB(upto,jft->jftConstant); return -1;	}
	    jft->jftType= JFT_CONSTANT;
	    break;

#	if USE_LIBPCRE2
	case '/':
	    jft->jftPattern= jsonPathParsePattern( upto, &upto ); 
	    if  ( ! jft->jftPattern )
		{ SXDEB(upto,jft->jftPattern); return -1;	}
	    jft->jftType= JFT_PATTERN;
	    break;
#	endif

	default:
	    SDEB(upto); return -1;
	}

    *pUpto= upto;
    return 0;
    }

int jsonPathParseByFilter(	JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    const char *	upto= from++;

    if  ( upto[0] != '?' )
	{ SDEB(upto); return -1;	}
    upto++;

    if  ( upto[0] != '(' )
	{ SDEB(upto); return -1;	}
    upto++;

    if  ( jsonParseFilterTerm( &(jpc->jpcFilterTerm0), upto, &upto ) )
	{ SDEB(upto); return -1;	}

    if  ( upto[0] != ')' )
	{
	int	o;

	for ( o= 0; o < JsonFilterOperatorCount; o++ )
	    {
	    const FilterOperator *	fo= &(JsonFilterOperators[o]);

	    if  ( ! strncmp( upto, fo->foSymbol, fo->foLength ) )
		{
		upto += fo->foLength;

		if  ( jsonParseFilterTerm( &(jpc->jpcFilterTerm1),
								upto, &upto ) )
		    { SDEB(upto); return -1;	}

		jpc->jpcFilterOperator= fo->foId;
		break;
		}
	    }
	}
    else{
	if  ( jpc->jpcFilterTerm0.jftType != JFT_EXPRESSION )
	    { LSDEB(jpc->jpcFilterTerm0.jftType,from); return -1; }

	jpc->jpcFilterOperator= FO_EXISTS;
	}

    if  ( upto[0] != ')' )
	{ SDEB(upto); return -1;	}
    upto++;

    jpc->jpcTestChild= jsonTestFilter;
    jpc->jpcCollectMatches= jsonPathCollectChildren;
    *pUpto= upto;

    return 0;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
