/************************************************************************/
/*									*/
/*  Miscelaneous utility routines for managing private dictionaries.	*/
/*									*/
/************************************************************************/

#   include	"indConfig.h"

#   include	"indPrivateDict.h"
#   include	"indlocal.h"
#   include	<string.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Move a word from forgotten to learnt, or the other way round.	*/
/*									*/
/************************************************************************/

int	indMoveWord(	IND *		fromInd,
			IND *		toInd,
			const char *	word )
    {
    int				rval= 0;
    int				accepted;

    if  ( fromInd					&&
	  indGetUtf8( &accepted, fromInd, word ) >= 0	&&
	  accepted					)
	{
	if  ( indForget( fromInd, word ) )
	    { SDEB((char *)word); rval= -1;	}
	}

    if  ( indGetUtf8( &accepted, toInd, word ) < 0	||
	  ! accepted					)
	{
	if  ( indPutUtf8( toInd, word ) < 0 )
	    { SDEB(word); rval= -1;	}
	}

    return rval;
    }

/************************************************************************/
/*  Read a private dictionary and store its contents in two indices.	*/
/************************************************************************/

#   define SPELBUFL	300

int indReadPrivateDictionary(	SimpleInputStream *	sis,
				IND **			pLearntInd,
				IND **			pForgotInd )
    {
    IND *		learntInd= *pLearntInd;
    IND *		forgotInd= *pForgotInd;
    
    char		buf[SPELBUFL+ 1];

    int			rval= 0;

    if  ( ! learntInd )
	{
	learntInd= indMake();
	if  ( ! learntInd )
	    { XDEB( learntInd ); return -1;	}
	*pLearntInd= learntInd;
	}

    if  ( ! forgotInd )
	{
	forgotInd= indMake();
	if  ( ! forgotInd )
	    { XDEB( forgotInd ); return -1;	}
	*pForgotInd= forgotInd;
	}

    while( sioInGetString( buf, SPELBUFL, sis ) == buf )
	{
	buf[strlen( buf )- 1]= '\0';

	switch( buf[0] )
	    {
	    case 'F':
		(void) indMoveWord( learntInd, forgotInd, buf+ 2 );
		break;
	    case 'L':
		(void) indMoveWord( forgotInd, learntInd, buf+ 2 );
		break;
	    default:
		SDEB(buf); rval= -1; break;
	    }
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Learn/Forget a word and store a record about this in the private	*/
/*  dictionary.								*/
/*									*/
/************************************************************************/

int indLearnWord(	SimpleOutputStream *	sos,
			IND *			learntInd,
			IND *			forgotInd,
			const char *		word )
    {
    sioOutPrintf( sos, "%s %s\n", "L", word );

    if  ( indMoveWord( forgotInd, learntInd, word ) )
	{ SDEB(word); return -1;	}

    return 0;
    }

int indForgetWord(	SimpleOutputStream *	sos,
			IND *			learntInd,
			IND *			forgotInd,
			const char *		word )
    {
    sioOutPrintf( sos, "%s %s\n", "F", word );

    if  ( indMoveWord( learntInd, forgotInd, word ) )
	{ SDEB(word); return -1;	}

    return 0;
    }

