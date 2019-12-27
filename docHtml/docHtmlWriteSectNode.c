/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<docSectProperties.h>
#   include	<docBuf.h>
#   include	<docTreeType.h>
#   include	<layoutContext.h>
#   include	<docBreakKind.h>
#   include	<docNotes.h>

#   include	<appDebugon.h>

int dochtmlEnterSectNode(	struct BufferItem *	node,
				HtmlWritingContext *	hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				rval= 0;
    unsigned char		applies= 0;
    const DocumentProperties *	dp= hws->hwsDocument->bdProperties;
    int				breakKind;

    SectionProperties		sp;

    docInitSectionProperties( &sp );

    if  ( docSectionHasHeaderFooter( node, &applies, dp,
					    DOCinFIRST_HEADER )		&&
	  applies							)
	{
	struct DocumentTree *	headerTree;

	headerTree= docSectionHeaderFooter( node, &applies, dp,
							DOCinFIRST_HEADER );

	if  ( docHtmlSaveSelection( hwc, headerTree,
				    (const struct DocumentSelection *)0 ) )
	    { LDEB(1);	}
	}

    docHtmlPutString( "<div style=\"", hwc );

    breakKind= docSectGetBreakKind( node->biSectProperties, dp,
			hws->hwsLayoutContext->lcHonourSpecialSectBreaks );

    switch( breakKind )
	{
	case DOCibkNONE:
	    break;

	case DOCibkCOL:
	case DOCibkPAGE:
	    docCssWriteProperty( &(xw->xwColumn), sos,
						"page-break-before", "always" );
	    break;
	case DOCibkEVEN:
	    docCssWriteProperty( &(xw->xwColumn), sos,
						"page-break-before", "left" );
	    break;
	case DOCibkODD:
	    docCssWriteProperty( &(xw->xwColumn), sos,
						"page-break-before", "right" );
	    break;
	}

    if  ( node->biSectColumnCount > 1 )
	{
	int		balanced;
	int		colWide;

	if  ( docCopySectionProperties( &sp, node->biSectProperties ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docSectSetEqualColumnWidth( &sp );

	balanced= docSectColumnsAreBalanced( node );

	colWide= docSectGetColumnWidth( (int *)0, (int *)0, &sp, 0 );

	docCssWriteNumber( &(xw->xwColumn), sos,
				"column-count", sp.spColumnCount );

	docCssWriteDimension( &(xw->xwColumn), sos,
				"column-width",
				TWIPS_TO_PIXELS( colWide ), "px" );

	docCssWriteDimension( &(xw->xwColumn), sos,
				"column-gap",
				TWIPS_TO_PIXELS( sp.spColumnSpacingTwips ),
				"px" );

	docCssWriteProperty( &(xw->xwColumn), sos,
						"column-fill",
						balanced ? "balance" : "auto" );

	if  ( sp.spLineBetweenColumns )
	    {
	    docCssWriteProperty( &(xw->xwColumn), sos,
						"column-rule", "1px solid" );
	    }
	}

    docHtmlPutString( "\">", hwc );

  ready:

    docCleanSectionProperties( &sp );

    return rval;
    }

int dochtmlLeaveSectNode(	struct BufferItem *	sectNode,
				HtmlWritingContext *	hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    if  ( sectNode->biTreeType == DOCinBODY			&&
	  docGetEndnotePlacement( bd ) == FTNplaceSECT_END	)
	{
	int			sect= sectNode->biNumberInParent;

	if  ( docHtmlSaveEndnotes( sect, hwc ) )
	    { LDEB(sect); return -1;	}
	}

    docHtmlWritelnCloseElement( "div", hwc );
    return 0;
    }
