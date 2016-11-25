/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated rather than typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_PROPERTY_H
#   define DOC_FIELD_PROPERTY_H

typedef enum FieldProperty
    {
    FPpropFIRST_COLUMN= 0,
    FPpropLAST_COLUMN,
    FPpropDIRTY,
    FPpropEDITED,
    FPpropLOCKED,
    FPpropPRIVATE,

    FPpropFLDRSLT,
    FPpropFLDINST,

#   define FPprop_COUNT FPprop_DATAFIELD

    FPprop_DATAFIELD,
    FPprop_FORMFIELD,

    FPprop_FULL_COUNT
    } FieldProperty;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

#   endif /*  DOC_FIELD_PROPERTY_H  */
