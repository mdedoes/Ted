/************************************************************************/
/*									*/
/*  Writes RTF attributes as CSS styles.				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<docBuf.h>
#   include	<psShading.h>
#   include	<docPropertiesAdmin.h>
#   include	<textAttributeAdmin.h>
#   include	<docItemShading.h>
#   include	<docBorderProperties.h>
#   include	"docWriteCss.h"
#   include	<docDocumentProperties.h>
#   include	<psTextExtents.h>
#   include	<textAttribute.h>
#   include	<fontDocFont.h>
#   include	<docAttributes.h>
#   include	<fontDocFontList.h>

#   include	<appDebugon.h>

int docCssWriteProperty(	int *			pCol,
				SimpleOutputStream *	sos,
				const char *		property,
				const char *		value )
    {
    sioOutPutString( property, sos );
    sioOutPutString( ": ", sos );
    sioOutPutString( value, sos );
    sioOutPutString( ";", sos );

    if  ( pCol )
	{
	(*pCol) += strlen( property )+ 2+ strlen( value )+ 1;
	}

    return 0;
    }

int docCssWriteDimension(	int *			pCol,
				SimpleOutputStream *	sos,
				const char *		property,
				int			value,
				const char *		unit )
    {
    char			scratch[40];

    sprintf( scratch, "%d%s", value, unit );

    docCssWriteProperty( pCol, sos, property, scratch );

    return 0;
    }

int docCssWriteNumber(		int *			pCol,
				SimpleOutputStream *	sos,
				const char *		property,
				int			value )
    {
    char			scratch[40];

    sprintf( scratch, "%d", value );

    docCssWriteProperty( pCol, sos, property, scratch );

    return 0;
    }

int docCssWriteRgb8Color(	int *			pCol,
				SimpleOutputStream *	sos,
				const char *		property,
				const RGB8Color *	rgb8 )
    {
    char			scratch[50];

    sprintf( scratch, "#%02x%02x%02x",
			    rgb8->rgb8Red, rgb8->rgb8Green, rgb8->rgb8Blue );

    docCssWriteProperty( pCol, sos, property, scratch );

    return 0;
    }

int docCssWritelnProperty(	SimpleOutputStream *	sos,
				int			indent,
				const char *		property,
				const char *		value )
    {
    if  ( indent > 0 )
	{ sioOutPrintf( sos, "%*s", indent, "" );	}

    if  ( docCssWriteProperty( (int *)0, sos, property, value ) )
	{ SSDEB(property,value); return -1;	}

    sioOutPutString( "\r\n", sos );

    return 0;
    }

int docCssWritelnDimension(	SimpleOutputStream *	sos,
				int			indent,
				const char *		property,
				int			value,
				const char *		unit )
    {
    char			scratch[40];

    sprintf( scratch, "%d%s", value, unit );

    docCssWritelnProperty( sos, indent, property, scratch );

    return 0;
    }

int docCssWritelnNumber(	SimpleOutputStream *	sos,
				int			indent,
				const char *		property,
				int			value )
    {
    char			scratch[40];

    sprintf( scratch, "%d", value );

    docCssWritelnProperty( sos, indent, property, scratch );

    return 0;
    }

int docCssWritelnRgb8Color(	SimpleOutputStream *	sos,
				int			indent,
				const char *		property,
				const RGB8Color *	rgb8 )
    {
    char			scratch[50];

    sprintf( scratch, "#%02x%02x%02x",
			    rgb8->rgb8Red, rgb8->rgb8Green, rgb8->rgb8Blue );

    docCssWritelnProperty( sos, indent, property, scratch );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit a background to CSS.						*/
/*									*/
/************************************************************************/

int docCssUseBackgroundStyle(	const ItemShading *		is,
				const struct BufferDocument *	bd )
    {
    int				isFilled= 0;
    RGB8Color			rgb8;

    if  ( is->isPattern == DOCspSOLID )
	{
	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, is ) )
	    { LDEB(1);	}
	if  ( isFilled )
	    { return 1;	}
	}

    return 0;
    }

