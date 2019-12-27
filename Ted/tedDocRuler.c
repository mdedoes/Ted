/************************************************************************/
/*									*/
/*  Rulers, Ted specific functionality.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<docPageGrid.h>
#   include	<guiDrawingWidget.h>

#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	<tedDocFront.h>
#   include	"tedRuler.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<tedToolFront.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTabStopList.h>
#   include	<docRowProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docLayout.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<docBuf.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docRulerColumnSeparator.h>
#   include	<docScreenRuler.h>
#   include	<layoutContext.h>

#   include	<appDebugon.h>

static int tedUpdateTableColumns( 
				EditDocument *			ed,
				const DocumentSelection *	ds,
				const BlockFrame *		bf,
				const LayoutContext *		lc,
				int				csCountNew,
				const ColumnSeparator *		csNew )
    {
    int				rval= 0;

    int				csCountOld;
    ColumnSeparator *		csOld;
    int				colX0;
    int				colX1;

    int				changed= 0;

    struct BufferItem *		parentNode;
    struct BufferItem *		rowNode;

    const RowProperties *	rpOld;
    RowProperties		rpSet;

    int				col;
    int				row;
    int				row0;
    int				row1;

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;
    const CellProperties *	cpSet= (const CellProperties *)0;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    docInitRowProperties( &rpSet );
    utilPropMaskClear( &rpSetMask );
    utilPropMaskClear( &cpSetMask );

    if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rowNode= parentNode->biChildren[row0];
    if  ( docRowGetRulerColumnsPixels( &colX0, &colX1, &csOld, &csCountOld,
							    rowNode, lc, bf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( csCountNew != csCountOld )
	{ LLDEB(csCountNew,csCountOld); rval= -1; goto ready;	}

    rpOld= rowNode->biRowProperties;
    if  ( docCopyRowProperties( &rpSet, rpOld, dam0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( csNew[0].csX0 != csOld[0].csX0 )
	{
	if  ( docRowGetRulerColumnTwips( &rpSet, lc, 0, csOld, csNew ) )
	    { LDEB(0); rval= -1; goto ready;	}

	PROPmaskADD( &rpSetMask, RPpropLEFT_INDENT );
	changed= 1;
	}
    else{
	int		sep;

	for ( sep= 1; sep < csCountOld; sep++ )
	    {
	    if  ( csNew[sep].csX0 != csOld[sep].csX0 )
		{
		/* HACK: select the relevant column (only) */
		if  ( sep != ds->dsCol0+ 1 || sep != ds->dsCol1+ 1 )
		    {
		    int			scrolledX= 0;
		    int			scrolledY= 0;
		    DocumentPosition	dpCol;
		    DocumentSelection	dsCol;

		    if  ( docHeadPosition( &dpCol,
					    rowNode->biChildren[sep-1] ) )
			{ LLDEB(sep,col); rval= -1; goto ready;	}
		    docSetIBarSelection( &dsCol, &dpCol );

		    tedSetSelectionLow( ed, &dsCol, 0, &scrolledX, &scrolledY );
		    }

		if  ( docRowGetRulerColumnTwips( &rpSet, lc,
							sep, csOld, csNew ) )
		    { LDEB(sep); rval= -1; goto ready;	}

		cpSet= &(rpSet.rpCells[sep-1]);
		PROPmaskADD( &cpSetMask, CLpropWIDTH );
		changed= 1;
		break;
		}
	    }
	}

    if  ( changed )
	{
	const int		wholeRow= 0;
	const int		wholeColumn= 1;

	tedDocSetTableProperties( ed, wholeRow, wholeColumn,
				&cpSetMask, cpSet, &rpSetMask, &rpSet );
	}

  ready:

    docCleanRowProperties( &rpSet );

    return rval;
    }

