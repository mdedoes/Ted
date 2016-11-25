/************************************************************************/
/*									*/
/*  A line of text in a paragraph.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_LINE_H
#   define	DOC_TEXT_LINE_H

#   include	"docLayoutPosition.h"
#   include	<geoRectangle.h>

/************************************************************************/
/*									*/
/*  Used to lay out the text on the page.				*/
/*  A text line consists of a series of particules.			*/
/*									*/
/************************************************************************/

typedef struct TextLine
    {
			/**
			 *  The offset in the paragraph of the head of 
			 *  the line
			 */
    int			tlStroff;
    int			tlFirstParticule;

			/**
			 *  The number in bytes in this line of text.
			 */
    short int		tlStrlen;
    short int		tlParticuleCount;

			/**
			 *  An estimate of the number of words in the line.
			 */
    short int		tlWordCount;

			/**
			 *  Page (and column) and Y position on the page.
			 */
    LayoutPosition	tlTopPosition;

			/**
			 *  The rectangle of the line. Verically, it is 
			 *  around the base line: Y0 is negative (ascender 
			 *  above the base line) and Y1 is positive (descender
			 *  below the base line.)
			 *
			 *  Horizontally, X0 and X1 are numbers relative 
			 *  to pf.pfCellContentRect.drX0. I.E without the 
			 *  takong left- or right indent into account.
			 */
    DocumentRectangle	tlRectangle;

			/**
			 *  The height of the line: The distance from the 
			 *  top of this line to the top of a potential 
			 *  line below it. It includes leading below this 
			 *  line.
			 */
    short int		tlLineStride;

			/**
			 *  Various boolean flags that the formatter determines 
			 *  and that are cached on the line for efficiency.
			 */
    unsigned char	tlFlags;
#			define TLflagBLOCKBREAK		0x01
/*			define TLflagLINEBREAK		0x02 */
#			define TLflagINLINECONTENT	0x04
#			define TLflagSHADING		0x08
#			define TLflagBORDER		0x10

			/**
			 *  The width of the frame for which the line was
			 *  last formatted. When the line is shifted to 
			 *  a frame with th the same width, it is sufficient
			 *  to change the coordinates of the line.
			 *  When the frame width is different, the line
			 *  must be reformatted. (and most probably subsequent
			 *  lines as well).
			 *  NOTE that a short int cannot hold the width of A0
			 *  paper in landscape orientation.
			 */
    unsigned short int	tlFrameWidthTwips;

			/**
			 *  In lines that are physically right-aligned, 
			 *  differences between the real font and the 
			 *  font that is matched on screen, can cause a 
			 *  dented right margin. Shift the contents of the 
			 *  last text run in the line by this number of 
			 *  pixels to draw a straight right margin.
			 */
    short int		tlPixelShift;
    } TextLine;

/************************************************************************/

# define docInvalidateTextLine( tl ) (tl)->tlFrameWidthTwips= 0

/************************************************************************/

extern void docInitTextLine(		TextLine *			tl );

extern void docTextLineGetBaseline(	LayoutPosition *		lp,
					const TextLine *		tl );

extern void docTextLineGetBelowLine(	LayoutPosition *		lp,
					const TextLine *		tl );

extern void docTextLineGetNextLineTop(	LayoutPosition *		lp,
					const TextLine *		tl );

extern void docTextLineGetShiftedTop(	LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl );

extern void docTextLineGetShiftedBaseline(
					LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl );

extern void docTextLineGetShiftedBelowLine(
					LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl );

extern void docTextLineGetShiftedNextLineTop(
					LayoutPosition *		lp,
					const struct BlockOrigin *	bo,
					const TextLine *		tl );

#   endif
