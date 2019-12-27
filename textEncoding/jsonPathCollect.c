#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	<appDebugon.h>

int jsonPathCollectOneChild(	json_t *			target,
				json_t *			into,
				const JsonPathComponent *	curr,
				const JsonPathComponent *	next )
    {
    /* Cope with the improbable ,, situation in an array */
    if  ( into )
	{
	int			valid= 1;

	if  ( curr->jpcTestChild )
	    {
	    valid= (*curr->jpcTestChild)( into, curr );
	    if  ( valid < 0 )
		{ LDEB(valid); return -1;	}
	    }

	if  ( valid )
	    {
	    if  ( ! next )
		{ json_array_append( target, into );	}
	    else{
		if  ( ! next->jpcCollectMatches )
		    { XDEB(next->jpcCollectMatches); return -1;	}

		if  ( (*next->jpcCollectMatches)( target, next, into ) )
		    { LDEB(valid); return -1;	}
		}
	    }
	}

    return 0;
    }

/**
 * Recursively scan all childen of a node.
 *
 * Collect the node every time that we reach the end of the path.
 */
int jsonPathCollectChildren(	json_t *			target,
				const JsonPathComponent *	jpc,
				json_t *			curr )
    {
    if  ( json_is_object( curr ) )
	{
	void *		iter= json_object_iter( curr );

	while( iter )
	    {
	    /*const char *	key= json_object_iter_key( iter );*/
	    json_t *		into= json_object_iter_value( iter );

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

	    if  ( jsonPathCollectOneChild( target, into, jpc, jpc->jpcNext ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
