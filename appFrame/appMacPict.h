/************************************************************************/
/*									*/
/*  Windows Meta File specific definitions.				*/
/*									*/
/************************************************************************/

#   include	<X11/Xlib.h>
#   include	<bitmap.h>
#   include	<docFont.h>
#   include	<utilPostscriptFace.h>
#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Opcodes.								*/
/*									*/
/************************************************************************/

# define MACPICT_NOP			0x00
# define MACPICT_Clip			0x01
# define MACPICT_BkPat			0x02
# define MACPICT_TxFont			0x03
# define MACPICT_TxFace			0x04
# define MACPICT_TxMode			0x05
# define MACPICT_SpExtra		0x06
# define MACPICT_PnSize			0x07
# define MACPICT_PnMode			0x08
# define MACPICT_PnPat			0x09
# define MACPICT_FillPat		0x0a
# define MACPICT_OvSize			0x0b
# define MACPICT_Origin			0x0c
# define MACPICT_TxSize			0x0d
# define MACPICT_FgColor		0x0e
# define MACPICT_BkColor		0x0f
# define MACPICT_TxRatio		0x10
# define MACPICT_Version		0x11
# define MACPICT_BkPixPat		0x12
# define MACPICT_PnPixPat		0x13
# define MACPICT_FillPixPat		0x14
# define MACPICT_PnLocHFrac		0x15
# define MACPICT_ChExtra		0x16
# define MACPICT_RGBFgCol		0x1a
# define MACPICT_RGBBkCol		0x1b
# define MACPICT_HiliteMode		0x1c
# define MACPICT_HiliteColor		0x1d
# define MACPICT_DefHilite		0x1e
# define MACPICT_OpColor		0x1f
# define MACPICT_Line			0x20
# define MACPICT_LineFrom		0x21
# define MACPICT_ShortLine		0x22
# define MACPICT_ShortLineFrom		0x23
# define MACPICT_LongText		0x28
# define MACPICT_DHText			0x29
# define MACPICT_DVText			0x2a
# define MACPICT_DHDVText		0x2b
# define MACPICT_fontName		0x2c
# define MACPICT_frameRect		0x30
# define MACPICT_paintRect		0x31
# define MACPICT_eraseRect		0x32
# define MACPICT_invertRect		0x33
# define MACPICT_fillRect		0x34
# define MACPICT_frameSameRect		0x38
# define MACPICT_paintSameRect		0x39
# define MACPICT_eraseSameRect		0x3a
# define MACPICT_invertSameRect		0x3b
# define MACPICT_fillSameRect		0x3c
# define MACPICT_frameRRect		0x40
# define MACPICT_paintRRect		0x41
# define MACPICT_eraseRRect		0x42
# define MACPICT_invertRRect		0x43
# define MACPICT_fillRRrect		0x44
# define MACPICT_frameSameRRect		0x48
# define MACPICT_paintSameRRect		0x49
# define MACPICT_eraseSameRRect		0x4a
# define MACPICT_invertSameRRect	0x4b
# define MACPICT_fillSameRRect		0x4c
# define MACPICT_frameOval		0x50
# define MACPICT_paintOval		0x51
# define MACPICT_eraseOval		0x52
# define MACPICT_invertOval		0x53
# define MACPICT_fillOval		0x54
# define MACPICT_frameSameOval		0x58
# define MACPICT_paintSameOval		0x59
# define MACPICT_eraseSameOval		0x5a
# define MACPICT_invertSameOval		0x5b
# define MACPICT_fillSameOval		0x5c
# define MACPICT_frameArc		0x60
# define MACPICT_paintArc		0x61
# define MACPICT_eraseArc		0x62
# define MACPICT_invertArc		0x63
# define MACPICT_fillArc		0x64
# define MACPICT_frameSameArc		0x68
# define MACPICT_paintSameArc		0x69
# define MACPICT_eraseSameArc		0x6a
# define MACPICT_invertSameArc		0x6b
# define MACPICT_fillSameArc		0x6c
# define MACPICT_framePoly		0x70
# define MACPICT_PaintPoly		0x71
# define MACPICT_erasePoly		0x72
# define MACPICT_invertPoly		0x73
# define MACPICT_fillPoly		0x74
# define MACPICT_frameSamePoly		0x78
# define MACPICT_paintSamePoly		0x79
# define MACPICT_eraseSamePoly		0x7a
# define MACPICT_invertSamePoly		0x7b
# define MACPICT_fillSamePoly		0x7c
# define MACPICT_frameRgn		0x80
# define MACPICT_paintRgn		0x81
# define MACPICT_eraseRgn		0x82
# define MACPICT_invertRgn		0x83
# define MACPICT_fillRgn		0x84
# define MACPICT_frameSameRgn		0x88
# define MACPICT_paintSameRgn		0x89
# define MACPICT_eraseSameRgn		0x8a
# define MACPICT_invertSameRgn		0x8b
# define MACPICT_fillSameRgn		0x8c
# define MACPICT_BitsRect		0x90
# define MACPICT_BitsRgn		0x91
# define MACPICT_PackBitsRect		0x98
# define MACPICT_PackBitsRgn		0x99
# define MACPICT_DirectBitsRect		0x9a
# define MACPICT_DirectBitsRgn		0x9b
# define MACPICT_ShortComment		0xa0
# define MACPICT_LongComment		0xa1

