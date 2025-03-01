#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedDraw.h"
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	"tedRuler.h"
#   include	<tedDocFront.h>

#   include	<appDocument.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<layoutContext.h>
#   include	<appDocFront.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scrolling callbacks.						*/
/*									*/
/************************************************************************/

void tedMoveObjectWindows(	EditDocument *		ed )
    {
    struct InsertedObject *	io;

    DocumentPosition		dpObj;
    const PositionGeometry *	pgObj;
    int				partObj;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    docInitDocumentPosition( &dpObj );

    if  ( tedGetObjectSelection( &pgObj, &partObj, &dpObj, &io, ed ) )
	{ LDEB(1); return;	}

    tedSetObjectWindows( ed, pgObj, io, &lc );
    }

/************************************************************************/
/*									*/
/*  Make a document readOnly.						*/
/*									*/
/************************************************************************/

void tedMakeDocumentReadonly(	EditDocument *	ed,
				int		readonly )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		visible= ! readonly;

    guiShowMenuOption( td->tdFileSaveOption, visible );

    guiShowMenuOption( td->tdCutOption, visible );
    guiShowMenuOption( td->tdPasteOption, visible );
    guiShowMenuOption( td->tdUndoOption, visible );
    guiShowMenuOption( td->tdRepeatOption, visible );

    guiShowMenuOption( td->tdInsPictOption, visible );
    guiShowMenuOption( td->tdInsSymbolOption, visible );
    guiShowMenuOption( td->tdInsFileOption, visible );
    guiShowMenuOption( td->tdInsInsertFootnoteOption, visible );
    guiShowMenuOption( td->tdInsInsertEndnoteOption, visible );
    guiShowMenuOption( td->tdInsInsertChftnsepOption, visible );
    guiShowMenuOption( td->tdInsInsertTableOption, visible );
    guiShowMenuOption( td->tdInsInsertPageNumberOption, visible );

    guiShowMenuOption( td->tdTabInsertTableOption, visible );
    guiShowMenuOption( td->tdTabAddRowOption, visible );
    guiShowMenuOption( td->tdTabAddColumnOption, visible );
    guiShowMenuOption( td->tdTabInsertSeparator, visible );

    guiShowMenuOption( td->tdTabDeleteTableOption, visible );
    guiShowMenuOption( td->tdTabDeleteRowOption, visible );
    guiShowMenuOption( td->tdTabDeleteColumnOption, visible );
    guiShowMenuOption( td->tdTabDeleteSeparator, visible );

    guiShowMenuOption( td->tdInsInsertLineBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertPageBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertColumnBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertSectBreakOption, visible );

    guiShowMenuOption( td->tdFontBoldOption, visible );
    guiShowMenuOption( td->tdFontItalicOption, visible );
    guiShowMenuOption( td->tdFontUnderlinedOption, visible );
    guiShowMenuOption( td->tdFontSuperscriptOption, visible );
    guiShowMenuOption( td->tdFontSubscriptOption, visible );
    guiShowMenuOption( td->tdFontToggleSeparator, visible );
    guiShowMenuOption( td->tdFontPasteOption, visible );

    guiShowMenuOption( td->tdFormatOneParaOption, visible );
    guiShowMenuOption( td->tdFormatOneParaSeparator, visible );

    guiShowMenuOption( td->tdFormatPasteRulerOption, visible );
    guiShowMenuOption( td->tdFormatIncreaseIndentOption, visible );
    guiShowMenuOption( td->tdFormatDecreaseIndentOption, visible );
    guiShowMenuOption( td->tdFormatRulerSeparator, visible );

    guiShowMenuOption( td->tdFormatAlignLeftOption, visible );
    guiShowMenuOption( td->tdFormatAlignRightOption, visible );
    guiShowMenuOption( td->tdFormatAlignCenterOption, visible );
    guiShowMenuOption( td->tdFormatAlignJustifyOption, visible );
    guiShowMenuOption( td->tdFormatAlignSeparator, visible );

    guiShowMenuOption( td->tdToolsFontOption, visible );
    guiShowMenuOption( td->tdToolsSpellingOption, visible );
    guiShowMenuOption( td->tdToolsPageLayoutOption, visible );
    guiShowMenuOption( td->tdToolsSymbolOption, visible );

    /* TODO: wrong call */
    guiShowMenuOption( td->tdInsertMenuButton, visible );
    }

