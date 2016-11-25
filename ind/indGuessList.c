#   include	"indConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	"indGuessList.h"
#   include	<appDebugoff.h>

/************************************************************************/
/*  Insert a guess in a guess list.					*/
/*  1)  If it is already there, just adapt score.			*/
/*  2)  Remember a copy of word.					*/
/*  3)  Store in list.							*/
/************************************************************************/

int	indAddGuess(	IndGuessList *		igl,
			const char *		word,
			int			score )
    {
    int			rval= 0;

    int			i;
    unsigned int	sz;
    char *		saved= (char *)0;
    IndGuessScore *	fresh;

    /*  1  */
    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( ! strcmp( igl->iglGuesses[i].igsWord, word ) )
	    {
	    if  ( igl->iglGuesses[i].igsScore < score )
		{ igl->iglGuesses[i].igsScore=  score;	}

	    goto ready;
	    }
	}

    /*  2  */
    sz= strlen( (char *)word )+ 1;
    saved= malloc( sz+ 1 );
    if  ( ! saved )
	{ LLDEB(sz,saved); rval= -1; goto ready;	}
    strcpy( saved, word );

    /*  3  */
    sz= ( igl->iglGuessCount+  1 )* sizeof(IndGuessScore);

    fresh= (IndGuessScore *)realloc( (void *)igl->iglGuesses, sz );
    if  ( ! fresh )
	{ LLDEB(sz,fresh); rval= -1; goto ready;	}

    igl->iglGuesses= fresh;

    fresh += igl->iglGuessCount++;

    fresh->igsWord= saved; saved= (char *)0; /* steal */
    fresh->igsScore= score;

  ready:

    if  ( saved )
	{ free( saved );	}

    return rval;
    }

void indCleanGuessList( IndGuessList *	igl )
    {
    int			i;

    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( igl->iglGuesses[i].igsWord )
	    { free( igl->iglGuesses[i].igsWord ); }
	}

    if  ( igl->iglGuesses )
	{ free( igl->iglGuesses ); }

    igl->iglGuessCount= 0;
    igl->iglGuesses= (IndGuessScore *)0;
    }

void indEmptyGuessList( IndGuessList *	igl )
    {
    int			i;

    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( igl->iglGuesses[i].igsWord )
	    { free( igl->iglGuesses[i].igsWord ); }
	}

    igl->iglGuessCount= 0;

    return;
    }

/************************************************************************/
/*  Sort a guess list.							*/
/************************************************************************/
static int indIgsCmp(	const void *	vigs1,
			const void *	vigs2	)
    {
    const IndGuessScore *	igs1= (const IndGuessScore *)vigs1;
    const IndGuessScore *	igs2= (const IndGuessScore *)vigs2;

    return igs2->igsScore- igs1->igsScore;
    }

void indSortGuesses( IndGuessList *	igl )
    {
    if  ( igl->iglGuessCount > 1 )
	{
	qsort(	(void *)igl->iglGuesses,
		igl->iglGuessCount, sizeof(IndGuessScore), indIgsCmp );
	}
    }

void indInitGuessList(	IndGuessList *	igl )
    {
    igl->iglGuessCount= 0;
    igl->iglGuesses= (IndGuessScore *)0;
    }
