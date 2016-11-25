/***** Nl *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Nl_1600_16ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0x01,0,};
static const unsigned char * const UCD_Nl_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    UCD_Nl_1600_16ff,
};
static const unsigned char UCD_Nl_2100_21ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Nl_2000_2fff[16]= {
    (const unsigned char *)0, /* 2000..20ff */
    UCD_Nl_2100_21ff,
};
static const unsigned char UCD_Nl_3000_30ff[32]= {0x80,0,0,0,0xfe,0x03,0,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Nl_3000_3fff[16]= {
    UCD_Nl_3000_30ff,
};
static const unsigned char * const * const UCD_Nl[16]={
    (const unsigned char **)0, /* 0000..0fff */
    UCD_Nl_1000_1fff,
    UCD_Nl_2000_2fff,
    UCD_Nl_3000_3fff,
};

int ucdIsNl( int sym ) {
    return ucdInClass( sym,  UCD_Nl );
}
