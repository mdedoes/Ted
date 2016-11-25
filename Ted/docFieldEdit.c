/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Editing and field balancing code.					*/
/*									*/
/*  Extend a selection such that it does not partially cover a text	*/
/*  level field. Besides find out whether the whole selection is inside	*/
/*  a field with a read-only result.					*/
/*									*/
/************************************************************************/

static int docTextFieldLevels(		int			textFieldLevel,
					int *			pReadonly,
					int *			pRonlyLevel,
					int *			pHigest,
					int *			pLowest,
					const BufferDocument *	bd,
					const BufferItem *	bi,
					int			part,
					int			partUpto )
    {
    const TextParticule *	tp;
    int				read_only= *pReadonly;
    int				readOnlyLevel= *pRonlyLevel;
    int				highest= *pHigest;
    int				lowest= *pLowest;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    if  ( partUpto > bi->biParaParticuleCount )
	{
	LLDEB(partUpto,bi->biParaParticuleCount);
	partUpto= bi->biParaParticuleCount;
	}

    tp= bi->biParaParticules+ part;
    while( part < partUpto )
	{
	const DocumentField *		df;
	const FieldKindInformation *	fki;

	if  ( lowest > textFieldLevel )
	    { lowest=  textFieldLevel;	}
	if  ( highest < textFieldLevel )
	    { highest=  textFieldLevel;	}

	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    if  ( tp->tpObjectNumber < 0			||
		  tp->tpObjectNumber >= dfl->dflFieldCount	)
		{ LLDEB(tp->tpObjectNumber,dfl->dflFieldCount);	}

	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( fki->fkiLevel == DOClevTEXT )
		{
		textFieldLevel++;
		if  ( ! fki->fkiResultEditable )
		    {
		    if  ( ! read_only )
			{ readOnlyLevel= textFieldLevel- 1;	}

		    read_only++;
		    }
		}
	    }

	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    if  ( tp->tpObjectNumber < 0			||
		  tp->tpObjectNumber >= dfl->dflFieldCount	)
		{ LLDEB(tp->tpObjectNumber,dfl->dflFieldCount);	}

	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( df->dfKind <  0			||
		  df->dfKind >= DOC_FieldKindCount	)
		{ LLDEB(df->dfKind,DOC_FieldKindCount);	}

	    if  ( fki->fkiLevel == DOClevTEXT )
		{
		textFieldLevel--;
		if  ( textFieldLevel < 0 )
		    { LDEB(textFieldLevel); textFieldLevel= 0;	}
		if  ( ! fki->fkiResultEditable )
		    {
		    read_only--;
		    if  ( read_only < 0 )
			{ LDEB(read_only); read_only= 0;	}
		    }
		}
	    }

	part++; tp++;
	}

    *pReadonly= read_only;
    *pRonlyLevel= readOnlyLevel;
    *pHigest= highest;
    *pLowest= lowest;

    return textFieldLevel;
    }

static int docFieldBalanceBegin(	int *			pMoved,
					DocumentPosition *	dp,
					const BufferDocument *	bd,
					int			beginLevel,
					int			lowest )
    {
    BufferItem *		bi= dp->dpBi;
    int				textFieldLevel= beginLevel;
    int				part;
    const TextParticule *	tp;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    part= dp->dpParticule;
    tp= bi->biParaParticules+ part;

    while( part > 0 )
	{
	const DocumentField *		df;
	const FieldKindInformation *	fki;

	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( fki->fkiLevel == DOClevTEXT )
		{
		textFieldLevel--;
		if  ( textFieldLevel < 0 )
		    { LDEB(textFieldLevel); textFieldLevel= 0;	}
		}
	    }

	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( fki->fkiLevel == DOClevTEXT )
		{ textFieldLevel++; }
	    }

	if  ( textFieldLevel <= lowest )
	    { break;	}

	part--; tp--;
	}

    if  ( dp->dpParticule != part )
	{ *pMoved= 1;	}
    dp->dpParticule= part;
    dp->dpStroff= tp->tpStroff;

    return 0;
    }

static int docFieldBalanceEnd(		int *			pMoved,
					DocumentPosition *	dp,
					const BufferDocument *	bd,
					int			endLevel,
					int			lowest )
    {
    BufferItem *		bi= dp->dpBi;
    int				textFieldLevel= endLevel;
    int				part;
    const TextParticule *	tp;

    const DocumentFieldList *	dfl= &(bd->bdFieldList);

    part= dp->dpParticule;
    tp= bi->biParaParticules+ part;

    while( part < bi->biParaParticuleCount )
	{
	const DocumentField *		df;
	const FieldKindInformation *	fki;

	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( fki->fkiLevel == DOClevTEXT )
		{ textFieldLevel++;	}
	    }

	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    df= dfl->dflFields+ tp->tpObjectNumber;
	    fki= DOC_FieldKinds+ df->dfKind;

	    if  ( fki->fkiLevel == DOClevTEXT )
		{
		textFieldLevel--;
		if  ( textFieldLevel < 0 )
		    { LDEB(textFieldLevel); textFieldLevel= 0;	}
		}
	    }

	if  ( textFieldLevel <= lowest )
	    { break;	}

	part++; tp++;
	}

    if  ( part >= bi->biParaParticuleCount )
	{
	part= bi->biParaParticuleCount- 1;
	tp= bi->biParaParticules+ part;
	}

    if  ( dp->dpParticule != part )
	{ *pMoved= 1;	}
    dp->dpParticule= part;
    dp->dpStroff= tp->tpStroff+ tp->tpStrlen;

    return 0;
    }

