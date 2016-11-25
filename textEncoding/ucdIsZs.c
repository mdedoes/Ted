/***** Zs *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Zs_0000_00ff[32]= {0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Zs_0000_0fff[16]= {
    UCD_Zs_0000_00ff,
};
static const unsigned char UCD_Zs_1600_16ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Zs_1800_18ff[32]= {0,0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Zs_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    UCD_Zs_1600_16ff,
    (const unsigned char *)0, /* 1700..17ff */
    UCD_Zs_1800_18ff,
};
static const unsigned char UCD_Zs_2000_20ff[32]= {0xff,0x07,0,0,0,0x80,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Zs_2000_2fff[16]= {
    UCD_Zs_2000_20ff,
};
static const unsigned char UCD_Zs_3000_30ff[32]= {0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Zs_3000_3fff[16]= {
    UCD_Zs_3000_30ff,
};
static const unsigned char * const * const UCD_Zs[16]={
    UCD_Zs_0000_0fff,
    UCD_Zs_1000_1fff,
    UCD_Zs_2000_2fff,
    UCD_Zs_3000_3fff,
};

int ucdIsZs( int sym ) {
    return ucdInClass( sym,  UCD_Zs );
}
