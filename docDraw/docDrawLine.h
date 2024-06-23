#   ifndef	DOC_DRAW_LINE_H
#   define	DOC_DRAW_LINE_H

#   include	<docLayoutPosition.h>
#   include	<geoRectangle.h>

struct DrawingContext;
struct ParticuleData;
struct TextLine;
struct BufferItem;
struct ParagraphFrame;
struct BufferDocument;

/************************************************************************/

typedef struct DrawTextLine
    {
    void *			dtlThrough;
    struct DrawingContext *	dtlDrawingContext;

				/**
				 *  The document that we are drawing
				 */
    struct BufferDocument *	dtlDocument;

				/**
				 *  The document tree that we are currently drawing
				 */
    const struct DocumentTree *	dtlTree;

				/**
				 *  Pixel rectangle for the line. It is used
				 *  to optimize and to centralize pixel 
				 *  calculations for screen drawing.
				 *
				 *  It holds the frame in which the line 
				 *  is formatted. (In the paragraph frame)
				 */
    DocumentRectangle		dtlLineFrameRectanglePixels;

				/**
				 *  The rectangle occupied by the particules 
				 *  on the line. Coordinates are true 
				 *  geometric coordinates on the actual 
				 *  page. So X0 is the left most position 
				 *  of the first particule in display order 
				 *  and X1 is the right most position  of 
				 *  the last particule in display order.
				 *  Coordinates are relative to the cell frame:
				 *  Indents are taken into account.
				 *
				 *  The Y range is from the ascender
				 *  to below the potential leading of the 
				 *  line.
				 *
				 *  [ Horizontal and vertical shifts of 
				 *  headers and footers have been accounted 
				 *  for. ]
				 */
    DocumentRectangle		dtlTextRectangle;

				/**
				 *  The vertical position of the top of the 
				 *  text line. Shifts of headers and footers 
				 *  have been taken into account.
				 */
    LayoutPosition		dtlShiftedTopPosition;

				/**
				 *  The vertical position of the base line of 
				 *  the text line. Shifts of headers and 
				 *  footers been taken into account.
				 */
    LayoutPosition		dtlShiftedBaselinePosition;

				/**
				 *  The vertical position just below the 
				 *  the text line. Shifts of headers and 
				 *  footers been taken into account.
				 */
    LayoutPosition		dtlShiftedBelowLinePosition;

				/**
				 *  The pixel position for 
				 *	dtlShiftedBaselinePosition
				 */
    int				dtlShiftedBaselinePixels;

				/**
				 *  The pixel pisition of the 'center' of 
				 *  the line. It is used to calculate 
				 *  the distance of a screen position from 
				 *  a position in the line.
				 */
    int				dtlShiftedLineCenterPixels;

				/**
				 *  The height of the line: Derived from 
				 *  ascender and descender of the line.
				 */
    int				dtlLineHeight;

				/**
				 *  The horizontal shift (on the page) that 
				 *  is used to draw a piece of text in a 
				 *  different location than where it was 
				 *  formatted for. (E.G. Table headers that are 
				 *  repeated on a subsequent page or in a 
				 *  subsequent column.) [From the BlockOrigin]
				 */
    int				dtlXShift;

				/**
				 *  The vertical shift (on the page) that 
				 *  is used to draw a piece of text in a 
				 *  different location than where it was 
				 *  formatted for. (E.G. Table headers that are 
				 *  repeated on a subsequent page or in a 
				 *  subsequent column.) [From the BlockOrigin]
				 */
    int				dtlYShift;

    int				dtlPartUpto;
    const struct ParticuleData * dtlParticuleData;

				/**
				 *  The line that we are drawing. Do NOT 
				 *  modify anything on the line, but as 
				 *  we pass the text line to a paragraph 
				 *  scanner, this cannot be a const member.
				 */
    struct TextLine *		dtlTextLine;

				/**
				 *  The paragraph that holds the line that we
				 *  are drawing. Do NOT modify anything in 
				 *  the paragraph, but as we pass the paragraph
				 *  to a paragraph scanner, this cannot be a 
				 *  const member.
				 */
    struct BufferItem *		dtlParaNode;

				/**
				 *  The frame in which the lines of the
				 *  paragraphs are positioned. Indents and
				 *  tabs are relative to this frame.
				 */
    const struct ParagraphFrame * dtlParagraphFrame;

				/**
				 *  Usually, we draw runs of text with the 
				 *  same attributes with one call to the 
				 *  actual drawing code. If this flag is set, 
				 *  we draw the particules separately.
				 */
    unsigned char		dtlDrawParticulesSeparately;

				/**
				 *  Current display X position in line in 
				 *  display order. It is incremented 
				 *  with the particule width while 
				 *  drawing. This is a twips value that 
				 *  is part of the state of the drawing 
				 *  process.
				 */
    int				dtlX;
    } DrawTextLine;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
