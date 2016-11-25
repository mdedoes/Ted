/***** Me *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Me_0400_04ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Me_0600_06ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0,0,0,0,};
static const unsigned char * const UCD_Me_0000_0fff[16]= {
    (const unsigned char *)0, /* 0000..00ff */
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    UCD_Me_0400_04ff,
    (const unsigned char *)0, /* 0500..05ff */
    UCD_Me_0600_06ff,
};
static const unsigned char UCD_Me_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xe0,0x1d,0,0,0,};
static const unsigned char * const UCD_Me_2000_2fff[16]= {
    UCD_Me_2000_20ff,
};
static const unsigned char * const * const UCD_Me[16]={
    UCD_Me_0000_0fff,
    (const unsigned char **)0, /* 1000..1fff */
    UCD_Me_2000_2fff,
};

int ucdIsMe( int sym ) {
    return ucdInClass( sym,  UCD_Me );
}
