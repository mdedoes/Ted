#   ifndef		DOC_LAYOUT_POSITION_H
#   define		DOC_LAYOUT_POSITION_H

struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  Used to layout lines of text.					*/
/*									*/
/*  A0 paper is 4* 29.7 cm high: 20* 28.3465* 4* 29.7= 67351.3 Twips.	*/
/*  So an unsigned short limits us to A1 paper.				*/
/*									*/
/************************************************************************/

typedef struct LayoutPosition
    {
    int			lpPageYTwips;
    unsigned short	lpPage;
    unsigned char	lpColumn;
    unsigned char	lpAtTopOfColumn;
    } LayoutPosition;

typedef struct BlockOrigin
    {
			/**
			 *  The horizontal shift (on the page) that 
			 *  is used to draw a piece of text in a different 
			 *  location than where it was formatted for. 
			 *  (E.G. Table headers that are repeated on a 
			 *  subsequent page or in a subsequent column.)
			 */
    int			boXShift;

			/**
			 *  The vertical shift (on the page) that 
			 *  is used to draw a piece of text in a different 
			 *  location than where it was formatted for. 
			 *  (E.G. Table headers that are repeated on a 
			 *  subsequent page or in a subsequent column.)
			 */
    int			boYShift;

			/**
			 *  Use this page, rather than the one 
			 *  from the document position. [If boOverrideFrame
			 *  != 0]
			 */
    int			boOverridePage;

			/**
			 *  Use this column, rather than the one 
			 *  from the document position. [If boOverrideFrame
			 *  != 0]
			 */
    int			boOverrideColumn;

			/**
			 *  Use boOverridePage and boOverrideColumn, rather 
			 *  than the ones from the document position. If 
			 *  boOverrideFrame != 0
			 *
			 *  This is used to draw table headers on a different 
			 *  page.
			 */
    unsigned char	boOverrideFrame;
    } BlockOrigin;

# define DOC_SAME_POSITION( lp1, lp2 ) ( \
		    (lp1)->lpPageYTwips == (lp2)->lpPageYTwips	&& \
		    (lp1)->lpPage == (lp2)->lpPage		&& \
		    (lp1)->lpColumn == (lp2)->lpColumn		)

#   define LPOSDEB(lp) appDebug( "%s(%3d): %s= [%2d.%d:%4d%c]\n",	\
			    __FILE__, __LINE__, #lp,		\
			    (lp)->lpPage, (lp)->lpColumn,	\
			    (lp)->lpPageYTwips,			\
			    (lp)->lpAtTopOfColumn?'^':'_' );

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitLayoutPosition(	LayoutPosition *	lp );
extern void docInitBlockOrigin(		BlockOrigin *		bo );

extern void docLayoutPushBottomDown(	LayoutPosition *	lpRowBottom,
					const LayoutPosition *	lpColBottom );

extern void docShiftPosition(	LayoutPosition *		to,
				const BlockOrigin *		bo,
				const LayoutPosition *		from );

extern int docCompareLayoutPositions(
				const LayoutPosition *		lp1,
				const LayoutPosition *		lp2 );

extern int docCompareLayoutPositionFrames(
				const LayoutPosition *		lp1,
				const LayoutPosition *		lp2 );

extern int docCompareLayoutPositionToFrame(
				const LayoutPosition *		lp,
				int				page,
				int				column );

extern void docShiftRectangle(	struct DocumentRectangle *	to,
				const BlockOrigin *		bo,
				const struct DocumentRectangle * from );

#   endif	/*	DOC_LAYOUT_POSITION_H	*/
