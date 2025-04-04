/************************************************************************/
/*									*/
/*  Properties of shapes.						*/
/*									*/
/*  Properties that do not hold a numeric value come before the numeric	*/
/*  ones.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_PROP_H
#   define	DOC_SHAPE_PROP_H

typedef enum DrawingShapeProperty
    {
	    /**
	     *  String
	     */
    DSHPprop_wzName,
    DSHPprop_wzDescription,
    DSHPprop_gtextUNICODE,
    DSHPprop_gtextFont,
    DSHPprop_pibName,

	    /**
	     *  Picture
	     */
    DSHPprop_pib,

	    /**
	     *  Geometry: Paths
	     */
    DSHPprop_pVerticies,
    DSHPprop_pWrapPolygonVertices,
    DSHPprop_pSegmentInfo,
    DSHPprop_pFragments,
    DSHPprop_pGuides,
    DSHPprop_pInscribe,
    DSHPprop_pAdjustHandles,

	    /**
	     *  Image to fill the shape
	     */
    DSHPprop_fillBlip,
    DSHPprop_fillBlipName,
    DSHPprop_lineFillBlipName,

	    /**
	     *  Custom or preset color ramps for graduated fills on shapes.
	     *  [Array)
	     */
    DSHPprop_fillShadeColors,

