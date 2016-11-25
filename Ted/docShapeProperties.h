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
    SHPshaCOLUMN
    } ShapeHorizontalAttachment;

typedef enum ShapeVerticalAttachment
    {
    SHPsvaPAGE= 0,
    SHPsvaMARGIN,
    SHPsvaPARAGRAPH
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
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeProperties(	ShapeProperties *	sp );

#   endif	/*  DOC_SHAPE_PROPERTIES_H	*/
