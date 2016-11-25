/************************************************************************/
/*									*/
/*  Read RTF shapes.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Translate shape type to string.					*/
/*									*/
/************************************************************************/

static char * docRtfShapeTypeString(	int	shapeType )
    {
    static char		scratch[12];

    switch( shapeType )
	{
	case SHPtyFREEFORM_OR_NON_AUTOSHAPE: return "FREEFORM_OR_NON_AUTOSHAPE";
	case SHPtyRECTANGLE: return "RECTANGLE";
	case SHPtyROUND_RECTANGLE: return "ROUND_RECTANGLE";
	case SHPtyELLIPSE: return "ELLIPSE";
	case SHPtyDIAMOND: return "DIAMOND";
	case SHPtyISOSCELES_TRIANGLE: return "ISOSCELES_TRIANGLE";
	case SHPtyRIGHT_TRIANGLE: return "RIGHT_TRIANGLE";
	case SHPtyPARALLELOGRAM: return "PARALLELOGRAM";
	case SHPtyTRAPEZOID: return "TRAPEZOID";
	case SHPtyHEXAGON: return "HEXAGON";
	case SHPtyOCTAGON: return "OCTAGON";
	case SHPtyPLUS_SIGN: return "PLUS_SIGN";
	case SHPtySTAR: return "STAR";
	case SHPtyARROW: return "ARROW";
	case SHPtyTHICK_ARROW: return "THICK_ARROW";
	case SHPtyHOME_PLATE: return "HOME_PLATE";
	case SHPtyCUBE: return "CUBE";
	case SHPtyBALLOON: return "BALLOON";
	case SHPtySEAL: return "SEAL";
	case SHPtyARC: return "ARC";
	case SHPtyLINE: return "LINE";
	case SHPtyPLAQUE: return "PLAQUE";
	case SHPtyCAN: return "CAN";
	case SHPtyDONUT: return "DONUT";
	case SHPtyTEXT_SIMPLE: return "TEXT_SIMPLE";
	case SHPtyTEXT_OCTAGON: return "TEXT_OCTAGON";
	case SHPtyTEXT_HEXAGON: return "TEXT_HEXAGON";
	case SHPtyTEXT_CURVE: return "TEXT_CURVE";
	case SHPtyTEXT_WAVE: return "TEXT_WAVE";
	case SHPtyTEXT_RING: return "TEXT_RING";
	case SHPtyTEXT_ON_CURVE: return "TEXT_ON_CURVE";
	case SHPtyTEXT_ON_RING: return "TEXT_ON_RING";
	case SHPtyCALLOUT_1: return "CALLOUT_1";
	case SHPtyCALLOUT_2: return "CALLOUT_2";
	case SHPtyCALLOUT_3: return "CALLOUT_3";
	case SHPtyACCENT_CALLOUT_1: return "ACCENT_CALLOUT_1";
	case SHPtyACCENT_CALLOUT_2: return "ACCENT_CALLOUT_2";
	case SHPtyACCENT_CALLOUT_3: return "ACCENT_CALLOUT_3";
	case SHPtyBORDER_CALLOUT_1: return "BORDER_CALLOUT_1";
	case SHPtyBORDER_CALLOUT_2: return "BORDER_CALLOUT_2";
	case SHPtyBORDER_CALLOUT_3: return "BORDER_CALLOUT_3";
	case SHPtyACCENT_BORDER_CALLOUT_1: return "ACCENT_BORDER_CALLOUT_1";
	case SHPtyACCENT_BORDER_CALLOUT_2: return "ACCENT_BORDER_CALLOUT_2";
	case SHPtyACCENT_BORDER_CALLOUT_3: return "ACCENT_BORDER_CALLOUT_3";
	case SHPtyRIBBON: return "RIBBON";
	case SHPtyRIBBON2: return "RIBBON2";
	case SHPtyCHEVRON: return "CHEVRON";
	case SHPtyPENTAGON: return "PENTAGON";
	case SHPtyNO_SMOKING: return "NO_SMOKING";
	case SHPtySEAL8: return "SEAL8";
	case SHPtySEAL16: return "SEAL16";
	case SHPtySEAL32: return "SEAL32";
	case SHPtyWEDGE_RECT_CALLOUT: return "WEDGE_RECT_CALLOUT";
	case SHPtyWEDGE_RRECT_CALLOUT: return "WEDGE_RRECT_CALLOUT";
	case SHPtyWEDGE_ELLIPSE_CALLOUT: return "WEDGE_ELLIPSE_CALLOUT";
	case SHPtyWAVE: return "WAVE";
	case SHPtyFOLDED_CORNER: return "FOLDED_CORNER";
	case SHPtyLEFT_ARROW: return "LEFT_ARROW";
	case SHPtyDOWN_ARROW: return "DOWN_ARROW";
	case SHPtyUP_ARROW: return "UP_ARROW";
	case SHPtyLEFT_RIGHT_ARROW: return "LEFT_RIGHT_ARROW";
	case SHPtyUP_DOWN_ARROW: return "UP_DOWN_ARROW";
	case SHPtyIRREGULARSEAL1: return "IRREGULARSEAL1";
	case SHPtyIRREGULARSEAL2: return "IRREGULARSEAL2";
	case SHPtyLIGHTNING_BOLT: return "LIGHTNING_BOLT";
	case SHPtyHEART: return "HEART";
	case SHPtyPICTURE_FRAME: return "PICTURE_FRAME";
	case SHPtyQUAD_ARROW: return "QUAD_ARROW";
	case SHPtyLEFT_ARROW_CALLOUT: return "LEFT_ARROW_CALLOUT";
	case SHPtyRIGHT_ARROW_CALLOUT: return "RIGHT_ARROW_CALLOUT";
	case SHPtyUP_ARROW_CALLOUT: return "UP_ARROW_CALLOUT";
	case SHPtyDOWN_ARROW_CALLOUT: return "DOWN_ARROW_CALLOUT";
	case SHPtyLEFT_RIGHT_ARROW_CALLOUT: return "LEFT_RIGHT_ARROW_CALLOUT";
	case SHPtyUP_DOWN_ARROW_CALLOUT: return "UP_DOWN_ARROW_CALLOUT";
	case SHPtyQUAD_ARROW_CALLOUT: return "QUAD_ARROW_CALLOUT";
	case SHPtyBEVEL: return "BEVEL";
	case SHPtyLEFT_BRACKET: return "LEFT_BRACKET";
	case SHPtyRIGHT_BRACKET: return "RIGHT_BRACKET";
	case SHPtyLEFT_BRACE: return "LEFT_BRACE";
	case SHPtyRIGHT_BRACE: return "RIGHT_BRACE";
	case SHPtyLEFT_UP_ARROW: return "LEFT_UP_ARROW";
	case SHPtyBENT_UP_ARROW: return "BENT_UP_ARROW";
	case SHPtyBENT_ARROW: return "BENT_ARROW";
	case SHPtySEAL24: return "SEAL24";
	case SHPtySTRIPED_RIGHT_ARROW: return "STRIPED_RIGHT_ARROW";
	case SHPtyNOTCHED_RIGHT_ARROW: return "NOTCHED_RIGHT_ARROW";
	case SHPtyBLOCK_ARC: return "BLOCK_ARC";
	case SHPtySMILEY_FACE: return "SMILEY_FACE";
	case SHPtyVERTICAL_SCROLL: return "VERTICAL_SCROLL";
	case SHPtyHORIZONTAL_SCROLL: return "HORIZONTAL_SCROLL";
	case SHPtyCIRCULAR_ARROW: return "CIRCULAR_ARROW";
	case SHPtyNOTCHED_CIRCULAR_ARROW: return "NOTCHED_CIRCULAR_ARROW";
	case SHPtyUTURN_ARROW: return "UTURN_ARROW";
	case SHPtyCURVED_RIGHT_ARROW: return "CURVED_RIGHT_ARROW";
	case SHPtyCURVED_LEFT_ARROW: return "CURVED_LEFT_ARROW";
	case SHPtyCURVED_UP_ARROW: return "CURVED_UP_ARROW";
	case SHPtyCURVED_DOWN_ARROW: return "CURVED_DOWN_ARROW";
	case SHPtyCLOUD_CALLOUT: return "CLOUD_CALLOUT";
	case SHPtyELLIPSE_RIBBON: return "ELLIPSE_RIBBON";
	case SHPtyELLIPSE_RIBBON_2: return "ELLIPSE_RIBBON_2";
	case SHPtyFLOW_CHART_PROCESS: return "FLOW_CHART_PROCESS";
	case SHPtyFLOW_CHART_DECISION: return "FLOW_CHART_DECISION";
	case SHPtyFLOW_CHART_INPUT_OUTPUT: return "FLOW_CHART_INPUT_OUTPUT";
	case SHPtyFLOW_CHART_PREDEFINED_PROCESS:
				    return "FLOW_CHART_PREDEFINED_PROCESS";
	case SHPtyFLOW_CHART_INTERNAL_STORAGE:
				    return "FLOW_CHART_INTERNAL_STORAGE";
	case SHPtyFLOW_CHART_DOCUMENT: return "FLOW_CHART_DOCUMENT";
	case SHPtyFLOW_CHART_MULTIDOCUMENT: return "FLOW_CHART_MULTIDOCUMENT";
	case SHPtyFLOW_CHART_TERMINATOR: return "FLOW_CHART_TERMINATOR";
	case SHPtyFLOW_CHART_PREPARATION: return "FLOW_CHART_PREPARATION";
	case SHPtyFLOW_CHART_MANUAL_INPUT: return "FLOW_CHART_MANUAL_INPUT";
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
				    return "FLOW_CHART_MANUAL_OPERATION";
	case SHPtyFLOW_CHART_CONNECTOR: return "FLOW_CHART_CONNECTOR";
	case SHPtyFLOW_CHART_PUNCHED_CARD: return "FLOW_CHART_PUNCHED_CARD";
	case SHPtyFLOW_CHART_PUNCHED_TAPE: return "FLOW_CHART_PUNCHED_TAPE";
	case SHPtyFLOW_CHART_SUMMING_JUNCTION:
				    return "FLOW_CHART_SUMMING_JUNCTION";
	case SHPtyFLOW_CHART_OR: return "FLOW_CHART_OR";
	case SHPtyFLOW_CHART_COLLATE: return "FLOW_CHART_COLLATE";
	case SHPtyFLOW_CHART_SORT: return "FLOW_CHART_SORT";
	case SHPtyFLOW_CHART_EXTRACT: return "FLOW_CHART_EXTRACT";
	case SHPtyFLOW_CHART_MERGE: return "FLOW_CHART_MERGE";
	case SHPtyFLOW_CHART_OFFLINE_STORAGE:
					return "FLOW_CHART_OFFLINE_STORAGE";
	case SHPtyFLOW_CHART_ONLINE_STORAGE: return "FLOW_CHART_ONLINE_STORAGE";
	case SHPtyFLOW_CHART_MAGNETIC_TAPE: return "FLOW_CHART_MAGNETIC_TAPE";
	case SHPtyFLOW_CHART_MAGNETIC_DISK: return "FLOW_CHART_MAGNETIC_DISK";
	case SHPtyFLOW_CHART_MAGNETIC_DRUM: return "FLOW_CHART_MAGNETIC_DRUM";
	case SHPtyFLOW_CHART_DISPLAY: return "FLOW_CHART_DISPLAY";
	case SHPtyFLOW_CHART_DELAY: return "FLOW_CHART_DELAY";
	case SHPtyTEXT_PLAIN_TEXT: return "TEXT_PLAIN_TEXT";
	case SHPtyTEXT_STOP: return "TEXT_STOP";
	case SHPtyTEXT_TRIANGLE: return "TEXT_TRIANGLE";
	case SHPtyTEXT_TRIANGLE_INVERTED: return "TEXT_TRIANGLE_INVERTED";
	case SHPtyTEXT_CHEVRON: return "TEXT_CHEVRON";
	case SHPtyTEXT_CHEVRON_INVERTED: return "TEXT_CHEVRON_INVERTED";
	case SHPtyTEXT_RING_INSIDE: return "TEXT_RING_INSIDE";
	case SHPtyTEXT_RING_OUTSIDE: return "TEXT_RING_OUTSIDE";
	case SHPtyTEXT_ARCH_UP_CURVE: return "TEXT_ARCH_UP_CURVE";
	case SHPtyTEXT_ARCH_DOWN_CURVE: return "TEXT_ARCH_DOWN_CURVE";
	case SHPtyTEXT_CIRCLE_CURVE: return "TEXT_CIRCLE_CURVE";
	case SHPtyTEXT_BUTTON_CURVE: return "TEXT_BUTTON_CURVE";
	case SHPtyTEXT_ARCH_UP_POUR: return "TEXT_ARCH_UP_POUR";
	case SHPtyTEXT_ARCH_DOWN_POUR: return "TEXT_ARCH_DOWN_POUR";
	case SHPtyTEXT_CIRCLE_POUR: return "TEXT_CIRCLE_POUR";
	case SHPtyTEXT_BUTTON_POUR: return "TEXT_BUTTON_POUR";
	case SHPtyTEXT_CURVE_UP: return "TEXT_CURVE_UP";
	case SHPtyTEXT_CURVE_DOWN: return "TEXT_CURVE_DOWN";
	case SHPtyTEXT_CASCADE_UP: return "TEXT_CASCADE_UP";
	case SHPtyTEXT_CASCADE_DOWN: return "TEXT_CASCADE_DOWN";
	case SHPtyTEXT_WAVE1: return "TEXT_WAVE1";
	case SHPtyTEXT_WAVE2: return "TEXT_WAVE2";
	case SHPtyTEXT_WAVE3: return "TEXT_WAVE3";
	case SHPtyTEXT_WAVE4: return "TEXT_WAVE4";
	case SHPtyTEXT_INFLATE: return "TEXT_INFLATE";
	case SHPtyTEXT_DEFLATE: return "TEXT_DEFLATE";
	case SHPtyTEXT_INFLATE_BOTTOM: return "TEXT_INFLATE_BOTTOM";
	case SHPtyTEXT_DEFLATE_BOTTOM: return "TEXT_DEFLATE_BOTTOM";
	case SHPtyTEXT_INFLATE_TOP: return "TEXT_INFLATE_TOP";
	case SHPtyTEXT_DEFLATE_TOP: return "TEXT_DEFLATE_TOP";
	case SHPtyTEXT_DEFLATE_INFLATE: return "TEXT_DEFLATE_INFLATE";
	case SHPtyTEXT_DEFLATE_INFLATE_DEFLATE:
					return "TEXT_DEFLATE_INFLATE_DEFLATE";
	case SHPtyTEXT_FADE_RIGHT: return "TEXT_FADE_RIGHT";
	case SHPtyTEXT_FADE_LEFT: return "TEXT_FADE_LEFT";
	case SHPtyTEXT_FADE_UP: return "TEXT_FADE_UP";
	case SHPtyTEXT_FADE_DOWN: return "TEXT_FADE_DOWN";
	case SHPtyTEXT_SLANT_UP: return "TEXT_SLANT_UP";
	case SHPtyTEXT_SLANT_DOWN: return "TEXT_SLANT_DOWN";
	case SHPtyTEXT_CAN_UP: return "TEXT_CAN_UP";
	case SHPtyTEXT_CAN_DOWN: return "TEXT_CAN_DOWN";
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
					return "FLOW_CHART_ALTERNATE_PROCESS";
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
					return "FLOW_CHART_OFF_PAGE_CONNECTOR";
	case SHPtyCALLOUT_90: return "CALLOUT_90";
	case SHPtyACCENT_CALLOUT_90: return "ACCENT_CALLOUT_90";
	case SHPtyBORDER_CALLOUT_90: return "BORDER_CALLOUT_90";
	case SHPtyACCENT_BORDER_CALLOUT_90: return "ACCENT_BORDER_CALLOUT_90";
	case SHPtyLEFT_RIGHT_UP_ARROW: return "LEFT_RIGHT_UP_ARROW";
	case SHPtySUN: return "SUN";
	case SHPtyMOON: return "MOON";
	case SHPtyBRACKET_PAIR: return "BRACKET_PAIR";
	case SHPtyBRACE_PAIR: return "BRACE_PAIR";
	case SHPtySEAL4: return "SEAL4";
	case SHPtyDOUBLE_WAVE: return "DOUBLE_WAVE";
	case SHPtyHOST_CONTROL: return "HOST_CONTROL";
	case SHPtyTEXT_BOX: return "TEXT_BOX";

	default:
	    sprintf( scratch, "SHPty%d", shapeType );
	    return scratch;
	}
    }

