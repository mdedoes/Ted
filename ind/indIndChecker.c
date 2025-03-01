/****************************************************************/
/*								*/
/*  Utility functions for scanning documents for typographical	*/
/*  errors.							*/
/*								*/
/****************************************************************/

#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	"indSpellScanJob.h"
#   include	"indSpellChecker.h"
#   include	"indIndSpellDictionary.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  1)  Give a judgement on the validity of a word.			*/
/*									*/
/*  2)  In a list of possiblities, count the number that is not yet	*/
/*	rejected.							*/
/*									*/
/************************************************************************/

/*  1  */
static int indCheckWord(	const char *		word,
				SpellDictionary *	sd,
				int			asPrefix )
    {
    int				accepted;
    int				ignoredHow;

    IndSpellDictionary *	isd= (IndSpellDictionary *)sd->sdBackend;

    if  ( ! asPrefix && isdigit( word[0] ) )
	{ return 0;	}

    if  ( ! asPrefix && isd->isdForgotInd )
	{
	/*  a  */
	if  ( indGetUtf8( &accepted, isd->isdForgotInd, word ) >= 0	&&
	      accepted							)
	    { return -1;	}
	}

    /*  b  */
    if  ( ! indGetWord( &ignoredHow, isd->isdStaticInd, word, asPrefix ) )
	{ return 0;	}

    /*  c  */
    if  ( isd->isdLearntInd &&
	  indGetUtf8( &accepted, isd->isdLearntInd, word ) >= 0	&&
	  ( accepted || asPrefix )				)
	{ return 0; }

    return -1;
    }

/*  2  */
int indCountPossibilities(	SpellScanJob *		ssj,
				SpellDictionary *	sd,
				int			position,
				int			rejectPrefices )
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    int			count= 0;

    while( pw )
	{
	if  ( pw->pwRejectedAt == -1 )
	    {
	    int	rejectedAsWord;
	    int	rejectedAsPrefix;

	    rejectedAsWord= indCheckWord( pw->pwForm, sd, 0 );

	    /* LSLDEB(pw->pwStartAt,pw->pwForm,rejectedAsWord); */

	    if  ( rejectedAsWord )
		{
		rejectedAsPrefix= rejectPrefices ||
					indCheckWord( pw->pwForm, sd, 1 );
		if  ( rejectedAsPrefix )
		    { pw->pwRejectedAt= position;	}
		}
	    else{
		rejectedAsPrefix= 0;
		pw->pwAcceptedAt= position;
		}

	    if  ( ! rejectedAsPrefix )
		{
		rejectedAsPrefix= indCheckWord( pw->pwForm, sd, 1 );
		/* SLDEB(pw->pwForm,rejected); */
		pw->pwForm[pw->pwInsertionPoint  ]= '\0';

		if  ( rejectedAsPrefix )
		    { pw->pwRejectedAt= position;	}
		}

	    if  ( ! rejectedAsWord || ! rejectedAsPrefix )
		{ count++;	}
	    }

	pw= pw->pwNext;
	}

    return count;
    }

