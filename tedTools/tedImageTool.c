/************************************************************************/
/*									*/
/*  Image page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<limits.h>

#   include	"tedImageTool.h"
#   include	<tedAppFront.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	<docObjectProperties.h>
#   include	<docObject.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<appGuiResource.h>
#   include	<appEditApplication.h>
#   include	<docSelect.h>
#   include	<guiWidgets.h>
#   include	"tedFormatToolSubject.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Image' page.					*/
/*									*/
/************************************************************************/

typedef struct ImagePageResources
    {
    char *		iprObjectType;
    char *		iprImageType;
    char *		iprPixelSize;
    char *		iprTotalBytes;
    char *		iprImageWide;
    char *		iprImageHigh;
    char *		iprXScale;
    char *		iprYScale;
    } ImagePageResources;

/************************************************************************/
/*									*/
/*  A bitmap tool, i.e. the 'Image' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ImageTool
    {
    struct EditApplication *		itApplication;
    struct AppInspector *		itInspector;
    const ImagePageResources *		itPageResources;

    int					itPictureHasRaster;
    int					itTotalBytes;
    BitmapDescription			itRasterDescription;
    int					itObjectType;
    int					itImageType;

    PictureProperties			itPropertiesChosen;
    PictureProperties			itPropertiesSet;

    int					itImageHighAbsChanged;
    int					itImageWideAbsChanged;

    APP_WIDGET				itObjectTypeRow;
    APP_WIDGET				itObjectTypeLabel;
    APP_WIDGET				itObjectTypeText;

    APP_WIDGET				itImageTypeRow;
    APP_WIDGET				itImageTypeLabel;
    APP_WIDGET				itImageTypeText;

    APP_WIDGET				itPixelSizeRow;
    APP_WIDGET				itPixelsSizeLabel;
    APP_WIDGET				itPixelsSizeText;

    APP_WIDGET				itByteSizeRow;
    APP_WIDGET				itByteSizeLabel;
    APP_WIDGET				itByteSizeText;

    APP_WIDGET				itImageWideRow;
    APP_WIDGET				itImageWideLabel;
    APP_WIDGET				itImageWideText;

    APP_WIDGET				itImageHighRow;
    APP_WIDGET				itImageHighLabel;
    APP_WIDGET				itImageHighText;

    APP_WIDGET				itXScaleRow;
    APP_WIDGET				itXScaleLabel;
    APP_WIDGET				itXScaleText;

    APP_WIDGET				itYScaleRow;
    APP_WIDGET				itYScaleLabel;
    APP_WIDGET				itYScaleText;
    } ImageTool;

static ImagePageResources		TED_ImagePageResources;
static InspectorSubjectResources	TED_ImageSubjectResources;

/************************************************************************/
/*									*/
/*  Clean the image tool.						*/
/*									*/
/************************************************************************/

static void tedInitImageTool(	ImageTool *	it )
    {
    docInitPictureProperties( &(it->itPropertiesSet) );
    docInitPictureProperties( &(it->itPropertiesChosen) );
    bmInitDescription( &(it->itRasterDescription) );

    it->itImageHighAbsChanged= 0;
    it->itImageWideAbsChanged= 0;

    it->itPictureHasRaster= 0;
    it->itObjectType= -1;
    it->itImageType= -1;

    return;
    }

static void tedFreeImageTool(	void *	vit )
    {
    ImageTool *			it= (ImageTool *)vit;

    bmCleanDescription( &(it->itRasterDescription) );

    free( it );

    return;
    }

/************************************************************************/
/*									*/
/*  Get/Set absolute values for the dimensions by translating from/to	*/
/*  the scale.								*/
/*									*/
/************************************************************************/

static void tedImageToolShowWideAbs(	ImageTool *	it )
    {
    struct EditApplication *		ea= it->itApplication;
    const PictureProperties *	pip= &(it->itPropertiesChosen);

    guiLengthToText( it->itImageWideText,
		( pip->pipScaleXSet* pip->pipTwipsWide )/ 100, ea->eaUnitInt );

    return;
    }

