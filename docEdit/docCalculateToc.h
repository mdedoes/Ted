/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CALCULATE_TOC_H
#   define	DOC_CALCULATE_TOC_H

#   include	<docTocField.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docListDepth.h>
#   include	<docSelect.h>
#   include	<docParaProperties.h>
#   include	<utilIndexSet.h>
#   include	<textAttribute.h>

struct DocumentField;
struct FieldStackLevel;
struct BufferItem;
struct DocumentStyle;

typedef struct TocEntry
    {
    int					teLevel;
    const struct DocumentField *	teField;
    DocumentSelection			teDsInside;
    DocumentSelection			teDsAround;
    int					tePart0;
    int					tePart1;
    const MemoryBuffer *		teMarkName;
    int					teNumbered;
    } TocEntry;

/************************************************************************/

typedef struct CalculateToc
    {
				/**
				 *  The intermediate document holding the 
				 *  table of contents. Finally we will 
				 *  insert it in the document.
				 */
    struct BufferDocument *	ctBdToc;
				/**
				 *  The document that we index and that will 
				 *  recieve the table of contents.
				 */
    struct BufferDocument *	ctBdDoc;

				/**
				 *  The section that will hold the table of 
				 *  contents.
				 */
    struct BufferItem *		ctSectNode;

				/**
				 *  The section in the body to whiche the 
				 *  node that will hold the table of  contents
				 * belongs.
				 */
    struct BufferItem *		ctBodySectNode;

				/**
				 * To build the paragraphs in the intermediary
				 * document.
				 */
    struct ParagraphBuilder *	ctParagraphBuilder;

				/**
				 *  Keep track of the stack of fields while
				 *  we build the field hierarchy in the 
				 *  temporary document;
				 */
    struct FieldStackLevel *	ctFieldStack;

    TocField			ctTocField;

				/**
				 *  Frame on the page for the layout of 
				 *  the TOC. Derived from the source document 
				 *  that holds the TOC
				 */
    BlockFrame			ctBlockFrame;
				/**
				 *  Frame to make the paragraps fit
				 */
    ParagraphFrame		ctParagraphFrame;

    ParagraphProperties		ctRefPP;
    TextAttribute		ctTextAttribute;

				/**
				 *  The styles to use fort the different 
				 *  levels.
				 */
    const struct DocumentStyle * ctLevelStyles[PPoutline_COUNT];
				/**
				 *  The text attributes to use fort the 
				 *  different levels.
				 */
    TextAttribute 		ctLevelAttributes[PPoutline_COUNT];

				/**
				 *  The numbers of the styles that are 
				 *  listed in the table of contents.
				 */
    IndexSet			ctStyleNumbers;

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
