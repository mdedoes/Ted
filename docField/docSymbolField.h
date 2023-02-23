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
    SYMBOLencANSI= 0,	/* \a: default */
    SYMBOLencUNICODE,	/* \u */
    SYMBOLencSHIFT_JIS,	/* \j */

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
			 *  \a, \u, \j: The font encoding to use to resolve the 
			 *  symbol value.
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
			 *  \t: (Ted extension) A text to describe the symbol.
			 *  This is similar to the alt attribute of an HTML image.
			 *  With PDF-UA, this is the /ActualText value that 
			 *  replaces the contents of the field.
			 */
    MemoryBuffer	sfActualText;

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
