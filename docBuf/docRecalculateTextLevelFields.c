/************************************************************************/
/*									*/
/*  Evaluate fields+ the list of kinds of fields.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<limits.h>

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docField.h"
#   include	"docParaParticules.h"
#   include	<docTreeType.h>
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	"docTreeScanner.h"
#   include	"docDebug.h"
#   include	"docRecalculateFields.h"
#   include	"docSectHeadersFooters.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docParaBuilder.h"
#   include	"docSelect.h"
#   include	<docFieldScanner.h>
#   include	<docScanner.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Substitute the text fields in a paragraph.				*/
/*									*/
/*  1)  For all fields in range...					*/
/*	NOTE that the end of the range may have been shifted by the	*/
/*	calculation of field results.					*/
/*  3)  Not the beginning of a field.. Irrelevant.			*/
/*  4)  Retrieve the field. Do some sanity checks: Only text level	*/
/*	fields are evaluated here.					*/
/*  5)  Count the number of particules currently in the field.		*/
/*  6)  When the field is to be recalculated.. do so.			*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*	Otherwise just shift the offsets of the particules inside the	*/
/*	field.								*/
/*  7)  When there is sufficient space inside the field to contain	*/
/*	yet another one.. go into recursion.				*/
/*	NOTE that this may shift both the array of particules and the	*/
/*	paragraph text.							*/
/*  8)  Shift the end particule of the field.				*/
/*  9)  Set the current position in the loop to the end particule. The	*/
/*	loop increment will move to the particule after the field.	*/
/*									*/
/************************************************************************/

typedef struct RecalculateParaTextFields
    {
    RecalculateFields *		rptfRecalculateFields;
    BufferItem *		rptfParaNode;
    int				rptfParaNr;
    int				rptfFieldsUpdated;

				/**
				 *  Paragraph builder to do part of the 
				 *  dirty work. Only instantiated if 
				 *  there are any fields in the paragraph.
				 *  (lazily)
				 */
    struct ParagraphBuilder *	rptfParagraphBuilder;
    } RecalculateParaTextFields;

static int docRecalculateParaTextField(
			    RecalculateParaTextFields *		rptf,
			    int					partHead,
			    int					partCount,
			    int					stroffHead,
			    int					stroffTail,
			    DocumentField *			df,
			    const FieldKindInformation *	fki )
    {
    RecalculateFields *	rf= rptf->rptfRecalculateFields;

    int			partTail;
    int			stroffShift= 0;
    int			calculated= 0;

    if  ( ! rptf->rptfParagraphBuilder )
	{
	rptf->rptfParagraphBuilder= docOpenParagraphBuilder(
			rf->rfDocument, rf->rfSelectionScope, rf->rfTree );

	if  ( ! rptf->rptfParagraphBuilder )
	    { XDEB(rptf->rptfParagraphBuilder); return -1;	}
	}

    if  ( docParaBuilderStartExistingParagraph( rptf->rptfParagraphBuilder,
					    rptf->rptfParaNode, stroffHead ) )
	{ LDEB(stroffHead); return -1;	}

    partTail= (*fki->fkiCalculateTextParticules)(
				&calculated, &stroffShift,
				rptf->rptfParagraphBuilder, df, rf,
				partHead, partCount );
    if  ( partTail <= partHead )
	{ LLDEB(partHead,partTail); return -1;	}

    if  ( calculated )
	{
	if  ( rf->rfTree == rf->rfSelectedTree )
	    {
	    docAdjustEditPositionOffsetB( &(rf->rfSelHead),
			rptf->rptfParaNr, stroffTail, stroffShift );
	    docAdjustEditPositionOffsetE( &(rf->rfSelTail),
			rptf->rptfParaNr, stroffTail, stroffShift );
	    }

	if  ( stroffShift != 0 )
	    {
	    if  ( docParagraphBuilderShiftOffsets(
			rptf->rptfParagraphBuilder,
			partTail+ 1, stroffHead, stroffShift ) )
		{ LDEB(stroffShift);	}
	    }

	rptf->rptfFieldsUpdated++;
	}

    return 0;
    }

