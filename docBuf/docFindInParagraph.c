/************************************************************************/
/*									*/
/*  Find: Look for something inside paragraphs.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	<textRegexp.h>
#   include	"docTreeNode.h"
#   include	"docFind.h"
#   include	"docSelect.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Replace a find pattern.						*/
/*									*/
/************************************************************************/

int docFindSetPattern(		void **			pProg,
				const char *		pattern,
				int			useRegex,
				int			asWord,
				int			caseSensitive )
    {
    regProg *			prog= (regProg *)0;
    int				options= 0;

    if  ( ! useRegex )
	{ options |= REGflagNO_REGEX;		}
    if  ( asWord )
	{ options |= REGflagAS_WORD;		}
    if  ( ! caseSensitive )
	{ options |= REGflagCASE_INSENSITIVE;	}

    if  ( pattern )
	{
	prog= regCompile( pattern, options );
	if  ( ! prog )
	    { SXDEB((char *)pattern,prog); return -1;	}
	}

    if  ( *pProg )
	{ regFree( *pProg );	}
    *pProg= (void *)prog;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the first occurrence of a regular expression in a paragraph.	*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docFindParaFindNext(	struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= 1;
    ExpressionMatch	em;

    int			from;
    int			past;

    if  (  docParaStrlen( paraNode ) == 0 )
	{ return 1;	}

    res= regFindLeftToRight( &em, prog, docParaString( paraNode, 0 ),
		    dpFrom->dpStroff, docParaStrlen( paraNode ) );

    if  ( ! res )
	{ return 1;	}

    regGetFullMatch( &from, &past, &em );
    docSetParaSelection( ds, paraNode, direction, from, past- from );

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docFindParaFindPrev(	struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= -1;
    ExpressionMatch	em;

    int			from;
    int			past;

    if  (  docParaStrlen( paraNode ) == 0 )
	{ return 1;	}

    res= regFindRightToLeft( &em, prog, docParaString( paraNode, 0 ),
		    dpFrom->dpStroff, docParaStrlen( paraNode ) );

    if  ( ! res )
	{ return 1;	}

    regGetFullMatch( &from, &past, &em );
    docSetParaSelection( ds, paraNode, direction, from, past- from );

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

