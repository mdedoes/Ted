/************************************************************************/
/*									*/
/*  Read/Write paragraph properties to/from RTF.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfControlWord.h"
#   include	<docBreakKind.h>
#   include	<docFrameProperties.h>
#   include	<utilPropMask.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/

void docRtfSaveParaTableNesting(	RtfWriter *		rw,
					int			nest )
    {
    if  ( rw->rwDeepestTableNesting < nest )
	{ rw->rwDeepestTableNesting=  nest;	}

    if  ( rw->rwDeepestTableNesting > 1 )
	{
	switch( nest )
	    {
	    case 0:
		docRtfWriteArgTag( rw, "itap", nest );
		break;
	    case 1:
	    default:
		docRtfWriteArgTag( rw, "intbl\\itap", nest );
		break;
	    }
	}
    else{
	switch( nest )
	    {
	    case 0:
		docRtfWriteArgTag( rw, "intbl", nest );
		break;
	    case 1:
		docRtfWriteTag( rw, "intbl" );
		break;
	    default:
		LLDEB(rw->rwDeepestTableNesting,nest);
		docRtfWriteArgTag( rw, "intbl\\itap", nest );
		break;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save Paragraph Properties.						*/
/*									*/
/************************************************************************/

static const int ParaRegularProperties[]=
{
    PPpropHYPHPAR,
    PPpropWIDCTLPAR,
    PPpropRTOL,
    PPpropKEEP,
    PPpropKEEPN,
    PPpropOUTLINELEVEL,
    PPpropFIRST_INDENT,
    PPpropLEFT_INDENT,
    PPpropRIGHT_INDENT,
    PPpropSPACE_BEFORE,
    PPpropSPACE_AFTER,
};

static const int ParaRegularPropertyCount=
			sizeof(ParaRegularProperties)/sizeof(int);

static const int ParaRegularBorders[]=
{
    PPpropTOP_BORDER,
    PPpropBOTTOM_BORDER,
    PPpropLEFT_BORDER,
    PPpropRIGHT_BORDER,
    PPpropBETWEEN_BORDER,
    PPpropBAR_BORDER,
};

static const int ParaRegularBorderCount=
			sizeof(ParaRegularBorders)/sizeof(int);

int docRtfSaveParagraphProperties(
				RtfWriter *			rw,
				const struct PropertyMask *	ppSetMask,
				const ParagraphProperties *	pp )
    {
    const int			scope= RTCscopePARA;

    if  ( PROPmaskISSET( ppSetMask, PPpropSTYLE ) )
	{
	if  ( docRtfWriteProperty( rw, scope,
					PPpropSTYLE, pp->ppStyleNumber ) )
	    { LLLDEB(scope,PPpropSTYLE,pp->ppStyleNumber); return -1;	}
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLISTOVERRIDE ) )
	{ docRtfWriteArgTag( rw, "ls", pp->ppListOverride ); }
    if  ( PROPmaskISSET( ppSetMask, PPpropLISTLEVEL ) )
	{ docRtfWriteArgTag( rw, "ilvl", pp->ppListLevel ); }

    if  ( PROPmaskISSET( ppSetMask, PPpropTABLE_NESTING ) )
	{ docRtfSaveParaTableNesting( rw, pp->ppTableNesting ); }

    if  ( docRtfWriteItemProperties( rw, scope, pp,
			(RtfGetIntProperty)docGetParaProperty, ppSetMask,
			ParaRegularProperties, ParaRegularPropertyCount ) )
	{ LLDEB(scope,ParaRegularPropertyCount); return -1;	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLINE_SPACING_DIST ) ||
	  PROPmaskISSET( ppSetMask, PPpropLINE_SPACING_MULT ) )
	{
	docRtfWriteArgTag( rw, "sl", pp->ppLineSpacingTwips );
	docRtfWriteArgTag( rw, "slmult", pp->ppLineSpacingIsMultiple );
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropALIGNMENT ) )
	{
	if  ( docRtfWriteProperty( rw, scope,
					PPpropALIGNMENT, pp->ppAlignment ) )
	    { LLLDEB(scope,PPpropALIGNMENT,pp->ppAlignment); return -1;	}
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropTAB_STOPS )	&&
	  pp->ppTabStopListNumber >= 0			)
	{
	const struct TabStopList *	tsl;

	tsl= docGetTabStopListByNumber( rw->rwDocument,
						    pp->ppTabStopListNumber );

	docRtfSaveTabStopList( rw, tsl );
	}

    if  ( docRtfWriteItemProperties( rw, scope, pp,
			(RtfGetIntProperty)docGetParaProperty, ppSetMask,
			ParaRegularBorders, ParaRegularBorderCount ) )
	{ LLDEB(scope,ParaRegularBorderCount); return -1;	}

    if  ( PROPmaskISSET( ppSetMask, PPpropFRAME ) )
	{
	const FrameProperties *	fp;

	fp= docGetFramePropertiesByNumber( rw->rwDocument, pp->ppFrameNumber );

	if  ( DOCisFRAME( fp ) )
	    { docRtfSaveParaFrameProperties( rw, fp );	}
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, RTCscopePARA_SHADING,
						    pp->ppShadingNumber );
	}

    /*  MUST be last, as DOCibkCOL results in a particule	*/
    if  ( PROPmaskISSET( ppSetMask, PPpropBREAK_KIND ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_ParaBreakTags, pp->ppBreakKind,
				DOCrtf_ParaBreakTagCount, DOCibkpara_COUNT );
	}

    return 0;
    }

