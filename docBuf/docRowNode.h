/************************************************************************/
/*									*/
/*  Text Editor Row specific definitions.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_NODE_H
#   define	DOC_ROW_NODE_H

#   include	<docLayoutPosition.h>
#   include	<utilMemoryBuffer.h>

struct RowProperties;

/************************************************************************/
/*									*/
/*  Row specific information.						*/
/*									*/
/*  4)  If a row is a table header, it might be repeated on every page	*/
/*	where the table is continued. When the row is drawn on a	*/
/*	different page. No separate administration has to be kept to	*/
/*	handle the case where a row starts on a new page and the header	*/
/*	appears above it: It we make the header row behave as if it	*/
/*	has the (obsolete) \\trkeepfollow tag, it will appear in the	*/
/*	correct location in the normal text flow.			*/
/*									*/
/************************************************************************/

typedef struct RowNode
    {
				/**
				 *  Properties of the row or NULL if the row 
				 *  is not really a row in a table and 
				 *  immediately after the row is created.
				 */
    const struct RowProperties * brProperties;

				/**
				 *  Index in the property administration of 
				 *  the properties of the row, or 0 if the row 
				 *  is not really a row in a table.
				 */
    int				brRowPropertyNumber;

				/**
				 *  A text string with a summary of the table.
				 *  The first summary on a table ends up as the
				 *  summary of the table in the generated PDF.
				 *  (This is an ad-hoc extension for tagged PDF.)
				 */
    MemoryBuffer		brSummary;

				/**
				 *  The rtf file format does not really support 
				 *  tables. Keep an administration of the set 
				 *  of rows that looks like a table. First and 
				 *  past refer to similar rows.
				 *
				 *  The sequence number of the first table
				 *  row in the parent of the sequence of rows 
				 *  that we have decided to be a table.
				 */
    int				brTableFirst;

				/**
				 *  The sequence number plus one of the last 
				 *  table row in the parent of the sequence of 
				 *  rows that we have decided to be a table.
				 */
    int				brTablePast;

				/**
				 *  The sequence number in the parent node of 
				 *  the first row below the header rows.
				 *  If there are no header rows, 
				 *  brPastHeaderRow == brTableFirst.
				 *
				 *  The way that we have implemented the 
				 *  header feature, actually assumes that 
				 *  the header rows in a table are contiguous
				 *  and this value actually points past the 
				 *  last header row. All rows before the last 
				 *  header row are treated as header rows.
				 */
    int				brPastHeaderRow;

				/**
				 *  Cells with a rowspan can protrude below the 
				 *  actual row: To optimize position lookups 
				 *  and to delimit areas to redraw correctly, 
				 *  the position below all 'rowspan' columns is
				 *  remembered.
				 */
    LayoutPosition		brBelowAllCellsPosition;

				/**
				 *  The top position of the table header row 
				 *  that appears above this ordinary row. 
				 *  (This must be kept with ordinary rows at 
				 *  the top of a page; I.E. if 
				 *  brPrecededByHeader is true. )
				 */
    LayoutPosition		brAboveHeaderPosition;

				/**
				 *  Space to skip for borders. The top inset is 
				 *  also used immediately after a real table 
				 *  row in plain text.
				 */
    short int			brTopInset;

				/**
				 *  A flag indicating that this row is preceded 
				 *  by a table header that does not appear at 
				 *  its home position.
				 */
    unsigned char		brPrecededByHeader;

				/**
				 *  A flag indicating that this row, nor any cell in this
				 *  row, has a property that makes it a real table row.
				 *  PDF generation uses this flag to decide that a table 
				 *  is a formatting means, rather than that the table is 
				 *  a meaningful table. The logic is only suitable to make 
				 *  a decision for PDF generation.
				 */
    unsigned char		brIsPlain;

				/**
				 *  Is this row going to be a table row? Only 
				 *  used while parsing rtf. In a complete 
				 *  document the cell count in the row 
				 *  properties is leading.
				 */
    unsigned char		brForTable;
    } RowNode;

/************************************************************************/
/*									*/
/*  Shortcut defines throuw the BufferItem union.			*/
/*									*/
/************************************************************************/

#   define	biRowSummary		BIU.biuRow.brSummary

#   define	biRowTopInset		BIU.biuRow.brTopInset

#   define	biRowTableFirst		BIU.biuRow.brTableFirst
#   define	biRowTablePast		BIU.biuRow.brTablePast
#   define	biRowPastHeaderRow	BIU.biuRow.brPastHeaderRow
#   define	biRowPrecededByHeader	BIU.biuRow.brPrecededByHeader
#   define	biRowIsPlain		BIU.biuRow.brIsPlain
#   define	biRowBelowAllCellsPosition \
					BIU.biuRow.brBelowAllCellsPosition
#   define	biRowAboveHeaderPosition \
					BIU.biuRow.brAboveHeaderPosition
#   define	biRowForTable		BIU.biuRow.brForTable

#   define	biRowProperties		BIU.biuRow.brProperties
#   define	biRowPropertyNumber	BIU.biuRow.brRowPropertyNumber

#   endif
