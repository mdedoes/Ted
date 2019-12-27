/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/*  Functionality to format notes and to subtract the space occupied by	*/
/*  notes and footnotes from what is left over prom the page (or the	*/
/*  column).								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"layoutContext.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docSelect.h>
#   include	<docDocumentProperties.h>
#   include	<docBuf.h>
#   include	<docFields.h>
#   include	<docBlockFrame.h>

#   include	<appDebugon.h>

#   define	SHOW_NOTE_LAYOUT	0	/* 0: nothing		*/
						/* 1: layout		*/
						/* 2: reservations	*/

/************************************************************************/
/*									*/
/*  Commit the space allocated for footnotes and the footnote separator	*/
/*									*/
/************************************************************************/

static void docLayoutCommitNotesReservation(
				NotesReservation *		nrTotal,
				const NotesReservation *	nr )
    {
    if  ( ! nrTotal->nrDfFirstFootnote		&&
	  nr->nrDfFirstFootnote			)
	{
	nrTotal->nrDfFirstFootnote= nr->nrDfFirstFootnote;
	nrTotal->nrFootnoteSectNode= nr->nrFootnoteSectNode;
	}

    if  ( nrTotal->nrFtnsepHeight == 0		&&
	  nr->nrFtnsepHeight > 0		)
	{ nrTotal->nrFtnsepHeight= nr->nrFtnsepHeight;	}

    nrTotal->nrFootnoteCount += nr->nrFootnoteCount;
    nrTotal->nrFootnoteHeight += nr->nrFootnoteHeight;

    return;
    }

/************************************************************************/
/*									*/
/*  Collect the sum of the height of footnotes between two positions	*/
/*  in the same paragraph. This routine and the subsequent one are	*/
/*  used to subtract the space claimed for footnotes on the current	*/
/*  page at the beginning of a formatting action.			*/
/*									*/
/************************************************************************/

