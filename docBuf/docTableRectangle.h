#   ifndef		DOC_TABLE_RECTANGLE_H
#   define		DOC_TABLE_RECTANGLE_H

typedef struct TableRectangle
    {
			/**
			 *  The first (left-most in a left to right table)
			 *  column of the selected rectangle.
			 */
    int			trCol0;

			/**
			 *  The last (right-most in a left to right table)
			 *  column of the selected rectangle.
			 */
    int			trCol1;

			/**
			 *  The last (right-most in a left to right table)
			 *  column of the table.
			 */
    int			trCol11;

			/**
			 *  The sequence number in the parent of the 
			 *  first row in the table.
			 */
    int			trRow00;

			/**
			 *  The sequence number in the parent of the 
			 *  first row in the selected rectangle.
			 */
    int			trRow0;

			/**
			 *  The sequence number in the parent of the 
			 *  last row in the selected rectangle.
			 */
    int			trRow1;

			/**
			 *  The sequence number in the parent of the 
			 *  last row in the table.
			 */
    int			trRow11;

			/**
			 *  The sequence number in the parent of the 
			 *  first row that is not a header.
			 */
    int			trPastHeaderRow;

			/**
			 *  The selection consists of one or more rows 
			 *  in the table. (IE the whole rows are selected)
			 */
    unsigned char	trIsRowSlice;

			/**
			 *  The selection consists of one or more columns 
			 *  in the table. (IE the whole columns are selected)
			 */
    unsigned char	trIsColumnSlice;

			/**
			 *  The selection is a table slice: It consists 
			 *  of whole rows, whole columns or the whole table.
			 */
    unsigned char	trIsTableSlice;

			/**
			 *  The selection is the whole table.
			 */
    unsigned char	trIsWholeTable;

			/**
			 *  True if and only if the selection is inside 
			 *  a single table cell.
			 */
    unsigned char	trIsSingleCell;

			/**
			 *  In single cell rectangles, this is the colspan 
			 *  of the cell.
			 */
    int			trCellColspan;

			/**
			 *  In single cell rectangles, this is the rowspan 
			 *  of the cell.
			 */
    int			trCellRowspan;
    } TableRectangle;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTableRectangle(	TableRectangle *	tr );

extern int docIntersectTableRectangle(	TableRectangle *		tr,
					const TableRectangle *		tr1,
					const TableRectangle *		tr2 );

extern void docExpandTableRectangleToWholeRows(	TableRectangle *	tr );
extern void docExpandTableRectangleToWholeColumns( TableRectangle *	tr );

#   endif	/*	DOC_TABLE_RECTANGLE_H	*/
