/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the first note on a page.					*/
/*									*/
/************************************************************************/

DocumentField * docGetFirstNoteFromColumn(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	if  ( dn->dnReferringPage > page )
	    { break;	}

	if  ( dn->dnReferringPage == page )
	    {
	    if  ( dn->dnReferringColumn > column )
		{ break;	}

	    if  ( dn->dnReferringColumn == column )
		{ *pDn= dn; return dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteInColumn(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	struct DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    {
	    if  ( dt->dtColumnFormattedFor > column )
		{ break;	}

	    if  ( dt->dtColumnFormattedFor == column )
		{ *pDn= dn; return dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetLastNoteInColumn(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				column,
				int				treeType )
    {
    DocumentField *	dfRet= (DocumentField *)0;
    DocumentNote *	dnRet= (DocumentNote *)0;

    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	struct DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    {
	    if  ( dt->dtColumnFormattedFor > column )
		{ break;	}

	    if  ( dt->dtColumnFormattedFor == column )
		{ dnRet= dn; dfRet= dfNote;	}
	    }

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    *pDn= dnRet;
    return dfRet;
    }

DocumentField * docGetFirstNoteOnPage(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				page,
				int				treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    while( dfNote )
	{
	struct DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( dt->dtPageFormattedFor > page )
	    { break;	}

	if  ( dt->dtPageFormattedFor == page )
	    { *pDn= dn; return dfNote;	}

	dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	}

    return (DocumentField *)0;
    }

static DocumentField * docGetNextSectionNote(
				DocumentField *			df,
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				sect,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn); 	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    { *pDn= dn; return df;	}
		}
	    }

	df= docGetNextFieldInSection( rootFields, sect, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteOfSection(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				sect,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    if  ( sect < 0 )
	{ return docGetFirstNoteOfDocument(  pDn, bd, treeType ); }

    df= docGetFirstFieldOfSection( rootFields, sect );
    return docGetNextSectionNote( df, pDn, bd, sect, treeType );
    }

DocumentField * docGetNextNoteInSection(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				sect,
				DocumentField *			df,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    if  ( sect < 0 )
	{ return docGetNextNoteInDocument(  pDn, bd, df, treeType ); }

    df= docGetNextFieldInSection( rootFields, sect, df );
    return docGetNextSectionNote( df, pDn, bd, sect, treeType );
    }

static DocumentField * docGetNextDocumentNote(
				DocumentField *			df,
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn);	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    {
		    if  ( pDn )
			{ *pDn= dn;	}
		    return df;
		    }
		}
	    }

	df= docGetNextField( rootFields, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetFirstNoteOfDocument(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    df= docGetFirstField( rootFields );

    return docGetNextDocumentNote( df, pDn, bd, treeType );
    }

DocumentField * docGetNextNoteInDocument(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				DocumentField *			df,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    df= docGetNextField( rootFields, df );

    return docGetNextDocumentNote( df, pDn, bd, treeType );
    }

static DocumentField * docGetPrevDocumentNote(
				DocumentField *			df,
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    while( df )
	{
	if  ( docFieldHasNote( df->dfKind ) )
	    {
	    DocumentNote *	dn= docGetNoteOfField( df, bd );

	    if  ( ! dn )
		{ XDEB(dn); 	}
	    else{
		if  ( treeType < 0					||
		      dn->dnNoteProperties.npTreeType == treeType	)
		    {
		    if  ( pDn )
			{ *pDn= dn;	}
		    return df;
		    }
		}
	    }

	df= docGetPrevField( rootFields, df );
	}

    return (DocumentField *)0;
    }

DocumentField * docGetLastNoteOfDocument(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);
    DocumentField *		df;

    df= docGetLastField( rootFields );

    return docGetPrevDocumentNote( df, pDn, bd, treeType );
    }

DocumentField * docGetPrevNoteInDocument(
				DocumentNote **			pDn,
				const struct BufferDocument *	bd,
				DocumentField *			df,
				int				treeType )
    {
    const ChildFields *		rootFields= &(bd->bdBody.dtRootFields);

    df= docGetPrevField( rootFields, df );

    return docGetPrevDocumentNote( df, pDn, bd, treeType );
    }

