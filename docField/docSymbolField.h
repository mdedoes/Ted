/************************************************************************/
/*									*/
/*  Bookkeeping for SYMBOL fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_SYMBOL_FIELD_H
#   define DOC_SYMBOL_FIELD_H

struct DocumentField;

#   include	<utilMemoryBuffer.h>

typedef enum SymbolFieldEncoding
    {
	    /**
	     *  \a: The symbol (number) is a code point in the ANSI 
	     *  encoding. This is the default. (ANSI=CP1252)
	     */
    SYMBOLencANSI= 0,

	    /**
	     *  \u: The symbol (number) is a unicode character number.
	     */
    SYMBOLencUNICODE,

	    /**
	     *  \j: The symbol (number) is a code point in the Shift-Jis
	     *  encoding.
	     */
    SYMBOLencSHIFT_JIS,	/* \j */

	    /**
	     *  \l: (Ted extension) The symbol actually is a literal string value.
	     *  Unless \u0000 unicode escapes encode other values, The symbol 
	     *  actually is a string in ANSI=CP1252 encoding.
	     */
    SYMBOLencLITERAL,	/* \l */

    SYMBOLenc_COUNT
    } SymbolFieldEncoding;

typedef struct SymbolField
    {
			/**
			 *  The integer value of the codepoint of the symbol
			 *  in the font. The meaning of the value depends on 
			 *  the value of sfEncoding.
			 */
    int			sfSymbol;

			/**
			 *  \f: The name of the font to use for the symbol
			 */
    MemoryBuffer	sfFontName;

			/**
			 *  \s: The font size in points.
			 */
    int			sfSizePoints;

			/**
			 *  \a, \u, \j \l: The font encoding to use to resolve the 
			 *  symbol value. (\l) is a Ted extension.
			 */
    int			sfEncoding;

			/**
			 * \h: (boolean) If set, the symbol does not reserve 
			 * space in the line height. (So, with big symbols, 
			 * ascender and descender values for the line ignore 
			 * those of the symbol. So the symboll might intersect 
			 * other lines of text.)
			 */
    unsigned char	sfIsHigh;

			/**
			 * \d: (Ted extension) The output of the field is 
			 * not part of the regular document text. It is a 
			 * decoration. E.G. The result is tagged as an /Artifact
			 * when we emit tagged PDF.
			 * This name and the abbreviated flag value are chosen
			 * because \a is occupied by the encoding flag.
			 */
    unsigned char	sfIsDecoration;

			/**
			 *  \t: (Ted extension) A text to describe the symbol.
			 *  This is similar to the alt attribute of an HTML image.
			 *  With PDF-UA, this is the /ActualText value that 
			 *  replaces the contents of the field.
			 */
    MemoryBuffer	sfActualText;

			/**
			 *  The literal text of the field. (Ted extension).
			 *  This holds the literal text of the field that 
			 *  is used wilt LITERAL (\l) encoding.
			 */
    MemoryBuffer	sfLiteral;
    } SymbolField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitSymbolField(		SymbolField *	sf );
extern void docCleanSymbolField(	SymbolField *	sf );

extern int docFieldGetSymbol(	SymbolField *			sf,
				const struct DocumentField *	df );

#   endif /*  DOC_SYMBOL_FIELD_H  */
