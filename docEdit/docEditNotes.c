/************************************************************************/
/*									*/
/*  Manage notes.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docParaString.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<textAttribute.h>
#   include	"docEditImpl.h"
#   include	"docEditOperation.h"
#   include	"docCopyNode.h"
#   include	"docDocumentCopyJob.h"
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<utilPropMask.h>
#   include	<docFields.h>
#   include	<docAttributes.h>
#   include	<docTreeType.h>
#   include	<docField.h>
#   include	<docParaParticuleAdmin.h>
#   include	<docParaBuilder.h>
#   include	<docNodeTree.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a note at the given position.					*/
/*									*/
/*  1)  Claim memory for a note.					*/
/*  2)  Insert a particule that refers to the note.			*/
/*  4)  Insert an empty paragraph in the note to begin with.		*/
/*  5)  Make a chftn field to represent the note in the external	*/
/*	paragraph.							*/
/*  6)  Insert a space after the note number field. This also is to	*/
/*	work around a bug in the editing around fields.			*/
/*									*/
/*  !)  The chftn field that precedes the note that was inserted by the	*/
/*	caller.								*/
/*									*/
/************************************************************************/

int docEditMakeNote(	DocumentNote **			pDn,
			struct BufferDocument *		bd,
			DocumentField *			dfNote,
			const struct BufferItem *	bodyNode,
			int				treeType,
			int				fieldKind )
    {
    int			rval= 0;

    int			textAttributeNumberPlain= -1;
    int			textAttributeNumberSuper= -1;

    DocumentField *	dfRef= (DocumentField *)0;
    DocumentNote *	dnRef= (DocumentNote *)0;
    const struct BufferItem *	refParaNode= (const struct BufferItem *)0;

    DocumentNote *	dn;

    int			noteIndex;

    int			stroffShift= 0;
    const int		autoNumber= 1;

    struct BufferItem *	noteParaNode;

    DocumentField *	dfHead;
    DocumentSelection	dsInsideHead;
    DocumentSelection	dsAroundHead;
    int			partHead= -1;
    int			partTail= -1;

    TextParticule *	tp;

    struct ParagraphBuilder * pb= (struct ParagraphBuilder *)0;

    /*  1  */
    noteIndex= docInsertNote( &dn, bd, dfNote, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); rval= -1; goto ready;	}

    dn->dnNoteProperties.npTreeType= treeType;

    if  ( ! dfRef )
	{ dfRef= docGetPrevNoteInDocument( &dnRef, bd, dfNote, treeType ); }
    if  ( ! dfRef )
	{ dfRef= docGetNextNoteInDocument( &dnRef, bd, dfNote, treeType ); }
    if  ( ! dfRef )
	{ dfRef= docGetPrevNoteInDocument( &dnRef, bd, dfNote, -1 ); }
    if  ( ! dfRef )
	{ dfRef= docGetNextNoteInDocument( &dnRef, bd, dfNote, -1 ); }

    if  ( dfRef )
	{
	DocumentPosition	dpRef;

	if  ( docHeadPosition( &dpRef, dnRef->dnDocumentTree.dtRoot ) )
	    { LDEB(1);	}
	else{
	    refParaNode= dpRef.dpNode;

	    if  ( docDelimitParaHeadField( (DocumentField **)0,
					    &dsInsideHead, &dsAroundHead,
					    &partHead, &partTail,
					    refParaNode, bd ) )
		{ LDEB(1);	}

	    if  ( partHead+ 1 < partTail )
		{
		textAttributeNumberSuper=
		    refParaNode->biParaParticules[partHead+ 1].tpTextAttrNr;
		}
	    if  ( partTail+ 1 < refParaNode->biParaParticuleCount )
		{
		textAttributeNumberPlain=
		    refParaNode->biParaParticules[partTail+ 1].tpTextAttrNr;
		}
	    }
	}

    if  ( textAttributeNumberPlain < 0 || textAttributeNumberSuper < 0 )
	{
	TextAttribute	ta;
	TextAttribute	taSet;

	docPlainTextAttribute( &ta, bd );
	ta.taFontSizeHalfPoints= 20;

	if  ( textAttributeNumberPlain < 0 )
	    { textAttributeNumberPlain= docTextAttributeNumber( bd, &ta ); }

	taSet= ta;
	taSet.taSuperSub= TEXTvaSUPERSCRIPT;
	if  ( textAttributeNumberSuper < 0 )
	    { textAttributeNumberSuper= docTextAttributeNumber( bd, &taSet ); }
	}

    /*  4  */
    noteParaNode= docStartDocumentTree( bd, &(dn->dnDocumentTree), treeType,
					bodyNode, dfNote->dfFieldNumber,
					textAttributeNumberPlain );
    if  ( ! noteParaNode )
	{ XDEB(noteParaNode); rval= -1; goto ready;	}

    if  ( refParaNode )
	{
	PropertyMask	ppSetMask;

	utilPropMaskClear( &ppSetMask );
	utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
	PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

	if  ( docParaNodeSetProperties( noteParaNode, &ppSetMask,
				    refParaNode->biParaProperties, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  5  */
    if  ( docInsertParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
		    &partHead, &partTail,
		    pb, fieldKind, textAttributeNumberSuper ) )
	{ LDEB(1); rval= -1; goto ready;	}
    noteParaNode->biParaParticules[partTail].tpTextAttrNr=
						textAttributeNumberPlain;
    /*  6  */
    if  ( docParaStringReplace( &stroffShift, noteParaNode,
					    dsAroundHead.dsTail.dpStroff,
					    dsAroundHead.dsTail.dpStroff,
					    " ", 1 ) )
	{ LDEB(dsAroundHead.dsTail.dpStroff); rval= -1; goto ready; }

    tp= &(noteParaNode->biParaParticules[noteParaNode->biParaParticuleCount- 1]);
    if  ( tp->tpKind == TPkindSPAN		&&
	  tp->tpStrlen == 0			)
	{
	tp->tpStrlen++;
	tp->tpTextAttrNr= textAttributeNumberPlain;
	}
    else{
	tp= docInsertTextParticule( noteParaNode,
				    noteParaNode->biParaParticuleCount,
				    docParaStrlen( noteParaNode )- 1, 1,
				    TPkindSPAN, textAttributeNumberPlain );
	if  ( ! tp )
	    { LDEB(noteParaNode->biParaParticuleCount); rval= -1; goto ready; }
	}

    *pDn= dn;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the kind of note for the current selection.			*/
/*									*/
/************************************************************************/

int docEditChangeNote(		EditOperation *		eo,
				DocumentField *		dfNote,
				DocumentNote *		dn,
				int			selInNote,
				const PropertyMask *	npSetMask,
				const NoteProperties *	npSet )
    {
    PropertyMask		npDoneMask;

    utilPropMaskClear( &npDoneMask );

    if  ( docUpdNoteProperties( &npDoneMask, &(dn->dnNoteProperties),
							npSetMask, npSet ) )
	{ LDEB(1); return -1;	}

    if  ( PROPmaskISSET( &npDoneMask, NOTEpropTREE_TYPE ) )
	{
	if  ( docCheckSeparatorExistenceForNoteType( eo->eoDocument,
					    dn->dnNoteProperties.npTreeType ) )
	    { LDEB(DOCinFTNSEP); return -1;	}

	dn->dnNoteNumber= 0;
	docInvalidateTreeLayout( &(dn->dnDocumentTree) );

	if  ( ! dn->dnDocumentTree.dtRoot )
	    { XDEB(dn->dnDocumentTree.dtRoot); return -1;	}

	docSetTreeTypeOfNode( dn->dnDocumentTree.dtRoot,
					    dn->dnNoteProperties.npTreeType );

	if  ( selInNote )
	    {
	    eo->eoSelectionScope.ssTreeType= dn->dnNoteProperties.npTreeType;
	    }
	}

    eo->eoFieldUpdate |= FIELDdoCHFTN;
    utilIndexSetAdd( &(eo->eoNoteFieldsAdded), dfNote->dfFieldNumber );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a note from one paragraph to another.				*/
/*									*/
/************************************************************************/

int docCopyNote(		DocumentCopyJob *	dcj,
				DocumentField *		dfTo,
				const DocumentField *	dfFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    struct BufferDocument *		bdTo= eo->eoDocument;

    DocumentNote *		dnTo;
    DocumentNote *		dnFrom;

    dnFrom= docGetNoteOfField( dfFrom, dcj->dcjSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    dfTo->dfNoteIndex= docInsertNote( &dnTo, bdTo, dfTo,
				    dnFrom->dnNoteProperties.npAutoNumber );
    if  ( dfTo->dfNoteIndex < 0 )
	{ LDEB(dfTo->dfNoteIndex); return -1;	}

    /* realloc */
    dnFrom= docGetNoteOfField( dfFrom, dcj->dcjSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    if  ( dnFrom->dnDocumentTree.dtRoot )
	{
	SelectionScope	ssRoot;

	docInitSelectionScope( &ssRoot );

	ssRoot.ssTreeType= dnFrom->dnNoteProperties.npTreeType;
	ssRoot.ssSectNr= 0;
	ssRoot.ssOwnerSectNr= dfTo->dfSelectionScope.ssSectNr;
	ssRoot.ssOwnerNumber= dfTo->dfFieldNumber;

	if  ( docCopyDocumentTree( dcj, &(dnTo->dnDocumentTree), &ssRoot,
						&(dnFrom->dnDocumentTree) ) )
	    { XDEB(dnTo->dnDocumentTree.dtRoot); return -1;	}
	}

    dnTo->dnNoteProperties.npTreeType=
				dnFrom->dnNoteProperties.npTreeType;

    if  ( docCheckSeparatorExistenceForNoteType( bdTo,
					dnTo->dnNoteProperties.npTreeType ) )
	{ LDEB(dnTo->dnNoteProperties.npTreeType); return -1; }

    return 0;
    }

