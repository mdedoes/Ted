/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	<textAttributeAdmin.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docItemShadingAdmin.h>
#   include	<docParaTabsAdmin.h>
#   include	<docPropertiesAdmin.h>
#   include	<docFramePropertiesAdmin.h>
#   include	<docDocumentProperties.h>
#   include	<psTextExtents.h>
#   include	<textAttribute.h>
#   include	<fontDocFontList.h>
#   include	"docAttributes.h"

#   include	<appDebugon.h>

/************************************************************************/

int docGetDefaultFont(		const BufferDocument *	bd )
    {
    int					i;
    const struct DocumentFontList *	dfl= bd->bdProperties->dpFontList;

    if  ( bd->bdProperties->dpDefaultFont >= 0			&&
	  bd->bdProperties->dpDefaultFont < dfl->dflFontCount	)
	{ return bd->bdProperties->dpDefaultFont;	}

    for ( i= 0; i < dfl->dflFontCount; i++ )
	{
	if  ( fontFontListGetFontByNumber( dfl, i ) )
	    {
	    bd->bdProperties->dpDefaultFont= i;
	    return bd->bdProperties->dpDefaultFont;
	    }
	}

    /*LDEB(bd->bdProperties->dpDefaultFont);*/
    return bd->bdProperties->dpDefaultFont;
    }

/************************************************************************/

int docGetFontByName(		const struct BufferDocument *	bd,
				const char *			name )
    {
    return fontListGetFontByName( bd->bdProperties->dpFontList, name );
    }

/************************************************************************/

void docGetColorByNumber(	RGB8Color *		rgb8,
				const struct BufferDocument *	bd,
				int			colorNumber )
    {
    const DocumentProperties *	dp= bd->bdProperties;
    const ColorPalette *	cp= dp->dpColorPalette;

    rgb8->rgb8Red= 0;
    rgb8->rgb8Green= 0;
    rgb8->rgb8Blue= 0;
    rgb8->rgb8Alpha= 255;

    if  ( colorNumber != 0 )
	{
	if  ( colorNumber < 1			||
	      colorNumber >= cp->cpColorCount	)
	    { LLDEB(colorNumber,cp->cpColorCount);	}
	else{ *rgb8= cp->cpColors[colorNumber];		}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine a text attribute number for a particule.			*/
/*									*/
/************************************************************************/

int docTextAttributeNumber(	const BufferDocument *	bd,
				const TextAttribute *	ta )
    {
    NumberedPropertiesList *	taList= &(bd->bdPropertyLists->dplTextAttributeList);
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentFontList *	dfl= dp->dpFontList;

    int				textAttributeNr;

    TextAttribute		taCopy= *ta;

    if  ( taCopy.taFontNumber < 0			||
	  taCopy.taFontNumber >= dfl->dflFontCount	)
	{
	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLSDEB(taCopy.taFontNumber,dfl->dflFontCount,"Using default font");	}

	taCopy.taFontNumber= docGetDefaultFont( bd );

	if  ( taCopy.taFontNumber < 0 )
	    { taCopy.taFontNumber= 0;	}

	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLDEB(taCopy.taFontNumber,dfl->dflFontCount);	}
	}

    textAttributeNr= textTextAttributeNumber( taList, &taCopy );
    if  ( textAttributeNr < 0 )
	{ LDEB(textAttributeNr); return -1;	}

    return textAttributeNr;
    }

const TextAttribute * docGetTextAttributeByNumber(
				const struct BufferDocument *	bd,
				int				n )
    {
    NumberedPropertiesList *	taList= &(bd->bdPropertyLists->dplTextAttributeList);

    return textGetTextAttributeByNumber( taList, n );
    }

/************************************************************************/

int docItemShadingNumber(	struct BufferDocument *		bd,
				const struct ItemShading *	is )
    {
    NumberedPropertiesList *	isList= &(bd->bdPropertyLists->dplItemShadingList);

    return docItemShadingNumberImpl( isList, is );
    }

