/***** Cf *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Cf_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Cf_0600_06ff[32]= {0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,0,0,0,0,};
static const unsigned char UCD_Cf_0700_07ff[32]= {0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Cf_0000_0fff[16]= {
    UCD_Cf_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    UCD_Cf_0600_06ff,
    UCD_Cf_0700_07ff,
};
static const unsigned char UCD_Cf_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x30,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Cf_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_Cf_1700_17ff,
};
static const unsigned char UCD_Cf_2000_20ff[32]= {0,0xf8,0,0,0,0x7c,0,0,0,0,0,0,0x0f,0xfc,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Cf_2000_2fff[16]= {
    UCD_Cf_2000_20ff,
};
static const unsigned char UCD_Cf_fe00_feff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,};
static const unsigned char UCD_Cf_ff00_ffff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0e,};
static const unsigned char * const UCD_Cf_f000_ffff[16]= {
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
    UCD_Cf_fe00_feff,
    UCD_Cf_ff00_ffff,
};
static const unsigned char * const * const UCD_Cf[16]={
    UCD_Cf_0000_0fff,
    UCD_Cf_1000_1fff,
    UCD_Cf_2000_2fff,
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
    UCD_Cf_f000_ffff,
};

int ucdIsCf( int sym ) {
    return ucdInClass( sym,  UCD_Cf );
}
