/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<string.h>

#   include	"docDrawLine.h"
#   include	"docPsPrintImpl.h"
#   include	<psPrint.h>
#   include	<docObject.h>
#   include	<docTextLine.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

StructItem * docPsPrintInlineStructItem(
				PrintingState *		ps,
				const char *		languageTag )
    {
    StructItem *	structItem;

    if  ( ps->psInsideLink )
	{
	structItem= psPdfAnnotatedStructItem( ps, STRUCTtypeLINK, 1 );

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return (StructItem *)0;	}

	if  ( psPdfmarkAppendMarkedLink( ps, structItem ) )
	    { LDEB(1); return (StructItem *)0;	}

	return structItem;
	}

    if  ( ps->psInsideListLabel )
	{
	structItem= psPdfAnnotatedStructItem( ps, STRUCTtypeLBL, 1 );

	if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	    { XDEB(structItem); return (StructItem *)0;	}

	if  ( psPdfmarkAppendMarkedLeaf( ps, structItem, (MemoryBuffer *)0 ) )
	    { LDEB(1); return (StructItem *)0;	}

	return structItem;
	}

    structItem= psPdfLeafStructItem( ps, STRUCTtypeSPAN, 1 );
    structItem->siLanguageTag= languageTag;

    if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
	{ XDEB(structItem); return (StructItem *)0;	}

    if  ( psPdfmarkAppendMarkedLeaf( ps, structItem, (MemoryBuffer *)0 ) )
	{ LDEB(1); return (StructItem *)0;	}

    return structItem;
    }

/**
 *  Make sure that if we are emitting textual content that is part 
 *  of the reading order of the document, that the current leaf in 
 *  the structure tree is a /Span or a /Link.
 */
int docPsPrintClaimInline(	PrintingState *		ps,
				const char *		languageTag )
    {
    int			rval= 0;

    MemoryBuffer	extraProperties;

    utilInitMemoryBuffer( &extraProperties );

    if  ( ! ps->psInArtifact )
	{
	if  ( ps->psCurrentStructItem 					&&
	      ps->psCurrentStructItem->siIsLeaf				&&
	      ! ps->psCurrentStructItem->siIsInline			)
	    {
	    psPdfPopStructItem( ps );
	    psPdfEndMarkedContent( ps, "ILSE" );
	    }

	if  ( ! ps->psCurrentStructItem 		||
	      ! ps->psCurrentStructItem->siIsLeaf	)
	    {
	    StructItem *	structItem= docPsPrintInlineStructItem( ps, languageTag );

	    if  ( ! structItem )
		{ XDEB(structItem); rval= -1; goto ready;	}

	    if  ( languageTag )
		{
		const char *		key= "Lang";
		SimpleOutputStream *	sosExtra= sioOutMemoryOpen( &extraProperties );

		if  ( sioOutPrintf( sosExtra, " /%s ", key ) < 0 )
		    { SDEB(key); rval= -1; goto ready;	}

		if  ( psPrintStringValue( sosExtra, languageTag, strlen( languageTag ), 0 ) )
		    { SDEB(languageTag); rval= -1; goto ready;	}

		sioOutClose( sosExtra );
		}

	    if  ( psPdfBeginMarkedContent( ps,
				structItem->siStructureType,
				structItem->siContentId,
				&extraProperties ) )
		{ LDEB(structItem->siContentId); rval= -1; goto ready;	}
	    }
	}

  ready:

    utilCleanMemoryBuffer( &extraProperties );

    return rval;
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
	psPdfEndMarkedContent( ps, "ILSE" );
	}

    return 0;
    }

/**
 *  Do we need to embed non-textual in-line output in an artifact?
 *  It looks like this is only needed outside an InLineStructElem (ILSE)
 */
int docPsPrintNeedInlineArtifact(	PrintingState *	ps )
    {
    if  ( ps->psInsideLink						&&
	  ps->psCurrentStructItem 					&&
	  ps->psCurrentStructItem->siIsLeaf				&&
	  ps->psCurrentStructItem->siIsInline				)
	{ return 0;	}
    else{
	return ps->psTagDocumentStructure && ! ps->psInArtifact;
	}
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
	return psPdfEndMarkedContent( ps, STRUCTtypeFIGURE );
	}
    else{
	return docPsPrintEndArtifact( ps );
	}
    }

