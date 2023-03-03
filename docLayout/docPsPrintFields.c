/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docHyperlinkField.h>
#   include	<docSymbolField.h>
#   include	<docBookmarkField.h>
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<psPrint.h>
#   include	<docFields.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docPsPrintStartBookmark(
		    const DrawTextLine *	dtl,
		    const DocumentField *	df,
		    int				lineTop )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    const MemoryBuffer *	mbBookmark;

    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	{
	psDestPdfmark( ps, lineTop, mbBookmark );
	}

    return 0;
    }

static int docPsPrintStartNote(
		    const DrawTextLine *	dtl,
		    int				part,
		    int				x0Twips,
		    const DocumentField *	df,
		    int				lineTop )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    struct BufferItem *		paraNode= dtl->dtlParaNode;


    int				cnt;
    int				closed;

    MemoryBuffer		mbSource;
    MemoryBuffer		mbTarget;

    utilInitMemoryBuffer( &mbSource );
    utilInitMemoryBuffer( &mbTarget );

    cnt= docCountParticulesInFieldFwd( paraNode, &closed, part,
				    paraNode->biParaParticuleCount );
    if  ( cnt < 0 )
	{ LDEB(cnt); }

    if  ( docSetNoteLinks( &mbTarget, &mbSource, paraNode, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* Include destination to jump to from the target */
    psDestPdfmark( ps, lineTop, &mbSource );

    /* Remember what the target is to use that at the end of the link */
    utilEmptyMemoryBuffer( &(ps->psLinkFile) );

    utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
    if  ( paraNode->biTreeType == DOCinBODY )
	{
	struct DocumentNote *	dn;

	dn= docGetNoteOfField( df, dtl->dtlDocument );
	if  ( dn )
	    {
	    if  ( docCollectNoteTitle( &(ps->psLinkTitle),
						    dn, dtl->dtlDocument ) )
		{ LDEB(1); rval= -1;	}
	    }
	}

    if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &mbTarget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ps->psInsideLink= 1;
    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= x0Twips;

  ready:

    utilCleanMemoryBuffer( &mbSource );
    utilCleanMemoryBuffer( &mbTarget );

    return rval;
    }

static int docPsPrintStartHyperlink(
		    const DrawTextLine *	dtl,
		    int				x0Twips,
		    const DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    HyperlinkField		hf;

    docInitHyperlinkField( &hf );

    if  ( ! docGetHyperlinkField( &hf, df ) )
	{
	ps->psInsideLink= 1;
	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x0Twips;

	utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
	if  ( utilCopyMemoryBuffer( &(ps->psLinkFile), &(hf.hfFile) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &(hf.hfBookmark) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ps->psTagDocumentStructure	&&
	      ! ps->psInArtifact		&&
	      docPsPrintFinishInline( ps )	)
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

static int docPsPrintStartSymbol(
		    const DrawTextLine *	dtl,
		    int				x0Twips,
		    const DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    SymbolField			sf;

    docInitSymbolField( &sf );

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( ! docFieldGetSymbol( &sf, df ) )
	    {
	    /* Make sure the field result gets its own span */
	    if  ( docPsPrintFinishInline( ps ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( sf.sfIsDecoration )
		{
		if  ( docPsPrintBeginInlineArtifact( dtl, x0Twips ) )
		    { LDEB(sf.sfIsDecoration); rval= -1; goto ready;	}
		}
	    else{
		/* Studioform/Beyondoc extension */
		if  ( ! utilMemoryBufferIsEmpty( &(sf.sfActualText) ) )
		    {
		    StructItem *	structItem= docPsPrintInlineStructItem( ps );

		    if  ( ! structItem )
			{ XDEB(structItem); return -1;	}

		    if  ( psPdfBeginMarkedContentActualText( ps,
					    structItem->siStructureType,
					    structItem->siContentId,
					    &(sf.sfActualText) ) )
			{ LDEB(structItem->siContentId); return -1;	}
		    }
		}
	    }
	}

  ready:

    docCleanSymbolField( &sf );

    return rval;
    }

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

    return 0;
    }

static int docPsPrintFinishLink( const DrawTextLine *	dtl,
				int			x1Twips,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    const TextLine *	tl= dtl->dtlTextLine;
    int			lineTop= tl->tlTopPosition.lpPageYTwips;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( docPsFinishAnnotation( ps, x1Twips, lineTop, dtl->dtlLineHeight ) )
	    { LDEB(1); return -1;	}

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(1); return -1;	}
	}
    else{
	psFlushLink( ps, x1Twips, lineTop, dtl->dtlLineHeight );
	}

    ps->psInsideLink= 0;

    return 0;
    }

static int docPsPrintFinishSymbol( const DrawTextLine *	dtl,
				const DocumentField *	df )
    {
    int				rval= 0;
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    SymbolField			sf;

    docInitSymbolField( &sf );

    if  ( ps->psTagDocumentStructure )
	{
	if  ( ! docFieldGetSymbol( &sf, df ) )
	    {
	    if  ( ! ps->psInArtifact )
		{
		if  ( ! ps->psCurrentStructItem 		||
		      ! ps->psCurrentStructItem->siIsLeaf	)
		    { XDEB(ps->psCurrentStructItem); rval= -1; goto ready;	}

		if  ( ! utilMemoryBufferIsEmpty( &(sf.sfActualText) )		&&
		      psPdfMarkSetActualText( ps, ps->psCurrentStructItem, &(sf.sfActualText) ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( docPsPrintFinishInline( ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
	    else{
		/* Strictly speaking this is incorrect. It assumes that we are inside an artifact because
		   the field is a decoration. But we could be in an artifact for a different reason.
		   E.G. in a page header */
		if  ( sf.sfIsDecoration && ps->psInArtifact == 1 && docPsPrintEndArtifact( ps ) )
		    { LLDEB(sf.sfIsDecoration,ps->psInArtifact); rval= -1; goto ready;	}
		}
	    }
	}

  ready:

    docCleanSymbolField( &sf );

    return rval;
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

    if  ( df->dfKind == DOCfkSYMBOL )
	{
	if  ( docPsPrintFinishSymbol( dtl, df ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    return 0;
    }

