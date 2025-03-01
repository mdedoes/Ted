/************************************************************************/
/*									*/
/*  Find a screen font with a font.					*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docScreenLayout.h"
#   include	"layoutContext.h"
#   include	<textAttributeUtil.h>
#   include	<drawDrawingSurface.h>
#   include	<docDocumentProperties.h>
#   include	<utilIndexMapping.h>
#   include	<textAttribute.h>
#   include	<docBuf.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find/Open the screen font belonging to an attribute number.		*/
/*									*/
/************************************************************************/

int docOpenScreenFont(	const LayoutContext *	lc,
			int			textAttrNr )
    {
    int		screenFont;

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
							    textAttrNr );
    if  ( screenFont < 0 )
	{
	struct BufferDocument *		bd= lc->lcDocument;
	struct DocumentFontList *	dfl= bd->bdProperties->dpFontList;
	const struct AfmFontInfo *	afi;
	const IndexSet *		unicodesWanted;
	const TextAttribute *		ta;
	int				pixelSize;

	ta= docGetTextAttributeByNumber( bd, textAttrNr );

	afi= (*lc->lcGetFontForAttribute)( &unicodesWanted, ta, dfl,
						    lc->lcPostScriptFontList );
	if  ( ! afi )
	    { LXDEB(ta->taFontNumber,afi); return -1;	}

	pixelSize= textGetPixelSize( lc->lcPixelsPerTwip, ta );
	screenFont= drawOpenScreenFont( lc->lcDrawingSurface, afi,
						pixelSize, unicodesWanted );
	if  ( screenFont < 0 )
	    { LLDEB(textAttrNr,screenFont); return -1;	}

	if  ( utilIndexMappingPut( lc->lcAttributeToScreenFont,
					    textAttrNr, screenFont ) < 0 )
	    { LLDEB(textAttrNr,screenFont); return -1;	}

	if  ( ta->taTextIsUnderlined != 0 || ta->taHasStrikethrough != 0 )
	    {
	    TextAttribute	taStripped= *ta;
	    int			textAttrNrStripped;

	    taStripped.taTextIsUnderlined= 0;
	    taStripped.taHasStrikethrough= 0;

	    textAttrNrStripped= docTextAttributeNumber( bd, &taStripped );

	    if  ( textAttrNrStripped < 0 )
		{ LDEB(textAttrNrStripped);	}
	    else{
		if  ( utilIndexMappingPut( lc->lcAttributeToScreenFont,
					textAttrNrStripped, screenFont ) < 0 )
		    { LLDEB(textAttrNr,screenFont); return -1;	}
		}
	    }
	}

    return screenFont;
    }

