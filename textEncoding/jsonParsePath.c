#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>
#   include	<string.h>

#   include	"jsonPathImpl.h"

#   include	"jsonPath.h"
#   include	"uniUtf8.h"

#   include	<appDebugon.h>

char * jsonPathParseName(	const char *		from,
				const char **		pUpto )
    {
    const char *	upto= from;
    int			n;
    char *		rval;

    if  ( upto[0] == '$' || upto[0] == '_' || isalpha( upto[0] ) )
	{
	upto++;
	while( upto[0] == '$' || upto[0] == '_' || isalnum( upto[0] ) )
	    { upto++;	}
	}

    n= upto- from;
    if  ( n < 1 )
	{ SLDEB(from,n); return (char *)0;	}

    rval= malloc( n+ 1 );
    if  ( ! rval )
	{ LXDEB(n,rval); return (char *)0;	}

    strncpy( rval, from, n )[n]= '\0';

    *pUpto= upto;
    return rval;
    }

/**
 * Consume a quoted string. This is a little more tolerant than 
 * the _string_ syntax on json.org. 
 */
char * jsonPathParseQuotedString( const char *		from,
				const char **		pUpto )
    {
    int			quote= *(from++);
    const char *	upto= from;
    const char *	past;

    const char *	f;
    char *		t;

    int			n;
    char *		rval;

    /* over-estimates the number of bytes needed */
    while( upto[0] )
	{
	if  ( upto[0] == '\\' && upto[1] )
	    { upto += 2; continue;	}

	if  ( upto[0] == quote )
	    { break;	}

	upto++;
	}

    past= upto;

    if  ( upto[0] != quote )
	{ XSDEB(quote,upto); return (char *)0;	}
    upto++;

    n= past- from;
    rval= malloc( n+ 1 );
    if  ( ! rval )
	{ XDEB(rval); return (char *)0;	}

    f= from;
    t= rval;
    while( f[0] )
	{
	if  ( f[0] == '\\' && f[1] )
	    {
	    f++;

	    switch( f[0] )
		{
		char	xdigits[5];

		case '\\': case '"': case '\'': case '/':
		    *(t++)= *(f++);
		    continue;

		case 'b': f++; *(t++)= '\b'; continue;
		case 'f': f++; *(t++)= '\f'; continue;
		case 'n': f++; *(t++)= '\n'; continue;
		case 'r': f++; *(t++)= '\r'; continue;
		case 't': f++; *(t++)= '\t'; continue;

		case 'u':
		    if  ( isxdigit( f[1] )	&&
			  isxdigit( f[2] )	&&
			  isxdigit( f[3] )	&&
			  isxdigit( f[4] )	)
			{
			int	unicode;
			int	step;

			memcpy( xdigits, f+ 1, 4 );
			xdigits[4]= '\0';
			f += 5;
			unicode= strtol( xdigits, (char **)0, 16 );
			step= uniPutUtf8( t, unicode );
			if  ( step < 1 )
			    { SXLDEB(xdigits,unicode,step);	}
			else{ t += step;			}
			}
		    continue;

		default:
		    SDEB(f);
		    *(t++)= *(f++);
		    continue;
		}
	    }
	else{
	    if  ( f[0] == quote )
		{ break;	}
	    else{ *(t++)= *(f++); continue;	}
	    }
	}
    t[0] = '\0';

    *pUpto= upto;

    return rval;
    }

static int jsonParseBracketStar(	JsonPathComponent *	jpc,
					const char *		from,
					const char **		pUpto )
    {
    const char *	upto= from+ 1;

    if  ( upto[0] != ']' )
	{ SDEB(upto); return -1;	}
    upto++;

    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonPathCollectChildren;
    *pUpto= upto;

    return 0;
    }

static int jsonParseDotStar(		JsonPathComponent *	jpc,
					const char *		from,
					const char **		pUpto )
    {
    const char *	upto= from+ 1;

    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonPathCollectChildren;
    *pUpto= upto;

    return 0;
    }

json_t * jsonPathParseConstant(	const char *		from,
				const char **		pUpto )
    {
    json_error_t	json_error;

    json_t * rval= json_loads( from,
		JSON_DECODE_ANY|JSON_DISABLE_EOF_CHECK, &json_error );

    if  ( ! rval )
	{
	XLSDEB(rval,json_error.position,json_error.text);
	SDEB(from); return (json_t *)0;
	}

    *pUpto= from+ json_error.position;
    return rval;
    }

static int jsonParseBracketTerm( JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    const char *	p;

    if  ( from[0] == '\'' || from[1] == '"' )
	{
	if  ( jsonPathParseByQuotedKey( jpc, from, &p ) )
	    { SDEB(from); return -1;	}
	*pUpto= p; return 0;
	}

    if  ( isdigit( from[0] ) || from[0] == '-' )
	{
	if  ( jsonPathParseBySubscript( jpc, from, &p ) )
	    { SDEB(from); return -1;	}
	*pUpto= p; return 0;
	}

    if  ( from[0] == '?' )
	{
	if  ( jsonPathParseByFilter( jpc, from, &p ) )
	    { SDEB(from); return -1;	}
	*pUpto= p; return 0;
	}

    SDEB(from); return -1;
    }

