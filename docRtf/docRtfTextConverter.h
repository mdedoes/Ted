/************************************************************************/
/*									*/
/*  Converting text encodings while reading/writing RTF.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TEXT_CONVERTER_H
#   define	DOC_RTF_TEXT_CONVERTER_H

#   include	<textConverterImpl.h>

typedef struct EncodedFont
    {
    int		ecFileFontNumber;
    int		ecBufFontNumber;
    int		ecCharset;
    int		ecCharsetIdx;
    int		ecCodepage;
    } EncodedFont;

typedef struct RtfTextConverter
    {
				/*
				 * Used for RTF conversions. I.E where 
				 * no font applies
				 */
    TextConverter		rtcRtfConverter;
				/*
				 * Used for Text conversions. I.E where 
				 * a font applies
				 */
    TextConverter		rtcTextConverter;
    } RtfTextConverter;

extern const char DOC_RTF_AnsiCharsetName[];	/*  Ansi= CP1252	*/

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfInitTextConverters(	RtfTextConverter *	rtc );
extern void docRtfCleanTextConverters(	RtfTextConverter *	rtc );

extern void docRtfInitEncodedFont(	EncodedFont *		ef );

#    endif	/*  DOC_RTF_TEXT_CONVERTER_H	*/
