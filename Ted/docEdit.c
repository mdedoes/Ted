/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

#   define	DEB_PARTICULES	0

/************************************************************************/
/*									*/
/*  Administration of fields that are deleted in the editing process.	*/
/*									*/
/************************************************************************/

static int docEditClaimFieldParticules(	TextParticule **	pTp,
					int			count )
    {
    TextParticule *	fresh;

    fresh= (TextParticule *)realloc( *pTp, count* sizeof( TextParticule ) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    *pTp= fresh; return 0;
    }

/************************************************************************/
/*									*/
/*  Cleanup the set of fields that have been set apart during a		*/
/*  text substitution in a paragrapph.					*/
/*									*/
/*  1)  Fields that begin and end in the text that is replaced are	*/
/*	dropped.							*/
/*  3)  Skip particules that have been deleted.				*/
/*  4)  Ends of fields are considered later on.				*/
/*  5)  Index entries and table of contents entries from the old text	*/
/*	are dropped.							*/
/*									*/
/*  a)  Superfluous as the array is reorganised.			*/
/*									*/
/************************************************************************/

static int docCleanupFields(	int *			pBulletDeleted,
				BufferDocument *	bd,
				BufferItem *		paraBi,
				TextParticule *		fieldParticules,
				int			fieldCount )
    {
    int		from;
    int		to;
    int		bulletDeleted= 0;

    to= 0;
    for ( from= 0; from < fieldCount; from++ )
	{
	DocumentField *		dfFrom;
	int			i;

	switch( fieldParticules[from].tpKind )
	    {
	    /*  1  */
	    case DOCkindFIELDSTART:
		dfFrom= bd->bdFieldList.dflFields+
					fieldParticules[from].tpObjectNumber;
		for ( i= from+ 1; i < fieldCount; i++ )
		    {
		    if  ( fieldParticules[i].tpKind == DOCkindFIELDEND )
			{ break;	}
		    }

		if  ( i < fieldCount )
		    {
		    if  ( dfFrom->dfKind == DOCfkLISTTEXT )
			{ bulletDeleted= 1;	}

		    docCleanField( dfFrom );
		    docInitField( dfFrom );
		    fieldParticules[from].tpKind= DOCkindUNKNOWN; /* a */
		    fieldParticules[i].tpKind= DOCkindUNKNOWN;
		    }
		else{ fieldParticules[to++]= fieldParticules[from]; }
		break;

	    /*  3  */
	    case DOCkindUNKNOWN:
		continue;

	    /*  4  */
	    case DOCkindFIELDEND:
		fieldParticules[to++]= fieldParticules[from];
		break;

	    /*  5  */
	    case DOCkindXE:
	    case DOCkindTC:
		dfFrom= bd->bdFieldList.dflFields+
					fieldParticules[from].tpObjectNumber;
		docCleanField( dfFrom );
		docInitField( dfFrom );
		fieldParticules[from].tpKind= DOCkindUNKNOWN; /* a */
		continue;

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
	    default:
		LDEB(fieldParticules[from].tpKind);
		return -1;
	    }
	}

    if  ( bulletDeleted && paraBi->biParaListOverride > 0 )
	{
	if  ( docRemoveParagraphFromList( paraBi, bd ) )
	    { LDEB(bulletDeleted);	}
	}

    *pBulletDeleted= bulletDeleted;
    return to;
    }

/************************************************************************/
/*									*/
/*  Reinsert a field particule that was contained in text that was	*/
/*  removed from the paragraph. Ends are only reinserted when the	*/
/*  corresponding beginning is also there.				*/
/*									*/
/************************************************************************/

static int docReinsertFieldParticule(	BufferDocument *	bd,
					BufferItem *		oldBi,
					int			stroff,
					int			part,
					TextParticule *		tpField,
					TextParticule *		tpTo )
    {
    TextParticule *	tpFrom;
    int			i;

    int			endObjectNumber= -1;

    switch( tpField->tpKind )
	{
	case DOCkindFIELDSTART:
	case DOCkindXE:
	case DOCkindTC:
	    break;

	case DOCkindFIELDEND:
	    tpFrom= oldBi->biParaParticules+ part;
	    endObjectNumber= tpField->tpObjectNumber;
	    tpFrom--;
	    for ( i= part- 1; i >= 0; tpFrom--, i-- )
		{
		if  ( tpFrom->tpKind == DOCkindFIELDSTART	&&
		      tpFrom->tpObjectNumber == endObjectNumber	)
		    { break;	}
		}

	    if  ( i < 0 )
		{ return 0;	}

	    break;

	case DOCkindUNKNOWN:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	default:
	    LDEB(tpField->tpKind);
	    return -1;
	}

    if  ( tpTo )
	{
	*tpTo= *tpField;
	tpTo->tpStroff= stroff;
	}
    else{
	tpTo= docInsertTextParticule( oldBi, part, stroff,
					tpField->tpStrlen, tpField->tpKind,
					tpField->tpTextAttributeNumber );
	}

#   if DEB_PARTICULES
    appDebug( "NFL %3d: [%4d..%4d] %s \"%.*s\" len= %d\n", part,
		    tpTo->tpStroff,
		    tpTo->tpStroff+ tpTo->tpStrlen,
		    docKindStr( tpTo->tpKind ),
		    (int)tpTo->tpStrlen,
		    oldBi->biParaString+ tpTo->tpStroff,
		    tpTo->tpStrlen );
#   endif

    return 1;
    }

/************************************************************************/
/*									*/
/*  Replace text in a paragraph.					*/
/*									*/
/*  1)  Claim space to set apart special particules for field,		*/
/*	bookmarks, index entries &c.					*/
/*  2)  Make sure that enough space is allocated for the paragraph	*/
/*	contents.							*/
/*  3)  If the substitution actually is a deletion, compare attributes	*/
/*	to the current position.					*/
/*									*/
/*  4)  Find the last particule that does not end before the beginning	*/
/*	of the substitution.						*/
/*	NOTE that this automatically includes all particules that start	*/
/*	or end bookmarks.						*/
/*  5)  If the substitution starts at the beginning of a particule, and	*/
/*	the previous one has the same attributes as the inserted text,	*/
/*	start the redivision of the text in particules one particule	*/
/*	back.								*/
/*									*/
/*  6)  Find the first particule that does not begin before the		*/
/*	beginning of the substitution.					*/
/*	NOTE that this automatically includes all particules that start	*/
/*	or end bookmarks.						*/
/*  7)  If the substitution ends at the end of a particule, and	the	*/
/*	next one has the same attributes as the inserted text, include	*/
/*	the next one in the redivision of the text in particules.	*/
/*									*/
/*  8)  Decide whether the start particule needs to be split.		*/
/*  9)  Decide whether the end particule needs to be split.		*/
/*  9b) If the substitution is in the middle of a particule, it is not	*/
/*	possible to just adjust the begin of the particule, as it is	*/
/*	needed to do so for the first half. So make a new one.		*/
/*  10) Adapt the range of particules that are to be replaced to the	*/
/*	outcome of the decisions.					*/
/*									*/
/*  11) Delete particules corresponding to what is deleted, and to what	*/
/*	is to be redivided in particules.				*/
/*									*/
/*  12) Change the string contents of the paragraph.			*/
/*  13) Get rid of fields and bookmarks that start and end in de	*/
/*	deleted range.							*/
/*  14) Does the replacemeny start in a field?				*/
/*  15) Reinsert field and bookmark starts defore the new contents.	*/
/*									*/
/*  16) Sanity check.							*/
/*  17) Divide the replacement and its surroundings in particules.	*/
/*									*/
/*  18) Insert the remaining field, bookmark and index entry related	*/
/*	particules after the replacement.				*/
/*  19) Delete the particumes that are left over.			*/
/*  20) If the final result is an empty paragraph, insert a dummy	*/
/*	particule.							*/
/*									*/
/*  21) Adjust the offsets for the particules in the rest of the	*/
/*	paragraph.							*/
/*									*/
/*  22) Realise that the TextLine administration is left in an		*/
/*	inconsistent state.						*/
/*  23) Remember the current particule number.				*/
/*									*/
/************************************************************************/

static void docFindSubstitutionBegin(	int *			pPartBegin,
					TextParticule **	pTpBegin,
					const BufferItem *	bi,
					int			addedAttrNr,
					int			stroffBegin,
					int			partBegin )
    {
    TextParticule *	tpBegin= bi->biParaParticules+ partBegin;

    if  ( partBegin > 0						&&
	   tpBegin[-1].tpKind == DOCkindFIELDSTART			&&
	   tpBegin->tpStroff == stroffBegin				)
	{
	while( partBegin > 0						&&
	       tpBegin[-1].tpKind == DOCkindFIELDSTART			&&
	       tpBegin->tpStroff == stroffBegin				)
	    { tpBegin--; partBegin--; }

	*pPartBegin= partBegin; *pTpBegin= tpBegin; return;
	}

    /*  4  */
    while( partBegin > 0						&&
	   tpBegin[-1].tpStroff+ tpBegin[-1].tpStrlen > stroffBegin	)
	{ tpBegin--; partBegin--; }

    while( partBegin > 0						&&
	   tpBegin[-1].tpKind == DOCkindTEXT				&&
	   tpBegin[-1].tpStroff+ tpBegin[-1].tpStrlen >= stroffBegin	)
	{ tpBegin--; partBegin--; }

    /*  5  */
    if  ( partBegin > 0							&&
	  tpBegin[-1].tpKind == DOCkindTEXT				&&
	  tpBegin->tpStroff == stroffBegin				&&
	  addedAttrNr == tpBegin[-1].tpTextAttributeNumber		)
	{ tpBegin--; partBegin--; }

    *pPartBegin= partBegin; *pTpBegin= tpBegin; return;
    }

static void docFindSubstitutionEnd(	int *			pPartEnd,
					TextParticule **	pTpEnd,
					const BufferItem *	bi,
					int			addedAttrNr,
					int			stroffEnd,
					int			partBegin )
    {
    int				partEnd= partBegin;
    TextParticule *		tpEnd= bi->biParaParticules+ partEnd;

    /*  6  */
    while( partEnd < bi->biParaParticuleCount- 1			&&
	   tpEnd[1].tpStroff < stroffEnd				)
	{ tpEnd++; partEnd++;	}

    if  ( partEnd < bi->biParaParticuleCount- 1				&&
	   tpEnd[1].tpKind == DOCkindFIELDEND				&&
	   tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	{
	while( partEnd < bi->biParaParticuleCount- 1			&&
	       tpEnd[1].tpKind == DOCkindFIELDEND			&&
	       tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	    { tpEnd++; partEnd++;	}

	while( partEnd < bi->biParaParticuleCount- 1			&&
	       tpEnd[1].tpKind == DOCkindNOTE				&&
	       tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	    { tpEnd++; partEnd++;	}

	*pPartEnd= partEnd; *pTpEnd= tpEnd; return;
	}

    while( partEnd < bi->biParaParticuleCount- 1			&&
	   tpEnd[1].tpKind == DOCkindTEXT				&&
	   tpEnd[1].tpStroff <= stroffEnd				)
	{ tpEnd++; partEnd++;	}

    /*  7  */
    if  ( partEnd < bi->biParaParticuleCount- 1				&&
	  tpEnd[1].tpKind == DOCkindTEXT				&&
	  tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd			&&
	  addedAttrNr == tpEnd[1].tpTextAttributeNumber			)
	{ tpEnd++; partEnd++;	}

    *pPartEnd= partEnd; *pTpEnd= tpEnd; return;
    }

int docParaReplaceText(		EditOperation *		eo,
				BufferItem *		bi,
				unsigned int		stroffBegin,
				int *			pPartShift,
				int *			pStroffShift,
				unsigned int		stroffEnd,
				const unsigned char *	addedString,
				unsigned int		addedStrlen,
				int			addedAttributeNumber )
    {
    int				stroffShift;
    int				partShift;
    int				oldCount;
    int				newCount;

    int				part;
    TextParticule *		tp;

    int				partBegin;
    int				partEnd;
    TextParticule *		tpBegin;
    TextParticule *		tpEnd;
    int				excludeBegin= 0;
    int				excludeEnd= 0;
    int				newBegin;
    int				newEnd;
    int				stroffEndEnd;

    int				i;
    TextParticule *		tpField;
    int				res;
    int				partsDone;

    static TextParticule *	fieldParticules;
    int				fieldCount= 0;

    int				savedAttributeNumber;
    const int			lastOne= 1;

    /*  1  */
    if  ( docEditClaimFieldParticules( &fieldParticules,
						bi->biParaParticuleCount ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    /*  2  */
    stroffShift= 0;
    partShift= 0;

    if  ( docFindParticule( &partBegin, bi, stroffBegin, lastOne ) )
	{ LDEB(stroffBegin); return -1;	}

    /*  3  */
    if  ( addedStrlen == 0 )
	{
	tpBegin= bi->biParaParticules+ partBegin;
	addedAttributeNumber= tpBegin->tpTextAttributeNumber;
	}

    /*  4,5  */
    docFindSubstitutionBegin( &partBegin, &tpBegin, bi,
				addedAttributeNumber, stroffBegin, partBegin );

    /*  6,7  */
    docFindSubstitutionEnd( &partEnd, &tpEnd, bi,
				addedAttributeNumber, stroffEnd, partBegin );
    stroffEndEnd= tpEnd->tpStroff+ tpEnd->tpStrlen;

    /*  8  */
    if  ( tpBegin->tpKind == DOCkindTEXT				&&
	  addedAttributeNumber == tpBegin->tpTextAttributeNumber	)
	{ newBegin= tpBegin->tpStroff;			}
    else{
	newBegin= stroffBegin;

	if  ( newBegin > tpBegin->tpStroff )
	    { excludeBegin= 1; }

	if  ( newBegin == bi->biParaStrlen )
	    {
	    if  ( tpBegin->tpKind == DOCkindNOTE	||
		  tpBegin->tpKind == DOCkindFIELDEND	)
		{ excludeBegin= 1;	}
	    }
	}

    /*  9  */
    if  ( tpEnd->tpKind == DOCkindTEXT					&&
	  addedAttributeNumber == tpEnd->tpTextAttributeNumber		)
	{ newEnd= tpEnd->tpStroff+ tpEnd->tpStrlen;	}
    else{
	newEnd= stroffEnd;

	if  ( newEnd < tpEnd->tpStroff+ tpEnd->tpStrlen )
	    { excludeEnd= 1; }
	}

    /*  8  */
    if  ( excludeBegin )
	{
	tpBegin->tpStrlen= newBegin- tpBegin->tpStroff;

#	if DEB_PARTICULES
	appDebug( "<<= %3d: [%4d..%4d] %s \"%.*s\"\n", partBegin,
				tpBegin->tpStroff,
				tpBegin->tpStroff+ tpBegin->tpStrlen,
				docKindStr( tpBegin->tpKind ),
				(int)tpBegin->tpStrlen,
				bi->biParaString+ tpBegin->tpStroff );
#	endif
	}

    /*  9  */
    if  ( excludeEnd )
	{
	/*  9b  */
	if  ( partEnd == partBegin	&&
	      excludeBegin		)
	    {
	    tp= docInsertTextParticule( bi, partEnd+ 1,
				newEnd, stroffEndEnd- newEnd,
				DOCkindTEXT, tpEnd->tpTextAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}

	    partShift++;
	    tpEnd= tp;
	    }
	else{
	    tpEnd->tpStroff= newEnd;
	    tpEnd->tpStrlen= stroffEndEnd- newEnd;
	    }

#	if DEB_PARTICULES
	appDebug( "=>> %3d: [%4d..%4d] %s \"%.*s\"\n", partEnd,
				tpEnd->tpStroff,
				tpEnd->tpStroff+ tpEnd->tpStrlen,
				docKindStr( tpEnd->tpKind ),
				(int)tpEnd->tpStrlen,
				bi->biParaString+ tpEnd->tpStroff );
#	endif
	}

    /*  10  */
    oldCount= 0;
    if  ( excludeBegin )
	{ partBegin++;	}
    if  ( excludeEnd )
	{ partEnd--;	}

    tpBegin= bi->biParaParticules+ partBegin;
    tpEnd= bi->biParaParticules+ partEnd;

    savedAttributeNumber= tpBegin->tpTextAttributeNumber;

    /*  11  */
    tp= tpBegin;
    for ( part= partBegin; part <= partEnd; tp++, part++ )
	{
#	if DEB_PARTICULES
	appDebug( "OLD %3d: [%4d..%4d] %s \"%.*s\"\n", part,
		    tp->tpStroff,
		    tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ),
		    (int)tp->tpStrlen,
		    bi->biParaString+ tp->tpStroff );
#	endif

	if  ( eo->eoCloseObject )
	    { (*eo->eoCloseObject)( eo->eoBd, bi, tp, eo->eoVoidadd );	}
	docCleanParticuleObject( bi, tp );

	if  ( tp->tpKind == DOCkindFIELDSTART	||
	      tp->tpKind == DOCkindFIELDEND	||
	      tp->tpKind == DOCkindXE		||
	      tp->tpKind == DOCkindTC		)
	    { fieldParticules[fieldCount++]= *tp;	}

	if  ( tp->tpKind == DOCkindNOTE )
	    {
	    if  ( docDeleteNoteOfParticule( eo->eoBd, bi, tp ) )
		{ LDEB(tp->tpStroff);	}

	    eo->eoNotesDeleted++;
	    }

	partShift--; oldCount++;
	}

    /*  12  */
    if  ( docParaStringReplace( &stroffShift, bi, stroffBegin, stroffEnd,
						addedString, addedStrlen ) )
	{ LDEB(addedStrlen); return -1;	}

    /*  13  */
    if  ( fieldCount > 0 )
	{
	int	bulletDeleted= 0;

	fieldCount= docCleanupFields( &bulletDeleted, eo->eoBd,
					    bi, fieldParticules, fieldCount );

	if  ( bulletDeleted )
	    { eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}
	}
    if  ( fieldCount < 0 )
	{ LDEB(fieldCount); return -1;	}

    /*  14  */
    newEnd += stroffShift;
    newCount= 0;

    /*  15  */
    tp= tpBegin;
    part= partBegin;
    tpField= fieldParticules;
    for ( i= 0; i < fieldCount; tpField++, i++ )
	{
	if  ( tpField->tpKind != DOCkindFIELDSTART )
	    { continue;	}

	if  ( newCount < oldCount )
	    {
	    res= docReinsertFieldParticule( eo->eoBd, bi, newBegin, part,
							tpField, tp );
	    }
	else{
	    res= docReinsertFieldParticule( eo->eoBd, bi, newBegin, part,
					    tpField, (TextParticule *)0 );
	    }

	if  ( res < 0 )
	    { LDEB(res); return -1;	}

	if  ( res > 0 )
	    { partShift++; newCount++; part++, tp++;	}

	tpField->tpKind= DOCkindUNKNOWN;
	}

    /*  16  */
    if  ( newBegin > newEnd )
	{ LLDEB(newBegin,newEnd); }

    /*  17  */
    partsDone= docRedivideStringInParticules( bi,
			    newBegin, newEnd- newBegin,
			    part, oldCount- newCount, addedAttributeNumber );
    if  ( partsDone < 0 )
	{ LDEB(partsDone); return -1;	}
    newCount += partsDone;
    partShift += partsDone;
    part += partsDone;

    /*  18  */
    tpField= fieldParticules;
    for ( i= 0; i < fieldCount; tpField++, i++ )
	{
	if  ( tpField->tpKind == DOCkindUNKNOWN	)
	    { continue;	}

	if  ( newCount < oldCount )
	    {
	    res= docReinsertFieldParticule( eo->eoBd, bi, newEnd, part,
					tpField, bi->biParaParticules+ part );
	    }
	else{
	    res= docReinsertFieldParticule( eo->eoBd, bi, newEnd, part,
					tpField, (TextParticule *)0 );
	    }

	if  ( res < 0 )
	    { LDEB(res); return -1;	}

	if  ( res > 0 )
	    { partShift++; newCount++; part++;	}
	}

    /*  19  */
    if  ( newCount < oldCount )
	{
	docDeleteParticules( bi, part, oldCount- newCount );

	if  ( bi->biParaParticuleCount == 0 )
	    {
	    if  ( part != 0 || bi->biParaStrlen != 0 )
		{ LLDEB(part,bi->biParaStrlen);	}

	    tp= docInsertTextParticule( bi, part, bi->biParaStrlen,
					0, DOCkindTEXT, savedAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}

	    part= 1; partShift++;
	    }
	}

    /*  21  */
    if  ( docShiftParticuleOffsets( eo->eoBd, bi,
			    part, bi->biParaParticuleCount, stroffShift ) )
	{ LDEB(stroffShift);	}

#   if DEB_PARTICULES
    {
    int		p;

    appDebug( "\n" );

    p= 0; tp= bi->biParaParticules;
    while( p < bi->biParaParticuleCount )
	{
	appDebug( "=== %3d: [%4d..%4d] %s \"%.*s\"\n", p,
		    tp->tpStroff,
		    tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ),
		    (int)tp->tpStrlen,
		    bi->biParaString+ tp->tpStroff );
	p++; tp++;
	}
    }
#   endif

    /*  22  */
    *pPartShift += partShift;
    *pStroffShift += stroffShift;

    eo->eoParaBi= bi;
    eo->eoStroff= stroffBegin+ addedStrlen;

    /* 23 */
    if  ( docFindParticule( &(eo->eoParticule), eo->eoParaBi,
						    eo->eoStroff, lastOne ) )
	{ LDEB(eo->eoStroff); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Append the tail of a paragraph to another paragraph in the same	*/
/*  document.								*/
/*									*/
/*  1)  Claim memory for the appended contents.				*/
/*  2)  Claim memory for the appended particules.			*/
/*  3)  Append the contents of the first particule as text. This will	*/
/*	merge it with the last one of the target if necessary.		*/
/*  4)  Copy contents.							*/
/*  5)  Copy particules.						*/
/*									*/
/************************************************************************/

static int docParaAppend(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				unsigned int *		pFieldUpd,
				BufferItem *		biTo,
				BufferItem *		biFrom,
				unsigned int		stroffFrom )
    {
    int			rval= 0;

    TextParticule *	tpFrom;
    TextParticule *	newTp;

    int			stroffShift= 0;
    int			partShift= 0;
    int			particuleCount;

    int			stroffTo;

    const int		lastOne= 0;
    int			part;

    if  ( docFindParticule( &part, biFrom, stroffFrom, lastOne ) )
	{ LLDEB(stroffFrom,biFrom->biParaStrlen); return -1;	}

    tpFrom= biFrom->biParaParticules+ part;

    while( part < biFrom->biParaParticuleCount- 1		&&
	  tpFrom->tpKind != DOCkindFIELDSTART			&&
	  stroffFrom == tpFrom->tpStroff+ tpFrom->tpStrlen	)
	{ tpFrom++; part++;	}

    /*  2  */
    particuleCount= biTo->biParaParticuleCount+
				    biFrom->biParaParticuleCount- part+ 1;
    newTp= (TextParticule *)realloc( biTo->biParaParticules,
				    particuleCount* sizeof(TextParticule) );
    if  ( ! newTp )
	{ LXDEB(particuleCount,newTp); rval= -1; goto ready;	}
    biTo->biParaParticules= newTp;

    if  ( biTo->biParaParticuleCount > 0 )
	{
	newTp=	biTo->biParaParticules+ biTo->biParaParticuleCount- 1;

	/*  3  */
	if  ( tpFrom->tpKind == DOCkindTEXT				&&
	      tpFrom->tpTextAttributeNumber ==
					  newTp->tpTextAttributeNumber	)
	    {
	    int		xPartShift= 0;

	    if  ( docParaReplaceText( eo, biTo, biTo->biParaStrlen,
				&partShift, &xPartShift,
				biTo->biParaStrlen,
				biFrom->biParaString+ stroffFrom,
				tpFrom->tpStroff+ tpFrom->tpStrlen- stroffFrom,
				tpFrom->tpTextAttributeNumber ) )
		{ LDEB(tpFrom->tpStrlen); rval= -1; goto ready;	}

	    stroffFrom= tpFrom->tpStroff+ tpFrom->tpStrlen;
	    part++; tpFrom++;
	    }
	}

    stroffTo= biTo->biParaStrlen;

    /*  1,4,5  */
    if  ( docParaStringReplace( &stroffShift, biTo, stroffTo, stroffTo,
					biFrom->biParaString+ stroffFrom,
					biFrom->biParaStrlen- stroffFrom ) )
	{ LDEB(biFrom->biParaStrlen); rval= -1; goto ready;	}

    while( part < biFrom->biParaParticuleCount )
	{
	int		strlenTo;
	const int	partTo= -1;

	strlenTo= tpFrom->tpStroff+ tpFrom->tpStrlen- stroffFrom;

	if  ( docCopyParticuleAndData( &newTp, dcj, eo, biTo, partTo,
				    stroffTo, strlenTo, biFrom, tpFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*
	appDebug( "NEW %3d: [%4d..%4d] %s \"%.*s\"\n",
					    biTo->biParaParticuleCount- 1,
		    newTp->tpStroff,
		    newTp->tpStroff+ newTp->tpStrlen,
		    docKindStr( tpFrom->tpKind ),
		    (int)newTp->tpStrlen,
		    biTo->biParaString+ newTp->tpStroff );
	*/

	stroffTo += strlenTo; stroffFrom += strlenTo;
	tpFrom++; part++; partShift++;
	}

    eo->eoParaBi= biTo;
    eo->eoStroff= biTo->biParaStrlen;
    eo->eoParticule= biTo->biParaParticuleCount- 1;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  The current selection consisted of several paragraphs. An edit	*/
/*  operation was executed on the first one.. The rest is to be		*/
/*  deleted.								*/
/*									*/
/************************************************************************/

int docRemoveSelectionTail(	EditOperation *			eo,
				const DocumentSelection *	ds )
    {
    BufferItem *	nextBi;
    int			paraShift= 0;
    int			sectShift= 0;

    int			firstDeleted= -1;

    nextBi= docNextParagraph( ds->dsBegin.dpBi );
    if  ( ! nextBi )
	{ XDEB(nextBi); return -1;	}

    for (;;)
	{
	BufferItem *	parent= nextBi->biParent;

	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	docEditDeleteItems( eo, &firstParaDeleted, &paragraphsDeleted,
					parent, nextBi->biNumberInParent, 1 );
	paraShift += paragraphsDeleted;
	if  ( firstDeleted < 0 )
	    { firstDeleted= firstParaDeleted;	}

	if  ( nextBi == ds->dsEnd.dpBi )
	    {
	    while( parent && parent->biChildCount == 0 )
		{
		BufferItem *	pp= parent->biParent;

		docDeleteItem( eo->eoBd, parent );
		parent= pp;
		}

	    break;
	    }

	nextBi= docNextParagraph( ds->dsBegin.dpBi );
	if  ( ! nextBi )
	    { XDEB(nextBi); return -1;	}

	while( parent && parent->biChildCount == 0 )
	    {
	    BufferItem *	pp= parent->biParent;

	    docDeleteItem( eo->eoBd, parent );
	    parent= pp;
	    }
	}

    if  ( paraShift > 0 )
	{
	if  ( ds->dsBegin.dpBi->biInExternalItem == DOCinBODY )
	    {
	    const int	isSplit= 0;
	    const int	stroffFrom= 0;
	    const int	stroffShift= 0;

	    if  ( firstDeleted < 0 )
		{ LDEB(firstDeleted);	}

	    docEditShiftReferences( eo->eoBd, firstDeleted+ paraShift,
					isSplit, stroffFrom,
					-sectShift, -paraShift, stroffShift );
	    }

	/*  NO! done by docEditDeleteItems()
	docEditShiftReformatRangeParaNr( eo, paraNr+ 1, -paraShift );
	*/
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace a selection with a piece of text.				*/
/*									*/
/*  1)  If the selection spans more than one paragraph...		*/
/*  2)  Replace the tail of the first paragraph with the new text.	*/
/*  2b) Make sure thet the bullet of the last paragraph is not included	*/
/*	in the pail that is to be appended to the first paragraph.	*/
/*  3)  Merge the two paragraphs. I.E: append what remains of the last	*/
/*	paragraph to the first paragraph.				*/
/*  4)  Erase all paragraphs after the beginning of the selection	*/
/*	upto and including the end of the selection.			*/
/*  5)  Otherwise just replace the text inside the selected paragraph.	*/
/*									*/
/************************************************************************/

int docReplaceSelection(EditOperation *			eo,
			const DocumentSelection *	ds,
			int *				pPartShift,
			int *				pStroffShift,
			const unsigned char *		addedString,
			int				addedStrlen,
			int				addedAttributeNumber )
    {
    int			rval= 0;
    DocumentCopyJob	dcj;

    docInitDocumentCopyJob( &dcj );

    /*  1  */
    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{
	int		replaceEnd= ds->dsEnd.dpStroff;

	/*  2  */
	if  ( docParaReplaceText( eo, ds->dsBegin.dpBi, ds->dsBegin.dpStroff,
		    pPartShift, pStroffShift,
		    ds->dsBegin.dpBi->biParaStrlen,
		    addedString, addedStrlen, addedAttributeNumber ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	/*  2b  */
	if  ( ds->dsEnd.dpBi->biParaListOverride > 0 )
	    {
	    int		fieldNr= -1;
	    int		partBegin= -1;
	    int		partEnd= -1;
	    int		stroffBegin= -1;
	    int		stroffEnd= -1;

	    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						ds->dsEnd.dpBi, eo->eoBd ) )
		{ LDEB(1);	}
	    else{
		if  ( replaceEnd < stroffEnd )
		    { replaceEnd=  stroffEnd;	}
		}
	    }

	/*  3  */
	if  ( replaceEnd < ds->dsEnd.dpBi->biParaStrlen )
	    {
	    unsigned int	fieldUpd= FIELDdoNOTHING;

	    if  ( docSet1DocumentCopyJob( &dcj, eo->eoBd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( docParaAppend( eo, &dcj, &fieldUpd,
			    ds->dsBegin.dpBi, ds->dsEnd.dpBi, replaceEnd ) )
		{ LDEB(addedStrlen); rval= -1; goto ready;	}
	    }

	/*  4  */
	if  ( docRemoveSelectionTail( eo, ds ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	/*  5  */
	if  ( docParaReplaceText( eo, ds->dsBegin.dpBi, ds->dsBegin.dpStroff,
			    pPartShift, pStroffShift,
			    ds->dsEnd.dpStroff,
			    addedString, addedStrlen, addedAttributeNumber ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  a)  Find the first particule that ends on or after the given	*/
/*	position.							*/
/*  1)  Insert a paragraph AFTER the current one.			*/
/*  1a) Copy indents and tab stops.					*/
/*  2)  Allocate space for the string of the successor.			*/
/*  3)  If the position is in the middle of a particule, split it.	*/
/*  4)  Move the rest of the particules to the new paragraph.		*/
/*  5)  Keep field ends at the split position in the old paragraph.	*/
/*									*/
/************************************************************************/

static int docSplitParaItemLow(	EditOperation *		eo,
				int *			fieldMap,
				BufferItem **		pNewBi,
				BufferItem *		oldBi,
				int			stroff )
    {
    BufferDocument *		bd= eo->eoBd;
    int				rval= 0;

    BufferItem *		newBi;
    int				part= 0;
    TextParticule *		tp;
    TextParticule *		newTp;

    int				fieldLevel= 0;

    int				stroffShift= 0;
    int				partShift= 0;
    int				newStrlen= 0;

    int				truncatedParticuleCount;

    static TextParticule *	fieldParticules;
    int				fieldCount= 0;

    unsigned int		fieldUpd= FIELDdoNOTHING;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet1DocumentCopyJob( &dcj, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditClaimFieldParticules( &fieldParticules,
						oldBi->biParaParticuleCount ) )
	{ LDEB(oldBi->biParaParticuleCount); rval= -1; goto ready; }

    /*  a,b  */
    if  ( docFindParticule( &part, oldBi, stroff, /* lastOne */ 0 ) )
	{ LLDEB(oldBi->biParaStrlen,stroff); rval= -1; goto ready; }

    tp= oldBi->biParaParticules+ part;

    if  ( part < oldBi->biParaParticuleCount-1		&&
	  stroff == tp->tpStroff+ tp->tpStrlen		&&
	  tp[1].tpKind == DOCkindFIELDEND		)
	{ tp++; part++;	}

    while( part < oldBi->biParaParticuleCount		&&
	   tp->tpStrlen == 0				&&
	   ( tp->tpKind == DOCkindFIELDEND	||
	     tp->tpKind == DOCkindNOTE		)	)
	{ tp++; part++;	}

    /*  1  */
    newBi= docInsertItem( bd, oldBi->biParent,
				oldBi->biNumberInParent+ 1, oldBi->biLevel );
    if  ( ! newBi )
	{ XDEB(newBi); rval= -1; goto ready;	}

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskFILL( &ppUpdMask, PPprop_COUNT );

    /*  1a  */
    if  ( docUpdParaProperties( &ppChgMask, &(newBi->biParaProperties),
				    &ppUpdMask, &(oldBi->biParaProperties),
				    dcj.dcjColorMap, dcj.dcjListStyleMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( oldBi->biParaListOverride > 0 )
	{ dcj.dcjBulletsCopied++;	}

    /*  2  */
    if  ( docParaStringReplace( &stroffShift, newBi, 0, 0,
						oldBi->biParaString+ stroff,
						oldBi->biParaStrlen- stroff ) )
	{
	LLDEB(oldBi->biParaStrlen,stroff);
	docDeleteItems( bd, oldBi->biParent, oldBi->biNumberInParent+ 1, 1 );
	rval= -1; goto ready;
	}

    fieldLevel= 0;
    {
    int				p;
    const TextParticule *	t;

    t= oldBi->biParaParticules;
    for ( p= 0; p < part; t++, p++ )
	{
	if  ( t->tpKind == DOCkindFIELDSTART )
	    { fieldLevel++;	}
	if  ( t->tpKind == DOCkindFIELDEND )
	    { fieldLevel--;	}
	}

    if  ( fieldLevel < 0 )
	{ LDEB(fieldLevel); fieldLevel= 0;	}
    }

    /*  3  */
    if  ( part < oldBi->biParaParticuleCount	&&
	  stroff > tp->tpStroff			)
	{
	if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	    {
	    newStrlen= 0;
	    truncatedParticuleCount= part+ 1;
	    part++; tp++;
	    }
	else{
	    if  ( docSplitTextParticule( &tp, &newTp, oldBi, part, stroff ) )
		{ LDEB(part); rval= -1; goto ready;	}

	    newStrlen= 0;
	    truncatedParticuleCount= part+ 1;
	    part++; tp= newTp;
	    }
	}
    else{
	newStrlen= 0;
	truncatedParticuleCount= part;
	}

    /*  4  */
    while( part < oldBi->biParaParticuleCount )
	{
	switch( tp->tpKind )
	    {
	    default:
		LDEB(tp->tpKind);
		newStrlen += tp->tpStrlen; part++; tp++;
		continue;

	    case DOCkindFIELDEND:
		/*  5  */
		fieldParticules[fieldCount++]= *tp;
		if  ( fieldLevel > 0 )
		    {
		    fieldLevel--;
		    newStrlen += tp->tpStrlen; part++; tp++;
		    continue;
		    }
		break;

	    case DOCkindFIELDSTART:
	    case DOCkindXE:
	    case DOCkindTC:
		break;

	    case DOCkindOBJECT:
	    case DOCkindNOTE:
	    case DOCkindTEXT:
	    case DOCkindTAB:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		break;
	    }

	if  ( docCopyParticuleAndData( &newTp, &dcj, eo,
						newBi, partShift++,
						newStrlen, tp->tpStrlen,
						oldBi, tp ) )
	    { LDEB(part); rval= -1; goto ready;	}

	newStrlen += tp->tpStrlen; part++; tp++;
	}

    if  ( fieldCount > 0 )
	{
	int		i;
	TextParticule *	tpField;
	int		bulletDeleted= 0;

	fieldCount= docCleanupFields( &bulletDeleted,
				    bd, oldBi, fieldParticules, fieldCount );

	if  ( bulletDeleted )
	    { fieldUpd |= FIELDdoLISTTEXT;	}

	if  ( fieldCount < 0 )
	    { LDEB(fieldCount); rval= -1; goto ready;	}

	tpField= fieldParticules;
	tp= oldBi->biParaParticules+ truncatedParticuleCount;
	for ( i= 0; i < fieldCount; tpField++, i++ )
	    {
	    int		res;

	    res= docReinsertFieldParticule( bd, oldBi, stroff,
					truncatedParticuleCount, tpField, tp );

	    if  ( res < 0 )
		{ LDEB(res); rval= -1; goto ready;	}

	    if  ( res > 0 )
		{ truncatedParticuleCount++; tp++;	}
	    }
	}

    oldBi->biParaParticuleCount= truncatedParticuleCount;
    oldBi->biParaStrlen= stroff;

    if  ( newBi->biParaParticuleCount == 0 )
	{
	tp= oldBi->biParaParticules+ oldBi->biParaParticuleCount- 1;

	newTp= docInsertTextParticule( newBi, -1, 0, 0,
				    DOCkindTEXT, tp->tpTextAttributeNumber );
	if  ( ! newTp )
	    { LDEB(newBi->biParaParticuleCount); rval= -1; goto ready; }
	}

    if  ( oldBi->biParaParticuleCount == 0 )
	{
	tp= oldBi->biParaParticules;

	newTp= docInsertTextParticule( oldBi, -1, 0, 0,
				    DOCkindTEXT, tp->tpTextAttributeNumber );
	if  ( ! newTp )
	    { LDEB(oldBi->biParaParticuleCount); rval= -1; goto ready; }
	}

    newBi->biParaLineCount= 0;
    oldBi->biParaLineCount= 0;

    *pNewBi= newBi;

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

int docSplitParaItem(		EditOperation *		eo,
				BufferItem **		pNewBi,
				BufferItem *		oldBi,
				int			stroff )
    {
    int *	fieldMap;
    int		rval;

    fieldMap= docAllocateFieldMap( eo->eoBd );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    rval= docSplitParaItemLow( eo, fieldMap, pNewBi, oldBi, stroff );

    free( fieldMap );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a tab or another kind of special particule for something	*/
/*  that takes up some space in the document.				*/
/*									*/
/*  1)  Insert a space in the text string.				*/
/*  2)  Replace the empty particule in an empty paragraph.		*/
/*  3)  Can be inserted before the current particule.			*/
/*  4)  Can be inserted after the current particule.			*/
/*  5)  First split the particule,					*/
/*  6)  Then insert between the two halves.				*/
/*  7)  Shift particile offsets in the rest of the paragraph.		*/
/*									*/
/************************************************************************/

TextParticule * docParaSpecialParticule(BufferDocument *	bd,
					BufferItem *		bi,
					int			kind,
					int			part,
					int			stroff,
					int *			pPartShift,
					int *			pStroffShift )
    {
    TextParticule *	tp;
    TextParticule *	tpRet;
    int			stroffShift= 0;

    /*  1  */
    if  ( docParaStringReplace( &stroffShift, bi, stroff, stroff,
						(unsigned char *)" ", 1 ) )
	{ LDEB(stroff); return (TextParticule *)0;	}

    if  ( part == bi->biParaParticuleCount		&&
	  stroff == bi->biParaStrlen- 1			)
	{
	tp= bi->biParaParticules+ part- 1;

	tpRet= docInsertTextParticule( bi, part, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	if  ( ! tpRet )
	    { LXDEB(part,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	*pPartShift= 1; *pStroffShift= stroffShift;
	return tpRet;
	}

    if  ( part < 0 || part >= bi->biParaParticuleCount )
	{ LLDEB(part,bi->biParaParticuleCount); return (TextParticule *)0; }

    tp= bi->biParaParticules+ part;

    /*  2  */
    if  ( bi->biParaParticuleCount == 1			&&
	  bi->biParaStrlen == 1				&&
	  bi->biParaParticules->tpKind == DOCkindTEXT	)
	{
	bi->biParaParticules->tpKind= kind;
	bi->biParaParticules->tpStrlen= 1;

	*pPartShift= 0; *pStroffShift= stroffShift;
	return bi->biParaParticules;
	}

    /*  3  */
    if  ( stroff == tp->tpStroff )
	{
	tpRet= docInsertTextParticule( bi, part, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	if  ( ! tpRet )
	    { LXDEB(part,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	*pPartShift= 1;
	part++;
	}
    else{
	/*  4  */
	if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	    {
	    tpRet= docInsertTextParticule( bi, part+ 1, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	    if  ( ! tpRet )
		{ LXDEB(part,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    part += 2;
	    }
	else{
	    TextParticule *	tpNew;

	    /*  5  */
	    if  ( docSplitTextParticule( &tp, &tpNew, bi, part, stroff ) )
		{ LLDEB(part,stroff); return (TextParticule *)0;	}

	    /*  6  */
	    tpRet= docInsertTextParticule( bi, part+ 1, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	    if  ( ! tpRet )
		{ LXDEB(part+1,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    part += 2;
	    }
	}

    /*  7  */
    if  ( docShiftParticuleOffsets( bd, bi,
					part, bi->biParaParticuleCount, 1 ) )
	{ LDEB(1);	}

    *pStroffShift= stroffShift;

    return tpRet;
    }

/************************************************************************/
/*									*/
/*  Close the objects in a buffer item.					*/
/*									*/
/************************************************************************/

static void docCleanParaObjects(	int *			pNoteCount,
					int *			pBulletsDeleted,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					void *			voidadd,
					DOC_CLOSE_OBJECT	closeObject )
    {
    int			part;
    TextParticule *	tp;

    int			noteCount= 0;

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind == DOCkindNOTE )
	    { noteCount++;	}

	if  ( tp->tpKind == DOCkindOBJECT )
	    { (*closeObject)( bd, paraBi, tp, voidadd );	}
	}

    if  ( paraBi->biParaListOverride > 0 )
	{
	if  ( docRemoveParagraphFromList( paraBi, bd ) )
	    { LDEB(1);	}

	(*pBulletsDeleted)++;
	}

    *pNoteCount += noteCount;
    return;
    }

void docCleanItemObjects(	int *			pNoteCount,
				int *			pBulletsDeleted,
				int *			pParagraphCount,
				BufferDocument *	bd,
				BufferItem *		bi,
				void *			voidadd,
				DOC_CLOSE_OBJECT	closeObject )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevROW:
	case DOClevCELL:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		docCleanItemObjects( pNoteCount, pBulletsDeleted,
						    pParagraphCount,
						    bd, bi->biChildren[i],
						    voidadd, closeObject );
		}
	    break;

	case DOClevPARA:
	    docCleanParaObjects( pNoteCount, pBulletsDeleted,
						bd, bi, voidadd, closeObject );
	    (*pParagraphCount)++;
	    break;

	default:
	    LDEB(bi->biLevel); return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delete some buffer items but first close the objects in them.	*/
/*									*/
/*  1)  Preliminary administration for shifting the bottom of the range	*/
/*	of paragraphs that are to be reformatted.			*/
/*  2)  Include the whole of the parent in the range of items to be	*/
/*	reformatted.							*/
/*									*/
/************************************************************************/

void docEditDeleteItems(	EditOperation *		eo,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				BufferItem *		bi,
				int			first,
				int			count )
    {
    int		i;
    int		f= first;

    int		firstParaDeleted= -1;
    int		bulletsDeleted= 0;
    int		paragraphsDeleted= 0;
    int		notesDeleted= 0;

    if  ( count > 0 )
	{
	DocumentPosition	dp;

	/*  1  */
	if  ( docFirstPosition( &dp, bi->biChildren[f] ) )
	    { LDEB(1);						}
	else{ firstParaDeleted= docNumberOfParagraph( dp.dpBi );	}

	/*  2  */
	docEditIncludeItemInReformatRange( eo, bi );
	}

    for ( i= 0; i < count; f++, i++ )
	{
	docCleanItemObjects( &notesDeleted, &bulletsDeleted, &paragraphsDeleted,
					eo->eoBd, bi->biChildren[f],
					eo->eoVoidadd, eo->eoCloseObject );
	}

    eo->eoNotesDeleted += notesDeleted;

    docDeleteItems( eo->eoBd, bi, first, count );

    if  ( firstParaDeleted < 0 )
	{ LDEB(firstParaDeleted);	}
    else{
	docEditShiftReformatRangeParaNr( eo,
					firstParaDeleted+ paragraphsDeleted,
					-paragraphsDeleted );
	}

    *pFirstParaDeleted= firstParaDeleted;
    eo->eoBulletsChanged += bulletsDeleted;
    *pParagraphsDeleted= paragraphsDeleted;
    return;
    }

/************************************************************************/
/*									*/
/*  Make a paragraph empty by deleting its contents.			*/
/*									*/
/************************************************************************/

int docEditMakeParagraphEmpty(		BufferItem *		paraBi,
					EditOperation *		eo )
    {
    TextParticule *	tp= paraBi->biParaParticules;
    int			textAttributeNumber= tp->tpTextAttributeNumber;

    int			notesDeleted= 0;
    int			bulletsDeleted= 0;

    docCleanParaObjects( &notesDeleted, &bulletsDeleted,
			eo->eoBd, paraBi, eo->eoVoidadd, eo->eoCloseObject );

    eo->eoNotesDeleted += notesDeleted;
    eo->eoBulletsChanged += bulletsDeleted;

    paraBi->biParaStrlen= 0;
    docDeleteParticules( paraBi, 0, paraBi->biParaParticuleCount );
    docDeleteLines( paraBi, 0, paraBi->biParaLineCount );

    if  ( ! docInsertTextParticule( paraBi, 0, 0, 0, DOCkindTEXT,
							textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Shift references in various administrations.			*/
/*									*/
/************************************************************************/

void docEditShiftReferences(		BufferDocument *	bd,
					int			paraFrom,
					int			isSplit,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    docShiftNoteReferences( bd, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );

    if  ( paraShift != 0 )
	{
	int			i;
	ListNumberTreeNode *	node;

	int			paraFromBullet= paraFrom;

	if  ( isSplit )
	    { paraFromBullet++;	}

	node= bd->bdListNumberTrees;
	for ( i= 0; i < bd->bdListNumberTreeCount; node++, i++ )
	    { docShiftBulletReferences( node, paraFromBullet, paraShift ); }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Update paragraph properties and do extra administration.		*/
/*									*/
/************************************************************************/

int docEditUpdParaProperties(	EditOperation *			eo,
				PropertyMask *			pPpDoneMask,
				BufferItem *			paraBi,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew,
				const int *			colorMap,
				const int *			listStyleMap )
    {
    BufferDocument *		bd= eo->eoBd;

    int				wasInList= 0;
    int				isInList= 0;

    ListNumberTreeNode *	root= (ListNumberTreeNode *)0;
    int				override= -1;
    int				ilvl= 0;

    PropertyMask		ppDoneMask;
    PropertyMask		ppSetMaskThisPara;

    int				bulletFieldNr= -1;
    int				bulletPartBegin= -1;
    int				bulletPartEnd= -1;
    int				bulletStroffBegin= -1;
    int				bulletStroffEnd= -1;

    ppSetMaskThisPara= *ppSetMask;

    PROPmaskCLEAR( &ppDoneMask );

    /*  A  */
    if  ( paraBi->biParaProperties.ppListOverride > 0 )
	{
	wasInList= 1;
	override= paraBi->biParaProperties.ppListOverride;
	ilvl= paraBi->biParaProperties.ppListLevel;
	root= &(bd->bdListNumberTrees[override]);

	if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd, paraBi, bd ) )
	    { LDEB(1);	}

	PROPmaskUNSET( &ppSetMaskThisPara, PPpropLEFT_INDENT );
	PROPmaskUNSET( &ppSetMaskThisPara, PPpropFIRST_INDENT );
	}

    if  ( ! PROPmaskISEMPTY( &ppSetMaskThisPara ) )
	{
	if  ( docUpdParaProperties( &ppDoneMask, &(paraBi->biParaProperties),
						&ppSetMaskThisPara, ppNew,
						colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}
	}

    if  ( paraBi->biParaProperties.ppListOverride > 0 )
	{ isInList= 1;	}

    if  ( wasInList && ! isInList )
	{
	int		paraNr= docNumberOfParagraph( paraBi );

	int		partShift= 0;
	int		stroffShift= 0;

	int		addedStrlen= 0;
	int		textAttributeNumberFrom= -1;

	if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	    { LLDEB(override,paraNr);	}

	if  ( docParaReplaceText( eo, paraBi, bulletStroffBegin,
				    &partShift, &stroffShift,
				    bulletStroffEnd, (const unsigned char *)0,
				    addedStrlen, textAttributeNumberFrom ) )
	    { LDEB(1); return -1;	}

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    if  ( ! wasInList && isInList )
	{
	if  ( docInsertListtextField( paraBi, bd ) )
	    { LDEB(1); return -1;	}

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    if  ( wasInList							&&
	  isInList							&&
	  ( paraBi->biParaProperties.ppListOverride != override	||
	    paraBi->biParaProperties.ppListLevel != ilvl	)	)
	{
	int			paraNr= docNumberOfParagraph( paraBi );

	if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	    { LLDEB(override,paraNr);	}

	override= paraBi->biParaProperties.ppListOverride;
	ilvl= paraBi->biParaProperties.ppListLevel;
	root= &(bd->bdListNumberTrees[override]);

	if  ( docListNumberTreeInsertParagraph( root, ilvl, paraNr ) )
	    { LLLDEB(override,ilvl,paraNr);	}

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    *pPpDoneMask= ppDoneMask;
    return 0;
    }

