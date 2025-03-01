/************************************************************************/
/*									*/
/*  The various kinds of trees in a document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TREE_TYPE_H
#   define	DOC_TREE_TYPE_H

typedef enum TreeType
    {
    DOCinUNKNOWN= 0,

    DOCinBODY,

		    /**
		     *  In the first page header of a section.
		     *  Only used if the title page switch is set.
		     */
    DOCinFIRST_HEADER,

		    /**
		     *  In the left (even) page header of a section.
		     *  Only used if the facing pages switch is set.
		     */
    DOCinLEFT_HEADER,

		    /**
		     *  In the right (odd) page header of a section.
		     *  As of Word 11, the right (odd) header is used for 
		     *  all pages if the facing pages switch is not set.
		     */
    DOCinRIGHT_HEADER,

		    /**
		     *  In the last page header of a section.
		     *  This is not a regular MS-Word RTF feature.
		     */
    DOCinLAST_HEADER,

		    /**
		     *  In the first page footer of a section.
		     *  Only used if the title page switch is set.
		     */
    DOCinFIRST_FOOTER,

		    /**
		     *  In the left (even) page footer of a section.
		     *  Only used if the facing pages switch is set.
		     */
    DOCinLEFT_FOOTER,

		    /**
		     *  In the right (odd) page footer of a section.
		     *  As of Word 11, the right (odd) header is used for 
		     *  all pages if the facing pages switch is not set.
		     */
    DOCinRIGHT_FOOTER,

		    /**
		     *  In the last page footer of a section.
		     *  This is not a regular MS-Word RTF feature.
		     */
    DOCinLAST_FOOTER,

    DOCinFOOTNOTE,
    DOCinENDNOTE,
    DOCinANNOTATION,

    DOCinFTNSEP,
    DOCinFTNSEPC,
    DOCinFTNCN,

    DOCinAFTNSEP,
    DOCinAFTNSEPC,
    DOCinAFTNCN,

    DOCinSHPTXT,

		    /**
		     *  Used for fields that are used in the 
		     *  instructions of another field.
		     */
    DOCinFIELD_INSTRUCTIONS,

    DOCin_COUNT
    } TreeType;

extern const int DOC_HeaderFooterTypes[];
extern const int DOC_HeaderFooterTypeCount;

# define docIsHeaderType( t )	( (t) == DOCinFIRST_HEADER || \
				  (t) == DOCinLEFT_HEADER || \
				  (t) == DOCinRIGHT_HEADER || \
				  (t) == DOCinLAST_HEADER )

# define docIsFooterType( t )	( (t) == DOCinFIRST_FOOTER || \
				  (t) == DOCinLEFT_FOOTER || \
				  (t) == DOCinRIGHT_FOOTER || \
				  (t) == DOCinLAST_FOOTER )

# define docIsSeparatorType( t )( (t) == DOCinFTNSEP || \
				  (t) == DOCinFTNSEPC || \
				  (t) == DOCinFTNCN || \
				  (t) == DOCinAFTNSEP || \
				  (t) == DOCinAFTNSEPC || \
				  (t) == DOCinAFTNCN )

# define docIsNoteType( t )	( (t) == DOCinFOOTNOTE || \
				  (t) == DOCinENDNOTE || \
				  (t) == DOCinANNOTATION )

# define docTreeInColumnType( t ) \
		    ( (t) == DOCinBODY		|| \
		      (t) == DOCinFOOTNOTE	|| \
		      (t) == DOCinENDNOTE	|| \
		      docIsSeparatorType( t )	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char * docTreeTypeStr(	int			kind );

#   endif
