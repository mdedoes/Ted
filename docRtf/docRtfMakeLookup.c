/************************************************************************/
/*									*/
/*  Generate efficient lookup structures for RTF control word lookup.	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"docRtfControlWord.h"
#   include	"docRtfMakeLookup.h"
#   include	<utilJenkinsPerfectHash.h>

#   include	<appDebugon.h>

static const RtfControlWord *	XX_controlWords;
static int			XX_controlWordCount;

static int docRtfCompareIndexPositions(	const void *	vi1,
					const void *	vi2 )
    {
    int		i1= *(const int *)vi1;
    int		i2= *(const int *)vi2;

    if  ( i1 < 0 || i1 >= XX_controlWordCount )
	{ LLDEB(i1,XX_controlWordCount); return 0;	}
    if  ( i2 < 0 || i2 >= XX_controlWordCount )
	{ LLDEB(i2,XX_controlWordCount); return 0;	}

    return docRtfCompareControlWords( &(XX_controlWords[i1]),
						&(XX_controlWords[i2]) );
    }

int docRtfMakeLookup(	const RtfControlWord *	controlWords,
			int			controlWordCount,
			const char *		cOutput,
			const char *		cInclude,
			const char *		arrayName,
			const char *		countName )
    {
    int			rval= 0;

    int *		lookup= (int *)0;
    FILE *		f= (FILE *)0;
    int			i;
    int			n;

    const RtfControlWord *	rcw;

    lookup= malloc( controlWordCount* sizeof( int ) );
    if  ( ! lookup )
	{ LXDEB(controlWordCount,lookup); rval= -1; goto ready;	}

    n= 0;
    rcw= controlWords;

    for ( i= 0; i < controlWordCount; rcw++, i++ )
	{
	if  ( rcw->rcwScope == RTCscopeANY )
	    { continue;	}

	lookup[n++]= i;
	}

    XX_controlWords= controlWords;
    XX_controlWordCount= controlWordCount;
    qsort( lookup, n, sizeof(int), docRtfCompareIndexPositions );

    f= fopen( cOutput, "w" );

    fprintf( f, "# include %s\n", cInclude );
    fprintf( f, "\n" );
    fprintf( f, "const int %s[] =\n", arrayName );
    fprintf( f, "{\n" );

    for ( i= 0; i < n; i++ )
	{
	rcw= controlWords+ lookup[i];

	fprintf( f, "%8d,\t/* %3d: %3d %3d %3d, \"%s\" */\n",
			lookup[i], i,
			rcw->rcwScope, rcw->rcwID, rcw->rcwEnumValue,
			rcw->rcwWord );
	}

    fprintf( f, "};\n" );

    fprintf( f, "\n" );
    fprintf( f, "const int %s=\\\n", countName );
    fprintf( f, "\t\tsizeof(%s)/sizeof(int);\n", arrayName );

  ready:

    if  ( f )
	{ fclose( f );	}
    if  ( lookup )
	{ free( lookup );	}

    return rval;
    }

int docRtfMakeHash(	const RtfControlWord *	controlWords,
			int			controlWordCount,
			const char *		cOutput,
			const char *		cInclude,
			const char *		procName,
			const char *		indexName )
    {
    int				rval= 0;
    int				i;

    const unsigned char **	keyStrings= (const unsigned char **)0;

    keyStrings= (const unsigned char **)malloc( controlWordCount* sizeof(const unsigned char *) );
    if  ( ! keyStrings )
	{ LXDEB(controlWordCount,keyStrings); rval= -1; goto ready;	}

    for ( i= 0; i < controlWordCount; i++ )
	{
	keyStrings[i]= (const unsigned char *)controlWords[i].rcwWord;
	}

    if  ( utilJenkinsPerfectHash( keyStrings, controlWordCount,
				    cOutput, cInclude, procName, indexName ) )
	{ LDEB(controlWordCount); rval= -1;	}

  ready:

    if  ( keyStrings )
	{ free( keyStrings );	}

    return rval;
    }

