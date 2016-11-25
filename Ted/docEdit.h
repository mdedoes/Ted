/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_H
#   define		DOC_EDIT_H

#   include		"docBuf.h"

/************************************************************************/
/*									*/
/*  Describes an edit operation. Part is only used for interactive edit	*/
/*  operations.								*/
/*									*/
/************************************************************************/

typedef struct EditPosition
    {
    int		epParagraphNumber;
    int		epStroff;
    } EditPosition;

typedef struct EditRange
    {
    int			erParagraphCount;
    int			erBottomLevel;
    EditPosition	erStartPosition;
    EditPosition	erEndPosition;
    } EditRange;

typedef struct EditOperation
    {
    SelectionScope	eoSelectionScope;

    DocumentRectangle	eoChangedRectangle;
    int			eoOldBackY1;

    int			eoIBarSelectionOld;
    int			eoMultiParagraphSelectionOld;

    /**/
    int			eoParaAdjustParagraphNumber;
    int			eoParaAdjustFromLine;
    int			eoParaAdjustStroffShift;
    int			eoParaAdjustStroffUpto;

    /**/
    EditRange		eoReformatRange;

    /**/
    int			eoNotesDeleted;
    int			eoNotesAdded;
    int			eoParagraphsInserted;

    unsigned int	eoFieldUpdate;

    /**/
    BufferDocument *	eoBd;
    void *		eoVoidadd;
    DOC_CLOSE_OBJECT	eoCloseObject;
    } EditOperation;

/************************************************************************/
/*									*/
/*  For copying information from one location to another. Possibly	*/
/*  between different documents.					*/
/*									*/
/************************************************************************/

typedef struct DocumentCopyJob
    {
    BufferDocument *	dcjBdTo;
    BufferDocument *	dcjBdFrom;
    int *		dcjFieldMap;
    int *		dcjFontMap;
    int *		dcjColorMap;
    char *		dcjRefFileName;
    int			dcjRefFileSize;

    int			dcjCurrentTextAttributeNumberFrom;
    int			dcjCurrentTextAttributeNumberTo;

    int			dcjNotesCopied;
    } DocumentCopyJob;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docParaReplaceText(	EditOperation *		eo,
				BufferItem *		bi,
				int			part,
				unsigned int		stroffBegin,
				int *			pPartShift,
				int *			pStroffShift,
				unsigned int		stroffEnd,
				const unsigned char *	addedText,
				unsigned int		addedLength,
				int			addedAttributeNumber );

extern int docSplitParaItem(	BufferDocument *	bd,
				BufferItem **		pNewBi,
				BufferItem *		oldBi,
				int			stroff );

extern int docRemoveSelectionTail(
				EditOperation *			eo,
				const DocumentSelection *	ds );

extern void docEditDeleteItems(	EditOperation *		eo,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				BufferItem *		bi,
				int			first,
				int			count );

extern void docCloseItemObjects(	int *			pNoteCount,
					int *			pParagraphCount,
					BufferDocument *	bd,
					BufferItem *		bi,
					void *			voidadd,
					DOC_CLOSE_OBJECT	closeObject );

extern TextParticule * docParaSpecialParticule(
					BufferDocument *	bd,
					BufferItem *		bi,
					int			kind,
					int			part,
					int			stroff,
					int *			pPartShift,
					int *			pStroffShift );

extern int docReplaceSelection(
			EditOperation *			eo,
			const DocumentSelection *	bs,
			int *				pPartShift,
			int *				pStroffShift,
			const unsigned char *		addedText,
			int				addedLength,
			int				addedAttributeNumber );

extern void docInitEditOperation(	EditOperation *	eo );
extern void docInitEditPosition(	EditPosition *	ep );
extern void docInitEditRange(		EditRange *	er );

extern void docIncludePositionInReformat(
					EditOperation *		eo,
					const BufferItem *	paraBi,
					int			stroff );

extern void docEditIncludeItemInReformatRange(	EditOperation *		eo,
						BufferItem *		bi );

extern void docEditIncludeRowsInReformatRange(
					EditOperation *		eo,
					BufferItem *		sectBi,
					int			row0,
					int			row1 );

extern void docEditShiftReformatRangeParaNr(
					EditOperation *		eo,
					int			from,
					int			by );

extern void docSetParagraphAdjust(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			fromLine,
					int			stroffShift,
					int			stroffUpto );

extern void docExtendParagraphAdjust(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			stroffShift );

extern int docEditMakeParagraphEmpty(	BufferItem *		paraBi,
					EditOperation *		eo );

extern int docChangeParticuleAttributes(PropertyMask *		pTaAllMask,
					BufferDocument *	bd,
					BufferItem *		bi,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask );

extern int docMapTextAttributeNumber(
				DocumentCopyJob *	dcj,
				int			attributeNumberFrom );

extern BufferItem * docCopyParaItem(	DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		biCellTo,
					int			n,
					BufferItem *		biParaFrom,
					int			inTable );

extern BufferItem * docCopyRowItem(	DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		biSectTo,
					int			n,
					BufferItem *		biRowFrom,
					int			inTable );

extern BufferItem * docCopySectItem(	DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		biParentTo,
					int			n,
					BufferItem *		biSectFrom,
					const SelectionScope *	ss );

extern int docCopyParticules(	DocumentCopyJob *	dcj,
				unsigned int *		pFieldUpd,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				int			partTo,
				int			partFrom,
				int			countFrom,
				int *			pParticulesInserted,
				int *			pCharactersCopied );

extern int docCopyParticuleData(	DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		biTo,
					const BufferItem *	biFrom,
					TextParticule *		tpTo,
					const TextParticule *	tpFrom );

extern void docInitDocumentCopyJob(	DocumentCopyJob *	dcj );
extern void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj );

extern int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
					BufferDocument *	bd );

extern int docSet2DocumentCopyJob(	DocumentCopyJob *	dcj,
					BufferDocument *	bdTo,
					BufferDocument *	bdFrom,
					const char *		refFileName );

extern int docCopyFieldRelative(	DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		biTo,
					const BufferItem *	biFrom,
					TextParticule *		tpTo,
					const TextParticule *	tpFrom );

extern int docCopyParticuleAndData(	TextParticule **	pTpTo,
					DocumentCopyJob *	dcj,
					unsigned int *		pFieldUpd,
					BufferItem *		paraBiTo,
					int			partTo,
					int			stroffTo,
					int			strlenTo,
					const BufferItem *	paraBiFrom,
					const TextParticule *	tpFrom );

extern int docInsertTableRows(	BufferItem *		sectBi,
				BufferItem **		pNewBi,
				const BufferItem *	refRowBi,
				const RowProperties *	rp,
				int			textAttributeNumber,
				int			n,
				int			paraNr,
				int			rows,
				EditOperation *		eo );

extern int docSplitColumnInRows(	BufferItem **	pNewParaBi,
					EditOperation *	eo,
					BufferItem *	sectBi,
					int		row0,
					int		row,
					int		row1,
					int		col,
					int		after );

extern void docChangeTableLayout(	EditOperation *		eo,
					BufferItem *		sectBi,
					int			row0,
					int			row1,
					int			col0,
					int			col1,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rpSet );

extern int docDeleteColumnsFromRows(	EditOperation *		eo,
					BufferItem *		sectBi,
					int			delRow0,
					int			delRow1,
					int			delCol0,
					int			delCol1 );

extern int docChangeCellSpans(	int *			pRowChanged1,
				BufferItem *		sectBi,
				int			row0,
				int			col0,
				int			rowspan,
				int			colspan );

#   endif	/*	DOC_EDIT_H	*/