# define MACPICT_OpEndPic		0xff

/************************************************************************/
/*									*/
/*  Pen Modes.								*/
/*									*/
/************************************************************************/

# define PENMODE_patCopy	8
# define PENMODE_patOr		9
# define PENMODE_patXor		10
# define PENMODE_patBic		11
# define PENMODE_notPatCopy	12
# define PENMODE_notPatOr	13
# define PENMODE_notPatXor	14
# define PENMODE_notPatBic	15

/************************************************************************/
/*									*/
/*  Describes a macpict device.						*/
/*									*/
/************************************************************************/

typedef struct MacpictDevice
    {
    int			mdVersion;

    int			mdInX0;
    int			mdInY0;
    int			mdInX1;
    int			mdInY1;

    int			mdOutWide;
    int			mdOutHigh;

    int			mdPenMode;

    RGB8Color		mdForeColor;
    RGB8Color		mdBackColor;
    RGB8Color		mdColorSet;

    APP_POINT *		mdPoints;

    AppDrawingData	mdDrawingData;
    } MacpictDevice;

# define MD_X( x, md ) \
    ( ( (md)->mdOutWide* ( (x)-(md)->mdInX0 ) )/( (md)->mdInX1-(md)->mdInX0 ) )
# define MD_Y( y, md ) \
    ( ( (md)->mdOutHigh* ( (y)-(md)->mdInY0 ) )/( (md)->mdInY1-(md)->mdInY0 ) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMacPictInitDeviceHeader(	MacpictDevice *		md,
					int			outWide,
					int			outHigh );

extern void appMacPictCleanDeviceHeader( MacpictDevice *	md );

extern int appMacPictGetDeviceHeader(	MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetOpcode(	int *				pOpcode,
				int *				pBytes,
				const MacpictDevice *		md,
				SimpleInputStream *		sis );

extern int appMacPictGetColor(	RGB8Color *		rgb8,
				const MacpictDevice *	md,
				SimpleInputStream *	sis );

extern int appMacPictGetCountAndPoints(	MacpictDevice *		md,
					int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					int *			pCount,
					SimpleInputStream *	sis );

extern int appMacPictReadPaletteColors(	BitmapDescription *	bd,
					int *			pBytesRead,
					const MacpictDevice *	md,
					int			colorCount,
					int			flags,
					SimpleInputStream *	sis );

extern int appMacPictReadPacBitsRow(
				unsigned char *			to,
				int *				pBytesRead,
				int				bytesPerRow,
				const MacpictDevice *		md,
				const BitmapDescription *	bd,
				SimpleInputStream *		sis );

extern int appMacPictPlayFileX11( SimpleInputStream *	sis,
				void **			pPrivate,
				AppColors *		ac,
				AppDrawingData *	add,
				APP_WINDOW		pixmap,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			pixelsWide,
				int			pixelsHigh,
				int			twipsWide,
				int			twipsHigh );

extern int appMacPictPlayFilePs( SimpleOutputStream *		sos,
				SimpleInputStream *		sis,
				const char *			afmDirectory,
				int				useFilters,
				int				indexedImages,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh );

extern int appMacPictListFontsPs( PostScriptFaceList *	psfl,
				SimpleInputStream *	sis,
				const char *		afmDirectory,
				const char *		prefix,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			twipsWide,
				int			twipsHigh );

