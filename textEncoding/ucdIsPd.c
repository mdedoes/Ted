/***** Pd *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Pd_0000_00ff[32]= {0,0,0,0,0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pd_0500_05ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pd_0000_0fff[16]= {
    UCD_Pd_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    UCD_Pd_0500_05ff,
};
static const unsigned char UCD_Pd_1800_18ff[32]= {0x40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pd_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    (const unsigned char *)0, /* 1300..13ff */
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    (const unsigned char *)0, /* 1700..17ff */
    UCD_Pd_1800_18ff,
};
static const unsigned char UCD_Pd_2000_20ff[32]= {0,0,0x3f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pd_2e00_2eff[32]= {0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pd_2000_2fff[16]= {
    UCD_Pd_2000_20ff,
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
    (const unsigned char *)0, /* 2d00..2dff */
    UCD_Pd_2e00_2eff,
};
static const unsigned char UCD_Pd_3000_30ff[32]= {0,0,0,0x10,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pd_3000_3fff[16]= {
    UCD_Pd_3000_30ff,
};
static const unsigned char UCD_Pd_fe00_feff[32]= {0,0,0,0,0,0,0x06,0,0,0,0,0x01,0x08,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Pd_ff00_ffff[32]= {0,0x20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_Pd_f000_ffff[16]= {
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
    UCD_Pd_fe00_feff,
    UCD_Pd_ff00_ffff,
};
static const unsigned char * const * const UCD_Pd[16]={
    UCD_Pd_0000_0fff,
    UCD_Pd_1000_1fff,
    UCD_Pd_2000_2fff,
    UCD_Pd_3000_3fff,
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
    UCD_Pd_f000_ffff,
};

int ucdIsPd( int sym ) {
    return ucdInClass( sym,  UCD_Pd );
}
