/************************************************************************/
/*									*/
/*  RGB chooser: Shortcut to the general one.				*/
/*  (Simply to fit in the general approach)				*/
/*									*/
/************************************************************************/

#   ifndef	TED_RGB_CHOOSER_PAGE_H
#   define	TED_RGB_CHOOSER_PAGE_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedRgbChooserFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );

#   endif	/*  TED_RGB_CHOOSER_PAGE_H */
