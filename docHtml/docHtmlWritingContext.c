/************************************************************************/
/*									*/
/*  Save a Document into an HTML file.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<ctype.h>

#   include	"docHtmlWriteImpl.h"

#   include	<appDebugon.h>

void docInitHtmlWritingSettings(	HtmlWritingSettings *	hws )
    {
    hws->hwsInlineCss= 0;
    hws->hwsGetCssName= (HtmlGetCssName)0;

    hws->hwsInlineImages= 0;
    hws->hwsOpenImageStream= (HtmlOpenImageStream)0;
    hws->hwsGetImageSrc= (HtmlGetImageSrc)0;

    hws->hwsInlineNotes= 0;

    hws->hwsEmitBackground= 0;

    hws->hwsLayoutContext= (const struct LayoutContext *)0;
    hws->hwsDocument= (struct BufferDocument *)0;
    }

void docInitHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    xmlInitXmlWriter( &(hwc->hwcXmlWriter) );

    hwc->hwcSettings= (const HtmlWritingSettings *)0;

    docLayoutInitBlockFrame( &(hwc->hwcBlockFrame) );
    docInitParagraphFrame( &(hwc->hwcParagraphFrame) );

    hwc->hwcPrivate= (void *)0;

    utilInitIndexMapping( &(hwc->hwcDeferredNotes) );

    hwc->hwcInHyperlink= 0;
    hwc->hwcInBookmark= 0;
    hwc->hwcInPageref= 0;
    hwc->hwcBytesInLink= 0;

    docInitParagraphProperties( &(hwc->hwcParagraphProperties) );

    hwc->hwcImageCount= 0;
    hwc->hwcNoteRefCount= 0;
    hwc->hwcNoteDefCount= 0;

    hwc->hwcTableNesting= 0;

    hwc->hwcCurrentAttributeNumber= -1;
    return;
    }

void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    utilCleanIndexMapping( &(hwc->hwcDeferredNotes) );

    docCleanParagraphProperties( &(hwc->hwcParagraphProperties) );

    docLayoutCleanBlockFrame( &(hwc->hwcBlockFrame) );
    /*docCleanParagraphFrame( &(hwc->hwcParagraphFrame) );*/

    /* NO! The stream usually is a borrowed one
    xmlCloseXmlWriter( &(hwc->hwcXmlWriter) );
    */

    return;
    }

int docStartHtmlWritingContext(		HtmlWritingContext *		hwc,
					const HtmlWritingSettings *	hws,
					struct SimpleOutputStream *	sos )
    {
    hwc->hwcSettings= hws;
    hwc->hwcXmlWriter.xwSos= sos;

    return 0;
    }

