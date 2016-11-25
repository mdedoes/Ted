/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docTextParticule.h"

void docInitTextParticule(	TextParticule *		tp )
    {
    tp->tpStroff= 0;
    tp->tpStrlen= 0;
    tp->tpKind= TPkindUNKNOWN;
    tp->tpFlags= 0;
    tp->tpWide= 0;	
    tp->tpObjectNumber= -1;
    tp->tpTextAttrNr= 0;

    return;
    }

