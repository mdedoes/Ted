/***** Sm *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_Sm_0000_00ff[32]= {0,0,0,0,0,0x08,0,0x70,0,0,0,0,0,0,0,0x50,0,0,0,0,0,0x10,0x02,0,0,0,0x80,0,0,0,0x80,0,};
static const unsigned char UCD_Sm_0300_03ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0,};
static const unsigned char * const UCD_Sm_0000_0fff[16]= {
    UCD_Sm_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    UCD_Sm_0300_03ff,
};
static const unsigned char UCD_Sm_2000_20ff[32]= {0,0,0,0,0,0,0,0,0x10,0,0x04,0,0,0,0,0x1c,0,0x1c,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sm_2100_21ff[32]= {0,0,0,0,0,0,0,0,0x1f,0x08,0,0,0,0,0,0,0,0,0x1f,0x0c,0x49,0x40,0,0,0,0xc0,0x14,0,0,0,0xf0,0xff,};
static const unsigned char UCD_Sm_2200_22ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
static const unsigned char UCD_Sm_2300_23ff[32]= {0,0x0f,0,0,0x03,0,0,0,0,0,0,0,0,0,0,0x10,0,0,0,0xf8,0xff,0xff,0x0f,0,0,0,0,0xf0,0x03,0,0,0,};
static const unsigned char UCD_Sm_2500_25ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,0,0x02,0,0,0,0,0,0,0xff,};
static const unsigned char UCD_Sm_2600_26ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sm_2700_27ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x9f,0x07,0xff,0xff,0x3f,0,0xff,0xff,};
static const unsigned char UCD_Sm_2900_29ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0,0,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,0xff,0xff,0xcf,};
static const unsigned char UCD_Sm_2a00_2aff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
static const unsigned char * const UCD_Sm_2000_2fff[16]= {
    UCD_Sm_2000_20ff,
    UCD_Sm_2100_21ff,
    UCD_Sm_2200_22ff,
    UCD_Sm_2300_23ff,
    (const unsigned char *)0, /* 2400..24ff */
    UCD_Sm_2500_25ff,
    UCD_Sm_2600_26ff,
    UCD_Sm_2700_27ff,
    (const unsigned char *)0, /* 2800..28ff */
    UCD_Sm_2900_29ff,
    UCD_Sm_2a00_2aff,
};
static const unsigned char UCD_Sm_fb00_fbff[32]= {0,0,0,0,0,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sm_fe00_feff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0x74,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_Sm_ff00_ffff[32]= {0,0x08,0,0x70,0,0,0,0,0,0,0,0x50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,0x1e,0,0,};
static const unsigned char * const UCD_Sm_f000_ffff[16]= {
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
    UCD_Sm_fb00_fbff,
    (const unsigned char *)0, /* fc00..fcff */
    (const unsigned char *)0, /* fd00..fdff */
    UCD_Sm_fe00_feff,
    UCD_Sm_ff00_ffff,
};
static const unsigned char * const * const UCD_Sm[16]={
    UCD_Sm_0000_0fff,
    (const unsigned char **)0, /* 1000..1fff */
    UCD_Sm_2000_2fff,
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
    UCD_Sm_f000_ffff,
};

int ucdIsSm( int sym ) {
    return ucdInClass( sym,  UCD_Sm );
}
