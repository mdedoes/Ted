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
28,0,243,0,258,65,468,0,481,468,162,243,0,468,404,36,
243,404,0,0,230,0,258,65,154,481,324,28,65,0,154,324,
0,0,484,0,468,154,0,28,0,337,0,154,258,469,404,471,
484,471,337,324,258,28,328,404,36,404,230,0,6,154,6,258,
468,162,0,243,28,258,258,258,395,154,0,484,154,0,36,0,
0,468,0,238,258,0,0,243,471,0,0,65,258,0,243,0,
112,136,6,0,6,234,0,462,243,0,258,154,469,468,162,324,
327,462,395,243,258,162,469,0,419,28,258,28,468,462,0,28,
26,258,6,328,65,0,345,258,162,162,0,324,471,0,319,65,
230,0,395,249,258,481,328,154,264,258,65,162,468,0,0,469,
328,79,204,0,419,162,0,3,256,6,154,65,201,258,325,154,
154,319,131,65,324,404,258,243,338,345,28,379,468,238,157,0,
364,258,154,243,154,258,26,324,328,0,0,419,0,157,243,0,
258,230,319,0,0,258,345,157,462,345,258,326,162,0,324,243,
344,243,327,6,0,319,146,159,258,343,328,0,338,234,419,230,
258,343,0,0,419,154,468,238,510,159,154,183,499,0,0,183,
};

/* map back array */

