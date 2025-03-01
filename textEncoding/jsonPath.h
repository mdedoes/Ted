#   include	<jansson.h>

struct SimpleInputStream;
struct JsonPath;

extern json_t * jsonLoad(	struct SimpleInputStream *	sis );

extern json_t * jsonPathFindByExpr(	const char *		query,
					json_t *		root );

extern json_t * jsonPathFindNamedByExpr( const char *		query,
					const char *		name,
					json_t *		root );

extern json_t * jsonPathFindByPattern(	const struct JsonPath *	jp,
					json_t *		root );

extern struct JsonPath * jsonParsePath(	const char *		query );

extern void jsonFreePath(	struct JsonPath *		jp );

extern char * jsonPathParseName( const char *			from,
				const char **			pUpto );
