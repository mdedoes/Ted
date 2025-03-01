/************************************************************************/
/*									*/
/*  Table of rtf tags that are not treated as an exception.		*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfShapeTab.h"
#   include	"docRtfTagHash.h"
#   include	"docRtfShapePropLookup.h"
#   include	"docRtfFindProperty.h"
#   include	"docRtfMakeLookup.h"
#   include	<docShapeProp.h>

#   include	<appDebugon.h>

#   define	BUILD_NO_HASH				/*  for sed */
#   define	BUILD_NO_LOOKUP				/*  for sed */

# if defined(BUILD_HASH) || defined(BUILD_LOOKUP)

#	define		docRtfShpArray				0
#	define		docRtfShpString				0
#	define		docRtfShpPicture			0
#	define		docRtfShapeNumber			0

#   endif

/************************************************************************/
/*									*/
/*  Table of common shape properties.					*/
/*									*/
/************************************************************************/

# define	SHAPE_ARRAY(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeANY, docRtfShpArray, }
# define	SHAPE_STRING(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeANY, docRtfShpString, }
# define	SHAPE_PICTURE(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeANY, docRtfShpPicture, }
# define	SHAPE_COLOR(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeANY, docRtfShapeNumber, }

# define	SHAPE_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_POS_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_POS_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_TYPE_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_TYPE_NUMBER_X(s,id) \
		    { s, RTCscopeANY, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_TYPE_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_LOCK_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_FILL_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_FILL_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_LINE_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_LINE_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }
# define	SHAPE_LINE_FLAG_X(s,id) \
		    { s, RTCscopeANY, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_GROUP_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_GROUP_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_SHADOW_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_SHADOW_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_CONN_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }

# define	SHAPE_TXBOX_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }
# define	SHAPE_TXBOX_FLAG(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeFLAG, docRtfShapeNumber, }

# define	SHAPE_GEO_NUMBER(s,id) \
		    { s, RTCscopeSHAPE, id, RTCtypeNUMBER, docRtfShapeNumber, }

