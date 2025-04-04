# include "docRtfShapePropLookup.h"

const int docRtfShapeLookup[] =
{
       0,	/*   0:   9   0   0, "wzName" */
       1,	/*   1:   9   1   0, "wzDescription" */
       2,	/*   2:   9   2   0, "gtextUNICODE" */
       3,	/*   3:   9   3   0, "gtextFont" */
       4,	/*   4:   9   4   0, "pibName" */
       6,	/*   5:   9   5   0, "pib" */
       7,	/*   6:   9   6   0, "pVerticies" */
       8,	/*   7:   9   7   0, "pWrapPolygonVertices" */
       9,	/*   8:   9   8   0, "pSegmentInfo" */
       5,	/*   9:   9  14   0, "fillBlipName" */
      10,	/*  10:   9  17   0, "posv" */
      11,	/*  11:   9  18   0, "posh" */
      12,	/*  12:   9  19   0, "posrelv" */
      13,	/*  13:   9  20   0, "posrelh" */
      14,	/*  14:   9  21   0, "fUseShapeAnchor" */
      15,	/*  15:   9  22   0, "fLayoutInCell" */
      16,	/*  16:   9  23   0, "fAllowOverlap" */
      17,	/*  17:   9  24   0, "fChangePage" */
      18,	/*  18:   9  25   0, "shapeType" */
      20,	/*  19:   9  26   0, "rotation" */
      21,	/*  20:   9  27   0, "dxWrapDistLeft" */
      22,	/*  21:   9  28   0, "dyWrapDistTop" */
      23,	/*  22:   9  29   0, "dxWrapDistRight" */
      24,	/*  23:   9  30   0, "dyWrapDistBottom" */
      25,	/*  24:   9  31   0, "hspMaster" */
      26,	/*  25:   9  32   0, "hspNext" */
      27,	/*  26:   9  33   0, "xLimo" */
      28,	/*  27:   9  34   0, "yLimo" */
      29,	/*  28:   9  35   0, "fIsBullet" */
      30,	/*  29:   9  36   0, "fFlipV" */
      31,	/*  30:   9  37   0, "fFlipH" */
      32,	/*  31:   9  38   0, "fBehindDocument" */
      33,	/*  32:   9  39   0, "fIsButton" */
      34,	/*  33:   9  40   0, "fHidden" */
      35,	/*  34:   9  41   0, "fReallyHidden" */
      36,	/*  35:   9  42   0, "fArrowheadsOK" */
      37,	/*  36:   9  43   0, "fBackground" */
      38,	/*  37:   9  44   0, "fDeleteAttachedObject" */
      39,	/*  38:   9  45   0, "fEditedWrap" */
      40,	/*  39:   9  46   0, "fHitTestFill" */
      41,	/*  40:   9  47   0, "fHitTestLine" */
      42,	/*  41:   9  48   0, "fInitiator" */
      43,	/*  42:   9  49   0, "fNoFillHitTest" */
      44,	/*  43:   9  50   0, "fNoHitTestPicture" */
      45,	/*  44:   9  51   0, "fNoLineDrawDash" */
      46,	/*  45:   9  52   0, "fOleIcon" */
      47,	/*  46:   9  53   0, "fOnDblClickNotify" */
      48,	/*  47:   9  54   0, "fOneD" */
      49,	/*  48:   9  55   0, "fPreferRelativeSize" */
      50,	/*  49:   9  56   0, "fPrint" */
      51,	/*  50:   9  57   0, "fPseudoInline" */
      52,	/*  51:   9  58   0, "dxTextLeft" */
      53,	/*  52:   9  59   0, "dyTextTop" */
      54,	/*  53:   9  60   0, "dxTextRight" */
      55,	/*  54:   9  61   0, "dyTextBottom" */
      56,	/*  55:   9  62   0, "scaleText" */
      57,	/*  56:   9  63   0, "lTxid" */
      58,	/*  57:   9  64   0, "WrapText" */
      59,	/*  58:   9  65   0, "anchorText" */
      60,	/*  59:   9  66   0, "txflTextFlow" */
      61,	/*  60:   9  67   0, "cdirFont" */
      62,	/*  61:   9  68   0, "fAutoTextMargin" */
      63,	/*  62:   9  69   0, "fRotateText" */
      64,	/*  63:   9  70   0, "fSelectText" */
      65,	/*  64:   9  71   0, "fFitShapeToText" */
      66,	/*  65:   9  72   0, "fFitTextToShape" */
     114,	/*  66:   9  73   0, "fLockRotation" */
     115,	/*  67:   9  74   0, "fLockAspectRatio" */
     116,	/*  68:   9  75   0, "fLockAgainstSelect" */
     117,	/*  69:   9  76   0, "fLockCropping" */
     118,	/*  70:   9  77   0, "fLockVerticies" */
     119,	/*  71:   9  78   0, "fLockText" */
     120,	/*  72:   9  79   0, "fLockAdjustHandles" */
     121,	/*  73:   9  80   0, "fLockAgainstGrouping" */
     122,	/*  74:   9  81   0, "fLockShapeType" */
     123,	/*  75:   9  82   0, "fLockPosition" */
     155,	/*  76:   9  83   0, "geoLeft" */
     156,	/*  77:   9  84   0, "geoTop" */
     157,	/*  78:   9  85   0, "geoRight" */
     158,	/*  79:   9  86   0, "geoBottom" */
     159,	/*  80:   9  87   0, "adjustValue" */
     160,	/*  81:   9  88   0, "adjust2Value" */
     161,	/*  82:   9  89   0, "adjust3Value" */
     162,	/*  83:   9  90   0, "adjust4Value" */
     163,	/*  84:   9  91   0, "adjust5Value" */
     164,	/*  85:   9  92   0, "adjust6Value" */
     165,	/*  86:   9  93   0, "adjust7Value" */
     166,	/*  87:   9  94   0, "adjust8Value" */
     167,	/*  88:   9  95   0, "adjust9Value" */
     168,	/*  89:   9  96   0, "adjust10Value" */
     169,	/*  90:   9  97   0, "relLeft" */
     170,	/*  91:   9  98   0, "relTop" */
     171,	/*  92:   9  99   0, "relRight" */
     172,	/*  93:   9 100   0, "relBottom" */
     173,	/*  94:   9 101   0, "relRotation" */
     174,	/*  95:   9 102   0, "lidRegroup" */
     175,	/*  96:   9 103   0, "groupLeft" */
     176,	/*  97:   9 104   0, "groupTop" */
     177,	/*  98:   9 105   0, "groupRight" */
     178,	/*  99:   9 106   0, "groupBottom" */
     180,	/* 100:   9 107   0, "fRelFlipH" */
     181,	/* 101:   9 108   0, "fRelFlipV" */
     179,	/* 102:   9 109   0, "fRelChangePage" */
     124,	/* 103:   9 110   0, "fillType" */
     125,	/* 104:   9 111   0, "fillOpacity" */
     126,	/* 105:   9 112   0, "fillBackOpacity" */
     127,	/* 106:   9 113   0, "fillblipflags" */
     128,	/* 107:   9 114   0, "fillWidth" */
     129,	/* 108:   9 115   0, "fillHeight" */
     130,	/* 109:   9 116   0, "fillAngle" */
     131,	/* 110:   9 117   0, "fillFocus" */
     132,	/* 111:   9 118   0, "fillToLeft" */
     133,	/* 112:   9 119   0, "fillToTop" */
     134,	/* 113:   9 120   0, "fillToRight" */
     135,	/* 114:   9 121   0, "fillToBottom" */
     136,	/* 115:   9 122   0, "fillOriginX" */
     137,	/* 116:   9 123   0, "fillOriginY" */
     138,	/* 117:   9 124   0, "fillShapeOriginX" */
     139,	/* 118:   9 125   0, "fillShapeOriginY" */
     140,	/* 119:   9 126   0, "fillDztype" */
     141,	/* 120:   9 127   0, "fillRectLeft" */
     142,	/* 121:   9 128   0, "fillRectTop" */
     143,	/* 122:   9 129   0, "fillRectRight" */
     144,	/* 123:   9 130   0, "fillRectBottom" */
     145,	/* 124:   9 131   0, "fillShadePreset" */
     146,	/* 125:   9 132   0, "fillShadeType" */
     149,	/* 126:   9 133   0, "fFilled" */
     147,	/* 127:   9 134   0, "fillShape" */
     148,	/* 128:   9 135   0, "fillUseRect" */
     150,	/* 129:   9 136   0, "fFillOK" */
     151,	/* 130:   9 137   0, "fFillShadeShapeOK" */
     152,	/* 131:   9 138   0, "fillColor" */
     153,	/* 132:   9 139   0, "fillBackColor" */
     154,	/* 133:   9 140   0, "fillCrMod" */
      67,	/* 134:   9 141   0, "lineType" */
      68,	/* 135:   9 142   0, "lineFillBlipFlags" */
      69,	/* 136:   9 143   0, "lineFillWidth" */
      70,	/* 137:   9 144   0, "lineFillHeight" */
      71,	/* 138:   9 145   0, "lineWidth" */
      72,	/* 139:   9 146   0, "lineStyle" */
      73,	/* 140:   9 147   0, "lineDashing" */
      74,	/* 141:   9 148   0, "lineDashStyle" */
      75,	/* 142:   9 149   0, "lineStartArrowhead" */
      76,	/* 143:   9 150   0, "lineEndArrowhead" */
      77,	/* 144:   9 151   0, "lineStartArrowWidth" */
      78,	/* 145:   9 152   0, "lineStartArrowLength" */
      79,	/* 146:   9 153   0, "lineEndArrowWidth" */
      80,	/* 147:   9 154   0, "lineEndArrowLength" */
      81,	/* 148:   9 155   0, "lineEndCapStyle" */
      82,	/* 149:   9 156   0, "lineFillDztype" */
      83,	/* 150:   9 157   0, "lineJoinStyle" */
      84,	/* 151:   9 158   0, "lineMiterLimit" */
      85,	/* 152:   9 159   0, "fLine" */
      87,	/* 153:   9 160   0, "fLineOK" */
      88,	/* 154:   9 161   0, "fLineUseShapeAnchor" */
      89,	/* 155:   9 162   0, "fColumnLineOK" */
      90,	/* 156:   9 163   0, "fColumnLine" */
      91,	/* 157:   9 164   0, "fLeftLine" */
      92,	/* 158:   9 165   0, "fTopLine" */
      93,	/* 159:   9 166   0, "fRightLine" */
      94,	/* 160:   9 167   0, "fBottomLine" */
      95,	/* 161:   9 168   0, "fColumnHitTestLine" */
      96,	/* 162:   9 169   0, "fLeftHitTestLine" */
      97,	/* 163:   9 170   0, "fTopHitTestLine" */
      98,	/* 164:   9 171   0, "fRightHitTestLine" */
      99,	/* 165:   9 172   0, "fBottomHitTestLine" */
     100,	/* 166:   9 173   0, "lineFillShape" */
     101,	/* 167:   9 174   0, "lineColumnFillShape" */
     102,	/* 168:   9 175   0, "lineLeftFillShape" */
     103,	/* 169:   9 176   0, "lineTopFillShape" */
     104,	/* 170:   9 177   0, "lineRightFillShape" */
     105,	/* 171:   9 178   0, "lineBottomFillShape" */
     106,	/* 172:   9 179   0, "fInsetPen" */
     107,	/* 173:   9 180   0, "fLeftInsetPen" */
     108,	/* 174:   9 181   0, "fTopInsetPen" */
     109,	/* 175:   9 182   0, "fRightInsetPen" */
     110,	/* 176:   9 183   0, "fBottomInsetPen" */
     111,	/* 177:   9 184   0, "lineColor" */
     112,	/* 178:   9 185   0, "lineBackColor" */
     113,	/* 179:   9 186   0, "lineCrMod" */
     271,	/* 180:   9 187   0, "lineOpacity" */
     272,	/* 181:   9 188   0, "lineblipflags" */
     182,	/* 182:   9 189   0, "shadowType" */
     183,	/* 183:   9 190   0, "shadowOpacity" */
     184,	/* 184:   9 191   0, "shadowOffsetX" */
     185,	/* 185:   9 192   0, "shadowOffsetY" */
     186,	/* 186:   9 193   0, "shadowSecondOffsetX" */
     187,	/* 187:   9 194   0, "shadowSecondOffsetY" */
     188,	/* 188:   9 195   0, "shadowScaleXToX" */
     189,	/* 189:   9 196   0, "shadowScaleYToX" */
     190,	/* 190:   9 197   0, "shadowScaleXToY" */
     191,	/* 191:   9 198   0, "shadowScaleYToY" */
     192,	/* 192:   9 199   0, "shadowPerspectiveX" */
     193,	/* 193:   9 200   0, "shadowPerspectiveY" */
     194,	/* 194:   9 201   0, "shadowWeight" */
     195,	/* 195:   9 202   0, "shadowOriginX" */
     196,	/* 196:   9 203   0, "shadowOriginY" */
     461,	/* 197:   9 204   0, "shadowColorExt" */
     462,	/* 198:   9 205   0, "shadowColorExtCMY" */
     463,	/* 199:   9 206   0, "shadowColorExtK" */
     464,	/* 200:   9 207   0, "shadowColorExtMod" */
     465,	/* 201:   9 208   0, "shadowHighlightExt" */
     466,	/* 202:   9 209   0, "shadowHighlightExtCMY" */
     467,	/* 203:   9 210   0, "shadowHighlightExtK" */
     468,	/* 204:   9 211   0, "shadowHighlightExtMod" */
     197,	/* 205:   9 212   0, "fShadow" */
     198,	/* 206:   9 213   0, "fshadowObscured" */
     199,	/* 207:   9 214   0, "fShadowOK" */
     201,	/* 208:   9 215   0, "shadowColor" */
     202,	/* 209:   9 216   0, "shadowHighlight" */
     200,	/* 210:   9 217   0, "shadowCrMod" */
     203,	/* 211:   9 218   0, "cxk" */
     204,	/* 212:   9 219   0, "cxstyle" */
     205,	/* 213:   9 220   0, "gtextAlign" */
     206,	/* 214:   9 221   0, "fGtext" */
     207,	/* 215:   9 222   0, "gtextFVertical" */
     208,	/* 216:   9 223   0, "gtextFKern" */
     209,	/* 217:   9 224   0, "gtextFTight" */
     210,	/* 218:   9 225   0, "gtextFStretch" */
     211,	/* 219:   9 226   0, "gtextFShrinkFit" */
     212,	/* 220:   9 227   0, "gtextFBestFit" */
     213,	/* 221:   9 228   0, "gtextFNormalize" */
     214,	/* 222:   9 229   0, "gtextFDxMeasure" */
     215,	/* 223:   9 230   0, "gtextFBold" */
     216,	/* 224:   9 231   0, "gtextFItalic" */
     217,	/* 225:   9 232   0, "gtextFUnderline" */
     218,	/* 226:   9 233   0, "gtextFShadow" */
     219,	/* 227:   9 234   0, "gtextFSmallcaps" */
     220,	/* 228:   9 235   0, "gtextFStrikethrough" */
     221,	/* 229:   9 236   0, "pibFlags" */
     222,	/* 230:   9 237   0, "pictureGray" */
     223,	/* 231:   9 238   0, "pictureBiLevel" */
     224,	/* 232:   9 239   0, "c3DEdgeThickness" */
     225,	/* 233:   9 240   0, "c3DExtrudeForward" */
     226,	/* 234:   9 241   0, "c3DExtrudeBackward" */
     227,	/* 235:   9 242   0, "f3D" */
     228,	/* 236:   9 243   0, "fc3DMetallic" */
     229,	/* 237:   9 244   0, "fc3DUseExtrusionColor" */
     230,	/* 238:   9 245   0, "fc3DLightFace" */
     231,	/* 239:   9 246   0, "c3DRotationAxisX" */
     232,	/* 240:   9 247   0, "c3DRotationAxisY" */
     233,	/* 241:   9 248   0, "c3DRotationAxisZ" */
     234,	/* 242:   9 249   0, "fC3DRotationCenterAut" */
     235,	/* 243:   9 250   0, "c3DRotationCenterZ" */
     236,	/* 244:   9 251   0, "c3DXViewpoint" */
     237,	/* 245:   9 252   0, "c3DYViewpoint" */
     238,	/* 246:   9 253   0, "c3DZViewpoint" */
     239,	/* 247:   9 254   0, "c3DKeyX" */
     240,	/* 248:   9 255   0, "c3DKeyY" */
     241,	/* 249:   9 256   0, "c3DKeyZ" */
     242,	/* 250:   9 257   0, "c3DFillX" */
     243,	/* 251:   9 258   0, "c3DFillY" */
     244,	/* 252:   9 259   0, "c3DFillZ" */
     245,	/* 253:   9 260   0, "fc3DParallel" */
     246,	/* 254:   9 261   0, "fc3DKeyHarsh" */
     247,	/* 255:   9 262   0, "fc3DFillHarsh" */
     248,	/* 256:   9 263   0, "fc3DConstrainRotation" */
     249,	/* 257:   9 264   0, "fc3DRotationCenterAuto" */
     250,	/* 258:   9 265   0, "spcot" */
     251,	/* 259:   9 266   0, "dxyCalloutGap" */
     252,	/* 260:   9 267   0, "spcoa" */
     253,	/* 261:   9 268   0, "spcod" */
     254,	/* 262:   9 269   0, "dxyCalloutDropSpecified" */
     255,	/* 263:   9 270   0, "dxyCalloutLengthSpecified" */
     256,	/* 264:   9 271   0, "fCallout" */
     257,	/* 265:   9 272   0, "fCalloutAccentBar" */
     258,	/* 266:   9 273   0, "fCalloutTextBorder" */
     259,	/* 267:   9 274   0, "fCalloutDropAuto" */
     260,	/* 268:   9 275   0, "fCalloutLengthSpecified" */
     261,	/* 269:   9 276   0, "pictureId" */
     262,	/* 270:   9 277   0, "pictureActive" */
     263,	/* 271:   9 278   0, "pictureContrast" */
     264,	/* 272:   9 279   0, "pictureBrightness" */
     265,	/* 273:   9 280   0, "pictureDblCrMod" */
     266,	/* 274:   9 281   0, "pictureFillCrMod" */
     267,	/* 275:   9 282   0, "pictureLineCrMod" */
     268,	/* 276:   9 283   0, "fCalloutMinusX" */
     269,	/* 277:   9 284   0, "fCalloutMinusY" */
     270,	/* 278:   9 285   0, "shapePath" */
     273,	/* 279:   9 286   0, "cropFromLeft" */
     274,	/* 280:   9 287   0, "cropFromRight" */
     275,	/* 281:   9 288   0, "cropFromTop" */
     276,	/* 282:   9 289   0, "cropFromBottom" */
     277,	/* 283:   9 290   0, "bWMode" */
     278,	/* 284:   9 291   0, "f3DOK" */
     279,	/* 285:   9 292   0, "fPreferRelativeResize" */
     280,	/* 286:   9 293   0, "fRecolorFillAsPicture" */
     281,	/* 287:   9 294   0, "dgmt" */
     282,	/* 288:   9 295   0, "ccol" */
     283,	/* 289:   9 296   0, "dzColMargin" */
     284,	/* 290:   9 297   0, "fScriptAnchor" */
     285,	/* 291:   9 298   0, "fFakeMaster" */
     286,	/* 292:   9 299   0, "fCameFromImgDummy" */
     287,	/* 293:   9 300   0, "c3DAmbientIntensity" */
     288,	/* 294:   9 301   0, "c3DDiffuseAmt" */
     289,	/* 295:   9 302   0, "c3DExtrudePlane" */
     290,	/* 296:   9 303   0, "c3DFillIntensity" */
     291,	/* 297:   9 304   0, "c3DKeyIntensity" */
     292,	/* 298:   9 305   0, "c3DOriginX" */
     293,	/* 299:   9 306   0, "c3DOriginY" */
     294,	/* 300:   9 307   0, "c3DRenderMode" */
     295,	/* 301:   9 308   0, "c3DRotationAngle" */
     296,	/* 302:   9 309   0, "c3DRotationCenterX" */
     297,	/* 303:   9 310   0, "c3DRotationCenterY" */
     298,	/* 304:   9 311   0, "c3DShininess" */
     299,	/* 305:   9 312   0, "c3DSkewAmount" */
     300,	/* 306:   9 313   0, "c3DSkewAngle" */
     301,	/* 307:   9 314   0, "c3DSpecularAmt" */
     302,	/* 308:   9 315   0, "c3DTolerance" */
     303,	/* 309:   9 316   0, "c3DXRotationAngle" */
     304,	/* 310:   9 317   0, "c3DYRotationAngle" */
     305,	/* 311:   9 318   0, "c3DExtrusionColorExt" */
     306,	/* 312:   9 319   0, "c3DExtrusionColorExtCMY" */
     307,	/* 313:   9 320   0, "c3DExtrusionColorExtK" */
     308,	/* 314:   9 321   0, "c3DExtrusionColorExtMod" */
     309,	/* 315:   9 322   0, "fillColorExt" */
     310,	/* 316:   9 323   0, "fillColorExtCMY" */
     311,	/* 317:   9 324   0, "fillColorExtK" */
     312,	/* 318:   9 325   0, "fillColorExtMod" */
     313,	/* 319:   9 326   0, "lineBackColorExt" */
     314,	/* 320:   9 327   0, "lineBackColorExtCMY" */
     315,	/* 321:   9 328   0, "lineBackColorExtK" */
     316,	/* 322:   9 329   0, "lineBackColorExtMod" */
     317,	/* 323:   9 330   0, "lineBottomBackColor" */
     318,	/* 324:   9 331   0, "lineBottomBackColorExt" */
     319,	/* 325:   9 332   0, "lineBottomBackColorExtCMY" */
     320,	/* 326:   9 333   0, "lineBottomBackColorExtK" */
     321,	/* 327:   9 334   0, "lineBottomBackColorExtMod" */
     322,	/* 328:   9 335   0, "lineBottomColor" */
     323,	/* 329:   9 336   0, "lineBottomColorExt" */
     324,	/* 330:   9 337   0, "lineBottomColorExtCMY" */
     325,	/* 331:   9 338   0, "lineBottomColorExtK" */
     326,	/* 332:   9 339   0, "lineBottomColorExtMod" */
     327,	/* 333:   9 340   0, "lineBottomCrMod" */
     328,	/* 334:   9 341   0, "lineBottomDashing" */
     329,	/* 335:   9 342   0, "lineBottomEndArrowLength" */
     330,	/* 336:   9 343   0, "lineBottomEndArrowWidth" */
     331,	/* 337:   9 344   0, "lineBottomEndArrowhead" */
     332,	/* 338:   9 345   0, "lineBottomEndCapStyle" */
     333,	/* 339:   9 346   0, "lineBottomFillBlipFlags" */
     334,	/* 340:   9 347   0, "lineBottomFillDztype" */
     335,	/* 341:   9 348   0, "lineBottomFillHeight" */
     336,	/* 342:   9 349   0, "lineBottomFillWidth" */
     337,	/* 343:   9 350   0, "lineBottomMiterLimit" */
     338,	/* 344:   9 351   0, "lineBottomOpacity" */
     339,	/* 345:   9 352   0, "lineBottomStartArrowLength" */
     340,	/* 346:   9 353   0, "lineBottomStartArrowWidth" */
     341,	/* 347:   9 354   0, "lineBottomStartArrowhead" */
     342,	/* 348:   9 355   0, "lineBottomStyle" */
     343,	/* 349:   9 356   0, "lineBottomType" */
     344,	/* 350:   9 357   0, "lineBottomWidth" */
     345,	/* 351:   9 358   0, "lineColorExt" */
     346,	/* 352:   9 359   0, "lineColorExtCMY" */
     347,	/* 353:   9 360   0, "lineColorExtK" */
     348,	/* 354:   9 361   0, "lineColorExtMod" */
     349,	/* 355:   9 362   0, "lineColumnBackColor" */
     350,	/* 356:   9 363   0, "lineColumnBackColorExt" */
     351,	/* 357:   9 364   0, "lineColumnBackColorExtCMY" */
     352,	/* 358:   9 365   0, "lineColumnBackColorExtK" */
     353,	/* 359:   9 366   0, "lineColumnBackColorExtMod" */
     354,	/* 360:   9 367   0, "lineColumnColor" */
     355,	/* 361:   9 368   0, "lineColumnColorExt" */
     356,	/* 362:   9 369   0, "lineColumnColorExtCMY" */
     357,	/* 363:   9 370   0, "lineColumnColorExtK" */
     358,	/* 364:   9 371   0, "lineColumnColorExtMod" */
     359,	/* 365:   9 372   0, "lineColumnCrMod" */
     360,	/* 366:   9 373   0, "lineColumnDashing" */
     361,	/* 367:   9 374   0, "lineColumnEndArrowLength" */
     362,	/* 368:   9 375   0, "lineColumnEndArrowWidth" */
     363,	/* 369:   9 376   0, "lineColumnEndArrowhead" */
     364,	/* 370:   9 377   0, "lineColumnEndCapStyle" */
     365,	/* 371:   9 378   0, "lineColumnFillBlipFlags" */
     366,	/* 372:   9 379   0, "lineColumnFillDztype" */
     367,	/* 373:   9 380   0, "lineColumnFillHeight" */
     368,	/* 374:   9 381   0, "lineColumnFillWidth" */
     369,	/* 375:   9 382   0, "lineColumnMiterLimit" */
     370,	/* 376:   9 383   0, "lineColumnOpacity" */
     371,	/* 377:   9 384   0, "lineColumnStartArrowLength" */
     372,	/* 378:   9 385   0, "lineColumnStartArrowWidth" */
     373,	/* 379:   9 386   0, "lineColumnStartArrowhead" */
     374,	/* 380:   9 387   0, "lineColumnStyle" */
     375,	/* 381:   9 388   0, "lineColumnType" */
     376,	/* 382:   9 389   0, "lineColumnWidth" */
     377,	/* 383:   9 390   0, "lineLeftBackColor" */
     378,	/* 384:   9 391   0, "lineLeftBackColorExt" */
     379,	/* 385:   9 392   0, "lineLeftBackColorExtCMY" */
     380,	/* 386:   9 393   0, "lineLeftBackColorExtK" */
     381,	/* 387:   9 394   0, "lineLeftBackColorExtMod" */
     382,	/* 388:   9 395   0, "lineLeftColor" */
     383,	/* 389:   9 396   0, "lineLeftColorExt" */
     384,	/* 390:   9 397   0, "lineLeftColorExtCMY" */
     385,	/* 391:   9 398   0, "lineLeftColorExtK" */
     386,	/* 392:   9 399   0, "lineLeftColorExtMod" */
     387,	/* 393:   9 400   0, "lineLeftCrMod" */
     388,	/* 394:   9 401   0, "lineLeftDashing" */
     389,	/* 395:   9 402   0, "lineLeftEndArrowLength" */
     390,	/* 396:   9 403   0, "lineLeftEndArrowWidth" */
     391,	/* 397:   9 404   0, "lineLeftEndArrowhead" */
     392,	/* 398:   9 405   0, "lineLeftEndCapStyle" */
     393,	/* 399:   9 406   0, "lineLeftFillBlipFlags" */
     394,	/* 400:   9 407   0, "lineLeftFillDztype" */
     395,	/* 401:   9 408   0, "lineLeftFillHeight" */
     396,	/* 402:   9 409   0, "lineLeftFillWidth" */
     397,	/* 403:   9 410   0, "lineLeftMiterLimit" */
     398,	/* 404:   9 411   0, "lineLeftOpacity" */
     399,	/* 405:   9 412   0, "lineLeftStartArrowLength" */
     400,	/* 406:   9 413   0, "lineLeftStartArrowWidth" */
     401,	/* 407:   9 414   0, "lineLeftStartArrowhead" */
     402,	/* 408:   9 415   0, "lineLeftStyle" */
     403,	/* 409:   9 416   0, "lineLeftType" */
     404,	/* 410:   9 417   0, "lineLeftWidth" */
     405,	/* 411:   9 418   0, "lineRightBackColor" */
     406,	/* 412:   9 419   0, "lineRightBackColorExt" */
     407,	/* 413:   9 420   0, "lineRightBackColorExtCMY" */
     408,	/* 414:   9 421   0, "lineRightBackColorExtK" */
     409,	/* 415:   9 422   0, "lineRightBackColorExtMod" */
     410,	/* 416:   9 423   0, "lineRightColor" */
     411,	/* 417:   9 424   0, "lineRightColorExt" */
     412,	/* 418:   9 425   0, "lineRightColorExtCMY" */
     413,	/* 419:   9 426   0, "lineRightColorExtK" */
     414,	/* 420:   9 427   0, "lineRightColorExtMod" */
     415,	/* 421:   9 428   0, "lineRightCrMod" */
     416,	/* 422:   9 429   0, "lineRightDashing" */
     417,	/* 423:   9 430   0, "lineRightEndArrowLength" */
     418,	/* 424:   9 431   0, "lineRightEndArrowWidth" */
     419,	/* 425:   9 432   0, "lineRightEndArrowhead" */
     420,	/* 426:   9 433   0, "lineRightEndCapStyle" */
     421,	/* 427:   9 434   0, "lineRightFillBlipFlags" */
     422,	/* 428:   9 435   0, "lineRightFillDztype" */
     423,	/* 429:   9 436   0, "lineRightFillHeight" */
     424,	/* 430:   9 437   0, "lineRightFillWidth" */
     425,	/* 431:   9 438   0, "lineRightMiterLimit" */
     426,	/* 432:   9 439   0, "lineRightOpacity" */
     427,	/* 433:   9 440   0, "lineRightStartArrowLength" */
     428,	/* 434:   9 441   0, "lineRightStartArrowWidth" */
     429,	/* 435:   9 442   0, "lineRightStartArrowhead" */
     430,	/* 436:   9 443   0, "lineRightStyle" */
     431,	/* 437:   9 444   0, "lineRightType" */
     432,	/* 438:   9 445   0, "lineRightWidth" */
     433,	/* 439:   9 446   0, "lineTopBackColor" */
     434,	/* 440:   9 447   0, "lineTopBackColorExt" */
     435,	/* 441:   9 448   0, "lineTopBackColorExtCMY" */
     436,	/* 442:   9 449   0, "lineTopBackColorExtK" */
     437,	/* 443:   9 450   0, "lineTopBackColorExtMod" */
     438,	/* 444:   9 451   0, "lineTopColor" */
     439,	/* 445:   9 452   0, "lineTopColorExt" */
     440,	/* 446:   9 453   0, "lineTopColorExtCMY" */
     441,	/* 447:   9 454   0, "lineTopColorExtK" */
     442,	/* 448:   9 455   0, "lineTopColorExtMod" */
     443,	/* 449:   9 456   0, "lineTopCrMod" */
     444,	/* 450:   9 457   0, "lineTopDashing" */
     445,	/* 451:   9 458   0, "lineTopEndArrowLength" */
     446,	/* 452:   9 459   0, "lineTopEndArrowWidth" */
     447,	/* 453:   9 460   0, "lineTopEndArrowhead" */
     448,	/* 454:   9 461   0, "lineTopEndCapStyle" */
     449,	/* 455:   9 462   0, "lineTopFillBlipFlags" */
     450,	/* 456:   9 463   0, "lineTopFillDztype" */
     451,	/* 457:   9 464   0, "lineTopFillHeight" */
     452,	/* 458:   9 465   0, "lineTopFillWidth" */
     453,	/* 459:   9 466   0, "lineTopMiterLimit" */
     454,	/* 460:   9 467   0, "lineTopOpacity" */
     455,	/* 461:   9 468   0, "lineTopStartArrowLength" */
     456,	/* 462:   9 469   0, "lineTopStartArrowWidth" */
     457,	/* 463:   9 470   0, "lineTopStartArrowhead" */
     458,	/* 464:   9 471   0, "lineTopStyle" */
     459,	/* 465:   9 472   0, "lineTopType" */
     460,	/* 466:   9 473   0, "lineTopWidth" */
     469,	/* 467:   9 474   0, "pictureTransparent" */
     470,	/* 468:   9 475   0, "c3DExtrusionColor" */
     471,	/* 469:   9 476   0, "c3DCrMod" */
     472,	/* 470:   9 477   0, "fStandardHR" */
     473,	/* 471:   9 478   0, "fNoshadeHR" */
     474,	/* 472:   9 479   0, "fHorizRule" */
     475,	/* 473:   9 480   0, "fUserDrawn" */
     476,	/* 474:   9 481   0, "dhgt" */
     477,	/* 475:   9 482   0, "sizerelh" */
     478,	/* 476:   9 483   0, "sizerelv" */
     479,	/* 477:   9 484   0, "pctHoriz" */
};

const int docRtfShapeLookupCount=\
		sizeof(docRtfShapeLookup)/sizeof(int);
