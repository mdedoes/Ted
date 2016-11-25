/***** Pc *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Pc_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pc_0000_0fff[16]= {
    UCD_Pc_0000_00ff,
};
static const unsigned char UCD_Pc_2000_20ff[32]= {0,0,0,0,0,0,0,0x80,0x01,0,0x10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pc_2000_2fff[16]= {
    UCD_Pc_2000_20ff,
};
static const unsigned char UCD_Pc_fe00_feff[32]= {0,0,0,0,0,0,0x18,0,0,0xe0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pc_ff00_ffff[32]= {0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pc_f000_ffff[16]= {
    (const unsigned char *)0, /* f000..f0ff */
    (const unsigned char *)0, /* f100..f1ff */
    (const unsigned char *)0, /* f200..f2ff */
    (const unsigned char *)0, /* f300..f3ff */
    (const unsigned char *)0, /* f400..f4ff */
    (const unsigned char *)0, /* f500..f5ff */
    (const unsigned char *)0, /* f600..f6ff */
    (const unsigned char *)0, /* f700..f7ff */
    (const unsigned char *)0, /* f800..f8ff */
    (const unsigned char *)0, /* f900..f9ff */
    (const unsigned char *)0, /* fa00..faff */
    (const unsigned char *)0, /* fb00..fbff */
    (const unsigned char *)0, /* fc00..fcff */
    (const unsigned char *)0, /* fd00..fdff */
    UCD_Pc_fe00_feff,
    UCD_Pc_ff00_ffff,
};
static const unsigned char * const * const UCD_Pc[16]={
    UCD_Pc_0000_0fff,
    (const unsigned char **)0, /* 1000..1fff */
    UCD_Pc_2000_2fff,
    (const unsigned char **)0, /* 3000..3fff */
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    (const unsigned char **)0, /* a000..afff */
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_Pc_f000_ffff,
};

int ucdIsPc( int sym ) {
    return ucdInClass( sym,  UCD_Pc );
}
