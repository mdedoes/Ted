/************************************************************************/
/*									*/
/*  Administration around the setup of a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_BUILDER_H
#   define	DOC_PARA_BUILDER_H

struct BufferItem;
struct BufferDocument;
struct ParagraphProperties;
struct DocumentTree;
struct TextAttribute;
struct ParagraphBuilder;
struct SelectionScope;
struct DocumentField;
struct DocumentSelection;
struct DocumentPosition;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct ParagraphBuilder * docOpenParagraphBuilder(
			struct BufferDocument *			bd,
			const struct SelectionScope *		ss,
			struct DocumentTree *			tree );

extern void docCloseParagraphBuilder(
			struct ParagraphBuilder *		pb );

extern void docParaBuilderSetSelectionScope(
			struct ParagraphBuilder *		pb,
			const struct SelectionScope *		ss );

extern struct BufferItem * docParaBuilderStartNewParagraph(
			struct ParagraphBuilder *		pb,
			struct BufferItem *			refNode,
			const struct ParagraphProperties *	pp,
			const struct TextAttribute *		ta,
			int					textRToL );

extern int docParaBuilderStartExistingParagraph(
			struct ParagraphBuilder *		pb,
			struct BufferItem *			paraNode,
			int					stroff );

extern int docParaBuilderSetParagraphProperties(
			struct ParagraphBuilder *		pb,
			const struct ParagraphProperties *	pp,
			int					breakOverride );

extern int docParaBuilderFinishParagraph(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta );

extern int docParagraphBuilderSetTextRToL(
			struct ParagraphBuilder *		pb,
			int					textRToL );

extern int docParagraphBuilderSetNativeEncodingName(
			struct ParagraphBuilder *		pb,
			const char *				encodingName );

extern int docParagraphBuilderAppendText(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta,
			const char *				text,
			int					len );

extern int docParagraphBuilderAppendSymbol(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta,
			int					symbol );

extern int docParaBuilderAppendSpecialParticule(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta,
			int					kind );

extern int docParaBuilderAppendTab(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta );

extern struct DocumentField * docParaBuilderAppendFieldHead(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta,
			int					fieldKind,
			const char *				instBytes,
			int					instSize );

extern int docParaBuilderAppendFieldTail(
			struct ParagraphBuilder *		pb,
			const struct TextAttribute *		ta,
			struct DocumentField *			df );

extern int docParagraphBuilderAppendObject(
			struct ParagraphBuilder *		pb,
			int					objectNumber,
			const struct TextAttribute *		ta );

extern int docParagraphBuilderHandleBidiClass(
			struct ParagraphBuilder *		pb,
			int					bidiClass );

extern int docParaBuilderAllocateTextParticule(
			struct ParagraphBuilder *		pb,
			int					upto );

extern int docParagraphBuilderDeleteEmptySpan(
			struct ParagraphBuilder *		pb );

extern int docParagraphBuilderShiftOffsets(
			struct ParagraphBuilder *		pb,
			int					partFrom,
			int					stroffFrom,
			int					stroffShift );

extern struct TextParticule * docParaGraphBuilderInsertSpanParticule(
			struct ParagraphBuilder *		pb,
			int					part,
			int					off,
			int					len,
			int					textAttrNr );

extern struct TextParticule * docParaGraphBuilderInsertObjectParticule(
			struct ParagraphBuilder *		pb,
			int					part,
			int					off,
			int					len,
			int					textAttrNr,
			int					objectNumber );

extern int docInsertParaHeadField(
				struct DocumentField **		pField,
				struct DocumentSelection *	dsInsideField,
				struct DocumentSelection *	dsAroundField,
				int *				pHeadPart,
				int *				pTailPart,
				struct ParagraphBuilder *	pb,
				int				fieldKind,
				int				textAttrNr );

extern struct DocumentField * docMakeTextLevelField(
				struct ParagraphBuilder *	pb,
				int				stroff0,
				int				stroff1,
				int				part0,
				int				part1,
				int				textAttrNr,
				int				fieldKind );

extern int docParaBuilderSubstitute(
			int *				pStroffShift,
			struct ParagraphBuilder *	pb,
			int				stroffReplaceHead,
			int				stroffReplaceTail,
			int				stroffRedivideHead,
			int				stroffRedivideTail,
			int				partHead,
			int				partUpto,
			const char *			addedText,
			unsigned int			addedLength,
			int				addedAttributeNr );

extern int docParaSubstituteFindHeadPositions(
			int *				pPartHead,
			int *				pStroffRedivideHead,
			int *				pSplitHead,
			int *				pMergeHead,
			const struct DocumentPosition *	dpHead,
			int				addedAttrNr );

extern int docParaSubstituteFindTailPositions(
			int *				pPartUpto,
			int *				pStroffRedivideTail,
			int *				pSplitTail,
			int *				pMergeTail,
			const struct DocumentPosition *	dpTail,
			int				addedAttrNr );

extern int docParaBuilderSplitTextParticule(
			struct TextParticule  **	pTpPart,
			struct TextParticule  **	pTpNext,
			struct ParagraphBuilder *	pb,
			int				part,
			int				stroff );

extern int docParaBuilderInsertFieldParticules(
				int *				pHeadStroff,
				int *				pTailStroff,
				struct ParagraphBuilder *	pb,
				int				part0,
				int				part1,
				int				textAttrNr,
				int				fieldNumber );

extern int docParaBuilderGetParagraphNumber(
				struct ParagraphBuilder *	pb );

extern int docParagraphBuilderInsertListtextField(
			struct DocumentField **		pField,
			struct DocumentSelection *	dsInsideField,
			struct DocumentSelection *	dsAroundField,
			int *				pPartBegin,
			int *				pPartEnd,
			struct ParagraphBuilder *	pb );

#   endif
