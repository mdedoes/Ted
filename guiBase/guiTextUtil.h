/************************************************************************/
/*									*/
/*  Exchange values with a text widget.					*/
/*									*/
/************************************************************************/

#   ifndef	GUI_TEXT_UTIL_H
#   define	GUI_TEXT_UTIL_H

#   include	"guiBase.h"

struct MemoryBuffer;

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   if  ! USE_HEADLESS

extern int guiLengthFromText(		APP_WIDGET	w,
					int *		pValue,
					int *		pChanged,
					int		defaultUnit,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int guiIntegerFromText(		APP_WIDGET	w,
					int *		pValue,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int guiDoubleFromText(		APP_WIDGET	w,
					double *	pValue,
					double		minValue,
					int		adaptToMin,
					double		maxValue,
					int		adaptToMax );

extern void guiIntegerToText(		APP_WIDGET		w,
					int			n );

extern void guiIntervalToText(		APP_WIDGET		w,
					int			n1,
					int			n2 );

void guiSubIntervalToText(		APP_WIDGET		w,
					int			part1,
					int			part2,
					int			whole1,
					int			whole2 );

extern void guiRectangleToText(		APP_WIDGET		w,
					int			row1,
					int			row2,
					int			col1,
					int			col2 );

extern void guiDoubleToText(		APP_WIDGET		w,
					double			d );

extern void guiLengthToText(		APP_WIDGET		w,
					int			l,
					int			unit );

extern int guiBufferFromText(		struct MemoryBuffer *	mb,
					APP_WIDGET		w );

extern void guiBufferToText(		APP_WIDGET			w,
					const struct MemoryBuffer *	mb );

#   endif

# ifdef __cplusplus
    }
# endif

#   endif
