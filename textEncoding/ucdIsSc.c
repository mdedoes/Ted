/***** Sc *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Sc_0000_00ff[32]= {0,0,0,0,0x10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3c,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sc_0600_06ff[32]= {0,0x08,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sc_0900_09ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0c,0,};
static const unsigned char UCD_Sc_0a00_0aff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,0,};
static const unsigned char UCD_Sc_0b00_0bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,};
static const unsigned char UCD_Sc_0e00_0eff[32]= {0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Sc_0000_0fff[16]= {
    UCD_Sc_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    UCD_Sc_0600_06ff,
    (const unsigned char *)0, /* 0700..07ff */
    (const unsigned char *)0, /* 0800..08ff */
    UCD_Sc_0900_09ff,
    UCD_Sc_0a00_0aff,
    UCD_Sc_0b00_0bff,
    (const unsigned char *)0, /* 0c00..0cff */
    (const unsigned char *)0, /* 0d00..0dff */
    UCD_Sc_0e00_0eff,
};
static const unsigned char UCD_Sc_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x08,0,0,0,0,};
static const unsigned char * const UCD_Sc_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Sc_1700_17ff,
};
static const unsigned char UCD_Sc_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0x3f,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Sc_2000_2fff[16]= {
    UCD_Sc_2000_20ff,
};
static const unsigned char UCD_Sc_fd00_fdff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10,};
static const unsigned char UCD_Sc_fe00_feff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sc_ff00_ffff[32]= {0x10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x63,0,0,0,};
static const unsigned char * const UCD_Sc_f000_ffff[16]= {
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
    UCD_Sc_fd00_fdff,
    UCD_Sc_fe00_feff,
    UCD_Sc_ff00_ffff,
};
static const unsigned char * const * const UCD_Sc[16]={
    UCD_Sc_0000_0fff,
    UCD_Sc_1000_1fff,
    UCD_Sc_2000_2fff,
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
    UCD_Sc_f000_ffff,
};

int ucdIsSc( int sym ) {
    return ucdInClass( sym,  UCD_Sc );
}
