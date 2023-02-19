/************************************************************************/
/*									*/
/*  Microsoft language tags.						*/
/*									*/
/************************************************************************/

#   include	<string.h>
#   include	<stdlib.h>

#   include	"textMsLcid.h"
#   include	"textMsLocale.h"
#   include	"textLocaleTag.h"

#   include	<appDebugon.h>

const char *	textGetMsLocaleTagById(	int		id )
    {
    int			i;
    const MsLcid *	ml;

    ml= TEXT_LcIds;
    for ( i= 0; i < TEXT_LcIdCount; i++, ml++ )
	{
	if  ( ml->mlId == id )
	    { return ml->mlTag;	}
	}

    if  ( id == 0x400 || id == 0 || id == -1 ) /* 0, -1: Against too many warnings */
	{ return (const char *)0;	}

    XDEB(id); return (const char *)0;
    }

/**
 *  @return the Microsoft locale id that matches the locale tag.
 *	If the tag does not match any of the standard locales,
 *	return -1.
 */
static int textGetMsLocaleIdByTagImpl(	const char *	tag )
    {
    int			i;
    const MsLcid *	ml;

    ml= TEXT_LcIds;
    for ( i= 0; i < TEXT_LcIdCount; i++, ml++ )
	{
	if  ( ! strcmp( ml->mlTag, tag ) )
	    { return ml->mlId;	}
	}

    return -1;
    }

static char * textMsLocaleNormalizeTag(	const char *	tag )
    {
    char *	normalized= strdup( tag );
    char *	s= normalized;

    if  ( ! normalized )
	{ XDEB(normalized); return normalized;	}

    while( *s )
	{
	if  ( *s == '_' )
	    { *s= '-';			}
	if  ( *s == '.' )
	    { *s= '\0'; break;		}
	s++;
	}

    return normalized;
    }

/**
 *  @param tag. The POSIX locale tag to match. We use some fix-ups 
 *	to accomodate for encoding differences and the standard 
 *	POSIX convention to include the encoding after a dot. (.)
 *  @return the Microsoft locale id that matches the locale tag.
 *	If the tag does not match any of the standard locales,
 *	return -1.
 */
int textGetMsLocaleIdByTag(	const char *	tag )
    {
    char *	normalized= textMsLocaleNormalizeTag( tag );
    int		rval;

    if  ( ! normalized )
	{ XDEB(normalized); return -1;	}

    rval= textGetMsLocaleIdByTagImpl( normalized );

    free( normalized );

    if  ( rval < 0 )
	{ SDEB(tag);	}

    return rval;
    }

static int textMatchLocaleTag(		const char *	normalized )
    {
    int			best= 0;
    int			found= -1;

    int			i;
    const MsLcid *	ml;

    ml= TEXT_LcIds;
    for ( i= 0; i < TEXT_LcIdCount; i++, ml++ )
	{
	int	score= textMatchLocaleTags( ml->mlTag, normalized );

	if  ( score > best )
	    { found= ml->mlId; best= score;	}
	}

    return found;
    }

static int textMatchLocaleEnvVar(		const char *	name )
    {
    const char *	value= getenv( name );

    if  ( value )
	{
	int found= textMatchMsLocaleIdByTag( value );
	if  ( found < 0 )
	    { SSLDEB(name,value,found);	}
	else{ return found;		}
	}

    return -1;
    }

/**
 *  @param tag. The POSIX locale tag to match. We use some fix-ups 
 *	to accomodate for encoding differences and the standard 
 *	POSIX convention to include the encoding after a dot. (.)
 *	More specific locales get priority over the less specific ones.
 *  @return the Microsoft locale id that matches the locale tag.
 *	If the tag does not match any of the standard locales,
 *	return -1.
 */
int textMatchMsLocaleIdByTag(	const char *	tag )
    {
    char *		normalized= textMsLocaleNormalizeTag( tag );
    int			found;

    if  ( ! normalized )
	{ XDEB(normalized); return -1;	}

    found= textMatchLocaleTag( normalized );

    free( normalized );

    return found;
    }

int textMatchConfiguredLocaleTag(	const char *	configured )
    {
    int		found;

    if  ( configured )
	{
	found= textMatchMsLocaleIdByTag( configured );
	if  ( found < 0 )
	    { SLDEB(configured,found);	}
	else{ return found;		}
	}

    found= textMatchLocaleEnvVar( "LC_ALL" );
    if  ( found >= 0 )
	{ return found;	}

    found= textMatchLocaleEnvVar( "LANG" );
    if  ( found >= 0 )
	{ return found;	}

    return -1;
    }
