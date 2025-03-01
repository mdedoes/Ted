/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docBuf.h"
#   include	"docParaParticules.h"
#   include	"docField.h"
#   include	"docSelect.h"
#   include	"docTreeNode.h"

#   include	<docDocumentField.h>
#   include	<docTextParticule.h>
#   include	"docFields.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

# if 0

Memory management of fields during editing:
-------------------------------------------

Before an edit operation is started, the selection that is affected 
is balanced with respect to field nesting: Fields head and tail are either
both inside the selection or both outside the selection.

If the edit operation is a deletion or a replacement all fields inside 
the selection are deleted before the edit operation starts to change 
anything else in the document.

This means that the edit operation only has to care about new fields: 
Existing fields are possibly moved around, but they never have to 
be created, copied or deleted.

# endif

/************************************************************************/
/*									*/
/*  Balance a selection such that it does not partially cover any	*/
/*  field.								*/
/*									*/
/*  A)  Determine particule numbers for the least nested particules	*/
/*	at the beginning and the end of the selection.			*/
/*  B)  Determine actual values of the text field nesting levels.	*/
/*  C)  And look for the matching particules at the same level.		*/
/*									*/
/************************************************************************/

static DocumentField * docGetLeftOuterField(
				    DocumentField *		df,
				    const EditPosition *	epHead,
				    const EditPosition *	epTail )
    {
    DocumentField *		rval= (DocumentField *)0;

    while( df )
	{
	int				bbcmp;
	int				ebcmp;
	int				becmp;
	int				eecmp;

	int				overlap;
	int				inside; /* sel inside field */
	int				readonly;
	EditPosition			fieldTailPosition= df->dfTailPosition;

	fieldTailPosition.epStroff++;

	readonly= ! DOC_FieldKinds[df->dfKind].fkiResultEditable;
	bbcmp= docCompareEditPositions( &(df->dfHeadPosition), epHead );
	ebcmp= docCompareEditPositions( &fieldTailPosition, epHead );
	becmp= docCompareEditPositions( &(df->dfHeadPosition), epTail );
	eecmp= docCompareEditPositions( &fieldTailPosition, epTail );

	overlap= ebcmp >  0 && becmp <  0;
	inside = bbcmp <= 0 && eecmp >= 0;

	/********************************************************/
	/*  Field protrudes to the left and it is readonly.	*/
	/*  or							*/
	/*  Partial overlap: Field protrudes to the left and	*/
	/*  it ends inside the selection.			*/
	/*  ....ssss						*/
	/*  ..ffff..						*/
	/*  or							*/
	/*  Also include fields that have the same extent as	*/
	/*  the selection. This approach avoids issues with	*/
	/*  completely empty fields in the document.		*/
	/********************************************************/
	if  ( ( overlap && readonly )			||
	      ( overlap && ! inside && bbcmp <= 0 )	||
	      ( bbcmp == 0 && eecmp == 0 )		)
	    { rval= df;		}

	df= df->dfParent;
	}

    return rval;
    }

static DocumentField * docGetRightOuterField(
				    DocumentField *	df,
				    const EditPosition *	epHead,
				    const EditPosition *	epTail )
    {
    DocumentField *		rval= (DocumentField *)0;

    while( df )
	{
	int				bbcmp;
	int				ebcmp;
	int				becmp;
	int				eecmp;

	int				overlap;
	int				inside; /* sel inside field */
	int				readonly;
	EditPosition			fieldTailPosition= df->dfTailPosition;

	fieldTailPosition.epStroff++;

	readonly= ! DOC_FieldKinds[df->dfKind].fkiResultEditable;
	bbcmp= docCompareEditPositions( &(df->dfHeadPosition), epHead );
	ebcmp= docCompareEditPositions( &fieldTailPosition, epHead );
	becmp= docCompareEditPositions( &(df->dfHeadPosition), epTail );
	eecmp= docCompareEditPositions( &fieldTailPosition, epTail );

	overlap= ebcmp >  0 && becmp <  0;
	inside = bbcmp <= 0 && eecmp >= 0;

	/********************************************************/
	/*  Field protrudes to the right and it is readonly.	*/
	/*  or							*/
	/*  Partial overlap: Field protrudes to the right and	*/
	/*  it begins inside the selection.			*/
	/*  ssss....						*/
	/*  ..ffff..						*/
	/*  or							*/
	/*  Also include fields that have the same extent as	*/
	/*  the selection. This approach avoids issues with	*/
	/*  completely empty fields in the document.		*/
	/********************************************************/
	if  ( ( overlap && readonly )			||
	      ( overlap && ! inside && eecmp >= 0 )	||
	      ( bbcmp == 0 && eecmp == 0 )		)
	    { rval= df;		}

	df= df->dfParent;
	}

    return rval;
    }

