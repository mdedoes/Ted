/************************************************************************/
/*									*/
/*  Ted: Interface to the find tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<docFind.h>
#   include	"tedFind.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<tedDocFront.h>
#   include	<tedAppFront.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>

#   include	<appDebugon.h>

int tedDocFindNext(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
		    (struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsTail), bd,
				docFindParaFindNext, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );
	}

    return ret;
    }

int tedDocFindPrev(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! td->tdFindProg )
	{ XDEB(td->tdFindProg); return -1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
			(struct DocumentTree **)0,
			(struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindPrevInDocument( &dsNew, &(ds.dsHead), bd,
				docFindParaFindPrev, (void *)td->tdFindProg );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int		lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );
	}

    return ret;
    }

int tedSpellFindNext(	void *			voidea,
			MemoryBuffer *		mbGuess,
			struct SpellScanJob *	ssj )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    struct BufferDocument *	bd;

    int				ret;

    DocumentSelection		dsNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(struct DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsTail), bd,
					docSpellParaFindNext, (void *)ssj );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	const int	lastLine= 0;

	tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

	if  ( mbGuess && docCollectReference( mbGuess, &dsNew, bd ) )
	    { LDEB(1); return -1;	}
	}

    return ret;
    }

