/************************************************************************/
/*									*/
/*  Manage fields.							*/
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
/*  Manage Footnotes/Endnotes.						*/
/*									*/
/************************************************************************/

void docInitNote(		DocumentNote *		dn )
    {
    docInitExternalItem( &(dn->dnExternalItem) );

    dn->dnNoteNumber= 0;

    dn->dnReferringPage= -1;
    dn->dnReferringColumn= -1;
    dn->dnSectNr= -1;
    dn->dnParaNr= -1;
    dn->dnStroff= -1;

    dn->dnExternalItemKind= DOCinFOOTNOTE;
    dn->dnAutoNumber= 0;

    return;
    }

void docCleanNote(		BufferDocument *	bd,
				DocumentNote *		dn )
    {
    docCleanExternalItem( bd, &(dn->dnExternalItem) );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a Footnote/Endnote in a BufferItem.				*/
/*									*/
/*  1)  Look for a note or the place to insert a new one.		*/
/*	NOTE that positions with a negative paragraph number are always	*/
/*	skipped.							*/
/*	NOTE that when a position for a new note is found, its index	*/
/*	is that of its successor, that usually will be shifted away.	*/
/*  2)  When the previous position is an empty hole, use it in stead	*/
/*	of shifting the successors away.				*/
/*  3)  Make a guess about the note number. If the guess is wrong, it	*/
/*	fixed later. Fixing can involve a reformat, so making not too	*/
/*	bad a guess here does help.					*/
/*									*/
/************************************************************************/

/*  1  */
static int docGetNoteIndex(		int *			pFound,
					const BufferDocument *	bd,
					int			paraNr,
					int			stroff )
    {
    DocumentNote *	dn;
    int			note;

    dn= bd->bdNotes;
    for ( note= 0; note < bd->bdNoteCount; dn++, note++ )
	{
	if  ( dn->dnParaNr < paraNr )
	    { continue;	}
	if  ( dn->dnParaNr > paraNr )
	    { break;	}

	if  ( dn->dnStroff < stroff )
	    { continue;	}
	if  ( dn->dnStroff > stroff )
	    { break;	}

	*pFound= 1; return note;
	}

    *pFound= 0; return note;
    }

int docInsertNote(			DocumentNote **		pDn,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			stroff,
					int			autoNumber )
    {
    int			paraNr;
    int			noteIndex;
    int			found;

    BufferItem *	sectBi;
    DocumentNote *	fresh;

    int			i;

    if  ( paraBi->biLevel != DOClevPARA		||
	  paraBi->biInExternalItem != DOCinBODY	)
	{
	SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
	return -1;
	}

    paraNr= docNumberOfParagraph( paraBi );
    if  ( paraNr < 0 )
	{ LDEB(paraNr); return -1;	}

    sectBi= paraBi;
    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    fresh= realloc( bd->bdNotes, ( bd->bdNoteCount+ 1 )* sizeof(DocumentNote) );
    if  ( ! fresh )
	{ LXDEB(bd->bdNoteCount,fresh); return -1;	}
    bd->bdNotes= fresh;

    noteIndex= docGetNoteIndex( &found, bd, paraNr, stroff );
    if  ( found )
	{ LDEB(found);	}

    /*  2  */
    if  ( noteIndex > 0 && fresh[noteIndex-1].dnParaNr < 0 )
	{ noteIndex--;	}
    else{
	for ( i= bd->bdNoteCount; i > noteIndex; i-- )
	    {
	    fresh[i]= fresh[i-1];

	    if  ( fresh[i].dnExternalItem.eiItem )
		{
		fresh[i].dnExternalItem.eiItem->
				    biSectSelectionScope.ssNoteArrayIndex= i;
		}
	    }
	}

    docInitNote( fresh+ noteIndex );

    /*  3  */
    if  ( autoNumber )
	{ fresh[noteIndex].dnNoteNumber= noteIndex;	}
    else{ fresh[noteIndex].dnNoteNumber= 0;		}

    fresh[noteIndex].dnSectNr= sectBi->biNumberInParent;
    fresh[noteIndex].dnParaNr= paraNr;
    fresh[noteIndex].dnStroff= stroff;
    fresh[noteIndex].dnAutoNumber= autoNumber;

    bd->bdNoteCount++;

    *pDn= fresh+ noteIndex; return noteIndex;
    }

/************************************************************************/
/*									*/
/*  Retrieve the note corresponding to the current position.		*/
/*									*/
/*  1)  It is either the note the current posion refers to,		*/
/*  2)  Or the note that contains the current position.			*/
/*									*/
/************************************************************************/

int docGetNote(		DocumentNote **		pDn,
			const BufferDocument *	bd,
			const BufferItem *	paraBi,
			int			stroff )
    {
    int			paraNr;
    int			noteIndex;
    int			found;

    if  ( paraBi->biLevel != DOClevPARA )
	{
	SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
	return -1;
	}

    /*  1  */
    if  ( paraBi->biInExternalItem == DOCinBODY	)
	{
	paraNr= docNumberOfParagraph( paraBi );
	if  ( paraNr < 0 )
	    { LDEB(paraNr); return -1;	}

	noteIndex= docGetNoteIndex( &found, bd, paraNr, stroff );
	if  ( ! found )
	    {
	    /* LLLDEB(paraNr,stroff,found); docListNotes( bd ); */
	    return -1;
	    }

	*pDn= bd->bdNotes+ noteIndex;
	return noteIndex;
	}

    /*  2  */
    if  ( paraBi->biInExternalItem == DOCinFOOTNOTE	||
	  paraBi->biInExternalItem == DOCinENDNOTE	)
	{
	const BufferItem *	noteSectBi;

	noteSectBi= paraBi;
	while( noteSectBi && noteSectBi->biLevel != DOClevSECT )
	    { noteSectBi= noteSectBi->biParent;	}

	if  ( ! noteSectBi )
	    { XDEB(noteSectBi); return -1;	}

	noteIndex= noteSectBi->biSectSelectionScope.ssNoteArrayIndex;

	*pDn= bd->bdNotes+ noteIndex;
	return noteIndex;
	}

    /*
    SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
    */
    return -1;
    }

/************************************************************************/
/*									*/
/*  Count the number f notes in the document.				*/
/*									*/
/************************************************************************/

int docCountNotes(		const BufferDocument *	bd )
    {
    int			count= 0;

    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}

	count++;
	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Return the first note on a page.					*/
/*									*/
/************************************************************************/

int docGetFirstNoteFromPage(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( dn->dnReferringPage == page )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOnPage(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	ExternalItem *	ei= &(dn->dnExternalItem);

	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( ei->eiPageFormattedFor == page )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOfSection(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			sect,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( dn->dnSectNr == sect )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOfDocument(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	*pDn= dn; return i;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Renumber the notes of a document.					*/
/*									*/
/************************************************************************/

void docRenumberNotes(		int *			pChanged,
				BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	np;

    int				footN= 0;
    int				endN= 0;

    int				page= -1;
    int				sect= -1;

    int				i;
    DocumentNote *		dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}

	if  ( ! dn->dnAutoNumber )
	    {
	    if  ( dn->dnNoteNumber != 0 )
		{ *pChanged= 1;	}

	    dn->dnNoteNumber= 0;
	    continue;
	    }

	/****/
	np= &(dp->dpFootnoteProperties);

	if  ( np->npRestart == DPftnRST_PER_SECTION	&&
	      dn->dnSectNr != sect			)
	    { footN= 0;	}

	if  ( np->npRestart == DPftnRST_PER_PAGE	&&
	      dn->dnReferringPage != page		)
	    { footN= 0;	}

	/****/
	np= &(dp->dpEndnoteProperties);

	if  ( np->npRestart == DPftnRST_PER_SECTION	&&
	      dn->dnSectNr != sect			)
	    { endN= 0;	}

	if  ( np->npRestart == DPftnRST_PER_PAGE	&&
	      dn->dnReferringPage != page		)
	    { endN= 0;	}

	/****/

	switch( dn->dnExternalItemKind )
	    {
	    case DOCinFOOTNOTE:
		if  ( dn->dnNoteNumber != footN )
		    { dn->dnNoteNumber=   footN; *pChanged= 1;	}
		footN++;
		break;

	    case DOCinENDNOTE:
		if  ( dn->dnNoteNumber != endN )
		    { dn->dnNoteNumber=   endN; *pChanged= 1;	}
		endN++;
		break;

	    default:
		LLDEB(i,dn->dnExternalItemKind);
		break;
	    }

	sect= dn->dnSectNr;
	page= dn->dnReferringPage;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt references from notes to the text because the number of	*/
/*  paragraphs changed in the document.					*/
/*									*/
/************************************************************************/

void docShiftNoteReferences(		BufferDocument *	bd,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    DocumentNote *	dn;
    int			i;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < paraFrom )
	    { continue;	}
	if  ( dn->dnParaNr > paraFrom )
	    {
	    if  ( sectShift == 0 && paraShift == 0 )
		{ break;	}

	    dn->dnSectNr += sectShift;
	    dn->dnParaNr += paraShift;
	    continue;
	    }

	if  ( dn->dnStroff < stroffFrom )
	    { continue;	}
	if  ( dn->dnStroff > stroffFrom )
	    {
	    dn->dnSectNr += sectShift;
	    dn->dnParaNr += paraShift;
	    dn->dnStroff += stroffShift;
	    continue;
	    }
	}

    return;
    }

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
/*  !)  The chftn field that precedes the note was inserted by the	*/
/*	caller.								*/
/*									*/
/************************************************************************/

int docMakeNote(	DocumentNote **			pDn,
			BufferDocument *		bd,
			const DocumentPosition *	dp,
			int				extItKind )
    {
    TextParticule *	tp;
    TextAttribute	ta;
    int			textAttributeNumberPlain;

    DocumentNote *	dn;
    BufferItem *	paraBi;

    DocumentField *	df;
    int			fieldNumber;
    int			noteIndex;

    int			stroffShift= 0;
    const int		autoNumber= 1;

    if  ( dp->dpBi->biInExternalItem != DOCinBODY )
	{ LDEB(dp->dpBi->biInExternalItem); return -1;	}


    /*  1  */
    noteIndex= docInsertNote( &dn, bd, dp->dpBi, dp->dpStroff, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return -1;	}

    dn->dnExternalItemKind= extItKind;

    if  ( dp->dpParticule > 0 )
	{ tp= dp->dpBi->biParaParticules+ dp->dpParticule- 1;	}
    else{ tp= dp->dpBi->biParaParticules+ dp->dpParticule;	}

    utilGetTextAttributeByNumber( &ta,
		&(bd->bdTextAttributeList), tp->tpTextAttributeNumber );
    ta.taSuperSub= DOCfontREGULAR;
    textAttributeNumberPlain=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &ta );
    /*
    ta.taSuperSub= DOCfontSUPERSCRIPT;
    textAttributeNumberSuper=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &ta );
    */

    /*  2  */
    tp= docInsertTextParticule( dp->dpBi, dp->dpParticule+ 1,
		    dp->dpStroff, 0, DOCkindNOTE, textAttributeNumberPlain );
    if  ( ! tp )
	{ LDEB(dp->dpBi->biParaParticuleCount); return -1; }

    /*  4  */
    paraBi= docMakeExternalparagraph( bd, &(dn->dnExternalItem),
					dp->dpBi, textAttributeNumberPlain,
					noteIndex, DOCinFOOTNOTE );
    if  ( ! paraBi )
	{ XDEB(paraBi); return -1;	}

    /*  5  */
    df= docClaimField( &fieldNumber, &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    df->dfKind= DOCfkCHFTN;

    tp= docInsertTextParticule( paraBi, 1, 0, 0, DOCkindFIELDEND,
						    textAttributeNumberPlain );
    if  ( ! tp )
	{ LDEB(paraBi->biParaParticuleCount); return -1; }

    tp= paraBi->biParaParticules;
    tp[0].tpKind= DOCkindFIELDSTART;
    tp[0].tpObjectNumber= fieldNumber;
    tp[1].tpObjectNumber= fieldNumber;

    {
    TextAttribute	taSet;
    PropertyMask	taSetMask;

    /*
    utilInitTextAttribute( &taSet );
    */
    taSet= ta;
    PROPmaskCLEAR( &taSetMask );

    taSet.taSuperSub= DOCfontSUPERSCRIPT;
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    if  ( docChangeParticuleAttributes( (PropertyMask *)0, bd,
						    paraBi, 0, 2,
						    &taSet, &taSetMask ) )
	{ LDEB(1);	}
    }

    /*  6  */
    if  ( docParaStringReplace( &stroffShift, paraBi,
				paraBi->biParaStrlen, paraBi->biParaStrlen,
				(const unsigned char *)" ", 1 ) )
	{ LDEB(paraBi->biParaStrlen); return -1; }

    tp= docInsertTextParticule( paraBi, paraBi->biParaParticuleCount,
				    paraBi->biParaStrlen- 1, 1,
				    DOCkindTEXT, textAttributeNumberPlain );
    if  ( ! tp )
	{ LDEB(paraBi->biParaParticuleCount); return -1; }

    bd->bdFieldList.dflFieldCount++;

    *pDn= dn; return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the note that corresponds to a particule.			*/
/*									*/
/************************************************************************/

int docDeleteNoteOfParticule(		BufferDocument *	bd,
					const BufferItem *	paraBi,
					const TextParticule *	tp )
    {
    DocumentNote *	dn;

    if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
	{ LDEB(tp->tpStroff); return -1;	}

    docCleanNote( bd, dn );
    docInitNote( dn );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the note corresponding to a selection.				*/
/*									*/
/************************************************************************/

int docGetSelectedNote(		DocumentNote **			pDn,
				BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    DocumentNote *		dn;
    int				noteIndex;

    noteIndex= docGetNote( &dn, bd, ds->dsEnd.dpBi, ds->dsEnd.dpStroff );
    if  ( noteIndex < 0 )
	{ return noteIndex;	}

    if  ( ds->dsSelectionScope.ssInExternalItem != DOCinFOOTNOTE	&&
	  ds->dsSelectionScope.ssInExternalItem != DOCinENDNOTE		&&
	  ! docIsIBarSelection( ds )					)
	{
	const TextParticule *		tpBegin;
	const TextParticule *		tpEnd;
	const DocumentField *		dfBegin;

	if  ( ! docIsParaSelection( ds ) )
	    { return -1;	}

	tpBegin= ds->dsBegin.dpBi->biParaParticules+ ds->dsBegin.dpParticule;
	if  ( tpBegin->tpKind != DOCkindFIELDSTART )
	    { return -1;	}

	dfBegin= bd->bdFieldList.dflFields+ tpBegin->tpObjectNumber;
	if  ( dfBegin->dfKind != DOCfkCHFTN )
	    { return -1;	}

	tpEnd= ds->dsEnd.dpBi->biParaParticules+ ds->dsEnd.dpParticule;
	if  ( tpEnd->tpKind != DOCkindFIELDEND )
	    { return -1;	}

	if  ( tpEnd->tpObjectNumber != tpBegin->tpObjectNumber )
	    { return -1;	}
	}

    *pDn= dn;
    return noteIndex;
    }

/************************************************************************/
/*									*/
/*  Verify that a certain kind of notes separator exists. Make it if	*/
/*  not.								*/
/*									*/
/*  1)  Correctly handeled by docTextAttributeNumber().			*/
/*									*/
/************************************************************************/

int docCheckNoteSeparatorItem(		BufferDocument *	bd,
					int			extItKind )
    {
    ExternalItem *	ei;

    ei= docDocumentNoteSeparator( bd, extItKind );
    if  ( ! ei )
	{ LXDEB(extItKind,ei); return -1;	}

    if  ( ! ei->eiItem )
	{
	const int		noteIndex= -1;
	int			attributeNumber;

	BufferItem *	bi;
	TextAttribute	ta;

	utilInitTextAttribute( &ta );
	ta.taFontNumber= -1; /*  1  */
	ta.taFontSizeHalfPoints= 24;

	attributeNumber= docTextAttributeNumber( bd, &ta );
	if  ( attributeNumber < 0 )
	    { LDEB(attributeNumber); return -1;	}

	bi= docMakeExternalparagraph( bd, ei, &(bd->bdItem), attributeNumber,
						    noteIndex, extItKind );
	if  ( ! bi )
	    { XDEB(bi);	}
	else{
	    if  ( docInflateTextString( bi, 1 ) )
		{ LDEB(bi->biParaStrlen);	}
	    else{
		TextParticule *	tp= bi->biParaParticules;

		tp->tpKind= DOCkindCHFTNSEP;
		tp->tpStrlen= 1;
		bi->biParaStrlen++;
		}
	    }
	}

    return 0;
    }

