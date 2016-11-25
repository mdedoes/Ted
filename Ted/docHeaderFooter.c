/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
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
/*  Determine what header/footer applies for a certain page.		*/
/*									*/
/************************************************************************/

int docWhatPageHeader(	ExternalItem **			pEi,
			BufferItem *			bodySectBi,
			int				page,
			const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);

    if  ( page == bodySectBi->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	*pEi= &(bodySectBi->biSectFirstPageHeader);
	return DOCinFIRST_HEADER;
	}

    if  ( ! dp->dpHasFacingPages )
	{
	*pEi= &(bodySectBi->biSectHeader);
	return DOCinSECT_HEADER;
	}

    if  ( page % 2 )
	{
	*pEi= &(bodySectBi->biSectLeftPageHeader);
	return DOCinLEFT_HEADER;
	}
    else{
	*pEi= &(bodySectBi->biSectRightPageHeader);
	return DOCinRIGHT_HEADER;
	}
    }

int docWhatPageFooter(	ExternalItem **			pEi,
			BufferItem *			bodySectBi,
			int				page,
			const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);

    if  ( page == bodySectBi->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	*pEi= &(bodySectBi->biSectFirstPageFooter);
	return DOCinFIRST_FOOTER;
	}

    if  ( ! dp->dpHasFacingPages )
	{
	*pEi= &(bodySectBi->biSectFooter);
	return DOCinSECT_FOOTER;
	}

    if  ( page % 2 )
	{
	*pEi= &(bodySectBi->biSectLeftPageFooter);
	return DOCinLEFT_FOOTER;
	}
    else{
	*pEi= &(bodySectBi->biSectRightPageFooter);
	return DOCinRIGHT_FOOTER;
	}
    }

/************************************************************************/
/*									*/
/*  Return a particular header or footer by scope.			*/
/*									*/
/************************************************************************/

ExternalItem *	docSectionHeaderFooter(	BufferItem *		bodySectBi,
					int			which )
    {
    switch( which )
	{
	case DOCinBODY:
	    LDEB(which); return (ExternalItem *)0;

	case DOCinSECT_HEADER:
	    return &(bodySectBi->biSectHeader);

	case DOCinFIRST_HEADER:
	    return &(bodySectBi->biSectFirstPageHeader);

	case DOCinLEFT_HEADER:
	    return &(bodySectBi->biSectLeftPageHeader);

	case DOCinRIGHT_HEADER:
	    return &(bodySectBi->biSectRightPageHeader);

	case DOCinSECT_FOOTER:
	    return &(bodySectBi->biSectFooter);

	case DOCinFIRST_FOOTER:
	    return &(bodySectBi->biSectFirstPageFooter);

	case DOCinLEFT_FOOTER:
	    return &(bodySectBi->biSectLeftPageFooter);

	case DOCinRIGHT_FOOTER:
	    return &(bodySectBi->biSectRightPageFooter);

	default:
	    LDEB(which); return (ExternalItem *)0;
	}
    }

int docSectionHeaderFooterFirstPage(
				int *				pExists,
				const BufferItem *		bodySectBi,
				int				which,
				const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);
    int				topPage= bodySectBi->biTopPosition.lpPage;
    int				page;

    if  ( bodySectBi->biNumberInParent > 0 )
	{
	const	BufferItem *	prevBi;

	prevBi= bodySectBi->biParent->biChildren[
					    bodySectBi->biNumberInParent- 1];

	if  ( prevBi->biBelowPosition.lpPage >= topPage )
	    { return -1;	}
	}

    switch( which )
	{
	case DOCinBODY:
	    LDEB(which); return -1;

	case DOCinSECT_HEADER:
	case DOCinSECT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{ return -1;	}
	    if  ( sp->spHasTitlePage )
		{ page= topPage+ 1;	}
	    else{ page= topPage;	}
	    break;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    if  ( ! sp->spHasTitlePage )
		{ return -1;	}
	    page= topPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ return -1;	}
	    if  ( topPage % 2 )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ return -1;	}
	    if  ( ! ( topPage % 2 ) )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	default:
	    LDEB(which); return -1;
	}

    *pExists= page <= bodySectBi->biBelowPosition.lpPage;

    return page;
    }

/************************************************************************/
/*									*/
/*  Return a particular separator of notice by scope.			*/
/*									*/
/************************************************************************/

