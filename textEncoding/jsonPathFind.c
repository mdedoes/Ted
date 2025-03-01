#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	"jsonPath.h"

#   include	<appDebugon.h>

json_t * jsonPathFindByPattern(	const JsonPath *	jp,
				json_t *		root )
    {
    json_t *			target= (json_t *)0;

    /* Or simply return an array holding the root? */
    if  ( ! jp->jpPath )
	{ XDEB(jp->jpPath); goto ready;	}

    if  ( ! jp->jpPath->jpcCollectMatches )
	{ XDEB(jp->jpPath->jpcCollectMatches); goto ready;	}

    target= json_array();
    if  ( ! target )
	{ XDEB(target); goto ready;	}

    if  ( (*jp->jpPath->jpcCollectMatches)( target, jp->jpPath, root ) )
	{
	LDEB(1);
	json_decref( target );
	target= (json_t *)0;
	goto ready;
	}

  ready:

    return target;
    }

static json_t * jsonPathFindByExprImpl(	const char *	query,
					const char *	name,
					json_t *	root )
    {
    JsonPath *		jp= (JsonPath *)0;
    json_t *		here= (json_t *)0;

    if  ( ! query || ! root )
	{ XXDEB(query,root); goto ready;	}
    if  ( ! name )
	{ XXXDEB(query,root,name); goto ready;	}

    jp= jsonParsePathImpl( query, name );
    if  ( ! jp )
	{ SXDEB(query,jp); goto ready;	}

    here= jsonPathFindByPattern( jp, root );
    if  ( ! here )
	{ XDEB(here); goto ready;	}

  ready:

    if  ( jp )
	{ jsonFreePath( jp );	}

    return here;
    }

/**
 * Resolve a JSON Path see: http://goessner.net/articles/JsonPath/
 *
 * Return a (potentially empty) json array with all matches on success.
 *	This is a new reference that the caller must dereference.
 *
 * Return (json_t *)0 on failure;
 */
json_t * jsonPathFindByExpr(	const char *	query,
				json_t *	root )
    { return jsonPathFindByExprImpl( query, "$", root );	}

json_t * jsonPathFindNamedByExpr(	const char *	query,
					const char *	name,
					json_t *	root )
    { return jsonPathFindByExprImpl( query, name, root );	}

#   endif	/*	USE_LIBJANSSON		}}	*/