static RtfControlWord	docRtfShapePropertyWords[]=
{
		    /*
		     *  String
		     */
    SHAPE_STRING( "wzName",		DSHPprop_wzName ),
    SHAPE_STRING( "wzDescription",	DSHPprop_wzDescription ),
    SHAPE_STRING( "gtextUNICODE",	DSHPprop_gtextUNICODE ),
    SHAPE_STRING( "gtextFont",		DSHPprop_gtextFont ),
    SHAPE_STRING( "pibName",		DSHPprop_pibName ),
    SHAPE_STRING( "fillBlipName",	DSHPprop_fillBlipName ),

		    /*
		     *  Picture
		     */
    SHAPE_PICTURE( "pib",		DSHPprop_pib ),

		    /*
		     *  Geometry: Paths
		     */
    SHAPE_ARRAY( "pVerticies",	 	DSHPprop_pVerticies ),
    SHAPE_ARRAY( "pWrapPolygonVertices",DSHPprop_pWrapPolygonVertices ),
    SHAPE_ARRAY( "pSegmentInfo",	DSHPprop_pSegmentInfo ),

			    /********************************************/
			    /*   Number/Flag: Position.			*/
			    /********************************************/
    SHAPE_POS_NUMBER( "posv",		DSHPprop_posv ),
    SHAPE_POS_NUMBER( "posh",		DSHPprop_posh ),
    SHAPE_POS_NUMBER( "posrelv",	DSHPprop_posrelv ),
    SHAPE_POS_NUMBER( "posrelh",	DSHPprop_posrelh ),

    SHAPE_POS_FLAG( "fUseShapeAnchor",	DSHPprop_fUseShapeAnchor ),
    SHAPE_POS_FLAG( "fLayoutInCell",	DSHPprop_fLayoutInCell ),
    SHAPE_POS_FLAG( "fAllowOverlap",	DSHPprop_fAllowOverlap ),
    SHAPE_POS_FLAG( "fChangePage",	DSHPprop_fChangePage ),

			    /********************************************/
			    /*						*/
			    /*  Object type.				*/
			    /*  1)  To deal with faulty documents.	*/
			    /*						*/
			    /********************************************/
    SHAPE_TYPE_NUMBER( "shapeType",		DSHPprop_shapeType ),
    SHAPE_TYPE_NUMBER_X( "ShapeType",		DSHPprop_shapeType ), /* 1 */
    SHAPE_TYPE_NUMBER( "rotation",		DSHPprop_rotation ),

    SHAPE_TYPE_NUMBER( "dxWrapDistLeft",	DSHPprop_dxWrapDistLeft ),
    SHAPE_TYPE_NUMBER( "dyWrapDistTop",		DSHPprop_dyWrapDistTop ),
    SHAPE_TYPE_NUMBER( "dxWrapDistRight",	DSHPprop_dxWrapDistRight ),
    SHAPE_TYPE_NUMBER( "dyWrapDistBottom",	DSHPprop_dyWrapDistBottom ),

    SHAPE_TYPE_NUMBER( "hspMaster",		DSHPprop_hspMaster ),
    SHAPE_TYPE_NUMBER( "hspNext",		DSHPprop_hspNext ),

    SHAPE_TYPE_NUMBER( "xLimo",			DSHPprop_xLimo ),
    SHAPE_TYPE_NUMBER( "yLimo",			DSHPprop_yLimo ),

    SHAPE_TYPE_FLAG( "fIsBullet",		DSHPprop_fIsBullet ),
    SHAPE_TYPE_FLAG( "fFlipV",			DSHPprop_fFlipV ),
    SHAPE_TYPE_FLAG( "fFlipH",			DSHPprop_fFlipH ),
    SHAPE_TYPE_FLAG( "fBehindDocument",		DSHPprop_fBehindDocument ),
    SHAPE_TYPE_FLAG( "fIsButton",		DSHPprop_fIsButton ),
    SHAPE_TYPE_FLAG( "fHidden",			DSHPprop_fHidden ),
    SHAPE_TYPE_FLAG( "fReallyHidden",		DSHPprop_fReallyHidden ),
    SHAPE_TYPE_FLAG( "fArrowheadsOK",		DSHPprop_fArrowheadsOK ),
    SHAPE_TYPE_FLAG( "fBackground",		DSHPprop_fBackground ),
    SHAPE_TYPE_FLAG( "fDeleteAttachedObject",	DSHPprop_fDeleteAttachedObject ),
    SHAPE_TYPE_FLAG( "fEditedWrap",		DSHPprop_fEditedWrap ),
    SHAPE_TYPE_FLAG( "fHitTestFill",		DSHPprop_fHitTestFill ),
    SHAPE_TYPE_FLAG( "fHitTestLine",		DSHPprop_fHitTestLine ),
    SHAPE_TYPE_FLAG( "fInitiator",		DSHPprop_fInitiator ),
    SHAPE_TYPE_FLAG( "fNoFillHitTest",		DSHPprop_fNoFillHitTest ),
    SHAPE_TYPE_FLAG( "fNoHitTestPicture",	DSHPprop_fNoHitTestPicture ),
    SHAPE_TYPE_FLAG( "fNoLineDrawDash",		DSHPprop_fNoLineDrawDash ),
    SHAPE_TYPE_FLAG( "fOleIcon",		DSHPprop_fOleIcon ),
    SHAPE_TYPE_FLAG( "fOnDblClickNotify",	DSHPprop_fOnDblClickNotify ),
    SHAPE_TYPE_FLAG( "fOneD",			DSHPprop_fOneD ),
    SHAPE_TYPE_FLAG( "fPreferRelativeSize",	DSHPprop_fPreferRelativeSize ),
    SHAPE_TYPE_FLAG( "fPrint",			DSHPprop_fPrint ),

    SHAPE_TYPE_FLAG( "fPseudoInline",		DSHPprop_fPseudoInline ),

			    /********************************************/
			    /*  Text Box.				*/
			    /********************************************/
    SHAPE_TXBOX_NUMBER( "dxTextLeft",		DSHPprop_dxTextLeft ),
    SHAPE_TXBOX_NUMBER( "dyTextTop",		DSHPprop_dyTextTop ),
    SHAPE_TXBOX_NUMBER( "dxTextRight",		DSHPprop_dxTextRight ),
    SHAPE_TXBOX_NUMBER( "dyTextBottom",		DSHPprop_dyTextBottom ),
    SHAPE_TXBOX_NUMBER( "scaleText",		DSHPprop_scaleText ),
    SHAPE_TXBOX_NUMBER( "lTxid",		DSHPprop_lTxid ),
    SHAPE_TXBOX_NUMBER( "WrapText",		DSHPprop_WrapText ),
    SHAPE_TXBOX_NUMBER( "anchorText",		DSHPprop_anchorText ),
    SHAPE_TXBOX_NUMBER( "txflTextFlow",		DSHPprop_txflTextFlow ),
    SHAPE_TXBOX_NUMBER( "cdirFont",		DSHPprop_cdirFont ),
    SHAPE_TXBOX_FLAG( "fAutoTextMargin",	DSHPprop_fAutoTextMargin ),
    SHAPE_TXBOX_FLAG( "fRotateText",		DSHPprop_fRotateText ),
    SHAPE_TXBOX_FLAG( "fSelectText",		DSHPprop_fSelectText ),
    SHAPE_TXBOX_FLAG( "fFitShapeToText",	DSHPprop_fFitShapeToText ),
    SHAPE_TXBOX_FLAG( "fFitTextToShape",	DSHPprop_fFitTextToShape ),

			    /********************************************/
			    /*						*/
			    /*  Line.					*/
			    /*  1)  Deal with faulty documents.		*/
			    /*						*/
			    /********************************************/
    SHAPE_LINE_NUMBER( "lineType",		DSHPprop_lineType ),

    SHAPE_LINE_NUMBER( "lineFillBlipFlags",	DSHPprop_lineFillBlipFlags ),

    SHAPE_LINE_NUMBER( "lineFillWidth",		DSHPprop_lineFillWidth ),
    SHAPE_LINE_NUMBER( "lineFillHeight",	DSHPprop_lineFillHeight ),
    SHAPE_LINE_NUMBER( "lineWidth",		DSHPprop_lineWidth ),

    SHAPE_LINE_NUMBER( "lineStyle",		DSHPprop_lineStyle ),
    SHAPE_LINE_NUMBER( "lineDashing",		DSHPprop_lineDashing ),
    SHAPE_LINE_NUMBER( "lineDashStyle",		DSHPprop_lineDashStyle ),
    SHAPE_LINE_NUMBER( "lineStartArrowhead",	DSHPprop_lineStartArrowhead ),
    SHAPE_LINE_NUMBER( "lineEndArrowhead",	DSHPprop_lineEndArrowhead ),
    SHAPE_LINE_NUMBER( "lineStartArrowWidth",	DSHPprop_lineStartArrowWidth ),
    SHAPE_LINE_NUMBER( "lineStartArrowLength",	DSHPprop_lineStartArrowLength ),
    SHAPE_LINE_NUMBER( "lineEndArrowWidth",	DSHPprop_lineEndArrowWidth ),
    SHAPE_LINE_NUMBER( "lineEndArrowLength",	DSHPprop_lineEndArrowLength ),
    SHAPE_LINE_NUMBER( "lineEndCapStyle",	DSHPprop_lineEndCapStyle ),
    SHAPE_LINE_NUMBER( "lineFillDztype",	DSHPprop_lineFillDztype ),
    SHAPE_LINE_NUMBER( "lineJoinStyle",		DSHPprop_lineJoinStyle ),

    SHAPE_LINE_NUMBER( "lineMiterLimit",	DSHPprop_lineMiterLimit ),

    SHAPE_LINE_FLAG( "fLine",			DSHPprop_fLine ),
    SHAPE_LINE_FLAG_X( "fline",			DSHPprop_fLine ), /* 1 */
    SHAPE_LINE_FLAG( "fLineOK",			DSHPprop_fLineOK ),

    SHAPE_LINE_FLAG( "fLineUseShapeAnchor",	DSHPprop_fLineUseShapeAnchor ),
    SHAPE_LINE_FLAG( "fColumnLineOK",		DSHPprop_fColumnLineOK ),
    SHAPE_LINE_FLAG( "fColumnLine",		DSHPprop_fColumnLine ),
    SHAPE_LINE_FLAG( "fLeftLine",		DSHPprop_fLeftLine ),
    SHAPE_LINE_FLAG( "fTopLine",		DSHPprop_fTopLine ),
    SHAPE_LINE_FLAG( "fRightLine",		DSHPprop_fRightLine ),
    SHAPE_LINE_FLAG( "fBottomLine",		DSHPprop_fBottomLine ),

    SHAPE_LINE_FLAG( "fColumnHitTestLine",	DSHPprop_fColumnHitTestLine ),
    SHAPE_LINE_FLAG( "fLeftHitTestLine",	DSHPprop_fLeftHitTestLine ),
    SHAPE_LINE_FLAG( "fTopHitTestLine",	DSHPprop_fTopHitTestLine ),
    SHAPE_LINE_FLAG( "fRightHitTestLine",	DSHPprop_fRightHitTestLine ),
    SHAPE_LINE_FLAG( "fBottomHitTestLine",	DSHPprop_fBottomHitTestLine ),

    SHAPE_LINE_NUMBER( "lineFillShape",		DSHPprop_lineFillShape ),
    SHAPE_LINE_NUMBER( "lineColumnFillShape",	DSHPprop_lineColumnFillShape ),
    SHAPE_LINE_NUMBER( "lineLeftFillShape",	DSHPprop_lineLeftFillShape ),
    SHAPE_LINE_NUMBER( "lineTopFillShape",	DSHPprop_lineTopFillShape ),
    SHAPE_LINE_NUMBER( "lineRightFillShape",	DSHPprop_lineRightFillShape ),
    SHAPE_LINE_NUMBER( "lineBottomFillShape",	DSHPprop_lineBottomFillShape ),

    SHAPE_LINE_FLAG( "fInsetPen",		DSHPprop_fInsetPen ),
    SHAPE_LINE_FLAG( "fLeftInsetPen",		DSHPprop_fLeftInsetPen ),
    SHAPE_LINE_FLAG( "fTopInsetPen",		DSHPprop_fTopInsetPen ),
    SHAPE_LINE_FLAG( "fRightInsetPen",		DSHPprop_fRightInsetPen ),
    SHAPE_LINE_FLAG( "fBottomInsetPen",		DSHPprop_fBottomInsetPen ),

    SHAPE_COLOR( "lineColor",			DSHPprop_lineColor ),
    SHAPE_COLOR( "lineBackColor",		DSHPprop_lineBackColor ),
    SHAPE_COLOR( "lineCrMod",			DSHPprop_lineCrMod ),

			    /********************************************/
			    /*  Lock.					*/
			    /********************************************/
    SHAPE_LOCK_FLAG( "fLockRotation",		DSHPprop_fLockRotation ),
    SHAPE_LOCK_FLAG( "fLockAspectRatio",	DSHPprop_fLockAspectRatio ),
    SHAPE_LOCK_FLAG( "fLockAgainstSelect",	DSHPprop_fLockAgainstSelect ),
    SHAPE_LOCK_FLAG( "fLockCropping",		DSHPprop_fLockCropping ),
    SHAPE_LOCK_FLAG( "fLockVerticies",		DSHPprop_fLockVerticies ),
    SHAPE_LOCK_FLAG( "fLockText",		DSHPprop_fLockText ),
    SHAPE_LOCK_FLAG( "fLockAdjustHandles",	DSHPprop_fLockAdjustHandles ),
    SHAPE_LOCK_FLAG( "fLockAgainstGrouping",	DSHPprop_fLockAgainstGrouping ),
    SHAPE_LOCK_FLAG( "fLockShapeType",		DSHPprop_fLockShapeType ),

    /*?*/
    SHAPE_LOCK_FLAG( "fLockPosition",		DSHPprop_fLockPosition ),

			    /********************************************/
			    /*  Fill.					*/
			    /********************************************/
    SHAPE_FILL_NUMBER( "fillType",		DSHPprop_fillType ),
    SHAPE_FILL_NUMBER( "fillOpacity",		DSHPprop_fillOpacity ),
    SHAPE_FILL_NUMBER( "fillBackOpacity",	DSHPprop_fillBackOpacity ),

    SHAPE_FILL_NUMBER( "fillblipflags",		DSHPprop_fillblipflags ),

    SHAPE_FILL_NUMBER( "fillWidth",		DSHPprop_fillWidth ),
    SHAPE_FILL_NUMBER( "fillHeight",		DSHPprop_fillHeight ),
    SHAPE_FILL_NUMBER( "fillAngle",		DSHPprop_fillAngle ),
    SHAPE_FILL_NUMBER( "fillFocus",		DSHPprop_fillFocus ),

    SHAPE_FILL_NUMBER( "fillToLeft",		DSHPprop_fillToLeft ),
    SHAPE_FILL_NUMBER( "fillToTop",		DSHPprop_fillToTop ),
    SHAPE_FILL_NUMBER( "fillToRight",		DSHPprop_fillToRight ),
    SHAPE_FILL_NUMBER( "fillToBottom",		DSHPprop_fillToBottom ),

    SHAPE_FILL_NUMBER( "fillOriginX",		DSHPprop_fillOriginX ),
    SHAPE_FILL_NUMBER( "fillOriginY",		DSHPprop_fillOriginY ),
    SHAPE_FILL_NUMBER( "fillShapeOriginX",	DSHPprop_fillShapeOriginX ),
    SHAPE_FILL_NUMBER( "fillShapeOriginY",	DSHPprop_fillShapeOriginY ),

    SHAPE_FILL_NUMBER( "fillDztype",		DSHPprop_fillDztype ),

    SHAPE_FILL_NUMBER( "fillRectLeft",		DSHPprop_fillRectLeft ),
    SHAPE_FILL_NUMBER( "fillRectTop",		DSHPprop_fillRectTop ),
    SHAPE_FILL_NUMBER( "fillRectRight",		DSHPprop_fillRectRight ),
    SHAPE_FILL_NUMBER( "fillRectBottom",	DSHPprop_fillRectBottom ),

    SHAPE_FILL_NUMBER( "fillShadePreset",	DSHPprop_fillShadePreset ),
    SHAPE_FILL_NUMBER( "fillShadeType",		DSHPprop_fillShadeType ),
    SHAPE_FILL_NUMBER( "fillShape",		DSHPprop_fillShape ),
    SHAPE_FILL_NUMBER( "fillUseRect",		DSHPprop_fillUseRect ),

    SHAPE_FILL_FLAG( "fFilled",			DSHPprop_fFilled ),
    SHAPE_FILL_FLAG( "fFillOK",			DSHPprop_fFillOK ),
    SHAPE_FILL_FLAG( "fFillShadeShapeOK",	DSHPprop_fFillShadeShapeOK ),

    SHAPE_COLOR( "fillColor",			DSHPprop_fillColor ),
    SHAPE_COLOR( "fillBackColor",		DSHPprop_fillBackColor ),
    SHAPE_COLOR( "fillCrMod",			DSHPprop_fillCrMod ),

				    /************************************/
				    /*  Geometry			*/
				    /************************************/

    SHAPE_GEO_NUMBER( "geoLeft",		DSHPprop_geoLeft ),
    SHAPE_GEO_NUMBER( "geoTop",			DSHPprop_geoTop ),
    SHAPE_GEO_NUMBER( "geoRight",		DSHPprop_geoRight ),
    SHAPE_GEO_NUMBER( "geoBottom",		DSHPprop_geoBottom ),
    SHAPE_GEO_NUMBER( "adjustValue",		DSHPprop_adjustValue ),
    SHAPE_GEO_NUMBER( "adjust2Value",		DSHPprop_adjust2Value ),
    SHAPE_GEO_NUMBER( "adjust3Value",		DSHPprop_adjust3Value ),
    SHAPE_GEO_NUMBER( "adjust4Value",		DSHPprop_adjust4Value ),
    SHAPE_GEO_NUMBER( "adjust5Value",		DSHPprop_adjust5Value ),
    SHAPE_GEO_NUMBER( "adjust6Value",		DSHPprop_adjust6Value ),
    SHAPE_GEO_NUMBER( "adjust7Value",		DSHPprop_adjust7Value ),
    SHAPE_GEO_NUMBER( "adjust8Value",		DSHPprop_adjust8Value ),
    SHAPE_GEO_NUMBER( "adjust9Value",		DSHPprop_adjust9Value ),
    SHAPE_GEO_NUMBER( "adjust10Value",		DSHPprop_adjust10Value ),

				    /************************************/
				    /*  Grouped Shapes.			*/
				    /************************************/

    SHAPE_GROUP_NUMBER( "relLeft", 		DSHPprop_relLeft ),
    SHAPE_GROUP_NUMBER( "relTop", 		DSHPprop_relTop ),
    SHAPE_GROUP_NUMBER( "relRight", 		DSHPprop_relRight ),
    SHAPE_GROUP_NUMBER( "relBottom", 		DSHPprop_relBottom ),

    SHAPE_GROUP_NUMBER( "relRotation", 		DSHPprop_relRotation ),
    SHAPE_GROUP_NUMBER( "lidRegroup", 		DSHPprop_lidRegroup ),

    SHAPE_GROUP_NUMBER( "groupLeft", 		DSHPprop_groupLeft ),
    SHAPE_GROUP_NUMBER( "groupTop", 		DSHPprop_groupTop ),
    SHAPE_GROUP_NUMBER( "groupRight", 		DSHPprop_groupRight ),
    SHAPE_GROUP_NUMBER( "groupBottom", 		DSHPprop_groupBottom ),

    SHAPE_GROUP_FLAG( "fRelChangePage", 	DSHPprop_fRelChangePage ),
    SHAPE_GROUP_FLAG( "fRelFlipH", 		DSHPprop_fRelFlipH ),
    SHAPE_GROUP_FLAG( "fRelFlipV", 		DSHPprop_fRelFlipV ),

			    /********************************************/
			    /*  Shadow.					*/
			    /********************************************/

    SHAPE_SHADOW_NUMBER( "shadowType",		DSHPprop_shadowType ),
    SHAPE_SHADOW_NUMBER( "shadowOpacity",	DSHPprop_shadowOpacity ),
    SHAPE_SHADOW_NUMBER( "shadowOffsetX",	DSHPprop_shadowOffsetX ),
    SHAPE_SHADOW_NUMBER( "shadowOffsetY",	DSHPprop_shadowOffsetY ),
    SHAPE_SHADOW_NUMBER( "shadowSecondOffsetX",	DSHPprop_shadowSecondOffsetX ),
    SHAPE_SHADOW_NUMBER( "shadowSecondOffsetY",	DSHPprop_shadowSecondOffsetY ),
    SHAPE_SHADOW_NUMBER( "shadowScaleXToX",	DSHPprop_shadowScaleXToX ),
    SHAPE_SHADOW_NUMBER( "shadowScaleYToX",	DSHPprop_shadowScaleYToX ),
    SHAPE_SHADOW_NUMBER( "shadowScaleXToY",	DSHPprop_shadowScaleXToY ),
    SHAPE_SHADOW_NUMBER( "shadowScaleYToY",	DSHPprop_shadowScaleYToY ),
    SHAPE_SHADOW_NUMBER( "shadowPerspectiveX",	DSHPprop_shadowPerspectiveX ),
    SHAPE_SHADOW_NUMBER( "shadowPerspectiveY",	DSHPprop_shadowPerspectiveY ),
    SHAPE_SHADOW_NUMBER( "shadowWeight",	DSHPprop_shadowWeight ),
    SHAPE_SHADOW_NUMBER( "shadowOriginX",	DSHPprop_shadowOriginX ),
    SHAPE_SHADOW_NUMBER( "shadowOriginY",	DSHPprop_shadowOriginY ),

    SHAPE_SHADOW_FLAG( "fShadow",		DSHPprop_fShadow ),
    SHAPE_SHADOW_FLAG( "fshadowObscured", 	DSHPprop_fshadowObscured ),
    SHAPE_SHADOW_FLAG( "fShadowOK",		DSHPprop_fShadowOK ),

    SHAPE_COLOR( "shadowCrMod",			DSHPprop_shadowCrMod ),
    SHAPE_COLOR( "shadowColor",			DSHPprop_shadowColor ),
    SHAPE_COLOR( "shadowHighlight",		DSHPprop_shadowHighlight ),

			    /********************************************/
			    /*  Connectors.				*/
			    /********************************************/

    SHAPE_CONN_NUMBER( "cxk", 			DSHPprop_cxk ),
    SHAPE_CONN_NUMBER( "cxstyle", 		DSHPprop_cxstyle ),

    SHAPE_NUMBER( "gtextAlign",			DSHPprop_gtextAlign ),
    SHAPE_FLAG( "fGtext",			DSHPprop_fGtext ),
    SHAPE_FLAG( "gtextFVertical",		DSHPprop_gtextFVertical ),
    SHAPE_FLAG( "gtextFKern",			DSHPprop_gtextFKern ),
    SHAPE_FLAG( "gtextFTight",			DSHPprop_gtextFTight ),
    SHAPE_FLAG( "gtextFStretch",		DSHPprop_gtextFStretch ),
    SHAPE_FLAG( "gtextFShrinkFit",		DSHPprop_gtextFShrinkFit ),
    SHAPE_FLAG( "gtextFBestFit",		DSHPprop_gtextFBestFit ),
    SHAPE_FLAG( "gtextFNormalize",		DSHPprop_gtextFNormalize ),
    SHAPE_FLAG( "gtextFDxMeasure",		DSHPprop_gtextFDxMeasure ),
    SHAPE_FLAG( "gtextFBold",			DSHPprop_gtextFBold ),
    SHAPE_FLAG( "gtextFItalic",			DSHPprop_gtextFItalic ),
    SHAPE_FLAG( "gtextFUnderline",		DSHPprop_gtextFUnderline ),
    SHAPE_FLAG( "gtextFShadow",			DSHPprop_gtextFShadow ),
    SHAPE_FLAG( "gtextFSmallcaps",		DSHPprop_gtextFSmallcaps ),
    SHAPE_FLAG( "gtextFStrikethrough",		DSHPprop_gtextFStrikethrough ),

			    /********************************************/
			    /*  Various numeric properties.		*/
			    /********************************************/

    SHAPE_NUMBER( "pibFlags",		DSHPprop_pibFlags ),
    SHAPE_NUMBER( "pictureGray",	DSHPprop_pictureGray ),
    SHAPE_NUMBER( "pictureBiLevel",	DSHPprop_pictureBiLevel ),
    SHAPE_NUMBER( "c3DEdgeThickness",	DSHPprop_c3DEdgeThickness ),
    SHAPE_NUMBER( "c3DExtrudeForward",	DSHPprop_c3DExtrudeForward ),
    SHAPE_NUMBER( "c3DExtrudeBackward",	DSHPprop_c3DExtrudeBackward ),
    SHAPE_NUMBER( "f3D",		DSHPprop_f3D ),
    SHAPE_NUMBER( "fc3DMetallic",	DSHPprop_fc3DMetallic ),
    SHAPE_NUMBER( "fc3DUseExtrusionColor", DSHPprop_fc3DUseExtrusionColor ),
    SHAPE_NUMBER( "fc3DLightFace",	DSHPprop_fc3DLightFace ),
    SHAPE_NUMBER( "c3DRotationAxisX",	DSHPprop_c3DRotationAxisX ),
    SHAPE_NUMBER( "c3DRotationAxisY",	DSHPprop_c3DRotationAxisY ),
    SHAPE_NUMBER( "c3DRotationAxisZ",	DSHPprop_c3DRotationAxisZ ),
    SHAPE_FLAG( "fC3DRotationCenterAut", DSHPprop_fC3DRotationCenterAut ),
    SHAPE_NUMBER( "c3DRotationCenterZ",	DSHPprop_c3DRotationCenterZ ),
    SHAPE_NUMBER( "c3DXViewpoint",	DSHPprop_c3DXViewpoint ),
    SHAPE_NUMBER( "c3DYViewpoint",	DSHPprop_c3DYViewpoint ),
    SHAPE_NUMBER( "c3DZViewpoint",	DSHPprop_c3DZViewpoint ),
    SHAPE_NUMBER( "c3DKeyX",		DSHPprop_c3DKeyX ),
    SHAPE_NUMBER( "c3DKeyY",		DSHPprop_c3DKeyY ),
    SHAPE_NUMBER( "c3DKeyZ",		DSHPprop_c3DKeyZ ),
    SHAPE_NUMBER( "c3DFillX",		DSHPprop_c3DFillX ),
    SHAPE_NUMBER( "c3DFillY",		DSHPprop_c3DFillY ),
    SHAPE_NUMBER( "c3DFillZ",		DSHPprop_c3DFillZ ),
    SHAPE_NUMBER( "fc3DParallel",		DSHPprop_fc3DParallel ),
    SHAPE_NUMBER( "fc3DKeyHarsh",		DSHPprop_fc3DKeyHarsh ),
    SHAPE_NUMBER( "fc3DFillHarsh",		DSHPprop_fc3DFillHarsh ),
    SHAPE_NUMBER( "fc3DConstrainRotation",	DSHPprop_fc3DConstrainRotation ),
    SHAPE_NUMBER( "fc3DRotationCenterAuto",	DSHPprop_fc3DRotationCenterAuto ),
    SHAPE_NUMBER( "spcot",			DSHPprop_spcot ),
    SHAPE_NUMBER( "dxyCalloutGap",		DSHPprop_dxyCalloutGap ),
    SHAPE_NUMBER( "spcoa",			DSHPprop_spcoa ),
    SHAPE_NUMBER( "spcod",			DSHPprop_spcod ),
    SHAPE_NUMBER( "dxyCalloutDropSpecified", DSHPprop_dxyCalloutDropSpecified ),
    SHAPE_NUMBER( "dxyCalloutLengthSpecified", DSHPprop_dxyCalloutLengthSpecified ),
    SHAPE_FLAG( "fCallout",			DSHPprop_fCallout ),
    SHAPE_FLAG( "fCalloutAccentBar",		DSHPprop_fCalloutAccentBar ),
    SHAPE_FLAG( "fCalloutTextBorder",		DSHPprop_fCalloutTextBorder ),
    SHAPE_FLAG( "fCalloutDropAuto",		DSHPprop_fCalloutDropAuto ),
    SHAPE_FLAG( "fCalloutLengthSpecified",	DSHPprop_fCalloutLengthSpecified ),
    SHAPE_NUMBER( "pictureId", 			DSHPprop_pictureId ),
    SHAPE_NUMBER( "pictureActive", 		DSHPprop_pictureActive ),
    SHAPE_NUMBER( "pictureContrast", 		DSHPprop_pictureContrast ),
    SHAPE_NUMBER( "pictureBrightness", 		DSHPprop_pictureBrightness ),
    SHAPE_NUMBER( "pictureDblCrMod", 		DSHPprop_pictureDblCrMod ),
    SHAPE_NUMBER( "pictureFillCrMod", 		DSHPprop_pictureFillCrMod ),
    SHAPE_NUMBER( "pictureLineCrMod", 		DSHPprop_pictureLineCrMod ),
    SHAPE_FLAG( "fCalloutMinusX", 		DSHPprop_fCalloutMinusX ),
    SHAPE_FLAG( "fCalloutMinusY", 		DSHPprop_fCalloutMinusY ),
    SHAPE_NUMBER( "shapePath", 			DSHPprop_shapePath ),
    SHAPE_NUMBER( "lineOpacity", 		DSHPprop_lineOpacity ),
    SHAPE_NUMBER( "lineblipflags", 		DSHPprop_lineblipflags ),
    SHAPE_NUMBER( "cropFromLeft",	 	DSHPprop_cropFromLeft ),
    SHAPE_NUMBER( "cropFromRight",	 	DSHPprop_cropFromRight ),
    SHAPE_NUMBER( "cropFromTop",	 	DSHPprop_cropFromTop ),
    SHAPE_NUMBER( "cropFromBottom",	 	DSHPprop_cropFromBottom ),
    SHAPE_NUMBER( "bWMode",			DSHPprop_bWMode ),
    SHAPE_NUMBER( "f3DOK",			DSHPprop_f3DOK ),
    SHAPE_FLAG( "fPreferRelativeResize",	DSHPprop_fPreferRelativeResize ),
    SHAPE_FLAG( "fRecolorFillAsPicture",	DSHPprop_fRecolorFillAsPicture ),
    SHAPE_NUMBER( "dgmt",			DSHPprop_dgmt ),
    SHAPE_NUMBER( "ccol",			DSHPprop_ccol ),
    SHAPE_NUMBER( "dzColMargin",		DSHPprop_dzColMargin ),

    SHAPE_FLAG( "fScriptAnchor",		DSHPprop_fScriptAnchor ),
    SHAPE_FLAG( "fFakeMaster",			DSHPprop_fFakeMaster ),
    SHAPE_FLAG( "fCameFromImgDummy",		DSHPprop_fCameFromImgDummy ),
    SHAPE_NUMBER( "c3DAmbientIntensity",	DSHPprop_c3DAmbientIntensity ),
    SHAPE_NUMBER( "c3DDiffuseAmt",		DSHPprop_c3DDiffuseAmt ),
    SHAPE_NUMBER( "c3DExtrudePlane",		DSHPprop_c3DExtrudePlane ),
    SHAPE_NUMBER( "c3DFillIntensity",		DSHPprop_c3DFillIntensity ),
    SHAPE_NUMBER( "c3DKeyIntensity",		DSHPprop_c3DKeyIntensity ),
    SHAPE_NUMBER( "c3DOriginX",			DSHPprop_c3DOriginX ),
    SHAPE_NUMBER( "c3DOriginY",			DSHPprop_c3DOriginY ),
    SHAPE_NUMBER( "c3DRenderMode",		DSHPprop_c3DRenderMode ),
    SHAPE_NUMBER( "c3DRotationAngle",		DSHPprop_c3DRotationAngle ),
    SHAPE_NUMBER( "c3DRotationCenterX",		DSHPprop_c3DRotationCenterX ),
    SHAPE_NUMBER( "c3DRotationCenterY",		DSHPprop_c3DRotationCenterY ),
    SHAPE_NUMBER( "c3DShininess",		DSHPprop_c3DShininess ),
    SHAPE_NUMBER( "c3DSkewAmount",		DSHPprop_c3DSkewAmount ),
    SHAPE_NUMBER( "c3DSkewAngle",		DSHPprop_c3DSkewAngle ),
    SHAPE_NUMBER( "c3DSpecularAmt",		DSHPprop_c3DSpecularAmt ),
    SHAPE_NUMBER( "c3DTolerance",		DSHPprop_c3DTolerance ),
    SHAPE_NUMBER( "c3DXRotationAngle",		DSHPprop_c3DXRotationAngle ),
    SHAPE_NUMBER( "c3DYRotationAngle",		DSHPprop_c3DYRotationAngle ),
    SHAPE_NUMBER( "c3DExtrusionColorExt",	DSHPprop_c3DExtrusionColorExt ),
    SHAPE_NUMBER( "c3DExtrusionColorExtCMY",	DSHPprop_c3DExtrusionColorExtCMY ),
    SHAPE_NUMBER( "c3DExtrusionColorExtK",	DSHPprop_c3DExtrusionColorExtK ),
    SHAPE_NUMBER( "c3DExtrusionColorExtMod",	DSHPprop_c3DExtrusionColorExtMod ),
    SHAPE_NUMBER( "fillColorExt",		DSHPprop_fillColorExt ),
    SHAPE_NUMBER( "fillColorExtCMY",		DSHPprop_fillColorExtCMY ),
    SHAPE_NUMBER( "fillColorExtK",		DSHPprop_fillColorExtK ),
    SHAPE_NUMBER( "fillColorExtMod",		DSHPprop_fillColorExtMod ),
    SHAPE_NUMBER( "lineBackColorExt",		DSHPprop_lineBackColorExt ),
    SHAPE_NUMBER( "lineBackColorExtCMY",	DSHPprop_lineBackColorExtCMY ),
    SHAPE_NUMBER( "lineBackColorExtK",		DSHPprop_lineBackColorExtK ),
    SHAPE_NUMBER( "lineBackColorExtMod",	DSHPprop_lineBackColorExtMod ),
    SHAPE_NUMBER( "lineBottomBackColor",	DSHPprop_lineBottomBackColor ),
    SHAPE_NUMBER( "lineBottomBackColorExt",	DSHPprop_lineBottomBackColorExt ),
    SHAPE_NUMBER( "lineBottomBackColorExtCMY",	DSHPprop_lineBottomBackColorExtCMY ),
    SHAPE_NUMBER( "lineBottomBackColorExtK",	DSHPprop_lineBottomBackColorExtK ),
    SHAPE_NUMBER( "lineBottomBackColorExtMod",	DSHPprop_lineBottomBackColorExtMod ),
    SHAPE_NUMBER( "lineBottomColor",		DSHPprop_lineBottomColor ),
    SHAPE_NUMBER( "lineBottomColorExt",		DSHPprop_lineBottomColorExt ),
    SHAPE_NUMBER( "lineBottomColorExtCMY",	DSHPprop_lineBottomColorExtCMY ),
    SHAPE_NUMBER( "lineBottomColorExtK",	DSHPprop_lineBottomColorExtK ),
    SHAPE_NUMBER( "lineBottomColorExtMod",	DSHPprop_lineBottomColorExtMod ),
    SHAPE_NUMBER( "lineBottomCrMod",		DSHPprop_lineBottomCrMod ),
    SHAPE_NUMBER( "lineBottomDashing",		DSHPprop_lineBottomDashing ),
    SHAPE_NUMBER( "lineBottomEndArrowLength",	DSHPprop_lineBottomEndArrowLength ),
    SHAPE_NUMBER( "lineBottomEndArrowWidth",	DSHPprop_lineBottomEndArrowWidth ),
    SHAPE_NUMBER( "lineBottomEndArrowhead",	DSHPprop_lineBottomEndArrowhead ),
    SHAPE_NUMBER( "lineBottomEndCapStyle",	DSHPprop_lineBottomEndCapStyle ),
    SHAPE_NUMBER( "lineBottomFillBlipFlags",	DSHPprop_lineBottomFillBlipFlags ),
    SHAPE_NUMBER( "lineBottomFillDztype",	DSHPprop_lineBottomFillDztype ),
    SHAPE_NUMBER( "lineBottomFillHeight",	DSHPprop_lineBottomFillHeight ),
    SHAPE_NUMBER( "lineBottomFillWidth",	DSHPprop_lineBottomFillWidth ),
    SHAPE_NUMBER( "lineBottomMiterLimit",	DSHPprop_lineBottomMiterLimit ),
    SHAPE_NUMBER( "lineBottomOpacity",		DSHPprop_lineBottomOpacity ),
    SHAPE_NUMBER( "lineBottomStartArrowLength",	DSHPprop_lineBottomStartArrowLength ),
    SHAPE_NUMBER( "lineBottomStartArrowWidth",	DSHPprop_lineBottomStartArrowWidth ),
    SHAPE_NUMBER( "lineBottomStartArrowhead",	DSHPprop_lineBottomStartArrowhead ),
    SHAPE_NUMBER( "lineBottomStyle",		DSHPprop_lineBottomStyle ),
    SHAPE_NUMBER( "lineBottomType",		DSHPprop_lineBottomType ),
    SHAPE_NUMBER( "lineBottomWidth",		DSHPprop_lineBottomWidth ),
    SHAPE_NUMBER( "lineColorExt",		DSHPprop_lineColorExt ),
    SHAPE_NUMBER( "lineColorExtCMY",		DSHPprop_lineColorExtCMY ),
    SHAPE_NUMBER( "lineColorExtK",		DSHPprop_lineColorExtK ),
    SHAPE_NUMBER( "lineColorExtMod",		DSHPprop_lineColorExtMod ),
    SHAPE_NUMBER( "lineColumnBackColor",	DSHPprop_lineColumnBackColor ),
    SHAPE_NUMBER( "lineColumnBackColorExt",	DSHPprop_lineColumnBackColorExt ),
    SHAPE_NUMBER( "lineColumnBackColorExtCMY",	DSHPprop_lineColumnBackColorExtCMY ),
    SHAPE_NUMBER( "lineColumnBackColorExtK",	DSHPprop_lineColumnBackColorExtK ),
    SHAPE_NUMBER( "lineColumnBackColorExtMod",	DSHPprop_lineColumnBackColorExtMod ),
    SHAPE_NUMBER( "lineColumnColor",		DSHPprop_lineColumnColor ),
    SHAPE_NUMBER( "lineColumnColorExt",		DSHPprop_lineColumnColorExt ),
    SHAPE_NUMBER( "lineColumnColorExtCMY",	DSHPprop_lineColumnColorExtCMY ),
    SHAPE_NUMBER( "lineColumnColorExtK",	DSHPprop_lineColumnColorExtK ),
    SHAPE_NUMBER( "lineColumnColorExtMod",	DSHPprop_lineColumnColorExtMod ),
    SHAPE_NUMBER( "lineColumnCrMod",		DSHPprop_lineColumnCrMod ),
    SHAPE_NUMBER( "lineColumnDashing",		DSHPprop_lineColumnDashing ),
    SHAPE_NUMBER( "lineColumnEndArrowLength",	DSHPprop_lineColumnEndArrowLength ),
    SHAPE_NUMBER( "lineColumnEndArrowWidth",	DSHPprop_lineColumnEndArrowWidth ),
    SHAPE_NUMBER( "lineColumnEndArrowhead",	DSHPprop_lineColumnEndArrowhead ),
    SHAPE_NUMBER( "lineColumnEndCapStyle",	DSHPprop_lineColumnEndCapStyle ),
    SHAPE_NUMBER( "lineColumnFillBlipFlags",	DSHPprop_lineColumnFillBlipFlags ),
    SHAPE_NUMBER( "lineColumnFillDztype",	DSHPprop_lineColumnFillDztype ),
    SHAPE_NUMBER( "lineColumnFillHeight",	DSHPprop_lineColumnFillHeight ),
    SHAPE_NUMBER( "lineColumnFillWidth",	DSHPprop_lineColumnFillWidth ),
    SHAPE_NUMBER( "lineColumnMiterLimit",	DSHPprop_lineColumnMiterLimit ),
    SHAPE_NUMBER( "lineColumnOpacity",		DSHPprop_lineColumnOpacity ),
    SHAPE_NUMBER( "lineColumnStartArrowLength",	DSHPprop_lineColumnStartArrowLength ),
    SHAPE_NUMBER( "lineColumnStartArrowWidth",	DSHPprop_lineColumnStartArrowWidth ),
    SHAPE_NUMBER( "lineColumnStartArrowhead",	DSHPprop_lineColumnStartArrowhead ),
    SHAPE_NUMBER( "lineColumnStyle",		DSHPprop_lineColumnStyle ),
    SHAPE_NUMBER( "lineColumnType",		DSHPprop_lineColumnType ),
    SHAPE_NUMBER( "lineColumnWidth",		DSHPprop_lineColumnWidth ),
    SHAPE_NUMBER( "lineLeftBackColor",		DSHPprop_lineLeftBackColor ),
    SHAPE_NUMBER( "lineLeftBackColorExt",	DSHPprop_lineLeftBackColorExt ),
    SHAPE_NUMBER( "lineLeftBackColorExtCMY",	DSHPprop_lineLeftBackColorExtCMY ),
    SHAPE_NUMBER( "lineLeftBackColorExtK",	DSHPprop_lineLeftBackColorExtK ),
    SHAPE_NUMBER( "lineLeftBackColorExtMod",	DSHPprop_lineLeftBackColorExtMod ),
    SHAPE_NUMBER( "lineLeftColor",		DSHPprop_lineLeftColor ),
    SHAPE_NUMBER( "lineLeftColorExt",		DSHPprop_lineLeftColorExt ),
    SHAPE_NUMBER( "lineLeftColorExtCMY",	DSHPprop_lineLeftColorExtCMY ),
    SHAPE_NUMBER( "lineLeftColorExtK",		DSHPprop_lineLeftColorExtK ),
    SHAPE_NUMBER( "lineLeftColorExtMod",	DSHPprop_lineLeftColorExtMod ),
    SHAPE_NUMBER( "lineLeftCrMod",		DSHPprop_lineLeftCrMod ),
    SHAPE_NUMBER( "lineLeftDashing",		DSHPprop_lineLeftDashing ),
    SHAPE_NUMBER( "lineLeftEndArrowLength",	DSHPprop_lineLeftEndArrowLength ),
    SHAPE_NUMBER( "lineLeftEndArrowWidth",	DSHPprop_lineLeftEndArrowWidth ),
    SHAPE_NUMBER( "lineLeftEndArrowhead",	DSHPprop_lineLeftEndArrowhead ),
    SHAPE_NUMBER( "lineLeftEndCapStyle",	DSHPprop_lineLeftEndCapStyle ),
    SHAPE_NUMBER( "lineLeftFillBlipFlags",	DSHPprop_lineLeftFillBlipFlags ),
    SHAPE_NUMBER( "lineLeftFillDztype",		DSHPprop_lineLeftFillDztype ),
    SHAPE_NUMBER( "lineLeftFillHeight",		DSHPprop_lineLeftFillHeight ),
    SHAPE_NUMBER( "lineLeftFillWidth",		DSHPprop_lineLeftFillWidth ),
    SHAPE_NUMBER( "lineLeftMiterLimit",		DSHPprop_lineLeftMiterLimit ),
    SHAPE_NUMBER( "lineLeftOpacity",		DSHPprop_lineLeftOpacity ),
    SHAPE_NUMBER( "lineLeftStartArrowLength",	DSHPprop_lineLeftStartArrowLength ),
    SHAPE_NUMBER( "lineLeftStartArrowWidth",	DSHPprop_lineLeftStartArrowWidth ),
    SHAPE_NUMBER( "lineLeftStartArrowhead",	DSHPprop_lineLeftStartArrowhead ),
    SHAPE_NUMBER( "lineLeftStyle",		DSHPprop_lineLeftStyle ),
    SHAPE_NUMBER( "lineLeftType",		DSHPprop_lineLeftType ),
    SHAPE_NUMBER( "lineLeftWidth",		DSHPprop_lineLeftWidth ),
    SHAPE_NUMBER( "lineRightBackColor",		DSHPprop_lineRightBackColor ),
    SHAPE_NUMBER( "lineRightBackColorExt",	DSHPprop_lineRightBackColorExt ),
    SHAPE_NUMBER( "lineRightBackColorExtCMY",	DSHPprop_lineRightBackColorExtCMY ),
    SHAPE_NUMBER( "lineRightBackColorExtK",	DSHPprop_lineRightBackColorExtK ),
    SHAPE_NUMBER( "lineRightBackColorExtMod",	DSHPprop_lineRightBackColorExtMod ),
    SHAPE_NUMBER( "lineRightColor",		DSHPprop_lineRightColor ),
    SHAPE_NUMBER( "lineRightColorExt",		DSHPprop_lineRightColorExt ),
    SHAPE_NUMBER( "lineRightColorExtCMY",	DSHPprop_lineRightColorExtCMY ),
    SHAPE_NUMBER( "lineRightColorExtK",		DSHPprop_lineRightColorExtK ),
    SHAPE_NUMBER( "lineRightColorExtMod",	DSHPprop_lineRightColorExtMod ),
    SHAPE_NUMBER( "lineRightCrMod",		DSHPprop_lineRightCrMod ),
    SHAPE_NUMBER( "lineRightDashing",		DSHPprop_lineRightDashing ),
    SHAPE_NUMBER( "lineRightEndArrowLength",	DSHPprop_lineRightEndArrowLength ),
    SHAPE_NUMBER( "lineRightEndArrowWidth",	DSHPprop_lineRightEndArrowWidth ),
    SHAPE_NUMBER( "lineRightEndArrowhead",	DSHPprop_lineRightEndArrowhead ),
    SHAPE_NUMBER( "lineRightEndCapStyle",	DSHPprop_lineRightEndCapStyle ),
    SHAPE_NUMBER( "lineRightFillBlipFlags",	DSHPprop_lineRightFillBlipFlags ),
    SHAPE_NUMBER( "lineRightFillDztype",	DSHPprop_lineRightFillDztype ),
    SHAPE_NUMBER( "lineRightFillHeight",	DSHPprop_lineRightFillHeight ),
    SHAPE_NUMBER( "lineRightFillWidth",		DSHPprop_lineRightFillWidth ),
    SHAPE_NUMBER( "lineRightMiterLimit",	DSHPprop_lineRightMiterLimit ),
    SHAPE_NUMBER( "lineRightOpacity",		DSHPprop_lineRightOpacity ),
    SHAPE_NUMBER( "lineRightStartArrowLength",	DSHPprop_lineRightStartArrowLength ),
    SHAPE_NUMBER( "lineRightStartArrowWidth",	DSHPprop_lineRightStartArrowWidth ),
    SHAPE_NUMBER( "lineRightStartArrowhead",	DSHPprop_lineRightStartArrowhead ),
    SHAPE_NUMBER( "lineRightStyle",		DSHPprop_lineRightStyle ),
    SHAPE_NUMBER( "lineRightType",		DSHPprop_lineRightType ),
    SHAPE_NUMBER( "lineRightWidth",		DSHPprop_lineRightWidth ),
    SHAPE_NUMBER( "lineTopBackColor",		DSHPprop_lineTopBackColor ),
    SHAPE_NUMBER( "lineTopBackColorExt",	DSHPprop_lineTopBackColorExt ),
    SHAPE_NUMBER( "lineTopBackColorExtCMY",	DSHPprop_lineTopBackColorExtCMY ),
    SHAPE_NUMBER( "lineTopBackColorExtK",	DSHPprop_lineTopBackColorExtK ),
    SHAPE_NUMBER( "lineTopBackColorExtMod",	DSHPprop_lineTopBackColorExtMod ),
    SHAPE_NUMBER( "lineTopColor",		DSHPprop_lineTopColor ),
    SHAPE_NUMBER( "lineTopColorExt",		DSHPprop_lineTopColorExt ),
    SHAPE_NUMBER( "lineTopColorExtCMY",		DSHPprop_lineTopColorExtCMY ),
    SHAPE_NUMBER( "lineTopColorExtK",		DSHPprop_lineTopColorExtK ),
    SHAPE_NUMBER( "lineTopColorExtMod",		DSHPprop_lineTopColorExtMod ),
    SHAPE_NUMBER( "lineTopCrMod",		DSHPprop_lineTopCrMod ),
    SHAPE_NUMBER( "lineTopDashing",		DSHPprop_lineTopDashing ),
    SHAPE_NUMBER( "lineTopEndArrowLength",	DSHPprop_lineTopEndArrowLength ),
    SHAPE_NUMBER( "lineTopEndArrowWidth",	DSHPprop_lineTopEndArrowWidth ),
    SHAPE_NUMBER( "lineTopEndArrowhead",	DSHPprop_lineTopEndArrowhead ),
    SHAPE_NUMBER( "lineTopEndCapStyle",		DSHPprop_lineTopEndCapStyle ),
    SHAPE_NUMBER( "lineTopFillBlipFlags",	DSHPprop_lineTopFillBlipFlags ),
    SHAPE_NUMBER( "lineTopFillDztype",		DSHPprop_lineTopFillDztype ),
    SHAPE_NUMBER( "lineTopFillHeight",		DSHPprop_lineTopFillHeight ),
    SHAPE_NUMBER( "lineTopFillWidth",		DSHPprop_lineTopFillWidth ),
    SHAPE_NUMBER( "lineTopMiterLimit",		DSHPprop_lineTopMiterLimit ),
    SHAPE_NUMBER( "lineTopOpacity",		DSHPprop_lineTopOpacity ),
    SHAPE_NUMBER( "lineTopStartArrowLength",	DSHPprop_lineTopStartArrowLength ),
    SHAPE_NUMBER( "lineTopStartArrowWidth",	DSHPprop_lineTopStartArrowWidth ),
    SHAPE_NUMBER( "lineTopStartArrowhead",	DSHPprop_lineTopStartArrowhead ),
    SHAPE_NUMBER( "lineTopStyle",		DSHPprop_lineTopStyle ),
    SHAPE_NUMBER( "lineTopType",		DSHPprop_lineTopType ),
    SHAPE_NUMBER( "lineTopWidth",		DSHPprop_lineTopWidth ),
    SHAPE_NUMBER( "shadowColorExt",		DSHPprop_shadowColorExt ),
    SHAPE_NUMBER( "shadowColorExtCMY",		DSHPprop_shadowColorExtCMY ),
    SHAPE_NUMBER( "shadowColorExtK",		DSHPprop_shadowColorExtK ),
    SHAPE_NUMBER( "shadowColorExtMod",		DSHPprop_shadowColorExtMod ),
    SHAPE_NUMBER( "shadowHighlightExt",		DSHPprop_shadowHighlightExt ),
    SHAPE_NUMBER( "shadowHighlightExtCMY",	DSHPprop_shadowHighlightExtCMY ),
    SHAPE_NUMBER( "shadowHighlightExtK",	DSHPprop_shadowHighlightExtK ),
    SHAPE_NUMBER( "shadowHighlightExtMod",	DSHPprop_shadowHighlightExtMod ),

    SHAPE_COLOR( "pictureTransparent",		DSHPprop_pictureTransparent ),
    SHAPE_COLOR( "c3DExtrusionColor",		DSHPprop_c3DExtrusionColor ),
    SHAPE_COLOR( "c3DCrMod",			DSHPprop_c3DCrMod ),

    SHAPE_FLAG( "fStandardHR",			DSHPprop_fStandardHR ),
    SHAPE_FLAG( "fNoshadeHR",			DSHPprop_fNoshadeHR ),
    SHAPE_FLAG( "fHorizRule",			DSHPprop_fHorizRule ),
    SHAPE_FLAG( "fUserDrawn",			DSHPprop_fUserDrawn ),

    SHAPE_NUMBER( "dhgt",			DSHPprop_dhgt ),
    SHAPE_NUMBER( "sizerelh",			DSHPprop_sizerelh ),
    SHAPE_NUMBER( "sizerelv",			DSHPprop_sizerelv ),
    SHAPE_NUMBER( "pctHoriz",			DSHPprop_pctHoriz ),

				/****************************************/
				/*  End.				*/
				/****************************************/
    { (char *)0, 0, 0 }
};

