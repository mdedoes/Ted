/************************************************************************/
/*									*/
/*  Converting text encodings while reading/writing RTF.		*/
/*									*/
/************************************************************************/

#   ifndef	FONT_ENCODED_FONT_H
#   define	FONT_ENCODED_FONT_H

struct DocumentFontList;
struct DocumentFont;
struct PagedList;

typedef struct EncodedFont
    {
		    /**
		     *  The \\fN font number as it occurs in the 
		     *  rtf file.
		     */
    int		efFileFontNumber;

		    /**
		     *  The font number as it is used in the 
		     *  text attributes in the document. As 
		     *  several (encoded with different code pages) fonts in 
		     *  the file correspond to one font in the document, 
		     *  there are fewer fonts in the document than in the 
		     *  file.
		     */
    int		ecBufFontNumber;

		    /**
		     *  The MS-Windows character set that is used by this 
		     *  encoded font.
		     */
    int		ecCharset;
    int		ecCharsetIdx;

		    /**
		     *  The MS-Windows code page that is used by this 
		     *  encoded font.
		     */
    int		ecCodepage;
    } EncodedFont;

extern const char DOC_RTF_AnsiCharsetName[];	/*  Ansi= CP1252	*/

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void fontInitEncodedFont(	EncodedFont *		ef );

extern int fontBuildEncodedFontAdmin(
			struct DocumentFontList *	dfl,
			struct PagedList *		encodedFontList );

int fontGetDefaultFileFontNumber(
			struct PagedList *		encodedFontList,
			const struct DocumentFont *	df );

extern int fontGetFileFont(
			const char **			pSuffix,
			int *				pFileFontNumber,
			int *				pOfficeCharset,
			const struct DocumentFont *	df,
			struct PagedList *		encodedFontList,
			int				csIdx );

extern int fontGetSymbolCharset(
			int *				pCharset,
			struct PagedList *		encodedFontList,
			const struct DocumentFont *	df,
			int				symbol );

extern int fontAddEncodedFontToList(
			struct DocumentFontList *	dfl,
			struct PagedList *		encodedFontList,
			struct DocumentFont *		dfIn,
			EncodedFont *			efIn );

extern void fontDetermineEncoding(	struct DocumentFont *	df );

extern int fontMapFileFont(
			const struct PagedList *	encodedFontList,
			int *				pDocFontNum,
			int *				pCharset,
			int				fileFontNum );

#    endif	/*  FONT_ENCODED_FONT_H	*/
