/************************************************************************/
/*									*/
/*  Renumber notes as the result of a modification to the document.	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<utilTree.h>
#   include	"docBuf.h"
#   include	"docRecalculateFields.h"

#   include	<docDocumentField.h>
#   include	<docFieldKind.h>

#   include	<appDebugon.h>

/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docTreeRestartSequence(	const char *		key,
					void *			val,
					void *			through )
    {
    *(int *)val= 0;
    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Renumber notes: Scan the fields in preorder and change the numbers.	*/
/*									*/
/************************************************************************/

int docRenumberSeqFields(	int *			pChanged,
				DocumentTree *		dt,
				BufferDocument *	bd )
    {
    int			changed= 0;
    DocumentField *	df;

    if  ( bd->bdSeqFieldIdentifiers )
	{
	const int	stopOnFailure= 1;

	if  ( utilTreeForAll( bd->bdSeqFieldIdentifiers, stopOnFailure,
					docTreeRestartSequence, (void *)0 ) )
	    { LDEB(1); return -1;	}
	}

    df= docGetFirstField( &(dt->dtRootFields) );
    while( df )
	{
	if  ( df->dfKind == DOCfkSEQ )
	    {
	    int		chg= 0;

	    if  ( docRenumberSeqField( &chg, df, bd ) )
		{ LDEB(1); return -1;	}

	    if  ( chg )
		{ changed= 1;	}
	    }

	df= docGetNextField( &(dt->dtRootFields), df );
	}

    if  ( pChanged )
	{ *pChanged= changed;	}

    return 0;
    }

