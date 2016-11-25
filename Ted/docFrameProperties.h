/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FRAME_PROPERTIES_H
#   define	DOC_FRAME_PROPERTIES_H

typedef enum HorizontalFrameReference
    {
    HFRphCOL= 0,
    HFRphPG,
    HFRphMRG,

    HFRph_COUNT
    } HorizontalFrameReference;

typedef enum VerticalFrameReference
    {
    VFRpvMRG= 0,
    VFRpvPG,
    VFRpvPARA,

    VFRpv_COUNT
    } VerticalFrameReference;

typedef enum HorizontalFrameAlignment
    {
    HFAposXL= 0,
    HFAposXR,
    HFAposXC,
    HFAposXI,
    HFAposXO,

    HFApos_COUNT
    } HorizontalFrameAlignment;

typedef enum VerticalFrameAlignment
    {
    VFAposYIL= 0,
    VFAposYT,
    VFAposYC,
    VFAposYB,

    VFApos_COUNT
    } VerticalFrameAlignment;

typedef enum FrameWrapStyle
    {
    FWSwrapWRAP= 0,
    FWSwrapNOWRAP,
    FWSwrapOVERLAY,

    FWSwrap_COUNT
    } FrameWrapStyle;

typedef struct TextFrameProperties
    {
    int			tfpFrameWidthTwips;
    int			tfpFrameHeightTwips;

    int			tfpFrameXTwips;
    int			tfpFrameYTwips;

    int			tfpDistanceFromTextTwips;
    int			tfpHorDistanceFromTextTwips;
    int			tfpVerDistanceFromTextTwips;

    unsigned int	tfpHorizontalFrameReference:2;
    unsigned int	tfpHorizontalFrameAlignment:3;

    unsigned int	tfpVerticalFrameReference:2;
    unsigned int	tfpVerticalFrameAlignment:3;
    unsigned int	tfpFrameLockedToParagraph:1;

    unsigned int	tfpFrameWrapStyle:2;
    } TextFrameProperties;

/************************************************************************/
/*									*/
/*  Property numbers and masks.						*/
/*									*/
/************************************************************************/

typedef enum TextFrameProperty
    {
    TFPprop_NONE= -1,

    TFPpropABSW= 0,
    TFPpropABSH,

    TFPpropHORIZONTAL_REFERENCE,
    TFPpropHORIZONTAL_POSITION,
    TFPpropHORIZONTAL_ALIGNMENT,

    TFPpropVERTICAL_REFERENCE,
    TFPpropVERTICAL_POSITION,
    TFPpropVERTICAL_ALIGNMENT,
    TFPpropLOCKED_TO_PARA,

    TFPpropWRAP_STYLE,
    TFPpropDXFRTEXT,
    TFPpropDFRMTXTX,
    TFPpropDFRMTXTY,

    TFPprop_COUNT
    } TextFrameProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTextFrameProperties(	TextFrameProperties *	tfp );

#   endif	/*  DOC_FRAME_PROPERTIES_H	*/
