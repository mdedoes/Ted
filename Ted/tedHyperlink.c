/************************************************************************/
/*									*/
/*  Ted: Get/Set/Delete Hyperlinks.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>

#   include	"tedEdit.h"
#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	"tedDocument.h"
#   include	"tedSelect.h"
#   include	<docField.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docEditCommand.h>
#   include	<docFieldKind.h>
#   include	<appEditDocument.h>
#   include	<docFieldInstructions.h>
#   include	<docDocumentFieldList.h>
#   include	<docBuf.h>
#   include	<docFields.h>

#   include	<appDebugon.h>

int tedDocSetHyperlink(	EditDocument *		ed,
			const HyperlinkField *	hf,
			int			traced )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentField *	dfHyperlink;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    textInitTextAttribute( &taSet );
    utilPropMaskClear( &taSetMask );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( docSetHyperlinkField( &fi, hf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dfHyperlink= docFindTypedFieldForPosition( bd, &(ds.dsHead),
							DOCfkHYPERLINK, 0 );
    if  ( ! dfHyperlink )
	{
	if  ( docSetHyperlinkAttribute( &taSet, &taSetMask, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( tedDocSetField( &teo, &ds,
				EDITcmdSET_HYPERLINK, DOCfkHYPERLINK,
				&fi, &taSetMask, &taSet ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedDocUpdField( &teo, dfHyperlink, &fi ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanFieldInstructions( &fi );
    tedCleanEditOperation( &teo );

    return rval;
    }

int tedDocRemoveHyperlink(	EditDocument *		ed,
				int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    PropertyMask		taSetMask;
    TextAttribute		taSet;

    if  ( docRemoveHyperlinkAttribute( &taSet, &taSetMask, bd ) )
	{ LDEB(1); return -1;	}

    return tedDocFlattenTypedField( ed, DOCfkHYPERLINK,
						&taSetMask, &taSet, traced );
    }

int tedDocFindBookmarkField(	struct DocumentField **		pDf,
				EditDocument *			ed,
				const MemoryBuffer *		markName )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    return docFindBookmarkField( pDf, bd, markName );
    }

int tedDocSetBookmark(	EditDocument *		ed,
			const MemoryBuffer *	markName,
			int			traced )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    struct DocumentField *	dfBookmark;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    struct DocumentField *	dfFound;
    FieldInstructions		fi;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fi );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( docSetBookmarkField( &fi, markName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFindBookmarkField( &dfFound, bd, markName ) >= 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    dfBookmark= docFindTypedFieldForPosition( bd, &(ds.dsHead),
							    DOCfkBOOKMARK, 0 );
    if  ( ! dfBookmark )
	{
	if  ( tedDocSetField( &teo, &ds,
		    EDITcmdSET_BOOKMARK, DOCfkBOOKMARK,
		    &fi, (const PropertyMask *)0, (const TextAttribute *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedDocUpdField( &teo, dfBookmark, &fi ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanFieldInstructions( &fi );
    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remove the bookmark that holds the head of the selection.		*/
/*  The contents of the bookmark are retained: Only the field is	*/
/*  removed.								*/
/*									*/
/************************************************************************/

int tedDocRemoveBookmark(	EditDocument *		ed,
				int			traced )
    {
    return tedDocFlattenTypedField( ed, DOCfkBOOKMARK,
		(const PropertyMask *)0, (const TextAttribute *)0, traced );
    }

/************************************************************************/
/*									*/
/*  Select a particular bookmark in a document.				*/
/*									*/
/************************************************************************/

int tedDocGoToBookmark(	EditDocument *		ed,
			const MemoryBuffer *	markName )
    {
    DocumentSelection		dsInside;

    TedDocument *		td;

    const int			lastLine= 0;

    if  ( ! markName )
	{ XDEB(markName); return -1;	}

    td= (TedDocument *)ed->edPrivateData;

    docInitDocumentSelection( &dsInside );

    if  ( docFindBookmarkInDocument( &dsInside, td->tdDocument, markName ) )
	{ LDEB(1); return -1;	}

    tedSetSelection( ed, &dsInside, lastLine, (int *)0, (int *)0 );

    return 0;
    }

