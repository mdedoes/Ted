/************************************************************************/
/*									*/
/*  Shapes.. Administration.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"docShapeProp.h"
#   include	"docShapeDrawing.h"
#   include	"docShapeType.h"
#   include	"docFrameProperties.h"
#   include	<geo2DInteger.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape arrow.					*/
/*									*/
/************************************************************************/

void docInitShapeArrow(	ShapeArrow *	sa )
    {
    sa->saArrowHead= 0;
    sa->saArrowWidth= 1;
    sa->saArrowLength= 1;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape.						*/
/*									*/
/************************************************************************/

void docCleanShapeDrawing(	ShapeDrawing *		sd )
    {
				/*
				 * Arrays
				 */
    if  ( sd->sdVertices )
	{ free( sd->sdVertices ); }
    if  ( sd->sdWrapPolygonVertices )
	{ free( sd->sdWrapPolygonVertices ); }
    if  ( sd->sdSegmentInfos )
	{ free( sd->sdSegmentInfos ); }

				/*
				 *  Strings
				 */
    utilCleanMemoryBuffer( &(sd->sd_wzName) );
    utilCleanMemoryBuffer( &(sd->sd_wzDescription) );
    utilCleanMemoryBuffer( &(sd->sd_gtextUNICODE) );
    utilCleanMemoryBuffer( &(sd->sd_gtextFont) );
    utilCleanMemoryBuffer( &(sd->sd_pibName) );
    utilCleanMemoryBuffer( &(sd->sd_fillBlipName) );

    return;
    }

void docInitShapeDrawingAllocated(	ShapeDrawing *	sd )
    {
    sd->sdVertexCount= 0;
    sd->sdVertices= (Point2DI *)0;

    sd->sdWrapPolygonVertexCount= 0;
    sd->sdWrapPolygonVertices= (Point2DI *)0;

    sd->sdSegmentInfoCount= 0;
    sd->sdSegmentInfos= (int *)0;

    return;
    }

void docInitShapeDrawing(	ShapeDrawing *	sd )
    {
    sd->sdXPosition= 0;
    sd->sdXReference= FXrefCOLUMN;
    sd->sdYPosition= 0;
    sd->sdYReference= FYrefPARA;

    sd->sd_fUseShapeAnchor= 1;
    sd->sd_fLayoutInCell= 0;
    sd->sd_fAllowOverlap= 1;
    sd->sd_fChangePage= 0;

			/************************************************/
			/*  Object Type.				*/
			/************************************************/
    sd->sdShapeType= SHPtyFREE;
    sd->sdRotation= 0;

    sd->sdWrapDistLeftEmu= 114305;
    sd->sdWrapDistTopEmu= 0;
    sd->sdWrapDistRightEmu= 114305;
    sd->sdWrapDistBottomEmu= 0;

    sd->sdMasterShape= 0;
    sd->sdNextShape= 0;

    sd->sdXLimo= 0;
    sd->sdYLimo= 0;

    sd->sd_fIsBullet= 0;
    sd->sd_fFlipH= 0;
    sd->sd_fFlipV= 0;
    sd->sd_fBehindDocument= 0;
    sd->sd_fIsButton= 0;
    sd->sd_fHidden= 0;
    sd->sd_fReallyHidden= 0;
    sd->sd_fArrowheadsOK= 0;
    sd->sd_fBackground= 0;
    sd->sd_fDeleteAttachedObject= 0;
    sd->sd_fEditedWrap= 0;
    sd->sd_fHitTestFill= 1;
    sd->sd_fHitTestLine= 1;
    sd->sd_fInitiator= 0;
    sd->sd_fNoFillHitTest= 0;
    sd->sd_fNoHitTestPicture= 0;
    sd->sd_fNoLineDrawDash= 0;
    sd->sd_fOleIcon= 0;
    sd->sd_fOnDblClickNotify= 0;
    sd->sd_fOneD= 0;
    sd->sd_fPreferRelativeSize= 0;
    sd->sd_fPrint= 1;

    sd->sd_fPseudoInline= 0;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    sd->sd_dxTextLeft= 91440;
    sd->sd_dyTextTop= 45720;
    sd->sd_dxTextRight= 91440;
    sd->sd_dyTextBottom= 45720;

    sd->sd_scaleText= 0;
    sd->sd_lTxid= 0;

    sd->sd_WrapText= SHAPEtextWRAP_SQUARE;
    sd->sd_anchorText= 0;
    sd->sd_txflTextFlow= 0;
    sd->sd_cdirFont= 0;

    sd->sd_fAutoTextMargin= 0;
    sd->sd_fRotateText= 0;
    sd->sd_fSelectText= 1;
    sd->sd_fFitShapeToText= 0;
    sd->sd_fFitTextToShape= 0;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    sd->sd_fLockRotation= 0;
    sd->sd_fLockAspectRatio= 0;
    sd->sd_fLockAgainstSelect= 0;
    sd->sd_fLockCropping= 0;
    sd->sd_fLockVerticies= 0;
    sd->sd_fLockText= 0;
    sd->sd_fLockAdjustHandles= 0;
    sd->sd_fLockAgainstGrouping= 0;
    sd->sd_fLockShapeType= 0;

    sd->sd_fLockPosition= 0;

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    sd->sdFillType= 0;
    sd->sdFillblipflags= 0;
    sd->sdFillFocus= 0;
    sd->sdFillDztype= 0;
    sd->sdFillShadeType= 0;

    sd->sdFillOpacity= 65536;
    sd->sdFillBackOpacity= 65536;

    sd->sdFillWidthEmu= 0;
    sd->sdFillHeightEmu= 0;
    sd->sdFillAngle= 0;

    sd->sdFillToLeft= 0;
    sd->sdFillToTop= 0;
    sd->sdFillToRight= 0;
    sd->sdFillToBottom= 0;

    sd->sdFillOriginX= 0;
    sd->sdFillOriginY= 0;
    sd->sdFillShapeOriginX= 0;
    sd->sdFillShapeOriginY= 0;

    sd->sdFillRectLeftEmu= 0;
    sd->sdFillRectTopEmu= 0;
    sd->sdFillRectRightEmu= 0;
    sd->sdFillRectBottomEmu= 0;

    sd->sdFillShadePreset= 0;

    utilInitRGB8Color( &(sd->sdFillColor) );
    utilInitRGB8Color( &(sd->sdFillBackColor) );
    utilInitRGB8Color( &(sd->sdFillCrModColor) );

    sd->sdFillColor.rgb8Red= 255;
    sd->sdFillColor.rgb8Green= 255;
    sd->sdFillColor.rgb8Blue= 255;

    sd->sdFillBackColor.rgb8Red= 255;
    sd->sdFillBackColor.rgb8Green= 255;
    sd->sdFillBackColor.rgb8Blue= 255;

    sd->sd_fFilled= 1;
    sd->sd_fillShape= 1;
    sd->sd_fillUseRect= 0;
    sd->sd_fFillOK= 1;
    sd->sd_fFillShadeShapeOK= 0;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    sd->sdLineType= 0;

    sd->sdLineFillBlipFlags= 0;

    sd->sdLineFillWidthEmu= 0;
    sd->sdLineFillHeightEmu= 0;
    sd->sdLineWidthEmu= 9525;

    sd->sdLineStyle= 0;
    sd->sdLineDashing= 0;
    sd->sdLineDashStyle= 0;
    docInitShapeArrow( &(sd->sdLineHeadArrow) );
    docInitShapeArrow( &(sd->sdLineTailArrow) );
    sd->sdLineEndCapStyle= 2;
    sd->sdLineFillDztype= 0;
    sd->sdLineJoinStyle= 2;

    sd->sdLineMiterLimit= 524288;

    utilInitRGB8Color( &(sd->sdLineColor) );
    utilInitRGB8Color( &(sd->sdLineBackColor) );
    utilInitRGB8Color( &(sd->sdLineCrModColor) );

    sd->sd_fLine= 1;
    sd->sd_fLineOK= 1;

    sd->sd_fLineUseShapeAnchor= 0;
    sd->sd_fColumnLineOK= 0;
    sd->sd_fColumnLine= 0;
    sd->sd_fLeftLine= 0;
    sd->sd_fTopLine= 0;
    sd->sd_fRightLine= 0;
    sd->sd_fBottomLine= 0;

    sd->sd_fColumnHitTestLine= 0;
    sd->sd_fLeftHitTestLine= 0;
    sd->sd_fTopHitTestLine= 0;
    sd->sd_fRightHitTestLine= 0;
    sd->sd_fBottomHitTestLine= 0;

    sd->sd_lineFillShape= 0;
    sd->sd_lineColumnFillShape= 0;
    sd->sd_lineLeftFillShape= 0;
    sd->sd_lineTopFillShape= 0;
    sd->sd_lineRightFillShape= 0;
    sd->sd_lineBottomFillShape= 0;

    sd->sd_fInsetPen= 0;
    sd->sd_fLeftInsetPen= 0;
    sd->sd_fTopInsetPen= 0;
    sd->sd_fRightInsetPen= 0;
    sd->sd_fBottomInsetPen= 0;

    sd->sd_lineOpacity= 0;
    sd->sd_lineblipflags= 0;
    sd->sd_lineBackColorExt= 0;
    sd->sd_lineBackColorExtCMY= 0;
    sd->sd_lineBackColorExtK= 0;
    sd->sd_lineBackColorExtMod= 0;
    sd->sd_lineBottomBackColor= 0;
    sd->sd_lineBottomBackColorExt= 0;
    sd->sd_lineBottomBackColorExtCMY= 0;
    sd->sd_lineBottomBackColorExtK= 0;
    sd->sd_lineBottomBackColorExtMod= 0;
    sd->sd_lineBottomColor= 0;
    sd->sd_lineBottomColorExt= 0;
    sd->sd_lineBottomColorExtCMY= 0;
    sd->sd_lineBottomColorExtK= 0;
    sd->sd_lineBottomColorExtMod= 0;
    sd->sd_lineBottomCrMod= 0;
    sd->sd_lineBottomDashing= 0;
    sd->sd_lineBottomEndArrowLength= 0;
    sd->sd_lineBottomEndArrowWidth= 0;
    sd->sd_lineBottomEndArrowhead= 0;
    sd->sd_lineBottomEndCapStyle= 0;
    sd->sd_lineBottomFillBlipFlags= 0;
    sd->sd_lineBottomFillDztype= 0;
    sd->sd_lineBottomFillHeight= 0;
    sd->sd_lineBottomFillWidth= 0;
    sd->sd_lineBottomMiterLimit= 0;
    sd->sd_lineBottomOpacity= 0;
    sd->sd_lineBottomStartArrowLength= 0;
    sd->sd_lineBottomStartArrowWidth= 0;
    sd->sd_lineBottomStartArrowhead= 0;
    sd->sd_lineBottomStyle= 0;
    sd->sd_lineBottomType= 0;
    sd->sd_lineBottomWidth= 0;
    sd->sd_lineColorExt= 0;
    sd->sd_lineColorExtCMY= 0;
    sd->sd_lineColorExtK= 0;
    sd->sd_lineColorExtMod= 0;
    sd->sd_lineColumnBackColor= 0;
    sd->sd_lineColumnBackColorExt= 0;
    sd->sd_lineColumnBackColorExtCMY= 0;
    sd->sd_lineColumnBackColorExtK= 0;
    sd->sd_lineColumnBackColorExtMod= 0;
    sd->sd_lineColumnColor= 0;
    sd->sd_lineColumnColorExt= 0;
    sd->sd_lineColumnColorExtCMY= 0;
    sd->sd_lineColumnColorExtK= 0;
    sd->sd_lineColumnColorExtMod= 0;
    sd->sd_lineColumnCrMod= 0;
    sd->sd_lineColumnDashing= 0;
    sd->sd_lineColumnEndArrowLength= 0;
    sd->sd_lineColumnEndArrowWidth= 0;
    sd->sd_lineColumnEndArrowhead= 0;
    sd->sd_lineColumnEndCapStyle= 0;
    sd->sd_lineColumnFillBlipFlags= 0;
    sd->sd_lineColumnFillDztype= 0;
    sd->sd_lineColumnFillHeight= 0;
    sd->sd_lineColumnFillWidth= 0;
    sd->sd_lineColumnMiterLimit= 0;
    sd->sd_lineColumnOpacity= 0;
    sd->sd_lineColumnStartArrowLength= 0;
    sd->sd_lineColumnStartArrowWidth= 0;
    sd->sd_lineColumnStartArrowhead= 0;
    sd->sd_lineColumnStyle= 0;
    sd->sd_lineColumnType= 0;
    sd->sd_lineColumnWidth= 0;
    sd->sd_lineLeftBackColor= 0;
    sd->sd_lineLeftBackColorExt= 0;
    sd->sd_lineLeftBackColorExtCMY= 0;
    sd->sd_lineLeftBackColorExtK= 0;
    sd->sd_lineLeftBackColorExtMod= 0;
    sd->sd_lineLeftColor= 0;
    sd->sd_lineLeftColorExt= 0;
    sd->sd_lineLeftColorExtCMY= 0;
    sd->sd_lineLeftColorExtK= 0;
    sd->sd_lineLeftColorExtMod= 0;
    sd->sd_lineLeftCrMod= 0;
    sd->sd_lineLeftDashing= 0;
    sd->sd_lineLeftEndArrowLength= 0;
    sd->sd_lineLeftEndArrowWidth= 0;
    sd->sd_lineLeftEndArrowhead= 0;
    sd->sd_lineLeftEndCapStyle= 0;
    sd->sd_lineLeftFillBlipFlags= 0;
    sd->sd_lineLeftFillDztype= 0;
    sd->sd_lineLeftFillHeight= 0;
    sd->sd_lineLeftFillWidth= 0;
    sd->sd_lineLeftMiterLimit= 0;
    sd->sd_lineLeftOpacity= 0;
    sd->sd_lineLeftStartArrowLength= 0;
    sd->sd_lineLeftStartArrowWidth= 0;
    sd->sd_lineLeftStartArrowhead= 0;
    sd->sd_lineLeftStyle= 0;
    sd->sd_lineLeftType= 0;
    sd->sd_lineLeftWidth= 0;
    sd->sd_lineRightBackColor= 0;
    sd->sd_lineRightBackColorExt= 0;
    sd->sd_lineRightBackColorExtCMY= 0;
    sd->sd_lineRightBackColorExtK= 0;
    sd->sd_lineRightBackColorExtMod= 0;
    sd->sd_lineRightColor= 0;
    sd->sd_lineRightColorExt= 0;
    sd->sd_lineRightColorExtCMY= 0;
    sd->sd_lineRightColorExtK= 0;
    sd->sd_lineRightColorExtMod= 0;
    sd->sd_lineRightCrMod= 0;
    sd->sd_lineRightDashing= 0;
    sd->sd_lineRightEndArrowLength= 0;
    sd->sd_lineRightEndArrowWidth= 0;
    sd->sd_lineRightEndArrowhead= 0;
    sd->sd_lineRightEndCapStyle= 0;
    sd->sd_lineRightFillBlipFlags= 0;
    sd->sd_lineRightFillDztype= 0;
    sd->sd_lineRightFillHeight= 0;
    sd->sd_lineRightFillWidth= 0;
    sd->sd_lineRightMiterLimit= 0;
    sd->sd_lineRightOpacity= 0;
    sd->sd_lineRightStartArrowLength= 0;
    sd->sd_lineRightStartArrowWidth= 0;
    sd->sd_lineRightStartArrowhead= 0;
    sd->sd_lineRightStyle= 0;
    sd->sd_lineRightType= 0;
    sd->sd_lineRightWidth= 0;
    sd->sd_lineTopBackColor= 0;
    sd->sd_lineTopBackColorExt= 0;
    sd->sd_lineTopBackColorExtCMY= 0;
    sd->sd_lineTopBackColorExtK= 0;
    sd->sd_lineTopBackColorExtMod= 0;
    sd->sd_lineTopColor= 0;
    sd->sd_lineTopColorExt= 0;
    sd->sd_lineTopColorExtCMY= 0;
    sd->sd_lineTopColorExtK= 0;
    sd->sd_lineTopColorExtMod= 0;
    sd->sd_lineTopCrMod= 0;
    sd->sd_lineTopDashing= 0;
    sd->sd_lineTopEndArrowLength= 0;
    sd->sd_lineTopEndArrowWidth= 0;
    sd->sd_lineTopEndArrowhead= 0;
    sd->sd_lineTopEndCapStyle= 0;
    sd->sd_lineTopFillBlipFlags= 0;
    sd->sd_lineTopFillDztype= 0;
    sd->sd_lineTopFillHeight= 0;
    sd->sd_lineTopFillWidth= 0;
    sd->sd_lineTopMiterLimit= 0;
    sd->sd_lineTopOpacity= 0;
    sd->sd_lineTopStartArrowLength= 0;
    sd->sd_lineTopStartArrowWidth= 0;
    sd->sd_lineTopStartArrowhead= 0;
    sd->sd_lineTopStyle= 0;
    sd->sd_lineTopType= 0;
    sd->sd_lineTopWidth= 0;

				/*
				 *  Strings
				 */
    utilInitMemoryBuffer( &(sd->sd_wzName) );
    utilInitMemoryBuffer( &(sd->sd_wzDescription) );
    utilInitMemoryBuffer( &(sd->sd_gtextUNICODE) );
    utilInitMemoryBuffer( &(sd->sd_gtextFont) );
    utilInitMemoryBuffer( &(sd->sd_pibName) );
    utilInitMemoryBuffer( &(sd->sd_fillBlipName) );

				/*
				 *  Text box
				 */
    sd->sdGeoRect.drX0= 0;
    sd->sdGeoRect.drY0= 0;
    sd->sdGeoRect.drX1= 21600;
    sd->sdGeoRect.drY1= 21600;
    sd->sdAdjustValue= 0;
    sd->sdAdjust2Value= 0;
    sd->sdAdjust3Value= 0;
    sd->sdAdjust4Value= 0;
    sd->sdAdjust5Value= 0;
    sd->sdAdjust6Value= 0;
    sd->sdAdjust7Value= 0;
    sd->sdAdjust8Value= 0;
    sd->sdAdjust9Value= 0;
    sd->sdAdjust10Value= 0;

				/****************************************/
				/*  Grouped Shapes			*/
				/****************************************/
    sd->sdRelRect.drX0= 0;
    sd->sdRelRect.drY0= 0;
    sd->sdRelRect.drX1= 1;
    sd->sdRelRect.drY1= 1;

    sd->sdRelRotation= 0;
    sd->sdRegroupID= 0;

    sd->sdGroupRect.drX0= 0;
    sd->sdGroupRect.drY0= 0;
    sd->sdGroupRect.drX1= 20000;
    sd->sdGroupRect.drY1= 20000;

    sd->sd_fRelChangePage= 0;
    sd->sd_fRelFlipH= 0;
    sd->sd_fRelFlipV= 0;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    sd->sdShadowType= 0;

    sd->sdShadowOpacity= 65536;
    sd->sdShadowOffsetXEmu= 0;
    sd->sdShadowOffsetYEmu= 0;
    sd->sdShadowOffset2XEmu= 25400;
    sd->sdShadowOffset2YEmu= 25400;
    sd->sdShadowScaleXToX= 65536;
    sd->sdShadowScaleYToX= 0;
    sd->sdShadowScaleXToY= 0;
    sd->sdShadowScaleYToY= 65536;
    sd->sdShadowPerspectiveX= 0;
    sd->sdShadowPerspectiveY= 0;
    sd->sdShadowWeight= 32768;
    sd->sdShadowOriginX= 0;
    sd->sdShadowOriginY= 0;

    sd->sd_shadowColorExt= 0;
    sd->sd_shadowColorExtCMY= 0;
    sd->sd_shadowColorExtK= 0;
    sd->sd_shadowColorExtMod= 0;
    sd->sd_shadowHighlightExt= 0;
    sd->sd_shadowHighlightExtCMY= 0;
    sd->sd_shadowHighlightExtK= 0;
    sd->sd_shadowHighlightExtMod= 0;

    sd->sd_fShadow= 0;
    sd->sd_fshadowObscured= 0;
    sd->sd_fshadowOK= 1;

    utilInitRGB8Color( &(sd->sdShadowColor) );
    utilInitRGB8Color( &(sd->sdShadowHighlightColor) );
    utilInitRGB8Color( &(sd->sdShadowCrModColor) );
    utilInitRGB8Color( &(sd->sdShadowc3DExtrusionColor) );
    utilInitRGB8Color( &(sd->sdShadowc3DCrModColor) );
    utilInitRGB8Color( &(sd->sdPictureTransparentColor) );

    sd->sdShadowColor.rgb8Red= 128;
    sd->sdShadowColor.rgb8Green= 128;
    sd->sdShadowColor.rgb8Blue= 128;

    sd->sdShadowHighlightColor.rgb8Red= 203;
    sd->sdShadowHighlightColor.rgb8Green= 203;
    sd->sdShadowHighlightColor.rgb8Blue= 203;

			/************************************************/
			/*  Connectors.					*/
			/************************************************/

    sd->sdConnectionSite= 1;
    sd->sdConnectionStyle= 3;

    sd->sd_gtextAlign= 0;
    sd->sd_fGtext= 0;
    sd->sd_gtextFVertical= 0;
    sd->sd_gtextFKern= 0;
    sd->sd_gtextFTight= 0;
    sd->sd_gtextFStretch= 0;
    sd->sd_gtextFShrinkFit= 0;
    sd->sd_gtextFBestFit= 0;
    sd->sd_gtextFNormalize= 0;
    sd->sd_gtextFDxMeasure= 0;
    sd->sd_gtextFBold= 0;
    sd->sd_gtextFItalic= 0;
    sd->sd_gtextFUnderline= 0;
    sd->sd_gtextFShadow= 0;
    sd->sd_gtextFSmallcaps= 0;
    sd->sd_gtextFStrikethrough= 0;

    sd->sd_shapePath= 0;

    sd->sd_pictureActive= 0;
    sd->sd_pictureId= 0;
    sd->sd_pictureContrast= 0;
    sd->sd_pictureBrightness= 0;
    sd->sd_pictureDblCrMod= 0;
    sd->sd_pictureFillCrMod= 0;
    sd->sd_pictureLineCrMod= 0;
    sd->sd_pibFlags= 0;
    sd->sd_dhgt= 0;
    sd->sd_sizerelh= 0;
    sd->sd_sizerelv= 0;
    sd->sd_pctHoriz= 0;
    sd->sd_pictureGray= 0;
    sd->sd_pictureBiLevel= 0;
    sd->sd_fPreferRelativeResize= 0;
    sd->sd_fScriptAnchor= 0;
    sd->sd_fFakeMaster= 0;
    sd->sd_fCameFromImgDummy= 0;
    sd->sd_fRecolorFillAsPicture= 0;

    sd->sd_cropFromLeft= 0;
    sd->sd_cropFromRight= 0;
    sd->sd_cropFromTop= 0;
    sd->sd_cropFromBottom= 0;

    sd->sd_c3DEdgeThickness= 0;
    sd->sd_c3DExtrudeForward= 0;
    sd->sd_c3DExtrudeBackward= 0;
    sd->sd_c3DRotationAxisX= 0;
    sd->sd_c3DRotationAxisY= 0;
    sd->sd_c3DRotationAxisZ= 0;
    sd->sd_c3DRotationCenterZ= 0;
    sd->sd_c3DXViewpoint= 0;
    sd->sd_c3DYViewpoint= 0;
    sd->sd_c3DZViewpoint= 0;
    sd->sd_c3DKeyX= 0;
    sd->sd_c3DKeyY= 0;
    sd->sd_c3DKeyZ= 0;
    sd->sd_c3DFillX= 0;
    sd->sd_c3DFillY= 0;
    sd->sd_c3DFillZ= 0;
    sd->sd_c3DAmbientIntensity= 0;
    sd->sd_c3DDiffuseAmt= 0;
    sd->sd_c3DExtrudePlane= 0;
    sd->sd_c3DFillIntensity= 0;
    sd->sd_c3DKeyIntensity= 0;
    sd->sd_c3DOriginX= 0;
    sd->sd_c3DOriginY= 0;
    sd->sd_c3DRenderMode= 0;
    sd->sd_c3DRotationAngle= 0;
    sd->sd_c3DRotationCenterX= 0;
    sd->sd_c3DRotationCenterY= 0;
    sd->sd_c3DShininess= 0;
    sd->sd_c3DSkewAmount= 0;
    sd->sd_c3DSkewAngle= 0;
    sd->sd_c3DSpecularAmt= 0;
    sd->sd_c3DTolerance= 0;
    sd->sd_c3DXRotationAngle= 0;
    sd->sd_c3DYRotationAngle= 0;
    sd->sd_c3DExtrusionColorExt= 0;
    sd->sd_c3DExtrusionColorExtCMY= 0;
    sd->sd_c3DExtrusionColorExtK= 0;
    sd->sd_c3DExtrusionColorExtMod= 0;

    sd->sd_f3D= 0;
    sd->sd_fC3DRotationCenterAut= 0;
    sd->sd_fc3DMetallic= 0;
    sd->sd_fc3DUseExtrusionColor= 0;
    sd->sd_fc3DLightFace= 0;
    sd->sd_fc3DParallel= 0;
    sd->sd_fc3DKeyHarsh= 0;
    sd->sd_fc3DFillHarsh= 0;
    sd->sd_fc3DConstrainRotation= 0;
    sd->sd_fc3DRotationCenterAuto= 0;

    sd->sd_spcot= 0;
    sd->sd_dxyCalloutGap= 0;
    sd->sd_spcoa= 0;
    sd->sd_spcod= 0;
    sd->sd_dxyCalloutDropSpecified= 0;
    sd->sd_dxyCalloutLengthSpecified= 0;
    sd->sd_bWMode= 0;
    sd->sd_dgmt= 0;
    sd->sd_ccol= 0;
    sd->sd_dzColMargin= 0;
    sd->sd_fillColorExt= 0;
    sd->sd_fillColorExtCMY= 0;
    sd->sd_fillColorExtK= 0;
    sd->sd_fillColorExtMod= 0;

    sd->sd_fStandardHR= 0;
    sd->sd_fNoshadeHR= 0;
    sd->sd_fHorizRule= 0;
    sd->sd_fUserDrawn= 0;
    sd->sd_fCallout= 0;
    sd->sd_fCalloutAccentBar= 0;
    sd->sd_fCalloutTextBorder= 0;
    sd->sd_fCalloutDropAuto= 0;
    sd->sd_fCalloutLengthSpecified= 0;
    sd->sd_fCalloutMinusX= 0;
    sd->sd_fCalloutMinusY= 0;
    sd->sd_f3DOK= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Copy a shape.							*/
/*									*/
/*  2)  Vertices.							*/
/*  2)  Wrap polygon.							*/
/*  4)  Segment information						*/
/*  5)  Text box content.						*/
/*									*/
/************************************************************************/

int docCopyShapeDrawing(	ShapeDrawing *		to,
				const ShapeDrawing *	from )
    {
    int				rval= 0;

    *to= *from;
    docInitShapeDrawingAllocated( to );

    /*  2  */
    to->sdVertices= (Point2DI *)malloc( from->sdVertexCount* sizeof(Point2DI) );
    if  ( ! to->sdVertices )
	{
	LXDEB(from->sdVertexCount,to->sdVertices);
	rval= -1; goto ready;
	}

    /*  3  */
    to->sdWrapPolygonVertices= (Point2DI *)malloc(
		    from->sdWrapPolygonVertexCount* sizeof(Point2DI) );
    if  ( ! to->sdWrapPolygonVertices )
	{
	LXDEB(from->sdWrapPolygonVertexCount,to->sdWrapPolygonVertices);
	rval= -1; goto ready;
	}

    /*  4  */
    to->sdSegmentInfos= (int *)malloc( from->sdSegmentInfoCount* sizeof(int) );
    if  ( ! to->sdSegmentInfos )
	{
	LXDEB(from->sdSegmentInfoCount,to->sdSegmentInfos);
	rval= -1; goto ready;
	}

    /*  2  */
    while( to->sdVertexCount < from->sdVertexCount )
	{
	to->sdVertices[to->sdVertexCount]= from->sdVertices[to->sdVertexCount];
	to->sdVertexCount++;
	}

    /*  3  */
    while( to->sdWrapPolygonVertexCount < from->sdWrapPolygonVertexCount )
	{
	to->sdWrapPolygonVertices[to->sdWrapPolygonVertexCount]=
		    from->sdWrapPolygonVertices[to->sdWrapPolygonVertexCount];
	to->sdWrapPolygonVertexCount++;
	}

    /*  4  */
    while( to->sdSegmentInfoCount < from->sdSegmentInfoCount )
	{
	to->sdSegmentInfos[to->sdSegmentInfoCount]=
			    from->sdSegmentInfos[to->sdSegmentInfoCount];
	to->sdSegmentInfoCount++;
	}

  ready:

    if  ( rval )
	{
	docCleanShapeDrawing( to );
	docInitShapeDrawingAllocated( to );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Translate shape type to string.					*/
/*									*/
/************************************************************************/

const char * docShapeTypeString(	int	shapeType )
    {
    static char		scratch[12];

    switch( shapeType )
	{
	case SHPtyFREEFORM_OR_NON_AUTOSHAPE: return "FREEFORM_OR_NON_AUTOSHAPE";
	case SHPtyRECTANGLE: return "RECTANGLE";
	case SHPtyROUND_RECTANGLE: return "ROUND_RECTANGLE";
	case SHPtyELLIPSE: return "ELLIPSE";
	case SHPtyDIAMOND: return "DIAMOND";
	case SHPtyISOSCELES_TRIANGLE: return "ISOSCELES_TRIANGLE";
	case SHPtyRIGHT_TRIANGLE: return "RIGHT_TRIANGLE";
	case SHPtyPARALLELOGRAM: return "PARALLELOGRAM";
	case SHPtyTRAPEZOID: return "TRAPEZOID";
	case SHPtyHEXAGON: return "HEXAGON";
	case SHPtyOCTAGON: return "OCTAGON";
	case SHPtyPLUS_SIGN: return "PLUS_SIGN";
	case SHPtySTAR: return "STAR";
	case SHPtyARROW: return "ARROW";
	case SHPtyTHICK_ARROW: return "THICK_ARROW";
	case SHPtyHOME_PLATE: return "HOME_PLATE";
	case SHPtyCUBE: return "CUBE";
	case SHPtyBALLOON: return "BALLOON";
	case SHPtySEAL: return "SEAL";
	case SHPtyARC: return "ARC";
	case SHPtyLINE: return "LINE";
	case SHPtyPLAQUE: return "PLAQUE";
	case SHPtyCAN: return "CAN";
	case SHPtyDONUT: return "DONUT";
	case SHPtyTEXT_SIMPLE: return "TEXT_SIMPLE";
	case SHPtyTEXT_OCTAGON: return "TEXT_OCTAGON";
	case SHPtyTEXT_HEXAGON: return "TEXT_HEXAGON";
	case SHPtyTEXT_CURVE: return "TEXT_CURVE";
	case SHPtyTEXT_WAVE: return "TEXT_WAVE";
	case SHPtyTEXT_RING: return "TEXT_RING";
	case SHPtyTEXT_ON_CURVE: return "TEXT_ON_CURVE";
	case SHPtyTEXT_ON_RING: return "TEXT_ON_RING";
	case SHPtyCALLOUT_1: return "CALLOUT_1";
	case SHPtyCALLOUT_2: return "CALLOUT_2";
	case SHPtyCALLOUT_3: return "CALLOUT_3";
	case SHPtyACCENT_CALLOUT_1: return "ACCENT_CALLOUT_1";
	case SHPtyACCENT_CALLOUT_2: return "ACCENT_CALLOUT_2";
	case SHPtyACCENT_CALLOUT_3: return "ACCENT_CALLOUT_3";
	case SHPtyBORDER_CALLOUT_1: return "BORDER_CALLOUT_1";
	case SHPtyBORDER_CALLOUT_2: return "BORDER_CALLOUT_2";
	case SHPtyBORDER_CALLOUT_3: return "BORDER_CALLOUT_3";
	case SHPtyACCENT_BORDER_CALLOUT_1: return "ACCENT_BORDER_CALLOUT_1";
	case SHPtyACCENT_BORDER_CALLOUT_2: return "ACCENT_BORDER_CALLOUT_2";
	case SHPtyACCENT_BORDER_CALLOUT_3: return "ACCENT_BORDER_CALLOUT_3";
	case SHPtyRIBBON: return "RIBBON";
	case SHPtyRIBBON2: return "RIBBON2";
	case SHPtyCHEVRON: return "CHEVRON";
	case SHPtyPENTAGON: return "PENTAGON";
	case SHPtyNO_SMOKING: return "NO_SMOKING";
	case SHPtySEAL8: return "SEAL8";
	case SHPtySEAL16: return "SEAL16";
	case SHPtySEAL32: return "SEAL32";
	case SHPtyWEDGE_RECT_CALLOUT: return "WEDGE_RECT_CALLOUT";
	case SHPtyWEDGE_RRECT_CALLOUT: return "WEDGE_RRECT_CALLOUT";
	case SHPtyWEDGE_ELLIPSE_CALLOUT: return "WEDGE_ELLIPSE_CALLOUT";
	case SHPtyWAVE: return "WAVE";
	case SHPtyFOLDED_CORNER: return "FOLDED_CORNER";
	case SHPtyLEFT_ARROW: return "LEFT_ARROW";
	case SHPtyDOWN_ARROW: return "DOWN_ARROW";
	case SHPtyUP_ARROW: return "UP_ARROW";
	case SHPtyLEFT_RIGHT_ARROW: return "LEFT_RIGHT_ARROW";
	case SHPtyUP_DOWN_ARROW: return "UP_DOWN_ARROW";
	case SHPtyIRREGULARSEAL1: return "IRREGULARSEAL1";
	case SHPtyIRREGULARSEAL2: return "IRREGULARSEAL2";
	case SHPtyLIGHTNING_BOLT: return "LIGHTNING_BOLT";
	case SHPtyHEART: return "HEART";
	case SHPtyPICTURE_FRAME: return "PICTURE_FRAME";
	case SHPtyQUAD_ARROW: return "QUAD_ARROW";
	case SHPtyLEFT_ARROW_CALLOUT: return "LEFT_ARROW_CALLOUT";
	case SHPtyRIGHT_ARROW_CALLOUT: return "RIGHT_ARROW_CALLOUT";
	case SHPtyUP_ARROW_CALLOUT: return "UP_ARROW_CALLOUT";
	case SHPtyDOWN_ARROW_CALLOUT: return "DOWN_ARROW_CALLOUT";
	case SHPtyLEFT_RIGHT_ARROW_CALLOUT: return "LEFT_RIGHT_ARROW_CALLOUT";
	case SHPtyUP_DOWN_ARROW_CALLOUT: return "UP_DOWN_ARROW_CALLOUT";
	case SHPtyQUAD_ARROW_CALLOUT: return "QUAD_ARROW_CALLOUT";
	case SHPtyBEVEL: return "BEVEL";
	case SHPtyLEFT_BRACKET: return "LEFT_BRACKET";
	case SHPtyRIGHT_BRACKET: return "RIGHT_BRACKET";
	case SHPtyLEFT_BRACE: return "LEFT_BRACE";
	case SHPtyRIGHT_BRACE: return "RIGHT_BRACE";
	case SHPtyLEFT_UP_ARROW: return "LEFT_UP_ARROW";
	case SHPtyBENT_UP_ARROW: return "BENT_UP_ARROW";
	case SHPtyBENT_ARROW: return "BENT_ARROW";
	case SHPtySEAL24: return "SEAL24";
	case SHPtySTRIPED_RIGHT_ARROW: return "STRIPED_RIGHT_ARROW";
	case SHPtyNOTCHED_RIGHT_ARROW: return "NOTCHED_RIGHT_ARROW";
	case SHPtyBLOCK_ARC: return "BLOCK_ARC";
	case SHPtySMILEY_FACE: return "SMILEY_FACE";
	case SHPtyVERTICAL_SCROLL: return "VERTICAL_SCROLL";
	case SHPtyHORIZONTAL_SCROLL: return "HORIZONTAL_SCROLL";
	case SHPtyCIRCULAR_ARROW: return "CIRCULAR_ARROW";
	case SHPtyNOTCHED_CIRCULAR_ARROW: return "NOTCHED_CIRCULAR_ARROW";
	case SHPtyUTURN_ARROW: return "UTURN_ARROW";
	case SHPtyCURVED_RIGHT_ARROW: return "CURVED_RIGHT_ARROW";
	case SHPtyCURVED_LEFT_ARROW: return "CURVED_LEFT_ARROW";
	case SHPtyCURVED_UP_ARROW: return "CURVED_UP_ARROW";
	case SHPtyCURVED_DOWN_ARROW: return "CURVED_DOWN_ARROW";
	case SHPtyCLOUD_CALLOUT: return "CLOUD_CALLOUT";
	case SHPtyELLIPSE_RIBBON: return "ELLIPSE_RIBBON";
	case SHPtyELLIPSE_RIBBON_2: return "ELLIPSE_RIBBON_2";
	case SHPtyFLOW_CHART_PROCESS: return "FLOW_CHART_PROCESS";
	case SHPtyFLOW_CHART_DECISION: return "FLOW_CHART_DECISION";
	case SHPtyFLOW_CHART_INPUT_OUTPUT: return "FLOW_CHART_INPUT_OUTPUT";
	case SHPtyFLOW_CHART_PREDEFINED_PROCESS:
				    return "FLOW_CHART_PREDEFINED_PROCESS";
	case SHPtyFLOW_CHART_INTERNAL_STORAGE:
				    return "FLOW_CHART_INTERNAL_STORAGE";
	case SHPtyFLOW_CHART_DOCUMENT: return "FLOW_CHART_DOCUMENT";
	case SHPtyFLOW_CHART_MULTIDOCUMENT: return "FLOW_CHART_MULTIDOCUMENT";
	case SHPtyFLOW_CHART_TERMINATOR: return "FLOW_CHART_TERMINATOR";
	case SHPtyFLOW_CHART_PREPARATION: return "FLOW_CHART_PREPARATION";
	case SHPtyFLOW_CHART_MANUAL_INPUT: return "FLOW_CHART_MANUAL_INPUT";
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
				    return "FLOW_CHART_MANUAL_OPERATION";
	case SHPtyFLOW_CHART_CONNECTOR: return "FLOW_CHART_CONNECTOR";
	case SHPtyFLOW_CHART_PUNCHED_CARD: return "FLOW_CHART_PUNCHED_CARD";
	case SHPtyFLOW_CHART_PUNCHED_TAPE: return "FLOW_CHART_PUNCHED_TAPE";
	case SHPtyFLOW_CHART_SUMMING_JUNCTION:
				    return "FLOW_CHART_SUMMING_JUNCTION";
	case SHPtyFLOW_CHART_OR: return "FLOW_CHART_OR";
	case SHPtyFLOW_CHART_COLLATE: return "FLOW_CHART_COLLATE";
	case SHPtyFLOW_CHART_SORT: return "FLOW_CHART_SORT";
	case SHPtyFLOW_CHART_EXTRACT: return "FLOW_CHART_EXTRACT";
	case SHPtyFLOW_CHART_MERGE: return "FLOW_CHART_MERGE";
	case SHPtyFLOW_CHART_OFFLINE_STORAGE:
					return "FLOW_CHART_OFFLINE_STORAGE";
	case SHPtyFLOW_CHART_ONLINE_STORAGE: return "FLOW_CHART_ONLINE_STORAGE";
	case SHPtyFLOW_CHART_MAGNETIC_TAPE: return "FLOW_CHART_MAGNETIC_TAPE";
	case SHPtyFLOW_CHART_MAGNETIC_DISK: return "FLOW_CHART_MAGNETIC_DISK";
	case SHPtyFLOW_CHART_MAGNETIC_DRUM: return "FLOW_CHART_MAGNETIC_DRUM";
	case SHPtyFLOW_CHART_DISPLAY: return "FLOW_CHART_DISPLAY";
	case SHPtyFLOW_CHART_DELAY: return "FLOW_CHART_DELAY";
	case SHPtyTEXT_PLAIN_TEXT: return "TEXT_PLAIN_TEXT";
	case SHPtyTEXT_STOP: return "TEXT_STOP";
	case SHPtyTEXT_TRIANGLE: return "TEXT_TRIANGLE";
	case SHPtyTEXT_TRIANGLE_INVERTED: return "TEXT_TRIANGLE_INVERTED";
	case SHPtyTEXT_CHEVRON: return "TEXT_CHEVRON";
	case SHPtyTEXT_CHEVRON_INVERTED: return "TEXT_CHEVRON_INVERTED";
	case SHPtyTEXT_RING_INSIDE: return "TEXT_RING_INSIDE";
	case SHPtyTEXT_RING_OUTSIDE: return "TEXT_RING_OUTSIDE";
	case SHPtyTEXT_ARCH_UP_CURVE: return "TEXT_ARCH_UP_CURVE";
	case SHPtyTEXT_ARCH_DOWN_CURVE: return "TEXT_ARCH_DOWN_CURVE";
	case SHPtyTEXT_CIRCLE_CURVE: return "TEXT_CIRCLE_CURVE";
	case SHPtyTEXT_BUTTON_CURVE: return "TEXT_BUTTON_CURVE";
	case SHPtyTEXT_ARCH_UP_POUR: return "TEXT_ARCH_UP_POUR";
	case SHPtyTEXT_ARCH_DOWN_POUR: return "TEXT_ARCH_DOWN_POUR";
	case SHPtyTEXT_CIRCLE_POUR: return "TEXT_CIRCLE_POUR";
	case SHPtyTEXT_BUTTON_POUR: return "TEXT_BUTTON_POUR";
	case SHPtyTEXT_CURVE_UP: return "TEXT_CURVE_UP";
	case SHPtyTEXT_CURVE_DOWN: return "TEXT_CURVE_DOWN";
	case SHPtyTEXT_CASCADE_UP: return "TEXT_CASCADE_UP";
	case SHPtyTEXT_CASCADE_DOWN: return "TEXT_CASCADE_DOWN";
	case SHPtyTEXT_WAVE1: return "TEXT_WAVE1";
	case SHPtyTEXT_WAVE2: return "TEXT_WAVE2";
	case SHPtyTEXT_WAVE3: return "TEXT_WAVE3";
	case SHPtyTEXT_WAVE4: return "TEXT_WAVE4";
	case SHPtyTEXT_INFLATE: return "TEXT_INFLATE";
	case SHPtyTEXT_DEFLATE: return "TEXT_DEFLATE";
	case SHPtyTEXT_INFLATE_BOTTOM: return "TEXT_INFLATE_BOTTOM";
	case SHPtyTEXT_DEFLATE_BOTTOM: return "TEXT_DEFLATE_BOTTOM";
	case SHPtyTEXT_INFLATE_TOP: return "TEXT_INFLATE_TOP";
	case SHPtyTEXT_DEFLATE_TOP: return "TEXT_DEFLATE_TOP";
	case SHPtyTEXT_DEFLATE_INFLATE: return "TEXT_DEFLATE_INFLATE";
	case SHPtyTEXT_DEFLATE_INFLATE_DEFLATE:
					return "TEXT_DEFLATE_INFLATE_DEFLATE";
	case SHPtyTEXT_FADE_RIGHT: return "TEXT_FADE_RIGHT";
	case SHPtyTEXT_FADE_LEFT: return "TEXT_FADE_LEFT";
	case SHPtyTEXT_FADE_UP: return "TEXT_FADE_UP";
	case SHPtyTEXT_FADE_DOWN: return "TEXT_FADE_DOWN";
	case SHPtyTEXT_SLANT_UP: return "TEXT_SLANT_UP";
	case SHPtyTEXT_SLANT_DOWN: return "TEXT_SLANT_DOWN";
	case SHPtyTEXT_CAN_UP: return "TEXT_CAN_UP";
	case SHPtyTEXT_CAN_DOWN: return "TEXT_CAN_DOWN";
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
					return "FLOW_CHART_ALTERNATE_PROCESS";
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
					return "FLOW_CHART_OFF_PAGE_CONNECTOR";
	case SHPtyCALLOUT_90: return "CALLOUT_90";
	case SHPtyACCENT_CALLOUT_90: return "ACCENT_CALLOUT_90";
	case SHPtyBORDER_CALLOUT_90: return "BORDER_CALLOUT_90";
	case SHPtyACCENT_BORDER_CALLOUT_90: return "ACCENT_BORDER_CALLOUT_90";
	case SHPtyLEFT_RIGHT_UP_ARROW: return "LEFT_RIGHT_UP_ARROW";
	case SHPtySUN: return "SUN";
	case SHPtyMOON: return "MOON";
	case SHPtyBRACKET_PAIR: return "BRACKET_PAIR";
	case SHPtyBRACE_PAIR: return "BRACE_PAIR";
	case SHPtySEAL4: return "SEAL4";
	case SHPtyDOUBLE_WAVE: return "DOUBLE_WAVE";
	case SHPtyHOST_CONTROL: return "HOST_CONTROL";
	case SHPtyTEXT_BOX: return "TEXT_BOX";

	case SHPtyUNKNOWN: return "UNKNOWN";
	case SHPtyGROUP: return "GROUP";
	case SHPtyPICPROP: return "PICPROP";

	default:
	    sprintf( scratch, "SHPty%d", shapeType );
	    return scratch;
	}
    }

static void docSetShapeDrawingColor(
				RGB8Color *		rgb8,
				long			val )
    {
    utilInitRGB8Color( rgb8 );

    rgb8->rgb8Red= val & 255; val /= 256;
    rgb8->rgb8Green= val & 255; val /= 256;
    rgb8->rgb8Blue= val & 255; /* val /= 256; */

    return;
    }

static long docGetShapeDrawingColor(
				const RGB8Color *	rgb8 )
    {
    return rgb8->rgb8Red+ 256L* rgb8->rgb8Green+ 256L* 256L* rgb8->rgb8Blue;
    }

int docSetShapeDrawingProperty(	ShapeDrawing *		sd,
				int			prop,
				long			val )
    {
    switch( prop )
	{
	case DSHPprop_posh:
	    sd->sdXPosition= val;
	    break;
	case DSHPprop_posrelh:
	    sd->sdXReference= val;
	    break;
	case DSHPprop_posv:
	    sd->sdYPosition= val;
	    break;
	case DSHPprop_posrelv:
	    sd->sdYReference= val;
	    break;

	case DSHPprop_fUseShapeAnchor:
	    sd->sd_fUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fLayoutInCell:
	    sd->sd_fLayoutInCell= val != 0;
	    break;
	case DSHPprop_fAllowOverlap:
	    sd->sd_fAllowOverlap= val != 0;
	    break;
	case DSHPprop_fChangePage:
	    sd->sd_fChangePage= val != 0;
	    break;

	case DSHPprop_shapeType:
	    sd->sdShapeType= val;
	    break;

	case DSHPprop_rotation:
	    sd->sdRotation= val;
	    break;

	case DSHPprop_dxWrapDistLeft:
	    sd->sdWrapDistLeftEmu= val;
	    break;
	case DSHPprop_dyWrapDistTop:
	    sd->sdWrapDistTopEmu= val;
	    break;
	case DSHPprop_dxWrapDistRight:
	    sd->sdWrapDistRightEmu= val;
	    break;
	case DSHPprop_dyWrapDistBottom:
	    sd->sdWrapDistBottomEmu= val;
	    break;

	case DSHPprop_hspMaster:
	    sd->sdMasterShape= val;
	    break;
	case DSHPprop_hspNext:
	    sd->sdNextShape= val;
	    break;

	case DSHPprop_xLimo:
	    sd->sdXLimo= val;
	    break;
	case DSHPprop_yLimo:
	    sd->sdXLimo= val;
	    break;

	case DSHPprop_fIsBullet:
	    sd->sd_fIsBullet= val != 0;
	    break;
	case DSHPprop_fFlipV:
	    sd->sd_fFlipV= val != 0;
	    break;
	case DSHPprop_fFlipH:
	    sd->sd_fFlipH= val != 0;
	    break;
	case DSHPprop_fBehindDocument:
	    sd->sd_fBehindDocument= val != 0;
	    break;
	case DSHPprop_fIsButton:
	    sd->sd_fIsButton= val != 0;
	    break;
	case DSHPprop_fHidden:
	    sd->sd_fHidden= val != 0;
	    break;
	case DSHPprop_fReallyHidden:
	    sd->sd_fReallyHidden= val != 0;
	    break;
	case DSHPprop_fArrowheadsOK:
	    sd->sd_fArrowheadsOK= val != 0;
	    break;
	case DSHPprop_fBackground:
	    sd->sd_fBackground= val != 0;
	    break;
	case DSHPprop_fDeleteAttachedObject:
	    sd->sd_fDeleteAttachedObject= val != 0;
	    break;
	case DSHPprop_fEditedWrap:
	    sd->sd_fEditedWrap= val != 0;
	    break;
	case DSHPprop_fHitTestFill:
	    sd->sd_fHitTestFill= val != 0;
	    break;
	case DSHPprop_fHitTestLine:
	    sd->sd_fHitTestLine= val != 0;
	    break;
	case DSHPprop_fInitiator:
	    sd->sd_fInitiator= val != 0;
	    break;
	case DSHPprop_fNoFillHitTest:
	    sd->sd_fNoFillHitTest= val != 0;
	    break;
	case DSHPprop_fNoHitTestPicture:
	    sd->sd_fNoHitTestPicture= val != 0;
	    break;
	case DSHPprop_fNoLineDrawDash:
	    sd->sd_fNoLineDrawDash= val != 0;
	    break;
	case DSHPprop_fOleIcon:
	    sd->sd_fOleIcon= val != 0;
	    break;
	case DSHPprop_fOnDblClickNotify:
	    sd->sd_fOnDblClickNotify= val != 0;
	    break;
	case DSHPprop_fOneD:
	    sd->sd_fOneD= val != 0;
	    break;
	case DSHPprop_fPreferRelativeSize:
	    sd->sd_fPreferRelativeSize= val != 0;
	    break;
	case DSHPprop_fPrint:
	    sd->sd_fPrint= val != 0;
	    break;

	case DSHPprop_fPseudoInline:
	    sd->sd_fPseudoInline= val != 0;
	    break;

	case DSHPprop_fLockRotation:
	    sd->sd_fLockRotation= val != 0;
	    break;
	case DSHPprop_fLockAspectRatio:
	    sd->sd_fLockAspectRatio= val != 0;
	    break;
	case DSHPprop_fLockAgainstSelect:
	    sd->sd_fLockAgainstSelect= val != 0;
	    break;
	case DSHPprop_fLockCropping:
	    sd->sd_fLockCropping= val != 0;
	    break;
	case DSHPprop_fLockVerticies:
	    sd->sd_fLockVerticies= val != 0;
	    break;
	case DSHPprop_fLockText:
	    sd->sd_fLockText= val != 0;
	    break;
	case DSHPprop_fLockAdjustHandles:
	    sd->sd_fLockAdjustHandles= val != 0;
	    break;
	case DSHPprop_fLockAgainstGrouping:
	    sd->sd_fLockAgainstGrouping= val != 0;
	    break;
	case DSHPprop_fLockShapeType:
	    sd->sd_fLockShapeType= val != 0;
	    break;

	case DSHPprop_fLockPosition:
	    sd->sd_fLockPosition= val != 0;
	    break;

	case DSHPprop_fillType:
	    sd->sdFillType= val;
	    break;
	case DSHPprop_fillOpacity:
	    sd->sdFillOpacity= val;
	    break;
	case DSHPprop_fillBackOpacity:
	    sd->sdFillBackOpacity= val;
	    break;

	case DSHPprop_fillblipflags:
	    sd->sdFillblipflags= val;
	    break;

	case DSHPprop_fillWidth:
	    sd->sdFillWidthEmu= val;
	    break;
	case DSHPprop_fillHeight:
	    sd->sdFillHeightEmu= val;
	    break;
	case DSHPprop_fillAngle:
	    sd->sdFillAngle= val;
	    break;
	case DSHPprop_fillFocus:
	    sd->sdFillFocus= val;
	    break;

	case DSHPprop_fillToLeft:
	    sd->sdFillToLeft= val;
	    break;
	case DSHPprop_fillToTop:
	    sd->sdFillToTop= val;
	    break;
	case DSHPprop_fillToRight:
	    sd->sdFillToRight= val;
	    break;
	case DSHPprop_fillToBottom:
	    sd->sdFillToBottom= val;
	    break;

	case DSHPprop_fillOriginX:
	    sd->sdFillOriginX= val;
	    break;
	case DSHPprop_fillOriginY:
	    sd->sdFillOriginY= val;
	    break;
	case DSHPprop_fillShapeOriginX:
	    sd->sdFillShapeOriginX= val;
	    break;
	case DSHPprop_fillShapeOriginY:
	    sd->sdFillShapeOriginY= val;
	    break;

	case DSHPprop_fillDztype:
	    sd->sdFillDztype= val;
	    break;

	case DSHPprop_fillRectLeft:
	    sd->sdFillRectLeftEmu= val;
	    break;
	case DSHPprop_fillRectTop:
	    sd->sdFillRectTopEmu= val;
	    break;
	case DSHPprop_fillRectRight:
	    sd->sdFillRectRightEmu= val;
	    break;
	case DSHPprop_fillRectBottom:
	    sd->sdFillRectBottomEmu= val;
	    break;

	case DSHPprop_fillShadePreset:
	    sd->sdFillShadePreset= val;
	    break;
	case DSHPprop_fillShadeType:
	    sd->sdFillShadeType= val;
	    break;

	case DSHPprop_fFilled:
	    sd->sd_fFilled= val != 0;
	    break;
	case DSHPprop_fillShape:
	    sd->sd_fillShape= val != 0;
	    break;
	case DSHPprop_fillUseRect:
	    sd->sd_fillUseRect= val != 0;
	    break;
	case DSHPprop_fFillOK:
	    sd->sd_fFillOK= val != 0;
	    break;
	case DSHPprop_fFillShadeShapeOK:
	    sd->sd_fFillShadeShapeOK= val != 0;
	    break;

	case DSHPprop_lineType:
	    sd->sdLineType= val;
	    break;

	case DSHPprop_lineFillBlipFlags:
	    sd->sdLineFillBlipFlags= val;
	    break;

	case DSHPprop_lineFillWidth:
	    sd->sdLineFillWidthEmu= val;
	    break;
	case DSHPprop_lineFillHeight:
	    sd->sdLineFillHeightEmu= val;
	    break;
	case DSHPprop_lineWidth:
	    sd->sdLineWidthEmu= val;
	    break;

	case DSHPprop_lineStyle:
	    sd->sdLineStyle= val;
	    break;
	case DSHPprop_lineDashing:
	    sd->sdLineDashing= val;
	    break;
	case DSHPprop_lineDashStyle:
	    sd->sdLineDashStyle= val;
	    break;
	case DSHPprop_lineStartArrowhead:
	    sd->sdLineHeadArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineEndArrowhead:
	    sd->sdLineTailArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineStartArrowWidth:
	    sd->sdLineHeadArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineStartArrowLength:
	    sd->sdLineHeadArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndArrowWidth:
	    sd->sdLineTailArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineEndArrowLength:
	    sd->sdLineTailArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndCapStyle:
	    sd->sdLineEndCapStyle= val;
	    break;
	case DSHPprop_lineFillDztype:
	    sd->sdLineFillDztype= val;
	    break;
	case DSHPprop_lineJoinStyle:
	    sd->sdLineJoinStyle= val;
	    break;

	case DSHPprop_lineMiterLimit:
	    sd->sdLineMiterLimit= val;
	    break;

	case DSHPprop_fLine:
	    sd->sd_fLine= val != 0;
	    break;
	case DSHPprop_fLineOK:
	    sd->sd_fLineOK= val != 0;
	    break;

	case DSHPprop_fLineUseShapeAnchor:
	    sd->sd_fLineUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fColumnLineOK:
	    sd->sd_fColumnLineOK= val != 0;
	    break;
	case DSHPprop_fColumnLine:
	    sd->sd_fColumnLine= val != 0;
	    break;
	case DSHPprop_fLeftLine:
	    sd->sd_fLeftLine= val != 0;
	    break;
	case DSHPprop_fTopLine:
	    sd->sd_fTopLine= val != 0;
	    break;
	case DSHPprop_fRightLine:
	    sd->sd_fRightLine= val != 0;
	    break;
	case DSHPprop_fBottomLine:
	    sd->sd_fBottomLine= val != 0;
	    break;

	case DSHPprop_fColumnHitTestLine:
	    sd->sd_fColumnHitTestLine= val != 0;
	    break;
	case DSHPprop_fLeftHitTestLine:
	    sd->sd_fLeftHitTestLine= val != 0;
	    break;
	case DSHPprop_fTopHitTestLine:
	    sd->sd_fTopHitTestLine= val != 0;
	    break;
	case DSHPprop_fRightHitTestLine:
	    sd->sd_fRightHitTestLine= val != 0;
	    break;
	case DSHPprop_fBottomHitTestLine:
	    sd->sd_fBottomHitTestLine= val != 0;
	    break;

	case DSHPprop_lineFillShape:
	    sd->sd_lineFillShape= val != 0;
	    break;
	case DSHPprop_lineColumnFillShape:
	    sd->sd_lineColumnFillShape= val != 0;
	    break;
	case DSHPprop_lineLeftFillShape:
	    sd->sd_lineLeftFillShape= val != 0;
	    break;
	case DSHPprop_lineTopFillShape:
	    sd->sd_lineTopFillShape= val != 0;
	    break;
	case DSHPprop_lineRightFillShape:
	    sd->sd_lineRightFillShape= val != 0;
	    break;
	case DSHPprop_lineBottomFillShape:
	    sd->sd_lineBottomFillShape= val != 0;
	    break;

	case DSHPprop_fInsetPen:
	    sd->sd_fInsetPen= val != 0;
	    break;
	case DSHPprop_fLeftInsetPen:
	    sd->sd_fLeftInsetPen= val != 0;
	    break;
	case DSHPprop_fTopInsetPen:
	    sd->sd_fTopInsetPen= val != 0;
	    break;
	case DSHPprop_fRightInsetPen:
	    sd->sd_fRightInsetPen= val != 0;
	    break;
	case DSHPprop_fBottomInsetPen:
	    sd->sd_fBottomInsetPen= val != 0;
	    break;

	case DSHPprop_lineOpacity:
	    sd->sd_lineOpacity= val;
	    break;
	case DSHPprop_lineblipflags:
	    sd->sd_lineblipflags= val;
	    break;

	case DSHPprop_lineBackColorExt:
	    sd->sd_lineBackColorExt= val;
	    break;
	case DSHPprop_lineBackColorExtCMY:
	    sd->sd_lineBackColorExtCMY= val;
	    break;
	case DSHPprop_lineBackColorExtK:
	    sd->sd_lineBackColorExtK= val;
	    break;
	case DSHPprop_lineBackColorExtMod:
	    sd->sd_lineBackColorExtMod= val;
	    break;
	case DSHPprop_lineBottomBackColor:
	    sd->sd_lineBottomBackColor= val;
	    break;
	case DSHPprop_lineBottomBackColorExt:
	    sd->sd_lineBottomBackColorExt= val;
	    break;
	case DSHPprop_lineBottomBackColorExtCMY:
	    sd->sd_lineBottomBackColorExtCMY= val;
	    break;
	case DSHPprop_lineBottomBackColorExtK:
	    sd->sd_lineBottomBackColorExtK= val;
	    break;
	case DSHPprop_lineBottomBackColorExtMod:
	    sd->sd_lineBottomBackColorExtMod= val;
	    break;
	case DSHPprop_lineBottomColor:
	    sd->sd_lineBottomColor= val;
	    break;
	case DSHPprop_lineBottomColorExt:
	    sd->sd_lineBottomColorExt= val;
	    break;
	case DSHPprop_lineBottomColorExtCMY:
	    sd->sd_lineBottomColorExtCMY= val;
	    break;
	case DSHPprop_lineBottomColorExtK:
	    sd->sd_lineBottomColorExtK= val;
	    break;
	case DSHPprop_lineBottomColorExtMod:
	    sd->sd_lineBottomColorExtMod= val;
	    break;
	case DSHPprop_lineBottomCrMod:
	    sd->sd_lineBottomCrMod= val;
	    break;
	case DSHPprop_lineBottomDashing:
	    sd->sd_lineBottomDashing= val;
	    break;
	case DSHPprop_lineBottomEndArrowLength:
	    sd->sd_lineBottomEndArrowLength= val;
	    break;
	case DSHPprop_lineBottomEndArrowWidth:
	    sd->sd_lineBottomEndArrowWidth= val;
	    break;
	case DSHPprop_lineBottomEndArrowhead:
	    sd->sd_lineBottomEndArrowhead= val;
	    break;
	case DSHPprop_lineBottomEndCapStyle:
	    sd->sd_lineBottomEndCapStyle= val;
	    break;
	case DSHPprop_lineBottomFillBlipFlags:
	    sd->sd_lineBottomFillBlipFlags= val;
	    break;
	case DSHPprop_lineBottomFillDztype:
	    sd->sd_lineBottomFillDztype= val;
	    break;
	case DSHPprop_lineBottomFillHeight:
	    sd->sd_lineBottomFillHeight= val;
	    break;
	case DSHPprop_lineBottomFillWidth:
	    sd->sd_lineBottomFillWidth= val;
	    break;
	case DSHPprop_lineBottomMiterLimit:
	    sd->sd_lineBottomMiterLimit= val;
	    break;
	case DSHPprop_lineBottomOpacity:
	    sd->sd_lineBottomOpacity= val;
	    break;
	case DSHPprop_lineBottomStartArrowLength:
	    sd->sd_lineBottomStartArrowLength= val;
	    break;
	case DSHPprop_lineBottomStartArrowWidth:
	    sd->sd_lineBottomStartArrowWidth= val;
	    break;
	case DSHPprop_lineBottomStartArrowhead:
	    sd->sd_lineBottomStartArrowhead= val;
	    break;
	case DSHPprop_lineBottomStyle:
	    sd->sd_lineBottomStyle= val;
	    break;
	case DSHPprop_lineBottomType:
	    sd->sd_lineBottomType= val;
	    break;
	case DSHPprop_lineBottomWidth:
	    sd->sd_lineBottomWidth= val;
	    break;
	case DSHPprop_lineColorExt:
	    sd->sd_lineColorExt= val;
	    break;
	case DSHPprop_lineColorExtCMY:
	    sd->sd_lineColorExtCMY= val;
	    break;
	case DSHPprop_lineColorExtK:
	    sd->sd_lineColorExtK= val;
	    break;
	case DSHPprop_lineColorExtMod:
	    sd->sd_lineColorExtMod= val;
	    break;
	case DSHPprop_lineColumnBackColor:
	    sd->sd_lineColumnBackColor= val;
	    break;
	case DSHPprop_lineColumnBackColorExt:
	    sd->sd_lineColumnBackColorExt= val;
	    break;
	case DSHPprop_lineColumnBackColorExtCMY:
	    sd->sd_lineColumnBackColorExtCMY= val;
	    break;
	case DSHPprop_lineColumnBackColorExtK:
	    sd->sd_lineColumnBackColorExtK= val;
	    break;
	case DSHPprop_lineColumnBackColorExtMod:
	    sd->sd_lineColumnBackColorExtMod= val;
	    break;
	case DSHPprop_lineColumnColor:
	    sd->sd_lineColumnColor= val;
	    break;
	case DSHPprop_lineColumnColorExt:
	    sd->sd_lineColumnColorExt= val;
	    break;
	case DSHPprop_lineColumnColorExtCMY:
	    sd->sd_lineColumnColorExtCMY= val;
	    break;
	case DSHPprop_lineColumnColorExtK:
	    sd->sd_lineColumnColorExtK= val;
	    break;
	case DSHPprop_lineColumnColorExtMod:
	    sd->sd_lineColumnColorExtMod= val;
	    break;
	case DSHPprop_lineColumnCrMod:
	    sd->sd_lineColumnCrMod= val;
	    break;
	case DSHPprop_lineColumnDashing:
	    sd->sd_lineColumnDashing= val;
	    break;
	case DSHPprop_lineColumnEndArrowLength:
	    sd->sd_lineColumnEndArrowLength= val;
	    break;
	case DSHPprop_lineColumnEndArrowWidth:
	    sd->sd_lineColumnEndArrowWidth= val;
	    break;
	case DSHPprop_lineColumnEndArrowhead:
	    sd->sd_lineColumnEndArrowhead= val;
	    break;
	case DSHPprop_lineColumnEndCapStyle:
	    sd->sd_lineColumnEndCapStyle= val;
	    break;
	case DSHPprop_lineColumnFillBlipFlags:
	    sd->sd_lineColumnFillBlipFlags= val;
	    break;
	case DSHPprop_lineColumnFillDztype:
	    sd->sd_lineColumnFillDztype= val;
	    break;
	case DSHPprop_lineColumnFillHeight:
	    sd->sd_lineColumnFillHeight= val;
	    break;
	case DSHPprop_lineColumnFillWidth:
	    sd->sd_lineColumnFillWidth= val;
	    break;
	case DSHPprop_lineColumnMiterLimit:
	    sd->sd_lineColumnMiterLimit= val;
	    break;
	case DSHPprop_lineColumnOpacity:
	    sd->sd_lineColumnOpacity= val;
	    break;
	case DSHPprop_lineColumnStartArrowLength:
	    sd->sd_lineColumnStartArrowLength= val;
	    break;
	case DSHPprop_lineColumnStartArrowWidth:
	    sd->sd_lineColumnStartArrowWidth= val;
	    break;
	case DSHPprop_lineColumnStartArrowhead:
	    sd->sd_lineColumnStartArrowhead= val;
	    break;
	case DSHPprop_lineColumnStyle:
	    sd->sd_lineColumnStyle= val;
	    break;
	case DSHPprop_lineColumnType:
	    sd->sd_lineColumnType= val;
	    break;
	case DSHPprop_lineColumnWidth:
	    sd->sd_lineColumnWidth= val;
	    break;
	case DSHPprop_lineLeftBackColor:
	    sd->sd_lineLeftBackColor= val;
	    break;
	case DSHPprop_lineLeftBackColorExt:
	    sd->sd_lineLeftBackColorExt= val;
	    break;
	case DSHPprop_lineLeftBackColorExtCMY:
	    sd->sd_lineLeftBackColorExtCMY= val;
	    break;
	case DSHPprop_lineLeftBackColorExtK:
	    sd->sd_lineLeftBackColorExtK= val;
	    break;
	case DSHPprop_lineLeftBackColorExtMod:
	    sd->sd_lineLeftBackColorExtMod= val;
	    break;
	case DSHPprop_lineLeftColor:
	    sd->sd_lineLeftColor= val;
	    break;
	case DSHPprop_lineLeftColorExt:
	    sd->sd_lineLeftColorExt= val;
	    break;
	case DSHPprop_lineLeftColorExtCMY:
	    sd->sd_lineLeftColorExtCMY= val;
	    break;
	case DSHPprop_lineLeftColorExtK:
	    sd->sd_lineLeftColorExtK= val;
	    break;
	case DSHPprop_lineLeftColorExtMod:
	    sd->sd_lineLeftColorExtMod= val;
	    break;
	case DSHPprop_lineLeftCrMod:
	    sd->sd_lineLeftCrMod= val;
	    break;
	case DSHPprop_lineLeftDashing:
	    sd->sd_lineLeftDashing= val;
	    break;
	case DSHPprop_lineLeftEndArrowLength:
	    sd->sd_lineLeftEndArrowLength= val;
	    break;
	case DSHPprop_lineLeftEndArrowWidth:
	    sd->sd_lineLeftEndArrowWidth= val;
	    break;
	case DSHPprop_lineLeftEndArrowhead:
	    sd->sd_lineLeftEndArrowhead= val;
	    break;
	case DSHPprop_lineLeftEndCapStyle:
	    sd->sd_lineLeftEndCapStyle= val;
	    break;
	case DSHPprop_lineLeftFillBlipFlags:
	    sd->sd_lineLeftFillBlipFlags= val;
	    break;
	case DSHPprop_lineLeftFillDztype:
	    sd->sd_lineLeftFillDztype= val;
	    break;
	case DSHPprop_lineLeftFillHeight:
	    sd->sd_lineLeftFillHeight= val;
	    break;
	case DSHPprop_lineLeftFillWidth:
	    sd->sd_lineLeftFillWidth= val;
	    break;
	case DSHPprop_lineLeftMiterLimit:
	    sd->sd_lineLeftMiterLimit= val;
	    break;
	case DSHPprop_lineLeftOpacity:
	    sd->sd_lineLeftOpacity= val;
	    break;
	case DSHPprop_lineLeftStartArrowLength:
	    sd->sd_lineLeftStartArrowLength= val;
	    break;
	case DSHPprop_lineLeftStartArrowWidth:
	    sd->sd_lineLeftStartArrowWidth= val;
	    break;
	case DSHPprop_lineLeftStartArrowhead:
	    sd->sd_lineLeftStartArrowhead= val;
	    break;
	case DSHPprop_lineLeftStyle:
	    sd->sd_lineLeftStyle= val;
	    break;
	case DSHPprop_lineLeftType:
	    sd->sd_lineLeftType= val;
	    break;
	case DSHPprop_lineLeftWidth:
	    sd->sd_lineLeftWidth= val;
	    break;
	case DSHPprop_lineRightBackColor:
	    sd->sd_lineRightBackColor= val;
	    break;
	case DSHPprop_lineRightBackColorExt:
	    sd->sd_lineRightBackColorExt= val;
	    break;
	case DSHPprop_lineRightBackColorExtCMY:
	    sd->sd_lineRightBackColorExtCMY= val;
	    break;
	case DSHPprop_lineRightBackColorExtK:
	    sd->sd_lineRightBackColorExtK= val;
	    break;
	case DSHPprop_lineRightBackColorExtMod:
	    sd->sd_lineRightBackColorExtMod= val;
	    break;
	case DSHPprop_lineRightColor:
	    sd->sd_lineRightColor= val;
	    break;
	case DSHPprop_lineRightColorExt:
	    sd->sd_lineRightColorExt= val;
	    break;
	case DSHPprop_lineRightColorExtCMY:
	    sd->sd_lineRightColorExtCMY= val;
	    break;
	case DSHPprop_lineRightColorExtK:
	    sd->sd_lineRightColorExtK= val;
	    break;
	case DSHPprop_lineRightColorExtMod:
	    sd->sd_lineRightColorExtMod= val;
	    break;
	case DSHPprop_lineRightCrMod:
	    sd->sd_lineRightCrMod= val;
	    break;
	case DSHPprop_lineRightDashing:
	    sd->sd_lineRightDashing= val;
	    break;
	case DSHPprop_lineRightEndArrowLength:
	    sd->sd_lineRightEndArrowLength= val;
	    break;
	case DSHPprop_lineRightEndArrowWidth:
	    sd->sd_lineRightEndArrowWidth= val;
	    break;
	case DSHPprop_lineRightEndArrowhead:
	    sd->sd_lineRightEndArrowhead= val;
	    break;
	case DSHPprop_lineRightEndCapStyle:
	    sd->sd_lineRightEndCapStyle= val;
	    break;
	case DSHPprop_lineRightFillBlipFlags:
	    sd->sd_lineRightFillBlipFlags= val;
	    break;
	case DSHPprop_lineRightFillDztype:
	    sd->sd_lineRightFillDztype= val;
	    break;
	case DSHPprop_lineRightFillHeight:
	    sd->sd_lineRightFillHeight= val;
	    break;
	case DSHPprop_lineRightFillWidth:
	    sd->sd_lineRightFillWidth= val;
	    break;
	case DSHPprop_lineRightMiterLimit:
	    sd->sd_lineRightMiterLimit= val;
	    break;
	case DSHPprop_lineRightOpacity:
	    sd->sd_lineRightOpacity= val;
	    break;
	case DSHPprop_lineRightStartArrowLength:
	    sd->sd_lineRightStartArrowLength= val;
	    break;
	case DSHPprop_lineRightStartArrowWidth:
	    sd->sd_lineRightStartArrowWidth= val;
	    break;
	case DSHPprop_lineRightStartArrowhead:
	    sd->sd_lineRightStartArrowhead= val;
	    break;
	case DSHPprop_lineRightStyle:
	    sd->sd_lineRightStyle= val;
	    break;
	case DSHPprop_lineRightType:
	    sd->sd_lineRightType= val;
	    break;
	case DSHPprop_lineRightWidth:
	    sd->sd_lineRightWidth= val;
	    break;
	case DSHPprop_lineTopBackColor:
	    sd->sd_lineTopBackColor= val;
	    break;
	case DSHPprop_lineTopBackColorExt:
	    sd->sd_lineTopBackColorExt= val;
	    break;
	case DSHPprop_lineTopBackColorExtCMY:
	    sd->sd_lineTopBackColorExtCMY= val;
	    break;
	case DSHPprop_lineTopBackColorExtK:
	    sd->sd_lineTopBackColorExtK= val;
	    break;
	case DSHPprop_lineTopBackColorExtMod:
	    sd->sd_lineTopBackColorExtMod= val;
	    break;
	case DSHPprop_lineTopColor:
	    sd->sd_lineTopColor= val;
	    break;
	case DSHPprop_lineTopColorExt:
	    sd->sd_lineTopColorExt= val;
	    break;
	case DSHPprop_lineTopColorExtCMY:
	    sd->sd_lineTopColorExtCMY= val;
	    break;
	case DSHPprop_lineTopColorExtK:
	    sd->sd_lineTopColorExtK= val;
	    break;
	case DSHPprop_lineTopColorExtMod:
	    sd->sd_lineTopColorExtMod= val;
	    break;
	case DSHPprop_lineTopCrMod:
	    sd->sd_lineTopCrMod= val;
	    break;
	case DSHPprop_lineTopDashing:
	    sd->sd_lineTopDashing= val;
	    break;
	case DSHPprop_lineTopEndArrowLength:
	    sd->sd_lineTopEndArrowLength= val;
	    break;
	case DSHPprop_lineTopEndArrowWidth:
	    sd->sd_lineTopEndArrowWidth= val;
	    break;
	case DSHPprop_lineTopEndArrowhead:
	    sd->sd_lineTopEndArrowhead= val;
	    break;
	case DSHPprop_lineTopEndCapStyle:
	    sd->sd_lineTopEndCapStyle= val;
	    break;
	case DSHPprop_lineTopFillBlipFlags:
	    sd->sd_lineTopFillBlipFlags= val;
	    break;
	case DSHPprop_lineTopFillDztype:
	    sd->sd_lineTopFillDztype= val;
	    break;
	case DSHPprop_lineTopFillHeight:
	    sd->sd_lineTopFillHeight= val;
	    break;
	case DSHPprop_lineTopFillWidth:
	    sd->sd_lineTopFillWidth= val;
	    break;
	case DSHPprop_lineTopMiterLimit:
	    sd->sd_lineTopMiterLimit= val;
	    break;
	case DSHPprop_lineTopOpacity:
	    sd->sd_lineTopOpacity= val;
	    break;
	case DSHPprop_lineTopStartArrowLength:
	    sd->sd_lineTopStartArrowLength= val;
	    break;
	case DSHPprop_lineTopStartArrowWidth:
	    sd->sd_lineTopStartArrowWidth= val;
	    break;
	case DSHPprop_lineTopStartArrowhead:
	    sd->sd_lineTopStartArrowhead= val;
	    break;
	case DSHPprop_lineTopStyle:
	    sd->sd_lineTopStyle= val;
	    break;
	case DSHPprop_lineTopType:
	    sd->sd_lineTopType= val;
	    break;
	case DSHPprop_lineTopWidth:
	    sd->sd_lineTopWidth= val;
	    break;

	case DSHPprop_relLeft:
	    sd->sdRelRect.drX0= val;
	    break;
	case DSHPprop_relTop:
	    sd->sdRelRect.drY0= val;
	    break;
	case DSHPprop_relRight:
	    sd->sdRelRect.drX1= val;
	    break;
	case DSHPprop_relBottom:
	    sd->sdRelRect.drY1= val;
	    break;

	case DSHPprop_relRotation:
	    sd->sdRelRotation= val;
	    break;
	case DSHPprop_lidRegroup:
	    sd->sdRegroupID= val;
	    break;

	case DSHPprop_groupLeft:
	    sd->sdGroupRect.drX0= val;
	    break;
	case DSHPprop_groupTop:
	    sd->sdGroupRect.drY0= val;
	    break;
	case DSHPprop_groupRight:
	    sd->sdGroupRect.drX1= val;
	    break;
	case DSHPprop_groupBottom:
	    sd->sdGroupRect.drY1= val;
	    break;

	case DSHPprop_fRelChangePage:
	    sd->sd_fRelChangePage= val != 0;
	    break;
	case DSHPprop_fRelFlipH:
	    sd->sd_fRelFlipH= val != 0;
	    break;
	case DSHPprop_fRelFlipV:
	    sd->sd_fRelFlipV= val != 0;
	    break;

	case DSHPprop_shadowType:
	    sd->sdShadowType= val;
	    break;
	case DSHPprop_shadowOpacity:
	    sd->sdShadowOpacity= val;
	    break;

	case DSHPprop_shadowOffsetX:
	    sd->sdShadowOffsetXEmu= val;
	    break;
	case DSHPprop_shadowOffsetY:
	    sd->sdShadowOffsetYEmu= val;
	    break;

	case DSHPprop_shadowSecondOffsetX:
	    sd->sdShadowOffset2XEmu= val;
	    break;
	case DSHPprop_shadowSecondOffsetY:
	    sd->sdShadowOffset2YEmu= val;
	    break;

	case DSHPprop_shadowScaleXToX:
	    sd->sdShadowScaleXToX= val;
	    break;
	case DSHPprop_shadowScaleYToX:
	    sd->sdShadowScaleYToX= val;
	    break;
	case DSHPprop_shadowScaleXToY:
	    sd->sdShadowScaleXToY= val;
	    break;
	case DSHPprop_shadowScaleYToY:
	    sd->sdShadowScaleYToY= val;
	    break;

	case DSHPprop_shadowPerspectiveX:
	    sd->sdShadowPerspectiveX= val;
	    break;
	case DSHPprop_shadowPerspectiveY:
	    sd->sdShadowPerspectiveY= val;
	    break;

	case DSHPprop_shadowWeight:
	    sd->sdShadowWeight= val;
	    break;

	case DSHPprop_shadowOriginX:
	    sd->sdShadowOriginX= val;
	    break;
	case DSHPprop_shadowOriginY:
	    sd->sdShadowOriginY= val;
	    break;

	case DSHPprop_shadowColorExt:
	    sd->sd_shadowColorExt= val;
	    break;
	case DSHPprop_shadowColorExtCMY:
	    sd->sd_shadowColorExtCMY= val;
	    break;
	case DSHPprop_shadowColorExtK:
	    sd->sd_shadowColorExtK= val;
	    break;
	case DSHPprop_shadowColorExtMod:
	    sd->sd_shadowColorExtMod= val;
	    break;
	case DSHPprop_shadowHighlightExt:
	    sd->sd_shadowHighlightExt= val;
	    break;
	case DSHPprop_shadowHighlightExtCMY:
	    sd->sd_shadowHighlightExtCMY= val;
	    break;
	case DSHPprop_shadowHighlightExtK:
	    sd->sd_shadowHighlightExtK= val;
	    break;
	case DSHPprop_shadowHighlightExtMod:
	    sd->sd_shadowHighlightExtMod= val;
	    break;

	case DSHPprop_fShadow:
	    sd->sd_fShadow= val != 0;
	    break;
	case DSHPprop_fshadowObscured:
	    sd->sd_fshadowObscured= val != 0;
	    break;
	case DSHPprop_fShadowOK:
	    sd->sd_fshadowOK= val != 0;
	    break;

	case DSHPprop_cxk:
	    sd->sdConnectionSite= val;
	    break;
	case DSHPprop_cxstyle:
	    sd->sdConnectionStyle= val;
	    break;

	case DSHPprop_gtextAlign:
	    sd->sd_gtextAlign= val;
	    break;
	case DSHPprop_fGtext:
	    sd->sd_fGtext= val != 0;
	    break;
	case DSHPprop_gtextFVertical:
	    sd->sd_gtextFVertical= val != 0;
	    break;
	case DSHPprop_gtextFKern:
	    sd->sd_gtextFKern= val != 0;
	    break;
	case DSHPprop_gtextFTight:
	    sd->sd_gtextFTight= val != 0;
	    break;
	case DSHPprop_gtextFStretch:
	    sd->sd_gtextFStretch= val != 0;
	    break;
	case DSHPprop_gtextFShrinkFit:
	    sd->sd_gtextFShrinkFit= val != 0;
	    break;
	case DSHPprop_gtextFBestFit:
	    sd->sd_gtextFBestFit= val != 0;
	    break;
	case DSHPprop_gtextFNormalize:
	    sd->sd_gtextFNormalize= val != 0;
	    break;
	case DSHPprop_gtextFDxMeasure:
	    sd->sd_gtextFDxMeasure= val != 0;
	    break;
	case DSHPprop_gtextFBold:
	    sd->sd_gtextFBold= val != 0;
	    break;
	case DSHPprop_gtextFItalic:
	    sd->sd_gtextFItalic= val != 0;
	    break;
	case DSHPprop_gtextFUnderline:
	    sd->sd_gtextFUnderline= val != 0;
	    break;
	case DSHPprop_gtextFShadow:
	    sd->sd_gtextFShadow= val != 0;
	    break;
	case DSHPprop_gtextFSmallcaps:
	    sd->sd_gtextFSmallcaps= val != 0;
	    break;
	case DSHPprop_gtextFStrikethrough:
	    sd->sd_gtextFStrikethrough= val != 0;
	    break;

	case DSHPprop_geoLeft:
	    sd->sdGeoRect.drX0= val;
	    break;
	case DSHPprop_geoTop:
	    sd->sdGeoRect.drY0= val;
	    break;
	case DSHPprop_geoRight:
	    sd->sdGeoRect.drX1= val;
	    break;
	case DSHPprop_geoBottom:
	    sd->sdGeoRect.drY1= val;
	    break;

	case DSHPprop_adjustValue:
	    sd->sdAdjustValue= val;
	    break;
	case DSHPprop_adjust2Value:
	    sd->sdAdjust2Value= val;
	    break;
	case DSHPprop_adjust3Value:
	    sd->sdAdjust3Value= val;
	    break;
	case DSHPprop_adjust4Value:
	    sd->sdAdjust4Value= val;
	    break;
	case DSHPprop_adjust5Value:
	    sd->sdAdjust5Value= val;
	    break;
	case DSHPprop_adjust6Value:
	    sd->sdAdjust6Value= val;
	    break;
	case DSHPprop_adjust7Value:
	    sd->sdAdjust7Value= val;
	    break;
	case DSHPprop_adjust8Value:
	    sd->sdAdjust8Value= val;
	    break;
	case DSHPprop_adjust9Value:
	    sd->sdAdjust9Value= val;
	    break;
	case DSHPprop_adjust10Value:
	    sd->sdAdjust10Value= val;
	    break;

	case DSHPprop_dxTextLeft:
	    sd->sd_dxTextLeft= val;
	    break;
	case DSHPprop_dyTextTop:
	    sd->sd_dyTextTop= val;
	    break;
	case DSHPprop_dxTextRight:
	    sd->sd_dxTextRight= val;
	    break;
	case DSHPprop_dyTextBottom:
	    sd->sd_dyTextBottom= val;
	    break;

	case DSHPprop_scaleText:
	    sd->sd_scaleText= val;
	    break;
	case DSHPprop_lTxid:
	    sd->sd_lTxid= val;
	    break;

	case DSHPprop_WrapText:
	    sd->sd_WrapText= val;
	    break;
	case DSHPprop_anchorText:
	    sd->sd_anchorText= val;
	    break;
	case DSHPprop_txflTextFlow:
	    sd->sd_txflTextFlow= val;
	    break;
	case DSHPprop_cdirFont:
	    sd->sd_cdirFont= val;
	    break;

	case DSHPprop_fAutoTextMargin:
	    sd->sd_fAutoTextMargin= val != 0;
	    break;
	case DSHPprop_fRotateText:
	    sd->sd_fRotateText= val != 0;
	    break;
	case DSHPprop_fSelectText:
	    sd->sd_fSelectText= val != 0;
	    break;

	case DSHPprop_fFitShapeToText:
	    sd->sd_fFitShapeToText= val != 0;
	    break;
	case DSHPprop_fFitTextToShape:
	    sd->sd_fFitTextToShape= val != 0;
	    break;

	case DSHPprop_shapePath:
	    sd->sd_shapePath= val;
	    break;

	case DSHPprop_pictureActive:
	    sd->sd_pictureActive= val;
	    break;
	case DSHPprop_pictureId:
	    sd->sd_pictureId= val;
	    break;
	case DSHPprop_pictureContrast:
	    sd->sd_pictureContrast= val;
	    break;
	case DSHPprop_pictureBrightness:
	    sd->sd_pictureBrightness= val;
	    break;
	case DSHPprop_pictureDblCrMod:
	    sd->sd_pictureDblCrMod= val;
	    break;
	case DSHPprop_pictureFillCrMod:
	    sd->sd_pictureFillCrMod= val;
	    break;
	case DSHPprop_pictureLineCrMod:
	    sd->sd_pictureLineCrMod= val;
	    break;

	case DSHPprop_pibFlags:
	    sd->sd_pibFlags= val;
	    break;
	case DSHPprop_dhgt:
	    sd->sd_dhgt= val;
	    break;
	case DSHPprop_sizerelh:
	    sd->sd_sizerelh= val;
	    break;
	case DSHPprop_sizerelv:
	    sd->sd_sizerelv= val;
	    break;
	case DSHPprop_pctHoriz:
	    sd->sd_pctHoriz= val;
	    break;
	case DSHPprop_pictureGray:
	    sd->sd_pictureGray= val;
	    break;
	case DSHPprop_pictureBiLevel:
	    sd->sd_pictureBiLevel= val;
	    break;
	case DSHPprop_fPreferRelativeResize:
	    sd->sd_fPreferRelativeResize= val;
	    break;
	case DSHPprop_fScriptAnchor:
	    sd->sd_fScriptAnchor= val;
	    break;
	case DSHPprop_fFakeMaster:
	    sd->sd_fFakeMaster= val;
	    break;
	case DSHPprop_fCameFromImgDummy:
	    sd->sd_fCameFromImgDummy= val;
	    break;

	case DSHPprop_fRecolorFillAsPicture:
	    sd->sd_fRecolorFillAsPicture= val;
	    break;

	case DSHPprop_cropFromLeft:
	    sd->sd_cropFromLeft= val;
	    break;
	case DSHPprop_cropFromRight:
	    sd->sd_cropFromRight= val;
	    break;
	case DSHPprop_cropFromTop:
	    sd->sd_cropFromTop= val;
	    break;
	case DSHPprop_cropFromBottom:
	    sd->sd_cropFromBottom= val;
	    break;

	case DSHPprop_fillColor:
	    docSetShapeDrawingColor( &(sd->sdFillColor), val );
	    break;
	case DSHPprop_fillBackColor:
	    docSetShapeDrawingColor( &(sd->sdFillBackColor), val );
	    break;
	case DSHPprop_fillCrMod:
	    docSetShapeDrawingColor( &(sd->sdFillCrModColor), val );
	    break;

	case DSHPprop_lineColor:
	    docSetShapeDrawingColor( &(sd->sdLineColor), val );
	    break;
	case DSHPprop_lineBackColor:
	    docSetShapeDrawingColor( &(sd->sdLineBackColor), val );
	    break;
	case DSHPprop_lineCrMod:
	    docSetShapeDrawingColor( &(sd->sdLineCrModColor), val );
	    break;

	case DSHPprop_shadowColor:
	    docSetShapeDrawingColor( &(sd->sdShadowColor), val );
	    break;
	case DSHPprop_shadowHighlight:
	    docSetShapeDrawingColor( &(sd->sdShadowHighlightColor), val );
	    break;
	case DSHPprop_shadowCrMod:
	    docSetShapeDrawingColor( &(sd->sdShadowCrModColor), val );
	    break;
	case DSHPprop_c3DExtrusionColor:
	    docSetShapeDrawingColor( &(sd->sdShadowc3DExtrusionColor), val );
	    break;
	case DSHPprop_c3DCrMod:
	    docSetShapeDrawingColor( &(sd->sdShadowc3DCrModColor), val );
	    break;
	case DSHPprop_pictureTransparent:
	    docSetShapeDrawingColor( &(sd->sdPictureTransparentColor), val );
	    break;

	case DSHPprop_c3DEdgeThickness:
	    sd->sd_c3DEdgeThickness= val;
	    break;
	case DSHPprop_c3DExtrudeForward:
	    sd->sd_c3DExtrudeForward= val;
	    break;
	case DSHPprop_c3DExtrudeBackward:
	    sd->sd_c3DExtrudeBackward= val;
	    break;
	case DSHPprop_c3DRotationAxisX:
	    sd->sd_c3DRotationAxisX= val;
	    break;
	case DSHPprop_c3DRotationAxisY:
	    sd->sd_c3DRotationAxisY= val;
	    break;
	case DSHPprop_c3DRotationAxisZ:
	    sd->sd_c3DRotationAxisZ= val;
	    break;
	case DSHPprop_c3DRotationCenterZ:
	    sd->sd_c3DRotationCenterZ= val;
	    break;
	case DSHPprop_c3DXViewpoint:
	    sd->sd_c3DXViewpoint= val;
	    break;
	case DSHPprop_c3DYViewpoint:
	    sd->sd_c3DYViewpoint= val;
	    break;
	case DSHPprop_c3DZViewpoint:
	    sd->sd_c3DZViewpoint= val;
	    break;
	case DSHPprop_c3DKeyX:
	    sd->sd_c3DKeyX= val;
	    break;
	case DSHPprop_c3DKeyY:
	    sd->sd_c3DKeyY= val;
	    break;
	case DSHPprop_c3DKeyZ:
	    sd->sd_c3DKeyZ= val;
	    break;
	case DSHPprop_c3DFillX:
	    sd->sd_c3DFillX= val;
	    break;
	case DSHPprop_c3DFillY:
	    sd->sd_c3DFillY= val;
	    break;
	case DSHPprop_c3DFillZ:
	    sd->sd_c3DFillZ= val;
	    break;
	case DSHPprop_c3DAmbientIntensity:
	    sd->sd_c3DAmbientIntensity= val;
	    break;
	case DSHPprop_c3DDiffuseAmt:
	    sd->sd_c3DDiffuseAmt= val;
	    break;
	case DSHPprop_c3DExtrudePlane:
	    sd->sd_c3DExtrudePlane= val;
	    break;
	case DSHPprop_c3DFillIntensity:
	    sd->sd_c3DFillIntensity= val;
	    break;
	case DSHPprop_c3DKeyIntensity:
	    sd->sd_c3DKeyIntensity= val;
	    break;
	case DSHPprop_c3DOriginX:
	    sd->sd_c3DOriginX= val;
	    break;
	case DSHPprop_c3DOriginY:
	    sd->sd_c3DOriginY= val;
	    break;
	case DSHPprop_c3DRenderMode:
	    sd->sd_c3DRenderMode= val;
	    break;
	case DSHPprop_c3DRotationAngle:
	    sd->sd_c3DRotationAngle= val;
	    break;
	case DSHPprop_c3DRotationCenterX:
	    sd->sd_c3DRotationCenterX= val;
	    break;
	case DSHPprop_c3DRotationCenterY:
	    sd->sd_c3DRotationCenterY= val;
	    break;
	case DSHPprop_c3DShininess:
	    sd->sd_c3DShininess= val;
	    break;
	case DSHPprop_c3DSkewAmount:
	    sd->sd_c3DSkewAmount= val;
	    break;
	case DSHPprop_c3DSkewAngle:
	    sd->sd_c3DSkewAngle= val;
	    break;
	case DSHPprop_c3DSpecularAmt:
	    sd->sd_c3DSpecularAmt= val;
	    break;
	case DSHPprop_c3DTolerance:
	    sd->sd_c3DTolerance= val;
	    break;
	case DSHPprop_c3DXRotationAngle:
	    sd->sd_c3DXRotationAngle= val;
	    break;
	case DSHPprop_c3DYRotationAngle:
	    sd->sd_c3DYRotationAngle= val;
	    break;
	case DSHPprop_c3DExtrusionColorExt:
	    sd->sd_c3DExtrusionColorExt= val;
	    break;
	case DSHPprop_c3DExtrusionColorExtCMY:
	    sd->sd_c3DExtrusionColorExtCMY= val;
	    break;
	case DSHPprop_c3DExtrusionColorExtK:
	    sd->sd_c3DExtrusionColorExtK= val;
	    break;
	case DSHPprop_c3DExtrusionColorExtMod:
	    sd->sd_c3DExtrusionColorExtMod= val;
	    break;

	case DSHPprop_f3D:
	    sd->sd_f3D= val != 0;
	    break;
	case DSHPprop_fC3DRotationCenterAut:
	    sd->sd_fC3DRotationCenterAut= val != 0;
	    break;
	case DSHPprop_fc3DMetallic:
	    sd->sd_fc3DMetallic= val != 0;
	    break;
	case DSHPprop_fc3DUseExtrusionColor:
	    sd->sd_fc3DUseExtrusionColor= val != 0;
	    break;
	case DSHPprop_fc3DLightFace:
	    sd->sd_fc3DLightFace= val != 0;
	    break;
	case DSHPprop_fc3DParallel:
	    sd->sd_fc3DParallel= val != 0;
	    break;
	case DSHPprop_fc3DKeyHarsh:
	    sd->sd_fc3DKeyHarsh= val != 0;
	    break;
	case DSHPprop_fc3DFillHarsh:
	    sd->sd_fc3DFillHarsh= val != 0;
	    break;
	case DSHPprop_fc3DConstrainRotation:
	    sd->sd_fc3DConstrainRotation= val != 0;
	    break;
	case DSHPprop_fc3DRotationCenterAuto:
	    sd->sd_fc3DRotationCenterAuto= val != 0;
	    break;

	case DSHPprop_spcot:
	    sd->sd_spcot= val;
	    break;
	case DSHPprop_dxyCalloutGap:
	    sd->sd_dxyCalloutGap= val;
	    break;
	case DSHPprop_spcoa:
	    sd->sd_spcoa= val;
	    break;
	case DSHPprop_spcod:
	    sd->sd_spcod= val;
	    break;
	case DSHPprop_dxyCalloutDropSpecified:
	    sd->sd_dxyCalloutDropSpecified= val;
	    break;
	case DSHPprop_dxyCalloutLengthSpecified:
	    sd->sd_dxyCalloutLengthSpecified= val;
	    break;
	case DSHPprop_bWMode:
	    sd->sd_bWMode= val;
	    break;
	case DSHPprop_dgmt:
	    sd->sd_dgmt= val;
	    break;
	case DSHPprop_ccol:
	    sd->sd_ccol= val;
	    break;
	case DSHPprop_dzColMargin:
	    sd->sd_dzColMargin= val;
	    break;
	case DSHPprop_fillColorExt:
	    sd->sd_fillColorExt= val;
	    break;
	case DSHPprop_fillColorExtCMY:
	    sd->sd_fillColorExtCMY= val;
	    break;
	case DSHPprop_fillColorExtK:
	    sd->sd_fillColorExtK= val;
	    break;
	case DSHPprop_fillColorExtMod:
	    sd->sd_fillColorExtMod= val;
	    break;

	case DSHPprop_fStandardHR:
	    sd->sd_fStandardHR= val != 0;
	    break;
	case DSHPprop_fNoshadeHR:
	    sd->sd_fNoshadeHR= val != 0;
	    break;
	case DSHPprop_fHorizRule:
	    sd->sd_fHorizRule= val != 0;
	    break;
	case DSHPprop_fUserDrawn:
	    sd->sd_fUserDrawn= val != 0;
	    break;
	case DSHPprop_fCallout:
	    sd->sd_fCallout= val != 0;
	    break;
	case DSHPprop_fCalloutAccentBar:
	    sd->sd_fCalloutAccentBar= val != 0;
	    break;
	case DSHPprop_fCalloutTextBorder:
	    sd->sd_fCalloutTextBorder= val != 0;
	    break;
	case DSHPprop_fCalloutDropAuto:
	    sd->sd_fCalloutDropAuto= val != 0;
	    break;
	case DSHPprop_fCalloutLengthSpecified:
	    sd->sd_fCalloutLengthSpecified= val != 0;
	    break;
	case DSHPprop_fCalloutMinusX:
	    sd->sd_fCalloutMinusX= val != 0;
	    break;
	case DSHPprop_fCalloutMinusY:
	    sd->sd_fCalloutMinusY= val != 0;
	    break;
	case DSHPprop_f3DOK:
	    sd->sd_f3DOK= val != 0;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

long docGetShapeDrawingProperty(	const ShapeDrawing *	sd,
					int			prop )
    {
    switch( prop )
	{
	case DSHPprop_posh:
	    return sd->sdXPosition;
	case DSHPprop_posrelh:
	    return sd->sdXReference;
	case DSHPprop_posv:
	    return sd->sdYPosition;
	case DSHPprop_posrelv:
	    return sd->sdYReference;

	case DSHPprop_fUseShapeAnchor:
	    return sd->sd_fUseShapeAnchor;
	case DSHPprop_fLayoutInCell:
	    return sd->sd_fLayoutInCell;
	case DSHPprop_fAllowOverlap:
	    return sd->sd_fAllowOverlap;
	case DSHPprop_fChangePage:
	    return sd->sd_fChangePage;

	case DSHPprop_shapeType:
	    return sd->sdShapeType;

	case DSHPprop_rotation:
	    return sd->sdRotation;

	case DSHPprop_dxWrapDistLeft:
	    return sd->sdWrapDistLeftEmu;
	case DSHPprop_dyWrapDistTop:
	    return sd->sdWrapDistTopEmu;
	case DSHPprop_dxWrapDistRight:
	    return sd->sdWrapDistRightEmu;
	case DSHPprop_dyWrapDistBottom:
	    return sd->sdWrapDistBottomEmu;

	case DSHPprop_hspMaster:
	    return sd->sdMasterShape;
	case DSHPprop_hspNext:
	    return sd->sdNextShape;

	case DSHPprop_xLimo:
	    return sd->sdXLimo;
	case DSHPprop_yLimo:
	    return sd->sdXLimo;

	case DSHPprop_fIsBullet:
	    return sd->sd_fIsBullet;
	case DSHPprop_fFlipV:
	    return sd->sd_fFlipV;
	case DSHPprop_fFlipH:
	    return sd->sd_fFlipH;
	case DSHPprop_fBehindDocument:
	    return sd->sd_fBehindDocument;
	case DSHPprop_fIsButton:
	    return sd->sd_fIsButton;
	case DSHPprop_fHidden:
	    return sd->sd_fHidden;
	case DSHPprop_fReallyHidden:
	    return sd->sd_fReallyHidden;
	case DSHPprop_fArrowheadsOK:
	    return sd->sd_fArrowheadsOK;
	case DSHPprop_fBackground:
	    return sd->sd_fBackground;
	case DSHPprop_fDeleteAttachedObject:
	    return sd->sd_fDeleteAttachedObject;
	case DSHPprop_fEditedWrap:
	    return sd->sd_fEditedWrap;
	case DSHPprop_fHitTestFill:
	    return sd->sd_fHitTestFill;
	case DSHPprop_fHitTestLine:
	    return sd->sd_fHitTestLine;
	case DSHPprop_fInitiator:
	    return sd->sd_fInitiator;
	case DSHPprop_fNoFillHitTest:
	    return sd->sd_fNoFillHitTest;
	case DSHPprop_fNoHitTestPicture:
	    return sd->sd_fNoHitTestPicture;
	case DSHPprop_fNoLineDrawDash:
	    return sd->sd_fNoLineDrawDash;
	case DSHPprop_fOleIcon:
	    return sd->sd_fOleIcon;
	case DSHPprop_fOnDblClickNotify:
	    return sd->sd_fOnDblClickNotify;
	case DSHPprop_fOneD:
	    return sd->sd_fOneD;
	case DSHPprop_fPreferRelativeSize:
	    return sd->sd_fPreferRelativeSize;
	case DSHPprop_fPrint:
	    return sd->sd_fPrint;

	case DSHPprop_fPseudoInline:
	    return sd->sd_fPseudoInline;

	case DSHPprop_fLockRotation:
	    return sd->sd_fLockRotation;
	case DSHPprop_fLockAspectRatio:
	    return sd->sd_fLockAspectRatio;
	case DSHPprop_fLockAgainstSelect:
	    return sd->sd_fLockAgainstSelect;
	case DSHPprop_fLockCropping:
	    return sd->sd_fLockCropping;
	case DSHPprop_fLockVerticies:
	    return sd->sd_fLockVerticies;
	case DSHPprop_fLockText:
	    return sd->sd_fLockText;
	case DSHPprop_fLockAdjustHandles:
	    return sd->sd_fLockAdjustHandles;
	case DSHPprop_fLockAgainstGrouping:
	    return sd->sd_fLockAgainstGrouping;
	case DSHPprop_fLockShapeType:
	    return sd->sd_fLockShapeType;

	case DSHPprop_fLockPosition:
	    return sd->sd_fLockPosition;

	case DSHPprop_fillType:
	    return sd->sdFillType;
	case DSHPprop_fillOpacity:
	    return sd->sdFillOpacity;
	case DSHPprop_fillBackOpacity:
	    return sd->sdFillBackOpacity;

	case DSHPprop_fillblipflags:
	    return sd->sdFillblipflags;

	case DSHPprop_fillWidth:
	    return sd->sdFillWidthEmu;
	case DSHPprop_fillHeight:
	    return sd->sdFillHeightEmu;
	case DSHPprop_fillAngle:
	    return sd->sdFillAngle;
	case DSHPprop_fillFocus:
	    return sd->sdFillFocus;

	case DSHPprop_fillToLeft:
	    return sd->sdFillToLeft;
	case DSHPprop_fillToTop:
	    return sd->sdFillToTop;
	case DSHPprop_fillToRight:
	    return sd->sdFillToRight;
	case DSHPprop_fillToBottom:
	    return sd->sdFillToBottom;

	case DSHPprop_fillOriginX:
	    return sd->sdFillOriginX;
	case DSHPprop_fillOriginY:
	    return sd->sdFillOriginY;
	case DSHPprop_fillShapeOriginX:
	    return sd->sdFillShapeOriginX;
	case DSHPprop_fillShapeOriginY:
	    return sd->sdFillShapeOriginY;

	case DSHPprop_fillDztype:
	    return sd->sdFillDztype;

	case DSHPprop_fillRectLeft:
	    return sd->sdFillRectLeftEmu;
	case DSHPprop_fillRectTop:
	    return sd->sdFillRectTopEmu;
	case DSHPprop_fillRectRight:
	    return sd->sdFillRectRightEmu;
	case DSHPprop_fillRectBottom:
	    return sd->sdFillRectBottomEmu;

	case DSHPprop_fillShadePreset:
	    return sd->sdFillShadePreset;
	case DSHPprop_fillShadeType:
	    return sd->sdFillShadeType;

	case DSHPprop_fFilled:
	    return sd->sd_fFilled;
	case DSHPprop_fillShape:
	    return sd->sd_fillShape;
	case DSHPprop_fillUseRect:
	    return sd->sd_fillUseRect;
	case DSHPprop_fFillOK:
	    return sd->sd_fFillOK;
	case DSHPprop_fFillShadeShapeOK:
	    return sd->sd_fFillShadeShapeOK;

	case DSHPprop_lineType:
	    return sd->sdLineType;

	case DSHPprop_lineFillBlipFlags:
	    return sd->sdLineFillBlipFlags;

	case DSHPprop_lineFillWidth:
	    return sd->sdLineFillWidthEmu;
	case DSHPprop_lineFillHeight:
	    return sd->sdLineFillHeightEmu;
	case DSHPprop_lineWidth:
	    return sd->sdLineWidthEmu;

	case DSHPprop_lineStyle:
	    return sd->sdLineStyle;
	case DSHPprop_lineDashing:
	    return sd->sdLineDashing;
	case DSHPprop_lineDashStyle:
	    return sd->sdLineDashStyle;
	case DSHPprop_lineStartArrowhead:
	    return sd->sdLineHeadArrow.saArrowHead;
	case DSHPprop_lineEndArrowhead:
	    return sd->sdLineTailArrow.saArrowHead;
	case DSHPprop_lineStartArrowWidth:
	    return sd->sdLineHeadArrow.saArrowWidth;
	case DSHPprop_lineStartArrowLength:
	    return sd->sdLineHeadArrow.saArrowLength;
	case DSHPprop_lineEndArrowWidth:
	    return sd->sdLineTailArrow.saArrowWidth;
	case DSHPprop_lineEndArrowLength:
	    return sd->sdLineTailArrow.saArrowLength;
	case DSHPprop_lineEndCapStyle:
	    return sd->sdLineEndCapStyle;
	case DSHPprop_lineFillDztype:
	    return sd->sdLineFillDztype;
	case DSHPprop_lineJoinStyle:
	    return sd->sdLineJoinStyle;

	case DSHPprop_lineMiterLimit:
	    return sd->sdLineMiterLimit;

	case DSHPprop_fLine:
	    return sd->sd_fLine;
	case DSHPprop_fLineOK:
	    return sd->sd_fLineOK;

	case DSHPprop_fLineUseShapeAnchor:
	    return sd->sd_fLineUseShapeAnchor;
	case DSHPprop_fColumnLineOK:
	    return sd->sd_fColumnLineOK;
	case DSHPprop_fColumnLine:
	    return sd->sd_fColumnLine;
	case DSHPprop_fLeftLine:
	    return sd->sd_fLeftLine;
	case DSHPprop_fTopLine:
	    return sd->sd_fTopLine;
	case DSHPprop_fRightLine:
	    return sd->sd_fRightLine;
	case DSHPprop_fBottomLine:
	    return sd->sd_fBottomLine;

	case DSHPprop_fColumnHitTestLine:
	    return sd->sd_fColumnHitTestLine;
	case DSHPprop_fLeftHitTestLine:
	    return sd->sd_fLeftHitTestLine;
	case DSHPprop_fTopHitTestLine:
	    return sd->sd_fTopHitTestLine;
	case DSHPprop_fRightHitTestLine:
	    return sd->sd_fRightHitTestLine;
	case DSHPprop_fBottomHitTestLine:
	    return sd->sd_fBottomHitTestLine;

	case DSHPprop_lineFillShape:
	    return sd->sd_lineFillShape;
	case DSHPprop_lineColumnFillShape:
	    return sd->sd_lineColumnFillShape;
	case DSHPprop_lineLeftFillShape:
	    return sd->sd_lineLeftFillShape;
	case DSHPprop_lineTopFillShape:
	    return sd->sd_lineTopFillShape;
	case DSHPprop_lineRightFillShape:
	    return sd->sd_lineRightFillShape;
	case DSHPprop_lineBottomFillShape:
	    return sd->sd_lineBottomFillShape;

	case DSHPprop_fInsetPen:
	    return sd->sd_fInsetPen;
	case DSHPprop_fLeftInsetPen:
	    return sd->sd_fLeftInsetPen;
	case DSHPprop_fTopInsetPen:
	    return sd->sd_fTopInsetPen;
	case DSHPprop_fRightInsetPen:
	    return sd->sd_fRightInsetPen;
	case DSHPprop_fBottomInsetPen:
	    return sd->sd_fBottomInsetPen;

	case DSHPprop_lineOpacity:
	    return sd->sd_lineOpacity;
	    break;
	case DSHPprop_lineblipflags:
	    return sd->sd_lineblipflags;
	    break;

	case DSHPprop_lineBackColorExt:
	    return sd->sd_lineBackColorExt;
	case DSHPprop_lineBackColorExtCMY:
	    return sd->sd_lineBackColorExtCMY;
	case DSHPprop_lineBackColorExtK:
	    return sd->sd_lineBackColorExtK;
	case DSHPprop_lineBackColorExtMod:
	    return sd->sd_lineBackColorExtMod;
	case DSHPprop_lineBottomBackColor:
	    return sd->sd_lineBottomBackColor;
	case DSHPprop_lineBottomBackColorExt:
	    return sd->sd_lineBottomBackColorExt;
	case DSHPprop_lineBottomBackColorExtCMY:
	    return sd->sd_lineBottomBackColorExtCMY;
	case DSHPprop_lineBottomBackColorExtK:
	    return sd->sd_lineBottomBackColorExtK;
	case DSHPprop_lineBottomBackColorExtMod:
	    return sd->sd_lineBottomBackColorExtMod;
	case DSHPprop_lineBottomColor:
	    return sd->sd_lineBottomColor;
	case DSHPprop_lineBottomColorExt:
	    return sd->sd_lineBottomColorExt;
	case DSHPprop_lineBottomColorExtCMY:
	    return sd->sd_lineBottomColorExtCMY;
	case DSHPprop_lineBottomColorExtK:
	    return sd->sd_lineBottomColorExtK;
	case DSHPprop_lineBottomColorExtMod:
	    return sd->sd_lineBottomColorExtMod;
	case DSHPprop_lineBottomCrMod:
	    return sd->sd_lineBottomCrMod;
	case DSHPprop_lineBottomDashing:
	    return sd->sd_lineBottomDashing;
	case DSHPprop_lineBottomEndArrowLength:
	    return sd->sd_lineBottomEndArrowLength;
	case DSHPprop_lineBottomEndArrowWidth:
	    return sd->sd_lineBottomEndArrowWidth;
	case DSHPprop_lineBottomEndArrowhead:
	    return sd->sd_lineBottomEndArrowhead;
	case DSHPprop_lineBottomEndCapStyle:
	    return sd->sd_lineBottomEndCapStyle;
	case DSHPprop_lineBottomFillBlipFlags:
	    return sd->sd_lineBottomFillBlipFlags;
	case DSHPprop_lineBottomFillDztype:
	    return sd->sd_lineBottomFillDztype;
	case DSHPprop_lineBottomFillHeight:
	    return sd->sd_lineBottomFillHeight;
	case DSHPprop_lineBottomFillWidth:
	    return sd->sd_lineBottomFillWidth;
	case DSHPprop_lineBottomMiterLimit:
	    return sd->sd_lineBottomMiterLimit;
	case DSHPprop_lineBottomOpacity:
	    return sd->sd_lineBottomOpacity;
	case DSHPprop_lineBottomStartArrowLength:
	    return sd->sd_lineBottomStartArrowLength;
	case DSHPprop_lineBottomStartArrowWidth:
	    return sd->sd_lineBottomStartArrowWidth;
	case DSHPprop_lineBottomStartArrowhead:
	    return sd->sd_lineBottomStartArrowhead;
	case DSHPprop_lineBottomStyle:
	    return sd->sd_lineBottomStyle;
	case DSHPprop_lineBottomType:
	    return sd->sd_lineBottomType;
	case DSHPprop_lineBottomWidth:
	    return sd->sd_lineBottomWidth;
	case DSHPprop_lineColorExt:
	    return sd->sd_lineColorExt;
	case DSHPprop_lineColorExtCMY:
	    return sd->sd_lineColorExtCMY;
	case DSHPprop_lineColorExtK:
	    return sd->sd_lineColorExtK;
	case DSHPprop_lineColorExtMod:
	    return sd->sd_lineColorExtMod;
	case DSHPprop_lineColumnBackColor:
	    return sd->sd_lineColumnBackColor;
	case DSHPprop_lineColumnBackColorExt:
	    return sd->sd_lineColumnBackColorExt;
	case DSHPprop_lineColumnBackColorExtCMY:
	    return sd->sd_lineColumnBackColorExtCMY;
	case DSHPprop_lineColumnBackColorExtK:
	    return sd->sd_lineColumnBackColorExtK;
	case DSHPprop_lineColumnBackColorExtMod:
	    return sd->sd_lineColumnBackColorExtMod;
	case DSHPprop_lineColumnColor:
	    return sd->sd_lineColumnColor;
	case DSHPprop_lineColumnColorExt:
	    return sd->sd_lineColumnColorExt;
	case DSHPprop_lineColumnColorExtCMY:
	    return sd->sd_lineColumnColorExtCMY;
	case DSHPprop_lineColumnColorExtK:
	    return sd->sd_lineColumnColorExtK;
	case DSHPprop_lineColumnColorExtMod:
	    return sd->sd_lineColumnColorExtMod;
	case DSHPprop_lineColumnCrMod:
	    return sd->sd_lineColumnCrMod;
	case DSHPprop_lineColumnDashing:
	    return sd->sd_lineColumnDashing;
	case DSHPprop_lineColumnEndArrowLength:
	    return sd->sd_lineColumnEndArrowLength;
	case DSHPprop_lineColumnEndArrowWidth:
	    return sd->sd_lineColumnEndArrowWidth;
	case DSHPprop_lineColumnEndArrowhead:
	    return sd->sd_lineColumnEndArrowhead;
	case DSHPprop_lineColumnEndCapStyle:
	    return sd->sd_lineColumnEndCapStyle;
	case DSHPprop_lineColumnFillBlipFlags:
	    return sd->sd_lineColumnFillBlipFlags;
	case DSHPprop_lineColumnFillDztype:
	    return sd->sd_lineColumnFillDztype;
	case DSHPprop_lineColumnFillHeight:
	    return sd->sd_lineColumnFillHeight;
	case DSHPprop_lineColumnFillWidth:
	    return sd->sd_lineColumnFillWidth;
	case DSHPprop_lineColumnMiterLimit:
	    return sd->sd_lineColumnMiterLimit;
	case DSHPprop_lineColumnOpacity:
	    return sd->sd_lineColumnOpacity;
	case DSHPprop_lineColumnStartArrowLength:
	    return sd->sd_lineColumnStartArrowLength;
	case DSHPprop_lineColumnStartArrowWidth:
	    return sd->sd_lineColumnStartArrowWidth;
	case DSHPprop_lineColumnStartArrowhead:
	    return sd->sd_lineColumnStartArrowhead;
	case DSHPprop_lineColumnStyle:
	    return sd->sd_lineColumnStyle;
	case DSHPprop_lineColumnType:
	    return sd->sd_lineColumnType;
	case DSHPprop_lineColumnWidth:
	    return sd->sd_lineColumnWidth;
	case DSHPprop_lineLeftBackColor:
	    return sd->sd_lineLeftBackColor;
	case DSHPprop_lineLeftBackColorExt:
	    return sd->sd_lineLeftBackColorExt;
	case DSHPprop_lineLeftBackColorExtCMY:
	    return sd->sd_lineLeftBackColorExtCMY;
	case DSHPprop_lineLeftBackColorExtK:
	    return sd->sd_lineLeftBackColorExtK;
	case DSHPprop_lineLeftBackColorExtMod:
	    return sd->sd_lineLeftBackColorExtMod;
	case DSHPprop_lineLeftColor:
	    return sd->sd_lineLeftColor;
	case DSHPprop_lineLeftColorExt:
	    return sd->sd_lineLeftColorExt;
	case DSHPprop_lineLeftColorExtCMY:
	    return sd->sd_lineLeftColorExtCMY;
	case DSHPprop_lineLeftColorExtK:
	    return sd->sd_lineLeftColorExtK;
	case DSHPprop_lineLeftColorExtMod:
	    return sd->sd_lineLeftColorExtMod;
	case DSHPprop_lineLeftCrMod:
	    return sd->sd_lineLeftCrMod;
	case DSHPprop_lineLeftDashing:
	    return sd->sd_lineLeftDashing;
	case DSHPprop_lineLeftEndArrowLength:
	    return sd->sd_lineLeftEndArrowLength;
	case DSHPprop_lineLeftEndArrowWidth:
	    return sd->sd_lineLeftEndArrowWidth;
	case DSHPprop_lineLeftEndArrowhead:
	    return sd->sd_lineLeftEndArrowhead;
	case DSHPprop_lineLeftEndCapStyle:
	    return sd->sd_lineLeftEndCapStyle;
	case DSHPprop_lineLeftFillBlipFlags:
	    return sd->sd_lineLeftFillBlipFlags;
	case DSHPprop_lineLeftFillDztype:
	    return sd->sd_lineLeftFillDztype;
	case DSHPprop_lineLeftFillHeight:
	    return sd->sd_lineLeftFillHeight;
	case DSHPprop_lineLeftFillWidth:
	    return sd->sd_lineLeftFillWidth;
	case DSHPprop_lineLeftMiterLimit:
	    return sd->sd_lineLeftMiterLimit;
	case DSHPprop_lineLeftOpacity:
	    return sd->sd_lineLeftOpacity;
	case DSHPprop_lineLeftStartArrowLength:
	    return sd->sd_lineLeftStartArrowLength;
	case DSHPprop_lineLeftStartArrowWidth:
	    return sd->sd_lineLeftStartArrowWidth;
	case DSHPprop_lineLeftStartArrowhead:
	    return sd->sd_lineLeftStartArrowhead;
	case DSHPprop_lineLeftStyle:
	    return sd->sd_lineLeftStyle;
	case DSHPprop_lineLeftType:
	    return sd->sd_lineLeftType;
	case DSHPprop_lineLeftWidth:
	    return sd->sd_lineLeftWidth;
	case DSHPprop_lineRightBackColor:
	    return sd->sd_lineRightBackColor;
	case DSHPprop_lineRightBackColorExt:
	    return sd->sd_lineRightBackColorExt;
	case DSHPprop_lineRightBackColorExtCMY:
	    return sd->sd_lineRightBackColorExtCMY;
	case DSHPprop_lineRightBackColorExtK:
	    return sd->sd_lineRightBackColorExtK;
	case DSHPprop_lineRightBackColorExtMod:
	    return sd->sd_lineRightBackColorExtMod;
	case DSHPprop_lineRightColor:
	    return sd->sd_lineRightColor;
	case DSHPprop_lineRightColorExt:
	    return sd->sd_lineRightColorExt;
	case DSHPprop_lineRightColorExtCMY:
	    return sd->sd_lineRightColorExtCMY;
	case DSHPprop_lineRightColorExtK:
	    return sd->sd_lineRightColorExtK;
	case DSHPprop_lineRightColorExtMod:
	    return sd->sd_lineRightColorExtMod;
	case DSHPprop_lineRightCrMod:
	    return sd->sd_lineRightCrMod;
	case DSHPprop_lineRightDashing:
	    return sd->sd_lineRightDashing;
	case DSHPprop_lineRightEndArrowLength:
	    return sd->sd_lineRightEndArrowLength;
	case DSHPprop_lineRightEndArrowWidth:
	    return sd->sd_lineRightEndArrowWidth;
	case DSHPprop_lineRightEndArrowhead:
	    return sd->sd_lineRightEndArrowhead;
	case DSHPprop_lineRightEndCapStyle:
	    return sd->sd_lineRightEndCapStyle;
	case DSHPprop_lineRightFillBlipFlags:
	    return sd->sd_lineRightFillBlipFlags;
	case DSHPprop_lineRightFillDztype:
	    return sd->sd_lineRightFillDztype;
	case DSHPprop_lineRightFillHeight:
	    return sd->sd_lineRightFillHeight;
	case DSHPprop_lineRightFillWidth:
	    return sd->sd_lineRightFillWidth;
	case DSHPprop_lineRightMiterLimit:
	    return sd->sd_lineRightMiterLimit;
	case DSHPprop_lineRightOpacity:
	    return sd->sd_lineRightOpacity;
	case DSHPprop_lineRightStartArrowLength:
	    return sd->sd_lineRightStartArrowLength;
	case DSHPprop_lineRightStartArrowWidth:
	    return sd->sd_lineRightStartArrowWidth;
	case DSHPprop_lineRightStartArrowhead:
	    return sd->sd_lineRightStartArrowhead;
	case DSHPprop_lineRightStyle:
	    return sd->sd_lineRightStyle;
	case DSHPprop_lineRightType:
	    return sd->sd_lineRightType;
	case DSHPprop_lineRightWidth:
	    return sd->sd_lineRightWidth;
	case DSHPprop_lineTopBackColor:
	    return sd->sd_lineTopBackColor;
	case DSHPprop_lineTopBackColorExt:
	    return sd->sd_lineTopBackColorExt;
	case DSHPprop_lineTopBackColorExtCMY:
	    return sd->sd_lineTopBackColorExtCMY;
	case DSHPprop_lineTopBackColorExtK:
	    return sd->sd_lineTopBackColorExtK;
	case DSHPprop_lineTopBackColorExtMod:
	    return sd->sd_lineTopBackColorExtMod;
	case DSHPprop_lineTopColor:
	    return sd->sd_lineTopColor;
	case DSHPprop_lineTopColorExt:
	    return sd->sd_lineTopColorExt;
	case DSHPprop_lineTopColorExtCMY:
	    return sd->sd_lineTopColorExtCMY;
	case DSHPprop_lineTopColorExtK:
	    return sd->sd_lineTopColorExtK;
	case DSHPprop_lineTopColorExtMod:
	    return sd->sd_lineTopColorExtMod;
	case DSHPprop_lineTopCrMod:
	    return sd->sd_lineTopCrMod;
	case DSHPprop_lineTopDashing:
	    return sd->sd_lineTopDashing;
	case DSHPprop_lineTopEndArrowLength:
	    return sd->sd_lineTopEndArrowLength;
	case DSHPprop_lineTopEndArrowWidth:
	    return sd->sd_lineTopEndArrowWidth;
	case DSHPprop_lineTopEndArrowhead:
	    return sd->sd_lineTopEndArrowhead;
	case DSHPprop_lineTopEndCapStyle:
	    return sd->sd_lineTopEndCapStyle;
	case DSHPprop_lineTopFillBlipFlags:
	    return sd->sd_lineTopFillBlipFlags;
	case DSHPprop_lineTopFillDztype:
	    return sd->sd_lineTopFillDztype;
	case DSHPprop_lineTopFillHeight:
	    return sd->sd_lineTopFillHeight;
	case DSHPprop_lineTopFillWidth:
	    return sd->sd_lineTopFillWidth;
	case DSHPprop_lineTopMiterLimit:
	    return sd->sd_lineTopMiterLimit;
	case DSHPprop_lineTopOpacity:
	    return sd->sd_lineTopOpacity;
	case DSHPprop_lineTopStartArrowLength:
	    return sd->sd_lineTopStartArrowLength;
	case DSHPprop_lineTopStartArrowWidth:
	    return sd->sd_lineTopStartArrowWidth;
	case DSHPprop_lineTopStartArrowhead:
	    return sd->sd_lineTopStartArrowhead;
	case DSHPprop_lineTopStyle:
	    return sd->sd_lineTopStyle;
	case DSHPprop_lineTopType:
	    return sd->sd_lineTopType;
	case DSHPprop_lineTopWidth:
	    return sd->sd_lineTopWidth;
	case DSHPprop_relLeft:
	    return sd->sdRelRect.drX0;
	case DSHPprop_relTop:
	    return sd->sdRelRect.drY0;
	case DSHPprop_relRight:
	    return sd->sdRelRect.drX1;
	case DSHPprop_relBottom:
	    return sd->sdRelRect.drY1;

	case DSHPprop_relRotation:
	    return sd->sdRelRotation;
	case DSHPprop_lidRegroup:
	    return sd->sdRegroupID;

	case DSHPprop_groupLeft:
	    return sd->sdGroupRect.drX0;
	case DSHPprop_groupTop:
	    return sd->sdGroupRect.drY0;
	case DSHPprop_groupRight:
	    return sd->sdGroupRect.drX1;
	case DSHPprop_groupBottom:
	    return sd->sdGroupRect.drY1;

	case DSHPprop_fRelChangePage:
	    return sd->sd_fRelChangePage;
	case DSHPprop_fRelFlipH:
	    return sd->sd_fRelFlipH;
	case DSHPprop_fRelFlipV:
	    return sd->sd_fRelFlipV;

	case DSHPprop_shadowType:
	    return sd->sdShadowType;
	case DSHPprop_shadowOpacity:
	    return sd->sdShadowOpacity;

	case DSHPprop_shadowOffsetX:
	    return sd->sdShadowOffsetXEmu;
	case DSHPprop_shadowOffsetY:
	    return sd->sdShadowOffsetYEmu;

	case DSHPprop_shadowSecondOffsetX:
	    return sd->sdShadowOffset2XEmu;
	case DSHPprop_shadowSecondOffsetY:
	    return sd->sdShadowOffset2YEmu;

	case DSHPprop_shadowScaleXToX:
	    return sd->sdShadowScaleXToX;
	case DSHPprop_shadowScaleYToX:
	    return sd->sdShadowScaleYToX;
	case DSHPprop_shadowScaleXToY:
	    return sd->sdShadowScaleXToY;
	case DSHPprop_shadowScaleYToY:
	    return sd->sdShadowScaleYToY;

	case DSHPprop_shadowPerspectiveX:
	    return sd->sdShadowPerspectiveX;
	case DSHPprop_shadowPerspectiveY:
	    return sd->sdShadowPerspectiveY;

	case DSHPprop_shadowWeight:
	    return sd->sdShadowWeight;

	case DSHPprop_shadowOriginX:
	    return sd->sdShadowOriginX;
	case DSHPprop_shadowOriginY:
	    return sd->sdShadowOriginY;

	case DSHPprop_shadowColorExt:
	    return sd->sd_shadowColorExt;
	case DSHPprop_shadowColorExtCMY:
	    return sd->sd_shadowColorExtCMY;
	case DSHPprop_shadowColorExtK:
	    return sd->sd_shadowColorExtK;
	case DSHPprop_shadowColorExtMod:
	    return sd->sd_shadowColorExtMod;
	case DSHPprop_shadowHighlightExt:
	    return sd->sd_shadowHighlightExt;
	case DSHPprop_shadowHighlightExtCMY:
	    return sd->sd_shadowHighlightExtCMY;
	case DSHPprop_shadowHighlightExtK:
	    return sd->sd_shadowHighlightExtK;
	case DSHPprop_shadowHighlightExtMod:
	    return sd->sd_shadowHighlightExtMod;

	case DSHPprop_fShadow:
	    return sd->sd_fShadow;
	case DSHPprop_fshadowObscured:
	    return sd->sd_fshadowObscured;
	case DSHPprop_fShadowOK:
	    return sd->sd_fshadowOK;

	case DSHPprop_cxk:
	    return sd->sdConnectionSite;
	case DSHPprop_cxstyle:
	    return sd->sdConnectionStyle;

	case DSHPprop_gtextAlign:
	    return sd->sd_gtextAlign;
	    break;
	case DSHPprop_fGtext:
	    return sd->sd_fGtext;
	    break;
	case DSHPprop_gtextFVertical:
	    return sd->sd_gtextFVertical;
	    break;
	case DSHPprop_gtextFKern:
	    return sd->sd_gtextFKern;
	    break;
	case DSHPprop_gtextFTight:
	    return sd->sd_gtextFTight;
	    break;
	case DSHPprop_gtextFStretch:
	    return sd->sd_gtextFStretch;
	    break;
	case DSHPprop_gtextFShrinkFit:
	    return sd->sd_gtextFShrinkFit;
	    break;
	case DSHPprop_gtextFBestFit:
	    return sd->sd_gtextFBestFit;
	    break;
	case DSHPprop_gtextFNormalize:
	    return sd->sd_gtextFNormalize;
	    break;
	case DSHPprop_gtextFDxMeasure:
	    return sd->sd_gtextFDxMeasure;
	    break;
	case DSHPprop_gtextFBold:
	    return sd->sd_gtextFBold;
	    break;
	case DSHPprop_gtextFItalic:
	    return sd->sd_gtextFItalic;
	    break;
	case DSHPprop_gtextFUnderline:
	    return sd->sd_gtextFUnderline;
	    break;
	case DSHPprop_gtextFShadow:
	    return sd->sd_gtextFShadow;
	    break;
	case DSHPprop_gtextFSmallcaps:
	    return sd->sd_gtextFSmallcaps;
	    break;
	case DSHPprop_gtextFStrikethrough:
	    return sd->sd_gtextFStrikethrough;
	    break;

	case DSHPprop_geoLeft:
	    return sd->sdGeoRect.drX0;
	case DSHPprop_geoTop:
	    return sd->sdGeoRect.drY0;
	case DSHPprop_geoRight:
	    return sd->sdGeoRect.drX1;
	case DSHPprop_geoBottom:
	    return sd->sdGeoRect.drY1;

	case DSHPprop_adjustValue:
	    return sd->sdAdjustValue;
	case DSHPprop_adjust2Value:
	    return sd->sdAdjust2Value;
	case DSHPprop_adjust3Value:
	    return sd->sdAdjust3Value;
	case DSHPprop_adjust4Value:
	    return sd->sdAdjust4Value;
	case DSHPprop_adjust5Value:
	    return sd->sdAdjust5Value;
	case DSHPprop_adjust6Value:
	    return sd->sdAdjust6Value;
	case DSHPprop_adjust7Value:
	    return sd->sdAdjust7Value;
	case DSHPprop_adjust8Value:
	    return sd->sdAdjust8Value;
	case DSHPprop_adjust9Value:
	    return sd->sdAdjust9Value;
	case DSHPprop_adjust10Value:
	    return sd->sdAdjust10Value;

	case DSHPprop_dxTextLeft:
	    return sd->sd_dxTextLeft;
	case DSHPprop_dyTextTop:
	    return sd->sd_dyTextTop;
	case DSHPprop_dxTextRight:
	    return sd->sd_dxTextRight;
	case DSHPprop_dyTextBottom:
	    return sd->sd_dyTextBottom;

	case DSHPprop_scaleText:
	    return sd->sd_scaleText;
	case DSHPprop_lTxid:
	    return sd->sd_lTxid;

	case DSHPprop_WrapText:
	    return sd->sd_WrapText;
	case DSHPprop_anchorText:
	    return sd->sd_anchorText;
	case DSHPprop_txflTextFlow:
	    return sd->sd_txflTextFlow;
	case DSHPprop_cdirFont:
	    return sd->sd_cdirFont;

	case DSHPprop_fAutoTextMargin:
	    return sd->sd_fAutoTextMargin;
	case DSHPprop_fRotateText:
	    return sd->sd_fRotateText;
	case DSHPprop_fSelectText:
	    return sd->sd_fSelectText;

	case DSHPprop_fFitShapeToText:
	    return sd->sd_fFitShapeToText;
	case DSHPprop_fFitTextToShape:
	    return sd->sd_fFitTextToShape;

	case DSHPprop_shapePath:
	    return sd->sd_shapePath;

	case DSHPprop_pictureActive:
	    return sd->sd_pictureActive;
	case DSHPprop_pictureId:
	    return sd->sd_pictureId;
	case DSHPprop_pictureContrast:
	    return sd->sd_pictureContrast;
	case DSHPprop_pictureBrightness:
	    return sd->sd_pictureBrightness;
	case DSHPprop_pictureDblCrMod:
	    return sd->sd_pictureDblCrMod;
	case DSHPprop_pictureFillCrMod:
	    return sd->sd_pictureFillCrMod;
	case DSHPprop_pictureLineCrMod:
	    return sd->sd_pictureLineCrMod;

	case DSHPprop_pibFlags:
	    return sd->sd_pibFlags;
	case DSHPprop_dhgt:
	    return sd->sd_dhgt;
	case DSHPprop_sizerelh:
	    return sd->sd_sizerelh;
	case DSHPprop_sizerelv:
	    return sd->sd_sizerelv;
	case DSHPprop_pctHoriz:
	    return sd->sd_pctHoriz;
	case DSHPprop_pictureGray:
	    return sd->sd_pictureGray;
	case DSHPprop_pictureBiLevel:
	    return sd->sd_pictureBiLevel;
	case DSHPprop_fPreferRelativeResize:
	    return sd->sd_fPreferRelativeResize;
	case DSHPprop_fScriptAnchor:
	    return sd->sd_fScriptAnchor;
	case DSHPprop_fFakeMaster:
	    return sd->sd_fFakeMaster;
	case DSHPprop_fCameFromImgDummy:
	    return sd->sd_fCameFromImgDummy;

	case DSHPprop_fRecolorFillAsPicture:
	    return sd->sd_fRecolorFillAsPicture;

	case DSHPprop_cropFromLeft:
	    return sd->sd_cropFromLeft;
	case DSHPprop_cropFromRight:
	    return sd->sd_cropFromRight;
	case DSHPprop_cropFromTop:
	    return sd->sd_cropFromTop;
	case DSHPprop_cropFromBottom:
	    return sd->sd_cropFromBottom;

	case DSHPprop_fillColor:
	    return docGetShapeDrawingColor( &(sd->sdFillColor) );
	case DSHPprop_fillBackColor:
	    return docGetShapeDrawingColor( &(sd->sdFillBackColor) );
	case DSHPprop_fillCrMod:
	    return docGetShapeDrawingColor( &(sd->sdFillCrModColor) );

	case DSHPprop_lineColor:
	    return docGetShapeDrawingColor( &(sd->sdLineColor) );
	case DSHPprop_lineBackColor:
	    return docGetShapeDrawingColor( &(sd->sdLineBackColor) );
	case DSHPprop_lineCrMod:
	    return docGetShapeDrawingColor( &(sd->sdLineCrModColor) );

	case DSHPprop_shadowColor:
	    return docGetShapeDrawingColor( &(sd->sdShadowColor) );
	case DSHPprop_shadowHighlight:
	    return docGetShapeDrawingColor( &(sd->sdShadowHighlightColor) );
	case DSHPprop_shadowCrMod:
	    return docGetShapeDrawingColor( &(sd->sdShadowCrModColor) );
	case DSHPprop_c3DExtrusionColor:
	    return docGetShapeDrawingColor( &(sd->sdShadowc3DExtrusionColor) );
	case DSHPprop_c3DCrMod:
	    return docGetShapeDrawingColor( &(sd->sdShadowc3DCrModColor) );
	case DSHPprop_pictureTransparent:
	    return docGetShapeDrawingColor( &(sd->sdPictureTransparentColor) );

	case DSHPprop_c3DEdgeThickness:
	    return sd->sd_c3DEdgeThickness;
	case DSHPprop_c3DExtrudeForward:
	    return sd->sd_c3DExtrudeForward;
	case DSHPprop_c3DExtrudeBackward:
	    return sd->sd_c3DExtrudeBackward;
	case DSHPprop_c3DRotationAxisX:
	    return sd->sd_c3DRotationAxisX;
	case DSHPprop_c3DRotationAxisY:
	    return sd->sd_c3DRotationAxisY;
	case DSHPprop_c3DRotationAxisZ:
	    return sd->sd_c3DRotationAxisZ;
	case DSHPprop_c3DRotationCenterZ:
	    return sd->sd_c3DRotationCenterZ;
	case DSHPprop_c3DXViewpoint:
	    return sd->sd_c3DXViewpoint;
	case DSHPprop_c3DYViewpoint:
	    return sd->sd_c3DYViewpoint;
	case DSHPprop_c3DZViewpoint:
	    return sd->sd_c3DZViewpoint;
	case DSHPprop_c3DKeyX:
	    return sd->sd_c3DKeyX;
	case DSHPprop_c3DKeyY:
	    return sd->sd_c3DKeyY;
	case DSHPprop_c3DKeyZ:
	    return sd->sd_c3DKeyZ;
	case DSHPprop_c3DFillX:
	    return sd->sd_c3DFillX;
	case DSHPprop_c3DFillY:
	    return sd->sd_c3DFillY;
	case DSHPprop_c3DFillZ:
	    return sd->sd_c3DFillZ;
	case DSHPprop_c3DAmbientIntensity:
	    return sd->sd_c3DAmbientIntensity;
	case DSHPprop_c3DDiffuseAmt:
	    return sd->sd_c3DDiffuseAmt;
	case DSHPprop_c3DExtrudePlane:
	    return sd->sd_c3DExtrudePlane;
	case DSHPprop_c3DFillIntensity:
	    return sd->sd_c3DFillIntensity;
	case DSHPprop_c3DKeyIntensity:
	    return sd->sd_c3DKeyIntensity;
	case DSHPprop_c3DOriginX:
	    return sd->sd_c3DOriginX;
	case DSHPprop_c3DOriginY:
	    return sd->sd_c3DOriginY;
	case DSHPprop_c3DRenderMode:
	    return sd->sd_c3DRenderMode;
	case DSHPprop_c3DRotationAngle:
	    return sd->sd_c3DRotationAngle;
	case DSHPprop_c3DRotationCenterX:
	    return sd->sd_c3DRotationCenterX;
	case DSHPprop_c3DRotationCenterY:
	    return sd->sd_c3DRotationCenterY;
	case DSHPprop_c3DShininess:
	    return sd->sd_c3DShininess;
	case DSHPprop_c3DSkewAmount:
	    return sd->sd_c3DSkewAmount;
	case DSHPprop_c3DSkewAngle:
	    return sd->sd_c3DSkewAngle;
	case DSHPprop_c3DSpecularAmt:
	    return sd->sd_c3DSpecularAmt;
	case DSHPprop_c3DTolerance:
	    return sd->sd_c3DTolerance;
	case DSHPprop_c3DXRotationAngle:
	    return sd->sd_c3DXRotationAngle;
	case DSHPprop_c3DYRotationAngle:
	    return sd->sd_c3DYRotationAngle;
	case DSHPprop_c3DExtrusionColorExt:
	    return sd->sd_c3DExtrusionColorExt;
	case DSHPprop_c3DExtrusionColorExtCMY:
	    return sd->sd_c3DExtrusionColorExtCMY;
	case DSHPprop_c3DExtrusionColorExtK:
	    return sd->sd_c3DExtrusionColorExtK;
	case DSHPprop_c3DExtrusionColorExtMod:
	    return sd->sd_c3DExtrusionColorExtMod;

	case DSHPprop_f3D:
	    return sd->sd_f3D;
	case DSHPprop_fC3DRotationCenterAut:
	    return sd->sd_fC3DRotationCenterAut;
	case DSHPprop_fc3DMetallic:
	    return sd->sd_fc3DMetallic;
	case DSHPprop_fc3DUseExtrusionColor:
	    return sd->sd_fc3DUseExtrusionColor;
	case DSHPprop_fc3DLightFace:
	    return sd->sd_fc3DLightFace;
	case DSHPprop_fc3DParallel:
	    return sd->sd_fc3DParallel;
	case DSHPprop_fc3DKeyHarsh:
	    return sd->sd_fc3DKeyHarsh;
	case DSHPprop_fc3DFillHarsh:
	    return sd->sd_fc3DFillHarsh;
	case DSHPprop_fc3DConstrainRotation:
	    return sd->sd_fc3DConstrainRotation;
	case DSHPprop_fc3DRotationCenterAuto:
	    return sd->sd_fc3DRotationCenterAuto;

	case DSHPprop_spcot:
	    return sd->sd_spcot;
	case DSHPprop_dxyCalloutGap:
	    return sd->sd_dxyCalloutGap;
	case DSHPprop_spcoa:
	    return sd->sd_spcoa;
	case DSHPprop_spcod:
	    return sd->sd_spcod;
	case DSHPprop_dxyCalloutDropSpecified:
	    return sd->sd_dxyCalloutDropSpecified;
	case DSHPprop_dxyCalloutLengthSpecified:
	    return sd->sd_dxyCalloutLengthSpecified;
	case DSHPprop_bWMode:
	    return sd->sd_bWMode;
	case DSHPprop_dgmt:
	    return sd->sd_dgmt;
	case DSHPprop_ccol:
	    return sd->sd_ccol;
	case DSHPprop_dzColMargin:
	    return sd->sd_dzColMargin;
	case DSHPprop_fillColorExt:
	    return sd->sd_fillColorExt;
	case DSHPprop_fillColorExtCMY:
	    return sd->sd_fillColorExtCMY;
	case DSHPprop_fillColorExtK:
	    return sd->sd_fillColorExtK;
	case DSHPprop_fillColorExtMod:
	    return sd->sd_fillColorExtMod;

	case DSHPprop_fStandardHR:
	    return sd->sd_fStandardHR;
	case DSHPprop_fNoshadeHR:
	    return sd->sd_fNoshadeHR;
	case DSHPprop_fHorizRule:
	    return sd->sd_fHorizRule;
	case DSHPprop_fUserDrawn:
	    return sd->sd_fUserDrawn;
	case DSHPprop_fCallout:
	    return sd->sd_fCallout;
	case DSHPprop_fCalloutAccentBar:
	    return sd->sd_fCalloutAccentBar;
	case DSHPprop_fCalloutTextBorder:
	    return sd->sd_fCalloutTextBorder;
	case DSHPprop_fCalloutDropAuto:
	    return sd->sd_fCalloutDropAuto;
	case DSHPprop_fCalloutLengthSpecified:
	    return sd->sd_fCalloutLengthSpecified;
	case DSHPprop_fCalloutMinusX:
	    return sd->sd_fCalloutMinusX;
	case DSHPprop_fCalloutMinusY:
	    return sd->sd_fCalloutMinusY;
	case DSHPprop_f3DOK:
	    return sd->sd_f3DOK;

	default:
	    LDEB(prop); return -1;
	}
    }

int docSetShapeDrawingStringProperty(	ShapeDrawing *		sd,
					int			prop,
					const char *		text,
					int			size )
    {
    switch( prop )
	{
	case DSHPprop_wzName:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_wzName),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;
	case DSHPprop_wzDescription:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_wzDescription),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;
	case DSHPprop_gtextUNICODE:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_gtextUNICODE),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;
	case DSHPprop_gtextFont:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_gtextFont),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;
	case DSHPprop_pibName:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_pibName),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;
	case DSHPprop_fillBlipName:
	    if  ( utilMemoryBufferSetBytes( &(sd->sd_fillBlipName),
					(const unsigned char *)text, size ) )
		{ LLDEB(prop,size); return -1;	}
	    return 0;

	default:
	    LDEB(prop); return -1;
	}
    }
