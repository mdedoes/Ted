/************************************************************************/
/*									*/
/*  Declare field evaluation routines.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EVAL_FIELD_H
#   define	DOC_EVAL_FIELD_H

struct DocumentField;
struct RecalculateFields;
struct BufferDocument;
struct MemoryBuffer;
struct DocumentSelection;
struct ParagraphBuilder;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docCollectNumbers(	double **			pNumbers,
				const struct DocumentSelection * ds,
				int				direction,
				struct BufferDocument *		bd );

extern int docCalculateChftnFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSymbolFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculatePagerefFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSectionFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSeqFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSectionPagesFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculatePageFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateNumpagesFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateDocDateFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateDocStringFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateFilenameFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateDocPropertyFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateRefFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docRecalculateMergeField(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docRecalculateIncludePictureField(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docRecalculateParaStringTextParticules(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docRecalculateParaChftnTextParticules(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docRecalculateParaListtextTextParticules(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docRecalculateParaSymbolTextParticules(
			int *					pCalculated,
			int *					pStroffShift,
			struct ParagraphBuilder *		pb,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf,
			int					partHead,
			int					partCount );

extern int docCalculateRefFieldValue(
			struct MemoryBuffer *			mbResult,
			const struct MemoryBuffer *		markName,
			struct BufferDocument *			bd );

extern int docCalculateFormulaFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateIfFieldString(
			int *					pCalculated,
			struct MemoryBuffer *			mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

#   endif	/*  DOC_EVAL_FIELD_H  */
