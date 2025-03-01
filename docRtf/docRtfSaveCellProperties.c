/************************************************************************/
/*									*/
/*  Read/Write paragraph cell to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	<docCellProperties.h>
#   include	<utilPropMask.h>
#   include	"docRtfControlWord.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save Cell Properties.						*/
/*									*/
/************************************************************************/

static const int CellRegularProperties[]=
{
    CLpropVALIGN,
    CLpropHOR_MERGE,
    CLpropVERT_MERGE,
    CLpropTOP_BORDER,
    CLpropLEFT_BORDER,
    CLpropRIGHT_BORDER,
    CLpropBOTTOM_BORDER,
    CLpropLEFT_RIGHT_DIAGONAL,
    CLpropRIGHT_LEFT_DIAGONAL,
    CLpropROW_HEADER,
};

static const int CellRegularPropertyCount=
			sizeof(CellRegularProperties)/sizeof(int);

int docRtfSaveCellProperties(	RtfWriter *			rw,
				const struct PropertyMask *	cpSetMask,
				const CellProperties *		cpSet,
				int				leftOffset )
    {
    const int			scope= RTCscopeCELL;

    if  ( docRtfWriteItemProperties( rw, scope, cpSet,
			(RtfGetIntProperty)docGetCellProperty, cpSetMask,
			CellRegularProperties, CellRegularPropertyCount ) )
	{ LLDEB(scope,CellRegularPropertyCount); return -1;	}

    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING )	)
	{
	/*  NOTE: Word 2003 swaps left and top  */
	docRtfSaveAutoSpace( rw,
			    "clpadft", cpSet->cpLeftPaddingUnit,
			    "clpadt", cpSet->cpLeftPadding );  /* !!!  */
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropTOP_PADDING )		)
	{
	/*  NOTE: Word 2003 swaps left and top  */
	docRtfSaveAutoSpace( rw,
			    "clpadfl", cpSet->cpTopPaddingUnit,
			    "clpadl", cpSet->cpTopPadding );  /* !!!  */
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clpadfr", cpSet->cpRightPaddingUnit,
			    "clpadr", cpSet->cpRightPadding );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING_UNIT )	||
	  PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clpadfb", cpSet->cpBottomPaddingUnit,
			    "clpadb", cpSet->cpBottomPadding );
	}
    if  ( PROPmaskISSET( cpSetMask, CLpropCLFTS_WIDTH )	||
	  PROPmaskISSET( cpSetMask, CLpropCLW_WIDTH )	)
	{
	docRtfSaveAutoSpace( rw,
			    "clftsWidth", cpSet->cpPreferredWidthUnit,
			    "clwWidth", cpSet->cpPreferredWidth );
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropSHADING ) )
	{
	docRtfSaveShadingByNumber( rw, RTCscopeCELL_SHADING,
						    cpSet->cpShadingNumber );
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropTEXT_FLOW ) )
	{
	if  ( docRtfWriteProperty( rw, scope,
					CLpropTEXT_FLOW, cpSet->cpTextFlow ) )
	    { LLLDEB(scope,CLpropTEXT_FLOW,cpSet->cpTextFlow); return -1; }
	}

    /*  LAST! and always. value > 0 triggers the mask in the reader */
    if  ( PROPmaskISSET( cpSetMask, CLpropWIDTH ) )
	{ docRtfWriteArgTag( rw, RTFtag_cellx, leftOffset+ cpSet->cpWide ); }
    else{ docRtfWriteArgTag( rw, RTFtag_cellx, 0 );			}

    docRtfWriteNextLine( rw );

    return 0;
    }

