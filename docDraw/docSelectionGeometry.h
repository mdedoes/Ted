#   ifndef		DOC_SELECTION_GEOMETRY_H
#   define		DOC_SELECTION_GEOMETRY_H

#   include		<docLayoutPosition.h>
#   include		<geoRectangle.h>

struct LayoutContext;
struct BufferItem;
struct DocumentPosition;
struct DocumentSelection;
struct TextLine;
struct ParagraphFrame;

/*  2  */
typedef struct PositionGeometry
    {
    int			pgPositionFlags;

			/**
			 *  X Coordinate of the position.
			 */
    int			pgXTwips;
    int			pgXPixels;

			/**
			 *  The top of the line that holds this position.
			 *  [ A position can be at the break of two lines, 
			 *  but the geometry always refers to one of the 
			 *  lines. This can be derived from the line flags 
			 *  or the (deprecated) line number.
			 */
    LayoutPosition	pgTopPosition;

			/**
			 *  The baseline of the line that holds this position.
			 *  [See note with top]
			 */
    LayoutPosition	pgBaselinePosition;

			/**
			 *  Immediately below the bottom of the line that 
			 *  holds this position. This is at the top of the 
			 *  leading. [See note with top]
			 */
    LayoutPosition	pgBelowLinePosition;

			/**
			 *  The (potential) top location of a line below the 
			 *  line that holds this position. This includes the 
			 *  leading below this line. [See note with top]
			 */
    LayoutPosition	pgNextLinePosition;

			/**
			 *  The X0 coordinate of the frame that limits 
			 *  the extents of the parent frame that holds 
			 *  the paragraph. It is derived from the page frame
			 *  or, for paragraphs in nested tables, from the 
			 *  cell in the parent table.
			 */
    int			pgParentFrameX0;
			/**
			 *  The X1 coordinate of the frame that limits 
			 *  the extents of the parent frame that holds 
			 *  the paragraph. It is derived from the page frame
			 *  or, for paragraphs in nested tables, from the 
			 *  cell in the parent table.
			 */
    int			pgParentFrameX1;
    } PositionGeometry;

typedef struct SelectionGeometry
    {
			/**
			 *  A rectangle that holds the whole selection.
			 *  It is used to expose the selection whenever 
			 *  something changes.
			 *
			 *  A rectangle that covers more than the selection
			 *  is inefficient. But functionally it is perfectly
			 *  OK. If the rectangle is too small, too little 
			 *  is redrawn after a change.
			 */
    DocumentRectangle	sgRectangle;

			/**
			 *  Geometry of the head of the selection.
			 */
    PositionGeometry	sgHead;

			/**
			 *  Geometry of the tail of the selection.
			 */
    PositionGeometry	sgTail;
    } SelectionGeometry;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitPositionGeometry(	PositionGeometry *	pg );
extern void docInitSelectionGeometry(	SelectionGeometry *	sg );

extern void docSetPositionGeometry(
				struct PositionGeometry *	pg,
				const struct ParagraphFrame *	pf,
				const struct TextLine *		tl,
				int				xPixels,
				int				xTwips,
				int				flags );

extern void docPositionGeometry(struct PositionGeometry *	pg,
				const struct DocumentPosition *	dp,
				int				lastOne,
				const struct LayoutContext *	lc );

extern void docSelectionGeometry(
				struct SelectionGeometry *	sg,
				const struct DocumentSelection * ds,
				const struct BufferItem *	bodySectNode,
				int				lastLine,
				const struct LayoutContext *	lc );

extern int docFindPositionInLine(
				struct DocumentPosition *	dp,
				struct PositionGeometry *	pg,
				const struct LayoutContext *	lc,
				struct BufferItem *		paraNode,
				struct TextLine *		tl,
				int				docXPixels );


#   endif	/*	DOC_SELECTION_GEOMETRY_H	*/
