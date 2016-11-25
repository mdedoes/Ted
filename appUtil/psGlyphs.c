/************************************************************************/
/*									*/
/*  Font Encodings							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

/************************************************************************/
/*									*/
/*  Indirection for variable names.					*/
/*									*/
/************************************************************************/

static const char GLYPH_A[]=			"A";
static const char GLYPH_AE[]=			"AE";
static const char GLYPH_Aacute[]=		"Aacute";
static const char GLYPH_Abreve[]=		"Abreve";
static const char GLYPH_Acircumflex[]=		"Acircumflex";
static const char GLYPH_Adiaeresis[]=		"Adiaeresis";
static const char GLYPH_Adieresis[]=		"Adieresis";
static const char GLYPH_Agrave[]=		"Agrave";
static const char GLYPH_Alpha[]=		"Alpha";
static const char GLYPH_Alphatonos[]=		"Alphatonos";
static const char GLYPH_Aogonek[]=		"Aogonek";
static const char GLYPH_Aring[]=		"Aring";
static const char GLYPH_Atilde[]=		"Atilde";
static const char GLYPH_B[]=			"B";
static const char GLYPH_Beta[]=			"Beta";
static const char GLYPH_C[]=			"C";
static const char GLYPH_Cacute[]=		"Cacute";
static const char GLYPH_Ccaron[]=		"Ccaron";
static const char GLYPH_Ccedilla[]=		"Ccedilla";
static const char GLYPH_Chi[]=			"Chi";
static const char GLYPH_D[]=			"D";
static const char GLYPH_Dcaron[]=		"Dcaron";
static const char GLYPH_Dcroat[]=		"Dcroat";
static const char GLYPH_Delta[]=		"Delta";
static const char GLYPH_Dslash[]=		"Dslash";
static const char GLYPH_Dstroke[]=		"Dstroke";
static const char GLYPH_E[]=			"E";
static const char GLYPH_Eacute[]=		"Eacute";
static const char GLYPH_Ecaron[]=		"Ecaron";
static const char GLYPH_Ecircumflex[]=		"Ecircumflex";
static const char GLYPH_Ediaeresis[]=		"Ediaeresis";
static const char GLYPH_Edieresis[]=		"Edieresis";
static const char GLYPH_Egrave[]=		"Egrave";
static const char GLYPH_Eogonek[]=		"Eogonek";
static const char GLYPH_Epsilon[]=		"Epsilon";
static const char GLYPH_Epsilontonos[]=		"Epsilontonos";
static const char GLYPH_Eta[]=			"Eta";
static const char GLYPH_Etatonos[]=		"Etatonos";
static const char GLYPH_Eth[]=			"Eth";
static const char GLYPH_F[]=			"F";
static const char GLYPH_G[]=			"G";
static const char GLYPH_Gamma[]=		"Gamma";
static const char GLYPH_H[]=			"H";
static const char GLYPH_I[]=			"I";
static const char GLYPH_Iacute[]=		"Iacute";
static const char GLYPH_Icircumflex[]=		"Icircumflex";
static const char GLYPH_Idiaeresis[]=		"Idiaeresis";
static const char GLYPH_Idieresis[]=		"Idieresis";
static const char GLYPH_Ifraktur[]=		"Ifraktur";
static const char GLYPH_Igrave[]=		"Igrave";
static const char GLYPH_Iota[]=			"Iota";
static const char GLYPH_Iotadieresis[]=		"Iotadieresis";
static const char GLYPH_Iotatonos[]=		"Iotatonos";
static const char GLYPH_J[]=			"J";
static const char GLYPH_K[]=			"K";
static const char GLYPH_Kappa[]=		"Kappa";
static const char GLYPH_L[]=			"L";
static const char GLYPH_Lacute[]=		"Lacute";
static const char GLYPH_Lambda[]=		"Lambda";
static const char GLYPH_Lcaron[]=		"Lcaron";
static const char GLYPH_Lslash[]=		"Lslash";
static const char GLYPH_Lstroke[]=		"Lstroke";
static const char GLYPH_M[]=			"M";
static const char GLYPH_Mu[]=			"Mu";
static const char GLYPH_N[]=			"N";
static const char GLYPH_Nacute[]=		"Nacute";
static const char GLYPH_Ncaron[]=		"Ncaron";
static const char GLYPH_Ntilde[]=		"Ntilde";
static const char GLYPH_Nu[]=			"Nu";
static const char GLYPH_O[]=			"O";
static const char GLYPH_Oacute[]=		"Oacute";
static const char GLYPH_Ocircumflex[]=		"Ocircumflex";
static const char GLYPH_Odiaeresis[]=		"Odiaeresis";
static const char GLYPH_Odieresis[]=		"Odieresis";
static const char GLYPH_Odoubleacute[]=		"Odoubleacute";
static const char GLYPH_OE[]=			"OE";
static const char GLYPH_Ograve[]=		"Ograve";
static const char GLYPH_Ohungarumlaut[]=	"Ohungarumlaut";
static const char GLYPH_Omega[]=		"Omega";
static const char GLYPH_Omegatonos[]=		"Omegatonos";
static const char GLYPH_Omicron[]=		"Omicron";
static const char GLYPH_Omicrontonos[]=		"Omicrontonos";
static const char GLYPH_Oslash[]=		"Oslash";
static const char GLYPH_Otilde[]=		"Otilde";
static const char GLYPH_P[]=			"P";
static const char GLYPH_Phi[]=			"Phi";
static const char GLYPH_Pi[]=			"Pi";
static const char GLYPH_Psi[]=			"Psi";
static const char GLYPH_Q[]=			"Q";
static const char GLYPH_R[]=			"R";
static const char GLYPH_Racute[]=		"Racute";
static const char GLYPH_Rcaron[]=		"Rcaron";
static const char GLYPH_Rfraktur[]=		"Rfraktur";
static const char GLYPH_Rho[]=			"Rho";
static const char GLYPH_S[]=			"S";
static const char GLYPH_Sacute[]=		"Sacute";
static const char GLYPH_Scaron[]=		"Scaron";
static const char GLYPH_Scedilla[]=		"Scedilla";
static const char GLYPH_Sigma[]=		"Sigma";
static const char GLYPH_T[]=			"T";
static const char GLYPH_Tau[]=			"Tau";
static const char GLYPH_Tcaron[]=		"Tcaron";
static const char GLYPH_Tcedilla[]=		"Tcedilla";
static const char GLYPH_Tcommaaccent[]=		"Tcommaaccent";
static const char GLYPH_Theta[]=		"Theta";
static const char GLYPH_Thorn[]=		"Thorn";
static const char GLYPH_U[]=			"U";
static const char GLYPH_Uacute[]=		"Uacute";
static const char GLYPH_Ucircumflex[]=		"Ucircumflex";
static const char GLYPH_Udiaeresis[]=		"Udiaeresis";
static const char GLYPH_Udoubleacute[]=		"Udoubleacute";
static const char GLYPH_Udieresis[]=		"Udieresis";
static const char GLYPH_Ugrave[]=		"Ugrave";
static const char GLYPH_Uhungarumlaut[]=	"Uhungarumlaut";
static const char GLYPH_Upsilon[]=		"Upsilon";
static const char GLYPH_Upsilondieresis[]=	"Upsilondieresis";
static const char GLYPH_Upsilontonos[]=		"Upsilontonos";
static const char GLYPH_Upsilon1[]=		"Upsilon1";
static const char GLYPH_Uring[]=		"Uring";
static const char GLYPH_V[]=			"V";
static const char GLYPH_W[]=			"W";
static const char GLYPH_X[]=			"X";
static const char GLYPH_Xi[]=			"Xi";
static const char GLYPH_Y[]=			"Y";
static const char GLYPH_Yacute[]=		"Yacute";
static const char GLYPH_Z[]=			"Z";
static const char GLYPH_Zabovedot[]=		"Zabovedot";
static const char GLYPH_Zacute[]=		"Zacute";
static const char GLYPH_Zcaron[]=		"Zcaron";
static const char GLYPH_Zdotaccent[]=		"Zdotaccent";
static const char GLYPH_Zeta[]=			"Zeta";
static const char GLYPH_a[]=			"a";
static const char GLYPH_aacute[]=		"aacute";
static const char GLYPH_abovedot[]=		"abovedot";
static const char GLYPH_abreve[]=		"abreve";
static const char GLYPH_acircumflex[]=		"acircumflex";
static const char GLYPH_acute[]=		"acute";
static const char GLYPH_adiaeresis[]=		"adiaeresis";
static const char GLYPH_adieresis[]=		"adieresis";
static const char GLYPH_ae[]=			"ae";
static const char GLYPH_agrave[]=		"agrave";
static const char GLYPH_aleph[]=		"aleph";
static const char GLYPH_alpha[]=		"alpha";
static const char GLYPH_alphatonos[]=		"alphatonos";
static const char GLYPH_ampersand[]=		"ampersand";
static const char GLYPH_angle[]=		"angle";
static const char GLYPH_angleleft[]=		"angleleft";
static const char GLYPH_angleright[]=		"angleright";
static const char GLYPH_aogonek[]=		"aogonek";
static const char GLYPH_apostrophe[]=		"apostrophe";
static const char GLYPH_approxequal[]=		"approxequal";
static const char GLYPH_aring[]=		"aring";
static const char GLYPH_arrowboth[]=		"arrowboth";
static const char GLYPH_arrowdblboth[]=		"arrowdblboth";
static const char GLYPH_arrowdbldown[]=		"arrowdbldown";
static const char GLYPH_arrowdblleft[]=		"arrowdblleft";
static const char GLYPH_arrowdblright[]=	"arrowdblright";
static const char GLYPH_arrowdblup[]=		"arrowdblup";
static const char GLYPH_arrowdown[]=		"arrowdown";
static const char GLYPH_arrowhorizex[]=		"arrowhorizex";
static const char GLYPH_arrowleft[]=		"arrowleft";
static const char GLYPH_arrowright[]=		"arrowright";
static const char GLYPH_arrowup[]=		"arrowup";
static const char GLYPH_arrowvertex[]=		"arrowvertex";
static const char GLYPH_asciicircum[]=		"asciicircum";
static const char GLYPH_asciitilde[]=		"asciitilde";
static const char GLYPH_asterisk[]=		"asterisk";
static const char GLYPH_asteriskmath[]=		"asteriskmath";
static const char GLYPH_at[]=			"at";
static const char GLYPH_atilde[]=		"atilde";
static const char GLYPH_b[]=			"b";
static const char GLYPH_backslash[]=		"backslash";
static const char GLYPH_bar[]=			"bar";
static const char GLYPH_beta[]=			"beta";
static const char GLYPH_braceex[]=		"braceex";
static const char GLYPH_braceleft[]=		"braceleft";
static const char GLYPH_braceleftbt[]=		"braceleftbt";
static const char GLYPH_braceleftmid[]=		"braceleftmid";
static const char GLYPH_bracelefttp[]=		"bracelefttp";
static const char GLYPH_braceright[]=		"braceright";
static const char GLYPH_bracerightbt[]=		"bracerightbt";
static const char GLYPH_bracerightmid[]=	"bracerightmid";
static const char GLYPH_bracerighttp[]=		"bracerighttp";
static const char GLYPH_bracketleft[]=		"bracketleft";
static const char GLYPH_bracketleftbt[]=	"bracketleftbt";
static const char GLYPH_bracketleftex[]=	"bracketleftex";
static const char GLYPH_bracketlefttp[]=	"bracketlefttp";
static const char GLYPH_bracketright[]=		"bracketright";
static const char GLYPH_bracketrightbt[]=	"bracketrightbt";
static const char GLYPH_bracketrightex[]=	"bracketrightex";
static const char GLYPH_bracketrighttp[]=	"bracketrighttp";
static const char GLYPH_breve[]=		"breve";
static const char GLYPH_brokenbar[]=		"brokenbar";
static const char GLYPH_bullet[]=		"bullet";
static const char GLYPH_c[]=			"c";
static const char GLYPH_cacute[]=		"cacute";
static const char GLYPH_caron[]=		"caron";
static const char GLYPH_carriagereturn[]=	"carriagereturn";
static const char GLYPH_ccaron[]=		"ccaron";
static const char GLYPH_ccedilla[]=		"ccedilla";
static const char GLYPH_cedilla[]=		"cedilla";
static const char GLYPH_cent[]=			"cent";
static const char GLYPH_chi[]=			"chi";
static const char GLYPH_circlemultiply[]=	"circlemultiply";
static const char GLYPH_circleplus[]=		"circleplus";
static const char GLYPH_circumflex[]=		"circumflex";
static const char GLYPH_club[]=			"club";
static const char GLYPH_colon[]=		"colon";
static const char GLYPH_comma[]=		"comma";
static const char GLYPH_congruent[]=		"congruent";
static const char GLYPH_copyright[]=		"copyright";
static const char GLYPH_copyrightsans[]=	"copyrightsans";
static const char GLYPH_copyrightserif[]=	"copyrightserif";
static const char GLYPH_currency[]=		"currency";
static const char GLYPH_d[]=			"d";
static const char GLYPH_dagger[]=		"dagger";
static const char GLYPH_daggerdbl[]=		"daggerdbl";
static const char GLYPH_dcaron[]=		"dcaron";
static const char GLYPH_dcroat[]=		"dcroat";
static const char GLYPH_dstroke[]=		"dstroke";
static const char GLYPH_degree[]=		"degree";
static const char GLYPH_delta[]=		"delta";
static const char GLYPH_diamond[]=		"diamond";
static const char GLYPH_diaeresis[]=		"diaeresis";
static const char GLYPH_dieresis[]=		"dieresis";
static const char GLYPH_divide[]=		"divide";
static const char GLYPH_division[]=		"division";
static const char GLYPH_dieresistonos[]=	"dieresistonos";
static const char GLYPH_dmacron[]=		"dmacron";
static const char GLYPH_dollar[]=		"dollar";
static const char GLYPH_dotaccent[]=		"dotaccent";
static const char GLYPH_dotlessi[]=		"dotlessi";
static const char GLYPH_dotmath[]=		"dotmath";
static const char GLYPH_doubleacute[]=		"doubleacute";
static const char GLYPH_e[]=			"e";
static const char GLYPH_eacute[]=		"eacute";
static const char GLYPH_ecaron[]=		"ecaron";
static const char GLYPH_ecircumflex[]=		"ecircumflex";
static const char GLYPH_ediaeresis[]=		"ediaeresis";
static const char GLYPH_edieresis[]=		"edieresis";
static const char GLYPH_egrave[]=		"egrave";
static const char GLYPH_eight[]=		"eight";
static const char GLYPH_element[]=		"element";
static const char GLYPH_ellipsis[]=		"ellipsis";
static const char GLYPH_emdash[]=		"emdash";
static const char GLYPH_emptyset[]=		"emptyset";
static const char GLYPH_endash[]=		"endash";
static const char GLYPH_eogonek[]=		"eogonek";
static const char GLYPH_epsilon[]=		"epsilon";
static const char GLYPH_epsilontonos[]=		"epsilontonos";
static const char GLYPH_equal[]=		"equal";
static const char GLYPH_equivalence[]=		"equivalence";
static const char GLYPH_eta[]=			"eta";
static const char GLYPH_etatonos[]=		"etatonos";
static const char GLYPH_eth[]=			"eth";
static const char GLYPH_exclam[]=		"exclam";
static const char GLYPH_exclamdown[]=		"exclamdown";
static const char GLYPH_existential[]=		"existential";
static const char GLYPH_f[]=			"f";
static const char GLYPH_fi[]=			"fi";
static const char GLYPH_five[]=			"five";
static const char GLYPH_fl[]=			"fl";
static const char GLYPH_florin[]=		"florin";
static const char GLYPH_four[]=			"four";
static const char GLYPH_fraction[]=		"fraction";
static const char GLYPH_fullstop[]=		"fullstop";
static const char GLYPH_g[]=			"g";
static const char GLYPH_gamma[]=		"gamma";
static const char GLYPH_germandbls[]=		"germandbls";
static const char GLYPH_gradient[]=		"gradient";
static const char GLYPH_grave[]=		"grave";
static const char GLYPH_greater[]=		"greater";
static const char GLYPH_greaterequal[]=		"greaterequal";
static const char GLYPH_guillemotleft[]=	"guillemotleft";
static const char GLYPH_guillemotright[]=	"guillemotright";
static const char GLYPH_guillsinglleft[]=	"guillsinglleft";
static const char GLYPH_guillsinglright[]=	"guillsinglright";
static const char GLYPH_guilsinglleft[]=	"guilsinglleft";
static const char GLYPH_guilsinglright[]=	"guilsinglright";
static const char GLYPH_h[]=			"h";
static const char GLYPH_heart[]=		"heart";
static const char GLYPH_hungarumlaut[]=		"hungarumlaut";
static const char GLYPH_hyphen[]=		"hyphen";
static const char GLYPH_i[]=			"i";
static const char GLYPH_iacute[]=		"iacute";
static const char GLYPH_icircumflex[]=		"icircumflex";
static const char GLYPH_idiaeresis[]=		"idiaeresis";
static const char GLYPH_idieresis[]=		"idieresis";
static const char GLYPH_igrave[]=		"igrave";
static const char GLYPH_infinity[]=		"infinity";
static const char GLYPH_integral[]=		"integral";
static const char GLYPH_integralbt[]=		"integralbt";
static const char GLYPH_integralex[]=		"integralex";
static const char GLYPH_integraltp[]=		"integraltp";
static const char GLYPH_intersection[]=		"intersection";
static const char GLYPH_iota[]=			"iota";
static const char GLYPH_iotadieresis[]=		"iotadieresis";
static const char GLYPH_iotadieresistonos[]=	"iotadieresistonos";
static const char GLYPH_iotatonos[]=		"iotatonos";
static const char GLYPH_j[]=			"j";
static const char GLYPH_k[]=			"k";
static const char GLYPH_kappa[]=		"kappa";
static const char GLYPH_l[]=			"l";
static const char GLYPH_lacute[]=		"lacute";
static const char GLYPH_lambda[]=		"lambda";
static const char GLYPH_lcaron[]=		"lcaron";
static const char GLYPH_less[]=			"less";
static const char GLYPH_lessequal[]=		"lessequal";
static const char GLYPH_logicaland[]=		"logicaland";
static const char GLYPH_logicalnot[]=		"logicalnot";
static const char GLYPH_logicalor[]=		"logicalor";
static const char GLYPH_lozenge[]=		"lozenge";
static const char GLYPH_lowline[]=		"lowline";
static const char GLYPH_lslash[]=		"lslash";
static const char GLYPH_lstroke[]=		"lstroke";
static const char GLYPH_m[]=			"m";
static const char GLYPH_macron[]=		"macron";
static const char GLYPH_minus[]=		"minus";
static const char GLYPH_minute[]=		"minute";
static const char GLYPH_mu[]=			"mu";
static const char GLYPH_mu1[]=			"mu1";
static const char GLYPH_multiply[]=		"multiply";
static const char GLYPH_n[]=			"n";
static const char GLYPH_nacute[]=		"nacute";
static const char GLYPH_ncaron[]=		"ncaron";
static const char GLYPH_nine[]=			"nine";
static const char GLYPH_nobreakspace[]=		"nobreakspace";
static const char GLYPH_notelement[]=		"notelement";
static const char GLYPH_notequal[]=		"notequal";
static const char GLYPH_notsubset[]=		"notsubset";
static const char GLYPH_ntilde[]=		"ntilde";
static const char GLYPH_nu[]=			"nu";
static const char GLYPH_numbersign[]=		"numbersign";
static const char GLYPH_o[]=			"o";
static const char GLYPH_oacute[]=		"oacute";
static const char GLYPH_ocircumflex[]=		"ocircumflex";
static const char GLYPH_odiaeresis[]=		"odiaeresis";
static const char GLYPH_odieresis[]=		"odieresis";
static const char GLYPH_odoubleacute[]=		"odoubleacute";
static const char GLYPH_oe[]=			"oe";
static const char GLYPH_ogonek[]=		"ogonek";
static const char GLYPH_ograve[]=		"ograve";
static const char GLYPH_ohungarumlaut[]=	"ohungarumlaut";
static const char GLYPH_omega[]=		"omega";
static const char GLYPH_omegatonos[]=		"omegatonos";
static const char GLYPH_omega1[]=		"omega1";
static const char GLYPH_omicron[]=		"omicron";
static const char GLYPH_omicrontonos[]=		"omicrontonos";
static const char GLYPH_one[]=			"one";
static const char GLYPH_onehalf[]=		"onehalf";
static const char GLYPH_onequarter[]=		"onequarter";
static const char GLYPH_onesuperior[]=		"onesuperior";
static const char GLYPH_ordfeminine[]=		"ordfeminine";
static const char GLYPH_ordmasculine[]=		"ordmasculine";
static const char GLYPH_oslash[]=		"oslash";
static const char GLYPH_otilde[]=		"otilde";
static const char GLYPH_overscore[]=		"overscore";
static const char GLYPH_p[]=			"p";
static const char GLYPH_paragraph[]=		"paragraph";
static const char GLYPH_parenleft[]=		"parenleft";
static const char GLYPH_parenleftbt[]=		"parenleftbt";
static const char GLYPH_parenleftex[]=		"parenleftex";
static const char GLYPH_parenlefttp[]=		"parenlefttp";
static const char GLYPH_parenright[]=		"parenright";
static const char GLYPH_parenrightbt[]=		"parenrightbt";
static const char GLYPH_parenrightex[]=		"parenrightex";
static const char GLYPH_parenrighttp[]=		"parenrighttp";
static const char GLYPH_partialdiff[]=		"partialdiff";
static const char GLYPH_percent[]=		"percent";
static const char GLYPH_perthousand[]=		"perthousand";
static const char GLYPH_period[]=		"period";
static const char GLYPH_periodcentered[]=	"periodcentered";
static const char GLYPH_perpendicular[]=	"perpendicular";
static const char GLYPH_phi[]=			"phi";
static const char GLYPH_phi1[]=			"phi1";
static const char GLYPH_pi[]=			"pi";
static const char GLYPH_plus[]=			"plus";
static const char GLYPH_plusminus[]=		"plusminus";
static const char GLYPH_product[]=		"product";
static const char GLYPH_propersubset[]=		"propersubset";
static const char GLYPH_propersuperset[]=	"propersuperset";
static const char GLYPH_proportional[]=		"proportional";
static const char GLYPH_psi[]=			"psi";
static const char GLYPH_q[]=			"q";
static const char GLYPH_question[]=		"question";
static const char GLYPH_questiondown[]=		"questiondown";
static const char GLYPH_quotedbl[]=		"quotedbl";
static const char GLYPH_quotedblbase[]=		"quotedblbase";
static const char GLYPH_quotedblleft[]=		"quotedblleft";
static const char GLYPH_quotedblright[]=	"quotedblright";
static const char GLYPH_quoteleft[]=		"quoteleft";
static const char GLYPH_quoteright[]=		"quoteright";
static const char GLYPH_quotesingle[]=		"quotesingle";
static const char GLYPH_quotesinglbase[]=	"quotesinglbase";
static const char GLYPH_r[]=			"r";
static const char GLYPH_racute[]=		"racute";
static const char GLYPH_radical[]=		"radical";
static const char GLYPH_radicalex[]=		"radicalex";
static const char GLYPH_rcaron[]=		"rcaron";
static const char GLYPH_reflexsubset[]=		"reflexsubset";
static const char GLYPH_reflexsuperset[]=	"reflexsuperset";
static const char GLYPH_registered[]=		"registered";
static const char GLYPH_registersans[]=		"registersans";
static const char GLYPH_registerserif[]=	"registerserif";
static const char GLYPH_rho[]=			"rho";
static const char GLYPH_ring[]=			"ring";
static const char GLYPH_s[]=			"s";
static const char GLYPH_sacute[]=		"sacute";
static const char GLYPH_scaron[]=		"scaron";
static const char GLYPH_scedilla[]=		"scedilla";
static const char GLYPH_second[]=		"second";
static const char GLYPH_section[]=		"section";
static const char GLYPH_semicolon[]=		"semicolon";
static const char GLYPH_seven[]=		"seven";
static const char GLYPH_sfthyphen[]=		"sfthyphen";
static const char GLYPH_sigma[]=		"sigma";
static const char GLYPH_sigma1[]=		"sigma1";
static const char GLYPH_similar[]=		"similar";
static const char GLYPH_six[]=			"six";
static const char GLYPH_slash[]=		"slash";
static const char GLYPH_space[]=		"space";
static const char GLYPH_spade[]=		"spade";
static const char GLYPH_ssharp[]=		"ssharp";
static const char GLYPH_sterling[]=		"sterling";
static const char GLYPH_suchthat[]=		"suchthat";
static const char GLYPH_summation[]=		"summation";
static const char GLYPH_t[]=			"t";
static const char GLYPH_tau[]=			"tau";
static const char GLYPH_tcaron[]=		"tcaron";
static const char GLYPH_tcedilla[]=		"tcedilla";
static const char GLYPH_tcommaaccent[]=		"tcommaaccent";
static const char GLYPH_therefore[]=		"therefore";
static const char GLYPH_theta[]=		"theta";
static const char GLYPH_theta1[]=		"theta1";
static const char GLYPH_thorn[]=		"thorn";
static const char GLYPH_three[]=		"three";
static const char GLYPH_threequarters[]=	"threequarters";
static const char GLYPH_threesuperior[]=	"threesuperior";
static const char GLYPH_tilde[]=		"tilde";
static const char GLYPH_tonos[]=		"tonos";
static const char GLYPH_trademark[]=		"trademark";
static const char GLYPH_trademarksans[]=	"trademarksans";
static const char GLYPH_trademarkserif[]=	"trademarkserif";
static const char GLYPH_two[]=			"two";
static const char GLYPH_twosuperior[]=		"twosuperior";
static const char GLYPH_u[]=			"u";
static const char GLYPH_uacute[]=		"uacute";
static const char GLYPH_ucircumflex[]=		"ucircumflex";
static const char GLYPH_udiaeresis[]=		"udiaeresis";
static const char GLYPH_udieresis[]=		"udieresis";
static const char GLYPH_udoubleacute[]=		"udoubleacute";
static const char GLYPH_ugrave[]=		"ugrave";
static const char GLYPH_uhungarumlaut[]=	"uhungarumlaut";
static const char GLYPH_underscore[]=		"underscore";
static const char GLYPH_union[]=		"union";
static const char GLYPH_universal[]=		"universal";
static const char GLYPH_upsilon[]=		"upsilon";
static const char GLYPH_upsilondieresis[]=	"upsilondieresis";
static const char GLYPH_upsilondieresistonos[]=	"upsilondieresistonos";
static const char GLYPH_upsilontonos[]=		"upsilontonos";
static const char GLYPH_uring[]=		"uring";
static const char GLYPH_v[]=			"v";
static const char GLYPH_w[]=			"w";
static const char GLYPH_weierstrass[]=		"weierstrass";
static const char GLYPH_x[]=			"x";
static const char GLYPH_xi[]=			"xi";
static const char GLYPH_y[]=			"y";
static const char GLYPH_yacute[]=		"yacute";
static const char GLYPH_ydiaeresis[]=		"ydiaeresis";
static const char GLYPH_ydieresis[]=		"ydieresis";
static const char GLYPH_yen[]=			"yen";
static const char GLYPH_z[]=			"z";
static const char GLYPH_zabovedot[]=		"zabovedot";
static const char GLYPH_zacute[]=		"zacute";
static const char GLYPH_zcaron[]=		"zcaron";
static const char GLYPH_zdotaccent[]=		"zdotaccent";
static const char GLYPH_zero[]=			"zero";
static const char GLYPH_zeta[]=			"zeta";

