/************************************************************************/
/*									*/
/*  Definitions for finding patterns in the document and spelling	*/
/*  checking.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_COLLECT_STATISTICS_H
#   define	DOC_COLLECT_STATISTICS_H

struct BufferDocument;
struct DocumentStatistics;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docCollectDocumentStatistics(
				struct DocumentStatistics *	ds,
				const struct BufferDocument *	bd );

#   endif
