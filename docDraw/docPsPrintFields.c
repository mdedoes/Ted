/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTextLine.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<psPrint.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start a field							*/
/*									*/
/************************************************************************/

int docPsPrintStartField(	const DrawTextLine *		dtl,
				int				part,
				int				x0Twips,
				const struct DocumentField *	df )
    {
    const TextLine *		tl= dtl->dtlTextLine;
    int				lineTop= tl->tlTopPosition.lpPageYTwips;

    switch( df->dfKind )
	{
	case DOCfkBOOKMARK:
	    if  ( docPsPrintStartBookmark( dtl, df, lineTop ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;

	case DOCfkCHFTN:
	    if  ( docPsPrintStartNote( dtl, part, x0Twips, df, lineTop ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;

	case DOCfkHYPERLINK:
	    if  ( docPsPrintStartHyperlink( dtl, x0Twips, df ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;

	case DOCfkSYMBOL:
	    if  ( docPsPrintStartSymbol( dtl, x0Twips, df ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;

	case DOCfkLISTTEXT:
	    if  ( docPsPrintStartListTextField( dtl, df ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;
	}

    return 0;
    }

int docPsPrintFinishField(	const DrawTextLine *	dtl,
				int			part,
				int			x1Twips,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    if  ( ps->psInsideLink )
		{
		if  ( docPsPrintFinishNote( dtl, x1Twips, df ) )
		    { LDEB(df->dfKind); return -1;	}
		}
	    break;

	case DOCfkHYPERLINK:
	    if  ( ps->psInsideLink )
		{
		if  ( docPsPrintFinishHyperlink( dtl, x1Twips, df ) )
		    { LDEB(df->dfKind); return -1;	}
		}
	    break;

	case DOCfkSYMBOL:
	    if  ( docPsPrintFinishSymbol( dtl, df ) )
		{ LDEB(df->dfKind); return -1;	}
	    break;

	case DOCfkLISTTEXT:
	    if  ( ps->psInsideListLabel )
		{
		if  ( docPsPrintFinishListTextField( dtl, df ) )
		    { LDEB(df->dfKind); return -1;	}
		}
	    break;
	}

    return 0;
    }

