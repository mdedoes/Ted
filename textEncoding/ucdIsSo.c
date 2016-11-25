/***** So *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_So_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0x42,0x41,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_0400_04ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_0600_06ff[32]= {0,0xc0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x02,0,0x60,};
static const unsigned char UCD_So_0700_07ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x40,0,};
static const unsigned char UCD_So_0900_09ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x04,};
static const unsigned char UCD_So_0b00_0bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf8,0x05,};
static const unsigned char UCD_So_0c00_0cff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x06,0,};
static const unsigned char UCD_So_0f00_0fff[32]= {0x0e,0,0xf8,0xfc,0,0,0x50,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xbf,0x9f,0,0,0,0,0,0,};
static const unsigned char * const UCD_So_0000_0fff[16]= {
    UCD_So_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    UCD_So_0400_04ff,
    (const unsigned char *)0, /* 0500..05ff */
    UCD_So_0600_06ff,
    UCD_So_0700_07ff,
    (const unsigned char *)0, /* 0800..08ff */
    UCD_So_0900_09ff,
    (const unsigned char *)0, /* 0a00..0aff */
    UCD_So_0b00_0bff,
    UCD_So_0c00_0cff,
    (const unsigned char *)0, /* 0d00..0dff */
    (const unsigned char *)0, /* 0e00..0eff */
    UCD_So_0f00_0fff,
};
static const unsigned char UCD_So_1300_13ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_1900_19ff[32]= {0,0,0,0,0,0,0,0,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,};
static const unsigned char UCD_So_1b00_1bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xfe,0x07,0xf0,0x1f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_So_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    UCD_So_1300_13ff,
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    (const unsigned char *)0, /* 1700..17ff */
    (const unsigned char *)0, /* 1800..18ff */
    UCD_So_1900_19ff,
    (const unsigned char *)0, /* 1a00..1aff */
    UCD_So_1b00_1bff,
};
static const unsigned char UCD_So_2100_21ff[32]= {0x7b,0x03,0xd0,0xc1,0xaf,0x42,0,0x0c,0,0x34,0,0,0,0,0,0,0,0,0xe0,0xf3,0xb6,0xbf,0xff,0xff,0xff,0x3f,0xeb,0xff,0xff,0xff,0x0f,0,};
static const unsigned char UCD_So_2300_23ff[32]= {0xff,0xf0,0xff,0xff,0xfc,0xf9,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xff,0xff,0xff,0x07,0,0,0xf0,0xff,0xff,0xff,0xff,0x0f,0xfc,0,0,0,};
static const unsigned char UCD_So_2400_24ff[32]= {0xff,0xff,0xff,0xff,0x7f,0,0,0,0xff,0x07,0,0,0,0,0,0,0,0,0,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03,0,0,};
static const unsigned char UCD_So_2500_25ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0,};
static const unsigned char UCD_So_2600_26ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0x1f,0xff,0xff,0x07,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_2700_27ff[32]= {0xde,0xf3,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xaf,0x47,0x7f,0xfe,0,0,0,0,0,0x10,0xff,0xff,0xff,0xfe,0x7f,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_2800_28ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
static const unsigned char UCD_So_2b00_2bff[32]= {0xff,0xff,0xff,0x07,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_2c00_2cff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xe0,0x07,0,0,};
static const unsigned char UCD_So_2e00_2eff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0,};
static const unsigned char UCD_So_2f00_2fff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0,0,0,0xff,0x0f,};
static const unsigned char * const UCD_So_2000_2fff[16]= {
    (const unsigned char *)0, /* 2000..20ff */
    UCD_So_2100_21ff,
    (const unsigned char *)0, /* 2200..22ff */
    UCD_So_2300_23ff,
    UCD_So_2400_24ff,
    UCD_So_2500_25ff,
    UCD_So_2600_26ff,
    UCD_So_2700_27ff,
    UCD_So_2800_28ff,
    (const unsigned char *)0, /* 2900..29ff */
    (const unsigned char *)0, /* 2a00..2aff */
    UCD_So_2b00_2bff,
    UCD_So_2c00_2cff,
    (const unsigned char *)0, /* 2d00..2dff */
    UCD_So_2e00_2eff,
    UCD_So_2f00_2fff,
};
static const unsigned char UCD_So_3000_30ff[32]= {0x10,0,0x0c,0,0x01,0,0xc0,0xc0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_3100_31ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc3,0xff,0,0,0,0,0xff,0xff,0,0,0,0,0,0,};
static const unsigned char UCD_So_3200_32ff[32]= {0xff,0xff,0xff,0x7f,0,0xfc,0xff,0xff,0x0f,0,0x01,0,0xff,0xff,0xff,0xff,0,0xfc,0xff,0xff,0xff,0xff,0x01,0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,};
static const unsigned char UCD_So_3300_33ff[32]= {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
static const unsigned char * const UCD_So_3000_3fff[16]= {
    UCD_So_3000_30ff,
    UCD_So_3100_31ff,
    UCD_So_3200_32ff,
    UCD_So_3300_33ff,
};
static const unsigned char UCD_So_4d00_4dff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
static const unsigned char * const UCD_So_4000_4fff[16]= {
    (const unsigned char *)0, /* 4000..40ff */
    (const unsigned char *)0, /* 4100..41ff */
    (const unsigned char *)0, /* 4200..42ff */
    (const unsigned char *)0, /* 4300..43ff */
    (const unsigned char *)0, /* 4400..44ff */
    (const unsigned char *)0, /* 4500..45ff */
    (const unsigned char *)0, /* 4600..46ff */
    (const unsigned char *)0, /* 4700..47ff */
    (const unsigned char *)0, /* 4800..48ff */
    (const unsigned char *)0, /* 4900..49ff */
    (const unsigned char *)0, /* 4a00..4aff */
    (const unsigned char *)0, /* 4b00..4bff */
    (const unsigned char *)0, /* 4c00..4cff */
    UCD_So_4d00_4dff,
};
static const unsigned char UCD_So_a400_a4ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0,0,0,0,0,0,0,};
static const unsigned char UCD_So_a800_a8ff[32]= {0,0,0,0,0,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_So_a000_afff[16]= {
    (const unsigned char *)0, /* a000..a0ff */
    (const unsigned char *)0, /* a100..a1ff */
    (const unsigned char *)0, /* a200..a2ff */
    (const unsigned char *)0, /* a300..a3ff */
    UCD_So_a400_a4ff,
    (const unsigned char *)0, /* a500..a5ff */
    (const unsigned char *)0, /* a600..a6ff */
    (const unsigned char *)0, /* a700..a7ff */
    UCD_So_a800_a8ff,
};
static const unsigned char UCD_So_fd00_fdff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,};
static const unsigned char UCD_So_ff00_ffff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10,0x61,0,0x30,};
static const unsigned char * const UCD_So_f000_ffff[16]= {
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
    UCD_So_fd00_fdff,
    (const unsigned char *)0, /* fe00..feff */
    UCD_So_ff00_ffff,
};
static const unsigned char * const * const UCD_So[16]={
    UCD_So_0000_0fff,
    UCD_So_1000_1fff,
    UCD_So_2000_2fff,
    UCD_So_3000_3fff,
    UCD_So_4000_4fff,
    (const unsigned char **)0, /* 5000..5fff */
    (const unsigned char **)0, /* 6000..6fff */
    (const unsigned char **)0, /* 7000..7fff */
    (const unsigned char **)0, /* 8000..8fff */
    (const unsigned char **)0, /* 9000..9fff */
    UCD_So_a000_afff,
    (const unsigned char **)0, /* b000..bfff */
    (const unsigned char **)0, /* c000..cfff */
    (const unsigned char **)0, /* d000..dfff */
    (const unsigned char **)0, /* e000..efff */
    UCD_So_f000_ffff,
};

int ucdIsSo( int sym ) {
    return ucdInClass( sym,  UCD_So );
}