/************************************************************************/
/*									*/
/*  Property control words inside a shape.				*/
/*									*/
/************************************************************************/

int docRtfShpProperty(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case	SHPpropLEFT:
	    rrc->rrcShapeProperties.spTwipsLeftOfAnchor= arg;
	    break;
	case	SHPpropRIGHT:
	    rrc->rrcShapeProperties.spTwipsRightOfAnchor= arg;
	    break;
	case	SHPpropTOP:
	    rrc->rrcShapeProperties.spTwipsAboveAnchor= arg;
	    break;
	case	SHPpropBOTTOM:
	    rrc->rrcShapeProperties.spTwipsBelowAnchor= arg;
	    break;
	case	SHPpropFHDR:
	    break;

	case	SHPpropBX:
	    rrc->rrcShapeProperties.spHorizontalAttachment= rcw->rcwEnumValue;
	    break;

	case	SHPpropBY:
	    rrc->rrcShapeProperties.spVerticalAttachment= rcw->rcwEnumValue;
	    break;

	case	SHPpropWR:
	    rrc->rrcShapeProperties.spWrapStyle= arg;
	    break;
	case	SHPpropWRK:
	    rrc->rrcShapeProperties.spWrapSide= arg;
	    break;
	case	SHPpropZ:
	case	SHPpropFBLWTXT:
	    break;
	case	SHPpropLOCKANCHOR:
	    rrc->rrcShapeProperties.spLockAnchor= ( arg != 0 );
	    break;
	case	SHPpropLID:
	    break;
	default:
	    /* SLDEB(rcw->rcwWord,arg); */
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Text inside a shape.						*/
/*									*/
/************************************************************************/

static int docRtfShpText(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int			res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read an {\sn .... } {\sv .... } pair.				*/
/*									*/
/*  NOTE that the concept of an 'RtfControlWord' is abused for		*/
/*	 the shape property.						*/
/*									*/
/************************************************************************/

static int docRtfShapeArray(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		c;
    long	val;
    int		sign;

    int		size;
    int		count;

    int		i;

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    sign= 1; size= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}
    while( isdigit( c ) )
	{ size= 10* size+ c- '0'; c= sioInGetCharacter( sis ); }
    size *= sign;

    if  ( c != ';' )
	{ CDEB(c); return -1;	}
    c= sioInGetCharacter( sis );

    sign= 1; count= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}
    while( isdigit( c ) )
	{ count= 10* count+ c- '0'; c= sioInGetCharacter( sis ); }
    count *= sign;

    switch( size )
	{
	case 8:
	    for ( i= 0; i < count; i++ )
		{
		while( c == '\n' || c == '\r' )
		    {
		    if  ( c == '\n' )
			{ rrc->rrcCurrentLine++;	}
		    c= sioInGetCharacter( sis );
		    }

		if  ( c != ';' )
		    { CDEB(c); return -1;	}
		c= sioInGetCharacter( sis );
		if  ( c != '(' )
		    { CDEB(c); return -1;	}
		c= sioInGetCharacter( sis );

		sign= 1; val= 0;
		if  ( c == '-' )
		    { sign= -1; c= sioInGetCharacter( sis );	}
		while( isdigit( c ) )
		    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }
		val *= sign;
		if  ( c != ',' )
		    { CDEB(c); return -1;	}
		c= sioInGetCharacter( sis );

		sign= 1; val= 0;
		if  ( c == '-' )
		    { sign= -1; c= sioInGetCharacter( sis );	}
		while( isdigit( c ) )
		    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }
		val *= sign;

		if  ( c != ')' )
		    { CDEB(c); return -1;	}
		c= sioInGetCharacter( sis );
		}

	    if  ( c != '}' )
		{ CDEB(c); return -1;	}
	    sioInUngetLastRead( sis );
	    break;
	case 2:
	    for ( i= 0; i < count; i++ )
		{
		while( c == '\n' || c == '\r' )
		    {
		    if  ( c == '\n' )
			{ rrc->rrcCurrentLine++;	}
		    c= sioInGetCharacter( sis );
		    }

		if  ( c != ';' )
		    { CDEB(c); return -1;	}
		c= sioInGetCharacter( sis );

		sign= 1; val= 0;
		if  ( c == '-' )
		    { sign= -1; c= sioInGetCharacter( sis );	}
		while( isdigit( c ) )
		    { val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }
		val *= sign;

		if  ( c == '}' )
		    { break;	}
		}

	    if  ( c != '}' )
		{ CDEB(c); return -1;	}
	    sioInUngetLastRead( sis );
	    break;
	case 4:
	default:
	    LLDEB(count,size); return -1;
	}

    return 0;
    }

