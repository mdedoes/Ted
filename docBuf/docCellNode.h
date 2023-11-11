/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of a (table) cell.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CELL_NODE_H
#   define	DOC_CELL_NODE_H

/************************************************************************/
/*									*/
/*  A Cell.								*/
/*									*/
/************************************************************************/

struct CellProperties;

typedef struct CellNode
    {
				/**
				 *  The properties of this cell. The
				 *  object is owned by the row properties
				 *  administration and it is set when the
				 *  properties of the parent row are set.
				 *  So it is NULL immediately after the node
				 *  is created.
				 */
    const struct CellProperties * bcProperties;

				/**
				 *  Used for the vertical alignment of
				 *  table cells: The first line of the
				 *  first paragraph is shifted by this
				 *  amount.
				 */
    int				bcTopInset;

				/**
				 *  Set by the formatter to know whether
				 *  the cell is part of a rowspan. It is
				 *  used to know when to calculate the
				 *  vertical alignment of the cell and
				 *  to reformat the correct rows after the
				 *  height of the cell changes.
				 */
    int				bcRowspan;

				/**
				 *  The number of the row in the table that
				 *  holds the top cell of a stack of cells
				 *  in a rowspan.
				 *
				 *  This is determined while the row is
				 *  formatted. It is only set in the
				 *  bottom cell of the rowspan.
				 */
    int				bcMergedCellTopRow;
				/**
				 *  The number of the column in the table that
				 *  holds the top cell of a stack of cells
				 *  in a rowspan.
				 *
				 *  This is determined while the row is
				 *  formatted. It is only set in the
				 *  bottom cell of the rowspan.
				 */
    int				bcMergedCellTopCol;

				/**
				 *  The head offset of this cell in its row.
				 *  The  value is derived from the properties
				 *  of the parent row every time they change.
				 *  This is not a geometric coordinate, but
				 *  the offset of the cell in the frame of
				 *  The row. Its value is independent of
				 *  the direction of the row. In left to
				 *  right rows, this is the offset of the
				 *  left hand side of the cell from the left
				 *  hand side of the frame of the row.
				 *  In right to left rows, it is the offset
				 *  of the right hand side of the cell from
				 *  the right hand side of the frame of the
				 *  row.
				 */
    int				bcHeadX;

				/**
				 *  The tail offset of this cell in its row.
				 *  The  value is derived from the properties
				 *  of the parent row every time they change.
				 *  This is not a geometric coordinate, but
				 *  the offset of the cell in the frame of
				 *  The row. Its value is independent of
				 *  the direction of the row. In left to
				 *  right rows, this is the offset of the
				 *  right hand side of the cell from the left
				 *  hand side of the frame of the row.
				 *  In right to left rows, it is the offset
				 *  of the left hand side of the cell from
				 *  the right hand side of the frame of the
				 *  row.
				 */
    int				bcTailX;

				/**
				 *  The column span of this cell. The
				 *  value is derived from the properties
				 *  of the parent row every time they change.
				 */
    int				bcColspan;
    } CellNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

#   define	biCellTopInset		BIU.biuCell.bcTopInset
#   define	biCellRowspan		BIU.biuCell.bcRowspan
#   define	biCellMergedCellTopRow	BIU.biuCell.bcMergedCellTopRow
#   define	biCellMergedCellTopCol	BIU.biuCell.bcMergedCellTopCol
#   define	biCellProperties	BIU.biuCell.bcProperties
#   define	biCellHeadX		BIU.biuCell.bcHeadX
#   define	biCellTailX		BIU.biuCell.bcTailX
#   define	biCellColspan		BIU.biuCell.bcColspan

/************************************************************************/

# define docIsMergedCell( node ) ( \
    docIsCellNode( (node) ) &&  \
    (node)->biCellProperties &&  \
    CELL_MERGED( (node)->biCellProperties ) )

#   endif
