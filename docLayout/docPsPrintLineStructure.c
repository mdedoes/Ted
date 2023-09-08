/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docDrawLine.h"
#   include	"docPsPrintImpl.h"
#   include	<psPrint.h>
#   include	<docObject.h>
#   include	<docTextLine.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Inline level ones
 */
static const char STRUCTtypeSPAN[]= "Span";
static const char STRUCTtypeLINK[]= "Link";
static const char STRUCTtypeFIGURE[]= "Figure";

StructItem * docPsPrintInlineStructItem( PrintingState *	ps )
    {
    StructItem *	structItem;

    if  ( ps->psInsideLink )
	{
	structItem= psPdfAnnotatedStructItem( ps, STRUCTtypeLINK, 1 );

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return (StructItem *)0;	}

	if  ( psPdfmarkAppendMarkedLink( ps, structItem ) )
	    { LDEB(1); return (StructItem *)0;	}
	}
    else{
	structItem= psPdfLeafStructItem( ps, STRUCTtypeSPAN, 1 );

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return (StructItem *)0;	}

	if  ( psPdfmarkAppendMarkedLeaf( ps, structItem, (MemoryBuffer *)0 ) )
	    { LDEB(1); return (StructItem *)0;	}
	}

    return structItem;
    }

/**
 *  Make sure that if we are emitting textual content that is part 
 *  of the reading order of the document, that the current leaf in 
 *  the structure tree is a /Span or a /Link.
 */
int docPsPrintClaimInline( PrintingState *	ps )
    {
    if  ( ! ps->psInArtifact )
	{
	if  ( ps->psCurrentStructItem 					&&
	      ps->psCurrentStructItem->siIsLeaf				&&
	      ! ps->psCurrentStructItem->siIsInline			)
	    {
	    psPdfPopStructItem( ps );
	    psPdfEndMarkedContent( ps );
	    }

	if  ( ! ps->psCurrentStructItem 		||
	      ! ps->psCurrentStructItem->siIsLeaf	)
	    {
	    StructItem *	structItem= docPsPrintInlineStructItem( ps );

	    if  ( ! structItem )
		{ XDEB(structItem); return -1;	}

	    if  ( psPdfBeginMarkedContent( ps,
				structItem->siStructureType,
				structItem->siContentId ) )
		{ LDEB(structItem->siContentId); return -1;	}
	    }
	}

    return 0;
    }

/**
 *  If the current StructItem on the stack is an inline one, (/Span, /Link)
 *  pop it from the stack
 */
int docPsPrintFinishInline(	PrintingState *		ps )
    {
    if  ( ps->psCurrentStructItem 					&&
	  ps->psCurrentStructItem->siIsLeaf				&&
	  ps->psCurrentStructItem->siIsInline				)
	{
	psPdfPopStructItem( ps );
	psPdfEndMarkedContent( ps );
	}

    return 0;
    }

int docPsPrintBeginInlineArtifact(
			const DrawTextLine *		dtl,
			int				xTwips )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psInsideLink						&&
	  ps->psCurrentStructItem 					&&
	  ps->psCurrentStructItem->siIsLeaf				&&
	  ps->psCurrentStructItem->siIsInline				)
	{
	const TextLine *	tl= dtl->dtlTextLine;
	int			lineTop= tl->tlTopPosition.lpPageYTwips;

	if  ( docPsFinishAnnotation( ps, xTwips, lineTop, dtl->dtlLineHeight ) )
	    { LDEB(1); return -1;	}
	}

    return docPsPrintBeginArtifact( ps );
    }

int docPsPrintBeginFigure(
		const DrawTextLine *			dtl,
		const struct DocumentRectangle *	drTwips,
		const InsertedObject *			io )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ! utilMemoryBufferIsEmpty( &(io->ioAltText) ) )
	{
	StructItem * structItem= psPdfLeafStructItem( ps, STRUCTtypeFIGURE, 0 );

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(1); return -1;	}

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return -1;	}

	if  ( psPdfmarkAppendMarkedIllustration(
				ps, structItem, drTwips, &(io->ioAltText) ) )
	    { LDEB(1); return -1;	}

	if  ( psPdfBeginMarkedFigure( ps,
	      structItem->siStructureType, structItem->siContentId,
	      drTwips, &(io->ioAltText) ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    else{
	return docPsPrintBeginInlineArtifact( dtl, dtl->dtlX );
	}
    }

int docPsPrintEndFigure(
		const DrawTextLine *			dtl,
		const InsertedObject *			io )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ! utilMemoryBufferIsEmpty( &(io->ioAltText) ) )
	{
	psPdfPopStructItem( ps );
	return psPdfEndMarkedContent( ps );
	}
    else{
	return docPsPrintEndArtifact( ps );
	}
    }

