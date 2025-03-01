/************************************************************************/
/*									*/
/*  Text Editor Section specifics.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_NODE_H
#   define	DOC_SECT_NODE_H

#   include	<docSelectionScope.h>

struct SectHeadersFooters;
struct BufferItem;
struct BufferDocument;
struct SectionProperties;

typedef struct SectNode
    {
				/**
				 * Formatting properties of the section.
				 */
    struct SectionProperties *	bsProperties;
				/**
				 *  Headers and footers. (If any)
				 */
    struct SectHeadersFooters *	bsHeadersFooters;

				/**
				 *  The role in the document. This is most
				 *  important in the roots of the various
				 *  external trees.
				 */
    SelectionScope		bsSelectionScope;
    } SectNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

#   define biSectHeadersFooters		BIU.biuSect.bsHeadersFooters
#   define biSectSelectionScope		BIU.biuSect.bsSelectionScope
#   define biSectProperties		BIU.biuSect.bsProperties

#   define biSectDocumentGeometry	biSectProperties->spDocumentGeometry
#   define biSectColumnCount		biSectProperties->spColumnCount
#   define biSectHasTitlePage		biSectProperties->spHasTitlePage
#   define biSectHasEndPage		biSectProperties->spHasEndPage
#   define biSectBreakKind		biSectProperties->spBreakKind
#   define biSectPageNumberStyle	biSectProperties->spPageNumberStyle
#   define biSectPageNumberHyphen	biSectProperties.spPageNumberHyphen
#   define biSectRestartPageNumbers	biSectProperties->spRestartPageNumbers
#   define biSectStartPageNumber	biSectProperties->spStartPageNumber

#   define biSectNotesProperties	biSectProperties->spNotesProperties

#   define biSectFirstPageHeaderNoPdfArtifact \
					biSectProperties->spFirstPageHeaderNoPdfArtifact
#   define biSectFirstPageFooterNoPdfArtifact \
					biSectProperties->spFirstPageFooterNoPdfArtifact
#   define biSectLastPageHeaderNoPdfArtifact \
					biSectProperties->spLastPageHeaderNoPdfArtifact
#   define biSectLastPageFooterNoPdfArtifact \
					biSectProperties->spLastPageFooterNoPdfArtifact

/************************************************************************/
/*									*/
/*  Routine dectarations.						*/
/*									*/
/************************************************************************/

extern int docSectColumnsAreBalanced(
				const struct BufferItem *	sectNode );

extern int docCopySectHeadersFooters(	struct BufferItem *	sectNodeTo,
				struct BufferDocument *		bdTo,
				const struct BufferItem *	sectNodeFrom,
				struct BufferDocument *		bdFrom );

extern int docCopySectDescription(
				struct BufferItem *		sectNodeTo,
				struct BufferDocument *		bdTo,
				const struct BufferItem *	sectNodeFrom,
				struct BufferDocument *		bdFrom );

#   endif
