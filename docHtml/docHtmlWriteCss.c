/************************************************************************/
/*									*/
/*  Writes RTF attributes as CSS styles.				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<docBuf.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docDocumentProperties.h>

#   include	<appDebugon.h>

static int docHtmlSaveBodyStyle(	HtmlWritingContext *	hwc,
					SimpleOutputStream *	sos )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    const BufferDocument *	bd= hws->hwsDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    const int			indent= 2;

    sioOutPutString( "body.ted\n", sos );
    sioOutPutString( "  {\n", sos );
    if  ( hws->hwsEmitBackground )
	{
	docCssWritelnProperty(
			sos, indent, "background-color", "#ffffff" );
	}
    docCssWritelnProperty( sos, indent, "color", "#000000" );

    if  ( dg->dgMargins.roTopOffset > 300 )
	{
	docCssWritelnDimension( sos, indent,
		"margin-top", dg->dgMargins.roTopOffset/ 20, "pt" );
	}

    if  ( dg->dgMargins.roLeftOffset > 300 )
	{
	docCssWritelnDimension( sos, indent,
		"margin-left", dg->dgMargins.roLeftOffset/ 20, "pt" );
	}

    if  ( dg->dgMargins.roRightOffset > 300 )
	{
	docCssWritelnDimension( sos, indent,
		"margin-right", dg->dgMargins.roRightOffset/ 20, "pt" );
	}

    if  ( dg->dgMargins.roBottomOffset > 300 )
	{
	docCssWritelnDimension( sos, indent,
		"margin-bottom", dg->dgMargins.roBottomOffset/ 20, "pt" );
	}

    sioOutPutString( "  }\n\n", sos );

    return 0;
    }

int docHtmlSaveDocumentStyles(	HtmlWritingContext *	hwc,
				SimpleOutputStream *	sos )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    if  ( docHtmlSaveBodyStyle( hwc, sos ) )
	{ LDEB(1); return -1;	}

    if  ( docCssSaveTextAttributeStyles( sos,
				(const IndexSet *)0, hws->hwsDocument ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