static int docParaEnterTextField(
			DocumentField *			df,
			const EditRange *		er,
			void *				vptf )
    {
    RecalculateParaTextFields *		rptf= (RecalculateParaTextFields *)vptf;
    const RecalculateFields *		rf= rptf->rptfRecalculateFields;
    const FieldKindInformation *	fki;

    DocumentPosition			dpHead;
    int					partHead;

    DocumentPosition			dpTail;
    int					partTail;

    docInitDocumentPosition( &dpHead );
    docInitDocumentPosition( &dpTail );

    if  ( df->dfKind >= DOC_FieldKindCount )
	{ LLDEB(df->dfKind,DOC_FieldKindCount); return SCANadviceOK;	}

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( fki->fkiLevel != DOClevSPAN				||
	  ! ( fki->fkiCalculateWhen & rf->rfUpdateFlags )	||
	  ! fki->fkiCalculateTextParticules			)
	{ return SCANadviceOK;	}

    /* Find head particule.. This MUST suceed */
    dpHead.dpNode= rptf->rptfParaNode;
    dpHead.dpStroff= df->dfHeadPosition.epStroff;
    if  ( docFindParticuleOfPosition( &partHead, (int *)0,
						&dpHead, PARAfindLAST ) )
	{ LDEB(dpHead.dpStroff); return SCANadviceERROR;	}

    /* Find tail particule.. In strange documents, this can fail */
    dpTail.dpNode= rptf->rptfParaNode;
    dpTail.dpStroff= df->dfTailPosition.epStroff;
    if  ( docFindParticuleOfPosition( &partTail, (int *)0,
						&dpTail, PARAfindLAST ) )
	{ LDEB(dpTail.dpStroff); return SCANadviceOK;	}

    {
    int		partCount= partTail- partHead- 1;

    if  ( docRecalculateParaTextField( rptf, partHead, partCount,
				dpHead.dpStroff, dpTail.dpStroff, df, fki ) )
	{ LDEB(partHead); return SCANadviceERROR;	}
    }

    return SCANadviceOK;
    }