# if 1
/* Collect Union... TODO: Return unique results */
static int jsonPathCollectUnion( json_t *			target,
				const JsonPathComponent *	jpc,
				json_t *			curr )
    {
    int		a;

    for ( a= 0; a < jpc->jpcAlternativeCount; a++ )
	{
	JsonPathComponent *	jpcA;

	jpcA= &(jpc->jpcAlternatives[a]);

	/* HACK */
	jpcA->jpcNext= jpc->jpcNext;

	if  ( (*jpcA->jpcCollectMatches)( target, jpcA, curr ) )
	    { LDEB(a); return -1;	}
	}

    return 0;
    }
# endif

static int jsonParseBracket(	JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    const char *	p;
    const char *	upto= from;
    int			count= 0;

    if  ( upto[0] == '*' )
	{
	if  ( jsonParseBracketStar( jpc, upto, &p ) )
	    { SDEB(upto); return -1;	}
	*pUpto= p; return 0;
	}

    for (;;)
	{
	JsonPathComponent *	jpcTo;
	if  ( count > 0 )
	    {
	    jpcTo= jsonPathAddAlternative( jpc );
	    if  ( ! jpcTo )
		{ XDEB(jpcTo); return -1;	}
	    }
	else{ jpcTo= jpc;			}

	if  ( jsonParseBracketTerm( jpcTo, upto, &p ) )
	    { SDEB(upto); return -1;	}

	count++;
	if  ( p[0] == ']' )
	    { break;	}

#	if 1
	if  ( p[0] == ',' )
	    {
	    /* Bookkeeping+ parse again */
	    if  ( count == 1 )
		{
		jsonPathCleanComponent( jpc );
		jsonPathInitComponent( jpc );

		jpc->jpcCollectMatches= jsonPathCollectUnion;

		jpcTo= jsonPathAddAlternative( jpc );
		if  ( ! jpcTo )
		    { XDEB(jpcTo); return -1;	}

		if  ( jsonParseBracketTerm( jpcTo, upto, &p ) )
		    { SDEB(upto); return -1;	}
		}

	    upto= p+ 1; continue;
	    }
#	endif

	SSDEB(upto,p); return -1;
	}

    /* Does not make sense yet:
    for ( n= 0; n < jpc->jpcAlternativeCount; n++ )
	{ jpc->jpcAlternatives[n].jpcNext= jpc->jpcNext;	}
    */

    *pUpto= p+ 1; return 0;
    }

static int jsonParseComponent(	JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    const char *	p;

    switch( from[0] )
	{
	case '.':
	    if  ( isalpha( from[1] ) )
		{
		if  ( jsonParseDotName( jpc, from+ 1, &p ) )
		    { SDEB(from); return -1;	}
		*pUpto= p; return 0;
		}
	    if  ( from[1] == '*' )
		{
		if  ( jsonParseDotStar( jpc, from+ 1, &p ) )
		    { SDEB(from); return -1;	}
		*pUpto= p; return 0;
		}
	    if  ( from[1] == '.' )
		{
		if  ( jsonParseDotDot( jpc, from+ 1, &p ) )
		    { SDEB(from); return -1;	}
		*pUpto= p; return 0;
		}
	    break;

	case '[':
	    if  ( jsonParseBracket( jpc, from+ 1, &p ) )
		{ SDEB(from); return -1;	}
	    *pUpto= p; return 0;

	default:
	    break;
	}

    return 1;
    }

int jsonPathParseComponents(	JsonPath *		jp,
				const char *		from,
				const char **		pUpto )
    {
    const char *	p= from;
    JsonPathComponent *	prev= (JsonPathComponent *)0;

    for (;;)
	{
	JsonPathComponent *	jpc;
	int			res;

	while( p[0] == ' ' )
	    { p++;	}
	if  ( ! p[0] )
	    { break;	}

	jpc= malloc( sizeof(JsonPathComponent) );
	if  ( ! jpc )
	    { XDEB(jpc); return -1;	}
	jsonPathInitComponent( jpc );

	res= jsonParseComponent( jpc, p, &p );
	if  ( res )
	    {
	    jsonPathCleanComponent( jpc );
	    free( jpc );

	    if  ( res < 0 )
		{ SDEB(p); return -1;	}
	    if  ( res > 0 )
		{ break;		}
	    }
	else{
	    if  ( prev )
		{ prev->jpcNext= jpc;	}
	    else{ jp->jpPath= jpc;	}

	    prev= jpc;
	    }
	}

    *pUpto= p;
    return 0;
    }

JsonPath * jsonParsePathImpl(	const char *	query,
				const char *	name )
    {
    JsonPath *		rval= (JsonPath *)0;
    JsonPath *		jp= (JsonPath *)0;
    int			l;
    const char *	p;

    if  ( ! query )
	{ XDEB(query); goto ready;	}

    p= query;
    while( p[0] == ' ' )
	{ p++;	}

    l= strlen( name );
    if  ( strncmp( p, name, l ) )
	{ SSSDEB(query,name,p); goto ready;	}
    p += l;

    jp= malloc( sizeof(JsonPath) );
    if  ( ! jp )
	{ XDEB(jp); goto ready;	}

    jsonInitPath( jp );

    if  ( jsonPathParseComponents( jp, p, &p ) )
	{ SDEB(p); goto ready;	}

    while( p[0] == ' ' )
	{ p++;	}
    if  ( p[0] )
	{ SDEB(p); goto ready;	}

    rval= jp; jp= (JsonPath *)0; /* steal */

  ready:

    if  ( jp )
	{ jsonFreePath( jp );	}

    return rval;
    }

JsonPath * jsonParsePath(	const char *	query )
    { return jsonParsePathImpl( query, "$" );	}

#   endif	/*	USE_LIBJANSSON		}}	*/