int docBalanceFieldSelection(	int *			pBeginMoved,
				int *			pEndMoved,
				const BufferDocument *	bd,
				DocumentPosition *	dpBegin,
				DocumentPosition *	dpEnd )
    {
    int				beginLevel;
    int				endLevel;
    int				read_only;
    int				readOnlyLevel;

    int				highest= 0;
    int				lowest;

    int				beginMoved= 0;
    int				endMoved= 0;

    lowest= INT_MAX;
    beginLevel= 0;
    read_only= 0;
    readOnlyLevel= -1;
    beginLevel= docTextFieldLevels( beginLevel, &read_only, &readOnlyLevel,
						&highest, &lowest,
						bd, dpBegin->dpBi,
						0,
						dpBegin->dpParticule+ 1 );

    if  ( dpBegin->dpBi == dpEnd->dpBi )
	{
	lowest= beginLevel;
	endLevel= docTextFieldLevels( beginLevel, &read_only, &readOnlyLevel,
						&highest, &lowest,
						bd, dpEnd->dpBi,
						dpBegin->dpParticule+ 1,
						dpEnd->dpParticule+ 1 );
	}
    else{
	lowest= INT_MAX;
	beginLevel= 0;
	read_only= 0;
	readOnlyLevel= -1;
	endLevel= docTextFieldLevels( beginLevel, &read_only, &readOnlyLevel,
						&highest, &lowest,
						bd, dpEnd->dpBi,
						0,
						dpEnd->dpParticule+ 1 );
	lowest= 0;
	}

    if  ( beginLevel > lowest )
	{
	if  ( docFieldBalanceBegin( &beginMoved, dpBegin, bd,
							beginLevel, lowest ) )
	    { LDEB(1); return -1;	}
	}

    if  ( endLevel > lowest )
	{
	if  ( docFieldBalanceEnd( &endMoved, dpEnd, bd, endLevel, lowest ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dpBegin->dpBi == dpEnd->dpBi	&&
	  read_only			)
	{
	lowest= INT_MAX;
	beginLevel= 0;
	read_only= 0;
	readOnlyLevel= -1;
	beginLevel= docTextFieldLevels( beginLevel, &read_only, &readOnlyLevel,
						&highest, &lowest,
						bd, dpBegin->dpBi,
						0,
						dpBegin->dpParticule );

	if  ( read_only )
	    {
	    if  ( docFieldBalanceBegin( &beginMoved, dpBegin, bd,
						beginLevel, readOnlyLevel ) )
		{ LDEB(1); return -1;	}

	    if  ( docFieldBalanceEnd( &endMoved, dpEnd, bd,
						endLevel, readOnlyLevel ) )
		{ LDEB(1); return -1;	}
	    }
	}

    *pBeginMoved= beginMoved;
    *pEndMoved= endMoved;
    return 0;
    }

static int docFieldCalculateBalance(	const BufferItem *	bi,
					int			part,
					int			partUpto )
    {
    int				fieldLevel= 0;
    const TextParticule *	tp;

    tp= bi->biParaParticules+ part;
    while( part < bi->biParaParticuleCount )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    { fieldLevel++;	}
	if  ( tp->tpKind == DOCkindFIELDEND )
	    { fieldLevel--;	}

	part++; tp++;
	}

    return fieldLevel;
    }

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

static int docFieldInsertEndParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpEnd,
				int *				pEndPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttributeNumber;

    int			endPart= dpEnd->dpParticule;
    BufferItem *	bi= dpEnd->dpBi;

    /*  1  */
    tp= bi->biParaParticules+ endPart;
    if  ( tp->tpStrlen > 0				&&
	  dpEnd->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ endPart++; tp++;	}

    if  ( endPart < bi->biParaParticuleCount )
	{ textAttributeNumber= tp[ 0].tpTextAttributeNumber; }
    else{ textAttributeNumber= tp[-1].tpTextAttributeNumber; }

    /*  2  */
    if  ( endPart < bi->biParaParticuleCount	&&
	  tp->tpStroff != dpEnd->dpStroff	)
	{
	int	len= tp->tpStroff+ tp->tpStrlen- dpEnd->dpStroff;

	tp->tpStrlen= dpEnd->dpStroff- tp->tpStroff;

	tp= docInsertTextParticule( bi, endPart+ 1,
		    dpEnd->dpStroff, len, tp->tpKind, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); return -1;	}

	endPart++;
	}

    /*  3  */
    tp= docInsertTextParticule( bi, endPart,
		    dpEnd->dpStroff, 0, DOCkindFIELDEND, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    /*  4  */
    if  ( taSetMask && ! PROPmaskISEMPTY( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (PropertyMask *)0,
						bd, bi, endPart, endPart+ 1,
						taSet, taSetMask ) )
	    { LDEB(endPart); return -1;	}
	}

    *pEndPart= endPart;
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

