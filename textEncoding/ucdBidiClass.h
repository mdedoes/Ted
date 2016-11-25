/************************************************************************/
/*									*/
/*  UCD Bidirectional classes.						*/
/*									*/
/************************************************************************/

#   ifndef		UCD_BIDI_CLASS_H
#   define		UCD_BIDI_CLASS_H

typedef enum UCDBidiClass
    {
	    /********************/
	    /*  STRONG		*/
	    /********************/

	    /**
	     * Left_To_Right any strong left-to-right character
	     * The value of 0 matches the lowest level that defaults to L.
	     */
    UCDbidi_L= 0,

	    /**
	     * Right_To_Left * any strong right-to-left (non-Arabic-type) 
	     * character
	     * The value of 1 matches the lowest level that defaults to R.
	     */
    UCDbidi_R= 1, 

	    /**
	     * Arabic_Letter any strong right-to-left (Arabic-type) character
	     */
    UCDbidi_AL, 

	    /********************/
	    /*  WEAK		*/
	    /********************/

	    /**
	     * European_Number any ASCII digit or Eastern Arabic-Indic digit
	     */
    UCDbidi_EN, 
	    /**
	     * European_Separator plus and minus signs
	     */
    UCDbidi_ES, 
	    /**
	     * European_Terminator
	     * a terminator in a numeric format context, includes currency signs
	     */
    UCDbidi_ET,
	    /**
	     * Arabic_Number any Arabic-Indic digit
	     */
    UCDbidi_AN, 
	    /**
	     * Common_Separator commas, colons, and slashes
	     */
    UCDbidi_CS, 
	    /**
	     * Nonspacing_Mark any nonspacing mark
	     */
    UCDbidi_NSM, 
	    /**
	     * Boundary_Neutral
	     * most format characters, control codes, or noncharacters
	     */
    UCDbidi_BN, 

	    /********************/
	    /*  NEUTRAL		*/
	    /********************/

	    /**
	     * Paragraph_Separator various newline characters
	     */
    UCDbidi_B, 
	    /**
	     * Segment_Separator various segment-related control codes
	     */
    UCDbidi_S, 
	    /**
	     * White_Space spaces
	     */
    UCDbidi_WS, 
	    /**
	     * Other_Neutral most other symbols and punctuation marks
	     */
    UCDbidi_ON, 

	    /********************/
	    /*  EXPLICIT	*/
	    /********************/

	    /**
	     * Left_To_Right_Embedding U+202A: the LR embedding control
	     */
    UCDbidi_LRE, 
	    /**
	     * Left_To_Right_Override U+202D: the LR override control
	     */
    UCDbidi_LRO, 
	    /**
	     * Right_To_Left_Embedding U+202B: the RL embedding control
	     */
    UCDbidi_RLE, 
	    /**
	     * Right_To_Left_Override U+202E: the RL override control
	     */
    UCDbidi_RLO, 
	    /**
	     * Pop_Directional_Format
	     * U+202C: terminates an embedding or override control
	     */
    UCDbidi_PDF, 

	    /**
	     * Left_To_Right_Isolate U+2066: The LR Isolate contol
	     */
    UCDbidi_LRI, 
	    /**
	     * Right_To_Left_Isolate U+2067: The RL Isolate contol
	     */
    UCDbidi_RLI, 
	    /**
	     * First_Strong_Isolate U+2068: The First Strong Isolate contol
	     */
    UCDbidi_FSI, 
	    /**
	     * Pop_Directional_Isolate U+2069: End the scope of the last 
	     * isolate.
	     */
    UCDbidi_PDI, 

    UCDbidi__COUNT
    } UCDBidiClass;

/************************************************************************/
/*									*/
/*  Classification following the Unicode Bidirectional algorithm.	*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

# define UNIbidi_IS_X9_REMOVED( c ) ( \
            (c) == UCDbidi_RLE ||  \
            (c) == UCDbidi_LRE ||  \
            (c) == UCDbidi_RLO ||  \
            (c) == UCDbidi_LRO ||  \
            (c) == UCDbidi_PDF ||  \
            (c) == UCDbidi_BN )

# define UNIbidi_IS_STRONG( c ) ( \
            (c) == UCDbidi_L   ||  \
            (c) == UCDbidi_LRE ||  \
            (c) == UCDbidi_LRO ||  \
            (c) == UCDbidi_R   ||  \
            (c) == UCDbidi_AL  ||  \
            (c) == UCDbidi_RLE ||  \
            (c) == UCDbidi_RLO )

# define UNIbidi_IS_WEAK( c ) ( \
            (c) == UCDbidi_PDF ||  \
            (c) == UCDbidi_EN  ||  \
            (c) == UCDbidi_ES  ||  \
            (c) == UCDbidi_ET  ||  \
            (c) == UCDbidi_AN  ||  \
            (c) == UCDbidi_CS  ||  \
            (c) == UCDbidi_NSM ||  \
            (c) == UCDbidi_BN  )

# define UNIbidi_IS_NEUTRAL( c ) ( \
            (c) == UCDbidi_B   ||  \
            (c) == UCDbidi_S   ||  \
            (c) == UCDbidi_WS  ||  \
            (c) == UCDbidi_ON  )

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern int ucdBidiClass( int sym );
extern const char * ucdBidiClassStr( int bidiClass );

#   endif	/*	UCD_BIDI_CLASS_H	*/
