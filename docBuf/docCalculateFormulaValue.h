/************************************************************************/
/*									*/
/*  Calculations of the value of = (Formula) and IF fields.		*/
/*									*/
/************************************************************************/

#   ifndef		DOC_CALCULATE_FORMULA_VALUE_H
#   define		DOC_CALCULATE_FORMULA_VALUE_H

#   include	<utilMemoryBuffer.h>
#   include	"docSelect.h"

struct FormulaField;
struct DocumentField;
struct RecalculateFields;
struct BufferItem;

		/**
		 *  An empty value: Type not yet known.
		 */
#   define	FVtypeUNKNOWN	0

		/**
		 *  A string value from a constant or calculated.
		 *  (Stored in fvBuffer)
		 */
#   define	FVtypeSTRING	1

		/**
		 *  The name of a reference. (Bookmark or cell reference)
		 *  (Stored in fvBuffer)
		 */
#   define	FVtypeNAME	2

		/**
		 *  A number value from a constant or calculated.
		 *  (Stored in fvDouble)
		 */
#   define	FVtypeNUMBER	3

		/**
		 *  A selection in the document.
		 *  (Stored in fvSelection and fvDirection)
		 */
#   define	FVtypeSELECTION	4

typedef struct CalculateFormula
    {
		/**
		 *  The description of the formula field.
		 */
    const struct FormulaField *	cfFormulaField;

		/**
		 *  The formula that we evaluate.
		 */
    const char *	cfFormulaString;

		/**
		 *  The description of the field.
		 */
    const struct DocumentField *	cfField;

		/**
		 *  The head position of the field that we 
		 *  evaluate. (For relative references.)
		 */
    DocumentPosition		cfHeadPosition;

		/**
		 * The active recalculate job.
		 */
    const struct RecalculateFields *	cfRecalculateFields;

		/**
		 * The row (if any) that the formula was found in.
		 */
    struct BufferItem *		cfRowNode;
    } CalculateFormula;

typedef struct FormulaValue
    {
			/**
			 *  A numeric value. Used with FVtypeNUMBER values.
			 */
    double		fvDouble;

			/**
			 *  A string value. Used with FVtypeNAME and 
			 *  FVtypeSTRING values.
			 */
    MemoryBuffer	fvBuffer;

			/**
			 *  A selection in the document. Used with
			 *  FVtypeSELECTION values.
			 */
    DocumentSelection	fvSelection;

			/**
			 *  The direction of the selection in the document.
			 *  Used with FVtypeSELECTION values.
			 *  <0 for LEFT and ABOVE
			 *  >0 for RIGHT and BELOW
			 *  =0 otherwise.
			 *
			 */
    int			fvDirection;

			/**
			 *  The type of value: A FVtypeSOMETING enum value
			 */
    unsigned char	fvType;
    } FormulaValue;

typedef struct InfixOperator
    {
    const char *	ioSymbol;
    int			ioPriority;
    int			(*ioOperation)(	const struct InfixOperator *	io,
					const CalculateFormula *	cf,
					FormulaValue *			top,
					const FormulaValue *		rhs );
    } InfixOperator;

typedef struct PrefixOperator
    {
    const char *	poSymbol;
    int			(*poOperation)(	const struct PrefixOperator *	po,
					const CalculateFormula *	cf,
					FormulaValue *			top );
    } PrefixOperator;

typedef struct FunctionCalculator
    {
    const char *	fcName;
    unsigned char	fcTakesRectangle;
    int			fcArgc;

    int			(*fcCalculate)(
				const struct FunctionCalculator *	fc,
				const CalculateFormula *		cf,
				FormulaValue *				top,
				const FormulaValue *			argv,
				int					argc );
    } FunctionCalculator;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docFormulaInitValue(	FormulaValue *		fv );
extern void docFormulaCleanValue(	FormulaValue *		fv );

extern int docFormulaGetStringAsNumber(	FormulaValue *			to,
					const MemoryBuffer *		from );

extern int docFormulaGetNumber(		FormulaValue *			to,
					const FormulaValue *		from,
					const CalculateFormula *	cf );

extern int docFormulaGetString(		FormulaValue *			to,
					const FormulaValue *		from,
					const CalculateFormula *	cf );

extern const InfixOperator * docFormulaGetInfixOperator(
						int *		pLen,
						const char *	formula );

extern const PrefixOperator * docFormulaGetPrefixOperator(
						int *		pLen,
						const char *	formula );

extern const FunctionCalculator * docFormulaGetFunctionCalculator(
						const char *	name,
						int		length );

extern int docFormulaGetNameAsSelection(DocumentSelection *		ds,
					const FormulaValue *		from,
					const CalculateFormula *	cf );

#   endif	/*	DOC_CALCULATE_FORMULA_VALUE_H	*/
