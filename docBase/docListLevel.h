/************************************************************************/
/*									*/
/*  Levels in the Lists of the list table of a document.		*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_LEVEL_H
#   define	DOC_LIST_LEVEL_H

#   include	<utilMemoryBuffer.h>
#   include	<textAttribute.h>
#   include	<utilPropMask.h>

struct ParagraphProperties;

typedef enum ListLevelAlignment
    {
    DOClla_NONE= -1,
    DOCllaLEFT= 0,
    DOCllaCENTER,
    DOCllaRIGHT,

    DOClla_COUNT
    } ListLevelAlignment;

typedef enum ListLevelFollow
    {
    DOCllfTAB= 0,
    DOCllfSPACE,
    DOCllfNONE,

    DOCllf_COUNT
    } ListLevelFollow;

/*===*/
# define	DOCpnDEC	0	/* Arabic (1, 2, 3) */
# define	DOCpnUCRM	1	/* Uppercase Roman numeral (I, II, III) */
# define	DOCpnLCRM	2	/* Lowercase Roman numeral (i, ii, iii) */
# define	DOCpnUCLTR	3	/* Uppercase letter (A, B, C) */
# define	DOCpnLCLTR	4	/* Lowercase letter (a, b, c) */
# define	DOCpnORD	5	/* Ordinal number (1st, 2nd, 3rd) */
# define	DOCpnCARD	6	/* Cardinal text number (One, Two Three) */
# define	DOCpnORDT	7	/* Ordinal text number (First, Second, Third) */

# define	DOCpnDBNUM	10	/* Kanji numbering without the digit character (*dbnum1) */
# define	DOCpnDBNUMD	11	/* Kanji numbering with the digit character (*dbnum2) */
# define	DOCpnAIU	12	/* 46 phonetic katakana characters in "aiueo" order (*aiueo) */
# define	DOCpnIROHA	13	/* 46 phonetic katakana characters in "iroha" order (*iroha) */
					/* 14	Double-byte character */
					/* 15	Single-byte character */
# define	DOCpnDBNUML	16	/* Kanji numbering 3 (*dbnum3) */
# define	DOCpnDBNUMK	17	/* Kanji numbering 4 (*dbnum4) */
# define	DOCpnCNUM	18	/* Circle numbering (*circlenum) */
# define	DOCpnDECD	19	/* Double-byte Arabic numbering	*/
# define	DOCpnAIUD	20	/* 46 phonetic double-byte katakana characters (*aiueo*dbchar) */
# define	DOCpnIROHAD	21	/* 46 phonetic double-byte katakana characters (*iroha*dbchar) */
# define	DOCpn_0DEC	22	/* Arabic with leading zero (01, 02, 03, ..., 10, 11) */
# define	DOCpn_BULLET	23	/* Bullet (no number at all) */
# define	DOCpnGANADA	24	/* Korean numbering 2 (*ganada) */
					/* 25	Korean numbering 1 (*chosung) */
# define	DOCpnGBNUM	26	/* Chinese numbering 1 (*gb1) */
# define	DOCpnGBNUMD	27	/* Chinese numbering 2 (*gb2) */
# define	DOCpnGBNUML	28	/* Chinese numbering 3 (*gb3) */
# define	DOCpnGBNUMK	29	/* Chinese numbering 4 (*gb4) */
# define	DOCpnZODIAC	30	/* Chinese Zodiac numbering 1 (* zodiac1) */
# define	DOCpnZODIACD	31	/* Chinese Zodiac numbering 2 (* zodiac2) */
# define	DOCpnZODIACL	32	/* Chinese Zodiac numbering 3 (* zodiac3) */
					/* 33	Taiwanese double-byte numbering 1 */
					/* 34	Taiwanese double-byte numbering 2 */
					/* 35	Taiwanese double-byte numbering 3 */
					/* 36	Taiwanese double-byte numbering 4 */
					/* 37	Chinese double-byte numbering 1 */
					/* 38	Chinese double-byte numbering 2 */
					/* 39	Chinese double-byte numbering 3 */
					/* 40	Chinese double-byte numbering 4 */
					/* 41	Korean double-byte numbering 1 */
# define	DOCpnCHOSUNG	42	/* Korean double-byte numbering 2 */
					/* 43	Korean double-byte numbering 3 */
					/* 44	Korean double-byte numbering 4 */
					/* 45	Hebrew non-standard decimal  */
# define	DOCpnBIDIA	46	/* Arabic Alif Ba Tah */
# define	DOCpnBIDIB	48	/* 48	Arabic Abjad style */
					/* 47	Hebrew Biblical standard */
# define	DOCpn_NONE	48	/* No number */
/*===*/
# define	DOCpnDBNUMT	DOCpnDBNUML	/* ? */
# define	DOCpnAIUEO	DOCpnAIU	/* ? */
# define	DOCpnAIUEOD	DOCpnAIUD	/* ? */

/************************************************************************/
/*									*/
/*  Holds the position of placeholders in the format that are replaced	*/
/*  the level numbers in the format string.				*/
/*  Actually there is an extra one that holds the length of the format	*/
/*  string. (In lnOffsetBytes)						*/
/*									*/
/************************************************************************/

typedef struct LevelNumber
    {
    int		lnOffsetBytes;	/*  Offset in llFormatString of the	*/
    				/*  end of the bytes that precede the	*/
				/*  level number.			*/
    int		lnConstChars;	/*  The number of unicode characters	*/
				/*  that is encoded in UTF-8 at the	*/
				/*  offset in the format string. This	*/
				/*  is only used to write RTF.		*/
    int		lnLevel;	/*  The list level that it shows.	*/
    } LevelNumber;

