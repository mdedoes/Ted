/* Regular expressions via PCRE(2) */

#   include	"textEncodingConfig.h"

#   include	<string.h>

#   define	PCRE2_CODE_UNIT_WIDTH	8
#   include	<pcre2.h>

#   include	"uniUtf8.h"
#   include	"textRegexp.h"

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
    pcre2_code *	rval;
    PCRE2_SIZE		erroffset= 0;
    int			errorcode= -1;
    char *		escaped= (char *)0;

    int			pcre_opts= PCRE2_UTF;

    {
    static int checkedOptions= 0;

    if  ( ! checkedOptions )
	{
	int res;
	int has;

	has= 0;
	res= pcre2_config( PCRE2_CONFIG_UNICODE, &has );
	if  ( res || ! has )
	    { appDebug( "PCRE2_CONFIG_UNICODE not set!\n" );	}

	/* Not needed for pcre2?
	has= 0;
	res= pcre_config( PCRE_CONFIG_UNICODE_PROPERTIES, &has );
	if  ( res || ! has )
	    { appDebug( "PCRE_CONFIG_UNICODE_PROPERTIES not set!\n" );	}
	*/

	checkedOptions= 1;
	}
    }

    if  ( ( options & REGflagAS_WORD ) != 0		&&
	  ( options & REGflagNO_REGEX ) == 0	)
	{
	XXXDEB(options,REGflagNO_REGEX,REGflagAS_WORD);
	return (regProg *)0;
	}

    if  ( options & REGflagNO_REGEX )
	{
	escaped= regEscape( pattern, (options&REGflagAS_WORD) != 0 );
	if  ( ! escaped )
	    { SXDEB(pattern,escaped); return (regProg *)0;	}
	pattern= escaped;
	}

    if  ( options & REGflagCASE_INSENSITIVE )
	{ pcre_opts |= PCRE2_CASELESS;	}

    rval= pcre2_compile( (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
			pcre_opts,
			&errorcode, &erroffset, (pcre2_compile_context *)0 );

    if  ( ! rval )
	{
	PCRE2_UCHAR message[256];
	pcre2_get_error_message(errorcode, message, sizeof(message));
	XLSSDEB(rval,errorcode,(char *)message,pattern+erroffset);
	}

    if  ( escaped )
	{ free( escaped );	}

    return (void *)rval;
    }

static int regFindCollectMatches(	ExpressionMatch *	em,
					const char *		string,
					pcre2_match_data *	match_data )
    {
    uint32_t		count= pcre2_get_ovector_count( match_data );
    PCRE2_SIZE *	matches= pcre2_get_ovector_pointer( match_data );
    int			i;

    if  ( count >= 1+ REG_MAX_MATCH )
	{ LLDEB(count,1+REG_MAX_MATCH); return -1;	}

    for ( i= 0; i < 2* count; i++ )
	{ em->emMatches[i]= matches[i];	}

    return 0;
    }

int regFindLeftToRight(	ExpressionMatch *	em,
			const regProg *		prog,
			const char *		string,
			int			fromByte,
			int			byteLength )
    {
    int			res= -1;
    int			opts= 0;

    pcre2_match_data *	match_data= (pcre2_match_data *)0;

    match_data= pcre2_match_data_create_from_pattern(
		    (pcre2_code *)prog, (pcre2_general_context *)0 );
    if  ( ! match_data )
	{ XDEB(match_data); goto ready;	}

    res= pcre2_match( (pcre2_code *)prog,
		    (PCRE2_UCHAR *)string, byteLength, fromByte, opts,
		    match_data, (pcre2_match_context *)0 );

    if  ( res >= 0 )
	{
	if  ( regFindCollectMatches( em, string, match_data ) )
	    { LDEB(res); return -1;	}
	}

  ready:

    if  ( match_data )
	{ pcre2_match_data_free( match_data );	}

    return res >= 0;
    }

int regFindRightToLeft(	ExpressionMatch *	em,
			const regProg *		prog,
			const char *		string,
			int			fromByte,
			int			byteLength )
    {
    int			res= PCRE2_ERROR_NOMATCH;
    int			opts= PCRE2_ANCHORED;

    pcre2_match_data *	match_data= (pcre2_match_data *)0;

    match_data= pcre2_match_data_create_from_pattern(
		    (pcre2_code *)prog, (pcre2_general_context *)0 );
    if  ( ! match_data )
	{ XDEB(match_data); goto ready;	}

    fromByte--;
    while( fromByte >= 0 )
	{
	if  ( UNI_UTF8_INTERN( string[fromByte] ) )
	    { fromByte--; continue;	}

	res= pcre2_match( (pcre2_code *)prog,
		    (PCRE2_UCHAR *)string, byteLength, fromByte, opts,
		    match_data, (pcre2_match_context *)0 );

	if  ( res != PCRE2_ERROR_NOMATCH )
	    { break;	}

	fromByte--;
	}

    if  ( res >= 0 )
	{
	if  ( regFindCollectMatches( em, string, match_data ) )
	    { LDEB(res); return -1;	}
	}

  ready:

    if  ( match_data )
	{ pcre2_match_data_free( match_data );	}

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
    { pcre2_code_free( prog );	}

