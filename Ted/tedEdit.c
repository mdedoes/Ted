/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docLayout.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Replace the selection in the document with new text.		*/
/*									*/
/*  b)  Replace the tail of the beginning paragraph with the new text.	*/
/*  c)  Merge the two paragraphs.					*/
/*  d)  Erase all paragraphs after the beginning of the selection	*/
/*	to, including the end.						*/
/*  e)  Update the paragraph buffer and the particule administration.	*/
/*									*/
/*  B)  The first particule of the line was split, also reformat the	*/
/*	previous line.							*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*  C)  - Recalculate the geometry from the start of the selection to	*/
/*	  the end of the paragraph.					*/
/*	- Redraw the affected part of the paragraph.			*/
/*  6)  Update the notion of current particule and current line.	*/
/*  7)  Redraw the I-Bar.						*/
/*									*/
/************************************************************************/

int tedEditReplaceSelection(	EditOperation *		eo,
				DocumentSelection *	dsReplacement,
				EditDocument *		ed,
				const unsigned char *	addedText,
				int			addedLength )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);

    int				stroffShift= 0;
    int				partShift= 0;

    BufferItem *		bi;

    int				line;
    int				stroff;

    TextLine *			tl;
    int				upto;

    DocumentSelection		dsRep;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    int				multiParagraph;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsRep );

    bi= ds.dsBegin.dpBi;
    stroff= ds.dsBegin.dpStroff;
    line= sg.sgBegin.pgLine;

    tl= bi->biParaLines+ line;
    upto= tl->tlStroff+ tl->tlStrlen;

    multiParagraph= ds.dsEnd.dpBi != ds.dsBegin.dpBi;

    /*  b,c,d,e  */
    if  ( docReplaceSelection( eo, &ds,
				    &partShift, &stroffShift,
				    addedText, addedLength,
				    td->tdCurrentTextAttributeNumber ) )
	{ LDEB(addedLength); return -1;	}

    if  ( addedLength > 0						&&
	  td->tdCurrentTextAttribute.taFontNumber >= 0			&&
	  td->tdCurrentTextAttribute.taFontNumber < dfl->dflFontCount	)
	{ dfl->dflFonts[td->tdCurrentTextAttribute.taFontNumber].dfUsed= 1; }

    /*  B  */
    tedAdjustRedrawBegin( ed, eo, &line );

    /*  C  */
    if  ( multiParagraph )
	{
	docEditIncludeItemInReformatRange( eo, bi );
	}
    else{
	docSetParagraphAdjust( eo, bi, line, stroffShift, upto+ addedLength );
	}

    docSetParaSelection( &dsRep, bi, addedLength > 0, stroff, addedLength );

    *dsReplacement= dsRep;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Merge the selection into one paragraph.				*/
/*									*/
/************************************************************************/

static int tedIsIndentationParticule(	const BufferItem *	bi,
					const TextParticule *	tp )
    {
    const unsigned char *	s= bi->biParaString+ tp->tpStroff;
    int				i;

    if  ( tp->tpKind == DOCkindTAB )
	{ return 1;	}

    if  ( tp->tpKind != DOCkindTEXT )
	{ return 0;	}

    for ( i= 0; i < tp->tpStrlen; i++ )
	{
	if  ( *(s++) != ' ' )
	    { return 0;	}
	}

    return 1;
    }

static void tedMergeParagraphsInSelectionLow(	EditDocument *	ed,
						int *		fieldMap )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		biTo;
    BufferItem *		biFrom;

    int				endMoved= 0;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( docSet1DocumentCopyJob( &dcj, bd ) )
	{ LDEB(1); goto ready;	}

    biTo= ds.dsBegin.dpBi;
    biFrom= docNextParagraph( biTo );
    if  ( ! biTo )
	{ XDEB(biTo); goto ready;	}

    endMoved= biTo->biParaStrlen;

    for (;;)
	{
	int	partFrom= 0;

	int	particulesInserted= 0;
	int	charactersCopied= 0;


	while( partFrom < biFrom->biParaParticuleCount- 1		&&
	       tedIsIndentationParticule( biFrom,
				biFrom->biParaParticules+ partFrom )	)
	    { partFrom++; }

	if  ( partFrom < biFrom->biParaParticuleCount )
	    {
	    int		toCount= biTo->biParaParticuleCount;

	    if  ( toCount > 0						&&
		  biTo->biParaParticules[toCount-1].tpKind ==
							DOCkindTEXT	&&
		  biTo->biParaString[biTo->biParaStrlen-1] != ' '	)
		{
		if  ( docInflateTextString( biTo, 1 ) )
		    { LDEB(biTo->biParaStrlen); goto ready; }

		biTo->biParaString[biTo->biParaStrlen++]= ' ';
		biTo->biParaString[biTo->biParaStrlen  ]= '\0';
		biTo->biParaParticules[toCount- 1].tpStrlen++;
		endMoved++;
		}

	    if  ( docCopyParticules( &dcj, &eo,
			biTo, biFrom,
			biTo->biParaParticuleCount, partFrom,
			biFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
		{ LDEB(biFrom->biParaParticuleCount); goto ready;	}
	    }

	if  ( biFrom == ds.dsEnd.dpBi )
	    {
	    endMoved -= biFrom->biParaParticules[partFrom].tpStroff;
	    break;
	    }

	endMoved += charactersCopied;

	biFrom= docNextParagraph( biFrom );
	if  ( ! biFrom )
	    { XDEB(biFrom); goto ready;	}
	}

    biTo= ds.dsBegin.dpBi;
    if  ( ! biTo )
	{ XDEB(biTo); goto ready;	}

    if  ( docRemoveSelectionTail( &eo, &ds ) )
	{ LDEB(1); goto ready;	}

    if  ( ed->edFormat == TEDdockindTEXT_SAVE_FOLDED )
	{ ed->edFormat= TEDdockindTEXT_SAVE_WIDE;	}

    docEditIncludeItemInReformatRange( &eo, biTo );

    eo.eoNotesAdded += dcj.dcjNotesCopied;
    eo.eoBulletsChanged += dcj.dcjBulletsCopied;

    {
    const int		direction= 1;
    const int		col0= -1;
    const int		col1= -1;

    DocumentSelection	dsNew;

    docInitDocumentSelection( &dsNew );

    docSetDocumentPosition( &(ds.dsEnd), biTo, ds.dsEnd.dpStroff+ endMoved );

    docSetRangeSelection( &dsNew, &(ds.dsBegin), &(ds.dsEnd),
					    direction, col0, col1 );

    if  ( tedEditFinishSelection( ed, &eo, &dsNew ) )
	{ LDEB(1);	}
    }

  ready:

    docCleanDocumentCopyJob( &dcj );

    return;
    }

void tedMergeParagraphsInSelection(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int *			fieldMap;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( docIsParaSelection( &ds ) )
	{ LDEB(1); return; }

    fieldMap= docAllocateFieldMap( bd );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return;	}

    tedMergeParagraphsInSelectionLow( ed, fieldMap );

    free( fieldMap );

    return;
    }

