/***** Sk *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Sk_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0x40,0x01,0,0,0,0,0,0,0,0,0x81,0x10,0x01,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sk_0200_02ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3c,0,0xfc,0xff,0xe0,0xbf,0xff,0xff,};
static const unsigned char UCD_Sk_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x30,0,0x30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Sk_0000_0fff[16]= {
    UCD_Sk_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    UCD_Sk_0200_02ff,
    UCD_Sk_0300_03ff,
};
static const unsigned char UCD_Sk_1f00_1fff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xa0,0x03,0xe0,0,0xe0,0,0xe0,0,0x60,};
static const unsigned char * const UCD_Sk_1000_1fff[16]= {
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
    UCD_Sk_1f00_1fff,
};
static const unsigned char UCD_Sk_3000_30ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x18,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Sk_3000_3fff[16]= {
    UCD_Sk_3000_30ff,
};
static const unsigned char UCD_Sk_a700_a7ff[32]= {0xff,0xff,0x7f,0,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Sk_a000_afff[16]= {
    (const unsigned char *)0, /* a000..a0ff */
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    (const unsigned char *)0, /* a400..a4ff */
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    UCD_Sk_a700_a7ff,
};
static const unsigned char UCD_Sk_ff00_ffff[32]= {0,0,0,0,0,0,0,0x40,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x08,0,0,0,};
static const unsigned char * const UCD_Sk_f000_ffff[16]= {
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
    (const unsigned char *)0, /* fe00..feff */
    UCD_Sk_ff00_ffff,
};
static const unsigned char * const * const UCD_Sk[16]={
    UCD_Sk_0000_0fff,
    UCD_Sk_1000_1fff,
    (const unsigned char **)0, /* 2000..2fff */
    UCD_Sk_3000_3fff,
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_Sk_a000_afff,
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_Sk_f000_ffff,
};

int ucdIsSk( int sym ) {
    return ucdInClass( sym,  UCD_Sk );
}
