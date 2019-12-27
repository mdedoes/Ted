#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<jansson.h>

#   include	"jsonPath.h"
#   include	"sioGeneral.h"

#   include	<appDebugon.h>

static size_t janssonRead(	void *			buffer,
				size_t			count,
				void *			vsis )
    {
    return sioInReadBytes( (SimpleInputStream *)vsis, buffer, count );
    }

json_t * jsonLoad(	SimpleInputStream *	sis )
    {
    json_error_t	error;
    size_t		flags= JSON_REJECT_DUPLICATES;

    json_t * rval= json_load_callback(
			    janssonRead, (void *)sis, flags, &error );

    if  ( ! rval )
	{
	SDEB(error.text);
	SDEB(error.source);
	LLLDEB(error.line,error.column,error.position);
	}

    return rval;
    }

#   endif	/*	USE_LIBJANSSON		}}	*/