static int docFieldInsertStartParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpStart,
				int *				pStartPart,
				int *				pEndPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttributeNumber;

    int			endPart= *pEndPart;

    int			startPart;
    BufferItem *	bi= dpStart->dpBi;

    /*  1  */
    startPart= dpStart->dpParticule;
    tp= bi->biParaParticules+ startPart;
    if  ( tp->tpStrlen > 0					&&
	  dpStart->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ startPart++; tp++;	}

    if  ( startPart < bi->biParaParticuleCount )
	{ textAttributeNumber= tp[ 0].tpTextAttributeNumber; }
    else{ textAttributeNumber= tp[-1].tpTextAttributeNumber; }

    /*  2  */
    if  ( startPart < bi->biParaParticuleCount	&&
	  tp->tpStroff != dpStart->dpStroff	)
	{
	int		stroff= tp->tpStroff;
	int		len= dpStart->dpStroff- tp->tpStroff;

	tp->tpStrlen= tp->tpStroff+ tp->tpStrlen- dpStart->dpStroff;
	tp->tpStroff= dpStart->dpStroff;

	tp= docInsertTextParticule( bi, startPart,
				stroff, len, tp->tpKind, textAttributeNumber );
	if  ( ! tp )
	    { XDEB(tp); return -1;	}

	if  ( endPart >= startPart )
	    { endPart++;	}

	startPart++;
	}

    /*  3  */
    tp= docInsertTextParticule( bi, startPart,
		dpStart->dpStroff, 0, DOCkindFIELDSTART, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    /*  4  */
    if  ( taSetMask && ! PROPmaskISEMPTY( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (PropertyMask *)0, bd,
						bi, startPart, startPart+ 1,
						taSet, taSetMask ) )
	    { LDEB(endPart); return -1;	}
	}

    if  ( endPart >= startPart )
	{ endPart++;	}

    *pStartPart= startPart; *pEndPart= endPart; return 0;
    }

/************************************************************************/
/*									*/
/*  Surround a selected range of text by a field level field.		*/
/*									*/
/************************************************************************/

int docSurroundTextSelectionByField(
				BufferDocument *		bd,
				DocumentSelection *		dsField,
				const DocumentSelection *	ds,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    DocumentSelection	dsNew;
    int			fieldBalance;

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ XXDEB(ds->dsBegin.dpBi,ds->dsEnd.dpBi); return -1;	}

    dsNew= *ds;

    fieldBalance= docFieldCalculateBalance( dsNew.dsBegin.dpBi,
					    dsNew.dsBegin.dpParticule,
					    dsNew.dsEnd.dpParticule );
    if  ( fieldBalance != 0 )
	{
	LLDEB(dsNew.dsBegin.dpParticule,dsNew.dsEnd.dpParticule);
	LDEB(fieldBalance); return -1;
	}

    /*  find end, split?, insert end particule */
    if  ( docFieldInsertEndParticule( bd, &(dsNew.dsEnd),
					    &(dsNew.dsEnd.dpParticule),
					    fieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    /*  find begin, split?, insert start particule */
    if  ( docFieldInsertStartParticule( bd, &(dsNew.dsBegin),
					    &(dsNew.dsBegin.dpParticule),
					    &(dsNew.dsEnd.dpParticule),
					    fieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    if  ( ! PROPmaskISEMPTY( taSetMask ) )
	{
	PropertyMask	taDoneMask;

	PROPmaskCLEAR( &taDoneMask );

	if  ( docChangeParticuleAttributes( &taDoneMask, bd,
						ds->dsBegin.dpBi,
						dsNew.dsBegin.dpParticule,
						dsNew.dsEnd.dpParticule,
						taSet, taSetMask ) )
	    { LDEB(1);	}
	}

    *dsField= dsNew;

    return 0;
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

int docCountParticulesInField(		const BufferItem *	bi,
					int			part,
					int			partUpto )
    {
    int				fieldLevel= 0;
    int				partCount= 0;
    int				fieldNumber;

    const TextParticule *	tp;

    tp= bi->biParaParticules+ part;
    if  ( tp->tpKind != DOCkindFIELDSTART )
	{ LDEB(tp->tpKind); return -1;	}
    fieldNumber= tp->tpObjectNumber;

    tp++; part++;
    while( part < partUpto )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    /*  1  */
	    if  ( tp->tpObjectNumber == fieldNumber )
		{ LLDEB(tp->tpObjectNumber,fieldNumber);	}

	    fieldLevel++;
	    }

	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    if  ( tp->tpObjectNumber == fieldNumber )
		{
		/*  1 
		if  ( fieldLevel != 0 )
		    { LDEB(fieldLevel);	}
		*/

		return partCount;
		}

	    fieldLevel--;
	    }

	partCount++, part++; tp++;
	}

    /*  1  */
    LLDEB(fieldLevel,fieldNumber);
    docListItem(0,bi);
    return -1;
    }

