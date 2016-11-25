#   ifndef		DOC_SCANNER_H
#   define		DOC_SCANNER_H

/**
 *  Return values for NodeVisitors, TreeVisitors, RunVisitors, ParticuleVisitors
 */
typedef enum ScanAdvice
    {
	    /**
	     *  ERROR. Actually any negative value must be treated as an 
	     *  error. So return this value from your code, but test for < 0
	     */
    SCANadviceERROR= -1,

	    /**
	     *  OK: Node handled: Enter the children of the node (if any) 
	     *  and proceed.
	     */
    SCANadviceOK= 0,

	    /**
	     *  STOP: The visitor reached its goal. Return from the scan.
	     */
    SCANadviceSTOP,

	    /**
	     *  SKIP: The enter visitor decides that we can skip this node 
	     *	including its children.
	     */
    SCANadviceSKIP,

    SCANadvice_COUNT
    } ScanAdvice;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*	DOC_SCANNER_H	*/