static const char GLYPH_afii00208[]=		"afii00208";
static const char GLYPH_afii10017[]=		"afii10017";
static const char GLYPH_afii10018[]=		"afii10018";
static const char GLYPH_afii10019[]=		"afii10019";
static const char GLYPH_afii10020[]=		"afii10020";
static const char GLYPH_afii10021[]=		"afii10021";
static const char GLYPH_afii10022[]=		"afii10022";
static const char GLYPH_afii10023[]=		"afii10023";
static const char GLYPH_afii10024[]=		"afii10024";
static const char GLYPH_afii10025[]=		"afii10025";
static const char GLYPH_afii10026[]=		"afii10026";
static const char GLYPH_afii10027[]=		"afii10027";
static const char GLYPH_afii10028[]=		"afii10028";
static const char GLYPH_afii10029[]=		"afii10029";
static const char GLYPH_afii10030[]=		"afii10030";
static const char GLYPH_afii10031[]=		"afii10031";
static const char GLYPH_afii10032[]=		"afii10032";
static const char GLYPH_afii10033[]=		"afii10033";
static const char GLYPH_afii10034[]=		"afii10034";
static const char GLYPH_afii10035[]=		"afii10035";
static const char GLYPH_afii10036[]=		"afii10036";
static const char GLYPH_afii10037[]=		"afii10037";
static const char GLYPH_afii10038[]=		"afii10038";
static const char GLYPH_afii10039[]=		"afii10039";
static const char GLYPH_afii10040[]=		"afii10040";
static const char GLYPH_afii10041[]=		"afii10041";
static const char GLYPH_afii10042[]=		"afii10042";
static const char GLYPH_afii10043[]=		"afii10043";
static const char GLYPH_afii10044[]=		"afii10044";
static const char GLYPH_afii10045[]=		"afii10045";
static const char GLYPH_afii10046[]=		"afii10046";
static const char GLYPH_afii10047[]=		"afii10047";
static const char GLYPH_afii10048[]=		"afii10048";
static const char GLYPH_afii10049[]=		"afii10049";
static const char GLYPH_afii10050[]=		"afii10050";
static const char GLYPH_afii10051[]=		"afii10051";
static const char GLYPH_afii10052[]=		"afii10052";
static const char GLYPH_afii10053[]=		"afii10053";
static const char GLYPH_afii10054[]=		"afii10054";
static const char GLYPH_afii10055[]=		"afii10055";
static const char GLYPH_afii10056[]=		"afii10056";
static const char GLYPH_afii10057[]=		"afii10057";
static const char GLYPH_afii10058[]=		"afii10058";
static const char GLYPH_afii10059[]=		"afii10059";
static const char GLYPH_afii10060[]=		"afii10060";
static const char GLYPH_afii10061[]=		"afii10061";
static const char GLYPH_afii10062[]=		"afii10062";
static const char GLYPH_afii10065[]=		"afii10065";
static const char GLYPH_afii10066[]=		"afii10066";
static const char GLYPH_afii10067[]=		"afii10067";
static const char GLYPH_afii10068[]=		"afii10068";
static const char GLYPH_afii10069[]=		"afii10069";
static const char GLYPH_afii10070[]=		"afii10070";
static const char GLYPH_afii10071[]=		"afii10071";
static const char GLYPH_afii10072[]=		"afii10072";
static const char GLYPH_afii10073[]=		"afii10073";
static const char GLYPH_afii10074[]=		"afii10074";
static const char GLYPH_afii10075[]=		"afii10075";
static const char GLYPH_afii10076[]=		"afii10076";
static const char GLYPH_afii10077[]=		"afii10077";
static const char GLYPH_afii10078[]=		"afii10078";
static const char GLYPH_afii10079[]=		"afii10079";
static const char GLYPH_afii10080[]=		"afii10080";
static const char GLYPH_afii10081[]=		"afii10081";
static const char GLYPH_afii10082[]=		"afii10082";
static const char GLYPH_afii10083[]=		"afii10083";
static const char GLYPH_afii10084[]=		"afii10084";
static const char GLYPH_afii10085[]=		"afii10085";
static const char GLYPH_afii10086[]=		"afii10086";
static const char GLYPH_afii10087[]=		"afii10087";
static const char GLYPH_afii10088[]=		"afii10088";
static const char GLYPH_afii10089[]=		"afii10089";
static const char GLYPH_afii10090[]=		"afii10090";
static const char GLYPH_afii10091[]=		"afii10091";
static const char GLYPH_afii10092[]=		"afii10092";
static const char GLYPH_afii10093[]=		"afii10093";
static const char GLYPH_afii10094[]=		"afii10094";
static const char GLYPH_afii10095[]=		"afii10095";
static const char GLYPH_afii10096[]=		"afii10096";
static const char GLYPH_afii10097[]=		"afii10097";
static const char GLYPH_afii10098[]=		"afii10098";
static const char GLYPH_afii10099[]=		"afii10099";
static const char GLYPH_afii10100[]=		"afii10100";
static const char GLYPH_afii10101[]=		"afii10101";
static const char GLYPH_afii10102[]=		"afii10102";
static const char GLYPH_afii10103[]=		"afii10103";
static const char GLYPH_afii10104[]=		"afii10104";
static const char GLYPH_afii10105[]=		"afii10105";
static const char GLYPH_afii10106[]=		"afii10106";
static const char GLYPH_afii10107[]=		"afii10107";
static const char GLYPH_afii10108[]=		"afii10108";
static const char GLYPH_afii10109[]=		"afii10109";
static const char GLYPH_afii10110[]=		"afii10110";
static const char GLYPH_afii10145[]=		"afii10145";
static const char GLYPH_afii10193[]=		"afii10193";
static const char GLYPH_afii61352[]=		"afii61352";
static const char GLYPH_afii57929[]=		"afii57929";
static const char GLYPH_afii64937[]=		"afii64937";