static int docRtfShapePicture(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				control;
    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				locArg= -1;
    int				c;

    control= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    /*
    LSDEB(rrc->rrcCurrentLine,controlWord);
    */

    if  ( strcmp( controlWord, "pict" ) )
	{ SDEB(controlWord); return -1;	}

    if  ( docRtfReadPict( sis, rcw, arg, rrc ) )
	{ SSDEB(rcw->rcwWord,controlWord); return -1;	}

    return 0;
    }

static int docRtfShapeString(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		c;

    c= sioInGetCharacter( sis );
    while( c != '}' )
	{
	if  ( c == EOF )
	    { LDEB(c); return -1;	}

	c= sioInGetCharacter( sis );
	}

    sioInUngetLastRead( sis );

    return 0;
    }

static int docRtfShapeNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int			c;
    long		val;
    int			sign= 1;

    DrawingShape *	ds= &(rrc->rrcShape);

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    val= 0;

    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}

    while( isdigit( c ) )
	{ val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }

    val *= sign;

    /* SLDEB(rcw->rcwWord,val); */

    sioInUngetLastRead( sis );

    switch( rcw->rcwID )
	{
	case RTFSHP_shapeType:
	    if  ( rcw->rcwID != RTFSHP_shapeType )
		{ SDEB(docRtfShapeTypeString(val));	}

	    ds->ds_shapeType= val;
	    break;

	case RTFSHP_hspNext:		ds->ds_hspNext= val;	break;

	case RTFSHP_relLeft:		ds->ds_relLeft= val;	break;
	case RTFSHP_relTop:		ds->ds_relTop= val;	break;
	case RTFSHP_relRight:		ds->ds_relRight= val;	break;
	case RTFSHP_relBottom:		ds->ds_relBottom= val;	break;

	case RTFSHP_dxTextLeft:		ds->ds_dxTextLeft= val;	break;
	case RTFSHP_dyTextTop:		ds->ds_dyTextTop= val;	break;
	case RTFSHP_dxTextRight:	ds->ds_dxTextRight= val;	break;
	case RTFSHP_dyTextBottom:	ds->ds_dyTextBottom= val; break;

	case RTFSHP_lineWidth:		ds->ds_lineWidth= val; break;
	case RTFSHP_shapePath:		ds->ds_shapePath= val; break;

	case RTFSHP_fFlipH:		ds->ds_fFlipH= val != 0; break;
	case RTFSHP_fFlipV:		ds->ds_fFlipV= val != 0; break;
	case RTFSHP_fRelFlipH:		ds->ds_fRelFlipH= val != 0; break;
	case RTFSHP_fRelFlipV:		ds->ds_fRelFlipV= val != 0; break;
	case RTFSHP_fLockAspectRatio:	ds->ds_fLockAspectRatio= val != 0;
					break;
	case RTFSHP_fArrowheadsOK:	ds->ds_fArrowheadsOK= val != 0;
					break;
	case RTFSHP_fFitShapeToText:	ds->ds_fFitShapeToText= val != 0;
					break;
	case RTFSHP_fLine:		ds->ds_fLine= val != 0;	break;
	case RTFSHP_fFillOK:		ds->ds_fFillOK= val != 0; break;
	case RTFSHP_fFilled:		ds->ds_fFilled= val != 0; break;

	case RTFSHP_pictureActive:
	case RTFSHP_pictureId:
	case RTFSHP_lTxid:
	    break;

	default:
	    /*SDEB(rcw->rcwWord);*/
	    break;
	}

    return 0;
    }

