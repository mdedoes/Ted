/************************************************************************/
/*									*/
/*  Kinds of fields.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_KIND_H
#   define DOC_FIELD_KIND_H

typedef enum FieldKind
    {
    DOCfkUNKNOWN= 0,
    DOCfkHYPERLINK,
    DOCfkXE,
    DOCfkTC,
    DOCfkTCN,
    DOCfkBOOKMARK,
    DOCfkSEQ,
    DOCfkSECTION,
    DOCfkSECTIONPAGES,
    DOCfkPAGE,
    DOCfkPAGEREF,
    DOCfkNUMPAGES,
    DOCfkREF,

    DOCfkCREATEDATE,
    DOCfkSAVEDATE,
    DOCfkPRINTDATE,
    DOCfkDATE,
    DOCfkTIME,

	    /**
	     *  Insert the author (according to the document properties) of 
	     *  the document.
	     */
    DOCfkAUTHOR,
    DOCfkCOMMENTS,
    DOCfkDOCCOMM,

	    /**
	     *  Insert the keywords (from the document properties) for
	     *  the document.
	     */
    DOCfkKEYWORDS,

	    /**
	     *  Insert the subject (according to the document properties) of 
	     *  the document.
	     */
    DOCfkSUBJECT,

	    /**
	     *  Insert the title (according to the document properties) of 
	     *  the document.
	     */
    DOCfkTITLE,

	    /**
	     *  Insert the file name of the document.
	     */
    DOCfkFILENAME,

	    /**
	     *  Insert the value of a user property that belongs to the
	     *  document.
	     */
    DOCfkDOCPROPERTY,

	    /**
	     *  Insert the value of a value that is obtained from another
	     *  document or a database etc.
	     */
    DOCfkMERGEFIELD,
    DOCfkFORMTEXT,
    DOCfkFORMCHECKBOX,

	    /**
	     *  A symbol. The reference is a numeric (Windows or Unicode)
	     *  value. Potentially the field also mentions the font to
	     *  use.
	     */
    DOCfkSYMBOL,

	    /**
	     *  A note reference in the body of the document, or the 
	     *  note number at the head of the note.
	     */
    DOCfkCHFTN,

	    /**
	     *  An annotation reference in the document. Only partially 
	     *  implemented. (MdD May 2010)
	     */
    DOCfkCHATN,

	    /**
	     *  The bullet or number at the head of a numbered paragraph
	     *  that is calculated programmatically.
	     */
    DOCfkLISTTEXT,

	    /**
	     *  An include picture field: Include the picture given
	     *  in the field instructions.
	     */
    DOCfkINCLUDEPICTURE,

	    /**
	     *  The Apple Cocoa/NextStep version of an include picture field.
	     */
    DOCfkNEXTGRAPHIC,

	    /**
	     *  A table of contents
	     */
    DOCfkTOC,

	    /**
	     *  An include text field: Include text from another document
	     */
    DOCfkINCLUDETEXT,

	    /**
	     *  An = (Formula) field. Refer to the Microsoft documentation
	     *  for the obscure expression syntax.
	     */
    DOCfkFORMULA,

	    /**
	     *  An IF field: Choose between two values. Refer to the Microsoft 
	     *  documentation for the obscure expression syntax.
	     */
    DOCfkIF,

	    /**
	     *  A SHAPE field: MS-Word stores a shape and a placeholder
	     *  image for an inline shape. Ted uses SHAPE fields to recognise
	     *  this. Shape fields are immediately recognised and both the
	     *  SHAPE field and the placeholder image are discarded. To make
	     *  MS-Word happy, Ted saves the field and an image in the RTF 
	     *  file.
	     */
    DOCfkSHAPE,

    DOCfk_COUNT
    } FieldKind;

    /**
      *  Field type that potentially has a note.
      */
#   define docFieldHasNote(t) ( (t) == DOCfkCHFTN || (t) == DOCfkCHATN )

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

#   endif /*  DOC_FIELD_KIND_H  */
