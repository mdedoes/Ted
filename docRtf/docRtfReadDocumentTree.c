/************************************************************************/
/*									*/
/*  Read footnotes, headers, footers, textboxes.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfFindProperty.h"
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docBuf.h>
#   include	<docFields.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/************************************************************************/
/*									*/
/*  Consume Headers, Footers, Notes &c: Separate item trees that are.	*/
/*  embedded in the document.						*/
/*									*/
/*  2)  Make the header/note &c.					*/
/*  3)  Consume its contents.						*/
/*  4)  Make sure that no bookmarks protrude beyond the end of the	*/
/*	external item.							*/
/*  5)  If the external item happens to be empty, insert an empty	*/
/*	paragraph to avoid problems later on.				*/
/*  6)  Restore the current position.					*/
/*									*/
/************************************************************************/

int docRtfReadDocumentTree(	const RtfControlWord *	rcw,
				struct DocumentTree *	tree,
				int *			pTreeType,
				RtfReader *		rr,
				int			ignoreEmpty,
				const SelectionScope *	ss )
    {
    int				rval= 0;

    RtfReadingState		internRrs;
    RtfTreeStack		internRts;

    docRtfInitTreeStack( &internRts );

    /*  2  */
    if  ( ! tree->dtRoot						&&
	  docMakeDocumentTree( rr->rrDocument, tree,
				    ss, &(rr->rrSectionProperties) )	)
	{ LDEB(1); return -1;	}

    docRtfPushReadingState( rr, &internRrs );
    /* popped in docRtfFinishCurrentTree() */
    docRtfPushTreeStack( rr, &internRts, ss, tree );

    docRtfResetParagraphProperties( &internRrs );
    docRtfResetTextAttribute( &internRrs, rr->rrDocument );

    /*  3  */
    if  ( docRtfReadGroup( rcw, 0, 0, rr,
			    docRtfDocumentGroups,
			    docRtfGotText,
			    docRtfFinishCurrentTree ) )
	{ LDEB(1); rval= -1;	}

    /*  5  */
    {
    DocumentPosition	dp;

    if  ( docTailPosition( &dp, tree->dtRoot ) )
	{
	if  ( ignoreEmpty )
	    { docEraseDocumentTree( rr->rrDocument, tree );	}
	else{
	    const int	textAttributeNr= 0;

	    if  ( ! docAppendParagraph( rr->rrDocument, tree->dtRoot,
						textAttributeNr ) )
		{ LDEB(textAttributeNr);	}
	    }
	}
    else{
	/********************************************************/
	/*  Delete empty paragraph caused by final \par		*/
	/********************************************************/
	if  ( docParaStrlen( dp.dpNode ) ==  0			&&
	      dp.dpNode->biParaProperties->ppTableNesting == 0	&&
	      docNumberOfParagraph( dp.dpNode ) > 1		&&
	      dp.dpNode->biParaParticuleCount == 1		)
	    {
	    struct BufferItem *	node= dp.dpNode;

	    while( node->biParent && node->biParent->biChildCount == 1 )
		{ node= node->biParent;	}

	    docDeleteNode( rr->rrDocument, tree, node );
	    }
	}
    }

    *pTreeType= internRts.rtsSelectionScope.ssTreeType;

    /*  6  */
    docRtfPopTreeStack( rr );
    docRtfPopReadingState( rr );

    return rval;
    }

int docRtfReadExtTree(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    struct BufferItem *		sectNode;
    struct BufferDocument *	bd= rr->rrDocument;
    struct DocumentTree *	dt;

    SelectionScope	ss;
    int			treeType;
    int			ignoreEmpty= 0;

    docInitSelectionScope( &ss );
    ss.ssSectNr= 0;

    switch( rcw->rcwID )
	{
	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    sectNode= docRtfGetSectNode( rr );
	    if  ( ! sectNode )
		{ XDEB(sectNode); return -1;	}

	    dt= docSectionHeaderFooter( sectNode, (unsigned char *)0,
			    rr->rrDocument->bdProperties, rcw->rcwID );
	    if  ( ! dt )
		{ LXDEB(rcw->rcwID,dt); return -1;	}

	    ss.ssOwnerSectNr= sectNode->biNumberInParent;
	    ignoreEmpty= 1;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! bd )
		{ XDEB(bd); return -1;	}

	    dt= docDocumentNoteSeparator( bd, rcw->rcwID );
	    if  ( ! dt )
		{ LXDEB(rcw->rcwID,dt); return -1;	}

	    ignoreEmpty= 1;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( dt->dtRoot )
	{ docEraseDocumentTree( bd, dt );	}

    ss.ssTreeType= rcw->rcwID;
    ss.ssOwnerNumber= -1;

    if  ( docRtfReadDocumentTree( rcw, dt, &treeType, rr, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume a footnote.							*/
/*									*/
/************************************************************************/

int docRtfReadNote(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			rval= 0;
    struct BufferDocument *	bd= rr->rrDocument;

    DocumentField *	dfNote;
    DocumentNote *	dn;
    int			noteIndex;

    struct BufferItem *	ownerNode;
    struct BufferItem *	sectNode;
    SelectionScope	ss;

    int			treeType;
    int			autoNumber= 0;
    const int		ignoreEmpty= 0;

    int			fixedStroff= 0;
    int			fixedStrlen= 0;

    const char *	instBytes= " -CHFTN ";
    int			instSize= 8;

    docInitSelectionScope( &ss );

    ownerNode= docRtfGetParaNode( rr );
    if  ( ! ownerNode )
	{ XDEB(ownerNode); rval= -1; goto ready;	}
    sectNode= docGetSectNode( ownerNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); rval= -1; goto ready;	}

    if  ( rr->rrAfterNoteref )
	{ autoNumber= 1;	}
    else{
	const char *		fieldRslt= (const char *)0;

	dfNote= docRtfSpecialField( DOCfkCHFTN,
				    instBytes, instSize, fieldRslt, rr );
	if  ( ! dfNote )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready; }

	if  ( docNoteIncludeFixedTextInField( &fixedStroff, &fixedStrlen,
						    ownerNode, dfNote ) )
	    { LDEB(dfNote->dfHeadPosition.epStroff); rval= -1; goto ready; }

	autoNumber= 0;
	if  ( fixedStrlen > 0 )
	    {
	    PROPmaskADD( &(rr->rrNotePropertyMask), NOTEpropFIXED_TEXT );
	    }
	}

    dfNote= docGetFieldByNumber( bd, rr->rrTreeStack->rtsLastFieldNumber );
    if  ( ! dfNote )
	{
	LPDEB(rr->rrTreeStack->rtsLastFieldNumber,dfNote);
	rval= -1; goto ready;
	}

    noteIndex= docInsertNote( &dn, bd, dfNote, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); rval= -1; goto ready;	}

    docRtfResetParagraphReadingState( rr );

    ss.ssTreeType= DOCinFOOTNOTE;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= sectNode->biNumberInParent;
    ss.ssOwnerNumber= dfNote->dfFieldNumber;

    if  ( docRtfReadDocumentTree( rcw, &(dn->dnDocumentTree), &treeType,
						rr, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    dn->dnNoteProperties.npAutoNumber= autoNumber;
    dn->dnNoteProperties.npTreeType= treeType;
    docSetTreeTypeOfNode( dn->dnDocumentTree.dtRoot, treeType );

    if  ( ! autoNumber							&&
	  fixedStrlen > 0						&&
	  docExtractFixedTextNote( dn, bd, ownerNode,
					fixedStroff, fixedStrlen,
					instBytes, instSize )		)
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

