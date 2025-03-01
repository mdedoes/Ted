/************************************************************************/
/*									*/
/*  The Frame page of the Format Tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<limits.h>

#   include	<geoUnit.h>

#   include	"tedFrameTool.h"
#   include	<tedAppFront.h>
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	"tedHeightTool.h"
#   include	<docFrameProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<guiWidgets.h>
#   include	<docAttributes.h>
#   include	"tedFormatToolSubject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Frame' page.					*/
/*									*/
/************************************************************************/

typedef struct FramePageResources
    {
    const char *	fprWidth;

    const char *	fprHorizontal;
    const char *	fprVertical;

    const char *	fprHeightFree;
    const char *	fprHeightAtLeast;
    const char *	fprHeightExactly;

    const char *	fprSetButtonText;
    const char *	fprUnsetButtonText;

    const char *	fprXReference;
    const char *	fprXRefItemTexts[FXref_COUNT];
    const char *	fprYReference;
    const char *	fprYRefItemTexts[FYref_COUNT];

    const char *	fprXPosItemTexts[FXpos_COUNT];
    const char *	fprYPosItemTexts[FYpos_COUNT];
    } FramePageResources;

/************************************************************************/
/*									*/
/*  A frame tool, i.e. the 'Frame' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct FrameTool
    {
    struct EditApplication *	ftApplication;
    struct AppInspector *	ftInspector;
    const FramePageResources *	ftPageResources;
    int				ftUnit;

    FrameProperties		ftPropertiesSet;
    FrameProperties		ftPropertiesChosen;

    unsigned char		ftCanChange;

    APP_WIDGET			ftHorizontalFrame;
    APP_WIDGET			ftHorizontalPaned;
    APP_WIDGET				ftWidthLabel;
    APP_WIDGET				ftWidthText;
    AppOptionmenu			ftXRefMenu;
    AppOptionmenu			ftXPosMenu;
    APP_WIDGET				ftXText;

    APP_WIDGET			ftVerticalFrame;
    APP_WIDGET			ftVerticalPaned;
    HeightChooser			ftHeightChooser;
    AppOptionmenu			ftYRefMenu;
    AppOptionmenu			ftYPosMenu;
    APP_WIDGET				ftYText;

    APP_WIDGET			ftSetRow;
    APP_WIDGET				ftSetButton;
    APP_WIDGET				ftUnsetButton;
    } FrameTool;

static FramePageResources		TED_FramePageResources;
static InspectorSubjectResources	TED_FrameSubjectResources;

/************************************************************************/
/*									*/
/*  Clean a frame tool.							*/
/*									*/
/************************************************************************/

static void tedInitFrameTool(	FrameTool *	ft )
    {
    ft->ftApplication= (struct EditApplication *)0;
    ft->ftInspector= (struct AppInspector *)0;
    ft->ftPageResources= (const FramePageResources *)0;

    ft->ftUnit= UNITtyPOINTS;

    docInitFrameProperties( &(ft->ftPropertiesSet) );
    docInitFrameProperties( &(ft->ftPropertiesChosen) );

    ft->ftWidthLabel= (APP_WIDGET)0;
    ft->ftWidthText= (APP_WIDGET)0;

    tedInitHeightChooser( &(ft->ftHeightChooser) );

    guiInitOptionmenu( &(ft->ftXRefMenu) );
    guiInitOptionmenu( &(ft->ftYRefMenu) );

    ft->ftXText= (APP_WIDGET)0;
    ft->ftYText= (APP_WIDGET)0;

    return;
    }

