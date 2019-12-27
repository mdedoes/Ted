#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	"jsonPath.h"

#   include	<appDebugon.h>

static void jsonInitFilterTerm(	JsonFilterTerm *	jft )
    {
    jft->jftExpression= (JsonPath *)0;
    jft->jftConstant= (json_t *)0;
#   if USE_LIBPCRE2
    jft->jftPattern= (pcre2_code *)0;
#   endif
    }

static void jsonCleanFilterTerm(	JsonFilterTerm *	jft )
    {
    if  ( jft->jftExpression )
	{ jsonFreePath( jft->jftExpression );	}

    if  ( jft->jftConstant )
	{ json_decref( jft->jftConstant );	}

#   if USE_LIBPCRE2
    if  ( jft->jftPattern )
	{ pcre2_code_free( jft->jftPattern );	}
#   endif
    }

void jsonPathInitComponent(	JsonPathComponent *		jpc )
    {
    jpc->jpcNext= (JsonPathComponent *)0;

    jpc->jpcSubscript0= 0;
    jpc->jpcSubscript1= 0;
    jpc->jpcKey= (char *)0;
    jsonInitFilterTerm( &(jpc->jpcFilterTerm0) );
    jsonInitFilterTerm( &(jpc->jpcFilterTerm1) );
    jpc->jpcFilterOperator= -1;
    jpc->jpcTestChild= 0;
    jpc->jpcCollectMatches= 0;

    jpc->jpcAlternatives= (JsonPathComponent *)0;
    jpc->jpcAlternativeCount= 0;
    }

void jsonPathCleanComponent(	JsonPathComponent *		jpc )
    {
    int		a;

    if  ( jpc->jpcKey )
	{ free( jpc->jpcKey );	}

    jsonCleanFilterTerm( &(jpc->jpcFilterTerm0) );
    jsonCleanFilterTerm( &(jpc->jpcFilterTerm1) );

    for ( a= 0; a < jpc->jpcAlternativeCount; a++ )
	{ jsonPathCleanComponent( &(jpc->jpcAlternatives[a]) );	}

    if  ( jpc->jpcAlternatives )
	{ free( jpc->jpcAlternatives );	}
    }

void jsonInitPath(	JsonPath *		jp )
    {
    jp->jpPath= (JsonPathComponent *)0;
    }

void jsonFreePath(	JsonPath *		jp )
    {
    JsonPathComponent *	next= jp->jpPath;

    while( next )
	{
	JsonPathComponent *	curr= next;

	next= next->jpcNext;

	jsonPathCleanComponent( curr );
	free( curr );
	}

    free( jp );
    }

JsonPathComponent * jsonPathAddAlternative(	JsonPathComponent *	jpc )
    {
    JsonPathComponent *	fresh;

    fresh= realloc( jpc->jpcAlternatives,
		( jpc->jpcAlternativeCount+ 1 )* sizeof(JsonPathComponent) );
    if  ( ! fresh )
	{
	LXDEB(jpc->jpcAlternativeCount,fresh);
	return (JsonPathComponent *)0;
	}

    jpc->jpcAlternatives= fresh;
    fresh += jpc->jpcAlternativeCount++;
    jsonPathInitComponent( fresh );

    return fresh;
    }

# if 0

/*
json_t * res= jsonPathFindByExpr( "$.store.book[*].author", root );
json_t * res= jsonPathFindByExpr( "$.store.*", root );
json_t * res= jsonPathFindByExpr( "$.store.book[0].author", root );
json_t * res= jsonPathFindByExpr( "$.store.book[-1].author", root );
json_t * res= jsonPathFindByExpr( "$.store['book'][1].author", root );
json_t * res= jsonPathFindByExpr( "$.store['book'].length", root );
json_t * res= jsonPathFindByExpr( "$.store['book'][2:3].author", root );
json_t * res= jsonPathFindByExpr( "$..author", root );
json_t * res= jsonPathFindByExpr( "$.store..price", root );
json_t * res= jsonPathFindByExpr( "$..price", root );
json_t * res= jsonPathFindByExpr( "$..book[2]", root );
json_t * res= jsonPathFindByExpr( "$..book[?(@.price<10)]", root );
json_t * res= jsonPathFindByExpr( "$.store.book[?(@.price<10)]", root );
json_t * res= jsonPathFindByExpr( "$.store.book[?(@.isbn)]", root );
json_t * res= jsonPathFindByExpr( "$.store.book[?(@.isbn)].title", root );
json_t * res= jsonPathFindByExpr( "$.store.book[?(@.price<10)].title", root );
json_t * res= jsonPathFindByExpr( "$..book[?(@.price<10)].isbn", root );
json_t * res= jsonPathFindByExpr( "$..book[?(@.title=~/^T/)].title", root );
json_t * res= jsonPathFindByExpr( "$..book[?(@.title=~/^T.*a/)].title", root );
json_t * res= jsonPathFindByExpr( "$..book[?(@.title=~/^t.*a/i)].title", root );
json_t * res= jsonPathFindByExpr( "$..book.length", root );
json_t * res= jsonPathFindByExpr( "$['store','library'].book[*].title", root );
*/

extern int xxx( void );

# include "sioFileio.h"
# include "sioGeneral.h"

int xxx( void )
    {
    const char *	filename= "goessner.json";
    SimpleInputStream * sis= sioInFileioOpenS( filename );

    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    json_t * root= jsonLoad( sis );
    sioInClose( sis );
    if  ( ! root )
	{ SXDEB(filename,root); return -1;	}

    json_t * res= jsonPathFindByExpr( "$['store','library'].book[*].title", root );
    if  ( ! res )
	{
	XDEB(res);
	json_decref( root );
	return -1;
	}

    char * s = json_dumps( res, JSON_INDENT(4) );
    SDEB(s);
    free(s);

    json_decref( res );
    json_decref( root );
    return 0;
    }
# endif

#   endif	/*	USE_LIBJANSSON		}}	*/
