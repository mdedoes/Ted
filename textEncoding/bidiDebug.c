#   include	"bidiDebug.h"
#   include	"ucdBidiClass.h"

#   include	<appDebugon.h>

void bidiDebugLevel(	const char *	file,
			int		line,
			const char *	rule,
			int		offset,
			int		level )
    {
    appDebug( "%s(%3d) %s: SetLevel[%d]=%d\n",
				file, line, rule, offset, level );
    return;
    }

void bidiDebugClass(	const char *	file,
			int		line,
			const char *	rule,
			int		offset,
			const char *	klass )
    {
    appDebug( "%s(%3d) %s: SetClass[%d]=%s\n",
				file, line, rule, offset, klass );
    return;
    }

void bidiDebugEmit(	const char *	file,
			int		line,
			int		embedding,
			int		level,
			int		from,
			int		upto )
    {
    appDebug( "%s(%3d) %s: EmitLevel[%d..%d]=%d (%d)\n",
			    file, line, ucdBidiClassStr( embedding ),
			    from, upto, level, upto- from );
    return;
    }

