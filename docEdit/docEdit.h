/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_H
#   define		DOC_EDIT_H

struct BufferDocument;
struct BufferItem;
struct CellProperties;
struct DocumentAttributeMap;
struct DocumentCopyJob;
struct DocumentField;
struct DocumentList;
struct DocumentPosition;
struct DocumentProperties;
struct DocumentSelection;
struct DocumentTree;
struct EditOperation;
struct FieldInstructions;
struct ListOverride;
struct ParagraphProperties;
struct PropertyMask;
struct RowProperties;
struct SectionProperties;
struct SelectionScope;
struct TextAttribute;
struct TableRectangle;
struct SelectionDescription;
struct ExpandedTextAttribute;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docRollNodeChildren(		struct EditOperation *	eo,
					struct BufferItem *	parentNode,
					int			from,
					int			to,
					int			by );

extern int docEditDeleteReplacedFields(
			struct EditOperation *			eo );

extern int docParaReplaceText(
			struct EditOperation *		eo,
			int				paraNr,
			struct DocumentPosition *	dpTail,
			const struct DocumentPosition *	dpHead,
			unsigned int			stroffTail,
			const char *			addedText,
			unsigned int			addedLength,
			int				addedAttributeNumber );

extern int docParaDeleteText(
			struct EditOperation *		eo,
			int				paraNr,
			struct DocumentPosition *	dpTail,
			const struct DocumentPosition *	dpHead,
			unsigned int			stroffTail );

extern int docParaInsertTail(
			struct DocumentCopyJob *	dcj,
			int				paraNrTarget,
			struct DocumentPosition *	dpTargetTail,
			const struct DocumentPosition *	dpTgtTo,
			const struct DocumentPosition *	dpSrcFrom,
			const struct DocumentPosition *	dpSrcUpto );

extern int docEditSplitParaNode( struct BufferItem **	pNewParaNode,
				struct EditOperation *	eo,
				int			splitLevel );

extern int docEditSplitParaParent(	struct EditOperation *	eo,
					struct BufferItem **	pBeforeNode,
					struct BufferItem **	pAfterNode,
					struct BufferItem *	paraNode,
					int			after,
					int			splitLevel );

extern int docRemoveSelectionTail(	struct EditOperation *	eo );

extern void docDeleteEmptyParents(	struct EditOperation *	eo,
					int *			pSectsDeleted,
					struct BufferItem *	node );

extern void docEditDeleteNodes(	struct EditOperation *	eo,
				int *			pSectionsDeleted,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				struct BufferItem *	node,
				int			first,
				int			count );

extern struct TextParticule * docEditParaSpecialParticule(
					struct EditOperation *	eo,
					int			kind );

extern int docReplaceSelection(
			struct EditOperation *	eo,
			const char *		addedText,
			int			addedLength,
			int			addedAttributeNumber );

extern int docDeleteSelection(
			struct EditOperation *		eo );

extern int docMergeParagraphsInSelection(	struct EditOperation *	eo );

extern int docEditAddRowToTable(	struct EditOperation *	eo,
					int			after );

extern int docSplitColumnInRows(	struct BufferItem **	pNewParaNode,
					struct EditOperation *	eo,
					struct BufferItem *	parentNode,
					int			row0,
					int			row,
					int			row1,
					int			col,
					int			after );

extern int docDeleteColumnsFromRows(	struct EditOperation *	eo,
					struct BufferItem *	sectNode,
					int			delRow0,
					int			delRow1,
					int			delCol0,
					int			delCol1 );

extern int docDocRollRowsInTable( struct EditOperation *	eo,
				const struct TableRectangle *	tr,
				struct BufferItem *		parentNode,
				int				row0,
				int				row1,
				int				rowsdown );

extern void docEditShiftReferences(
				struct EditOperation *		eo,
				const struct SelectionScope *	ssRoot,
				int				paraFrom,
				int				stroffFrom,
				int				sectShift,
				int				paraShift,
				int				stroffShift );

extern void docEditAvoidDeletedParagraphs(
				struct EditOperation *		eo,
				const struct SelectionScope *	ssRoot,
				int				paraFrom,
				int				paraUpto );

extern int docEditUpdParaProperties(
				struct EditOperation *		eo,
				struct PropertyMask *		pPpDonePask,
				struct BufferItem *		paraNode,
				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties *	ppNew,
				const struct DocumentAttributeMap *	dam );

extern int docEditTransferParaProperties(
		struct EditOperation *			eo,
		struct BufferItem *			paraNodeTo,
		const struct BufferItem *		paraNodeFrom,
		int					alsoTableNesting,
		const struct DocumentAttributeMap *     dam );

extern int docEditUpdSectProperties(
				struct EditOperation *		eo,
				struct PropertyMask *		pSpDonePask,
				struct BufferItem *		sectNode,
				const struct PropertyMask *	spSetMask,
				const struct SectionProperties * spNew );

extern int docEditShiftParticuleOffsets( struct EditOperation *	eo,
					struct BufferItem *	paraNode,
					int			paraNr,
					int			partFrom,
					int			stroffFrom,
					int			stroffShift );

extern int docEditCleanParticules(	struct EditOperation *	eo,
					struct BufferItem *	paraNode,
					int			partFrom,
					int			partUpto );

extern int docCheckNoBreakAsLast(	struct EditOperation *	eo,
					struct BufferItem *	paraNode );

