#   include <appUtilConfig.h>

#   include <utilJenkinsHash.h>

#   include "docRtfTagHash.h"

#   define ub4 UtilUint32
#   define ub2 UtilUint16
#   define ub1 unsigned char

#   define CHECKSTATE 8

/* smax= 512, blen= 256 */

/* small adjustments to _a_ to make values distinct */
static const ub2 tab[] = {
28,0,162,0,62,65,471,154,481,468,327,65,0,65,258,484,
243,404,0,258,479,0,258,230,154,28,154,28,65,0,243,36,
0,0,337,0,468,154,0,258,0,6,0,154,258,469,404,154,
146,468,174,395,243,28,154,471,36,404,484,0,6,154,469,258,
468,154,0,243,28,258,258,404,112,154,0,484,258,0,36,0,
0,468,0,4,258,0,0,243,258,0,0,65,154,0,258,0,
157,419,6,0,325,26,0,62,404,0,258,395,472,468,6,399,
36,258,0,331,258,162,154,325,445,28,154,469,264,462,0,28,
249,258,6,243,324,0,162,258,162,157,0,258,469,0,319,65,
395,0,324,155,258,337,260,65,337,258,378,162,471,0,0,469,
0,481,344,0,419,162,0,264,154,6,154,65,0,258,0,154,
154,201,258,65,324,243,258,243,324,275,28,157,28,270,146,0,
378,258,157,243,469,258,419,328,484,0,0,484,0,230,243,0,
258,243,329,0,0,258,406,162,158,345,258,382,183,0,345,243,
329,243,154,112,0,379,249,378,258,101,328,0,253,471,183,345,
258,200,0,0,174,154,162,445,365,352,154,499,378,0,0,146,
};

/* map back array */

