/***** Pe *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Pe_0000_00ff[32]= {0,0,0,0,0,0x02,0,0,0,0,0,0x20,0,0,0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pe_0f00_0fff[32]= {0,0,0,0,0,0,0,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pe_0000_0fff[16]= {
    UCD_Pe_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    (const unsigned char *)0, /* 0600..06ff */
    (const unsigned char *)0, /* 0700..07ff */
    (const unsigned char *)0, /* 0800..08ff */
    (const unsigned char *)0, /* 0900..09ff */
    (const unsigned char *)0, /* 0a00..0aff */
    (const unsigned char *)0, /* 0b00..0bff */
    (const unsigned char *)0, /* 0c00..0cff */
    (const unsigned char *)0, /* 0d00..0dff */
    (const unsigned char *)0, /* 0e00..0eff */
    UCD_Pe_0f00_0fff,
};
static const unsigned char UCD_Pe_1600_16ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pe_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    UCD_Pe_1600_16ff,
};
static const unsigned char UCD_Pe_2000_20ff[32]= {0,0,0,0,0,0,0,0,0x40,0,0,0,0,0,0,0x40,0,0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pe_2300_23ff[32]= {0,0,0,0,0,0x04,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pe_2700_27ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0xaa,0x2a,0,0,0,0,0,0,0,0,0,0x40,0,0,0,0x80,0x0a,0,0,};
static const unsigned char UCD_Pe_2900_29ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x50,0x55,0x55,0x01,0,0,0,0,0,0,0,0x0a,0,0,0,0x20,};
static const unsigned char * const UCD_Pe_2000_2fff[16]= {
    UCD_Pe_2000_20ff,
    (const unsigned char *)0, /* 2100..21ff */
    (const unsigned char *)0, /* 2200..22ff */
    UCD_Pe_2300_23ff,
    (const unsigned char *)0, /* 2400..24ff */
    (const unsigned char *)0, /* 2500..25ff */
    (const unsigned char *)0, /* 2600..26ff */
    UCD_Pe_2700_27ff,
    (const unsigned char *)0, /* 2800..28ff */
    UCD_Pe_2900_29ff,
};
static const unsigned char UCD_Pe_3000_30ff[32]= {0,0xaa,0xa2,0xca,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pe_3000_3fff[16]= {
    UCD_Pe_3000_30ff,
};
static const unsigned char UCD_Pe_fd00_fdff[32]= {0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pe_fe00_feff[32]= {0,0,0,0x01,0,0,0x40,0x55,0x15,0x01,0,0x54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pe_ff00_ffff[32]= {0,0x02,0,0,0,0,0,0x20,0,0,0,0x20,0x09,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pe_f000_ffff[16]= {
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
    UCD_Pe_fd00_fdff,
    UCD_Pe_fe00_feff,
    UCD_Pe_ff00_ffff,
};
static const unsigned char * const * const UCD_Pe[16]={
    UCD_Pe_0000_0fff,
    UCD_Pe_1000_1fff,
    UCD_Pe_2000_2fff,
    UCD_Pe_3000_3fff,
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
    UCD_Pe_f000_ffff,
};

int ucdIsPe( int sym ) {
    return ucdInClass( sym,  UCD_Pe );
}
