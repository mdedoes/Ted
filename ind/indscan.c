/****************************************************************/
/*								*/
/*  Utility functions for scanning documents for typographical	*/
/*  errors.							*/
/*								*/
/****************************************************************/

#   include	"indConfig.h"

#   include	<stdlib.h>
#   include	<textRegexp.h>

#   include	"indSpellScanJob.h"

#   include	<appDebugon.h>

/****************************************************************/

/* PCRE syntax */
/*
Only work for ASCII:
static const char indSpellWordPattern[]= "\\b(\\w+)\\W*";
static const char indSpellWordPattern[]= "\\b([[:alnum:]]+)[^[:alnum:]]*";
Look like unicode, but \\b only works for ASCII
static const char indSpellWordPattern[]= "\\b([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";
*/

static const char indSpellFirstWordPat[]=
		    "[^\\p{N}\\p{L}]*([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";

static const char indSpellToWordPat[]=
		    "[\\p{N}\\p{L}]*[^\\p{N}\\p{L}]*";

/* UNUSED
static const char indSpellNextWordPat[]=
		    "([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";
*/

/****************************************************************/
/*								*/
/*  Cleanup routine: Free a possibility list.			*/
/*								*/
/****************************************************************/

static void indCleanSpellScanWords(	SpellScanJob *	ssj )
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	PossibleWord *	next= pw->pwNext;

	free( (char *)pw );

	pw= next;
	}

    return;
    }

void	indCleanSpellScanJob(	SpellScanJob *	ssj	)
    {
    indCleanSpellScanWords( ssj );

    /*
    if  ( ssj->ssjFirstWordExpr )
	{ regFree( ssj->ssjFirstWordExpr );	}
    */
    if  ( ssj->ssjToWordExpr )
	{ regFree( ssj->ssjToWordExpr );	}
    if  ( ssj->ssjNextWordExpr )
	{ regFree( ssj->ssjNextWordExpr );	}

    return;
    }

int indStartSpellScanJob(	SpellScanJob *	ssj,
				int		acceptedPos,
				const char *	localeTag )
    {
    indCleanSpellScanWords( ssj );

    ssj->ssjPossibleWords= (PossibleWord *)0;
    ssj->ssjPossibleWordCount= 0;

    ssj->ssjAcceptedPos= acceptedPos;

    /*
    if  ( ! ssj->ssjFirstWordExpr )
	{
	int		options= 0;

	ssj->ssjFirstWordExpr= regCompile( indSpellFirstWordPat, options );

	if  ( ! ssj->ssjFirstWordExpr )
	    { SXDEB(indSpellFirstWordPat,ssj->ssjFirstWordExpr); return -1; }
	}
    */

    if  ( ! ssj->ssjToWordExpr )
	{
	int		options= 0;

	ssj->ssjToWordExpr= regCompile( indSpellToWordPat, options );

	if  ( ! ssj->ssjToWordExpr )
	    { SXDEB(indSpellToWordPat,ssj->ssjToWordExpr); return -1; }
	}

    if  ( ! ssj->ssjNextWordExpr )
	{
	int		options= 0;

	ssj->ssjNextWordExpr= regCompile( indSpellFirstWordPat, options );

	if  ( ! ssj->ssjNextWordExpr )
	    { SXDEB(indSpellFirstWordPat,ssj->ssjNextWordExpr); return -1; }
	}

    ssj->ssjOverrideLocaleTag= localeTag;

    return 0;
    }

