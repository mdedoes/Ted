/***** Lt *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Lt_0100_01ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,0x09,0,0,0,0,0x04,0,};
static const unsigned char * const UCD_Lt_0000_0fff[16]= {
    (const unsigned char *)0, /* 0000..00ff */
    UCD_Lt_0100_01ff,
};
static const unsigned char UCD_Lt_1f00_1fff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0,0xff,0,0xff,0,0x10,0,0x10,0,0,0,0,0,0x10,};
static const unsigned char * const UCD_Lt_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    (const unsigned char *)0, /* 1700..17ff */
    (const unsigned char *)0, /* 1800..18ff */
    (const unsigned char *)0, /* 1900..19ff */
    (const unsigned char *)0, /* 1a00..1aff */
    (const unsigned char *)0, /* 1b00..1bff */
    (const unsigned char *)0, /* 1c00..1cff */
    (const unsigned char *)0, /* 1d00..1dff */
    (const unsigned char *)0, /* 1e00..1eff */
    UCD_Lt_1f00_1fff,
};
static const unsigned char * const * const UCD_Lt[16]={
    UCD_Lt_0000_0fff,
    UCD_Lt_1000_1fff,
};

int ucdIsLt( int sym ) {
    return ucdInClass( sym,  UCD_Lt );
}