int docBalanceFieldSelection(	DocumentField **	pLeftField,
				DocumentField **	pRightField,
				int *			pHeadMoved,
				int *			pTailMoved,
				int *			pHeadPart,
				int *			pTailPart,
				DocumentPosition *	dpHead,
				DocumentPosition *	dpTail,
				struct DocumentTree *	dt,
				struct BufferDocument *	bd )
    {
    int				rval= 0;
    int				leftEditable= 0;

    EditPosition		epHead;
    EditPosition		epTail;

    DocumentField *		dfHead;
    DocumentField *		dfTail;

    DocumentField *		dfRight= (DocumentField *)0;
    DocumentField *		dfLeft= (DocumentField *)0;

    int				headMoved= 0;
    int				tailMoved= 0;

    const DocumentField **	pathHead= (const DocumentField **)0;
    const DocumentField **	pathTail= (const DocumentField **)0;

    DocumentSelection		dsInsideB;
    DocumentSelection		dsAroundB;

    docSetRangeSelection( &dsInsideB, dpHead, dpTail, 1 );

    docSetEditPosition( &epHead, dpHead );
    docSetEditPosition( &epTail, dpTail );

    dfHead= docFindChildField( &(dt->dtRootFields), &epHead, 1 );
    if  ( dfHead )
	{
	dfLeft= docGetLeftOuterField( dfHead, &epHead, &epTail );
	if  ( dfLeft )
	    {
	    int					headPart= -1;
	    int					tailPart= -1;

	    DocumentSelection			dsInside;
	    DocumentSelection			dsAround;

	    const FieldKindInformation *	fki;

	    fki= DOC_FieldKinds+ dfLeft->dfKind;
	    leftEditable= fki->fkiResultEditable;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					&headPart, &tailPart, bd, dfLeft ) )
		{ LDEB(1); rval= -1; goto ready; }

	    headMoved= 1;
	    dsInsideB.dsHead= dsInside.dsHead;
	    dsAroundB.dsHead= dsAround.dsHead;
	    if  ( pHeadPart )
		{ *pHeadPart= headPart;	}
	    }
	}

    dfTail= docFindChildField( &(dt->dtRootFields), &epTail, 0 );
    if  ( dfTail )
	{
	dfRight= docGetRightOuterField( dfTail, &epHead, &epTail );
	if  ( dfRight )
	    {
	    int			headPart= -1;
	    int			tailPart= -1;

	    DocumentSelection	dsInside;
	    DocumentSelection	dsAround;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					&headPart, &tailPart, bd, dfRight ) )
		{ LDEB(1); rval= -1; goto ready; }

	    tailMoved= 1;
	    dsInsideB.dsTail= dsInside.dsTail;
	    dsAroundB.dsTail= dsAround.dsTail;
	    if  ( pTailPart )
		{ *pTailPart= tailPart;	}
	    }
	}

    if  ( dfLeft && dfLeft == dfRight && leftEditable )
	{
	if  ( headMoved )
	    { *dpHead= dsInsideB.dsHead;	}
	if  ( tailMoved )
	    { *dpTail= dsInsideB.dsTail;	}
	}
    else{
	if  ( headMoved )
	    { *dpHead= dsAroundB.dsHead;	}
	if  ( tailMoved )
	    { *dpTail= dsAroundB.dsTail;	}
	}

    if  ( pLeftField )
	{ *pLeftField= dfLeft;	}
    if  ( pRightField )
	{ *pRightField= dfRight;	}
    *pHeadMoved= headMoved;
    *pTailMoved= tailMoved;

  ready:

    if  ( pathHead )
	{ free( pathHead );	}
    if  ( pathTail )
	{ free( pathTail );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Count the number of particules inside a text level field. The	*/
/*  begin and end particules are not counted.				*/
/*									*/
/*  1)  Do some sanity checks on the proper nesting of fields.. Just	*/
/*	to have a safe feeling.						*/
/*									*/
/************************************************************************/

int docCountParticulesInFieldFwd( const struct BufferItem *	paraNode,
				int *				pClosed,
				int				part,
				int				partUpto )
    {
    int				fieldLevel= 0;
    int				partCount= 0;
    int				fieldNumber;

    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ part;
    if  ( tp->tpKind != TPkindFIELDHEAD )
	{ LSDEB(tp->tpKind,docFieldKindStr(tp->tpKind)); return -1;	}
    fieldNumber= tp->tpObjectNumber;

    tp++; part++;
    while( part < partUpto )
	{
	if  ( tp->tpKind == TPkindFIELDHEAD )
	    {
	    /*  1  */
	    if  ( tp->tpObjectNumber == fieldNumber )
		{ LLDEB(tp->tpObjectNumber,fieldNumber);	}

	    fieldLevel++;
	    }

	if  ( tp->tpKind == TPkindFIELDTAIL )
	    {
	    if  ( tp->tpObjectNumber == fieldNumber )
		{
		/*  1 
		if  ( fieldLevel != 0 )
		    { LDEB(fieldLevel);	}
		*/

		*pClosed= 1;
		return partCount;
		}

	    fieldLevel--;
	    }

	partCount++, part++; tp++;
	}

    /*  1  */
    *pClosed= 0;
    return partCount;
    }

int docCountParticulesInFieldBwd( const struct BufferItem *	paraNode,
				int *				pClosed,
				int				part,
				int				partFrom )
    {
    int				fieldLevel= 0;
    int				partCount= 0;
    int				fieldNumber;

    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ part;
    if  ( tp->tpKind != TPkindFIELDTAIL )
	{ LSDEB(tp->tpKind,docFieldKindStr(tp->tpKind)); return -1;	}
    fieldNumber= tp->tpObjectNumber;

    tp--; part--;
    while( part >= partFrom )
	{
	if  ( tp->tpKind == TPkindFIELDHEAD )
	    {
	    /*  1  */
	    if  ( tp->tpObjectNumber == fieldNumber )
		{ LLDEB(tp->tpObjectNumber,fieldNumber);	}

	    fieldLevel++;
	    }

	if  ( tp->tpKind == TPkindFIELDTAIL )
	    {
	    if  ( tp->tpObjectNumber == fieldNumber )
		{
		/*  1 
		if  ( fieldLevel != 0 )
		    { LDEB(fieldLevel);	}
		*/

		*pClosed= 1;
		return partCount;
		}

	    fieldLevel--;
	    }

	partCount++, part--; tp--;
	}

    /*  1  */
    *pClosed= 0;
    return partCount;
    }

/************************************************************************/
/*									*/
/*  Find the particules that delimit a field.				*/
/*									*/
/************************************************************************/

static int docFindFieldParticules(
				int *				pPart0,
				int *				pPart1,
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				const DocumentSelection *	dsField,
				const DocumentField *		df )
    {
    int				part0;
    int				part1;
    int				flags;

    TextParticule *		tp;

    /*  2  */
    if  ( docFindParticuleOfPosition( &part0, (int *)0,
					&(dsField->dsHead), PARAfindLAST ) )
	{ LDEB(1); return -1;	}

    if  ( dsInside )
	{ *dsInside= *dsField;	}
    if  ( dsAround )
	{ *dsAround= *dsField;	}

    tp= dsField->dsHead.dpNode->biParaParticules+ part0;

    if  ( part0 >= dsField->dsHead.dpNode->biParaParticuleCount	||
	  tp->tpStroff != dsField->dsHead.dpStroff		||
	  tp->tpKind != TPkindFIELDHEAD				||
	  tp->tpObjectNumber != df->dfFieldNumber		)
	{
	LLDEB(part0,dsField->dsHead.dpNode->biParaParticuleCount);
	LSLDEB(tp->tpKind,docFieldKindStr(df->dfKind),TPkindFIELDHEAD);
	LLDEB(tp->tpObjectNumber,df->dfFieldNumber);
	docListNode((const struct BufferDocument *)0,0,dsField->dsHead.dpNode,0);
	return -1;
	}

    if  ( dsAround )
	{ dsAround->dsHead.dpStroff= tp->tpStroff;			}
    if  ( dsInside )
	{ dsInside->dsHead.dpStroff= tp->tpStroff+ tp->tpStrlen;	}

    /*  3  */
    flags= 0;
    if  ( docFindParticuleOfPosition( &part1, &flags,
					&(dsField->dsTail), PARAfindLAST ) )
	{ LDEB(1); return -1;	}

    tp= dsField->dsTail.dpNode->biParaParticules+ part1;

    if  ( part1 < 0						||
	  part1 >= dsField->dsTail.dpNode->biParaParticuleCount	||
	  tp->tpKind != TPkindFIELDTAIL				||
	  tp->tpObjectNumber != df->dfFieldNumber		)
	{
	LLDEB(part1,dsField->dsTail.dpNode->biParaParticuleCount);
	LLDEB(tp->tpKind,TPkindFIELDTAIL);
	LLDEB(tp->tpObjectNumber,df->dfFieldNumber);
	LLDEB(df->dfTailPosition.epParaNr,df->dfTailPosition.epStroff);
	return -1;
	}

    if  ( tp->tpStroff != dsField->dsTail.dpStroff )
	{
	LLLDEB(part1,tp->tpStroff,dsField->dsTail.dpStroff);
	LLLDEB(df->dfFieldNumber,df->dfTailPosition.epParaNr,
						df->dfTailPosition.epStroff);
	docListNode((const struct BufferDocument *)0,0,dsField->dsTail.dpNode,0);
	return -1;
	}

    if  ( dsInside )
	{ dsInside->dsTail.dpStroff= tp->tpStroff;			}
    if  ( dsAround )
	{ dsAround->dsTail.dpStroff= tp->tpStroff+ tp->tpStrlen;	}

    if  ( pPart0 )
	{ *pPart0= part0;	}
    if  ( pPart1 )
	{ *pPart1= part1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit a field in a document.					*/
/*									*/
/*  1)  Find selection.							*/
/*  2)  Find start particule.						*/
/*  3)  Find end particule.						*/
/*									*/
/************************************************************************/

int docDelimitFieldInDoc(	DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pPart0,
				int *				pPart1,
				const struct BufferDocument *	bd,
				const DocumentField *		df )
    {
    DocumentSelection	dsField;

    /*  1  */
    if  ( docSelectionForEditPositionsInDoc( &dsField, bd,
						&(df->dfSelectionScope),
						&(df->dfHeadPosition),
						&(df->dfTailPosition) ) )
	{ LDEB(1); return -1;	}

    if  ( dsField.dsHead.dpStroff != df->dfHeadPosition.epStroff )
	{
	LLDEB(dsField.dsHead.dpStroff,df->dfHeadPosition.epStroff);
	return -1;
	}

    if  ( dsField.dsTail.dpStroff != df->dfTailPosition.epStroff )
	{
	LLDEB(dsField.dsTail.dpStroff,df->dfTailPosition.epStroff);
	return -1;
	}

    /*  2  */
    if  ( ! pPart0 && ! pPart1 && ! dsInside && ! dsAround )
	{ return 0;	}

    return docFindFieldParticules( pPart0, pPart1,
					    dsInside, dsAround, &dsField, df );
    }

int docDelimitFieldInTree(	DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pPart0,
				int *				pPart1,
				const struct DocumentTree *	tree,
				const DocumentField *		df )
    {
    DocumentSelection	dsField;

    /*  1  */
    if  ( docSelectionForEditPositionsInTree( &dsField, tree,
			    &(df->dfHeadPosition), &(df->dfTailPosition) ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    return docFindFieldParticules( pPart0, pPart1,
					    dsInside, dsAround, &dsField, df );
    }

/************************************************************************/
/*									*/
/*  Delimit the field that must be at the head of a paragraph.		*/
/*  - The list text field in a paragraph that is part of a list.	*/
/*  - The note number at the head of a foot/endnote.			*/
/*									*/
/************************************************************************/

int docDelimitParaHeadField(	DocumentField **		pDfHead,
				DocumentSelection *		dsInsideHead,
				DocumentSelection *		dsAroundHead,
				int *				pHeadPart,
				int *				pTailPart,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd )
    {
    int				part;
    const TextParticule *	tp;
    int				fieldKind;

    fieldKind= docParaHeadFieldKind( paraNode );
    if  ( fieldKind < 0 )
	{ return 1;	}

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	DocumentField *	df;

	if  ( tp->tpKind == TPkindFIELDHEAD )
	    {
	    df= docGetFieldByNumber( bd, tp->tpObjectNumber );
	    if  ( ! df )
		{ LXDEB(tp->tpObjectNumber,df); continue;	}

	    if  ( df->dfKind == fieldKind )
		{
		if  ( dsInsideHead || dsAroundHead || pHeadPart || pTailPart )
		    {
		    if  ( docDelimitFieldInDoc( dsInsideHead, dsAroundHead,
					    pHeadPart, pTailPart, bd, df ) )
			{ LDEB(1); return -1; }
		    }

		if  ( pDfHead )
		    { *pDfHead= df;	}

		return 0;
		}
	    }
	}

    return 1;
    }

