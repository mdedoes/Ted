/* Regular expressions via PCRE */

#   include	"textEncodingConfig.h"

#   include	<string.h>

#   include	"uniUtf8.h"
#   include	"textRegexp.h"
#   include	<pcre.h>

#   include	<appDebugon.h>

static char * regEscape(	const char *	pattern,
				int		asWord )
    {
    int		l= 2* strlen( pattern )+ 4* ( asWord != 0 );
    char *	escaped= (char *)malloc( 2* l+ 1 );

    if  ( escaped )
	{
	const char *	from= pattern;
	char *		to= escaped;

	const char *	b= "\\b";

	if  ( asWord )
	    { strcpy( to, b ); to += 2;	}

	while( *from )
	    {
	    if  ( *from == '\\'	||
		  *from == '^'	||
		  *from == '$'	||
		  *from == '.'	||
		  *from == '['	||
		  *from == '|'	||
		  *from == '('	||
		  *from == ')'	||
		  *from == '?'	||
		  *from == '*'	||
		  *from == '+'	||
		  *from == '{'	)
		{ *(to++)= '\\';	}

	    *(to++)= *(from++);
	    }

	if  ( asWord )
	    { strcpy( to, b ); to += 2;	}

	*to= '\0';
	}

    return escaped;
    }

regProg * regCompile(	const char *	pattern,
			int		options )
    {
    pcre *			rval;
    int				error= 0;
    int				erroffset= 0;
    const char *		errormsg= (const char *)0;
    char *			escaped= (char *)0;

    const unsigned char * const	tableptr= (const unsigned char *)0;
    int				pcre_opts= PCRE_UTF8;

    {
    static int checkedOptions= 0;

    if  ( ! checkedOptions )
	{
	int res;
	int has;

	has= 0;
	res= pcre_config( PCRE_CONFIG_UTF8, &has );
	if  ( res || ! has )
	    { appDebug( "PCRE_CONFIG_UTF8 not set!\n" );	}

	has= 0;
	res= pcre_config( PCRE_CONFIG_UNICODE_PROPERTIES, &has );
	if  ( res || ! has )
	    { appDebug( "PCRE_CONFIG_UNICODE_PROPERTIES not set!\n" );	}

	checkedOptions= 1;
	}
    }

    if  ( ( options & REGflagAS_WORD ) != 0		&&
	  ( options & REGflagESCAPE_REGEX ) == 0	)
	{
	XXXDEB(options,REGflagESCAPE_REGEX,REGflagAS_WORD);
	return (regProg *)0;
	}

    if  ( options & REGflagESCAPE_REGEX )
	{
	escaped= regEscape( pattern, (options&REGflagAS_WORD) != 0 );
	if  ( ! escaped )
	    { SXDEB(pattern,escaped); return (regProg *)0;	}
	pattern= escaped;
	}

    if  ( options & REGflagCASE_INSENSITIVE )
	{ pcre_opts |= PCRE_CASELESS;	}

    rval= pcre_compile2( pattern, pcre_opts,
				    &error, &errormsg, &erroffset,
				    tableptr );

    if  ( ! rval )
	{ XSSDEB(rval,errormsg,pattern+erroffset);	}

    if  ( escaped )
	{ free( escaped );	}

    return (void *)rval;
    }

int regFindLeftToRight(	ExpressionMatch *	em,
			const regProg *		prog,
			const char *		string,
			int			fromByte,
			int			byteLength )
    {
    int		res;

    int		opts= 0;

    res= pcre_exec( (pcre *)prog, (pcre_extra *)0,
		    string, byteLength, fromByte, opts,
		    em->emMatches, 2+(2*REG_MAX_MATCH)/*!*/+1+REG_MAX_MATCH );

#   if 0
    if  ( res >= 0 )
	{
	appDebug( "# %d..%d: \"%.*s\"\n",
				    em->emMatches[0], em->emMatches[1],
				    em->emMatches[1]- em->emMatches[0],
				    string+ em->emMatches[0] );
	}
#   endif

    return res >= 0;
    }

int regFindRightToLeft(	ExpressionMatch *	em,
			const regProg *		prog,
			const char *		string,
			int			fromByte,
			int			byteLength )
    {
    int		res= PCRE_ERROR_NOMATCH;

    int		opts= PCRE_ANCHORED;

    fromByte--;
    while( fromByte >= 0 )
	{
	if  ( UNI_UTF8_INTERN( string[fromByte] ) )
	    { fromByte--; continue;	}

	res= pcre_exec( (pcre *)prog, (pcre_extra *)0,
		    string, byteLength, fromByte, opts,
		    em->emMatches, 2+(2*REG_MAX_MATCH)/*!*/+1+REG_MAX_MATCH );

	if  ( res != PCRE_ERROR_NOMATCH )
	    { break;	}

	fromByte--;
	}

    return res >= 0;
    }

int regGetMatch(	int *			pFrom,
			int *			pPast,
			const ExpressionMatch *	em,
			int			n )
    {
    if  ( n >= REG_MAX_MATCH )
	{ LLDEB(n,REG_MAX_MATCH); return -1;	}

    if  ( em->emMatches[0] >= 0 && em->emMatches[2+n+0] >= 0 )
	{
	*pFrom= em->emMatches[2+n+0];
	*pPast= em->emMatches[2+n+1];
	return 0;
	}

    return -1;
    }

int regGetFullMatch(	int *			pFrom,
			int *			pPast,
			const ExpressionMatch *	em )
    {
    if  ( em->emMatches[0] >= 0 )
	{
	*pFrom= em->emMatches[0];
	*pPast= em->emMatches[1];
	return 0;
	}

    return -1;
    }

void regFree(	regProg *	prog )
    { pcre_free( prog );	}

