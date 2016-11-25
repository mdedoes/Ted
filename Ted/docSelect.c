/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a BufferPosition/BufferSelection.			*/
/*									*/
/************************************************************************/

void docInitSelectionScope(	SelectionScope *	ss )
    {
    ss->ssInExternalItem= DOCinUNKNOWN;
    ss->ssSectNrExternalTo= -1;
    ss->ssNoteArrayIndex= -1;
    }

void docInitDocumentPosition(	DocumentPosition *	dp )
    {
    dp->dpBi= (BufferItem *)0;
    dp->dpParticule= -1;
    dp->dpStroff= -1;

    return;
    }

void docInitDocumentSelection(	DocumentSelection *	ds )
    {
    docInitSelectionScope( &(ds->dsSelectionScope) );

    docInitDocumentPosition( &(ds->dsBegin) );
    docInitDocumentPosition( &(ds->dsEnd) );
    docInitDocumentPosition( &(ds->dsAnchor) );

    ds->dsCol0= -1;
    ds->dsCol1= -1;

    ds->dsDirection= 0;

    return;
    }

void docSetIBarSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dp )
    {
    docSetSelectionScope( ds, dp->dpBi );

    ds->dsBegin= *dp;
    ds->dsEnd= *dp;
    ds->dsAnchor= *dp;

    ds->dsCol0= -1;
    ds->dsCol1= -1;
    ds->dsDirection= 0;

    return;
    }

void docSetRangeSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const DocumentPosition *	dpTo,
				int				direction,
				int				col0,
				int				col1 )
    {
    docInitDocumentSelection( ds );

    docSetSelectionScope( ds, dpFrom->dpBi );

    ds->dsBegin= *dpFrom;
    ds->dsEnd= *dpTo;
    ds->dsDirection= direction;

    ds->dsCol0= col0;
    ds->dsCol1= col1;
    
    if  ( direction >= 0 )
	{ ds->dsAnchor= *dpFrom;	}
    else{ ds->dsAnchor= *dpTo;		}

    return;
    }

int docIsIBarSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsBegin.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpStroff != ds->dsEnd.dpStroff )
	{ return 0;	}

    return 1;
    }

int docIsParaSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsBegin.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ return 0;	}

    return 1;
    }

void docInitPositionGeometry(	PositionGeometry *		pg )
    {
    pg->pgLine= 0;

    pg->pgXPixels= -1;
    docInitLayoutPosition( &pg->pgTopPosition );
    pg->pgY1Pixels= -1;
    pg->pgBaselinePixels= -1;

    return;
    }

void docInitSelectionGeometry(	SelectionGeometry *		sg )
    {
    docInitRectangle( &(sg->sgRectangle) );

    docInitPositionGeometry( &(sg->sgBegin) );
    docInitPositionGeometry( &(sg->sgEnd) );
    docInitPositionGeometry( &(sg->sgAnchor) );
    }

/************************************************************************/
/*									*/
/*  Reduce a selection to one paragraph.				*/
/*									*/
/************************************************************************/

