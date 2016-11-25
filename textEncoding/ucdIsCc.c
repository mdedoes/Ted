/***** Cc *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Cc_0000_00ff[32]= {0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0,0,0,0,0x80,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Cc_0000_0fff[16]= {
    UCD_Cc_0000_00ff,
};
static const unsigned char * const * const UCD_Cc[16]={
    UCD_Cc_0000_0fff,
};

int ucdIsCc( int sym ) {
    return ucdInClass( sym,  UCD_Cc );
}