/************************************************************************/
/*									*/
/*  The user wants something with the mouse on the horizontal ruler	*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( tedTopRulerButtonDown, w, voided, downEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				leftIndentNew;
    int				firstIndentNew;
    int				rightIndentNew;
    TabStopList			tslNew;

    int				csCountNew= 0;
    ColumnSeparator *		csNew= (ColumnSeparator *)0;

    int				prop= PPprop_NONE;

    PropertyMask		ppSetMask;

    ParagraphProperties		ppNew;

    ParagraphFrame		pf;
    BlockFrame			bf;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    LayoutContext		lc;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;
    int				mouseX;
    int				mouseY;

    struct BufferItem *		bodySectNode;

    docInitParagraphProperties( &ppNew );
    docInitTabStopList( &tslNew );

    docLayoutInitBlockFrame( &bf );

    if  ( ed->edFileReadOnly )
	{ goto ready;	}

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&mouseX, &mouseY, &button, &upDown, &seq, &keyState,
			w, downEvent ) )
	{ goto ready;	}

    if  ( upDown < 1 || seq > 1 )
	{ goto ready;	}

    switch( button )
	{
	case 1:
	    break;

	case 3:
	    prop= tedTopRulerFindMouse( &seq, mouseX, mouseY, ed );
	    switch( prop )
		{
		case PPpropTAB_STOPS:
		case PPprop_TAB_KIND_BUTTON:
		    tedAppShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    tedAdaptFormatToolToDocument( ed, 0 );
		    tedAppFormatShowParaTabsPage( ed->edApplication );
		    goto ready;

		case PPpropLEFT_INDENT:
		case PPpropFIRST_INDENT:
		case PPpropRIGHT_INDENT:
		    tedAppShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    tedAdaptFormatToolToDocument( ed, 0 );
		    tedAppFormatShowParaLayoutPage( ed->edApplication );
		    goto ready;

		case PPprop_COLUMNS:
		    tedAppShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    if  ( seq == 0 )
			{
			if  ( tedDocSelectColumn( ed, seq ) )
			    { LDEB(seq); goto ready;	}

			tedAppFormatShowTablePage( ed->edApplication );
			}
		    else{
			if  ( tedDocSelectColumn( ed, seq- 1 ) )
			    { LDEB(seq); goto ready;	}

			tedAppFormatShowColumnPage( ed->edApplication );
			}
		    goto ready;

		default:
		    /* LDEB(prop); */ goto ready;
		}
	    /*break;*/

	default:
	    goto ready;
	}

    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &ds, &sg, &sd, (struct DocumentTree **)0,
							&bodySectNode, ed ) )
	{ LDEB(1); goto ready;	}

    docSectionBlockFrameTwips( &bf, ds.dsHead.dpNode, bodySectNode, bd,
			    sg.sgHead.pgTopPosition.lpPage,
			    sg.sgHead.pgTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, ds.dsHead.dpNode );

    if  ( docParaGetRulerPixels(
			    (int *)0, (int *)0,
			    &firstIndentNew, &leftIndentNew, &rightIndentNew,
			    &lc, ds.dsHead.dpNode->biParaProperties, &pf ) )
	{ LDEB(1); goto ready;	}

    prop= tedTopRulerTrackMouse(
			    &firstIndentNew, &leftIndentNew, &rightIndentNew,
			    &tslNew, &csCountNew, &csNew, w, downEvent, ed );

    utilPropMaskClear( &ppSetMask );

    switch( prop )
	{
	case PPprop_NONE:
	case PPprop_TAB_KIND_BUTTON:
	    goto ready;

	case PPpropLEFT_INDENT:
	case PPpropFIRST_INDENT:
	case PPpropRIGHT_INDENT:
	case PPpropTAB_STOPS:
	    PROPmaskADD( &ppSetMask, prop );
	    break;

	case PPprop_COLUMNS:
	    tedUpdateTableColumns( ed, &ds, &bf, &lc, csCountNew, csNew );
	    goto ready;
	default:
	    LDEB(prop); goto ready;
	}

    if  ( docCopyParagraphProperties( &ppNew,
				ds.dsHead.dpNode->biParaProperties ) )
	{ LDEB(1); goto ready;	}

    if  ( docParaGetRulerTwips( &lc, &bf, &pf, bd, &ppNew,
			    firstIndentNew, leftIndentNew, rightIndentNew,
			    &tslNew ) )
	{ LDEB(1); goto ready;	}

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppNew,
							    td->tdTraced ) )
	{ LDEB(1); goto ready;	}

  ready:

    docCleanParagraphProperties( &ppNew );
    docCleanTabStopList( &tslNew );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the horizontal ruler to the current paragraph.		*/
/*									*/
/************************************************************************/