static short mapBack[]=
    {
     352L,	/*     0: "lineColumnBackColorExtK" */
     266L,	/*     1: "pictureFillCrMod" */
     217L,	/*     2: "gtextFUnderline" */
     459L,	/*     3: "lineTopType" */
      34L,	/*     4: "fHidden" */
     270L,	/*     5: "shapePath" */
     264L,	/*     6: "pictureBrightness" */
     416L,	/*     7: "lineRightDashing" */
     391L,	/*     8: "lineLeftEndArrowhead" */
     289L,	/*     9: "c3DExtrudePlane" */
     479L,	/*    10: "pctHoriz" */
     458L,	/*    11: "lineTopStyle" */
     362L,	/*    12: "lineColumnEndArrowWidth" */
     105L,	/*    13: "lineBottomFillShape" */
     319L,	/*    14: "lineBottomBackColorExtCMY" */
     293L,	/*    15: "c3DOriginY" */
      83L,	/*    16: "lineJoinStyle" */
      13L,	/*    17: "posrelh" */
     198L,	/*    18: "fshadowObscured" */
     311L,	/*    19: "fillColorExtK" */
     376L,	/*    20: "lineColumnWidth" */
      17L,	/*    21: "fChangePage" */
     341L,	/*    22: "lineBottomStartArrowhead" */
      89L,	/*    23: "fColumnLineOK" */
     251L,	/*    24: "dxyCalloutGap" */
     135L,	/*    25: "fillToBottom" */
      70L,	/*    26: "lineFillHeight" */
      51L,	/*    27: "fPseudoInline" */
     463L,	/*    28: "shadowColorExtK" */
     450L,	/*    29: "lineTopFillDztype" */
     259L,	/*    30: "fCalloutDropAuto" */
     174L,	/*    31: "lidRegroup" */
     392L,	/*    32: "lineLeftEndCapStyle" */
     448L,	/*    33: "lineTopEndCapStyle" */
     461L,	/*    34: "shadowColorExt" */
      10L,	/*    35: "posv" */
     178L,	/*    36: "groupBottom" */
     433L,	/*    37: "lineTopBackColor" */
      52L,	/*    38: "dxTextLeft" */
     201L,	/*    39: "shadowColor" */
     421L,	/*    40: "lineRightFillBlipFlags" */
     112L,	/*    41: "lineBackColor" */
     172L,	/*    42: "relBottom" */
     242L,	/*    43: "c3DFillX" */
     323L,	/*    44: "lineBottomColorExt" */
     411L,	/*    45: "lineRightColorExt" */
     373L,	/*    46: "lineColumnStartArrowhead" */
     365L,	/*    47: "lineColumnFillBlipFlags" */
      67L,	/*    48: "lineType" */
     254L,	/*    49: "dxyCalloutDropSpecified" */
     208L,	/*    50: "gtextFKern" */
     358L,	/*    51: "lineColumnColorExtMod" */
     294L,	/*    52: "c3DRenderMode" */
     412L,	/*    53: "lineRightColorExtCMY" */
     326L,	/*    54: "lineBottomColorExtMod" */
     306L,	/*    55: "c3DExtrusionColorExtCMY" */
      25L,	/*    56: "hspMaster" */
      68L,	/*    57: "lineFillBlipFlags" */
     330L,	/*    58: "lineBottomEndArrowWidth" */
     313L,	/*    59: "lineBackColorExt" */
     226L,	/*    60: "c3DExtrudeBackward" */
     441L,	/*    61: "lineTopColorExtK" */
     394L,	/*    62: "lineLeftFillDztype" */
     333L,	/*    63: "lineBottomFillBlipFlags" */
     395L,	/*    64: "lineLeftFillHeight" */
     342L,	/*    65: "lineBottomStyle" */
       8L,	/*    66: "pWrapPolygonVertices" */
     103L,	/*    67: "lineTopFillShape" */
      53L,	/*    68: "dyTextTop" */
     422L,	/*    69: "lineRightFillDztype" */
      73L,	/*    70: "lineDashing" */
     207L,	/*    71: "gtextFVertical" */
     353L,	/*    72: "lineColumnBackColorExtMod" */
     379L,	/*    73: "lineLeftBackColorExtCMY" */
     295L,	/*    74: "c3DRotationAngle" */
     263L,	/*    75: "pictureContrast" */
     192L,	/*    76: "shadowPerspectiveX" */
     302L,	/*    77: "c3DTolerance" */
      96L,	/*    78: "fLeftHitTestLine" */
     167L,	/*    79: "adjust9Value" */
     210L,	/*    80: "gtextFStretch" */
     377L,	/*    81: "lineLeftBackColor" */
     224L,	/*    82: "c3DEdgeThickness" */
     328L,	/*    83: "lineBottomDashing" */
      79L,	/*    84: "lineEndArrowWidth" */
     398L,	/*    85: "lineLeftOpacity" */
     464L,	/*    86: "shadowColorExtMod" */
     255L,	/*    87: "dxyCalloutLengthSpecified" */
     268L,	/*    88: "fCalloutMinusX" */
     283L,	/*    89: "dzColMargin" */
       6L,	/*    90: "pib" */
     452L,	/*    91: "lineTopFillWidth" */
     231L,	/*    92: "c3DRotationAxisX" */
     238L,	/*    93: "c3DZViewpoint" */
     269L,	/*    94: "fCalloutMinusY" */
      57L,	/*    95: "lTxid" */
      88L,	/*    96: "fLineUseShapeAnchor" */
     250L,	/*    97: "spcot" */
      95L,	/*    98: "fColumnHitTestLine" */
     349L,	/*    99: "lineColumnBackColor" */
     147L,	/*   100: "fillShape" */
       9L,	/*   101: "pSegmentInfo" */
     348L,	/*   102: "lineColorExtMod" */
     324L,	/*   103: "lineBottomColorExtCMY" */
      63L,	/*   104: "fRotateText" */
     399L,	/*   105: "lineLeftStartArrowLength" */
      48L,	/*   106: "fOneD" */
      43L,	/*   107: "fNoFillHitTest" */
     314L,	/*   108: "lineBackColorExtCMY" */
     472L,	/*   109: "fStandardHR" */
     181L,	/*   110: "fRelFlipV" */
     129L,	/*   111: "fillHeight" */
     437L,	/*   112: "lineTopBackColorExtMod" */
     378L,	/*   113: "lineLeftBackColorExt" */
     445L,	/*   114: "lineTopEndArrowLength" */
     334L,	/*   115: "lineBottomFillDztype" */
     346L,	/*   116: "lineColorExtCMY" */
     118L,	/*   117: "fLockVerticies" */
     157L,	/*   118: "geoRight" */
     170L,	/*   119: "relTop" */
     356L,	/*   120: "lineColumnColorExtCMY" */
     262L,	/*   121: "pictureActive" */
     359L,	/*   122: "lineColumnCrMod" */
      41L,	/*   123: "fHitTestLine" */
      55L,	/*   124: "dyTextBottom" */
      94L,	/*   125: "fBottomLine" */
     474L,	/*   126: "fHorizRule" */
     297L,	/*   127: "c3DRotationCenterY" */
     396L,	/*   128: "lineLeftFillWidth" */
     303L,	/*   129: "c3DXRotationAngle" */
     225L,	/*   130: "c3DExtrudeForward" */
     423L,	/*   131: "lineRightFillHeight" */
     478L,	/*   132: "sizerelv" */
     322L,	/*   133: "lineBottomColor" */
      60L,	/*   134: "txflTextFlow" */
      39L,	/*   135: "fEditedWrap" */
      87L,	/*   136: "fLineOK" */
     337L,	/*   137: "lineBottomMiterLimit" */
     111L,	/*   138: "lineColor" */
     430L,	/*   139: "lineRightStyle" */
     205L,	/*   140: "gtextAlign" */
     185L,	/*   141: "shadowOffsetY" */
      77L,	/*   142: "lineStartArrowWidth" */
     253L,	/*   143: "spcod" */
     413L,	/*   144: "lineRightColorExtK" */
     165L,	/*   145: "adjust7Value" */
     401L,	/*   146: "lineLeftStartArrowhead" */
     155L,	/*   147: "geoLeft" */
      36L,	/*   148: "fArrowheadsOK" */
      78L,	/*   149: "lineStartArrowLength" */
     382L,	/*   150: "lineLeftColor" */
     153L,	/*   151: "fillBackColor" */
      16L,	/*   152: "fAllowOverlap" */
     318L,	/*   153: "lineBottomBackColorExt" */
     279L,	/*   154: "fPreferRelativeResize" */
     465L,	/*   155: "shadowHighlightExt" */
     325L,	/*   156: "lineBottomColorExtK" */
     438L,	/*   157: "lineTopColor" */
     447L,	/*   158: "lineTopEndArrowhead" */
     134L,	/*   159: "fillToRight" */
     109L,	/*   160: "fRightInsetPen" */
     122L,	/*   161: "fLockShapeType" */
     419L,	/*   162: "lineRightEndArrowhead" */
     175L,	/*   163: "groupLeft" */
     257L,	/*   164: "fCalloutAccentBar" */
     274L,	/*   165: "cropFromRight" */
     159L,	/*   166: "adjustValue" */
     168L,	/*   167: "adjust10Value" */
      99L,	/*   168: "fBottomHitTestLine" */
     440L,	/*   169: "lineTopColorExtCMY" */
     442L,	/*   170: "lineTopColorExtMod" */
     347L,	/*   171: "lineColorExtK" */
     234L,	/*   172: "fC3DRotationCenterAut" */
      82L,	/*   173: "lineFillDztype" */
     117L,	/*   174: "fLockCropping" */
     316L,	/*   175: "lineBackColorExtMod" */
     410L,	/*   176: "lineRightColor" */
     338L,	/*   177: "lineBottomOpacity" */
      35L,	/*   178: "fReallyHidden" */
       0L,	/*   179: "wzName" */
     321L,	/*   180: "lineBottomBackColorExtMod" */
     154L,	/*   181: "fillCrMod" */
      72L,	/*   182: "lineStyle" */
     473L,	/*   183: "fNoshadeHR" */
     121L,	/*   184: "fLockAgainstGrouping" */
     189L,	/*   185: "shadowScaleYToX" */
     296L,	/*   186: "c3DRotationCenterX" */
     369L,	/*   187: "lineColumnMiterLimit" */
     393L,	/*   188: "lineLeftFillBlipFlags" */
      32L,	/*   189: "fBehindDocument" */
     350L,	/*   190: "lineColumnBackColorExt" */
     206L,	/*   191: "fGtext" */
     367L,	/*   192: "lineColumnFillHeight" */
      54L,	/*   193: "dxTextRight" */
     176L,	/*   194: "groupTop" */
      50L,	/*   195: "fPrint" */
     123L,	/*   196: "fLockPosition" */
     414L,	/*   197: "lineRightColorExtMod" */
     443L,	/*   198: "lineTopCrMod" */
     171L,	/*   199: "relRight" */
     124L,	/*   200: "fillType" */
     451L,	/*   201: "lineTopFillHeight" */
     243L,	/*   202: "c3DFillY" */
     107L,	/*   203: "fLeftInsetPen" */
     115L,	/*   204: "fLockAspectRatio" */
     320L,	/*   205: "lineBottomBackColorExtK" */
     360L,	/*   206: "lineColumnDashing" */
      90L,	/*   207: "fColumnLine" */
     130L,	/*   208: "fillAngle" */
     371L,	/*   209: "lineColumnStartArrowLength" */
     163L,	/*   210: "adjust5Value" */
     420L,	/*   211: "lineRightEndCapStyle" */
     345L,	/*   212: "lineColorExt" */
     236L,	/*   213: "c3DXViewpoint" */
     276L,	/*   214: "cropFromBottom" */
     460L,	/*   215: "lineTopWidth" */
     184L,	/*   216: "shadowOffsetX" */
     140L,	/*   217: "fillDztype" */
     329L,	/*   218: "lineBottomEndArrowLength" */
     223L,	/*   219: "pictureBiLevel" */
     213L,	/*   220: "gtextFNormalize" */
     339L,	/*   221: "lineBottomStartArrowLength" */
      64L,	/*   222: "fSelectText" */
     116L,	/*   223: "fLockAgainstSelect" */
     199L,	/*   224: "fShadowOK" */
      14L,	/*   225: "fUseShapeAnchor" */
     462L,	/*   226: "shadowColorExtCMY" */
     233L,	/*   227: "c3DRotationAxisZ" */
     471L,	/*   228: "c3DCrMod" */
     219L,	/*   229: "gtextFSmallcaps" */
     229L,	/*   230: "fc3DUseExtrusionColor" */
     273L,	/*   231: "cropFromLeft" */
     260L,	/*   232: "fCalloutLengthSpecified" */
     357L,	/*   233: "lineColumnColorExtK" */
      86L,	/*   234: "fline" */
     467L,	/*   235: "shadowHighlightExtK" */
     291L,	/*   236: "c3DKeyIntensity" */
      44L,	/*   237: "fNoHitTestPicture" */
     144L,	/*   238: "fillRectBottom" */
     164L,	/*   239: "adjust6Value" */
     152L,	/*   240: "fillColor" */
     449L,	/*   241: "lineTopFillBlipFlags" */
     476L,	/*   242: "dhgt" */
     305L,	/*   243: "c3DExtrusionColorExt" */
      24L,	/*   244: "dyWrapDistBottom" */
     457L,	/*   245: "lineTopStartArrowhead" */
     177L,	/*   246: "groupRight" */
      98L,	/*   247: "fRightHitTestLine" */
     387L,	/*   248: "lineLeftCrMod" */
     182L,	/*   249: "shadowType" */
     193L,	/*   250: "shadowPerspectiveY" */
     114L,	/*   251: "fLockRotation" */
     139L,	/*   252: "fillShapeOriginY" */
     227L,	/*   253: "f3D" */
     136L,	/*   254: "fillOriginX" */
     149L,	/*   255: "fFilled" */
     408L,	/*   256: "lineRightBackColorExtK" */
     418L,	/*   257: "lineRightEndArrowWidth" */
     150L,	/*   258: "fFillOK" */
     125L,	/*   259: "fillOpacity" */
     240L,	/*   260: "c3DKeyY" */
     173L,	/*   261: "relRotation" */
      56L,	/*   262: "scaleText" */
      21L,	/*   263: "dxWrapDistLeft" */
     331L,	/*   264: "lineBottomEndArrowhead" */
     368L,	/*   265: "lineColumnFillWidth" */
     383L,	/*   266: "lineLeftColorExt" */
     180L,	/*   267: "fRelFlipH" */
     146L,	/*   268: "fillShadeType" */
     417L,	/*   269: "lineRightEndArrowLength" */
     336L,	/*   270: "lineBottomFillWidth" */
      97L,	/*   271: "fTopHitTestLine" */
      28L,	/*   272: "yLimo" */
     249L,	/*   273: "fc3DRotationCenterAuto" */
     197L,	/*   274: "fShadow" */
      93L,	/*   275: "fRightLine" */
      61L,	/*   276: "cdirFont" */
     380L,	/*   277: "lineLeftBackColorExtK" */
     161L,	/*   278: "adjust3Value" */
      84L,	/*   279: "lineMiterLimit" */
     281L,	/*   280: "dgmt" */
     475L,	/*   281: "fUserDrawn" */
      66L,	/*   282: "fFitTextToShape" */
      15L,	/*   283: "fLayoutInCell" */
     384L,	/*   284: "lineLeftColorExtCMY" */
     200L,	/*   285: "shadowCrMod" */
     304L,	/*   286: "c3DYRotationAngle" */
     402L,	/*   287: "lineLeftStyle" */
     288L,	/*   288: "c3DDiffuseAmt" */
     166L,	/*   289: "adjust8Value" */
     187L,	/*   290: "shadowSecondOffsetY" */
     267L,	/*   291: "pictureLineCrMod" */
     209L,	/*   292: "gtextFTight" */
     470L,	/*   293: "c3DExtrusionColor" */
     101L,	/*   294: "lineColumnFillShape" */
     312L,	/*   295: "fillColorExtMod" */
     466L,	/*   296: "shadowHighlightExtCMY" */
     261L,	/*   297: "pictureId" */
     390L,	/*   298: "lineLeftEndArrowWidth" */
     406L,	/*   299: "lineRightBackColorExt" */
     148L,	/*   300: "fillUseRect" */
     403L,	/*   301: "lineLeftType" */
      31L,	/*   302: "fFlipH" */
     222L,	/*   303: "pictureGray" */
     244L,	/*   304: "c3DFillZ" */
      38L,	/*   305: "fDeleteAttachedObject" */
      37L,	/*   306: "fBackground" */
      62L,	/*   307: "fAutoTextMargin" */
     364L,	/*   308: "lineColumnEndCapStyle" */
      76L,	/*   309: "lineEndArrowhead" */
      85L,	/*   310: "fLine" */
     409L,	/*   311: "lineRightBackColorExtMod" */
     220L,	/*   312: "gtextFStrikethrough" */
     455L,	/*   313: "lineTopStartArrowLength" */
     194L,	/*   314: "shadowWeight" */
     137L,	/*   315: "fillOriginY" */
     245L,	/*   316: "fc3DParallel" */
     292L,	/*   317: "c3DOriginX" */
     265L,	/*   318: "pictureDblCrMod" */
      65L,	/*   319: "fFitShapeToText" */
      71L,	/*   320: "lineWidth" */
      59L,	/*   321: "anchorText" */
     191L,	/*   322: "shadowScaleYToY" */
     127L,	/*   323: "fillblipflags" */
     221L,	/*   324: "pibFlags" */
      42L,	/*   325: "fInitiator" */
     196L,	/*   326: "shadowOriginY" */
     444L,	/*   327: "lineTopDashing" */
     256L,	/*   328: "fCallout" */
     434L,	/*   329: "lineTopBackColorExt" */
      58L,	/*   330: "WrapText" */
     241L,	/*   331: "c3DKeyZ" */
     343L,	/*   332: "lineBottomType" */
     277L,	/*   333: "bWMode" */
      92L,	/*   334: "fTopLine" */
      33L,	/*   335: "fIsButton" */
     271L,	/*   336: "lineOpacity" */
     104L,	/*   337: "lineRightFillShape" */
     128L,	/*   338: "fillWidth" */
     120L,	/*   339: "fLockAdjustHandles" */
     141L,	/*   340: "fillRectLeft" */
     286L,	/*   341: "fCameFromImgDummy" */
     425L,	/*   342: "lineRightMiterLimit" */
       5L,	/*   343: "fillBlipName" */
     212L,	/*   344: "gtextFBestFit" */
     340L,	/*   345: "lineBottomStartArrowWidth" */
      30L,	/*   346: "fFlipV" */
     211L,	/*   347: "gtextFShrinkFit" */
     119L,	/*   348: "fLockText" */
     278L,	/*   349: "f3DOK" */
     108L,	/*   350: "fTopInsetPen" */
     110L,	/*   351: "fBottomInsetPen" */
      46L,	/*   352: "fOleIcon" */
     280L,	/*   353: "fRecolorFillAsPicture" */
     468L,	/*   354: "shadowHighlightExtMod" */
     142L,	/*   355: "fillRectTop" */
     469L,	/*   356: "pictureTransparent" */
     190L,	/*   357: "shadowScaleXToY" */
      18L,	/*   358: "shapeType" */
     179L,	/*   359: "fRelChangePage" */
     237L,	/*   360: "c3DYViewpoint" */
       3L,	/*   361: "gtextFont" */
     428L,	/*   362: "lineRightStartArrowWidth" */
     158L,	/*   363: "geoBottom" */
     389L,	/*   364: "lineLeftEndArrowLength" */
     216L,	/*   365: "gtextFItalic" */
     188L,	/*   366: "shadowScaleXToX" */
      75L,	/*   367: "lineStartArrowhead" */
     344L,	/*   368: "lineBottomWidth" */
      20L,	/*   369: "rotation" */
     355L,	/*   370: "lineColumnColorExt" */
     156L,	/*   371: "geoTop" */
     309L,	/*   372: "fillColorExt" */
     143L,	/*   373: "fillRectRight" */
     183L,	/*   374: "shadowOpacity" */
     375L,	/*   375: "lineColumnType" */
     386L,	/*   376: "lineLeftColorExtMod" */
     275L,	/*   377: "cropFromTop" */
     317L,	/*   378: "lineBottomBackColor" */
     218L,	/*   379: "gtextFShadow" */
     424L,	/*   380: "lineRightFillWidth" */
     284L,	/*   381: "fScriptAnchor" */
      12L,	/*   382: "posrelv" */
     431L,	/*   383: "lineRightType" */
     397L,	/*   384: "lineLeftMiterLimit" */
      45L,	/*   385: "fNoLineDrawDash" */
      29L,	/*   386: "fIsBullet" */
     203L,	/*   387: "cxk" */
     454L,	/*   388: "lineTopOpacity" */
     204L,	/*   389: "cxstyle" */
     290L,	/*   390: "c3DFillIntensity" */
     247L,	/*   391: "fc3DFillHarsh" */
       7L,	/*   392: "pVerticies" */
     453L,	/*   393: "lineTopMiterLimit" */
     385L,	/*   394: "lineLeftColorExtK" */
     351L,	/*   395: "lineColumnBackColorExtCMY" */
     160L,	/*   396: "adjust2Value" */
     131L,	/*   397: "fillFocus" */
      26L,	/*   398: "hspNext" */
     285L,	/*   399: "fFakeMaster" */
     435L,	/*   400: "lineTopBackColorExtCMY" */
     258L,	/*   401: "fCalloutTextBorder" */
      22L,	/*   402: "dyWrapDistTop" */
     298L,	/*   403: "c3DShininess" */
     246L,	/*   404: "fc3DKeyHarsh" */
     169L,	/*   405: "relLeft" */
     202L,	/*   406: "shadowHighlight" */
      27L,	/*   407: "xLimo" */
      80L,	/*   408: "lineEndArrowLength" */
     235L,	/*   409: "c3DRotationCenterZ" */
     370L,	/*   410: "lineColumnOpacity" */
     407L,	/*   411: "lineRightBackColorExtCMY" */
     308L,	/*   412: "c3DExtrusionColorExtMod" */
     214L,	/*   413: "gtextFDxMeasure" */
     138L,	/*   414: "fillShapeOriginX" */
      40L,	/*   415: "fHitTestFill" */
      47L,	/*   416: "fOnDblClickNotify" */
     388L,	/*   417: "lineLeftDashing" */
     381L,	/*   418: "lineLeftBackColorExtMod" */
     427L,	/*   419: "lineRightStartArrowLength" */
      91L,	/*   420: "fLeftLine" */
     307L,	/*   421: "c3DExtrusionColorExtK" */
       4L,	/*   422: "pibName" */
     332L,	/*   423: "lineBottomEndCapStyle" */
     228L,	/*   424: "fc3DMetallic" */
     366L,	/*   425: "lineColumnFillDztype" */
     429L,	/*   426: "lineRightStartArrowhead" */
     272L,	/*   427: "lineblipflags" */
     239L,	/*   428: "c3DKeyX" */
     363L,	/*   429: "lineColumnEndArrowhead" */
     195L,	/*   430: "shadowOriginX" */
     215L,	/*   431: "gtextFBold" */
     456L,	/*   432: "lineTopStartArrowWidth" */
     106L,	/*   433: "fInsetPen" */
     102L,	/*   434: "lineLeftFillShape" */
      49L,	/*   435: "fPreferRelativeSize" */
      23L,	/*   436: "dxWrapDistRight" */
     252L,	/*   437: "spcoa" */
      74L,	/*   438: "lineDashStyle" */
     432L,	/*   439: "lineRightWidth" */
     287L,	/*   440: "c3DAmbientIntensity" */
     400L,	/*   441: "lineLeftStartArrowWidth" */
     133L,	/*   442: "fillToTop" */
      19L,	/*   443: "ShapeType" */
     446L,	/*   444: "lineTopEndArrowWidth" */
     361L,	/*   445: "lineColumnEndArrowLength" */
      11L,	/*   446: "posh" */
     113L,	/*   447: "lineCrMod" */
     282L,	/*   448: "ccol" */
     151L,	/*   449: "fFillShadeShapeOK" */
     301L,	/*   450: "c3DSpecularAmt" */
       1L,	/*   451: "wzDescription" */
     404L,	/*   452: "lineLeftWidth" */
     248L,	/*   453: "fc3DConstrainRotation" */
     186L,	/*   454: "shadowSecondOffsetX" */
     315L,	/*   455: "lineBackColorExtK" */
     126L,	/*   456: "fillBackOpacity" */
       2L,	/*   457: "gtextUNICODE" */
     299L,	/*   458: "c3DSkewAmount" */
     436L,	/*   459: "lineTopBackColorExtK" */
     232L,	/*   460: "c3DRotationAxisY" */
     372L,	/*   461: "lineColumnStartArrowWidth" */
     310L,	/*   462: "fillColorExtCMY" */
     300L,	/*   463: "c3DSkewAngle" */
     426L,	/*   464: "lineRightOpacity" */
      69L,	/*   465: "lineFillWidth" */
     230L,	/*   466: "fc3DLightFace" */
     439L,	/*   467: "lineTopColorExt" */
     100L,	/*   468: "lineFillShape" */
     415L,	/*   469: "lineRightCrMod" */
     477L,	/*   470: "sizerelh" */
      81L,	/*   471: "lineEndCapStyle" */
     374L,	/*   472: "lineColumnStyle" */
     132L,	/*   473: "fillToLeft" */
     405L,	/*   474: "lineRightBackColor" */
     327L,	/*   475: "lineBottomCrMod" */
     335L,	/*   476: "lineBottomFillHeight" */
     354L,	/*   477: "lineColumnColor" */
     145L,	/*   478: "fillShadePreset" */
     162L,	/*   479: "adjust4Value" */
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

