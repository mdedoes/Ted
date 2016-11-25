/************************************************************************/
/*									*/
/*  Drawing objects as used by Ms-Word 95				*/
/*									*/
/************************************************************************/

#   ifndef DOC_DRAWING_OBJECT_H
#   define DOC_DRAWING_OBJECT_H

#   include	<appFrameConfig.h>

#   include	<bmcolor.h>
#   include	<appGuiBase.h>

typedef enum DrawingObjectXAttach
    {
    DOxaPAGE= 0,
    DOxaCOLUMN,
    DOxaMARGIN,

    DOxa_COUNT
    } DrawingObjectXAttach;

# define DOxa_BITS	2

typedef enum DrawingObjectYAttach
    {
    DOyaPAGE= 0,
    DOyaPARAGRAPH,
    DOyaMARGIN,

    DOya_COUNT
    } DrawingObjectYAttach;

# define DOya_BITS	2

typedef enum DrawingObjectKind
    {
    DOkindARC= 0,
    DOkindCALLOUT,
    DOkindELLIPSE,
    DOkindLINE,
    DOkindPOLYGON,
    DOkindPOLYLINE,
    DOkindRECT,
    DOkindTXBX,

    DOkind_COUNT
    } DrawingObjectKind;

# define DOkind_BITS	3

typedef enum DrawingObjectLineStyle
    {
    DOlineSOLID= 0,
    DOlineHOLLOW,
    DOlineDASH,
    DOlineDOT,
    DOlineDADO,
    DOlineDADODO,

    DOline_COUNT
    } DrawingObjectLineStyle;

# define DOline_BITS	3

typedef enum DrawingObjectFillPattern
    {
    DOfillCLEAR= 0,
    DOfillSOLID,

    DOfillSHADE_5P,
    DOfillSHADE_10P,
    DOfillSHADE_20P,
    DOfillSHADE_25P,
    DOfillSHADE_30P,
    DOfillSHADE_40P,
    DOfillSHADE_50P,
    DOfillSHADE_60P,
    DOfillSHADE_70P,
    DOfillSHADE_75P,
    DOfillSHADE_80P,
    DOfillSHADE_90P,

    DOfillDKHORIZ,
    DOfillDKVERT,
    DOfillDKFDIAG,
    DOfillDKBDIAG,
    DOfillDKCROSS,
    DOfillDKDCROSS,

    DOfillHORIZ,
    DOfillVERT,
    DOfillFDIAG,
    DOfillBDIAG,
    DOfillCROSS,
    DOfillDCROSS,

    DOfill_COUNT
    } DrawingObjectFillPattern;

# define DOfill_BITS	5

typedef struct DrawingObject
    {
    unsigned int	doAnchorLocked:1;
    unsigned int	doXAttach:DOxa_BITS;
    unsigned int	doYAttach:DOya_BITS;
    unsigned int	doKind:DOkind_BITS;
    unsigned int	doLineStyle:DOline_BITS;
    unsigned int	doFillPattern:DOfill_BITS;
    unsigned int	doArcFlipX:1;
    unsigned int	doArcFlipY:1;

    unsigned int	doLineColorGray:1;
    unsigned int	doFillForeColorGray:1;
    unsigned int	doFillBackColorGray:1;

    short		doX;
    short		doY;
    short		doZ;
    short		doWide;
    short		doHigh;

    short		doLineWidth;

    RGB8Color		doLineColor;
    RGB8Color		doFillForeColor;
    RGB8Color		doFillBackColor;

    APP_POINT *		doPoints;
    int			doPointCount;
    int			doNextPoint;
    } DrawingObject;

typedef enum DrawingObjectProperty
    {
    DOpropANCHOR_LOCKED= 0,
    DOpropX_ATTACH,
    DOpropY_ATTACH,
    DOpropKIND,
    DOpropLINE_STYLE,
    DOpropFILL_PATTERN,

    DOpropARC_FLIP_X,
    DOpropARC_FLIP_Y,

    DOpropZ,

    DOpropX,
    DOpropY,
    DOpropWIDE,
    DOpropHIGH,

    DOpropLINE_WIDTH,
    DOpropPOINT_COUNT,

    DOpropLINE_RED,
    DOpropLINE_GREEN,
    DOpropLINE_BLUE,
    DOpropLINE_GRAY,

    DOpropFILL_FORE_RED,
    DOpropFILL_FORE_GREEN,
    DOpropFILL_FORE_BLUE,
    DOpropFILL_FORE_GRAY,

    DOpropFILL_BACK_RED,
    DOpropFILL_BACK_GREEN,
    DOpropFILL_BACK_BLUE,
    DOpropFILL_BACK_GRAY,

    DOprop_COUNT
    } DrawingObjectProperty;

/************************************************************************/
/*									*/
/*  Function prototypes.						*/
/*									*/
/************************************************************************/

void docInitDrawingObject(		DrawingObject *		wdo );
void docCleanDrawingObject(		DrawingObject *		wdo );

#   endif /*  DOC_DRAWING_OBJECT_H  */
