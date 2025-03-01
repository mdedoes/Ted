/************************************************************************/
/*									*/
/*  List related editing and formatting changes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docEditCommand.h>
#   include	<docExpandedTextAttribute.h>
#   include	<docDocumentList.h>
#   include	<appEditDocument.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

int tedDocChangeCurrentList(	EditDocument *		ed,
				const DocumentList *	dlNew,
				int			traced )
    {
    int					rval= 0;

    SelectionGeometry			sg;
    SelectionDescription		sd;

    TedEditOperation			teo;
    EditOperation *			eo= &(teo.teoEo);

    const int				fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartStep( &teo, EDITcmdUPD_LIST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docChangeCurrentList( eo, &sd, dlNew,
				teo.teoEditTrace != (struct EditTrace *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the list font tool was pushed.		*/
/*									*/
/************************************************************************/

int tedDocListFontToolSet(	EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    int				rval= 0;
    int				res;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentList		dlNew;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentList( &dlNew );

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ LDEB(1); rval= -1; goto ready;	}

    res= docUpdateListLevelAttributes( &dlNew, taSetMask, etaSet, &sd, bd );
    if  ( res < 0 )
	{ LDEB(res); rval= -1; goto ready;	}
    if  ( res > 0 )
	{ goto ready;	}

    if  ( tedDocChangeCurrentList( ed, &dlNew, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentList( &dlNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a new list and set it as the list of the current selection.	*/
/*									*/
/************************************************************************/

int tedDocSetNewList(		EditDocument *		ed,
				int			traced )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;
    DocumentSelection		ds;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedEditStartStep( &teo, EDITcmdSET_NEW_LIST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditSetNewList( &(teo.teoRefreshScreenRectangle), eo,
			&ds, &sd, teo.teoEditTrace != (struct EditTrace *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