static const char GLYPH_a1[]=			"a1";
static const char GLYPH_a2[]=			"a2";
static const char GLYPH_a202[]=			"a202";
static const char GLYPH_a3[]=			"a3";
static const char GLYPH_a4[]=			"a4";
static const char GLYPH_a5[]=			"a5";
static const char GLYPH_a119[]=			"a119";
static const char GLYPH_a118[]=			"a118";
static const char GLYPH_a117[]=			"a117";
static const char GLYPH_a11[]=			"a11";
static const char GLYPH_a12[]=			"a12";
static const char GLYPH_a13[]=			"a13";
static const char GLYPH_a14[]=			"a14";
static const char GLYPH_a15[]=			"a15";
static const char GLYPH_a16[]=			"a16";
static const char GLYPH_a105[]=			"a105";
static const char GLYPH_a17[]=			"a17";
static const char GLYPH_a18[]=			"a18";
static const char GLYPH_a19[]=			"a19";
static const char GLYPH_a20[]=			"a20";
static const char GLYPH_a21[]=			"a21";
static const char GLYPH_a22[]=			"a22";
static const char GLYPH_a23[]=			"a23";
static const char GLYPH_a24[]=			"a24";
static const char GLYPH_a25[]=			"a25";
static const char GLYPH_a26[]=			"a26";
static const char GLYPH_a27[]=			"a27";
static const char GLYPH_a28[]=			"a28";
static const char GLYPH_a6[]=			"a6";
static const char GLYPH_a7[]=			"a7";
static const char GLYPH_a8[]=			"a8";
static const char GLYPH_a9[]=			"a9";
static const char GLYPH_a10[]=			"a10";
static const char GLYPH_a29[]=			"a29";
static const char GLYPH_a30[]=			"a30";
static const char GLYPH_a31[]=			"a31";
static const char GLYPH_a32[]=			"a32";
static const char GLYPH_a33[]=			"a33";
static const char GLYPH_a34[]=			"a34";
static const char GLYPH_a35[]=			"a35";
static const char GLYPH_a36[]=			"a36";
static const char GLYPH_a37[]=			"a37";
static const char GLYPH_a38[]=			"a38";
static const char GLYPH_a39[]=			"a39";
static const char GLYPH_a40[]=			"a40";
static const char GLYPH_a41[]=			"a41";
static const char GLYPH_a42[]=			"a42";
static const char GLYPH_a43[]=			"a43";
static const char GLYPH_a44[]=			"a44";
static const char GLYPH_a45[]=			"a45";
static const char GLYPH_a46[]=			"a46";
static const char GLYPH_a47[]=			"a47";
static const char GLYPH_a48[]=			"a48";
static const char GLYPH_a49[]=			"a49";
static const char GLYPH_a50[]=			"a50";
static const char GLYPH_a51[]=			"a51";
static const char GLYPH_a52[]=			"a52";
static const char GLYPH_a53[]=			"a53";
static const char GLYPH_a54[]=			"a54";
static const char GLYPH_a55[]=			"a55";
static const char GLYPH_a56[]=			"a56";
static const char GLYPH_a57[]=			"a57";
static const char GLYPH_a58[]=			"a58";
static const char GLYPH_a59[]=			"a59";
static const char GLYPH_a60[]=			"a60";
static const char GLYPH_a61[]=			"a61";
static const char GLYPH_a62[]=			"a62";
static const char GLYPH_a63[]=			"a63";
static const char GLYPH_a64[]=			"a64";
static const char GLYPH_a65[]=			"a65";
static const char GLYPH_a66[]=			"a66";
static const char GLYPH_a67[]=			"a67";
static const char GLYPH_a68[]=			"a68";
static const char GLYPH_a69[]=			"a69";
static const char GLYPH_a70[]=			"a70";
static const char GLYPH_a71[]=			"a71";
static const char GLYPH_a72[]=			"a72";
static const char GLYPH_a73[]=			"a73";
static const char GLYPH_a74[]=			"a74";
static const char GLYPH_a203[]=			"a203";
static const char GLYPH_a75[]=			"a75";
static const char GLYPH_a204[]=			"a204";
static const char GLYPH_a76[]=			"a76";
static const char GLYPH_a77[]=			"a77";
static const char GLYPH_a78[]=			"a78";
static const char GLYPH_a79[]=			"a79";
static const char GLYPH_a81[]=			"a81";
static const char GLYPH_a82[]=			"a82";
static const char GLYPH_a83[]=			"a83";
static const char GLYPH_a84[]=			"a84";
static const char GLYPH_a97[]=			"a97";
static const char GLYPH_a98[]=			"a98";
static const char GLYPH_a99[]=			"a99";
static const char GLYPH_a100[]=			"a100";
static const char GLYPH_a101[]=			"a101";
static const char GLYPH_a102[]=			"a102";
static const char GLYPH_a103[]=			"a103";
static const char GLYPH_a104[]=			"a104";
static const char GLYPH_a106[]=			"a106";
static const char GLYPH_a107[]=			"a107";
static const char GLYPH_a108[]=			"a108";
static const char GLYPH_a112[]=			"a112";
static const char GLYPH_a111[]=			"a111";
static const char GLYPH_a110[]=			"a110";
static const char GLYPH_a109[]=			"a109";
static const char GLYPH_a120[]=			"a120";
static const char GLYPH_a121[]=			"a121";
static const char GLYPH_a122[]=			"a122";
static const char GLYPH_a123[]=			"a123";
static const char GLYPH_a124[]=			"a124";
static const char GLYPH_a125[]=			"a125";
static const char GLYPH_a126[]=			"a126";
static const char GLYPH_a127[]=			"a127";
static const char GLYPH_a128[]=			"a128";
static const char GLYPH_a129[]=			"a129";
static const char GLYPH_a130[]=			"a130";
static const char GLYPH_a131[]=			"a131";
static const char GLYPH_a132[]=			"a132";
static const char GLYPH_a133[]=			"a133";
static const char GLYPH_a134[]=			"a134";
static const char GLYPH_a135[]=			"a135";
static const char GLYPH_a136[]=			"a136";
static const char GLYPH_a137[]=			"a137";
static const char GLYPH_a138[]=			"a138";
static const char GLYPH_a139[]=			"a139";
static const char GLYPH_a140[]=			"a140";
static const char GLYPH_a141[]=			"a141";
static const char GLYPH_a142[]=			"a142";
static const char GLYPH_a143[]=			"a143";
static const char GLYPH_a144[]=			"a144";
static const char GLYPH_a145[]=			"a145";
static const char GLYPH_a146[]=			"a146";
static const char GLYPH_a147[]=			"a147";
static const char GLYPH_a148[]=			"a148";
static const char GLYPH_a149[]=			"a149";
static const char GLYPH_a150[]=			"a150";
static const char GLYPH_a151[]=			"a151";
static const char GLYPH_a152[]=			"a152";
static const char GLYPH_a153[]=			"a153";
static const char GLYPH_a154[]=			"a154";
static const char GLYPH_a155[]=			"a155";
static const char GLYPH_a156[]=			"a156";
static const char GLYPH_a157[]=			"a157";
static const char GLYPH_a158[]=			"a158";
static const char GLYPH_a159[]=			"a159";
static const char GLYPH_a160[]=			"a160";
static const char GLYPH_a161[]=			"a161";
static const char GLYPH_a163[]=			"a163";
static const char GLYPH_a164[]=			"a164";
static const char GLYPH_a196[]=			"a196";
static const char GLYPH_a165[]=			"a165";
static const char GLYPH_a192[]=			"a192";
static const char GLYPH_a166[]=			"a166";
static const char GLYPH_a167[]=			"a167";
static const char GLYPH_a168[]=			"a168";
static const char GLYPH_a169[]=			"a169";
static const char GLYPH_a170[]=			"a170";
static const char GLYPH_a171[]=			"a171";
static const char GLYPH_a172[]=			"a172";
static const char GLYPH_a173[]=			"a173";
static const char GLYPH_a162[]=			"a162";
static const char GLYPH_a174[]=			"a174";
static const char GLYPH_a175[]=			"a175";
static const char GLYPH_a176[]=			"a176";
static const char GLYPH_a177[]=			"a177";
static const char GLYPH_a178[]=			"a178";
static const char GLYPH_a179[]=			"a179";
static const char GLYPH_a193[]=			"a193";
static const char GLYPH_a180[]=			"a180";
static const char GLYPH_a199[]=			"a199";
static const char GLYPH_a181[]=			"a181";
static const char GLYPH_a200[]=			"a200";
static const char GLYPH_a182[]=			"a182";
static const char GLYPH_a201[]=			"a201";
static const char GLYPH_a183[]=			"a183";
static const char GLYPH_a184[]=			"a184";
static const char GLYPH_a197[]=			"a197";
static const char GLYPH_a185[]=			"a185";
static const char GLYPH_a194[]=			"a194";
static const char GLYPH_a198[]=			"a198";
static const char GLYPH_a186[]=			"a186";
static const char GLYPH_a195[]=			"a195";
static const char GLYPH_a187[]=			"a187";
static const char GLYPH_a188[]=			"a188";
static const char GLYPH_a189[]=			"a189";
static const char GLYPH_a190[]=			"a190";
static const char GLYPH_a191[]=			"a191";

