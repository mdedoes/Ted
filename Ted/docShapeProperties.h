/************************************************************************/
/*									*/
/*  Shape Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_PROPERTIES_H
#   define	DOC_SHAPE_PROPERTIES_H

/************************************************************************/
/*									*/
/*  Primitives for shapes.						*/
/*									*/
/************************************************************************/

typedef enum ShapeHorizontalAttachment
    {
    SHPshaPAGE= 0,
    SHPshaMARGIN,
    SHPshaCOLUMN,
    SHPshaIGNORE,

    SHPsha_COUNT
    } ShapeHorizontalAttachment;

typedef enum ShapeVerticalAttachment
    {
    SHPsvaPAGE= 0,
    SHPsvaMARGIN,
    SHPsvaPARAGRAPH,
    SHPsvaIGNORE,

    SHPsva_COUNT
    } ShapeVerticalAttachment;

typedef enum ShapeWrapStyle
    {
    SHPswsTOPBOTTOM= 1,
    SHPswsAROUND,
    SHPswsNONE,
    SHPswsTIGHTLY,
    SHPswsTHROUGH
    } ShapeWrapStyle;

typedef enum ShapeWrapSide
    {
    SHPswsBOTH= 0,
    SHPswsLEFT,
    SHPswsRIGHT,
    SHPswsLARGEST
    } ShapeWrapSide;

typedef struct ShapeProperties
    {
    int			spTwipsLeftOfAnchor;
    int			spTwipsAboveAnchor;
    int			spTwipsBelowAnchor;
    int			spTwipsRightOfAnchor;

    unsigned int	spHorizontalAttachment:2;	/*  (enum)	*/
    unsigned int	spVerticalAttachment:2;		/*  (enum)	*/
    unsigned int	spWrapStyle:3;			/*  (enum)	*/
    unsigned int	spWrapSide:2;			/*  (enum)	*/
    unsigned int	spLockAnchor:1;
    } ShapeProperties;

/************************************************************************/
/*									*/
/*  Numbers of shape properies.					*/
/*									*/
/************************************************************************/

typedef enum ShapeProperty
    {
    SHPpropLEFT= 0,
    SHPpropTOP,
    SHPpropBOTTOM,
    SHPpropRIGHT,
    SHPpropLID,
    SHPpropZ,
    SHPpropFHDR,
    SHPpropBX,
    SHPpropBY,
    SHPpropWR,
    SHPpropWRK,
    SHPpropFBLWTXT,
    SHPpropLOCKANCHOR,

    SHPprop_COUNT
    } ShapeProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeProperties(	ShapeProperties *	sp );

#   endif	/*  DOC_SHAPE_PROPERTIES_H	*/
