/***** Zl *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Zl_2000_20ff[32]= {0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Zl_2000_2fff[16]= {
    UCD_Zl_2000_20ff,
};
static const unsigned char * const * const UCD_Zl[16]={
    (const unsigned char **)0, /* 0000..0fff */
    (const unsigned char **)0, /* 1000..1fff */
    UCD_Zl_2000_2fff,
};

int ucdIsZl( int sym ) {
    return ucdInClass( sym,  UCD_Zl );
}
