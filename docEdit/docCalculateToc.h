/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	<docTocField.h>
#   include	<docBlockFrame.h>

#   ifndef	DOC_CALCULATE_TOC_H
#   define	DOC_CALCULATE_TOC_H

typedef struct TocEntry
    {
    int				teLevel;
    const DocumentField *	teField;
    DocumentSelection		teDsInside;
    DocumentSelection		teDsAround;
    int				tePart0;
    int				tePart1;
    const MemoryBuffer *	teMarkName;
    int				teNumbered;
    } TocEntry;

/************************************************************************/

typedef struct CalculateToc
    {
    BufferDocument *		ctBdToc;
    BufferDocument *		ctBdDoc;
    struct BufferItem *		ctSectBi;
    DocumentField *		ctDfTocTo;
    TocField			ctTocField;
    BlockFrame			ctBlockFrame;
    ParagraphProperties		ctRefPP;
    TextAttribute		ctTextAttribute;
    int				ctDefaultTextAttributeNumber;
    const DocumentStyle *	ctLevelStyles[PPoutline_COUNT];
    int				ctLevelAttributeNumbers[PPoutline_COUNT];

    TocEntry *			ctEntries;
    int				ctEntryCount;
    int				ctDepth;

    unsigned char *		ctStyleLevels;
    int				ctStyleLevelCount;
    } CalculateToc;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCalculateToc(	CalculateToc *	ct );
extern void docCleanCalculateToc(	CalculateToc *	ct );

extern int docCollectTocInput(	CalculateToc *			ct );

#   endif /*	DOC_CALCULATE_TOC_H	*/
