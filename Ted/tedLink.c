/************************************************************************/
/*									*/
/*  Ted: The 'Hyperlink' dialog.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"docFind.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Management of link related stuff.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Determine the area covered by a field or a bookmark.		*/
/*									*/
/************************************************************************/

static void tedLinkArea(	DocumentRectangle *	drChanged,
				EditDocument *		ed,
				BufferItem *		bi,
				int			startPart,
				int			endPart )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sgNew;

    const int			direction= 1;

    docInitDocumentSelection( &dsNew );

    docSetParaSelection( &dsNew, bi, direction,
				bi->biParaParticules[startPart].tpStroff,
				bi->biParaParticules[endPart].tpStroff );

    tedSelectionGeometry( &sgNew, &dsNew, bd, add );

    *drChanged= sgNew.sgRectangle;

    return;
    }

/************************************************************************/
/*									*/
/*  Convert the current selection to a field or a bookmark.		*/
/*									*/
/************************************************************************/

static int tedLayoutFieldItem(	EditOperation *		eo,
				EditDocument *		ed,
				DocumentSelection *	dsField,
				unsigned int		whenMask )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		bi;
    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    bi= docGetSelectionRoot( &ei, &bodySectBi, bd, dsField );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    if  ( whenMask )
	{
	RecalculateFields		rf;

	docInitRecalculateFields( &rf );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= tedCloseObject;
	rf.rfUpdateFlags= whenMask;
	rf.rfFieldsUpdated= 0;

	if  ( docRecalculateTextLevelFields( &rf, bi ) )
	    { XDEB(whenMask);	}
	}

    docEditIncludeItemInReformatRange( eo, bi );

    return 0;
    }

static int tedFinishSetField(	EditDocument *			ed,
				DocumentSelection *		dsField,
				EditOperation *			eo,
				unsigned int			whenMask )
    {
    if  ( tedLayoutFieldItem( eo, ed, dsField, whenMask ) )
	{ LDEB(1); return -1;	}

    tedEditFinishSelection( ed, eo, dsField );

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove the particules that delimit a link or bookmark and adjust	*/
/*  the selection to changed particule numbers.				*/
/*									*/
/*  0)  First clean the field.						*/
/*									*/
/*  1)  Determine the rectangle that is to be redrawn.			*/
/*  2)  Delete the particules from the paragraph.			*/
/*  3)  Adjust the selection.						*/
/*  4)  Redraw.								*/
/*									*/
/************************************************************************/

static void tedDeleteField(	EditDocument *		ed,
				BufferItem *		bi,
				int			startPart,
				int			endPart )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    DocumentSelection		ds;
    SelectionGeometry		sg;

    TextParticule *		tp;
    DocumentField *		df;

    EditOperation		eo;

    const int			direction= 1;
    const int			col0= -1;
    const int			col1= -1;

    tedStartEditOperation( &eo, &ds, &sg, ed, 0 );

    /*  0  */
    tp= bi->biParaParticules+ startPart;
    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
    docCleanField( df ); docInitField( df );

    /*  1  */
    tedLinkArea( &(eo.eoChangedRectangle), ed, bi, startPart, endPart );

    /*  2  */
    docDeleteParticules( bi, endPart, 1 );
    docDeleteParticules( bi, startPart, 1 );

    docEditIncludeItemInReformatRange( &eo, bi );

    /*  3  */
    docSetDocumentPosition( &(ds.dsBegin),
				ds.dsBegin.dpBi, ds.dsBegin.dpStroff, 1 );
    docSetDocumentPosition( &(ds.dsEnd),
				ds.dsEnd.dpBi, ds.dsEnd.dpStroff, 0 );

    docInitDocumentSelection( &dsNew );

    docSetRangeSelection( &dsNew, &(ds.dsBegin), &(ds.dsEnd),
						    direction, col0, col1 );

    /*  4  */
    tedEditFinishSelection( ed, &eo, &dsNew );

    return;
    }