/************************************************************************/
/*									*/
/*  ISO Latin1 (ISO 8859-1) to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char *	const psIsoLatin1GlyphNames[256]=
    {
    (const char *)0,		/*  nul		0	*/
    (const char *)0,		/*  soh		1	*/
    (const char *)0,		/*  stx		2	*/
    (const char *)0,		/*  etx		3	*/
    (const char *)0,		/*  eot		4	*/
    (const char *)0,		/*  enq		5	*/
    (const char *)0,		/*  ack		6	*/
    (const char *)0,		/*  bel		7	*/
    (const char *)0,		/*  bs		8	*/
    (const char *)0,		/*  ht		9	*/
    (const char *)0,		/*  nl		10	*/
    (const char *)0,		/*  vt		11	*/
    (const char *)0,		/*  np		12	*/
    (const char *)0,		/*  cr		13	*/
    (const char *)0,		/*  so		14	*/
    (const char *)0,		/*  si		15	*/
    (const char *)0,		/*  dle		16	*/
    (const char *)0,		/*  dc1		17	*/
    (const char *)0,		/*  dc2		18	*/
    (const char *)0,		/*  dc3		19	*/
    (const char *)0,		/*  dc4		20	*/
    (const char *)0,		/*  nak		21	*/
    (const char *)0,		/*  syn		22	*/
    (const char *)0,		/*  etb		23	*/
    (const char *)0,		/*  can		24	*/
    (const char *)0,		/*  em		25	*/
    (const char *)0,		/*  sub		26	*/
    (const char *)0,		/*  esc		27	*/
    (const char *)0,		/*  fs		28	*/
    (const char *)0,		/*  gs		29	*/
    (const char *)0,		/*  rs		30	*/
    (const char *)0,		/*  us		31	*/
    GLYPH_space,		/*  32			*/
    GLYPH_exclam,		/*  33			*/
    GLYPH_quotedbl,		/*  34			*/
    GLYPH_numbersign,		/*  35			*/
    GLYPH_dollar,		/*  36			*/
    GLYPH_percent,		/*  37			*/
    GLYPH_ampersand,		/*  38			*/
    GLYPH_quoteright,		/*  39			*/
    GLYPH_parenleft,		/*  40			*/
    GLYPH_parenright,		/*  41			*/
    GLYPH_asterisk,		/*  42			*/
    GLYPH_plus,			/*  43			*/
    GLYPH_comma,		/*  44			*/
    GLYPH_minus,		/*  45			*/
    GLYPH_period,		/*  46			*/
    GLYPH_slash,		/*  47			*/
    GLYPH_zero,			/*  48			*/
    GLYPH_one,			/*  49			*/
    GLYPH_two,			/*  50			*/
    GLYPH_three,		/*  51			*/
    GLYPH_four,			/*  52			*/
    GLYPH_five,			/*  53			*/
    GLYPH_six,			/*  54			*/
    GLYPH_seven,		/*  55			*/
    GLYPH_eight,		/*  56			*/
    GLYPH_nine,			/*  57			*/
    GLYPH_colon,		/*  58			*/
    GLYPH_semicolon,		/*  59			*/
    GLYPH_less,			/*  60			*/
    GLYPH_equal,		/*  61			*/
    GLYPH_greater,		/*  62			*/
    GLYPH_question,		/*  63			*/
    GLYPH_at,			/*  64			*/
    GLYPH_A,			/*  65			*/
    GLYPH_B,			/*  66			*/
    GLYPH_C,			/*  67			*/
    GLYPH_D,			/*  68			*/
    GLYPH_E,			/*  69			*/
    GLYPH_F,			/*  70			*/
    GLYPH_G,			/*  71			*/
    GLYPH_H,			/*  72			*/
    GLYPH_I,			/*  73			*/
    GLYPH_J,			/*  74			*/
    GLYPH_K,			/*  75			*/
    GLYPH_L,			/*  76			*/
    GLYPH_M,			/*  77			*/
    GLYPH_N,			/*  78			*/
    GLYPH_O,			/*  79			*/
    GLYPH_P,			/*  80			*/
    GLYPH_Q,			/*  81			*/
    GLYPH_R,			/*  82			*/
    GLYPH_S,			/*  83			*/
    GLYPH_T,			/*  84			*/
    GLYPH_U,			/*  85			*/
    GLYPH_V,			/*  86			*/
    GLYPH_W,			/*  87			*/
    GLYPH_X,			/*  88			*/
    GLYPH_Y,			/*  89			*/
    GLYPH_Z,			/*  90			*/
    GLYPH_bracketleft,		/*  91			*/
    GLYPH_backslash,		/*  92			*/
    GLYPH_bracketright,		/*  93			*/
    GLYPH_asciicircum,		/*  94			*/
    GLYPH_underscore,		/*  95			*/
    GLYPH_quoteleft,		/*  96			*/
    GLYPH_a,			/*  97			*/
    GLYPH_b,			/*  98			*/
    GLYPH_c,			/*  99			*/
    GLYPH_d,			/*  100			*/
    GLYPH_e,			/*  101			*/
    GLYPH_f,			/*  102			*/
    GLYPH_g,			/*  103			*/
    GLYPH_h,			/*  104			*/
    GLYPH_i,			/*  105			*/
    GLYPH_j,			/*  106			*/
    GLYPH_k,			/*  107			*/
    GLYPH_l,			/*  108			*/
    GLYPH_m,			/*  109			*/
    GLYPH_n,			/*  110			*/
    GLYPH_o,			/*  111			*/
    GLYPH_p,			/*  112			*/
    GLYPH_q,			/*  113			*/
    GLYPH_r,			/*  114			*/
    GLYPH_s,			/*  115			*/
    GLYPH_t,			/*  116			*/
    GLYPH_u,			/*  117			*/
    GLYPH_v,			/*  118			*/
    GLYPH_w,			/*  119			*/
    GLYPH_x,			/*  120			*/
    GLYPH_y,			/*  121			*/
    GLYPH_z,			/*  122			*/
    GLYPH_braceleft,		/*  123			*/
    GLYPH_bar,			/*  124			*/
    GLYPH_braceright,		/*  125			*/
    GLYPH_asciitilde,		/*  126			*/
    (char *)0,			/*  127			*/
    (char *)0,			/*  128			*/
    (char *)0,			/*  129			*/
    (char *)0,			/*  130			*/
    (char *)0,			/*  131			*/
    (char *)0,			/*  132			*/
    (char *)0,			/*  133			*/
    (char *)0,			/*  134			*/
    (char *)0,			/*  135			*/
    (char *)0,			/*  136			*/
    (char *)0,			/*  137			*/
    (char *)0,			/*  138			*/
    (char *)0,			/*  139			*/
    (char *)0,			/*  140			*/
    (char *)0,			/*  141			*/
    (char *)0,			/*  142			*/
    (char *)0,			/*  143			*/
    GLYPH_dotlessi,		/*  144			*/
    GLYPH_grave,		/*  145			*/
    GLYPH_acute,		/*  146			*/
    GLYPH_circumflex,		/*  147			*/
    GLYPH_tilde,		/*  148			*/
    GLYPH_macron,		/*  149			*/
    GLYPH_breve,		/*  150			*/
    GLYPH_dotaccent,		/*  151			*/
    GLYPH_dieresis,		/*  152			*/
    (char *)0,			/*  153			*/
    GLYPH_ring,			/*  154			*/
    GLYPH_cedilla,		/*  155			*/
    (char *)0,			/*  156			*/
    GLYPH_hungarumlaut,		/*  157			*/
    GLYPH_ogonek,		/*  158			*/
    GLYPH_caron,		/*  159			*/
    GLYPH_nobreakspace,		/*  160			*/
    GLYPH_exclamdown,		/*  161			*/
    GLYPH_cent,			/*  162			*/
    GLYPH_sterling,		/*  163			*/
    GLYPH_currency,		/*  164			*/
    GLYPH_yen,			/*  165			*/
    GLYPH_brokenbar,		/*  166			*/
    GLYPH_section,		/*  167			*/
    GLYPH_dieresis,		/*  168			*/
    GLYPH_copyright,		/*  169			*/
    GLYPH_ordfeminine,		/*  170			*/
    GLYPH_guillemotleft,	/*  171			*/
    GLYPH_logicalnot,		/*  172			*/
    GLYPH_hyphen,		/*  173			*/
    GLYPH_registered,		/*  174			*/
    GLYPH_macron,		/*  175			*/
    GLYPH_degree,		/*  176			*/
    GLYPH_plusminus,		/*  177			*/
    GLYPH_twosuperior,		/*  178			*/
    GLYPH_threesuperior,	/*  179			*/
    GLYPH_acute,		/*  180			*/
    GLYPH_mu,			/*  181			*/
    GLYPH_paragraph,		/*  182			*/
    GLYPH_periodcentered,	/*  183			*/
    GLYPH_cedilla,		/*  184			*/
    GLYPH_onesuperior,		/*  185			*/
    GLYPH_ordmasculine,		/*  186			*/
    GLYPH_guillemotright,	/*  187			*/
    GLYPH_onequarter,		/*  188			*/
    GLYPH_onehalf,		/*  189			*/
    GLYPH_threequarters,	/*  190			*/
    GLYPH_questiondown,		/*  191			*/
    GLYPH_Agrave,		/*  192			*/
    GLYPH_Aacute,		/*  193			*/
    GLYPH_Acircumflex,		/*  194			*/
    GLYPH_Atilde,		/*  195			*/
    GLYPH_Adieresis,		/*  196			*/
    GLYPH_Aring,		/*  197			*/
    GLYPH_AE,			/*  198			*/
    GLYPH_Ccedilla,		/*  199			*/
    GLYPH_Egrave,		/*  200			*/
    GLYPH_Eacute,		/*  201			*/
    GLYPH_Ecircumflex,		/*  202			*/
    GLYPH_Edieresis,		/*  203			*/
    GLYPH_Igrave,		/*  204			*/
    GLYPH_Iacute,		/*  205			*/
    GLYPH_Icircumflex,		/*  206			*/
    GLYPH_Idieresis,		/*  207			*/
    GLYPH_Eth,			/*  208			*/
    GLYPH_Ntilde,		/*  209			*/
    GLYPH_Ograve,		/*  210			*/
    GLYPH_Oacute,		/*  211			*/
    GLYPH_Ocircumflex,		/*  212			*/
    GLYPH_Otilde,		/*  213			*/
    GLYPH_Odieresis,		/*  214			*/
    GLYPH_multiply,		/*  215			*/
    GLYPH_Oslash,		/*  216			*/
    GLYPH_Ugrave,		/*  217			*/
    GLYPH_Uacute,		/*  218			*/
    GLYPH_Ucircumflex,		/*  219			*/
    GLYPH_Udieresis,		/*  220			*/
    GLYPH_Yacute,		/*  221			*/
    GLYPH_Thorn,		/*  222			*/
    GLYPH_germandbls,		/*  223			*/
    GLYPH_agrave,		/*  224			*/
    GLYPH_aacute,		/*  225			*/
    GLYPH_acircumflex,		/*  226			*/
    GLYPH_atilde,		/*  227			*/
    GLYPH_adieresis,		/*  228			*/
    GLYPH_aring,		/*  229			*/
    GLYPH_ae,			/*  230			*/
    GLYPH_ccedilla,		/*  231			*/
    GLYPH_egrave,		/*  232			*/
    GLYPH_eacute,		/*  233			*/
    GLYPH_ecircumflex,		/*  234			*/
    GLYPH_edieresis,		/*  235			*/
    GLYPH_igrave,		/*  236			*/
    GLYPH_iacute,		/*  237			*/
    GLYPH_icircumflex,		/*  238			*/
    GLYPH_idieresis,		/*  239			*/
    GLYPH_eth,			/*  240			*/
    GLYPH_ntilde,		/*  241			*/
    GLYPH_ograve,		/*  242			*/
    GLYPH_oacute,		/*  243			*/
    GLYPH_ocircumflex,		/*  244			*/
    GLYPH_otilde,		/*  245			*/
    GLYPH_odieresis,		/*  246			*/
    GLYPH_divide,		/*  247			*/
    GLYPH_oslash,		/*  248			*/
    GLYPH_ugrave,		/*  249			*/
    GLYPH_uacute,		/*  250			*/
    GLYPH_ucircumflex,		/*  251			*/
    GLYPH_udieresis,		/*  252			*/
    GLYPH_yacute,		/*  253			*/
    GLYPH_thorn,		/*  254			*/
    GLYPH_ydieresis,		/*  255			*/
    };

/************************************************************************/
/*									*/
/*  Adobe standard encoding.						*/
/*									*/
/************************************************************************/