static int docRecalculateParaNodeTextFields(
					RecalculateFields *	rf,
					struct BufferItem *	paraNode )
    {
    int				rval= 0;
    RecalculateParaTextFields	rptf;

    EditRange			er;

    rptf.rptfRecalculateFields= rf;
    rptf.rptfParaNode= paraNode;
    rptf.rptfParaNr= docNumberOfParagraph( paraNode );
    rptf.rptfFieldsUpdated= 0;
    rptf.rptfParagraphBuilder= (struct ParagraphBuilder *)0;

    docInitEditRange( &er );
    er.erHead.epParaNr= rptf.rptfParaNr;
    er.erHead.epStroff= 0;
    er.erTail.epParaNr= rptf.rptfParaNr;
    er.erTail.epStroff= INT_MAX; /* make sure to reach the shifting end */

    if  ( docScanFieldsInRange( &(rf->rfTree->dtRootFields), &er,
		    docParaEnterTextField,
		    (TreeFieldVisitor)0,
		    (void *)&rptf ) < 0 )
	{ LDEB(0); rval= -1; goto ready;	}

    if  ( rptf.rptfFieldsUpdated > 0 )
	{
	docInvalidateParagraphLayout( paraNode );

	rf->rfFieldsUpdated += rptf.rptfFieldsUpdated;
	}

  ready:

    if  ( rptf.rptfParagraphBuilder )
	{ docCloseParagraphBuilder( rptf.rptfParagraphBuilder );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Substitute the text fields in a document tree.			*/
/*									*/
/*  1)	Unexpected recursion?						*/
/*									*/
/************************************************************************/

int docRecalculateTextLevelFieldsInDocumentTree(
				RecalculateFields *		rf,
				struct DocumentTree *		dt,
				const struct BufferItem *	bodySectNode,
				int				page )
    {
    int			rval= 0;
    int			ret;

    int			saveFieldsUpdated= rf->rfFieldsUpdated;
    DocumentTree *	saveTree= rf->rfTree;
    const SelectionScope * saveSelectionScope= rf->rfSelectionScope;
    const struct BufferItem *	saveBodySectNode= rf->rfBodySectNode;
    int			saveBodySectPage= rf->rfBodySectPage;

    if  ( ! dt->dtRoot )
	{ return 0;	}

    if  ( dt->dtRoot->biTreeType != DOCinBODY			&&
          ( ! bodySectNode				||
            bodySectNode->biTreeType != DOCinBODY	)	)
	{ XDEB(rf->rfBodySectNode); return -1;	}

    rf->rfBodySectNode= bodySectNode;
    rf->rfBodySectPage= page;

    rf->rfFieldsUpdated= 0;
    rf->rfTree= dt;
    rf->rfSelectionScope= &(dt->dtRoot->biSectSelectionScope);

    ret= docRecalculateTextLevelFields( rf, dt->dtRoot );
    if  ( ret )
	{ LDEB(ret); rval= -1;	}

    if  ( rf->rfFieldsUpdated > 0 )
	{ docInvalidateTreeLayout( dt );	}

    rf->rfFieldsUpdated += saveFieldsUpdated;

    rf->rfTree= saveTree;
    rf->rfSelectionScope= saveSelectionScope;
    rf->rfBodySectNode= saveBodySectNode;
    rf->rfBodySectPage= saveBodySectPage;

    return rval;
    }

static int docRecalculateTextLevelFieldsInSeparators(
				RecalculateFields *		rf,
				const struct BufferItem *	bodySectNode )
    {
    const int			page= -1;
    struct BufferDocument *	bd= rf->rfDocument;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdFtnsep), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdFtnsepc), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdFtncn), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdAftnsep), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdAftnsepc), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			    &(bd->bdAftncn), bodySectNode, page )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRecalculateTextLevelFieldsInSectHdFt(
				RecalculateFields *		rf,
				const struct BufferItem *	bodySectNode )
    {
    const int			page= -1;
    SectHeadersFooters *	shf= bodySectNode->biSectHeadersFooters;

    rf->rfBodySectNode= bodySectNode;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfFirstPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLeftPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfRightPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLastPageHeader), bodySectNode, page ) )
	{ LDEB(1); return -1;	}


    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfFirstPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLeftPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfRightPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
			&(shf->shfLastPageFooter), bodySectNode, page ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRecalculateTextLevelFieldsInSectNotes(
					RecalculateFields *	rf,
					const BufferItem *	bodySectNode )
    {
    struct BufferDocument *	bd= rf->rfDocument;

    DocumentField *	dfNote;
    DocumentNote *	dn;
    const int		page= -1;
    const int		treeType= -1;

    dfNote= docGetFirstNoteOfSection( &dn, bd,
				bodySectNode->biNumberInParent, treeType );
    while( dfNote )
	{
	struct DocumentTree *	dt= &(dn->dnDocumentTree);

	if  ( ! dt->dtRoot )
	    { continue;	}

	if  ( docRecalculateTextLevelFieldsInDocumentTree( rf,
						dt, bodySectNode, page ) )
	    { LDEB(1); return -1;	}

	dfNote= docGetNextNoteInSection( &dn, bd,
			bodySectNode->biNumberInParent, dfNote, treeType );
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRecalculateTextLevelFieldsLeaveNode(
			    struct BufferItem *			node,
			    const struct DocumentSelection *	ds,
			    const struct BufferItem *		bodySectNode,
			    void *				voidrf )
    {
    RecalculateFields *	rf= (RecalculateFields *)voidrf;
    int			rval= 0;

    const struct BufferItem *	saveBodySectNode= rf->rfBodySectNode;

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docRecalculateTextLevelFieldsInSeparators( rf,
						    node->biChildren[0] ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    break;

	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY )
		{
		SectHeadersFooters *	shf= node->biSectHeadersFooters;

		if  ( ! shf )
		    { XDEB(shf); rval= -1; goto ready;	}

		if  ( docRecalculateTextLevelFieldsInSectHdFt( rf, node ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( docRecalculateTextLevelFieldsInSectNotes( rf, node ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}

	    rf->rfSelectionScope= &(node->biSectSelectionScope);

	    break;

	case DOClevPARA:

	    if  ( docRecalculateParaNodeTextFields( rf, node ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    break;

	default:
	    break;
	}

  ready:

    rf->rfBodySectNode= saveBodySectNode;

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docRecalculateTextLevelFields(	RecalculateFields *	rf,
					struct BufferItem *	node )
    {
    const int		flags= 0;

    if  ( docScanTreeNode( rf->rfDocument, node,
		    (NodeVisitor)0, docRecalculateTextLevelFieldsLeaveNode,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)rf ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