void docConstrainSelectionToOneParagraph( int *			pBeginMoved,
					int *			pEndMoved,
					DocumentSelection *	ds )
    {
    int		lenBegin;
    int		lenEnd;

    if  ( docIsParaSelection( ds ) )
	{ return;	}

    lenBegin= ds->dsBegin.dpBi->biParaStrlen- ds->dsBegin.dpStroff;
    lenEnd= ds->dsEnd.dpStroff;

    if  ( lenEnd > lenBegin )
	{
	ds->dsBegin.dpBi= ds->dsEnd.dpBi;
	ds->dsBegin.dpParticule= 0;
	ds->dsBegin.dpStroff= 0;
	ds->dsCol0= ds->dsCol1;

	if  ( ds->dsAnchor.dpBi != ds->dsBegin.dpBi )
	    { ds->dsAnchor= ds->dsBegin;	}

	*pBeginMoved= 1; *pEndMoved= 0; return;
	}
    else{
	ds->dsEnd.dpBi= ds->dsBegin.dpBi;
	ds->dsEnd.dpParticule= ds->dsEnd.dpBi->biParaParticuleCount -1;
	ds->dsEnd.dpStroff= ds->dsEnd.dpBi->biParaStrlen;
	ds->dsCol1= ds->dsCol0;

	if  ( ds->dsAnchor.dpBi != ds->dsEnd.dpBi )
	    { ds->dsAnchor= ds->dsEnd;	}

	*pBeginMoved= 0; *pEndMoved= 1; return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Return the union of two BufferSelections in the same paragraph.	*/
/*									*/
/************************************************************************/

void docUnionParaSelections(		DocumentSelection *	ds,
					const DocumentSelection *	ds1,
					const DocumentSelection *	ds2 )
    {
    DocumentSelection	dsNew;

    if  ( ds1->dsBegin.dpBi !=  ds1->dsEnd.dpBi )
	{ XXDEB(ds1->dsBegin.dpBi,ds1->dsEnd.dpBi); return;	}
    if  ( ds2->dsBegin.dpBi !=  ds2->dsEnd.dpBi )
	{ XXDEB(ds2->dsBegin.dpBi,ds2->dsEnd.dpBi); return;	}
    if  ( ds1->dsBegin.dpBi !=  ds2->dsEnd.dpBi )
	{ XXDEB(ds1->dsBegin.dpBi,ds2->dsEnd.dpBi); return;	}

    dsNew= *ds1;

    if  ( ds2->dsBegin.dpStroff < ds1->dsBegin.dpStroff )
	{ dsNew.dsBegin.dpStroff= ds2->dsBegin.dpStroff;	}
    else{ dsNew.dsBegin.dpStroff= ds1->dsBegin.dpStroff;	}

    if  ( ds2->dsBegin.dpParticule < ds1->dsBegin.dpParticule )
	{ dsNew.dsBegin.dpParticule= ds2->dsBegin.dpParticule;	}
    else{ dsNew.dsBegin.dpParticule= ds1->dsBegin.dpParticule;	}

    /**/

    if  ( ds2->dsEnd.dpStroff > ds1->dsEnd.dpStroff )
	{ dsNew.dsEnd.dpStroff= ds2->dsEnd.dpStroff;	}
    else{ dsNew.dsEnd.dpStroff= ds1->dsEnd.dpStroff;	}

    if  ( ds2->dsEnd.dpParticule > ds1->dsEnd.dpParticule )
	{ dsNew.dsEnd.dpParticule= ds2->dsEnd.dpParticule;	}
    else{ dsNew.dsEnd.dpParticule= ds1->dsEnd.dpParticule;	}

    /**/

    *ds= dsNew; return;
    }

/************************************************************************/
/*									*/
/*  Move to the first/last position.					*/
/*									*/
/************************************************************************/

int docFirstPosition(	DocumentPosition *	dp,
			BufferItem *		bi )
    {
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA )
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= 0;
	    dp->dpParticule= 0;

	    return 0;
	    }

	if  ( bi->biChildCount == 0 )
	    { /*LDEB(bi->biChildCount);*/ return -1;	}

	bi= bi->biChildren[0];
	}

    /*XDEB(bi);*/ return -1;
    }

int docLastPosition(	DocumentPosition *	dp,
			BufferItem *		bi )
    {
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA )
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= bi->biParaStrlen;
	    dp->dpParticule= bi->biParaParticuleCount- 1;

	    return 0;
	    }

	if  ( bi->biChildCount == 0 )
	    { /*LDEB(bi->biChildCount);*/ return -1;	}

	bi= bi->biChildren[bi->biChildCount- 1];
	}

    /*XDEB(bi);*/ return -1;
    }

int docFirstDocumentPosition(	DocumentPosition *	dp,
				BufferDocument *	bd )
    {
    if  ( docFirstPosition( dp, &(bd->bdItem) ) )
	{ return -1;	}

    return 0;
    }