static int docRtfShapeColor(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		c;
    long	val;
    int		sign= 1;

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    val= 0;

    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}

    while( isdigit( c ) )
	{ val= 10* val+ c- '0'; c= sioInGetCharacter( sis ); }

    val *= sign;

    /* SLDEB(rcw->rcwWord,val); */

    sioInUngetLastRead( sis );

    return 0;
    }

static RtfControlWord	docRtfShapeProperties[]=
{
    { "posv",			RTFSHP_posv,
				DOClevPARA, docRtfShapeNumber, },
    { "posh",			RTFSHP_posh,
				DOClevPARA, docRtfShapeNumber, },
    { "posrelv",		RTFSHP_posrelv,
				DOClevPARA, docRtfShapeNumber, },
    { "posrelh",		RTFSHP_posrelh,
				DOClevPARA, docRtfShapeNumber, },
    { "fFlipV",			RTFSHP_fFlipV,
				DOClevPARA, docRtfShapeNumber, },
    { "fFlipH",			RTFSHP_fFlipH,
				DOClevPARA, docRtfShapeNumber, },
    { "shapeType",		RTFSHP_shapeType,
				DOClevPARA, docRtfShapeNumber, },
    { "dxWrapDistLeft",		RTFSHP_dxWrapDistLeft,
				DOClevPARA, docRtfShapeNumber, },
    { "dyWrapDistTop",		RTFSHP_dyWrapDistTop,
				DOClevPARA, docRtfShapeNumber, },
    { "dxWrapDistRight",	RTFSHP_dxWrapDistRight,
				DOClevPARA, docRtfShapeNumber, },
    { "dyWrapDistBottom",	RTFSHP_dyWrapDistBottom,
				DOClevPARA, docRtfShapeNumber, },
    { "fBehindDocument",	RTFSHP_fBehindDocument,
				DOClevPARA, docRtfShapeNumber, },
    { "fIsButton",		RTFSHP_fIsButton,
				DOClevPARA, docRtfShapeNumber, },
    { "fHidden",		RTFSHP_fHidden,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockRotation",		RTFSHP_fLockRotation,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockAspectRatio",	RTFSHP_fLockAspectRatio,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockAgainstSelect",	RTFSHP_fLockAgainstSelect,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockCropping",		RTFSHP_fLockCropping,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockVerticies",		RTFSHP_fLockVerticies,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockText",		RTFSHP_fLockText,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockAdjustHandles",	RTFSHP_fLockAdjustHandles,
				DOClevPARA, docRtfShapeNumber, },
    { "fLockAgainstGrouping",	RTFSHP_fLockAgainstGrouping,
				DOClevPARA, docRtfShapeNumber, },
    { "dxTextLeft",		RTFSHP_dxTextLeft,
				DOClevPARA, docRtfShapeNumber, },
    { "dyTextTop",		RTFSHP_dyTextTop,
				DOClevPARA, docRtfShapeNumber, },
    { "dxTextRight",		RTFSHP_dxTextRight,
				DOClevPARA, docRtfShapeNumber, },
    { "dyTextBottom",		RTFSHP_dyTextBottom,
				DOClevPARA, docRtfShapeNumber, },
    { "WrapText",		RTFSHP_WrapText,
				DOClevPARA, docRtfShapeNumber, },
    { "anchorText",		RTFSHP_anchorText,
				DOClevPARA, docRtfShapeNumber, },
    { "txflTextFlow",		RTFSHP_txflTextFlow,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextAlign",		RTFSHP_gtextAlign,
				DOClevPARA, docRtfShapeNumber, },
    { "fGtext",			RTFSHP_fGtext,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFVertical",		RTFSHP_gtextFVertical,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFKern",		RTFSHP_gtextFKern,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFTight",		RTFSHP_gtextFTight,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFStretch",		RTFSHP_gtextFStretch,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFShrinkFit",	RTFSHP_gtextFShrinkFit,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFBestFit",		RTFSHP_gtextFBestFit,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFNormalize",	RTFSHP_gtextFNormalize,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFDxMeasure",	RTFSHP_gtextFDxMeasure,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFBold",		RTFSHP_gtextFBold,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFItalic",		RTFSHP_gtextFItalic,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFUnderline",	RTFSHP_gtextFUnderline,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFShadow",		RTFSHP_gtextFShadow,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFSmallcaps",	RTFSHP_gtextFSmallcaps,
				DOClevPARA, docRtfShapeNumber, },
    { "gtextFStrikethrough",	RTFSHP_gtextFStrikethrough,
				DOClevPARA, docRtfShapeNumber, },
    { "pibFlags",		RTFSHP_pibFlags,
				DOClevPARA, docRtfShapeNumber, },
    { "pictureGray",		RTFSHP_pictureGray,
				DOClevPARA, docRtfShapeNumber, },
    { "pictureBiLevel",		RTFSHP_pictureBiLevel,
				DOClevPARA, docRtfShapeNumber, },
    { "geoLeft",		RTFSHP_geoLeft,
				DOClevPARA, docRtfShapeNumber, },
    { "geoTop",			RTFSHP_geoTop,
				DOClevPARA, docRtfShapeNumber, },
    { "geoRight",		RTFSHP_geoRight,
				DOClevPARA, docRtfShapeNumber, },
    { "geoBottom",		RTFSHP_geoBottom,
				DOClevPARA, docRtfShapeNumber, },
    { "fillType",		RTFSHP_fillType,
				DOClevPARA, docRtfShapeNumber, },
    { "fillblipflags",		RTFSHP_fillblipflags,
				DOClevPARA, docRtfShapeNumber, },
    { "fillWidth",		RTFSHP_fillWidth,
				DOClevPARA, docRtfShapeNumber, },
    { "fillHeight",		RTFSHP_fillHeight,
				DOClevPARA, docRtfShapeNumber, },
    { "fFilled",		RTFSHP_fFilled,
				DOClevPARA, docRtfShapeNumber, },
    { "lineType",		RTFSHP_lineType,
				DOClevPARA, docRtfShapeNumber, },
    { "lineblipflags",		RTFSHP_lineblipflags,
				DOClevPARA, docRtfShapeNumber, },
    { "lineFillWidth",		RTFSHP_lineFillWidth,
				DOClevPARA, docRtfShapeNumber, },
    { "lineFillHeight",		RTFSHP_lineFillHeight,
				DOClevPARA, docRtfShapeNumber, },
    { "lineFillDztype",		RTFSHP_lineFillDztype,
				DOClevPARA, docRtfShapeNumber, },
    { "lineMiterLimit",		RTFSHP_lineMiterLimit,
				DOClevPARA, docRtfShapeNumber, },
    { "lineWidth",		RTFSHP_lineWidth,
				DOClevPARA, docRtfShapeNumber, },
    { "lineStyle",		RTFSHP_lineStyle,
				DOClevPARA, docRtfShapeNumber, },
    { "lineDashing",		RTFSHP_lineDashing,
				DOClevPARA, docRtfShapeNumber, },
    { "lineStartArrowhead",	RTFSHP_lineStartArrowhead,
				DOClevPARA, docRtfShapeNumber, },
    { "lineEndArrowhead",	RTFSHP_lineEndArrowhead,
				DOClevPARA, docRtfShapeNumber, },
    { "lineStartArrowWidth",	RTFSHP_lineStartArrowWidth,
				DOClevPARA, docRtfShapeNumber, },
    { "lineStartArrowLength",	RTFSHP_lineStartArrowLength,
				DOClevPARA, docRtfShapeNumber, },
    { "lineEndArrowWidth",	RTFSHP_lineEndArrowWidth,
				DOClevPARA, docRtfShapeNumber, },
    { "lineEndArrowLength",	RTFSHP_lineEndArrowLength,
				DOClevPARA, docRtfShapeNumber, },
    { "lineCrMod", 		RTFSHP_lineCrMod,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "lineFillBlipFlags", 	RTFSHP_lineFillBlipFlags,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fLine",			RTFSHP_fLine,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowType",		RTFSHP_shadowType,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowOffsetX",		RTFSHP_shadowOffsetX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowOffsetY",		RTFSHP_shadowOffsetY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowSecondOffsetX",	RTFSHP_shadowSecondOffsetX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowSecondOffsetY",	RTFSHP_shadowSecondOffsetY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowScaleXToX",	RTFSHP_shadowScaleXToX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowScaleYToX",	RTFSHP_shadowScaleYToX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowScaleXToY",	RTFSHP_shadowScaleXToY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowScaleYToY",	RTFSHP_shadowScaleYToY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowPerspectiveX",	RTFSHP_shadowPerspectiveX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowPerspectiveY",	RTFSHP_shadowPerspectiveY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowWeight",		RTFSHP_shadowWeight,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowOriginX",		RTFSHP_shadowOriginX,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowOriginY",		RTFSHP_shadowOriginY,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowCrMod",		RTFSHP_shadowCrMod,
				DOClevPARA, docRtfShapeNumber, },
    { "shadowOpacity",		RTFSHP_shadowOpacity,
				DOClevPARA, docRtfShapeNumber, },
    { "fShadow",		RTFSHP_fShadow,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DEdgeThickness",	RTFSHP_c3DEdgeThickness,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DExtrudeForward",	RTFSHP_c3DExtrudeForward,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DExtrudeBackward",	RTFSHP_c3DExtrudeBackward,
				DOClevPARA, docRtfShapeNumber, },
    { "f3D",			RTFSHP_f3D,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DMetallic",		RTFSHP_fc3DMetallic,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DUseExtrusionColor",	RTFSHP_fc3DUseExtrusionColor,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DLightFace",		RTFSHP_fc3DLightFace,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DRotationAxisX",	RTFSHP_c3DRotationAxisX,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DRotationAxisY",	RTFSHP_c3DRotationAxisY,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DRotationAxisZ",	RTFSHP_c3DRotationAxisZ,
				DOClevPARA, docRtfShapeNumber, },
    { "fC3DRotationCenterAut",	RTFSHP_fC3DRotationCenterAut,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DRotationCenterZ",	RTFSHP_c3DRotationCenterZ,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DXViewpoint",		RTFSHP_c3DXViewpoint,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DYViewpoint",		RTFSHP_c3DYViewpoint,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DZViewpoint",		RTFSHP_c3DZViewpoint,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DKeyX",		RTFSHP_c3DKeyX,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DKeyY",		RTFSHP_c3DKeyY,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DKeyZ",		RTFSHP_c3DKeyZ,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DFillX",		RTFSHP_c3DFillX,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DFillY",		RTFSHP_c3DFillY,
				DOClevPARA, docRtfShapeNumber, },
    { "c3DFillZ",		RTFSHP_c3DFillZ,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DParallel",		RTFSHP_fc3DParallel,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DKeyHarsh",		RTFSHP_fc3DKeyHarsh,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DFillHarsh",		RTFSHP_fc3DFillHarsh,
				DOClevPARA, docRtfShapeNumber, },
    { "fc3DConstrainRotation",	RTFSHP_fc3DConstrainRotation,
				DOClevPARA, docRtfShapeNumber, },  /*  ?  */
    { "fc3DRotationCenterAuto",	RTFSHP_fc3DRotationCenterAuto,
				DOClevPARA, docRtfShapeNumber, },  /*  ?  */
    { "spcot",			RTFSHP_spcot,
				DOClevPARA, docRtfShapeNumber, },
    { "dxyCalloutGap",		RTFSHP_dxyCalloutGap,
				DOClevPARA, docRtfShapeNumber, },
    { "spcoa",			RTFSHP_spcoa,
				DOClevPARA, docRtfShapeNumber, },
    { "spcod",			RTFSHP_spcod,
				DOClevPARA, docRtfShapeNumber, },
    { "dxyCalloutDropSpecified", RTFSHP_dxyCalloutDropSpecified,
				DOClevPARA, docRtfShapeNumber, },
    { "dxyCalloutLengthSpecified", RTFSHP_dxyCalloutLengthSpecified,
				DOClevPARA, docRtfShapeNumber, },
    { "fCallout",		RTFSHP_fCallout,
				DOClevPARA, docRtfShapeNumber, },
    { "fCalloutAccentBar",	RTFSHP_fCalloutAccentBar,
				DOClevPARA, docRtfShapeNumber, },
    { "fCalloutTextBorder",	RTFSHP_fCalloutTextBorder,
				DOClevPARA, docRtfShapeNumber, },
    { "fCalloutDropAuto",	RTFSHP_fCalloutDropAuto,
				DOClevPARA, docRtfShapeNumber, },
    { "fCalloutLengthSpecified", RTFSHP_fCalloutLengthSpecified,
				DOClevPARA, docRtfShapeNumber, },

    { "pib",			RTFSHP_pib,
				DOClevPARA, docRtfShapePicture, },

    { "pictureId", 		RTFSHP_pictureId,
				DOClevPARA, docRtfShapeNumber, },
    { "pictureActive", 		RTFSHP_pictureActive,
				DOClevPARA, docRtfShapeNumber, },
    { "fHitTestFill", 		RTFSHP_fHitTestFill,
				DOClevPARA, docRtfShapeNumber, },
    { "fillShape", 		RTFSHP_fillShape,
				DOClevPARA, docRtfShapeNumber, },
    { "fillUseRect", 		RTFSHP_fillUseRect,
				DOClevPARA, docRtfShapeNumber, },
    { "fNoFillHitTest", 	RTFSHP_fNoFillHitTest,
				DOClevPARA, docRtfShapeNumber, },
    { "fArrowheadsOK", 		RTFSHP_fArrowheadsOK,
				DOClevPARA, docRtfShapeNumber, },
    { "fHitTestLine", 		RTFSHP_fHitTestLine,
				DOClevPARA, docRtfShapeNumber, },
    { "lineFillShape", 		RTFSHP_lineFillShape,
				DOClevPARA, docRtfShapeNumber, },
    { "fNoLineDrawDash", 	RTFSHP_fNoLineDrawDash,
				DOClevPARA, docRtfShapeNumber, },
    { "fshadowObscured", 	RTFSHP_fshadowObscured,
				DOClevPARA, docRtfShapeNumber, },
    { "fEditedWrap", 		RTFSHP_fEditedWrap,
				DOClevPARA, docRtfShapeNumber, },
    { "fOnDblClickNotify", 	RTFSHP_fOnDblClickNotify,
				DOClevPARA, docRtfShapeNumber, },
    { "fOneD", 			RTFSHP_fOneD,
				DOClevPARA, docRtfShapeNumber, },
    { "fPrint", 		RTFSHP_fPrint,
				DOClevPARA, docRtfShapeNumber, },

    { "groupLeft", 		RTFSHP_groupLeft,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "groupTop", 		RTFSHP_groupTop,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "groupRight", 		RTFSHP_groupRight,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "groupBottom", 		RTFSHP_groupBottom,
				DOClevPARA, docRtfShapeNumber, }, /* ? */

    { "relLeft", 		RTFSHP_relLeft,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "relTop", 		RTFSHP_relTop,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "relRight", 		RTFSHP_relRight,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "relBottom", 		RTFSHP_relBottom,
				DOClevPARA, docRtfShapeNumber, }, /* ? */

    { "fRelFlipH", 		RTFSHP_fRelFlipH,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fRelFlipV", 		RTFSHP_fRelFlipV,
				DOClevPARA, docRtfShapeNumber, }, /* ? */

    { "pictureContrast", 	RTFSHP_pictureContrast,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "pictureBrightness", 	RTFSHP_pictureBrightness,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "pictureDblCrMod", 	RTFSHP_pictureDblCrMod,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "pictureFillCrMod", 	RTFSHP_pictureFillCrMod,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "pictureLineCrMod", 	RTFSHP_pictureLineCrMod,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillShadeType", 		RTFSHP_fillShadeType,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fSelectText", 		RTFSHP_fSelectText,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fRotateText", 		RTFSHP_fRotateText,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fFitShapeToText", 	RTFSHP_fFitShapeToText,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fFitTextToShape", 	RTFSHP_fFitTextToShape,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fAutoTextMargin", 	RTFSHP_fAutoTextMargin,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fCalloutMinusX", 	RTFSHP_fCalloutMinusX,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fCalloutMinusY", 	RTFSHP_fCalloutMinusY,
				DOClevPARA, docRtfShapeNumber, }, /* ? */

    { "shapePath", 		RTFSHP_shapePath,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fFillOK", 		RTFSHP_fFillOK,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "lTxid", 			RTFSHP_lTxid,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "hspNext", 		RTFSHP_hspNext,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "lidRegroup", 		RTFSHP_lidRegroup,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "lineOpacity", 		RTFSHP_lineOpacity,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillOpacity", 		RTFSHP_fillOpacity,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillBackOpacity", 	RTFSHP_fillBackOpacity,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillCrMod", 		RTFSHP_fillCrMod,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillBlipFlags", 		RTFSHP_fillBlipFlags,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillToLeft", 		RTFSHP_fillToLeft,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillToTop", 		RTFSHP_fillToTop,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillToRight", 		RTFSHP_fillToRight,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillToBottom", 		RTFSHP_fillToBottom,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillRectLeft", 		RTFSHP_fillRectLeft,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillRectTop", 		RTFSHP_fillRectTop,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillRectRight", 		RTFSHP_fillRectRight,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillRectBottom", 	RTFSHP_fillRectBottom,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillDztype", 		RTFSHP_fillDztype,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillShadePreset", 	RTFSHP_fillShadePreset,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillOriginX", 		RTFSHP_fillOriginX,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillOriginY", 		RTFSHP_fillOriginY,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillShapeOriginX", 	RTFSHP_fillShapeOriginX,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "fillShapeOriginY", 	RTFSHP_fillShapeOriginY,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "lineEndCapStyle", 	RTFSHP_lineEndCapStyle,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "rotation",	 	RTFSHP_rotation,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "cdirFont",	 	RTFSHP_cdirFont,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "cropFromLeft",	 	RTFSHP_cropFromLeft,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "cropFromRight",	 	RTFSHP_cropFromRight,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "cropFromTop",	 	RTFSHP_cropFromTop,
				DOClevPARA, docRtfShapeNumber, }, /* ? */
    { "cropFromBottom",	 	RTFSHP_cropFromBottom,
				DOClevPARA, docRtfShapeNumber, }, /* ? */

    { "fillAngle", 		RTFSHP_fillAngle,
				DOClevPARA, docRtfShapeNumber, },
    { "fillFocus", 		RTFSHP_fillFocus,
				DOClevPARA, docRtfShapeNumber, },

    { "pWrapPolygonVertices", 	RTFSHP_pWrapPolygonVertices,
				DOClevPARA, docRtfShapeArray, },
    { "pVerticies",	 	RTFSHP_pVerticies,
				DOClevPARA, docRtfShapeArray, },
    { "pSegmentInfo",	 	RTFSHP_pSegmentInfo,
				DOClevPARA, docRtfShapeArray, },

    { "WzName",			RTFSHP_WzName,
				DOClevPARA, docRtfShapeString, },
    { "gtextUNICODE",		RTFSHP_gtextUNICODE,
				DOClevPARA, docRtfShapeString, },
    { "gtextFont",		RTFSHP_gtextFont,
				DOClevPARA, docRtfShapeString, },
    { "pibName",		RTFSHP_pibName,
				DOClevPARA, docRtfShapeString, },
    { "fillBlipName",		RTFSHP_fillBlipName,
				DOClevPARA, docRtfShapeString, },

    { "pictureTransparent",	RTFSHP_pictureTransparent,
				DOClevPARA, docRtfShapeColor, },
    { "fillColor",		RTFSHP_fillColor,
				DOClevPARA, docRtfShapeColor, },
    { "fillBackColor",		RTFSHP_fillBackColor,
				DOClevPARA, docRtfShapeColor, },
    { "lineColor",		RTFSHP_lineColor,
				DOClevPARA, docRtfShapeColor, },
    { "lineBackColor",		RTFSHP_lineBackColor,
				DOClevPARA, docRtfShapeColor, },
    { "shadowColor",		RTFSHP_shadowColor,
				DOClevPARA, docRtfShapeColor, },
    { "shadowHighlight",	RTFSHP_shadowHighlight,
				DOClevPARA, docRtfShapeColor, },
    { "c3DExtrusionColor",	RTFSHP_c3DExtrusionColor,
				DOClevPARA, docRtfShapeColor, },

    { "c3DSpecularAmt",		RTFSHP_c3DSpecularAmt,
				DOClevPARA, docRtfShapeColor, },
    { "c3DDiffuseAmt",		RTFSHP_c3DDiffuseAmt,
				DOClevPARA, docRtfShapeColor, },
    { "c3DShininess",		RTFSHP_c3DShininess,
				DOClevPARA, docRtfShapeColor, },
    { "c3DExtrudePlane",	RTFSHP_c3DExtrudePlane,
				DOClevPARA, docRtfShapeColor, },
    { "c3DCrMod",		RTFSHP_c3DCrMod,
				DOClevPARA, docRtfShapeColor, },
    { "c3DYRotationAngle",	RTFSHP_c3DYRotationAngle,
				DOClevPARA, docRtfShapeColor, },
    { "c3DXRotationAngle",	RTFSHP_c3DXRotationAngle,
				DOClevPARA, docRtfShapeColor, },
    { "c3DRotationAngle",	RTFSHP_c3DRotationAngle,
				DOClevPARA, docRtfShapeColor, },
    { "c3DRotationCenterX",	RTFSHP_c3DRotationCenterX,
				DOClevPARA, docRtfShapeColor, },
    { "c3DRotationCenterY",	RTFSHP_c3DRotationCenterY,
				DOClevPARA, docRtfShapeColor, },
    { "c3DRenderMode",		RTFSHP_c3DRenderMode,
				DOClevPARA, docRtfShapeColor, },
    { "c3DTolerance",		RTFSHP_c3DTolerance,
				DOClevPARA, docRtfShapeColor, },
    { "c3DOriginX",		RTFSHP_c3DOriginX,
				DOClevPARA, docRtfShapeColor, },
    { "c3DOriginY",		RTFSHP_c3DOriginY,
				DOClevPARA, docRtfShapeColor, },
    { "c3DSkewAngle",		RTFSHP_c3DSkewAngle,
				DOClevPARA, docRtfShapeColor, },
    { "c3DSkewAmount",		RTFSHP_c3DSkewAmount,
				DOClevPARA, docRtfShapeColor, },
    { "c3DAmbientIntensity",	RTFSHP_c3DAmbientIntensity,
				DOClevPARA, docRtfShapeColor, },
    { "c3DKeyIntensity",	RTFSHP_c3DKeyIntensity,
				DOClevPARA, docRtfShapeColor, },
    { "c3DFillIntensity",	RTFSHP_c3DFillIntensity,
				DOClevPARA, docRtfShapeColor, },

    { "bWMode",			RTFSHP_bWMode,
				DOClevPARA, docRtfShapeNumber, },
    { "fLayoutInCell",		RTFSHP_fLayoutInCell,
				DOClevPARA, docRtfShapeNumber, },
};