const char *	psStdEncodingGlyphNames[256]=
    {
    (char *)0,			/*  nul		0	*/
    (char *)0,			/*  soh		1	*/
    (char *)0,			/*  stx		2	*/
    (char *)0,			/*  etx		3	*/
    (char *)0,			/*  eot		4	*/
    (char *)0,			/*  enq		5	*/
    (char *)0,			/*  ack		6	*/
    (char *)0,			/*  bel		7	*/
    (char *)0,			/*  bs		8	*/
    (char *)0,			/*  ht		9	*/
    (char *)0,			/*  nl		10	*/
    (char *)0,			/*  vt		11	*/
    (char *)0,			/*  np		12	*/
    (char *)0,			/*  cr		13	*/
    (char *)0,			/*  so		14	*/
    (char *)0,			/*  si		15	*/
    (char *)0,			/*  dle		16	*/
    (char *)0,			/*  dc1		17	*/
    (char *)0,			/*  dc2		18	*/
    (char *)0,			/*  dc3		19	*/
    (char *)0,			/*  dc4		20	*/
    (char *)0,			/*  nak		21	*/
    (char *)0,			/*  syn		22	*/
    (char *)0,			/*  etb		23	*/
    (char *)0,			/*  can		24	*/
    (char *)0,			/*  em		25	*/
    (char *)0,			/*  sub		26	*/
    (char *)0,			/*  esc		27	*/
    (char *)0,			/*  fs		28	*/
    (char *)0,			/*  gs		29	*/
    (char *)0,			/*  rs		30	*/
    (char *)0,			/*  us		31	*/
    GLYPH_space,		/*  32			*/
    GLYPH_exclam,		/*  33			*/
    GLYPH_quotedbl,		/*  34			*/
    GLYPH_numbersign,		/*  35			*/
    GLYPH_dollar,		/*  36			*/
    GLYPH_percent,		/*  37			*/
    GLYPH_ampersand,		/*  38			*/
    GLYPH_quoteright,		/*  39			*/
    GLYPH_parenleft,		/*  40			*/
    GLYPH_parenright,		/*  41			*/
    GLYPH_asterisk,		/*  42			*/
    GLYPH_plus,			/*  43			*/
    GLYPH_comma,		/*  44			*/
    GLYPH_minus,		/*  45			*/
    GLYPH_period,		/*  46			*/
    GLYPH_slash,		/*  47			*/
    GLYPH_zero,			/*  48			*/
    GLYPH_one,			/*  49			*/
    GLYPH_two,			/*  50			*/
    GLYPH_three,		/*  51			*/
    GLYPH_four,			/*  52			*/
    GLYPH_five,			/*  53			*/
    GLYPH_six,			/*  54			*/
    GLYPH_seven,		/*  55			*/
    GLYPH_eight,		/*  56			*/
    GLYPH_nine,			/*  57			*/
    GLYPH_colon,		/*  58			*/
    GLYPH_semicolon,		/*  59			*/
    GLYPH_less,			/*  60			*/
    GLYPH_equal,		/*  61			*/
    GLYPH_greater,		/*  62			*/
    GLYPH_question,		/*  63			*/
    GLYPH_at,			/*  64			*/
    GLYPH_A,			/*  65			*/
    GLYPH_B,			/*  66			*/
    GLYPH_C,			/*  67			*/
    GLYPH_D,			/*  68			*/
    GLYPH_E,			/*  69			*/
    GLYPH_F,			/*  70			*/
    GLYPH_G,			/*  71			*/
    GLYPH_H,			/*  72			*/
    GLYPH_I,			/*  73			*/
    GLYPH_J,			/*  74			*/
    GLYPH_K,			/*  75			*/
    GLYPH_L,			/*  76			*/
    GLYPH_M,			/*  77			*/
    GLYPH_N,			/*  78			*/
    GLYPH_O,			/*  79			*/
    GLYPH_P,			/*  80			*/
    GLYPH_Q,			/*  81			*/
    GLYPH_R,			/*  82			*/
    GLYPH_S,			/*  83			*/
    GLYPH_T,			/*  84			*/
    GLYPH_U,			/*  85			*/
    GLYPH_V,			/*  86			*/
    GLYPH_W,			/*  87			*/
    GLYPH_X,			/*  88			*/
    GLYPH_Y,			/*  89			*/
    GLYPH_Z,			/*  90			*/
    GLYPH_bracketleft,		/*  91			*/
    GLYPH_backslash,		/*  92			*/
    GLYPH_bracketright,		/*  93			*/
    GLYPH_asciicircum,		/*  94			*/
    GLYPH_underscore,		/*  95			*/
    GLYPH_quoteleft,		/*  96			*/
    GLYPH_a,			/*  97			*/
    GLYPH_b,			/*  98			*/
    GLYPH_c,			/*  99			*/
    GLYPH_d,			/*  100			*/
    GLYPH_e,			/*  101			*/
    GLYPH_f,			/*  102			*/
    GLYPH_g,			/*  103			*/
    GLYPH_h,			/*  104			*/
    GLYPH_i,			/*  105			*/
    GLYPH_j,			/*  106			*/
    GLYPH_k,			/*  107			*/
    GLYPH_l,			/*  108			*/
    GLYPH_m,			/*  109			*/
    GLYPH_n,			/*  110			*/
    GLYPH_o,			/*  111			*/
    GLYPH_p,			/*  112			*/
    GLYPH_q,			/*  113			*/
    GLYPH_r,			/*  114			*/
    GLYPH_s,			/*  115			*/
    GLYPH_t,			/*  116			*/
    GLYPH_u,			/*  117			*/
    GLYPH_v,			/*  118			*/
    GLYPH_w,			/*  119			*/
    GLYPH_x,			/*  120			*/
    GLYPH_y,			/*  121			*/
    GLYPH_z,			/*  122			*/
    GLYPH_braceleft,		/*  123			*/
    GLYPH_bar,			/*  124			*/
    GLYPH_braceright,		/*  125			*/
    GLYPH_asciitilde,		/*  126			*/
    (char *)0,			/*  127			*/
    (char *)0,			/*  128			*/
    (char *)0,			/*  129			*/
    (char *)0,			/*  130			*/
    (char *)0,			/*  131			*/
    (char *)0,			/*  132			*/
    (char *)0,			/*  133			*/
    (char *)0,			/*  134			*/
    (char *)0,			/*  135			*/
    (char *)0,			/*  136			*/
    (char *)0,			/*  137			*/
    (char *)0,			/*  138			*/
    (char *)0,			/*  139			*/
    (char *)0,			/*  140			*/
    (char *)0,			/*  141			*/
    (char *)0,			/*  142			*/
    (char *)0,			/*  143			*/
    (char *)0,			/*  144			*/
    (char *)0,			/*  145			*/
    (char *)0,			/*  146			*/
    (char *)0,			/*  147			*/
    (char *)0,			/*  148			*/
    (char *)0,			/*  149			*/
    (char *)0,			/*  150			*/
    (char *)0,			/*  151			*/
    (char *)0,			/*  152			*/
    (char *)0,			/*  153			*/
    (char *)0,			/*  154			*/
    (char *)0,			/*  155			*/
    (char *)0,			/*  156			*/
    (char *)0,			/*  157			*/
    (char *)0,			/*  158			*/
    (char *)0,			/*  159			*/
    (char *)0,			/*  160			*/
    GLYPH_exclamdown,		/*  161			*/
    GLYPH_cent,			/*  162			*/
    GLYPH_sterling,		/*  163			*/
    GLYPH_fraction,		/*  164			*/
    GLYPH_yen,			/*  165			*/
    GLYPH_florin,		/*  166			*/
    GLYPH_section,		/*  167			*/
    GLYPH_currency,		/*  168			*/
    GLYPH_quotesingle,		/*  169			*/
    GLYPH_quotedblleft,		/*  170			*/
    GLYPH_guillemotleft,	/*  171			*/
    GLYPH_guillsinglleft,	/*  172			*/
    GLYPH_guillsinglright,	/*  173			*/
    GLYPH_fi,			/*  174			*/
    GLYPH_fl,			/*  175			*/
    (char *)0,			/*  176			*/
    GLYPH_endash,		/*  177			*/
    GLYPH_dagger,		/*  178			*/
    GLYPH_daggerdbl,		/*  179			*/
    GLYPH_periodcentered,	/*  180			*/
    (char *)0,			/*  181			*/
    GLYPH_paragraph,		/*  182			*/
    GLYPH_bullet,		/*  183			*/
    GLYPH_quotesinglbase,	/*  184			*/
    GLYPH_quotedblbase,		/*  185			*/
    GLYPH_quotedblright,	/*  186			*/
    GLYPH_guillemotright,	/*  187			*/
    GLYPH_ellipsis,		/*  188			*/
    GLYPH_perthousand,		/*  189			*/
    (char *)0,			/*  190			*/
    GLYPH_questiondown,		/*  191			*/
    (char *)0,			/*  192			*/
    GLYPH_grave,		/*  193			*/
    GLYPH_acute,		/*  194			*/
    GLYPH_circumflex,		/*  195			*/
    GLYPH_tilde,		/*  196			*/
    GLYPH_macron,		/*  197			*/
    GLYPH_breve,		/*  198			*/
    GLYPH_dotaccent,		/*  199			*/
    GLYPH_dieresis,		/*  200			*/
    (char *)0,			/*  201			*/
    GLYPH_ring,			/*  202			*/
    GLYPH_cedilla,		/*  203			*/
    (char *)0,			/*  204			*/
    GLYPH_hungarumlaut,		/*  205			*/
    GLYPH_ogonek,		/*  206			*/
    GLYPH_caron,		/*  207			*/
    GLYPH_emdash,		/*  208			*/
    (char *)0,			/*  209			*/
    (char *)0,			/*  210			*/
    (char *)0,			/*  211			*/
    (char *)0,			/*  212			*/
    (char *)0,			/*  213			*/
    (char *)0,			/*  214			*/
    (char *)0,			/*  215			*/
    (char *)0,			/*  216			*/
    (char *)0,			/*  217			*/
    (char *)0,			/*  218			*/
    (char *)0,			/*  219			*/
    (char *)0,			/*  220			*/
    (char *)0,			/*  221			*/
    (char *)0,			/*  222			*/
    (char *)0,			/*  223			*/
    (char *)0,			/*  224			*/
    GLYPH_AE,			/*  225			*/
    (char *)0,			/*  226			*/
    GLYPH_ordfeminine,		/*  227			*/
    (char *)0,			/*  228			*/
    (char *)0,			/*  229			*/
    (char *)0,			/*  230			*/
    (char *)0,			/*  231			*/
    GLYPH_Lslash,		/*  232			*/
    GLYPH_Oslash,		/*  233			*/
    GLYPH_OE,			/*  234			*/
    GLYPH_ordmasculine,		/*  235			*/
    (char *)0,			/*  236			*/
    (char *)0,			/*  237			*/
    (char *)0,			/*  238			*/
    (char *)0,			/*  239			*/
    (char *)0,			/*  240			*/
    GLYPH_ae,			/*  241			*/
    (char *)0,			/*  242			*/
    (char *)0,			/*  243			*/
    (char *)0,			/*  244			*/
    GLYPH_dotlessi,		/*  245			*/
    (char *)0,			/*  246			*/
    (char *)0,			/*  247			*/
    GLYPH_lslash,		/*  248			*/
    GLYPH_oslash,		/*  249			*/
    GLYPH_oe,			/*  250			*/
    GLYPH_germandbls,		/*  251			*/
    (char *)0,			/*  252			*/
    (char *)0,			/*  253			*/
    (char *)0,			/*  254			*/
    (char *)0,			/*  255			*/
    };

/************************************************************************/
/*									*/
/*  Postscript symbol encoding to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psSymbolGlyphNames[256]=
    {
    (const char *)0,		/*  0	*/
    (const char *)0,		/*  1	*/
    (const char *)0,		/*  2	*/
    (const char *)0,		/*  3	*/
    (const char *)0,		/*  4	*/
    (const char *)0,		/*  5	*/
    (const char *)0,		/*  6	*/
    (const char *)0,		/*  7	*/
    (const char *)0,		/*  8	*/
    (const char *)0,		/*  9	*/
    (const char *)0,		/*  10	*/
    (const char *)0,		/*  11	*/
    (const char *)0,		/*  12	*/
    (const char *)0,		/*  13	*/
    (const char *)0,		/*  14	*/
    (const char *)0,		/*  15	*/
    (const char *)0,		/*  16	*/
    (const char *)0,		/*  17	*/
    (const char *)0,		/*  18	*/
    (const char *)0,		/*  19	*/
    (const char *)0,		/*  20	*/
    (const char *)0,		/*  21	*/
    (const char *)0,		/*  22	*/
    (const char *)0,		/*  23	*/
    (const char *)0,		/*  24	*/
    (const char *)0,		/*  25	*/
    (const char *)0,		/*  26	*/
    (const char *)0,		/*  27	*/
    (const char *)0,		/*  28	*/
    (const char *)0,		/*  29	*/
    (const char *)0,		/*  30	*/
    (const char *)0,		/*  31	*/
    GLYPH_space,		/*  32	*/
    GLYPH_exclam,		/*  33	*/
    GLYPH_universal,		/*  34	*/
    GLYPH_numbersign,		/*  35	*/
    GLYPH_existential,		/*  36	*/
    GLYPH_percent,		/*  37	*/
    GLYPH_ampersand,		/*  38	*/
    GLYPH_suchthat,		/*  39	*/
    GLYPH_parenleft,		/*  40	*/
    GLYPH_parenright,		/*  41	*/
    GLYPH_asteriskmath,		/*  42	*/
    GLYPH_plus,			/*  43	*/
    GLYPH_comma,		/*  44	*/
    GLYPH_minus,		/*  45	*/
    GLYPH_period,		/*  46	*/
    GLYPH_slash,		/*  47	*/
    GLYPH_zero,			/*  48	*/
    GLYPH_one,			/*  49	*/
    GLYPH_two,			/*  50	*/
    GLYPH_three,		/*  51	*/
    GLYPH_four,			/*  52	*/
    GLYPH_five,			/*  53	*/
    GLYPH_six,			/*  54	*/
    GLYPH_seven,		/*  55	*/
    GLYPH_eight,		/*  56	*/
    GLYPH_nine,			/*  57	*/
    GLYPH_colon,		/*  58	*/
    GLYPH_semicolon,		/*  59	*/
    GLYPH_less,			/*  60	*/
    GLYPH_equal,		/*  61	*/
    GLYPH_greater,		/*  62	*/
    GLYPH_question,		/*  63	*/
    GLYPH_congruent,		/*  64	*/
    GLYPH_Alpha,		/*  65	*/
    GLYPH_Beta,			/*  66	*/
    GLYPH_Chi,			/*  67	*/
    GLYPH_Delta,		/*  68	*/
    GLYPH_Epsilon,		/*  69	*/
    GLYPH_Phi,			/*  70	*/
    GLYPH_Gamma,		/*  71	*/
    GLYPH_Eta,			/*  72	*/
    GLYPH_Iota,			/*  73	*/
    GLYPH_theta1,		/*  74	*/
    GLYPH_Kappa,		/*  75	*/
    GLYPH_Lambda,		/*  76	*/
    GLYPH_Mu,			/*  77	*/
    GLYPH_Nu,			/*  78	*/
    GLYPH_Omicron,		/*  79	*/
    GLYPH_Pi,			/*  80	*/
    GLYPH_Theta,		/*  81	*/
    GLYPH_Rho,			/*  82	*/
    GLYPH_Sigma,		/*  83	*/
    GLYPH_Tau,			/*  84	*/
    GLYPH_Upsilon,		/*  85	*/
    GLYPH_sigma1,		/*  86	*/
    GLYPH_Omega,		/*  87	*/
    GLYPH_Xi,			/*  88	*/
    GLYPH_Psi,			/*  89	*/
    GLYPH_Zeta,			/*  90	*/
    GLYPH_bracketleft,		/*  91	*/
    GLYPH_therefore,		/*  92	*/
    GLYPH_bracketright,		/*  93	*/
    GLYPH_perpendicular,	/*  94	*/
    GLYPH_underscore,		/*  95	*/
    GLYPH_radicalex,		/*  96	*/
    GLYPH_alpha,		/*  97	*/
    GLYPH_beta,			/*  98	*/
    GLYPH_chi,			/*  99	*/
    GLYPH_delta,		/*  100	*/
    GLYPH_epsilon,		/*  101	*/
    GLYPH_phi,			/*  102	*/
    GLYPH_gamma,		/*  103	*/
    GLYPH_eta,			/*  104	*/
    GLYPH_iota,			/*  105	*/
    GLYPH_phi1,			/*  106	*/
    GLYPH_kappa,		/*  107	*/
    GLYPH_lambda,		/*  108	*/
    GLYPH_mu,			/*  109	*/
    GLYPH_nu,			/*  110	*/
    GLYPH_omicron,		/*  111	*/
    GLYPH_pi,			/*  112	*/
    GLYPH_theta,		/*  113	*/
    GLYPH_rho,			/*  114	*/
    GLYPH_sigma,		/*  115	*/
    GLYPH_tau,			/*  116	*/
    GLYPH_upsilon,		/*  117	*/
    GLYPH_omega1,		/*  118	*/
    GLYPH_omega,		/*  119	*/
    GLYPH_xi,			/*  120	*/
    GLYPH_psi,			/*  121	*/
    GLYPH_zeta,			/*  122	*/
    GLYPH_braceleft,		/*  123	*/
    GLYPH_bar,			/*  124	*/
    GLYPH_braceright,		/*  125	*/
    GLYPH_similar,		/*  126	*/
    (const char *)0,		/*  127	*/
    (const char *)0,		/*  128	*/
    (const char *)0,		/*  129	*/
    (const char *)0,		/*  130	*/
    (const char *)0,		/*  131	*/
    (const char *)0,		/*  132	*/
    (const char *)0,		/*  133	*/
    (const char *)0,		/*  134	*/
    (const char *)0,		/*  135	*/
    (const char *)0,		/*  136	*/
    (const char *)0,		/*  137	*/
    (const char *)0,		/*  138	*/
    (const char *)0,		/*  139	*/
    (const char *)0,		/*  140	*/
    (const char *)0,		/*  141	*/
    (const char *)0,		/*  142	*/
    (const char *)0,		/*  143	*/
    (const char *)0,		/*  144	*/
    (const char *)0,		/*  145	*/
    (const char *)0,		/*  146	*/
    (const char *)0,		/*  147	*/
    (const char *)0,		/*  148	*/
    (const char *)0,		/*  149	*/
    (const char *)0,		/*  150	*/
    (const char *)0,		/*  151	*/
    (const char *)0,		/*  152	*/
    (const char *)0,		/*  153	*/
    (const char *)0,		/*  154	*/
    (const char *)0,		/*  155	*/
    (const char *)0,		/*  156	*/
    (const char *)0,		/*  157	*/
    (const char *)0,		/*  158	*/
    (const char *)0,		/*  159	*/
    (const char *)0,		/*  160	*/
    GLYPH_Upsilon1,		/*  161	*/
    GLYPH_minute,		/*  162	*/
    GLYPH_lessequal,		/*  163	*/
    GLYPH_fraction,		/*  164	*/
    GLYPH_infinity,		/*  165	*/
    GLYPH_florin,		/*  166	*/
    GLYPH_club,			/*  167	*/
    GLYPH_diamond,		/*  168	*/
    GLYPH_heart,		/*  169	*/
    GLYPH_spade,		/*  170	*/
    GLYPH_arrowboth,		/*  171	*/
    GLYPH_arrowleft,		/*  172	*/
    GLYPH_arrowup,		/*  173	*/
    GLYPH_arrowright,		/*  174	*/
    GLYPH_arrowdown,		/*  175	*/
    GLYPH_degree,		/*  176	*/
    GLYPH_plusminus,		/*  177	*/
    GLYPH_second,		/*  178	*/
    GLYPH_greaterequal,		/*  179	*/
    GLYPH_multiply,		/*  180	*/
    GLYPH_proportional,		/*  181	*/
    GLYPH_partialdiff,		/*  182	*/
    GLYPH_bullet,		/*  183	*/
    GLYPH_divide,		/*  184	*/
    GLYPH_notequal,		/*  185	*/
    GLYPH_equivalence,		/*  186	*/
    GLYPH_approxequal,		/*  187	*/
    GLYPH_ellipsis,		/*  188	*/
    GLYPH_arrowvertex,		/*  189	*/
    GLYPH_arrowhorizex,		/*  190	*/
    GLYPH_carriagereturn,	/*  191	*/
    GLYPH_aleph,		/*  192	*/
    GLYPH_Ifraktur,		/*  193	*/
    GLYPH_Rfraktur,		/*  194	*/
    GLYPH_weierstrass,		/*  195	*/
    GLYPH_circlemultiply,	/*  196	*/
    GLYPH_circleplus,		/*  197	*/
    GLYPH_emptyset,		/*  198	*/
    GLYPH_intersection,		/*  199	*/
    GLYPH_union,		/*  200	*/
    GLYPH_propersuperset,	/*  201	*/
    GLYPH_reflexsuperset,	/*  202	*/
    GLYPH_notsubset,		/*  203	*/
    GLYPH_propersubset,		/*  204	*/
    GLYPH_reflexsubset,		/*  205	*/
    GLYPH_element,		/*  206	*/
    GLYPH_notelement,		/*  207	*/
    GLYPH_angle,		/*  208	*/
    GLYPH_gradient,		/*  209	*/
    GLYPH_registerserif,	/*  210	*/
    GLYPH_copyrightserif,	/*  211	*/
    GLYPH_trademarkserif,	/*  212	*/
    GLYPH_product,		/*  213	*/
    GLYPH_radical,		/*  214	*/
    GLYPH_dotmath,		/*  215	*/
    GLYPH_logicalnot,		/*  216	*/
    GLYPH_logicaland,		/*  217	*/
    GLYPH_logicalor,		/*  218	*/
    GLYPH_arrowdblboth,		/*  219	*/
    GLYPH_arrowdblleft,		/*  220	*/
    GLYPH_arrowdblup,		/*  221	*/
    GLYPH_arrowdblright,	/*  222	*/
    GLYPH_arrowdbldown,		/*  223	*/
    GLYPH_lozenge,		/*  224	*/
    GLYPH_angleleft,		/*  225	*/
    GLYPH_registersans,		/*  226	*/
    GLYPH_copyrightsans,	/*  227	*/
    GLYPH_trademarksans,	/*  228	*/
    GLYPH_summation,		/*  229	*/
    GLYPH_parenlefttp,		/*  230	*/
    GLYPH_parenleftex,		/*  231	*/
    GLYPH_parenleftbt,		/*  232	*/
    GLYPH_bracketlefttp,	/*  233	*/
    GLYPH_bracketleftex,	/*  234	*/
    GLYPH_bracketleftbt,	/*  235	*/
    GLYPH_bracelefttp,		/*  236	*/
    GLYPH_braceleftmid,		/*  237	*/
    GLYPH_braceleftbt,		/*  238	*/
    GLYPH_braceex,		/*  239	*/
    (const char *)0,		/*  240	*/
    GLYPH_angleright,		/*  241	*/
    GLYPH_integral,		/*  242	*/
    GLYPH_integraltp,		/*  243	*/
    GLYPH_integralex,		/*  244	*/
    GLYPH_integralbt,		/*  245	*/
    GLYPH_parenrighttp,		/*  246	*/
    GLYPH_parenrightex,		/*  247	*/
    GLYPH_parenrightbt,		/*  248	*/
    GLYPH_bracketrighttp,	/*  249	*/
    GLYPH_bracketrightex,	/*  250	*/
    GLYPH_bracketrightbt,	/*  251	*/
    GLYPH_bracerighttp,		/*  252	*/
    GLYPH_bracerightmid,	/*  253	*/
    GLYPH_bracerightbt,		/*  254	*/
    (const char *)0,		/*  255	*/
    };

