/************************************************************************/
/*									*/
/*  Manage field hierarchy.						*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<stdlib.h>
#   include	<limits.h>

#   include	"docDocumentField.h"
#   include	"docTreeType.h"
#   include	"docFieldScanner.h"
#   include	"docScanner.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a child field into a field or into the root of a document	*/
/*  tree.								*/
/*									*/
/************************************************************************/

static int docInsertFieldAt(		DocumentField *		dfPa,
					ChildFields *		cf,
					DocumentField *		dfCh,
					int			pos )
    {
    DocumentField **		fresh;
    int				i;

    fresh= (DocumentField **)realloc( cf->cfChildren,
			( cf->cfChildCount+ 1 )* sizeof(DocumentField *) );
    if  ( ! fresh )
	{ LXDEB(cf->cfChildCount,fresh); return -1;	}
    cf->cfChildren= fresh;

    for ( i= cf->cfChildCount; i > pos; i-- )
	{
	cf->cfChildren[i]= cf->cfChildren[i-1];
	cf->cfChildren[i]->dfNumberInParent= i;
	}
    cf->cfChildren[pos]= dfCh;
    cf->cfChildren[pos]->dfNumberInParent= pos;
    dfCh->dfParent= dfPa;

    cf->cfChildCount++;

    return 0;
    }

int docInsertChildField(		DocumentField *		dfPa,
					ChildFields *		cf,
					DocumentField *		dfCh )
    {
    int				pos;

    pos= cf->cfChildCount- 1;
    while( pos >= 0 && docCompareEditPositions(
				&(dfCh->dfHeadPosition),
				&(cf->cfChildren[pos]->dfTailPosition) ) < 0 )
	{ pos--;	}
    pos++;

    return docInsertFieldAt( dfPa, cf, dfCh, pos );
    }

/************************************************************************/
/*									*/
/*  Delete a field from its parent: Replace it with its children.	*/
/*  This is for child fields. NOT for instruction fields.		*/
/*									*/
/************************************************************************/