void docCssEmitBackgroundStyle( int *				pCol,
				SimpleOutputStream *		sos,
				const struct BufferDocument *	bd,
				const ItemShading *		is )
    {
    int				isFilled= 0;
    RGB8Color			rgb8;

    if  ( is->isPattern == DOCspSOLID )
	{
	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, is ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    { docCssWriteRgb8Color( pCol, sos, "background-color", &rgb8 ); }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit border styles.							*/
/*									*/
/************************************************************************/

static int docCssEmitBorderStyle(
			    int *			pCol,
			    SimpleOutputStream *	sos,
			    const struct BufferDocument *	bd,
			    const char *		whatBorder,
			    const BorderProperties *	bp )
    {
    int		col= *pCol;

    if  ( sioOutPutString( whatBorder, sos ) < 0 )
	{ LDEB(1); return -1;	}
    col += strlen( whatBorder );
    if  ( sioOutPutByte( ':', sos ) < 0 )
	{ LDEB(1); return -1;	}
    col++;

    /* width */
    sioOutPrintf( sos, " %dpt", ( bp->bpPenWideTwips+ 10 )/ 20 );
    col += 5;

    /* style */
    if  ( sioOutPutString( " solid", sos ) < 0 )
	{ LDEB(1); return -1;	}
    col += 6;

    /* color */
    if  ( bp->bpColor == 0 )
	{
	if  ( sioOutPutString( " black", sos ) < 0 )
	    { LDEB(1); return -1;	}
	col += 6;
	}
    else{
	const DocumentProperties *	dp= bd->bdProperties;
	const ColorPalette *		colpal= dp->dpColorPalette;

	char				scratch[50];

	if  ( bp->bpColor < colpal->cpColorCount )
	    {
	    const RGB8Color *	rgb8= colpal->cpColors+ bp->bpColor;

	    sprintf( scratch, " #%02x%02x%02x",
					    rgb8->rgb8Red,
					    rgb8->rgb8Green,
					    rgb8->rgb8Blue );

	    if  ( sioOutPutString( scratch, sos ) < 0 )
		{ LDEB(1); return -1;	}
	    col += strlen( scratch );
	    }
	else{
	    if  ( sioOutPutString( " black", sos ) < 0 )
		{ LDEB(1); return -1;	}
	    col += 6;
	    }
	}

    if  ( sioOutPutByte( ';', sos ) < 0 )
	{ LDEB(1); return -1;	}
    col++;

    *pCol= col;
    return 0;
    }

void docCssEmitBorderStyleByNumber(
			    int *			pCol,
			    SimpleOutputStream *	sos,
			    const struct BufferDocument *	bd,
			    const char *		whatBorder,
			    int				num )
    {
    const BorderProperties *		bp;

    bp= docGetBorderPropertiesByNumber( bd, num );

    if  ( ! DOCisBORDER( bp ) )
	{ return;	}

    docCssEmitBorderStyle( pCol, sos, bd, whatBorder, bp );
    }

/************************************************************************/
/*									*/
/*  Save styles corresponding to the different text attributes in the	*/
/*  document.								*/
/*									*/
/*  1)  Silly netscape really makes fonts too small. Add a little.	*/
/*									*/
/************************************************************************/

typedef struct AttributesThrough
    {
    SimpleOutputStream *	atSos;
    const IndexSet *		atUsed;
    const struct BufferDocument *	atDocument;
    } AttributesThrough;

static int docCssSaveTextAttributeStyle(
				    int				n,
				    const TextAttribute *	ta,
				    void *			through )
    {
    AttributesThrough *			at= (AttributesThrough *)through;
    char				scratch[150+ 1];
    int					size;
    int					fontSize;

    const struct BufferDocument *	bd= at->atDocument;
    const DocumentProperties *		dp= bd->bdProperties;
    const struct DocumentFontList *	dfl= dp->dpFontList;
    const DocumentFont *		df;

    const int				indent= 2;

    /*  1  */
    fontSize= ta->taFontSizeHalfPoints;
    fontSize= ( 6* ta->taFontSizeHalfPoints )/ 5;

    df= fontFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return -1;	}

    sprintf( scratch, "span.t%d, div.t%d, text.t%d\n", n, n, n );
    sioOutPutString( scratch, at->atSos );
    sioOutPutString( "  {\n", at->atSos );

    if  ( ta->taTextColorNumber > 0 )
	{
	docCssWritelnRgb8Color( at->atSos, indent, "color",
		    dp->dpColorPalette->cpColors+ ta->taTextColorNumber );
	}

    size= cssFontFamilyIndicator( scratch, sizeof(scratch)- 1,
					    df->dfStyleInt, &(df->dfName) );
    if  ( size < 0 )
	{ LDEB(size);	}
    if  ( size > 0 )
	{
	docCssWritelnProperty( at->atSos, indent, "font-family", scratch );
	}

    if  ( ta->taFontIsBold )
	{
	docCssWritelnProperty( at->atSos, indent, "font-weight", "bold" );
	}

    if  ( ta->taFontIsSlanted )
	{
	docCssWritelnProperty( at->atSos, indent, "font-style", "italic" );
	}

    if  ( ta->taTextIsUnderlined )
	{
	docCssWritelnProperty( at->atSos, indent,
					"text-decoration", "underline" );
	}

    if  ( ta->taHasStrikethrough )
	{
	docCssWritelnProperty( at->atSos, indent,
					"text-decoration", "line-through" );
	}

    if  ( ta->taSmallCaps )
	{
	docCssWritelnProperty( at->atSos, indent,
					"font-variant", "small-caps" );
	}

    if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	{
	docCssWritelnProperty( at->atSos, indent, "vertical-align", "super" );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	{
	docCssWritelnProperty( at->atSos, indent, "vertical-align", "sub" );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    if  ( docBorderNumberIsBorder( bd, ta->taBorderNumber ) )
	{
	int			col= 0;

	docCssEmitBorderStyleByNumber( &col, at->atSos, bd,
					    "border", ta->taBorderNumber );
	}

    if  ( docShadingNumberIsShading( bd, ta->taShadingNumber ) )
	{
	int			col= 0;
	const ItemShading *	is;

	is= docGetItemShadingByNumber( bd, ta->taShadingNumber );

	if  ( docCssUseBackgroundStyle( is, at->atDocument ) )
	    {
	    docCssEmitBackgroundStyle( &col, at->atSos, at->atDocument, is );
	    }
	}

    sprintf( scratch, "%d%spt", fontSize/ 2, (fontSize % 2)?".5":"" );
    docCssWritelnProperty( at->atSos, indent, "font-size", scratch );

    sioOutPutString( "  }\n", at->atSos );

    return 0;
    }

int docCssSaveTextAttributeStyles(	SimpleOutputStream *		sos,
					const IndexSet *		used,
					const struct BufferDocument *	bd )
    {
    AttributesThrough			at;
    const DocumentPropertyLists *	dpl= bd->bdPropertyLists;

    at.atSos= sos;
    at.atUsed= used;
    at.atDocument= bd;

    textForAllAttributesInList( &(dpl->dplTextAttributeList), used,
			    docCssSaveTextAttributeStyle, (void *)&at );

    return 0;
    }

