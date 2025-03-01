/************************************************************************/
/*									*/
/*  Symbol Picker:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_SYMBOL_PICKER_H
#   define	TED_SYMBOL_PICKER_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedSymbolPickerFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_SYMBOL_PICKER_H */
