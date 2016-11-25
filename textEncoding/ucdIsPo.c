/***** Po *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Po_0000_00ff[32]= {0,0,0,0,0xee,0xd4,0,0x8c,0x01,0,0,0x10,0,0,0,0,0,0,0,0,0x02,0,0x80,0x80,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_0500_05ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0xfc,0,0,0,0,0,0x02,0,0,0,0,0,0x40,0x49,0,0,0,0,0,0x18,0,};
static const unsigned char UCD_Po_0600_06ff[32]= {0,0x30,0,0xc8,0,0,0,0,0,0,0,0,0,0x3c,0,0,0,0,0,0,0,0,0,0,0,0,0x10,0,0,0,0,0,};
static const unsigned char UCD_Po_0700_07ff[32]= {0xff,0x3f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,0x03,};
static const unsigned char UCD_Po_0900_09ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0x30,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_0d00_0dff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10,0,};
static const unsigned char UCD_Po_0e00_0eff[32]= {0,0,0,0,0,0,0,0,0,0x80,0,0x0c,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_0f00_0fff[32]= {0xf0,0xff,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,0,0,0,0,0,0,0,0,0,0x03,0,0,0,0,0,};
static const unsigned char * const UCD_Po_0000_0fff[16]= {
    UCD_Po_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    UCD_Po_0300_03ff,
    (const unsigned char *)0, /* 0400..04ff */
    UCD_Po_0500_05ff,
    UCD_Po_0600_06ff,
    UCD_Po_0700_07ff,
    (const unsigned char *)0, /* 0800..08ff */
    UCD_Po_0900_09ff,
    (const unsigned char *)0, /* 0a00..0aff */
    (const unsigned char *)0, /* 0b00..0bff */
    (const unsigned char *)0, /* 0c00..0cff */
    UCD_Po_0d00_0dff,
    UCD_Po_0e00_0eff,
    UCD_Po_0f00_0fff,
};
static const unsigned char UCD_Po_1000_10ff[32]= {0,0,0,0,0,0,0,0,0,0xfc,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x08,};
static const unsigned char UCD_Po_1300_13ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xfe,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_1600_16ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0x60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x38,0,0,};
static const unsigned char UCD_Po_1700_17ff[32]= {0,0,0,0,0,0,0x60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x70,0x07,0,0,0,0,};
static const unsigned char UCD_Po_1800_18ff[32]= {0xbf,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_1900_19ff[32]= {0,0,0,0,0,0,0,0,0x30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0,0,0,0,};
static const unsigned char UCD_Po_1a00_1aff[32]= {0,0,0,0xc0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_1b00_1bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0xfc,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Po_1000_1fff[16]= {
    UCD_Po_1000_10ff,
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    UCD_Po_1300_13ff,
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    UCD_Po_1600_16ff,
    UCD_Po_1700_17ff,
    UCD_Po_1800_18ff,
    UCD_Po_1900_19ff,
    UCD_Po_1a00_1aff,
    UCD_Po_1b00_1bff,
};
static const unsigned char UCD_Po_2000_20ff[32]= {0,0,0xc0,0,0xff,0,0xff,0x79,0x8e,0xff,0xeb,0x7f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_2c00_2cff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xde,};
static const unsigned char UCD_Po_2e00_2eff[32]= {0xc3,0xc9,0x7f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Po_2000_2fff[16]= {
    UCD_Po_2000_20ff,
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
    UCD_Po_2c00_2cff,
    (const unsigned char *)0, /* 2d00..2dff */
    UCD_Po_2e00_2eff,
};
static const unsigned char UCD_Po_3000_30ff[32]= {0x0e,0,0,0,0,0,0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x08,};
static const unsigned char * const UCD_Po_3000_3fff[16]= {
    UCD_Po_3000_30ff,
};
static const unsigned char UCD_Po_a800_a8ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Po_a000_afff[16]= {
    (const unsigned char *)0, /* a000..a0ff */
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    (const unsigned char *)0, /* a400..a4ff */
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    (const unsigned char *)0, /* a700..a7ff */
    UCD_Po_a800_a8ff,
};
static const unsigned char UCD_Po_fe00_feff[32]= {0,0,0x7f,0x02,0,0,0x01,0,0x60,0x1e,0xf7,0x80,0x03,0x0d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Po_ff00_ffff[32]= {0xee,0xd4,0,0x8c,0x01,0,0,0x10,0,0,0,0,0x32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Po_f000_ffff[16]= {
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
    UCD_Po_fe00_feff,
    UCD_Po_ff00_ffff,
};
static const unsigned char * const * const UCD_Po[16]={
    UCD_Po_0000_0fff,
    UCD_Po_1000_1fff,
    UCD_Po_2000_2fff,
    UCD_Po_3000_3fff,
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_Po_a000_afff,
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_Po_f000_ffff,
};

int ucdIsPo( int sym ) {
    return ucdInClass( sym,  UCD_Po );
}
