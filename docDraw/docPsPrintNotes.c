/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<docSelect.h>
#   include	<psPrint.h>
#   include	<docFields.h>
#   include	<docTreeType.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static int docPsPrintNoteLinkTitle(	MemoryBuffer *		mbTitle,
					const DrawTextLine *	dtl,
					const DocumentField *	df )
    {
    struct BufferDocument *	bd= dtl->dtlDocument;
    BufferItem *		paraNode= dtl->dtlParaNode;
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    struct DocumentNote *	dn= docGetNoteOfField( df, bd );

    if  ( paraNode->biTreeType == DOCinBODY )
	{
	if  ( dn )
	    {
	    if  ( docCollectNoteTitle( mbTitle, dn, bd ) )
		{ LDEB(1); return -1;	}
	    }
	}
    else{
	DocumentSelection	dsInside;

	if  ( ! docDelimitFieldInTree( &dsInside, (DocumentSelection *)0,
					(int *)0, (int *)0, dtl->dtlTree, df ) )
	    {
	    if  ( docCollectReference( &(ps->psLinkTitle), &dsInside, dtl->dtlDocument ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int docPsPrintStartNote(
		    const DrawTextLine *	dtl,
		    int				part,
		    int				x0Twips,
		    const DocumentField *	df,
		    int				lineTop )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    BufferItem *		paraNode= dtl->dtlParaNode;


    int				cnt;
    int				closed;

    MemoryBuffer		mbSource;

    utilInitMemoryBuffer( &mbSource );

    /* Sanity check */
    cnt= docCountParticulesInFieldFwd( paraNode, &closed, part,
				    paraNode->biParaParticuleCount );
    if  ( cnt < 0 )
	{ LDEB(cnt); }

    ps->psInsideLink= 1;
    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= x0Twips;

    /* Remember what the target is to use that at the end of the link */
    utilEmptyMemoryBuffer( &(ps->psAnnotationDictionaryName) );
    utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
    utilEmptyMemoryBuffer( &(ps->psLinkFile) );

    /* Retrieve bookmark names for links back and forth */
    if  ( docSetNoteLinks( &(ps->psLinkMark), &mbSource, paraNode, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* Collect the title for the link */
    if  ( docPsPrintNoteLinkTitle( &(ps->psLinkTitle), dtl, df ) )
	{ LDEB(1); rval= -1;	}

    if  ( ps->psTagDocumentStructure	&&
	  ! ps->psInArtifact		&&
	  docPsPrintFinishInline( ps )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /* Include source bookmark as the destination for the jump back from the target */
    psDestPdfmark( ps, lineTop, &mbSource );

  ready:

    utilCleanMemoryBuffer( &mbSource );

    return rval;
    }

int docPsPrintFinishNote(
			const DrawTextLine *	dtl,
			int			x1Twips,
			const DocumentField *	df )
    {
    return docPsPrintFinishLink( dtl, x1Twips, df );
    }

static void fillStructureAttributes(
			MemoryBuffer *			structureAttributes,
			const char *			noteType,
			const struct BufferItem *	rootNode )
    {
    int				fieldNumber= rootNode->biSectSelectionScope.ssOwnerNumber;
    SimpleOutputStream *	sosAttributes= sioOutMemoryOpen( structureAttributes );

    char			scratch[30];

    sprintf( scratch, DOC_NOTES_FORMAT_DEF, fieldNumber );

    sioOutPrintf( sosAttributes, "/NoteType/%s", noteType ); /* Is a PDF-UA 2 FENote attribute */
    sioOutPrintf( sosAttributes, " /ID (%s)", scratch );

    if  ( rootNode->biTreeType != DOCinBODY )
	{
	sioOutPrintf( sosAttributes, " /A <</Placement/Block /O/Layout>>" );
	}

    sioOutClose( sosAttributes );
    }

const char * docPsNoteNodeMark(
		const PrintingState *		ps,
		const struct BufferItem *	rootNode,
		MemoryBuffer *			structureAttributes )
    {
    if  ( structureAttributes )
	{
	switch( rootNode->biTreeType )
	    {
	    case DOCinFOOTNOTE:
		fillStructureAttributes( structureAttributes, "Footnote", rootNode );
		break;

	    case DOCinENDNOTE:
		fillStructureAttributes( structureAttributes, "Endnote", rootNode );
		break;

	    default:
		LDEB(rootNode->biTreeType);
		return (char *)0;
	    }
	}

    return STRUCTtypeNOTE;
    }