/************************************************************************/
/*									*/
/*  Run the 'Link Dialog' from the 'Insert Link' menu option.		*/
/*									*/
/************************************************************************/

void tedDocInsertLink(	APP_WIDGET	option,
			void *		voided,
			void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    int				startPart;
    int				endPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    const char *		markName= (const char *)0;
    int				markSize= 0;

    tedGetSelection( &ds, &sg, td );

    docGetHyperlinkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize );

    tedRunLinkDialog( ea, ed, option, fileName, fileSize, markName, markSize );

    tedAdaptToolsToSelection( ed );

    return;
    }

int tedSetHyperlink(	EditDocument *		ed,
			const char *		file,
			const char *		mark,
			int			asRef,
			int			asPageref )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    int				startPart;
    int				endPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    int				newFileSize= strlen( file );
    const char *		markName= (const char *)0;
    int				markSize= 0;
    int				newMarkSize= 0;

    DocumentField *		dfHyper;

    char *			adaptedMark= (char *)0;

    EditOperation		eo;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, ed, 0 );

    if  ( mark )
	{
	adaptedMark= strdup( mark );
	if  ( ! adaptedMark )
	    { XDEB(adaptedMark); return -1;	}

	docAdaptBookmarkName( &newMarkSize, adaptedMark );
	}

    if  ( docGetHyperlinkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{
	DocumentProperties *	dp= &(bd->bdProperties);

	int			numHyper;

	DocumentSelection	dsRep;
	DocumentSelection	dsHyperlink;
	DocumentSelection	dsField;

	int			beginMoved= 0;
	int			endMoved= 0;

	unsigned int		whenMask= FIELDdoNOTHING;

	RGB8Color		blueColor;
	int			blueIndex;

	const int		avoidZero= 1;
	const int		maxColors= 255;

	bmInitRGB8Color( &blueColor );
	blueColor.rgb8Red= 0;
	blueColor.rgb8Green= 0;
	blueColor.rgb8Blue= 255;

	blueIndex= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
					avoidZero, maxColors, &blueColor );
	if  ( blueIndex < 0 )
	    { LDEB(blueIndex);					}
	else{ PROPmaskADD( &taSetMask, TApropTEXT_COLOR );	}

	taSet.taTextColorNumber= blueIndex;
	taSet.taTextIsUnderlined= 1;

	PROPmaskADD( &taSetMask, TApropTEXTUNDERLINED );

	dsRep= ds;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &dsRep );

	dsHyperlink= dsRep;

	if  ( asRef || asPageref )
	    {
	    const FieldKindInformation *	fki;

	    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)"?", 1 ) )
		{ LDEB(1); return -1;	}

	    dsHyperlink= dsRep;

	    if  ( asRef )
		{
		int		numRef;
		DocumentField *	dfRef;

		dfRef= docClaimField( &numRef, &(bd->bdFieldList) );
		if  ( ! dfRef )
		    { XDEB(dfRef); return -1;	}

		if  ( docFieldSetRef( dfRef,
				(unsigned char *)adaptedMark, newMarkSize ) )
		    { LDEB(newMarkSize); return -1;	}

		if  ( docSurroundTextSelectionByField( bd, &dsField,
						    &dsRep, numRef,
						    &taSetMask, &taSet ) )
		    { LDEB(1); return -1;	}

		dsHyperlink.dsEnd.dpParticule += 2;

		dfRef->dfKind= DOCfkREF;
		fki= DOC_FieldKinds+ dfRef->dfKind;
		whenMask |= fki->fkiCalculateWhen;

		docUnionParaSelections( &dsHyperlink, &dsHyperlink, &dsField );
		dsRep= dsField;
		dsRep.dsEnd.dpParticule++;
		dsRep.dsBegin= dsRep.dsEnd;
		}

	    if  ( asRef && asPageref )
		{
		TextParticule *		tp;
		int			partShift= 0;
		int			stroffShift= 0;

		tp= docParaSpecialParticule( bd, dsRep.dsEnd.dpBi, DOCkindTAB,
				    dsRep.dsEnd.dpParticule,
				    dsRep.dsEnd.dpStroff,
				    &partShift, &stroffShift );
		if  ( ! tp )
		    { XDEB(tp); return -1;	}
		
		dsRep.dsEnd.dpParticule += partShift;
		dsRep.dsEnd.dpStroff += stroffShift;
		dsRep.dsBegin= dsRep.dsEnd;

		dsHyperlink.dsEnd.dpParticule += partShift;

		docUnionParaSelections( &dsHyperlink, &dsHyperlink, &dsRep );
		}

	    if  ( asPageref )
		{
		int		numPageref;
		DocumentField *	dfPageref;

		dfPageref= docClaimField( &numPageref, &(bd->bdFieldList) );
		if  ( ! dfPageref )
		    { XDEB(dfPageref); return -1;	}

		if  ( docFieldSetPageref( dfPageref,
				(unsigned char *)adaptedMark, newMarkSize ) )
		    { LDEB(newMarkSize); return -1;	}

		if  ( docSurroundTextSelectionByField( bd, &dsField,
						    &dsRep, numPageref,
						    &taSetMask, &taSet ) )
		    {
		    LLDEB(dsRep.dsBegin.dpParticule,dsRep.dsEnd.dpParticule);
		    docListItem(0,dsRep.dsEnd.dpBi);
		    return -1;
		    }

		dsHyperlink.dsEnd.dpParticule += 2;

		dfPageref->dfKind= DOCfkPAGEREF;
		fki= DOC_FieldKinds+ dfPageref->dfKind;
		whenMask |= fki->fkiCalculateWhen;

		docUnionParaSelections( &dsHyperlink, &dsHyperlink, &dsField );
		dsRep= dsField;
		dsRep.dsEnd.dpParticule++;
		dsRep.dsBegin= dsRep.dsEnd;
		}
	    }

	dfHyper= docClaimField( &numHyper, &(bd->bdFieldList) );
	if  ( ! dfHyper )
	    { XDEB(dfHyper); return -1;	}

	if  ( docFieldSetHyperlink( dfHyper, (unsigned char *)file, newFileSize,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SSDEB(file,adaptedMark); return -1;	}

	/*  4  */
	if  ( docSurroundTextSelectionByField( bd, &dsField,
						&dsHyperlink, numHyper,
						&taSetMask, &taSet ) )
	    { LDEB(1); return -1;	}

	dfHyper->dfKind= DOCfkHYPERLINK;

	/*  5  */
	if  ( tedFinishSetField( ed, &dsField, &eo, whenMask ) )
	    { LDEB(numHyper); return -1;	}

	if  ( adaptedMark )
	    { free( adaptedMark );	}

	return 0;
	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	dfHyper= bd->bdFieldList.dflFields;
	dfHyper += bi->biParaParticules[startPart].tpObjectNumber;

	if  ( docFieldSetHyperlink( dfHyper,
				(unsigned char *)file, newFileSize,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SSDEB(file,adaptedMark); return -1;	}

	appDocumentChanged( ed, 1 );

	if  ( adaptedMark )
	    { free( adaptedMark );	}

	return 0;
	}
    }

