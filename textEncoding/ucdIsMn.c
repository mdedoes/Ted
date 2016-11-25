/***** Mn *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Mn_0300_03ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_0400_04ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x78,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_0500_05ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xfe,0xff,0xff,0xff,0xff,0xbf,0xb6,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_0600_06ff[32]= {0,0,0x3f,0,0,0,0,0,0,0xf8,0xff,0x7f,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0xc0,0x9f,0x9f,0x3d,0,0,};
static const unsigned char UCD_Mn_0700_07ff[32]= {0,0,0x02,0,0,0,0xff,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0x01,0,0,0,0,0,0,0xf8,0x0f,0,};
static const unsigned char UCD_Mn_0900_09ff[32]= {0x06,0,0,0,0,0,0,0x10,0xfe,0x21,0x1e,0,0x0c,0,0,0,0x02,0,0,0,0,0,0,0x10,0x1e,0x20,0,0,0x0c,0,0,0,};
static const unsigned char UCD_Mn_0a00_0aff[32]= {0x06,0,0,0,0,0,0,0x10,0x86,0x39,0,0,0,0,0x03,0,0x06,0,0,0,0,0,0,0x10,0xbe,0x21,0,0,0x0c,0,0,0,};
static const unsigned char UCD_Mn_0b00_0bff[32]= {0x02,0,0,0,0,0,0,0x90,0x0e,0x20,0x40,0,0,0,0,0,0x04,0,0,0,0,0,0,0,0x01,0x20,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_0c00_0cff[32]= {0,0,0,0,0,0,0,0xc0,0xc1,0x3d,0x60,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x40,0x30,0,0,0x0c,0,0,0,};
static const unsigned char UCD_Mn_0d00_0dff[32]= {0,0,0,0,0,0,0,0,0x0e,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,0x5c,0,0,0,0,0,};
static const unsigned char UCD_Mn_0e00_0eff[32]= {0,0,0,0,0,0,0xf2,0x07,0x80,0x7f,0,0,0,0,0,0,0,0,0,0,0,0,0xf2,0x1b,0,0x3f,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_0f00_0fff[32]= {0,0,0,0x03,0,0,0xa0,0x02,0,0,0,0,0,0,0xfe,0x7f,0xdf,0,0xff,0xfe,0xff,0xff,0xff,0x1f,0x40,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mn_0000_0fff[16]= {
    (const unsigned char *)0, /* 0000..00ff */
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    UCD_Mn_0300_03ff,
    UCD_Mn_0400_04ff,
    UCD_Mn_0500_05ff,
    UCD_Mn_0600_06ff,
    UCD_Mn_0700_07ff,
    (const unsigned char *)0, /* 0800..08ff */
    UCD_Mn_0900_09ff,
    UCD_Mn_0a00_0aff,
    UCD_Mn_0b00_0bff,
    UCD_Mn_0c00_0cff,
    UCD_Mn_0d00_0dff,
    UCD_Mn_0e00_0eff,
    UCD_Mn_0f00_0fff,
};
static const unsigned char UCD_Mn_1000_10ff[32]= {0,0,0,0,0,0xe0,0xc5,0x02,0,0,0,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1300_13ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1700_17ff[32]= {0,0,0x1c,0,0,0,0x1c,0,0,0,0x0c,0,0,0,0x0c,0,0,0,0,0,0,0,0x80,0x3f,0x40,0xfe,0x0f,0x20,0,0,0,0,};
static const unsigned char UCD_Mn_1800_18ff[32]= {0,0x38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1900_19ff[32]= {0,0,0,0,0x87,0x01,0x04,0x0e,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1a00_1aff[32]= {0,0,0x80,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1b00_1bff[32]= {0x0f,0,0,0,0,0,0xd0,0x17,0x04,0,0,0,0,0xf8,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_1d00_1dff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x07,0,0,0,0,0,0xc0,};
static const unsigned char * const UCD_Mn_1000_1fff[16]= {
    UCD_Mn_1000_10ff,
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    UCD_Mn_1300_13ff,
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Mn_1700_17ff,
    UCD_Mn_1800_18ff,
    UCD_Mn_1900_19ff,
    UCD_Mn_1a00_1aff,
    UCD_Mn_1b00_1bff,
    (const unsigned char *)0, /* 1c00..1cff */
    UCD_Mn_1d00_1dff,
};
static const unsigned char UCD_Mn_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x1f,0xe2,0xff,0,0,};
static const unsigned char * const UCD_Mn_2000_2fff[16]= {
    UCD_Mn_2000_20ff,
};
static const unsigned char UCD_Mn_3000_30ff[32]= {0,0,0,0,0,0xfc,0,0,0,0,0,0,0,0,0,0,0,0,0,0x06,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mn_3000_3fff[16]= {
    UCD_Mn_3000_30ff,
};
static const unsigned char UCD_Mn_a800_a8ff[32]= {0x40,0x08,0,0,0x60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mn_a000_afff[16]= {
    (const unsigned char *)0, /* a000..a0ff */
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    (const unsigned char *)0, /* a400..a4ff */
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    (const unsigned char *)0, /* a700..a7ff */
    UCD_Mn_a800_a8ff,
};
static const unsigned char UCD_Mn_fb00_fbff[32]= {0,0,0,0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mn_fe00_feff[32]= {0xff,0xff,0,0,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mn_f000_ffff[16]= {
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
    UCD_Mn_fb00_fbff,
    (const unsigned char *)0, /* fc00..fcff */
    (const unsigned char *)0, /* fd00..fdff */
    UCD_Mn_fe00_feff,
};
static const unsigned char * const * const UCD_Mn[16]={
    UCD_Mn_0000_0fff,
    UCD_Mn_1000_1fff,
    UCD_Mn_2000_2fff,
    UCD_Mn_3000_3fff,
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_Mn_a000_afff,
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_Mn_f000_ffff,
};

int ucdIsMn( int sym ) {
    return ucdInClass( sym,  UCD_Mn );
}
