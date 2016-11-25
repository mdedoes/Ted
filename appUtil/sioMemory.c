/************************************************************************/
/*									*/
/*  Simple io streams to/from MemoryBuffers.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioMemory.h"
#   include	"sioGeneral.h"
#   include	"utilMemoryBuffer.h"

#   include	<appDebugon.h>

typedef struct SioMemoryPrivate
    {
			/**
			 *  The (array of) memory buffer(s) that we consume 
			 *  or produce. [We only produce one]
			 */
    MemoryBuffer *	smpBuffers;

			/**
			 *  The index in the array of buffers that 
			 *  we consume. [We only produce one]
			 */
    int			smpIndex;

			/**
			 *  The number of buffers in the array that 
			 *  we consume. [We only produce one]
			 */
    int			smpCount;

			/**
			 *  The position in the buffer that 
			 *  we currently produce or consume.
			 *  [We only produce one]
			 */
    int			smpPosition;
    } SioMemoryPrivate;

static int sioMemoryClose(	void *	voidsmp )
    { free( voidsmp ); return 0;	}

static int sioInMemoryReadBytes(	void *		voidsmp,
					unsigned char *	buffer,
					unsigned int	count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    int			done= 0;

    while( done < count && smp->smpIndex < smp->smpCount )
	{
	const MemoryBuffer *	mb= smp->smpBuffers+ smp->smpIndex;
	int			todo;

	/* Nothing to consume at this index */
	if  ( smp->smpPosition >= mb->mbSize )
	    {
	    smp->smpIndex++;
	    smp->smpPosition= 0;
	    continue;
	    }

	todo= count- done;
	if  ( smp->smpPosition+ todo > mb->mbSize )
	    { todo= mb->mbSize- smp->smpPosition;	}
	memcpy( buffer, mb->mbBytes+ smp->smpPosition, todo );

	smp->smpPosition += todo;
	done += todo;
	buffer += todo;
	}

    if  ( done < 1 )
	{ return -1;	}

    return done;
    }

static SimpleInputStream * sioInMemoryOpenArrayOrFrom(
						const MemoryBuffer *	mb,
						int			count,
						int			from )
    {
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)0;

    if  ( count < 1 || from < 0 || ( count > 1 && from > 0 ) )
	{ LLDEB(count,from); goto ready;	}

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); goto ready;	}

    smp->smpBuffers= (MemoryBuffer *)mb;
    smp->smpPosition= from;
    smp->smpIndex= 0;
    smp->smpCount= count;

    sis= sioInOpen( (void *)smp, sioInMemoryReadBytes, sioMemoryClose );

    if  ( ! sis )
	{ XDEB(sis); goto ready;	}

     smp= (SioMemoryPrivate *)0; /* steal */

  ready:

    return sis;
    }

SimpleInputStream * sioInMemoryOpenFrom(	const MemoryBuffer *	mb,
						int			from )
    {
    const int	count= 1;

    return sioInMemoryOpenArrayOrFrom( mb, count, from );
    }

SimpleInputStream * sioInMemoryOpen(	const MemoryBuffer *	mb )
    {
    const int	count= 1;
    const int	from= 0;

    return sioInMemoryOpenArrayOrFrom( mb, count, from );
    }

SimpleInputStream * sioInMemoryArrayOpen(
					const struct MemoryBuffer *	mb,
					int				count )
    {
    const int	from= 0;

    return sioInMemoryOpenArrayOrFrom( mb, count, from );
    }

static int sioOutMemoryWriteBytes(	void *			voidsmp,
					const unsigned char *	buffer,
					int			count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    MemoryBuffer *	mb= smp->smpBuffers;

    if  ( smp->smpPosition+ count > mb->mbSize )
	{
	unsigned char *	fresh;

	fresh= (unsigned char *)realloc( mb->mbBytes, smp->smpPosition+ count+ 1 );
	if  ( ! fresh )
	    { LXDEB(smp->smpPosition+ count,fresh); return -1;	}

	mb->mbBytes= fresh;
	}

    memcpy( mb->mbBytes+ smp->smpPosition, buffer, count );

    smp->smpPosition += count;
    if  ( mb->mbSize < smp->smpPosition )
	{
	mb->mbSize=  smp->smpPosition;
	mb->mbBytes[mb->mbSize]= '\0';
	}

    return count;
    }

SimpleOutputStream * sioOutMemoryOpenTo(	MemoryBuffer *	mb,
						int		to )
    {
    SimpleOutputStream *	sos;
    SioMemoryPrivate *		smp;

    if  ( mb->mbSize < to )
	{ LLDEB(mb->mbSize,to); return (SimpleOutputStream *)0;	}

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); return (SimpleOutputStream *)0;	}

    smp->smpBuffers= mb;
    smp->smpPosition= to;
    smp->smpIndex= 0;
    smp->smpCount= 1;

    sos= sioOutOpen( (void *)smp, sioOutMemoryWriteBytes, sioMemoryClose );
    if  ( ! sos )
	{ XDEB(sos); free( smp ); return (SimpleOutputStream *)0; }

    mb->mbSize= to;

    return sos;
    }

SimpleOutputStream * sioOutMemoryOpen(	MemoryBuffer *	mb )
    { return sioOutMemoryOpenTo( mb, 0 );	}
