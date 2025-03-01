/************************************************************************/
/*									*/
/*  Issue (foot)note numbers as prescribed in the Chicago Manual of	*/
/*  Style.								*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"textChicago.h"
#   include	"uniUtf8.h"
#   include	<utilMemoryBufferPrintf.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

/************************************************************************/

int textChicagoString(	MemoryBuffer *	mbResult,
			int		n )
    {
    char	scratch[6];
    int		step;

    static const int syms[]= { 0x2a, 0x2020, 0x2021, 0xa7 };
    static const int nsym= sizeof(syms)/sizeof(syms[0]);

    if  ( n < 1 )
	{
	LDEB(n);
	utilMemoryBufferPrintf( mbResult, "%d", n );
	return 0;
	}

    n--;

    step= uniPutUtf8( scratch, syms[n%nsym] );
    if  ( step < 1 )
	{ LDEB(step); return -1;	}

    while( n > 0 )
	{
	if  ( utilMemoryBufferAppendBytes( mbResult,
					(unsigned char *)scratch, step ) )
	    { LDEB(step); return -1;	}

	n -= nsym;
	}

    return 0;
    }

