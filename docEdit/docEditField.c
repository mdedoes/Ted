/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<docField.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	<docDocumentField.h>
#   include	"docEditOperation.h"
#   include	"docEdit.h"
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	<utilPropMask.h>
#   include	<docFields.h>
#   include	<docParaParticuleAdmin.h>
#   include	<docParaBuilder.h>
#   include	<docFields.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert the particule that terminates a field.			*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert end particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertTailParticule(
				EditOperation *			eo,
				const DocumentPosition *	dpTail,
				int				part1,
				int *				pTailPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const struct TextAttribute *	taSet )
    {
    TextParticule *	tp;
    int			textAttrNr;

    struct BufferItem *	paraNode= dpTail->dpNode;

    /*  1  */
    tp= paraNode->biParaParticules+ part1;
    if  ( tp->tpStrlen > 0				&&
	  dpTail->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part1++; tp++;	}

    if  ( part1 < paraNode->biParaParticuleCount )
	{ textAttrNr= tp[ 0].tpTextAttrNr; }
    else{ textAttrNr= tp[-1].tpTextAttrNr; }

    /*  2  */
    if  ( part1 < paraNode->biParaParticuleCount	&&
	  tp->tpStroff != dpTail->dpStroff	)
	{
	if  ( docSplitTextParticule( (TextParticule **)0, &tp, paraNode,
						    part1, dpTail->dpStroff ) )
	    { LDEB(part1); return -1;	}

	part1++;
	}

    /*  3  */
    tp= docMakeSpecialParticule( paraNode, part1,
				dpTail->dpStroff, TPkindFIELDTAIL, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;
    docParagraphBuilderShiftOffsets( eo->eoParagraphBuilder,
				part1+ 1, dpTail->dpStroff, tp->tpStrlen );

    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				eo->eoDocument, paraNode, part1, part1+ 1,
				taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    *pTailPart= part1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the particule that starts a field.				*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert start particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertHeadParticule(
				EditOperation *			eo,
				const DocumentPosition *	dpHead,
				int *				pStroffShift,
				int				part0,
				int *				pHeadPart,
				int *				pTailPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const struct TextAttribute *	taSet )
    {
    TextParticule *	tp;
    int			textAttrNr;

    int			part1= *pTailPart;

    struct BufferItem *	paraNode= dpHead->dpNode;

    /*  1  */
    tp= paraNode->biParaParticules+ part0;
    if  ( tp->tpStrlen > 0					&&
	  dpHead->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part0++; tp++;	}

    if  ( part0 < paraNode->biParaParticuleCount )
	{ textAttrNr= tp[ 0].tpTextAttrNr; }
    else{ textAttrNr= tp[-1].tpTextAttrNr; }

    /*  2  */
    if  ( part0 < paraNode->biParaParticuleCount	&&
	  tp->tpStroff != dpHead->dpStroff	)
	{
	if  ( docSplitTextParticule( (TextParticule **)0, &tp, paraNode,
						part0, dpHead->dpStroff ) )
	    { LDEB(part1); return -1;	}

	if  ( part1 >= part0 )
	    { part1++;	}

	part0++;
	}

    /*  3  */
    tp= docMakeSpecialParticule( paraNode, part0,
			    dpHead->dpStroff, TPkindFIELDHEAD, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    docParagraphBuilderShiftOffsets( eo->eoParagraphBuilder,
				    part0+ 1, dpHead->dpStroff, tp->tpStrlen );

    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				eo->eoDocument, paraNode, part0, part0+ 1,
				taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    if  ( part1 >= part0 )
	{ part1++;	}

    *pStroffShift= tp->tpStrlen;
    *pHeadPart= part0;
    *pTailPart= part1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Surround a selected range of text by a text level field.		*/
/*									*/
/************************************************************************/

int docEditSurroundTextSelectionByField(
				EditOperation *			eo,
				struct DocumentField **		pDf,
				DocumentSelection *		pDsInside,
				DocumentSelection *		pDsAround,
				int *				pHeadPart,
				int *				pTailPart,
				const struct PropertyMask *	taSetMask,
				const struct TextAttribute *	taSet,
				int				fieldKind,
				const struct FieldInstructions *	fi )
    {
    int				rval= 0;

    int				headPart;
    int				tailPart;
    int				startShift= 0;

    DocumentField *		df;
    DocumentField *		dfLeft;
    DocumentField *		dfRight;

    int				headMoved= 0;
    int				tailMoved= 0;

    const struct BufferItem *	sectNode;
    int				headParaNr;
    int				tailParaNr;

    int				singlePara;
    
    struct BufferDocument *	bd= eo->eoDocument;
    struct DocumentTree *	dt= eo->eoTree;

    DocumentSelection		dsAround;
    DocumentSelection		dsInside;
    DocumentSelection		dsBalanced;

    docEditOperationGetSelection( &dsBalanced, eo );

    singlePara= DOC_FieldKinds[fieldKind].fkiSingleParagraph;

    if  ( singlePara && dsBalanced.dsHead.dpNode != dsBalanced.dsTail.dpNode )
	{
	LXXDEB(singlePara,dsBalanced.dsHead.dpNode,dsBalanced.dsTail.dpNode);
	rval= -1; goto ready;
	}

    sectNode= docGetSectNode( dsBalanced.dsHead.dpNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); rval= -1; goto ready;	}

    if  ( docFindParticuleOfPosition( &headPart, (int *)0,
					&(dsBalanced.dsHead), PARAfindLAST ) )
	{ LDEB(dsBalanced.dsHead.dpStroff); rval= -1; goto ready;	}
    if  ( docFindParticuleOfPosition( &tailPart, (int *)0,
					&(dsBalanced.dsTail), PARAfindFIRST ) )
	{ LDEB(dsBalanced.dsTail.dpStroff); rval= -1; goto ready;	}

    if  ( docBalanceFieldSelection( &dfLeft, &dfRight,
			&headMoved, &tailMoved,
			&headPart, &tailPart, 
			&(dsBalanced.dsHead), &(dsBalanced.dsTail), dt, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    df= docClaimField( bd );
    if  ( ! df )
	{ XDEB(df); rval= -1; goto ready;	}

    if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
						dsBalanced.dsTail.dpNode,
						dsBalanced.dsTail.dpStroff ) )
	{ LDEB(dsBalanced.dsTail.dpStroff); rval= -1; goto ready;	}

    tailParaNr= docParaBuilderGetParagraphNumber(
					eo->eoParagraphBuilder );

    /*  find end, split?, insert end particule */
    if  ( docFieldInsertTailParticule( eo, &(dsBalanced.dsTail),
					tailPart, &tailPart,
					df->dfFieldNumber, taSetMask, taSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
						dsBalanced.dsHead.dpNode,
						dsBalanced.dsHead.dpStroff ) )
	{ LDEB(dsBalanced.dsHead.dpStroff); rval= -1; goto ready;	}

    headParaNr= docParaBuilderGetParagraphNumber(
					eo->eoParagraphBuilder );

    /*  find begin, split?, insert start particule */
    if  ( docFieldInsertHeadParticule( eo, &(dsBalanced.dsHead),
					&startShift,
					headPart, &headPart, &tailPart,
					df->dfFieldNumber, taSetMask, taSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dsBalanced.dsHead.dpNode == dsBalanced.dsTail.dpNode )
	{ dsBalanced.dsTail.dpStroff += startShift;	}

    df->dfSelectionScope= sectNode->biSectSelectionScope;
    df->dfHeadPosition.epParaNr= headParaNr;
    df->dfHeadPosition.epStroff= dsBalanced.dsHead.dpStroff;
    df->dfTailPosition.epParaNr= tailParaNr;
    df->dfTailPosition.epStroff= dsBalanced.dsTail.dpStroff;

    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	PropertyMask	taDoneMask;

	utilPropMaskClear( &taDoneMask );

	if  ( docChangeParticuleAttributes( (int *)0, &taDoneMask, bd,
						dsBalanced.dsHead.dpNode,
						headPart, tailPart,
						taSet, taSetMask ) )
	    { LDEB(1);	}
	}

    if  ( docInsertFieldInTree( &(dt->dtRootFields), df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					    pHeadPart, pTailPart, bd, df ) )
	{ LDEB(df->dfFieldNumber); rval= -1; goto ready; }

    df->dfKind= fieldKind;

    if  ( fi )
	{
	if  ( docCopyFieldInstructions( &(df->dfInstructions), fi ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    docSetEditPosition( &(eo->eoSelectedRange.erTail), &(dsAround.dsTail) );
    docSetEditPosition( &(eo->eoAffectedRange.erTail), &(dsAround.dsTail) );

    if  ( pDf )
	{ *pDf= df;		}
    if  ( pDsInside )
	{ *pDsInside= dsInside;	}
    if  ( pDsAround )
	{ *pDsAround= dsAround;	}

  ready:

    return rval;
    }

/************************************************************************/

static int docDeleteFieldParticule(	EditOperation *		eo,
					BufferItem *		paraNode,
					int			paraNr,
					int			part )
    {
    int				textAttrNr;
    int				stroffShift= 0;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				partStroff= tp->tpStroff;
    int				partStrlen= tp->tpStrlen;

    textAttrNr= paraNode->biParaParticules[0].tpTextAttrNr;
    docParaStringReplace( &stroffShift, paraNode,
				partStroff, partStroff+ partStrlen,
				(const char *)0, 0 );
    docDeleteParticules( paraNode, part, 1 );

    if  ( docParaBuilderStartExistingParagraph( eo->eoParagraphBuilder,
							paraNode, partStroff ) )
	{ LDEB(partStroff); return -1;	}

    docEditShiftParticuleOffsets( eo, paraNode, paraNr, part,
						partStroff, -partStrlen );

    if  ( paraNode->biParaParticuleCount == 0 )
	{
	if  ( ! docInsertTextParticule( paraNode,
					0, 0, 0, TPkindSPAN, textAttrNr ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the begin and start particules of a field and the field	*/
/*  itself.								*/
/*									*/
/*  *dsExInside is set to a value that is appropriate to delete the	*/
/*  contents of the field afterward.					*/
/*									*/
/************************************************************************/

int docDeleteField(		DocumentSelection *		dsExInside,
				EditOperation *			eo,
				DocumentField *			df )
    {
    int			rval= 0;

    const int		direction= 1;

    DocumentSelection	dsInside;
    DocumentSelection	dsAround;

    int			partHead;
    int			partTail;

    if  ( docDelimitFieldInDoc( &dsInside, &dsAround, &partHead, &partTail,
							eo->eoDocument, df ) )
	{ LDEB(1); return -1;	}

    if  ( docDeleteFieldParticule( eo,
		dsAround.dsTail.dpNode, df->dfTailPosition.epParaNr,
		partTail ) )
	{ LDEB(partTail); return -1; }

    if  ( docDeleteFieldParticule( eo,
		dsAround.dsHead.dpNode, df->dfHeadPosition.epParaNr,
		partHead ) )
	{ LDEB(partHead); return -1; }

    if  ( docDeleteFieldFromParent( eo->eoTree, df ) )
	{ LDEB(1); rval= -1;	}

    docDeleteFieldFromDocument( eo->eoDocument, df );

    if  ( docSelectionSingleParagraph( &dsInside ) )
	{ dsInside.dsTail.dpStroff--;	}

    if  ( dsExInside )
	{
	docSetRangeSelection( dsExInside,
			&(dsInside.dsHead), &(dsInside.dsTail), direction );
	}

    return rval;
    }

