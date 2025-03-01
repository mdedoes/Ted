/************************************************************************/
/*									*/
/*  Bookkeeping for INCLUDETEXT fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_INCLUDETEXT_FIELD_H
#   define DOC_INCLUDETEXT_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct IncludeTextField
    {
			/**
			 *  The file name of the document to include.
			 */
    MemoryBuffer	itfFilename;

			/**
			 *  The (optional) bookmark in the source document.
			 *  If a bookmark is given, only include the contents
			 *  of the bookmark.
			 */
    MemoryBuffer	itfBookmark;

    unsigned char	itfConstantFields;	/*  \!	*/
    MemoryBuffer	itfClassName;		/*  \c	*/
    MemoryBuffer	itfNamespace;		/*  \n	*/
    MemoryBuffer	itfTransform;		/*  \t	*/
    MemoryBuffer	itfXPath;		/*  \x	*/

			/**
			 *  Ted JSON merge specific extension. The name that
			 *  is given to every instance of the JSON path query 
			 *  expression that determines how many instances are 
			 *  to be included and into which content is to be 
			 *  substituted via MERGEFIELDs
			 */

    MemoryBuffer	itfFor;			/*  \f	*/
			/**
			 *  Ted JSON merge specific extension. The JSON path 
			 *  query expression that determines how many 
			 *  instances are to be included and into which 
			 *  content is to be substituted via MERGEFIELDs and 
			 *  other INCLUDETEXT fields.
			 */
    MemoryBuffer	itfIn;			/*  \i	*/
    } IncludeTextField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitIncludeTextField(		IncludeTextField *	itf );
extern void docCleanIncludeTextField(		IncludeTextField *	itf );

extern int docGetIncludeTextField(	IncludeTextField *		itf,
					const struct DocumentField *	df );

#   endif /*  DOC_INCLUDETEXT_FIELD_H  */
