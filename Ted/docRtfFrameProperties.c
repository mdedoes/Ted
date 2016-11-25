/************************************************************************/
/*									*/
/*  Read/Write text frame properties to/from RTF.			*/
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
/*  Text Frame Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveTextFrameProperties( SimpleOutputStream *	sos,
				int *				pCol,
				const PropertyMask *		updMask,
				const TextFrameProperties *	tfp )
    {
    if  ( PROPmaskISSET( updMask, TFPpropABSW ) )
	{ docRtfWriteArgTag( "\\absw", pCol, tfp->tfpFrameWidthTwips, sos ); }

    if  ( PROPmaskISSET( updMask, TFPpropABSH ) )
	{ docRtfWriteArgTag( "\\absh", pCol, tfp->tfpFrameHeightTwips, sos ); }

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_REFERENCE ) )
	{
	switch( tfp->tfpHorizontalFrameReference )
	    {
	    case HFRphCOL:
		docRtfWriteTag( "\\phcol", pCol, sos );
		break;

	    case HFRphPG:
		docRtfWriteTag( "\\phpg", pCol, sos );
		break;

	    case HFRphMRG:
		docRtfWriteTag( "\\phmrg", pCol, sos );
		break;

	    default:
		LDEB(tfp->tfpHorizontalFrameReference);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_POSITION ) )
	{
	if  ( tfp->tfpFrameXTwips >= 0 )
	    { docRtfWriteArgTag( "\\posx", pCol, tfp->tfpFrameXTwips, sos ); }
	else{
	    docRtfWriteArgTag( "\\posxneg", pCol, -tfp->tfpFrameXTwips, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_ALIGNMENT ) )
	{
	switch( tfp->tfpHorizontalFrameAlignment )
	    {
	    case HFAposXL:
		docRtfWriteTag( "\\posxl", pCol, sos );
		break;

	    case HFAposXR:
		docRtfWriteTag( "\\posxr", pCol, sos );
		break;

	    case HFAposXC:
		docRtfWriteTag( "\\posxc", pCol, sos );
		break;

	    case HFAposXI:
		docRtfWriteTag( "\\posxi", pCol, sos );
		break;

	    case HFAposXO:
		docRtfWriteTag( "\\posxo", pCol, sos );
		break;

	    default:
		LDEB(tfp->tfpHorizontalFrameAlignment);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_REFERENCE ) )
	{
	switch( tfp->tfpVerticalFrameReference )
	    {
	    case VFRpvMRG:
		docRtfWriteTag( "\\pvmrg", pCol, sos );
		break;

	    case VFRpvPG:
		docRtfWriteTag( "\\pvpg", pCol, sos );
		break;

	    case VFRpvPARA:
		docRtfWriteTag( "\\pvpara", pCol, sos );
		break;

	    default:
		LDEB(tfp->tfpVerticalFrameReference);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_POSITION ) )
	{
	if  ( tfp->tfpFrameXTwips >= 0 )
	    { docRtfWriteArgTag( "\\posy", pCol, tfp->tfpFrameYTwips, sos ); }
	else{
	    docRtfWriteArgTag( "\\posyneg", pCol, -tfp->tfpFrameYTwips, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_ALIGNMENT ) )
	{
	switch( tfp->tfpVerticalFrameAlignment )
	    {
	    case VFAposYIL:
		docRtfWriteTag( "\\posyil", pCol, sos );
		break;

	    case VFAposYT:
		docRtfWriteTag( "\\posyt", pCol, sos );
		break;

	    case VFAposYC:
		docRtfWriteTag( "\\posyc", pCol, sos );
		break;

	    case VFAposYB:
		docRtfWriteTag( "\\posyb", pCol, sos );
		break;


	    default:
		LDEB(tfp->tfpVerticalFrameAlignment);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropLOCKED_TO_PARA ) )
	{
	if  ( tfp->tfpFrameLockedToParagraph )
	    { docRtfWriteTag( "\\abslock", pCol, sos );		 }
	else{
	    docRtfWriteArgTag( "\\abslock", pCol,
					tfp->tfpFrameLockedToParagraph, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropWRAP_STYLE ) )
	{
	switch( tfp->tfpFrameWrapStyle )
	    {
	    case FWSwrapWRAP:
		LDEB(tfp->tfpFrameWrapStyle);
		break;

	    case FWSwrapNOWRAP:
		docRtfWriteTag( "\\nowrap", pCol, sos );
		break;

	    case FWSwrapOVERLAY:
		docRtfWriteTag( "\\overlay", pCol, sos );
		break;

	    default:
		LDEB(tfp->tfpFrameWrapStyle);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropDXFRTEXT ) )
	{
	docRtfWriteArgTag( "\\dxfrtext", pCol,
				    tfp->tfpDistanceFromTextTwips, sos );
	}

    if  ( PROPmaskISSET( updMask, TFPpropDFRMTXTX ) )
	{
	docRtfWriteArgTag( "\\dfrmtxtx", pCol,
				    tfp->tfpHorDistanceFromTextTwips, sos );
	}

    if  ( PROPmaskISSET( updMask, TFPpropDFRMTXTY ) )
	{
	docRtfWriteArgTag( "\\dfrmtxty", pCol,
				    tfp->tfpVerDistanceFromTextTwips, sos );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a text frame property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberTextFrameProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    TextFrameProperties *	tfp= &(rrc->rrcTextFrameProperties);

    switch( rcw->rcwID )
	{
	case TFPpropABSW:
	    tfp->tfpFrameWidthTwips= arg;
	    break;

	case TFPpropABSH:
	    tfp->tfpFrameHeightTwips= arg;
	    break;

	case TFPpropHORIZONTAL_REFERENCE:
	    tfp->tfpHorizontalFrameReference= rcw->rcwEnumValue;
	    break;

	case TFPpropHORIZONTAL_POSITION:
	    tfp->tfpFrameXTwips= arg;
	    break;

	case TFPpropHORIZONTAL_ALIGNMENT:
	    tfp->tfpHorizontalFrameAlignment= rcw->rcwEnumValue;
	    break;

	case TFPpropVERTICAL_REFERENCE:
	    tfp->tfpVerticalFrameReference= rcw->rcwEnumValue;
	    break;

	case TFPpropVERTICAL_POSITION:
	    tfp->tfpFrameYTwips= arg;
	    break;

	case TFPpropVERTICAL_ALIGNMENT:
	    tfp->tfpVerticalFrameAlignment= rcw->rcwEnumValue;
	    break;

	case TFPpropLOCKED_TO_PARA:
	    tfp->tfpFrameLockedToParagraph= arg != 0;
	    break;

	case TFPpropWRAP_STYLE:
	    tfp->tfpFrameWrapStyle= rcw->rcwEnumValue;
	    break;

	case TFPpropDXFRTEXT:
	    tfp->tfpDistanceFromTextTwips= arg;
	    break;

	case TFPpropDFRMTXTX:
	    tfp->tfpHorDistanceFromTextTwips= arg;
	    break;

	case TFPpropDFRMTXTY:
	    tfp->tfpVerDistanceFromTextTwips= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return 0;
	}

    /*
    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );
    */

    return 0;
    }
