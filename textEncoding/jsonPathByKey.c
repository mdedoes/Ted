#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>
#   include	<string.h>

#   include	"jsonPathImpl.h"
#   include	"jsonPath.h"

#   include	<appDebugon.h>

/**
 * Step over a constant key
 */
static int jsonCollectThroughKey( json_t *			target,
				const JsonPathComponent *	jpc,
				json_t *			curr )
    {
    if  ( json_is_object( curr ) )
	{
	json_t *	into= json_object_get( curr, jpc->jpcKey );

	if  ( jsonPathCollectOneChild( target, into, jpc, jpc->jpcNext ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    /* Exception for final .length */
    if  ( ! jpc->jpcNext			&&
	  ! strcmp( jpc->jpcKey, "length" )	)
	{
	/* Exception for array.length */
	if  ( json_is_array( curr ) )
	    {
	    json_t *	length= json_integer( json_array_size( curr ) );

	    if  ( ! length )
		{ XDEB(length); return -1;	}

	    json_array_append( target, length );
	    json_decref( length );

	    return 0;
	    }

	/* Exception for string.length */
	if  ( json_is_string( curr ) )
	    {
	    /* TODO: Check that this is in characters (not in bytes) */
	    json_t *	length= json_integer( json_string_length( curr ) );

	    if  ( ! length )
		{ XDEB(length); return -1;	}

	    json_array_append( target, length );
	    json_decref( length );

	    return 0;
	    }
	}

    return 0;
    }

int jsonParseDotName(	JsonPathComponent *	jpc,
			const char *		from,
			const char **		pUpto )
    {
    const char *	upto= from;

    jpc->jpcKey= jsonPathParseName( from, &upto );
    if  ( ! jpc->jpcKey )
	{ XDEB(jpc->jpcKey); return -1;	}

    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonCollectThroughKey;
    *pUpto= upto;

    return 0;
    }

int jsonPathParseByQuotedKey(	JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    const char *	upto= from;

    jpc->jpcKey= jsonPathParseQuotedString( from, &upto );
    if  ( ! jpc->jpcKey )
	{ SXDEB(from,jpc->jpcKey); return -1;	}

    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonCollectThroughKey;
    *pUpto= upto;

    return 0;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
