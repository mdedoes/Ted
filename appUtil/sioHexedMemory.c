/************************************************************************/
/*									*/
/*  Simple io streams to/from MemoryBuffers with hexed (binary) content.*/
/*  E.G: The 'Objects' in an RTF file.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioHexedMemory.h"

#   include	"sioHex.h"
#   include	"sioMemory.h"
#   include	"sioGeneral.h"

#   include	<appDebugon.h>

typedef struct SioInHexedMemoryPrivate
    {
    struct SimpleInputStream *	shmpSisMem;
    struct SimpleInputStream *	shmpSisHex;
    } SioInHexedMemoryPrivate;

static int sioInHexedMemoryClose(	void *	voidshmp )
    {
    SioInHexedMemoryPrivate *	shmp= (SioInHexedMemoryPrivate *)voidshmp;

    if  ( shmp->shmpSisHex )
	{ sioInClose( shmp->shmpSisHex );	}

    if  ( shmp->shmpSisMem )
	{ sioInClose( shmp->shmpSisMem );	}

    free( shmp );

    return 0;
    }

static int sioInHexedMemoryReadBytes(	void *		voidshmp,
					unsigned char *	buffer,
					unsigned int	count )
    {
    SioInHexedMemoryPrivate *	shmp= (SioInHexedMemoryPrivate *)voidshmp;

    return sioInReadBytes( shmp->shmpSisHex, buffer, count );
    }

SimpleInputStream * sioInHexedMemoryOpen( const struct MemoryBuffer *	mb )
    {
    SimpleInputStream *		sis;
    SioInHexedMemoryPrivate *	shmp;

    shmp= (SioInHexedMemoryPrivate *)malloc( sizeof(SioInHexedMemoryPrivate) );
    if  ( ! shmp )
	{ XDEB(shmp); return (SimpleInputStream *)0;	}

    shmp->shmpSisMem= (SimpleInputStream *)0;
    shmp->shmpSisHex= (SimpleInputStream *)0;

    shmp->shmpSisMem= sioInMemoryOpen( mb );
    if  ( ! shmp->shmpSisMem )
	{
	XDEB(shmp->shmpSisMem);
	sioInHexedMemoryClose( shmp );
	return (SimpleInputStream *)0;
	}

    shmp->shmpSisHex= sioInHexOpen( shmp->shmpSisMem );
    if  ( ! shmp->shmpSisHex )
	{
	XDEB(shmp->shmpSisHex);
	sioInHexedMemoryClose( shmp );
	return (SimpleInputStream *)0;
	}

    sis= sioInOpen( (void *)shmp,
			    sioInHexedMemoryReadBytes, sioInHexedMemoryClose );

    if  ( ! sis )
	{
	XDEB(sis);
	sioInHexedMemoryClose( shmp );
	return (SimpleInputStream *)0;
	}

    return sis;
    }

typedef struct SioOutHexedMemoryPrivate
    {
    struct SimpleOutputStream *	shmpSosMem;
    struct SimpleOutputStream *	shmpSosHex;
    } SioOutHexedMemoryPrivate;

static int sioOutHexedMemoryClose(	void *	voidshmp )
    {
    SioOutHexedMemoryPrivate *	shmp= (SioOutHexedMemoryPrivate *)voidshmp;

    if  ( shmp->shmpSosHex )
	{ sioOutClose( shmp->shmpSosHex );	}

    if  ( shmp->shmpSosMem )
	{ sioOutClose( shmp->shmpSosMem );	}

    free( shmp );

    return 0;
    }

static int sioOutHexedMemoryWriteBytes(	void *			voidshmp,
					const unsigned char *	buffer,
					int			count )
    {
    SioOutHexedMemoryPrivate *	shmp= (SioOutHexedMemoryPrivate *)voidshmp;

    return sioOutWriteBytes( shmp->shmpSosHex, buffer, count );
    }

SimpleOutputStream * sioOutHexedMemoryOpen(	struct MemoryBuffer *	mb )
    {
    SimpleOutputStream *	sos;
    SioOutHexedMemoryPrivate *	shmp;

    shmp= (SioOutHexedMemoryPrivate *)malloc(
					sizeof(SioOutHexedMemoryPrivate) );
    if  ( ! shmp )
	{ XDEB(shmp); return (SimpleOutputStream *)0;	}

    shmp->shmpSosHex= (SimpleOutputStream *)0;
    shmp->shmpSosMem= (SimpleOutputStream *)0;

    shmp->shmpSosMem= sioOutMemoryOpen( mb );
    if  ( ! shmp->shmpSosMem )
	{
	XDEB(shmp->shmpSosMem);
	sioOutHexedMemoryClose( shmp );
	return (SimpleOutputStream *)0;
	}

    shmp->shmpSosHex= sioOutHexOpen( shmp->shmpSosMem );
    if  ( ! shmp->shmpSosHex )
	{
	XDEB(shmp->shmpSosHex);
	sioOutHexedMemoryClose( shmp );
	return (SimpleOutputStream *)0;
	}

    sos= sioOutOpen( (void *)shmp,
			sioOutHexedMemoryWriteBytes, sioOutHexedMemoryClose );
    if  ( ! sos )
	{
	XDEB(sos);
	sioOutHexedMemoryClose( shmp );
	return (SimpleOutputStream *)0;
	}

    return sos;
    }