int tedRemoveHyperlink(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				startPart;
    int				endPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    tedGetSelection( &ds, &sg, td );

    if  ( docGetHyperlinkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{ LDEB(1); return -1;	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	tedDeleteField( ed, bi, startPart, endPart );

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

/************************************************************************/
/*									*/
/*  Run the 'Bookmark Dialog' from the 'Insert Bookmark' menu option.	*/
/*									*/
/************************************************************************/

void tedDocInsertBookmark(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				startPart;
    int				endPart;

    int				nameProposed= 0;
    char *			savedMark= (char *)0;

    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    tedGetSelection( &ds, &sg, td );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
						    &markName, &markSize ) )
	{
	BufferItem *	bi= ds.dsBegin.dpBi;
	int		stroff= ds.dsBegin.dpStroff;

	if  ( ! ed->edIsReadonly		&&
	      bi				&&
	      bi == ds.dsEnd.dpBi		)
	    {
	    unsigned char *	s= bi->biParaString+ stroff;
	    unsigned char *	e;
	    unsigned char *	p;

	    while( ! isalnum( *s )		&&
		   stroff < ds.dsEnd.dpStroff	)
		{ s++; stroff++;	}

	    p= e= s;
	    while( stroff < ds.dsEnd.dpStroff )
		{
		if  ( isalnum( *e ) )
		    { p= e+ 1;	}

		e++; stroff++;
		}

	    if  ( p- s > DOCmaxBOOKMARK )
		{ p= s+ DOCmaxBOOKMARK;	}

	    savedMark= (char *)malloc( DOCmaxBOOKMARK+ 1 );
	    if  ( savedMark )
		{
		DocumentField *	df;

		if  ( p- s > 3 )
		    { strncpy( savedMark, (char *)s, p- s )[p- s]= '\0'; }
		else{ strcpy( savedMark, "bkmk" ); p= s+ 4;		 }

		markName= savedMark;
		nameProposed= 1;

		docAdaptBookmarkName( &markSize, savedMark );

		if  ( docGetBookmarkField( &df, savedMark, markSize,
						&(bd->bdFieldList) ) >= 0 )
		    {
		    int		i;

		    if  ( markSize+ 8 > DOCmaxBOOKMARK )
			{ markSize= DOCmaxBOOKMARK- 8;	}

		    markSize += 8;

		    for ( i= 0; i < 100000000; i++ )
			{
			sprintf( savedMark+ markSize- 8, "%08d", i );

			if  ( docGetBookmarkField( &df,
						savedMark, markSize,
						&(bd->bdFieldList) ) < 0 )
			    { break;	}
			}
		    }
		}
	    }
	}

    tedRunBookmarkDialog( ea, ed, option, nameProposed, markName, markSize );

    if  ( savedMark )
	{ free( savedMark );	}

    tedAdaptToolsToSelection( ed );

    return;
    }

int tedSetBookmark(	EditDocument *		ed,
			const char *		mark )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				startPart;
    int				endPart;

    const char *		markName= (const char *)0;
    int				markSize= 0;
    int				newMarkSize;

    DocumentField *		df;

    char *			adaptedMark= (char *)0;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, ed, 0 );

    adaptedMark= strdup( mark );
    if  ( ! adaptedMark )
	{ XDEB(adaptedMark); return -1;	}

    docAdaptBookmarkName( &newMarkSize, adaptedMark );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
						    &markName, &markSize ) )
	{
	int			fieldNumber;

	DocumentSelection	dsRep;
	DocumentSelection	dsField;

	int			beginMoved= 0;
	int			endMoved= 0;

	dsRep= ds;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &dsRep );

	markSize= strlen( adaptedMark );

	df= docClaimField( &fieldNumber, &(bd->bdFieldList) );
	if  ( ! df )
	    { XDEB(df); return -1;	}

	if  ( docFieldSetBookmark( df,
			    (unsigned char *)adaptedMark, newMarkSize ) )
	    { SDEB(adaptedMark); return -1;	}

	/*  4  */
	if  ( docSurroundTextSelectionByField( bd, &dsField,
						&dsRep, fieldNumber,
						&taSetMask, &taSet ) )
	    { LDEB(1); return -1;	}

	df->dfKind= DOCfkBOOKMARK;

	/*  5  */
	if  ( tedFinishSetField( ed, &dsField, &eo, FIELDdoNOTHING ) )
	    { LDEB(fieldNumber); return -1;	}

	return 0;
	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	df= bd->bdFieldList.dflFields;
	df += bi->biParaParticules[startPart].tpObjectNumber;

	if  ( docFieldSetBookmark( df,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SDEB(adaptedMark); return -1;	}

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

int tedRemoveBookmark(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				startPart;
    int				endPart;

    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    tedGetSelection( &ds, &sg, td );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin), &startPart, &endPart,
						    &markName, &markSize ) )
	{ LDEB(1); return -1;	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	tedDeleteField( ed, bi, startPart, endPart );

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

int tedCopyBookmarkAsLink(	EditDocument *		ed,
				int			asRef,
				int			asPageref,
				const char *		markName )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				markSize= strlen( markName );

    DocumentSelection		ds;

    docInitDocumentSelection( &ds );

    if  ( docFindBookmarkInDocument( &ds, td->tdDocument, markName, markSize ) )
	{ SDEB(markName); return -1;	}

    if  ( tedCopyAsLink( ed, &ds, asRef, asPageref,
				    ed->edFilename, markName, markSize ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Select a particular bookmark in a document.				*/
/*									*/
/************************************************************************/

int tedGoToBookmark(	EditDocument *		ed,
			const char *		markName,
			int			markSize )
    {
    DocumentSelection		ds;

    TedDocument *		td;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				beginMoved= 0;
    int				endMoved= 0;

    td= (TedDocument *)ed->edPrivateData;

    docInitDocumentSelection( &ds );

    if  ( docFindBookmarkInDocument( &ds, td->tdDocument, markName, markSize ) )
	{ SDEB(markName); return -1;	}

    docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &ds );

    tedSetSelection( ed, &ds, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection of the document with a field.		*/
/*									*/
/*  2)  Replace the current selection with something arbitrary.		*/
/*  3)  Make the footnote number field.					*/
/*  4)  Finish by surrounding the replacement with the field.		*/
/*  5)  Final bookkeeping and redrawing.				*/
/*									*/
/************************************************************************/

static int tedDocReplaceSelectionWithField(
					DocumentSelection *	dsField,
					EditOperation *		eo,
					EditDocument *		ed,
					const char *		fieldInst,
					int			fieldKind,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsRep;

    int				fieldNumber;
    DocumentField *		df;

    /*  2  */
    if  ( tedEditReplaceSelection( eo, &dsRep, ed,
					    (const unsigned char *)"?", 1 ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    df= docClaimField( &fieldNumber, &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    if  ( docSetFieldInst( df,
			(unsigned char *)fieldInst, strlen( fieldInst ) ) )
	{ SDEB(fieldInst); return -1;	}

    /*  4  */
    if  ( docSurroundTextSelectionByField( bd, dsField, &dsRep, fieldNumber,
							taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    df->dfKind= fieldKind;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a page number in the document.				*/
/*									*/
/*  1)  Force the header or footer that will receive the page number	*/
/*	to be reformatted the next time that it is drawn.		*/
/*  2)  Replace the selection with a PAGE field. As the replecement is	*/
/*	a page number that is recalculated every time the		*/
/*	header/footer is drawn, no field calculation is required.	*/
/*									*/
/************************************************************************/

void tedDocInsertPageNumber(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    const char *		fieldInst= "PAGE";

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    DocumentSelection		dsField;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, ed, 0 );

    /*  1  */
    if  ( docGetHeaderFooter( &ei, &bodySectBi, &ds, bd,
				    ds.dsSelectionScope.ssInExternalItem ) )
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    ei->eiPageFormattedFor= -1;

    /*  2  */
    if  ( tedDocReplaceSelectionWithField( &dsField, &eo, ed, 
						    fieldInst, DOCfkPAGE,
						    &taSetMask, &taSet ) )
	{ SDEB(fieldInst); return;	}

    /*  5  */
    if  ( tedFinishSetField( ed, &dsField, &eo, FIELDdoNOTHING ) )
	{ LDEB(1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a footnote in the document.					*/
/*									*/
/*  1)  Change the selection to a reference to a note.			*/
/*  2)	Insert a note immediately after the reference.			*/
/*  3)	Preliminary layout of the footnote.				*/
/*  4)	Layout of the item thet receives the footnote. (Usually redoes	*/
/*	the layout of the footnote)					*/
/*  5)	Select the end of the freshly inserted footnote.		*/
/*									*/
/************************************************************************/

static void tedDocInsertNote(	APP_WIDGET	option,
				void *		voided,
				int		noteInExtIt,
				int		sepInExtIt )
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsField;

    const char * const		fieldInst= "";
    const unsigned int		fieldUpdMask= FIELDdoCHFTN;

    DocumentNote *		dn;
    BufferItem *		bodySectBi;

    int				changed;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    DocumentPosition		dpEnd;

    RecalculateFields		rf;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    PROPmaskADD( &taSetMask, TApropSUPERSUB );
    taSet.taSuperSub= DOCfontSUPERSCRIPT;

    tedStartEditOperation( &eo, &ds, &sg, ed, 0 );

    if  ( ds.dsSelectionScope.ssInExternalItem != DOCinBODY )
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    bodySectBi= ds.dsBegin.dpBi;
    while( bodySectBi && bodySectBi->biLevel != DOClevSECT )
	{ bodySectBi= bodySectBi->biParent;	}
    if  ( ! bodySectBi )
	{ XDEB(bodySectBi); return;	}

    /*  1  */
    if  ( tedDocReplaceSelectionWithField( &dsField, &eo, ed,
						fieldInst, DOCfkCHFTN,
						&taSetMask, &taSet ) )
	{ SDEB(fieldInst); return;	}

    /*  2  */
    if  ( docMakeNote( &dn, bd, &(dsField.dsEnd), noteInExtIt ) )
	{ LDEB(1); return;	}

    if  ( docCheckNoteSeparatorItem( bd, sepInExtIt ) )
	{ LDEB(1); return;	}

    changed= 0;
    docRenumberNotes( &changed, bd );
    eo.eoNotesAdded++;

    if  ( changed )
	{
	docInitRecalculateFields( &rf );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= tedCloseObject;
	rf.rfUpdateFlags= fieldUpdMask;
	rf.rfFieldsUpdated= 0;

	if  ( docRecalculateTextLevelFields( &rf, bodySectBi ) )
	    { XDEB(fieldUpdMask);	}
	}

    /*  3  */
    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= fieldUpdMask;
    rf.rfFieldsUpdated= 0;

    if  ( docRecalculateTextLevelFields( &rf, dn->dnExternalItem.eiItem ) )
	{ XDEB(fieldUpdMask);	}

    /*  NO! Is just to setup the note.
    docEditIncludeItemInReformatRange( eo, dn->dnExternalItem.eiItem );
    */

    /*  Exclude from Edit operation: In another tree */
    if  ( tedLayoutItem( dn->dnExternalItem.eiItem, bd, add,
						&(eo.eoChangedRectangle) ) )
	{ LDEB(1); return;	}

    /*  4  */
    if  ( tedLayoutFieldItem( &eo, ed, &dsField, fieldUpdMask ) )
	{ LDEB(1); return;	}

    /*  5  */
    if  ( docLastPosition( &dpEnd, dn->dnExternalItem.eiItem ) )
	{ LDEB(1); return;	}

    /*  6  */
    tedEditFinishIBarSelection( ed, &eo, dpEnd.dpBi, dpEnd.dpStroff );

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return;
    }


void tedDocInsertFootnote(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    tedDocInsertNote( option, voided, DOCinFOOTNOTE, DOCinFTNSEP );
    }

void tedDocInsertEndnote(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    tedDocInsertNote( option, voided, DOCinENDNOTE, DOCinAFTNSEP );
    }

