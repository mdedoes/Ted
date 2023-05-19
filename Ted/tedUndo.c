/************************************************************************/
/*									*/
/*  Editor Undo functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEditOperation.h"
#   include	"tedSelect.h"
#   include	<tedDocFront.h>
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docTrace.h>
#   include	<docRtfTrace.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docSelect.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docListOverride.h>
#   include	<docEditCommand.h>
#   include	<docRtfTraceImpl.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docEditStep.h>
#   include	<docDocumentProperties.h>
#   include	<appEditDocument.h>
#   include	<docListAdmin.h>
#   include	<docFields.h>
#   include	<docBuf.h>
#   include	<docEditImpl.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	VALIDATE_TREE	0

typedef struct UndoOperation
    {
    TedEditOperation		uoEditOperation;
    SelectionGeometry		uoSelectionGeometry;
    SelectionDescription	uoSelectionDescription;
    const EditStep *		uoEditStep;
    } UndoOperation;

/************************************************************************/

static int tedStartUndoOperation( UndoOperation *	uo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentSelection		ds;
    DocumentSelection		dsGeo;
    const int			lastLine= 0;
    const int			fullWidth= 1;

#   if VALIDATE_TREE
    {
    if  ( docCheckNode( bd->bdBody.dtRoot, bd, 1 ) )
	{ LDEB(2); docListNode( bd, 0, bd->bdBody.dtRoot, 0 ); abort(); }
    }
#   endif

    /**/
    teo->teoEditDocument= ed;
    teo->teoEditTrace= (EditTrace *)0;
    tedSetScreenLayoutContext( &(teo->teoLayoutContext), ed );

    docInitSelectionGeometry( &(uo->uoSelectionGeometry) );
    docInitSelectionDescription( &(uo->uoSelectionDescription) );

    {
    struct BufferItem *	bodySectNode;

    if  ( docGetRootOfSelectionScope( &(eo->eoTree), &bodySectNode,
					    bd, &(es->esNewSelectionScope) ) )
	{ LDEB(1); return -1;	}

    eo->eoBodySectNode= bodySectNode;
    }

    if  ( docSelectionForEditPositionsInTree( &ds, eo->eoTree,
					&(es->esNewEditRange.erHead),
					&(es->esNewEditRange.erTail) ) )
	{ LDEB(1); return -1;	}

    dsGeo= ds;

    if  ( ! utilPropMaskIsEmpty( &(es->esNewStyle.dsCellMask) )		&&
	  ds.dsCol0 >= 0 && ds.dsCol1 >= 0				)
	{
	EditRange	er= es->esNewEditRange;

	docAvoidMergedCellTail( &dsGeo, &er );
	}

    docSelectionGeometry( &(uo->uoSelectionGeometry),
					&dsGeo, eo->eoBodySectNode, lastLine,
					&(teo->teoLayoutContext) );
    docDescribeSelection( &(uo->uoSelectionDescription), &ds, bd,
			uo->uoSelectionGeometry.sgHead.pgTopPosition.lpPage,
			uo->uoSelectionGeometry.sgTail.pgTopPosition.lpPage,
			ed->edDocumentId, ed->edFileReadOnly );

    if  ( docStartEditOperation( eo, &ds, bd, (struct DocumentField *)0 ) )
	{ LDEB(1); return -1;	}

		/*
		 *  Save attributes to have the edit operation in a 
		 *  consistent state. As the undo operations always 
		 *  use an actual document as the source of document 
		 *  content, the values are never used.
		 */
    eo->eoSavedTextAttribute=
			td->tdSelectionDescription.sdTextAttribute;
    eo->eoSavedTextAttributeNumber=
			td->tdSelectionDescription.sdTextAttributeNumber;

    eo->eoIBarSelectionOld= uo->uoSelectionDescription.sdIsIBarSelection;
    eo->eoMultiParagraphSelectionOld=
			! uo->uoSelectionDescription.sdIsSingleParagraph;

    tedIncludeRectangleInChange( teo,
			    &(uo->uoSelectionGeometry.sgRectangle) );
    teo->teoChangedRect.drX1= ed->edFullRect.drX1;

    if  ( fullWidth )
	{ teo->teoChangedRect.drX0= ed->edFullRect.drX0; }

    uo->uoEditStep= es;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish an undo with the old selection.				*/
/*									*/
/*  Also set tyhe fiends that are normally set by the replacement	*/
/*  process.								*/
/*									*/
/************************************************************************/

static int tedUndoFinishOld(	TedEditOperation *	teo,
				const EditStep *	es )
    {
    EditOperation *		eo= &(teo->teoEo);

    struct DocumentTree *		dt;
    DocumentSelection		dsNew;

    if  ( docGetRootOfSelectionScope( &dt, (struct BufferItem **)0,
				eo->eoDocument, &(es->esOldSelectionScope) ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    if  ( docSelectionForEditPositionsInTree( &dsNew, dt,
					    &(es->esOldEditRange.erHead),
					    &(es->esOldEditRange.erTail) ) )
	{ LDEB(1); return -1;	}

    dsNew.dsCol0= es->esOldCol0;
    dsNew.dsCol1= es->esOldCol1;

    tedEditFinishSelection( teo, &dsNew );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo a replacement.							*/
/*									*/
/************************************************************************/

static int tedUndoReplace(	UndoOperation *		uo,
				int			posWhere,
				int			copyTailParaProps )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoReplace( eo, es, copyTailParaProps ) )
	{ LDEB(1); return -1;	}

    switch( posWhere )
	{
	case SELposTAIL:
	    tedEditFinishPosition( teo, &(es->esOldEditRange.erTail) );
	    break;
	case SELposHEAD:
	    tedEditFinishPosition( teo, &(es->esOldEditRange.erHead) );
	    break;
	case SELposALL:
	    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );
	    break;
	default:
	    LDEB(posWhere);
	    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of paragraphs.				*/
/*									*/
/************************************************************************/

static int tedUndoDeletePara(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoDeletePara( eo, es ) )
	{ LDEB(1); return -1;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of columns.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteColumn(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoDeleteColumn( eo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of rows.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteRow(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoDeleteRow( eo, es ) )
	{ LDEB(1); return -1;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of sections.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteSect(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoDeleteSect( eo, es ) )
	{ LDEB(1); return -1;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

    return 0;
    }

static int tedUndoInsertNode(	TedEditOperation *	teo,
				const EditStep *	es,
				struct BufferItem *	node )
    {
    EditOperation *		eo= &(teo->teoEo);

    int				sectionsDeleted= 0;
    int				firstParaDeleted= -1;
    int				paragraphsDeleted= 0;

    eo->eoReformatRange= es->esNewEditRange;

    docEditIncludeNodeInReformatRange( eo, node->biParent );

    if  ( node->biLevel == DOClevSECT					&&
	  node->biTreeType == DOCinBODY					&&
	  node->biNumberInParent == node->biParent->biChildCount- 1	)
	{
	eo->eoSelectionScope.ssSectNr= node->biNumberInParent- 1;
	}

    docEditDeleteNodes( eo, &sectionsDeleted,
			    &firstParaDeleted, &paragraphsDeleted,
			    node->biParent, node->biNumberInParent, 1 );

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int tedUndoInsertPara(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( tedUndoInsertNode( teo, es, eo->eoHeadDp.dpNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int tedUndoInsertSect(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    struct BufferItem *		sectNode;

    sectNode= docGetSectNode( eo->eoHeadDp.dpNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); rval= -1; goto ready;	}

    if  ( tedUndoInsertNode( teo, es, sectNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoInsertRow(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    struct BufferItem *		rowNode;

    rowNode= docGetRowNode( eo->eoHeadDp.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); rval= -1; goto ready;	}

    if  ( tedUndoInsertNode( teo, es, rowNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoInsertColumn(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoInsertColumn( eo, es ) )
	{ return -1;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the header or footer that was inserted.			*/
/*									*/
/*  We know that the insert funtion moved the selection to the fresh	*/
/*  header or footer. So we can remove the currently selected tree.	*/
/*									*/
/************************************************************************/

static int tedUndoInsertHeaderFooter(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    struct DocumentTree * 		dtHdFt= eo->eoTree;

    docMoveEditOperationToBodySect( eo, eo->eoBodySectNode );

    docEraseDocumentTree( eo->eoDocument, dtHdFt );

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a header or a footer.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteHeaderFooter(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoDeleteHeaderFooter( eo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoUpdateField(		UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    const int			lastOne= 0;

    const DocumentField *	dfFrom;
    DocumentField *		dfTo;

    DocumentSelection		dsAround;

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    dfTo= docFindChildField( &(eo->eoTree->dtRootFields),
				    &(es->esNewEditRange.erHead), lastOne );
    if  ( ! dfTo )
	{ XDEB(dfTo); rval= -1; goto ready;	}

    if  ( docCopyFieldProperties( dfTo, dfFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dfTo->dfKind == DOCfkTOC			&&
	  tedRefreshTocField( &dsAround, teo, dfTo )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the creation of a field.					*/
/*									*/
/*  Just remove the field: More advanced scenarios are handled as a	*/
/*  replace action.							*/
/*									*/
/************************************************************************/

static int tedUndoSetField(		UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    DocumentField *		df;

    const int			lastOne= 0;

    DocumentSelection		dsInsideField;
    DocumentSelection		dsAroundField;
    int				headPart;
    int				tailPart;

    df= docFindChildField( &(eo->eoTree->dtRootFields),
				    &(es->esNewEditRange.erHead), lastOne );
    if  ( ! df )
	{ XDEB(df); rval= -1; goto ready;	}

    if  ( docDelimitFieldInDoc( &dsInsideField, &dsAroundField,
				&headPart, &tailPart, eo->eoDocument, df ) )
	{ SDEB(docFieldKindStr(df->dfKind)); rval= -1; goto ready; }

    if  ( tedFlattenFieldImpl( teo, (DocumentSelection *)0,
				    &dsAroundField, headPart, tailPart, df ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoDelField(		UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    const DocumentField *	dfFrom;
    DocumentField *		dfTo;

    DocumentSelection		dsInside;
    DocumentSelection		dsAround;
    int				headPart;
    int				tailPart;

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    if  ( docEditSurroundTextSelectionByField( eo, &dfTo,
			    &dsInside, &dsAround, &headPart, &tailPart,
			    (PropertyMask *)0, (TextAttribute *)0,
			    dfFrom->dfKind, &(dfFrom->dfInstructions) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a property update.						*/
/*									*/
/************************************************************************/

static int tedUndoUpdProps(	UndoOperation *		uo )
    {
    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoUpdProps( eo, es ) )
	{ LDEB(1); return -1;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo a shift-indent.						*/
/*									*/
/************************************************************************/

static int tedUndoShiftIndent(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    if  ( docUndoShiftIndent( eo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a object/picture property update.				*/
/*									*/
/************************************************************************/

static int tedUndoUpdObject(	UndoOperation *		uo )
    {
    const struct PictureProperties *	pipFrom;

    int					partObjTo;
    DocumentPosition			dpObjTo;
    const struct PositionGeometry *	pgObjTo;
    struct InsertedObject *		ioTo;

    int					rval= 0;

    TedEditOperation *			teo= &(uo->uoEditOperation);
    const EditStep *			es= uo->uoEditStep;

    pipFrom= docTraceGetFromPictureProps( es );
    if  ( ! pipFrom )
	{ XDEB(pipFrom); rval= -1; goto ready;	}

    if  ( tedGetObjectSelection( &pgObjTo, &partObjTo, &dpObjTo, &ioTo,
							teo->teoEditDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedObjectSetImagePropertiesImpl( teo, ioTo, &dpObjTo, partObjTo,
					&(es->esPicturePropMask), pipFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a foot/end note property update.				*/
/*									*/
/************************************************************************/

static int tedUndoUpdNote(	UndoOperation *		uo )
    {
    int				rval= 0;

    TedEditOperation *		teo= &(uo->uoEditOperation);
    EditOperation *		eo= &(teo->teoEo);
    const EditStep *		es= uo->uoEditStep;

    int				selInNote= 0;

    const DocumentField *	dfFrom;
    const DocumentNote *	dnFrom= (DocumentNote *)0;
    DocumentField *		dfTo= (DocumentField *)0;
    DocumentNote *		dnTo= (DocumentNote *)0;

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}
    dnFrom= docGetNoteOfField( dfFrom, es->esSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); rval= -1; goto ready;	}

    dfTo= docEditOperationGetSelectedNote( &dnTo, &selInNote, eo );
    if  ( ! dfTo )
	{ XDEB(dfTo); rval= -1; goto ready;	}

    if  ( docEditChangeNote( eo, dfTo, dnTo, selInNote,
						&(es->esNotePropMask),
						&(dnFrom->dnNoteProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a change to a list.						*/
/*									*/
/************************************************************************/

static int tedUndoUpdList(	UndoOperation *		uo )
    {
    int					rval= 0;

    struct ListOverride *		lo;
    DocumentList *			dl;

    TedEditOperation *			teo= &(uo->uoEditOperation);
    EditOperation *			eo= &(teo->teoEo);
    const EditStep *			es= uo->uoEditStep;
    const SelectionDescription *	sd= &(uo->uoSelectionDescription);

    if  ( ! sd->sdHasLists		||
	  sd->sdListOverride < 1	)
	{ LLDEB(sd->sdHasLists,sd->sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl,
				    sd->sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( docEditChangeList( eo, dl, lo, &(es->esDocumentList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the creation of a new list.					*/
/*									*/
/************************************************************************/

static int tedUndoSetNewList(	UndoOperation *		uo )
    {
    int					rval= 0;

    struct ListOverride *		lo;
    DocumentList *			dl;

    TedEditOperation *			teo= &(uo->uoEditOperation);
    EditOperation *			eo= &(teo->teoEo);
    const EditStep *			es= uo->uoEditStep;
    const SelectionDescription *	sd= &(uo->uoSelectionDescription);

    if  ( ! sd->sdHasLists	||
	  sd->sdListOverride < 1	)
	{ LLDEB(sd->sdHasLists,sd->sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl,
				    sd->sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( docUndoUpdProps( eo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    {
    ListAdmin *		la= eo->eoDocument->bdProperties->dpListAdmin;

    if  ( docDocumentListTableDeleteList( &(la->laListTable),
							    lo->loListIndex ) )
	{ LDEB(lo->loListIndex); rval= -1;	}

    if  ( docListOverrideTableDeleteOverride( &(la->laListOverrideTable),
							    lo->loIndex ) )
	{ LDEB(lo->loIndex); rval= -1;	}
    }

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo roll table rows.						*/
/*									*/
/************************************************************************/

static int tedUndoShiftRows(	UndoOperation *		uo )
    {
    int					rval= 0;

    TedEditOperation *			teo= &(uo->uoEditOperation);
    const EditStep *			es= uo->uoEditStep;
    const SelectionDescription *	sd= &(uo->uoSelectionDescription);

    const int				move= 1;
    int					rowsdown= 0;

    /*  It is an undo: swap rows down */
    if  ( es->esCommand == EDITcmdSHIFT_ROWS_DOWN )
	{ rowsdown= -1;	}
    else{ rowsdown=  1;	}

    if  ( tedDocRollRowsInTableImpl( teo, &(sd->sdTableRectangle),
							    move, rowsdown ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo.								*/
/*									*/
/************************************************************************/

void tedDocUndo(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;
    EditTrace *			et= &(td->tdEditTrace);

    UndoOperation		uo;
    EditStep			es;
    const int			direction= -1;

    docInitEditStep( &es );
    tedInitEditOperation( &(uo.uoEditOperation) );

    if  ( docEditReadTraceStep( &es, (int *)0, direction, et, bd ) )
	{ LDEB(1); goto ready;	}

    tedStartUndoOperation( &uo, ed, &es );

    switch( es.esCommand )
	{
	case EDITcmdMERGE_PARAS:
	case EDITcmdREPLACE:
	case EDITcmdREPLACE_FTNSEP:
	case EDITcmdREPLACE_BODY_LEVEL:
	case EDITcmdDELETE_SELECTION:
	case EDITcmdSPLIT_SECT:
	case EDITcmdINS_NOTE:
	case EDITcmdINSERT_TABLE:
	    if  ( tedUndoReplace( &uo, SELposALL, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdDELETE_SELECTION_DEL:
	    if  ( tedUndoReplace( &uo, SELposHEAD, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdDELETE_SELECTION_BS:
	    if  ( tedUndoReplace( &uo, SELposTAIL, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;


	case EDITcmdDELETE_PARA:
	    if  ( tedUndoDeletePara( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_COLUMN:
	    if  ( tedUndoDeleteColumn( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_ROW:
	case EDITcmdDELETE_TABLE:
	    if  ( tedUndoDeleteRow( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_SECT:
	    if  ( tedUndoDeleteSect( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_HEADER:
	case EDITcmdDELETE_FOOTER:
	    if  ( tedUndoDeleteHeaderFooter( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdINSERT_PARA:
	case EDITcmdAPPEND_PARA:
	    if  ( tedUndoInsertPara( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_COLUMN:
	case EDITcmdAPPEND_COLUMN:
	    if  ( tedUndoInsertColumn( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_ROW:
	case EDITcmdAPPEND_ROW:
	    if  ( tedUndoInsertRow( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_SECT:
	case EDITcmdAPPEND_SECT:
	    if  ( tedUndoInsertSect( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_HEADER:
	case EDITcmdINSERT_FOOTER:
	    if  ( tedUndoInsertHeaderFooter( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_FIELD:
	    if  ( tedUndoUpdateField( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSET_HYPERLINK:
	case EDITcmdSET_BOOKMARK:
	    if  ( utilPropMaskIsEmpty( &(es.esNewStyle.dsTextMask) ) )
		{
		if  ( tedUndoSetField( &uo ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    else{
		if  ( tedUndoReplace( &uo, SELposALL, 0 ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    break;

	case EDITcmdDEL_FIELD:
	    if  ( utilPropMaskIsEmpty( &(es.esNewStyle.dsTextMask) ) )
		{
		if  ( tedUndoDelField( &uo ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    else{
		if  ( tedUndoReplace( &uo, SELposALL, 0 ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    break;

	case EDITcmdUPD_SPAN_PROPS:
	case EDITcmdUPD_PARA_PROPS:
	case EDITcmdUPD_TABLE_PROPS:
	case EDITcmdUPD_ROW_PROPS:
	case EDITcmdUPD_COLUMN_PROPS:
	case EDITcmdUPD_CELL_PROPS:
	case EDITcmdUPD_SECT_PROPS:
	case EDITcmdUPD_SECTDOC_PROPS:
	case EDITcmdUPD_DOC_PROPS:
	    if  ( tedUndoUpdProps( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_OBJECT:
	    if  ( tedUndoUpdObject( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdUPD_NOTE:
	    if  ( tedUndoUpdNote( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_LIST:
	    if  ( tedUndoUpdList( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdSET_NEW_LIST:
	    if  ( tedUndoSetNewList( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSHIFT_ROWS_DOWN:
	case EDITcmdSHIFT_ROWS_UP:
	    if  ( tedUndoShiftRows( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSHIFT_RIGHT:
	case EDITcmdSHIFT_LEFT:
	    if  ( tedUndoShiftIndent( &uo ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmd_OPEN:
	case EDITcmd_SAVE:
	case EDITcmd_NEW:
	default:
	    LSDEB(es.esCommand,docRtfGetTraceTag(es.esCommand));
	    goto ready;
	}

    et->etIndex= es.esTraceIndex;

    tedFinishEditOperation( &(uo.uoEditOperation) );

  ready:

    docCleanEditStep( &es );
    tedCleanEditOperation( &(uo.uoEditOperation) );

    return;
    }

