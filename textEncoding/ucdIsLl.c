/***** Ll *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Ll_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xfe,0xff,0xff,0x07,0,0,0,0,0,0x04,0x20,0x04,0,0,0,0x80,0xff,0xff,0x7f,0xff,};
static const unsigned char UCD_Ll_0100_01ff[32]= {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x55,0x55,0xab,0xaa,0xaa,0xaa,0xaa,0xaa,0xd4,0x29,0x31,0x24,0x4e,0x2a,0x2d,0x51,0xe6,0x40,0x52,0x55,0xb5,0xaa,0xaa,0x29,0xaa,};
static const unsigned char UCD_Ll_0200_02ff[32]= {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xfa,0x93,0x85,0xaa,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Ll_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x38,0,0,0x01,0,0,0xf0,0xff,0xff,0xff,0x7f,0xe3,0xaa,0xaa,0xaa,0x2f,0x19,};
static const unsigned char UCD_Ll_0400_04ff[32]= {0,0,0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,0xaa,0xaa,0xaa,0xaa,0x02,0xa8,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x54,0xd5,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,};
static const unsigned char UCD_Ll_0500_05ff[32]= {0xaa,0xaa,0x0a,0,0,0,0,0,0,0,0,0,0xfe,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Ll_0000_0fff[16]= {
    UCD_Ll_0000_00ff,
    UCD_Ll_0100_01ff,
    UCD_Ll_0200_02ff,
    UCD_Ll_0300_03ff,
    UCD_Ll_0400_04ff,
    UCD_Ll_0500_05ff,
};
static const unsigned char UCD_Ll_1d00_1dff[32]= {0xff,0xff,0xff,0xff,0xff,0x0f,0,0,0,0,0,0,0xfc,0xff,0xff,0xfe,0xff,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Ll_1e00_1eff[32]= {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xea,0x0f,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x02,};
static const unsigned char UCD_Ll_1f00_1fff[32]= {0xff,0,0x3f,0,0xff,0,0xff,0,0x3f,0,0xff,0,0xff,0,0xff,0x3f,0xff,0,0xff,0,0xff,0,0xdf,0x40,0xdc,0,0xcf,0,0xff,0,0xdc,0,};
static const unsigned char * const UCD_Ll_1000_1fff[16]= {
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
    UCD_Ll_1d00_1dff,
    UCD_Ll_1e00_1eff,
    UCD_Ll_1f00_1fff,
};
static const unsigned char UCD_Ll_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Ll_2100_21ff[32]= {0,0xc4,0x08,0,0,0x80,0x10,0x32,0xc0,0x43,0,0,0,0,0,0,0x10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Ll_2c00_2cff[32]= {0,0,0,0,0,0,0xff,0xff,0xff,0xff,0xff,0x7f,0x62,0x15,0xd0,0,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x1a,0,0,0,};
static const unsigned char UCD_Ll_2d00_2dff[32]= {0xff,0xff,0xff,0xff,0x3f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Ll_2000_2fff[16]= {
    UCD_Ll_2000_20ff,
    UCD_Ll_2100_21ff,
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
    UCD_Ll_2c00_2cff,
    UCD_Ll_2d00_2dff,
};
static const unsigned char UCD_Ll_fb00_fbff[32]= {0x7f,0,0xf8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Ll_ff00_ffff[32]= {0,0,0,0,0,0,0,0,0xfe,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Ll_f000_ffff[16]= {
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
    UCD_Ll_fb00_fbff,
    (const unsigned char *)0, /* fc00..fcff */
    (const unsigned char *)0, /* fd00..fdff */
    (const unsigned char *)0, /* fe00..feff */
    UCD_Ll_ff00_ffff,
};
static const unsigned char * const * const UCD_Ll[16]={
    UCD_Ll_0000_0fff,
    UCD_Ll_1000_1fff,
    UCD_Ll_2000_2fff,
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
    UCD_Ll_f000_ffff,
};

int ucdIsLl( int sym ) {
    return ucdInClass( sym,  UCD_Ll );
}