extern void docEditAdaptRowPropertiesToFirstChild(
				struct EditOperation *		eo,
				const struct BufferItem *	paraNode );

extern int docEditFixParaBreakKind(
				struct EditOperation *		eo,
				struct DocumentTree *		dt,
				struct BufferDocument *		bd,
				int				paraNr );

extern int docIncludeSelection(
			struct DocumentCopyJob *		dcjInsert,
			const struct DocumentSelection *	dsFrom );

extern int docIncludeDocument(	struct DocumentCopyJob *	dcj );

extern int docEditIncludeDocument(	struct EditOperation *		eo,
					struct DocumentCopyJob *	dcj );

extern int docSectionParagraph( struct EditOperation *	eo,
			struct BufferItem **		pParaNode,
			struct BufferItem *		sectNode,
			int				sectShift,
			const struct ParagraphProperties *	pp,
			int				textAttributeNr );

extern int docEditDeleteFieldsForBlockDelete(
			struct EditOperation *			eo,
			const struct DocumentSelection *	ds );

extern int docEditSurroundTextSelectionByField(
			struct EditOperation *			eo,
			struct DocumentField **			pDf,
			struct DocumentSelection *		dsInside,
			struct DocumentSelection *		dsAround,
			int *					pHeadPart,
			int *					pTailPart,
			const struct PropertyMask *		taSetMask,
			const struct TextAttribute *		taSet,
			int					fieldKind,
			const struct FieldInstructions *	fi );

extern int docChangeSelectionProperties(
			struct EditOperation *		eo,
			const struct DocumentSelection *	ds,

			struct PropertyMask *		selTaChgMask,
			const struct PropertyMask *	taSetMask,
			const struct TextAttribute *	taSet,

			struct PropertyMask *		selPpChgMask,
			const struct PropertyMask *	ppSetMask,
			const struct ParagraphProperties *	ppSet,

			struct PropertyMask *		selCpChgMask,
			const struct PropertyMask *	cpSetMask,
			const struct CellProperties *	cpSet,

			struct PropertyMask *		selRpChgMask,
			const struct PropertyMask *	rpSetMask,
			const struct RowProperties *	rpSet,

			struct PropertyMask *		selSpChgMask,
			const struct PropertyMask *	spSetMask,
			const struct SectionProperties * spSet,

			struct PropertyMask *		docDpChgMask,
			const struct PropertyMask *	dpSetMask,
			const struct DocumentProperties *	dpSet );

extern int docEditChangeSelectionPropertiesImpl(
			int *					pRefreshRect,
			struct EditOperation *			eo,
			const struct DocumentSelection *	ds,
			const struct PropertyMask *		taSetMask,
			const struct TextAttribute *		taSet,
			const struct PropertyMask *		ppSetMask,
			const struct ParagraphProperties *	ppSet,
			const struct PropertyMask *		cpSetMask,
			const struct CellProperties *		cpSet,
			const struct PropertyMask *		rpSetMask,
			const struct RowProperties *		rpSet,
			const struct PropertyMask *		spSetMask,
			const struct SectionProperties *	spSet,
			const struct PropertyMask *		dpSetMask,
			const struct DocumentProperties *	dpSet );

extern int docInsertParagraph(	struct EditOperation *	eo,
				struct DocumentPosition *	dpNew,
				struct BufferItem *	paraNode,
				int			after,
				int			textAttributeNr );

extern int docInsertSection(	struct EditOperation *	eo,
				struct DocumentPosition *	dpBeforeSplit,
				struct DocumentPosition *	dpAfterSplit,
				struct BufferItem *	paraNode,
				int			split,
				int			after,
				int			textAttributeNr );

extern int docEditChangeList(	struct EditOperation *		eo,
				struct DocumentList *		dl,
				struct ListOverride *		lo,
				const struct DocumentList *	dlNew );

extern int docUpdateListLevelAttributes(
			struct DocumentList *			dlNew,
			const struct PropertyMask *		taSetMask,
			const struct ExpandedTextAttribute *	etaSet,
			const struct SelectionDescription *	sd,
			struct BufferDocument *			bd );

extern int docEditSetNewList(
			int *					pRefreshRect,
			struct EditOperation *			eo,
			struct DocumentSelection *		ds,
			const struct SelectionDescription *	sd,
			int					traceEdits );

extern int docChangeCurrentList(
			struct EditOperation *			eo,
			const struct SelectionDescription *	sd,
			const struct DocumentList *		dlNew,
			int					traceEdits );

extern int docCopySelectionProperties(
			struct DocumentCopyJob *		dcj,
			const struct DocumentSelection *	dsTo,
			const struct DocumentSelection *	dsFrom,
			const struct TextAttribute *		taSet,
			const struct PropertyMask *		taSetMask,
			const struct PropertyMask *		ppSetMask,
			const struct PropertyMask *		cpSetMask,
			const struct PropertyMask *		rpSetMask,
			const struct PropertyMask *		spSetMask,
			const struct PropertyMask *		dpSetMask );

extern int docEditShiftIndent(		struct EditOperation *	eo,
					int			step );

extern int docDeleteField(	struct DocumentSelection *	dsExInside,
				struct EditOperation *		eo,
				struct DocumentField *		df );

extern int docFillTableDocument( struct BufferDocument *	bdTable,
				struct BufferItem *	parentNode,
				int			textAttrNr,
				int			wide,
				int			rows,
				int			columns );

#   endif	/*	DOC_EDIT_H	*/
