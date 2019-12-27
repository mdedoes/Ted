/************************************************************************/
/*									*/
/*  Editor Undo/Redo functionality.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docEditStep.h"
#   include	"docTrace.h"
#   include	<docDocumentField.h>
#   include	<docPictureProperties.h>
#   include	<docObject.h>
#   include	"docEditOperation.h"
#   include	<docBuf.h>

#   include	<appDebugon.h>

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

