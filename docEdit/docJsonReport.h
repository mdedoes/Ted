/************************************************************************/
/*									*/
/*  Merge data from a JSON file into a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_JSON_REPORT_H
#   define	DOC_JSON_REPORT_H

struct SimpleLocale;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSubstituteJsonProperties(
			const char *				jsonName,
			const struct SimpleLocale *		sl,
			struct BufferDocument *			bd );

#    endif	/*  DOC_JSON_REPORT_H	*/
