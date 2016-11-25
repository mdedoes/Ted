/***** Lu *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Lu_0000_00ff[32]= {0,0,0,0,0,0,0,0,0xfe,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0x7f,0x7f,0,0,0,0,};
static const unsigned char UCD_Lu_0100_01ff[32]= {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0xaa,0xaa,0x54,0x55,0x55,0x55,0x55,0x55,0x2b,0xd6,0xce,0xdb,0xb1,0xd5,0xd2,0xae,0x11,0x90,0xa4,0xaa,0x4a,0x55,0x55,0xd2,0x55,};
static const unsigned char UCD_Lu_0200_02ff[32]= {0x55,0x55,0x55,0x55,0x55,0x55,0x05,0x6c,0x7a,0x55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lu_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0xd7,0xfe,0xff,0xfb,0x0f,0,0,0,0,0x1c,0x55,0x55,0x55,0x90,0xe6,};
static const unsigned char UCD_Lu_0400_04ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0x55,0x55,0x55,0x55,0x01,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0xab,0x2a,0x55,0x55,0x55,0x55,0x55,0x55,};
static const unsigned char UCD_Lu_0500_05ff[32]= {0x55,0x55,0x05,0,0,0,0xfe,0xff,0xff,0xff,0x7f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lu_0000_0fff[16]= {
    UCD_Lu_0000_00ff,
    UCD_Lu_0100_01ff,
    UCD_Lu_0200_02ff,
    UCD_Lu_0300_03ff,
    UCD_Lu_0400_04ff,
    UCD_Lu_0500_05ff,
};
static const unsigned char UCD_Lu_1000_10ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,0x3f,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lu_1e00_1eff[32]= {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x15,0,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x01,};
static const unsigned char UCD_Lu_1f00_1fff[32]= {0,0xff,0,0x3f,0,0xff,0,0xff,0,0x3f,0,0xaa,0,0xff,0,0,0,0,0,0,0,0,0,0x0f,0,0x0f,0,0x0f,0,0x1f,0,0x0f,};
static const unsigned char * const UCD_Lu_1000_1fff[16]= {
    UCD_Lu_1000_10ff,
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
    UCD_Lu_1e00_1eff,
    UCD_Lu_1f00_1fff,
};
static const unsigned char UCD_Lu_2100_21ff[32]= {0x84,0x38,0x27,0x3e,0x50,0x3d,0x0f,0xc0,0x20,0,0,0,0,0,0,0,0x08,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lu_2c00_2cff[32]= {0xff,0xff,0xff,0xff,0xff,0x7f,0,0,0,0,0,0,0x9d,0x0a,0x20,0,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x05,0,0,0,};
static const unsigned char * const UCD_Lu_2000_2fff[16]= {
    (const unsigned char *)0, /* 2000..20ff */
    UCD_Lu_2100_21ff,
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
    UCD_Lu_2c00_2cff,
};
static const unsigned char UCD_Lu_ff00_ffff[32]= {0,0,0,0,0xfe,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lu_f000_ffff[16]= {
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
    UCD_Lu_ff00_ffff,
};
static const unsigned char * const * const UCD_Lu[16]={
    UCD_Lu_0000_0fff,
    UCD_Lu_1000_1fff,
    UCD_Lu_2000_2fff,
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
    UCD_Lu_f000_ffff,
};

int ucdIsLu( int sym ) {
    return ucdInClass( sym,  UCD_Lu );
}
