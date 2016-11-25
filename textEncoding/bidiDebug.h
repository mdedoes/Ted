# define CLS_DEB(r,o,c) bidiDebugClass( __FILE__, __LINE__, (r), (o), ucdBidiClassStr(c) )
# define LEV_DEB(r,o,l) bidiDebugLevel( __FILE__, __LINE__, (r), (o), (l) )
# define EMIT_DEB(e,l,f,u) bidiDebugEmit( __FILE__, __LINE__, (e), (l), f, u );

/************************************************************************/

extern void bidiDebugLevel(		const char *	file,
					int		line,
					const char *	rule,
					int		offset,
					int		level );

extern void bidiDebugClass(		const char *	file,
					int		line,
					const char *	rule,
					int		offset,
					const char *	klass );

extern void bidiDebugEmit(		const char *	file,
					int		line,
					int		embedding,
					int		level,
					int		from,
					int		upto );

