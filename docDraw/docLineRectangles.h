#   ifndef	DOC_LINE_RECTANGLES_H
#   define	DOC_LINE_RECTANGLES_H

/************************************************************************/
/*									*/
/*  Visit the intersection of a text line with the selection.		*/
/*									*/
/************************************************************************/

#   include	<docSelect.h>
#   include	<geoRectangle.h>

struct DocumentRectangle;
struct BufferItem;
struct BufferDocument;
struct TextLine;

typedef int (*VisitLineRectangle)(
				const struct DocumentRectangle * dr,
				struct BufferItem *		paraNode,
				void *				through );

typedef struct LineRectangles
    {
				/**
				 *  The document that holds the line with 
				 *  a selection highlight to draw
				 */
    struct BufferDocument *	lrDocument;

				/**
				 *  The paragraph that holds the line with 
				 *  a selection highlight to draw
				 */
    struct BufferItem *		lrParaNode;

				/**
				 *  The line with  a selection highlight to 
				 *  draw
				 */
    struct TextLine *		lrTextLine;

				/**
				 *  Intersection of the selection with 
				 *  this line.
				 */
    DocumentSelection		lrDsThisLine;

				/**
				 *  The frame that was used to format this
				 *  line. (Pixel coordinates relative to 
				 *  the document.)
				 */
    const struct DocumentRectangle * lrDrThisLineFrame;

				/**
				 *  Layout context for geometry calculations.
				 */
    const struct LayoutContext * lrLayoutContext;

				/**
				 *  True if and only if the line 
				 *  intersects the current selection.
				 *  (Also true if the tail of the selection 
				 *  is at the head of the line, or if the 
				 *  or the head of the selection is at the 
				 *  tail of the line.)
				 */
    unsigned char		lrIntersectsSelection;

				/**
				 *  Compare slection head to line head.
				 */
    int				lrCmp_ShLh;

				/**
				 *  Compare slection tail to line head.
				 */
    int				lrCmp_StLh;

				/**
				 *  Compare slection head to line tail.
				 */
    int				lrCmp_ShLt;

				/**
				 *  Compare slection tail to line tail.
				 */
    int				lrCmp_StLt;

				/**
				 *  True if and only if the selection is 
				 *  in more than one cell of a table. 
				 *  (Highlight the whole cell)
				 */
    int				lrAsTableRectangle;

				/*********/

				/**
				 *  We traverse the line from left to right.
				 *  On our way, we enter and leave the 
				 *  selection. Every time we leave the higlight,
				 *  and after the last piece, (if we are in a 
				 *  highlighted stretch.) we draw the rectangle
				 *  that we leave.
				 */
    int				lrInHighlight;

				/**
				 *  The pixel coordinate where we last 
				 *  entered the highlight.
				 */
    int				lrHighlightX0;

				/**
				 *  Sliding X Coordinate.
				 */
    int				lrXTwips;

				/**
				 *  A realistic value for Y.
				 */
    int				lrYPixels;

				/**
				 *  Pass-through pointer for visitRectangle.
				 */
    void *			lrThrough;

				/**
				 *  Called for all rectangle encountered along 
				 *  the way.
				 */
    VisitLineRectangle		lrVisitRectangle;

    } LineRectangles;

typedef struct CollectRectangles
    {
    int			crCount;
    DocumentRectangle	crUnion;
    } CollectRectangles;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCollectRectangles(
			CollectRectangles *		cr );

extern int docForAllLineRectangles(
			LineRectangles *		lr,
			int				x0Twips,
			int				partsSeparately,
			VisitLineRectangle		visitRectangle,
			void *				through );

extern void docSetLineRectangles(
			LineRectangles *		lr,
			const struct LayoutContext *	lc,
			const struct DocumentSelection * ds,
			struct BufferItem *		paraNode,
			struct TextLine *		tl,
			int				yPixels,
			const struct DocumentRectangle * drLineFrame );

extern int docTextLineSelectedCollectRectangles(
			LineRectangles *		lr,
			int				x0Twips,
			int				partsSeparately,
			CollectRectangles *		cr );

#   endif
