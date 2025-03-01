/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FRAME_PROPERTIES_H
#   define	DOC_FRAME_PROPERTIES_H

typedef enum FrameWrapStyle
    {
    FWSwrapWRAP= 0,
    FWSwrapNOWRAP,
    FWSwrapOVERLAY,

    FWSwrap_COUNT
    } FrameWrapStyle;

typedef struct FrameProperties
    {
    int			fpWideTwips;
    int			fpHighTwips;

    int			fpXTwips;
    int			fpYTwips;

    int			fpDistanceFromTextTwipsLeft;
    int			fpDistanceFromTextTwipsRight;
    int			fpDistanceFromTextTwipsTop;
    int			fpDistanceFromTextTwipsBottom;
    unsigned char	fpDistanceFromTextSet;
#			define DISTsetLEFT	(1<<0)
#			define DISTsetRIGHT	(1<<1)
#			define DISTsetTOP	(1<<2)
#			define DISTsetBOTTOM	(1<<3)

    unsigned char	fpIsSet;
#   define DOCisFRAME( fp ) ( (fp)->fpIsSet != 0 )

    unsigned char	fpXReference;
    unsigned char	fpXPosition;

    unsigned char	fpYReference;
    unsigned char	fpYPosition;
    unsigned char	fpLockAnchor;

    unsigned char	fpFrameWrapStyle;
    unsigned char	fpNoOverlap;
    
    unsigned char	fpTextFlowDirection;
    } FrameProperties;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameXReference
    {
    FXrefMARGIN= 0,
    FXrefPAGE,
    FXrefCOLUMN,
#   define	FXref__FRAME_COUNT	(FXrefCOLUMN+1)
    FXrefCHARACTER,
    FXrefIGNORE,

    FXref_COUNT
    } FrameXReference;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameYReference
    {
    FYrefMARGIN= 0,
    FYrefPAGE,
    FYrefPARA,
#   define	FYref__FRAME_COUNT	(FYrefPARA+1)
		    /**
		     *  Position relative to the line of the anchor.
		     *  Experimenting with Word 2003 shows that for 
		     *  shapes the deepest level is positioning relative
		     *  to the paragraph. (For shapes) In the user interface,
		     *  it is possible to select the line. If you select
		     *  positioning relative to the line, MS-Word calculates 
		     *  the vertical offset relative to the paragraph.
		     */
    FYrefLINE,
    FYrefIGNORE,

    FYref_COUNT
    } FrameYReference;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameXPosition
    {
    FXposXGIVEN= 0,
    FXposXL,
    FXposXC,
    FXposXR,
    FXposXI,
    FXposXO,

    FXpos_COUNT
    } FrameXPosition;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameYPosition
    {
    FYposYGIVEN= 0,
    FYposYT,
    FYposYC,
    FYposYB,
    FYposYIN,
    FYposYOUT,

    FYpos_COUNT
    } FrameYPosition;

/************************************************************************/
/*									*/
/*  Property numbers and masks.						*/
/*									*/
/************************************************************************/

typedef enum TextFrameProperty
    {
    TFPprop_NONE= -1,

    TFPpropISSET= 0,

    TFPpropABSW,
    TFPpropABSH,

    TFPpropNO_OVERLAP,

    TFPpropHORIZONTAL_REF,
    TFPpropHORIZONTAL_POS,
    TFPpropHORIZONTAL_ALIGN,

    TFPpropVERTICAL_REF,
    TFPpropVERTICAL_POS,
    TFPpropVERTICAL_ALIGN,
    TFPpropLOCK_ANCHOR,

    TFPpropWRAP_STYLE,
    TFPpropDFRMTXTL,
    TFPpropDFRMTXTR,
    TFPpropDFRMTXTT,
    TFPpropDFRMTXTB,

    TFPpropTEXT_FLOW,

    TFPprop_COUNT,

    TFPprop_IGNORED,

    TFPprop_DFRMTXT,		/*  shortcut: read/write/tools only	*/
    TFPprop_DFRMTXTX,		/*  shortcut: read/write/tools only	*/
    TFPprop_DFRMTXTY,		/*  shortcut: read/write/tools only	*/

    TFPprop_FULL_COUNT
    } TextFrameProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFrameYAllowed(	int		pos,
				int		ref );

# define docCopyFrameProperties( to, from ) ( *(to)= *(from) )
# define docCleanFrameProperties( fp ) /* nothing */

extern void docInitFrameProperties(	FrameProperties *	fp );

extern int docSetFrameProperty(	FrameProperties *	fp,
				int			prop,
				int			arg );

extern int docGetFrameProperty(	const FrameProperties *	fp,
				int			prop );

#   endif	/*  DOC_FRAME_PROPERTIES_H	*/