/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
/*									*/
/*  B)  The first particule of the line was split, probably, part fits	*/
/*	on the previous line. Reformat from the previous particule.	*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*									*/
/*  1)  Replace the selection of the target with the text of those	*/
/*	particules at the beginning of the source that have the same	*/
/*	attributes.							*/
/*  2)  Insert the rest of the first paragraph of the source into the	*/
/*	target.								*/
/*  4)  If the insertion consists of more than one paragraph, split the	*/
/*	target paragraph.						*/
/*  5)  Insert the last particule of the insertion as text.		*/
/*									*/
/*  z)  Copy all paragraphs between the first and last (exclusive) of	*/
/*	the source to the target.					*/
/*									*/
/************************************************************************/

static int tedIncludeParagraphs(	EditOperation *		eo,
					DocumentCopyJob *	dcj,
					int *			pEndedInTable,
					int			startWithTable,
					BufferItem *		biFrom,
					BufferItem *		biTo,
					DocumentPosition *	dpEndFrom )
    {
    BufferItem *		rowBi= (BufferItem *)0;
    int				inTable= 0;

    BufferDocument *		bdTo= dcj->dcjBdTo;

    if  ( ! startWithTable )
	{
	biFrom= docNextParagraph( biFrom );
	if  ( ! biFrom )
	    { XDEB(biFrom); return -1;	}

	if  ( biFrom == dpEndFrom->dpBi )
	    { return 0; }
	}

    for (;;)
	{
	BufferItem *	insBi;
	BufferItem *	aftBi;

	if  ( biFrom->biParaInTable )
	    {
	    DocumentPosition	dp;

	    BufferItem *	biRowTo= biTo->biParent->biParent;
	    BufferItem *	biRowFrom= biFrom->biParent->biParent;
	    int			parasCopied;

	    inTable= 1;

	    if  ( biTo->biNumberInParent !=
					biTo->biParent->biChildCount -1 )
		{
		if  ( docSplitGroupItem( bdTo, &insBi, &aftBi,
				biTo->biParent, biTo->biNumberInParent+ 1,
				DOClevCELL ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( biTo->biParent->biNumberInParent ==
					    biRowTo->biChildCount -1 )
		{
		docEditIncludeItemInReformatRange( eo, biTo->biParent );
		docEditIncludeItemInReformatRange( eo, insBi );
		}
	    else{
		if  ( docSplitGroupItem( bdTo, &insBi, &aftBi,
			    biRowTo, biTo->biParent->biNumberInParent+ 1,
			    DOClevROW ) )
		    { LDEB(1); return -1;	}

		docEditIncludeItemInReformatRange( eo, biRowTo );
		docEditIncludeItemInReformatRange( eo, insBi );

		biRowTo= insBi;
		}

	    rowBi= docCopyRowItem( dcj, eo, &parasCopied,
			biRowTo->biParent, biRowTo->biNumberInParent+ 1,
			biRowFrom, biFrom->biParaInTable );
	    if  ( ! rowBi )
		{ XDEB(rowBi); return -1;	}

	    if  ( rowBi->biInExternalItem == DOCinBODY )
		{
		if  ( docLastPosition( &dp, rowBi ) )
		    { LDEB(1);	}
		else{
		    const int	paraNr= docNumberOfParagraph( dp.dpBi );
		    const int	isSplit= 0;
		    const int	stroff= 0;
		    const int	sectShift= 0;
		    const int	paraShift= parasCopied;

		    docEditShiftReferences( bdTo, paraNr+ 1, isSplit, stroff,
					    sectShift, paraShift, -stroff );
		    }
		}

	    docEditIncludeItemInReformatRange( eo, rowBi );

	    if  ( docLastPosition( &dp, biRowFrom ) )
		{ LDEB(1); break;	}

	    if  ( biFrom == dpEndFrom->dpBi )
		{ break;	}

	    biFrom= docNextParagraph( dp.dpBi );
	    if  ( ! biFrom )
		{ break;	}

	    if  ( biFrom == dpEndFrom->dpBi && ! biFrom->biParaInTable )
		{ break;	}

	    if  ( docLastPosition( &dp, rowBi ) )
		{ LDEB(1); break;	}

	    biTo= dp.dpBi;
	    }
	else{
	    inTable= 0;

	    if  ( rowBi )
		{
		BufferItem *	childBi;

		insBi= docInsertItem( bdTo, rowBi->biParent,
				    rowBi->biNumberInParent+ 1, DOClevROW );
		if  ( ! insBi )
		    { XDEB(insBi); return -1;	}

		childBi= docInsertItem( bdTo, insBi, 0, DOClevCELL );
		if  ( ! childBi )
		    { XDEB(childBi); return -1;	}

		childBi= docCopyParaItem( dcj, eo,
						childBi, 0, biFrom, inTable );

		if  ( ! childBi )
		    { XDEB(childBi); return -1;	}

		biTo= childBi;
		}
	    else{
		insBi= docCopyParaItem( dcj, eo, biTo->biParent,
			    biTo->biNumberInParent+ 1, biFrom, inTable );

		if  ( biTo->biParaInTable )
		    { insBi->biParaInTable= biTo->biParaInTable;	}

		if  ( ! insBi )
		    { XDEB(insBi); return -1;	}

		biTo= insBi;
		}

	    docEditIncludeItemInReformatRange( eo, insBi );

	    if  ( insBi->biInExternalItem == DOCinBODY )
		{
		const int	paraNr= docNumberOfParagraph( biTo )+ 1;
		const int	isSplit= 0;
		const int	stroff= 0;
		const int	sectShift= 0;
		const int	paraShift= 1;

		docEditShiftReferences( bdTo, paraNr, isSplit, stroff,
					    sectShift, paraShift, -stroff );
		}

	    biFrom= docNextParagraph( biFrom );
	    if  ( ! biFrom )
		{ XDEB(biFrom); return -1;	}

	    if  ( biFrom == dpEndFrom->dpBi )
		{ break;	}

	    rowBi= (BufferItem *)0;
	    }
	}

    *pEndedInTable= inTable; return 0;
    }

/************************************************************************/
/*									*/
/*  Include the tail of a paragraph in another paragraph.		*/
/*									*/
/*  It is known by the caller that at the beginning if the series of	*/
/*  particules that is to be included, no particule merging is		*/
/*  possible/necessary.							*/
/*									*/
/*  1)  Inclusion at the end of the paragraph... No particule merging.	*/
/*  2)  If the last particule of the source, and the subsequent		*/
/*	particule of the target have the same attributes, let the	*/
/*	text replacement routine decide whether to merge them or not.	*/
/*  3)  Copy those particules that are to be copied 'as is.'		*/
/*  4)  Insert the last paricule as text if this is desirable.		*/
/*									*/
/************************************************************************/

static int tedIncludeTail(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				BufferItem *            biTo,
				const BufferItem *	biFrom,
				int			partTo,
				int			partFrom,
				int *			pPartShift,
				int *			pStroffShift )
    {
    int			copyLastAsText;
    int			particulesCopied;

    TextParticule *	tpTo;
    TextParticule *	tpFrom;

    int			particulesInserted= 0;
    int			charactersCopied= 0;

    /*  1  */
    if  ( biTo->biParaStrlen == 0		||
	  partTo == biTo->biParaParticuleCount	)
	{ copyLastAsText= 0;	}
    else{
	int		attributeNumberFrom;

	tpTo= biTo->biParaParticules+ partTo;
	tpFrom= biFrom->biParaParticules+ biFrom->biParaParticuleCount- 1;

	attributeNumberFrom= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );

	/*  2  */
	if  ( tpFrom->tpKind == DOCkindTEXT				&&
	      tpTo->tpKind == DOCkindTEXT				&&
	      attributeNumberFrom == tpTo->tpTextAttributeNumber	)
	    { copyLastAsText= 1;	}
	else{ copyLastAsText= 0;	}
	}

    /*  3  */
    particulesCopied= biFrom->biParaParticuleCount- partFrom- copyLastAsText;
    if  ( particulesCopied > 0 )
	{
	if  ( docCopyParticules( dcj, eo,
				    biTo, biFrom, partTo, partFrom,
				    particulesCopied,
				    &particulesInserted,
				    &charactersCopied ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( particulesCopied < 0 )
	    { LDEB(particulesCopied); particulesCopied= 0;	}
	}

    /*  4  */
    if  ( copyLastAsText > 0 )
	{
	int	textAttributeNumberFrom;

	tpTo= biTo->biParaParticules+ partTo+ particulesCopied;

	tpFrom= biFrom->biParaParticules+
				biFrom->biParaParticuleCount- copyLastAsText;

	textAttributeNumberFrom= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );
	if  ( textAttributeNumberFrom < 0 )
	    { LDEB(textAttributeNumberFrom); return -1;	}

	if  ( docParaReplaceText( eo, biTo, tpTo->tpStroff,
		    &particulesInserted, &charactersCopied,
		    tpTo->tpStroff, biFrom->biParaString+ tpFrom->tpStroff,
		    tpFrom->tpStrlen, textAttributeNumberFrom ) )
	    { LDEB(1); return -1;	}
	}

    *pPartShift += particulesInserted;
    *pStroffShift += charactersCopied;

    return 0;
    }

static int tedIncludeHead(
			EditOperation *			eo,
			DocumentCopyJob *		dcj,
			const DocumentSelection *	dsTo,
			const BufferItem *		biFrom,
			int *				pStroffShift,
			int *				pCharactersCopied )
    {
    TextParticule *		tpTo;
    TextParticule *		tpFrom;

    BufferItem *		biTo= dsTo->dsBegin.dpBi;
    unsigned int		stroffTo= dsTo->dsBegin.dpStroff;

    int				partShift= 0;
    int				charactersCopied= 0;
    int				particulesCopied;
    int				particuleSplit= 0;

    int				attributeNumberTo;

    tpTo= biTo->biParaParticules+ eo->eoParticule;
    tpFrom= biFrom->biParaParticules;

    attributeNumberTo= tpTo->tpTextAttributeNumber;

    if  ( biTo->biParaStrlen == 0 )
	{
	int		toInTable= biTo->biParaInTable;

	PropertyMask	ppChgMask;
	PropertyMask	ppUpdMask;

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	PROPmaskADD( &ppUpdMask, PPpropLEFT_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropFIRST_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropRIGHT_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropALIGNMENT );
	PROPmaskADD( &ppUpdMask, PPpropTAB_STOPS );

	if  ( docUpdParaProperties( &ppChgMask, &(biTo->biParaProperties),
				&ppUpdMask, &(biFrom->biParaProperties),
				dcj->dcjColorMap, dcj->dcjListStyleMap ) )
	    { LDEB(1); }

	biTo->biParaInTable= toInTable;

	tpTo->tpTextAttributeNumber= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );
	}

    /*  1  */
    particulesCopied= 0;
    while( particulesCopied < biFrom->biParaParticuleCount		&&
	   tpTo->tpKind == DOCkindTEXT					&&
	   tpFrom[particulesCopied].tpKind == DOCkindTEXT		)
	{
	int		attributeNumberFrom;

	attributeNumberFrom= docMapTextAttributeNumber( dcj,
			    tpFrom[particulesCopied].tpTextAttributeNumber );

	if  ( attributeNumberFrom != tpTo->tpTextAttributeNumber )
	    { break;	}

	particulesCopied++;
	}

    if  ( particulesCopied > 0 )
	{
	int		textAttributeNumberFrom;

	charactersCopied= tpFrom[particulesCopied- 1].tpStroff+
				    tpFrom[particulesCopied- 1].tpStrlen;
			    
	textAttributeNumberFrom= docMapTextAttributeNumber( dcj,
			tpFrom[particulesCopied- 1].tpTextAttributeNumber );

	if  ( textAttributeNumberFrom < 0 )
	    { LDEB(textAttributeNumberFrom); return -1;	}

	if  ( docReplaceSelection( eo, dsTo, &partShift, pStroffShift,
		    biFrom->biParaString+ tpFrom->tpStroff, charactersCopied,
		    textAttributeNumberFrom ) )
	    { LDEB(tpFrom->tpStrlen); return -1;	}
	}
    else{
	if  ( ! eo->eoIBarSelectionOld					&&
	      docReplaceSelection( eo, dsTo, &partShift, pStroffShift,
			(unsigned char *)0, 0, attributeNumberTo )	)
	    { LDEB(0); return -1;	}
	}

    stroffTo += charactersCopied;

    if  ( stroffTo >= biTo->biParaStrlen )
	{ eo->eoParticule= biTo->biParaParticuleCount;	}
    else{
	eo->eoParticule= 0; tpTo= biTo->biParaParticules;
	while( tpTo->tpStroff+ tpTo->tpStrlen < stroffTo )
	    { eo->eoParticule++; tpTo++;	}

	if  ( tpTo->tpStroff != stroffTo				&&
	      tpTo->tpStroff+ tpTo->tpStrlen != stroffTo		)
	    { particuleSplit= 1;	}

	if  ( particulesCopied < biFrom->biParaParticuleCount	&&
	      particuleSplit					)
	    {
	    TextParticule *	tpOld;

	    if  ( docSplitTextParticule( &tpOld, &tpTo,
					    biTo, eo->eoParticule, stroffTo ) )
		{ LDEB(eo->eoParticule); return -1;	}

	    partShift++; eo->eoParticule++;
	    }

	if  ( stroffTo == tpTo->tpStroff+ tpTo->tpStrlen )
	    { eo->eoParticule++; tpTo++;	}
	}

    if  ( particulesCopied < biFrom->biParaParticuleCount )
	{
	int	partFrom= particulesCopied;

	if  ( tedIncludeTail( eo, dcj, biTo, biFrom, eo->eoParticule, partFrom,
						&partShift, pStroffShift ) )
	    { LLDEB(eo->eoParticule,partFrom); return -1;	}

	charactersCopied= biFrom->biParaStrlen;
	}

    *pCharactersCopied= charactersCopied;

    return 0;
    }

static int tedIncludeDocument(	EditDocument *			edTo,
				EditOperation *			eo,
				DocumentCopyJob *		dcj,
				const DocumentSelection *	dsTo,
				const SelectionGeometry *	sgTo )
    {
    BufferDocument *		bdFrom= dcj->dcjBdFrom;

    int				stroffShift= 0;
    int				partShift= 0;
    int				endedInTable= 0;

    DocumentPosition		dpBeginFrom;
    DocumentPosition		dpEndFrom;

    BufferItem *		biFrom;

    BufferItem *		biTo;

    int				line;
    int				part;
    int				stroff;
    int				stroffEnd;

    int				charactersCopied= 0;

    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &dpEndFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); return -1;	}

    biFrom= dpBeginFrom.dpBi;
    biTo= dsTo->dsBegin.dpBi;

    stroff= dsTo->dsBegin.dpStroff;
    line= sgTo->sgBegin.pgLine;

    if  ( ! biFrom->biParaInTable )
	{
	tedAdjustRedrawBegin( edTo, eo, &line );

	if  ( tedIncludeHead( eo, dcj, dsTo, biFrom,
					&stroffShift, &charactersCopied ) )
	    { LDEB(1); return -1;	}

	}

    if  ( dpEndFrom.dpBi != biFrom )
	{
	BufferItem *	newBi;

	const int	particulesCopied= 0;
	TextParticule *	tpFrom;

	/*  4  */
	if  ( docSplitParaItem( eo, &newBi,
				biTo, stroff+ charactersCopied, DOClevPARA ) )
	    { LDEB(stroff+ charactersCopied); return -1; }

	/*  C  */
	docEditIncludeItemInReformatRange( eo, biTo );

	if  ( tedIncludeParagraphs( eo, dcj, &endedInTable,
					biFrom->biParaInTable,
					biFrom, biTo, &dpEndFrom ) )
	    { LDEB(1); return -1;	}

	part= 0; stroff= 0; partShift= 0; stroffShift= 0;

	biFrom= dpEndFrom.dpBi;
	tpFrom= biFrom->biParaParticules;

	if  ( ! endedInTable && biFrom->biParaStrlen > 0 )
	    {
	    int			partTo= part;
	    int			partFrom= particulesCopied;
	    int			wasEmpty= newBi->biParaStrlen == 0;

	    PropertyMask	ppChgMask;
	    PropertyMask	ppUpdMask;

	    if  ( biFrom->biParaListOverride > 0 )
		{
		int		bulletFieldNr= -1;
		int		bulletPartBegin= -1;
		int		bulletPartEnd= -1;
		int		bulletStroffBegin= -1;
		int		bulletStroffEnd= -1;

		if  ( docDelimitParaHeadField( &bulletFieldNr,
				&bulletPartBegin, &bulletPartEnd,
				&bulletStroffBegin, &bulletStroffEnd,
				biFrom, bdFrom ) )
		    { LDEB(1);	}

		if  ( partFrom <= bulletPartEnd )
		    { partFrom= bulletPartEnd+ 1;	}
		}

	    if  ( tedIncludeTail( eo, dcj, newBi, biFrom, partTo, partFrom,
						&partShift, &stroffShift ) )
		{ LLDEB(partTo,partFrom); return -1;	}

	    PROPmaskCLEAR( &ppChgMask );

	    PROPmaskCLEAR( &ppUpdMask );
	    PROPmaskFILL( &ppUpdMask, PPprop_COUNT );
	    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	    if  ( wasEmpty &&
		  docEditUpdParaProperties( eo, &ppChgMask, newBi,
				    &ppUpdMask, &(biFrom->biParaProperties),
				    dcj->dcjColorMap, dcj->dcjListStyleMap ) )
		{ LDEB(wasEmpty);	}
	    }

	docEditIncludeItemInReformatRange( eo, newBi );

	biTo= newBi;
	stroffEnd= 0+ stroffShift;
	}
    else{
	stroffEnd= dsTo->dsBegin.dpStroff+ charactersCopied;

	/*  C  */
	docSetParagraphAdjust( eo, biTo, line, stroffShift, stroffEnd );
	}

    eo->eoNotesAdded += dcj->dcjNotesCopied;
    eo->eoBulletsChanged += dcj->dcjBulletsCopied;

    tedEditFinishIBarSelection( edTo, eo, biTo, stroffEnd );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a plain text document into the current document.		*/
/*									*/
/*  This is the implementation of 'Include File', but also of a paste	*/
/*  E.G. from a program that does not support pasting RTF such as	*/
/*  Xterm.								*/
/*									*/
/*  1)  Determine the text attributes of the target location.		*/
/*  2)  Merge the font of the current position n the TARGER document	*/
/*	into the font list of the SOURCE document. This sounds to be	*/
/*	the wrong direction, but it makes subsequent steps a lot	*/
/*	easier.								*/
/*  3)  Determine the attribute number in the source document of the	*/
/*	text attribute at the target position. This is possible as we	*/
/*	have just inserted the font into the source document.		*/
/*  4)  Patch the text attributes of the whole body of the source	*/
/*	document to those of the target position. The preparations	*/
/*	above have made it possible to represent them in the source	*/
/*	document.							*/
/*  5)  Make a mapping of the fonts in the source document to those in	*/
/*	the target document. [There is only one as we gave the whole	*/
/*	body the same text attribute.]					*/
/*  6)  Allocate memory to keep an administration of the fields that	*/
/*	have been copied from one document to the other.		*/
/*  7)  Actually include the source in the target.			*/
/*									*/
/************************************************************************/

int tedIncludePlainDocument(	APP_WIDGET		w,
				EditDocument *		ed,
				BufferDocument *	bdFrom )
    {
    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    TedDocument *		tdTo= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bdTo= tdTo->tdDocument;

    DocumentPosition		dpBeginTo;
    DocumentPosition		dpBeginFrom;
    BufferItem *		paraBi;

    int				textAttributeNumber;
    TextAttribute		ta;

    int				rval;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    dpBeginTo= ds.dsBegin;
    if  ( ! dpBeginTo.dpBi				||
	  dpBeginTo.dpBi->biParaParticuleCount == 0	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  1  */
    textAttributeNumber= dpBeginTo.dpBi->biParaParticules[
				eo.eoParticule].tpTextAttributeNumber;

    utilGetTextAttributeByNumber( &ta, &(bdTo->bdTextAttributeList),
						    textAttributeNumber );

    /*  2  */
    {
    DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    DocumentFontList *		dflFrom= &(dpFrom->dpFontList);

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    DocumentFontList *		dflTo= &(dpTo->dpFontList);

    DocumentFont *		dfTo;

    int				to;

    dfTo= dflTo->dflFonts+ ta.taFontNumber;
    to= docMergeFontIntoFontlist( dflFrom, dfTo );
    if  ( to < 0 )
	{ LDEB(to); rval= -1; goto ready;	}
    ta.taFontNumber= to;

    if  ( ta.taTextColorNumber > 0 )
	{
	RGB8Color *	rgb8= dpTo->dpColors+ ta.taTextColorNumber;

	to= docAllocateDocumentColor( dpFrom, rgb8 );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}
	ta.taTextColorNumber= to;
	}
    }

    /*  3  */
    textAttributeNumber= utilTextAttributeNumber(
					&(bdFrom->bdTextAttributeList), &ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); rval= -1; goto ready;	}

    /*  4  */
    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    paraBi= dpBeginFrom.dpBi;

    for (;;)
	{
	TextParticule *		tp;
	int			part;

	tp= paraBi->biParaParticules;

	for ( part= 0; part < paraBi->biParaParticuleCount; part++, tp++ )
	    { tp->tpTextAttributeNumber= textAttributeNumber; }

	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { break;	}
	}

    /*  5,6  */
    if  ( docSet2DocumentCopyJob( &dcj, bdTo, bdFrom, psfl, ed->edFilename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  7  */
    rval= tedIncludeDocument( ed, &eo, &dcj, &ds, &sg );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

int tedIncludeRtfDocument(	APP_WIDGET		w,
				EditDocument *		ed,
				BufferDocument *	bdFrom )
    {
    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    int				rval= 0;

    TedDocument *		tdTo= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bdTo= tdTo->tdDocument;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( bdFrom->bdProperties.dpContainsTables )
	{
	if  ( ds.dsCol0 >= 0 )
	    { goto ready;	}
	if  ( ds.dsBegin.dpBi->biParaInTable )
	    { goto ready;	}
	if  ( ds.dsEnd.dpBi->biParaInTable )
	    { goto ready;	}
	}

    if  ( docSet2DocumentCopyJob( &dcj, bdTo, bdFrom, psfl, ed->edFilename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rval= tedIncludeDocument( ed, &eo, &dcj, &ds, &sg );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  1)  Split in the buffer administration.				*/
/*  2)  Recalculate the geometry from the start of the selection to the	*/
/*	end of the paragraph.						*/
/*  3)  Redraw the affected part of the paragraph.			*/
/*  4)  Cause the new paragraph to be reformatted in this edit		*/
/*	operation.							*/
/*  5)  If the new paragraph is part of a numbered/bulleted list,	*/
/*	insert a list text field at the head.				*/
/*									*/
/************************************************************************/

int tedSplitParaContents(	EditOperation *		eo,
				BufferItem **		pNewBi,
				BufferItem *		bi,
				int			stroff,
				EditDocument *		ed,
				int			splitLevel,
				int			onNewPage )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		newBi;

    AppDrawingData *		add= &(ed->edDrawingData);

    int				paraNr= -1;
    const int			paraShift= 1;

    paraNr= docNumberOfParagraph( bi );

    /*  1  */
    if  ( docSplitParaItem( eo, &newBi, bi, stroff, splitLevel ) )
	{ LDEB(stroff); return -1;	}

    newBi->biParaStartsOnNewPage= ! newBi->biParaInTable && onNewPage;
    
    docEditShiftReformatRangeParaNr( eo, paraNr+ 1, paraShift );

    /*  3  */
    eo->eoChangedRect.drX0= add->addBackRect.drX0;

    if  ( bi->biParaBottomBorder.bpStyle != DOCbsNONE )
	{ bi->biParaBottomBorder.bpStyle=   DOCbsNONE;	}
    if  ( bi->biParaSpaceAfterTwips > 0 )
	{ bi->biParaSpaceAfterTwips= 0;		}

    if  ( newBi->biParaTopBorder.bpStyle != DOCbsNONE )
	{ newBi->biParaTopBorder.bpStyle=   DOCbsNONE;	}
    if  ( newBi->biParaSpaceBeforeTwips > 0 )
	{ newBi->biParaSpaceBeforeTwips= 0;	}

    /*  5  */
    if  ( newBi->biParaListOverride > 0 )
	{
	if  ( docInsertListtextField( newBi, bd ) )
	    { LDEB(1); return -1;	}

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    *pNewBi= newBi; return 0;
    }

void tedSplitParagraph(		EditDocument *		ed,
				int			onNewPage )
    {
    BufferItem *		newBi;

    DocumentSelection		dsRep;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed, (unsigned char *)0, 0 ) )
	{ return;	}

    if  ( tedSplitParaContents( &eo, &newBi,
			    dsRep.dsBegin.dpBi, dsRep.dsBegin.dpStroff,
			    ed, DOClevPARA, onNewPage ) )
	{ LDEB(1); return;	}

    /*  7,8  */
    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an 'Object'.							*/
/*									*/
/************************************************************************/

int tedReplaceSelectionWithObject(	EditDocument *		ed,
					InsertedObject *	io )
    {
    TextParticule *		tp;

    int				stroffShift;
    int				partShift;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsRep;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)0, 0 ) )
	{ LDEB(1); return -1;	}

    tp= docParaSpecialParticule( eo.eoBd, eo.eoParaBi, DOCkindOBJECT,
		    eo.eoParticule, eo.eoStroff, &partShift, &stroffShift );
    if  ( ! tp )
	{ LXDEB(eo.eoParticule,tp); return -1;	}

    tp->tpObjectNumber= eo.eoParaBi->biParaObjectCount++;

    /*  3,4,5  */
    docExtendParagraphAdjust( &eo, eo.eoParaBi, stroffShift );

    /*  6,7  */
    eo.eoStroff += stroffShift;
    tedEditFinishIBarSelection( ed, &eo, eo.eoParaBi, eo.eoStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  An object has been resized: Redo layout.				*/
/*									*/
/************************************************************************/

int tedResizeObject(		EditDocument *			ed,
				int				part,
				const DocumentPosition *	dpObj,
				const PositionGeometry *	pgObj )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);

    BufferItem *		bi= dpObj->dpBi;
    TextParticule *		tp= bi->biParaParticules+ part;
    int				line= pgObj->pgLine;

    const int			stroffShift= 0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    tedAdjustRedrawBegin( ed, &eo, &line );

    if  ( tedReopenObject( bd, bi, tp, &(ed->edColors), add ) )
	{ LDEB(1); return -1;	}

    docSetParagraphAdjust( &eo, bi, line, stroffShift,
					dpObj->dpStroff+ 1+ stroffShift );

    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the selection with something new.				*/
/*									*/
/*  1)  No selection... Just refuse.					*/
/*  2)  Replace nothing with nothing: Do nothing.			*/
/*	(Saves a lot of work, and avoids some combinations of special	*/
/*	cases)								*/
/*									*/
/************************************************************************/

void tedDocReplaceSelection(	EditDocument *		ed,
				const unsigned char *	word,
				int			len )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DocumentSelection		dsRep;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    /*  1  */
    if  ( ! tedHasSelection( td ) )
	{ LDEB(1); return; }

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    /*  2  */
    if  ( eo.eoIBarSelectionOld && len == 0 )
	{ return;	}

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed, word, len ) )
	{ return;	}

    /*  6,7  */
    tedEditFinishIBarSelection( ed, &eo,
				    dsRep.dsEnd.dpBi,
				    dsRep.dsEnd.dpStroff );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete the current paragraph from a document.			*/
