/***** No *****/
#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned char UCD_No_0000_00ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x0c,0x72,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_0900_09ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf0,0x03,};
static const unsigned char UCD_No_0b00_0bff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x07,0,};
static const unsigned char UCD_No_0f00_0fff[32]= {0,0,0,0,0,0xfc,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_No_0000_0fff[16]= {
    UCD_No_0000_00ff,
    (const unsigned char *)0, /* 0100..01ff */
    (const unsigned char *)0, /* 0200..02ff */
    (const unsigned char *)0, /* 0300..03ff */
    (const unsigned char *)0, /* 0400..04ff */
    (const unsigned char *)0, /* 0500..05ff */
    (const unsigned char *)0, /* 0600..06ff */
    (const unsigned char *)0, /* 0700..07ff */
    (const unsigned char *)0, /* 0800..08ff */
    UCD_No_0900_09ff,
    (const unsigned char *)0, /* 0a00..0aff */
    UCD_No_0b00_0bff,
    (const unsigned char *)0, /* 0c00..0cff */
    (const unsigned char *)0, /* 0d00..0dff */
    (const unsigned char *)0, /* 0e00..0eff */
    UCD_No_0f00_0fff,
};
static const unsigned char UCD_No_1300_13ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0xfe,0xff,0x1f,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_1700_17ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0x03,};
static const unsigned char * const UCD_No_1000_1fff[16]= {
    (const unsigned char *)0, /* 1000..10ff */
    (const unsigned char *)0, /* 1100..11ff */
    (const unsigned char *)0, /* 1200..12ff */
    UCD_No_1300_13ff,
    (const unsigned char *)0, /* 1400..14ff */
    (const unsigned char *)0, /* 1500..15ff */
    (const unsigned char *)0, /* 1600..16ff */
    UCD_No_1700_17ff,
};
static const unsigned char UCD_No_2000_20ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xf1,0x03,0xff,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_2100_21ff[32]= {0,0,0,0,0,0,0,0,0,0,0xf8,0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_2400_24ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0,0,0,0,0,0,0,0,0,0xfc,0xff,0xff,};
static const unsigned char UCD_No_2700_27ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xc0,0xff,0xff,0xff,0x0f,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_2c00_2cff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x20,};
static const unsigned char * const UCD_No_2000_2fff[16]= {
    UCD_No_2000_20ff,
    UCD_No_2100_21ff,
    (const unsigned char *)0, /* 2200..22ff */
    (const unsigned char *)0, /* 2300..23ff */
    UCD_No_2400_24ff,
    (const unsigned char *)0, /* 2500..25ff */
    (const unsigned char *)0, /* 2600..26ff */
    UCD_No_2700_27ff,
    (const unsigned char *)0, /* 2800..28ff */
    (const unsigned char *)0, /* 2900..29ff */
    (const unsigned char *)0, /* 2a00..2aff */
    (const unsigned char *)0, /* 2b00..2bff */
    UCD_No_2c00_2cff,
};
static const unsigned char UCD_No_3100_31ff[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x3c,0,0,0,0,0,0,0,0,0,0,0,0,0,};
static const unsigned char UCD_No_3200_32ff[32]= {0,0,0,0,0xff,0x03,0,0,0,0,0xfe,0xff,0,0,0,0,0xff,0x03,0,0,0,0,0xfe,0xff,0,0,0,0,0,0,0,0,};
static const unsigned char * const UCD_No_3000_3fff[16]= {
    (const unsigned char *)0, /* 3000..30ff */
    UCD_No_3100_31ff,
    UCD_No_3200_32ff,
};
static const unsigned char * const * const UCD_No[16]={
    UCD_No_0000_0fff,
    UCD_No_1000_1fff,
    UCD_No_2000_2fff,
    UCD_No_3000_3fff,
};

int ucdIsNo( int sym ) {
    return ucdInClass( sym,  UCD_No );
}