# define DSHPprop__FIRST_NUMERIC (DSHPprop_fillShadeColors+1)

			    /********************************************/
			    /*   Number/Flag: Position.			*/
			    /********************************************/
    DSHPprop_posv,
    DSHPprop_posh,
    DSHPprop_posrelv,
    DSHPprop_posrelh,

    DSHPprop_fUseShapeAnchor,
    DSHPprop_fLayoutInCell,
    DSHPprop_fAllowOverlap,
    DSHPprop_fChangePage,

			    /********************************************/
			    /*  Number/Flag: Object Type		*/
			    /********************************************/
    DSHPprop_shapeType,
    DSHPprop_rotation,

    DSHPprop_dxWrapDistLeft,
    DSHPprop_dyWrapDistTop,
    DSHPprop_dxWrapDistRight,
    DSHPprop_dyWrapDistBottom,

    DSHPprop_hspMaster,
    DSHPprop_hspNext,

    DSHPprop_xLimo,
    DSHPprop_yLimo,

    DSHPprop_fIsBullet,
    DSHPprop_fFlipV,
    DSHPprop_fFlipH,
    DSHPprop_fBehindDocument,
    DSHPprop_fIsButton,
    DSHPprop_fHidden,
    DSHPprop_fReallyHidden,
    DSHPprop_fArrowheadsOK,
    DSHPprop_fBackground,
    DSHPprop_fDeleteAttachedObject,
    DSHPprop_fEditedWrap,
    DSHPprop_fHitTestFill,
    DSHPprop_fHitTestLine,
    DSHPprop_fInitiator,
    DSHPprop_fNoFillHitTest,
    DSHPprop_fNoHitTestPicture,
    DSHPprop_fNoLineDrawDash,
    DSHPprop_fOleIcon,
    DSHPprop_fOnDblClickNotify,
    DSHPprop_fOneD,
    DSHPprop_fPreferRelativeSize,
    DSHPprop_fPrint,

			    /**
			     *  The shape is pseudo-inline, meaning it behaves 
			     *  like an inline image as far as positioning 
			     *  goes, but has the features of shapes.
			     */
    DSHPprop_fPseudoInline,

			    /**
			     *  Text box left padding
			     */
    DSHPprop_dxTextLeft,
			    /**
			     *  Text box top padding
			     */
    DSHPprop_dyTextTop,
			    /**
			     *  Text box right padding
			     */
    DSHPprop_dxTextRight,
			    /**
			     *  Text box bottom padding
			     */
    DSHPprop_dyTextBottom,

    DSHPprop_scaleText,
    DSHPprop_lTxid,

    DSHPprop_WrapText,
    DSHPprop_anchorText,
    DSHPprop_txflTextFlow,
    DSHPprop_cdirFont,

    DSHPprop_fAutoTextMargin,
    DSHPprop_fRotateText,
    DSHPprop_fSelectText,
			    /**
			     *  Text box: Resize the shape to fit around 
			     *  its text.
			     */
    DSHPprop_fFitShapeToText,
    DSHPprop_fFitTextToShape,

			    /********************************************/
			    /*  Number/Flag: Lock.			*/
			    /********************************************/
    DSHPprop_fLockRotation,
    DSHPprop_fLockAspectRatio,
    DSHPprop_fLockAgainstSelect,
    DSHPprop_fLockCropping,
    DSHPprop_fLockVerticies,
    DSHPprop_fLockText,
    DSHPprop_fLockAdjustHandles,
    DSHPprop_fLockAgainstGrouping,
    DSHPprop_fLockShapeType,

    /* ? */
    DSHPprop_fLockPosition,

			    /********************************************/
			    /*  Geometry				*/
			    /********************************************/
    DSHPprop_geoLeft,
    DSHPprop_geoTop,
    DSHPprop_geoRight,
    DSHPprop_geoBottom,
    DSHPprop_adjustValue,
    DSHPprop_adjust2Value,
    DSHPprop_adjust3Value,
    DSHPprop_adjust4Value,
    DSHPprop_adjust5Value,
    DSHPprop_adjust6Value,
    DSHPprop_adjust7Value,
    DSHPprop_adjust8Value,
    DSHPprop_adjust9Value,
    DSHPprop_adjust10Value,
			    /********************************************/
			    /*  Number/Flag: Grouped			*/
			    /********************************************/
    DSHPprop_relLeft,
    DSHPprop_relTop,
    DSHPprop_relRight,
    DSHPprop_relBottom,

    DSHPprop_relRotation,
    DSHPprop_lidRegroup,

    DSHPprop_groupLeft,
    DSHPprop_groupTop,
    DSHPprop_groupRight,
    DSHPprop_groupBottom,

    DSHPprop_fRelFlipH,
    DSHPprop_fRelFlipV,
    DSHPprop_fRelChangePage,

			    /********************************************/
			    /*  Number/Flag: Fill.			*/
			    /********************************************/
    DSHPprop_fillType,
    DSHPprop_fillOpacity,
    DSHPprop_fillBackOpacity,

    DSHPprop_fillblipflags,

    DSHPprop_fillWidth,
    DSHPprop_fillHeight,
    DSHPprop_fillAngle,
    DSHPprop_fillFocus,

    DSHPprop_fillToLeft,
    DSHPprop_fillToTop,
    DSHPprop_fillToRight,
    DSHPprop_fillToBottom,

    DSHPprop_fillOriginX,
    DSHPprop_fillOriginY,
    DSHPprop_fillShapeOriginX,
    DSHPprop_fillShapeOriginY,

    DSHPprop_fillDztype,

    DSHPprop_fillRectLeft,
    DSHPprop_fillRectTop,
    DSHPprop_fillRectRight,
    DSHPprop_fillRectBottom,

    DSHPprop_fillShadePreset,
    DSHPprop_fillShadeType,

    DSHPprop_fFilled,
    DSHPprop_fillShape,
    DSHPprop_fillUseRect,
    DSHPprop_fFillOK,
    DSHPprop_fFillShadeShapeOK,

    DSHPprop_fillColor,
    DSHPprop_fillBackColor,
    DSHPprop_fillCrMod,

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    DSHPprop_lineType,

    DSHPprop_lineFillBlipFlags,

    DSHPprop_lineFillWidth,
    DSHPprop_lineFillHeight,
    DSHPprop_lineWidth,

    DSHPprop_lineStyle,
    DSHPprop_lineDashing,
    DSHPprop_lineDashStyle,
    DSHPprop_lineStartArrowhead,
    DSHPprop_lineEndArrowhead,
    DSHPprop_lineStartArrowWidth,
    DSHPprop_lineStartArrowLength,
    DSHPprop_lineEndArrowWidth,
    DSHPprop_lineEndArrowLength,
    DSHPprop_lineEndCapStyle,
    DSHPprop_lineFillDztype,
    DSHPprop_lineJoinStyle,

    DSHPprop_lineMiterLimit,

    DSHPprop_fLine,
    DSHPprop_fLineOK,

    DSHPprop_fLineUseShapeAnchor,
    DSHPprop_fColumnLineOK,
    DSHPprop_fColumnLine,
    DSHPprop_fLeftLine,
    DSHPprop_fTopLine,
    DSHPprop_fRightLine,
    DSHPprop_fBottomLine,

    DSHPprop_fColumnHitTestLine,
    DSHPprop_fLeftHitTestLine,
    DSHPprop_fTopHitTestLine,
    DSHPprop_fRightHitTestLine,
    DSHPprop_fBottomHitTestLine,

    DSHPprop_lineFillShape,
    DSHPprop_lineColumnFillShape,
    DSHPprop_lineLeftFillShape,
    DSHPprop_lineTopFillShape,
    DSHPprop_lineRightFillShape,
    DSHPprop_lineBottomFillShape,

    DSHPprop_fInsetPen,
    DSHPprop_fLeftInsetPen,
    DSHPprop_fTopInsetPen,
    DSHPprop_fRightInsetPen,
    DSHPprop_fBottomInsetPen,

    DSHPprop_lineColor,
    DSHPprop_lineBackColor,
    DSHPprop_lineCrMod,

    DSHPprop_lineOpacity,
    DSHPprop_lineblipflags,

			    /********************************************/
			    /*  Number/Flag: Shadow.			*/
			    /********************************************/
    DSHPprop_shadowType,
    DSHPprop_shadowOpacity,
    DSHPprop_shadowOffsetX,
    DSHPprop_shadowOffsetY,
    DSHPprop_shadowSecondOffsetX,
    DSHPprop_shadowSecondOffsetY,
    DSHPprop_shadowScaleXToX,
    DSHPprop_shadowScaleYToX,
    DSHPprop_shadowScaleXToY,
    DSHPprop_shadowScaleYToY,
    DSHPprop_shadowPerspectiveX,
    DSHPprop_shadowPerspectiveY,
    DSHPprop_shadowWeight,
    DSHPprop_shadowOriginX,
    DSHPprop_shadowOriginY,

    DSHPprop_shadowColorExt,
    DSHPprop_shadowColorExtCMY,
    DSHPprop_shadowColorExtK,
    DSHPprop_shadowColorExtMod,
    DSHPprop_shadowHighlightExt,
    DSHPprop_shadowHighlightExtCMY,
    DSHPprop_shadowHighlightExtK,
    DSHPprop_shadowHighlightExtMod,
    DSHPprop_fShadow,

    DSHPprop_fshadowObscured,
    DSHPprop_fShadowOK,

    DSHPprop_shadowColor,
    DSHPprop_shadowHighlight,
    DSHPprop_shadowCrMod,

			    /********************************************/
			    /*  Number/Flag: Connectors.		*/
			    /********************************************/
    DSHPprop_cxk,
    DSHPprop_cxstyle,

			    /****************************/
			    /*  Number/Flag		*/
			    /****************************/
    DSHPprop_gtextAlign,
    DSHPprop_fGtext,
    DSHPprop_gtextFVertical,
    DSHPprop_gtextFKern,
    DSHPprop_gtextFTight,
    DSHPprop_gtextFStretch,
    DSHPprop_gtextFShrinkFit,
    DSHPprop_gtextFBestFit,
    DSHPprop_gtextFNormalize,
    DSHPprop_gtextFDxMeasure,
    DSHPprop_gtextFBold,
    DSHPprop_gtextFItalic,
    DSHPprop_gtextFUnderline,
    DSHPprop_gtextFShadow,
    DSHPprop_gtextFSmallcaps,
    DSHPprop_gtextFStrikethrough,

    DSHPprop_pibFlags,
    DSHPprop_pictureGray,
    DSHPprop_pictureBiLevel,
    DSHPprop_c3DEdgeThickness,
    DSHPprop_c3DExtrudeForward,
    DSHPprop_c3DExtrudeBackward,
    DSHPprop_f3D,
    DSHPprop_fc3DMetallic,
    DSHPprop_fc3DUseExtrusionColor,
    DSHPprop_fc3DLightFace,
    DSHPprop_c3DRotationAxisX,
    DSHPprop_c3DRotationAxisY,
    DSHPprop_c3DRotationAxisZ,
    DSHPprop_fC3DRotationCenterAut,
    DSHPprop_c3DRotationCenterZ,
    DSHPprop_c3DXViewpoint,
    DSHPprop_c3DYViewpoint,
    DSHPprop_c3DZViewpoint,
    DSHPprop_c3DKeyX,
    DSHPprop_c3DKeyY,
    DSHPprop_c3DKeyZ,
    DSHPprop_c3DFillX,
    DSHPprop_c3DFillY,
    DSHPprop_c3DFillZ,
    DSHPprop_fc3DParallel,
    DSHPprop_fc3DKeyHarsh,
    DSHPprop_fc3DFillHarsh,
    DSHPprop_fc3DConstrainRotation,
    DSHPprop_fc3DRotationCenterAuto,
    DSHPprop_spcot,
    DSHPprop_dxyCalloutGap,
    DSHPprop_spcoa,
    DSHPprop_spcod,
    DSHPprop_dxyCalloutDropSpecified,
    DSHPprop_dxyCalloutLengthSpecified,
    DSHPprop_fCallout,
    DSHPprop_fCalloutAccentBar,
    DSHPprop_fCalloutTextBorder,
    DSHPprop_fCalloutDropAuto,
    DSHPprop_fCalloutLengthSpecified,
    DSHPprop_pictureId,
    DSHPprop_pictureActive,
    DSHPprop_pictureContrast,
    DSHPprop_pictureBrightness,
    DSHPprop_pictureDblCrMod,
    DSHPprop_pictureFillCrMod,
    DSHPprop_pictureLineCrMod,
    DSHPprop_fCalloutMinusX,
    DSHPprop_fCalloutMinusY,
    DSHPprop_shapePath,
    DSHPprop_cropFromLeft,
    DSHPprop_cropFromRight,
    DSHPprop_cropFromTop,
    DSHPprop_cropFromBottom,
    DSHPprop_bWMode,
    DSHPprop_f3DOK,
    DSHPprop_fPreferRelativeResize,
    DSHPprop_fRecolorFillAsPicture,
    DSHPprop_dgmt,
    DSHPprop_ccol,
    DSHPprop_dzColMargin,
    DSHPprop_fScriptAnchor,
    DSHPprop_fFakeMaster,
    DSHPprop_fCameFromImgDummy,
			    /****************************/
    DSHPprop_c3DAmbientIntensity,
    DSHPprop_c3DDiffuseAmt,
    DSHPprop_c3DExtrudePlane,
    DSHPprop_c3DFillIntensity,
    DSHPprop_c3DKeyIntensity,
    DSHPprop_c3DOriginX,
    DSHPprop_c3DOriginY,
    DSHPprop_c3DRenderMode,
    DSHPprop_c3DRotationAngle,
    DSHPprop_c3DRotationCenterX,
    DSHPprop_c3DRotationCenterY,
    DSHPprop_c3DShininess,
    DSHPprop_c3DSkewAmount,
    DSHPprop_c3DSkewAngle,
    DSHPprop_c3DSpecularAmt,
    DSHPprop_c3DTolerance,
    DSHPprop_c3DXRotationAngle,
    DSHPprop_c3DYRotationAngle,
    DSHPprop_c3DExtrusionColorExt,
    DSHPprop_c3DExtrusionColorExtCMY,
    DSHPprop_c3DExtrusionColorExtK,
    DSHPprop_c3DExtrusionColorExtMod,
    DSHPprop_fillColorExt,
    DSHPprop_fillColorExtCMY,
    DSHPprop_fillColorExtK,
    DSHPprop_fillColorExtMod,
    DSHPprop_lineBackColorExt,
    DSHPprop_lineBackColorExtCMY,
    DSHPprop_lineBackColorExtK,
    DSHPprop_lineBackColorExtMod,
    DSHPprop_lineBottomBackColor,
    DSHPprop_lineBottomBackColorExt,
    DSHPprop_lineBottomBackColorExtCMY,
    DSHPprop_lineBottomBackColorExtK,
    DSHPprop_lineBottomBackColorExtMod,
    DSHPprop_lineBottomColor,
    DSHPprop_lineBottomColorExt,
    DSHPprop_lineBottomColorExtCMY,
    DSHPprop_lineBottomColorExtK,
    DSHPprop_lineBottomColorExtMod,
    DSHPprop_lineBottomCrMod,
    DSHPprop_lineBottomDashing,
    DSHPprop_lineBottomEndArrowLength,
    DSHPprop_lineBottomEndArrowWidth,
    DSHPprop_lineBottomEndArrowhead,
    DSHPprop_lineBottomEndCapStyle,
    DSHPprop_lineBottomFillBlipFlags,
    DSHPprop_lineBottomFillDztype,
    DSHPprop_lineBottomFillHeight,
    DSHPprop_lineBottomFillWidth,
    DSHPprop_lineBottomMiterLimit,
    DSHPprop_lineBottomOpacity,
    DSHPprop_lineBottomStartArrowLength,
    DSHPprop_lineBottomStartArrowWidth,
    DSHPprop_lineBottomStartArrowhead,
    DSHPprop_lineBottomStyle,
    DSHPprop_lineBottomType,
    DSHPprop_lineBottomWidth,
    DSHPprop_lineColorExt,
    DSHPprop_lineColorExtCMY,
    DSHPprop_lineColorExtK,
    DSHPprop_lineColorExtMod,
    DSHPprop_lineColumnBackColor,
    DSHPprop_lineColumnBackColorExt,
    DSHPprop_lineColumnBackColorExtCMY,
    DSHPprop_lineColumnBackColorExtK,
    DSHPprop_lineColumnBackColorExtMod,
    DSHPprop_lineColumnColor,
    DSHPprop_lineColumnColorExt,
    DSHPprop_lineColumnColorExtCMY,
    DSHPprop_lineColumnColorExtK,
    DSHPprop_lineColumnColorExtMod,
    DSHPprop_lineColumnCrMod,
    DSHPprop_lineColumnDashing,
    DSHPprop_lineColumnEndArrowLength,
    DSHPprop_lineColumnEndArrowWidth,
    DSHPprop_lineColumnEndArrowhead,
    DSHPprop_lineColumnEndCapStyle,
    DSHPprop_lineColumnFillBlipFlags,
    DSHPprop_lineColumnFillDztype,
    DSHPprop_lineColumnFillHeight,
    DSHPprop_lineColumnFillWidth,
    DSHPprop_lineColumnMiterLimit,
    DSHPprop_lineColumnOpacity,
    DSHPprop_lineColumnStartArrowLength,
    DSHPprop_lineColumnStartArrowWidth,
    DSHPprop_lineColumnStartArrowhead,
    DSHPprop_lineColumnStyle,
    DSHPprop_lineColumnType,
    DSHPprop_lineColumnWidth,
    DSHPprop_lineLeftBackColor,
    DSHPprop_lineLeftBackColorExt,
    DSHPprop_lineLeftBackColorExtCMY,
    DSHPprop_lineLeftBackColorExtK,
    DSHPprop_lineLeftBackColorExtMod,
    DSHPprop_lineLeftColor,
    DSHPprop_lineLeftColorExt,
    DSHPprop_lineLeftColorExtCMY,
    DSHPprop_lineLeftColorExtK,
    DSHPprop_lineLeftColorExtMod,
    DSHPprop_lineLeftCrMod,
    DSHPprop_lineLeftDashing,
    DSHPprop_lineLeftEndArrowLength,
    DSHPprop_lineLeftEndArrowWidth,
    DSHPprop_lineLeftEndArrowhead,
    DSHPprop_lineLeftEndCapStyle,
    DSHPprop_lineLeftFillBlipFlags,
    DSHPprop_lineLeftFillDztype,
    DSHPprop_lineLeftFillHeight,
    DSHPprop_lineLeftFillWidth,
    DSHPprop_lineLeftMiterLimit,
    DSHPprop_lineLeftOpacity,
    DSHPprop_lineLeftStartArrowLength,
    DSHPprop_lineLeftStartArrowWidth,
    DSHPprop_lineLeftStartArrowhead,
    DSHPprop_lineLeftStyle,
    DSHPprop_lineLeftType,
    DSHPprop_lineLeftWidth,
    DSHPprop_lineRightBackColor,
    DSHPprop_lineRightBackColorExt,
    DSHPprop_lineRightBackColorExtCMY,
    DSHPprop_lineRightBackColorExtK,
    DSHPprop_lineRightBackColorExtMod,
    DSHPprop_lineRightColor,
    DSHPprop_lineRightColorExt,
    DSHPprop_lineRightColorExtCMY,
    DSHPprop_lineRightColorExtK,
    DSHPprop_lineRightColorExtMod,
    DSHPprop_lineRightCrMod,
    DSHPprop_lineRightDashing,
    DSHPprop_lineRightEndArrowLength,
    DSHPprop_lineRightEndArrowWidth,
    DSHPprop_lineRightEndArrowhead,
    DSHPprop_lineRightEndCapStyle,
    DSHPprop_lineRightFillBlipFlags,
    DSHPprop_lineRightFillDztype,
    DSHPprop_lineRightFillHeight,
    DSHPprop_lineRightFillWidth,
    DSHPprop_lineRightMiterLimit,
    DSHPprop_lineRightOpacity,
    DSHPprop_lineRightStartArrowLength,
    DSHPprop_lineRightStartArrowWidth,
    DSHPprop_lineRightStartArrowhead,
    DSHPprop_lineRightStyle,
    DSHPprop_lineRightType,
    DSHPprop_lineRightWidth,
    DSHPprop_lineTopBackColor,
    DSHPprop_lineTopBackColorExt,
    DSHPprop_lineTopBackColorExtCMY,
    DSHPprop_lineTopBackColorExtK,
    DSHPprop_lineTopBackColorExtMod,
    DSHPprop_lineTopColor,
    DSHPprop_lineTopColorExt,
    DSHPprop_lineTopColorExtCMY,
    DSHPprop_lineTopColorExtK,
    DSHPprop_lineTopColorExtMod,
    DSHPprop_lineTopCrMod,
    DSHPprop_lineTopDashing,
    DSHPprop_lineTopEndArrowLength,
    DSHPprop_lineTopEndArrowWidth,
    DSHPprop_lineTopEndArrowhead,
    DSHPprop_lineTopEndCapStyle,
    DSHPprop_lineTopFillBlipFlags,
    DSHPprop_lineTopFillDztype,
    DSHPprop_lineTopFillHeight,
    DSHPprop_lineTopFillWidth,
    DSHPprop_lineTopMiterLimit,
    DSHPprop_lineTopOpacity,
    DSHPprop_lineTopStartArrowLength,
    DSHPprop_lineTopStartArrowWidth,
    DSHPprop_lineTopStartArrowhead,
    DSHPprop_lineTopStyle,
    DSHPprop_lineTopType,
    DSHPprop_lineTopWidth,
			    /****************************/
			    /*  Color			*/
			    /****************************/
    DSHPprop_pictureTransparent,
    DSHPprop_c3DExtrusionColor,
    DSHPprop_c3DCrMod,
			    /****************************/
    DSHPprop_fStandardHR,
    DSHPprop_fNoshadeHR,
    DSHPprop_fHorizRule,
    DSHPprop_fUserDrawn,

    DSHPprop_dhgt,
    DSHPprop_sizerelh,
    DSHPprop_sizerelv,
    DSHPprop_pctHoriz,

    DSHPprop__COUNT
    } DrawingShapeProperty;

#   endif	/*  DOC_SHAPE_PROP_H	*/
