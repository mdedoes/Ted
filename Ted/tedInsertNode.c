/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	<tedDocFront.h>
#   include	<docRtfTrace.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a paragraph Before/After the selection, depending on the	*/
/*  value of the 'after' argument.					*/
/*									*/
/************************************************************************/

int tedDocInsertParagraph(	struct EditDocument *	ed,
				int			after,
				int			traced )
    {
    int				rval= 0;

    struct BufferItem *		paraNode;
    int				textAttributeNr;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpNew;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( after )
	{
	if  ( tedEditStartStep( &teo, EDITcmdINSERT_PARA ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdAPPEND_PARA ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( after )
	{
	paraNode= eo->eoTailDp.dpNode;
	if  ( ! paraNode )
	    { XDEB(paraNode); rval= -1; goto ready;	}

	textAttributeNr= paraNode->biParaParticules[
			paraNode->biParaParticuleCount-1].tpTextAttrNr;
	}
    else{
	paraNode= eo->eoHeadDp.dpNode;
	if  ( ! paraNode )
	    { XDEB(paraNode); rval= -1; goto ready;	}

	textAttributeNr= paraNode->biParaParticules[0].tpTextAttrNr;
	}

    tedEditIncludeNodeInRedraw( &teo, paraNode );

    if  ( docInsertParagraph( eo, &dpNew, paraNode, after, textAttributeNr ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    docSetIBarRange( &(eo->eoSelectedRange), &dpNew );

    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	const SelectionScope * const s0= (const SelectionScope *)0;

	if  ( after )
	    { docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a new section before/after the current one.			*/
/*									*/
/************************************************************************/

int tedEditInsertSection(	DocumentPosition *	dpBeforeSplit,
				DocumentPosition *	dpAfterSplit,
				TedEditOperation *	teo,
				int			split,
				int			after )
    {
    struct BufferItem *		paraNode;
    struct BufferItem *		sectNode;

    int				textAttributeNr;

    EditOperation *		eo= &(teo->teoEo);

    if  ( after )
	{
	paraNode= eo->eoTailDp.dpNode;
	if  ( ! paraNode || paraNode->biTreeType != DOCinBODY )
	    { XDEB(paraNode); return -1;	}

	sectNode= docGetSectNode( paraNode );
	if  ( ! sectNode )
	    { XDEB(sectNode); return -1;	}

	textAttributeNr= paraNode->biParaParticules[
			paraNode->biParaParticuleCount-1].tpTextAttrNr;
	}
    else{
	paraNode= eo->eoHeadDp.dpNode;
	if  ( ! paraNode || paraNode->biTreeType != DOCinBODY )
	    { XDEB(paraNode); return -1;	}

	sectNode= docGetSectNode( paraNode );
	if  ( ! sectNode )
	    { XDEB(sectNode); return -1;	}

	textAttributeNr= paraNode->biParaParticules[0].tpTextAttrNr;
	}

    tedEditIncludeNodeInRedraw( teo, sectNode );

    if  ( docInsertSection( eo, dpBeforeSplit, dpAfterSplit, paraNode,
					split, after, textAttributeNr ) )
	{ LLDEB(split,after); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a new section before/after the current one.			*/
/*									*/
/************************************************************************/

int tedDocInsertSection(	struct EditDocument *	ed,
				int			after,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpBefore;
    DocumentPosition		dpAfter;

    const int			split= 0;
    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( after )
	{
	if  ( tedEditStartStep( &teo, EDITcmdAPPEND_SECT ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdINSERT_SECT ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedEditInsertSection( &dpBefore, &dpAfter, &teo, split, after ) )
	{ LDEB(after); rval= -1; goto ready;	}

    if  ( after )
	{
	docSetIBarRange( &(eo->eoAffectedRange), &dpAfter );
	docSetIBarRange( &(eo->eoSelectedRange), &dpAfter );
	}
    else{
	docSetIBarRange( &(eo->eoAffectedRange), &dpBefore );
	docSetIBarRange( &(eo->eoSelectedRange), &dpBefore );
	}

    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	const SelectionScope * const s0= (const SelectionScope *)0;

	if  ( after )
	    { docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a row close to the current row in a table.			*/
/*									*/
/*  1)  Start edit operation.						*/
/*  2)  Find the position of the beginning/end of the selection in the	*/
/*	table.								*/
/*  3)  Get the row that serves as a template.				*/
/*  4)  Paragraph number of the paragraph that comes directly		*/
/*	below/above the fresh rows.					*/
/*  5)  Finish..							*/
/*									*/
/************************************************************************/

int tedDocAddRowToTable(	struct EditDocument *	ed,
				int			after,
				int			traced )
    {
    int				rval= 0;
    struct BufferItem *		paraNode;
    struct BufferItem *		parentNode;
    struct BufferItem *		refRowNode;
    const struct RowProperties * refRp;

    int				col;
    int				row;
    int				row0;
    int				row1;

    const int			rows= 1;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;
    int				part;
    int				paraNr;
    int				textAttributeNr;

    DocumentSelection		dsRows;

    int				command;

    const int			fullWidth= 1;

    after= ( after != 0 );

    /*  1  */
    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( after )
	{ command= EDITcmdAPPEND_ROW;	}
    else{ command= EDITcmdINSERT_ROW;	}

    if  ( tedEditStartStep( &teo, command ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( after )
	{ paraNode= eo->eoTailDp.dpNode;	}
    else{ paraNode= eo->eoHeadDp.dpNode;	}

    /*  2  */
    if  ( docDelimitTable( paraNode, &parentNode, &col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    refRowNode= parentNode->biChildren[row];
    refRp= refRowNode->biRowProperties;
    if  ( after )
	{
	if  ( docTailPosition( &dpRef, refRowNode ) )
	    { LDEB(row); rval= -1; goto ready;	}
	if  ( docFindParticuleOfPosition( &part, (int *)0,
						    &dpRef, PARAfindFIRST ) )
	    { LDEB(dpRef.dpStroff); rval= -1; goto ready;	}
	}
    else{
	if  ( docHeadPosition( &dpRef, refRowNode ) )
	    { LDEB(row); rval= -1; goto ready;	}
	if  ( docFindParticuleOfPosition( &part, (int *)0,
						    &dpRef, PARAfindLAST ) )
	    { LDEB(dpRef.dpStroff); rval= -1; goto ready;	}
	}

    /*  4  */
    paraNr= docNumberOfParagraph( dpRef.dpNode )+ after;
    textAttributeNr= dpRef.dpNode->biParaParticules[part].tpTextAttrNr;

    /*  5  */
    if  ( docInsertTableRows( &dsRows, eo, parentNode, refRowNode, refRp,
			    textAttributeNr, row+ after, paraNr, rows ) )
	{ LDEB(row+after); rval= -1; goto ready;	}
    
    /*  5  */
    docSetIBarRange( &(eo->eoAffectedRange), &(dsRows.dsHead) );
    docSetIBarRange( &(eo->eoSelectedRange), &(dsRows.dsHead) );
    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	const SelectionScope * const s0= (const SelectionScope *)0;

	if  ( after )
	    { docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

