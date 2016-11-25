/***** Mc *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Mc_0900_09ff[32]= {0x08,0,0,0,0,0,0,0xc0,0x01,0x1e,0,0,0,0,0,0,0x0c,0,0,0,0,0,0,0xc0,0x81,0x19,0x80,0,0,0,0,0,};
static const unsigned char UCD_Mc_0a00_0aff[32]= {0x08,0,0,0,0,0,0,0xc0,0x01,0,0,0,0,0,0,0,0x08,0,0,0,0,0,0,0xc0,0x01,0x1a,0,0,0,0,0,0,};
static const unsigned char UCD_Mc_0b00_0bff[32]= {0x0c,0,0,0,0,0,0,0x40,0x81,0x19,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xc6,0x1d,0x80,0,0,0,0,0,};
static const unsigned char UCD_Mc_0c00_0cff[32]= {0x0e,0,0,0,0,0,0,0,0x1e,0,0,0,0,0,0,0,0x0c,0,0,0,0,0,0,0x40,0x9f,0x0d,0x60,0,0,0,0,0,};
static const unsigned char UCD_Mc_0d00_0dff[32]= {0x0c,0,0,0,0,0,0,0xc0,0xc1,0x1d,0x80,0,0,0,0,0,0x0c,0,0,0,0,0,0,0,0,0x80,0x03,0xff,0,0,0x0c,0,};
static const unsigned char UCD_Mc_0f00_0fff[32]= {0,0,0,0,0,0,0,0xc0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mc_0000_0fff[16]= {
    (const unsigned char *)0, /* 0000..00ff */
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    (const unsigned char *)0, /* 0600..06ff */
    (const unsigned char *)0, /* 0700..07ff */
    (const unsigned char *)0, /* 0800..08ff */
    UCD_Mc_0900_09ff,
    UCD_Mc_0a00_0aff,
    UCD_Mc_0b00_0bff,
    UCD_Mc_0c00_0cff,
    UCD_Mc_0d00_0dff,
    (const unsigned char *)0, /* 0e00..0eff */
    UCD_Mc_0f00_0fff,
};
static const unsigned char UCD_Mc_1000_10ff[32]= {0,0,0,0,0,0x10,0x02,0x01,0,0,0xc0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mc_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0xc0,0xbf,0x01,0,0,0,0,0,0,};
static const unsigned char UCD_Mc_1900_19ff[32]= {0,0,0,0,0x78,0x0e,0xfb,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0x01,0x03,0,0,0,0,0,0,};
static const unsigned char UCD_Mc_1a00_1aff[32]= {0,0,0,0x0e,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Mc_1b00_1bff[32]= {0x10,0,0,0,0,0,0x20,0xe8,0x1b,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mc_1000_1fff[16]= {
    UCD_Mc_1000_10ff,
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Mc_1700_17ff,
    (const unsigned char *)0, /* 1800..18ff */
    UCD_Mc_1900_19ff,
    UCD_Mc_1a00_1aff,
    UCD_Mc_1b00_1bff,
};
static const unsigned char UCD_Mc_a800_a8ff[32]= {0x04,0,0,0,0x98,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Mc_a000_afff[16]= {
    (const unsigned char *)0, /* a000..a0ff */
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    (const unsigned char *)0, /* a400..a4ff */
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    (const unsigned char *)0, /* a700..a7ff */
    UCD_Mc_a800_a8ff,
};
static const unsigned char * const * const UCD_Mc[16]={
    UCD_Mc_0000_0fff,
    UCD_Mc_1000_1fff,
    (const unsigned char **)0, /* 2000..2fff */
    (const unsigned char **)0, /* 3000..3fff */
    (const unsigned char **)0, /* 4000..4fff */
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_Mc_a000_afff,
};

int ucdIsMc( int sym ) {
    return ucdInClass( sym,  UCD_Mc );
}
