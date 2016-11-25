/************************************************************************/
/*									*/
/*  Scan a bidi tree or a text to build the tree.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"bidiScanner.h"

void bidiInitScanner(		BidiScanner *	bs )
    {
    bs->bsThrough= (void *)0;
    bs->bsGotRun= (BidiGotRun)0;
    }