/************************************************************************/
/*									*/
/*  ISO Latin2 (ISO 8859-2) to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin2GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quoteright,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_space,		/*  160		*/
    GLYPH_Aogonek,		/*  161		*/
    GLYPH_breve,		/*  162		*/
    GLYPH_Lslash,		/*  163		*/
    GLYPH_currency,		/*  164		*/
    GLYPH_Lcaron,		/*  165		*/
    GLYPH_Sacute,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_dieresis,		/*  168		*/
    GLYPH_Scaron,		/*  169		*/
    GLYPH_Scedilla,		/*  170		*/
    GLYPH_Tcaron,		/*  171		*/
    GLYPH_Zacute,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_Zcaron,		/*  174		*/
    GLYPH_Zdotaccent,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_aogonek,		/*  177		*/
    GLYPH_ogonek,		/*  178		*/
    GLYPH_lslash,		/*  179		*/
    GLYPH_acute,		/*  180		*/
    GLYPH_lcaron,		/*  181		*/
    GLYPH_sacute,		/*  182		*/
    GLYPH_caron,		/*  183		*/
    GLYPH_cedilla,		/*  184		*/
    GLYPH_scaron,		/*  185		*/
    GLYPH_scedilla,		/*  186		*/
    GLYPH_tcaron,		/*  187		*/
    GLYPH_zacute,		/*  188		*/
    GLYPH_hungarumlaut,		/*  189		*/
    GLYPH_zcaron,		/*  190		*/
    GLYPH_zdotaccent,		/*  191		*/
    GLYPH_Racute,		/*  192		*/
    GLYPH_Aacute,		/*  193		*/
    GLYPH_Acircumflex,		/*  194		*/
    GLYPH_Abreve,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Lacute,		/*  197		*/
    GLYPH_Cacute,		/*  198		*/
    GLYPH_Ccedilla,		/*  199		*/
    GLYPH_Ccaron,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Eogonek,		/*  202		*/
    GLYPH_Edieresis,		/*  203		*/
    GLYPH_Ecaron,		/*  204		*/
    GLYPH_Iacute,		/*  205		*/
    GLYPH_Icircumflex,		/*  206		*/
    GLYPH_Dcaron,		/*  207		*/
    GLYPH_Dcroat,		/*  208		*/
    GLYPH_Nacute,		/*  209		*/
    GLYPH_Ncaron,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Ocircumflex,		/*  212		*/
    GLYPH_Ohungarumlaut,	/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_multiply,		/*  215		*/
    GLYPH_Rcaron,		/*  216		*/
    GLYPH_Uring,		/*  217		*/
    GLYPH_Uacute,		/*  218		*/
    GLYPH_Uhungarumlaut,	/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Yacute,		/*  221		*/
    GLYPH_Tcommaaccent,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_racute,		/*  224		*/
    GLYPH_aacute,		/*  225		*/
    GLYPH_acircumflex,		/*  226		*/
    GLYPH_abreve,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_lacute,		/*  229		*/
    GLYPH_cacute,		/*  230		*/
    GLYPH_ccedilla,		/*  231		*/
    GLYPH_ccaron,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_eogonek,		/*  234		*/
    GLYPH_edieresis,		/*  235		*/
    GLYPH_ecaron,		/*  236		*/
    GLYPH_iacute,		/*  237		*/
    GLYPH_icircumflex,		/*  238		*/
    GLYPH_dcaron,		/*  239		*/
    GLYPH_dcroat,		/*  240		*/
    GLYPH_nacute,		/*  241		*/
    GLYPH_ncaron,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_ocircumflex,		/*  244		*/
    GLYPH_ohungarumlaut,	/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_divide,		/*  247		*/
    GLYPH_rcaron,		/*  248		*/
    GLYPH_uring,		/*  249		*/
    GLYPH_uacute,		/*  250		*/
    GLYPH_uhungarumlaut,	/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_yacute,		/*  253		*/
    GLYPH_tcommaaccent,		/*  254		*/
    GLYPH_dotaccent,		/*  255		*/
    };


