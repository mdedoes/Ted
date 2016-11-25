/************************************************************************/
/*									*/
/*  Administration of EditOperations.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

void docInitEditPosition(	EditPosition *	ep )
    {
    ep->epParagraphNumber= -1;
    ep->epStroff= -1;
    }

void docInitEditRange(	EditRange *	er )
    {
    er->erParagraphCount= 0;
    er->erBottomLevel= DOClevOUT;

    docInitEditPosition( &(er->erStartPosition) );
    docInitEditPosition( &(er->erEndPosition) );
    }

void docInitEditOperation(	EditOperation *	eo )
    {
    docInitSelectionScope( &(eo->eoSelectionScope) );

    docInitRectangle( &(eo->eoChangedRectangle) );
    eo->eoOldBackY1= 0;

    eo->eoIBarSelectionOld= 0;
    eo->eoMultiParagraphSelectionOld= 0;

    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustFromLine= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    docInitEditRange( &(eo->eoReformatRange) );

    eo->eoNotesDeleted= 0;
    eo->eoNotesAdded= 0;
    eo->eoParagraphsInserted= 0;
    eo->eoFieldUpdate= FIELDdoNOTHING;

    eo->eoBd= (BufferDocument *)0;
    eo->eoVoidadd= (void *)0;
    eo->eoCloseObject= (DOC_CLOSE_OBJECT)0;

    return;
    }

void docInitDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    dcj->dcjBdTo= (BufferDocument *)0;
    dcj->dcjBdFrom= (BufferDocument *)0;
    dcj->dcjFieldMap= (int *)0;
    dcj->dcjFontMap= (int *)0;
    dcj->dcjColorMap= (int *)0;
    dcj->dcjRefFileName= (char *)0;
    dcj->dcjRefFileSize= 0;

    dcj->dcjCurrentTextAttributeNumberFrom= -1;
    dcj->dcjCurrentTextAttributeNumberTo= -1;

    dcj->dcjNotesCopied= 0;

    return;
    }

void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj )
    {
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    if  ( dcj->dcjFontMap )
	{ free( dcj->dcjFontMap );	}

    if  ( dcj->dcjColorMap )
	{ free( dcj->dcjColorMap );	}

    return;
    }

int docSet1DocumentCopyJob(	DocumentCopyJob *		dcj,
				BufferDocument *		bd )
    {
    int *	fieldMap;

    dcj->dcjBdTo= bd;
    dcj->dcjBdFrom= bd;

    fieldMap= docAllocateFieldMap( bd );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return -1;	}

    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}

    dcj->dcjFieldMap= fieldMap;

    return 0;
    }

int docSet2DocumentCopyJob(	DocumentCopyJob *		dcj,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const char *			refFileName )
    {
    int *	fmap;
    int *	cmap;

    dcj->dcjBdTo= bdTo;
    dcj->dcjBdFrom= bdFrom;

    fmap= docAllocateFieldMap( bdFrom );
    if  ( ! fmap )
	{ XDEB(fmap); return -1;	}
    if  ( dcj->dcjFieldMap )
	{ free( dcj->dcjFieldMap );	}
    dcj->dcjFieldMap= fmap;

    if  ( docMergeFontAndColorLists( &fmap, &cmap, bdTo, bdFrom ) )
	{ LDEB(1); return -1;	}

    if  ( dcj->dcjFontMap )
	{ free( dcj->dcjFontMap );	}
    dcj->dcjFontMap= fmap;

    if  ( dcj->dcjColorMap )
	{ free( dcj->dcjColorMap );	}
    dcj->dcjColorMap= cmap;

    if  ( refFileName )
	{
	int	l= strlen( refFileName );
	char *	fresh= malloc( l+ 1 );

	if  ( ! fresh )
	    { XDEB(fresh); return -1;	}

	strcpy( fresh, refFileName );

	if  ( dcj->dcjRefFileName )
	    { free( dcj->dcjRefFileName );	}

	dcj->dcjRefFileName= fresh;
	dcj->dcjRefFileSize= l;
	}
    else{
	if  ( dcj->dcjRefFileName )
	    { free( dcj->dcjRefFileName );	}

	dcj->dcjRefFileName= (char *)0;
	dcj->dcjRefFileSize= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a position in the range in the document that is to be	*/
/*  reformatted.							*/
/*									*/
/*  0)  Not used for adjusts: Remove the adjust.			*/
/*  1)  The document has been empty. in the course of the modification.	*/
/*	Now that a paragraph is included it is not empty anymore. Move	*/
/*	the start position to the beginning of the document.		*/
/*  2)  Shift beginning if necessary.					*/
/*  3)  Shift end if necessary.						*/
/*									*/
/************************************************************************/