ExternalItem *	docDocumentNoteSeparator(
					BufferDocument *	bd,
					int			which )
    {
    switch( which )
	{
	case DOCinFTNSEP:
	    return &(bd->bdEiFtnsep);

	case DOCinFTNSEPC:
	    return &(bd->bdEiFtnsepc);

	case DOCinFTNCN:
	    return &(bd->bdEiFtncn);

	case DOCinAFTNSEP:
	    return &(bd->bdEiAftnsep);

	case DOCinAFTNSEPC:
	    return &(bd->bdEiAftnsepc);

	case DOCinAFTNCN:
	    return &(bd->bdEiAftncn);

	default:
	    LDEB(which); return (ExternalItem *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Get the external item that an item is contained in.			*/
/*									*/
/************************************************************************/

int docGetRootOfSelectionScope(	ExternalItem **		pEi,
				BufferItem **		pSelRootBi,
				BufferItem **		pBodySectBi,
				BufferDocument *	bd,
				const SelectionScope *	ss )
    {
    BufferItem *	docBi= &(bd->bdItem);
    BufferItem *	bodySectBi;
    BufferItem *	selRootBi;
    ExternalItem *	ei;

    int			noteIndex;
    DocumentNote *	dn;

    switch( ss->ssInExternalItem )
	{
	case DOCinBODY:
	    selRootBi= docBi;
	    bodySectBi= (BufferItem *)0;
	    ei= (ExternalItem *)0;
	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    bodySectBi= docBi->biChildren[ss->ssSectNrExternalTo];
	    if  ( ! bodySectBi || bodySectBi->biLevel != DOClevSECT )
		{ XDEB(bodySectBi); return -1;	}

	    ei= docSectionHeaderFooter( bodySectBi, ss->ssInExternalItem );
	    if  ( ! ei )
		{ LXDEB(ss->ssInExternalItem,ei); return -1;	}
	    selRootBi= ei->eiItem;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    noteIndex= ss->ssNoteArrayIndex;
	    if  ( noteIndex < 0 || noteIndex >= bd->bdNoteCount )
		{
		LLDEB(noteIndex,bd->bdNoteCount);
		return -1;
		}
	    dn= bd->bdNotes+ noteIndex;
	    ei= &(dn->dnExternalItem);
	    selRootBi= ei->eiItem;

	    if  ( dn->dnSectNr < 0 || dn->dnSectNr >= docBi->biChildCount )
		{ LLDEB(dn->dnSectNr,docBi->biChildCount); return -1; }

	    bodySectBi= docBi->biChildren[dn->dnSectNr];
	    if  ( ! bodySectBi || bodySectBi->biLevel != DOClevSECT )
		{ XDEB(bodySectBi); return -1;	}

	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    ei= docDocumentNoteSeparator( bd, ss->ssInExternalItem );
	    if  ( ! ei )
		{ LXDEB(ss->ssInExternalItem,ei); return -1;	}
	    selRootBi= ei->eiItem;
	    bodySectBi= (BufferItem *)0;
	    break;

	default:
	    LDEB(ss->ssInExternalItem);
	    return -1;
	}

    *pEi= ei;
    *pSelRootBi= selRootBi;
    *pBodySectBi= bodySectBi;

    return 0;
    }

int docGetExternalItem(		ExternalItem **		pEi,
				BufferItem **		pBodySectBi,
				BufferDocument *	bd,
				BufferItem *		bi )
    {
    BufferItem *	selRootBi;

    BufferItem *	selSectBi;
    BufferItem *	bodySectBi;

    ExternalItem *	ei;

    SelectionScope *	ss;

    while( bi && bi->biLevel != DOClevSECT )
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    selSectBi= bi;
    ss= &(selSectBi->biSectSelectionScope);

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi, &bodySectBi, bd, ss ) )
	{ LDEB(1); return -1;	}

    if  ( ss->ssInExternalItem == DOCinBODY )
	{ bodySectBi= selSectBi;	}

    *pEi= ei;
    *pBodySectBi= bodySectBi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  What BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

BufferItem * docGetSelectionRoot(
			ExternalItem **			pEi,
			BufferItem **			pBodySectBi,
			BufferDocument *		bd,
			const DocumentSelection *	ds )
    {
    BufferItem *	bi1= ds->dsBegin.dpBi;
    BufferItem *	bi2= ds->dsEnd.dpBi;

    ExternalItem *	ei= (ExternalItem *)0;
    BufferItem *	selRootBi= (BufferItem *)0;
    BufferItem *	bodySectBi= (BufferItem *)0;

    if  ( ! bi1 || ! bi2 )
	{ XXDEB(bi1,bi2); return (BufferItem *)0;	}

    while( bi1 && bi1->biLevel > bi2->biLevel )
	{ bi1= bi1->biParent;	}

    if  ( ! bi1 )
	{
	XDEB(bi1);
	LLDEB(ds->dsBegin.dpBi->biLevel,ds->dsEnd.dpBi->biLevel);
	/*
	return (BufferItem *)0;
	*/
	}

    while( bi2 && bi2->biLevel > bi1->biLevel )
	{ bi2= bi2->biParent;	}

    while( bi1					&&
	   bi2					&&
	   bi1 != bi2				)
	{ bi1= bi1->biParent; bi2= bi2->biParent; }

    if  ( ! bi1 || ! bi2 )
	{ XXDEB(bi1,bi2); return (BufferItem *)0;	}

    selRootBi= bi1;

    if  ( selRootBi->biInExternalItem != DOCinBODY )
	{
	if  ( docGetExternalItem( &ei, &bodySectBi, bd, bi1 ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    if  ( pEi )
	{ *pEi= ei;			}
    if  ( pBodySectBi )
	{ *pBodySectBi= bodySectBi;	}

    return selRootBi;
    }

/************************************************************************/
/*									*/
/*  Return the Header/Footer corrsponding to 'which'. If the selection	*/
/*  itself is in an external item, go to the section in the body to	*/
/*  get the header or footer.						*/
/*									*/
/************************************************************************/

int docGetHeaderFooter(		ExternalItem **			pEi,
				BufferItem **			pBodySectBi,
				const DocumentSelection *	ds,
				BufferDocument *		bd,
				int				which )
    {
    ExternalItem *	ei;
    BufferItem *	bodySectBi;

    if  ( docGetExternalItem( &ei, &bodySectBi, bd, ds->dsBegin.dpBi ) )
	{ LDEB(1); return -1;	}

    ei= docSectionHeaderFooter( bodySectBi, which );
    if  ( ! ei )
	{ XDEB(ei); return -1;	}

    *pEi= ei; *pBodySectBi= bodySectBi; return 0;
    }

