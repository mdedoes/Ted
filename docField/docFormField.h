/************************************************************************/
/*									*/
/*  Form Field								*/
/*									*/
/************************************************************************/

#   ifndef DOC_FORM_FIELD_H
#   define DOC_FORM_FIELD_H

#   include	<utilMemoryBuffer.h>

typedef struct FormField
    {
    unsigned char		ffType;		/*  fftype		*/
    unsigned char		ffOwnHelp;	/*  ffownhelpN		*/
    unsigned char		ffOwnStatus;	/*  ffownstatN		*/
    unsigned char		ffProtected;	/*  ffprotN		*/
    unsigned char		ffSize;		/*  ffsizeN		*/
    unsigned char		ffTextType;	/*  fftypetxtN		*/
    unsigned char		ffRecalculate;	/*  ffrecalcN		*/
    unsigned char		ffHasListbox;	/*  ffhaslistboxN	*/

    short			ffMaxlen;	/*  ffmaxlen		*/
    short			ffCheckboxSize;	/*  ffhpsN		*/
    short			ffDefaultIndex;	/*  ffdefres		*/

    MemoryBuffer		ffName;		/*  ffname		*/
    MemoryBuffer		ffDefaultText;	/*  ffdeftext		*/
    MemoryBuffer		ffFormat;	/*  ffformat		*/
    MemoryBuffer		ffHelpText;	/*  ffhelptext		*/
    MemoryBuffer		ffStatusText;	/*  ffstattext		*/
    MemoryBuffer		ffEnterMacro;	/*  ffentrymcr		*/
    MemoryBuffer		ffExitMacro;	/*  ffexitmcr		*/
#   if 0
    char **			ffListValues;	/*  ffl			*/
    short			ffListValue;	/*  ffresN		*/
#   endif
    } FormField;

typedef enum FormFieldProperty
    {
    FFpropTYPE= 0,
    FFpropOWN_HELP,
    FFpropOWN_STAT,
    FFpropPROT,
    FFpropSIZE,
    FFpropTYPE_TXT,
    FFpropRECALC,
    FFpropHAS_LISTBOX,

    FFpropMAX_LEN,
    FFpropCHECKBOX_SIZE,
    FFpropDEF_INDEX,

    FFpropNAME,
    FFpropDEF_TEXT,
    FFpropFORMAT,
    FFpropHELP_TEXT,
    FFpropSTAT_TEXT,
    FFpropENTRY_MACRO,
    FFpropEXIT_MACRO,

    /*
    FFpropLIST_VALUES,
    FFpropLIST_RESULT,
    */

    FFprop_COUNT
    } FormFieldProperty;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitFormField(		FormField *		ff );
extern void docCleanFormField(		FormField *		ff );

extern int docSetFormFieldProperty(	FormField *		ff,
					int			prop,
					int			val );

extern int docSetFormFieldString(	FormField *		ff,
					int			prop,
					const char *		val,
					int			vallen );

#   endif /* DOC_FORM_FIELD_H */
