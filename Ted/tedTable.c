/************************************************************************/
/*									*/
/*  Ted: Table manipulation.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	"tedDocument.h"
#   include	<docIntermediaryDocument.h>
#   include	<docRtfTrace.h>
#   include	<docParaParticules.h>
#   include	<docEditCommand.h>
#   include	<docDocumentCopyJob.h>
#   include	<appEditDocument.h>
#   include	<docBuf.h>
#   include	<docNodeTree.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a table into the document.					*/
/*									*/
/*  0)  Also expose the table borders.					*/
/*  1)  If a range of data was selected, first discard it.		*/
/*  2)  If the selection was at the end of a paragraph, try to move to	*/
/*	the beginning of the next one.					*/
/*  3)  If the IBar is in the middle of a paragraph, split it.		*/
/*	tedSplitParaNode() forces the layout of both halves.		*/
/*  4)  If the paragraph is not the first one in its parent, split the	*/
/*	the parent. This is part of a legacy decision to have some	*/
/*	kind of fake rows and cells even outside a table.		*/
/*  5)  If the 'cell' is not the first one in the 'row' that contains	*/
/*	it, split the row. (Legacy again).				*/
/*									*/
/************************************************************************/

int tedInsertTable(		EditDocument *		ed,
				int			rows,
				int			columns,
				int			traced )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    struct BufferDocument *	bdTable= (struct BufferDocument *)0;
    struct BufferItem *		sectNodeTable;
    int				textAttrNr;
    int				wide;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentCopyJob		dcj;

    int				posWhere= SELposHEAD;

    const int			fullWidth= 1;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( ( sd.sdHeadFlags & POSflagPARA_HEAD )	&&
	  ( sd.sdPastFlags & POSflagPARA_TAIL )	&&
	  ! docNextPosition( &(eo->eoTailDp)  )	)
	{
	eo->eoLastDp= eo->eoTailDp;
	docSetEditPosition( &(eo->eoReformatRange.erTail), &(eo->eoTailDp) );
	eo->eoSelectedRange.erTail= eo->eoReformatRange.erTail;
	eo->eoAffectedRange.erTail= eo->eoReformatRange.erTail;
	}

    if  ( sd.sdHeadFlags & POSflagPARA_HEAD )
	{ posWhere= SELposHEAD;	}
    else{ posWhere= SELposNEXT;	}

    bdTable= docIntermediaryDocument( &sectNodeTable, eo->eoDocument );
    if  ( ! bdTable )
	{ XDEB(bdTable); rval= -1; goto ready;	}

    textAttrNr= docMapTextAttributeNumberFromTo( bdTable, eo->eoDocument,
			    td->tdSelectionDescription.sdTextAttributeNumber );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); rval= -1; goto ready;	}

    wide= sg.sgHead.pgParentFrameX1- sg.sgHead.pgParentFrameX0;

    if  ( docFillTableDocument( bdTable, sectNodeTable, textAttrNr,
						    wide, rows, columns ) )
	{ LLDEB(rows,columns); rval= -1; goto ready;	}

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdTable, &(bdTable->bdBody),
						    &(ed->edFilename), -1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  0  */
    teo.teoChangedRect.drY0--;

    if  ( tedEditIncludeDocument( &teo, &dcj,
					EDITcmdINSERT_TABLE, posWhere ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    tedCleanEditOperation( &teo );

    if  ( bdTable )
	{ docFreeIntermediaryDocument( bdTable );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Roll rows in a table.						*/
/*									*/
/************************************************************************/

int tedDocRollRowsInTableImpl(	TedEditOperation *		teo,
				const TableRectangle  *		tr,
				int				move,
				int				rowsdown )
    {
    EditOperation *		eo= &(teo->teoEo);

    struct BufferItem *		parentNode;

    int				row0= tr->trRow0;
    int				row1= tr->trRow1;

    if  ( move )
	{
	if  ( rowsdown < 0 )
	    { row0 += rowsdown;	}
	if  ( rowsdown > 0 )
	    { row1 += rowsdown;	}
	}

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
				(int *)0, (int *)0, (int *)0, (int *)0 ) )
	{ LDEB(1); return -1;	}

    tedEditIncludeRowsInRedraw( teo, parentNode, row0, row1 );

    if  ( docDocRollRowsInTable( eo, tr, parentNode, row0, row1, rowsdown ) )
	{ LLDEB(move,rowsdown); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static int tedSplitColumnInRows(	TedEditOperation *	teo,
					DocumentPosition *	dpNew,
					struct BufferItem *	parentNode,
					int			row0,
					int			row,
					int			row1,
					int			col,
					int			after )
    {
    EditOperation *		eo= &(teo->teoEo);
    struct BufferItem *		newParaNode= (struct BufferItem *)0;
    const int			recursively= 0;

    tedEditIncludeRowsInRedraw( teo, parentNode, row0, row1 );

    if  ( docSplitColumnInRows( &newParaNode, eo,
				    parentNode, row0, row, row1, col, after ) )
	{ LDEB(1); return -1;	}

    docDelimitTables( parentNode, recursively );

    if  ( docHeadPosition( dpNew, newParaNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int tedDocAddColumnToTable(	EditDocument *		ed,
				int			after,
				int			traced )
    {
    int				rval= 0;
    struct BufferItem *		parentNode;

    int				col;
    int				row;
    int				row0;
    int				row1;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpNew;

    const int			fullWidth= 1;

    int				editCmd;
    int				selPos;

    if  ( after )
	{ editCmd= EDITcmdAPPEND_COLUMN; selPos= SELposAFTER;	}
    else{ editCmd= EDITcmdINSERT_COLUMN; selPos= SELposBEFORE;	}

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartStep( &teo, editCmd ) )
	{ LDEB(editCmd); rval= -1; goto ready;	}

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedSplitColumnInRows( &teo, &dpNew,
				    parentNode, row0, row, row1, col, after ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{ docRtfTraceNewPosition( eo, (const SelectionScope *)0, selPos ); }

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