int docLastDocumentPosition(	DocumentPosition *	dp,
				BufferDocument *	bd )
    {
    if  ( docLastPosition( dp, &(bd->bdItem) ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

static BufferItem * docNextItem(	BufferItem *	bi,
					int		level )
    {
    for (;;)
	{
	if  ( ! bi->biParent )
	    { return (BufferItem *)0;	}

	if  ( bi->biNumberInParent < bi->biParent->biChildCount- 1 )
	    {
	    bi= bi->biParent->biChildren[bi->biNumberInParent+ 1];

	    while( bi->biLevel < level	&&
		   bi->biChildCount > 0	)
		{ bi= bi->biChildren[0]; }

	    if  ( bi->biLevel == level )
		{ return bi;	}
	    }
	else{ bi= bi->biParent;	}
	}
    }

BufferItem *	docNextParagraph(	BufferItem *	bi )
    { return docNextItem( bi, DOClevPARA );	}

BufferItem *	docNextSection(	BufferItem *	bi )
    { return docNextItem( bi, DOClevSECT );	}

static BufferItem * docPrevItem(	BufferItem *	bi,
					int		level )
    {
    for (;;)
	{
	if  ( ! bi->biParent )
	    { return (BufferItem *)0;	}

	if  ( bi->biNumberInParent > 0 )
	    {
	    bi= bi->biParent->biChildren[bi->biNumberInParent- 1];

	    while( bi->biLevel < level	&&
		   bi->biChildCount > 0	)
		{ bi= bi->biChildren[bi->biChildCount- 1]; }

	    if  ( bi->biLevel == level )
		{ return bi;	}
	    }
	else{ bi= bi->biParent;	}
	}
    }

BufferItem *	docPrevParagraph(	BufferItem *	bi )
    { return docPrevItem( bi, DOClevPARA );	}

BufferItem *	docPrevSection(		BufferItem *	bi )
    { return docPrevItem( bi, DOClevSECT );	}

int docNextPosition(	DocumentPosition *	dp )
    {
    BufferItem *	bi= dp->dpBi;

    int			stroff= dp->dpStroff+ 1;

    int			lastOne= 1;

    while( bi )
	{
	if  ( stroff <= bi->biParaStrlen )
	    {
	    docSetDocumentPosition( dp, bi, stroff, lastOne );
	    return 0;
	    }

	bi= docNextParagraph( bi );
	stroff= 0;

	if  ( bi && bi->biParaStrlen == 0 )
	    {
	    docSetDocumentPosition( dp, bi, stroff, lastOne );
	    return 0;
	    }
	}

    return -1;
    }

int docPrevPosition(	DocumentPosition *	dp,
			int			lastOne )
    {
    BufferItem *	bi= dp->dpBi;

    int			stroff= dp->dpStroff- 1;

    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA		&&
	      stroff >= 0			)
	    {
	    docSetDocumentPosition( dp, bi, stroff, lastOne );
	    return 0;
	    }

	bi= docPrevParagraph( bi );
	if  ( bi )
	    { stroff= bi->biParaStrlen; }
	}

    return -1;
    }

int docParaBegin(	DocumentPosition *	dp,
			int			lastOne )
    {
    BufferItem *	bi= dp->dpBi;
    const int		stroff= 0;

    docSetDocumentPosition( dp, bi, stroff, lastOne );
    return 0;
    }

int docParaEnd(		DocumentPosition *	dp,
			int			lastOne )
    {
    BufferItem *	bi= dp->dpBi;

    docSetDocumentPosition( dp, bi, bi->biParaStrlen, lastOne );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Move to the Next/Previous word.					*/
/*									*/
/*  1)  Words start after a particule ending in a space or at the	*/
/*	beginning of a paragraph.					*/
/*									*/
/************************************************************************/

int docNextWord(	DocumentPosition *	dp,
			int			lastOne )
    {
    BufferItem *	bi= dp->dpBi;
    int			stroff= dp->dpStroff;

    if  ( stroff < bi->biParaStrlen )
	{
	int		part= dp->dpParticule;
	TextParticule *	tp= bi->biParaParticules+ part;
	unsigned char *	s= bi->biParaString;

	if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	    { part++; tp++; }

	while( part < bi->biParaParticuleCount )
	    {
	    /*  1  */
	    if  ( tp->tpKind == DOCkindTEXT				&&
		  tp->tpStrlen > 0					&&
		  ( s[tp->tpStroff+ tp->tpStrlen- 1] == ' '	||
		    tp->tpStroff+ tp->tpStrlen == bi->biParaStrlen )	)
		{ stroff= tp->tpStroff+ tp->tpStrlen; break;	}

	    part++; tp++;
	    }

	if  ( part == bi->biParaParticuleCount )
	    { stroff= bi->biParaStrlen; }
	}
    else{
	bi= docNextParagraph( bi );

	if  ( ! bi )
	    { return -1;	}

	stroff= 0;
	}

    docSetDocumentPosition( dp, bi, stroff, lastOne );

    return 0;
    }

int docPrevWord(	DocumentPosition *	dp,
			int			lastOne )
    {
    BufferItem *	bi= dp->dpBi;
    int			stroff= dp->dpStroff;

    if  ( stroff > 0 )
	{
	int		part= dp->dpParticule;
	TextParticule *	tp= bi->biParaParticules+ part;
	unsigned char *	s= bi->biParaString;

	if  ( stroff == tp->tpStroff )
	    { part--; tp--; }

	while( part >= 0 )
	    {
	    /*  1  */
	    if  ( tp->tpKind == DOCkindTEXT				&&
		  tp->tpStrlen > 0					&&
		  ( s[tp->tpStroff+ tp->tpStrlen- 1] == ' '	||
		    tp->tpStroff+ tp->tpStrlen == bi->biParaStrlen )	)
		{ break;	}

	    part--; tp--;
	    }

	part--; tp--;

	while( part >= 0 )
	    {
	    /*  1  */
	    if  ( tp->tpKind == DOCkindTEXT				&&
		  tp->tpStrlen > 0					&&
		  ( s[tp->tpStroff+ tp->tpStrlen- 1] == ' '	||
		    tp->tpStroff+ tp->tpStrlen == bi->biParaStrlen )	)
		{ stroff= tp->tpStroff+ tp->tpStrlen; break;	}

	    part--; tp--;
	    }

	if  ( part < 0 )
	    { stroff= 0;	}
	}
    else{
	bi= docPrevParagraph( bi );

	if  ( ! bi )
	    { return -1;	}

	stroff= bi->biParaStrlen;
	}

    docSetDocumentPosition( dp, bi, stroff, lastOne );

    return 0;
    }


/************************************************************************/
/*									*/
/*  Move one line up.							*/
/*									*/
/*  1)  Go to previous line of the paragraph or to the last line of the	*/
/*	last paragraph before this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineUp(		TextParticule **	pTp,
			TextLine **		pTl,
			DocumentPosition *	dp )
    {
    TextLine *		tl;
    TextParticule *	tp;
    BufferItem *	bi= dp->dpBi;

    int			part= dp->dpParticule;
    int			line;

    if  ( docFindLineOfParticule( &line, bi, part ) )
	{ LDEB(part); return -1;	}

    line--;

    /*  1  */
    while( bi )
	{
	if  ( bi->biParaLineCount == 0 )
	    { LDEB(bi->biParaLineCount);	}

	if  ( bi->biLevel == DOClevPARA		&&
	      line < bi->biParaLineCount	&& /* against crashes */
	      line >= 0				)
	    {
	    tl= bi->biParaLines+ line;
	    tp= bi->biParaParticules+ part;

	    while( part > tl->tlFirstParticule		&&
		  part > 0				)
		{ tp--; part--;	}
	    
	    dp->dpBi= bi;
	    dp->dpStroff= tl->tlStroff;
	    dp->dpParticule= part;

	    if  ( pTp )
		{ *pTp= tp;	}
	    if  ( pTl )
		{ *pTl= tl;	}

	    return 0;
	    }

	/*  2  */
	if  ( bi->biLevel == DOClevPARA		&&
	      bi->biParaInTable			&&
	      bi->biNumberInParent == 0		)
	    {
	    BufferItem *	cellBi= bi->biParent;
	    BufferItem *	rowBi= cellBi->biParent;
	    BufferItem *	sectBi= rowBi->biParent;

	    if  ( rowBi->biNumberInParent > 0 )
		{
		rowBi= sectBi->biChildren[rowBi->biNumberInParent- 1];

		if  ( rowBi->biRowHasTableParagraphs		&&
		      rowBi->biChildCount > 0			)
		    {
		    if  ( cellBi->biNumberInParent < rowBi->biChildCount )
			{ cellBi= rowBi->biChildren[cellBi->biNumberInParent]; }
		    else{ cellBi= rowBi->biChildren[rowBi->biChildCount- 1]; }

		    while( cellBi->biChildCount > 0 )
			{ cellBi= cellBi->biChildren[cellBi->biChildCount-1]; }
		    if  ( cellBi->biLevel == DOClevPARA )
			{
			bi= cellBi;
			part= bi->biParaParticuleCount- 1;
			line= bi->biParaLineCount- 1;
			continue;
			}
		    }
		}
	    }

	bi= docPrevParagraph( bi );
	if  ( bi )
	    {
	    part= bi->biParaParticuleCount- 1;
	    line= bi->biParaLineCount- 1;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move one line down.							*/
/*									*/
/*  1)  Go to the next line of the paragraph or to the first line of	*/
/*	the first paragraph after this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineDown(	TextParticule **	pTp,
			TextLine **		pTl,
			DocumentPosition *	dp )
    {
    TextLine *		tl;
    TextParticule *	tp;
    BufferItem *	bi= dp->dpBi;

    int			part= dp->dpParticule;
    int			line;

    if  ( docFindLineOfParticule( &line, bi, part ) )
	{ LDEB(part); return -1;	}

    line++;

    /*  1  */
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA		&&
	      line < bi->biParaLineCount	&&
	      line >= 0				)   /*  against crashes  */
	    {
	    tl= bi->biParaLines+ line;
	    tp= bi->biParaParticules+ part;

	    while( part < tl->tlFirstParticule		&&
		  part < bi->biParaParticuleCount	)
		{ tp++; part++;	}
	    
	    dp->dpBi= bi;
	    dp->dpStroff= tl->tlStroff;
	    dp->dpParticule= part;
	    *pTp= tp;
	    *pTl= tl;

	    return 0;
	    }

	/*  2  */
	if  ( bi->biLevel == DOClevPARA					&&
	      bi->biParaInTable						&&
	      bi->biNumberInParent == bi->biParent->biChildCount- 1	)
	    {
	    BufferItem *	cellBi= bi->biParent;
	    BufferItem *	rowBi= cellBi->biParent;
	    BufferItem *	sectBi= rowBi->biParent;

	    if  ( rowBi->biNumberInParent < sectBi->biChildCount- 1 )
		{
		rowBi= sectBi->biChildren[rowBi->biNumberInParent+ 1];

		if  ( rowBi->biRowHasTableParagraphs		&&
		      rowBi->biChildCount > 0			)
		    {
		    if  ( cellBi->biNumberInParent < rowBi->biChildCount )
			{ cellBi= rowBi->biChildren[cellBi->biNumberInParent]; }
		    else{ cellBi= rowBi->biChildren[rowBi->biChildCount- 1]; }

		    while( cellBi->biChildCount > 0 )
			{ cellBi= cellBi->biChildren[0]; }
		    if  ( cellBi->biLevel == DOClevPARA )
			{ bi= cellBi; part= 0; line= 0; continue; }
		    }
		}
	    }

	bi= docNextParagraph( bi );
	part= 0; line= 0;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move to the beginning/end of a line					*/
/*									*/
/************************************************************************/

int docBeginOfLine(	DocumentPosition *	dp )
    {
    BufferItem *	bi= dp->dpBi;

    int			line;
    TextLine *		tl;

    tl= bi->biParaLines;
    for ( line= 0; line < bi->biParaLineCount; tl++, line++ )
	{
	int			part= tl->tlFirstParticule;
	TextParticule *		tp= bi->biParaParticules+ part;

	if  ( part+ tl->tlParticuleCount > dp->dpParticule )
	    {
	    dp->dpBi= bi;
	    dp->dpParticule= part;
	    dp->dpStroff= tp->tpStroff;
	    return 0;
	    }
	}

    LDEB(1); return 1;
    }

int docEndOfLine(	DocumentPosition *	dp )
    {
    BufferItem *	bi= dp->dpBi;

    int			line;
    TextLine *		tl;

    tl= bi->biParaLines;
    for ( line= 0; line < bi->biParaLineCount; tl++, line++ )
	{
	int			part= tl->tlFirstParticule;
	TextParticule *		tp= bi->biParaParticules+ part;

	if  ( part+ tl->tlParticuleCount > dp->dpParticule )
	    {
	    part += tl->tlParticuleCount- 1;
	    tp= bi->biParaParticules+ part;

	    dp->dpBi= bi;
	    dp->dpParticule= part;
	    dp->dpStroff= tp->tpStroff+ tp->tpStrlen;
	    return 0;
	    }
	}

    LDEB(1); return 1;
    }

/************************************************************************/
/*									*/
/*  Compare two positions in a document.				*/
/*									*/
/*  Comparison is in terms of the reading order in the document.	*/
/*									*/
/************************************************************************/

int docCompareItemPositions(	const BufferItem *	bi1,
				const BufferItem *	bi2 )
    {
    while( bi1->biParent && bi1->biLevel > bi2->biLevel )
	{ bi1= bi1->biParent;	}

    while( bi2->biParent && bi2->biLevel > bi1->biLevel )
	{ bi2= bi2->biParent;	}

    if  ( bi1 == bi2 )
	{ return 0;	}

    while( bi1->biParent			&&
	   bi2->biParent			&&
	   ( bi1->biParent != bi2->biParent )	)
	{ bi1= bi1->biParent; bi2= bi2->biParent; }

    if  ( bi1->biParent && bi1->biParent == bi2->biParent )
	{
	if  ( bi1->biNumberInParent > bi2->biNumberInParent )
	    { return  1;	}

	if  ( bi1->biNumberInParent < bi2->biNumberInParent )
	    { return -1;	}

	return 0;
	}

    XXDEB(bi1->biParent,bi2->biParent);

    return 0;
    }

int docComparePositions(	const DocumentPosition *	dp1,
				const DocumentPosition *	dp2,
				int				mindParticule )
    {
    const BufferItem *	bi1= dp1->dpBi;
    const BufferItem *	bi2= dp2->dpBi;

    if  ( bi1 == bi2 )
	{
	if  ( dp1->dpStroff > dp2->dpStroff )
	    { return  1;	}

	if  ( dp1->dpStroff < dp2->dpStroff )
	    { return -1;	}

	if  ( mindParticule )
	    {
	    if  ( dp1->dpParticule > dp2->dpParticule )
		{ return  1;	}

	    if  ( dp1->dpParticule < dp2->dpParticule )
		{ return -1;	}
	    }

	return 0;
	}

    return docCompareItemPositions( bi1, bi2 );
    }

/************************************************************************/
/*									*/
/*  Go to the top/bottom of a certain page.				*/
/*									*/
/************************************************************************/

int docGetTopOfColumn(	DocumentPosition *		dp,
			BufferDocument *		bd,
			int				page,
			int				column )
    {
    BufferItem *	bi= &(bd->bdItem);
    int			i;
    TextLine *		tl;

    while( bi && bi->biLevel != DOClevPARA )
	{
	if  ( bi->biTopPosition.lpPage > page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}
	if  ( bi->biBelowPosition.lpPage < page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}

	for ( i= 0; i < bi->biChildCount; i++ )
	    {
	    if  ( bi->biChildren[i]->biBelowPosition.lpPage >= page )
		{
		if  ( bi->biChildren[i]->biBelowPosition.lpPage > page )
		    { break;	}
		if  ( bi->biChildren[i]->biBelowPosition.lpColumn >= column )
		    { break;	}
		}
	    }

	if  ( i >= bi->biChildCount )
	    {
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ LLDEB(i,bi->biChildren[i]->biBelowPosition.lpPage); }
	    LLLDEB(bi->biTopPosition.lpPage,bi->biBelowPosition.lpPage,page);
	    return -1;
	    }

	bi= bi->biChildren[i];
	}

    if  ( ! bi || bi->biLevel != DOClevPARA )
	{ XDEB(bi); return -1;	}

    tl= bi->biParaLines;
    for ( i= 0; i < bi->biParaLineCount; tl++, i++ )
	{
	if  ( tl->tlTopPosition.lpPage >= page )
	    {
	    if  ( tl->tlTopPosition.lpPage > page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn >= column )
		{ break;	}
	    }
	}

    if  ( i >= bi->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{ LLDEB(i,bi->biParaLineCount); return -1;	}

    dp->dpBi= bi;
    dp->dpStroff= tl->tlStroff;
    dp->dpParticule= tl->tlFirstParticule;

    return 0;
    }

int docGetBottomOfColumn(	DocumentPosition *		dp,
				BufferDocument *		bd,
				int				page,
				int				column )
    {
    BufferItem *	bi= &(bd->bdItem);
    int			i;
    TextLine *		tl;

    while( bi && bi->biLevel != DOClevPARA )
	{
	if  ( bi->biTopPosition.lpPage > page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}
	if  ( bi->biBelowPosition.lpPage < page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}

	for ( i= bi->biChildCount- 1; i >= 0; i-- )
	    {
	    if  ( bi->biChildren[i]->biTopPosition.lpPage <= page )
		{
		if  ( bi->biChildren[i]->biTopPosition.lpPage < page )
		    { break;	}
		if  ( bi->biChildren[i]->biTopPosition.lpColumn <= column )
		    { break;	}
		}
	    }

	if  ( i >= bi->biChildCount )
	    {
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ LLDEB(i,bi->biChildren[i]->biTopPosition.lpPage); }
	    LLLDEB(bi->biTopPosition.lpPage,bi->biTopPosition.lpPage,page);
	    return -1;
	    }

	bi= bi->biChildren[i];
	}

    if  ( ! bi || bi->biLevel != DOClevPARA )
	{ XDEB(bi); return -1;	}

    tl= bi->biParaLines+ bi->biParaLineCount- 1;
    for ( i= bi->biParaLineCount- 1; i >= 0; tl--, i-- )
	{
	if  ( tl->tlTopPosition.lpPage <= page )
	    {
	    if  ( tl->tlTopPosition.lpPage < page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn <= column )
		{ break;	}
	    }
	}

    if  ( i >= bi->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{ LLDEB(i,bi->biParaLineCount); return -1;	}

    dp->dpBi= bi;
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;
    dp->dpParticule= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate the result of a search in a paragraph to a selection.	*/
/*									*/
/************************************************************************/

void docSetParaSelection(	DocumentSelection *	ds,
				BufferItem *		bi,
				int			direction,
				int			stroff,
				int			length )
    {
    if  ( bi->biLevel != DOClevPARA )
	{ LDEB(bi->biLevel);	}

    docSetDocumentPosition( &(ds->dsBegin), bi, stroff, 1 );
    docSetDocumentPosition( &(ds->dsEnd), bi, stroff+ length, 0 );

    ds->dsDirection= direction;

    ds->dsCol0= ds->dsCol1= -1;

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsBegin;	}
    else{ ds->dsAnchor= ds->dsEnd;	}

    docSetSelectionScope( ds, bi );

    return;
    }

/************************************************************************/
/*									*/
/*  Derive the scope of a selection from a buffer item inside.		*/
/*									*/
/************************************************************************/

void docSetSelectionScope(	DocumentSelection *	ds,
				const BufferItem *	sectBi )
    {
    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi);	}
    else{ ds->dsSelectionScope= sectBi->biSectSelectionScope;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Are we in a something.						*/
/*									*/
/************************************************************************/

int docParticuleInField(	BufferItem *	bi,
				int		part )
    {
    TextParticule *	tp;

    part--;

    tp= bi->biParaParticules+ part;
    while( part >= 0 )
	{
	if  ( tp->tpKind == DOCkindFIELDEND )
	    { return 0;	}
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    { return 1;	}

	tp--; part--;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get buffer positions for a text line.				*/
/*									*/
/************************************************************************/

void docLineSelection(	DocumentSelection *	dsLine,
			const BufferItem *	bi,
			int			line )
    {
    const TextLine *	tl= bi->biParaLines+ line;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return;	}
    if  ( line < 0 || line >= bi->biParaLineCount )
	{ LLDEB(line,bi->biParaLineCount); return;	}

    docInitDocumentSelection( dsLine );

    dsLine->dsBegin.dpBi= (BufferItem *)bi;
    dsLine->dsBegin.dpParticule= tl->tlFirstParticule;
    dsLine->dsBegin.dpStroff= tl->tlStroff;

    dsLine->dsEnd.dpBi= (BufferItem *)bi;
    dsLine->dsEnd.dpParticule= tl->tlFirstParticule+ tl->tlParticuleCount- 1;
    dsLine->dsEnd.dpStroff= tl->tlStroff+ tl->tlStrlen;

    dsLine->dsAnchor= dsLine->dsBegin;
    dsLine->dsDirection= 1;

    dsLine->dsCol0= dsLine->dsCol1= -1;

    docSetSelectionScope( dsLine, bi );

    return;
    }

void docWordSelection(	DocumentSelection *		dsWord,
			int *				pIsObject,
			const DocumentPosition *	dpAround )
    {
    TextParticule *	tp;

    BufferItem *	bi= dpAround->dpBi;
    int			part= dpAround->dpParticule;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return;	}
    if  ( part < 0 || part >= bi->biParaParticuleCount )
	{ LLDEB(part,bi->biParaParticuleCount); return;	}

    tp= bi->biParaParticules+ part;

    if  ( tp->tpKind == DOCkindOBJECT )
	{
	docSetParaSelection( dsWord, bi, 1, tp->tpStroff, tp->tpStrlen );
	*pIsObject= 1;
	}
    else{
	int	partBegin= part;
	int	stroffBegin= tp->tpStroff;

	int	partEnd= part;
	int	stroffEnd= tp->tpStroff+ tp->tpStrlen;

	while( partBegin > 0				&&
	       tp[-1].tpKind == DOCkindTEXT		&&
	       bi->biParaString[tp->tpStroff- 1] != ' '	)
	    {
	    tp--; partBegin--;
	    stroffBegin= tp->tpStroff;
	    }

	tp= bi->biParaParticules+ part;
	while( partEnd < bi->biParaParticuleCount- 1	&&
	       tp[1].tpKind == DOCkindTEXT		&&
	       bi->biParaString[stroffEnd- 1] != ' '	)
	    {
	    tp++; partEnd++;
	    stroffEnd= tp->tpStroff+ tp->tpStrlen;
	    }

	while( stroffEnd > tp->tpStroff			&&
	       bi->biParaString[stroffEnd- 1] == ' '	)
	    { stroffEnd--; }

	docSetParaSelection( dsWord, bi, 1,
				    stroffBegin, stroffEnd- stroffBegin );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a TableRectangle to a DocumentSelection.			*/
/*									*/
/************************************************************************/

int docTableRectangleSelection(	DocumentSelection *	ds,
				BufferItem **		pSelSectBi,
				BufferDocument *	bd,
				const TableRectangle *	tr )
    {
    DocumentSelection	dsNew;

    BufferItem *	selSectBi;
    BufferItem *	rowBi;
    BufferItem *	cellBi;

    ExternalItem *	ei;
    BufferItem *	bodySectBi;

    docInitDocumentSelection( &dsNew );

    /*******/

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent;	}
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    /*******/

    if  ( tr->trRow0 < 0			||
	  tr->trRow0 >= selSectBi->biChildCount	)
	{ LLDEB(tr->trRow0,selSectBi->biChildCount); return -1;	}
    rowBi= selSectBi->biChildren[tr->trRow0];

    if  ( tr->trCol0 < 0			||
	  tr->trCol0 >= rowBi->biChildCount	)
	{ LLDEB(tr->trCol0,rowBi->biChildCount); return -1;	}
    cellBi= rowBi->biChildren[tr->trCol0];

    if  ( docFirstPosition( &dsNew.dsBegin, cellBi ) )
	{ LDEB(0); return -1;	}

    /*******/

    if  ( tr->trRow1 < 0			||
	  tr->trRow1 >= selSectBi->biChildCount	)
	{ LLDEB(tr->trRow0,selSectBi->biChildCount); return -1;	}
    rowBi= selSectBi->biChildren[tr->trRow1];

    if  ( tr->trCol1 < 0			||
	  tr->trCol1 >= rowBi->biChildCount	)
	{ LLDEB(tr->trCol1,rowBi->biChildCount); return -1;	}
    cellBi= rowBi->biChildren[tr->trCol1];

    if  ( docLastPosition( &dsNew.dsEnd, cellBi ) )
	{ LDEB(0); return -1;	}

    /*******/

    dsNew.dsCol0= tr->trCol0;
    dsNew.dsCol1= tr->trCol1;
    dsNew.dsDirection= 1;
    dsNew.dsAnchor= dsNew.dsBegin;

    if  ( tr->trRow1 < tr->trRow0					||
	  ( tr->trRow1 == tr->trRow0 && tr->trCol1 < tr->trCol0 )	)
	{
	dsNew.dsDirection= -1;
	dsNew.dsAnchor= dsNew.dsEnd;
	}

    docSetSelectionScope( &dsNew, selSectBi );

    *ds= dsNew;
    *pSelSectBi= selSectBi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Decide whether the selection moved to a different scope.		*/
/*									*/
/*  NOTE: Only use the scope fields of the old selection as deletions	*/
/*	or insertions of BufferItems may have invalidated the pointers.	*/
/*									*/
/************************************************************************/

int docSelectionDifferentRoot(
			const DocumentSelection *	dsOld,
			const BufferItem *		biSet )
    {
    const SelectionScope *	ssOld= &(dsOld->dsSelectionScope);
    const SelectionScope *	ssSet;

    while( biSet && biSet->biLevel != DOClevSECT )
	{ biSet= biSet->biParent;	}

    if  ( ! biSet )
	{ XDEB(biSet); return -1;	}
    ssSet= &(biSet->biSectSelectionScope);

    if  ( ssSet->ssInExternalItem != ssOld->ssInExternalItem )
	{ return 1;	}

    switch( ssSet->ssInExternalItem )
	{
	case DOCinBODY:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    return 0;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ssSet->ssSectNrExternalTo != ssOld->ssSectNrExternalTo )
		{ return 1;	}

	    return 0;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  Implied by ssNoteArrayIndex test:
	    if  ( ssSet->ssSectNrExternalTo != ssOld->ssSectNrExternalTo )
		{ return 1;	}
	    */
	    if  ( ssSet->ssNoteArrayIndex != ssOld->ssNoteArrayIndex )
		{ return 1;	}

	    return 0;

	default:
	    LDEB(biSet->biInExternalItem);
	    LDEB(ssSet->ssInExternalItem);
	    return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Implementation of a 'Select All' menu option.			*/
/*									*/
/************************************************************************/

int docSelectAll(		DocumentSelection *	ds,
				BufferDocument *	bd )
    {
    BufferItem *		selSectBi;

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    if  ( ! selSectBi )
	{ selSectBi= &(bd->bdItem);	}

    while( selSectBi && selSectBi->biParent )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &(ds->dsEnd), selSectBi ) )
	{ LDEB(1); return -1;	}

    ds->dsAnchor= ds->dsBegin;
    ds->dsDirection= 1;

    docSetSelectionScope( ds, ds->dsBegin.dpBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeSection(	DocumentSelection *	ds,
				BufferDocument *	bd,
				int			direction )
    {
    BufferItem *		selSectBi;

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docLastPosition( &(ds->dsBegin), selSectBi ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsBegin) ) )
	    { return 1;	}

	ds->dsEnd= ds->dsBegin;

	selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! selSectBi )
	    { XDEB(selSectBi); return -1;	}
	}

    if  ( direction < 0 )
	{
	const int	lastOne= 1;

	if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsBegin), lastOne ) )
	    { return 1;	}

	ds->dsEnd= ds->dsBegin;

	selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! selSectBi )
	    { XDEB(selSectBi); return -1;	}
	}

    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( direction == 0 )
	{ direction= 1;	}

    if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &(ds->dsEnd), selSectBi ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsBegin;	}
    else{ ds->dsAnchor= ds->dsEnd;	}

    ds->dsDirection= direction;

    docSetSelectionScope( ds, ds->dsBegin.dpBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is the selection exactly an object.					*/
/*									*/
/************************************************************************/

int docGetObjectSelection(	DocumentSelection *	ds,
				DocumentPosition *	dpObject,
				InsertedObject **	pIo )
    {
    if  ( ds->dsBegin.dpBi			&&
	  ds->dsEnd.dpBi == ds->dsBegin.dpBi	)
	{
	BufferItem *		bi= ds->dsBegin.dpBi;
	int			part= ds->dsBegin.dpParticule;
	TextParticule *		tp= bi->biParaParticules+ part;

	if  ( tp->tpKind == DOCkindOBJECT			&&
	      ds->dsBegin.dpStroff == tp->tpStroff		&&
	      ds->dsEnd.dpStroff == tp->tpStroff+ tp->tpStrlen	)
	    {
	    *dpObject= ds->dsBegin;
	    *pIo= bi->biParaObjects+ tp->tpObjectNumber;

	    return 0;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Fill a DocumentPosition.						*/
/*									*/
/************************************************************************/

int docSetDocumentPosition(	DocumentPosition *	dp,
				BufferItem *		bi,
				int			stroff,
				int			lastOne )
    {
    int		part;

    if  ( docFindParticule( &part, bi, stroff, lastOne ) )
	{ LLDEB(stroff,bi->biParaStrlen); return -1; }

    dp->dpBi= bi;
    dp->dpParticule= part;
    dp->dpStroff= stroff;

    return 0;
    }

