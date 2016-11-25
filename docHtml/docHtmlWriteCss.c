/************************************************************************/
/*									*/
/*  Save a struct BufferDocument into an HTML file.				*/
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
    const struct BufferDocument *	bd= hwc->hwcDocument;
    const DocumentProperties *	dp= bd->bdProperties;
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    char			scratch[40];

    sioOutPutString( "body.ted\n", sos );
    sioOutPutString( "  {\n", sos );
    if  ( hwc->hwcEmitBackground )
	{ sioOutPutString( "  background-color: #ffffff;\n", sos );	}
    sioOutPutString( "  color: #000000;\n", sos );

    if  ( dg->dgMargins.roTopOffset > 300 )
	{
	sprintf( scratch, "  margin-top: %dpt;\n",
					dg->dgMargins.roTopOffset/ 20 );
	sioOutPutString( scratch, sos );
	}

    if  ( dg->dgMargins.roLeftOffset > 300 )
	{
	sprintf( scratch, "  margin-left: %dpt;\n",
					dg->dgMargins.roLeftOffset/ 20 );
	sioOutPutString( scratch, sos );
	}

    if  ( dg->dgMargins.roRightOffset > 300 )
	{
	sprintf( scratch, "  margin-right: %dpt;\n",
					dg->dgMargins.roRightOffset/ 20 );
	sioOutPutString( scratch, sos );
	}

    if  ( dg->dgMargins.roBottomOffset > 300 )
	{
	sprintf( scratch, "  margin-bottom: %dpt;\n",
					dg->dgMargins.roBottomOffset/ 20 );
	sioOutPutString( scratch, sos );
	}

    sioOutPutString( "  }\n\n", sos );

    return 0;
    }

int docHtmlSaveDocumentStyles(	HtmlWritingContext *	hwc,
				SimpleOutputStream *	sos )
    {
    if  ( docHtmlSaveBodyStyle( hwc, sos ) )
	{ LDEB(1); return -1;	}

    if  ( docCssSaveTextAttributeStyles( sos, (const IndexSet *)0, hwc->hwcDocument ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

