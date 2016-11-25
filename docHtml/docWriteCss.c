/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
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

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Emit a background to CSS.						*/
/*									*/
/************************************************************************/

int docCssUseBackgroundStyle(	const ItemShading *		is,
				const struct BufferDocument *		bd )
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
				const struct BufferDocument *		bd,
				const ItemShading *		is )
    {
    int				isFilled= 0;
    RGB8Color			rgb8;

    char			scratch[50];

    if  ( is->isPattern == DOCspSOLID )
	{
	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, is ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    {
	    sprintf( scratch, "background-color: #%02x%02x%02x;",
			    rgb8.rgb8Red, rgb8.rgb8Green, rgb8.rgb8Blue );

	    sioOutPutString( scratch, sos );
	    (*pCol) += strlen( scratch );
	    }
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

    /* color */
    if  ( bp->bpColor == 0 )
	{
	if  ( sioOutPutString( " black", sos ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	const DocumentProperties *	dp= bd->bdProperties;
	const ColorPalette *		colpal= dp->dpColorPalette;
	const RGB8Color *		rgb8= colpal->cpColors+ bp->bpColor;

	char				scratch[50];

	sprintf( scratch, " #%02x%02x%02x",
					rgb8->rgb8Red,
					rgb8->rgb8Green,
					rgb8->rgb8Blue );

	if  ( sioOutPutString( scratch, sos ) < 0 )
	    { LDEB(1); return -1;	}
	col += strlen( scratch );
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

    const struct BufferDocument *		bd= at->atDocument;
    const DocumentProperties *		dp= bd->bdProperties;
    const DocumentFontList *		dfl= dp->dpFontList;
    const DocumentFont *		df;

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
	const RGB8Color *	rgb8= dp->dpColorPalette->cpColors+ ta->taTextColorNumber;

	sprintf( scratch, "#%02x%02x%02x",
					rgb8->rgb8Red,
					rgb8->rgb8Green,
					rgb8->rgb8Blue );

	sioOutPutString( "  color: ", at->atSos );
	sioOutPutString( scratch, at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    size= cssFontFamilyIndicator( scratch, sizeof(scratch)- 1,
					    df->dfStyleInt, &(df->dfName) );
    if  ( size < 0 )
	{ LDEB(size);	}
    if  ( size > 0 )
	{
	sioOutPutString( "  font-family: ", at->atSos );
	sioOutPutString( scratch, at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taFontIsBold )
	{
	sioOutPutString( "  font-weight: bold", at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taFontIsSlanted )
	{
	sioOutPutString( "  font-style: italic", at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taTextIsUnderlined )
	{
	sioOutPutString( "  text-decoration: underline", at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taHasStrikethrough )
	{
	sioOutPutString( "  text-decoration: line-through", at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taSmallCaps )
	{
	sioOutPutString( "  font-variant: small-caps", at->atSos );
	sioOutPutString( ";\n", at->atSos );
	}

    if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	{
	sioOutPutString( "  vertical-align: super", at->atSos );
	sioOutPutString( ";\n", at->atSos );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	{
	sioOutPutString( "  vertical-align: sub", at->atSos );
	sioOutPutString( ";\n", at->atSos );

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
    sioOutPutString( "  font-size: ", at->atSos );
    sioOutPutString( scratch, at->atSos );
    sioOutPutString( ";\n", at->atSos );

    sioOutPutString( "  }\n", at->atSos );

    return 0;
    }

int docCssSaveTextAttributeStyles(	SimpleOutputStream *	sos,
					const IndexSet *	used,
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