static void tedImageToolShowHighAbs(	ImageTool *	it )
    {
    struct EditApplication *		ea= it->itApplication;
    const PictureProperties *	pip= &(it->itPropertiesChosen);

    guiLengthToText( it->itImageHighText,
		( pip->pipScaleYSet* pip->pipTwipsHigh )/ 100, ea->eaUnitInt );

    return;
    }

static int tedImageToolGetWideAbs(	ImageTool *	it )
    {
    struct EditApplication *		ea= it->itApplication;
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minSize= 1;
    const int			adaptToMin= 0;

    int				value= 0;
    int				changed= 0;

    if  ( guiLengthFromText( it->itImageWideText,
				&value, &changed, ea->eaUnitInt,
				minSize, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    pip->pipScaleXSet= ( 100* value )/ pip->pipTwipsWide;

    return 0;
    }

static int tedImageToolGetHighAbs(	ImageTool *	it )
    {
    struct EditApplication *		ea= it->itApplication;
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minSize= 1;
    const int			adaptToMin= 0;

    int				value= 0;
    int				changed= 0;

    if  ( guiLengthFromText( it->itImageHighText,
				&value, &changed, ea->eaUnitInt,
				minSize, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    pip->pipScaleYSet= ( 100* value )/ pip->pipTwipsHigh;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Image' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshImagePage(	ImageTool *	it )
    {
    const PictureProperties *	pip= &(it->itPropertiesChosen);
    char			scratch[50];

    if  ( it->itPictureHasRaster )
	{
	const BitmapDescription *	bd= &(it->itRasterDescription);

	sprintf( scratch, "%d x %d", bd->bdPixelsWide, bd->bdPixelsHigh );
	guiStringToText( it->itPixelsSizeText, scratch );
	}
    else{
	guiStringToText( it->itPixelsSizeText, "" );
	}

    if  ( it->itObjectType >= 0 )
	{
	guiStringToText( it->itObjectTypeText,
					docObjectKindStr( it->itObjectType ) );
	}
    else{
	guiStringToText( it->itObjectTypeText, "" );
	}

    if  ( it->itImageType >= 0 )
	{
	guiStringToText( it->itImageTypeText,
					docObjectKindStr( it->itImageType ) );
	}
    else{
	guiStringToText( it->itImageTypeText, "" );
	}

    guiIntegerToText( it->itByteSizeText, it->itTotalBytes );

    tedImageToolShowWideAbs( it );
    tedImageToolShowHighAbs( it );

    guiIntegerToText( it->itXScaleText, pip->pipScaleXSet );
    guiIntegerToText( it->itYScaleText, pip->pipScaleYSet );
    }

static void tedRefreshImageTool( void *				vit,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    ImageTool *		it= (ImageTool *)vit;

    int			part;
    DocumentPosition	dpObject;
    InsertedObject *	io;
    PictureProperties *	pip;

    if  ( ! sd->sdIsObjectSelection )
	{ *pEnabled= 0; return;	}

    if  ( docGetObjectSelection( &part, &dpObject, &io, bd, ds ) )
	{ *pEnabled= 0; return;	}

    it->itPropertiesSet= io->ioPictureProperties;
    it->itPropertiesChosen= io->ioPictureProperties;
    pip= &(it->itPropertiesChosen);

    it->itTotalBytes= io->ioObjectData.mbSize;
    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	)
	{ it->itTotalBytes= io->ioResultData.mbSize; }

    it->itObjectType= io->ioKind;
    if  ( io->ioKind == DOCokOLEOBJECT )
	{ it->itImageType= io->ioResultKind;	}
    else{ it->itImageType= io->ioKind;		}

    if  ( pip->pipType == DOCokDIBITMAP				||
	  pip->pipType == DOCokWBITMAP				||
	  pip->pipType == DOCokPICTPNGBLIP			||
	  pip->pipType == DOCokPICTJPEGBLIP			||
	  ( pip->pipType == DOCokPICTWMETAFILE &&
				    pip->pipMetafileIsRaster )	)
	{
	if  ( ! io->ioRasterImage.riBytes )
	    {
	    XDEB(io->ioRasterImage.riBytes);
	    it->itPictureHasRaster= 0;
	    }
	else{
	    it->itPictureHasRaster= 1;
	    }
	}
    else{
	it->itPictureHasRaster= 0;
	}

    if  ( it->itPictureHasRaster )
	{
	bmCopyDescription( &(it->itRasterDescription),
					&(io->ioRasterImage.riDescription) );
	}
    else{
	bmCleanDescription( &(it->itRasterDescription) );
	bmInitDescription( &(it->itRasterDescription) );
	}

    tedFormatToolRefreshImagePage( it );

    guiEnableWidget( it->itObjectTypeRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itImageTypeRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itPixelSizeRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itByteSizeRow, cmdEnabled[EDITcmdUPD_OBJECT] );

    guiEnableWidget( it->itImageWideRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itImageHighRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itXScaleRow, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( it->itYScaleRow, cmdEnabled[EDITcmdUPD_OBJECT] );

    guiEnableWidget( is->isRevertButton, cmdEnabled[EDITcmdUPD_OBJECT] );
    guiEnableWidget( is->isApplyButton, cmdEnabled[EDITcmdUPD_OBJECT] );

    it->itImageHighAbsChanged= 0;
    it->itImageWideAbsChanged= 0;

    *pPref += 3;
    *pEnabled= 1;
    return;
    }

/************************************************************************/

static int tedImageToolGetChosen(	ImageTool *		it )
    {
    PictureProperties *		pipChosen= &(it->itPropertiesChosen);

    int				minValue= 1;
    const int			adaptToMin= 0;
    int				maxValue= 100000;
    const int			adaptToMax= 1;

    if  ( it->itImageWideAbsChanged )
	{
	if  ( tedImageToolGetWideAbs( it ) )
	    { return -1;	}
	}
    else{
	if  ( guiIntegerFromText( it->itXScaleText,
			    &(pipChosen->pipScaleXSet),
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	}

    if  ( it->itImageHighAbsChanged )
	{
	if  ( tedImageToolGetHighAbs( it ) )
	    { return -1;	}
	}
    else{
	if  ( guiIntegerFromText( it->itYScaleText,
			    &(pipChosen->pipScaleYSet),
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	}
    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeImagePushed, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;
    PictureProperties *		pipChosen= &(it->itPropertiesChosen);
    PictureProperties *		pipSet= &(it->itPropertiesSet);

    PropertyMask		pipCmpMask;
    PropertyMask		pipDifMask;

    if  ( tedImageToolGetChosen( it ) )
	{ return;	}

    utilPropMaskClear( &pipCmpMask );
    utilPropMaskClear( &pipDifMask );
    PROPmaskADD( &pipCmpMask, PIPpropPICSCALE_X );
    PROPmaskADD( &pipCmpMask, PIPpropPICSCALE_Y );

    docPicturePropertyDifference( &pipDifMask, pipChosen, &pipCmpMask, pipSet );

    tedAppSetImageProperties( it->itApplication, &pipDifMask, pipChosen );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertImagePushed, w, voidit )
    {
    ImageTool *	it= (ImageTool *)voidit;

    it->itPropertiesChosen= it->itPropertiesSet;

    tedFormatToolRefreshImagePage( it );

    return;
    }

/************************************************************************/
/*									*/
/*  1)  User wants to validate the scale.. Get scale and display size.	*/
/*  2)  User wants to set the image size.. Get size  and display scale.	*/
/*  3)  Keep track of whether size or scale was last manipulated.	*/
/*									*/
/************************************************************************/

/*  1  */
static void tedImageToolWideRelToAbs(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minScale= 1;
    const int			adaptToMin= 0;

    if  ( guiIntegerFromText( it->itXScaleText,
					    &(pip->pipScaleXSet),
					    minScale, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return;	}

    tedImageToolShowWideAbs( it );

    return;
    }

/*  1  */
static void tedImageToolHighRelToAbs(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minScale= 1;
    const int			adaptToMin= 0;

    if  ( guiIntegerFromText( it->itYScaleText,
					    &(pip->pipScaleYSet),
					    minScale, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return;	}

    tedImageToolShowHighAbs( it );

    return;
    }

/*  2  */
static void tedImageToolWideAbsToRel(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    if  ( tedImageToolGetWideAbs( it ) )
	{ return;	}

    guiIntegerToText( it->itXScaleText, pip->pipScaleXSet );

    return;
    }

/*  2  */
static void tedImageToolHighAbsToRel(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    if  ( tedImageToolGetHighAbs( it ) )
	{ return;	}

    guiIntegerToText( it->itYScaleText, pip->pipScaleYSet );

    return;
    }

/*  1  */
static APP_TXACTIVATE_CALLBACK_H( tedImageXScaleChanged, w, voidit )
    { tedImageToolWideRelToAbs( (ImageTool *)voidit );	}

/*  1  */
static APP_TXACTIVATE_CALLBACK_H( tedImageYScaleChanged, w, voidit )
    { tedImageToolHighRelToAbs( (ImageTool *)voidit );	}

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedImageWideChanged, w, voidit )
    { tedImageToolWideAbsToRel( (ImageTool *)voidit );	}

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedImageHighChanged, w, voidit )
    { tedImageToolHighAbsToRel( (ImageTool *)voidit );	}

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolWideAbsTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageWideAbsChanged= 1;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolHighAbsTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageHighAbsChanged= 1;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolWideRelTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageWideAbsChanged= 0;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolHighRelTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageHighAbsChanged= 0;
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Image' page of the format tool.				*/
/*									*/
/************************************************************************/

static void * tedFormatBuildImagePage(
				int				subjectPage,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const int			textColumns= 10;

    const ImagePageResources *	ipr= &TED_ImagePageResources;
    ImageTool *			it= malloc( sizeof(ImageTool) );

    if  ( ! it )
	{ XDEB(it); return it;	}

    tedInitImageTool( it );

    /**/
    it->itPageResources= ipr;
    it->itInspector= ai;
    it->itApplication= ea;

    /**/
    docInitPictureProperties( &(it->itPropertiesSet) );
    docInitPictureProperties( &(it->itPropertiesChosen) );
    bmInitDescription( &(it->itRasterDescription) );
    it->itPictureHasRaster= 0;
    it->itObjectType= -1;
    it->itImageType= -1;

    /**************/
    guiToolMakeLabelAndTextRow( &(it->itObjectTypeRow),
			&(it->itObjectTypeLabel), &(it->itObjectTypeText),
			pageWidget, ipr->iprObjectType, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(it->itImageTypeRow),
			&(it->itImageTypeLabel), &(it->itImageTypeText),
			pageWidget, ipr->iprImageType, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(it->itPixelSizeRow),
			&(it->itPixelsSizeLabel), &(it->itPixelsSizeText),
			pageWidget, ipr->iprPixelSize, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(it->itByteSizeRow),
			&(it->itByteSizeLabel), &(it->itByteSizeText),
			pageWidget, ipr->iprTotalBytes, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(it->itImageWideRow),
			&(it->itImageWideLabel), &(it->itImageWideText),
			pageWidget, ipr->iprImageWide, textColumns, 1 );

    guiToolMakeLabelAndTextRow( &(it->itImageHighRow),
			&(it->itImageHighLabel), &(it->itImageHighText),
			pageWidget, ipr->iprImageHigh, textColumns, 1 );

    guiToolMakeLabelAndTextRow( &(it->itXScaleRow),
			&(it->itXScaleLabel), &(it->itXScaleText),
			pageWidget, ipr->iprXScale, textColumns, 1 );

    guiToolMakeLabelAndTextRow( &(it->itYScaleRow),
			&(it->itYScaleLabel), &(it->itYScaleText),
			pageWidget, ipr->iprYScale, textColumns, 1 );

    guiSetGotValueCallbackForText( it->itXScaleText,
					tedImageXScaleChanged, (void *)it );
    guiSetGotValueCallbackForText( it->itYScaleText,
					tedImageYScaleChanged, (void *)it );
    guiSetTypingCallbackForText( it->itXScaleText,
					tedImageToolWideRelTyped, (void *)it );
    guiSetTypingCallbackForText( it->itYScaleText,
					tedImageToolHighRelTyped, (void *)it );

    guiSetGotValueCallbackForText( it->itImageWideText,
					tedImageWideChanged, (void *)it );
    guiSetGotValueCallbackForText( it->itImageHighText,
					tedImageHighChanged, (void *)it );
    guiSetTypingCallbackForText( it->itImageWideText,
					tedImageToolWideAbsTyped, (void *)it );
    guiSetTypingCallbackForText( it->itImageHighText,
					tedImageToolHighAbsTyped, (void *)it );

    /**************/

    return (void *)it;
    }

/************************************************************************/
/*									*/
/*  Get Image tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedImageSubjectResourceTable[]=
    {
    APP_RESOURCE( "imageToolImage",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Image" ),
    APP_RESOURCE( "imageToolChangeImage",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Image" ),
    APP_RESOURCE( "imageToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Image */
    /*  no Prev Image */
    /*  no Select Image */
    /*  no Delete Image */
    /*  no Insert Image */
    /*  no Append Image */
    };

static AppConfigurableResource TED_TedImageToolResourceTable[]=
    {
    APP_RESOURCE( "imageToolObjectType",
		    offsetof(ImagePageResources,iprObjectType),
		    "Object Type" ),
    APP_RESOURCE( "imageToolImageType",
		    offsetof(ImagePageResources,iprImageType),
		    "Image Type" ),

    APP_RESOURCE( "imageToolPixelSize",
		    offsetof(ImagePageResources,iprPixelSize),
		    "Pixel Size" ),
    APP_RESOURCE( "imageToolTotalBytes",
		    offsetof(ImagePageResources,iprTotalBytes),
		    "Size in Bytes" ),

    APP_RESOURCE( "imageToolImageWide",
		    offsetof(ImagePageResources,iprImageWide),
		    "Image Wide" ),
    APP_RESOURCE( "imageToolImageHigh",
		    offsetof(ImagePageResources,iprImageHigh),
		    "Image High" ),

    APP_RESOURCE( "imageToolXScale",
		    offsetof(ImagePageResources,iprXScale),
		    "Horizontal Scale (%)" ),
    APP_RESOURCE( "imageToolYScale",
		    offsetof(ImagePageResources,iprYScale),
		    "Vertical Scale (%)" ),
    };

static void tedImageToolGetResourceTable(
					struct EditApplication *		ea,
					ImagePageResources *		ipr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)ipr,
				TED_TedImageToolResourceTable,
				sizeof(TED_TedImageToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedImageSubjectResourceTable,
				sizeof(TED_TedImageSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

int tedImageToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    tedImageToolGetResourceTable( ea,
			&TED_ImagePageResources,
			&TED_ImageSubjectResources );

    ist->istResources= &TED_ImageSubjectResources;

    ist->istBuildTool= tedFormatBuildImagePage;
    /*fts->ftsToolFillChoosers*/
    /*fts->ftsToolFinishPage*/
    ftst->ftstRefreshPage= tedRefreshImageTool;
    ist->istFreeTool= tedFreeImageTool;
    /*fts->ftsToolResetCache*/

    ist->istRevertChanges= tedFormatRevertImagePushed;
    ist->istApplyChanges= tedFormatChangeImagePushed;

    return 0;
    }