/************************************************************************/
/*									*/
/*  ISO Latin7 (ISO 8859-7: Greek) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin7GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_space,		/*  160		*/
    GLYPH_afii64937,		/*  161		*/
    GLYPH_afii57929,		/*  162		*/
    GLYPH_sterling,		/*  163		*/
    (const char *)0,		/*  164		*/
    (const char *)0,		/*  165		*/
    GLYPH_brokenbar,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_dieresis,		/*  168		*/
    GLYPH_copyright,		/*  169		*/
    (const char *)0,		/*  170		*/
    GLYPH_guillemotleft,	/*  171		*/
    GLYPH_logicalnot,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    (const char *)0,		/*  174		*/
    GLYPH_afii00208,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_plusminus,		/*  177		*/
    GLYPH_twosuperior,		/*  178		*/
    GLYPH_threesuperior,	/*  179		*/
    GLYPH_tonos,		/*  180		*/
    GLYPH_dieresistonos,	/*  181		*/
    GLYPH_Alphatonos,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_Epsilontonos,		/*  184		*/
    GLYPH_Etatonos,		/*  185		*/
    GLYPH_Iotatonos,		/*  186		*/
    GLYPH_guillemotright,	/*  187		*/
    GLYPH_Omicrontonos,		/*  188		*/
    GLYPH_onehalf,		/*  189		*/
    GLYPH_Upsilontonos,		/*  190		*/
    GLYPH_Omegatonos,		/*  191		*/
    GLYPH_iotadieresistonos,	/*  192		*/
    GLYPH_Alpha,		/*  193		*/
    GLYPH_Beta,			/*  194		*/
    GLYPH_Gamma,		/*  195		*/
    GLYPH_Delta,		/*  196		*/
    GLYPH_Epsilon,		/*  197		*/
    GLYPH_Zeta,			/*  198		*/
    GLYPH_Eta,			/*  199		*/
    GLYPH_Theta,		/*  200		*/
    GLYPH_Iota,			/*  201		*/
    GLYPH_Kappa,		/*  202		*/
    GLYPH_Lambda,		/*  203		*/
    GLYPH_Mu,			/*  204		*/
    GLYPH_Nu,			/*  205		*/
    GLYPH_Xi,			/*  206		*/
    GLYPH_Omicron,		/*  207		*/
    GLYPH_Pi,			/*  208		*/
    GLYPH_Rho,			/*  209		*/
    (const char *)0,		/*  210		*/
    GLYPH_Sigma,		/*  211		*/
    GLYPH_Tau,			/*  212		*/
    GLYPH_Upsilon,		/*  213		*/
    GLYPH_Phi,			/*  214		*/
    GLYPH_Chi,			/*  215		*/
    GLYPH_Psi,			/*  216		*/
    GLYPH_Omega,		/*  217		*/
    GLYPH_Iotadieresis,		/*  218		*/
    GLYPH_Upsilondieresis,	/*  219		*/
    GLYPH_alphatonos,		/*  220		*/
    GLYPH_epsilontonos,		/*  221		*/
    GLYPH_etatonos,		/*  222		*/
    GLYPH_iotatonos,		/*  223		*/
    GLYPH_upsilondieresistonos,	/*  224		*/
    GLYPH_alpha,		/*  225		*/
    GLYPH_beta,			/*  226		*/
    GLYPH_gamma,		/*  227		*/
    GLYPH_delta,		/*  228		*/
    GLYPH_epsilon,		/*  229		*/
    GLYPH_zeta,			/*  230		*/
    GLYPH_eta,			/*  231		*/
    GLYPH_theta,		/*  232		*/
    GLYPH_iota,			/*  233		*/
    GLYPH_kappa,		/*  234		*/
    GLYPH_lambda,		/*  235		*/
    GLYPH_mu,			/*  236		*/
    GLYPH_nu,			/*  237		*/
    GLYPH_xi,			/*  238		*/
    GLYPH_omicron,		/*  239		*/
    GLYPH_pi,			/*  240		*/
    GLYPH_rho,			/*  241		*/
    GLYPH_sigma1,		/*  242		*/
    GLYPH_sigma,		/*  243		*/
    GLYPH_tau,			/*  244		*/
    GLYPH_upsilon,		/*  245		*/
    GLYPH_phi,			/*  246		*/
    GLYPH_chi,			/*  247		*/
    GLYPH_psi,			/*  248		*/
    GLYPH_omega,		/*  249		*/
    GLYPH_iotadieresis,		/*  250		*/
    GLYPH_upsilondieresis,	/*  251		*/
    GLYPH_omicrontonos,		/*  252		*/
    GLYPH_upsilontonos,		/*  253		*/
    GLYPH_omegatonos,		/*  254		*/
    (const char *)0,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  Adobe Cyrillic Encoding to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psCyrillicGlyphNames[256]=
    {
    (char *)0,			/*  0x00=   0  */
    (char *)0,			/*  0x01=   1  */
    (char *)0,			/*  0x02=   2  */
    (char *)0,			/*  0x03=   3  */
    (char *)0,			/*  0x04=   4  */
    (char *)0,			/*  0x05=   5  */
    (char *)0,			/*  0x06=   6  */
    (char *)0,			/*  0x07=   7  */
    (char *)0,			/*  0x08=   8  */
    (char *)0,			/*  0x09=   9  */
    (char *)0,			/*  0x0a=  10  */
    (char *)0,			/*  0x0b=  11  */
    (char *)0,			/*  0x0c=  12  */
    (char *)0,			/*  0x0d=  13  */
    (char *)0,			/*  0x0e=  14  */
    (char *)0,			/*  0x0f=  15  */
    (char *)0,			/*  0x10=  16  */
    (char *)0,			/*  0x11=  17  */
    (char *)0,			/*  0x12=  18  */
    (char *)0,			/*  0x13=  19  */
    (char *)0,			/*  0x14=  20  */
    (char *)0,			/*  0x15=  21  */
    (char *)0,			/*  0x16=  22  */
    (char *)0,			/*  0x17=  23  */
    (char *)0,			/*  0x18=  24  */
    (char *)0,			/*  0x19=  25  */
    (char *)0,			/*  0x1a=  26  */
    (char *)0,			/*  0x1b=  27  */
    (char *)0,			/*  0x1c=  28  */
    (char *)0,			/*  0x1d=  29  */
    (char *)0,			/*  0x1e=  30  */
    (char *)0,			/*  0x1f=  31  */
    GLYPH_space,		/*  0x20=  32  */
    GLYPH_exclam,		/*  0x21=  33  */
    GLYPH_quotedbl,		/*  0x22=  34  */
    GLYPH_numbersign,		/*  0x23=  35  */
    GLYPH_dollar,		/*  0x24=  36  */
    GLYPH_percent,		/*  0x25=  37  */
    GLYPH_ampersand,		/*  0x26=  38  */
    GLYPH_quotesingle,		/*  0x27=  39  */
    GLYPH_parenleft,		/*  0x28=  40  */
    GLYPH_parenright,		/*  0x29=  41  */
    GLYPH_asterisk,		/*  0x2a=  42  */
    GLYPH_plus,			/*  0x2b=  43  */
    GLYPH_comma,		/*  0x2c=  44  */
    GLYPH_hyphen,		/*  0x2d=  45  */
    GLYPH_period,		/*  0x2e=  46  */
    GLYPH_slash,		/*  0x2f=  47  */
    GLYPH_zero,			/*  0x30=  48  */
    GLYPH_one,			/*  0x31=  49  */
    GLYPH_two,			/*  0x32=  50  */
    GLYPH_three,		/*  0x33=  51  */
    GLYPH_four,			/*  0x34=  52  */
    GLYPH_five,			/*  0x35=  53  */
    GLYPH_six,			/*  0x36=  54  */
    GLYPH_seven,		/*  0x37=  55  */
    GLYPH_eight,		/*  0x38=  56  */
    GLYPH_nine,			/*  0x39=  57  */
    GLYPH_colon,		/*  0x3a=  58  */
    GLYPH_semicolon,		/*  0x3b=  59  */
    GLYPH_less,			/*  0x3c=  60  */
    GLYPH_equal,		/*  0x3d=  61  */
    GLYPH_greater,		/*  0x3e=  62  */
    GLYPH_question,		/*  0x3f=  63  */
    GLYPH_at,			/*  0x40=  64  */
    GLYPH_A,			/*  0x41=  65  */
    GLYPH_B,			/*  0x42=  66  */
    GLYPH_C,			/*  0x43=  67  */
    GLYPH_D,			/*  0x44=  68  */
    GLYPH_E,			/*  0x45=  69  */
    GLYPH_F,			/*  0x46=  70  */
    GLYPH_G,			/*  0x47=  71  */
    GLYPH_H,			/*  0x48=  72  */
    GLYPH_I,			/*  0x49=  73  */
    GLYPH_J,			/*  0x4a=  74  */
    GLYPH_K,			/*  0x4b=  75  */
    GLYPH_L,			/*  0x4c=  76  */
    GLYPH_M,			/*  0x4d=  77  */
    GLYPH_N,			/*  0x4e=  78  */
    GLYPH_O,			/*  0x4f=  79  */
    GLYPH_P,			/*  0x50=  80  */
    GLYPH_Q,			/*  0x51=  81  */
    GLYPH_R,			/*  0x52=  82  */
    GLYPH_S,			/*  0x53=  83  */
    GLYPH_T,			/*  0x54=  84  */
    GLYPH_U,			/*  0x55=  85  */
    GLYPH_V,			/*  0x56=  86  */
    GLYPH_W,			/*  0x57=  87  */
    GLYPH_X,			/*  0x58=  88  */
    GLYPH_Y,			/*  0x59=  89  */
    GLYPH_Z,			/*  0x5a=  90  */
    GLYPH_bracketleft,		/*  0x5b=  91  */
    GLYPH_backslash,		/*  0x5c=  92  */
    GLYPH_bracketright,		/*  0x5d=  93  */
    GLYPH_asciicircum,		/*  0x5e=  94  */
    GLYPH_underscore,		/*  0x5f=  95  */
    GLYPH_grave,		/*  0x60=  96  */
    GLYPH_a,			/*  0x61=  97  */
    GLYPH_b,			/*  0x62=  98  */
    GLYPH_c,			/*  0x63=  99  */
    GLYPH_d,			/*  0x64= 100  */
    GLYPH_e,			/*  0x65= 101  */
    GLYPH_f,			/*  0x66= 102  */
    GLYPH_g,			/*  0x67= 103  */
    GLYPH_h,			/*  0x68= 104  */
    GLYPH_i,			/*  0x69= 105  */
    GLYPH_j,			/*  0x6a= 106  */
    GLYPH_k,			/*  0x6b= 107  */
    GLYPH_l,			/*  0x6c= 108  */
    GLYPH_m,			/*  0x6d= 109  */
    GLYPH_n,			/*  0x6e= 110  */
    GLYPH_o,			/*  0x6f= 111  */
    GLYPH_p,			/*  0x70= 112  */
    GLYPH_q,			/*  0x71= 113  */
    GLYPH_r,			/*  0x72= 114  */
    GLYPH_s,			/*  0x73= 115  */
    GLYPH_t,			/*  0x74= 116  */
    GLYPH_u,			/*  0x75= 117  */
    GLYPH_v,			/*  0x76= 118  */
    GLYPH_w,			/*  0x77= 119  */
    GLYPH_x,			/*  0x78= 120  */
    GLYPH_y,			/*  0x79= 121  */
    GLYPH_z,			/*  0x7a= 122  */
    GLYPH_braceleft,		/*  0x7b= 123  */
    GLYPH_bar,			/*  0x7c= 124  */
    GLYPH_braceright,		/*  0x7d= 125  */
    GLYPH_asciitilde,		/*  0x7e= 126  */
    (char *)0,			/*  0x7f= 127  */
    GLYPH_afii10051,		/*  0x80= 128  */
    GLYPH_afii10052,		/*  0x81= 129  */
    GLYPH_quotesinglbase,	/*  0x82= 130  */
    GLYPH_afii10100,		/*  0x83= 131  */
    GLYPH_quotedblbase,		/*  0x84= 132  */
    GLYPH_ellipsis,		/*  0x85= 133  */
    GLYPH_dagger,		/*  0x86= 134  */
    GLYPH_daggerdbl,		/*  0x87= 135  */
    (char *)0,			/*  0x88= 136  */
    GLYPH_perthousand,		/*  0x89= 137  */
    GLYPH_afii10058,		/*  0x8a= 138  */
    GLYPH_guilsinglleft,	/*  0x8b= 139  */
    GLYPH_afii10059,		/*  0x8c= 140  */
    GLYPH_afii10061,		/*  0x8d= 141  */
    GLYPH_afii10060,		/*  0x8e= 142  */
    GLYPH_afii10145,		/*  0x8f= 143  */
    GLYPH_afii10099,		/*  0x90= 144  */
    GLYPH_quoteleft,		/*  0x91= 145  */
    GLYPH_quoteright,		/*  0x92= 146  */
    GLYPH_quotedblleft,		/*  0x93= 147  */
    GLYPH_quotedblright,	/*  0x94= 148  */
    GLYPH_bullet,		/*  0x95= 149  */
    GLYPH_endash,		/*  0x96= 150  */
    GLYPH_emdash,		/*  0x97= 151  */
    (char *)0,			/*  0x98= 152  */
    GLYPH_trademark,		/*  0x99= 153  */
    GLYPH_afii10106,		/*  0x9a= 154  */
    GLYPH_guilsinglright,	/*  0x9b= 155  */
    GLYPH_afii10107,		/*  0x9c= 156  */
    GLYPH_afii10109,		/*  0x9d= 157  */
    GLYPH_afii10108,		/*  0x9e= 158  */
    GLYPH_afii10193,		/*  0x9f= 159  */
    GLYPH_space,		/*  0xa0= 160  */
    GLYPH_afii10062,		/*  0xa1= 161  */
    GLYPH_afii10110,		/*  0xa2= 162  */
    GLYPH_afii10057,		/*  0xa3= 163  */
    GLYPH_currency,		/*  0xa4= 164  */
    GLYPH_afii10050,		/*  0xa5= 165  */
    GLYPH_brokenbar,		/*  0xa6= 166  */
    GLYPH_section,		/*  0xa7= 167  */
    GLYPH_afii10023,		/*  0xa8= 168  */
    GLYPH_copyright,		/*  0xa9= 169  */
    GLYPH_afii10053,		/*  0xaa= 170  */
    GLYPH_guillemotleft,	/*  0xab= 171  */
    GLYPH_logicalnot,		/*  0xac= 172  */
    GLYPH_hyphen,		/*  0xad= 173  */
    GLYPH_registered,		/*  0xae= 174  */
    GLYPH_afii10056,		/*  0xaf= 175  */
    GLYPH_degree,		/*  0xb0= 176  */
    GLYPH_plusminus,		/*  0xb1= 177  */
    GLYPH_afii10055,		/*  0xb2= 178  */
    GLYPH_afii10103,		/*  0xb3= 179  */
    GLYPH_afii10098,		/*  0xb4= 180  */
    GLYPH_mu,			/*  0xb5= 181  */
    GLYPH_paragraph,		/*  0xb6= 182  */
    GLYPH_periodcentered,	/*  0xb7= 183  */
    GLYPH_afii10071,		/*  0xb8= 184  */
    GLYPH_afii61352,		/*  0xb9= 185  */
    GLYPH_afii10101,		/*  0xba= 186  */
    GLYPH_guillemotright,	/*  0xbb= 187  */
    GLYPH_afii10105,		/*  0xbc= 188  */
    GLYPH_afii10054,		/*  0xbd= 189  */
    GLYPH_afii10102,		/*  0xbe= 190  */
    GLYPH_afii10104,		/*  0xbf= 191  */
    GLYPH_afii10017,		/*  0xc0= 192  */
    GLYPH_afii10018,		/*  0xc1= 193  */
    GLYPH_afii10019,		/*  0xc2= 194  */
    GLYPH_afii10020,		/*  0xc3= 195  */
    GLYPH_afii10021,		/*  0xc4= 196  */
    GLYPH_afii10022,		/*  0xc5= 197  */
    GLYPH_afii10024,		/*  0xc6= 198  */
    GLYPH_afii10025,		/*  0xc7= 199  */
    GLYPH_afii10026,		/*  0xc8= 200  */
    GLYPH_afii10027,		/*  0xc9= 201  */
    GLYPH_afii10028,		/*  0xca= 202  */
    GLYPH_afii10029,		/*  0xcb= 203  */
    GLYPH_afii10030,		/*  0xcc= 204  */
    GLYPH_afii10031,		/*  0xcd= 205  */
    GLYPH_afii10032,		/*  0xce= 206  */
    GLYPH_afii10033,		/*  0xcf= 207  */
    GLYPH_afii10034,		/*  0xd0= 208  */
    GLYPH_afii10035,		/*  0xd1= 209  */
    GLYPH_afii10036,		/*  0xd2= 210  */
    GLYPH_afii10037,		/*  0xd3= 211  */
    GLYPH_afii10038,		/*  0xd4= 212  */
    GLYPH_afii10039,		/*  0xd5= 213  */
    GLYPH_afii10040,		/*  0xd6= 214  */
    GLYPH_afii10041,		/*  0xd7= 215  */
    GLYPH_afii10042,		/*  0xd8= 216  */
    GLYPH_afii10043,		/*  0xd9= 217  */
    GLYPH_afii10044,		/*  0xda= 218  */
    GLYPH_afii10045,		/*  0xdb= 219  */
    GLYPH_afii10046,		/*  0xdc= 220  */
    GLYPH_afii10047,		/*  0xdd= 221  */
    GLYPH_afii10048,		/*  0xde= 222  */
    GLYPH_afii10049,		/*  0xdf= 223  */
    GLYPH_afii10065,		/*  0xe0= 224  */
    GLYPH_afii10066,		/*  0xe1= 225  */
    GLYPH_afii10067,		/*  0xe2= 226  */
    GLYPH_afii10068,		/*  0xe3= 227  */
    GLYPH_afii10069,		/*  0xe4= 228  */
    GLYPH_afii10070,		/*  0xe5= 229  */
    GLYPH_afii10072,		/*  0xe6= 230  */
    GLYPH_afii10073,		/*  0xe7= 231  */
    GLYPH_afii10074,		/*  0xe8= 232  */
    GLYPH_afii10075,		/*  0xe9= 233  */
    GLYPH_afii10076,		/*  0xea= 234  */
    GLYPH_afii10077,		/*  0xeb= 235  */
    GLYPH_afii10078,		/*  0xec= 236  */
    GLYPH_afii10079,		/*  0xed= 237  */
    GLYPH_afii10080,		/*  0xee= 238  */
    GLYPH_afii10081,		/*  0xef= 239  */
    GLYPH_afii10082,		/*  0xf0= 240  */
    GLYPH_afii10083,		/*  0xf1= 241  */
    GLYPH_afii10084,		/*  0xf2= 242  */
    GLYPH_afii10085,		/*  0xf3= 243  */
    GLYPH_afii10086,		/*  0xf4= 244  */
    GLYPH_afii10087,		/*  0xf5= 245  */
    GLYPH_afii10088,		/*  0xf6= 246  */
    GLYPH_afii10089,		/*  0xf7= 247  */
    GLYPH_afii10090,		/*  0xf8= 248  */
    GLYPH_afii10091,		/*  0xf9= 249  */
    GLYPH_afii10092,		/*  0xfa= 250  */
    GLYPH_afii10093,		/*  0xfb= 251  */
    GLYPH_afii10094,		/*  0xfc= 252  */
    GLYPH_afii10095,		/*  0xfd= 253  */
    GLYPH_afii10096,		/*  0xfe= 254  */
    GLYPH_afii10097,		/*  0xff= 255  */
    };

