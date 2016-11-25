/************************************************************************/
/*									*/
/*  Implements a mapping between font names as implemented by the	*/
/*  GhostScript 'Fontmap' file.						*/
/*									*/
/************************************************************************/

#   include	<appUtilConfig.h>

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<utilFontmap.h>

#   include	<appDebugon.h>

typedef struct FontmapEntry
    {
    char *		feFrom;
    char *		feTo;
    int			feIsAlias;
    } FontmapEntry;

static FontmapEntry *	UTIL_FontmapEntries;
int			UTIL_FontmapEntryCount;

static FontmapEntry *	UTIL_FontmapT1Entries;
int			UTIL_FontmapT1EntryCount;

/************************************************************************/
/*									*/
/*  Get a name resulting from one of the mappings.			*/
/*									*/
/************************************************************************/

static const char *	utilFontmapGet(	const char *		from,
					const FontmapEntry *	entries,
					int			entryCount )
    {
    int			turn;

    for ( turn= 0; turn < 20; turn++ )
	{
	const FontmapEntry *	fe;
	int			i;

	fe= entries;
	for ( i= 0; i < entryCount; fe++, i++ )
	    {
	    if  ( ! strcmp( fe->feFrom, from ) )
		{ break;	}
	    }

	if  ( i >= entryCount )
	    { return (const char *)0;	}

	if  ( fe->feIsAlias )
	    { from= fe->feTo; continue;	}

	return fe->feTo;
	}

    LSDEB(turn,from);
    return (const char *)0;
    }

const char * utilFontmapGetEntry(	const char *	from )
    {
    return utilFontmapGet( from,
			UTIL_FontmapEntries, UTIL_FontmapEntryCount );
    }
const char * utilFontmapGetT1Entry(	const char *	from )
    {
    return utilFontmapGet( from,
			UTIL_FontmapT1Entries, UTIL_FontmapT1EntryCount );
    }

/************************************************************************/
/*									*/
/*  Read the GhostScript font map.					*/
/*									*/
/*  1)  Syntax error in the Cyrillic fonts section.			*/
/*	As usual, people do not follow their own file format.		*/
/*									*/
/************************************************************************/

static int utilFontmapNextItem(	FILE *	f )
    {
    int		c;

    c= getc( f );
    for (;;)
	{
	if  ( c == '%' )
	    {
	    c= getc( f );
	    while( c != EOF && c != '\n' )
		{ c= getc( f );	}

	    if  ( c == '\n' )
		{ c= getc( f );	}

	    continue;
	    }

	if  ( isspace( c ) )
	    { c= getc( f ); continue;	}

	break;
	}

    return c;
    }