static int docLayoutCollectOneFootnote(
				NotesReservation *		nr,
				int *				pHigh,
				struct BufferDocument *		bd,
				const struct BufferItem *	bodySectNode,
				const DocumentNote *		dn,
				DocumentField *			dfNote )
    {
    const struct DocumentTree *	tree;
    int				high;

    if  ( nr->nrFootnoteCount == 0 )
	{
	nr->nrDfFirstFootnote= dfNote;
	nr->nrFootnoteSectNode= bodySectNode;

	tree= docDocumentNoteSeparator( bd, DOCinFTNSEP );
	if  ( ! tree )
	    { LXDEB(DOCinFTNSEP,tree); return -1;	}
	if  ( ! tree->dtRoot )
	    { LXDEB(DOCinFTNSEP,tree->dtRoot);	}

	high= tree->dtY1UsedTwips- tree->dtY0UsedTwips;
	nr->nrFtnsepHeight= high;
	}

    tree= &(dn->dnDocumentTree);
    high= tree->dtY1UsedTwips- tree->dtY0UsedTwips;

    nr->nrFootnoteCount++;
    nr->nrFootnoteHeight += high;

    if  ( pHigh )
	{ *pHigh= high;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect the footnotes that belong to a range in a paragraph.	*/
/*  If the range plus the notes fit on the page, the caller will	*/
/*  convert the range into a line and place the notes on this page.	*/
/*									*/
/************************************************************************/

int docLayoutCollectParaFootnoteHeight(
				NotesReservation *		nrTotal,
				int				referringPage,
				int				referringColumn,
				struct BufferDocument *		bd,
				const struct BufferItem *	bodySectNode,
				const struct BufferItem *	paraNode,
				int				partFrom,
				int				partUpto )
    {
    int			part;
    TextParticule *	tp;
    
    NotesReservation	nrLocal;

    if  ( paraNode->biTreeType != DOCinBODY )
	{ return 0;	}

    docInitNotesReservation( &nrLocal );

    if  ( partUpto > paraNode->biParaParticuleCount )
	{ LLDEB(partUpto,paraNode->biParaParticuleCount); return -1;	}

    tp= paraNode->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
	DocumentField *	df;
	DocumentNote *	dn;

	if  ( tp->tpKind != TPkindFIELDHEAD )
	    { continue;	}
	df= docGetFieldByNumber( bd, tp->tpObjectNumber );
	if  ( ! df || df->dfKind != DOCfkCHFTN )
	    { continue;	}
	dn= docGetNoteOfField( df, bd );
	if  ( ! dn )
	    { XDEB(dn); continue;	}

	dn->dnReferringPage= referringPage;
	dn->dnReferringColumn= referringColumn;

	if  ( docGetNotePosition( dn, bd ) == FTNplacePAGE_END )
	    {
	    if  ( docLayoutCollectOneFootnote( &nrLocal, (int *)0,
						    bd, bodySectNode, dn, df ) )
		{ LDEB(df->dfNoteIndex); return -1;	}

#	    if SHOW_NOTE_LAYOUT > 1
	    appDebug( "PAGE %3d:%d FOOTNOTE      --%5dtw note %d (Reserve)\n",
						referringPage,
						referringColumn,
						high, df->dfNoteIndex );
#	    endif
	    }
	}

    docLayoutCommitNotesReservation( nrTotal, &nrLocal );

#   if SHOW_NOTE_LAYOUT > 1
    if  ( nrTotal->nrFootnoteCount > 0 )
	{
	appDebug( "PAGE %3d:%d FOOTNOTES     --%5dtw %d notes (Reserved)\n",
					    referringPage,
					    referringColumn,
					    nrTotal->nrFootnoteHeight,
					    nrTotal->nrFootnoteCount );
	}
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect the footnotes from a certain column above the beginning of	*/
/*  a layout action.							*/
/*									*/
/*  Usually, this is the column where we start a layout action.		*/
/*  On pages with different numbers of columns, it may be from the	*/
/*  beginning of the page.						*/
/*									*/
/************************************************************************/

static int docCollectFootnotesForNode(
			    int *			pReady,
			    NotesReservation *		nr,
			    struct BufferDocument *	bd,
			    const struct BufferItem *	bodySectNode,
			    const struct BufferItem *	node,
			    const DocumentPosition *	dpUpto,
			    int				partUpto,
			    int				referringPage,
			    int				referringColumn )
    {
    int		i;

    if  ( node == dpUpto->dpNode )
	{ *pReady= 1;	}

    /*  NO!! Bottom positions are not yet known if layout is 
        incomplete.
	Filter on the page of the lines inside the paragraphs instead.

    if  ( docCompareLayoutPositionToFrame( &(node->biBelowPosition),
				    referringPage, referringColumn ) < 0 )
	{ goto ready;		}
    */

    if  ( docCompareLayoutPositionToFrame( &(node->biTopPosition),
				    referringPage, referringColumn ) > 0 )
	{ goto ready;		}

    for ( i= 0; i < node->biChildCount; i++ )
	{
	if  ( node->biTreeType == DOCinBODY			&&
	      node->biChildren[i]->biLevel == DOClevSECT	)
	    { bodySectNode= node->biChildren[i];	}

	if  ( docCollectFootnotesForNode( pReady, nr, bd,
					    bodySectNode, node->biChildren[i],
					    dpUpto, partUpto,
					    referringPage, referringColumn ) )
	    { LLDEB(referringPage,referringColumn); return -1;	}

	if  ( *pReady )
	    { goto ready;	}
	}

    if  ( node->biLevel == DOClevPARA )
	{
	const TextLine *	tl= node->biParaLines;

	for ( i= 0; i < node->biParaLineCount; tl++, i++ )
	    {
	    int		from;
	    int		upto;

	    if  ( docCompareLayoutPositionToFrame( &(tl->tlTopPosition),
				    referringPage, referringColumn ) < 0 )
		{ continue;		}

	    from= tl->tlFirstParticule;
	    upto= tl->tlFirstParticule+ tl->tlParticuleCount;

	    if  ( node == dpUpto->dpNode && upto > partUpto )
		{ upto= partUpto;	}

	    if  ( upto > from )
		{
		/* Filtering should have been done by the caller */
		if  ( docCompareLayoutPositionToFrame( &(tl->tlTopPosition),
				    referringPage, referringColumn ) > 0 )
		    {
		    LLDEB(tl->tlTopPosition.lpPage,referringPage);
		    LLDEB(tl->tlTopPosition.lpColumn,referringColumn);
		    break;
		    }

		if  ( docLayoutCollectParaFootnoteHeight( nr,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn,
					bd, bodySectNode, node, from, upto ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( node == dpUpto->dpNode && upto >= partUpto )
		{ /* Above: *pReady= 1; */ break;	}
	    }
	}

  ready:

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect footnotes that have been formatted earlier.			*/
/*									*/
/************************************************************************/

int docCollectFootnotesFromColumn(
			BlockFrame *			bf,
			const DocumentPosition *	dpHere,
			int				partHere,
			struct BufferDocument *		bd,
			int				referringPage,
			int				referringColumn )
    {
    NotesReservation		nr;
    int				ready= 0;

    docInitNotesReservation( &nr );

    /*  Does not work because part of the document might not be 
        formatted yet.
    if  ( docGetTopOfColumn( &dpPageTop, &linePageTop, &partPageTop, bd,
					referringPage, referringColumn ) )
	{ LLDEB(referringPage,referringColumn); return -1;	}
    */

    if  ( docCollectFootnotesForNode( &ready, &nr, bd,
			bd->bdBody.dtRoot->biChildren[0], bd->bdBody.dtRoot,
			dpHere, partHere, referringPage, referringColumn ) )
	{ LDEB(1); return -1;	}

    bf->bfNotesReservation= nr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place the lines of a footnote on the page that refers to it.	*/
/*									*/
/*  2)  Get the separator between body and notes and calculate its	*/
/*	height.								*/
/*  3)  Start a new layout job directly below the section text.		*/
/*  4)  When the notes are to be placed at the bottom of the page, find	*/
/*	another start position.						*/
/*  5)  Skip the space reserved for the separator line between the	*/
/*	body text and the notes.					*/
/*  6)  Place the notes one below the other from the calculated top	*/
/*	position. As the notes are supposed to be small, their layout	*/
/*	is recalculated at little extra cost.				*/
/*  7)  Force the footnote separator to be reformatted later on.	*/
/*									*/
/************************************************************************/

int docLayoutFootnotesForColumn(	LayoutPosition *	lpBelowNotes,
					const LayoutPosition *	lpBelowText,
					const BlockFrame *	refBf,
					int			belowText,
					const LayoutJob *	refLj )
    {
    const LayoutContext *	lc= refLj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const NotesReservation *	refNr= &(refBf->bfNotesReservation);
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npFootnotes= &(dp->dpNotesProps.fepFootnotesProps);

    LayoutJob			notesLj;
    LayoutPosition		lpHere;

    struct DocumentTree *	tree;
    struct DocumentTree *	noteSepTree;
    int				notesDone;

    int				sepHigh= 0;

    DocumentField *		dfNote;
    DocumentNote *		dn;

    if  ( refBf->bfFootnotesPlaced )
	{ LDEB(refBf->bfFootnotesPlaced); return 0;	}

    /*  2  */
    noteSepTree= docDocumentNoteSeparator( bd, DOCinFTNSEP );
    if  ( ! noteSepTree )
	{ LXDEB(DOCinFTNSEP,noteSepTree); return -1;	}
    if  ( ! noteSepTree->dtRoot )
	{ LXDEB(DOCinFTNSEP,noteSepTree->dtRoot);	}

    sepHigh= noteSepTree->dtY1UsedTwips- noteSepTree->dtY0UsedTwips;

    /*  3  */
    notesLj= *refLj;
    lpHere.lpPage= lpBelowText->lpPage;
    lpHere.lpColumn= lpBelowText->lpColumn;
    lpHere.lpPageYTwips= lpBelowText->lpPageYTwips;
    lpHere.lpAtTopOfColumn= 1; /* not really */

    /*  4  */
    if  ( ! belowText && npFootnotes->npJustification == FTNjustifyPAGE_BOTTOM )
	{
	int		high= sepHigh;

	dfNote= refNr->nrDfFirstFootnote;
	dn= docGetNoteOfField( dfNote, bd );
	notesDone= 0;

	for ( ;
	      dfNote && notesDone < refNr->nrFootnoteCount;
	      dfNote= docGetNextNoteInDocument( &dn, bd,
						    dfNote, DOCinFOOTNOTE ) )
	    {
	    if  ( ! dn )
		 { XDEB(dn); continue;	}
	    tree= &(dn->dnDocumentTree);
	    if  ( ! tree->dtRoot )
		{ XDEB(tree->dtRoot); continue;	}

	    high += tree->dtY1UsedTwips- tree->dtY0UsedTwips;
	    notesDone++;
	    }

	lpHere.lpPageYTwips= refBf->bfContentRect.drY1- high;
	}

    /*  5  */
    lpHere.lpPageYTwips += sepHigh;

    /*  6  */
    dfNote= refNr->nrDfFirstFootnote;
    dn= docGetNoteOfField( dfNote, bd );
    notesDone= 0;
    for ( ;
          dfNote && notesDone < refNr->nrFootnoteCount;
	  dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, DOCinFOOTNOTE ) )
	{
	int			y0Twips= lpHere.lpPageYTwips;

	/*  7  */
	docInvalidateTreeLayout( noteSepTree );

	if  ( ! dn )
	    { XDEB(dn); continue;	}
	tree= &(dn->dnDocumentTree);
	if  ( ! tree->dtRoot )
	    { XDEB(tree->dtRoot); continue;	}

	if  ( tree->dtPageFormattedFor != lpBelowText->lpPage	||
	      tree->dtColumnFormattedFor != lpBelowText->lpColumn	||
	      tree->dtY0UsedTwips != y0Twips			)
	    {
	    struct DocumentTree *	bodyTree;
	    BlockFrame		bfNote;
	    struct BufferItem *	bodySectNode;

	    if  ( docGetRootOfSelectionScope( &bodyTree, &bodySectNode,
					    bd, &(dfNote->dfSelectionScope) ) )
		{ LDEB(1); return -1;	}

	    notesLj.ljBodySectNode= bodySectNode;

	    docLayoutInitBlockFrame( &bfNote );
	    if  ( docLayoutGetInitialFrame( &bfNote, &notesLj, &lpHere,
								tree->dtRoot ) )
		{ LDEB(1); return -1;	}

	    if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
					    tree->dtRoot, &bfNote, &notesLj ) )
		{ LDEB(1); return -1;	}

	    /*  HACK to avoid corruption of the height administration.
	        Eventually, this should be done differently.
	     */
	    if  ( lpHere.lpPage != lpBelowText->lpPage		||
		  lpHere.lpColumn != lpBelowText->lpColumn	)
		{
		if  ( refLj->ljBalancePage < 0 )
		    {
		    LLDEB(lpHere.lpPage,lpBelowText->lpPage);
		    LLDEB(lpHere.lpColumn,lpBelowText->lpColumn);
		    }
		}
	    else{
		tree->dtPageFormattedFor= lpBelowText->lpPage;
		tree->dtColumnFormattedFor= lpBelowText->lpColumn;
		tree->dtY0UsedTwips= y0Twips;
		tree->dtY1UsedTwips= lpHere.lpPageYTwips;
		}
	    }
	else{
	    lpHere.lpPageYTwips= tree->dtY1UsedTwips;
	    }

#	if SHOW_NOTE_LAYOUT > 0
	appDebug( "PAGE %3d:%d NOTE          ::%5dtw note %d (Layout)\n",
			    lpBelowText->lpPage,
			    lpBelowText->lpColumn,
			    tree->dtRoot->biBelowPosition.lpPageYTwips-
				tree->dtRoot->biTopPosition.lpPageYTwips,
			    dfNote->dfNoteIndex );
#       endif

	notesDone++;
	}

    *lpBelowNotes= lpHere;
    lpBelowNotes->lpAtTopOfColumn= lpBelowText->lpAtTopOfColumn;

    return 0;
    }

/************************************************************************/

static int docLayoutEndnoteSeparator(	LayoutPosition *	lpHere,
					struct DocumentTree *	noteSepTree,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    int		y0Twips= lpHere->lpPageYTwips;

    /*  3  */
    if  ( docLayoutNodeImplementation( lpHere, lpHere,
					noteSepTree->dtRoot, bf, lj ) )
	{ LDEB(1); return -1;	}

    noteSepTree->dtPageFormattedFor=
			    noteSepTree->dtRoot->biTopPosition.lpPage;
    noteSepTree->dtColumnFormattedFor=
			    noteSepTree->dtRoot->biTopPosition.lpColumn;
    noteSepTree->dtY0UsedTwips= y0Twips;
    noteSepTree->dtY1UsedTwips= lpHere->lpPageYTwips;

    /*  7  */
    docInvalidateTreeLayout( noteSepTree );

    return 0;
    }

static int docLayoutEndnote(	LayoutPosition *	lpHere,
				struct DocumentTree *	noteTree,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int		y0Twips= lpHere->lpPageYTwips;

    if  ( ! noteTree->dtRoot )
	{ XDEB(noteTree->dtRoot); return 0;	}

    if  ( docLayoutNodeImplementation( lpHere, lpHere,
					noteTree->dtRoot, bf, lj ) )
	{ LDEB(1); return -1;	}

    noteTree->dtPageFormattedFor= noteTree->dtRoot->biTopPosition.lpPage;
    noteTree->dtColumnFormattedFor= noteTree->dtRoot->biTopPosition.lpColumn;
    noteTree->dtY0UsedTwips= y0Twips;
    noteTree->dtY1UsedTwips= lpHere->lpPageYTwips;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place the notes belonging to a section immediately below its text.	*/
/*									*/
/*  1)  Are there any?							*/
/*  2)  Get separator.							*/
/*  2a) If there are footnotes on the same page, the footnotes come	*/
/*	first.								*/
/*  3)  Layout of separator.						*/
/*  4)  Layout of the endnotes of this section.				*/
/*	If the first note modes to the next page, retry with the	*/
/*	separator on that page as well.					*/
/*  7)  Force the footnote separator to be reformatted later on.	*/
/*									*/
/************************************************************************/

int docLayoutEndnotesForSection(	LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					int			sect,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const NotesProperties *	npFootnotes= &(dp->dpNotesProps.fepFootnotesProps);

    DocumentNote *		dn;
    DocumentField *		dfFirstNote;

    struct DocumentTree *	noteSepTree;

    int				attempt;

    LayoutPosition		lpHere= *lpTop;

    /*  1  */
    if  ( sect >= 0 )
	{
	dfFirstNote= docGetFirstNoteOfSection( &dn, bd, sect, DOCinENDNOTE );
	}
    else{
	dfFirstNote= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );
	}

    if  ( ! dfFirstNote )
	{ return 0;	}

    /*  2  */
    noteSepTree= docDocumentNoteSeparator( bd, DOCinAFTNSEP );
    if  ( ! noteSepTree )
	{ LXDEB(DOCinAFTNSEP,noteSepTree); return -1;	}
    if  ( ! noteSepTree->dtRoot )
	{ LXDEB(DOCinAFTNSEP,noteSepTree->dtRoot);	}

    /*  2a  */
    if  ( BF_HAS_FOOTNOTES( bf )				&&
	  npFootnotes->npJustification == FTNjustifyPAGE_BOTTOM	)
	{
	const int	belowText= 1;

	if  ( docLayoutFootnotesForColumn( &lpHere, &lpHere, bf,
							    belowText, lj ) )
	    { LDEB(1); return -1;	}

	bf->bfFootnotesPlaced= 1;
	}

    for ( attempt= 0; attempt < 2; attempt++ )
	{
	int		retry= 0;
	DocumentField *	dfNote;

	/*  3  */
	if  ( noteSepTree->dtRoot )
	    {
	    if  ( docLayoutEndnoteSeparator( &lpHere, noteSepTree, bf, lj ) )
		{ LDEB(1); return -1;	}
	    }

	/*  4  */
	for ( dfNote= dfFirstNote;
	      dfNote;
	      dfNote= docGetNextNoteInSection( &dn, bd, sect,
						    dfNote, DOCinENDNOTE ) )
	    {
	    if  ( docLayoutEndnote( &lpHere, &(dn->dnDocumentTree), bf, lj ) )
		{ LDEB(1); return -1;	}

	    if  ( attempt == 0						&&
		  dfNote == dfFirstNote					&&
		  noteSepTree->dtRoot					&&
		  noteSepTree->dtRoot->biTopPosition.lpPage <
			dn->dnDocumentTree.dtRoot->biTopPosition.lpPage	)
		{
		lpHere= dn->dnDocumentTree.dtRoot->biTopPosition;
		retry= 1;
		break;
		}
	    }

	if  ( ! retry )
	    { break;	}
	}

    *lpBelow= lpHere;
    return 0;
    }

int docLayoutEndnotesForDocument(	LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    return docLayoutEndnotesForSection( lpBelow, lpTop, -1, bf, lj );
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle that the note separator would occupy on a	*/
/*  certain page.							*/
/*									*/
/************************************************************************/

int docNoteSeparatorRectangle(	DocumentRectangle *		drExtern,
				struct DocumentTree **		pNoteSepTree,
				int *				pY0Twips,
				const DocumentNote *		dnFirstNote,
				int				treeType,
				const struct BufferItem *	bodySectNode,
				const LayoutContext *		lc )
    {
    const struct DocumentTree *	eiFirstNote= &(dnFirstNote->dnDocumentTree);
    struct DocumentTree *	noteSepTree;

    int				page;
    int				column;

    int				y0Twips;
    int				y1Twips;

    int				high;

    BlockFrame			bfNoteSep;
    DocumentRectangle		drTwips;

    noteSepTree= docDocumentNoteSeparator( lc->lcDocument, treeType );
    if  ( ! noteSepTree )
	{ LXDEB(treeType,noteSepTree); return -1;	}
    if  ( ! noteSepTree->dtRoot )
	{ LXDEB(treeType,noteSepTree->dtRoot); return 1;	}

    page= eiFirstNote->dtRoot->biTopPosition.lpPage;
    column= eiFirstNote->dtRoot->biTopPosition.lpColumn;
    y1Twips= eiFirstNote->dtRoot->biTopPosition.lpPageYTwips;
    high= noteSepTree->dtY1UsedTwips- noteSepTree->dtY0UsedTwips;
    y0Twips= y1Twips- high;

    docLayoutInitBlockFrame( &bfNoteSep );
    docSectionBlockFrameTwips( &bfNoteSep, eiFirstNote->dtRoot,
				bodySectNode, lc->lcDocument, page, column );

    drTwips.drX0= bfNoteSep.bfContentRect.drX0;
    drTwips.drX1= bfNoteSep.bfContentRect.drX1;
    drTwips.drY0= y0Twips;
    drTwips.drY1= y1Twips;

    docGetPixelRect( drExtern, lc, &drTwips, page );

    *pNoteSepTree= noteSepTree;
    *pY0Twips= y0Twips;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Commit the space allocated for footnotes and the footnote separator	*/
/*  in a frame.								*/
/*									*/
/************************************************************************/

void docLayoutReserveNoteHeight(	BlockFrame *			bf,
					const NotesReservation *	nr )
    {
    int			footnoteHeight;
    NotesReservation *	nrBf= &(bf->bfNotesReservation);

    docLayoutCommitNotesReservation( nrBf, nr );

    footnoteHeight= nrBf->nrFtnsepHeight+ nrBf->nrFootnoteHeight;

    if  ( bf->bfFlowRect.drY1 > bf->bfContentRect.drY1- footnoteHeight )
	{ bf->bfFlowRect.drY1=  bf->bfContentRect.drY1- footnoteHeight;	}

#   if SHOW_NOTE_LAYOUT > 1
    if  ( nrBf->nrFootnoteCount > 0 )
	{
	appDebug( "           NOTES         ,,%5dtw %d notes (Page %d:%d,First=%d)\n",
					footnoteHeight,
					nrBf->nrFootnoteCount,
					bf->bfPage, bf->bfColumn,
					nrBf->nrDfFirstFootnote->dfNoteIndex );
	}
#   endif

    return;
    }

int docSectNotesPrelayout(	int				sect,
				const struct BufferItem *	bodySectNode,
				LayoutJob *			lj )
    {
    const LayoutContext *	lc= lj->ljContext;
    struct BufferDocument *	bd= lc->lcDocument;

    DocumentField *		dfNote;
    DocumentNote *		dn;
    const int			treeType= -1;

    if  ( sect >= 0 )
	{
	dfNote= docGetFirstNoteOfSection( &dn, bd, sect, treeType );
	}
    else{
	dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
	}

    for ( ;
	  dfNote;
	  dfNote= docGetNextNoteInSection( &dn, bd, sect, dfNote, treeType ) )
	{
	if  ( docTreePrelayout( &(dn->dnDocumentTree), bodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }
