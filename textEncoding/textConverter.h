/************************************************************************/
/*									*/
/*  Convert text from/to UTF-8 using iconv.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_CONVERTER_H
#   define	UTIL_TEXT_CONVERTER_H

struct TextConverter;

/************************************************************************/
/*									*/
/*  Convert bytes.. Return the size of the result.			*/
/*									*/
/************************************************************************/

typedef int (*TextConverterProduce)(	void *			through,
					int			produced,
					const char *		bytes,
					int			count );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void textConverterSetNativeEncodingName(
					struct TextConverter *	tc,
					const char *		encodingName );

extern void utilInitTextConverter(	struct TextConverter *	tc );
extern void utilCleanTextConverter(	struct TextConverter *	tc );

extern int textConverterConvertToUtf8(	struct TextConverter *	tc,
					void *			through,
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len );

extern int textConverterConvertFromUtf8( struct TextConverter *	tc,
					void *			through,
					TextConverterProduce	produce,
					int *			pConsumed,
					int			produced,
					const char *		text,
					int			len );

#   endif
