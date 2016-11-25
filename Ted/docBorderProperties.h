/************************************************************************/
/*									*/
/*  Border properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BORDER_PROP_H
#   define	DOC_BORDER_PROP_H

#   include	<utilPropMask.h>
#   include	<bmcolor.h>

/************************************************************************/
/*									*/
/*  Border Styles: How they are drawn.					*/
/*									*/
/************************************************************************/

typedef enum BorderStyle
    {
    DOCbsNONE= 0,
    DOCbsS,
    DOCbsTH,
    DOCbsSH,

    DOCbsDB,
    DOCbsHAIR,
    DOCbsDOT,
    DOCbsDASH,
    DOCbsDASHD,
    DOCbsDASHDD,
    DOCbsTRIPLE,

    DOCbsTNTHSG,
    DOCbsTHTNSG,
    DOCbsTNTHTNSG,
    DOCbsTNTHMG,
    DOCbsTHTNMG,
    DOCbsTNTHTNMG,
    DOCbsTNTHLG,
    DOCbsTHTNLG,
    DOCbsTNTHTNLG,

    DOCbsWAVY,
    DOCbsWAVYDB,
    DOCbsDASHSM,
    DOCbsDASHDOTSTR,
    DOCbsEMBOSS,
    DOCbsENGRAVE,
    DOCbsOUTSET,

    DOCbs_COUNT
    } BorderStyle;

#   define DOCbs_BITS	5

#   if  (1<<DOCbs_BITS) < DOCbs_COUNT
    This will crash: Increase DOCbs_BITS
#   endif

typedef struct BorderProperties
    {
    unsigned int	bpColor:8;
    unsigned int	bpPenWideTwips:8;
    unsigned int	bpStyle:DOCbs_BITS;	/*  (enum)		*/
    unsigned int	bpSpacingTwips:16;	/*  Unused in recent	*/
						/*  versions of Word	*/
    } BorderProperties;

typedef struct ExpandedBorderProperties
    {
    int			ebpColorExplicit;
    RGB8Color		ebpColor;

    int			ebpPenWideTwips;
    int			ebpStyle;
    int			ebpSpacingTwips;
    } ExpandedBorderProperties;

typedef enum BorderProperty
    {
    BRDRprop_NONE= -1,

    BRDRpropCOLOR= 0,
    BRDRpropSPACING,
    BRDRpropPEN_WIDE,
    BRDRpropSTYLE,

    BRDRprop_COUNT
    } BorderProperty;

#   define	docCleanBorderProperties( bp ) /*void*/

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBorderProperties(	BorderProperties *	bp );

extern void docUpdateBorderProperties(
				    PropertyMask * 		pBpDoneMask,
				    BorderProperties *		bpTo,
				    const PropertyMask * 	bpSetMask,
				    const BorderProperties *	bpFrom );

extern void docCopyBorderProperties(BorderProperties *		bpTo,
				    const BorderProperties *	bpFrom,
				    const int *			colorMap );

extern int docBordersDiffer(	const BorderProperties *	bp1,
				const BorderProperties *	bp2,
				const int *			colorMap );

extern void docExpandBorderProperties(
				ExpandedBorderProperties *	ebp,
				const BorderProperties *	bp,
				const RGB8Color *		colors,
				int				colorCount );

extern int docIndirectBorderProperties(
			PropertyMask *				pBpDoneMask,
			BorderProperties *			bp,
			const PropertyMask * 			bpSetMask,
			const ExpandedBorderProperties *	ebp,
			RGB8Color **				pColors,
			int *					pColorCount );

extern void docInitExpandedBorderProperties(
				ExpandedBorderProperties *	ebp );

extern int docBorderThick(	int *				pWide,
				const BorderProperties *	bp );

#   endif	/*  DOC_BORDER_PROP_H  */