typedef struct ListLevel
    {
    int			llStartAt;
			/**
			 * The kind of number or bullet: A DOCpn* value
			 */
    unsigned char	llNumberStyle;
    unsigned char	llJustification;
    unsigned char	llFollow;
    unsigned char	llPrevToDigits;
    unsigned char	llNoRestart;
    unsigned char	llFontBias;
    short int		llPictureNumber;

    MemoryBuffer	llFormatString;
			/************************************************/
			/*  In memory:					*/
			/*  ----------					*/
			/*  A string thet holds the constant pieces of	*/
			/*  the format. The level numbers and the length*/
			/*  are in llLevelNumbers.			*/
			/*  In the file:				*/
			/*  ------------				*/
			/*  The first position holds the size of the	*/
			/*  rest of the string. The rest is a format	*/
			/*  string consisting of pieces of constant	*/
			/*  string and placeholders for the level	*/
			/*  numbers in the paragraph number. The	*/
			/*  placeholders have low values that indicate	*/
			/*  what level number to substitute. Use	*/
			/*  llLevelNumbers to find the placeholders	*/
			/*  Though the byte values are a clue,		*/
			/*  llLevelNumbers is leading. E.G:		*/
			/*  1.1.1 -> \'06\'00.\'01.\'02 in the file	*/
			/*  and ".." in memory.				*/
			/************************************************/
    long		llTemplateID;
    LevelNumber *	llLevelNumbers;
    int			llLevelNumberCount;
			/************************************************/
			/*  In the file:				*/
			/*  ------------				*/
			/*  Bytes giving the positions of level place-	*/
			/*  holders in llTextFormat. The subscript	*/
			/*  does not tell what level to substitute.	*/
			/*  The byte in llTextFormat holds the level	*/
			/*  number to substitute. As the format starts	*/
			/*  with its size, indices start from 1. E.G:	*/
			/*  1.1.1 -> \'01\'03\'05 in the file.		*/
			/*  NOTE that the indices are in terms of	*/
			/*  characters: As the format string might have	*/
			/*  a multi-byte encoding, the indices cannot	*/
			/*  be used as byte indices.			*/
			/************************************************/

    unsigned char	llFromOld;		/*  O  */
    unsigned char	llUsePrevText;		/*  O  */
    unsigned char	llUsePrevSpace;	/*  O  */
    int			llIndent;		/*  O  */
    int			llSpace;		/*  O  */

    int			llTabStopListNumber;
    int			llLeftIndentTwips;
    int			llFirstIndentTwips;
    PropertyMask	llParaPropertyMask;

    TextAttribute	llTextAttribute;
    PropertyMask	llTextAttributeMask;
    } ListLevel;

typedef enum ListLevelProperty
    {
    LLpropSTARTAT,
    LLpropSTYLE,
    LLpropJUSTIFY,
    LLpropFOLLOW,
    LLpropLEGAL,
    LLpropNORESTART,
    LLpropPICTURE,
    LLpropFBIAS,

    LLpropTEXT,
    LLpropTEMPLATEID,
    LLpropNUMBERS,

    LLpropOLD,
    LLpropPREV,
    LLpropPREVSPACE,
    LLpropINDENT,
    LLpropSPACE,

    LLprop_COUNT
    } ListLevelProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentListLevel(	ListLevel *		ll );
extern void docCleanDocumentListLevel(	ListLevel *		ll );

extern int docCopyDocumentListLevel(	ListLevel *		to,
					const ListLevel *	from,
					int			copyIds,
					const int *		fontMap,
					const int *		colorMap,
					const int *		rulerMap );

extern int docListLevelSetStyle( ListLevel *			ll,
				const PropertyMask *		paraMask,
				const struct ParagraphProperties *	pp,
				const PropertyMask *		textMask,
				const TextAttribute *		ta );

extern int docListLevelSetText(	int *			pChanged,
				ListLevel *		ll,
				const char *		text,
				int			idx );

extern int docListLevelGetText(	char *			to,
				int			maxsize,
				const ListLevel *	ll,
				int			idx );

extern int docListLevelSetNumber(	int *			pChanged,
					ListLevel *		ll,
					int			number,
					int			idx );

extern int docListLevelInsertNumber(	ListLevel *		ll,
					int			number,
					int			idx );

extern int docListLevelDeleteNumber(	ListLevel *		ll,
					int			idx );

extern int docListLevelFormatParagraphNumber( char *		to,
					int			maxsize,
					int *			offsets,
					int			maxoffsets,
					int			ilvl,
					const int *		numberPath,
					const int *		startPath,
					const int *		formatPath,
					const ListLevel *	ll );

extern int docListLevelFormatLevelNumber(
				char *				target,
				int				maxsize,
				int				val,
				int				format );

extern int docDefaultListLevel(	ListLevel *			llTo,
				int				level,
				int				deftab,
				int				levelTemplateID,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern int docListLevelGetField( int *				pConstOff,
				int *				pConstLen,
				int *				pConstChars,
				int *				pLevel,
				const ListLevel *		ll,
				int				field );

extern int docListLevelToRtfStrings(
				MemoryBuffer *			mbtext,
				MemoryBuffer *			mbnumbers,
				const ListLevel *		ll );

extern int docListLevelFromRtfStrings(
				ListLevel *			ll,
				const MemoryBuffer *		mbtext,
				const MemoryBuffer *		mbnumbers );

extern void docListDocumentListLevel(	int			n,
					const ListLevel *	ll );

extern int docSetListLevelProperty(	ListLevel *		ll,
					int			prop,
					int			val );

extern int docListLevelApplyTextAttribute(
					ListLevel *		ll,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet );

#   endif
