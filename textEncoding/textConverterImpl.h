/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8 using iconv.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_CONVERTER_IMPL_H
#   define	UTIL_TEXT_CONVERTER_IMPL_H

#   include	<iconv.h>

typedef struct TextConverter
    {
    char *		tcNativeEncodingName;
    iconv_t		tcIconvToUtf8;
    iconv_t		tcIconvFrUtf8;
    } TextConverter;

#   endif