/*									*/
/************************************************************************/

int tedDeleteCurrentParagraph(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		paraBi;

    DocumentPosition		dpNew;
    BufferItem *		parentBi;
    int				childNumber;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    paraBi= ds.dsBegin.dpBi;

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, paraBi ) 	||
	  docNextPosition( &dpNew )		)
	{
	docInitDocumentPosition( &dpNew );
	if  ( docFirstPosition( &dpNew, paraBi )	||
	      docPrevPosition( &dpNew )			)
	    { docInitDocumentPosition( &dpNew ); }
	}

    parentBi= paraBi->biParent;
    childNumber= paraBi->biNumberInParent;
    while( parentBi->biChildCount < 2			&&
	   parentBi->biParent				)
	{
	if  ( parentBi->biParent->biLevel == DOClevROW		&&
	      parentBi->biParent->biRowHasTableParagraphs	)
	    { break;	}

	if  ( parentBi->biLevel == DOClevSECT )
	    { break;	}

	childNumber= parentBi->biNumberInParent;
	parentBi= parentBi->biParent;
	}

    if  ( parentBi->biChildCount < 2 )
	{
	if  ( docFirstPosition( &dpNew, paraBi ) )
	    { LDEB(1); return -1;	}

	tedEditIncludeItemInRedraw( &eo, ed, paraBi );

	if  ( docEditMakeParagraphEmpty( paraBi, &eo ) )
	    { LDEB(1); return -1;	}

	docEditIncludeItemInReformatRange( &eo, paraBi );

	dpNew.dpBi= paraBi; dpNew.dpStroff= 0;
	}
    else{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	tedEditIncludeItemInRedraw( &eo, ed, paraBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					parentBi, childNumber, 1 );

	if  ( parentBi->biInExternalItem == DOCinBODY )
	    {
	    const int		isSplit= 0;
	    const int		stroffFrom= 0;
	    const int		stroffShift= 0;

	    docEditShiftReferences( bd, firstParaDeleted+ paragraphsDeleted,
			    isSplit, stroffFrom,
			    -sectionsDeleted, -paragraphsDeleted, stroffShift );
	    }

	/*  NO ! called by docEditDeleteItems()
	docEditShiftReformatRangeParaNr( &eo, paraNr+ 1, -paraShift );
	*/

	docEditIncludeItemInReformatRange( &eo, parentBi );
	}

    tedEditFinishIBarSelection( ed, &eo, dpNew.dpBi, dpNew.dpStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make sure that a Section contains an empty paragraph.		*/
/*									*/
/************************************************************************/

static int tedSectionParagraph(
			BufferDocument *		bd,
			BufferItem **			pParaBi,
			BufferItem *			sectBi,
			const ParagraphProperties *	pp,
			int				textAttributeNumber )
    {
    BufferItem *	paraBi;

    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    paraBi= docInsertEmptyParagraph( bd, sectBi, textAttributeNumber );
    if  ( ! paraBi )
	{ XDEB(paraBi); return -1;	}

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskFILL( &ppUpdMask, PPprop_COUNT );
    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

    if  ( docUpdParaProperties( &ppChgMask, &(paraBi->biParaProperties),
				    &ppUpdMask, pp, colorMap, listStyleMap ) )
	{ LDEB(1);	}

    *pParaBi= paraBi; return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the current section.						*/
/*									*/
/************************************************************************/

int tedDeleteCurrentSection(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		paraBi;
    BufferItem *		sectBi;

    DocumentPosition		dpNew;
    BufferItem *		parentBi;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				newPositionIsPast= 1;

    int				paraNrOld= -1;
    int				paraNrNew= -1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    paraBi= ds.dsBegin.dpBi;

    sectBi= paraBi->biParent->biParent->biParent;
    parentBi= sectBi->biParent;

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, sectBi ) 	||
	  docNextPosition( &dpNew )		)
	{
	newPositionIsPast= 0;

	docInitDocumentPosition( &dpNew );
	if  ( docFirstPosition( &dpNew, sectBi )	||
	      docPrevPosition( &dpNew )			)
	    { docInitDocumentPosition( &dpNew ); }
	}

    if  ( parentBi->biInExternalItem == DOCinBODY	&&
	  newPositionIsPast				)
	{ paraNrOld= docNumberOfParagraph( dpNew.dpBi );	}

    if  ( parentBi->biChildCount < 2 )
	{
	ParagraphProperties	pp;
	TextParticule *		tp= paraBi->biParaParticules;
	int			textAttributeNumber= tp->tpTextAttributeNumber;

	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	int			paraNr;
	int			firstParaDeleted= -1;
	int			sectionsDeleted= 0;
	int			paragraphsDeleted= 0;

	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	docInitParagraphProperties( &pp );

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	PROPmaskFILL( &ppUpdMask, PPprop_COUNT );
	PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	if  ( docUpdParaProperties( &ppChgMask, &pp,
				&ppUpdMask, &(paraBi->biParaProperties),
				colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}

	tedEditIncludeItemInRedraw( &eo, ed, sectBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					sectBi, 0, sectBi->biChildCount );

	if  ( tedSectionParagraph( bd, &paraBi, sectBi,
						&pp, textAttributeNumber ) )
	    { LDEB(1); return -1;	}

	paraNr= docNumberOfParagraph( paraBi );
	docEditShiftReformatRangeParaNr( &eo, paraNr, 1 );

	docCleanParagraphProperties( &pp );

	if  ( newPositionIsPast )
	    {
	    paraNrNew= docNumberOfParagraph( dpNew.dpBi );

	    if  ( parentBi->biInExternalItem == DOCinBODY )
		{
		const int	isSplit= 0;
		const int	stroffFrom= 0;
		const int	stroffShift= 0;

		docEditShiftReferences( bd, paraNrOld+ 1, isSplit, stroffFrom,
				-sectionsDeleted,
				paraNrNew- paraNrOld, stroffShift );
		}

	    /*  NO! Done by docEditDeleteItems()
	    docEditShiftReformatRangeParaNr( &eo, paraNrOld+ 1,
						    paraNrNew- paraNrOld );
	    */
	    }

	dpNew.dpBi= paraBi; dpNew.dpStroff= 0;
	}
    else{
	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	tedEditIncludeItemInRedraw( &eo, ed, sectBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentBi, sectBi->biNumberInParent, 1 );

	if  ( newPositionIsPast )
	    {
	    paraNrNew= docNumberOfParagraph( dpNew.dpBi );

	    if  ( parentBi->biInExternalItem == DOCinBODY )
		{
		const int	isSplit= 0;
		const int	stroffFrom= 0;
		const int	stroffShift= 0;

		docEditShiftReferences( bd, paraNrOld, isSplit, stroffFrom,
			    -sectionsDeleted,
			    paraNrNew- paraNrOld, stroffShift );
		}

	    /*  NO! Done by docEditDeleteItems()
	    docEditShiftReformatRangeParaNr( &eo, paraNrOld+ 1,
						    paraNrNew- paraNrOld );
	    */
	    }
	}

    docEditIncludeItemInReformatRange( &eo, parentBi );

    tedEditFinishIBarSelection( ed, &eo, dpNew.dpBi, dpNew.dpStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a paragraph Before/After the selection, depending on the	*/
/*  value of the 'after' argument.					*/
/*									*/
/************************************************************************/

int tedInsertParagraph(	EditApplication *	ea,
			int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		paraBi;
    BufferItem *		newBi;

    int				textAttributeNumber;
    BufferItem *		parentBi;
    int				pos;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;

    int				paraNr;
    const int			paraShift= 1;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( after )
	{
	paraBi= ds.dsEnd.dpBi;
	if  ( ! paraBi )
	    { XDEB(paraBi); return -1;	}

	if  ( docLastPosition( &dpRef, paraBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= paraBi->biNumberInParent+ 1;
	textAttributeNumber= paraBi->biParaParticules[
			paraBi->biParaParticuleCount-1].tpTextAttributeNumber;
	}
    else{
	paraBi= ds.dsBegin.dpBi;
	if  ( ! paraBi )
	    { XDEB(paraBi); return -1;	}

	if  ( docFirstPosition( &dpRef, paraBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= paraBi->biNumberInParent;
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttributeNumber;
	}

    tedEditIncludeItemInRedraw( &eo, ed, paraBi );

    parentBi= paraBi->biParent;

    newBi= docInsertItem( bd, parentBi, pos, DOClevPARA );
    if  ( ! newBi )
	{ XDEB(newBi); return -1;	}

    paraNr= docNumberOfParagraph( newBi );

    if  ( parentBi->biInExternalItem == DOCinBODY )
	{
	const int	sectShift= 0;
	const int	isSplit= 0;
	const int	stroffFrom= 0;
	const int	stroffShift= 0;

	docEditShiftReferences( bd, paraNr+ 1, isSplit, stroffFrom,
					sectShift, paraShift, stroffShift );
	}

    docEditShiftReformatRangeParaNr( &eo, paraNr+ 1, paraShift );

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskFILL( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask, &(newBi->biParaProperties),
				    &ppUpdMask, &(paraBi->biParaProperties),
				    colorMap, listStyleMap ) )
	{ LDEB(1);	}

    if  ( ! docInsertTextParticule( newBi, 0, 0, 0,
					DOCkindTEXT, textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    docEditIncludeItemInReformatRange( &eo, paraBi );
    docEditIncludeItemInReformatRange( &eo, newBi );

    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    appDocumentChanged( ed, 1 );

    return 0;
    }

int tedInsertSection(	EditApplication *	ea,
			int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		paraBi;
    BufferItem *		sectBi;
    BufferItem *		newSectBi;
    BufferItem *		newParaBi;
    int				paraNr;

    int				textAttributeNumber;
    BufferItem *		parentBi;
    int				pos;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( after )
	{
	paraBi= ds.dsEnd.dpBi;
	if  ( ! paraBi || paraBi->biInExternalItem != DOCinBODY )
	    { XDEB(paraBi); return -1;	}

	sectBi= paraBi;
	while( sectBi->biLevel != DOClevSECT )
	    { sectBi= sectBi->biParent;	}
	if  ( ! sectBi )
	    { XDEB(sectBi); return -1;	}

	if  ( docLastPosition( &dpRef, sectBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= sectBi->biNumberInParent+ 1;
	textAttributeNumber= paraBi->biParaParticules[
			paraBi->biParaParticuleCount-1].tpTextAttributeNumber;

	paraNr= docNumberOfParagraph( dpRef.dpBi )+ 1;
	}
    else{
	paraBi= ds.dsBegin.dpBi;
	if  ( ! paraBi || paraBi->biInExternalItem != DOCinBODY )
	    { XDEB(paraBi); return -1;	}

	sectBi= paraBi;
	while( sectBi->biLevel != DOClevSECT )
	    { sectBi= sectBi->biParent;	}
	if  ( ! sectBi )
	    { XDEB(sectBi); return -1;	}

	if  ( docFirstPosition( &dpRef, sectBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= sectBi->biNumberInParent;
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttributeNumber;

	paraNr= docNumberOfParagraph( dpRef.dpBi );
	}

    tedEditIncludeItemInRedraw( &eo, ed, sectBi );

    parentBi= sectBi->biParent;

    newSectBi= docInsertItem( bd, parentBi, pos, DOClevSECT );
    if  ( ! newSectBi )
	{ XDEB(newSectBi); return -1;	}

    if  ( docCopySectionProperties( &(newSectBi->biSectProperties),
						&(sectBi->biSectProperties) ) )
	{ LDEB(1);	}

    if  ( tedSectionParagraph( bd, &newParaBi, newSectBi,
			&(paraBi->biParaProperties), textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    if  ( sectBi->biInExternalItem == DOCinBODY )
	{
	const int	isSplit= 0;
	const int	stroffFrom= 0;
	const int	sectShift= 1;
	const int	paraShift= 1;
	const int	stroffShift= 0;

	docEditShiftReferences( bd, paraNr, isSplit, stroffFrom,
					sectShift, paraShift, stroffShift );
	}

    docEditIncludeItemInReformatRange( &eo, sectBi );
    docEditIncludeItemInReformatRange( &eo, newSectBi );

    tedEditFinishIBarSelection( ed, &eo, newParaBi, 0 );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a (line/page) break particule in the document.		*/
/*									*/
/*  TABs could have been inserted with this routine, but against	*/
/*  flashing, a more subtle approach is used for TABs.			*/
/*									*/
/************************************************************************/

static void tedEditInsertSpecialParticule(	EditDocument *	ed,
						int		kind,
						int		redoLayout )
    {
    TextParticule *		tp;

    int				stroffShift;
    int				partShift;

    DocumentPosition *		dp;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsRep;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    dp= &(ds.dsBegin);

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)0, 0 ) )
	{ return;	}

    dp= &(dsRep.dsBegin);

    tp= docParaSpecialParticule( eo.eoBd, eo.eoParaBi, kind,
				    eo.eoParticule, eo.eoStroff,
				    &partShift, &stroffShift );
    if  ( ! tp )
	{ LXDEB(eo.eoParticule,tp); return;	}

    /*  3,4,5  */
    if  ( redoLayout )
	{ docEditIncludeItemInReformatRange( &eo, eo.eoParaBi );	}
    else{ docExtendParagraphAdjust( &eo, eo.eoParaBi, stroffShift );	}

    /*  6,7  */
    tedEditFinishIBarSelection( ed, &eo, eo.eoParaBi,
						eo.eoStroff+ stroffShift );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a line/page break in the document.				*/
/*									*/
/************************************************************************/

void tedDocInsertLineBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed, DOCkindLINEBREAK, redoLayout );

    return;
    }

void tedDocInsertPageBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed, DOCkindPAGEBREAK, redoLayout );

    return;
    }

void tedDocInsertChftnsep(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed, DOCkindCHFTNSEP, redoLayout );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a 'Tab'.							*/
/*									*/
/************************************************************************/

void tedEditReplaceSelectionWithTab( EditDocument *	ed )
    {
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed, DOCkindTAB, redoLayout );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a section break in the document.				*/
/*									*/
/************************************************************************/

void tedDocInsertSectBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		newBi;

    const int			onNewPage= 0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpSplit;
    const int			paraShift= 1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    dpSplit= ds.dsBegin;

    if  ( dpSplit.dpStroff == dpSplit.dpBi->biParaStrlen )
	{ docNextPosition( &dpSplit );	}

    if  ( dpSplit.dpStroff > 0 )
	{
	if  ( tedSplitParaContents( &eo, &newBi, 
				dpSplit.dpBi, dpSplit.dpStroff,
				ed, DOClevSECT, onNewPage ) )
	    { LDEB(1); return;	}
	}
    else{
	BufferItem *		insBi;
	BufferItem *		aftBi;

	DocumentPosition	dpNew;

	int			paraNr= -1;

	int			n= dpSplit.dpBi->biNumberInParent;

	paraNr= docNumberOfParagraph( dpSplit.dpBi );

	if  ( docSplitGroupItem( bd, &insBi, &aftBi, dpSplit.dpBi->biParent,
							    n, DOClevSECT ) )
	    { LDEB(1); return;	}

	if  ( docFirstPosition( &dpNew, insBi ) )
	    {
	    int		textAttributeNumber;

	    textAttributeNumber=
		    dpSplit.dpBi->biParaParticules[0].tpTextAttributeNumber;

	    if  ( tedSectionParagraph( bd, &newBi, insBi,
					    &(dpSplit.dpBi->biParaProperties),
					    textAttributeNumber ) )
		{ LDEB(1); return;	}

	    if  ( dpSplit.dpBi->biInExternalItem == DOCinBODY )
		{
		const int	isSplit= 0;
		const int	stroffFrom= 0;
		const int	sectShift= 1;
		const int	stroffShift= 0;

		docEditShiftReferences( bd, paraNr+ 1, isSplit, stroffFrom,
					sectShift, paraShift, stroffShift );
		}

	    docEditShiftReformatRangeParaNr( &eo, paraNr+ 1, paraShift );
	    }
	else{ newBi= dpNew.dpBi;	}

	if  ( aftBi )
	    {
	    if  ( ! docFirstPosition( &dpNew, aftBi ) )
		{ newBi= dpNew.dpBi;	}
	    }

	if  ( aftBi && aftBi->biParent )
	    { docEditIncludeItemInReformatRange( &eo, aftBi->biParent ); }
	else{ XDEB(aftBi);	}
	}

    /*  7,8  */
    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection in a document with a bitmap image.	*/
/*									*/
/************************************************************************/

int tedReplaceSelectionWithBitmapImage(	EditDocument *		ed,
					AppBitmapImage *	abi )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    InsertedObject *		io;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition *		dp;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}
    dp= &(ds.dsBegin);

    io= tedObjectMakeBitmapObject( ed, dp->dpBi, abi );
    if  ( ! io )
	{ XDEB(io); return -1; }

    if  ( tedReplaceSelectionWithObject( ed, io ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

