/************************************************************************/
/*									*/
/*  Document Layout: Reasons for the formatter to stop.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_STOP_CODE_H
#   define	DOC_LAYOUT_STOP_CODE_H

/************************************************************************/
/*									*/
/*  Reasons why the formatter stops.					*/
/*									*/
/************************************************************************/

typedef enum FormatterResult
    {
	    /**
	     *  Stop the formatting process because it is ready.
	     */
    FORMATstopREADY= 0,

	    /**
	     *  Stop the formatting process (in this frame) because 
	     *  all nodes in the frame have been formatted.
	     */
    FORMATstopNEXT_FRAME,

	    /**
	     *  Stop the formatting process (in this frame) because 
	     *  we have reached the bottom of the available space that 
	     *  is constrained by a parent node. E.G. by a fixed height 
	     *  table row.
	     */
    FORMATstopFRAME_FULL,

	    /**
	     *  Stop the formatting process (in this block) because 
	     *  we have reached the bottom of the available space in the 
	     *  current column of the page. (Or simply the bottom of the 
	     *  page in the usual single page column layout case.)
	     */
    FORMATstopBLOCK_FULL,

	    /**
	     *  Stop the formatting process (in this column) because 
	     *  we have encountered an explicit column break.
	     */
    FORMATstopCOLUMN_BREAK,

	    /**
	     *  Stop the formatting process (in this column or page) because 
	     *  we have encountered an explicit page break.
	     */
    FORMATstopPAGE_BREAK,

	    /**
	     *  Similar to FORMATstopBLOCK_FULL. But the vertical position
	     *  was rewound to implement widow/orphan control.
	     */
    FORMATstopPARTIAL,

	    /**
	     *  The number of possible values
	     */
    FORMATstop_COUNT
    } FormatterResult;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern const char *	docLayoutStopCodeStr(	int	stopCode );

#   endif	/*  DOC_LAYOUT_STOP_CODE_H  */
