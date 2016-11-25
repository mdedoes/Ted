/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>

#   include	"docNotes.h"
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	"docParaParticules.h"
#   include	"docParaParticuleAdmin.h"
#   include	"docParaBuilder.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Extract the fixed text from a note and the paragraph that holds it.	*/
/*  The situation arises when a document holds a note without a note	*/
/*  field that immediately precedes it.					*/
/*									*/
/*  NOTE that the paragraph builder in the RTF reader that calls this	*/
/*	routine is for another tree.					*/
/*									*/
/************************************************************************/

int docExtractFixedTextNote(	struct DocumentNote *	dn,
				struct BufferDocument *	bd,
				struct BufferItem *	ownerNode,
				int			fixedStroff,
				int			fixedStrlen,
				const char * const	instBytes,
				const int		instSize )
    {
    int			rval= 0;

    DocumentSelection	dsNoteHeadField;
    const int		part0= 0;

    struct ParagraphBuilder * pb= (struct ParagraphBuilder *)0;

    pb= docOpenParagraphBuilder( bd,
		&(dn->dnDocumentTree.dtRoot->biSectSelectionScope),
		&(dn->dnDocumentTree) );
    if  ( ! pb )
	{ XDEB(pb); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetBytes( &(dn->dnNoteProperties.npFixedText),
		    (unsigned char *)docParaString( ownerNode, fixedStroff ),
		    fixedStrlen ) )
	{ LDEB(fixedStrlen); rval= -1; goto ready;	}

    if  ( docHeadPosition( &(dsNoteHeadField.dsHead),
					dn->dnDocumentTree.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}
    docSetDocumentPosition( &(dsNoteHeadField.dsTail),
				dsNoteHeadField.dsHead.dpNode, fixedStrlen );

    if  ( docParaStrlen( dsNoteHeadField.dsHead.dpNode ) >= fixedStrlen	&&
	  ! memcmp( docParaString( dsNoteHeadField.dsHead.dpNode, 0 ),
		docParaString( ownerNode, fixedStroff ), fixedStrlen )	)
	{
	int			part1;
	TextParticule *		tp1;
	DocumentField *		df;

	const int		keepSpace= 1;

	if  ( docParaBuilderStartExistingParagraph( pb,
					    dsNoteHeadField.dsTail.dpNode,
					    dsNoteHeadField.dsTail.dpStroff ) )
	    { LDEB(dsNoteHeadField.dsTail.dpStroff); return -1;	}

	if  ( docFindParticuleOfPosition( &part1, (int *)0,
				&(dsNoteHeadField.dsTail), PARAfindFIRST ) )
	    { LDEB(fixedStroff); rval= -1; goto ready; }

	tp1= dsNoteHeadField.dsTail.dpNode->biParaParticules+ part1;

	if  ( tp1->tpStroff+ tp1->tpStrlen > fixedStrlen )
	    {
	    if  ( docSplitTextParticule(
			(TextParticule **)0, (TextParticule **)0,
			dsNoteHeadField.dsTail.dpNode, part1,
			dsNoteHeadField.dsTail.dpStroff ) )
		{ LDEB(part1); rval= -1; goto ready;	}
	    }

	part1++;

	df= docMakeTextLevelField( pb,
		    dsNoteHeadField.dsHead.dpStroff,
		    dsNoteHeadField.dsTail.dpStroff,
		    part0, part1, tp1->tpTextAttrNr, DOCfkCHFTN );
	if  ( ! df )
	    { XDEB(df); rval= -1; goto ready;	}

	if  ( docSetFieldInst( df, keepSpace, instBytes, instSize ) )
	    { XDEB(df); rval= -1; goto ready;	}
	}

  ready:

    if  ( pb )
	{ docCloseParagraphBuilder( pb );	}

    return rval;
    }

int docNoteIncludeFixedTextInField(	int *			pFixedStroff,
					int *			pFixedStrlen,
					struct BufferItem *	ownerNode,
					struct DocumentField *	dfNote )
    {
    DocumentPosition	dp;
    int			part0;

    int			fixedStroff= 0;
    int			fixedStrlen= 0;

    docSetDocumentPosition( &dp, ownerNode,
				    dfNote->dfHeadPosition.epStroff );
    if  ( docFindParticuleOfPosition( &part0, (int *)0,
						&dp, PARAfindFIRST ) )
	{ LDEB(dfNote->dfHeadPosition.epStroff); return -1; }

    /* Move the fixed text into the note field */
    if  ( ownerNode->biParaParticules[part0   ].tpKind == TPkindSPAN	&&
	  ownerNode->biParaParticules[part0+ 1].tpKind == TPkindFIELDHEAD )
	{
	fixedStroff= ownerNode->biParaParticules[part0   ].tpStroff;
	fixedStrlen= ownerNode->biParaParticules[part0   ].tpStrlen;

	dfNote->dfHeadPosition.epStroff= fixedStroff;

	ownerNode->biParaParticules[part0+ 1].tpKind= TPkindSPAN;
	ownerNode->biParaParticules[part0+ 1].tpObjectNumber= -1;
	ownerNode->biParaParticules[part0+ 1].tpStroff= fixedStroff;
	ownerNode->biParaParticules[part0+ 1].tpStrlen= fixedStrlen;

	ownerNode->biParaParticules[part0   ].tpKind= TPkindFIELDHEAD;
	ownerNode->biParaParticules[part0   ].tpObjectNumber=
						dfNote->dfFieldNumber;
	ownerNode->biParaParticules[part0   ].tpStrlen= 0;
	}

    if  ( pFixedStroff )
	{ *pFixedStroff= fixedStroff; }
    if  ( pFixedStrlen )
	{ *pFixedStrlen= fixedStrlen; }

    return 0;
    }
