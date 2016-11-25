/***** Lm *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Lm_0200_02ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xc3,0xff,0x03,0,0x1f,0x40,0,0,};
static const unsigned char UCD_Lm_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lm_0500_05ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lm_0600_06ff[32]= {0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x60,0,0,0,};
static const unsigned char UCD_Lm_0700_07ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x30,0x04,};
static const unsigned char UCD_Lm_0e00_0eff[32]= {0,0,0,0,0,0,0,0,0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lm_0000_0fff[16]= {
    (const unsigned char *)0, /* 0000..00ff */
    (const unsigned char *)0, /* 0100..01ff */
    UCD_Lm_0200_02ff,
    UCD_Lm_0300_03ff,
    (const unsigned char *)0, /* 0400..04ff */
    UCD_Lm_0500_05ff,
    UCD_Lm_0600_06ff,
    UCD_Lm_0700_07ff,
    (const unsigned char *)0, /* 0800..08ff */
    (const unsigned char *)0, /* 0900..09ff */
    (const unsigned char *)0, /* 0a00..0aff */
    (const unsigned char *)0, /* 0b00..0bff */
    (const unsigned char *)0, /* 0c00..0cff */
    (const unsigned char *)0, /* 0d00..0dff */
    UCD_Lm_0e00_0eff,
};
static const unsigned char UCD_Lm_1000_10ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10,};
static const unsigned char UCD_Lm_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,};
static const unsigned char UCD_Lm_1800_18ff[32]= {0,0,0,0,0,0,0,0,0x08,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lm_1d00_1dff[32]= {0,0,0,0,0,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0,0,0x01,0,0,0,0xf8,0xff,0xff,0xff,0xff,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lm_1000_1fff[16]= {
    UCD_Lm_1000_10ff,
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Lm_1700_17ff,
    UCD_Lm_1800_18ff,
    (const unsigned char *)0, /* 1900..19ff */
    (const unsigned char *)0, /* 1a00..1aff */
    (const unsigned char *)0, /* 1b00..1bff */
    (const unsigned char *)0, /* 1c00..1cff */
    UCD_Lm_1d00_1dff,
};
static const unsigned char UCD_Lm_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x1f,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lm_2d00_2dff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lm_2000_2fff[16]= {
    UCD_Lm_2000_20ff,
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
    UCD_Lm_2d00_2dff,
};
static const unsigned char UCD_Lm_3000_30ff[32]= {0x20,0,0,0,0,0,0x3e,0x08,0,0,0,0,0,0,0,0,0,0,0,0x60,0,0,0,0,0,0,0,0,0,0,0,0x70,};
static const unsigned char * const UCD_Lm_3000_3fff[16]= {
    UCD_Lm_3000_30ff,
};
static const unsigned char UCD_Lm_a000_a0ff[32]= {0,0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Lm_a700_a7ff[32]= {0,0,0x80,0x07,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lm_a000_afff[16]= {
    UCD_Lm_a000_a0ff,
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    (const unsigned char *)0, /* a400..a4ff */
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    UCD_Lm_a700_a7ff,
};
static const unsigned char UCD_Lm_ff00_ffff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0xc0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Lm_f000_ffff[16]= {
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
    UCD_Lm_ff00_ffff,
};
static const unsigned char * const * const UCD_Lm[16]={
    UCD_Lm_0000_0fff,
    UCD_Lm_1000_1fff,
    UCD_Lm_2000_2fff,
    UCD_Lm_3000_3fff,
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_Lm_a000_afff,
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_Lm_f000_ffff,
};

int ucdIsLm( int sym ) {
    return ucdInClass( sym,  UCD_Lm );
}