int utilFontmapReadMap(	const char *	filename )
    {
    int		rval= 0;
    FILE *	f= (FILE *)0;
    int		c;

    if  ( UTIL_FontmapEntryCount > 0 )
	{ /* LDEB(UTIL_FontmapEntryCount); rval= -1; */ goto ready;	}

    f= fopen( filename, "r" );
    if  ( ! f )
	{ SXDEB(filename,f); rval= -1; goto ready;	}

    c= utilFontmapNextItem( f );
    while( c != EOF )
	{
	FontmapEntry *	fe;

	char		scratch[200+ 1];
	int		l;

	/*  1  */
	if  ( c == ';' )
	    { c= utilFontmapNextItem( f ); continue;	}

	if  ( c != '/' )
	    { CDEB(c); rval= -1; goto ready;	}

	fe= realloc( UTIL_FontmapEntries,
			( UTIL_FontmapEntryCount+ 1)* sizeof(FontmapEntry) );
	if  ( ! fe )
	    { LXDEB(UTIL_FontmapEntryCount,fe); rval= -1; goto ready; }
	UTIL_FontmapEntries= fe;
	fe += UTIL_FontmapEntryCount;

	for ( l= 0; l < 200; l++ )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    if  ( isspace( c ) )
		{ break;	}

	    scratch[l]= c;
	    }
	if  ( l >= 200 )
	    { LDEB(l); rval= -1; goto ready;	}
	scratch[l]= '\0';

	fe->feFrom= strdup( scratch );
	if  ( ! fe->feFrom )
	    { XDEB(fe->feFrom); rval= -1; goto ready;	}

	while( isspace( c ) )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    }

	switch( c )
	    {
	    case '/':
		fe->feIsAlias= 1;
		break;
	    case '(':
		fe->feIsAlias= 0;
		break;
	    default:
		CDEB(c); rval= -1; goto ready;
	    }

	for ( l= 0; l < 200; l++ )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    if  ( isspace( c ) )
		{ break;	}

	    scratch[l]= c;
	    }
	if  ( l >= 200 )
	    { LDEB(l); rval= -1; goto ready;	}
	scratch[l]= '\0';

	if  ( ! fe->feIsAlias )
	    {
	    if  ( l < 1 || scratch[l-1] != ')' )
		{ SDEB(scratch); rval= -1; goto ready;	}

	    scratch[l-1]= '\0';
	    }

	fe->feTo= strdup( scratch );
	if  ( ! fe->feTo )
	    { XDEB(fe->feTo); rval= -1; goto ready;	}

	while( isspace( c ) )
	    {
	    c= getc( f );
	    if  ( c == EOF )
		{ LDEB(c); rval= -1; goto ready;	}
	    }

	if  ( c != ';' )
	    { CDEB(c); rval= -1; goto ready;	}

	UTIL_FontmapEntryCount++;
	c= utilFontmapNextItem( f );
	}

  ready:
    if  ( f )
	{ fclose( f );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read the T1 font map.						*/
/*									*/
/************************************************************************/

static char *	utilFontmapGetInputLine(	char *		s,
						int *		pLen,
						int		size,
						FILE *		stream )
    {
    s= fgets( s, size, stream );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0 )
	    {
	    if  ( s[lineLength-1] == '\n' )
		{
		s[--lineLength]= '\0';

		if  ( lineLength > 0		&&
		      s[lineLength-1] == '\r'	)
		    { s[--lineLength]= '\0'; }
		}

	    while( lineLength > 0 && s[lineLength-1] == ' ' )
		{ s[--lineLength]= '\0'; }
	    }

	*pLen= lineLength;
	}

    return s;
    }

int utilFontmapReadT1Map(	const char *	filename )
    {
    int		rval= 0;
    FILE *	f= (FILE *)0;

    char	scratch[200+1];
    int		l;
    int		line= 0;

    if  ( UTIL_FontmapT1EntryCount > 0 )
	{ /* LDEB(UTIL_FontmapT1EntryCount); rval= -1; */ goto ready;	}

    f= fopen( filename, "r" );
    if  ( ! f )
	{ SXDEB(filename,f); rval= -1; goto ready;	}

    while( utilFontmapGetInputLine( scratch, &l, 200, f ) )
	{
	FontmapEntry *	fe;
	char *		s= scratch;

	while( *s && ! isspace( *s ) )
	    { s++; }

	if  ( ! *s )
	    {
	    if  ( line != 0 )
		{ SDEB(scratch);	}

	    line++; continue;
	    }

	*(s++)= '\0';
	while( isspace( *s ) )
	    { s++;	}
	if  ( ! *s )
	    { SDEB(scratch); continue;	}

	fe= realloc( UTIL_FontmapT1Entries,
			( UTIL_FontmapT1EntryCount+ 1)* sizeof(FontmapEntry) );
	if  ( ! fe )
	    { LXDEB(UTIL_FontmapT1EntryCount,fe); rval= -1; goto ready; }
	UTIL_FontmapT1Entries= fe;

	fe += UTIL_FontmapT1EntryCount;

	fe->feFrom= strdup( scratch );
	if  ( ! fe->feFrom )
	    { XDEB(fe->feFrom); rval= -1; goto ready;	}

	fe->feTo= strdup( s );
	if  ( ! fe->feTo )
	    { XDEB(fe->feTo); rval= -1; goto ready;	}

	fe->feIsAlias= 0;

	UTIL_FontmapT1EntryCount++;
	line++; continue;
	}

  ready:
    if  ( f )
	{ fclose( f );	}

    return rval;
    }

