/************************************************************************/
/*									*/
/*  Editor Undo/Redo functionality.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docCopyNode.h"
#   include	"docEditStep.h"
#   include	"docTrace.h"
#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	<docDocumentField.h>
#   include	<docPictureProperties.h>
#   include	<docObject.h>
#   include	"docDocumentCopyJob.h"
#   include	"docEditOperation.h"
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reinsert nodes as part of an undo operation.			*/
/*									*/
/************************************************************************/

int docReinsertNodes(	EditOperation *		eo,
			struct BufferItem *		parentTo,
			const struct BufferItem *	parentFrom,
			int			to,
			const EditStep *	es )
    {
    int				rval= 0;
    DocumentCopyJob		dcj;
    const int			from= 0;

    /*  Deletion was at the tail of the parent? */
    switch( es->esSelectionPosition )
	{
	case SELposAFTER:
	    break;
	case SELposBEFORE:
	    to++;
	    break;
	case SELposTAIL: /* was replace whole document */
	    break;
	default:
	    LDEB(es->esSelectionPosition);
	    break;
	}

    docInitDocumentCopyJob( &dcj );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docGetSelectionScope( &(dcj.dcjTargetSelectionScope), parentTo );
    if  ( docGetRootOfSelectionScope( &(dcj.dcjTargetTree), (struct BufferItem **)0,
			    eo->eoDocument, &(dcj.dcjTargetSelectionScope) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docCopyNodeChildren( &dcj, parentTo, to,
				parentFrom, from,  parentFrom->biChildCount ) )
	{ LLDEB(from,to); rval= -1; goto ready;	}

    to +=  parentFrom->biChildCount;

    /* remove the empty child that was added to prevent an empty parent */
    if  ( es->esSelectionPosition == SELposTAIL )
	{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	docEditDeleteNodes( eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentTo, to, 1 );
	}

    docEditIncludeNodeInReformatRange( eo, parentTo );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the field instructions to apply.				*/
/*									*/
/************************************************************************/

const struct DocumentField * docTraceGetFromField( const EditStep * 	es )
    {
    const int				lastOne= 0;

    DocumentPosition			dpFrom;
    EditPosition			epFrom;
    const struct DocumentField *	dfFrom;

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return (const struct DocumentField *)0;	}
    docSetEditPosition( &epFrom, &dpFrom );

    dfFrom= docFindChildField( &(es->esSourceDocument->bdBody.dtRootFields),
							&epFrom, lastOne );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); return (const struct DocumentField *)0;	}

    return dfFrom;
    }

/************************************************************************/
/*									*/
/*  Get the picture properties to apply.				*/
/*									*/
/************************************************************************/

const PictureProperties * docTraceGetFromPictureProps( const EditStep *	es )
    {
    DocumentSelection		dsObjFrom;
    int				partObjFrom;
    DocumentPosition		dpObjFrom;
    InsertedObject *		ioFrom;

    if  ( docHeadPosition( &(dsObjFrom.dsHead),
					es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return (const PictureProperties *)0;	}
    dsObjFrom.dsAnchor= dsObjFrom.dsHead;
    dsObjFrom.dsTail= dsObjFrom.dsHead;
    if  ( docNextPosition( &dsObjFrom.dsTail ) )
	{ LDEB(1); return (const PictureProperties *)0;	}

    if  ( docGetObjectSelection( &partObjFrom, &dpObjFrom, &ioFrom,
					es->esSourceDocument, &dsObjFrom ) )
	{ LDEB(1); return (const PictureProperties *)0;	}

    return &(ioFrom->ioPictureProperties);
    }

