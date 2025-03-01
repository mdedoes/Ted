/************************************************************************/
/*									*/
/*  For what pages do headers and footers apply?			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_HDFT_SCOPES_H
#   define	DOC_HDFT_SCOPES_H

struct DocumentTree;
struct BufferItem;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Headers and/or footers appear on different pages, depending on	*/
/*  whether the section has a title page and/or the document has facing	*/
/*  pages. These are the options as they exist on the tool. In my	*/
/*  opinion these are also the possibilities from the users perspective.*/
/*									*/
/*  The arrays are a mapping from the options to the header/footer data	*/
/*  structure that is used in the implementation.			*/
/*									*/
/************************************************************************/

typedef enum WhatPages
    {
    PAGES_FIRST_PAGE= 0,
    PAGES_SUBSEQUENT_PAGES,
    PAGES_ALL_PAGES,
    PAGES_ODD_PAGES,
    PAGES_EVEN_PAGES,
    PAGES_LAST_PAGE,

    PAGES__COUNT
    } WhatPages;

extern const int DOC_HeaderScopes[PAGES__COUNT];
extern const int DOC_FooterScopes[PAGES__COUNT];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docDrawWhatPageHeader( struct DocumentTree **	pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd );

extern int docDrawWhatPageFooter( struct DocumentTree **		pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd );

extern int docLayoutWhatPageHeader( struct DocumentTree **	pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd );

extern int docLayoutWhatPageFooter( struct DocumentTree **	pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const struct BufferDocument *	bd );

#   endif
