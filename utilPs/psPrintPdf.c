/************************************************************************/
/*									*/
/*  Print Pdf through ghostscript.					*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<utilMemoryBufferPrintf.h>
#   include	"psPrint.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print Pdf through ghostscript.					*/
/*									*/
/************************************************************************/

int psSetToPdfCommand(	MemoryBuffer *			command,
			int				pageTwipsWide,
			int				pageTwipsHigh,
			const MemoryBuffer *		filename )
    {
    if  ( filename && ! utilMemoryBufferIsEmpty( filename ) )
	{
	utilMemoryBufferPrintf( command,
	    "ps2pdf -dDEVICEWIDTHPOINTS=%d -dDEVICEHEIGHTPOINTS=%d - '%s'",
				( pageTwipsWide+ 19)/20,
				( pageTwipsHigh+ 19)/20,
				utilMemoryBufferGetString( filename ) );
	}
    else{
	utilMemoryBufferPrintf( command,
	    "ps2pdf -dDEVICEWIDTHPOINTS=%d -dDEVICEHEIGHTPOINTS=%d - -",
				( pageTwipsWide+ 19)/20,
				( pageTwipsHigh+ 19)/20 );
	}

    return 0;
    }

int psSetTestToPdfCommand(	MemoryBuffer *			command )
    {
    if  ( utilMemoryBufferSetString( command,
			    "echo showpage | ps2pdf - > /tmp/xx.pdf" ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
