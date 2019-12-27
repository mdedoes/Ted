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

int textGetMsLocaleIdByTag(	const char *	tag )
    {
    char *	t= strdup( tag );
    char *	s= t;
    int		rval;

    if  ( ! t )
	{ XDEB(t); return -1;	}

    while( *s )
	{
	if  ( *s == '_' )
	    { *s= '-';		}
	s++;
	}

    rval= textGetMsLocaleIdByTagImpl( t );

    free( t );

    if  ( rval < 0 )
	{ SDEB(tag);	}

    return rval;
    }

int textMatchMsLocaleIdByTag(	const char *	tag )
    {
    int			best= 0;
    int			found= -1;

    int			i;
    const MsLcid *	ml;

    ml= TEXT_LcIds;
    for ( i= 0; i < TEXT_LcIdCount; i++, ml++ )
	{
	int	score= textMatchLocaleTags( ml->mlTag, tag );

	if  ( score > best )
	    { found= ml->mlId; best= score;	}
	}

    return found;
    }

