/************************************************************************/
/*									*/
/*  Save the images in a document and include references in the	HTML	*/
/*  file.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	<drawMetafileSvg.h>
#   include	<docLayoutObject.h>
#   include	"docSvgDrawImpl.h"
#   include	"docHtmlWriteDrawingShape.h"

#   include	<sioGeneral.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docShape.h>
#   include	"docHtmlWriteImpl.h"

#   include	<docObject.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

int docHtmlSaveDrawingShapeToSvgElement(
				HtmlWritingContext *	hwc,
				InsertedObject *	io,
				int			includeSvgDeclaration,
				SimpleOutputStream *	sosImage )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    DocumentRectangle		drDest;
    int				pixelsWide;
    int				pixelsHigh;
    int				page;

    struct DocumentTree *	tree= (struct DocumentTree *)0;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;

    DrawingShape *		ds= io->ioDrawingShape;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    pixelsWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    pixelsHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);
    page= io->ioY0Position.lpPage;

    if  ( docGetRootOfSelectionScope( &tree, &bodySectNode, hws->hwsDocument,
					&(ds->dsSelectionScope) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSvgSaveShapeObject( sosImage, includeSvgDeclaration,
				    page, pixelsWide, pixelsHigh,
				    io, bodySectNode, hws->hwsLayoutContext ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

int docHtmlSaveDrawingShapeToSvgDataUrl(
				HtmlWritingContext *	hwc,
				const char *		contentType,
				InsertedObject *	io )
    {
    int				rval= 0;

    XmlWriter *			xw= &(hwc->hwcXmlWriter);

    const int			includeSvgDeclaration= 1;
    SimpleOutputStream * 	sosImage= (SimpleOutputStream *)0;

    sosImage= xmlStartDataUrl( xw, "src", contentType );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docHtmlSaveDrawingShapeToSvgElement(
				hwc, io, includeSvgDeclaration, sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

    xmlFinishDataUrl( xw, sosImage );
    sosImage= (SimpleOutputStream *)0;

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/

static void docHtmlSetShapeLeft(	char *			style )
    {
    strcat( style, "float: left;" );
    strcat( style, "clear: left;" );
    strcat( style, "margin: 5px 5px 5px 0;" );
    }

static void docHtmlSetShapeRight(	char *			style )
    {
    strcat( style, "float: right;" );
    strcat( style, "clear: right;" );
    strcat( style, "margin: 5px 0 5px 5px;" );
    }

void docHtmlSetShapeStyle(	char *				style,
				HtmlWritingContext *		hwc,
				const struct BufferItem *	paraNode,
				const InsertedObject *		io )
    {
    const DrawingShape *	ds= io->ioDrawingShape;
    const ShapeProperties *	sp= &(ds->dsShapeProperties);

    switch( sp->spWrapStyle )
	{
	case SHPswtTOPBOTTOM:
	    strcat( style, "clear: both;" );
	    break;

	case SHPswtAROUND:
	case SHPswtTIGHTLY:
	    switch( sp->spWrapSide )
		{
		case SHPswsBOTH:
		case SHPswsLARGEST:
		    {
		    int	px0;
		    int	px1;

		    docParagraphFrameTwips( &(hwc->hwcParagraphFrame),
					&(hwc->hwcBlockFrame), paraNode );

		    px0= hwc->hwcParagraphFrame.pfParaContentRect.drX0;
		    px1= hwc->hwcParagraphFrame.pfParaContentRect.drX1;

		    if  ( sp->spRect.drX0- px0 >= px1- sp->spRect.drX1 )
			{ docHtmlSetShapeRight( style );	}
		    else{ docHtmlSetShapeLeft( style );		}
		    }

		    break;

		case SHPswsLEFT:
		    docHtmlSetShapeRight( style );
		    break;

		case SHPswsRIGHT:
		    docHtmlSetShapeLeft( style );
		    break;

		default:
		    LLDEB(sp->spWrapStyle,sp->spWrapSide);
		    break;
		}
	    break;

	case SHPswtNONE:
	case SHPswtTHROUGH:
	    LDEB(sp->spWrapStyle);
	    break;

	default:
	    LDEB(sp->spWrapStyle);
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

int docHtmlSaveDrawingShapeToSvgStream(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const char *		contentType,
					const char *		ext )
    {
    int				rval= 0;
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    const int			includeSvgDeclaration= 1;

    if  ( ! hws->hwsOpenImageStream )
	{ XDEB(hws->hwsOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hws->hwsOpenImageStream)( hwc, n, io, contentType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docHtmlSaveDrawingShapeToSvgElement(
				hwc, io, includeSvgDeclaration, sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }
