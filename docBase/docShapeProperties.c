/************************************************************************/
/*									*/
/*  Shapes.. Root properties.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docFrameProperties.h"
#   include	"docShapeProperties.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

void docInitShapeProperties(	ShapeProperties *	sp )
    {
    geoInitRectangle( &(sp->spRect) );

    sp->spXReference= FXrefCOLUMN;
    sp->spYReference= FYrefPARA;
    sp->spWrapStyle= SHPswtTOPBOTTOM;
    sp->spWrapSide= SHPswsBOTH;
    sp->spLockAnchor= 0;

    sp->spID= 0;
    sp->spZ= 0;

    sp->spShapeBelowText= 0;
    sp->spInHeader= 0;
    sp->spLockAnchor= 0;

    return;
    }

int docSetShapeProperty(	ShapeProperties *	sp,
				int			prop,
				int			value )
    {
    switch( prop )
	{
	case SHPpropLEFT:
	    sp->spRect.drX0= value;
	    return 0;
	case SHPpropRIGHT:
	    sp->spRect.drX1= value;
	    return 0;
	case SHPpropTOP:
	    sp->spRect.drY0= value;
	    return 0;
	case SHPpropBOTTOM:
	    sp->spRect.drY1= value;
	    return 0;

	case SHPpropLID:
	    sp->spID= value;
	    return 0;
	case SHPpropZ:
	    sp->spZ= value;
	    return 0;

	case SHPpropFHDR:
	    sp->spInHeader= value != 0;
	    return 0;

	case SHPpropBX:
	    sp->spXReference= value;
	    return 0;

	case SHPpropBY:
	    sp->spYReference= value;
	    return 0;

	case SHPpropWR:
	    sp->spWrapStyle= value;
	    return 0;
	case SHPpropWRK:
	    sp->spWrapSide= value;
	    return 0;

	case SHPpropFBLWTXT:
	    sp->spShapeBelowText= ( value != 0 );
	    return 0;
	case SHPpropLOCKANCHOR:
	    sp->spLockAnchor= ( value != 0 );
	    return 0;

	default:
	    LLDEB(prop,value); return -1;
	}
    }

int docGetShapeProperty(	const ShapeProperties *	sp,
				int			prop )
    {
    switch( prop )
	{
	case SHPpropLEFT:
	    return sp->spRect.drX0;
	case SHPpropRIGHT:
	    return sp->spRect.drX1;
	case SHPpropTOP:
	    return sp->spRect.drY0;
	case SHPpropBOTTOM:
	    return sp->spRect.drY1;

	case SHPpropLID:
	    return sp->spID;
	case SHPpropZ:
	    return sp->spZ;

	case SHPpropFHDR:
	    return ( sp->spInHeader != 0 );

	case SHPpropBX:
	    return sp->spXReference;
	case SHPpropBY:
	    return sp->spYReference;

	case SHPpropWR:
	    return sp->spWrapStyle;
	case SHPpropWRK:
	    return sp->spWrapSide;

	case SHPpropFBLWTXT:
	    return ( sp->spShapeBelowText != 0 );
	case SHPpropLOCKANCHOR:
	    return ( sp->spLockAnchor != 0 );

	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Compare shape properties.						*/
/*									*/
/************************************************************************/

static int DocShapeIntProps[]=
    {
    SHPpropLEFT,
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
    };

static const int DocShapeIntPropCount= sizeof(DocShapeIntProps)/sizeof(int);

void docShapePropertyDifference( PropertyMask *			pDifMask,
				const ShapeProperties *		spTo,
				const PropertyMask *		spCmpMask,
				const ShapeProperties *		spFrom )
    {
    PropertyMask		spDifMask;
    int				p;

    utilPropMaskClear( &spDifMask );

    for ( p= 0; p < DocShapeIntPropCount; p++ )
	{
	const int	prop= DocShapeIntProps[p];

	if  ( PROPmaskISSET( spCmpMask, prop ) )
	    {
	    int		from= docGetShapeProperty( spFrom, prop );
	    int		to= docGetShapeProperty( spTo, prop );

	    if  ( to != from )
		{ PROPmaskADD( &spDifMask, prop );	}
	    }
	}

    *pDifMask= spDifMask;
    return;
    }