void docIncludePositionInReformat(	EditOperation *		eo,
					const BufferItem *	paraBi,
					int			stroff )
    {
    int			paraNr;
    EditRange *		er= &(eo->eoReformatRange);

    if  ( paraBi->biLevel != DOClevPARA )
	{ LDEB(paraBi->biLevel); return;	}

    /*  0  */
    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustFromLine= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    paraNr= docNumberOfParagraph( paraBi );

    /*  1  */
    if  ( er->erStartPosition.epParagraphNumber == 0	)
	{
	er->erStartPosition.epParagraphNumber= 1;
	er->erStartPosition.epStroff= 0;
	}

    /*  2  */
    if  ( er->erStartPosition.epParagraphNumber < 0		||
	  paraNr < er->erStartPosition.epParagraphNumber	)
	{
	er->erStartPosition.epParagraphNumber= paraNr;
	er->erStartPosition.epStroff= stroff;
	}

    if  ( paraNr == er->erStartPosition.epParagraphNumber	&&
	  stroff < er->erStartPosition.epStroff			)
	{
	er->erStartPosition.epStroff= stroff;
	}

    /*  3  */
    if  ( er->erEndPosition.epParagraphNumber < 0		||
	  paraNr > er->erEndPosition.epParagraphNumber		)
	{
	er->erEndPosition.epParagraphNumber= paraNr;
	er->erEndPosition.epStroff= stroff;
	}

    if  ( paraNr == er->erEndPosition.epParagraphNumber	&&
	  stroff > er->erEndPosition.epStroff			)
	{
	er->erEndPosition.epStroff= stroff;
	}

    return;
    }

void docEditIncludeItemInReformatRange(	EditOperation *		eo,
					BufferItem *		bi )
    {
    EditRange *		er= &(eo->eoReformatRange);
    DocumentPosition	dp;

    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustFromLine= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    if  ( er->erBottomLevel == DOClevOUT	||
	  er->erBottomLevel > bi->biLevel	)
	{ er->erBottomLevel= bi->biLevel; }

    if  ( docFirstPosition( &dp, bi ) )
	{ LDEB(1);	}
    else{ docIncludePositionInReformat( eo, dp.dpBi, dp.dpStroff );	}

    if  ( docLastPosition( &dp, bi ) )
	{ LDEB(1);	}
    else{ docIncludePositionInReformat( eo, dp.dpBi, dp.dpStroff );	}

    return;
    }

void docEditIncludeRowsInReformatRange(	EditOperation *		eo,
					BufferItem *		sectBi,
					int			row0,
					int			row1 )
    {
    docEditIncludeItemInReformatRange( eo, sectBi->biChildren[row0] );
    docEditIncludeItemInReformatRange( eo, sectBi->biChildren[row1] );

    return;
    }

void docSetParagraphAdjust(	EditOperation *		eo,
				BufferItem *		paraBi,
				int			fromLine,
				int			stroffShift,
				int			stroffUpto )
    {
    EditRange *		er= &(eo->eoReformatRange);
    int			paraNr;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LDEB(paraBi->biLevel); return;	}

    if  ( er->erBottomLevel == DOClevOUT )
	{ er->erBottomLevel= paraBi->biLevel; }

    if  ( eo->eoParaAdjustParagraphNumber >= 0 )
	{ docEditIncludeItemInReformatRange( eo, paraBi ); return; }

    paraNr= docNumberOfParagraph( paraBi );

    if  ( er->erStartPosition.epParagraphNumber == paraNr	&&
	  er->erEndPosition.epParagraphNumber == paraNr		)
	{
	eo->eoParaAdjustParagraphNumber= paraNr;
	eo->eoParaAdjustFromLine= fromLine;
	eo->eoParaAdjustStroffShift= stroffShift;
	eo->eoParaAdjustStroffUpto= stroffUpto;
	}
    else{
	docEditIncludeItemInReformatRange( eo, paraBi );
	}

    return;
    }

void docExtendParagraphAdjust(	EditOperation *		eo,
				BufferItem *		paraBi,
				int			stroffShift )
    {
    int			paraNr;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LDEB(paraBi->biLevel); return;	}

    paraNr= docNumberOfParagraph( paraBi );

    if  ( eo->eoParaAdjustParagraphNumber != paraNr )
	{ LLDEB(eo->eoParaAdjustParagraphNumber,paraNr); return;	}

    eo->eoParaAdjustStroffShift += stroffShift;
    eo->eoParaAdjustStroffUpto += stroffShift;

    return;
    }

void docEditShiftReformatRangeParaNr(	EditOperation *		eo,
					int			from,
					int			by )
    {
    EditRange *		er= &(eo->eoReformatRange);

    if  ( from < by )
	{ LLDEB(from,by);	}

    if  ( eo->eoParaAdjustParagraphNumber >= 0	&&
	  eo->eoParaAdjustParagraphNumber >= from	)
	{ eo->eoParaAdjustParagraphNumber += by;	}

    if  ( er->erStartPosition.epParagraphNumber >= 0	&&
	  er->erStartPosition.epParagraphNumber >= from	)
	{ er->erStartPosition.epParagraphNumber += by;	}

    if  ( er->erEndPosition.epParagraphNumber >= 0	&&
	  er->erEndPosition.epParagraphNumber >= from	)
	{ er->erEndPosition.epParagraphNumber += by;	}

    er->erParagraphCount += by;

    if  ( er->erStartPosition.epParagraphNumber >= 0			&&
	  er->erStartPosition.epParagraphNumber > er->erParagraphCount	)
	{ er->erStartPosition.epParagraphNumber=  er->erParagraphCount; }

    if  ( er->erEndPosition.epParagraphNumber >= 0			&&
	  er->erEndPosition.epParagraphNumber > er->erParagraphCount	)
	{ er->erEndPosition.epParagraphNumber=  er->erParagraphCount; }

    if  ( er->erStartPosition.epParagraphNumber == 0	&&
	  er->erParagraphCount > 0			)
	{ er->erStartPosition.epParagraphNumber= 1;	}

    if  ( er->erEndPosition.epParagraphNumber == 0	&&
	  er->erParagraphCount > 0			)
	{ er->erEndPosition.epParagraphNumber= 1;	}

    return;
    }
