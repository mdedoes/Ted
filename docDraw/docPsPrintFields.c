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

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	if  ( docPsPrintStartBookmark( dtl, df, lineTop ) )
	    { LDEB(1); return -1;	}
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	if  ( docPsPrintStartNote( dtl, part, x0Twips, df, lineTop ) )
	    { LDEB(1); return -1;	}
	}

    if  ( df->dfKind == DOCfkHYPERLINK )
	{
	if  ( docPsPrintStartHyperlink( dtl, x0Twips, df ) )
	    { LDEB(1); return -1;	}
	}

    if  ( df->dfKind == DOCfkSYMBOL )
	{
	if  ( docPsPrintStartSymbol( dtl, x0Twips, df ) )
	    { LDEB(1); return -1;	}
	}

    if  ( df->dfKind == DOCfkLISTTEXT )
	{
	if  ( docPsPrintStartListTextField( dtl, df ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int docPsPrintFinishField(	const DrawTextLine *	dtl,
				int			part,
				int			x1Twips,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( df->dfKind == DOCfkHYPERLINK )
	{
	if  ( ps->psInsideLink )
	    {
	    if  ( docPsPrintFinishLink( dtl, x1Twips, df ) )
		{ LDEB(1); return -1;	}
	    }

	return 0;
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	/* TODO Make notes work correctly */
	ps->psInsideLink= 0;
	return 0;
	}

    if  ( df->dfKind == DOCfkSYMBOL )
	{
	if  ( docPsPrintFinishSymbol( dtl, df ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    if  ( df->dfKind == DOCfkLISTTEXT )
	{
	if  ( ps->psInsideListLabel )
	    {
	    if  ( docPsPrintFinishListTextField( dtl, df ) )
		{ LDEB(1); return -1;	}
	    }

	return 0;
	}

    return 0;
    }

