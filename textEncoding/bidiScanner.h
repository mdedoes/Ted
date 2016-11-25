/************************************************************************/
/*									*/
/*  Scan a bidi tree or a text to build the tree.			*/
/*									*/
/************************************************************************/

#   ifndef		TEXT_BIDI_SCANNER_H
#   define		TEXT_BIDI_SCANNER_H

/**
 *  Callback for scanning text or bidirectional trees
 */
typedef int (*BidiGotRun)(		void *		through,
					int		initiator,
					int		level,
					int		from,
					int		upto );

typedef struct BidiScanner
    {
			/**
			 *  A pass through object
			 */
    void *		bsThrough;
			/**
			 *  The call to make every time a run or a (candidate) 
			 *  node is encountered
			 */
    BidiGotRun		bsGotRun;
    } BidiScanner;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void bidiInitScanner(		BidiScanner *		bs );

#   endif	/*	TEXT_BIDI_SCANNER_H	*/
