/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docNodeTree.h"
#   include	"docField.h"
#   include	"docParaString.h"
#   include	<docTreeType.h>
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<textAttribute.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<utilMemoryBufferPrintf.h>
#   include	"docAttributes.h"
#   include	"docFields.h"
#   include	<docNotesProperties.h>
#   include	<docDocumentProperties.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Manage Footnotes/Endnotes.						*/
/*									*/
/************************************************************************/

void docInitNote(		DocumentNote *		dn )
    {
    docInitDocumentTree( &(dn->dnDocumentTree) );

    dn->dnNoteNumber= -1;
    dn->dnReferringPage= -1;
    dn->dnReferringColumn= -1;

    docInitNoteProperties( &(dn->dnNoteProperties) );

    return;
    }

void docCleanNote(		struct BufferDocument *	bd,
				DocumentNote *		dn )
    {
    docCleanDocumentTree( bd, &(dn->dnDocumentTree) );
    docCleanNoteProperties( &(dn->dnNoteProperties) );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a Footnote/Endnote in a struct BufferItem.				*/
/*									*/
/*  1)  Look for a note or the place to insert a new one.		*/
/*	NOTE that positions with a negative paragraph number are always	*/
/*	skipped.							*/
/*	NOTE that when a position for a new note is found, its index	*/
/*	is that of its successor, that usually will be shifted away.	*/
/*  2)  When the previous position is an empty hole, use it in stead	*/
/*	of shifting the successors away.				*/
/*  3)  Make a guess about the note number. If the guess is wrong, it	*/
/*	fixed later. Fixing can involve a reformat, so making not too	*/
/*	bad a guess here does help.					*/
/*									*/
/************************************************************************/

static void docSetNote(		DocumentNote *		fresh,
				int			autoNumber )
    {
    docInitNote( fresh );

    /*  3  */
    if  ( autoNumber )
	{ fresh->dnNoteNumber= autoNumber;	}
    else{ fresh->dnNoteNumber= 0;		}

    fresh->dnNoteProperties.npAutoNumber= autoNumber;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a note or an annotation.					*/
/*									*/
/*  1)  More than one annotation can be attached to the same location.	*/
/*									*/
/************************************************************************/

int docInsertNote(			DocumentNote **		pDn,
					struct BufferDocument *	bd,
					DocumentField *		dfNote,
					int			autoNumber )
    {
    NotesList *		nl= &(bd->bdNotesList);
    DocumentNote **	fresh;

    int			i;

    fresh= (DocumentNote **)realloc( nl->nlNotes,
			    ( nl->nlNoteCount+ 1 )* sizeof(DocumentNote *) );
    if  ( ! fresh )
	{ LXDEB(nl->nlNoteCount,fresh); return -1;	}
    nl->nlNotes= fresh;

    for ( i= 0; i < nl->nlNoteCount; i++ )
	{
	if  ( ! fresh[i] )
	    { break;	}
	}

    fresh[i]= malloc( sizeof(DocumentNote) );
    if  ( ! fresh[i] )
	{ XDEB(fresh[i]); return -1;	}

    docSetNote( fresh[i], autoNumber );
    
    if  ( i >= nl->nlNoteCount )
	{ nl->nlNoteCount++;	}

    dfNote->dfNoteIndex= i;
    *pDn= fresh[i]; return i;
    }

void docDeleteNote(		struct BufferDocument *	bd,
				int			noteIndex )
    {
    if  ( bd->bdNotesList.nlNotes[noteIndex] )
	{
	docCleanNote( bd, bd->bdNotesList.nlNotes[noteIndex] );
	free( bd->bdNotesList.nlNotes[noteIndex] );
	bd->bdNotesList.nlNotes[noteIndex]= (DocumentNote *)0;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the note corresponding to a selection.				*/
/*  Always return the field that owns the note.				*/
/*									*/
/************************************************************************/

DocumentField * docGetSelectedNote(
				DocumentNote **			pDn,
				int *				pSelInNote,
				struct BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    DocumentField *		dfNote= (DocumentField *)0;
    DocumentNote *		dn= (DocumentNote *)0;
    int				selInNote= 0;

    /*  2  */
    if  ( ds->dsSelectionScope.ssTreeType == DOCinFOOTNOTE	||
	  ds->dsSelectionScope.ssTreeType == DOCinENDNOTE	)
	{
	struct BufferItem *	node= ds->dsHead.dpNode;
	int			ownerNumber;

	node= docGetSectNode( ds->dsHead.dpNode );
	if  ( ! node )
	    { XDEB(node); return (DocumentField *)0;		}

	ownerNumber= node->biSectSelectionScope.ssOwnerNumber;
	dfNote= docGetFieldByNumber( bd, ownerNumber );
	dn= docGetNoteOfField( dfNote, bd );
	if  ( ! dn )
	    { XDEB(dn); return (DocumentField *)0;	}

	selInNote= 1;
	}

    if  ( ds->dsSelectionScope.ssTreeType == DOCinBODY )
	{
	const int	lastOne= 0;

	if  ( ! docIsParaSelection( ds ) )
	    { return (DocumentField *)0;	}

	dfNote= docFindTypedFieldForPosition( bd, &(ds->dsHead),
							DOCfkCHFTN, lastOne );
	if  ( ! dfNote && docIsIBarSelection( ds ) )
	    {
	    DocumentPosition	dpPrev= ds->dsHead;

	    if  ( ! docPrevPosition( &dpPrev )		&&
		  dpPrev.dpNode == ds->dsHead.dpNode	)
		{
		dfNote= docFindTypedFieldForPosition( bd, &dpPrev,
							DOCfkCHFTN, lastOne );
		}
	    }

	if  ( ! dfNote )
	    { return (DocumentField *)0;	}
	else{
	    dn= docGetNoteOfField( dfNote, bd );
	    if  ( ! dn )
		{ XDEB(dn); return (DocumentField *)0;	}
	    }

	selInNote= 0;
	}

    if  ( ! dn || ! dfNote )
	{ /* XXDEB(dn,dfNote); */ return (DocumentField *)0;	}

    if  ( pDn )
	{ *pDn= dn;			}
    if  ( pSelInNote )
	{ *pSelInNote= selInNote;	}

    return dfNote;
    }

/************************************************************************/

int docCheckSeparatorExistenceForNoteType(
					struct BufferDocument *	bd,
					int			noteTreeType )
    {
    switch( noteTreeType )
	{
	case DOCinFOOTNOTE:
	    if  ( docCheckNoteSeparatorExistence( bd, DOCinFTNSEP ) )
		{ LDEB(noteTreeType); return -1; }
	    break;

	case DOCinENDNOTE:
	    if  ( docCheckNoteSeparatorExistence( bd, DOCinAFTNSEP ) )
		{ LDEB(noteTreeType); return -1; }
	    break;

	default:
	    LDEB(noteTreeType); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return a particular separator for an annotation by scope.		*/
/*									*/
/************************************************************************/

struct DocumentTree *	docDocumentNoteSeparator(
					BufferDocument *	bd,
					int			treeType )
    {
    switch( treeType )
	{
	case DOCinFTNSEP:
	    return &(bd->bdFtnsep);

	case DOCinFTNSEPC:
	    return &(bd->bdFtnsepc);

	case DOCinFTNCN:
	    return &(bd->bdFtncn);

	case DOCinAFTNSEP:
	    return &(bd->bdAftnsep);

	case DOCinAFTNSEPC:
	    return &(bd->bdAftnsepc);

	case DOCinAFTNCN:
	    return &(bd->bdAftncn);

	default:
	    LDEB(treeType); return (struct DocumentTree *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Verify that a certain kind of notes separator exists. Make it if	*/
/*  not.								*/
/*									*/
/*  1)  Correctly handeled by docTextAttributeNumber().			*/
/*									*/
/************************************************************************/

int docCheckNoteSeparatorExistence(
				struct BufferDocument *	bd,
				int			sepTreeType )
    {
    struct DocumentTree *	dt;
    int			particuleKind;

    switch( sepTreeType )
	{
	case DOCinFTNSEP:
	case DOCinAFTNSEP:
	    particuleKind= TPkindCHFTNSEP;
	    break;

	case DOCinFTNSEPC:
	case DOCinAFTNSEPC:
	    particuleKind= TPkindCHFTNSEPC;
	    break;

	default:
	    LDEB(sepTreeType);
	    particuleKind= TPkindCHFTNSEP;
	    break;
	}

    dt= docDocumentNoteSeparator( bd, sepTreeType );
    if  ( ! dt )
	{ LXDEB(sepTreeType,dt); return -1;	}

    if  ( ! dt->dtRoot )
	{
	const int		ownerNumber= -1;
	int			txtAttrNr;

	struct BufferItem *		paraNode;
	TextAttribute		ta;

	textInitTextAttribute( &ta );
	ta.taFontNumber= -1; /*  1  */
	ta.taFontSizeHalfPoints= 24;

	txtAttrNr= docTextAttributeNumber( bd, &ta );
	if  ( txtAttrNr < 0 )
	    { LDEB(txtAttrNr); return -1;	}

	paraNode= docStartDocumentTree( bd, dt, sepTreeType,
				bd->bdBody.dtRoot, ownerNumber, txtAttrNr );
	if  ( ! paraNode )
	    { XDEB(paraNode);	}
	else{
	    int		stroffShift= 0;
	    int		off= docParaStrlen( paraNode );

	    if  ( docParaStringReplace( &stroffShift, paraNode,
							off, off, "-", 1 ) )
		{ LDEB(1);	}
	    else{
		TextParticule *	tp= paraNode->biParaParticules;

		tp->tpKind= particuleKind;
		tp->tpStrlen= 1;
		}
	    }
	}

    return 0;
    }

/************************************************************************/

DocumentNote *	docGetNoteOfField(	const DocumentField *	dfNote,
					const struct BufferDocument *	bd )
    {
    const NotesList *	nl= &(bd->bdNotesList);

    if  ( ! docFieldHasNote( dfNote->dfKind ) )
	{
	LSDEB(dfNote->dfKind,docFieldKindStr(dfNote->dfKind));
	return (DocumentNote *)0;
	}

    if  ( dfNote->dfSelectionScope.ssTreeType == DOCinBODY )
	{
	if  ( dfNote->dfNoteIndex < 0			||
	      dfNote->dfNoteIndex >= nl->nlNoteCount	)
	    {
	    LLLDEB(dfNote->dfFieldNumber,dfNote->dfNoteIndex,nl->nlNoteCount);
	    return (DocumentNote *)0;
	    }

	return nl->nlNotes[dfNote->dfNoteIndex];
	}

    if  ( dfNote->dfSelectionScope.ssTreeType == DOCinFOOTNOTE		||
	  dfNote->dfSelectionScope.ssTreeType == DOCinENDNOTE		||
	  dfNote->dfSelectionScope.ssTreeType == DOCinANNOTATION	)
	{
	DocumentField *	dfOwner;

	dfOwner= docGetFieldByNumber( bd,
				dfNote->dfSelectionScope.ssOwnerNumber );

	if  ( dfOwner->dfNoteIndex < 0			||
	      dfOwner->dfNoteIndex >= nl->nlNoteCount	)
	    {
	    LLLDEB(dfOwner->dfFieldNumber,dfOwner->dfNoteIndex,nl->nlNoteCount);
	    return (DocumentNote *)0;
	    }

	return nl->nlNotes[dfOwner->dfNoteIndex];
	}

    LDEB(dfNote->dfSelectionScope.ssTreeType);
    return (DocumentNote *)0;
    }

int docSetNoteLinks(	MemoryBuffer *			mbTarget,
			MemoryBuffer *			mbSource,
			BufferItem *			paraNode,
			const DocumentField *		dfChftn )
    {
    const char *	formatRef= "_NREF_%d";
    const char *	formatDef= "_NDEF_%d";

    if  ( paraNode->biTreeType != DOCinBODY )
	{
	struct BufferItem *	rootNode= docGetSectNode( paraNode );

	if  ( ! rootNode )
	    { XDEB(rootNode); return -1;	}
	else{
	    int	fieldNumber= rootNode->biSectSelectionScope.ssOwnerNumber;

	    utilMemoryBufferPrintf( mbTarget, formatRef, fieldNumber+ 1 );
	    utilMemoryBufferPrintf( mbSource, formatDef, fieldNumber+ 1 );

	    return 0;
	    }
	}
    else{
	int	fieldNumber= dfChftn->dfFieldNumber;

	utilMemoryBufferPrintf( mbTarget, formatDef, fieldNumber+ 1 );
	utilMemoryBufferPrintf( mbSource, formatRef, fieldNumber+ 1 );

	return 0;
	}
    }

int docCollectNoteTitle(	MemoryBuffer *		mbTitle,
				const DocumentNote *	dn,
				struct BufferDocument *	bd )
    {
    DocumentPosition	dpLast;
    DocumentSelection	dsTail;

    if  ( docTailPosition( &dpLast, dn->dnDocumentTree.dtRoot ) )
	{ LDEB(1); return -1;	}
    else{
	int	paraNr= docNumberOfParagraph( dpLast.dpNode );

	if  ( paraNr == 1 )
	    {
	    dpLast.dpStroff= 0;
	    docAvoidParaHeadField( &dpLast, (int *)0, bd );

	    docSetParaSelection( &dsTail,
		    dpLast.dpNode, 1,
		    dpLast.dpStroff,
		    docParaStrlen( dpLast.dpNode )- dpLast.dpStroff );

	    if  ( docCollectReference( mbTitle, &dsTail, bd ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int docGetNotePosition(		DocumentNote *		dn,
				BufferDocument *	bd )
    {
    switch( dn->dnNoteProperties.npTreeType )
	{
	case DOCinFOOTNOTE:
	    return FTNplacePAGE_END;

	case DOCinENDNOTE:
	    return bd->bdProperties->dpNotesProps.fepEndnotesProps.npPlacement;

	default:
	    LDEB(dn->dnNoteProperties.npTreeType); return -1;
	}

    return -1;
    }

int docGetEndnotePlacement(	BufferDocument *	bd )
    {
    return bd->bdProperties->dpNotesProps.fepEndnotesProps.npPlacement;
    }