/************************************************************************/
/*									*/
/*  Adobe ITC Zapf Dingbats to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psDingbatGlyphNames[256]=
    {
    (char *)0,		/*  0x00=   0  */
    (char *)0,		/*  0x01=   1  */
    (char *)0,		/*  0x02=   2  */
    (char *)0,		/*  0x03=   3  */
    (char *)0,		/*  0x04=   4  */
    (char *)0,		/*  0x05=   5  */
    (char *)0,		/*  0x06=   6  */
    (char *)0,		/*  0x07=   7  */
    (char *)0,		/*  0x08=   8  */
    (char *)0,		/*  0x09=   9  */
    (char *)0,		/*  0x0a=  10  */
    (char *)0,		/*  0x0b=  11  */
    (char *)0,		/*  0x0c=  12  */
    (char *)0,		/*  0x0d=  13  */
    (char *)0,		/*  0x0e=  14  */
    (char *)0,		/*  0x0f=  15  */
    (char *)0,		/*  0x10=  16  */
    (char *)0,		/*  0x11=  17  */
    (char *)0,		/*  0x12=  18  */
    (char *)0,		/*  0x13=  19  */
    (char *)0,		/*  0x14=  20  */
    (char *)0,		/*  0x15=  21  */
    (char *)0,		/*  0x16=  22  */
    (char *)0,		/*  0x17=  23  */
    (char *)0,		/*  0x18=  24  */
    (char *)0,		/*  0x19=  25  */
    (char *)0,		/*  0x1a=  26  */
    (char *)0,		/*  0x1b=  27  */
    (char *)0,		/*  0x1c=  28  */
    (char *)0,		/*  0x1d=  29  */
    (char *)0,		/*  0x1e=  30  */
    (char *)0,		/*  0x1f=  31  */
    GLYPH_space,	/*  0x20=  32  */
    GLYPH_a1,		/*  0x21=  33  */
    GLYPH_a2,		/*  0x22=  34  */
    GLYPH_a202,		/*  0x23=  35  */
    GLYPH_a3,		/*  0x24=  36  */
    GLYPH_a4,		/*  0x25=  37  */
    GLYPH_a5,		/*  0x26=  38  */
    GLYPH_a119,		/*  0x27=  39  */
    GLYPH_a118,		/*  0x28=  40  */
    GLYPH_a117,		/*  0x29=  41  */
    GLYPH_a11,		/*  0x2a=  42  */
    GLYPH_a12,		/*  0x2b=  43  */
    GLYPH_a13,		/*  0x2c=  44  */
    GLYPH_a14,		/*  0x2d=  45  */
    GLYPH_a15,		/*  0x2e=  46  */
    GLYPH_a16,		/*  0x2f=  47  */
    GLYPH_a105,		/*  0x30=  48  */
    GLYPH_a17,		/*  0x31=  49  */
    GLYPH_a18,		/*  0x32=  50  */
    GLYPH_a19,		/*  0x33=  51  */
    GLYPH_a20,		/*  0x34=  52  */
    GLYPH_a21,		/*  0x35=  53  */
    GLYPH_a22,		/*  0x36=  54  */
    GLYPH_a23,		/*  0x37=  55  */
    GLYPH_a24,		/*  0x38=  56  */
    GLYPH_a25,		/*  0x39=  57  */
    GLYPH_a26,		/*  0x3a=  58  */
    GLYPH_a27,		/*  0x3b=  59  */
    GLYPH_a28,		/*  0x3c=  60  */
    GLYPH_a6,		/*  0x3d=  61  */
    GLYPH_a7,		/*  0x3e=  62  */
    GLYPH_a8,		/*  0x3f=  63  */
    GLYPH_a9,		/*  0x40=  64  */
    GLYPH_a10,		/*  0x41=  65  */
    GLYPH_a29,		/*  0x42=  66  */
    GLYPH_a30,		/*  0x43=  67  */
    GLYPH_a31,		/*  0x44=  68  */
    GLYPH_a32,		/*  0x45=  69  */
    GLYPH_a33,		/*  0x46=  70  */
    GLYPH_a34,		/*  0x47=  71  */
    GLYPH_a35,		/*  0x48=  72  */
    GLYPH_a36,		/*  0x49=  73  */
    GLYPH_a37,		/*  0x4a=  74  */
    GLYPH_a38,		/*  0x4b=  75  */
    GLYPH_a39,		/*  0x4c=  76  */
    GLYPH_a40,		/*  0x4d=  77  */
    GLYPH_a41,		/*  0x4e=  78  */
    GLYPH_a42,		/*  0x4f=  79  */
    GLYPH_a43,		/*  0x50=  80  */
    GLYPH_a44,		/*  0x51=  81  */
    GLYPH_a45,		/*  0x52=  82  */
    GLYPH_a46,		/*  0x53=  83  */
    GLYPH_a47,		/*  0x54=  84  */
    GLYPH_a48,		/*  0x55=  85  */
    GLYPH_a49,		/*  0x56=  86  */
    GLYPH_a50,		/*  0x57=  87  */
    GLYPH_a51,		/*  0x58=  88  */
    GLYPH_a52,		/*  0x59=  89  */
    GLYPH_a53,		/*  0x5a=  90  */
    GLYPH_a54,		/*  0x5b=  91  */
    GLYPH_a55,		/*  0x5c=  92  */
    GLYPH_a56,		/*  0x5d=  93  */
    GLYPH_a57,		/*  0x5e=  94  */
    GLYPH_a58,		/*  0x5f=  95  */
    GLYPH_a59,		/*  0x60=  96  */
    GLYPH_a60,		/*  0x61=  97  */
    GLYPH_a61,		/*  0x62=  98  */
    GLYPH_a62,		/*  0x63=  99  */
    GLYPH_a63,		/*  0x64= 100  */
    GLYPH_a64,		/*  0x65= 101  */
    GLYPH_a65,		/*  0x66= 102  */
    GLYPH_a66,		/*  0x67= 103  */
    GLYPH_a67,		/*  0x68= 104  */
    GLYPH_a68,		/*  0x69= 105  */
    GLYPH_a69,		/*  0x6a= 106  */
    GLYPH_a70,		/*  0x6b= 107  */
    GLYPH_a71,		/*  0x6c= 108  */
    GLYPH_a72,		/*  0x6d= 109  */
    GLYPH_a73,		/*  0x6e= 110  */
    GLYPH_a74,		/*  0x6f= 111  */
    GLYPH_a203,		/*  0x70= 112  */
    GLYPH_a75,		/*  0x71= 113  */
    GLYPH_a204,		/*  0x72= 114  */
    GLYPH_a76,		/*  0x73= 115  */
    GLYPH_a77,		/*  0x74= 116  */
    GLYPH_a78,		/*  0x75= 117  */
    GLYPH_a79,		/*  0x76= 118  */
    GLYPH_a81,		/*  0x77= 119  */
    GLYPH_a82,		/*  0x78= 120  */
    GLYPH_a83,		/*  0x79= 121  */
    GLYPH_a84,		/*  0x7a= 122  */
    GLYPH_a97,		/*  0x7b= 123  */
    GLYPH_a98,		/*  0x7c= 124  */
    GLYPH_a99,		/*  0x7d= 125  */
    GLYPH_a100,		/*  0x7e= 126  */
    (char *)0,		/*  0x7f= 127  */
    (char *)0,		/*  0x80= 128  */
    (char *)0,		/*  0x81= 129  */
    (char *)0,		/*  0x82= 130  */
    (char *)0,		/*  0x83= 131  */
    (char *)0,		/*  0x84= 132  */
    (char *)0,		/*  0x85= 133  */
    (char *)0,		/*  0x86= 134  */
    (char *)0,		/*  0x87= 135  */
    (char *)0,		/*  0x88= 136  */
    (char *)0,		/*  0x89= 137  */
    (char *)0,		/*  0x8a= 138  */
    (char *)0,		/*  0x8b= 139  */
    (char *)0,		/*  0x8c= 140  */
    (char *)0,		/*  0x8d= 141  */
    (char *)0,		/*  0x8e= 142  */
    (char *)0,		/*  0x8f= 143  */
    (char *)0,		/*  0x90= 144  */
    (char *)0,		/*  0x91= 145  */
    (char *)0,		/*  0x92= 146  */
    (char *)0,		/*  0x93= 147  */
    (char *)0,		/*  0x94= 148  */
    (char *)0,		/*  0x95= 149  */
    (char *)0,		/*  0x96= 150  */
    (char *)0,		/*  0x97= 151  */
    (char *)0,		/*  0x98= 152  */
    (char *)0,		/*  0x99= 153  */
    (char *)0,		/*  0x9a= 154  */
    (char *)0,		/*  0x9b= 155  */
    (char *)0,		/*  0x9c= 156  */
    (char *)0,		/*  0x9d= 157  */
    (char *)0,		/*  0x9e= 158  */
    (char *)0,		/*  0x9f= 159  */
    (char *)0,		/*  0xa0= 160  */
    GLYPH_a101,		/*  0xa1= 161  */
    GLYPH_a102,		/*  0xa2= 162  */
    GLYPH_a103,		/*  0xa3= 163  */
    GLYPH_a104,		/*  0xa4= 164  */
    GLYPH_a106,		/*  0xa5= 165  */
    GLYPH_a107,		/*  0xa6= 166  */
    GLYPH_a108,		/*  0xa7= 167  */
    GLYPH_a112,		/*  0xa8= 168  */
    GLYPH_a111,		/*  0xa9= 169  */
    GLYPH_a110,		/*  0xaa= 170  */
    GLYPH_a109,		/*  0xab= 171  */
    GLYPH_a120,		/*  0xac= 172  */
    GLYPH_a121,		/*  0xad= 173  */
    GLYPH_a122,		/*  0xae= 174  */
    GLYPH_a123,		/*  0xaf= 175  */
    GLYPH_a124,		/*  0xb0= 176  */
    GLYPH_a125,		/*  0xb1= 177  */
    GLYPH_a126,		/*  0xb2= 178  */
    GLYPH_a127,		/*  0xb3= 179  */
    GLYPH_a128,		/*  0xb4= 180  */
    GLYPH_a129,		/*  0xb5= 181  */
    GLYPH_a130,		/*  0xb6= 182  */
    GLYPH_a131,		/*  0xb7= 183  */
    GLYPH_a132,		/*  0xb8= 184  */
    GLYPH_a133,		/*  0xb9= 185  */
    GLYPH_a134,		/*  0xba= 186  */
    GLYPH_a135,		/*  0xbb= 187  */
    GLYPH_a136,		/*  0xbc= 188  */
    GLYPH_a137,		/*  0xbd= 189  */
    GLYPH_a138,		/*  0xbe= 190  */
    GLYPH_a139,		/*  0xbf= 191  */
    GLYPH_a140,		/*  0xc0= 192  */
    GLYPH_a141,		/*  0xc1= 193  */
    GLYPH_a142,		/*  0xc2= 194  */
    GLYPH_a143,		/*  0xc3= 195  */
    GLYPH_a144,		/*  0xc4= 196  */
    GLYPH_a145,		/*  0xc5= 197  */
    GLYPH_a146,		/*  0xc6= 198  */
    GLYPH_a147,		/*  0xc7= 199  */
    GLYPH_a148,		/*  0xc8= 200  */
    GLYPH_a149,		/*  0xc9= 201  */
    GLYPH_a150,		/*  0xca= 202  */
    GLYPH_a151,		/*  0xcb= 203  */
    GLYPH_a152,		/*  0xcc= 204  */
    GLYPH_a153,		/*  0xcd= 205  */
    GLYPH_a154,		/*  0xce= 206  */
    GLYPH_a155,		/*  0xcf= 207  */
    GLYPH_a156,		/*  0xd0= 208  */
    GLYPH_a157,		/*  0xd1= 209  */
    GLYPH_a158,		/*  0xd2= 210  */
    GLYPH_a159,		/*  0xd3= 211  */
    GLYPH_a160,		/*  0xd4= 212  */
    GLYPH_a161,		/*  0xd5= 213  */
    GLYPH_a163,		/*  0xd6= 214  */
    GLYPH_a164,		/*  0xd7= 215  */
    GLYPH_a196,		/*  0xd8= 216  */
    GLYPH_a165,		/*  0xd9= 217  */
    GLYPH_a192,		/*  0xda= 218  */
    GLYPH_a166,		/*  0xdb= 219  */
    GLYPH_a167,		/*  0xdc= 220  */
    GLYPH_a168,		/*  0xdd= 221  */
    GLYPH_a169,		/*  0xde= 222  */
    GLYPH_a170,		/*  0xdf= 223  */
    GLYPH_a171,		/*  0xe0= 224  */
    GLYPH_a172,		/*  0xe1= 225  */
    GLYPH_a173,		/*  0xe2= 226  */
    GLYPH_a162,		/*  0xe3= 227  */
    GLYPH_a174,		/*  0xe4= 228  */
    GLYPH_a175,		/*  0xe5= 229  */
    GLYPH_a176,		/*  0xe6= 230  */
    GLYPH_a177,		/*  0xe7= 231  */
    GLYPH_a178,		/*  0xe8= 232  */
    GLYPH_a179,		/*  0xe9= 233  */
    GLYPH_a193,		/*  0xea= 234  */
    GLYPH_a180,		/*  0xeb= 235  */
    GLYPH_a199,		/*  0xec= 236  */
    GLYPH_a181,		/*  0xed= 237  */
    GLYPH_a200,		/*  0xee= 238  */
    GLYPH_a182,		/*  0xef= 239  */
    (char *)0,		/*  0xf0= 240  */
    GLYPH_a201,		/*  0xf1= 241  */
    GLYPH_a183,		/*  0xf2= 242  */
    GLYPH_a184,		/*  0xf3= 243  */
    GLYPH_a197,		/*  0xf4= 244  */
    GLYPH_a185,		/*  0xf5= 245  */
    GLYPH_a194,		/*  0xf6= 246  */
    GLYPH_a198,		/*  0xf7= 247  */
    GLYPH_a186,		/*  0xf8= 248  */
    GLYPH_a195,		/*  0xf9= 249  */
    GLYPH_a187,		/*  0xfa= 250  */
    GLYPH_a188,		/*  0xfb= 251  */
    GLYPH_a189,		/*  0xfc= 252  */
    GLYPH_a190,		/*  0xfd= 253  */
    GLYPH_a191,		/*  0xfe= 254  */
    (char *)0,		/*  0xff= 255  */
    };

/************************************************************************/
/*									*/
/*  Alternate glyphs or glyph names as I have encontered them in	*/
/*  actual afm files.							*/
/*									*/
/*  The mapping og "nobreakspace" to "space" is a hack to use this	*/
/*  mechanism to find the match between the latin* encodings and	*/
/*									*/
/************************************************************************/

const AlternateGlyphName PS_AlternateNames[]=
    {
	{ GLYPH_nobreakspace,		GLYPH_space		},

	{ GLYPH_quoteright,		GLYPH_apostrophe	},
	{ GLYPH_minus,			GLYPH_hyphen		},
	{ GLYPH_period,			GLYPH_fullstop		},
	{ GLYPH_underscore,		GLYPH_lowline		},
	{ GLYPH_quoteleft,		GLYPH_grave		},

	{ GLYPH_Lslash,			GLYPH_Lstroke		},
	{ GLYPH_dieresis,		GLYPH_diaeresis		},
	{ GLYPH_Zdotaccent,		GLYPH_Zabovedot		},
	{ GLYPH_lslash,			GLYPH_lstroke		},
	{ GLYPH_hungarumlaut,		GLYPH_doubleacute	},
	{ GLYPH_zdotaccent,		GLYPH_zabovedot		},
	{ GLYPH_Adieresis,		GLYPH_Adiaeresis	},
	{ GLYPH_Edieresis,		GLYPH_Ediaeresis	},
	{ GLYPH_Dcroat,			GLYPH_Dstroke		},
	{ GLYPH_Ohungarumlaut,		GLYPH_Odoubleacute	},
	{ GLYPH_Odieresis,		GLYPH_Odiaeresis	},
	{ GLYPH_Uhungarumlaut,		GLYPH_Udoubleacute	},
	{ GLYPH_Udieresis,		GLYPH_Udiaeresis	},
	{ GLYPH_germandbls,		GLYPH_ssharp		},
	{ GLYPH_adieresis,		GLYPH_adiaeresis	},
	{ GLYPH_edieresis,		GLYPH_ediaeresis	},
	{ GLYPH_dcroat,			GLYPH_dstroke		},
	{ GLYPH_ohungarumlaut,		GLYPH_odoubleacute	},
	{ GLYPH_odieresis,		GLYPH_odiaeresis	},
	{ GLYPH_divide,			GLYPH_division		},
	{ GLYPH_uhungarumlaut,		GLYPH_udoubleacute	},
	{ GLYPH_udieresis,		GLYPH_udiaeresis	},
	{ GLYPH_dotaccent,		GLYPH_abovedot		},

	{ GLYPH_tcommaaccent,		GLYPH_tcedilla		},
	{ GLYPH_Tcommaaccent,		GLYPH_Tcedilla		},
	{ GLYPH_tcedilla,		GLYPH_tcommaaccent	},
	{ GLYPH_Tcedilla,		GLYPH_Tcommaaccent	},

	{ GLYPH_mu,			GLYPH_mu1		},

	{ GLYPH_macron,			GLYPH_overscore		},

	/* urw latin2 fonts ( Peter Ivanyi ) */
	{ GLYPH_quoteright,		GLYPH_quotesingle	},
	{ GLYPH_hyphen,			GLYPH_sfthyphen		},
	{ GLYPH_degree,			GLYPH_ring		},
	{ GLYPH_Dcroat,			GLYPH_Dslash		},
	{ GLYPH_dcroat,			GLYPH_dmacron		},

	{ (char *)0,			(char *)0	}
    };

