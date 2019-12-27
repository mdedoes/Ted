/************************************************************************/
/*									*/
/*  Description of a TOC field.						*/
/*									*/
/************************************************************************/

# ifndef DOC_TOC_FIELD_H
# define DOC_TOC_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;
struct FieldInstructions;

typedef enum TocType
    {
    TOCtypeTOC,
    TOCtypeSEQ,

    TOCtype_COUNT
    } TocType;

typedef struct StyleNameLevel
    {
    int		snlLevel;
    char *	snlStyleName;
    } StyleNameLevel;

typedef struct TocField
    {
			/* a,c,f */
    MemoryBuffer	tfIdentifierName;

			/**
			 * Flag b
			 *
			 * Limit the table of contents to the contents of 
			 * this bookmark.
			 */
    MemoryBuffer	tfMarkName;

			/* p */
    MemoryBuffer	tfSeparatorBytes;

			/**
			 *  Flag s
			 *
			 *  Holds the name of the sequence to index.
			 *  E.G: In a list of tables or figures.
			 *  The TOC indexes the paragraphs that hold a SEQ 
			 *  field with this name.
			 */
    MemoryBuffer	tfSeqName;

			/* d */
    MemoryBuffer	tfSeqSeparatorBytes;

			/**
			 * Flag t.
			 *
			 * Holds a comma separated list of style names and
			 * levels. (First the name, then the level) Include 
			 * paragraphs with the style n the TOC at the level
			 * given.
			 */
    MemoryBuffer	tfStyleNamesBytes;

			/* include what */
    unsigned char	tfType;

    unsigned char	tfUseTcEntries;		/* \\l */
    unsigned char	tfEntryLevel0;
    unsigned char	tfEntryLevel1;

    unsigned char	tfUseStyleNames;	/* \\t */

			/**
			 * Flag o
			 *
			 * Use outline levels to decide what paragraphs 
			 * to include in the table of contents. tfStyleLevel[01]
			 * give the range of outline levels.
			 * If the paragraph overrides the outline level from
			 * the style, use the one from the style.
			 */
    unsigned char	tfUseStyleLevels;
    unsigned char	tfStyleLevel0;
    unsigned char	tfStyleLevel1;

			/**
			 * Flag u
			 *
			 * Use the explicit outline levels, rather than those 
			 * from the style.
			 */
    unsigned char	tfUseOutlineLevels;

			/* presentation */
    unsigned char	tfIncludeSeqLabelAndNumber;

			/**
			 * Flag h
			 *
			 * Emit hyperlinks
			 */
    unsigned char	tfHyperlinks;

			/**
			 * Flag z
			 *
			 * Do not include separators and/or page numbers
			 * (Only in combination with hyperlinks?)
			 */
    unsigned char	tfWebNoNumber;

    unsigned char	tfPreserveTabs;
    unsigned char	tfPreserveNewlines;
    unsigned char	tfNLevel0;
    unsigned char	tfNLevel1;

			/* derived */
    StyleNameLevel *	tfStyleNameLevels;
    int			tfStyleNameLevelCount;
    } TocField;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTocField(	TocField *			tf );
extern void docCleanTocField(	TocField *			tf );

extern int docCopyTocField(	TocField *			to,
				const TocField *		from );

extern int docFieldGetToc(	TocField *			tf,
				const struct DocumentField *	df );

extern int docTocFieldSetTocInstructions(
				struct FieldInstructions *	fi,
				const TocField *		tf );

#   endif
