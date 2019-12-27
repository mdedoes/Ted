/************************************************************************/
/*									*/
/*  Editor functionality: Split nodes in the document tree.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<docParaParticules.h>
#   include	<docRtfTrace.h>
#   include	<docLayout.h>
#   include	<docEditCommand.h>
#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>
#   include	<docBreakKind.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  1)  Split in the buffer administration.				*/
/*  2)  Recalculate the geometry from the start of the selection to the	*/
/*	end of the paragraph.						*/
/*  3)  Redraw the affected part of the paragraph.			*/
/*  4)  Cause the new paragraph to be reformatted in this edit		*/
/*	operation.							*/
/*  5)  If the new paragraph is part of a numbered/bulleted list,	*/
/*	insert a list text field at the head.				*/
/*									*/
/************************************************************************/

static int tedSplitParaNode(	TedEditOperation *		teo,
				int				splitLevel,
				int				breakKind )
    {
    EditOperation *		eo= &(teo->teoEo);
    const LayoutContext *	lc= &(teo->teoLayoutContext);
    struct BufferItem *		oldParaNode= eo->eoTailDp.dpNode;
    struct BufferItem *		newParaNode;

    DocumentRectangle		drPara;

    docNodeRectangle( &drPara, oldParaNode, lc, (const BlockOrigin *)0 );

    /*  1  */
    if  ( docEditSplitParaNode( &newParaNode, &(teo->teoEo), splitLevel ) )
	{ LDEB(1); return -1;	}

    if  ( newParaNode->biParaProperties->ppTableNesting != 0 )
	{ breakKind= DOCibkNONE;	}
    else{
	/*TODO: Is not undone by undo
	if  ( breakKind != DOCibkNONE )
	    { docParaNodeSetKeepWithNext( oldParaNode, 0, eo->eoDocument ); }
	*/
	}

    docParaNodeSetBreakKind( newParaNode, breakKind, eo->eoDocument );
    
    /*  3  */
    geoUnionRectangle( &(teo->teoChangedRect),
				    &(teo->teoChangedRect), &drPara );

    if  ( docHeadPosition( &(eo->eoTailDp), newParaNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void tedDocSplitParagraph(	struct EditDocument *	ed,
				int			breakKind,
				const int		editCommand,
				int			traced )
    {
    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpBefore;
    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo, editCommand, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    if  ( docDeleteSelection( &(teo.teoEo) ) )
	{ LDEB(1); goto ready;	}

    if  ( tedSplitParaNode( &teo, DOClevPARA, breakKind ) )
	{ LDEB(1); goto ready;	}

    dpBefore= teo.teoEo.eoTailDp;
    if  ( docPrevPosition( &dpBefore  ) )
	{ LDEB(1); goto ready;	}

    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( &(teo.teoEo), SELposTAIL ) )
	    { LDEB(1); goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a section break in the document.				*/
/*									*/
/*  We do not want to insert a section break immediately before or	*/
/*  after a paragraph break: In those cases, let the section break	*/
/*  replace the paragraph break.					*/
/*									*/
/*  1)  If we are at the head of the paragraph, and it is possible to	*/
/*	navigate to a previous position.. Do so: It does not make sense	*/
/*	to split the section inside the paragraph.			*/
/*  2)  If we are at the end of the paragraph, and it is possible to	*/
/*	navigate to a subsequent position.. Do so: It does not make	*/
/*	sense to split the section inside the paragraph.		*/
/*									*/
/************************************************************************/

int tedDocInsertSectBreak(	struct EditDocument *	ed,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsTraced;
    int				pastFlags= 0;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    /*  1  */
    if  ( ( sd.sdHeadFlags & POSflagPARA_HEAD )	&&
	  ! docPrevPosition( &(eo->eoHeadDp) )	)
	{
	docSetEditPosition( &(eo->eoReformatRange.erHead), &(eo->eoHeadDp) );
	eo->eoSelectedRange.erHead= eo->eoReformatRange.erHead;
	eo->eoAffectedRange.erHead= eo->eoReformatRange.erHead;
	}

    /*  2  */
    if  ( ( sd.sdPastFlags & POSflagPARA_TAIL )	&&
	  ! docNextPosition( &(eo->eoTailDp) )	)
	{
	eo->eoLastDp= eo->eoTailDp;
	docSetEditPosition( &(eo->eoReformatRange.erTail), &(eo->eoTailDp) );
	eo->eoSelectedRange.erTail= eo->eoReformatRange.erTail;
	eo->eoAffectedRange.erTail= eo->eoReformatRange.erTail;
	}

    if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdSPLIT_SECT, DOClevSPAN, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDeleteSelection( eo ) )
	{ rval= -1; goto ready;	}

    if  ( docFindParticuleOfPosition( (int *)0, &pastFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! ( pastFlags & (POSflagPARA_HEAD|POSflagPARA_TAIL) ) )
	{
	if  ( tedSplitParaNode( &teo, DOClevSECT, DOCibkNONE ) )
	    { LDEB(1); rval= -1; goto ready;	}

	eo->eoSectionsAdded++;

	/*  7,8  */
	tedEditFinishSelectionTail( &teo );
	}
    else{
	const int		after= ( pastFlags & POSflagPARA_HEAD ) == 0;
	DocumentPosition	dpBefore;
	DocumentPosition	dpAfter;
	DocumentSelection	dsSplit;

	const int		split= 1;
	const int		direction= 1;

	if  ( tedEditInsertSection( &dpBefore, &dpAfter, &teo, split, after ) )
	    { LDEB(after); rval= -1; goto ready;	}

	docSetRangeSelection( &dsSplit, &dpBefore, &dpAfter, direction );
	docSetEditRange( &(eo->eoAffectedRange), &dsSplit );
	eo->eoSelectedRange= eo->eoAffectedRange;

	/*  7,8  */
	tedEditFinishSelectionTail( &teo );
	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }
