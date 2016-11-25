/***** Nd *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Nd_0000_00ff[32]= {0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Nd_0600_06ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,};
static const unsigned char UCD_Nd_0700_07ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,};
static const unsigned char UCD_Nd_0900_09ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,};
static const unsigned char UCD_Nd_0a00_0aff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,};
static const unsigned char UCD_Nd_0b00_0bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,};
static const unsigned char UCD_Nd_0c00_0cff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,};
static const unsigned char UCD_Nd_0d00_0dff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Nd_0e00_0eff[32]= {0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,};
static const unsigned char UCD_Nd_0f00_0fff[32]= {0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Nd_0000_0fff[16]= {
    UCD_Nd_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    UCD_Nd_0600_06ff,
    UCD_Nd_0700_07ff,
    (const unsigned char *)0, /* 0800..08ff */
    UCD_Nd_0900_09ff,
    UCD_Nd_0a00_0aff,
    UCD_Nd_0b00_0bff,
    UCD_Nd_0c00_0cff,
    UCD_Nd_0d00_0dff,
    UCD_Nd_0e00_0eff,
    UCD_Nd_0f00_0fff,
};
static const unsigned char UCD_Nd_1000_10ff[32]= {0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Nd_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,};
static const unsigned char UCD_Nd_1800_18ff[32]= {0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Nd_1900_19ff[32]= {0,0,0,0,0,0,0,0,0xc0,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,};
static const unsigned char UCD_Nd_1b00_1bff[32]= {0,0,0,0,0,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Nd_1000_1fff[16]= {
    UCD_Nd_1000_10ff,
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Nd_1700_17ff,
    UCD_Nd_1800_18ff,
    UCD_Nd_1900_19ff,
    (const unsigned char *)0, /* 1a00..1aff */
    UCD_Nd_1b00_1bff,
};
static const unsigned char UCD_Nd_ff00_ffff[32]= {0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Nd_f000_ffff[16]= {
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
    UCD_Nd_ff00_ffff,
};
static const unsigned char * const * const UCD_Nd[16]={
    UCD_Nd_0000_0fff,
    UCD_Nd_1000_1fff,
    (const unsigned char **)0, /* 2000..2fff */
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
    UCD_Nd_f000_ffff,
};

int ucdIsNd( int sym ) {
    return ucdInClass( sym,  UCD_Nd );
}
