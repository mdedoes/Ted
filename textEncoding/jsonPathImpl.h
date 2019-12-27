#   if		USE_LIBJANSSON		/*  {{	*/

#   include	<jansson.h>

#   if USE_LIBPCRE2
#	define	PCRE2_CODE_UNIT_WIDTH	8
#	include	<pcre2.h>
#   endif

/**
 * A (parsed) Json Path
 */
typedef struct JsonPath
    {
				/**
				 *  A linked list of components.
				 *  (Beware of the alternatives with union!)
				 */
    struct JsonPathComponent *	jpPath;
    } JsonPath;

typedef struct JsonFilterTerm
    {
		/**
		 *  Expression (For JFT_EXPRESSION)
		 */
    JsonPath *	jftExpression;

		/**
		 *  Constant (For JFT_CONSTANT)
		 */
    json_t *	jftConstant;

#   if USE_LIBPCRE2
		/**
		 * The regular expression (For JFT_PATTERN)
		 */
    pcre2_code * jftPattern;
#   endif

    enum {
	    /**
	     *  Term is a json path expression that hopefully evaluates 
	     *  to a single value.
	     */
	JFT_EXPRESSION,

	    /**
	     *  Term is a constant
	     */
	JFT_CONSTANT,

#	if USE_LIBPCRE2
	    /**
	     *  Term is a pattern (2nd term with FO_MATCHES)
	     */
	JFT_PATTERN,
#	 endif
	} jftType;
    } JsonFilterTerm;

/*
 * See
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Equality_comparisons_and_sameness
 * As this is a data oriented query language, our idea of equality is 
 * data oriented. It is closer to JavaScript's == than ===.
 * However, values of a different type are never equal. (As in jansson)
 * [ Janssons INTEGER and FLOAT are NUMBER in JavaScript: The same type! ]
 */
typedef struct FilterOperator
    {
    const char *	foSymbol;
    int			foLength;
    enum {
	FO_EQ,
	FO_NE,
	FO_GE,
	FO_LE,
	FO_GT,
	FO_LT,
	FO_EXISTS,
	FO_MATCHES,
	} foId;
    } FilterOperator;

/**
 *  A component on a path
 */
typedef struct JsonPathComponent
    {
		/**
		 *  The path components are stured as a linked list.
		 *  (Beware of the alternatives however)
		 */
    struct JsonPathComponent *	jpcNext;

		/**
		 *  If the step is through subscripts.. Use these values.
		 */
    long	jpcSubscript0;
    long	jpcSubscript1;

		/**
		 *  If the step is through a key.. Use this value.
		 */
    char *	jpcKey;

		/**
		 *  Filter terms;
		 */
    JsonFilterTerm jpcFilterTerm0;
    JsonFilterTerm jpcFilterTerm1;

		/**
		 *  Filter operator (Where applicable)
		 */
    int		jpcFilterOperator;

		/**
		 * Filter children.
		 * Return 0 to reject the child.
		 * Return 1 to accept the child.
		 * Return -1 or other negative values on failure.
		 */
    int		(*jpcTestChild)( json_t *				into,
				const struct JsonPathComponent *	jpc );

		/**
		 * Collect offspring of matching children
		 */
    int 	(*jpcCollectMatches)(
				json_t *				target,
				const struct JsonPathComponent *	jpc,
				json_t *				curr );

		/**
		 * Alternatives in a [,,,,] context
		 */
    struct JsonPathComponent *	jpcAlternatives;
    int				jpcAlternativeCount;
    } JsonPathComponent;

/************************************************************************/
/*									*/
/*  Routine Declarations						*/
/*									*/
/************************************************************************/

extern void jsonInitPath(	struct JsonPath *		jp );

extern char * jsonPathParseQuotedString(
				const char *			from,
				const char **			pUpto );

extern json_t * jsonPathParseConstant(
				const char *			from,
				const char **			pUpto );

# if USE_LIBPCRE2

extern pcre2_code * jsonPathParsePattern(
				const char *			from,
				const char **			pUpto );

extern int jsonTestMatch(	json_t *			v0,
				pcre2_code *			pattern,
				const struct JsonPathComponent * jpc );

# endif

extern int jsonPathParseByFilter(
				struct JsonPathComponent *	jpc,
				const char *			from,
				const char **			pUpto );

extern int jsonPathCollectChildren(
				json_t *			target,
				const struct JsonPathComponent * jpc,
				json_t *			curr );

extern int jsonPathParseComponents(
				struct JsonPath *		jp,
				const char *			from,
				const char **			pUpto );

extern int jsonPathCollectOneChild(
				json_t *			target,
				json_t *			into,
				const struct JsonPathComponent * curr,
				const struct JsonPathComponent * next );

extern int jsonParseDotName(	struct JsonPathComponent *	jpc,
				const char *			from,
				const char **			pUpto );

extern int jsonPathParseByQuotedKey( struct JsonPathComponent *	jpc,
				const char *			from,
				const char **			pUpto );

extern int jsonPathParseBySubscript( struct JsonPathComponent *	jpc,
				const char *			from,
				const char **			pUpto );

extern int jsonParseDotDot(	struct JsonPathComponent *	jpc,
				const char *			from,
				const char **			pUpto );

extern void jsonPathInitComponent( struct JsonPathComponent *	jpc );
extern void jsonPathCleanComponent( struct JsonPathComponent *	jpc );

extern struct JsonPathComponent * jsonPathAddAlternative(
			       struct  JsonPathComponent *	jpc );

extern JsonPath * jsonParsePathImpl(	const char *	query,
					const char *	name );

#   endif		/*  USE_LIBJANSSON		}}	*/
