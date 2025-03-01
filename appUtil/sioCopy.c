/************************************************************************/
/*									*/
/*  Copy a stream							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"sioGeneral.h"
#   include	"sioUtil.h"
#   include	"sioFileio.h"
#   include	"sioMemory.h"
#   include	<appDebugon.h>

int sioCopyStream(	SimpleOutputStream *	sos,
			SimpleInputStream *	sis )
    {
    unsigned char	buf[2500];
    int			todo;

    while( ( todo= sioInReadBytes( sis, buf, sizeof(buf) ) ) > 0 )
	{
	int	done= sioOutWriteBytes( sos, buf, todo );

	if  ( done != todo )
	    { LLDEB(done,todo); return -1;	}
	}

    return 0;
    }

int sioCopyInputStreamToFile(
			const char *			fileName,
			SimpleInputStream *		sis )
    {
    int				rval= 0;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    sos= sioOutFileioOpenS( fileName );
    if  ( ! sos )
	{ SXDEB(fileName,sos); rval= -1; goto ready;	}

    if  ( sioCopyStream( sos, sis ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

int sioCopyMemoryBufferToFile(
			const char *			fileName,
			const struct MemoryBuffer *	mb )
    {
    int				rval= 0;

    SimpleInputStream *		sis= (SimpleInputStream *)0;

    sis= sioInMemoryOpen( mb );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    if  ( sioCopyInputStreamToFile( fileName, sis ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }
