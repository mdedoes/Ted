/************************************************************************/
/*									*/
/*  Declarations for shapes.						*/
/*									*/
/*  Distances are in EMU's.						*/
/*  12700 EMU/Point.							*/
/*  635 EMU/Twip.							*/
/*  914400 EMU/Inch.							*/
/*  360000 EMU/cm.							*/
/*									*/
/*  With the exception of 'shplid', the ShapeProperties that come from	*/
/*  regular rtf only apply for the top level 'shp' or shpgrp'.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_DRAWING_H
#   define	DOC_SHAPE_DRAWING_H

struct Point2DI;

#   include	<utilColor.h>
#   include	<utilMemoryBuffer.h>
#   include	<geoRectangle.h>

#   define	EMUtoTWIPS(e)	((e+317)/635)
#   define	TWIPStoEMU(t)	(635*(t))

/************************************************************************/
/*									*/
/*  A shape arrow description as it appears in the shape description.	*/
/*									*/
/************************************************************************/

typedef struct ShapeArrow
    {
    unsigned char	saArrowHead;
#				define	DSarrowNONE			0
#				define	DSarrowARROW			1
#				define	DSarrowSTEALTH_ARROW		2
#				define	DSarrowDIAMOND			3
#				define	DSarrowOVAL			4
#				define	DSarrowOPEN_ARROW		5
#				define	DSarrowCHEVRON_ARROW		7
#				define	DSarrow2CHEVRON_ARROW		8
    unsigned char	saArrowWidth;
#				define	DSarrowNARROW			0
#				define	DSarrowMEDIUM			1
#				define	DSarrowWIDE			2
    unsigned char	saArrowLength;
#				define	DSarrowSHORT			0
/*				define	DSarrowMEDIUM			1 */
#				define	DSarrowLONG			2
    } ShapeArrow;

typedef struct ShapeDrawing
    {
			/**
			 *  Shape type
			 */
    int			sdShapeType;

			/************************************************/
			/*  Position.					*/
			/************************************************/
    unsigned char	sdXPosition;
    unsigned char	sdXReference;
    unsigned char	sdYPosition;
    unsigned char	sdYReference;

    unsigned char	sd_fUseShapeAnchor;
    unsigned char	sd_fLayoutInCell;
    unsigned char	sd_fAllowOverlap;
    unsigned char	sd_fChangePage;

			/************************************************/
			/*  Object Type					*/
			/************************************************/
			/**
			 *  Rotation. Unit is 1/65536 degree
			 */
    int			sdRotation;

    int			sdWrapDistLeftEmu;
    int			sdWrapDistTopEmu;
    int			sdWrapDistRightEmu;
    int			sdWrapDistBottomEmu;

    long		sdMasterShape;
    long		sdNextShape;

    int			sdXLimo;
    int			sdYLimo;

    unsigned char	sd_fIsBullet;
    unsigned char	sd_fFlipH;
    unsigned char	sd_fFlipV;
    unsigned char	sd_fBehindDocument;
    unsigned char	sd_fIsButton;
    unsigned char	sd_fHidden;
    unsigned char	sd_fReallyHidden;
    unsigned char	sd_fArrowheadsOK;
    unsigned char	sd_fBackground;
    unsigned char	sd_fDeleteAttachedObject;
    unsigned char	sd_fEditedWrap;
    unsigned char	sd_fHitTestFill;
    unsigned char	sd_fHitTestLine;
    unsigned char	sd_fInitiator;
    unsigned char	sd_fNoFillHitTest;
    unsigned char	sd_fNoHitTestPicture;
    unsigned char	sd_fNoLineDrawDash;
    unsigned char	sd_fOleIcon;
    unsigned char	sd_fOnDblClickNotify;
    unsigned char	sd_fOneD;
    unsigned char	sd_fPreferRelativeSize;
    unsigned char	sd_fPrint;

    unsigned char	sd_fPseudoInline;

			/************************************************/
			/*  Text box.					*/
			/************************************************/
    int			sd_dxTextLeft;
    int			sd_dyTextTop;
    int			sd_dxTextRight;
    int			sd_dyTextBottom;

    int			sd_scaleText;
    int			sd_lTxid;

			/**
			 *  Text wrapping:
			 */
#			define	SHAPEtextWRAP_SQUARE		0
#			define	SHAPEtextWRAP_TIGHT		1
#			define	SHAPEtextWRAP_NONE		2
#			define	SHAPEtextWRAP_TOP_BOTTOM	3
#			define	SHAPEtextWRAP_THROUGH		4
    unsigned char	sd_WrapText;
    unsigned char	sd_anchorText;
    unsigned char	sd_txflTextFlow;
    unsigned char	sd_cdirFont;

    unsigned char	sd_fAutoTextMargin;
    unsigned char	sd_fRotateText;
    unsigned char	sd_fSelectText;

    unsigned char	sd_fFitShapeToText;
    unsigned char	sd_fFitTextToShape;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    unsigned char	sd_fLockRotation;
    unsigned char	sd_fLockAspectRatio;
    unsigned char	sd_fLockAgainstSelect;
    unsigned char	sd_fLockCropping;
    unsigned char	sd_fLockVerticies;
    unsigned char	sd_fLockText;
    unsigned char	sd_fLockAdjustHandles;
    unsigned char	sd_fLockAgainstGrouping;
    unsigned char	sd_fLockShapeType;

    unsigned char	sd_fLockPosition;

			/************************************************/
			/*  Grouped Shapes				*/
			/*						*/
			/*  Relative position of children.		*/
			/*						*/
			/*  Position of groups. Actually the positions	*/
			/*  are given by shpleft,shpright,		*/
			/*  shptop,shpbottom in a root shape.		*/
			/************************************************/
    DocumentRectangle	sdRelRect;

    int			sdRelRotation;
    long		sdRegroupID;

    DocumentRectangle	sdGroupRect;

    unsigned char	sd_fRelChangePage;
    unsigned char	sd_fRelFlipH;
    unsigned char	sd_fRelFlipV;

				/*
				 *  Strings
				 */
    MemoryBuffer	sd_wzName;
    MemoryBuffer	sd_wzDescription;
    MemoryBuffer	sd_gtextUNICODE;
    MemoryBuffer	sd_gtextFont;
    MemoryBuffer	sd_pibName;
    MemoryBuffer	sd_fillBlipName;


				/*
				 *  Geometry
				 */
    DocumentRectangle	sdGeoRect;
    long		sdAdjustValue;
    long		sdAdjust2Value;
    long		sdAdjust3Value;
    long		sdAdjust4Value;
    long		sdAdjust5Value;
    long		sdAdjust6Value;
    long		sdAdjust7Value;
    long		sdAdjust8Value;
    long		sdAdjust9Value;
    long		sdAdjust10Value;

    struct Point2DI *	sdVertices;
    int			sdVertexCount;
    struct Point2DI *	sdWrapPolygonVertices;
    int			sdWrapPolygonVertexCount;
    int *		sdSegmentInfos;
    int			sdSegmentInfoCount;

    /*
    DSHPprop_pFragments,
    DSHPprop_pGuides,
    DSHPprop_pInscribe,
    DSHPprop_pAdjustHandles,
    */

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    unsigned char	sdFillType;
#				define	DSfillSOLID			0
#				define	DSfillPATTERN			1
#				define	DSfillTEXTURE			2
#				define	DSfillCENTERED_PICTURE		3
#				define	DSfillSHADE_START_TO_END	4
#				define	DSfillSHADE_BOUNDS_TO_END	5
#				define	DSfillSHADE_OUTLINE_TO_END	6
#				define	DSfillSHADE_ANGLE		7
    unsigned char	sdFillblipflags;
    unsigned char	sdFillFocus;
    unsigned char	sdFillDztype;
    unsigned char	sdFillShadeType;

    int			sdFillOpacity;
    int			sdFillBackOpacity;

    int			sdFillWidthEmu;
    int			sdFillHeightEmu;
    int			sdFillAngle;

    int			sdFillToLeft;
    int			sdFillToTop;
    int			sdFillToRight;
    int			sdFillToBottom;

    int			sdFillOriginX;
    int			sdFillOriginY;
    int			sdFillShapeOriginX;
    int			sdFillShapeOriginY;

    int			sdFillRectLeftEmu;
    int			sdFillRectTopEmu;
    int			sdFillRectRightEmu;
    int			sdFillRectBottomEmu;

    long		sdFillShadePreset;

    RGB8Color		sdFillColor;
    RGB8Color		sdFillBackColor;
    RGB8Color		sdFillCrModColor;

    unsigned char	sd_fFilled;
    unsigned char	sd_fillShape;
    unsigned char	sd_fillUseRect;
    unsigned char	sd_fFillOK;
    unsigned char	sd_fFillShadeShapeOK;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    unsigned char	sdLineType;
#				define	DSlineSOLID			0
#				define	DSlinePATTERN			1
#				define	DSlineTEXTURE			2
#				define	DSlinePICTURE			3

    unsigned char	sdLineFillBlipFlags;

    int			sdLineFillWidthEmu;
    int			sdLineFillHeightEmu;
    int			sdLineWidthEmu;

    unsigned char	sdLineStyle;
    unsigned char	sdLineDashing;
#				define	DSdashSOLID			0
#				define	DSdashDASHED			1
#				define	DSdashDOT			2
#				define	DSdashDASHDOT			3
#				define	DSdashDASHDOTDOT		4
#				define	DSdashDOT_X			6
#				define	DSdashDASHED_X			7
#				define	DSdashDASHED_L			8
#				define	DSdashDASHDOT_X			9
#				define	DSdashDASHDOT_L			10
#				define	DSdashDASHDOTDOT_L		11
#				define	DSdashHOLLOW			12/*do*/
    unsigned char	sdLineDashStyle;
    ShapeArrow		sdLineHeadArrow;
    ShapeArrow		sdLineTailArrow;
    unsigned char	sdLineEndCapStyle;
    unsigned char	sdLineFillDztype;
    unsigned char	sdLineJoinStyle;

    int			sdLineMiterLimit;

    RGB8Color		sdLineColor;
    RGB8Color		sdLineBackColor;
    RGB8Color		sdLineCrModColor;

    unsigned char	sd_fLine;
    unsigned char	sd_fLineOK;

    unsigned char	sd_fLineUseShapeAnchor;
    unsigned char	sd_fColumnLineOK;
    unsigned char	sd_fColumnLine;
    unsigned char	sd_fLeftLine;
    unsigned char	sd_fTopLine;
    unsigned char	sd_fRightLine;
    unsigned char	sd_fBottomLine;

    unsigned char	sd_fColumnHitTestLine;
    unsigned char	sd_fLeftHitTestLine;
    unsigned char	sd_fTopHitTestLine;
    unsigned char	sd_fRightHitTestLine;
    unsigned char	sd_fBottomHitTestLine;

    unsigned char	sd_lineFillShape;
    unsigned char	sd_lineColumnFillShape;
    unsigned char	sd_lineLeftFillShape;
    unsigned char	sd_lineTopFillShape;
    unsigned char	sd_lineRightFillShape;
    unsigned char	sd_lineBottomFillShape;

    unsigned char	sd_fInsetPen;
    unsigned char	sd_fLeftInsetPen;
    unsigned char	sd_fTopInsetPen;
    unsigned char	sd_fRightInsetPen;
    unsigned char	sd_fBottomInsetPen;

    int			sd_lineOpacity;
    int			sd_lineblipflags;

    int			sd_lineBackColorExt;
    int			sd_lineBackColorExtCMY;
    int			sd_lineBackColorExtK;
    int			sd_lineBackColorExtMod;
    int			sd_lineBottomBackColor;
    int			sd_lineBottomBackColorExt;
    int			sd_lineBottomBackColorExtCMY;
    int			sd_lineBottomBackColorExtK;
    int			sd_lineBottomBackColorExtMod;
    int			sd_lineBottomColor;
    int			sd_lineBottomColorExt;
    int			sd_lineBottomColorExtCMY;
    int			sd_lineBottomColorExtK;
    int			sd_lineBottomColorExtMod;
    int			sd_lineBottomCrMod;
    int			sd_lineBottomDashing;
    int			sd_lineBottomEndArrowLength;
    int			sd_lineBottomEndArrowWidth;
    int			sd_lineBottomEndArrowhead;
    int			sd_lineBottomEndCapStyle;
    int			sd_lineBottomFillBlipFlags;
    int			sd_lineBottomFillDztype;
    int			sd_lineBottomFillHeight;
    int			sd_lineBottomFillWidth;
    int			sd_lineBottomMiterLimit;
    int			sd_lineBottomOpacity;
    int			sd_lineBottomStartArrowLength;
    int			sd_lineBottomStartArrowWidth;
    int			sd_lineBottomStartArrowhead;
    int			sd_lineBottomStyle;
    int			sd_lineBottomType;
    int			sd_lineBottomWidth;
    int			sd_lineColorExt;
    int			sd_lineColorExtCMY;
    int			sd_lineColorExtK;
    int			sd_lineColorExtMod;
    int			sd_lineColumnBackColor;
    int			sd_lineColumnBackColorExt;
    int			sd_lineColumnBackColorExtCMY;
    int			sd_lineColumnBackColorExtK;
    int			sd_lineColumnBackColorExtMod;
    int			sd_lineColumnColor;
    int			sd_lineColumnColorExt;
    int			sd_lineColumnColorExtCMY;
    int			sd_lineColumnColorExtK;
    int			sd_lineColumnColorExtMod;
    int			sd_lineColumnCrMod;
    int			sd_lineColumnDashing;
    int			sd_lineColumnEndArrowLength;
    int			sd_lineColumnEndArrowWidth;
    int			sd_lineColumnEndArrowhead;
    int			sd_lineColumnEndCapStyle;
    int			sd_lineColumnFillBlipFlags;
    int			sd_lineColumnFillDztype;
    int			sd_lineColumnFillHeight;
    int			sd_lineColumnFillWidth;
    int			sd_lineColumnMiterLimit;
    int			sd_lineColumnOpacity;
    int			sd_lineColumnStartArrowLength;
    int			sd_lineColumnStartArrowWidth;
    int			sd_lineColumnStartArrowhead;
    int			sd_lineColumnStyle;
    int			sd_lineColumnType;
    int			sd_lineColumnWidth;
    int			sd_lineLeftBackColor;
    int			sd_lineLeftBackColorExt;
    int			sd_lineLeftBackColorExtCMY;
    int			sd_lineLeftBackColorExtK;
    int			sd_lineLeftBackColorExtMod;
    int			sd_lineLeftColor;
    int			sd_lineLeftColorExt;
    int			sd_lineLeftColorExtCMY;
    int			sd_lineLeftColorExtK;
    int			sd_lineLeftColorExtMod;
    int			sd_lineLeftCrMod;
    int			sd_lineLeftDashing;
    int			sd_lineLeftEndArrowLength;
    int			sd_lineLeftEndArrowWidth;
    int			sd_lineLeftEndArrowhead;
    int			sd_lineLeftEndCapStyle;
    int			sd_lineLeftFillBlipFlags;
    int			sd_lineLeftFillDztype;
    int			sd_lineLeftFillHeight;
    int			sd_lineLeftFillWidth;
    int			sd_lineLeftMiterLimit;
    int			sd_lineLeftOpacity;
    int			sd_lineLeftStartArrowLength;
    int			sd_lineLeftStartArrowWidth;
    int			sd_lineLeftStartArrowhead;
    int			sd_lineLeftStyle;
    int			sd_lineLeftType;
    int			sd_lineLeftWidth;
    int			sd_lineRightBackColor;
    int			sd_lineRightBackColorExt;
    int			sd_lineRightBackColorExtCMY;
    int			sd_lineRightBackColorExtK;
    int			sd_lineRightBackColorExtMod;
    int			sd_lineRightColor;
    int			sd_lineRightColorExt;
    int			sd_lineRightColorExtCMY;
    int			sd_lineRightColorExtK;
    int			sd_lineRightColorExtMod;
    int			sd_lineRightCrMod;
    int			sd_lineRightDashing;
    int			sd_lineRightEndArrowLength;
    int			sd_lineRightEndArrowWidth;
    int			sd_lineRightEndArrowhead;
    int			sd_lineRightEndCapStyle;
    int			sd_lineRightFillBlipFlags;
    int			sd_lineRightFillDztype;
    int			sd_lineRightFillHeight;
    int			sd_lineRightFillWidth;
    int			sd_lineRightMiterLimit;
    int			sd_lineRightOpacity;
    int			sd_lineRightStartArrowLength;
    int			sd_lineRightStartArrowWidth;
    int			sd_lineRightStartArrowhead;
    int			sd_lineRightStyle;
    int			sd_lineRightType;
    int			sd_lineRightWidth;
    int			sd_lineTopBackColor;
    int			sd_lineTopBackColorExt;
    int			sd_lineTopBackColorExtCMY;
    int			sd_lineTopBackColorExtK;
    int			sd_lineTopBackColorExtMod;
    int			sd_lineTopColor;
    int			sd_lineTopColorExt;
    int			sd_lineTopColorExtCMY;
    int			sd_lineTopColorExtK;
    int			sd_lineTopColorExtMod;
    int			sd_lineTopCrMod;
    int			sd_lineTopDashing;
    int			sd_lineTopEndArrowLength;
    int			sd_lineTopEndArrowWidth;
    int			sd_lineTopEndArrowhead;
    int			sd_lineTopEndCapStyle;
    int			sd_lineTopFillBlipFlags;
    int			sd_lineTopFillDztype;
    int			sd_lineTopFillHeight;
    int			sd_lineTopFillWidth;
    int			sd_lineTopMiterLimit;
    int			sd_lineTopOpacity;
    int			sd_lineTopStartArrowLength;
    int			sd_lineTopStartArrowWidth;
    int			sd_lineTopStartArrowhead;
    int			sd_lineTopStyle;
    int			sd_lineTopType;
    int			sd_lineTopWidth;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    unsigned char	sdShadowType;
    int			sdShadowOpacity;
    int			sdShadowOffsetXEmu;
    int			sdShadowOffsetYEmu;
    int			sdShadowOffset2XEmu;
    int			sdShadowOffset2YEmu;
    int			sdShadowScaleXToX;
    int			sdShadowScaleYToX;
    int			sdShadowScaleXToY;
    int			sdShadowScaleYToY;
    int			sdShadowPerspectiveX;
    int			sdShadowPerspectiveY;
    int			sdShadowWeight;
    int			sdShadowOriginX;
    int			sdShadowOriginY;

    int			sd_shadowColorExt;
    int			sd_shadowColorExtCMY;
    int			sd_shadowColorExtK;
    int			sd_shadowColorExtMod;
    int			sd_shadowHighlightExt;
    int			sd_shadowHighlightExtCMY;
    int			sd_shadowHighlightExtK;
    int			sd_shadowHighlightExtMod;

    unsigned char	sd_fShadow;
    unsigned char	sd_fshadowObscured;
    unsigned char	sd_fshadowOK;

    RGB8Color		sdShadowColor;
    RGB8Color		sdShadowHighlightColor;
    RGB8Color		sdShadowCrModColor;
    RGB8Color		sdShadowc3DExtrusionColor;
    RGB8Color		sdShadowc3DCrModColor;
    RGB8Color		sdPictureTransparentColor;

			/************************************************/
			/*  Connectors.					*/
			/************************************************/
    unsigned char	sdConnectionSite;
    unsigned char	sdConnectionStyle;

    int			sd_gtextAlign;

    unsigned char	sd_fGtext;
    unsigned char	sd_gtextFVertical;
    unsigned char	sd_gtextFKern;
    unsigned char	sd_gtextFTight;
    unsigned char	sd_gtextFStretch;
    unsigned char	sd_gtextFShrinkFit;
    unsigned char	sd_gtextFBestFit;
    unsigned char	sd_gtextFNormalize;
    unsigned char	sd_gtextFDxMeasure;
    unsigned char	sd_gtextFBold;
    unsigned char	sd_gtextFItalic;
    unsigned char	sd_gtextFUnderline;
    unsigned char	sd_gtextFShadow;
    unsigned char	sd_gtextFSmallcaps;
    unsigned char	sd_gtextFStrikethrough;

				/****************************************/

    int			sd_shapePath;

    int			sd_pictureActive;
    int			sd_pictureId;
    int			sd_pictureContrast;
    int			sd_pictureBrightness;
    int			sd_pictureDblCrMod;
    int			sd_pictureFillCrMod;
    int			sd_pictureLineCrMod;
    int			sd_pibFlags;
    int			sd_dhgt;
    int			sd_sizerelh;
    int			sd_sizerelv;
    int			sd_pctHoriz;
    int			sd_pictureGray;
    int			sd_pictureBiLevel;

    unsigned char	sd_fPreferRelativeResize;
    unsigned char	sd_fScriptAnchor;
    unsigned char	sd_fFakeMaster;
    unsigned char	sd_fCameFromImgDummy;
    unsigned char	sd_fRecolorFillAsPicture;

    int			sd_cropFromLeft;
    int			sd_cropFromRight;
    int			sd_cropFromTop;
    int			sd_cropFromBottom;

    int			sd_c3DEdgeThickness;
    int			sd_c3DExtrudeForward;
    int			sd_c3DExtrudeBackward;
    int			sd_c3DRotationAxisX;
    int			sd_c3DRotationAxisY;
    int			sd_c3DRotationAxisZ;
    int			sd_c3DRotationCenterZ;
    int			sd_c3DXViewpoint;
    int			sd_c3DYViewpoint;
    int			sd_c3DZViewpoint;
    int			sd_c3DKeyX;
    int			sd_c3DKeyY;
    int			sd_c3DKeyZ;
    int			sd_c3DFillX;
    int			sd_c3DFillY;
    int			sd_c3DFillZ;
    int			sd_c3DAmbientIntensity;
    int			sd_c3DDiffuseAmt;
    int			sd_c3DExtrudePlane;
    int			sd_c3DFillIntensity;
    int			sd_c3DKeyIntensity;
    int			sd_c3DOriginX;
    int			sd_c3DOriginY;
    int			sd_c3DRenderMode;
    int			sd_c3DRotationAngle;
    int			sd_c3DRotationCenterX;
    int			sd_c3DRotationCenterY;
    int			sd_c3DShininess;
    int			sd_c3DSkewAmount;
    int			sd_c3DSkewAngle;
    int			sd_c3DSpecularAmt;
    int			sd_c3DTolerance;
    int			sd_c3DXRotationAngle;
    int			sd_c3DYRotationAngle;
    int			sd_c3DExtrusionColorExt;
    int			sd_c3DExtrusionColorExtCMY;
    int			sd_c3DExtrusionColorExtK;
    int			sd_c3DExtrusionColorExtMod;

    unsigned char	sd_f3D;
    unsigned char	sd_fC3DRotationCenterAut;
    unsigned char	sd_fc3DMetallic;
    unsigned char	sd_fc3DUseExtrusionColor;
    unsigned char	sd_fc3DLightFace;
    unsigned char	sd_fc3DParallel;
    unsigned char	sd_fc3DKeyHarsh;
    unsigned char	sd_fc3DFillHarsh;
    unsigned char	sd_fc3DConstrainRotation;
    unsigned char	sd_fc3DRotationCenterAuto;

    int			sd_spcot;
    int			sd_dxyCalloutGap;
    int			sd_spcoa;
    int			sd_spcod;
    int			sd_dxyCalloutDropSpecified;
    int			sd_dxyCalloutLengthSpecified;
    int			sd_bWMode;
    int			sd_dgmt;
    int			sd_ccol;
    int			sd_dzColMargin;
    int			sd_fillColorExt;
    int			sd_fillColorExtCMY;
    int			sd_fillColorExtK;
    int			sd_fillColorExtMod;

    unsigned char	sd_fStandardHR;
    unsigned char	sd_fNoshadeHR;
    unsigned char	sd_fHorizRule;
    unsigned char	sd_fUserDrawn;
    unsigned char	sd_fCallout;
    unsigned char	sd_fCalloutAccentBar;
    unsigned char	sd_fCalloutTextBorder;
    unsigned char	sd_fCalloutDropAuto;
    unsigned char	sd_fCalloutLengthSpecified;
    unsigned char	sd_fCalloutMinusX;
    unsigned char	sd_fCalloutMinusY;
    unsigned char	sd_f3DOK;
    } ShapeDrawing;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeArrow(		ShapeArrow *		sa );

extern void docInitShapeDrawing(	ShapeDrawing *		sd );

extern const char * docShapeTypeString(	int	shapeType );

extern void docInitShapeDrawingAllocated(	ShapeDrawing *	sd );

extern int docCopyShapeDrawing(	ShapeDrawing *			to,
				const ShapeDrawing *		from );

extern void docCleanShapeDrawing(	ShapeDrawing *		sd );

extern int docSetShapeDrawingProperty(	ShapeDrawing *		sd,
					int			prop,
					long			value );

extern long docGetShapeDrawingProperty(	const ShapeDrawing *	sd,
					int			prop );

extern int docSetShapeDrawingStringProperty(
					ShapeDrawing *		sd,
					int			prop,
					const char *		text,
					int			size );

#   endif	/*  DOC_SHAPE_DRAWING_H	*/
