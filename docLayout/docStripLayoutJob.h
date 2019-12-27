/************************************************************************/
/*									*/
/*  Document Layout functionality: Perform the layout of a series	*/
/*  of paragraphs (css: display=block) and table rows			*/
/*  (css: display=table-row)						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STRIP_LAYOUT_JOB_H
#   define	DOC_STRIP_LAYOUT_JOB_H

#   include	<docStripFrame.h>
#   include	<docLayoutPosition.h>

/************************************************************************/
/*									*/
/*  For formatting a series of paragraphs. ( Inside a Cell or simply	*/
/*  below eachother )							*/
/*									*/
/************************************************************************/

struct ParagraphLayoutJob;
struct LayoutJob;

typedef struct ParagraphLayoutPosition
    {
				    /**
				     *  The frame that we use to layout the 
				     *  children.
				     *  It belongs to the position as it depends
				     *  on the paragraph margins.
				     */
    ParagraphFrame		plpParagraphFrame;

				    /**
				     *  Progress in the chain of children.
				     */
    int				pspChild;
    int				pspChildAdvanced;
    int				pspPart;
    int				pspLine;

				    /**
				     *  Recursion in parallel children.
				     *  I.E: cells in a row.
				     */
    struct ParagraphLayoutJob *	pspChildren;
    int				pspChildCount;

				    /**
				     *  The current vertical progress on the 
				     *  page. (Actually in the newspaper style 
				     *  column.)
				     */
    LayoutPosition		plpPos;
    } ParagraphLayoutPosition;

typedef struct ParagraphLayoutJob
    {
    const struct LayoutJob *	pljLayoutJob;
    int				pljChildUpto;

    LayoutPosition		cljMergedCellBelowPosion;

    ParagraphLayoutPosition	pljPos;
    ParagraphLayoutPosition	pljPos0;
    } ParagraphLayoutJob;

# define docLayoutAtStripHead( plp ) \
		    ( (plp)->pspChild == 0	&& \
		      (plp)->pspPart == 0	)

/* TODO: Should it not have been this way?
# define docLayoutAtStripHead( plp ) \
		    ( (plp)->pspChild == 0	&& \
		      (plp)->pspChildAdvanced == 0	)
*/

# define docLayoutStripDone( plp, plj ) \
		    ( (plp)->pspChild >= (plj)->pljChildUpto )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParagraphLayoutPosition( ParagraphLayoutPosition *	plp );

extern void docInitParagraphLayoutJob( ParagraphLayoutJob *		plj );
extern void docCleanParagraphLayoutJob( ParagraphLayoutJob *		plj );

extern int docParagraphLayoutPosClaimChildren(
					ParagraphLayoutPosition *	plj,
					const struct LayoutJob *	lj,
					int				count );

extern void docBeginParagraphLayoutProgress(
					ParagraphLayoutJob *	plj,
					const struct LayoutJob *	lj,
					int			child,
					int			line,
					int			part,
					int			pUpto,
					const LayoutPosition *	lp );

extern void docStripLayoutStartChild(	ParagraphLayoutPosition *	plp,
					int				child);
extern void docStripLayoutNextChild(	ParagraphLayoutPosition *	plp );

extern int docCompareLayoutProgress(
				const ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1 );

#   endif /*	DOC_STRIP_LAYOUT_JOB_H */

