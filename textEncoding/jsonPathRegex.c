#   include	"textEncodingConfig.h"

#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<ctype.h>

#   include	"jsonPathImpl.h"

#   include	<appDebugon.h>

#if USE_LIBPCRE2
/**
 * Parse/skip a pattern.
 * See: https://www.pcre.org/current/doc/html/pcre2pattern.html
 */
pcre2_code * jsonPathParsePattern(	const char *		from,
					const char **		pUpto )
    {
    const char *	upto= ++from;
    int			l;

    pcre2_code *	pattern= (pcre2_code *)0;
    int			pcre_opts= PCRE2_UTF;
    PCRE2_SIZE		erroffset= 0;
    int			errorcode= -1;

    while( upto[0] != '/' )
	{
	if  ( ! upto[0] )
	    { SDEB(from); return (pcre2_code *)0;	}
	if  ( upto[0] == '\\' )
	    {
	    if  ( ! upto[1] )
		{ SDEB(from); return (pcre2_code *)0;	}
	    else{ upto += 2; continue;			}
	    }

	if  ( upto[0] == '[' )
	    {
	    upto++;

	    if  ( ! upto[0] )
		{ SSDEB(from,upto); return (pcre2_code *)0;	}
	    if  ( upto[0] == '^' )
		{ upto++;	}
	    if  ( upto[0] == ']' )
		{ upto++;	}

	    while( upto[0] != ']' )
		{
		if  ( ! upto[0] )
		    { SDEB(from); return (pcre2_code *)0;	}

		if  ( upto[0] == '\\' )
		    {
		    if  ( ! upto[1] )
			{ SDEB(from); return (pcre2_code *)0;	}
		    else{ upto += 2; continue;		}
		    }

		if  ( upto[0] == '[' && upto[1] == ':' )
		    {
		    upto += 2;
		    while( isalpha( upto[0] ) )
			{ upto++;	}
		    if  ( upto[0] != ':' || upto[1] != ']' )
			{ SSDEB(from,upto); return (pcre2_code *)0;	}

		    upto += 2; continue;
		    }

		upto++;
		}

	    upto++;
	    }

	upto++;
	}

    l= upto++ - from; /* / */

    if  ( upto[0] == 'i' )
	{ pcre_opts |= PCRE2_CASELESS; upto++;	}

    pattern= pcre2_compile( (PCRE2_SPTR)from, l, pcre_opts,
			&errorcode, &erroffset, (pcre2_compile_context *)0 );
    if  ( ! pattern )
	{
	PCRE2_UCHAR message[256];
	pcre2_get_error_message(errorcode, message, sizeof(message));
	XLSSDEB(pattern,errorcode,(char *)message,from+erroffset);
	return (pcre2_code *)0;
	}

    *pUpto= upto;
    return pattern;
    }

int jsonTestMatch(	json_t *			v0,
			pcre2_code *			pattern,
			const JsonPathComponent *	jpc )
    {
    int			rval= 0;
    int			res;
    const char *	s0;

    pcre2_match_data *	match_data= (pcre2_match_data *)0;
    const int		opts= 0;

    if  ( ! v0 )
	{ goto ready;	}
    if  ( ! json_is_string( v0 ) )
	{ goto ready;	}

    match_data= pcre2_match_data_create_from_pattern(
					pattern, (pcre2_general_context *)0 );
    if  ( ! match_data )
	{ XDEB(match_data); rval= -1; goto ready;	}

    s0= json_string_value( v0 );
    if  ( ! s0 )
	{ XDEB(s0); rval= -1; goto ready;	}

    res= pcre2_match( pattern,
		    (PCRE2_UCHAR *)s0, PCRE2_ZERO_TERMINATED, 0, opts,
		    match_data, (pcre2_match_context *)0 );

    if  ( res < 0 && res != PCRE2_ERROR_NOMATCH )
	{
	PCRE2_UCHAR message[256];
	pcre2_get_error_message(res, message, sizeof(message));
	LSDEB(res,(char *)message);
	rval= -1; goto ready;
	}

    rval= res > 0;

  ready:

    if  ( match_data )
	{ pcre2_match_data_free( match_data );	}

    return rval;
    }

#endif

#   endif	/*	USE_LIBJANSSON		}}	*/