static int docRtfShapeProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				control;
    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				locArg= -1;
    int				c;

    int				i;

    const int			listOnly= 1;

    const RtfControlWord *	rcwSn;

    control= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sn" ) )
	{ SDEB(controlWord); return -1;	}

    i= 0;
    for (;;)
	{
	c= sioInGetCharacter( sis );
	if  ( c == EOF )
	    { LDEB(c); return -1;	}
	if  ( i >= TEDszRTFCONTROL )
	    {
	    LLDEB(i,TEDszRTFCONTROL);
	    controlWord[i]= '\0';
	    SDEB(controlWord); return -1;
	    }
	if  ( c == '}' )
	    { controlWord[i]= '\0'; break;	}

	controlWord[i++]= c;
	}

    rcwSn= docRtfFindWord( controlWord, docRtfShapeProperties, listOnly );
    if  ( ! rcwSn )
	{ SDEB(controlWord);	}

    control= docRtfFindControl( sis, rrc, &c,
					    controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sv" ) )
	{ SDEB(controlWord); return -1;	}

    if  ( rcwSn )
	{
	if  ( (*rcwSn->rcwApply)( sis, rcwSn, 0, rrc ) )
	    {  SDEB(controlWord); return -1;	}

	c= sioInGetCharacter( sis );
	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetCharacter( sis );
	    }
	
	if  ( c != '}' )
	    { CDEB(c); return -1;	}
	}
    else{
	if  ( docRtfSkipGroup( sis, rcw, DOClevTEXT, rrc ) )
	    { LDEB(1); return -1;	}
	}

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}
    
    if  ( c != '}' )
	{ CDEB(c); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read shape instructions.						*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShpinstGroups[]=
    {
	{ "shptxt",	RTFidSHPTXT,	DOClevPARA, docRtfShpText, },
	{ "sp",		RTFidSP,	DOClevPARA, docRtfShapeProperty, },
    };

static int docRtfShpinst(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, docRtfShpinstGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfShprslt(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    rrc->rrcInIgnoredGroup++;

    res= docRtfReadGroup( sis, DOClevANY,
				(RtfControlWord *)0, 0, 0, rrc,
				(const RtfControlWord *)0,
				(const RtfControlWord *)0,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcInIgnoredGroup--;

    return res;
    }

/************************************************************************/
/*									*/
/*  The actual shape as opposed to the result in simpler RTF.		*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShapeGroups[]=
    {
	{ "shptxt",	RTFidSHPTXT,	DOClevPARA, docRtfShpText, },
	{ "shprslt",	RTFidSHPRSLT,	DOClevPARA, docRtfShprslt, },
	{ "shpgrp",	RTFidSHPGRP,	DOClevPARA, docRtfReadShape, },
	{ "shp",	RTFidSHP,	DOClevPARA, docRtfReadShape, },
	{ "shpinst",	RTFidSHPINST,	DOClevPARA, docRtfShpinst, },

	{ 0, 0, 0, }
    };

int docRtfReadShape(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc	)
    {
    int			res;

    res= docRtfReadGroup( sis, DOClevTEXT,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, docRtfShapeGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

