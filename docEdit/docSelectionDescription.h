#   ifndef		DOC_SELECTION_DESCRIPTION_H
#   define		DOC_SELECTION_DESCRIPTION_H

#   include		<textAttribute.h>
#   include		<docTableRectangle.h>
#   include		<utilPropMask.h>
#   include		<docCellProperties.h>

struct DocumentSelection;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Describe a selection and its relevance for application tools.	*/
/*									*/
/************************************************************************/

typedef struct SelectionDescription
    {
    unsigned int	sdDocumentId;

			/**
			 *  The rest of the fields is only relevant if 
			 *  sdIsSet != 0.
			 */
    unsigned char	sdIsSet;

    unsigned char	sdDocumentReadonly;
    unsigned char	sdIsIBarSelection;
    unsigned char	sdIsSingleParagraph;
    unsigned char	sdInContiguousParagraphs;
    unsigned char	sdIsSingleFrame;
    unsigned char	sdHeadInFrame;

			/**
			 *  Is the head of the selection in a table?
			 */
    int			sdHeadTableNesting;
    unsigned char	sdHeadInTable;
			/**
			 *  Is the tail of the selection in a table?
			 */
    int			sdTailTableNesting;
    unsigned char	sdTailInTable;
			/**
			 *  Is the head of the selection in the header of 
			 *  a table?
			 */
    unsigned char	sdHeadInTableHeader;
    unsigned char	sdHeadInMultiColumnSection;

			/**
			 *  The selection is in a single table.
			 */
    unsigned char	sdInOneTable;
			/**
			 *  The selection is a rectangular block in a single
			 *  table.
			 */
    unsigned char	sdIsTableSlice;
			/**
			 *  The selection is an object or image.
			 */
    unsigned char	sdIsObjectSelection;

			/**
			 * from
			 * docFindParticuleOfPosition( dsHead, PARAfindFIRST )
			 */
    int			sdHeadFlags;
			/**
			 * from
			 * docFindParticuleOfPosition( dsTail, PARAfindPAST )
			 */
    int			sdPastFlags;

    int			sdHeadPage;
    int			sdTailPage;
    int			sdDocumentPages;

    int			sdHeadSection;
    int			sdTailSection;
    int			sdDocumentSections;

			/**
			 *  Is is possible to replace the selection with 
			 *  something else?
			 */
    int			sdCanReplace;

    int			sdInTreeType;
			/**
			 *  Is the selection in the document body?
			 *  Avoid testing on the actual value of sdInTreeType.
			 */
    unsigned char	sdInDocumentBody;
    int			sdInHeaderFooter;

			/**
			 *  The most relevant header type:
			 *  If the selection is in the document body, this
			 *  the kind of header that applies for the current 
			 *  page. If the selection is in a header it is the 
			 *  type of header. In all other cases the value must 
			 *  be ignored.
			 */
    int			sdHeaderTypeForSelection;

			/**
			 *  The most relevant footer type:
			 *  If the selection is in the document body, this
			 *  the kind of footer that applies for the current 
			 *  page. If the selection is in a footer it is the 
			 *  type of footer. In all other cases the value must 
			 *  be ignored.
			 */
    int			sdFooterTypeForSelection;

    unsigned char	sdHeadInField;
    unsigned char	sdHeadInHyperlink;
    unsigned char	sdHeadInBookmark;
    unsigned char	sdHeadFieldKind;

    unsigned char	sdIsListBullet;
    unsigned char	sdHasNote;
    unsigned char	sdInNote;

    unsigned char	sdHasLists;
    int			sdFirstListParaNr;
    int			sdListOverride;
    int			sdListLevel;
    int			sdMultiList;
    int			sdMultiLevel;

    int			sdMajorityFontSize;

			/**
			 *  The block in a table that holds the selection.
			 *  Is also set if we have a selection inside a single
			 *  cell.
			 */
    TableRectangle	sdTableRectangle;
			/**
			 *  The cell properties of the selection if it is 
			 *  a table rectangle.  Is also set if we have a
			 *  selection inside a single cell.
			 */
    CellProperties	sdCellProperties;

    TextAttribute	sdTextAttribute;
    PropertyMask	sdTextAttributeMask;
    int			sdTextAttributeNumber;
    } SelectionDescription;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSelectionDescription(
			SelectionDescription *			sd );

extern void docDescribeSelection(
			SelectionDescription *			sd,
			const struct DocumentSelection *	ds,
			struct BufferDocument *			bd,
			int					headPage,
			int					tailPage,
			unsigned int				documentId,
			int					documentRo );

extern void docEnableEditCommands(
			unsigned char *				cmdEnabled,
			const SelectionDescription *		sd );

#   endif	/*	DOC_SELECTION_DESCRIPTION_H	*/
