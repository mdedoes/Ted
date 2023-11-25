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
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<psPrint.h>
#   include	<docFields.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

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

static int docPsPrintStartListTextField(
		    const DrawTextLine *	dtl,
		    const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	ps->psInsideListLabel= 1;

	if  ( docPsPrintFinishInline( ps )	)
	    { LDEB(1); return -1;	}
	}

    return 0;
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

    if  ( df->dfKind == DOCfkLISTTEXT )
	{
	if  ( docPsPrintStartListTextField( dtl, df ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docPsPrintFinishListTextField(
				const DrawTextLine *	dtl,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(ps->psInsideListLabel); return -1;	}
	}

    ps->psInsideListLabel= 0;

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