void tedDocAdaptTopRuler(	EditDocument *			ed,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferItem *	bodySectNode )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				leftIndentPixels;
    int				firstIndentPixels;
    int				rightIndentPixels;

    BlockFrame			bf;
    ParagraphFrame		pf;

    int				pfX0Pixels;
    int				pfX1Pixels;

    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    struct BufferItem *		paraNode= ds->dsHead.dpNode;

    TabStopList			tslPixels;

    LayoutContext		lc;

    docLayoutInitBlockFrame( &bf );
    docInitTabStopList( &tslPixels );

    tedSetScreenLayoutContext( &lc, ed );

    if  ( ! ed->edTopRuler )
	{ goto ready;	}

    docSectionBlockFrameTwips( &bf, paraNode, bodySectNode, bd,
			    sg->sgHead.pgTopPosition.lpPage,
			    sg->sgHead.pgTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    if  ( docParaGetRulerPixels(
		    &pfX0Pixels, &pfX1Pixels,
		    &firstIndentPixels, &leftIndentPixels, &rightIndentPixels,
		    &lc, ds->dsHead.dpNode->biParaProperties, &pf ) )
	{ LDEB(1); goto ready;	}

    docPageRectsPixels( &drOutside, &drInside,
				    lc.lcPixelsPerTwip, bodySectNode, bd );

    if  ( docParaGetTabStopsPixels( &tslPixels,
				paraNode->biParaProperties, &pf, &lc, 
				docParaNodeGetTabStopList( paraNode, bd ) ) )
	{ LDEB(1); goto ready;	}

    tedAdaptTopRuler( ed->edTopRuler, ed->edTopRulerWidget,
		drOutside.drX0, drOutside.drX1,		/*  doc */
		drInside.drX0, drInside.drX1,		/*  marg */
		firstIndentPixels, leftIndentPixels, rightIndentPixels,	/*ind*/
		pfX0Pixels, pfX1Pixels, &tslPixels );

    if  ( paraNode->biParaProperties->ppTableNesting > 0	&&
	  sd->sdInOneTable					)
	{
	struct BufferItem *	rowNode;
	int			csCount= 0;
	ColumnSeparator *	cs= (ColumnSeparator *)0;

	int			bfX0Pixels;
	int			bfX1Pixels;

	rowNode= docGetRowNode( paraNode );
	if  ( ! rowNode )
	    { XDEB(rowNode); goto ready;	}

	if  ( docRowGetRulerColumnsPixels( &bfX0Pixels, &bfX1Pixels,
							&cs, &csCount,
							rowNode, &lc, &bf ) )
	    { LDEB(1); goto ready;	}

	tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
				    bfX0Pixels, bfX1Pixels, cs, csCount );

	if  ( cs )
	    { free( cs );	}
	}
    else{
	tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
				    -1, -1, (ColumnSeparator *)0, 0 );
	}

  ready:

    docCleanTabStopList( &tslPixels );
    docLayoutCleanBlockFrame( &bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the top ruler widget of a		*/
/*  document.								*/
/*									*/
/************************************************************************/

int tedDocSetTopRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;

    int				topRulerHeight= ed->edTopRulerHighPixels;
    int				unitInt= ea->eaUnitInt;

    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    const struct BufferItem *	bodySectNode= bd->bdBody.dtRoot->biChildren[0];

    docPageRectsPixels( &drOutside, &drInside,
					td->tdPixelsPerTwip, bodySectNode, bd );

    ed->edTopRuler= tedMakeTopRuler(
			topRulerHeight,		/*  sizeAcross		*/
			ea->eaPixelsPerTwip,
			ea->eaMagnification,	/*  magnification	*/
			&(ea->eaPostScriptFontList),

			ed->edLeftRulerWidePixels,	/*  minUnused	*/
			ed->edRightRulerWidePixels,	/*  maxUnused	*/

			drOutside.drX0, drOutside.drX1,
			drInside.drX0, drInside.drX1,
			ed->edVisibleRect.drX0, ed->edVisibleRect.drX1,

			unitInt );		/*  whatUnit		*/

    guiDrawSetConfigureHandler( ed->edTopRulerWidget,
			    tedTopRulerConfigure, (void *)ed->edTopRuler );

    guiDrawSetRedrawHandler( ed->edTopRulerWidget,
				tedRedrawTopRuler, (void *)ed->edTopRuler );

    guiDrawSetButtonPressHandler( ed->edTopRulerWidget,
					tedTopRulerButtonDown, (void *)ed );

#   if USE_GTK
    gtk_widget_add_events( ed->edTopRulerWidget, GDK_POINTER_MOTION_MASK );
#   endif

    return 0;
    }

int tedDocSetBottomRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    int				bottomRulerHigh= ed->edBottomRulerHighPixels;

    ed->edBottomRuler= tedMakeBottomRuler(
			&(ea->eaPostScriptFontList),
			bottomRulerHigh,		/*  sizeAcross	*/
			ed->edLeftRulerWidePixels,	/*  minUnused	*/
			ed->edRightRulerWidePixels,	/*  maxUnused	*/
			tar->tarPageNumberFormat );

    guiDrawSetConfigureHandler( ed->edBottomRulerWidget,
			tedBottomRulerConfigure, (void *)ed->edBottomRuler );

    guiDrawSetRedrawHandler( ed->edBottomRulerWidget,
			    tedRedrawBottomRuler, (void *)ed->edBottomRuler );

    return 0;
    }
