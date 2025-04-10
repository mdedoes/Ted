#   include "ucd.h"
#   include "ucdIntern.h"
static const unsigned short    UCD_FINA_0620_062f[16]= {0x620,0x621,0xfe82,0xfe84,0xfe86,0xfe88,0xfe8a,0xfe8e,0xfe90,0xfe94,0xfe96,0xfe9a,0xfe9e,0xfea2,0xfea6,0xfeaa,};
static const unsigned short    UCD_FINA_0630_063f[16]= {0xfeac,0xfeae,0xfeb0,0xfeb2,0xfeb6,0xfeba,0xfebe,0xfec2,0xfec6,0xfeca,0xfece,0x63b,0x63c,0x63d,0x63e,0x63f,};
static const unsigned short    UCD_FINA_0640_064f[16]= {0x640,0xfed2,0xfed6,0xfeda,0xfede,0xfee2,0xfee6,0xfeea,0xfeee,0xfef0,0xfef2,0x64b,0x64c,0x64d,0x64e,0x64f,};
static const unsigned short    UCD_FINA_0670_067f[16]= {0x670,0xfb51,0x672,0x673,0x674,0x675,0x676,0x677,0x678,0xfb67,0xfb5f,0xfb53,0x67c,0x67d,0xfb57,0xfb63,};
static const unsigned short    UCD_FINA_0680_068f[16]= {0xfb5b,0x681,0x682,0xfb77,0xfb73,0x685,0xfb7b,0xfb7f,0xfb89,0x689,0x68a,0x68b,0xfb85,0xfb83,0xfb87,0x68f,};
static const unsigned short    UCD_FINA_0690_069f[16]= {0x690,0xfb8d,0x692,0x693,0x694,0x695,0x696,0x697,0xfb8b,0x699,0x69a,0x69b,0x69c,0x69d,0x69e,0x69f,};
static const unsigned short    UCD_FINA_06a0_06af[16]= {0x6a0,0x6a1,0x6a2,0x6a3,0xfb6b,0x6a5,0xfb6f,0x6a7,0x6a8,0xfb8f,0x6aa,0x6ab,0x6ac,0xfbd4,0x6ae,0xfb93,};
static const unsigned short    UCD_FINA_06b0_06bf[16]= {0x6b0,0xfb9b,0x6b2,0xfb97,0x6b4,0x6b5,0x6b6,0x6b7,0x6b8,0x6b9,0xfb9f,0xfba1,0x6bc,0x6bd,0xfbab,0x6bf,};
static const unsigned short    UCD_FINA_06c0_06cf[16]= {0xfba5,0xfba7,0x6c2,0x6c3,0x6c4,0xfbe1,0xfbda,0xfbd8,0xfbdc,0xfbe3,0x6ca,0xfbdf,0xfbfd,0x6cd,0x6ce,0x6cf,};
static const unsigned short    UCD_FINA_06d0_06df[16]= {0xfbe5,0x6d1,0xfbaf,0xfbb1,0x6d4,0x6d5,0x6d6,0x6d7,0x6d8,0x6d9,0x6da,0x6db,0x6dc,0x6dd,0x6de,0x6df,};
static const unsigned short * const UCD_FINA_0600_06ff[16]= {
	(const unsigned short *)0,
	(const unsigned short *)0,
	UCD_FINA_0620_062f,
	UCD_FINA_0630_063f,
	UCD_FINA_0640_064f,
	(const unsigned short *)0,
	(const unsigned short *)0,
	UCD_FINA_0670_067f,
	UCD_FINA_0680_068f,
	UCD_FINA_0690_069f,
	UCD_FINA_06a0_06af,
	UCD_FINA_06b0_06bf,
	UCD_FINA_06c0_06cf,
	UCD_FINA_06d0_06df,
};
static const unsigned short * const * const UCD_FINA_0000_0fff[16]= {
	(const unsigned short * const *)0,
	(const unsigned short * const *)0,
	(const unsigned short * const *)0,
	(const unsigned short * const *)0,
	(const unsigned short * const *)0,
	(const unsigned short * const *)0,
	UCD_FINA_0600_06ff,
};
static const unsigned short * const * const * const UCD_FINA[16]={
    UCD_FINA_0000_0fff,
};

int ucdToFinalShape( int sym ) {
    return ucdMapSymbol( sym, UCD_FINA );
}
