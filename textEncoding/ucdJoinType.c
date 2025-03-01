#   include <stdio.h>

#   include "ucdIntern.h"
#   include "ucdJoinType.h"

#   define C	UCDjoin_C
#   define D	UCDjoin_D
#   define L	UCDjoin_L
#   define R	UCDjoin_R
#   define T	UCDjoin_T
#   define U	UCDjoin_U

static const unsigned char    UCD_JT_0000_000f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_00a0_00af[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_0000_00ff[16]= {
    UCD_JT_0000_000f,
    UCD_JT_0000_000f, /*  UCD_JT_0010_001f  */
    UCD_JT_0000_000f, /*  UCD_JT_0020_002f  */
    UCD_JT_0000_000f, /*  UCD_JT_0030_003f  */
    UCD_JT_0000_000f, /*  UCD_JT_0040_004f  */
    UCD_JT_0000_000f, /*  UCD_JT_0050_005f  */
    UCD_JT_0000_000f, /*  UCD_JT_0060_006f  */
    UCD_JT_0000_000f, /*  UCD_JT_0070_007f  */
    UCD_JT_0000_000f, /*  UCD_JT_0080_008f  */
    UCD_JT_0000_000f, /*  UCD_JT_0090_009f  */
    UCD_JT_00a0_00af,
    UCD_JT_0000_000f, /*  UCD_JT_00b0_00bf  */
    UCD_JT_0000_000f, /*  UCD_JT_00c0_00cf  */
    UCD_JT_0000_000f, /*  UCD_JT_00d0_00df  */
    UCD_JT_0000_000f, /*  UCD_JT_00e0_00ef  */
    UCD_JT_0000_000f, /*  UCD_JT_00f0_00ff  */
};
static const unsigned char * const UCD_JT_0100_01ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0100_010f  */
    UCD_JT_0000_000f, /*  UCD_JT_0110_011f  */
    UCD_JT_0000_000f, /*  UCD_JT_0120_012f  */
    UCD_JT_0000_000f, /*  UCD_JT_0130_013f  */
    UCD_JT_0000_000f, /*  UCD_JT_0140_014f  */
    UCD_JT_0000_000f, /*  UCD_JT_0150_015f  */
    UCD_JT_0000_000f, /*  UCD_JT_0160_016f  */
    UCD_JT_0000_000f, /*  UCD_JT_0170_017f  */
    UCD_JT_0000_000f, /*  UCD_JT_0180_018f  */
    UCD_JT_0000_000f, /*  UCD_JT_0190_019f  */
    UCD_JT_0000_000f, /*  UCD_JT_01a0_01af  */
    UCD_JT_0000_000f, /*  UCD_JT_01b0_01bf  */
    UCD_JT_0000_000f, /*  UCD_JT_01c0_01cf  */
    UCD_JT_0000_000f, /*  UCD_JT_01d0_01df  */
    UCD_JT_0000_000f, /*  UCD_JT_01e0_01ef  */
    UCD_JT_0000_000f, /*  UCD_JT_01f0_01ff  */
};
static const unsigned char    UCD_JT_0300_030f[16]= {T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char * const UCD_JT_0300_03ff[16]= {
    UCD_JT_0300_030f,
    UCD_JT_0300_030f, /*  UCD_JT_0310_031f  */
    UCD_JT_0300_030f, /*  UCD_JT_0320_032f  */
    UCD_JT_0300_030f, /*  UCD_JT_0330_033f  */
    UCD_JT_0300_030f, /*  UCD_JT_0340_034f  */
    UCD_JT_0300_030f, /*  UCD_JT_0350_035f  */
    UCD_JT_0300_030f, /*  UCD_JT_0360_036f  */
    UCD_JT_0000_000f, /*  UCD_JT_0370_037f  */
    UCD_JT_0000_000f, /*  UCD_JT_0380_038f  */
    UCD_JT_0000_000f, /*  UCD_JT_0390_039f  */
    UCD_JT_0000_000f, /*  UCD_JT_03a0_03af  */
    UCD_JT_0000_000f, /*  UCD_JT_03b0_03bf  */
    UCD_JT_0000_000f, /*  UCD_JT_03c0_03cf  */
    UCD_JT_0000_000f, /*  UCD_JT_03d0_03df  */
    UCD_JT_0000_000f, /*  UCD_JT_03e0_03ef  */
    UCD_JT_0000_000f, /*  UCD_JT_03f0_03ff  */
};
static const unsigned char    UCD_JT_0480_048f[16]= {U,U,U,T,T,T,T,T,T,T,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_0400_04ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0400_040f  */
    UCD_JT_0000_000f, /*  UCD_JT_0410_041f  */
    UCD_JT_0000_000f, /*  UCD_JT_0420_042f  */
    UCD_JT_0000_000f, /*  UCD_JT_0430_043f  */
    UCD_JT_0000_000f, /*  UCD_JT_0440_044f  */
    UCD_JT_0000_000f, /*  UCD_JT_0450_045f  */
    UCD_JT_0000_000f, /*  UCD_JT_0460_046f  */
    UCD_JT_0000_000f, /*  UCD_JT_0470_047f  */
    UCD_JT_0480_048f,
    UCD_JT_0000_000f, /*  UCD_JT_0490_049f  */
    UCD_JT_0000_000f, /*  UCD_JT_04a0_04af  */
    UCD_JT_0000_000f, /*  UCD_JT_04b0_04bf  */
    UCD_JT_0000_000f, /*  UCD_JT_04c0_04cf  */
    UCD_JT_0000_000f, /*  UCD_JT_04d0_04df  */
    UCD_JT_0000_000f, /*  UCD_JT_04e0_04ef  */
    UCD_JT_0000_000f, /*  UCD_JT_04f0_04ff  */
};
static const unsigned char    UCD_JT_0590_059f[16]= {U,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_05b0_05bf[16]= {T,T,T,T,T,T,T,T,T,T,T,T,T,T,U,T,};
static const unsigned char    UCD_JT_05c0_05cf[16]= {U,T,T,U,T,T,U,T,U,U,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_0500_05ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0500_050f  */
    UCD_JT_0000_000f, /*  UCD_JT_0510_051f  */
    UCD_JT_0000_000f, /*  UCD_JT_0520_052f  */
    UCD_JT_0000_000f, /*  UCD_JT_0530_053f  */
    UCD_JT_0000_000f, /*  UCD_JT_0540_054f  */
    UCD_JT_0000_000f, /*  UCD_JT_0550_055f  */
    UCD_JT_0000_000f, /*  UCD_JT_0560_056f  */
    UCD_JT_0000_000f, /*  UCD_JT_0570_057f  */
    UCD_JT_0000_000f, /*  UCD_JT_0580_058f  */
    UCD_JT_0590_059f,
    UCD_JT_0300_030f, /*  UCD_JT_05a0_05af  */
    UCD_JT_05b0_05bf,
    UCD_JT_05c0_05cf,
    UCD_JT_0000_000f, /*  UCD_JT_05d0_05df  */
    UCD_JT_0000_000f, /*  UCD_JT_05e0_05ef  */
    UCD_JT_0000_000f, /*  UCD_JT_05f0_05ff  */
};
static const unsigned char    UCD_JT_0610_061f[16]= {T,T,T,T,T,T,T,T,T,T,T,U,T,U,U,U,};
static const unsigned char    UCD_JT_0620_062f[16]= {D,U,R,R,R,R,D,R,D,R,D,D,D,D,D,R,};
static const unsigned char    UCD_JT_0630_063f[16]= {R,R,R,D,D,D,D,D,D,D,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_0640_064f[16]= {C,D,D,D,D,D,D,D,R,D,D,T,T,T,T,T,};
static const unsigned char    UCD_JT_0660_066f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,U,D,D,};
static const unsigned char    UCD_JT_0670_067f[16]= {T,R,R,R,U,R,R,R,D,D,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_0680_068f[16]= {D,D,D,D,D,D,D,D,R,R,R,R,R,R,R,R,};
static const unsigned char    UCD_JT_0690_069f[16]= {R,R,R,R,R,R,R,R,R,R,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_06a0_06af[16]= {D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_06c0_06cf[16]= {R,D,D,R,R,R,R,R,R,R,R,R,D,R,D,R,};
static const unsigned char    UCD_JT_06d0_06df[16]= {D,D,R,R,U,R,T,T,T,T,T,T,T,U,U,T,};
static const unsigned char    UCD_JT_06e0_06ef[16]= {T,T,T,T,T,U,U,T,T,U,T,T,T,T,R,R,};
static const unsigned char    UCD_JT_06f0_06ff[16]= {U,U,U,U,U,U,U,U,U,U,D,D,D,U,U,D,};
static const unsigned char * const UCD_JT_0600_06ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0600_060f  */
    UCD_JT_0610_061f,
    UCD_JT_0620_062f,
    UCD_JT_0630_063f,
    UCD_JT_0640_064f,
    UCD_JT_0300_030f, /*  UCD_JT_0650_065f  */
    UCD_JT_0660_066f,
    UCD_JT_0670_067f,
    UCD_JT_0680_068f,
    UCD_JT_0690_069f,
    UCD_JT_06a0_06af,
    UCD_JT_06a0_06af, /*  UCD_JT_06b0_06bf  */
    UCD_JT_06c0_06cf,
    UCD_JT_06d0_06df,
    UCD_JT_06e0_06ef,
    UCD_JT_06f0_06ff,
};
static const unsigned char    UCD_JT_0700_070f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,T,};
static const unsigned char    UCD_JT_0710_071f[16]= {R,T,D,D,D,R,R,R,R,R,D,D,D,D,R,D,};
static const unsigned char    UCD_JT_0720_072f[16]= {D,D,D,D,D,D,D,D,R,D,R,D,R,D,D,R,};
static const unsigned char    UCD_JT_0740_074f[16]= {T,T,T,T,T,T,T,T,T,T,T,U,U,R,D,D,};
static const unsigned char    UCD_JT_0750_075f[16]= {D,D,D,D,D,D,D,D,D,R,R,R,D,D,D,D,};
static const unsigned char    UCD_JT_0760_076f[16]= {D,D,D,D,D,D,D,D,D,D,D,R,R,D,D,D,};
static const unsigned char    UCD_JT_0770_077f[16]= {D,R,D,R,R,D,D,D,R,R,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_07a0_07af[16]= {U,U,U,U,U,U,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_07b0_07bf[16]= {T,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_07c0_07cf[16]= {U,U,U,U,U,U,U,U,U,U,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_07e0_07ef[16]= {D,D,D,D,D,D,D,D,D,D,D,T,T,T,T,T,};
static const unsigned char    UCD_JT_07f0_07ff[16]= {T,T,T,T,U,U,U,U,U,U,C,U,U,U,U,U,};
static const unsigned char * const UCD_JT_0700_07ff[16]= {
    UCD_JT_0700_070f,
    UCD_JT_0710_071f,
    UCD_JT_0720_072f,
    UCD_JT_0300_030f, /*  UCD_JT_0730_073f  */
    UCD_JT_0740_074f,
    UCD_JT_0750_075f,
    UCD_JT_0760_076f,
    UCD_JT_0770_077f,
    UCD_JT_0000_000f, /*  UCD_JT_0780_078f  */
    UCD_JT_0000_000f, /*  UCD_JT_0790_079f  */
    UCD_JT_07a0_07af,
    UCD_JT_07b0_07bf,
    UCD_JT_07c0_07cf,
    UCD_JT_06a0_06af, /*  UCD_JT_07d0_07df  */
    UCD_JT_07e0_07ef,
    UCD_JT_07f0_07ff,
};
static const unsigned char    UCD_JT_0810_081f[16]= {U,U,U,U,U,U,T,T,T,T,U,T,T,T,T,T,};
static const unsigned char    UCD_JT_0820_082f[16]= {T,T,T,T,U,T,T,T,U,T,T,T,T,T,U,U,};
static const unsigned char    UCD_JT_0840_084f[16]= {R,D,D,D,D,D,R,R,D,R,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_0850_085f[16]= {D,D,D,D,R,D,U,U,U,T,T,T,U,U,U,U,};
static const unsigned char    UCD_JT_0860_086f[16]= {D,U,D,D,D,D,U,R,D,R,R,U,U,U,U,U,};
static const unsigned char    UCD_JT_08a0_08af[16]= {D,D,D,D,D,D,D,D,D,D,R,R,R,U,R,D,};
static const unsigned char    UCD_JT_08b0_08bf[16]= {D,R,R,D,D,U,D,D,D,R,D,D,D,D,U,U,};
static const unsigned char    UCD_JT_08d0_08df[16]= {U,U,U,U,T,T,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_08e0_08ef[16]= {T,T,U,T,T,T,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char * const UCD_JT_0800_08ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0800_080f  */
    UCD_JT_0810_081f,
    UCD_JT_0820_082f,
    UCD_JT_0000_000f, /*  UCD_JT_0830_083f  */
    UCD_JT_0840_084f,
    UCD_JT_0850_085f,
    UCD_JT_0860_086f,
    UCD_JT_0000_000f, /*  UCD_JT_0870_087f  */
    UCD_JT_0000_000f, /*  UCD_JT_0880_088f  */
    UCD_JT_0000_000f, /*  UCD_JT_0890_089f  */
    UCD_JT_08a0_08af,
    UCD_JT_08b0_08bf,
    UCD_JT_0000_000f, /*  UCD_JT_08c0_08cf  */
    UCD_JT_08d0_08df,
    UCD_JT_08e0_08ef,
    UCD_JT_0300_030f, /*  UCD_JT_08f0_08ff  */
};
static const unsigned char    UCD_JT_0900_090f[16]= {T,T,T,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0930_093f[16]= {U,U,U,U,U,U,U,U,U,U,T,U,T,U,U,U,};
static const unsigned char    UCD_JT_0940_094f[16]= {U,T,T,T,T,T,T,T,T,U,U,U,U,T,U,U,};
static const unsigned char    UCD_JT_0950_095f[16]= {U,T,T,T,T,T,T,T,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0960_096f[16]= {U,U,T,T,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0980_098f[16]= {U,T,U,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_09b0_09bf[16]= {U,U,U,U,U,U,U,U,U,U,U,U,T,U,U,U,};
static const unsigned char    UCD_JT_09c0_09cf[16]= {U,T,T,T,T,U,U,U,U,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_0900_09ff[16]= {
    UCD_JT_0900_090f,
    UCD_JT_0000_000f, /*  UCD_JT_0910_091f  */
    UCD_JT_0000_000f, /*  UCD_JT_0920_092f  */
    UCD_JT_0930_093f,
    UCD_JT_0940_094f,
    UCD_JT_0950_095f,
    UCD_JT_0960_096f,
    UCD_JT_0000_000f, /*  UCD_JT_0970_097f  */
    UCD_JT_0980_098f,
    UCD_JT_0000_000f, /*  UCD_JT_0990_099f  */
    UCD_JT_0000_000f, /*  UCD_JT_09a0_09af  */
    UCD_JT_09b0_09bf,
    UCD_JT_09c0_09cf,
    UCD_JT_0000_000f, /*  UCD_JT_09d0_09df  */
    UCD_JT_0960_096f, /*  UCD_JT_09e0_09ef  */
    UCD_JT_0000_000f, /*  UCD_JT_09f0_09ff  */
};
static const unsigned char    UCD_JT_0a00_0a0f[16]= {U,T,T,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0a40_0a4f[16]= {U,T,T,U,U,U,U,T,T,U,U,T,T,T,U,U,};
static const unsigned char    UCD_JT_0a70_0a7f[16]= {T,T,U,U,U,T,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0ac0_0acf[16]= {U,T,T,T,T,T,U,T,T,U,U,U,U,T,U,U,};
static const unsigned char    UCD_JT_0af0_0aff[16]= {U,U,U,U,U,U,U,U,U,U,T,T,T,T,T,T,};
static const unsigned char * const UCD_JT_0a00_0aff[16]= {
    UCD_JT_0a00_0a0f,
    UCD_JT_0000_000f, /*  UCD_JT_0a10_0a1f  */
    UCD_JT_0000_000f, /*  UCD_JT_0a20_0a2f  */
    UCD_JT_09b0_09bf, /*  UCD_JT_0a30_0a3f  */
    UCD_JT_0a40_0a4f,
    UCD_JT_0980_098f, /*  UCD_JT_0a50_0a5f  */
    UCD_JT_0000_000f, /*  UCD_JT_0a60_0a6f  */
    UCD_JT_0a70_0a7f,
    UCD_JT_0a00_0a0f, /*  UCD_JT_0a80_0a8f  */
    UCD_JT_0000_000f, /*  UCD_JT_0a90_0a9f  */
    UCD_JT_0000_000f, /*  UCD_JT_0aa0_0aaf  */
    UCD_JT_09b0_09bf, /*  UCD_JT_0ab0_0abf  */
    UCD_JT_0ac0_0acf,
    UCD_JT_0000_000f, /*  UCD_JT_0ad0_0adf  */
    UCD_JT_0960_096f, /*  UCD_JT_0ae0_0aef  */
    UCD_JT_0af0_0aff,
};
static const unsigned char    UCD_JT_0b30_0b3f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,T,U,U,T,};
static const unsigned char    UCD_JT_0b50_0b5f[16]= {U,U,U,U,U,U,T,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0b80_0b8f[16]= {U,U,T,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0bc0_0bcf[16]= {T,U,U,U,U,U,U,U,U,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_0b00_0bff[16]= {
    UCD_JT_0980_098f, /*  UCD_JT_0b00_0b0f  */
    UCD_JT_0000_000f, /*  UCD_JT_0b10_0b1f  */
    UCD_JT_0000_000f, /*  UCD_JT_0b20_0b2f  */
    UCD_JT_0b30_0b3f,
    UCD_JT_09c0_09cf, /*  UCD_JT_0b40_0b4f  */
    UCD_JT_0b50_0b5f,
    UCD_JT_0960_096f, /*  UCD_JT_0b60_0b6f  */
    UCD_JT_0000_000f, /*  UCD_JT_0b70_0b7f  */
    UCD_JT_0b80_0b8f,
    UCD_JT_0000_000f, /*  UCD_JT_0b90_0b9f  */
    UCD_JT_0000_000f, /*  UCD_JT_0ba0_0baf  */
    UCD_JT_0000_000f, /*  UCD_JT_0bb0_0bbf  */
    UCD_JT_0bc0_0bcf,
    UCD_JT_0000_000f, /*  UCD_JT_0bd0_0bdf  */
    UCD_JT_0000_000f, /*  UCD_JT_0be0_0bef  */
    UCD_JT_0000_000f, /*  UCD_JT_0bf0_0bff  */
};
static const unsigned char    UCD_JT_0c30_0c3f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,U,T,T,};
static const unsigned char    UCD_JT_0c40_0c4f[16]= {T,U,U,U,U,U,T,T,T,U,T,T,T,T,U,U,};
static const unsigned char    UCD_JT_0c50_0c5f[16]= {U,U,U,U,U,T,T,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0cc0_0ccf[16]= {U,U,U,U,U,U,T,U,U,U,U,U,T,T,U,U,};
static const unsigned char * const UCD_JT_0c00_0cff[16]= {
    UCD_JT_07b0_07bf, /*  UCD_JT_0c00_0c0f  */
    UCD_JT_0000_000f, /*  UCD_JT_0c10_0c1f  */
    UCD_JT_0000_000f, /*  UCD_JT_0c20_0c2f  */
    UCD_JT_0c30_0c3f,
    UCD_JT_0c40_0c4f,
    UCD_JT_0c50_0c5f,
    UCD_JT_0960_096f, /*  UCD_JT_0c60_0c6f  */
    UCD_JT_0000_000f, /*  UCD_JT_0c70_0c7f  */
    UCD_JT_0980_098f, /*  UCD_JT_0c80_0c8f  */
    UCD_JT_0000_000f, /*  UCD_JT_0c90_0c9f  */
    UCD_JT_0000_000f, /*  UCD_JT_0ca0_0caf  */
    UCD_JT_0b30_0b3f, /*  UCD_JT_0cb0_0cbf  */
    UCD_JT_0cc0_0ccf,
    UCD_JT_0000_000f, /*  UCD_JT_0cd0_0cdf  */
    UCD_JT_0960_096f, /*  UCD_JT_0ce0_0cef  */
    UCD_JT_0000_000f, /*  UCD_JT_0cf0_0cff  */
};
static const unsigned char    UCD_JT_0d00_0d0f[16]= {T,T,U,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0d30_0d3f[16]= {U,U,U,U,U,U,U,U,U,U,U,T,T,U,U,U,};
static const unsigned char    UCD_JT_0dc0_0dcf[16]= {U,U,U,U,U,U,U,U,U,U,T,U,U,U,U,U,};
static const unsigned char    UCD_JT_0dd0_0ddf[16]= {U,U,T,T,T,U,T,U,U,U,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_0d00_0dff[16]= {
    UCD_JT_0d00_0d0f,
    UCD_JT_0000_000f, /*  UCD_JT_0d10_0d1f  */
    UCD_JT_0000_000f, /*  UCD_JT_0d20_0d2f  */
    UCD_JT_0d30_0d3f,
    UCD_JT_09c0_09cf, /*  UCD_JT_0d40_0d4f  */
    UCD_JT_0000_000f, /*  UCD_JT_0d50_0d5f  */
    UCD_JT_0960_096f, /*  UCD_JT_0d60_0d6f  */
    UCD_JT_0000_000f, /*  UCD_JT_0d70_0d7f  */
    UCD_JT_0000_000f, /*  UCD_JT_0d80_0d8f  */
    UCD_JT_0000_000f, /*  UCD_JT_0d90_0d9f  */
    UCD_JT_0000_000f, /*  UCD_JT_0da0_0daf  */
    UCD_JT_0000_000f, /*  UCD_JT_0db0_0dbf  */
    UCD_JT_0dc0_0dcf,
    UCD_JT_0dd0_0ddf,
    UCD_JT_0000_000f, /*  UCD_JT_0de0_0def  */
    UCD_JT_0000_000f, /*  UCD_JT_0df0_0dff  */
};
static const unsigned char    UCD_JT_0e30_0e3f[16]= {U,T,U,U,T,T,T,T,T,T,T,U,U,U,U,U,};
static const unsigned char    UCD_JT_0e40_0e4f[16]= {U,U,U,U,U,U,U,T,T,T,T,T,T,T,T,U,};
static const unsigned char    UCD_JT_0eb0_0ebf[16]= {U,T,U,U,T,T,T,T,T,T,U,T,T,U,U,U,};
static const unsigned char    UCD_JT_0ec0_0ecf[16]= {U,U,U,U,U,U,U,U,T,T,T,T,T,T,U,U,};
static const unsigned char * const UCD_JT_0e00_0eff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0e00_0e0f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e10_0e1f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e20_0e2f  */
    UCD_JT_0e30_0e3f,
    UCD_JT_0e40_0e4f,
    UCD_JT_0000_000f, /*  UCD_JT_0e50_0e5f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e60_0e6f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e70_0e7f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e80_0e8f  */
    UCD_JT_0000_000f, /*  UCD_JT_0e90_0e9f  */
    UCD_JT_0000_000f, /*  UCD_JT_0ea0_0eaf  */
    UCD_JT_0eb0_0ebf,
    UCD_JT_0ec0_0ecf,
    UCD_JT_0000_000f, /*  UCD_JT_0ed0_0edf  */
    UCD_JT_0000_000f, /*  UCD_JT_0ee0_0eef  */
    UCD_JT_0000_000f, /*  UCD_JT_0ef0_0eff  */
};
static const unsigned char    UCD_JT_0f10_0f1f[16]= {U,U,U,U,U,U,U,U,T,T,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0f30_0f3f[16]= {U,U,U,U,U,T,U,T,U,T,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_0f70_0f7f[16]= {U,T,T,T,T,T,T,T,T,T,T,T,T,T,T,U,};
static const unsigned char    UCD_JT_0f80_0f8f[16]= {T,T,T,T,T,U,T,T,U,U,U,U,U,T,T,T,};
static const unsigned char    UCD_JT_0f90_0f9f[16]= {T,T,T,T,T,T,T,T,U,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_0fb0_0fbf[16]= {T,T,T,T,T,T,T,T,T,T,T,T,T,U,U,U,};
static const unsigned char * const UCD_JT_0f00_0fff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_0f00_0f0f  */
    UCD_JT_0f10_0f1f,
    UCD_JT_0000_000f, /*  UCD_JT_0f20_0f2f  */
    UCD_JT_0f30_0f3f,
    UCD_JT_0000_000f, /*  UCD_JT_0f40_0f4f  */
    UCD_JT_0000_000f, /*  UCD_JT_0f50_0f5f  */
    UCD_JT_0000_000f, /*  UCD_JT_0f60_0f6f  */
    UCD_JT_0f70_0f7f,
    UCD_JT_0f80_0f8f,
    UCD_JT_0f90_0f9f,
    UCD_JT_0300_030f, /*  UCD_JT_0fa0_0faf  */
    UCD_JT_0fb0_0fbf,
    UCD_JT_0b50_0b5f, /*  UCD_JT_0fc0_0fcf  */
    UCD_JT_0000_000f, /*  UCD_JT_0fd0_0fdf  */
    UCD_JT_0000_000f, /*  UCD_JT_0fe0_0fef  */
    UCD_JT_0000_000f, /*  UCD_JT_0ff0_0fff  */
};
static const unsigned char * const * const UCD_JT_0000_0fff[16]= {
    UCD_JT_0000_00ff,
    UCD_JT_0100_01ff,
    UCD_JT_0100_01ff, /*  UCD_JT_0200_02ff  */
    UCD_JT_0300_03ff,
    UCD_JT_0400_04ff,
    UCD_JT_0500_05ff,
    UCD_JT_0600_06ff,
    UCD_JT_0700_07ff,
    UCD_JT_0800_08ff,
    UCD_JT_0900_09ff,
    UCD_JT_0a00_0aff,
    UCD_JT_0b00_0bff,
    UCD_JT_0c00_0cff,
    UCD_JT_0d00_0dff,
    UCD_JT_0e00_0eff,
    UCD_JT_0f00_0fff,
};
static const unsigned char    UCD_JT_1020_102f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,T,T,T,};
static const unsigned char    UCD_JT_1030_103f[16]= {T,U,T,T,T,T,T,T,U,T,T,U,U,T,T,U,};
static const unsigned char    UCD_JT_1050_105f[16]= {U,U,U,U,U,U,U,U,T,T,U,U,U,U,T,T,};
static const unsigned char    UCD_JT_1070_107f[16]= {U,T,T,T,T,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_1080_108f[16]= {U,U,T,U,U,T,T,U,U,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_1000_10ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1000_100f  */
    UCD_JT_0000_000f, /*  UCD_JT_1010_101f  */
    UCD_JT_1020_102f,
    UCD_JT_1030_103f,
    UCD_JT_0000_000f, /*  UCD_JT_1040_104f  */
    UCD_JT_1050_105f,
    UCD_JT_07b0_07bf, /*  UCD_JT_1060_106f  */
    UCD_JT_1070_107f,
    UCD_JT_1080_108f,
    UCD_JT_00a0_00af, /*  UCD_JT_1090_109f  */
    UCD_JT_0000_000f, /*  UCD_JT_10a0_10af  */
    UCD_JT_0000_000f, /*  UCD_JT_10b0_10bf  */
    UCD_JT_0000_000f, /*  UCD_JT_10c0_10cf  */
    UCD_JT_0000_000f, /*  UCD_JT_10d0_10df  */
    UCD_JT_0000_000f, /*  UCD_JT_10e0_10ef  */
    UCD_JT_0000_000f, /*  UCD_JT_10f0_10ff  */
};
static const unsigned char * const UCD_JT_1300_13ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1300_130f  */
    UCD_JT_0000_000f, /*  UCD_JT_1310_131f  */
    UCD_JT_0000_000f, /*  UCD_JT_1320_132f  */
    UCD_JT_0000_000f, /*  UCD_JT_1330_133f  */
    UCD_JT_0000_000f, /*  UCD_JT_1340_134f  */
    UCD_JT_1020_102f, /*  UCD_JT_1350_135f  */
    UCD_JT_0000_000f, /*  UCD_JT_1360_136f  */
    UCD_JT_0000_000f, /*  UCD_JT_1370_137f  */
    UCD_JT_0000_000f, /*  UCD_JT_1380_138f  */
    UCD_JT_0000_000f, /*  UCD_JT_1390_139f  */
    UCD_JT_0000_000f, /*  UCD_JT_13a0_13af  */
    UCD_JT_0000_000f, /*  UCD_JT_13b0_13bf  */
    UCD_JT_0000_000f, /*  UCD_JT_13c0_13cf  */
    UCD_JT_0000_000f, /*  UCD_JT_13d0_13df  */
    UCD_JT_0000_000f, /*  UCD_JT_13e0_13ef  */
    UCD_JT_0000_000f, /*  UCD_JT_13f0_13ff  */
};
static const unsigned char    UCD_JT_1710_171f[16]= {U,U,T,T,T,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_17b0_17bf[16]= {U,U,U,U,T,T,U,T,T,T,T,T,T,T,U,U,};
static const unsigned char    UCD_JT_17c0_17cf[16]= {U,U,U,U,U,U,T,U,U,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_17d0_17df[16]= {T,T,T,T,U,U,U,U,U,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_1700_17ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1700_170f  */
    UCD_JT_1710_171f,
    UCD_JT_0000_000f, /*  UCD_JT_1720_172f  */
    UCD_JT_1710_171f, /*  UCD_JT_1730_173f  */
    UCD_JT_0000_000f, /*  UCD_JT_1740_174f  */
    UCD_JT_0960_096f, /*  UCD_JT_1750_175f  */
    UCD_JT_0000_000f, /*  UCD_JT_1760_176f  */
    UCD_JT_0960_096f, /*  UCD_JT_1770_177f  */
    UCD_JT_0000_000f, /*  UCD_JT_1780_178f  */
    UCD_JT_0000_000f, /*  UCD_JT_1790_179f  */
    UCD_JT_0000_000f, /*  UCD_JT_17a0_17af  */
    UCD_JT_17b0_17bf,
    UCD_JT_17c0_17cf,
    UCD_JT_17d0_17df,
    UCD_JT_0000_000f, /*  UCD_JT_17e0_17ef  */
    UCD_JT_0000_000f, /*  UCD_JT_17f0_17ff  */
};
static const unsigned char    UCD_JT_1800_180f[16]= {U,U,U,U,U,U,U,D,U,U,C,T,T,T,U,U,};
static const unsigned char    UCD_JT_1870_187f[16]= {D,D,D,D,D,D,D,D,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_1880_188f[16]= {U,U,U,U,U,T,T,D,D,D,D,D,D,D,D,D,};
static const unsigned char    UCD_JT_18a0_18af[16]= {D,D,D,D,D,D,D,D,D,T,D,U,U,U,U,U,};
static const unsigned char * const UCD_JT_1800_18ff[16]= {
    UCD_JT_1800_180f,
    UCD_JT_0000_000f, /*  UCD_JT_1810_181f  */
    UCD_JT_06a0_06af, /*  UCD_JT_1820_182f  */
    UCD_JT_06a0_06af, /*  UCD_JT_1830_183f  */
    UCD_JT_06a0_06af, /*  UCD_JT_1840_184f  */
    UCD_JT_06a0_06af, /*  UCD_JT_1850_185f  */
    UCD_JT_06a0_06af, /*  UCD_JT_1860_186f  */
    UCD_JT_1870_187f,
    UCD_JT_1880_188f,
    UCD_JT_06a0_06af, /*  UCD_JT_1890_189f  */
    UCD_JT_18a0_18af,
    UCD_JT_0000_000f, /*  UCD_JT_18b0_18bf  */
    UCD_JT_0000_000f, /*  UCD_JT_18c0_18cf  */
    UCD_JT_0000_000f, /*  UCD_JT_18d0_18df  */
    UCD_JT_0000_000f, /*  UCD_JT_18e0_18ef  */
    UCD_JT_0000_000f, /*  UCD_JT_18f0_18ff  */
};
static const unsigned char    UCD_JT_1920_192f[16]= {T,T,T,U,U,U,U,T,T,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_1930_193f[16]= {U,U,T,U,U,U,U,U,U,T,T,T,U,U,U,U,};
static const unsigned char * const UCD_JT_1900_19ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1900_190f  */
    UCD_JT_0000_000f, /*  UCD_JT_1910_191f  */
    UCD_JT_1920_192f,
    UCD_JT_1930_193f,
    UCD_JT_0000_000f, /*  UCD_JT_1940_194f  */
    UCD_JT_0000_000f, /*  UCD_JT_1950_195f  */
    UCD_JT_0000_000f, /*  UCD_JT_1960_196f  */
    UCD_JT_0000_000f, /*  UCD_JT_1970_197f  */
    UCD_JT_0000_000f, /*  UCD_JT_1980_198f  */
    UCD_JT_0000_000f, /*  UCD_JT_1990_199f  */
    UCD_JT_0000_000f, /*  UCD_JT_19a0_19af  */
    UCD_JT_0000_000f, /*  UCD_JT_19b0_19bf  */
    UCD_JT_0000_000f, /*  UCD_JT_19c0_19cf  */
    UCD_JT_0000_000f, /*  UCD_JT_19d0_19df  */
    UCD_JT_0000_000f, /*  UCD_JT_19e0_19ef  */
    UCD_JT_0000_000f, /*  UCD_JT_19f0_19ff  */
};
static const unsigned char    UCD_JT_1a10_1a1f[16]= {U,U,U,U,U,U,U,T,T,U,U,T,U,U,U,U,};
static const unsigned char    UCD_JT_1a50_1a5f[16]= {U,U,U,U,U,U,T,U,T,T,T,T,T,T,T,U,};
static const unsigned char    UCD_JT_1a60_1a6f[16]= {T,U,T,U,U,T,T,T,T,T,T,T,T,U,U,U,};
static const unsigned char    UCD_JT_1a70_1a7f[16]= {U,U,U,T,T,T,T,T,T,T,T,T,T,U,U,T,};
static const unsigned char    UCD_JT_1ab0_1abf[16]= {T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,U,};
static const unsigned char * const UCD_JT_1a00_1aff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1a00_1a0f  */
    UCD_JT_1a10_1a1f,
    UCD_JT_0000_000f, /*  UCD_JT_1a20_1a2f  */
    UCD_JT_0000_000f, /*  UCD_JT_1a30_1a3f  */
    UCD_JT_0000_000f, /*  UCD_JT_1a40_1a4f  */
    UCD_JT_1a50_1a5f,
    UCD_JT_1a60_1a6f,
    UCD_JT_1a70_1a7f,
    UCD_JT_0000_000f, /*  UCD_JT_1a80_1a8f  */
    UCD_JT_0000_000f, /*  UCD_JT_1a90_1a9f  */
    UCD_JT_0000_000f, /*  UCD_JT_1aa0_1aaf  */
    UCD_JT_1ab0_1abf,
    UCD_JT_0000_000f, /*  UCD_JT_1ac0_1acf  */
    UCD_JT_0000_000f, /*  UCD_JT_1ad0_1adf  */
    UCD_JT_0000_000f, /*  UCD_JT_1ae0_1aef  */
    UCD_JT_0000_000f, /*  UCD_JT_1af0_1aff  */
};
static const unsigned char    UCD_JT_1b00_1b0f[16]= {T,T,T,T,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_1b30_1b3f[16]= {U,U,U,U,T,U,T,T,T,T,T,U,T,U,U,U,};
static const unsigned char    UCD_JT_1b60_1b6f[16]= {U,U,U,U,U,U,U,U,U,U,U,T,T,T,T,T,};
static const unsigned char    UCD_JT_1ba0_1baf[16]= {U,U,T,T,T,T,U,U,T,T,U,T,T,T,U,U,};
static const unsigned char    UCD_JT_1be0_1bef[16]= {U,U,U,U,U,U,T,U,T,T,U,U,U,T,U,T,};
static const unsigned char * const UCD_JT_1b00_1bff[16]= {
    UCD_JT_1b00_1b0f,
    UCD_JT_0000_000f, /*  UCD_JT_1b10_1b1f  */
    UCD_JT_0000_000f, /*  UCD_JT_1b20_1b2f  */
    UCD_JT_1b30_1b3f,
    UCD_JT_0b80_0b8f, /*  UCD_JT_1b40_1b4f  */
    UCD_JT_0000_000f, /*  UCD_JT_1b50_1b5f  */
    UCD_JT_1b60_1b6f,
    UCD_JT_1b00_1b0f, /*  UCD_JT_1b70_1b7f  */
    UCD_JT_0d00_0d0f, /*  UCD_JT_1b80_1b8f  */
    UCD_JT_0000_000f, /*  UCD_JT_1b90_1b9f  */
    UCD_JT_1ba0_1baf,
    UCD_JT_0000_000f, /*  UCD_JT_1bb0_1bbf  */
    UCD_JT_0000_000f, /*  UCD_JT_1bc0_1bcf  */
    UCD_JT_0000_000f, /*  UCD_JT_1bd0_1bdf  */
    UCD_JT_1be0_1bef,
    UCD_JT_0d00_0d0f, /*  UCD_JT_1bf0_1bff  */
};
static const unsigned char    UCD_JT_1c20_1c2f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,T,T,T,T,};
static const unsigned char    UCD_JT_1c30_1c3f[16]= {T,T,T,T,U,U,T,T,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_1cd0_1cdf[16]= {T,T,T,U,T,T,T,T,T,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_1ce0_1cef[16]= {T,U,T,T,T,T,T,T,T,U,U,U,U,T,U,U,};
static const unsigned char    UCD_JT_1cf0_1cff[16]= {U,U,U,U,T,U,U,U,T,T,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_1c00_1cff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1c00_1c0f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c10_1c1f  */
    UCD_JT_1c20_1c2f,
    UCD_JT_1c30_1c3f,
    UCD_JT_0000_000f, /*  UCD_JT_1c40_1c4f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c50_1c5f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c60_1c6f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c70_1c7f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c80_1c8f  */
    UCD_JT_0000_000f, /*  UCD_JT_1c90_1c9f  */
    UCD_JT_0000_000f, /*  UCD_JT_1ca0_1caf  */
    UCD_JT_0000_000f, /*  UCD_JT_1cb0_1cbf  */
    UCD_JT_0000_000f, /*  UCD_JT_1cc0_1ccf  */
    UCD_JT_1cd0_1cdf,
    UCD_JT_1ce0_1cef,
    UCD_JT_1cf0_1cff,
};
static const unsigned char    UCD_JT_1df0_1dff[16]= {T,T,T,T,T,T,T,T,T,T,U,T,T,T,T,T,};
static const unsigned char * const UCD_JT_1d00_1dff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_1d00_1d0f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d10_1d1f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d20_1d2f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d30_1d3f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d40_1d4f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d50_1d5f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d60_1d6f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d70_1d7f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d80_1d8f  */
    UCD_JT_0000_000f, /*  UCD_JT_1d90_1d9f  */
    UCD_JT_0000_000f, /*  UCD_JT_1da0_1daf  */
    UCD_JT_0000_000f, /*  UCD_JT_1db0_1dbf  */
    UCD_JT_0300_030f, /*  UCD_JT_1dc0_1dcf  */
    UCD_JT_0300_030f, /*  UCD_JT_1dd0_1ddf  */
    UCD_JT_0300_030f, /*  UCD_JT_1de0_1def  */
    UCD_JT_1df0_1dff,
};
static const unsigned char * const * const UCD_JT_1000_1fff[16]= {
    UCD_JT_1000_10ff,
    UCD_JT_0100_01ff, /*  UCD_JT_1100_11ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_1200_12ff  */
    UCD_JT_1300_13ff,
    UCD_JT_0100_01ff, /*  UCD_JT_1400_14ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_1500_15ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_1600_16ff  */
    UCD_JT_1700_17ff,
    UCD_JT_1800_18ff,
    UCD_JT_1900_19ff,
    UCD_JT_1a00_1aff,
    UCD_JT_1b00_1bff,
    UCD_JT_1c00_1cff,
    UCD_JT_1d00_1dff,
    UCD_JT_0100_01ff, /*  UCD_JT_1e00_1eff  */
    UCD_JT_0100_01ff, /*  UCD_JT_1f00_1fff  */
};
static const unsigned char    UCD_JT_2000_200f[16]= {U,U,U,U,U,U,U,U,U,U,U,T,U,C,T,T,};
static const unsigned char    UCD_JT_2020_202f[16]= {U,U,U,U,U,U,U,U,U,U,T,T,T,T,T,U,};
static const unsigned char    UCD_JT_2060_206f[16]= {T,T,T,T,T,U,U,U,U,U,T,T,T,T,T,T,};
static const unsigned char * const UCD_JT_2000_20ff[16]= {
    UCD_JT_2000_200f,
    UCD_JT_0000_000f, /*  UCD_JT_2010_201f  */
    UCD_JT_2020_202f,
    UCD_JT_0000_000f, /*  UCD_JT_2030_203f  */
    UCD_JT_0000_000f, /*  UCD_JT_2040_204f  */
    UCD_JT_0000_000f, /*  UCD_JT_2050_205f  */
    UCD_JT_2060_206f,
    UCD_JT_0000_000f, /*  UCD_JT_2070_207f  */
    UCD_JT_0000_000f, /*  UCD_JT_2080_208f  */
    UCD_JT_0000_000f, /*  UCD_JT_2090_209f  */
    UCD_JT_0000_000f, /*  UCD_JT_20a0_20af  */
    UCD_JT_0000_000f, /*  UCD_JT_20b0_20bf  */
    UCD_JT_0000_000f, /*  UCD_JT_20c0_20cf  */
    UCD_JT_0300_030f, /*  UCD_JT_20d0_20df  */
    UCD_JT_0300_030f, /*  UCD_JT_20e0_20ef  */
    UCD_JT_07b0_07bf, /*  UCD_JT_20f0_20ff  */
};
static const unsigned char * const UCD_JT_2c00_2cff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_2c00_2c0f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c10_2c1f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c20_2c2f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c30_2c3f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c40_2c4f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c50_2c5f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c60_2c6f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c70_2c7f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c80_2c8f  */
    UCD_JT_0000_000f, /*  UCD_JT_2c90_2c9f  */
    UCD_JT_0000_000f, /*  UCD_JT_2ca0_2caf  */
    UCD_JT_0000_000f, /*  UCD_JT_2cb0_2cbf  */
    UCD_JT_0000_000f, /*  UCD_JT_2cc0_2ccf  */
    UCD_JT_0000_000f, /*  UCD_JT_2cd0_2cdf  */
    UCD_JT_0700_070f, /*  UCD_JT_2ce0_2cef  */
    UCD_JT_0d00_0d0f, /*  UCD_JT_2cf0_2cff  */
};
static const unsigned char * const UCD_JT_2d00_2dff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_2d00_2d0f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d10_2d1f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d20_2d2f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d30_2d3f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d40_2d4f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d50_2d5f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d60_2d6f  */
    UCD_JT_0700_070f, /*  UCD_JT_2d70_2d7f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d80_2d8f  */
    UCD_JT_0000_000f, /*  UCD_JT_2d90_2d9f  */
    UCD_JT_0000_000f, /*  UCD_JT_2da0_2daf  */
    UCD_JT_0000_000f, /*  UCD_JT_2db0_2dbf  */
    UCD_JT_0000_000f, /*  UCD_JT_2dc0_2dcf  */
    UCD_JT_0000_000f, /*  UCD_JT_2dd0_2ddf  */
    UCD_JT_0300_030f, /*  UCD_JT_2de0_2def  */
    UCD_JT_0300_030f, /*  UCD_JT_2df0_2dff  */
};
static const unsigned char * const * const UCD_JT_2000_2fff[16]= {
    UCD_JT_2000_20ff,
    UCD_JT_0100_01ff, /*  UCD_JT_2100_21ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2200_22ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2300_23ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2400_24ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2500_25ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2600_26ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2700_27ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2800_28ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2900_29ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2a00_2aff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2b00_2bff  */
    UCD_JT_2c00_2cff,
    UCD_JT_2d00_2dff,
    UCD_JT_0100_01ff, /*  UCD_JT_2e00_2eff  */
    UCD_JT_0100_01ff, /*  UCD_JT_2f00_2fff  */
};
static const unsigned char    UCD_JT_3020_302f[16]= {U,U,U,U,U,U,U,U,U,U,T,T,T,T,U,U,};
static const unsigned char    UCD_JT_3090_309f[16]= {U,U,U,U,U,U,U,U,U,T,T,U,U,U,U,U,};
static const unsigned char * const UCD_JT_3000_30ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_3000_300f  */
    UCD_JT_0000_000f, /*  UCD_JT_3010_301f  */
    UCD_JT_3020_302f,
    UCD_JT_0000_000f, /*  UCD_JT_3030_303f  */
    UCD_JT_0000_000f, /*  UCD_JT_3040_304f  */
    UCD_JT_0000_000f, /*  UCD_JT_3050_305f  */
    UCD_JT_0000_000f, /*  UCD_JT_3060_306f  */
    UCD_JT_0000_000f, /*  UCD_JT_3070_307f  */
    UCD_JT_0000_000f, /*  UCD_JT_3080_308f  */
    UCD_JT_3090_309f,
    UCD_JT_0000_000f, /*  UCD_JT_30a0_30af  */
    UCD_JT_0000_000f, /*  UCD_JT_30b0_30bf  */
    UCD_JT_0000_000f, /*  UCD_JT_30c0_30cf  */
    UCD_JT_0000_000f, /*  UCD_JT_30d0_30df  */
    UCD_JT_0000_000f, /*  UCD_JT_30e0_30ef  */
    UCD_JT_0000_000f, /*  UCD_JT_30f0_30ff  */
};
static const unsigned char * const * const UCD_JT_3000_3fff[16]= {
    UCD_JT_3000_30ff,
    UCD_JT_0100_01ff, /*  UCD_JT_3100_31ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3200_32ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3300_33ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3400_34ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3500_35ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3600_36ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3700_37ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3800_38ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3900_39ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3a00_3aff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3b00_3bff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3c00_3cff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3d00_3dff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3e00_3eff  */
    UCD_JT_0100_01ff, /*  UCD_JT_3f00_3fff  */
};
static const unsigned char * const * const UCD_JT_4000_4fff[16]= {
    UCD_JT_0100_01ff, /*  UCD_JT_4000_40ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4100_41ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4200_42ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4300_43ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4400_44ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4500_45ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4600_46ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4700_47ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4800_48ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4900_49ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4a00_4aff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4b00_4bff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4c00_4cff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4d00_4dff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4e00_4eff  */
    UCD_JT_0100_01ff, /*  UCD_JT_4f00_4fff  */
};
static const unsigned char    UCD_JT_a670_a67f[16]= {T,T,T,U,T,T,T,T,T,T,T,T,T,T,U,U,};
static const unsigned char * const UCD_JT_a600_a6ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_a600_a60f  */
    UCD_JT_0000_000f, /*  UCD_JT_a610_a61f  */
    UCD_JT_0000_000f, /*  UCD_JT_a620_a62f  */
    UCD_JT_0000_000f, /*  UCD_JT_a630_a63f  */
    UCD_JT_0000_000f, /*  UCD_JT_a640_a64f  */
    UCD_JT_0000_000f, /*  UCD_JT_a650_a65f  */
    UCD_JT_0700_070f, /*  UCD_JT_a660_a66f  */
    UCD_JT_a670_a67f,
    UCD_JT_0000_000f, /*  UCD_JT_a680_a68f  */
    UCD_JT_0c30_0c3f, /*  UCD_JT_a690_a69f  */
    UCD_JT_0000_000f, /*  UCD_JT_a6a0_a6af  */
    UCD_JT_0000_000f, /*  UCD_JT_a6b0_a6bf  */
    UCD_JT_0000_000f, /*  UCD_JT_a6c0_a6cf  */
    UCD_JT_0000_000f, /*  UCD_JT_a6d0_a6df  */
    UCD_JT_0000_000f, /*  UCD_JT_a6e0_a6ef  */
    UCD_JT_0d00_0d0f, /*  UCD_JT_a6f0_a6ff  */
};
static const unsigned char    UCD_JT_a800_a80f[16]= {U,U,T,U,U,U,T,U,U,U,U,T,U,U,U,U,};
static const unsigned char    UCD_JT_a870_a87f[16]= {D,D,L,U,U,U,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_a8c0_a8cf[16]= {U,U,U,U,T,T,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_a800_a8ff[16]= {
    UCD_JT_a800_a80f,
    UCD_JT_0000_000f, /*  UCD_JT_a810_a81f  */
    UCD_JT_0c50_0c5f, /*  UCD_JT_a820_a82f  */
    UCD_JT_0000_000f, /*  UCD_JT_a830_a83f  */
    UCD_JT_06a0_06af, /*  UCD_JT_a840_a84f  */
    UCD_JT_06a0_06af, /*  UCD_JT_a850_a85f  */
    UCD_JT_06a0_06af, /*  UCD_JT_a860_a86f  */
    UCD_JT_a870_a87f,
    UCD_JT_0000_000f, /*  UCD_JT_a880_a88f  */
    UCD_JT_0000_000f, /*  UCD_JT_a890_a89f  */
    UCD_JT_0000_000f, /*  UCD_JT_a8a0_a8af  */
    UCD_JT_0000_000f, /*  UCD_JT_a8b0_a8bf  */
    UCD_JT_a8c0_a8cf,
    UCD_JT_0000_000f, /*  UCD_JT_a8d0_a8df  */
    UCD_JT_0300_030f, /*  UCD_JT_a8e0_a8ef  */
    UCD_JT_0d00_0d0f, /*  UCD_JT_a8f0_a8ff  */
};
static const unsigned char    UCD_JT_a920_a92f[16]= {U,U,U,U,U,U,T,T,T,T,T,T,T,T,U,U,};
static const unsigned char    UCD_JT_a940_a94f[16]= {U,U,U,U,U,U,U,T,T,T,T,T,T,T,T,T,};
static const unsigned char    UCD_JT_a9b0_a9bf[16]= {U,U,U,T,U,U,T,T,T,T,U,U,T,U,U,U,};
static const unsigned char    UCD_JT_a9e0_a9ef[16]= {U,U,U,U,U,T,U,U,U,U,U,U,U,U,U,U,};
static const unsigned char * const UCD_JT_a900_a9ff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_a900_a90f  */
    UCD_JT_0000_000f, /*  UCD_JT_a910_a91f  */
    UCD_JT_a920_a92f,
    UCD_JT_0000_000f, /*  UCD_JT_a930_a93f  */
    UCD_JT_a940_a94f,
    UCD_JT_0d00_0d0f, /*  UCD_JT_a950_a95f  */
    UCD_JT_0000_000f, /*  UCD_JT_a960_a96f  */
    UCD_JT_0000_000f, /*  UCD_JT_a970_a97f  */
    UCD_JT_0900_090f, /*  UCD_JT_a980_a98f  */
    UCD_JT_0000_000f, /*  UCD_JT_a990_a99f  */
    UCD_JT_0000_000f, /*  UCD_JT_a9a0_a9af  */
    UCD_JT_a9b0_a9bf,
    UCD_JT_0000_000f, /*  UCD_JT_a9c0_a9cf  */
    UCD_JT_0000_000f, /*  UCD_JT_a9d0_a9df  */
    UCD_JT_a9e0_a9ef,
    UCD_JT_0000_000f, /*  UCD_JT_a9f0_a9ff  */
};
static const unsigned char    UCD_JT_aa20_aa2f[16]= {U,U,U,U,U,U,U,U,U,T,T,T,T,T,T,U,};
static const unsigned char    UCD_JT_aa30_aa3f[16]= {U,T,T,U,U,T,T,U,U,U,U,U,U,U,U,U,};
static const unsigned char    UCD_JT_aa40_aa4f[16]= {U,U,U,T,U,U,U,U,U,U,U,U,T,U,U,U,};
static const unsigned char    UCD_JT_aab0_aabf[16]= {T,U,T,T,T,U,U,T,T,U,U,U,U,U,T,T,};
static const unsigned char    UCD_JT_aae0_aaef[16]= {U,U,U,U,U,U,U,U,U,U,U,U,T,T,U,U,};
static const unsigned char * const UCD_JT_aa00_aaff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_aa00_aa0f  */
    UCD_JT_0000_000f, /*  UCD_JT_aa10_aa1f  */
    UCD_JT_aa20_aa2f,
    UCD_JT_aa30_aa3f,
    UCD_JT_aa40_aa4f,
    UCD_JT_0000_000f, /*  UCD_JT_aa50_aa5f  */
    UCD_JT_0000_000f, /*  UCD_JT_aa60_aa6f  */
    UCD_JT_09b0_09bf, /*  UCD_JT_aa70_aa7f  */
    UCD_JT_0000_000f, /*  UCD_JT_aa80_aa8f  */
    UCD_JT_0000_000f, /*  UCD_JT_aa90_aa9f  */
    UCD_JT_0000_000f, /*  UCD_JT_aaa0_aaaf  */
    UCD_JT_aab0_aabf,
    UCD_JT_0980_098f, /*  UCD_JT_aac0_aacf  */
    UCD_JT_0000_000f, /*  UCD_JT_aad0_aadf  */
    UCD_JT_aae0_aaef,
    UCD_JT_0b50_0b5f, /*  UCD_JT_aaf0_aaff  */
};
static const unsigned char    UCD_JT_abe0_abef[16]= {U,U,U,U,U,T,U,U,T,U,U,U,U,T,U,U,};
static const unsigned char * const UCD_JT_ab00_abff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_ab00_ab0f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab10_ab1f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab20_ab2f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab30_ab3f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab40_ab4f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab50_ab5f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab60_ab6f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab70_ab7f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab80_ab8f  */
    UCD_JT_0000_000f, /*  UCD_JT_ab90_ab9f  */
    UCD_JT_0000_000f, /*  UCD_JT_aba0_abaf  */
    UCD_JT_0000_000f, /*  UCD_JT_abb0_abbf  */
    UCD_JT_0000_000f, /*  UCD_JT_abc0_abcf  */
    UCD_JT_0000_000f, /*  UCD_JT_abd0_abdf  */
    UCD_JT_abe0_abef,
    UCD_JT_0000_000f, /*  UCD_JT_abf0_abff  */
};
static const unsigned char * const * const UCD_JT_a000_afff[16]= {
    UCD_JT_0100_01ff, /*  UCD_JT_a000_a0ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_a100_a1ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_a200_a2ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_a300_a3ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_a400_a4ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_a500_a5ff  */
    UCD_JT_a600_a6ff,
    UCD_JT_0100_01ff, /*  UCD_JT_a700_a7ff  */
    UCD_JT_a800_a8ff,
    UCD_JT_a900_a9ff,
    UCD_JT_aa00_aaff,
    UCD_JT_ab00_abff,
    UCD_JT_0100_01ff, /*  UCD_JT_ac00_acff  */
    UCD_JT_0100_01ff, /*  UCD_JT_ad00_adff  */
    UCD_JT_0100_01ff, /*  UCD_JT_ae00_aeff  */
    UCD_JT_0100_01ff, /*  UCD_JT_af00_afff  */
};
static const unsigned char    UCD_JT_fb10_fb1f[16]= {U,U,U,U,U,U,U,U,U,U,U,U,U,U,T,U,};
static const unsigned char * const UCD_JT_fb00_fbff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_fb00_fb0f  */
    UCD_JT_fb10_fb1f,
    UCD_JT_0000_000f, /*  UCD_JT_fb20_fb2f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb30_fb3f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb40_fb4f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb50_fb5f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb60_fb6f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb70_fb7f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb80_fb8f  */
    UCD_JT_0000_000f, /*  UCD_JT_fb90_fb9f  */
    UCD_JT_0000_000f, /*  UCD_JT_fba0_fbaf  */
    UCD_JT_0000_000f, /*  UCD_JT_fbb0_fbbf  */
    UCD_JT_0000_000f, /*  UCD_JT_fbc0_fbcf  */
    UCD_JT_0000_000f, /*  UCD_JT_fbd0_fbdf  */
    UCD_JT_0000_000f, /*  UCD_JT_fbe0_fbef  */
    UCD_JT_0000_000f, /*  UCD_JT_fbf0_fbff  */
};
static const unsigned char * const UCD_JT_fe00_feff[16]= {
    UCD_JT_0300_030f, /*  UCD_JT_fe00_fe0f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe10_fe1f  */
    UCD_JT_0300_030f, /*  UCD_JT_fe20_fe2f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe30_fe3f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe40_fe4f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe50_fe5f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe60_fe6f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe70_fe7f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe80_fe8f  */
    UCD_JT_0000_000f, /*  UCD_JT_fe90_fe9f  */
    UCD_JT_0000_000f, /*  UCD_JT_fea0_feaf  */
    UCD_JT_0000_000f, /*  UCD_JT_feb0_febf  */
    UCD_JT_0000_000f, /*  UCD_JT_fec0_fecf  */
    UCD_JT_0000_000f, /*  UCD_JT_fed0_fedf  */
    UCD_JT_0000_000f, /*  UCD_JT_fee0_feef  */
    UCD_JT_0700_070f, /*  UCD_JT_fef0_feff  */
};
static const unsigned char    UCD_JT_fff0_ffff[16]= {U,U,U,U,U,U,U,U,U,T,T,T,U,U,U,U,};
static const unsigned char * const UCD_JT_ff00_ffff[16]= {
    UCD_JT_0000_000f, /*  UCD_JT_ff00_ff0f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff10_ff1f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff20_ff2f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff30_ff3f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff40_ff4f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff50_ff5f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff60_ff6f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff70_ff7f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff80_ff8f  */
    UCD_JT_0000_000f, /*  UCD_JT_ff90_ff9f  */
    UCD_JT_0000_000f, /*  UCD_JT_ffa0_ffaf  */
    UCD_JT_0000_000f, /*  UCD_JT_ffb0_ffbf  */
    UCD_JT_0000_000f, /*  UCD_JT_ffc0_ffcf  */
    UCD_JT_0000_000f, /*  UCD_JT_ffd0_ffdf  */
    UCD_JT_0000_000f, /*  UCD_JT_ffe0_ffef  */
    UCD_JT_fff0_ffff,
};
static const unsigned char * const * const UCD_JT_f000_ffff[16]= {
    UCD_JT_0100_01ff, /*  UCD_JT_f000_f0ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f100_f1ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f200_f2ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f300_f3ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f400_f4ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f500_f5ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f600_f6ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f700_f7ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f800_f8ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_f900_f9ff  */
    UCD_JT_0100_01ff, /*  UCD_JT_fa00_faff  */
    UCD_JT_fb00_fbff,
    UCD_JT_0100_01ff, /*  UCD_JT_fc00_fcff  */
    UCD_JT_0100_01ff, /*  UCD_JT_fd00_fdff  */
    UCD_JT_fe00_feff,
    UCD_JT_ff00_ffff,
};
static const unsigned char * const * const * const UCD_JT[16]={
    UCD_JT_0000_0fff,
    UCD_JT_1000_1fff,
    UCD_JT_2000_2fff,
    UCD_JT_3000_3fff,
    UCD_JT_4000_4fff,
    UCD_JT_4000_4fff, /*  UCD_JT_5000_5fff  */
    UCD_JT_4000_4fff, /*  UCD_JT_6000_6fff  */
    UCD_JT_4000_4fff, /*  UCD_JT_7000_7fff  */
    UCD_JT_4000_4fff, /*  UCD_JT_8000_8fff  */
    UCD_JT_4000_4fff, /*  UCD_JT_9000_9fff  */
    UCD_JT_a000_afff,
    UCD_JT_4000_4fff, /*  UCD_JT_b000_bfff  */
    UCD_JT_4000_4fff, /*  UCD_JT_c000_cfff  */
    UCD_JT_4000_4fff, /*  UCD_JT_d000_dfff  */
    UCD_JT_4000_4fff, /*  UCD_JT_e000_efff  */
    UCD_JT_f000_ffff,
};

int ucdJoinType( int sym ) {
    return ucdSymbolClass( sym, UCD_JT, U );
}
const char * ucdJoinTypeStr( int sym ) {
    static char scratch[20];
    switch( sym )
	{
	case C:	return "C";
	case D:	return "D";
	case L:	return "L";
	case R:	return "R";
	case T:	return "T";
	case U:	return "U";
	default:
	    sprintf( scratch, "%d", sym );
	    return scratch;
	}
}