/************************************************************************/
/*									*/
/*  Scroll to the selection the first time the document becomes visible	*/
/*									*/
/************************************************************************/

void tedDocumentFirstVisible(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DocumentRectangle		dr= ed->edVisibleRect;

    if  ( td->tdSelectionGeometry.sgRectangle.drX1 > dr.drX1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drX1- dr.drX1;

	dr.drX0 += sh;
	dr.drX1 += sh;
	}
    if  ( td->tdSelectionGeometry.sgRectangle.drY1 > dr.drY1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drY1- dr.drY1;

	dr.drY0 += sh;
	dr.drY1 += sh;
	}

    appScrollToRectangle( (int *)0, (int *)0, ed, &dr );

    return;
    }

int tedMakeDocumentWidget(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    if  ( appMakeDocumentWidget( ed ) )
	{ LDEB(1); return -1;	}

    if  ( ! ea->eaDocumentCursor	&&
	  tedMakeDocumentCursor( ea )	)
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

void tedObserveFocus(	EditDocument *	ed,
			int		inout )
    {
    EditApplication *	ea= ed->edApplication;

    if  ( inout > 0 )
	{
	if  ( ea->eaCurrentDocument != ed )
	    {
	    appSetCurrentDocument( ea, ed );

	    tedAdaptPageToolToDocument( ed );

	    tedAdaptFormatToolToDocument( ed, 0 );
	    }

	if  ( tedHasIBarSelection( ed ) )
	    { tedStartCursorBlink( ed );	}
	}

    if  ( inout < 0 )
	{ tedStopCursorBlink( ed ); }

    return;
    }

/************************************************************************/

void tedSetupDocumentColors(	EditDocument *	ed,
				TedDocument *	td )
    {
    int				luma;
    int				chroma;
    int				hue;

    /*
    if  ( dw->dwColors.acAllocator.caDepth < 4 )
    if  ( 0 )
	{
	td->tdDrawMonochrome= 1;

	utilRGB8SolidBlack( &(td->tdSelColor) );
	utilRGB8SolidBlack( &(td->tdCopiedSelColor) );
	utilRGB8SolidBlack( &(td->tdTableColor) );
	}
    else{
    */
	td->tdDrawMonochrome= 0;

	td->tdSelColor.rgb8Red= 176;
	td->tdSelColor.rgb8Green= 196;
	td->tdSelColor.rgb8Blue= 222;

	td->tdCopiedSelColor.rgb8Red= 176;
	td->tdCopiedSelColor.rgb8Green= 176;
	td->tdCopiedSelColor.rgb8Blue= 176;

	td->tdTableColor.rgb8Red= 192;
	td->tdTableColor.rgb8Green= 192;
	td->tdTableColor.rgb8Blue= 192;
    /*
	}
    */

    if  ( utilRGB8LumaChromaHue( &luma, &chroma, &hue,
						&(ed->edBackgroundColor) ) )
	{ LDEB(1);	}
    else{
	const int	maxLuma= 220;
	const int	minLuma= 64;
	int		changed= 0;

	/*  Must be possible to distinguish background from white */
	if  ( luma > maxLuma )
	    { luma= maxLuma; changed= 1;	}
	if  ( luma < minLuma )
	    { luma= minLuma; changed= 1;	}

	/*? td->tdTableColor= ed->edBackgroundColor; */

	if  ( changed )
	    {
	    if  ( utilRGB8FromLumaChromaHue( &(ed->edBackgroundColor),
							luma, chroma, hue ) )
		{ LLLDEB(luma,chroma,hue);	}

	    /*? td->tdTableColor= ed->edBackgroundColor; */

	    if  ( ed->edTopRuler )
		{
		tedTopRulerSetBackground( ed->edTopRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edLeftRuler )
		{
		tedVerticalRulerSetBackground( ed->edLeftRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edRightRuler )
		{
		tedVerticalRulerSetBackground( ed->edRightRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edBottomRuler )
		{
		tedBottomRulerSetBackground( ed->edBottomRuler,
						&(ed->edBackgroundColor) );
		}
	    }
	}

    }

