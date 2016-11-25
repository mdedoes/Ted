/***** Pf *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Pf_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x08,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pf_0000_0fff[16]= {
    UCD_Pf_0000_00ff,
};
static const unsigned char UCD_Pf_2000_20ff[32]= {0,0,0,0x22,0,0,0,0x04,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pf_2e00_2eff[32]= {0x28,0x24,0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pf_2000_2fff[16]= {
    UCD_Pf_2000_20ff,
    (const unsigned char *)0, /* 2100..21ff */
    (const unsigned char *)0, /* 2200..22ff */
    (const unsigned char *)0, /* 2300..23ff */
    (const unsigned char *)0, /* 2400..24ff */
    (const unsigned char *)0, /* 2500..25ff */
    (const unsigned char *)0, /* 2600..26ff */
    (const unsigned char *)0, /* 2700..27ff */
    (const unsigned char *)0, /* 2800..28ff */
    (const unsigned char *)0, /* 2900..29ff */
    (const unsigned char *)0, /* 2a00..2aff */
    (const unsigned char *)0, /* 2b00..2bff */
    (const unsigned char *)0, /* 2c00..2cff */
    (const unsigned char *)0, /* 2d00..2dff */
    UCD_Pf_2e00_2eff,
};
static const unsigned char * const * const UCD_Pf[16]={
    UCD_Pf_0000_0fff,
    (const unsigned char **)0, /* 1000..1fff */
    UCD_Pf_2000_2fff,
};

int ucdIsPf( int sym ) {
    return ucdInClass( sym,  UCD_Pf );
}
