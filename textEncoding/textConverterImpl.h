/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8 using iconv.				*/
/*									*/
/************************************************************************/

#   ifndef	TEXT_CONVERTER_IMPL_H
#   define	TEXT_CONVERTER_IMPL_H

#   include	"textConverter.h"

struct TextConverterImpl; /*  read iconv_t */

typedef struct TextConverter
    {
				/**
				 *  The name of the native encoding that 
				 *  we convert from/to.
				 *  (Memory owned by the TextConverter)
				 */
    char *			tcNativeEncodingName;

				/**
				 *  The iconv_t instance that we use to 
				 *  convert to UTF-8
				 */
    struct TextConverterImpl *	tcIconvToUtf8;

				/**
				 *  The iconv_t instance that we use to 
				 *  convert from UTF-8
				 */
    struct TextConverterImpl *	tcIconvFrUtf8;

				/**
				 *  Emit the bytes that have been converted.
				 */
    TextConverterProduce	tcProduce;
    } TextConverter;

#   endif
