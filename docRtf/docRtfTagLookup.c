# include "docRtfTagLookup.h"

const int docRtfTagLookup[] =
{
       3,	/*   0:   1   3   0, "sect" */
       2,	/*   1:   1   4   0, "row" */
       1,	/*   2:   1   5   0, "cell" */
       0,	/*   3:   1   6   0, "par" */
       4,	/*   4:   1   8   0, "nestcell" */
       5,	/*   5:   1   9   0, "nestrow" */
       6,	/*   6:   1  10   0, "nonesttables" */
    1096,	/*   7:   2   2   0, "headerf" */
    1097,	/*   8:   2   3   0, "headerl" */
    1098,	/*   9:   2   4   0, "headerr" */
    1099,	/*  10:   2   5   0, "headere" */
    1091,	/*  11:   2   6   0, "footerf" */
    1092,	/*  12:   2   7   0, "footerl" */
    1093,	/*  13:   2   8   0, "footerr" */
    1094,	/*  14:   2   9   0, "footere" */
    1106,	/*  15:   2  10   0, "footnote" */
    1107,	/*  16:   2  12   0, "annotation" */
    1100,	/*  17:   2  13   0, "ftnsep" */
    1101,	/*  18:   2  14   0, "ftnsepc" */
    1102,	/*  19:   2  15   0, "ftncn" */
    1103,	/*  20:   2  16   0, "aftnsep" */
    1104,	/*  21:   2  17   0, "aftnsepc" */
    1105,	/*  22:   2  18   0, "aftncn" */
     440,	/*  23:   3   0   1, "tsbghoriz" */
     441,	/*  24:   3   0   2, "tsbgvert" */
     442,	/*  25:   3   0   3, "tsbgfdiag" */
     443,	/*  26:   3   0   4, "tsbgbdiag" */
     444,	/*  27:   3   0   5, "tsbgcross" */
     445,	/*  28:   3   0   6, "tsbgdcross" */
     446,	/*  29:   3   0   7, "tsbgdkhor" */
     447,	/*  30:   3   0   8, "tsbgdkvert" */
     448,	/*  31:   3   0   9, "tsbgdkfdiag" */
     449,	/*  32:   3   0  10, "tsbgdkbdiag" */
     450,	/*  33:   3   0  11, "tsbgdkcross" */
     451,	/*  34:   3   0  12, "tsbgdkdcross" */
     696,	/*  35:   3   1  10, "NoFtnalt" */
     695,	/*  36:   3   1  11, "ftnalt" */
     697,	/*  37:   3   2   0, "FtnAuto" */
     425,	/*  38:   3  25   1, "trbghoriz" */
     426,	/*  39:   3  25   2, "trbgvert" */
     427,	/*  40:   3  25   3, "trbgfdiag" */
     428,	/*  41:   3  25   4, "trbgbdiag" */
     429,	/*  42:   3  25   5, "trbgcross" */
     430,	/*  43:   3  25   6, "trbgdcross" */
     431,	/*  44:   3  25   7, "trbgdkhor" */
     432,	/*  45:   3  25   8, "trbgdkvert" */
     433,	/*  46:   3  25   9, "trbgdkfdiag" */
     434,	/*  47:   3  25  10, "trbgdkbdiag" */
     435,	/*  48:   3  25  11, "trbgdkcross" */
     436,	/*  49:   3  25  12, "trbgdkdcross" */
     884,	/*  50:   4   0   1, "wmetafile" */
     885,	/*  51:   4   0   2, "pngblip" */
     886,	/*  52:   4   0   3, "jpegblip" */
     887,	/*  53:   4   0   4, "emfblip" */
     883,	/*  54:   4   0   5, "macpict" */
     890,	/*  55:   4   0   6, "pmmetafile" */
     888,	/*  56:   4   0   7, "dibitmap" */
     889,	/*  57:   4   0   8, "wbitmap" */
     894,	/*  58:   4   1   0, "picw" */
     895,	/*  59:   4   2   0, "pich" */
     896,	/*  60:   4   3   0, "picwgoal" */
     897,	/*  61:   4   4   0, "pichgoal" */
     898,	/*  62:   4   5   0, "picscalex" */
     899,	/*  63:   4   6   0, "picscaley" */
     900,	/*  64:   4   7   0, "piccropt" */
     901,	/*  65:   4   8   0, "piccropb" */
     902,	/*  66:   4   9   0, "piccropl" */
     903,	/*  67:   4  10   0, "piccropr" */
     906,	/*  68:   4  11   0, "bliptag" */
     908,	/*  69:   4  12   0, "picbpp" */
     909,	/*  70:   4  13   0, "blipupi" */
     891,	/*  71:   4  14   0, "wbmbitspixel" */
     892,	/*  72:   4  15   0, "wbmplanes" */
     893,	/*  73:   4  16   0, "wbmwidthbytes" */
     904,	/*  74:   4  17   0, "defshp" */
     907,	/*  75:   4  18   0, "picbmp" */
     905,	/*  76:   4  19   0, "picscaled" */
     864,	/*  77:   5   0   0, "tx" */
     854,	/*  78:   5   1   0, "tql" */
     855,	/*  79:   5   1   1, "tqr" */
     856,	/*  80:   5   1   2, "tqc" */
     857,	/*  81:   5   1   3, "tqdec" */
     858,	/*  82:   5   2   1, "tldot" */
     859,	/*  83:   5   2   2, "tlul" */
     860,	/*  84:   5   2   3, "tlhyph" */
     861,	/*  85:   5   2   4, "tlth" */
     862,	/*  86:   5   2   5, "tleq" */
     863,	/*  87:   5   3   0, "jclisttab" */
     910,	/*  88:   6   0   1, "rsltrtf" */
     911,	/*  89:   6   0   2, "rslttxt" */
     912,	/*  90:   6   0   3, "rsltpict" */
     913,	/*  91:   6   0   4, "rsltbmp" */
     914,	/*  92:   6   0   5, "rslthtml" */
     915,	/*  93:   6   1   0, "objemb" */
     916,	/*  94:   6   1   1, "objlink" */
     917,	/*  95:   6   1   2, "objautlink" */
     918,	/*  96:   6   1   3, "objsub" */
     919,	/*  97:   6   1   4, "objpub" */
     920,	/*  98:   6   1   5, "objicemb" */
     921,	/*  99:   6   1   6, "objhtml" */
     922,	/* 100:   6   1   7, "objocx" */
     923,	/* 101:   6   2   0, "objw" */
     924,	/* 102:   6   3   0, "objh" */
     925,	/* 103:   6   4   0, "objscalex" */
     926,	/* 104:   6   5   0, "objscaley" */
     927,	/* 105:   6   6   0, "objcropt" */
     928,	/* 106:   6   7   0, "objcropb" */
     929,	/* 107:   6   8   0, "objcropl" */
     930,	/* 108:   6   9   0, "objcropr" */
     737,	/* 109:   7   0   0, "bkmkcolf" */
     738,	/* 110:   7   1   0, "bkmkcoll" */
     733,	/* 111:   7   2   0, "flddirty" */
     734,	/* 112:   7   3   0, "fldedit" */
     735,	/* 113:   7   4   0, "fldlock" */
     736,	/* 114:   7   5   0, "fldpriv" */
    1055,	/* 115:   7   6   0, "fldrslt" */
    1054,	/* 116:   7   7   0, "fldinst" */
    1056,	/* 117:   7   8   0, "datafield" */
    1057,	/* 118:   7   9   0, "formfield" */
    1018,	/* 119:   8   0   0, "shpleft" */
    1019,	/* 120:   8   1   0, "shptop" */
    1020,	/* 121:   8   2   0, "shpbottom" */
    1021,	/* 122:   8   3   0, "shpright" */
    1022,	/* 123:   8   4   0, "shplid" */
    1023,	/* 124:   8   5   0, "shpz" */
    1024,	/* 125:   8   6   0, "shpfhdr" */
    1025,	/* 126:   8   7   0, "shpbxmargin" */
    1026,	/* 127:   8   7   1, "shpbxpage" */
    1027,	/* 128:   8   7   2, "shpbxcolumn" */
    1028,	/* 129:   8   7   4, "shpbxignore" */
    1029,	/* 130:   8   8   0, "shpbymargin" */
    1030,	/* 131:   8   8   1, "shpbypage" */
    1031,	/* 132:   8   8   2, "shpbypara" */
    1032,	/* 133:   8   8   4, "shpbyignore" */
    1033,	/* 134:   8   9   0, "shpwr" */
    1034,	/* 135:   8  10   0, "shpwrk" */
    1035,	/* 136:   8  11   0, "shpfblwtxt" */
    1036,	/* 137:   8  12   0, "shplockanchor" */
    1088,	/* 138:  10   3   0, "width" */
    1089,	/* 139:  10   4   0, "height" */
    1053,	/* 140:  11   0   0, "field" */
    1078,	/* 141:  11   2   0, "xe" */
    1084,	/* 142:  11   7   0, "sectnum" */
    1080,	/* 143:  11   9   0, "chpgn" */
    1079,	/* 144:  11  16   0, "chdate" */
    1081,	/* 145:  11  17   0, "chtime" */
    1082,	/* 146:  11  30   0, "chftn" */
    1083,	/* 147:  11  31   0, "chatn" */
    1076,	/* 148:  11  32   0, "listtext" */
    1087,	/* 149:  11  34   0, "NeXTGraphic" */
     716,	/* 150:  12   0   0, "red" */
     717,	/* 151:  12   1   0, "green" */
     718,	/* 152:  12   2   0, "blue" */
     699,	/* 153:  13   1   0, "sbasedon" */
     698,	/* 154:  13   2   0, "additive" */
     700,	/* 155:  13   3   0, "snext" */
     701,	/* 156:  13   4   0, "slink" */
     702,	/* 157:  13   6   0, "sautoupd" */
     703,	/* 158:  13   7   0, "shidden" */
     704,	/* 159:  13   8   0, "slocked" */
     705,	/* 160:  13   9   0, "ssemihidden" */
     706,	/* 161:  13  10   0, "spersonal" */
     866,	/* 162:  14   0   0, "fnil" */
     867,	/* 163:  14   0   1, "froman" */
     868,	/* 164:  14   0   2, "fswiss" */
     869,	/* 165:  14   0   3, "fmodern" */
     870,	/* 166:  14   0   4, "fscript" */
     871,	/* 167:  14   0   5, "fdecor" */
     872,	/* 168:  14   0   6, "ftech" */
     873,	/* 169:  14   0   7, "fbidi" */
     882,	/* 170:  14   2   0, "falt" */
     874,	/* 171:  14   3   0, "fcharset" */
     875,	/* 172:  14   4   0, "cpg" */
     876,	/* 173:  14   5   0, "fprq" */
     881,	/* 174:  14   6   0, "panose" */
     877,	/* 175:  14   7   1, "ftnil" */
     878,	/* 176:  14   7   2, "ftruetype" */
     880,	/* 177:  14   8   0, "fontfile" */
     879,	/* 178:  14   9   0, "fontemb" */
       7,	/* 179:  16  -1   0, "plain" */
       8,	/* 180:  16   0   0, "f" */
       9,	/* 181:  16   1   0, "fs" */
      34,	/* 182:  16   2   0, "b" */
      35,	/* 183:  16   3   0, "i" */
      37,	/* 184:  16   4   0, "ulnone" */
      36,	/* 185:  16   4   1, "ul" */
      54,	/* 186:  16   5   0, "nosupersub" */
      55,	/* 187:  16   5   1, "super" */
      56,	/* 188:  16   5   2, "sub" */
      58,	/* 189:  16   6   0, "scaps" */
      57,	/* 190:  16   7   0, "caps" */
      59,	/* 191:  16   8   0, "strike" */
      10,	/* 192:  16   9   0, "cf" */
      12,	/* 193:  16  10   0, "cs" */
      15,	/* 194:  16  11   0, "chbrdr" */
      13,	/* 195:  16  13   0, "up" */
      16,	/* 196:  16  14   0, "lang" */
      17,	/* 197:  16  15   0, "noproof" */
      14,	/* 198:  16  16   0, "dn" */
      19,	/* 199:  17   0   0, "chcbpat" */
      18,	/* 200:  17   1   0, "chcfpat" */
      20,	/* 201:  17   2   0, "chshdng" */
      22,	/* 202:  17   3   1, "chbghoriz" */
      23,	/* 203:  17   3   2, "chbgvert" */
      24,	/* 204:  17   3   3, "chbgfdiag" */
      25,	/* 205:  17   3   4, "chbgbdiag" */
      26,	/* 206:  17   3   5, "chbgcross" */
      27,	/* 207:  17   3   6, "chbgdcross" */
      28,	/* 208:  17   3   7, "chbgdkhoriz" */
      29,	/* 209:  17   3   8, "chbgdkvert" */
      30,	/* 210:  17   3   9, "chbgdkfdiag" */
      31,	/* 211:  17   3  10, "chbgdkbdiag" */
      32,	/* 212:  17   3  11, "chbgdkcross" */
      33,	/* 213:  17   3  12, "chbgdkdcross" */
      21,	/* 214:  17   5   0, "cb" */
      61,	/* 215:  18   0   0, "ltrch" */
      62,	/* 216:  18   0   1, "rtlch" */
     139,	/* 217:  19  -1   0, "pard" */
     140,	/* 218:  19   0   0, "s" */
     141,	/* 219:  19   1   0, "ls" */
     162,	/* 220:  19   2   0, "ilvl" */
     165,	/* 221:  19   3   0, "itap" */
     163,	/* 222:  19   4   0, "hyphpar" */
     151,	/* 223:  19   5   0, "nowidctlpar" */
     152,	/* 224:  19   5   1, "widctlpar" */
     188,	/* 225:  19   6   0, "ltrpar" */
     189,	/* 226:  19   6   1, "rtlpar" */
     153,	/* 227:  19   7   0, "keep" */
     154,	/* 228:  19   8   0, "keepn" */
     161,	/* 229:  19   9   0, "outlinelevel" */
     143,	/* 230:  19  10   0, "fi" */
     142,	/* 231:  19  11   0, "li" */
     144,	/* 232:  19  12   0, "ri" */
     155,	/* 233:  19  13   0, "sb" */
     156,	/* 234:  19  14   0, "sa" */
     157,	/* 235:  19  15   0, "sl" */
     160,	/* 236:  19  16   0, "slmult" */
     145,	/* 237:  19  17   0, "ql" */
     146,	/* 238:  19  17   1, "qr" */
     148,	/* 239:  19  17   2, "qc" */
     147,	/* 240:  19  17   3, "qj" */
     181,	/* 241:  19  19   0, "brdrt" */
     182,	/* 242:  19  20   0, "brdrb" */
     183,	/* 243:  19  21   0, "brdrl" */
     184,	/* 244:  19  22   0, "brdrr" */
     185,	/* 245:  19  23   0, "brdrbtw" */
     186,	/* 246:  19  24   0, "brdrbar" */
     149,	/* 247:  19  27   0, "nopagebb" */
     150,	/* 248:  19  27   2, "pagebb" */
     164,	/* 249:  19  29   0, "intbl" */
     187,	/* 250:  19  33   0, "box" */
     158,	/* 251:  19  34   0, "lisb" */
     159,	/* 252:  19  35   0, "lisa" */
     179,	/* 253:  20   0   0, "cbpat" */
     178,	/* 254:  20   1   0, "cfpat" */
     180,	/* 255:  20   2   0, "shading" */
     166,	/* 256:  20   3   1, "bghoriz" */
     167,	/* 257:  20   3   2, "bgvert" */
     168,	/* 258:  20   3   3, "bgfdiag" */
     169,	/* 259:  20   3   4, "bgbdiag" */
     170,	/* 260:  20   3   5, "bgcross" */
     171,	/* 261:  20   3   6, "bgdcross" */
     172,	/* 262:  20   3   7, "bgdkhoriz" */
     173,	/* 263:  20   3   8, "bgdkvert" */
     174,	/* 264:  20   3   9, "bgdkfdiag" */
     175,	/* 265:  20   3  10, "bgdkbdiag" */
     176,	/* 266:  20   3  11, "bgdkcross" */
     177,	/* 267:  20   3  12, "bgdkdcross" */
     226,	/* 268:  21   1   0, "absw" */
     227,	/* 269:  21   2   0, "absh" */
     231,	/* 270:  21   3   0, "absnoovrlp" */
     232,	/* 271:  21   4   0, "phmrg" */
     233,	/* 272:  21   4   1, "phpg" */
     234,	/* 273:  21   4   2, "phcol" */
     235,	/* 274:  21   5   0, "posx" */
     237,	/* 275:  21   6   1, "posxl" */
     239,	/* 276:  21   6   2, "posxc" */
     238,	/* 277:  21   6   3, "posxr" */
     240,	/* 278:  21   6   4, "posxi" */
     241,	/* 279:  21   6   5, "posxo" */
     242,	/* 280:  21   7   0, "pvmrg" */
     244,	/* 281:  21   7   1, "pvpg" */
     243,	/* 282:  21   7   2, "pvpara" */
     245,	/* 283:  21   7   3, "posyil" */
     246,	/* 284:  21   8   0, "posy" */
     252,	/* 285:  21   9   1, "posyt" */
     249,	/* 286:  21   9   2, "posyc" */
     248,	/* 287:  21   9   3, "posyb" */
     250,	/* 288:  21   9   4, "posyin" */
     251,	/* 289:  21   9   5, "posyout" */
     228,	/* 290:  21  10   0, "abslock" */
     229,	/* 291:  21  11   1, "nowrap" */
     230,	/* 292:  21  11   2, "overlay" */
     256,	/* 293:  21  16   0, "frmtxlrtb" */
     257,	/* 294:  21  16   1, "frmtxtbrl" */
     258,	/* 295:  21  16   2, "frmtxbtlr" */
     259,	/* 296:  21  16   3, "frmtxlrtbv" */
     260,	/* 297:  21  16   4, "frmtxtbrlv" */
     253,	/* 298:  21  19   0, "dxfrtext" */
     254,	/* 299:  21  20   0, "dfrmtxtx" */
     255,	/* 300:  21  21   0, "dfrmtxty" */
     295,	/* 301:  22  -1   0, "celld" */
     294,	/* 302:  22   0   0, "cellx" */
     296,	/* 303:  22   1   0, "clbrdrt" */
     298,	/* 304:  22   2   0, "clbrdrl" */
     299,	/* 305:  22   3   0, "clbrdrr" */
     297,	/* 306:  22   4   0, "clbrdrb" */
     300,	/* 307:  22   5   0, "cldglu" */
     301,	/* 308:  22   6   0, "cldgll" */
     340,	/* 309:  22   8   0, "clwWidth" */
     341,	/* 310:  22   9   0, "clftsWidth" */
     331,	/* 311:  22  10   0, "ClmNone" */
     332,	/* 312:  22  10   1, "clmgf" */
     333,	/* 313:  22  10   2, "clmrg" */
     334,	/* 314:  22  11   0, "ClvmNone" */
     335,	/* 315:  22  11   1, "clvmgf" */
     336,	/* 316:  22  11   2, "clvmrg" */
     337,	/* 317:  22  12   0, "clNoWrap" */
     320,	/* 318:  22  13   0, "clvertalt" */
     321,	/* 319:  22  13   1, "clvertalc" */
     322,	/* 320:  22  13   2, "clvertalb" */
     326,	/* 321:  22  14   0, "cltxlrtb" */
     327,	/* 322:  22  14   1, "cltxtbrl" */
     328,	/* 323:  22  14   2, "cltxbtlr" */
     329,	/* 324:  22  14   3, "cltxlrtbv" */
     330,	/* 325:  22  14   4, "cltxtbrlv" */
     348,	/* 326:  22  15   0, "clpadl" */
     347,	/* 327:  22  16   0, "clpadb" */
     346,	/* 328:  22  17   0, "clpadt" */
     349,	/* 329:  22  18   0, "clpadr" */
     352,	/* 330:  22  19   0, "clpadfl" */
     351,	/* 331:  22  20   0, "clpadfb" */
     350,	/* 332:  22  21   0, "clpadft" */
     353,	/* 333:  22  22   0, "clpadfr" */
     339,	/* 334:  22  23   0, "clshdrawnil" */
     338,	/* 335:  22  24   0, "clhdr" */
     315,	/* 336:  23   0   0, "clcbpat" */
     314,	/* 337:  23   1   0, "clcfpat" */
     316,	/* 338:  23   2   0, "clshdng" */
     302,	/* 339:  23   3   1, "clbghoriz" */
     303,	/* 340:  23   3   2, "clbgvert" */
     304,	/* 341:  23   3   3, "clbgfdiag" */
     305,	/* 342:  23   3   4, "clbgbdiag" */
     306,	/* 343:  23   3   5, "clbgcross" */
     307,	/* 344:  23   3   6, "clbgdcross" */
     308,	/* 345:  23   3   7, "clbgdkhor" */
     309,	/* 346:  23   3   8, "clbgdkvert" */
     310,	/* 347:  23   3   9, "clbgdkfdiag" */
     311,	/* 348:  23   3  10, "clbgdkbdiag" */
     312,	/* 349:  23   3  11, "clbgdkcross" */
     313,	/* 350:  23   3  12, "clbgdkdcross" */
     367,	/* 351:  24  -1   0, "trowd" */
     369,	/* 352:  24   0   0, "trgaph" */
     370,	/* 353:  24   1   0, "trleft" */
     371,	/* 354:  24   2   0, "trrh" */
     381,	/* 355:  24   3   0, "trbrdrt" */
     383,	/* 356:  24   4   0, "trbrdrl" */
     384,	/* 357:  24   5   0, "trbrdrr" */
     382,	/* 358:  24   6   0, "trbrdrb" */
     386,	/* 359:  24   7   0, "trbrdrv" */
     385,	/* 360:  24   8   0, "trbrdrh" */
     376,	/* 361:  24  11   0, "trql" */
     377,	/* 362:  24  11   1, "trqr" */
     378,	/* 363:  24  11   2, "trqc" */
     372,	/* 364:  24  12   0, "trhdr" */
     373,	/* 365:  24  13   0, "trkeep" */
     375,	/* 366:  24  14   0, "trautofit" */
     380,	/* 367:  24  15   0, "ltrrow" */
     379,	/* 368:  24  15   1, "rtlrow" */
     395,	/* 369:  24  16   0, "trwWidth" */
     396,	/* 370:  24  17   0, "trftsWidth" */
     399,	/* 371:  24  18   0, "trspdt" */
     397,	/* 372:  24  19   0, "trspdl" */
     398,	/* 373:  24  20   0, "trspdr" */
     400,	/* 374:  24  21   0, "trspdb" */
     407,	/* 375:  24  22   0, "trpaddt" */
     405,	/* 376:  24  23   0, "trpaddl" */
     406,	/* 377:  24  24   0, "trpaddr" */
     408,	/* 378:  24  25   0, "trpaddb" */
     403,	/* 379:  24  26   0, "trspdft" */
     401,	/* 380:  24  27   0, "trspdfl" */
     402,	/* 381:  24  28   0, "trspdfr" */
     404,	/* 382:  24  29   0, "trspdfb" */
     411,	/* 383:  24  30   0, "trpaddft" */
     409,	/* 384:  24  31   0, "trpaddfl" */
     410,	/* 385:  24  32   0, "trpaddfr" */
     412,	/* 386:  24  33   0, "trpaddfb" */
     421,	/* 387:  24  34   0, "trwWidthB" */
     422,	/* 388:  24  35   0, "trwWidthA" */
     423,	/* 389:  24  36   0, "trftsWidthB" */
     424,	/* 390:  24  37   0, "trftsWidthA" */
     454,	/* 391:  24  38   0, "tbllkborder" */
     455,	/* 392:  24  39   0, "tbllkshading" */
     456,	/* 393:  24  40   0, "tbllkfont" */
     457,	/* 394:  24  41   0, "tbllkcolor" */
     458,	/* 395:  24  42   0, "tbllkbestfit" */
     459,	/* 396:  24  43   0, "tbllkhdrrows" */
     460,	/* 397:  24  44   0, "tbllklastrow" */
     461,	/* 398:  24  45   0, "tbllkhdrcols" */
     462,	/* 399:  24  46   0, "tbllklastcol" */
     463,	/* 400:  24  47   0, "irow" */
     464,	/* 401:  24  48   0, "irowband" */
     465,	/* 402:  24  49   0, "ts" */
     466,	/* 403:  24  50   0, "lastrow" */
     467,	/* 404:  24  51   0, "trauth" */
     468,	/* 405:  24  52   0, "trflat" */
     374,	/* 406:  24  53   0, "trkeepfollow" */
     472,	/* 407:  24  57   0, "trsummary" */
     438,	/* 408:  25   0   0, "trcbpat" */
     437,	/* 409:  25   1   0, "trcfpat" */
     439,	/* 410:  25   2   0, "trshdng" */
     200,	/* 411:  26   3   0, "tabsnoovrlp" */
     201,	/* 412:  26   4   0, "tphmrg" */
     202,	/* 413:  26   4   1, "tphpg" */
     203,	/* 414:  26   4   2, "tphcol" */
     204,	/* 415:  26   5   0, "tposx" */
     206,	/* 416:  26   6   1, "tposxl" */
     208,	/* 417:  26   6   2, "tposxc" */
     207,	/* 418:  26   6   3, "tposxr" */
     209,	/* 419:  26   6   4, "tposxi" */
     210,	/* 420:  26   6   5, "tposxo" */
     211,	/* 421:  26   7   0, "tpvmrg" */
     213,	/* 422:  26   7   1, "tpvpg" */
     212,	/* 423:  26   7   2, "tpvpara" */
     214,	/* 424:  26   7   3, "tposyil" */
     215,	/* 425:  26   8   0, "tposy" */
     221,	/* 426:  26   9   1, "tposyt" */
     218,	/* 427:  26   9   2, "tposyc" */
     217,	/* 428:  26   9   3, "tposyb" */
     219,	/* 429:  26   9   4, "tposyin" */
     220,	/* 430:  26   9   5, "tposyout" */
     222,	/* 431:  26  12   0, "tdfrmtxtLeft" */
     223,	/* 432:  26  13   0, "tdfrmtxtRight" */
     224,	/* 433:  26  14   0, "tdfrmtxtTop" */
     225,	/* 434:  26  15   0, "tdfrmtxtBottom" */
     582,	/* 435:  27  -1   0, "sectd" */
     585,	/* 436:  27   0   0, "pgwsxn" */
     586,	/* 437:  27   1   0, "pghsxn" */
     587,	/* 438:  27   2   0, "marglsxn" */
     589,	/* 439:  27   3   0, "margrsxn" */
     588,	/* 440:  27   4   0, "margtsxn" */
     590,	/* 441:  27   5   0, "margbsxn" */
     583,	/* 442:  27   6   0, "headery" */
     584,	/* 443:  27   7   0, "footery" */
     591,	/* 444:  27   8   0, "guttersxn" */
     592,	/* 445:  27   9   0, "margmirsxn" */
     593,	/* 446:  27  10   0, "ds" */
     594,	/* 447:  27  11   0, "titlepg" */
     595,	/* 448:  27  12   0, "endpg" */
     600,	/* 449:  27  13   0, "sbknone" */
     601,	/* 450:  27  13   1, "sbkcol" */
     602,	/* 451:  27  13   2, "sbkpage" */
     603,	/* 452:  27  13   3, "sbkeven" */
     604,	/* 453:  27  13   4, "sbkodd" */
     605,	/* 454:  27  14   0, "pgndec" */
     606,	/* 455:  27  14   1, "pgnucrm" */
     607,	/* 456:  27  14   2, "pgnlcrm" */
     608,	/* 457:  27  14   3, "pgnucltr" */
     609,	/* 458:  27  14   4, "pgnlcltr" */
     621,	/* 459:  27  15   0, "pgnhnsh" */
     622,	/* 460:  27  15   1, "pgnhnsp" */
     623,	/* 461:  27  15   2, "pgnhnsc" */
     624,	/* 462:  27  15   3, "pgnhnsm" */
     625,	/* 463:  27  15   4, "pgnhnsn" */
     610,	/* 464:  27  16   0, "pgncont" */
     611,	/* 465:  27  16   1, "pgnrestart" */
     613,	/* 466:  27  17   0, "ltrsect" */
     612,	/* 467:  27  17   1, "rtlsect" */
     596,	/* 468:  27  18   0, "sfsthdnopdfart" */
     597,	/* 469:  27  19   0, "sfstftnopdfart" */
     598,	/* 470:  27  20   0, "slsthdnopdfart" */
     599,	/* 471:  27  21   0, "slsftdnopdfart" */
     614,	/* 472:  27  22   0, "pgnstarts" */
     615,	/* 473:  27  23   0, "cols" */
     616,	/* 474:  27  24   0, "colsx" */
     620,	/* 475:  27  25   0, "linebetcol" */
     648,	/* 476:  27  27   0, "sftnstart" */
     646,	/* 477:  27  28   0, "sftntj" */
     647,	/* 478:  27  28   1, "sftnbj" */
     652,	/* 479:  27  29   0, "sftnrstcont" */
     651,	/* 480:  27  29   1, "sftnrestart" */
     650,	/* 481:  27  29   2, "sftnrstpg" */
     655,	/* 482:  27  30   0, "sftnnar" */
     656,	/* 483:  27  30   1, "sftnnalc" */
     657,	/* 484:  27  30   2, "sftnnauc" */
     658,	/* 485:  27  30   3, "sftnnrlc" */
     659,	/* 486:  27  30   4, "sftnnruc" */
     660,	/* 487:  27  30   5, "sftnnchi" */
     649,	/* 488:  27  31   0, "saftnstart" */
     653,	/* 489:  27  32   0, "saftnrstcont" */
     654,	/* 490:  27  32   1, "saftnrestart" */
     661,	/* 491:  27  33   0, "saftnnar" */
     662,	/* 492:  27  33   1, "saftnnalc" */
     663,	/* 493:  27  33   2, "saftnnauc" */
     664,	/* 494:  27  33   3, "saftnnrlc" */
     665,	/* 495:  27  33   4, "saftnnruc" */
     666,	/* 496:  27  33   5, "saftnnchi" */
     617,	/* 497:  27  35   0, "colno" */
     619,	/* 498:  27  36   0, "colw" */
     618,	/* 499:  27  37   0, "colsr" */
     478,	/* 500:  28   0   0, "paperw" */
     477,	/* 501:  28   1   0, "paperh" */
     473,	/* 502:  28   2   0, "margl" */
     474,	/* 503:  28   3   0, "margr" */
     475,	/* 504:  28   4   0, "margt" */
     476,	/* 505:  28   5   0, "margb" */
     479,	/* 506:  28   8   0, "gutter" */
     480,	/* 507:  28   9   0, "margmirror" */
     509,	/* 508:  28  10   0, "pgnstart" */
     481,	/* 509:  28  11   0, "facingp" */
     482,	/* 510:  28  12   0, "gutterprl" */
     483,	/* 511:  28  13   0, "widowctrl" */
     484,	/* 512:  28  14   0, "twoonone" */
     485,	/* 513:  28  15   0, "doctemp" */
     487,	/* 514:  28  16   0, "ltrdoc" */
     486,	/* 515:  28  16   1, "rtldoc" */
     667,	/* 516:  28  17   0, "fet" */
     676,	/* 517:  28  18   0, "ftnstart" */
     670,	/* 518:  28  19   0, "ftntj" */
     671,	/* 519:  28  19   1, "ftnbj" */
     668,	/* 520:  28  20   1, "endnotes" */
     669,	/* 521:  28  20   2, "enddoc" */
     680,	/* 522:  28  21   0, "ftnrstcont" */
     679,	/* 523:  28  21   1, "ftnrestart" */
     678,	/* 524:  28  21   2, "ftnrstpg" */
     683,	/* 525:  28  22   0, "ftnnar" */
     684,	/* 526:  28  22   1, "ftnnalc" */
     685,	/* 527:  28  22   2, "ftnnauc" */
     686,	/* 528:  28  22   3, "ftnnrlc" */
     687,	/* 529:  28  22   4, "ftnnruc" */
     688,	/* 530:  28  22   5, "ftnnchi" */
     677,	/* 531:  28  23   0, "aftnstart" */
     674,	/* 532:  28  24   0, "aftntj" */
     675,	/* 533:  28  24   1, "aftnbj" */
     672,	/* 534:  28  25   1, "aendnotes" */
     673,	/* 535:  28  25   2, "aenddoc" */
     681,	/* 536:  28  26   0, "aftnrstcont" */
     682,	/* 537:  28  26   1, "aftnrestart" */
     689,	/* 538:  28  27   0, "aftnnar" */
     690,	/* 539:  28  27   1, "aftnnalc" */
     691,	/* 540:  28  27   2, "aftnnauc" */
     692,	/* 541:  28  27   3, "aftnnrlc" */
     693,	/* 542:  28  27   4, "aftnnruc" */
     694,	/* 543:  28  27   5, "aftnnchi" */
     510,	/* 544:  28  28   0, "deftab" */
     710,	/* 545:  28  29   0, "ansi" */
     711,	/* 546:  28  29   1, "mac" */
     712,	/* 547:  28  29   2, "pc" */
     713,	/* 548:  28  29   3, "pca" */
     714,	/* 549:  28  30   0, "ansicpg" */
     715,	/* 550:  28  31   0, "deflang" */
     511,	/* 551:  28  32   0, "deff" */
     512,	/* 552:  28  33   0, "stshfdbch" */
     513,	/* 553:  28  34   0, "stshfloch" */
     514,	/* 554:  28  35   0, "stshfhich" */
     515,	/* 555:  28  36   0, "stshfbi" */
    1123,	/* 556:  28  37   0, "generator" */
    1115,	/* 557:  28  38   0, "title" */
    1118,	/* 558:  28  39   0, "subject" */
    1119,	/* 559:  28  40   0, "keywords" */
    1121,	/* 560:  28  41   0, "doccomm" */
    1116,	/* 561:  28  42   0, "author" */
    1117,	/* 562:  28  43   0, "company" */
    1122,	/* 563:  28  44   0, "hlinkbase" */
    1124,	/* 564:  28  45   0, "creatim" */
    1125,	/* 565:  28  46   0, "revtim" */
    1126,	/* 566:  28  47   0, "printim" */
     516,	/* 567:  28  48   0, "pgbrdrt" */
     517,	/* 568:  28  49   0, "pgbrdrb" */
     518,	/* 569:  28  50   0, "pgbrdrl" */
     519,	/* 570:  28  51   0, "pgbrdrr" */
     520,	/* 571:  28  52   0, "pgbrdrhead" */
     521,	/* 572:  28  53   0, "pgbrdrfoot" */
    1038,	/* 573:  28  54   0, "fonttbl" */
    1039,	/* 574:  28  55   0, "colortbl" */
    1040,	/* 575:  28  56   0, "stylesheet" */
    1041,	/* 576:  28  57   0, "listtable" */
    1042,	/* 577:  28  58   0, "listoverridetable" */
    1043,	/* 578:  28  61   0, "info" */
    1044,	/* 579:  28  62   0, "revtbl" */
    1127,	/* 580:  28  63   0, "operator" */
    1128,	/* 581:  28  64   0, "version" */
    1129,	/* 582:  28  65   0, "edmins" */
    1130,	/* 583:  28  66   0, "nofpages" */
    1131,	/* 584:  28  67   0, "nofwords" */
    1132,	/* 585:  28  68   0, "nofchars" */
    1133,	/* 586:  28  69   0, "nofcharsws" */
    1134,	/* 587:  28  70   0, "vern" */
     822,	/* 588:  29  57   0, "list" */
     845,	/* 589:  30   0   0, "listid" */
     846,	/* 590:  30   1   0, "listtemplateid" */
     847,	/* 591:  30   2   0, "listsimple" */
     848,	/* 592:  30   2   1, "listhybrid" */
     849,	/* 593:  30   3   0, "listrestarthdn" */
     850,	/* 594:  30   4   0, "liststyleid" */
     827,	/* 595:  30   5   0, "liststylename" */
     826,	/* 596:  30   6   0, "listname" */
     823,	/* 597:  30   7   0, "listlevel" */
     828,	/* 598:  31   0   0, "levelstartat" */
     829,	/* 599:  31   1   0, "levelnfcn" */
     831,	/* 600:  31   2   0, "leveljcn" */
     833,	/* 601:  31   3   0, "levelfollow" */
     834,	/* 602:  31   4   0, "levellegal" */
     835,	/* 603:  31   5   0, "levelnorestart" */
     836,	/* 604:  31   6   0, "levelpicture" */
     837,	/* 605:  31   7   0, "fbias" */
     824,	/* 606:  31   8   0, "leveltext" */
     839,	/* 607:  31   9   0, "leveltemplateid" */
     825,	/* 608:  31  10   0, "levelnumbers" */
     851,	/* 609:  33   2   0, "listoverridecount" */
     852,	/* 610:  34   0   0, "listoverridestartat" */
     853,	/* 611:  34   1   0, "listoverrideformat" */
    1109,	/* 612:  35   0   0, "bkmkstart" */
    1110,	/* 613:  35   1   0, "bkmkend" */
    1049,	/* 614:  35   2   0, "shppict" */
    1048,	/* 615:  35   3   0, "nonshppict" */
    1046,	/* 616:  35   4   0, "pict" */
    1047,	/* 617:  35   5   0, "do" */
    1050,	/* 618:  35   6   0, "object" */
    1051,	/* 619:  36 203   0, "shp" */
    1052,	/* 620:  36 204   0, "shpgrp" */
     935,	/* 621:  37  32   0, "emspace" */
     938,	/* 622:  37  34   0, "ldblquote" */
     941,	/* 623:  37  39   0, "rquote" */
     939,	/* 624:  37  96   0, "lquote" */
     948,	/* 625:  37 117   0, "u" */
     934,	/* 626:  37 8212   0, "emdash" */
     933,	/* 627:  37 8226   0, "bullet" */
     950,	/* 628:  38   2   0, "tab" */
     951,	/* 629:  38   6   0, "line" */
     957,	/* 630:  38   7   0, "page" */
     958,	/* 631:  38   8   0, "column" */
     953,	/* 632:  38   9   0, "chftnsep" */
     954,	/* 633:  38  10   0, "chftnsepc" */
     949,	/* 634:  38  11   0, "-" */
     955,	/* 635:  38  12   0, "ltrmark" */
     956,	/* 636:  38  13   0, "rtlmark" */
    1058,	/* 637:  39   0   0, "fftype" */
    1059,	/* 638:  39   1   0, "ffownhelp" */
    1060,	/* 639:  39   2   0, "ffownstat" */
    1061,	/* 640:  39   3   0, "ffprot" */
    1062,	/* 641:  39   4   0, "ffsize" */
    1063,	/* 642:  39   5   0, "fftypetxt" */
    1064,	/* 643:  39   6   0, "ffrecalc" */
    1065,	/* 644:  39   7   0, "ffhaslistbox" */
    1066,	/* 645:  39   8   0, "ffmaxlen" */
    1067,	/* 646:  39   9   0, "ffhps" */
    1068,	/* 647:  39  10   0, "ffdefres" */
    1069,	/* 648:  39  11   0, "ffname" */
    1070,	/* 649:  39  12   0, "ffdeftext" */
    1071,	/* 650:  39  13   0, "ffformat" */
    1072,	/* 651:  39  14   0, "ffhelptext" */
    1073,	/* 652:  39  15   0, "ffstattext" */
    1074,	/* 653:  39  16   0, "ffentrymcr" */
    1075,	/* 654:  39  17   0, "ffexitmcr" */
    1167,	/* 655:  40   0   0, "brdrcf" */
    1168,	/* 656:  40   1   0, "brsp" */
    1166,	/* 657:  40   2   0, "brdrw" */
    1170,	/* 658:  40   3   0, "brdrnone" */
    1171,	/* 659:  40   3   1, "brdrs" */
    1172,	/* 660:  40   3   2, "brdrth" */
    1173,	/* 661:  40   3   3, "brdrsh" */
    1174,	/* 662:  40   3   4, "brdrdb" */
    1175,	/* 663:  40   3   5, "brdrhair" */
    1176,	/* 664:  40   3   6, "brdrdot" */
    1177,	/* 665:  40   3   7, "brdrdash" */
    1178,	/* 666:  40   3   8, "brdrdashd" */
    1179,	/* 667:  40   3   9, "brdrdashdd" */
    1180,	/* 668:  40   3  10, "brdrtriple" */
    1181,	/* 669:  40   3  11, "brdrtnthsg" */
    1182,	/* 670:  40   3  12, "brdrthtnsg" */
    1183,	/* 671:  40   3  13, "brdrtnthtnsg" */
    1184,	/* 672:  40   3  14, "brdrtnthmg" */
    1185,	/* 673:  40   3  15, "brdrthtnmg" */
    1186,	/* 674:  40   3  16, "brdrtnthtnmg" */
    1187,	/* 675:  40   3  17, "brdrtnthlg" */
    1188,	/* 676:  40   3  18, "brdrthtnlg" */
    1189,	/* 677:  40   3  19, "brdrtnthtnlg" */
    1190,	/* 678:  40   3  20, "brdrwavy" */
    1191,	/* 679:  40   3  21, "brdrwavydb" */
    1192,	/* 680:  40   3  22, "brdrdashsm" */
    1193,	/* 681:  40   3  23, "brdrdashdotstr" */
    1194,	/* 682:  40   3  24, "brdremboss" */
    1195,	/* 683:  40   3  25, "brdrengrave" */
    1196,	/* 684:  40   3  26, "brdrframe" */
    1197,	/* 685:  40   3  27, "brdroutset" */
    1198,	/* 686:  40   3  28, "brdrinset" */
    1199,	/* 687:  40   3  29, "brdrtbl" */
    1200,	/* 688:  40   3  30, "brdrnil" */
    1169,	/* 689:  40   4   0, "brdrart" */
    1135,	/* 690:  41   0   0, "OTT" */
    1136,	/* 691:  41   1   0, "OSN" */
    1137,	/* 692:  41   2   0, "OOSN" */
    1138,	/* 693:  41   3   0, "OON" */
    1139,	/* 694:  41   4   0, "OPG" */
    1140,	/* 695:  41   5   0, "OCO" */
    1141,	/* 696:  41   6   0, "NTT" */
    1142,	/* 697:  41   7   0, "NSN" */
    1143,	/* 698:  41   8   0, "NOSN" */
    1144,	/* 699:  41   9   0, "NON" */
    1145,	/* 700:  41  10   0, "NPG" */
    1146,	/* 701:  41  11   0, "NCO" */
    1147,	/* 702:  42   0   0, "OHC" */
    1148,	/* 703:  42   1   0, "OHP" */
    1149,	/* 704:  42   2   0, "OHS" */
    1150,	/* 705:  42   3   0, "OTC" */
    1151,	/* 706:  42   4   0, "OTP" */
    1152,	/* 707:  42   5   0, "OTS" */
    1153,	/* 708:  42   7   0, "NHP" */
    1154,	/* 709:  42   8   0, "NHS" */
    1155,	/* 710:  42  10   0, "NTP" */
    1156,	/* 711:  42  11   0, "NTS" */
    1157,	/* 712:  42  12   0, "SEH" */
    1158,	/* 713:  42  12   1, "SEN" */
    1159,	/* 714:  42  12   2, "SAL" */
    1160,	/* 715:  42  12   3, "SET" */
    1161,	/* 716:  42  12   4, "SEB" */
    1162,	/* 717:  42  12   5, "SEA" */
    1163,	/* 718:  42  13   0, "FLK" */
    1164,	/* 719:  42  14   0, "PRL" */
    1111,	/* 720:  43   1   0, "sp" */
    1112,	/* 721:  43   2   0, "sn" */
    1113,	/* 722:  43   3   0, "sv" */
     808,	/* 723:  44   0   0, "hr" */
     809,	/* 724:  44   1   0, "min" */
     810,	/* 725:  44   2   0, "sec" */
     811,	/* 726:  44   3   0, "dy" */
     812,	/* 727:  44   4   0, "mo" */
     813,	/* 728:  44   5   0, "yr" */
};

const int docRtfTagLookupCount=\
		sizeof(docRtfTagLookup)/sizeof(int);
