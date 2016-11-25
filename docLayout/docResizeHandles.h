/************************************************************************/
/*									*/
/*  Resize handles on an object or an image.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RESIZE_HANDLES_H
#   define	DOC_RESIZE_HANDLES_H

		    /**
		     *  The size in pixels of the resize handles 
		     *  for an object or an image.
		     */
#   define	RESIZE_BLOCK		10

		    /**
		     *  Names for the eight resize handles.
		     */
typedef enum ResizeHandle
    {
    RESIZE_BOTTOM_LEFT= 0,
    RESIZE_BOTTOM_MIDDLE,
    RESIZE_BOTTOM_RIGHT,

    RESIZE_MIDDLE_LEFT,
    RESIZE_MIDDLE_RIGHT,

    RESIZE_TOP_LEFT,
    RESIZE_TOP_MIDDLE,
    RESIZE_TOP_RIGHT,

    RESIZE_COUNT
    } ResizeHandle;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  DOC_RESIZE_HANDLES_H	*/