const struct ItemShading * docGetItemShadingByNumber(
				const struct BufferDocument *		bd,
				int					n )
    {
    NumberedPropertiesList *	isList= &(bd->bdPropertyLists->dplItemShadingList);

    return docGetItemShadingByNumberImpl( isList, n );
    }

int docShadingNumberIsShading(	const struct BufferDocument *		bd,
				int					n )
    {
    NumberedPropertiesList *	isList= &(bd->bdPropertyLists->dplItemShadingList);

    return docShadingNumberIsShadingImpl( isList, n );
    }

/************************************************************************/

int docBorderPropertiesNumber(	struct BufferDocument *		bd,
				const struct BorderProperties *	bp )
    {
    NumberedPropertiesList *	bpList= &(bd->bdPropertyLists->dplBorderPropertyList);

    return docBorderPropertiesNumberImpl( bpList, bp );
    }

const struct BorderProperties * docGetBorderPropertiesByNumber(
				const struct BufferDocument *	bd,
				int				n )
    {
    NumberedPropertiesList *	bpList= &(bd->bdPropertyLists->dplBorderPropertyList);

    return docGetBorderPropertiesByNumberImpl( bpList, n );
    }

int docBorderNumberIsBorder(	const struct BufferDocument *	bd,
				int				n )
    {
    NumberedPropertiesList *	bpList= &(bd->bdPropertyLists->dplBorderPropertyList);

    return docBorderNumberIsBorderImpl( bpList, n );
    }

/************************************************************************/

int docFramePropertiesNumber(	struct BufferDocument *		bd,
				const struct FrameProperties *	fp )
    {
    NumberedPropertiesList *	fpList= &(bd->bdPropertyLists->dplFramePropertyList);

    return docFramePropertiesNumberImpl( fpList, fp );
    }

const struct FrameProperties * docGetFramePropertiesByNumber(
				const struct BufferDocument *	bd,
				int				n )
    {
    NumberedPropertiesList *	fpList= &(bd->bdPropertyLists->dplFramePropertyList);

    return docGetFramePropertiesByNumberImpl( fpList, n );
    }

/************************************************************************/

int docTabStopListNumber(	struct BufferDocument *		bd,
				const struct TabStopList *	tsl )
    {
    NumberedPropertiesList *	tslList= &(bd->bdPropertyLists->dplTabStopListList);

    return docTabStopListNumberImpl( tslList, tsl );
    }

const struct TabStopList * docGetTabStopListByNumber(
				const struct BufferDocument *	bd,
				int				n )
    {
    NumberedPropertiesList *	tslList= &(bd->bdPropertyLists->dplTabStopListList);

    return docGetTabStopListByNumberImpl( tslList, n );
    }

/************************************************************************/
/*									*/
/*  Determine the plain text attribute for this document.		*/
/*									*/
/************************************************************************/

void docPlainTextAttribute(		TextAttribute *		ta,
					struct BufferDocument *	bd )
    {
    textInitTextAttribute( ta );
    if  ( bd )
	{ ta->taFontNumber= docGetDefaultFont( bd );	}
    ta->taFontSizeHalfPoints= 24;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the number of the attribute belonging to the scaps	*/
/*  font size. Note that this is used for the shifted lower case	*/
/*  letters only.							*/
/*									*/
/************************************************************************/

int docScapsAttributeNumber(		struct BufferDocument *	bd,
					const TextAttribute *	ta )
    {
    NumberedPropertiesList *	taList= &(bd->bdPropertyLists->dplTextAttributeList);
    TextAttribute	taScaps= *ta;

    taScaps.taFontSizeHalfPoints= SCAPS_SIZE( ta->taFontSizeHalfPoints );
    taScaps.taSmallCaps= 0;

    return textTextAttributeNumber( taList, &taScaps );
    }

