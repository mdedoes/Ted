/************************************************************************/
/*									*/
/*  Ruler drawing primitives.						*/
/*									*/
/************************************************************************/

#   include	<drawDrawingSurface.h>
#   include	<appRuler.h>
#   include	<docTabStopList.h>
#   include	<docRulerColumnSeparator.h>

typedef struct TedTopRuler
    {
    RulerData		ttrRulerData;
    double		ttrMagnification;

				/**
				 *  Area that is accessible for column 
				 *  separators. It is not possible to shift
				 *  a column separator out of this area.
				 */
    int			ttrTableColumnX0;
    int			ttrTableColumnX1;

				/**
				 *  Area that can be used to set tabs
				 *  It is not possible to drag a tab
				 *  out of this area or to create a tab 
				 *  outside it.
				 */
    int			ttrParaFrameX0;
    int			ttrParaFrameX1;

				 /**
				  *  Page margins from document or section.
				  */
    int			ttrMarginX0;
    int			ttrMarginX1;
					/********************************/
					/*  Describe the area that is	*/
					/*  accessible as values.	*/
					/********************************/

    int			ttrFirstIndent;		/*  pixels		*/
    int			ttrLeftIndent;		/*  pixels		*/
    int			ttrRightIndent;		/*  pixels		*/

			    /**
			     *  Tab stop list for the tabs stops that are 
			     *  displayed.
			     */
    TabStopList		ttrTabStopList;

			    /**
			     *  Alignment for fresh tab stops.
			     */
    int			ttrTabKind;

			    /**
			     *  Table columns in Pixels
			     */
    int			ttrColumnCount;
    ColumnSeparator *	ttrColumns;
    } TedTopRuler;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedTopRulerDrawTicks(	TedTopRuler *		ttr,
					int			ox,
					int			c0,
					int			c1 );

extern void tedHangingButtonRect(	struct DocumentRectangle * drButton,
					int			value,
					const RulerData *	rd );

extern void tedRulerDrawHangingButton(	int			value,
					const RulerData *	rd );

extern void tedStandingButtonRect(	struct DocumentRectangle * drButton,
					int			value,
					const RulerData *	rd );

extern void tedRulerDrawStandingButton(	int			value,
					const RulerData *	rd );

extern void tedRulerColumnControlRect(	struct DocumentRectangle * drButton,
					int			x0,
					int			x1,
					const RulerData *	rd );

extern void tedRulerDrawColumnControl(	int			x0,
					int			x1,
					int			ox,
					const RulerData *	rd );

extern void tedTabSymbolRect(		struct DocumentRectangle *	drButton,
					int			tabX,
					const RulerData *	rd );

extern void tedRulerDrawTab(		const RulerData *	rd,
					int			tabX,
					int			kind );

extern void tedTabButtonRectangle(	struct DocumentRectangle *	drButton,
					int			x,
					const RulerData *	rd );

extern void tedDrawTabButton(		const RulerData *	rd,
					int			kind );


extern void tedRulerActiveRect(		struct DocumentRectangle *	drActive,
					const TedTopRuler * 	ttr );

extern int tedRulerMakeDrawingSurface(	TedTopRuler *		ttr,
					APP_WIDGET		w );