void indInitSpellScanJob(	SpellScanJob *	ssj )
    {
    ssj->ssjAcceptedPos= 0;

    ssj->ssjPossibleWords= (PossibleWord *)0;
    ssj->ssjPossibleWordCount= 0;

    ssj->ssjDictionary= (struct SpellDictionary *)0;
    ssj->ssjChecker= (struct SpellChecker *)0;

    ssj->ssjComplain= (SpellComplain)0;
    ssj->ssjThrough= (void *)0;

    /*
    ssj->ssjFirstWordExpr= (void *)0;
    */
    ssj->ssjToWordExpr= (void *)0;
    ssj->ssjNextWordExpr= (void *)0;

    ssj->ssjOverrideLocaleTag= (const char *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Start a new possibility.						*/
/*									*/
/************************************************************************/

int	indNewPossibility(	SpellScanJob *		ssj,
				int			position )
    {
    PossibleWord *	next= ssj->ssjPossibleWords;
    PossibleWord *	pw= (PossibleWord *)malloc(sizeof(PossibleWord));

    if  ( ! pw )
	{ XDEB(pw); return -1;	}

    pw->pwStartAt= position;
    pw->pwInsertionPoint= 0;
    pw->pwRejectedAt= -1;
    pw->pwAcceptedAt= -1;
    pw->pwNext= next;
    pw->pwForm[0]= '\0';

    ssj->ssjPossibleWords= pw;

    return 0;
    }

/****************************************************************/
/*  Add a character to all the possibilities in a list.		*/
/****************************************************************/

void indAddCharacterToPossibilities(	SpellScanJob *	ssj,
					int		c )
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	if  ( pw->pwRejectedAt == -1 )
	    {
	    if  ( pw->pwInsertionPoint >= FORM_MAX )
		{
		pw->pwRejectedAt= pw->pwStartAt+ pw->pwInsertionPoint;
		}
	    else{
		pw->pwForm[pw->pwInsertionPoint++]= c;
		pw->pwForm[pw->pwInsertionPoint  ]= '\0';
		}
	    }

	pw= pw->pwNext;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a list of the possibilities.					*/
/*									*/
/************************************************************************/

void indLogPossibilities(	SpellScanJob *	ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	appDebug( "Start %d Rejected %d Accepted %d \"%s\" \r\n",
			pw->pwStartAt, pw->pwRejectedAt, pw->pwAcceptedAt,
			pw->pwForm );

	pw= pw->pwNext;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Cleanup routine: Remove rejected possibilities.			*/
/*									*/
/*  1)  Scan the list opto the first possibility that has not yet been	*/
/*	rejected.							*/
/*  2)  Also remove rejected alternatives from the rest of the list.	*/
/*									*/
/************************************************************************/

void indRejectPossibilities(	SpellScanJob *		ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    PossibleWord *	pww= pw;
    PossibleWord *	next;

    int			acceptedPos= ssj->ssjAcceptedPos;

    /*  1  */
    while( pww						&&
	   pww->pwRejectedAt != -1			&&
	   pww->pwRejectedAt >= pww->pwAcceptedAt	)
	{
	next= pww->pwNext;

	if  ( acceptedPos < pww->pwAcceptedAt )
	    { acceptedPos=  pww->pwAcceptedAt;	}

	free( (char *)pww );

	pww= next;
	}

    /*  2  */
    if  ( pww )
	{
	pw= pww;
	while( pw )
	    {
	    next= pw->pwNext;

	    while( next						&&
		   next->pwRejectedAt != -1			&&
		   next->pwRejectedAt >= next->pwAcceptedAt	)
		{
		next= next->pwNext;

		if  ( acceptedPos < pww->pwAcceptedAt )
		    { acceptedPos=  pww->pwAcceptedAt;	}

		free( (char *)pw->pwNext );
		pw->pwNext= next;
		}

	    pw= next;
	    }
	}

    ssj->ssjPossibleWords= pww;
    ssj->ssjAcceptedPos= acceptedPos;

    return;
    }

/************************************************************************/
/*									*/
/*  Find a maximal possibility to reject. This is rather arbitrary.	*/
/*									*/
/************************************************************************/

PossibleWord * indMaximalPossibility(	SpellScanJob *	ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    PossibleWord *	rval= pw;

    if  ( ! rval )
	{ return rval;	}

    while( rval && rval->pwAcceptedAt >= 0 )
	{ rval= rval->pwNext; }

    if  ( ! rval )
	{ return rval; }

    pw= rval->pwNext;

    while( pw )
	{
	if  ( pw->pwAcceptedAt == -1 )
	    {
	    if  ( pw->pwStartAt < rval->pwStartAt )
		{ rval= pw; }
	    else{
		if  ( pw->pwStartAt == rval->pwStartAt	&&
		      pw->pwRejectedAt    >  rval->pwRejectedAt   	)
		    { rval= pw; }
		}
	    }

	pw= pw->pwNext;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Skip to the word after the initial position to check in the		*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int indSkipToWord(	SpellScanJob *			ssj,
			int *				pHere,
			const char *			paraStr,
			int				here,
			int				upto )
    {
    int			foundWord;
    int			head= here;

    ExpressionMatch	em;

    foundWord= regFindLeftToRight( &em, ssj->ssjToWordExpr,
							paraStr, here, upto );
    if  ( ! foundWord )
	{ return foundWord;	}

    if  ( regGetFullMatch( &head, &here, &em ) )
	{ LDEB(1); return -1;	}

    *pHere= here;
    return foundWord;
    }

/************************************************************************/
/*									*/
/*  Delimit the word after the current position.			*/
/*									*/
/************************************************************************/

int indFindNextWord(	SpellScanJob *			ssj,
			int *				pHead,
			int *				pTail,
			int *				pNext,
			const char *			paraStr,
			int				here,
			int				upto )
    {
    int			foundWord;
    int			head= here;
    int			tail= here;
    int			next= here;

    ExpressionMatch	em;

    foundWord= regFindLeftToRight( &em, ssj->ssjNextWordExpr,
							paraStr, here, upto );
    if  ( ! foundWord )
	{ return foundWord;	}

    if  ( regGetFullMatch( &here, &next, &em ) )
	{ LDEB(1); return -1;	}

    if  ( regGetMatch( &head, &tail, &em, 0 ) )
	{ LDEB(1); return -1;	}

    *pHead= head;
    *pTail= tail;
    *pNext= next;
    return foundWord;
    }

