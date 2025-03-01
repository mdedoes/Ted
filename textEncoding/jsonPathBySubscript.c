#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	<appDebugon.h>

/*
 * Step over an array slice
 */
static int jsonCollectThroughSubscripts(
				json_t *			target,
				const JsonPathComponent *	jpc,
				json_t *			curr )
    {
    if  ( json_is_array( curr ) )
	{
	int		count= json_array_size( curr );
	int		i0;
	int		i1;
	int		i;

	if  ( jpc->jpcSubscript0 >= 0 )
	    { i0= jpc->jpcSubscript0;		}
	else{ i0= count+ jpc->jpcSubscript0;	}

	if  ( jpc->jpcSubscript1 >= 0 )
	    { i1= jpc->jpcSubscript1;		}
	else{ i1= count+ jpc->jpcSubscript1;	}

	if  ( i0 < 0 )
	    { i0= 0;	}

	for ( i= i0; i <= i1; i++ )
	    {
	    json_t *	into= json_array_get( curr, i );

	    if  ( jsonPathCollectOneChild( target, into, jpc, jpc->jpcNext ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int jsonPathParseBySubscript(	JsonPathComponent *	jpc,
				const char *		from,
				const char **		pUpto )
    {
    long		subscript0= 0;
    long		subscript1;
    const char *	upto= from;

    char *		p;

    subscript0= strtol( upto, &p, 0 );
    if  ( p == upto )
	{ SDEB(upto); return -1;	}
    upto= p;

    if  ( upto[0] == ':' )
	{
	upto++;

	subscript1= strtol( upto, &p, 0 );
	if  ( p == upto )
	    { SDEB(upto); return -1;	}
	upto= p;

	/* TODO? step */
	}
    else{
	subscript1= subscript0;
	}

    *pUpto= upto;

    jpc->jpcSubscript0= subscript0;
    jpc->jpcSubscript1= subscript1;
    /* No Filtering: jpc->jpcTestChild= 0;*/
    jpc->jpcCollectMatches= jsonCollectThroughSubscripts;

    return 0;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