static const int docRtfShapePropertyWordCount=
		    sizeof(docRtfShapePropertyWords)/sizeof(RtfControlWord)- 1;

#   if ! defined(BUILD_HASH) && ! defined(BUILD_LOOKUP)

const RtfControlWord * docRtfFindShapePropertyWord(
					const char *	controlWord )
    {
    static int			checked= 1;

    if  ( ! checked )
	{
	if  ( docRtfCheckHash( docRtfShapePropertyWords,
				    docRtfShapePropertyWordCount,
				    docRtfShapePropIndex ) )
	    { checked= -1;	}
	else{ checked=  1;	}
	}

    if  ( checked > 0 )
	{
	return docRtfFindWordInTab(
		    docRtfShapePropertyWords, docRtfShapePropertyWordCount,
		    controlWord, docRtfShapePropIndex );
	}
    else{
	return docRtfFindWordInTab(
		    docRtfShapePropertyWords, docRtfShapePropertyWordCount,
		    controlWord, (DOC_RTF_HASH_WORD)0 );
	}
    }

const RtfControlWord * docRtfFindShapeProperty(
					int			scope,
					int			prop,
					int			value )
    {
    static int		checked= 1;

    if  ( ! checked )
	{
	if  ( docRtfCheckLookup( docRtfShapePropertyWords,
				docRtfShapePropertyWordCount,
				docRtfShapeLookup, docRtfShapeLookupCount ) )
	    { checked= -1;	}
	else{ checked=  1;	}
	}

    return docRtfLookupProperty( docRtfShapePropertyWords,
				docRtfShapeLookup, docRtfShapeLookupCount,
				scope, prop, value );
    }

#   endif

#   ifdef	BUILD_HASH

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeHash( docRtfShapePropertyWords,
						docRtfShapePropertyWordCount,
						"docRtfShapePropHash.c",
						"\"docRtfTagHash.h\"",
						"docRtfShapePropHash",
						"docRtfShapePropIndex" ) )
	    { LDEB(docRtfShapePropertyWordCount); return 1;	}

	return 0;
	}

#   endif

#   ifdef	BUILD_LOOKUP

    int main(	int	argc,
		char **	argcv )
	{
	if  ( docRtfMakeLookup( docRtfShapePropertyWords,
						docRtfShapePropertyWordCount,
						"docRtfShapePropLookup.c",
						"\"docRtfShapePropLookup.h\"",
						"docRtfShapeLookup",
						"docRtfShapeLookupCount" ) )
	    { LDEB(docRtfShapePropertyWordCount); return 1;	}

	return 0;
	}

#   endif

