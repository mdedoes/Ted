/************************************************************************/
/*									*/
/*  Text Editor Section Headers and Footers specifics.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_HDFT_H
#   define	DOC_SECT_HDFT_H

#   include	"docDocumentTree.h"

typedef struct SectHeadersFooters
    {
				/*  ============= Page headers ============ */
				/**
				 *  Header used on the first page of the
				 *  section. Its applicability depends on the
				 *  section properties.
				 */
    DocumentTree		shfFirstPageHeader;

				/**
				 *  Header used on the even pages of the
				 *  section. Its applicability for the even
				 *  pages of this section and for the first page
				 *  of this section depends on the section
				 *  properties.
				 */
    DocumentTree		shfLeftPageHeader;

				/**
				 *  Header used on the odd pages of the
				 *  section. Its applicability for the even
				 *  pages of this section and for the first page
				 *  of this section depends on the section
				 *  properties.
				 */
    DocumentTree		shfRightPageHeader;

				/**
				 *  Header used on the last page of the section.
				 *  Its applicability depends on the section 
				 *  properties A one page section with a
				 *  distinct title page and a special last page
				 *  uses the headers and footers for the first 
				 *  page on the single page.
				 *  properties. NOTE: This implements a Ted 
				 *  extension of the rtf file format and the 
				 *  MS-Word document model. To avoid a hoard 
				 *  of issues, this header must not be higher 
				 *  than either the left or the right header.
				 */
    DocumentTree		shfLastPageHeader;

				/*  ============= Page footers ============ */
				/**
				 *  Footer used on the first page of the
				 *  section. Its applicability depends on the
				 *  section properties.
				 */
    DocumentTree		shfFirstPageFooter;

				/**
				 *  Footer used on the even pages of the
				 *  section. Its applicability for the even
				 *  pages of this section and for the first page
				 *  of this section depends on the section
				 *  properties.
				 */
    DocumentTree		shfLeftPageFooter;

				/**
				 *  Footer used on the odd pages of the
				 *  section. Its applicability for the even
				 *  pages of this section and for the first page
				 *  of this section depends on the section
				 *  properties.
				 */
    DocumentTree		shfRightPageFooter;

				/**
				 *  Footer used on the last page of the section.
				 *  Its applicability depends on the section 
				 *  properties A one page section with a
				 *  distinct title page and a special last page
				 *  uses the headers and footers for the first 
				 *  page on the single page.
				 *  properties. NOTE: This implements a Ted 
				 *  extension of the rtf file format and the 
				 *  MS-Word document model. To avoid a hoard 
				 *  of issues, this footer must not be higher 
				 *  than either the left or the right footer.
				 */
    DocumentTree		shfLastPageFooter;
    } SectHeadersFooters;

/************************************************************************/
/*									*/
/*  Routine dectarations.						*/
/*									*/
/************************************************************************/

extern void docCleanSectHeadersFooters(
				struct BufferDocument *		bd,
				SectHeadersFooters *		shf );

extern void docInitSectHeadersFooters(
				SectHeadersFooters *		shf );

#   endif
