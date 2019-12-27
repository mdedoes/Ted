/************************************************************************/
/*									*/
/*  Document Layout: The frame in which a series of paragraps and	*/
/*  table rows is to be layed out on the page.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STRIP_FRAME_H
#   define	DOC_STRIP_FRAME_H

#   include	<geoRectangle.h>

/************************************************************************/
/*									*/
/*  The frame in which a series of successive lines in a paragraph is	*/
/*  is formatted. The frame lies on one page.				*/
/*									*/
/*  Coordinates are absolute coordinates on the page or on the window.	*/
/*  [On the window they are shifted by the scrollbars.]			*/
/*									*/
/************************************************************************/

typedef struct ParagraphFrame
    {
			/**
			 *  The frame in which the lines of the paragraph 
			 *  have to fit. Left- and Right indent have been 
			 *  subtracted from the rectangle.
			 *
			 *  This is very similar to the content rect in CSS2.
			 *
			 *  The first line potentially has a different
			 *  rectangle. With a negative first line indent,
			 *  the first line protrudes to the left in 
			 *  left-to-right paragraphs and to the right in 
			 *  right-to-left paragraphs.
			 */
    DocumentRectangle	pfParaContentRect;

			/**
			 *  The frame in which the paragraph itself has to fit.
			 *  If the parent of the paragraph is a table cell,
			 *  cell margins/padding have been subtracted.
			 *
			 *  Tabs are relative to this rectangle.
			 */
    DocumentRectangle	pfCellContentRect;
    DocumentRectangle	pfCellRect;

			/**
			 *  The content rectangle that limits the extents
			 *  the frame that holds the paragraph. It is 
			 *  derived from the page frame or, for paragraphs in 
			 *  nested tables, from the cell in the parent table.
			 */
    DocumentRectangle	pfParentContentRect;
    } ParagraphFrame;

# define PARA_FRAME_WIDTH( pf ) \
		((pf)->pfParaContentRect.drX1-(pf)->pfParaContentRect.drX0)
# define PARA_FRAME_RIGHT( pf ) \
		((pf)->pfCellContentRect.drX1)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParagraphFrame(	ParagraphFrame *	pf );

#   endif /*	DOC_STRIP_FRAME_H */