int docDeleteChildField(	ChildFields *		cf,
				DocumentField *		df )
    {
    int		pos= df->dfNumberInParent;

    if  ( df->dfNumberInParent < 0			||
	  df->dfNumberInParent >= cf->cfChildCount	)
	{
	LLLDEB(df->dfFieldNumber,df->dfNumberInParent,cf->cfChildCount);
	return -1;
	}

    if  ( df->dfResultFields.cfChildCount >= 1 )
	{
	int		i;
	int		step= df->dfResultFields.cfChildCount- 1;

	if  ( step > 0 )
	    {
	    DocumentField **	fresh;
	    int			newCount;

	    newCount= cf->cfChildCount+ step;

	    fresh= (DocumentField **)realloc( cf->cfChildren,
					newCount* sizeof(DocumentField *) );
	    if  ( ! fresh )
		{ LXDEB(newCount,fresh); return -1;	}
	    cf->cfChildren= fresh;

	    for ( i= cf->cfChildCount+ step- 1; i > pos+ step; i-- )
		{
		cf->cfChildren[i]= cf->cfChildren[i-step];
		cf->cfChildren[i]->dfNumberInParent= i;
		}
	    }

	for ( i= 0; i <= step; i++ ) /* <= !! */
	    {
	    cf->cfChildren[pos+ i]= df->dfResultFields.cfChildren[i];
	    cf->cfChildren[pos+ i]->dfNumberInParent= pos+ i;
	    cf->cfChildren[pos+ i]->dfParent= df->dfParent;
	    }

	cf->cfChildCount += step;
	}
    else{
	int			i;

	cf->cfChildCount--;

	for ( i= pos; i < cf->cfChildCount; i++ )
	    {
	    cf->cfChildren[i]= cf->cfChildren[i+1];
	    cf->cfChildren[i]->dfNumberInParent= i;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the innermost field for a position in the document tree.	*/
/*  If at a certain position there is more than one field, the first	*/
/*  one is returned.							*/
/*									*/
/*  This function looks complicated, but is not: Start in the root	*/
/*  fields of the document tree. As soon as you find a field that holds	*/
/*  the position remember the field and look for a child that also	*/
/*  holds the position. Eventually that returns the innermost field	*/
/*  that contains the position.						*/
/*									*/
/************************************************************************/

DocumentField * docFindChildField(	const ChildFields *	cf,
					const EditPosition *	ep,
					int			lastOne )
    {
    DocumentField *	dfFound= (DocumentField *)0;

    for (;;)
	{
	const int	count= cf->cfChildCount;
	int		i;

	if  ( lastOne )
	    {
	    for ( i= count- 1; i >= 0; i-- )
		{
		DocumentField *	df= cf->cfChildren[i];

		if  ( docEditPositionInField( df, ep ) )
		    {
		    dfFound= df;
		    cf= &(dfFound->dfResultFields);
		    break;
		    }
		}

	    if  ( i < 0 )
		{ break;	}
	    }
	else{
	    for ( i= 0; i < count; i++ )
		{
		DocumentField *	df= cf->cfChildren[i];

		if  ( docEditPositionInField( df, ep ) )
		    {
		    dfFound= df;
		    cf= &(dfFound->dfResultFields);
		    break;
		    }
		}

	    if  ( i >= count )
		{ break;	}
	    }
	}

    return dfFound;
    }

DocumentField * docFindTypedChildField(	const ChildFields *	cf,
					const EditPosition *	ep,
					int			kind )
    {
    const int		lastOne= 1;
    DocumentField *	df= docFindChildField( cf, ep, lastOne );

    while( df && df->dfKind != kind )
	{ df= df->dfParent;	}

    return df;
    }

/************************************************************************/
/*									*/
/*  Find a field with a certain kind inside a range.			*/
/*									*/
/************************************************************************/

DocumentField * docFindFieldInRange(	const EditRange *	er,
					const ChildFields *	cf,
					int			lastOne,
					int			kind )
    {
    int		pos;

    if  ( lastOne )
	{
	for ( pos= cf->cfChildCount- 1; pos >= 0; pos-- )
	    {
	    DocumentField *		dfC= cf->cfChildren[pos];
	    const EditPosition *	epHead= &(dfC->dfHeadPosition);
	    const EditPosition *	epTail= &(dfC->dfTailPosition);
	    DocumentField *		df;

	    if  ( docCompareEditPositions( epHead, &(er->erTail) ) > 0 )
		{ continue;	}
	    if  ( docCompareEditPositions( epTail, &(er->erHead) ) < 0 )
		{ break;	}

	    if  ( docCompareEditPositions( epHead, &(er->erHead) ) >= 0	&&
		  docCompareEditPositions( epTail, &(er->erTail) ) <= 0	&&
		  ( dfC->dfKind == kind || kind < 0 )			)
		{ return dfC;	}

	    df= docFindFieldInRange( er, &(dfC->dfResultFields), lastOne, kind );
	    if  ( df )
		{ return df;	}
	    }
	}
    else{
	for ( pos= 0; pos < cf->cfChildCount; pos++ )
	    {
	    DocumentField *		dfC= cf->cfChildren[pos];
	    const EditPosition *	epHead= &(dfC->dfHeadPosition);
	    const EditPosition *	epTail= &(dfC->dfTailPosition);
	    DocumentField *		df;

	    if  ( docCompareEditPositions( epTail, &(er->erHead) ) < 0 )
		{ continue;	}
	    if  ( docCompareEditPositions( epHead, &(er->erTail) ) > 0 )
		{ break;	}

	    if  ( docCompareEditPositions( epHead, &(er->erHead) ) >= 0	&&
		  docCompareEditPositions( epTail, &(er->erTail) ) <= 0	&&
		  ( dfC->dfKind == kind || kind < 0 )			)
		{ return dfC;	}

	    df= docFindFieldInRange( er, &(dfC->dfResultFields), lastOne, kind );
	    if  ( df )
		{ return df;	}
	    }
	}

    return (DocumentField *)0;
    }

/************************************************************************/
/*									*/
/*  Remember the document position of the end of a field.		*/
/*									*/
/************************************************************************/

void docSetFieldTail(		DocumentField *		dfPa,
				const EditPosition *	epTail )
    {
    dfPa->dfTailPosition= *epTail;

    if  ( dfPa->dfResultFields.cfChildCount > 0 )
	{
	DocumentField *	dfCh;

	dfCh= dfPa->dfResultFields.cfChildren[
					dfPa->dfResultFields.cfChildCount- 1];

	if  ( docCompareEditPositions( &(dfCh->dfTailPosition),
						&(dfPa->dfTailPosition) ) >= 0 )
	    { LDEB(1);	}
	}

    return;
    }

void docSetFieldHead(		DocumentField *		dfPa,
				const EditPosition *	epHead )
    {
    dfPa->dfHeadPosition= *epHead;
    }

/************************************************************************/
/*  									*/
/*  Keep track of the hierarchy of fields. Do some elementary checks	*/
/*									*/
/*  NOTE: The stacked construction of fields makes that children are	*/
/*	added to the parent before the end position of the parent is	*/
/*	set: Do not verify the end position.				*/
/*  									*/
/************************************************************************/

int docAddResultChildToField(		DocumentField *		dfPa,
					DocumentField *		dfCh )
    {
    if  ( ! docSelectionSameScope( &(dfCh->dfSelectionScope),
					&(dfPa->dfSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( docCompareEditPositions( &(dfCh->dfHeadPosition),
					&(dfPa->dfHeadPosition) ) < 0 )
	{ LDEB(1); return -1;	}
    /* NO!
    if  ( docCompareEditPositions( &(dfCh->dfTailPosition),
					&(dfPa->dfTailPosition) ) > 0 )
	{ LDEB(1); return -1;	}
    */

    if  ( docInsertChildField( dfPa, &(dfPa->dfResultFields), dfCh ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docAddInstructionsChildToField(	DocumentField *		dfPa,
					DocumentField *		dfCh )
    {
    if  ( dfCh->dfSelectionScope.ssTreeType != DOCinFIELD_INSTRUCTIONS )
	{ LDEB(dfCh->dfSelectionScope.ssTreeType); return -1;	}

    return docInsertFieldAt( dfPa, &(dfPa->dfInstructionFields), dfCh,
				    dfPa->dfInstructionFields.cfChildCount );
    }

int docInsertFieldInTree(		ChildFields *		cf,
					DocumentField *		df )
    {
    DocumentField *	parent= (DocumentField *)0;

    if  ( df->dfResultFields.cfChildCount > 0 )
	{ LDEB(df->dfResultFields.cfChildCount); return -1;	}

    for (;;)
	{
	int		i0;
	int		i1;

	/* Find the last field that is completely before the insert	*/
	i0= 0;
	while( i0 < cf->cfChildCount )
	    {
	    DocumentField *	df0= cf->cfChildren[i0];

	    if  ( docCompareEditPositions( &(df->dfTailPosition),
					    &(df0->dfHeadPosition) ) <= 0 )
		{ break;	}

	    i0++;
	    }
	i0--;

	/* Find the first field that is completely after the insert	*/
	i1= cf->cfChildCount- 1;
	while( i1 >= 0 )
	    {
	    DocumentField *	df1= cf->cfChildren[i1];

	    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					    &(df1->dfTailPosition) ) >= 0 )
		{ break;	}

	    i1--;
	    }
	i1++;

	/* between fields */
	if  ( i1 == i0+ 1 )
	    {
	    return docInsertFieldAt( parent, cf, df, i1 );
	    }

	/* inside a field: continue there */
	if  ( i0 == i1 )
	    {
	    DocumentField *	df1= cf->cfChildren[i1];

	    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					&(df1->dfHeadPosition) ) >= 0	&&
		  docCompareEditPositions( &(df->dfTailPosition),
					&(df1->dfTailPosition) ) <= 0	)
		{
		cf= &(df1->dfResultFields);
		parent= df1;
		continue;
		}
	    }

	/* Surround the fields found with the insert */
	if  ( i1 <= i0 )
	    {
	    int		i;
	    int		pos;
	    int		step;

	    pos= 0;
	    for ( i= i1; i <= i0; i++ )
		{
		if  ( docInsertFieldAt( df, &(df->dfResultFields),
						cf->cfChildren[i], pos++ ) )
		    { LDEB(pos); return -1;	}
		}
	    step= i0- i1+ 1;
	    cf->cfChildCount -= step;
	    i= i1;
	    while( i < cf->cfChildCount )
		{ cf->cfChildren[i]= cf->cfChildren[i+ step]; i++; }

	    return docInsertFieldAt( parent, cf, df, i1 );
	    }

	return docInsertFieldAt( parent, cf, df, i1 );
	}
    }

/************************************************************************/

static int docFieldPath(	DocumentField ***	pPath,
				DocumentField *	dfTo )
    {
    DocumentField *	df;
    int			deep= 0;
    DocumentField **	path= (DocumentField **)0;

    df= dfTo;
    while( df )
	{ deep++; df= df->dfParent;	}

    if  ( deep > 0 )
	{
	int		d= deep;

	path= (DocumentField **)malloc( deep* sizeof(DocumentField *) );
	if  ( ! path )
	    { LXDEB(deep,path); return -1;	}

	df= dfTo;
	while( df )
	    { path[--d]= df; df= df->dfParent;	}
	}

    *pPath= path;
    return deep;
    }

DocumentField *	docFieldGetCommonParent(	DocumentField *	dfHead,
						DocumentField *	dfTail )
    {
    DocumentField *	rval= (DocumentField *)0;
    DocumentField **	pathHead= (DocumentField **)0;
    DocumentField **	pathTail= (DocumentField **)0;

    int			deepHead;
    int			deepTail;
    int			deep;

    deepHead= docFieldPath( &pathHead, dfHead );
    deepTail= docFieldPath( &pathTail, dfTail );
    if  ( deepHead < 0 || deepTail < 0 )
	{ LLDEB(deepHead,deepTail); goto ready;	}

    if  ( pathHead && pathTail ) /* To make gcc -fanalyzer happy */
	{
	for ( deep= 0; deep < deepHead && deep < deepTail; deep++ )
	    {
	    DocumentField *	df;

	    if  ( pathHead[deep] != pathTail[deep] )
		{ break;	}

	    df= pathHead[deep];
	    if  ( df == dfHead )
		{ break;	}
	    if  ( df == dfTail )
		{ break;	}

	    rval= df;
	    }
	}

  ready:

    if  ( pathHead )
	{ free( pathHead );	}
    if  ( pathTail )
	{ free( pathTail );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Scan fields.							*/
/*									*/
/*  1)  Look for the first child that does not end before the range.	*/
/*  2)  Stop if the field begins after the end of the range.		*/
/*									*/
/************************************************************************/

int docScanFieldsInRange(	const ChildFields *	cf,
				const EditRange * 	er,
				TreeFieldVisitor	enterField,
				TreeFieldVisitor	leaveField,
				void *			through )
    {
    int		m= 0;

    if  ( ! cf )
	{ XDEB(cf); return SCANadviceOK;	}

    if  ( cf->cfChildCount == 0 )
	{ return SCANadviceOK;	}

    /*  1  */
    if  ( er )
	{
	DocumentField *	dfm;
	int		l;
	int		r;

	int		cmp;

	l= 0; r= cf->cfChildCount; m= ( l+ r )/ 2;
	while( l < m )
	    {
	    dfm= cf->cfChildren[m];

	    cmp= docCompareEditPositions(
				&(dfm->dfTailPosition), &(er->erHead) );

	    if  ( cmp < 0 )
		{ l= m;	}
	    else{ r= m;	}

	    m= ( l+ r )/ 2;
	    }

	dfm= cf->cfChildren[m];
	cmp= docCompareEditPositions(
				&(dfm->dfTailPosition), &(er->erHead) );
	if  ( cmp < 0 )
	    { m++;	}
	}

    for ( ; m < cf->cfChildCount; m++ )
	{
	DocumentField *	dfc= cf->cfChildren[m];
	int		res;

	/*  2  */
	if  ( er )
	    {
	    int cmp;
	    
	    cmp= docCompareEditPositions(
				&(dfc->dfHeadPosition), &(er->erTail) );
	    if  ( cmp > 0 )
		{ break;	}
	    }

	if  ( dfc->dfFieldNumber < 0 )
	    { LDEB(dfc->dfFieldNumber); continue;	}

	if  ( enterField )
	    {
	    res= (*enterField)( dfc, er, through );

	    switch( res )
		{
		case SCANadviceOK:
		    break;
		case SCANadviceSKIP:
		    continue;
		case SCANadviceSTOP:
		    return res;
		default:
		    LDEB(res); return -1;
		}
	    }

	if  ( dfc->dfResultFields.cfChildCount > 0 )
	    {
	    res= docScanFieldsInRange( &(dfc->dfResultFields), er,
					enterField, leaveField, through );
	    switch( res )
		{
		case SCANadviceOK:
		    break;
		case SCANadviceSKIP:
		    LDEB(res); break;
		case SCANadviceSTOP:
		    return res;
		default:
		    LDEB(res); return -1;
		}
	    }

	if  ( leaveField )
	    {
	    res= (*leaveField)( dfc, er, through );

	    switch( res )
		{
		case SCANadviceOK:
		    break;
		case SCANadviceSKIP:
		    LDEB(res); break;
		case SCANadviceSTOP:
		    return res;
		default:
		    LDEB(res); return -1;
		}
	    }
	}

    return SCANadviceOK;
    }

/************************************************************************/
/*									*/
/*  Shift fields in an edit operation.					*/
/*  NOTE that we depend on the paragraph number only for shifting the	*/
/*  section number: Only the body has more than one section and there	*/
/*  is a colose relationship in that case.				*/
/*									*/
/*  1)	For (BODY) fields that span sections, the section number is	*/
/*	that of the head position of the field.				*/
/*									*/
/************************************************************************/

typedef struct ContentShift
    {
    int		csSectFrom;

    int		csSectShift;
    int		csParaShift;
    int		csStroffShift;
    } ContentShift;

static void docShiftFieldStart(		DocumentField *		df,
					const EditPosition *	epFrom,
					const ContentShift *	cs )
    {
    /*  earlier paragraph  */
    if  ( df->dfHeadPosition.epParaNr < epFrom->epParaNr )
	{ return;	}

    /*  later paragraph  */
    if  ( df->dfHeadPosition.epParaNr > epFrom->epParaNr )
	{
	df->dfHeadPosition.epParaNr += cs->csParaShift;
	/*  1  */
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += cs->csSectShift;	}

	return;
	}

    /* Same paragraph */
    if  ( df->dfHeadPosition.epStroff < epFrom->epStroff )
	{ return;	}
    if  ( df->dfHeadPosition.epStroff >= epFrom->epStroff )
	{
	df->dfHeadPosition.epParaNr += cs->csParaShift;
	df->dfHeadPosition.epStroff += cs->csStroffShift;
	/*  1  */
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += cs->csSectShift;	}

	return;
	}

    return;
    }

static void docShiftFieldEnd(		DocumentField *		df,
					const EditPosition *	epFrom,
					const ContentShift *	cs )
    {
    /*  earlier paragraph  */
    if  ( df->dfTailPosition.epParaNr < epFrom->epParaNr )
	{ return;	}

    /*  later paragraph  */
    if  ( df->dfTailPosition.epParaNr > epFrom->epParaNr )
	{
	df->dfTailPosition.epParaNr += cs->csParaShift;
	/*  1 NO: See comment above
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += cs->csSectShift;	}
	*/

	return;
	}

    /* Same paragraph */
    if  ( df->dfTailPosition.epStroff < epFrom->epStroff )
	{ return;	}
    if  ( df->dfTailPosition.epStroff >= epFrom->epStroff )
	{
	df->dfTailPosition.epParaNr += cs->csParaShift;
	df->dfTailPosition.epStroff += cs->csStroffShift;
	/*  1
	if  ( df->dfSelectionScope.ssTreeType == DOCinBODY )
	    { df->dfSelectionScope.ssSectNr += cs->csSectShift;	}
	*/

	return;
	}

    return;
    }

static int docShiftField(		DocumentField *		df,
					const EditRange *	er,
					void *			through )
    {
    ContentShift *	cs= (ContentShift *)through;

    if  ( df->dfSelectionScope.ssTreeType == DOCinBODY			&&
	  df->dfSelectionScope.ssOwnerSectNr >= 0			&&
	  df->dfSelectionScope.ssOwnerSectNr > cs->csSectFrom		)
	{ df->dfSelectionScope.ssOwnerSectNr += cs->csSectShift;	}

    docShiftFieldStart( df, &(er->erHead), cs );
    docShiftFieldEnd( df, &(er->erHead), cs );

    return SCANadviceOK;
    }

void docShiftChildFieldReferences(	const ChildFields *	cf,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    EditRange		er;
    ContentShift	cs;

    er.erHead.epParaNr= paraFrom;
    er.erHead.epStroff= stroffFrom;
    er.erTail.epParaNr= INT_MAX;
    er.erTail.epStroff= INT_MAX;

    cs.csSectFrom= sectFrom;
    cs.csSectShift= sectShift;
    cs.csParaShift= paraShift;
    cs.csStroffShift= stroffShift;

    if  ( docScanFieldsInRange( cf, &er,
			docShiftField, (TreeFieldVisitor)0, (void *)&cs ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the parent that is in the document: I.E: That is not in the	*/
/*  field instructions.							*/
/*									*/
/************************************************************************/

const DocumentField * docGetLocationParent(
					const DocumentField *	df )
    {
    while( df && df->dfSelectionScope.ssTreeType == DOCinFIELD_INSTRUCTIONS )
	{ df= df->dfParent;	}

    if  ( ! df )
	{ XDEB(df);	}

    return df;
    }

