#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	<appDebugon.h>

/*
 * Collect the offspring children of a node.
 */
static int jsonCollectOffspring(
			json_t *				target,
			const struct JsonPathComponent *	jpc,
			json_t *				curr )
    {
    if  ( json_is_object( curr ) )
	{
	void *		iter= json_object_iter( curr );

	while( iter )
	    {
	    /*const char *	key= json_object_iter_key( iter );*/
	    json_t *		into= json_object_iter_value( iter );

	    if  ( jpc->jpcNext 					&&
		  jsonPathCollectOneChild( target, into, jpc, jpc ) )
		{ LDEB(1); return -1;	}
	    if  ( jsonPathCollectOneChild( target, into, jpc, jpc->jpcNext ) )
		{ LDEB(1); return -1;	}

	    iter= json_object_iter_next( curr, iter );
	    }
	}

    if  ( json_is_array( curr ) )
	{
	int		count= json_array_size( curr );
	int		i;

	for ( i= 0; i < count; i++ )
	    {
	    json_t *	into= json_array_get( curr, i );

	    if  ( jpc->jpcNext 					&&
		  jsonPathCollectOneChild( target, into, jpc, jpc ) )
		{ LDEB(1); return -1;	}
	    if  ( jsonPathCollectOneChild( target, into, jpc, jpc->jpcNext ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int jsonParseDotDot(	JsonPathComponent *	jpc,
			const char *		from,
			const char **		pUpto )
    {
    const char *	upto= from;

    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonCollectOffspring;
    *pUpto= upto;

    return 0;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