static short mapBack[]=
    {
     349L,	/*     0: "lineColumnBackColorExtK" */
     265L,	/*     1: "pictureFillCrMod" */
     216L,	/*     2: "gtextFUnderline" */
     456L,	/*     3: "lineTopType" */
      34L,	/*     4: "fHidden" */
     269L,	/*     5: "shapePath" */
     263L,	/*     6: "pictureBrightness" */
     212L,	/*     7: "gtextFNormalize" */
     388L,	/*     8: "lineLeftEndArrowhead" */
     286L,	/*     9: "c3DExtrudePlane" */
      45L,	/*    10: "fNoLineDrawDash" */
     455L,	/*    11: "lineTopStyle" */
     359L,	/*    12: "lineColumnEndArrowWidth" */
     104L,	/*    13: "lineBottomFillShape" */
     316L,	/*    14: "lineBottomBackColorExtCMY" */
     290L,	/*    15: "c3DOriginY" */
     198L,	/*    16: "fShadowOK" */
      13L,	/*    17: "posrelh" */
     197L,	/*    18: "fshadowObscured" */
     308L,	/*    19: "fillColorExtK" */
     373L,	/*    20: "lineColumnWidth" */
     351L,	/*    21: "lineColumnColor" */
     338L,	/*    22: "lineBottomStartArrowhead" */
      88L,	/*    23: "fColumnLineOK" */
     250L,	/*    24: "dxyCalloutGap" */
     134L,	/*    25: "fillToBottom" */
      70L,	/*    26: "lineFillHeight" */
      51L,	/*    27: "fPseudoInline" */
     460L,	/*    28: "shadowColorExtK" */
     447L,	/*    29: "lineTopFillDztype" */
     258L,	/*    30: "fCalloutDropAuto" */
     173L,	/*    31: "lidRegroup" */
     389L,	/*    32: "lineLeftEndCapStyle" */
     445L,	/*    33: "lineTopEndCapStyle" */
      78L,	/*    34: "lineEndArrowWidth" */
     164L,	/*    35: "adjust7Value" */
     177L,	/*    36: "groupBottom" */
     430L,	/*    37: "lineTopBackColor" */
      52L,	/*    38: "dxTextLeft" */
     200L,	/*    39: "shadowColor" */
     424L,	/*    40: "lineRightStartArrowLength" */
      12L,	/*    41: "posrelv" */
     171L,	/*    42: "relBottom" */
     241L,	/*    43: "c3DFillX" */
     320L,	/*    44: "lineBottomColorExt" */
     170L,	/*    45: "relRight" */
     370L,	/*    46: "lineColumnStartArrowhead" */
     362L,	/*    47: "lineColumnFillBlipFlags" */
      67L,	/*    48: "lineType" */
     253L,	/*    49: "dxyCalloutDropSpecified" */
     207L,	/*    50: "gtextFKern" */
       7L,	/*    51: "pibName" */
     291L,	/*    52: "c3DRenderMode" */
     409L,	/*    53: "lineRightColorExtCMY" */
     323L,	/*    54: "lineBottomColorExtMod" */
     303L,	/*    55: "c3DExtrusionColorExtCMY" */
      25L,	/*    56: "hspMaster" */
      68L,	/*    57: "lineFillBlipFlags" */
     312L,	/*    58: "lineBackColorExtK" */
     310L,	/*    59: "lineBackColorExt" */
     225L,	/*    60: "c3DExtrudeBackward" */
     438L,	/*    61: "lineTopColorExtK" */
     391L,	/*    62: "lineLeftFillDztype" */
     330L,	/*    63: "lineBottomFillBlipFlags" */
     392L,	/*    64: "lineLeftFillHeight" */
     339L,	/*    65: "lineBottomStyle" */
       1L,	/*    66: "pWrapPolygonVertices" */
     102L,	/*    67: "lineTopFillShape" */
     159L,	/*    68: "adjust2Value" */
     355L,	/*    69: "lineColumnColorExtMod" */
      73L,	/*    70: "lineDashing" */
     206L,	/*    71: "gtextFVertical" */
     288L,	/*    72: "c3DKeyIntensity" */
     376L,	/*    73: "lineLeftBackColorExtCMY" */
     292L,	/*    74: "c3DRotationAngle" */
     262L,	/*    75: "pictureContrast" */
     191L,	/*    76: "shadowPerspectiveX" */
     299L,	/*    77: "c3DTolerance" */
      95L,	/*    78: "fLeftHitTestLine" */
     166L,	/*    79: "adjust9Value" */
     386L,	/*    80: "lineLeftEndArrowLength" */
     374L,	/*    81: "lineLeftBackColor" */
     223L,	/*    82: "c3DEdgeThickness" */
     325L,	/*    83: "lineBottomDashing" */
     248L,	/*    84: "fc3DRotationCenterAuto" */
     395L,	/*    85: "lineLeftOpacity" */
     461L,	/*    86: "shadowColorExtMod" */
     406L,	/*    87: "lineRightBackColorExtMod" */
     267L,	/*    88: "fCalloutMinusX" */
     281L,	/*    89: "dzColMargin" */
       9L,	/*    90: "pib" */
     449L,	/*    91: "lineTopFillWidth" */
     230L,	/*    92: "c3DRotationAxisX" */
     181L,	/*    93: "shadowType" */
     319L,	/*    94: "lineBottomColor" */
      27L,	/*    95: "xLimo" */
      87L,	/*    96: "fLineUseShapeAnchor" */
     249L,	/*    97: "spcot" */
      94L,	/*    98: "fColumnHitTestLine" */
     100L,	/*    99: "lineColumnFillShape" */
     147L,	/*   100: "fillShape" */
      56L,	/*   101: "scaleText" */
     345L,	/*   102: "lineColorExtMod" */
     321L,	/*   103: "lineBottomColorExtCMY" */
      63L,	/*   104: "fRotateText" */
     396L,	/*   105: "lineLeftStartArrowLength" */
     211L,	/*   106: "gtextFBestFit" */
      43L,	/*   107: "fNoFillHitTest" */
      64L,	/*   108: "fSelectText" */
     304L,	/*   109: "c3DExtrusionColorExtK" */
     180L,	/*   110: "fRelFlipV" */
     128L,	/*   111: "fillHeight" */
     434L,	/*   112: "lineTopBackColorExtMod" */
     375L,	/*   113: "lineLeftBackColorExt" */
     442L,	/*   114: "lineTopEndArrowLength" */
     331L,	/*   115: "lineBottomFillDztype" */
     313L,	/*   116: "lineBackColorExtMod" */
     358L,	/*   117: "lineColumnEndArrowLength" */
     156L,	/*   118: "geoRight" */
     246L,	/*   119: "fc3DFillHarsh" */
     353L,	/*   120: "lineColumnColorExtCMY" */
     261L,	/*   121: "pictureActive" */
     356L,	/*   122: "lineColumnCrMod" */
      41L,	/*   123: "fHitTestLine" */
      55L,	/*   124: "dyTextBottom" */
     287L,	/*   125: "c3DFillIntensity" */
     202L,	/*   126: "cxk" */
     294L,	/*   127: "c3DRotationCenterY" */
     393L,	/*   128: "lineLeftFillWidth" */
     347L,	/*   129: "lineColumnBackColorExt" */
     224L,	/*   130: "c3DExtrudeForward" */
     420L,	/*   131: "lineRightFillHeight" */
       5L,	/*   132: "gtextUNICODE" */
     293L,	/*   133: "c3DRotationCenterX" */
     273L,	/*   134: "cropFromTop" */
     399L,	/*   135: "lineLeftStyle" */
     203L,	/*   136: "cxstyle" */
      60L,	/*   137: "txflTextFlow" */
      14L,	/*   138: "fUseShapeAnchor" */
     427L,	/*   139: "lineRightStyle" */
     437L,	/*   140: "lineTopColorExtCMY" */
     184L,	/*   141: "shadowOffsetY" */
      76L,	/*   142: "lineStartArrowWidth" */
      24L,	/*   143: "dyWrapDistBottom" */
     410L,	/*   144: "lineRightColorExtK" */
     340L,	/*   145: "lineBottomType" */
     161L,	/*   146: "adjust4Value" */
     332L,	/*   147: "lineBottomFillHeight" */
     394L,	/*   148: "lineLeftMiterLimit" */
      77L,	/*   149: "lineStartArrowLength" */
     379L,	/*   150: "lineLeftColor" */
     152L,	/*   151: "fillBackColor" */
     141L,	/*   152: "fillRectTop" */
     315L,	/*   153: "lineBottomBackColorExt" */
     157L,	/*   154: "geoBottom" */
     285L,	/*   155: "c3DDiffuseAmt" */
     334L,	/*   156: "lineBottomMiterLimit" */
     435L,	/*   157: "lineTopColor" */
     444L,	/*   158: "lineTopEndArrowhead" */
     423L,	/*   159: "lineRightOpacity" */
     108L,	/*   160: "fRightInsetPen" */
     318L,	/*   161: "lineBottomBackColorExtMod" */
     416L,	/*   162: "lineRightEndArrowhead" */
     401L,	/*   163: "lineLeftWidth" */
     256L,	/*   164: "fCalloutAccentBar" */
     350L,	/*   165: "lineColumnBackColorExtMod" */
     158L,	/*   166: "adjustValue" */
     167L,	/*   167: "adjust10Value" */
      98L,	/*   168: "fBottomHitTestLine" */
      33L,	/*   169: "fIsButton" */
     123L,	/*   170: "fillType" */
     344L,	/*   171: "lineColorExtK" */
     233L,	/*   172: "fC3DRotationCenterAut" */
      81L,	/*   173: "lineFillDztype" */
     116L,	/*   174: "fLockCropping" */
     378L,	/*   175: "lineLeftBackColorExtMod" */
     407L,	/*   176: "lineRightColor" */
      36L,	/*   177: "fArrowheadsOK" */
      35L,	/*   178: "fReallyHidden" */
     209L,	/*   179: "gtextFStretch" */
     204L,	/*   180: "gtextAlign" */
     153L,	/*   181: "fillCrMod" */
      72L,	/*   182: "lineStyle" */
     150L,	/*   183: "fFillShadeShapeOK" */
     120L,	/*   184: "fLockAgainstGrouping" */
     188L,	/*   185: "shadowScaleYToX" */
     194L,	/*   186: "shadowOriginX" */
     366L,	/*   187: "lineColumnMiterLimit" */
     441L,	/*   188: "lineTopDashing" */
      32L,	/*   189: "fBehindDocument" */
     125L,	/*   190: "fillBackOpacity" */
     205L,	/*   191: "fGtext" */
      10L,	/*   192: "posv" */
      54L,	/*   193: "dxTextRight" */
     175L,	/*   194: "groupTop" */
      50L,	/*   195: "fPrint" */
      66L,	/*   196: "fFitTextToShape" */
      57L,	/*   197: "lTxid" */
     107L,	/*   198: "fTopInsetPen" */
     390L,	/*   199: "lineLeftFillBlipFlags" */
     121L,	/*   200: "fLockShapeType" */
     448L,	/*   201: "lineTopFillHeight" */
     469L,	/*   202: "fStandardHR" */
     106L,	/*   203: "fLeftInsetPen" */
     470L,	/*   204: "fNoshadeHR" */
     127L,	/*   205: "fillWidth" */
     357L,	/*   206: "lineColumnDashing" */
     227L,	/*   207: "fc3DMetallic" */
     129L,	/*   208: "fillAngle" */
     368L,	/*   209: "lineColumnStartArrowLength" */
     162L,	/*   210: "adjust5Value" */
     417L,	/*   211: "lineRightEndCapStyle" */
     451L,	/*   212: "lineTopOpacity" */
     235L,	/*   213: "c3DXViewpoint" */
     274L,	/*   214: "cropFromBottom" */
     172L,	/*   215: "relRotation" */
      71L,	/*   216: "lineWidth" */
     114L,	/*   217: "fLockAspectRatio" */
     326L,	/*   218: "lineBottomEndArrowLength" */
     126L,	/*   219: "fillblipflags" */
     195L,	/*   220: "shadowOriginY" */
     336L,	/*   221: "lineBottomStartArrowLength" */
     436L,	/*   222: "lineTopColorExt" */
     466L,	/*   223: "pictureTransparent" */
     115L,	/*   224: "fLockAgainstSelect" */
     122L,	/*   225: "fLockPosition" */
       3L,	/*   226: "wzName" */
     232L,	/*   227: "c3DRotationAxisZ" */
     222L,	/*   228: "pictureBiLevel" */
      39L,	/*   229: "fEditedWrap" */
     228L,	/*   230: "fc3DUseExtrusionColor" */
     271L,	/*   231: "cropFromLeft" */
     259L,	/*   232: "fCalloutLengthSpecified" */
     354L,	/*   233: "lineColumnColorExtK" */
      85L,	/*   234: "fline" */
     464L,	/*   235: "shadowHighlightExtK" */
     367L,	/*   236: "lineColumnOpacity" */
      44L,	/*   237: "fNoHitTestPicture" */
      79L,	/*   238: "lineEndArrowLength" */
     163L,	/*   239: "adjust6Value" */
     151L,	/*   240: "fillColor" */
     405L,	/*   241: "lineRightBackColorExtK" */
     221L,	/*   242: "pictureGray" */
     302L,	/*   243: "c3DExtrusionColorExt" */
     196L,	/*   244: "fShadow" */
     454L,	/*   245: "lineTopStartArrowhead" */
     133L,	/*   246: "fillToRight" */
     322L,	/*   247: "lineBottomColorExtK" */
     146L,	/*   248: "fFilled" */
     346L,	/*   249: "lineColumnBackColor" */
     251L,	/*   250: "spcoa" */
     140L,	/*   251: "fillRectLeft" */
     138L,	/*   252: "fillShapeOriginY" */
     179L,	/*   253: "fRelFlipH" */
     135L,	/*   254: "fillOriginX" */
     364L,	/*   255: "lineColumnFillHeight" */
      16L,	/*   256: "fAllowOverlap" */
     415L,	/*   257: "lineRightEndArrowWidth" */
     277L,	/*   258: "fPreferRelativeResize" */
     124L,	/*   259: "fillOpacity" */
     239L,	/*   260: "c3DKeyY" */
      59L,	/*   261: "anchorText" */
     419L,	/*   262: "lineRightFillDztype" */
      21L,	/*   263: "dxWrapDistLeft" */
     328L,	/*   264: "lineBottomEndArrowhead" */
     365L,	/*   265: "lineColumnFillWidth" */
      37L,	/*   266: "fBackground" */
      19L,	/*   267: "ShapeType" */
     226L,	/*   268: "f3D" */
     414L,	/*   269: "lineRightEndArrowLength" */
     279L,	/*   270: "dgmt" */
      96L,	/*   271: "fTopHitTestLine" */
     118L,	/*   272: "fLockText" */
      29L,	/*   273: "fIsBullet" */
      97L,	/*   274: "fRightHitTestLine" */
     176L,	/*   275: "groupRight" */
      61L,	/*   276: "cdirFont" */
     377L,	/*   277: "lineLeftBackColorExtK" */
     160L,	/*   278: "adjust3Value" */
      83L,	/*   279: "lineMiterLimit" */
     257L,	/*   280: "fCalloutTextBorder" */
     472L,	/*   281: "fUserDrawn" */
     272L,	/*   282: "cropFromRight" */
       8L,	/*   283: "fillBlipName" */
     381L,	/*   284: "lineLeftColorExtCMY" */
     199L,	/*   285: "shadowCrMod" */
     301L,	/*   286: "c3DYRotationAngle" */
      92L,	/*   287: "fRightLine" */
     458L,	/*   288: "shadowColorExt" */
     165L,	/*   289: "adjust8Value" */
     186L,	/*   290: "shadowSecondOffsetY" */
     266L,	/*   291: "pictureLineCrMod" */
     327L,	/*   292: "lineBottomEndArrowWidth" */
     467L,	/*   293: "c3DExtrusionColor" */
     408L,	/*   294: "lineRightColorExt" */
     309L,	/*   295: "fillColorExtMod" */
     463L,	/*   296: "shadowHighlightExtCMY" */
     260L,	/*   297: "pictureId" */
     387L,	/*   298: "lineLeftEndArrowWidth" */
      82L,	/*   299: "lineJoinStyle" */
     148L,	/*   300: "fillUseRect" */
     400L,	/*   301: "lineLeftType" */
      31L,	/*   302: "fFlipH" */
     189L,	/*   303: "shadowScaleXToY" */
     243L,	/*   304: "c3DFillZ" */
      38L,	/*   305: "fDeleteAttachedObject" */
     110L,	/*   306: "lineColor" */
      62L,	/*   307: "fAutoTextMargin" */
     361L,	/*   308: "lineColumnEndCapStyle" */
      75L,	/*   309: "lineEndArrowhead" */
      84L,	/*   310: "fLine" */
     252L,	/*   311: "spcod" */
     219L,	/*   312: "gtextFStrikethrough" */
     452L,	/*   313: "lineTopStartArrowLength" */
     193L,	/*   314: "shadowWeight" */
     136L,	/*   315: "fillOriginY" */
     139L,	/*   316: "fillDztype" */
     289L,	/*   317: "c3DOriginX" */
     264L,	/*   318: "pictureDblCrMod" */
      65L,	/*   319: "fFitShapeToText" */
     276L,	/*   320: "f3DOK" */
      53L,	/*   321: "dyTextTop" */
     190L,	/*   322: "shadowScaleYToY" */
     237L,	/*   323: "c3DZViewpoint" */
     220L,	/*   324: "pibFlags" */
     471L,	/*   325: "fHorizRule" */
     270L,	/*   326: "lineOpacity" */
     240L,	/*   327: "c3DKeyZ" */
     255L,	/*   328: "fCallout" */
     431L,	/*   329: "lineTopBackColorExt" */
      58L,	/*   330: "WrapText" */
     137L,	/*   331: "fillShapeOriginX" */
     317L,	/*   332: "lineBottomBackColorExtK" */
     275L,	/*   333: "bWMode" */
      91L,	/*   334: "fTopLine" */
     131L,	/*   335: "fillToLeft" */
      17L,	/*   336: "fChangePage" */
     103L,	/*   337: "lineRightFillShape" */
     208L,	/*   338: "gtextFTight" */
     119L,	/*   339: "fLockAdjustHandles" */
       0L,	/*   340: "pVerticies" */
     439L,	/*   341: "lineTopColorExtMod" */
     422L,	/*   342: "lineRightMiterLimit" */
     144L,	/*   343: "fillShadePreset" */
      42L,	/*   344: "fInitiator" */
     337L,	/*   345: "lineBottomStartArrowWidth" */
      30L,	/*   346: "fFlipV" */
     210L,	/*   347: "gtextFShrinkFit" */
     268L,	/*   348: "fCalloutMinusY" */
      15L,	/*   349: "fLayoutInCell" */
     242L,	/*   350: "c3DFillY" */
     432L,	/*   351: "lineTopBackColorExtCMY" */
      46L,	/*   352: "fOleIcon" */
     278L,	/*   353: "fRecolorFillAsPicture" */
     465L,	/*   354: "shadowHighlightExtMod" */
     352L,	/*   355: "lineColumnColorExt" */
     300L,	/*   356: "c3DXRotationAngle" */
     244L,	/*   357: "fc3DParallel" */
      18L,	/*   358: "shapeType" */
     178L,	/*   359: "fRelChangePage" */
     236L,	/*   360: "c3DYViewpoint" */
       6L,	/*   361: "gtextFont" */
     425L,	/*   362: "lineRightStartArrowWidth" */
     117L,	/*   363: "fLockVerticies" */
     111L,	/*   364: "lineBackColor" */
     215L,	/*   365: "gtextFItalic" */
     192L,	/*   366: "shadowPerspectiveY" */
      74L,	/*   367: "lineStartArrowhead" */
     280L,	/*   368: "ccol" */
      20L,	/*   369: "rotation" */
     398L,	/*   370: "lineLeftStartArrowhead" */
     155L,	/*   371: "geoTop" */
     306L,	/*   372: "fillColorExt" */
     142L,	/*   373: "fillRectRight" */
     182L,	/*   374: "shadowOpacity" */
     372L,	/*   375: "lineColumnType" */
      93L,	/*   376: "fBottomLine" */
     231L,	/*   377: "c3DRotationAxisY" */
     314L,	/*   378: "lineBottomBackColor" */
     217L,	/*   379: "gtextFShadow" */
     421L,	/*   380: "lineRightFillWidth" */
     282L,	/*   381: "fScriptAnchor" */
     101L,	/*   382: "lineLeftFillShape" */
     428L,	/*   383: "lineRightType" */
     130L,	/*   384: "fillFocus" */
     411L,	/*   385: "lineRightColorExtMod" */
     247L,	/*   386: "fc3DConstrainRotation" */
     468L,	/*   387: "c3DCrMod" */
     457L,	/*   388: "lineTopWidth" */
     404L,	/*   389: "lineRightBackColorExtCMY" */
      28L,	/*   390: "yLimo" */
     403L,	/*   391: "lineRightBackColorExt" */
     218L,	/*   392: "gtextFSmallcaps" */
     450L,	/*   393: "lineTopMiterLimit" */
     382L,	/*   394: "lineLeftColorExtK" */
     348L,	/*   395: "lineColumnBackColorExtCMY" */
     335L,	/*   396: "lineBottomOpacity" */
     307L,	/*   397: "fillColorExtCMY" */
      26L,	/*   398: "hspNext" */
     283L,	/*   399: "fFakeMaster" */
     109L,	/*   400: "fBottomInsetPen" */
     418L,	/*   401: "lineRightFillBlipFlags" */
      22L,	/*   402: "dyWrapDistTop" */
     295L,	/*   403: "c3DShininess" */
     245L,	/*   404: "fc3DKeyHarsh" */
     168L,	/*   405: "relLeft" */
     201L,	/*   406: "shadowHighlight" */
     183L,	/*   407: "shadowOffsetX" */
     333L,	/*   408: "lineBottomFillWidth" */
     402L,	/*   409: "lineRightBackColor" */
     234L,	/*   410: "c3DRotationCenterZ" */
     371L,	/*   411: "lineColumnStyle" */
     305L,	/*   412: "c3DExtrusionColorExtMod" */
     213L,	/*   413: "gtextFDxMeasure" */
     254L,	/*   414: "dxyCalloutLengthSpecified" */
      40L,	/*   415: "fHitTestFill" */
      47L,	/*   416: "fOnDblClickNotify" */
     187L,	/*   417: "shadowScaleXToX" */
     462L,	/*   418: "shadowHighlightExt" */
     385L,	/*   419: "lineLeftDashing" */
      90L,	/*   420: "fLeftLine" */
     311L,	/*   421: "lineBackColorExtCMY" */
     380L,	/*   422: "lineLeftColorExt" */
     329L,	/*   423: "lineBottomEndCapStyle" */
      89L,	/*   424: "fColumnLine" */
     363L,	/*   425: "lineColumnFillDztype" */
     426L,	/*   426: "lineRightStartArrowhead" */
     440L,	/*   427: "lineTopCrMod" */
     238L,	/*   428: "c3DKeyX" */
     459L,	/*   429: "shadowColorExtCMY" */
     360L,	/*   430: "lineColumnEndArrowhead" */
     214L,	/*   431: "gtextFBold" */
     453L,	/*   432: "lineTopStartArrowWidth" */
     105L,	/*   433: "fInsetPen" */
     174L,	/*   434: "groupLeft" */
      49L,	/*   435: "fPreferRelativeSize" */
      23L,	/*   436: "dxWrapDistRight" */
     429L,	/*   437: "lineRightWidth" */
       2L,	/*   438: "pSegmentInfo" */
     383L,	/*   439: "lineLeftColorExtMod" */
     284L,	/*   440: "c3DAmbientIntensity" */
     397L,	/*   441: "lineLeftStartArrowWidth" */
     132L,	/*   442: "fillToTop" */
     341L,	/*   443: "lineBottomWidth" */
     443L,	/*   444: "lineTopEndArrowWidth" */
     343L,	/*   445: "lineColorExtCMY" */
      11L,	/*   446: "posh" */
     112L,	/*   447: "lineCrMod" */
     154L,	/*   448: "geoLeft" */
       4L,	/*   449: "wzDescription" */
     298L,	/*   450: "c3DSpecularAmt" */
     145L,	/*   451: "fillShadeType" */
      48L,	/*   452: "fOneD" */
     384L,	/*   453: "lineLeftCrMod" */
     185L,	/*   454: "shadowSecondOffsetX" */
     413L,	/*   455: "lineRightDashing" */
     446L,	/*   456: "lineTopFillBlipFlags" */
     149L,	/*   457: "fFillOK" */
     296L,	/*   458: "c3DSkewAmount" */
     433L,	/*   459: "lineTopBackColorExtK" */
      86L,	/*   460: "fLineOK" */
     369L,	/*   461: "lineColumnStartArrowWidth" */
     169L,	/*   462: "relTop" */
     297L,	/*   463: "c3DSkewAngle" */
     113L,	/*   464: "fLockRotation" */
     229L,	/*   465: "fc3DLightFace" */
      69L,	/*   466: "lineFillWidth" */
     143L,	/*   467: "fillRectBottom" */
      99L,	/*   468: "lineFillShape" */
     412L,	/*   469: "lineRightCrMod" */
     342L,	/*   470: "lineColorExt" */
      80L,	/*   471: "lineEndCapStyle" */
     324L,	/*   472: "lineBottomCrMod" */
    };

/* The hash function */

unsigned long docRtfShapePropHash(const unsigned char * key, int len )
{
    const ub4 initlev= 0x7b82e09b;
    ub4 rsl= 0;
    ub4 val = utilJenkinsHash( key, len, initlev );
    rsl = ((val>>25)^tab[val&0xff]);
    return rsl;
}

int docRtfShapePropIndex( const unsigned char * key, int len )
{
    ub4 rsl= docRtfShapePropHash( key, len );

    if  ( rsl >= sizeof(mapBack)/sizeof(short) )
	{ return -1;	}

    return mapBack[rsl];
}