static void tedFreeFrameTool(	void *	vft )
    {
    /* Nothing
    FrameTool *			ft= (FrameTool *)vft;

    docCleanFrameProperties( &(ft->ftPropertiesSet) );
    docCleanFrameProperties( &(ft->ftPropertiesChosen) );
    */

    free( vft );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the frame page.						*/
/*									*/
/************************************************************************/

static void tedFormatRefreshXPosText(	FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( fp->fpXPosition == FXposXGIVEN )
	{
	guiEnableText( ft->ftXText, 1 );
	guiLengthToText( ft->ftXText,  fp->fpXTwips, ft->ftUnit );
	}
    else{
	guiEnableText( ft->ftXText, 0 );
	guiStringToText( ft->ftXText, "" );
	}

    return;
    }

static void tedFormatRefreshYPosText(	FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( fp->fpYPosition == FYposYGIVEN )
	{
	guiEnableText( ft->ftYText, 1 );
	guiLengthToText( ft->ftYText,  fp->fpYTwips, ft->ftUnit );
	}
    else{
	guiEnableText( ft->ftYText, 0 );
	guiStringToText( ft->ftYText, "" );
	}

    return;
    }

static void tedFormatToolRefreshFramePage( FrameTool *	ft )
    {
    const FrameProperties *	fp= &(ft->ftPropertiesChosen);

    /*  X  */
    guiLengthToText( ft->ftWidthText, fp->fpWideTwips, ft->ftUnit );

    tedFormatRefreshXPosText( ft );

    guiSetOptionmenu( &(ft->ftXRefMenu), fp->fpXReference );
    guiSetOptionmenu( &(ft->ftXPosMenu), fp->fpXPosition );

    /*  Y  */
    tedFormatEnableHeightChooser( &(ft->ftHeightChooser),
						    ft->ftCanChange );
    tedFormatRefreshHeightChooser( &(ft->ftHeightChooser),
						    fp->fpHighTwips );

    tedFormatRefreshYPosText( ft );

    guiSetOptionmenu( &(ft->ftYRefMenu), fp->fpYReference );
    guiSetOptionmenu( &(ft->ftYPosMenu), fp->fpYPosition );
    }

static void tedRefreshFrameTool( void *				vft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry * sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *	bd,
				const unsigned char *		cmdEnabled )
    {
    FrameTool *			ft= (FrameTool *)vft;

    const struct BufferItem *		paraNode;

    paraNode= ds->dsHead.dpNode;

if(1)
{ *pEnabled= 0; return;	}

    if  ( ! paraNode || ! sd->sdInContiguousParagraphs )
	{ *pEnabled= 0; return;	}

    ft->ftPropertiesSet= *docParaNodeGetFrameProperties( (int *)0,
							    paraNode, bd );

    if  ( DOCisFRAME( &(ft->ftPropertiesSet) ) )
	{ *pPref += 2;	}

    docCopyFrameProperties( &(ft->ftPropertiesChosen),
						&(ft->ftPropertiesSet) );

    tedFormatToolRefreshFramePage( ft );

    guiEnableWidget( is->isSelectButton, sd->sdIsSingleFrame );
    guiEnableWidget( is->isDeleteButton, sd->sdIsSingleFrame );

    *pEnabled= ! sd->sdDocumentReadonly;
    return;
    }

/************************************************************************/

static int tedFrameToolGetChosen(	FrameTool *		ft )
    {
    FrameProperties *		fpChosen= &(ft->ftPropertiesChosen);

    int				changed;

    const int			minPos= INT_MIN;
    const int			minWidth= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( tedFormatToolGetHeight( &(fpChosen->fpHighTwips),
						&(ft->ftHeightChooser) ) )
	{ return -1;	}

    if  ( guiLengthFromText( ft->ftWidthText,
					    &(fpChosen->fpWideTwips), &changed,
					    UNITtyPOINTS,
					    minWidth, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( fpChosen->fpXPosition == FXposXGIVEN )
	{
	if  ( guiLengthFromText( ft->ftXText,
					    &(fpChosen->fpXTwips), &changed,
					    UNITtyPOINTS,
					    minPos, adaptToMin,
					    maxValue, adaptToMax ) )
	    { return -1;	}
	}

    if  ( fpChosen->fpYPosition == FYposYGIVEN )
	{
	if  ( guiLengthFromText( ft->ftYText,
					    &(fpChosen->fpYTwips), &changed,
					    UNITtyPOINTS,
					    minPos, adaptToMin,
					    maxValue, adaptToMax ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeFramePushed, w, voidft )
    {
    FrameTool *			ft= (FrameTool *)voidft;
    const FrameProperties *	fpSet= &(ft->ftPropertiesSet);
    const FrameProperties *	fpChosen= &(ft->ftPropertiesChosen);
    int				frameNumberSet;

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ft->ftApplication );

    ParagraphProperties		ppChosen;
    PropertyMask		ppSetMask;

    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    docInitParagraphProperties( &ppChosen );

    if  ( tedFrameToolGetChosen( ft ) )
	{ goto ready;	}

    ppChosen.ppFrameNumber= docFramePropertiesNumber( bd, fpChosen );
    if  ( ppChosen.ppFrameNumber < 0 )
	{ LDEB(ppChosen.ppFrameNumber); goto ready;	}
    frameNumberSet= docFramePropertiesNumber( bd, fpSet );
    if  ( frameNumberSet < 0 )
	{ LDEB(frameNumberSet); goto ready;	}

    utilPropMaskClear( &ppSetMask );
    if  ( ppChosen.ppFrameNumber != frameNumberSet )
	{ PROPmaskADD( &ppSetMask, PPpropFRAME );	}

    tedAppChangeParagraphProperties( ft->ftApplication,
						    &ppSetMask, &ppChosen );

  ready:

    docCleanParagraphProperties( &ppChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  The frame height/width changed, check it.				*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedFrameHeightChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    int			value;

    if  ( ! tedFormatToolGetHeight( &value, &(ft->ftHeightChooser) ) )
	{
	tedFormatRefreshHeightChooser( &(ft->ftHeightChooser), value );
	}
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameWidthChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftWidthText, fp->fpWideTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameXChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftXText, fp->fpXTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedFrameYChanged, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    int			value;
    int			changed;

    tedFormatValidateDimension( &value, &changed,
				    ft->ftYText, fp->fpYTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertFramePushed, w, voidft )
    {
    FrameTool *	ft= (FrameTool *)voidft;

    docCopyFrameProperties( &(ft->ftPropertiesChosen), &(ft->ftPropertiesSet) );

    tedFormatToolRefreshFramePage( ft );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    struct EditApplication *	ea= ft->ftApplication;

    tedAppSelectCurrentFrame( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Delete' button of the frame tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatDeleteFrame, w, voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    struct EditApplication *	ea= ft->ftApplication;

    tedAppDeleteCurrentFrame( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Row buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatSetFrame, w, voidft )
    {
    FrameTool *			ft= (FrameTool *)voidft;
    struct EditApplication *	ea= ft->ftApplication;

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ea );

    if  ( ! ed )
	{ XDEB(ed); return;	}

    XDEB(bd);
    }

static APP_BUTTON_CALLBACK_H( tedFormatUnsetFrame, w, voidft )
    {
    FrameTool *			ft= (FrameTool *)voidft;
    struct EditApplication *	ea= ft->ftApplication;

    struct EditDocument *	ed;
    int				traced;
    struct BufferDocument *	bd= tedFormatCurDoc( &ed, &traced, ea );

    if  ( ! ed )
	{ XDEB(ed); return;	}

    XDEB(bd);
    }

static void tedFrameHeightChosen(	int		how,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    HeightChooser *	hc= &(ft->ftHeightChooser);

    int			defaultValue;

    defaultValue= ft->ftPropertiesChosen.fpHighTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= ft->ftPropertiesSet.fpHighTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

static void tedFrameXRefChosen(		int		ref,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( ref < 0 || ref >= FXref__FRAME_COUNT )
	{ LLDEB(ref,FXref__FRAME_COUNT); return;	}

    fp->fpXReference= ref;

    return;
    }

static void tedFrameXPosChosen(		int		pos,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( pos < 0 || pos >= FXpos_COUNT )
	{ LLDEB(pos,FXpos_COUNT); return;	}

    fp->fpXPosition= pos;
    tedFormatRefreshXPosText( ft );

    return;
    }

/************************************************************************/
/*									*/
/*  Combinations of YRef/Ypos:						*/
/*									*/
/************************************************************************/

static void tedFrameYRefChosen(		int		ref,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( ref < 0 || ref >= FYref__FRAME_COUNT )
	{ LLDEB(ref,FYref__FRAME_COUNT); return;	}

    fp->fpYReference= ref;

    if  ( ! docFrameYAllowed( fp->fpYPosition, fp->fpYReference ) )
	{
	int	pos;

	for ( pos= 0; pos < FYpos_COUNT; pos++ )
	    {
	    if  ( docFrameYAllowed( pos, ref ) )
		{
		fp->fpYPosition= pos;
		guiSetOptionmenu( &(ft->ftYPosMenu), fp->fpYPosition );
		tedFormatRefreshYPosText( ft );
		}
	    }
	}

    return;
    }

static void tedFrameYPosChosen(		int		pos,
					void *		voidft )
    {
    FrameTool *		ft= (FrameTool *)voidft;
    FrameProperties *	fp= &(ft->ftPropertiesChosen);

    if  ( pos < 0 || pos >= FYpos_COUNT )
	{ LLDEB(pos,FYpos_COUNT); return;	}

    fp->fpYPosition= pos;
    tedFormatRefreshYPosText( ft );

    if  ( ! docFrameYAllowed( fp->fpYPosition, fp->fpYReference ) )
	{
	int		ref;

	for ( ref= 0; ref < FYref__FRAME_COUNT; ref++ )
	    {
	    if  ( docFrameYAllowed( pos, ref ) )
		{
		fp->fpYReference= ref;
		guiSetOptionmenu( &(ft->ftYRefMenu), fp->fpYReference );
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the controls to select a frame position.			*/
/*									*/
/************************************************************************/

static void tedFillFramePositionTool(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pText,
				void *				through,
				APP_WIDGET			parent,
				AppOptionmenu *			aom,
				APP_TXACTIVATE_CALLBACK_T	textCallback,
				OptionmenuCallback		menuCallback )
    {
    APP_WIDGET	row;
    APP_WIDGET	text;

    const int	menuColumn= 0;
    const int	menuColspan= 1;
    const int	textColumn= menuColumn+ menuColspan;
    const int	textColspan= 1;
    const int	heightResizable= 0;

    row= guiMakeRowInColumn( parent, 2, heightResizable );

    guiMakeOptionmenuInRow( aom, row, menuColumn, menuColspan,
						    menuCallback, through );

    guiMakeTextInRow( &text, row, textColumn, textColspan, 10, 0 );

    if  ( textCallback )
	{ guiSetGotValueCallbackForText( text, textCallback, through ); }

    *pRow= row;
    *pText= text;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame that controls the horizontal position and size.	*/
/*									*/
/************************************************************************/

static void tedFrameToolMakeHorizontalGroup(
			    FrameTool *				ft,
			    const FramePageResources *		fpr,
			    APP_WIDGET				pageWidget )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    const int	textColumns= 9;

    guiMakeColumnFrameInColumn( &(ft->ftHorizontalFrame),
				&(ft->ftHorizontalPaned),
				pageWidget, fpr->fprHorizontal );

    guiToolMakeLabelAndTextRow( &row, &(ft->ftWidthLabel), &(ft->ftWidthText),
				ft->ftHorizontalPaned,
				fpr->fprWidth, textColumns, 1 );
    guiSetGotValueCallbackForText( ft->ftWidthText,
				tedFrameWidthChanged, (void *)ft );


    tedFillFramePositionTool( &row, &(ft->ftXText), (void *)ft,
				ft->ftHorizontalPaned,
				&(ft->ftXPosMenu),
				tedFrameXChanged,
				tedFrameXPosChosen );

    guiToolMakeLabelAndMenuRow( &row, &(ft->ftXRefMenu),
				&label, ft->ftHorizontalPaned,
				fpr->fprXReference,
				tedFrameXRefChosen, (void *)ft );
    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame that controls the fertical position and size.	*/
/*									*/
/************************************************************************/

static void tedFrameToolMakeVerticalGroup(
			    FrameTool *				ft,
			    const FramePageResources *		fpr,
			    APP_WIDGET				pageWidget )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    guiMakeColumnFrameInColumn( &(ft->ftVerticalFrame),
				&(ft->ftVerticalPaned),
				pageWidget, fpr->fprVertical );

    tedFormatMakeHeightRow( ft, ft->ftVerticalPaned,
				&(ft->ftHeightChooser),
				tedFrameHeightChanged,
				tedFrameHeightChosen );

    tedFillFramePositionTool( &row, &(ft->ftYText), (void *)ft,
				ft->ftVerticalPaned,
				&(ft->ftYPosMenu),
				tedFrameYChanged,
				tedFrameYPosChosen );

    guiToolMakeLabelAndMenuRow( &row, &(ft->ftYRefMenu),
				&label, ft->ftVerticalPaned,
				fpr->fprYReference,
				tedFrameYRefChosen, (void *)ft );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a frame tool. I.E. the 'Frame' page of the format tool.	*/
/*									*/
/************************************************************************/

static void * tedFormatBuildFramePage(
			int					subjectPage,
			struct AppInspector *			ai,
			struct EditApplication *		ea,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const FramePageResources *	fpr= &TED_FramePageResources;

    FrameTool *			ft= malloc( sizeof(FrameTool) );

    if  ( ! ft )
	{ XDEB(ft); return ft;	}

    tedInitFrameTool( ft );

    /**/
    ft->ftPageResources= fpr;
    ft->ftInspector= ai;
    ft->ftApplication= ea;

    /**************/
    tedFrameToolMakeHorizontalGroup( ft, fpr, pageWidget );
    tedFrameToolMakeVerticalGroup( ft, fpr, pageWidget );

    /**************/

    guiToolMake2BottonRow( &(ft->ftSetRow), pageWidget,
	    &(ft->ftSetButton), &(ft->ftUnsetButton),
	    fpr->fprSetButtonText, fpr->fprUnsetButtonText,
	    tedFormatSetFrame, tedFormatUnsetFrame, (void *)ft );

    return (void *)ft;
    }

static void tedFrameToolFillChoosers( void *	vft )
    {
    FrameTool *			ft= (FrameTool *)vft;
    const FramePageResources *	fpr= &TED_FramePageResources;

    tedFormatFillHeightChooser( &(ft->ftHeightChooser),
					    fpr->fprHeightFree,
					    fpr->fprHeightAtLeast,
					    fpr->fprHeightExactly );

    appFillInspectorMenu( FXref__FRAME_COUNT, FXrefCOLUMN,
					    fpr->fprXRefItemTexts,
					    &(ft->ftXRefMenu) );

    appFillInspectorMenu( FXpos_COUNT, FXposXL,
					    fpr->fprXPosItemTexts,
					    &(ft->ftXPosMenu) );

    appFillInspectorMenu( FYref__FRAME_COUNT, FYrefMARGIN,
					    fpr->fprYRefItemTexts,
					    &(ft->ftYRefMenu) );

    appFillInspectorMenu( FYpos_COUNT, FYposYT,
					    fpr->fprYPosItemTexts,
					    &(ft->ftYPosMenu) );
    }

static void tedFinishFrameTool( void *	vft )
    {
    FrameTool *			ft= (FrameTool *)vft;

    guiOptionmenuRefreshWidth( &(ft->ftHeightChooser.hcOptionmenu) );

    guiOptionmenuRefreshWidth( &(ft->ftXRefMenu) );
    guiOptionmenuRefreshWidth( &(ft->ftXPosMenu) );

    guiOptionmenuRefreshWidth( &(ft->ftYRefMenu) );
    guiOptionmenuRefreshWidth( &(ft->ftYPosMenu) );
    }

/************************************************************************/
/*									*/
/*  Get frame tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedFrameSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolFrame",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Frame" ),
    APP_RESOURCE( "frameToolChangeRow",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply" ),
    APP_RESOURCE( "frameToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "frameToolSelectFrame",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Frame" ),
    APP_RESOURCE( "frameToolDeleteFrame",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Frame" ),
    };

static AppConfigurableResource TED_TedFrameToolResourceTable[]=
    {
    APP_RESOURCE( "frameToolHorizontal",
	    offsetof(FramePageResources,fprHorizontal),
	    "Horizontal" ),
    APP_RESOURCE( "frameToolVertical",
	    offsetof(FramePageResources,fprVertical),
	    "Vertical" ),

    APP_RESOURCE( "frameToolWidth",
	    offsetof(FramePageResources,fprWidth),
	    "Width" ),

    /**/
    APP_RESOURCE( "frameToolHeightFree",
	    offsetof(FramePageResources,fprHeightFree),
	    "Height Free" ),
    APP_RESOURCE( "frameToolHeightExactly",
	    offsetof(FramePageResources,fprHeightExactly),
	    "Height Exactly" ),
    APP_RESOURCE( "frameToolHeightAtLeast",
	    offsetof(FramePageResources,fprHeightAtLeast),
	    "Height at Least" ),

    /**/
    APP_RESOURCE( "frameToolSetFrame",
	    offsetof(FramePageResources,fprSetButtonText),
	    "Set" ),
    APP_RESOURCE( "frameToolSetFrame",
	    offsetof(FramePageResources,fprUnsetButtonText),
	    "Unset" ),

    /**/
    APP_RESOURCE( "frameToolXPosGiven",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXGIVEN]),
		"X at" ),
    APP_RESOURCE( "frameToolXPosLeft",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXL]),
		"X Left" ),
    APP_RESOURCE( "frameToolXPosRight",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXR]),
		"X Right" ),
    APP_RESOURCE( "frameToolXPosCenter",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXC]),
		"X Centered" ),
    APP_RESOURCE( "frameToolXPosInside",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXI]),
		"X Inside" ),
    APP_RESOURCE( "frameToolXPosOutside",
		offsetof(FramePageResources,fprXPosItemTexts[FXposXO]),
		"X Outside" ),
    APP_RESOURCE( "frameToolXReference",
	    offsetof(FramePageResources,fprXReference),
	    "Relative To" ),
    APP_RESOURCE( "frameToolXRefColumn",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefCOLUMN]),
		"Column" ),
    APP_RESOURCE( "frameToolXRefPage",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefPAGE]),
		"Page" ),
    APP_RESOURCE( "frameToolXRefMargin",
		offsetof(FramePageResources,fprXRefItemTexts[FXrefMARGIN]),
		"Margin" ),
    APP_RESOURCE( "frameToolYPosGiven",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYGIVEN]),
		"Y at" ),
    APP_RESOURCE( "frameToolYPosInside",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYIN]),
		"Y Inside" ),
    APP_RESOURCE( "frameToolYPosOutside",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYOUT]),
		"Y Outside" ),
    APP_RESOURCE( "frameToolYPosTop",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYT]),
		"Y at Top" ),
    APP_RESOURCE( "frameToolYPosCenter",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYC]),
		"Y at Center" ),
    APP_RESOURCE( "frameToolYPosBottom",
		offsetof(FramePageResources,fprYPosItemTexts[FYposYB]),
		"Y at Bottom" ),
    APP_RESOURCE( "frameToolYReference",
	    offsetof(FramePageResources,fprYReference),
	    "Relative To" ),
    APP_RESOURCE( "frameToolYRefMargin",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefMARGIN]),
		"Margin" ),
    APP_RESOURCE( "frameToolYRefPage",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefPAGE]),
		"Page" ),
    APP_RESOURCE( "frameToolYRefParagraph",
		offsetof(FramePageResources,fprYRefItemTexts[FYrefPARA]),
		"Paragraph" ),
    };

static void tedFrameToolGetResourceTable(
					struct EditApplication *	ea,
					FramePageResources *		fpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)fpr,
				TED_TedFrameToolResourceTable,
				sizeof(TED_TedFrameToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedFrameSubjectResourceTable,
				sizeof(TED_TedFrameSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedFrameToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedFrameToolGetResourceTable( ea,
				&TED_FramePageResources,
				&TED_FrameSubjectResources );

    ist->istResources= &TED_FrameSubjectResources;

    ist->istBuildTool= tedFormatBuildFramePage;
    ist->istFillChoosers= tedFrameToolFillChoosers;
    ist->istFinishPage= tedFinishFrameTool;
    ftst->ftstRefreshPage= tedRefreshFrameTool;
    ist->istFreeTool= tedFreeFrameTool;
    /*fts->ftsToolResetCache*/

    ist->istSelectInstance= tedFormatSelectFrame;
    ist->istDeleteInstance= tedFormatDeleteFrame;

    ist->istRevertChanges= tedFormatRevertFramePushed;
    ist->istApplyChanges= tedFormatChangeFramePushed;

    return 0;
    }
