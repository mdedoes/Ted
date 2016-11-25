/************************************************************************/
/*									*/
/*  Map Font Encodings							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

/************************************************************************/
/*									*/
/*  Character set mapping tables.					*/
/*									*/
/************************************************************************/

const unsigned char docWIN1250_to_ISO2[256]=
    {
    ISO2__nul,			/*	WIN1250__nul		0	*/
    ISO2__soh,			/*	WIN1250__soh		1	*/
    ISO2__stx,			/*	WIN1250__stx		2	*/
    ISO2__etx,			/*	WIN1250__etx		3	*/
    ISO2__eot,			/*	WIN1250__eot		4	*/
    ISO2__enq,			/*	WIN1250__enq		5	*/
    ISO2__ack,			/*	WIN1250__ack		6	*/
    ISO2__bel,			/*	WIN1250__bel		7	*/
    ISO2__bs,			/*	WIN1250__bs		8	*/
    ISO2__ht,			/*	WIN1250__ht		9	*/
    ISO2__nl,			/*	WIN1250__nl		10	*/
    ISO2__vt,			/*	WIN1250__vt		11	*/
    ISO2__np,			/*	WIN1250__np		12	*/
    ISO2__cr,			/*	WIN1250__cr		13	*/
    ISO2__so,			/*	WIN1250__so		14	*/
    ISO2__si,			/*	WIN1250__si		15	*/
    ISO2__dle,			/*	WIN1250__dle		16	*/
    ISO2__dc1,			/*	WIN1250__dc1		17	*/
    ISO2__dc2,			/*	WIN1250__dc2		18	*/
    ISO2__dc3,			/*	WIN1250__dc3		19	*/
    ISO2__dc4,			/*	WIN1250__dc4		20	*/
    ISO2__nak,			/*	WIN1250__nak		21	*/
    ISO2__syn,			/*	WIN1250__syn		22	*/
    ISO2__etb,			/*	WIN1250__etb		23	*/
    ISO2__can,			/*	WIN1250__can		24	*/
    ISO2__em,			/*	WIN1250__em		25	*/
    ISO2__sub,			/*	WIN1250__sub		26	*/
    ISO2__esc,			/*	WIN1250__esc		27	*/
    ISO2__fs,			/*	WIN1250__fs		28	*/
    ISO2__gs,			/*	WIN1250__gs		29	*/
    ISO2__rs,			/*	WIN1250__rs		30	*/
    ISO2__us,			/*	WIN1250__us		31	*/

    ISO2_space,			/*	WIN1250_space		32	*/
    ISO2_exclam,		/*	WIN1250_exclam		33	*/
    ISO2_quotedbl,		/*	WIN1250_quotedbl	34	*/
    ISO2_numbersign,		/*	WIN1250_numbersign	35	*/
    ISO2_dollar,		/*	WIN1250_dollar		36	*/
    ISO2_percent,		/*	WIN1250_percent		37	*/
    ISO2_ampersand,		/*	WIN1250_ampersand	38	*/
    ISO2_quoteright,		/*	WIN1250_quoteright	39	*/
    ISO2_parenleft,		/*	WIN1250_parenleft	40	*/
    ISO2_parenright,		/*	WIN1250_parenright	41	*/
    ISO2_asterisk,		/*	WIN1250_asterisk	42	*/
    ISO2_plus,			/*	WIN1250_plus		43	*/
    ISO2_comma,			/*	WIN1250_comma		44	*/
    ISO2_minus,			/*	WIN1250_minus		45	*/
    ISO2_period,		/*	WIN1250_period		46	*/
    ISO2_slash,			/*	WIN1250_slash		47	*/
    ISO2_zero,			/*	WIN1250_zero		48	*/
    ISO2_one,			/*	WIN1250_one		49	*/
    ISO2_two,			/*	WIN1250_two		50	*/
    ISO2_three,			/*	WIN1250_three		51	*/
    ISO2_four,			/*	WIN1250_four		52	*/
    ISO2_five,			/*	WIN1250_five		53	*/
    ISO2_six,			/*	WIN1250_six		54	*/
    ISO2_seven,			/*	WIN1250_seven		55	*/
    ISO2_eight,			/*	WIN1250_eight		56	*/
    ISO2_nine,			/*	WIN1250_nine		57	*/
    ISO2_colon,			/*	WIN1250_colon		58	*/
    ISO2_semicolon,		/*	WIN1250_semicolon	59	*/
    ISO2_less,			/*	WIN1250_less		60	*/
    ISO2_equal,			/*	WIN1250_equal		61	*/
    ISO2_greater,		/*	WIN1250_greater		62	*/
    ISO2_question,		/*	WIN1250_question	63	*/

    ISO2_at,			/*	WIN1250_at		64	*/
    ISO2_A,			/*	WIN1250_A		65	*/
    ISO2_B,			/*	WIN1250_B		66	*/
    ISO2_C,			/*	WIN1250_C		67	*/
    ISO2_D,			/*	WIN1250_D		68	*/
    ISO2_E,			/*	WIN1250_E		69	*/
    ISO2_F,			/*	WIN1250_F		70	*/
    ISO2_G,			/*	WIN1250_G		71	*/
    ISO2_H,			/*	WIN1250_H		72	*/
    ISO2_I,			/*	WIN1250_I		73	*/
    ISO2_J,			/*	WIN1250_J		74	*/
    ISO2_K,			/*	WIN1250_K		75	*/
    ISO2_L,			/*	WIN1250_L		76	*/
    ISO2_M,			/*	WIN1250_M		77	*/
    ISO2_N,			/*	WIN1250_N		78	*/
    ISO2_O,			/*	WIN1250_O		79	*/
    ISO2_P,			/*	WIN1250_P		80	*/
    ISO2_Q,			/*	WIN1250_Q		81	*/
    ISO2_R,			/*	WIN1250_R		82	*/
    ISO2_S,			/*	WIN1250_S		83	*/
    ISO2_T,			/*	WIN1250_T		84	*/
    ISO2_U,			/*	WIN1250_U		85	*/
    ISO2_V,			/*	WIN1250_V		86	*/
    ISO2_W,			/*	WIN1250_W		87	*/
    ISO2_X,			/*	WIN1250_X		88	*/
    ISO2_Y,			/*	WIN1250_Y		89	*/
    ISO2_Z,			/*	WIN1250_Z		90	*/

    ISO2_bracketleft,		/*	WIN1250_bracketleft	91	*/
    ISO2_backslash,		/*	WIN1250_backslash	92	*/
    ISO2_bracketright,		/*	WIN1250_bracketright	93	*/
    ISO2_circumflex,		/*	WIN1250_circumflex	94	*/
    ISO2_underscore,		/*	WIN1250_underscore	95	*/

    ISO2_quoteleft,		/*	WIN1250_quoteleft	96	*/
    ISO2_a,			/*	WIN1250_a		97	*/
    ISO2_b,			/*	WIN1250_b		98	*/
    ISO2_c,			/*	WIN1250_c		99	*/
    ISO2_d,			/*	WIN1250_d		100	*/
    ISO2_e,			/*	WIN1250_e		101	*/
    ISO2_f,			/*	WIN1250_f		102	*/
    ISO2_g,			/*	WIN1250_g		103	*/
    ISO2_h,			/*	WIN1250_h		104	*/
    ISO2_i,			/*	WIN1250_i		105	*/
    ISO2_j,			/*	WIN1250_j		106	*/
    ISO2_k,			/*	WIN1250_k		107	*/
    ISO2_l,			/*	WIN1250_l		108	*/
    ISO2_m,			/*	WIN1250_m		109	*/
    ISO2_n,			/*	WIN1250_n		110	*/
    ISO2_o,			/*	WIN1250_o		111	*/
    ISO2_p,			/*	WIN1250_p		112	*/
    ISO2_q,			/*	WIN1250_q		113	*/
    ISO2_r,			/*	WIN1250_r		114	*/
    ISO2_s,			/*	WIN1250_s		115	*/
    ISO2_t,			/*	WIN1250_t		116	*/
    ISO2_u,			/*	WIN1250_u		117	*/
    ISO2_v,			/*	WIN1250_v		118	*/
    ISO2_w,			/*	WIN1250_w		119	*/
    ISO2_x,			/*	WIN1250_x		120	*/
    ISO2_y,			/*	WIN1250_y		121	*/
    ISO2_z,			/*	WIN1250_z		122	*/
    ISO2_braceleft,		/*	WIN1250_braceleft	123	*/
    ISO2_bar,			/*	WIN1250_bar		124	*/
    ISO2_braceright,		/*	WIN1250_braceright	125	*/
    ISO2_tilde,			/*	WIN1250_tilde		126	*/
    ISO2__del,			/*	WIN1250__del		127	*/

    128,			/*	WIN1250__0x80		128	*//**/
    129,			/*	WIN1250__0x81		129	*//**/
    ISO2_quoteleft,		/*	WIN1250_quotesinglbase	130	*//**/
    ISO2_f,			/*	WIN1250_florin		131	*//**/
    ISO2_quotedbl,		/*	WIN1250_quotedblbase	132	*//**/
    ISO2_hyphen,		/*	WIN1250_ellipsis	133	*//**/
    ISO2_currency,		/*	WIN1250_dagger		134	*//**/
    ISO2_currency,		/*	WIN1250_daggerdbl	135	*//**/
    ISO2_circumflex,		/*	WIN1250__circumflex_136	136	*//**/
    ISO2_currency,		/*	WIN1250_perthousand	137	*//**/
    ISO2_Scaron,		/*	WIN1250_Scaron		138	*/
    ISO2_quoteleft,		/*	WIN1250_guilsinglleft	139	*//**/
    ISO2_Sacute,		/*	WIN1250_Sacute		140	*/
    ISO2_Tcaron,		/*	WIN1250_Tcaron		141	*/
    ISO2_Zcaron,		/*	WIN1250_Zcaron		142	*/
    ISO2_Zacute,		/*	WIN1250_Zacute		143	*/
    144,			/*	WIN1250__0x90		144	*//**/
    ISO2_quoteleft,		/*	WIN1250__quoteleft_145	145	*//**/
    ISO2_quoteright,		/*	WIN1250__quoteright_146	146	*//**/
    ISO2_quotedbl,		/*	WIN1250_quotedblleft	147	*/
    ISO2_quotedbl,		/*	WIN1250_quotedblright	148	*/
    ISO2_currency,		/*	WIN1250_bullet		149	*//**/
    ISO2_hyphen,		/*	WIN1250_endash		150	*//**/
    ISO2_hyphen,		/*	WIN1250_emdash		151	*//**/
    ISO2_tilde,			/*	WIN1250__tilde_152	152	*//**/
    ISO2_currency,		/*	WIN1250_trademark	153	*//**/
    ISO2_scaron,		/*	WIN1250_scaron		154	*/
    ISO2_quoteright,		/*	WIN1250_guilsinglright	155	*//**/
    ISO2_sacute,		/*	WIN1250_sacute		156	*/
    ISO2_tcaron,		/*	WIN1250_tcaron		157	*/
    ISO2_zcaron,		/*	WIN1250_zcaron		158	*/
    ISO2_zacute,		/*	WIN1250_zacute		159	*/
    160,			/*	WIN1250__0xa0		160	*//**/
    ISO2_caron,			/*	WIN1250_caron		161	*/
    ISO2_breve,			/*	WIN1250_breve		162	*/
    ISO2_Lslash,		/*	WIN1250_Lslash		163	*/
    ISO2_currency,		/*	WIN1250_currency	164	*/
    ISO2_Aogonek,		/*	WIN1250_Aogonek		165	*/
    ISO2_bar,			/*	WIN1250_brokenbar	166	*//**/
    ISO2_section,		/*	WIN1250_section		167	*/
    ISO2_dieresis,		/*	WIN1250_dieresis	168	*/
    ISO2_currency,		/*	WIN1250_copyright	169	*//**/
    ISO2_Scedilla,		/*	WIN1250_Scedilla	170	*/
    ISO2_quoteleft,		/*	WIN1250_guillemotleft	171	*//**/
    ISO2_currency,		/*	WIN1250_logicalnot	172	*//**/
    ISO2_hyphen,		/*	WIN1250_hyphen		173	*/
    ISO2_currency,		/*	WIN1250_registered	174	*//**/
    ISO2_Zdotaccent,		/*	WIN1250_Zdotaccent	175	*/
    ISO2_degree,		/*	WIN1250_degree		176	*/
    ISO2_currency,		/*	WIN1250_plusminus	177	*/
    ISO2_ogonek,		/*	WIN1250_ogonek		178	*/
    ISO2_lslash,		/*	WIN1250_lslash		179	*/
    ISO2_acute,			/*	WIN1250_acute		180	*/
    ISO2_currency,		/*	WIN1250_mu		181	*//**/
    ISO2_currency,		/*	WIN1250_paragraph	182	*//**/
    ISO2_currency,		/*	WIN1250_periodcentered	183	*//**/
    ISO2_cedilla,		/*	WIN1250_cedilla		184	*/
    ISO2_aogonek,		/*	WIN1250_aogonek		185	*/
    ISO2_scedilla,		/*	WIN1250_scedilla	186	*/
    ISO2_quoteright,		/*	WIN1250_guillemotright	187	*//**/
    ISO2_Lcaron,		/*	WIN1250_Lcaron		188	*/
    ISO2_hungarumlaut,		/*	WIN1250_hungarumlaut	189	*/
    ISO2_lcaron,		/*	WIN1250_lcaron		190	*/
    ISO2_zdotaccent,		/*	WIN1250_zdotaccent	191	*/
    ISO2_Racute,		/*	WIN1250_Racute		192	*/
    ISO2_Aacute,		/*	WIN1250_Aacute		193	*/
    ISO2_Acircumflex,		/*	WIN1250_Acircumflex	194	*/
    ISO2_Abreve,		/*	WIN1250_Abreve		195	*/
    ISO2_Adieresis,		/*	WIN1250_Adieresis	195	*/
    ISO2_Lacute,		/*	WIN1250_Lacute		197	*/
    ISO2_Cacute,		/*	WIN1250_Cacute		198	*/
    ISO2_Ccedilla,		/*	WIN1250_Ccedilla	199	*/
    ISO2_Ccaron,		/*	WIN1250_Ccaron		200	*/
    ISO2_Eacute,		/*	WIN1250_Eacute		201	*/
    ISO2_Eogonek,		/*	WIN1250_Eogonek		202	*/
    ISO2_Edieresis,		/*	WIN1250_Edieresis	202	*/
    ISO2_Ecaron,		/*	WIN1250_Ecaron		204	*/
    ISO2_Iacute,		/*	WIN1250_Iacute		204	*/
    ISO2_Icircumflex,		/*	WIN1250_Icircumflex	204	*/
    ISO2_Dcaron,		/*	WIN1250_Dcaron		207	*/
    ISO2_Dcroat,		/*	WIN1250_Dcroat		208	*/
    ISO2_Nacute,		/*	WIN1250_Nacute		209	*/
    ISO2_Ncaron,		/*	WIN1250_Ncaron		210	*/
    ISO2_Oacute,		/*	WIN1250_Oacute		211	*/
    ISO2_Ocircumflex,		/*	WIN1250_Ocircumflex	212	*/
    ISO2_Ohungarumlaut,		/*	WIN1250_Ohungarumlaut	213	*/
    ISO2_Odieresis,		/*	WIN1250_Odieresis	214	*/
    ISO2_multiply,		/*	WIN1250_multiply	215	*/
    ISO2_Rcaron,		/*	WIN1250_Rcaron		216	*/
    ISO2_Uring,			/*	WIN1250_Uring		217	*/
    ISO2_Uacute,		/*	WIN1250_Uacute		218	*/
    ISO2_Uhungarumlaut,		/*	WIN1250_Uhungarumlaut	219	*/
    ISO2_Udieresis,		/*	WIN1250_Udieresis	220	*/
    ISO2_Yacute,		/*	WIN1250_Yacute		221	*/
    ISO2_Tcedilla,		/*	WIN1250_Tcedilla	222	*/
    ISO2_germandbls,		/*	WIN1250_germandbls	223	*/
    ISO2_racute,		/*	WIN1250_racute		224	*/
    ISO2_aacute,		/*	WIN1250_aacute		225	*/
    ISO2_acircumflex,		/*	WIN1250_acircumflex	226	*/
    ISO2_abreve,		/*	WIN1250_abreve		227	*/
    ISO2_adieresis,		/*	WIN1250_adieresis	228	*/
    ISO2_lacute,		/*	WIN1250_lacute		229	*/
    ISO2_cacute,		/*	WIN1250_cacute		230	*/
    ISO2_ccedilla,		/*	WIN1250_ccedilla	231	*/
    ISO2_ccaron,		/*	WIN1250_ccaron		232	*/
    ISO2_eacute,		/*	WIN1250_eacute		233	*/
    ISO2_eogonek,		/*	WIN1250_eogonek		234	*/
    ISO2_edieresis,		/*	WIN1250_edieresis	235	*/
    ISO2_ecaron,		/*	WIN1250_ecaron		236	*/
    ISO2_iacute,		/*	WIN1250_iacute		237	*/
    ISO2_icircumflex,		/*	WIN1250_icircumflex	238	*/
    ISO2_dcaron,		/*	WIN1250_dcaron		239	*/
    ISO2_dcroat,		/*	WIN1250_dcroat		240	*/
    ISO2_nacute,		/*	WIN1250_nacute		241	*/
    ISO2_ncaron,		/*	WIN1250_ncaron		242	*/
    ISO2_oacute,		/*	WIN1250_oacute		243	*/
    ISO2_ocircumflex,		/*	WIN1250_ocircumflex	244	*/
    ISO2_ohungarumlaut,		/*	WIN1250_ohungarumlaut	245	*/
    ISO2_odieresis,		/*	WIN1250_odieresis	246	*/
    ISO2_divide,		/*	WIN1250_divide		247	*/
    ISO2_rcaron,		/*	WIN1250_rcaron		248	*/
    ISO2_uring,			/*	WIN1250_uring		249	*/
    ISO2_uacute,		/*	WIN1250_uacute		250	*/
    ISO2_uhungarumlaut,		/*	WIN1250_uhungarumlaut	251	*/
    ISO2_udieresis,		/*	WIN1250_udieresis	252	*/
    ISO2_yacute,		/*	WIN1250_yacute		252	*/
    ISO2_tcedilla,		/*	WIN1250_tcedilla	254	*/
    ISO2_dotaccent,		/*	WIN1250_dotaccent	255	*/
    };

const unsigned char docISO2_to_WIN1250[256]=
    {
    WIN1250__nul,		/*	ISO2__nul		0	*/
    WIN1250__soh,		/*	ISO2__soh		1	*/
    WIN1250__stx,		/*	ISO2__stx		2	*/
    WIN1250__etx,		/*	ISO2__etx		3	*/
    WIN1250__eot,		/*	ISO2__eot		4	*/
    WIN1250__enq,		/*	ISO2__enq		5	*/
    WIN1250__ack,		/*	ISO2__ack		6	*/
    WIN1250__bel,		/*	ISO2__bel		7	*/
    WIN1250__bs,		/*	ISO2__bs		8	*/
    WIN1250__ht,		/*	ISO2__ht		9	*/
    WIN1250__nl,		/*	ISO2__nl		10	*/
    WIN1250__vt,		/*	ISO2__vt		11	*/
    WIN1250__np,		/*	ISO2__np		12	*/
    WIN1250__cr,		/*	ISO2__cr		13	*/
    WIN1250__so,		/*	ISO2__so		14	*/
    WIN1250__si,		/*	ISO2__si		15	*/
    WIN1250__dle,		/*	ISO2__dle		16	*/
    WIN1250__dc1,		/*	ISO2__dc1		17	*/
    WIN1250__dc2,		/*	ISO2__dc2		18	*/
    WIN1250__dc3,		/*	ISO2__dc3		19	*/
    WIN1250__dc4,		/*	ISO2__dc4		20	*/
    WIN1250__nak,		/*	ISO2__nak		21	*/
    WIN1250__syn,		/*	ISO2__syn		22	*/
    WIN1250__etb,		/*	ISO2__etb		23	*/
    WIN1250__can,		/*	ISO2__can		24	*/
    WIN1250__em,		/*	ISO2__em		25	*/
    WIN1250__sub,		/*	ISO2__sub		26	*/
    WIN1250__esc,		/*	ISO2__esc		27	*/
    WIN1250__fs,		/*	ISO2__fs		28	*/
    WIN1250__gs,		/*	ISO2__gs		29	*/
    WIN1250__rs,		/*	ISO2__rs		30	*/
    WIN1250__us,		/*	ISO2__us		31	*/

    WIN1250_space,		/*	ISO2_space		32	*/
    WIN1250_exclam,		/*	ISO2_exclam		33	*/
    WIN1250_quotedbl,		/*	ISO2_quotedbl		34	*/
    WIN1250_numbersign,		/*	ISO2_numbersign		35	*/
    WIN1250_dollar,		/*	ISO2_dollar		36	*/
    WIN1250_percent,		/*	ISO2_percent		37	*/
    WIN1250_ampersand,		/*	ISO2_ampersand		38	*/
    WIN1250_quoteright,		/*	ISO2_quoteright		39	*/
    WIN1250_parenleft,		/*	ISO2_parenleft		40	*/
    WIN1250_parenright,		/*	ISO2_parenright		41	*/
    WIN1250_asterisk,		/*	ISO2_asterisk		42	*/
    WIN1250_plus,		/*	ISO2_plus		43	*/
    WIN1250_comma,		/*	ISO2_comma		44	*/
    WIN1250_minus,		/*	ISO2_minus		45	*/
    WIN1250_period,		/*	ISO2_period		46	*/
    WIN1250_slash,		/*	ISO2_slash		47	*/
    WIN1250_zero,		/*	ISO2_zero		48	*/
    WIN1250_one,		/*	ISO2_one		49	*/
    WIN1250_two,		/*	ISO2_two		50	*/
    WIN1250_three,		/*	ISO2_three		51	*/
    WIN1250_four,		/*	ISO2_four		52	*/
    WIN1250_five,		/*	ISO2_five		53	*/
    WIN1250_six,		/*	ISO2_six		54	*/
    WIN1250_seven,		/*	ISO2_seven		55	*/
    WIN1250_eight,		/*	ISO2_eight		56	*/
    WIN1250_nine,		/*	ISO2_nine		57	*/
    WIN1250_colon,		/*	ISO2_colon		58	*/
    WIN1250_semicolon,		/*	ISO2_semicolon		59	*/
    WIN1250_less,		/*	ISO2_less		60	*/
    WIN1250_equal,		/*	ISO2_equal		61	*/
    WIN1250_greater,		/*	ISO2_greater		62	*/
    WIN1250_question,		/*	ISO2_question		63	*/

    WIN1250_at,			/*	ISO2_at			64	*/
    WIN1250_A,			/*	ISO2_A			65	*/
    WIN1250_B,			/*	ISO2_B			66	*/
    WIN1250_C,			/*	ISO2_C			67	*/
    WIN1250_D,			/*	ISO2_D			68	*/
    WIN1250_E,			/*	ISO2_E			69	*/
    WIN1250_F,			/*	ISO2_F			70	*/
    WIN1250_G,			/*	ISO2_G			71	*/
    WIN1250_H,			/*	ISO2_H			72	*/
    WIN1250_I,			/*	ISO2_I			73	*/
    WIN1250_J,			/*	ISO2_J			74	*/
    WIN1250_K,			/*	ISO2_K			75	*/
    WIN1250_L,			/*	ISO2_L			76	*/
    WIN1250_M,			/*	ISO2_M			77	*/
    WIN1250_N,			/*	ISO2_N			78	*/
    WIN1250_O,			/*	ISO2_O			79	*/
    WIN1250_P,			/*	ISO2_P			80	*/
    WIN1250_Q,			/*	ISO2_Q			81	*/
    WIN1250_R,			/*	ISO2_R			82	*/
    WIN1250_S,			/*	ISO2_S			83	*/
    WIN1250_T,			/*	ISO2_T			84	*/
    WIN1250_U,			/*	ISO2_U			85	*/
    WIN1250_V,			/*	ISO2_V			86	*/
    WIN1250_W,			/*	ISO2_W			87	*/
    WIN1250_X,			/*	ISO2_X			88	*/
    WIN1250_Y,			/*	ISO2_Y			89	*/
    WIN1250_Z,			/*	ISO2_Z			90	*/
    WIN1250_bracketleft,	/*	ISO2_bracketleft	91	*/
    WIN1250_backslash,		/*	ISO2_backslash		92	*/
    WIN1250_bracketright,	/*	ISO2_bracketright	93	*/
    WIN1250_circumflex,		/*	ISO2_circumflex		94	*/
    WIN1250_underscore,		/*	ISO2_underscore		95	*/

    WIN1250_quoteleft,		/*	ISO2_quoteleft		96	*/
    WIN1250_a,			/*	ISO2_a			97	*/
    WIN1250_b,			/*	ISO2_b			98	*/
    WIN1250_c,			/*	ISO2_c			99	*/
    WIN1250_d,			/*	ISO2_d			100	*/
    WIN1250_e,			/*	ISO2_e			101	*/
    WIN1250_f,			/*	ISO2_f			102	*/
    WIN1250_g,			/*	ISO2_g			103	*/
    WIN1250_h,			/*	ISO2_h			104	*/
    WIN1250_i,			/*	ISO2_i			105	*/
    WIN1250_j,			/*	ISO2_j			106	*/
    WIN1250_k,			/*	ISO2_k			107	*/
    WIN1250_l,			/*	ISO2_l			108	*/
    WIN1250_m,			/*	ISO2_m			109	*/
    WIN1250_n,			/*	ISO2_n			110	*/
    WIN1250_o,			/*	ISO2_o			111	*/
    WIN1250_p,			/*	ISO2_p			112	*/
    WIN1250_q,			/*	ISO2_q			113	*/
    WIN1250_r,			/*	ISO2_r			114	*/
    WIN1250_s,			/*	ISO2_s			115	*/
    WIN1250_t,			/*	ISO2_t			116	*/
    WIN1250_u,			/*	ISO2_u			117	*/
    WIN1250_v,			/*	ISO2_v			118	*/
    WIN1250_w,			/*	ISO2_w			119	*/
    WIN1250_x,			/*	ISO2_x			120	*/
    WIN1250_y,			/*	ISO2_y			121	*/
    WIN1250_z,			/*	ISO2_z			122	*/
    WIN1250_braceleft,		/*	ISO2_braceleft		123	*/
    WIN1250_bar,		/*	ISO2_bar		124	*/
    WIN1250_braceright,		/*	ISO2_braceright		125	*/
    WIN1250_tilde,		/*	ISO2_tilde		126	*/
    WIN1250__del,		/*	ISO2__del		127	*/

    WIN1250_currency,		/*	.NOTDEF_128		128	*//**/
    WIN1250_currency,		/*	.NOTDEF_129		129	*//**/
    WIN1250_currency,		/*	.NOTDEF_130		130	*//**/
    WIN1250_currency,		/*	.NOTDEF_131		131	*//**/
    WIN1250_currency,		/*	.NOTDEF_132		132	*//**/
    WIN1250_currency,		/*	.NOTDEF_133		133	*//**/
    WIN1250_currency,		/*	.NOTDEF_134		134	*//**/
    WIN1250_currency,		/*	.NOTDEF_135		135	*//**/
    WIN1250_currency,		/*	.NOTDEF_136		136	*//**/
    WIN1250_currency,		/*	.NOTDEF_137		137	*//**/
    WIN1250_currency,		/*	.NOTDEF_138		138	*//**/
    WIN1250_currency,		/*	.NOTDEF_139		139	*//**/
    WIN1250_currency,		/*	.NOTDEF_140		140	*//**/
    WIN1250_currency,		/*	.NOTDEF_141		141	*//**/
    WIN1250_currency,		/*	.NOTDEF_142		142	*//**/
    WIN1250_currency,		/*	.NOTDEF_143		143	*//**/

    WIN1250_currency,		/*	ISO2_dotlessi		144	*//**/
    WIN1250_currency,		/*	ISO2_grave		145	*//**/
    WIN1250_acute,		/*	ISO2__acute_146		146	*/
    WIN1250_circumflex,		/*	ISO2__circumflex	147	*/
    WIN1250_tilde,		/*	ISO2__tilde_148		148	*/
    WIN1250_currency,		/*	ISO2__macron		149	*//**/
    WIN1250_breve,		/*	ISO2__breve_150		150	*/
    WIN1250_dotaccent,		/*	ISO2__dotaccent		151	*/

    WIN1250_dieresis,		/*	ISO2__dieresis		152	*/
    WIN1250_currency,		/*	._NOTDEF_153		153	*/
    WIN1250_degree,		/*	ISO2_ring		154	*//**/
    WIN1250_cedilla,		/*	ISO2__cedilla		155	*/
    WIN1250_currency,		/*	._NOTDEF_156		156	*/
    WIN1250_hungarumlaut,	/*	ISO2__hungarumlaut	157	*/
    WIN1250_ogonek,		/*	ISO2__ogonek		158	*/
    WIN1250_caron,		/*	ISO2__caron_159		159	*/

    WIN1250_space,		/*	ISO2_nobreakspace	160	*//**/
    WIN1250_Aogonek,		/*	ISO2_Aogonek		161	*/
    WIN1250_breve,		/*	ISO2_breve		162	*/
    WIN1250_Lslash,		/*	ISO2_Lslash		163	*/
    WIN1250_currency,		/*	ISO2_currency		164	*/
    WIN1250_Lcaron,		/*	ISO2_Lcaron		165	*/
    WIN1250_Sacute,		/*	ISO2_Sacute		166	*/
    WIN1250_section,		/*	ISO2_section		167	*/

    WIN1250_dieresis,		/*	ISO2_dieresis		168	*/
    WIN1250_Scaron,		/*	ISO2_Scaron		169	*/
    WIN1250_Scedilla,		/*	ISO2_Scedilla		170	*/
    WIN1250_Tcaron,		/*	ISO2_Tcaron		171	*/
    WIN1250_Zacute,		/*	ISO2_Zacute		172	*/
    WIN1250_hyphen,		/*	ISO2_hyphen		173	*/
    WIN1250_Zcaron,		/*	ISO2_Zcaron		174	*/
    WIN1250_Zdotaccent,		/*	ISO2_Zdotaccent		175	*/

    WIN1250_degree,		/*	ISO2_degree		176	*/
    WIN1250_aogonek,		/*	ISO2_aogonek		177	*/
    WIN1250_ogonek,		/*	ISO2_ogonek		178	*/
    WIN1250_lslash,		/*	ISO2_lslash		179	*/
    WIN1250_acute,		/*	ISO2_acute		180	*/
    WIN1250_lcaron,		/*	ISO2_lcaron		181	*/
    WIN1250_sacute,		/*	ISO2_sacute		182	*/
    WIN1250_caron,		/*	ISO2_caron		183	*/

    WIN1250_cedilla,		/*	ISO2_cedilla		184	*/
    WIN1250_scaron,		/*	ISO2_scaron		185	*/
    WIN1250_scedilla,		/*	ISO2_scedilla		186	*/
    WIN1250_tcaron,		/*	ISO2_tcaron		187	*/
    WIN1250_zacute,		/*	ISO2_zacute		188	*/
    WIN1250_hungarumlaut,	/*	ISO2_hungarumlaut	189	*/
    WIN1250_zcaron,		/*	ISO2_zcaron		190	*/
    WIN1250_zdotaccent,		/*	ISO2_zdotaccent		191	*/

    WIN1250_Racute,		/*	ISO2_Racute		192	*/
    WIN1250_Aacute,		/*	ISO2_Aacute		193	*/
    WIN1250_Acircumflex,	/*	ISO2_Acircumflex	194	*/
    WIN1250_Abreve,		/*	ISO2_Abreve		195	*/
    WIN1250_Adieresis,		/*	ISO2_Adieresis		196	*/
    WIN1250_Lacute,		/*	ISO2_Lacute		197	*/
    WIN1250_Cacute,		/*	ISO2_Cacute		198	*/
    WIN1250_Ccedilla,		/*	ISO2_Ccedilla		199	*/

    WIN1250_Ccaron,		/*	ISO2_Ccaron		200	*/
    WIN1250_Eacute,		/*	ISO2_Eacute		201	*/
    WIN1250_Eogonek,		/*	ISO2_Eogonek		202	*/
    WIN1250_Edieresis,		/*	ISO2_Edieresis		203	*/
    WIN1250_Ecaron,		/*	ISO2_Ecaron		204	*/
    WIN1250_Iacute,		/*	ISO2_Iacute		205	*/
    WIN1250_Icircumflex,	/*	ISO2_Icircumflex	206	*/
    WIN1250_Dcaron,		/*	ISO2_Dcaron		207	*/

    WIN1250_Dcroat,		/*	ISO2_Dcroat		208	*/
    WIN1250_Nacute,		/*	ISO2_Nacute		209	*/
    WIN1250_Ncaron,		/*	ISO2_Ncaron		210	*/
    WIN1250_Oacute,		/*	ISO2_Oacute		211	*/
    WIN1250_Ocircumflex,	/*	ISO2_Ocircumflex	212	*/
    WIN1250_Ohungarumlaut,	/*	ISO2_Ohungarumlaut	213	*/
    WIN1250_Odieresis,		/*	ISO2_Odieresis		214	*/
    WIN1250_multiply,		/*	ISO2_multiply		215	*/

    WIN1250_Rcaron,		/*	ISO2_Rcaron		216	*/
    WIN1250_Uring,		/*	ISO2_Uring		217	*/
    WIN1250_Uacute,		/*	ISO2_Uacute		218	*/
    WIN1250_Uhungarumlaut,	/*	ISO2_Uhungarumlaut	219	*/
    WIN1250_Udieresis,		/*	ISO2_Udieresis		220	*/
    WIN1250_Yacute,		/*	ISO2_Yacute		221	*/
    WIN1250_Tcedilla,		/*	ISO2_Tcedilla		222	*/
    WIN1250_germandbls,		/*	ISO2_germandbls		223	*/

    WIN1250_racute,		/*	ISO2_racute		224	*/
    WIN1250_aacute,		/*	ISO2_aacute		225	*/
    WIN1250_acircumflex,	/*	ISO2_acircumflex	226	*/
    WIN1250_abreve,		/*	ISO2_abreve		227	*/
    WIN1250_adieresis,		/*	ISO2_adieresis		228	*/
    WIN1250_lacute,		/*	ISO2_lacute		229	*/
    WIN1250_cacute,		/*	ISO2_cacute		230	*/
    WIN1250_ccedilla,		/*	ISO2_ccedilla		231	*/

    WIN1250_ccaron,		/*	ISO2_ccaron		232	*/
    WIN1250_eacute,		/*	ISO2_eacute		233	*/
    WIN1250_eogonek,		/*	ISO2_eogonek		234	*/
    WIN1250_edieresis,		/*	ISO2_edieresis		235	*/
    WIN1250_ecaron,		/*	ISO2_ecaron		236	*/
    WIN1250_iacute,		/*	ISO2_iacute		237	*/
    WIN1250_icircumflex,	/*	ISO2_icircumflex	238	*/
    WIN1250_dcaron,		/*	ISO2_dcaron		239	*/

    WIN1250_dcroat,		/*	ISO2_dcroat		240	*/
    WIN1250_nacute,		/*	ISO2_nacute		241	*/
    WIN1250_ncaron,		/*	ISO2_ncaron		242	*/
    WIN1250_oacute,		/*	ISO2_oacute		243	*/
    WIN1250_ocircumflex,	/*	ISO2_ocircumflex	244	*/
    WIN1250_ohungarumlaut,	/*	ISO2_ohungarumlaut	245	*/
    WIN1250_odieresis,		/*	ISO2_odieresis		246	*/
    WIN1250_divide,		/*	ISO2_divide		247	*/

    WIN1250_rcaron,		/*	ISO2_rcaron		248	*/
    WIN1250_uring,		/*	ISO2_uring		249	*/
    WIN1250_uacute,		/*	ISO2_uacute		250	*/
    WIN1250_uhungarumlaut,	/*	ISO2_uhungarumlaut	251	*/
    WIN1250_udieresis,		/*	ISO2_udieresis		252	*/
    WIN1250_yacute,		/*	ISO2_yacute		253	*/
    WIN1250_tcedilla,		/*	ISO2_tcedilla		254	*/
    WIN1250_dotaccent,		/*	ISO2_dotaccent		255	*/
    };

const unsigned char	docDOS437_to_ISO1[256]=
    {
    0x00,		/*  			  0	*/
    0x01,		/*  			  1	*/
    0x02,		/*  			  2	*/
    0x03,		/*  			  3	*/
    0x04,		/*  			  4	*/
    0x05,		/*  			  5	*/
    0x06,		/*  			  6	*/
    0x07,		/*  			  7	*/
    0x08,		/*  			  8	*/
    0x09,		/*  			  9	*/
    0x0a,		/*  			 10	*/
    0x0b,		/*  			 11	*/
    0x0c,		/*  			 12	*/
    0x0d,		/*  			 13	*/
    0x0e,		/*  			 14	*/
    0x0f,		/*  			 15	*/
    0x10,		/*  			 16	*/
    0x11,		/*  			 17	*/
    0x12,		/*  			 18	*/
    0x13,		/*  			 19	*/
    0x14,		/*  			 20	*/
    0x15,		/*  			 21	*/
    0x16,		/*  			 22	*/
    0x17,		/*  			 23	*/
    0x18,		/*  			 24	*/
    0x19,		/*  			 25	*/
    0x1a,		/*  			 26	*/
    0x1b,		/*  			 27	*/
    0x1c,		/*  			 28	*/
    0x1d,		/*  			 29	*/
    0x1e,		/*  			 30	*/
    0x1f,		/*  			 31	*/
    ISO1_space,		/*  DOS437_space	 32	*/
    ISO1_exclam,	/*  DOS437_exclam	 33	*/
    ISO1_quotedbl,	/*  DOS437_quotedbl	 34	*/
    ISO1_numbersign,	/*  DOS437_numbersign	 35	*/
    ISO1_dollar,	/*  DOS437_dollar	 36	*/
    ISO1_percent,	/*  DOS437_percent	 37	*/
    ISO1_ampersand,	/*  DOS437_ampersand	 38	*/
    ISO1_quoteright,	/*  DOS437_quoteright	 39	*/
    ISO1_parenleft,	/*  DOS437_parenleft	 40	*/
    ISO1_parenright,	/*  DOS437_parenright	 41	*/
    ISO1_asterisk,	/*  DOS437_asterisk	 42	*/
    ISO1_plus,		/*  DOS437_plus		 43	*/
    ISO1_comma,		/*  DOS437_comma	 44	*/
    ISO1_minus,		/*  DOS437_minus	 45	*/
    ISO1_period,	/*  DOS437_period	 46	*/
    ISO1_slash,		/*  DOS437_slash	 47	*/
    ISO1_zero,		/*  DOS437_zero		 48	*/
    ISO1_one,		/*  DOS437_one		 49	*/
    ISO1_two,		/*  DOS437_two		 50	*/
    ISO1_three,		/*  DOS437_three	 51	*/
    ISO1_four,		/*  DOS437_four		 52	*/
    ISO1_five,		/*  DOS437_five		 53	*/
    ISO1_six,		/*  DOS437_six		 54	*/
    ISO1_seven,		/*  DOS437_seven	 55	*/
    ISO1_eight,		/*  DOS437_eight	 56	*/
    ISO1_nine,		/*  DOS437_nine		 57	*/
    ISO1_colon,		/*  DOS437_colon	 58	*/
    ISO1_semicolon,	/*  DOS437_semicolon	 59	*/
    ISO1_less,		/*  DOS437_less		 60	*/
    ISO1_equal,		/*  DOS437_equal	 61	*/
    ISO1_greater,	/*  DOS437_greater	 62	*/
    ISO1_question,	/*  DOS437_question	 63	*/
    ISO1_at,		/*  DOS437_at		 64	*/
    ISO1_A,		/*  DOS437_A		 65	*/
    ISO1_B,		/*  DOS437_B		 66	*/
    ISO1_C,		/*  DOS437_C		 67	*/
    ISO1_D,		/*  DOS437_D		 68	*/
    ISO1_E,		/*  DOS437_E		 69	*/
    ISO1_F,		/*  DOS437_F		 70	*/
    ISO1_G,		/*  DOS437_G		 71	*/
    ISO1_H,		/*  DOS437_H		 72	*/
    ISO1_I,		/*  DOS437_I		 73	*/
    ISO1_J,		/*  DOS437_J		 74	*/
    ISO1_K,		/*  DOS437_K		 75	*/
    ISO1_L,		/*  DOS437_L		 76	*/
    ISO1_M,		/*  DOS437_M		 77	*/
    ISO1_N,		/*  DOS437_N		 78	*/
    ISO1_O,		/*  DOS437_O		 79	*/
    ISO1_P,		/*  DOS437_P		 80	*/
    ISO1_Q,		/*  DOS437_Q		 81	*/
    ISO1_R,		/*  DOS437_R		 82	*/
    ISO1_S,		/*  DOS437_S		 83	*/
    ISO1_T,		/*  DOS437_T		 84	*/
    ISO1_U,		/*  DOS437_U		 85	*/
    ISO1_V,		/*  DOS437_V		 86	*/
    ISO1_W,		/*  DOS437_W		 87	*/
    ISO1_X,		/*  DOS437_X		 88	*/
    ISO1_Y,		/*  DOS437_Y		 89	*/
    ISO1_Z,		/*  DOS437_Z		 90	*/
    ISO1_bracketleft,	/*  DOS437_bracketleft	 91	*/
    ISO1_backslash,	/*  DOS437_backslash	 92	*/
    ISO1_bracketright,	/*  DOS437_bracketright	 93	*/
    ISO1_circumflex,	/*  DOS437_circumflex	 94	*/
    ISO1_underscore,	/*  DOS437_underscore	 95	*/
    ISO1_quoteleft,	/*  DOS437_quoteleft	 96	*/
    ISO1_a,		/*  DOS437_a		 97	*/
    ISO1_b,		/*  DOS437_b		 98	*/
    ISO1_c,		/*  DOS437_c		 99	*/
    ISO1_d,		/*  DOS437_d		100	*/
    ISO1_e,		/*  DOS437_e		101	*/
    ISO1_f,		/*  DOS437_f		102	*/
    ISO1_g,		/*  DOS437_g		103	*/
    ISO1_h,		/*  DOS437_h		104	*/
    ISO1_i,		/*  DOS437_i		105	*/
    ISO1_j,		/*  DOS437_j		106	*/
    ISO1_k,		/*  DOS437_k		107	*/
    ISO1_l,		/*  DOS437_l		108	*/
    ISO1_m,		/*  DOS437_m		109	*/
    ISO1_n,		/*  DOS437_n		110	*/
    ISO1_o,		/*  DOS437_o		111	*/
    ISO1_p,		/*  DOS437_p		112	*/
    ISO1_q,		/*  DOS437_q		113	*/
    ISO1_r,		/*  DOS437_r		114	*/
    ISO1_s,		/*  DOS437_s		115	*/
    ISO1_t,		/*  DOS437_t		116	*/
    ISO1_u,		/*  DOS437_u		117	*/
    ISO1_v,		/*  DOS437_v		118	*/
    ISO1_w,		/*  DOS437_w		119	*/
    ISO1_x,		/*  DOS437_x		120	*/
    ISO1_y,		/*  DOS437_y		121	*/
    ISO1_z,		/*  DOS437_z		122	*/
    ISO1_braceleft,	/*  DOS437_braceleft	123	*/
    ISO1_bar,		/*  DOS437_bar		124	*/
    ISO1_braceright,	/*  DOS437_braceright	125	*/
    ISO1_tilde,		/*  DOS437_tilde	126	*/
    0x7f,		/*  0x7f		127	*/
    ISO1_Ccedilla,	/*  DOS437_Ccedilla	128	*/
    ISO1_udieresis,	/*  DOS437_udieresis	129	*/
    ISO1_eacute,	/*  DOS437_eacute	130	*/
    ISO1_acircumflex,	/*  DOS437_acircumflex	131	*/
    ISO1_adieresis,	/*  DOS437_adieresis	132	*/
    ISO1_agrave,	/*  DOS437_agrave	133	*/
    ISO1_aring,		/*  DOS437_aring	134	*/
    ISO1_ccedilla,	/*  DOS437_ccedilla	135	*/
    ISO1_ecircumflex,	/*  DOS437_ecircumflex	136	*/
    ISO1_edieresis,	/*  DOS437_edieresis	137	*/
    ISO1_egrave,	/*  DOS437_egrave	138	*/
    ISO1_idieresis,	/*  DOS437_idieresis	139	*/
    ISO1_icircumflex,	/*  DOS437_icircumflex	140	*/
    ISO1_igrave,	/*  DOS437_igrave	141	*/
    ISO1_Adieresis,	/*  DOS437_Adieresis	142	*/
    ISO1_Aring,		/*  DOS437_Aring	143	*/
    ISO1_Eacute,	/*  DOS437_Eacute	144	*/
    ISO1_ae,		/*  DOS437_ae		145	*/
    ISO1_AE,		/*  DOS437_AE		146	*/
    ISO1_ocircumflex,	/*  DOS437_ocircumflex	147	*/
    ISO1_odieresis,	/*  DOS437_odieresis	148	*/
    ISO1_ograve,	/*  DOS437_ograve	149	*/
    ISO1_ucircumflex,	/*  DOS437_ucircumflex	150	*/
    ISO1_ugrave,	/*  DOS437_ugrave	151	*/
    ISO1_ydieresis,	/*  DOS437_ydieresis	152	*/
    ISO1_Odieresis,	/*  DOS437_Odieresis	153	*/
    ISO1_Udieresis,	/*  DOS437_Udieresis	154	*/
    ISO1_cent,		/*  DOS437_cent		155	*/
    ISO1_sterling,	/*  DOS437_sterling	156	*/
    ISO1_yen,		/*  DOS437_yen		157	*/
    ISO1_currency,	/*  DOS437_peseta	158	*//**/
    ISO1_currency,	/*  DOS437_florin	159	*//**/
    ISO1_aacute,	/*  DOS437_aacute	160	*/
    ISO1_iacute,	/*  DOS437_iacute	161	*/
    ISO1_oacute,	/*  DOS437_oacute	162	*/
    ISO1_uacute,	/*  DOS437_uacute	163	*/
    ISO1_ntilde,	/*  DOS437_ntilde	164	*/
    ISO1_Ntilde,	/*  DOS437_Ntilde	165	*/
    ISO1_ordfeminine,	/*  DOS437_ordfeminine	166	*/
    ISO1_ordmasculine,	/*  DOS437_ordmasculine	167	*/
    ISO1_questiondown,	/*  DOS437_questiondown	168	*/
    ISO1_currency,	/*  DOS437_revlogicalnot 169	*//**/
    ISO1_logicalnot,	/*  DOS437_logicalnot	170	*/
    ISO1_onehalf,	/*  DOS437_onehalf	171	*/
    ISO1_onequarter,	/*  DOS437_onequarter	172	*/
    ISO1_exclamdown,	/*  DOS437_exclamdown	173	*/
    ISO1_guillemotleft,	/*  DOS437_guillemotleft 174	*/
    ISO1_guillemotright,/*  DOS437_guillemotright 175	*/
    ISO1_currency,	/*  DOS437_ltshade	176	*//**/
    ISO1_currency,	/*  DOS437_shade	177	*//**/
    ISO1_currency,	/*  DOS437_dkshade	178	*//**/
    ISO1_currency,	/*  DOS437_SF110000	179	*//**/
    ISO1_currency,	/*  DOS437_SF090000	180	*//**/
    ISO1_currency,	/*  DOS437_SF190000	181	*//**/
    ISO1_currency,	/*  DOS437_SF200000	182	*//**/
    ISO1_currency,	/*  DOS437_SF210000	183	*//**/
    ISO1_currency,	/*  DOS437_SF220000	184	*//**/
    ISO1_currency,	/*  DOS437_SF230000	185	*//**/
    ISO1_currency,	/*  DOS437_SF240000	186	*//**/
    ISO1_currency,	/*  DOS437_SF250000	187	*//**/
    ISO1_currency,	/*  DOS437_SF260000	188	*//**/
    ISO1_currency,	/*  DOS437_SF270000	189	*//**/
    ISO1_currency,	/*  DOS437_SF280000	190	*//**/
    ISO1_currency,	/*  DOS437_SF030000	191	*//**/
    ISO1_currency,	/*  DOS437_SF020000	192	*//**/
    ISO1_currency,	/*  DOS437_SF070000	193	*//**/
    ISO1_currency,	/*  DOS437_SF060000	194	*//**/
    ISO1_currency,	/*  DOS437_SF080000	195	*//**/
    ISO1_currency,	/*  DOS437_SF100000	196	*//**/
    ISO1_currency,	/*  DOS437_SF050000	197	*//**/
    ISO1_currency,	/*  DOS437_SF360000	198	*//**/
    ISO1_currency,	/*  DOS437_SF370000	199	*//**/
    ISO1_currency,	/*  DOS437_SF380000	200	*//**/
    ISO1_currency,	/*  DOS437_SF390000	201	*//**/
    ISO1_currency,	/*  DOS437_SF400000	202	*//**/
    ISO1_currency,	/*  DOS437_SF410000	203	*//**/
    ISO1_currency,	/*  DOS437_SF420000	204	*//**/
    ISO1_currency,	/*  DOS437_SF430000	205	*//**/
    ISO1_currency,	/*  DOS437_SF440000	206	*//**/
    ISO1_currency,	/*  DOS437_SF450000	207	*//**/
    ISO1_currency,	/*  DOS437_SF460000	208	*//**/
    ISO1_currency,	/*  DOS437_SF470000	209	*//**/
    ISO1_currency,	/*  DOS437_SF480000	210	*//**/
    ISO1_currency,	/*  DOS437_SF490000	211	*//**/
    ISO1_currency,	/*  DOS437_SF500000	212	*//**/
    ISO1_currency,	/*  DOS437_SF510000	213	*//**/
    ISO1_currency,	/*  DOS437_SF520000	214	*//**/
    ISO1_currency,	/*  DOS437_SF530000	215	*//**/
    ISO1_currency,	/*  DOS437_SF540000	216	*//**/
    ISO1_currency,	/*  DOS437_SF040000	217	*//**/
    ISO1_currency,	/*  DOS437_SF010000	218	*//**/
    ISO1_currency,	/*  DOS437_block	219	*//**/
    ISO1_currency,	/*  DOS437_dnblock	220	*//**/
    ISO1_currency,	/*  DOS437_lfblock	221	*//**/
    ISO1_currency,	/*  DOS437_rtblock	222	*//**/
    ISO1_currency,	/*  DOS437_upblock	223	*//**/
    ISO1_currency,	/*  DOS437_alpha	224	*//**/
    ISO1_germandbls,	/*  DOS437_germandbls	225	*/
    ISO1_currency,	/*  DOS437_Gamma	226	*//**/
    ISO1_currency,	/*  DOS437_pi		227	*//**/
    ISO1_currency,	/*  DOS437_Sigma	228	*//**/
    ISO1_currency,	/*  DOS437_sigma	229	*//**/
    ISO1_mu,		/*  DOS437_mu		230	*/
    ISO1_currency,	/*  DOS437_tau		231	*//**/
    ISO1_currency,	/*  DOS437_Phi		232	*//**/
    ISO1_currency,	/*  DOS437_Theta	233	*//**/
    ISO1_currency,	/*  DOS437_Omega	234	*//**/
    ISO1_currency,	/*  DOS437_delta	235	*//**/
    ISO1_currency,	/*  DOS437_infinity	236	*//**/
    ISO1_currency,	/*  DOS437_phi		237	*//**/
    ISO1_currency,	/*  DOS437_epsilon	238	*//**/
    ISO1_currency,	/*  DOS437_intersection	239	*//**/
    ISO1_currency,	/*  DOS437_equivalence	240	*//**/
    ISO1_plusminus,	/*  DOS437_plusminus	241	*/
    ISO1_currency,	/*  DOS437_greaterequal	242	*//**/
    ISO1_currency,	/*  DOS437_lessequal	243	*//**/
    ISO1_currency,	/*  DOS437_integraltp	244	*//**/
    ISO1_currency,	/*  DOS437_integralbt	245	*//**/
    ISO1_divide,	/*  DOS437_divide	246	*/
    ISO1_currency,	/*  DOS437_approxequal	247	*/
    ISO1_degree,	/*  DOS437_degree	248	*/
    ISO1_periodcentered,/*  DOS437_bullet	249	*//**/
    ISO1_periodcentered,/*  DOS437_periodcentered 250	*/
    ISO1_currency,	/*  DOS437_radical	251	*//**/
    ISO1_currency,	/*  DOS437_nsuperior	252	*//**/
    ISO1_twosuperior,	/*  DOS437_twosuperior	253	*/
    ISO1_currency,	/*  DOS437_filledbox	254	*//**/
    ISO1_nobreakspace,	/*  DOS437_nobreakspace	255	*/
    };

const unsigned char	docDOS850_to_ISO1[256]=
    {
    0x00,		/*  			  0	*/
    0x01,		/*  			  1	*/
    0x02,		/*  			  2	*/
    0x03,		/*  			  3	*/
    0x04,		/*  			  4	*/
    0x05,		/*  			  5	*/
    0x06,		/*  			  6	*/
    0x07,		/*  			  7	*/
    0x08,		/*  			  8	*/
    0x09,		/*  			  9	*/
    0x0a,		/*  			 10	*/
    0x0b,		/*  			 11	*/
    0x0c,		/*  			 12	*/
    0x0d,		/*  			 13	*/
    0x0e,		/*  			 14	*/
    0x0f,		/*  			 15	*/
    0x10,		/*  			 16	*/
    0x11,		/*  			 17	*/
    0x12,		/*  			 18	*/
    0x13,		/*  			 19	*/
    0x14,		/*  			 20	*/
    0x15,		/*  			 21	*/
    0x16,		/*  			 22	*/
    0x17,		/*  			 23	*/
    0x18,		/*  			 24	*/
    0x19,		/*  			 25	*/
    0x1a,		/*  			 26	*/
    0x1b,		/*  			 27	*/
    0x1c,		/*  			 28	*/
    0x1d,		/*  			 29	*/
    0x1e,		/*  			 30	*/
    0x1f,		/*  			 31	*/
    ISO1_space,		/*  DOS850_space	 32	*/
    ISO1_exclam,	/*  DOS850_exclam	 33	*/
    ISO1_quotedbl,	/*  DOS850_quotedbl	 34	*/
    ISO1_numbersign,	/*  DOS850_numbersign	 35	*/
    ISO1_dollar,	/*  DOS850_dollar	 36	*/
    ISO1_percent,	/*  DOS850_percent	 37	*/
    ISO1_ampersand,	/*  DOS850_ampersand	 38	*/
    ISO1_quoteright,	/*  DOS850_quoteright	 39	*/
    ISO1_parenleft,	/*  DOS850_parenleft	 40	*/
    ISO1_parenright,	/*  DOS850_parenright	 41	*/
    ISO1_asterisk,	/*  DOS850_asterisk	 42	*/
    ISO1_plus,		/*  DOS850_plus		 43	*/
    ISO1_comma,		/*  DOS850_comma	 44	*/
    ISO1_minus,		/*  DOS850_minus	 45	*/
    ISO1_period,	/*  DOS850_period	 46	*/
    ISO1_slash,		/*  DOS850_slash	 47	*/
    ISO1_zero,		/*  DOS850_zero		 48	*/
    ISO1_one,		/*  DOS850_one		 49	*/
    ISO1_two,		/*  DOS850_two		 50	*/
    ISO1_three,		/*  DOS850_three	 51	*/
    ISO1_four,		/*  DOS850_four		 52	*/
    ISO1_five,		/*  DOS850_five		 53	*/
    ISO1_six,		/*  DOS850_six		 54	*/
    ISO1_seven,		/*  DOS850_seven	 55	*/
    ISO1_eight,		/*  DOS850_eight	 56	*/
    ISO1_nine,		/*  DOS850_nine		 57	*/
    ISO1_colon,		/*  DOS850_colon	 58	*/
    ISO1_semicolon,	/*  DOS850_semicolon	 59	*/
    ISO1_less,		/*  DOS850_less		 60	*/
    ISO1_equal,		/*  DOS850_equal	 61	*/
    ISO1_greater,	/*  DOS850_greater	 62	*/
    ISO1_question,	/*  DOS850_question	 63	*/
    ISO1_at,		/*  DOS850_at		 64	*/
    ISO1_A,		/*  DOS850_A		 65	*/
    ISO1_B,		/*  DOS850_B		 66	*/
    ISO1_C,		/*  DOS850_C		 67	*/
    ISO1_D,		/*  DOS850_D		 68	*/
    ISO1_E,		/*  DOS850_E		 69	*/
    ISO1_F,		/*  DOS850_F		 70	*/
    ISO1_G,		/*  DOS850_G		 71	*/
    ISO1_H,		/*  DOS850_H		 72	*/
    ISO1_I,		/*  DOS850_I		 73	*/
    ISO1_J,		/*  DOS850_J		 74	*/
    ISO1_K,		/*  DOS850_K		 75	*/
    ISO1_L,		/*  DOS850_L		 76	*/
    ISO1_M,		/*  DOS850_M		 77	*/
    ISO1_N,		/*  DOS850_N		 78	*/
    ISO1_O,		/*  DOS850_O		 79	*/
    ISO1_P,		/*  DOS850_P		 80	*/
    ISO1_Q,		/*  DOS850_Q		 81	*/
    ISO1_R,		/*  DOS850_R		 82	*/
    ISO1_S,		/*  DOS850_S		 83	*/
    ISO1_T,		/*  DOS850_T		 84	*/
    ISO1_U,		/*  DOS850_U		 85	*/
    ISO1_V,		/*  DOS850_V		 86	*/
    ISO1_W,		/*  DOS850_W		 87	*/
    ISO1_X,		/*  DOS850_X		 88	*/
    ISO1_Y,		/*  DOS850_Y		 89	*/
    ISO1_Z,		/*  DOS850_Z		 90	*/
    ISO1_bracketleft,	/*  DOS850_bracketleft	 91	*/
    ISO1_backslash,	/*  DOS850_backslash	 92	*/
    ISO1_bracketright,	/*  DOS850_bracketright	 93	*/
    ISO1_circumflex,	/*  DOS850_circumflex	 94	*/
    ISO1_underscore,	/*  DOS850_underscore	 95	*/
    ISO1_quoteleft,	/*  DOS850_quoteleft	 96	*/
    ISO1_a,		/*  DOS850_a		 97	*/
    ISO1_b,		/*  DOS850_b		 98	*/
    ISO1_c,		/*  DOS850_c		 99	*/
    ISO1_d,		/*  DOS850_d		100	*/
    ISO1_e,		/*  DOS850_e		101	*/
    ISO1_f,		/*  DOS850_f		102	*/
    ISO1_g,		/*  DOS850_g		103	*/
    ISO1_h,		/*  DOS850_h		104	*/
    ISO1_i,		/*  DOS850_i		105	*/
    ISO1_j,		/*  DOS850_j		106	*/
    ISO1_k,		/*  DOS850_k		107	*/
    ISO1_l,		/*  DOS850_l		108	*/
    ISO1_m,		/*  DOS850_m		109	*/
    ISO1_n,		/*  DOS850_n		110	*/
    ISO1_o,		/*  DOS850_o		111	*/
    ISO1_p,		/*  DOS850_p		112	*/
    ISO1_q,		/*  DOS850_q		113	*/
    ISO1_r,		/*  DOS850_r		114	*/
    ISO1_s,		/*  DOS850_s		115	*/
    ISO1_t,		/*  DOS850_t		116	*/
    ISO1_u,		/*  DOS850_u		117	*/
    ISO1_v,		/*  DOS850_v		118	*/
    ISO1_w,		/*  DOS850_w		119	*/
    ISO1_x,		/*  DOS850_x		120	*/
    ISO1_y,		/*  DOS850_y		121	*/
    ISO1_z,		/*  DOS850_z		122	*/
    ISO1_braceleft,	/*  DOS850_braceleft	123	*/
    ISO1_bar,		/*  DOS850_bar		124	*/
    ISO1_braceright,	/*  DOS850_braceright	125	*/
    ISO1_tilde,		/*  DOS850_tilde	126	*/
    0x7f,		/*  0x7f		127	*/
    ISO1_Ccedilla,	/*  DOS850_Ccedilla	128	*/
    ISO1_udieresis,	/*  DOS850_udieresis	129	*/
    ISO1_eacute,	/*  DOS850_eacute	130	*/
    ISO1_acircumflex,	/*  DOS850_acircumflex	131	*/
    ISO1_adieresis,	/*  DOS850_adieresis	132	*/
    ISO1_agrave,	/*  DOS850_agrave	133	*/
    ISO1_aring,		/*  DOS850_aring	134	*/
    ISO1_ccedilla,	/*  DOS850_ccedilla	135	*/
    ISO1_ecircumflex,	/*  DOS850_ecircumflex	136	*/
    ISO1_edieresis,	/*  DOS850_edieresis	137	*/
    ISO1_egrave,	/*  DOS850_egrave	138	*/
    ISO1_idieresis,	/*  DOS850_idieresis	139	*/
    ISO1_icircumflex,	/*  DOS850_icircumflex	140	*/
    ISO1_igrave,	/*  DOS850_igrave	141	*/
    ISO1_Adieresis,	/*  DOS850_Adieresis	142	*/
    ISO1_Aring,		/*  DOS850_Aring	143	*/
    ISO1_Eacute,	/*  DOS850_Eacute	144	*/
    ISO1_ae,		/*  DOS850_ae		145	*/
    ISO1_AE,		/*  DOS850_AE		146	*/
    ISO1_ocircumflex,	/*  DOS850_ocircumflex	147	*/
    ISO1_odieresis,	/*  DOS850_odieresis	148	*/
    ISO1_ograve,	/*  DOS850_ograve	149	*/
    ISO1_ucircumflex,	/*  DOS850_ucircumflex	150	*/
    ISO1_ugrave,	/*  DOS850_ugrave	151	*/
    ISO1_ydieresis,	/*  DOS850_ydieresis	152	*/
    ISO1_Odieresis,	/*  DOS850_Odieresis	153	*/
    ISO1_Udieresis,	/*  DOS850_Udieresis	154	*/
    ISO1_oslash,	/*  DOS850_oslash	155	*/
    ISO1_sterling,	/*  DOS850_sterling	156	*/
    ISO1_Oslash,	/*  DOS850_Oslash	157	*/
    ISO1_multiply,	/*  DOS850_multiply	158	*/
    ISO1_currency,	/*  DOS850_florin	159	*//**/
    ISO1_aacute,	/*  DOS850_aacute	160	*/
    ISO1_iacute,	/*  DOS850_iacute	161	*/
    ISO1_oacute,	/*  DOS850_oacute	162	*/
    ISO1_uacute,	/*  DOS850_uacute	163	*/
    ISO1_ntilde,	/*  DOS850_ntilde	164	*/
    ISO1_Ntilde,	/*  DOS850_Ntilde	165	*/
    ISO1_ordfeminine,	/*  DOS850_ordfeminine	166	*/
    ISO1_ordmasculine,	/*  DOS850_ordmasculine	167	*/
    ISO1_questiondown,	/*  DOS850_questiondown	168	*/
    ISO1_registered,	/*  DOS850_registered	169	*/
    ISO1_logicalnot,	/*  DOS850_logicalnot	170	*/
    ISO1_onehalf,	/*  DOS850_onehalf	171	*/
    ISO1_onequarter,	/*  DOS850_onequarter	172	*/
    ISO1_exclamdown,	/*  DOS850_exclamdown	173	*/
    ISO1_guillemotleft,	/*  DOS850_guillemotleft 174	*/
    ISO1_guillemotright,/*  DOS850_guillemotright 175	*/
    ISO1_currency,	/*  DOS850_ltshade	176	*//**/
    ISO1_currency,	/*  DOS850_shade	177	*//**/
    ISO1_currency,	/*  DOS850_dkshade	178	*//**/
    ISO1_currency,	/*  DOS850_SF110000	179	*//**/
    ISO1_currency,	/*  DOS850_SF090000	180	*//**/
    ISO1_Aacute,	/*  DOS850_Aacute	181	*/
    ISO1_Acircumflex,	/*  DOS850_Acircumflex	182	*/
    ISO1_Agrave,	/*  DOS850_Agrave	183	*/
    ISO1_copyright,	/*  DOS850_copyright	184	*/
    ISO1_currency,	/*  DOS850_SF230000	185	*//**/
    ISO1_currency,	/*  DOS850_SF240000	186	*//**/
    ISO1_currency,	/*  DOS850_SF250000	187	*//**/
    ISO1_currency,	/*  DOS850_SF260000	188	*//**/
    ISO1_cent,		/*  DOS850_cent		189	*/
    ISO1_yen,		/*  DOS850_yen		190	*/
    ISO1_currency,	/*  DOS850_SF030000	191	*//**/
    ISO1_currency,	/*  DOS850_SF020000	192	*//**/
    ISO1_currency,	/*  DOS850_SF070000	193	*//**/
    ISO1_currency,	/*  DOS850_SF060000	194	*//**/
    ISO1_currency,	/*  DOS850_SF080000	195	*//**/
    ISO1_currency,	/*  DOS850_SF100000	196	*//**/
    ISO1_currency,	/*  DOS850_SF050000	197	*//**/
    ISO1_atilde,	/*  DOS850_atilde	198	*/
    ISO1_Atilde,	/*  DOS850_Atilde	199	*/
    ISO1_currency,	/*  DOS850_SF380000	200	*//**/
    ISO1_currency,	/*  DOS850_SF390000	201	*//**/
    ISO1_currency,	/*  DOS850_SF400000	202	*//**/
    ISO1_currency,	/*  DOS850_SF410000	203	*//**/
    ISO1_currency,	/*  DOS850_SF420000	204	*//**/
    ISO1_currency,	/*  DOS850_SF430000	205	*//**/
    ISO1_currency,	/*  DOS850_SF440000	206	*//**/
    ISO1_currency,	/*  DOS850_currency	207	*/
    ISO1_eth,		/*  DOS850_eth		208	*/
    ISO1_Eth,		/*  DOS850_Eth		209	*/
    ISO1_Ecircumflex,	/*  DOS850_Ecircumflex	210	*/
    ISO1_Edieresis,	/*  DOS850_Edieresis	211	*/
    ISO1_Egrave,	/*  DOS850_Egrave	212	*/
    ISO1_dotlessi,	/*  DOS850_dotlessi	213	*/
    ISO1_Iacute,	/*  DOS850_Iacute	214	*/
    ISO1_Icircumflex,	/*  DOS850_Icircumflex	215	*/
    ISO1_Idieresis,	/*  DOS850_Idieresis	216	*/
    ISO1_currency,	/*  DOS850_SF040000	217	*//**/
    ISO1_currency,	/*  DOS850_SF010000	218	*//**/
    ISO1_currency,	/*  DOS850_block	219	*//**/
    ISO1_currency,	/*  DOS850_dnblock	220	*/
    ISO1_brokenbar,	/*  DOS850_brokenbar	221	*/
    ISO1_Igrave,	/*  DOS850_Igrave	222	*/
    ISO1_currency,	/*  DOS850_upblock	223	*//**/
    ISO1_Oacute,	/*  DOS850_Oacute	224	*/
    ISO1_germandbls,	/*  DOS850_germandbls	225	*/
    ISO1_Ocircumflex,	/*  DOS850_Ocircumflex	226	*/
    ISO1_Ograve,	/*  DOS850_Ograve	227	*/
    ISO1_otilde,	/*  DOS850_otilde	228	*/
    ISO1_Otilde,	/*  DOS850_Otilde	229	*/
    ISO1_mu,		/*  DOS850_mu		230	*/
    ISO1_thorn,		/*  DOS850_thorn	231	*/
    ISO1_Thorn,		/*  DOS850_Thorn	232	*/
    ISO1_Uacute,	/*  DOS850_Uacute	233	*/
    ISO1_Ucircumflex,	/*  DOS850_Ucircumflex	234	*/
    ISO1_Ugrave,	/*  DOS850_Ugrave	235	*/
    ISO1_yacute,	/*  DOS850_yacute	236	*/
    ISO1_Yacute,	/*  DOS850_Yacute	237	*/
    ISO1_macron,	/*  DOS850_macron	238	*/
    ISO1_acute,		/*  DOS850_acute	239	*/
    ISO1_hyphen,	/*  DOS850_hyphen	240	*/
    ISO1_plusminus,	/*  DOS850_plusminus	241	*/
    ISO1_underscore,	/*  DOS850_underscoredbl 242	*//**/
    ISO1_threequarters,	/*  DOS850_threequarters 243	*/
    ISO1_paragraph,	/*  DOS850_paragraph	244	*/
    ISO1_section,	/*  DOS850_section	245	*/
    ISO1_divide,	/*  DOS850_divide	246	*/
    ISO1_cedilla,	/*  DOS850_cedilla	247	*/
    ISO1_degree,	/*  DOS850_degree	248	*/
    ISO1_dieresis,	/*  DOS850_dieresis	249	*/
    ISO1_periodcentered,/*  DOS850_periodcentered 250	*/
    ISO1_onesuperior,	/*  DOS850_onesuperior	251	*/
    ISO1_threesuperior,	/*  DOS850_threesuperior 252	*/
    ISO1_twosuperior,	/*  DOS850_twosuperior	253	*/
    ISO1_currency,	/*  DOS850_filledbox	254	*//**/
    ISO1_nobreakspace,	/*  DOS850_nobreakspace	255	*/
    };

const unsigned char	docMAC_to_ISO1[256]=
    {
    0x00,		/*  			  0	*/
    0x01,		/*  			  1	*/
    0x02,		/*  			  2	*/
    0x03,		/*  			  3	*/
    0x04,		/*  			  4	*/
    0x05,		/*  			  5	*/
    0x06,		/*  			  6	*/
    0x07,		/*  			  7	*/
    0x08,		/*  			  8	*/
    0x09,		/*  			  9	*/
    0x0a,		/*  			 10	*/
    0x0b,		/*  			 11	*/
    0x0c,		/*  			 12	*/
    0x0d,		/*  			 13	*/
    0x0e,		/*  			 14	*/
    0x0f,		/*  			 15	*/
    0x10,		/*  			 16	*/
    0x11,		/*  			 17	*/
    0x12,		/*  			 18	*/
    0x13,		/*  			 19	*/
    0x14,		/*  			 20	*/
    0x15,		/*  			 21	*/
    0x16,		/*  			 22	*/
    0x17,		/*  			 23	*/
    0x18,		/*  			 24	*/
    0x19,		/*  			 25	*/
    0x1a,		/*  			 26	*/
    0x1b,		/*  			 27	*/
    0x1c,		/*  			 28	*/
    0x1d,		/*  			 29	*/
    0x1e,		/*  			 30	*/
    0x1f,		/*  			 31	*/
    ISO1_space,		/*  MAC_space		 32	*/
    ISO1_exclam,	/*  MAC_exclam		 33	*/
    ISO1_quotedbl,	/*  MAC_quotedbl	 34	*/
    ISO1_numbersign,	/*  MAC_numbersign	 35	*/
    ISO1_dollar,	/*  MAC_dollar		 36	*/
    ISO1_percent,	/*  MAC_percent		 37	*/
    ISO1_ampersand,	/*  MAC_ampersand	 38	*/
    ISO1_quoteright,	/*  MAC_quoteright	 39	*/
    ISO1_parenleft,	/*  MAC_parenleft	 40	*/
    ISO1_parenright,	/*  MAC_parenright	 41	*/
    ISO1_asterisk,	/*  MAC_asterisk	 42	*/
    ISO1_plus,		/*  MAC_plus		 43	*/
    ISO1_comma,		/*  MAC_comma		 44	*/
    ISO1_minus,		/*  MAC_minus		 45	*/
    ISO1_period,	/*  MAC_period		 46	*/
    ISO1_slash,		/*  MAC_slash		 47	*/
    ISO1_zero,		/*  MAC_zero		 48	*/
    ISO1_one,		/*  MAC_one		 49	*/
    ISO1_two,		/*  MAC_two		 50	*/
    ISO1_three,		/*  MAC_three		 51	*/
    ISO1_four,		/*  MAC_four		 52	*/
    ISO1_five,		/*  MAC_five		 53	*/
    ISO1_six,		/*  MAC_six		 54	*/
    ISO1_seven,		/*  MAC_seven		 55	*/
    ISO1_eight,		/*  MAC_eight		 56	*/
    ISO1_nine,		/*  MAC_nine		 57	*/
    ISO1_colon,		/*  MAC_colon		 58	*/
    ISO1_semicolon,	/*  MAC_semicolon	 59	*/
    ISO1_less,		/*  MAC_less		 60	*/
    ISO1_equal,		/*  MAC_equal		 61	*/
    ISO1_greater,	/*  MAC_greater		 62	*/
    ISO1_question,	/*  MAC_question	 63	*/
    ISO1_at,		/*  MAC_at		 64	*/
    ISO1_A,		/*  MAC_A		 65	*/
    ISO1_B,		/*  MAC_B		 66	*/
    ISO1_C,		/*  MAC_C		 67	*/
    ISO1_D,		/*  MAC_D		 68	*/
    ISO1_E,		/*  MAC_E		 69	*/
    ISO1_F,		/*  MAC_F		 70	*/
    ISO1_G,		/*  MAC_G		 71	*/
    ISO1_H,		/*  MAC_H		 72	*/
    ISO1_I,		/*  MAC_I		 73	*/
    ISO1_J,		/*  MAC_J		 74	*/
    ISO1_K,		/*  MAC_K		 75	*/
    ISO1_L,		/*  MAC_L		 76	*/
    ISO1_M,		/*  MAC_M		 77	*/
    ISO1_N,		/*  MAC_N		 78	*/
    ISO1_O,		/*  MAC_O		 79	*/
    ISO1_P,		/*  MAC_P		 80	*/
    ISO1_Q,		/*  MAC_Q		 81	*/
    ISO1_R,		/*  MAC_R		 82	*/
    ISO1_S,		/*  MAC_S		 83	*/
    ISO1_T,		/*  MAC_T		 84	*/
    ISO1_U,		/*  MAC_U		 85	*/
    ISO1_V,		/*  MAC_V		 86	*/
    ISO1_W,		/*  MAC_W		 87	*/
    ISO1_X,		/*  MAC_X		 88	*/
    ISO1_Y,		/*  MAC_Y		 89	*/
    ISO1_Z,		/*  MAC_Z		 90	*/
    ISO1_bracketleft,	/*  MAC_bracketleft	 91	*/
    ISO1_backslash,	/*  MAC_backslash	 92	*/
    ISO1_bracketright,	/*  MAC_bracketright	 93	*/
    ISO1_circumflex,	/*  MAC_circumflex	 94	*/
    ISO1_underscore,	/*  MAC_underscore	 95	*/
    ISO1_grave,		/*  MAC_grave		 96	*/
    ISO1_a,		/*  MAC_a		 97	*/
    ISO1_b,		/*  MAC_b		 98	*/
    ISO1_c,		/*  MAC_c		 99	*/
    ISO1_d,		/*  MAC_d		100	*/
    ISO1_e,		/*  MAC_e		101	*/
    ISO1_f,		/*  MAC_f		102	*/
    ISO1_g,		/*  MAC_g		103	*/
    ISO1_h,		/*  MAC_h		104	*/
    ISO1_i,		/*  MAC_i		105	*/
    ISO1_j,		/*  MAC_j		106	*/
    ISO1_k,		/*  MAC_k		107	*/
    ISO1_l,		/*  MAC_l		108	*/
    ISO1_m,		/*  MAC_m		109	*/
    ISO1_n,		/*  MAC_n		110	*/
    ISO1_o,		/*  MAC_o		111	*/
    ISO1_p,		/*  MAC_p		112	*/
    ISO1_q,		/*  MAC_q		113	*/
    ISO1_r,		/*  MAC_r		114	*/
    ISO1_s,		/*  MAC_s		115	*/
    ISO1_t,		/*  MAC_t		116	*/
    ISO1_u,		/*  MAC_u		117	*/
    ISO1_v,		/*  MAC_v		118	*/
    ISO1_w,		/*  MAC_w		119	*/
    ISO1_x,		/*  MAC_x		120	*/
    ISO1_y,		/*  MAC_y		121	*/
    ISO1_z,		/*  MAC_z		122	*/
    ISO1_braceleft,	/*  MAC_braceleft	123	*/
    ISO1_bar,		/*  MAC_bar		124	*/
    ISO1_braceright,	/*  MAC_braceright	125	*/
    ISO1_tilde,		/*  MAC_tilde		126	*/
    0x7f,		/*  0x7f		127	*/
    ISO1_Adieresis,	/*  MAC_Adieresis	128	*/
    ISO1_Aring,		/*  MAC_Aring		129	*/
    ISO1_Ccedilla,	/*  MAC_Ccedilla	130	*/
    ISO1_Eacute,	/*  MAC_Eacute		131	*/
    ISO1_Ntilde,	/*  MAC_Ntilde		132	*/
    ISO1_Odieresis,	/*  MAC_Odieresis	133	*/
    ISO1_Udieresis,	/*  MAC_Udieresis	134	*/
    ISO1_aacute,	/*  MAC_aacute		135	*/
    ISO1_agrave,	/*  MAC_agrave		136	*/
    ISO1_acircumflex,	/*  MAC_acircumflex	137	*/
    ISO1_adieresis,	/*  MAC_adieresis	138	*/
    ISO1_atilde,	/*  MAC_atilde		139	*/
    ISO1_aring,		/*  MAC_aring		140	*/
    ISO1_ccedilla,	/*  MAC_ccedilla	141	*/
    ISO1_eacute,	/*  MAC_eacute		142	*/
    ISO1_egrave,	/*  MAC_egrave		143	*/
    ISO1_ecircumflex,	/*  MAC_ecircumflex	144	*/
    ISO1_edieresis,	/*  MAC_edieresis	145	*/
    ISO1_iacute,	/*  MAC_iacute		146	*/
    ISO1_igrave,	/*  MAC_igrave		147	*/
    ISO1_icircumflex,	/*  MAC_icircumflex	148	*/
    ISO1_idieresis,	/*  MAC_idieresis	149	*/
    ISO1_ntilde,	/*  MAC_ntilde		150	*/
    ISO1_oacute,	/*  MAC_oacute		151	*/
    ISO1_ograve,	/*  MAC_ograve		152	*/
    ISO1_ocircumflex,	/*  MAC_ocircumflex	153	*/
    ISO1_odieresis,	/*  MAC_odieresis	154	*/
    ISO1_otilde,	/*  MAC_otilde		155	*/
    ISO1_uacute,	/*  MAC_uacute		156	*/
    ISO1_ugrave,	/*  MAC_ugrave		157	*/
    ISO1_ucircumflex,	/*  MAC_ucircumflex	158	*/
    ISO1_udieresis,	/*  MAC_udieresis	159	*/
    ISO1_currency,	/*  MAC_dagger		160	*//**/
    ISO1_degree,	/*  MAC_degree		161	*/
    ISO1_cent,		/*  MAC_cent		162	*/
    ISO1_sterling,	/*  MAC_sterling	163	*/
    ISO1_section,	/*  MAC_section		164	*/
    ISO1_periodcentered,/*  MAC_bullet		165	*//**/
    ISO1_paragraph,	/*  MAC_paragraph	166	*/
    ISO1_germandbls,	/*  MAC_germandbls	167	*/
    ISO1_registered,	/*  MAC_registered	168	*/
    ISO1_copyright,	/*  MAC_copyright	169	*/
    ISO1_currency,	/*  MAC_trademark	170	*//**/
    ISO1_acute,		/*  MAC_acute		171	*/
    ISO1_dieresis,	/*  MAC_dieresis	172	*/
    ISO1_currency,	/*  MAC_notequal	173	*//**/
    ISO1_AE,		/*  MAC_AE		174	*/
    ISO1_Oslash,	/*  MAC_Oslash		175	*/
    ISO1_currency,	/*  MAC_infinity	176	*//**/
    ISO1_plusminus,	/*  MAC_plusminus	177	*/
    ISO1_currency,	/*  MAC_lessequal	178	*//**/
    ISO1_currency,	/*  MAC_greaterequal	179	*//**/
    ISO1_yen,		/*  MAC_yen		180	*/
    ISO1_mu,		/*  MAC_mu		181	*/
    ISO1_currency,	/*  MAC_partialdiff	182	*//**/
    ISO1_currency,	/*  MAC_summation	183	*//**/
    ISO1_currency,	/*  MAC_product		184	*//**/
    ISO1_currency,	/*  MAC_pi		185	*//**/
    ISO1_currency,	/*  MAC_integral	186	*//**/
    ISO1_ordfeminine,	/*  MAC_ordfeminine	187	*/
    ISO1_ordmasculine,	/*  MAC_ordmasculine	188	*/
    ISO1_currency,	/*  MAC_Omega		189	*//**/
    ISO1_ae,		/*  MAC_ae		190	*/
    ISO1_oslash,	/*  MAC_oslash		191	*/
    ISO1_questiondown,	/*  MAC_questiondown	192	*/
    ISO1_exclamdown,	/*  MAC_exclamdown	193	*/
    ISO1_logicalnot,	/*  MAC_logicalnot	194	*/
    ISO1_currency,	/*  MAC_radical		195	*//**/
    ISO1_currency,	/*  MAC_florin		196	*//**/
    ISO1_currency,	/*  MAC_approxequal	197	*//**/
    ISO1_currency,	/*  MAC_Delta		198	*//**/
    ISO1_guillemotleft,	/*  MAC_guillemotleft	199	*/
    ISO1_guillemotright,/*  MAC_guillemotright	200	*/
    ISO1_currency,	/*  MAC_ellipsis	201	*//**/
    ISO1_nobreakspace,	/*  MAC_nobreakspace	202	*/
    ISO1_Agrave,	/*  MAC_Agrave		203	*/
    ISO1_Atilde,	/*  MAC_Atilde		204	*/
    ISO1_Otilde,	/*  MAC_Otilde		205	*/
    ISO1_currency,	/*  MAC_OE		206	*//**/
    ISO1_currency,	/*  MAC_oe		207	*//**/
    ISO1_minus,		/*  MAC_endash		208	*//**/
    ISO1_minus,		/*  MAC_emdash		209	*//**/
    ISO1_quotedbl,	/*  MAC_quotedblleft	210	*//**/
    ISO1_quotedbl,	/*  MAC_quotedblright	211	*//**/
    ISO1_quoteleft,	/*  MAC_quoteleft	212	*/
    ISO1_quoteright,	/*  MAC_quoteright	213	*/
    ISO1_divide,	/*  MAC_divide		214	*/
    ISO1_currency,	/*  MAC_lozenge		215	*//**/
    ISO1_ydieresis,	/*  MAC_ydieresis	216	*/
    ISO1_ydieresis,	/*  MAC_Ydieresis	217	*//**/
    ISO1_currency,	/*  MAC_fraction	218	*//**/
    ISO1_currency,	/*  MAC_currency	219	*/
    ISO1_guillemotleft,	/*  MAC_guilsinglleft	220	*//**/
    ISO1_guillemotright,/*  MAC_guilsinglright	221	*//**/
    ISO1_currency,	/*  MAC_fi		222	*//**/
    ISO1_currency,	/*  MAC_fl		223	*//**/
    ISO1_currency,	/*  MAC_daggerdbl	224	*//**/
    ISO1_periodcentered,/*  MAC_periodcentered	225	*/
    ISO1_quoteleft,	/*  MAC_quotesinglbase	226	*//**/
    ISO1_quotedbl,	/*  MAC_quotedblbase	227	*//**/
    ISO1_currency,	/*  MAC_perthousand	228	*//**/
    ISO1_Acircumflex,	/*  MAC_Acircumflex	229	*/
    ISO1_Ecircumflex,	/*  MAC_Ecircumflex	230	*/
    ISO1_Aacute,	/*  MAC_Aacute		231	*/
    ISO1_Edieresis,	/*  MAC_Edieresis	232	*/
    ISO1_Egrave,	/*  MAC_Egrave		233	*/
    ISO1_Iacute,	/*  MAC_Iacute		234	*/
    ISO1_Icircumflex,	/*  MAC_Icircumflex	235	*/
    ISO1_Idieresis,	/*  MAC_Idieresis	236	*/
    ISO1_Igrave,	/*  MAC_Igrave		237	*/
    ISO1_Oacute,	/*  MAC_Oacute		238	*/
    ISO1_Ocircumflex,	/*  MAC_Ocircumflex	239	*/
    ISO1_currency,	/*  MAC__apple_logo	240	*//**/
    ISO1_Ograve,	/*  MAC_Ograve		241	*/
    ISO1_Uacute,	/*  MAC_Uacute		242	*/
    ISO1_Ucircumflex,	/*  MAC_Ucircumflex	243	*/
    ISO1_Ugrave,	/*  MAC_Ugrave		244	*/
    ISO1_dotlessi,	/*  MAC_dotlessi	245	*/
    ISO1_circumflex,	/*  MAC_circumflex	246	*/
    ISO1_tilde,		/*  MAC_tilde		247	*/
    ISO1_macron,	/*  MAC_macron		248	*/
    ISO1_breve,		/*  MAC_breve		249	*/
    ISO1_dotaccent,	/*  MAC_dotaccent	250	*/
    ISO1_ring,		/*  MAC_ring		251	*/
    ISO1_cedilla,	/*  MAC_cedilla		252	*/
    ISO1_hungarumlaut,	/*  MAC_hungarumlaut	253	*/
    ISO1_ogonek,	/*  MAC_ogonek		254	*/
    ISO1_caron,		/*  MAC_caron		255	*/
    };

const unsigned char docKOI8R_to_WIN1251[256]=
    {
    0x00,			/*  			  0	*/
    0x01,			/*  			  1	*/
    0x02,			/*  			  2	*/
    0x03,			/*  			  3	*/
    0x04,			/*  			  4	*/
    0x05,			/*  			  5	*/
    0x06,			/*  			  6	*/
    0x07,			/*  			  7	*/
    0x08,			/*  			  8	*/
    0x09,			/*  			  9	*/
    0x0A,			/*  			 10	*/
    0x0B,			/*  			 11	*/
    0x0C,			/*  			 12	*/
    0x0D,			/*  			 13	*/
    0x0E,			/*  			 14	*/
    0x0F,			/*  			 15	*/
    0x10,			/*  			 16	*/
    0x11,			/*  			 17	*/
    0x12,			/*  			 18	*/
    0x13,			/*  			 19	*/
    0x14,			/*  			 20	*/
    0x15,			/*  			 21	*/
    0x16,			/*  			 22	*/
    0x17,			/*  			 23	*/
    0x18,			/*  			 24	*/
    0x19,			/*  			 25	*/
    0x1A,			/*  			 26	*/
    0x1B,			/*  			 27	*/
    0x1C,			/*  			 28	*/
    0x1D,			/*  			 29	*/
    0x1E,			/*  			 30	*/
    0x1F,			/*  			 31	*/
    WIN1251_space,		/*  KOI8R_space		 32	*/
    WIN1251_exclam,		/*  KOI8R_exclam	 33	*/
    WIN1251_quotedbl,		/*  KOI8R_quotedbl	 34	*/
    WIN1251_numbersign,		/*  KOI8R_numbersign	 35	*/
    WIN1251_dollar,		/*  KOI8R_dollar	 36	*/
    WIN1251_percent,		/*  KOI8R_percent	 37	*/
    WIN1251_ampersand,		/*  KOI8R_ampersand	 38	*/
    WIN1251_quotesingle,	/*  KOI8R_quotesingle	 39	*/
    WIN1251_parenleft,		/*  KOI8R_parenleft	 40	*/
    WIN1251_parenright,		/*  KOI8R_parenright	 41	*/
    WIN1251_asterisk,		/*  KOI8R_asterisk	 42	*/
    WIN1251_plus,		/*  KOI8R_plus		 43	*/
    WIN1251_comma,		/*  KOI8R_comma		 44	*/
    WIN1251_minus,		/*  KOI8R_minus		 45	*/
    WIN1251_period,		/*  KOI8R_period	 46	*/
    WIN1251_slash,		/*  KOI8R_slash		 47	*/
    WIN1251_zero,		/*  KOI8R_zero		 48	*/
    WIN1251_one,		/*  KOI8R_one		 49	*/
    WIN1251_two,		/*  KOI8R_two		 50	*/
    WIN1251_three,		/*  KOI8R_three		 51	*/
    WIN1251_four,		/*  KOI8R_four		 52	*/
    WIN1251_five,		/*  KOI8R_five		 53	*/
    WIN1251_six,		/*  KOI8R_six		 54	*/
    WIN1251_seven,		/*  KOI8R_seven		 55	*/
    WIN1251_eight,		/*  KOI8R_eight		 56	*/
    WIN1251_nine,		/*  KOI8R_nine		 57	*/
    WIN1251_colon,		/*  KOI8R_colon		 58	*/
    WIN1251_semicolon,		/*  KOI8R_semicolon	 59	*/
    WIN1251_less,		/*  KOI8R_less		 60	*/
    WIN1251_equal,		/*  KOI8R_equal		 61	*/
    WIN1251_greater,		/*  KOI8R_greater	 62	*/
    WIN1251_question,		/*  KOI8R_question	 63	*/
    WIN1251_at,			/*  KOI8R_at		 64	*/
    WIN1251_A,			/*  KOI8R_A		 65	*/
    WIN1251_B,			/*  KOI8R_B		 66	*/
    WIN1251_C,			/*  KOI8R_C		 67	*/
    WIN1251_D,			/*  KOI8R_D		 68	*/
    WIN1251_E,			/*  KOI8R_E		 69	*/
    WIN1251_F,			/*  KOI8R_F		 70	*/
    WIN1251_G,			/*  KOI8R_G		 71	*/
    WIN1251_H,			/*  KOI8R_H		 72	*/
    WIN1251_I,			/*  KOI8R_I		 73	*/
    WIN1251_J,			/*  KOI8R_J		 74	*/
    WIN1251_K,			/*  KOI8R_K		 75	*/
    WIN1251_L,			/*  KOI8R_L		 76	*/
    WIN1251_M,			/*  KOI8R_M		 77	*/
    WIN1251_N,			/*  KOI8R_N		 78	*/
    WIN1251_O,			/*  KOI8R_O		 79	*/
    WIN1251_P,			/*  KOI8R_P		 80	*/
    WIN1251_Q,			/*  KOI8R_Q		 81	*/
    WIN1251_R,			/*  KOI8R_R		 82	*/
    WIN1251_S,			/*  KOI8R_S		 83	*/
    WIN1251_T,			/*  KOI8R_T		 84	*/
    WIN1251_U,			/*  KOI8R_U		 85	*/
    WIN1251_V,			/*  KOI8R_V		 86	*/
    WIN1251_W,			/*  KOI8R_W		 87	*/
    WIN1251_X,			/*  KOI8R_X		 88	*/
    WIN1251_Y,			/*  KOI8R_Y		 89	*/
    WIN1251_Z,			/*  KOI8R_Z		 90	*/
    WIN1251_bracketleft,	/*  KOI8R_bracketleft	 91	*/
    WIN1251_backslash,		/*  KOI8R_backslash	 92	*/
    WIN1251_bracketright,	/*  KOI8R_bracketright	 93	*/
    WIN1251_asciicircum,	/*  KOI8R_asciicircum	 94	*/
    WIN1251_underscore,		/*  KOI8R_underscore	 95	*/
    WIN1251_quoteleft,		/*  KOI8R_quoteleft	 96	*/
    WIN1251_a,			/*  KOI8R_a		 97	*/
    WIN1251_b,			/*  KOI8R_b		 98	*/
    WIN1251_c,			/*  KOI8R_c		 99	*/
    WIN1251_d,			/*  KOI8R_d		100	*/
    WIN1251_e,			/*  KOI8R_e		101	*/
    WIN1251_f,			/*  KOI8R_f		102	*/
    WIN1251_g,			/*  KOI8R_g		103	*/
    WIN1251_h,			/*  KOI8R_h		104	*/
    WIN1251_i,			/*  KOI8R_i		105	*/
    WIN1251_j,			/*  KOI8R_j		106	*/
    WIN1251_k,			/*  KOI8R_k		107	*/
    WIN1251_l,			/*  KOI8R_l		108	*/
    WIN1251_m,			/*  KOI8R_m		109	*/
    WIN1251_n,			/*  KOI8R_n		110	*/
    WIN1251_o,			/*  KOI8R_o		111	*/
    WIN1251_p,			/*  KOI8R_p		112	*/
    WIN1251_q,			/*  KOI8R_q		113	*/
    WIN1251_r,			/*  KOI8R_r		114	*/
    WIN1251_s,			/*  KOI8R_s		115	*/
    WIN1251_t,			/*  KOI8R_t		116	*/
    WIN1251_u,			/*  KOI8R_u		117	*/
    WIN1251_v,			/*  KOI8R_v		118	*/
    WIN1251_w,			/*  KOI8R_w		119	*/
    WIN1251_x,			/*  KOI8R_x		120	*/
    WIN1251_y,			/*  KOI8R_y		121	*/
    WIN1251_z,			/*  KOI8R_z		122	*/
    WIN1251_braceleft,		/*  KOI8R_braceleft	123	*/
    WIN1251_bar,		/*  KOI8R_bar		124	*/
    WIN1251_braceright,		/*  KOI8R_braceright	125	*/
    WIN1251_asciitilde,		/*  KOI8R_asciitilde	126	*/
    WIN1251__0x7F,		/*  KOI8R__0x7F		127	*/
    WIN1251_currency,		/*  KOI8R_SF100000	128	*//**/
    WIN1251_currency,		/*  KOI8R_SF110000	129	*//**/
    WIN1251_currency,		/*  KOI8R_SF010000	130	*//**/
    WIN1251_currency,		/*  KOI8R_SF030000	131	*//**/
    WIN1251_currency,		/*  KOI8R_SF020000	132	*//**/
    WIN1251_currency,		/*  KOI8R_SF040000	133	*//**/
    WIN1251_currency,		/*  KOI8R_SF080000	134	*//**/
    WIN1251_currency,		/*  KOI8R_SF090000	135	*//**/
    WIN1251_currency,		/*  KOI8R_SF060000	136	*//**/
    WIN1251_currency,		/*  KOI8R_SF070000	137	*//**/
    WIN1251_currency,		/*  KOI8R_SF050000	138	*//**/
    WIN1251_currency,		/*  KOI8R_upblock	139	*//**/
    WIN1251_currency,		/*  KOI8R_dnblock	140	*//**/
    WIN1251_currency,		/*  KOI8R_block		141	*//**/
    WIN1251_currency,		/*  KOI8R_lfblock	142	*//**/
    WIN1251_currency,		/*  KOI8R_rtblock	143	*//**/
    WIN1251_currency,		/*  KOI8R_ltshade	144	*//**/
    WIN1251_currency,		/*  KOI8R_shade		145	*//**/
    WIN1251_currency,		/*  KOI8R_dkshade	146	*//**/
    WIN1251_currency,		/*  KOI8R_integraltp	147	*//**/
    WIN1251_currency,		/*  KOI8R_filledbox	148	*//**/
    WIN1251_bullet,		/*  KOI8R_bullet 	149	*/
    WIN1251_currency,		/*  KOI8R_radical	150	*//**/
    WIN1251_currency,		/*  KOI8R_approxequal	151	*//**/
    WIN1251_currency,		/*  KOI8R_lessequal	152	*//**/
    WIN1251_currency,		/*  KOI8R_greaterequal	153	*//**/
    WIN1251_nobreakspace,	/*  KOI8R_nobreakspace	154	*/
    WIN1251_currency,		/*  KOI8R_integralbt	155	*//**/
    WIN1251_degree,		/*  KOI8R_degree	156	*/
    WIN1251_currency,		/*  KOI8R_twosuperior	157	*//**/
    WIN1251_periodcentered,	/*  KOI8R_periodcentered 158	*/
    WIN1251_currency,		/*  KOI8R_divide	159	*//**/
    WIN1251_currency,		/*  KOI8R_SF430000	160	*//**/
    WIN1251_currency,		/*  KOI8R_SF240000	161	*//**/
    WIN1251_currency,		/*  KOI8R_SF510000	162	*//**/
    WIN1251_afii10071,		/*  KOI8R_afii10071	163	*/
    WIN1251_currency,		/*  KOI8R_SF520000	164	*//**/
    WIN1251_currency,		/*  KOI8R_SF390000	165	*//**/
    WIN1251_currency,		/*  KOI8R_SF220000	166	*//**/
    WIN1251_currency,		/*  KOI8R_SF210000	167	*//**/
    WIN1251_currency,		/*  KOI8R_SF250000	168	*//**/
    WIN1251_currency,		/*  KOI8R_SF500000	169	*//**/
    WIN1251_currency,		/*  KOI8R_SF490000	170	*//**/
    WIN1251_currency,		/*  KOI8R_SF380000	171	*//**/
    WIN1251_currency,		/*  KOI8R_SF280000	172	*//**/
    WIN1251_currency,		/*  KOI8R_SF270000	173	*//**/
    WIN1251_currency,		/*  KOI8R_SF260000	174	*//**/
    WIN1251_currency,		/*  KOI8R_SF360000	175	*//**/
    WIN1251_currency,		/*  KOI8R_SF370000	176	*//**/
    WIN1251_currency,		/*  KOI8R_SF420000	177	*//**/
    WIN1251_currency,		/*  KOI8R_SF190000	178	*//**/
    WIN1251_afii10023,		/*  KOI8R_afii10023	179	*/
    WIN1251_currency,		/*  KOI8R_SF200000	180	*//**/
    WIN1251_currency,		/*  KOI8R_SF230000	181	*//**/
    WIN1251_currency,		/*  KOI8R_SF470000	182	*//**/
    WIN1251_currency,		/*  KOI8R_SF480000	183	*//**/
    WIN1251_currency,		/*  KOI8R_SF410000	184	*//**/
    WIN1251_currency,		/*  KOI8R_SF450000	185	*//**/
    WIN1251_currency,		/*  KOI8R_SF460000	186	*//**/
    WIN1251_currency,		/*  KOI8R_SF400000	187	*//**/
    WIN1251_currency,		/*  KOI8R_SF540000	188	*//**/
    WIN1251_currency,		/*  KOI8R_SF530000	189	*//**/
    WIN1251_currency,		/*  KOI8R_SF440000	190	*//**/
    WIN1251_copyright,		/*  KOI8R_copyright	191	*/
    WIN1251_afii10096,		/*  KOI8R_afii10096	192	*/
    WIN1251_afii10065,		/*  KOI8R_afii10065	193	*/
    WIN1251_afii10066,		/*  KOI8R_afii10066	194	*/
    WIN1251_afii10088,		/*  KOI8R_afii10088	195	*/
    WIN1251_afii10069,		/*  KOI8R_afii10069	196	*/
    WIN1251_afii10070,		/*  KOI8R_afii10070	197	*/
    WIN1251_afii10086,		/*  KOI8R_afii10086	198	*/
    WIN1251_afii10068,		/*  KOI8R_afii10068	199	*/
    WIN1251_afii10087,		/*  KOI8R_afii10087	200	*/
    WIN1251_afii10074,		/*  KOI8R_afii10074	201	*/
    WIN1251_afii10075,		/*  KOI8R_afii10075	202	*/
    WIN1251_afii10076,		/*  KOI8R_afii10076	203	*/
    WIN1251_afii10077,		/*  KOI8R_afii10077	204	*/
    WIN1251_afii10078,		/*  KOI8R_afii10078	205	*/
    WIN1251_afii10079,		/*  KOI8R_afii10079	206	*/
    WIN1251_afii10080,		/*  KOI8R_afii10080	207	*/
    WIN1251_afii10081,		/*  KOI8R_afii10081	208	*/
    WIN1251_afii10097,		/*  KOI8R_afii10097	209	*/
    WIN1251_afii10082,		/*  KOI8R_afii10082	210	*/
    WIN1251_afii10083,		/*  KOI8R_afii10083	211	*/
    WIN1251_afii10084,		/*  KOI8R_afii10084	212	*/
    WIN1251_afii10085,		/*  KOI8R_afii10085	213	*/
    WIN1251_afii10072,		/*  KOI8R_afii10072	214	*/
    WIN1251_afii10067,		/*  KOI8R_afii10067	215	*/
    WIN1251_afii10094,		/*  KOI8R_afii10094	216	*/
    WIN1251_afii10093,		/*  KOI8R_afii10093	217	*/
    WIN1251_afii10073,		/*  KOI8R_afii10073	218	*/
    WIN1251_afii10090,		/*  KOI8R_afii10090	219	*/
    WIN1251_afii10095,		/*  KOI8R_afii10095	220	*/
    WIN1251_afii10091,		/*  KOI8R_afii10091	221	*/
    WIN1251_afii10089,		/*  KOI8R_afii10089	222	*/
    WIN1251_afii10092,		/*  KOI8R_afii10092	223	*/
    WIN1251_afii10048,		/*  KOI8R_afii10048	224	*/
    WIN1251_afii10017,		/*  KOI8R_afii10017	225	*/
    WIN1251_afii10018,		/*  KOI8R_afii10018	226	*/
    WIN1251_afii10040,		/*  KOI8R_afii10040	227	*/
    WIN1251_afii10021,		/*  KOI8R_afii10021	228	*/
    WIN1251_afii10022,		/*  KOI8R_afii10022	229	*/
    WIN1251_afii10038,		/*  KOI8R_afii10038	230	*/
    WIN1251_afii10020,		/*  KOI8R_afii10020	231	*/
    WIN1251_afii10039,		/*  KOI8R_afii10039	232	*/
    WIN1251_afii10026,		/*  KOI8R_afii10026	233	*/
    WIN1251_afii10027,		/*  KOI8R_afii10027	234	*/
    WIN1251_afii10028,		/*  KOI8R_afii10028	235	*/
    WIN1251_afii10029,		/*  KOI8R_afii10029	236	*/
    WIN1251_afii10030,		/*  KOI8R_afii10030	237	*/
    WIN1251_afii10031,		/*  KOI8R_afii10031	238	*/
    WIN1251_afii10032,		/*  KOI8R_afii10032	239	*/
    WIN1251_afii10033,		/*  KOI8R_afii10033	240	*/
    WIN1251_afii10049,		/*  KOI8R_afii10049	241	*/
    WIN1251_afii10034,		/*  KOI8R_afii10034	242	*/
    WIN1251_afii10035,		/*  KOI8R_afii10035	243	*/
    WIN1251_afii10036,		/*  KOI8R_afii10036	244	*/
    WIN1251_afii10037,		/*  KOI8R_afii10037	245	*/
    WIN1251_afii10024,		/*  KOI8R_afii10024	246	*/
    WIN1251_afii10019,		/*  KOI8R_afii10019	247	*/
    WIN1251_afii10046,		/*  KOI8R_afii10046	248	*/
    WIN1251_afii10045,		/*  KOI8R_afii10045	249	*/
    WIN1251_afii10025,		/*  KOI8R_afii10025	250	*/
    WIN1251_afii10042,		/*  KOI8R_afii10042	251	*/
    WIN1251_afii10047,		/*  KOI8R_afii10047	252	*/
    WIN1251_afii10043,		/*  KOI8R_afii10043	253	*/
    WIN1251_afii10041,		/*  KOI8R_afii10041	254	*/
    WIN1251_afii10044,		/*  KOI8R_afii10044	255	*/
    };

const unsigned char docWIN1251_to_KOI8R[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    KOI8R_space,		/*  WIN1251_space		 32	*/
    KOI8R_exclam,		/*  WIN1251_exclam		 33	*/
    KOI8R_quotedbl,		/*  WIN1251_quotedbl		 34	*/
    KOI8R_numbersign,		/*  WIN1251_numbersign		 35	*/
    KOI8R_dollar,		/*  WIN1251_dollar		 36	*/
    KOI8R_percent,		/*  WIN1251_percent		 37	*/
    KOI8R_ampersand,		/*  WIN1251_ampersand		 38	*/
    KOI8R_quotesingle,		/*  WIN1251_quotesingle		 39	*/
    KOI8R_parenleft,		/*  WIN1251_parenleft		 40	*/
    KOI8R_parenright,		/*  WIN1251_parenright		 41	*/
    KOI8R_asterisk,		/*  WIN1251_asterisk		 42	*/
    KOI8R_plus,			/*  WIN1251_plus		 43	*/
    KOI8R_comma,		/*  WIN1251_comma		 44	*/
    KOI8R_minus,		/*  WIN1251_minus		 45	*/
    KOI8R_period,		/*  WIN1251_period		 46	*/
    KOI8R_slash,		/*  WIN1251_slash		 47	*/
    KOI8R_zero,			/*  WIN1251_zero		 48	*/
    KOI8R_one,			/*  WIN1251_one			 49	*/
    KOI8R_two,			/*  WIN1251_two			 50	*/
    KOI8R_three,		/*  WIN1251_three		 51	*/
    KOI8R_four,			/*  WIN1251_four		 52	*/
    KOI8R_five,			/*  WIN1251_five		 53	*/
    KOI8R_six,			/*  WIN1251_six			 54	*/
    KOI8R_seven,		/*  WIN1251_seven		 55	*/
    KOI8R_eight,		/*  WIN1251_eight		 56	*/
    KOI8R_nine,			/*  WIN1251_nine		 57	*/
    KOI8R_colon,		/*  WIN1251_colon		 58	*/
    KOI8R_semicolon,		/*  WIN1251_semicolon		 59	*/
    KOI8R_less,			/*  WIN1251_less		 60	*/
    KOI8R_equal,		/*  WIN1251_equal		 61	*/
    KOI8R_greater,		/*  WIN1251_greater		 62	*/
    KOI8R_question,		/*  WIN1251_question		 63	*/
    KOI8R_at,			/*  WIN1251_at			 64	*/
    KOI8R_A,			/*  WIN1251_A			 65	*/
    KOI8R_B,			/*  WIN1251_B			 66	*/
    KOI8R_C,			/*  WIN1251_C			 67	*/
    KOI8R_D,			/*  WIN1251_D			 68	*/
    KOI8R_E,			/*  WIN1251_E			 69	*/
    KOI8R_F,			/*  WIN1251_F			 70	*/
    KOI8R_G,			/*  WIN1251_G			 71	*/
    KOI8R_H,			/*  WIN1251_H			 72	*/
    KOI8R_I,			/*  WIN1251_I			 73	*/
    KOI8R_J,			/*  WIN1251_J			 74	*/
    KOI8R_K,			/*  WIN1251_K			 75	*/
    KOI8R_L,			/*  WIN1251_L			 76	*/
    KOI8R_M,			/*  WIN1251_M			 77	*/
    KOI8R_N,			/*  WIN1251_N			 78	*/
    KOI8R_O,			/*  WIN1251_O			 79	*/
    KOI8R_P,			/*  WIN1251_P			 80	*/
    KOI8R_Q,			/*  WIN1251_Q			 81	*/
    KOI8R_R,			/*  WIN1251_R			 82	*/
    KOI8R_S,			/*  WIN1251_S			 83	*/
    KOI8R_T,			/*  WIN1251_T			 84	*/
    KOI8R_U,			/*  WIN1251_U			 85	*/
    KOI8R_V,			/*  WIN1251_V			 86	*/
    KOI8R_W,			/*  WIN1251_W			 87	*/
    KOI8R_X,			/*  WIN1251_X			 88	*/
    KOI8R_Y,			/*  WIN1251_Y			 89	*/
    KOI8R_Z,			/*  WIN1251_Z			 90	*/
    KOI8R_bracketleft,		/*  WIN1251_bracketleft		 91	*/
    KOI8R_backslash,		/*  WIN1251_backslash		 92	*/
    KOI8R_bracketright,		/*  WIN1251_bracketright	 93	*/
    KOI8R_asciicircum,		/*  WIN1251_asciicircum		 94	*/
    KOI8R_underscore,		/*  WIN1251_underscore		 95	*/
    KOI8R_quoteleft,		/*  WIN1251_quoteleft		 96	*/
    KOI8R_a,			/*  WIN1251_a			 97	*/
    KOI8R_b,			/*  WIN1251_b			 98	*/
    KOI8R_c,			/*  WIN1251_c			 99	*/
    KOI8R_d,			/*  WIN1251_d			100	*/
    KOI8R_e,			/*  WIN1251_e			101	*/
    KOI8R_f,			/*  WIN1251_f			102	*/
    KOI8R_g,			/*  WIN1251_g			103	*/
    KOI8R_h,			/*  WIN1251_h			104	*/
    KOI8R_i,			/*  WIN1251_i			105	*/
    KOI8R_j,			/*  WIN1251_j			106	*/
    KOI8R_k,			/*  WIN1251_k			107	*/
    KOI8R_l,			/*  WIN1251_l			108	*/
    KOI8R_m,			/*  WIN1251_m			109	*/
    KOI8R_n,			/*  WIN1251_n			110	*/
    KOI8R_o,			/*  WIN1251_o			111	*/
    KOI8R_p,			/*  WIN1251_p			112	*/
    KOI8R_q,			/*  WIN1251_q			113	*/
    KOI8R_r,			/*  WIN1251_r			114	*/
    KOI8R_s,			/*  WIN1251_s			115	*/
    KOI8R_t,			/*  WIN1251_t			116	*/
    KOI8R_u,			/*  WIN1251_u			117	*/
    KOI8R_v,			/*  WIN1251_v			118	*/
    KOI8R_w,			/*  WIN1251_w			119	*/
    KOI8R_x,			/*  WIN1251_x			120	*/
    KOI8R_y,			/*  WIN1251_y			121	*/
    KOI8R_z,			/*  WIN1251_z			122	*/
    KOI8R_braceleft,		/*  WIN1251_braceleft		123	*/
    KOI8R_bar,			/*  WIN1251_bar			124	*/
    KOI8R_braceright,		/*  WIN1251_braceright		125	*/
    KOI8R_asciitilde,		/*  WIN1251_asciitilde		126	*/
    KOI8R__0x7F,		/*  WIN1251__0x7F		127	*/
    KOI8R_copyright,		/*  WIN1251_afii10051		128	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10052		129	*//**/
    KOI8R_quoteleft,		/*  WIN1251_quotesinglbase	130	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10100		131	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblbase	132	*//**/
    KOI8R_copyright,		/*  WIN1251_ellipsis		133	*//**/
    KOI8R_copyright,		/*  WIN1251_dagger		134	*//**/
    KOI8R_copyright,		/*  WIN1251_daggerdbl		135	*//**/
    KOI8R_copyright,		/*  WIN1251__0x88		136	*//**/
    KOI8R_copyright,		/*  WIN1251_perthousand		137	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10058		138	*//**/
    KOI8R_copyright,		/*  WIN1251_guilsinglleft	139	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10059		140	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10061		141	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10060		142	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10145		143	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10099		144	*//**/
    KOI8R_quotesingle,		/*  WIN1251_quoteleft		145	*//**/
    KOI8R_quotesingle,		/*  WIN1251_quoteright		146	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblleft	147	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblright	148	*//**/
    KOI8R_bullet,		/*  WIN1251_bullet		149	*/
    KOI8R_minus,		/*  WIN1251_endash		150	*//**/
    KOI8R_minus,		/*  WIN1251_emdash		151	*//**/
    KOI8R_copyright,		/*  WIN1251__0x98		152	*//**/
    KOI8R_copyright,		/*  WIN1251_trademark		153	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10106		154	*//**/
    KOI8R_copyright,		/*  WIN1251_guilsinglright	155	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10107		156	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10109		157	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10108		158	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10193		159	*//**/
    KOI8R_nobreakspace,		/*  WIN1251_nobreakspace	160	*/
    KOI8R_copyright,		/*  WIN1251_afii10062		161	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10110		162	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10057		163	*//**/
    KOI8R_copyright,		/*  WIN1251_currency		164	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10050		165	*//**/
    KOI8R_bar,			/*  WIN1251_brokenbar		166	*//**/
    KOI8R_copyright,		/*  WIN1251_section		167	*//**/
    KOI8R_afii10023,		/*  WIN1251_afii10023		168	*/
    KOI8R_copyright,		/*  WIN1251_copyright		169	*/
    KOI8R_copyright,		/*  WIN1251_afii10053		170	*//**/
    KOI8R_copyright,		/*  WIN1251_guillemotleft	171	*//**/
    KOI8R_copyright,		/*  WIN1251_logicalnot		172	*//**/
    KOI8R_minus,		/*  WIN1251_hyphen		173	*//**/
    KOI8R_copyright,		/*  WIN1251_registered		174	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10056		175	*//**/
    KOI8R_degree,		/*  WIN1251_degree		176	*/
    KOI8R_copyright,		/*  WIN1251_plusminus		177	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10055		178	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10103		179	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10098		180	*//**/
    KOI8R_copyright,		/*  WIN1251_mu			181	*//**/
    KOI8R_copyright,		/*  WIN1251_paragraph		182	*//**/
    KOI8R_periodcentered,	/*  WIN1251_periodcentered	183	*/
    KOI8R_afii10071,		/*  WIN1251_afii10071		184	*/
    KOI8R_copyright,		/*  WIN1251_afii61352		185	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10101		186	*//**/
    KOI8R_copyright,		/*  WIN1251_guillemotright	187	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10105		188	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10054		189	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10102		190	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10104		191	*//**/
    KOI8R_afii10017,		/*  WIN1251_afii10017		192	*/
    KOI8R_afii10018,		/*  WIN1251_afii10018		193	*/
    KOI8R_afii10019,		/*  WIN1251_afii10019		194	*/
    KOI8R_afii10020,		/*  WIN1251_afii10020		195	*/
    KOI8R_afii10021,		/*  WIN1251_afii10021		196	*/
    KOI8R_afii10022,		/*  WIN1251_afii10022		197	*/
    KOI8R_afii10024,		/*  WIN1251_afii10024		198	*/
    KOI8R_afii10025,		/*  WIN1251_afii10025		199	*/
    KOI8R_afii10026,		/*  WIN1251_afii10026		200	*/
    KOI8R_afii10027,		/*  WIN1251_afii10027		201	*/
    KOI8R_afii10028,		/*  WIN1251_afii10028		202	*/
    KOI8R_afii10029,		/*  WIN1251_afii10029		203	*/
    KOI8R_afii10030,		/*  WIN1251_afii10030		204	*/
    KOI8R_afii10031,		/*  WIN1251_afii10031		205	*/
    KOI8R_afii10032,		/*  WIN1251_afii10032		206	*/
    KOI8R_afii10033,		/*  WIN1251_afii10033		207	*/
    KOI8R_afii10034,		/*  WIN1251_afii10034		208	*/
    KOI8R_afii10035,		/*  WIN1251_afii10035		209	*/
    KOI8R_afii10036,		/*  WIN1251_afii10036		210	*/
    KOI8R_afii10037,		/*  WIN1251_afii10037		211	*/
    KOI8R_afii10038,		/*  WIN1251_afii10038		212	*/
    KOI8R_afii10039,		/*  WIN1251_afii10039		213	*/
    KOI8R_afii10040,		/*  WIN1251_afii10040		214	*/
    KOI8R_afii10041,		/*  WIN1251_afii10041		215	*/
    KOI8R_afii10042,		/*  WIN1251_afii10042		216	*/
    KOI8R_afii10043,		/*  WIN1251_afii10043		217	*/
    KOI8R_afii10044,		/*  WIN1251_afii10044		218	*/
    KOI8R_afii10045,		/*  WIN1251_afii10045		219	*/
    KOI8R_afii10046,		/*  WIN1251_afii10046		220	*/
    KOI8R_afii10047,		/*  WIN1251_afii10047		221	*/
    KOI8R_afii10048,		/*  WIN1251_afii10048		222	*/
    KOI8R_afii10049,		/*  WIN1251_afii10049		223	*/
    KOI8R_afii10065,		/*  WIN1251_afii10065		224	*/
    KOI8R_afii10066,		/*  WIN1251_afii10066		225	*/
    KOI8R_afii10067,		/*  WIN1251_afii10067		226	*/
    KOI8R_afii10068,		/*  WIN1251_afii10068		227	*/
    KOI8R_afii10069,		/*  WIN1251_afii10069		228	*/
    KOI8R_afii10070,		/*  WIN1251_afii10070		229	*/
    KOI8R_afii10072,		/*  WIN1251_afii10072		230	*/
    KOI8R_afii10073,		/*  WIN1251_afii10073		231	*/
    KOI8R_afii10074,		/*  WIN1251_afii10074		232	*/
    KOI8R_afii10075,		/*  WIN1251_afii10075		233	*/
    KOI8R_afii10076,		/*  WIN1251_afii10076		234	*/
    KOI8R_afii10077,		/*  WIN1251_afii10077		235	*/
    KOI8R_afii10078,		/*  WIN1251_afii10078		236	*/
    KOI8R_afii10079,		/*  WIN1251_afii10079		237	*/
    KOI8R_afii10080,		/*  WIN1251_afii10080		238	*/
    KOI8R_afii10081,		/*  WIN1251_afii10081		239	*/
    KOI8R_afii10082,		/*  WIN1251_afii10082		240	*/
    KOI8R_afii10083,		/*  WIN1251_afii10083		241	*/
    KOI8R_afii10084,		/*  WIN1251_afii10084		242	*/
    KOI8R_afii10085,		/*  WIN1251_afii10085		243	*/
    KOI8R_afii10086,		/*  WIN1251_afii10086		244	*/
    KOI8R_afii10087,		/*  WIN1251_afii10087		245	*/
    KOI8R_afii10088,		/*  WIN1251_afii10088		246	*/
    KOI8R_afii10089,		/*  WIN1251_afii10089		247	*/
    KOI8R_afii10090,		/*  WIN1251_afii10090		248	*/
    KOI8R_afii10091,		/*  WIN1251_afii10091		249	*/
    KOI8R_afii10092,		/*  WIN1251_afii10092		250	*/
    KOI8R_afii10093,		/*  WIN1251_afii10093		251	*/
    KOI8R_afii10094,		/*  WIN1251_afii10094		252	*/
    KOI8R_afii10095,		/*  WIN1251_afii10095		253	*/
    KOI8R_afii10096,		/*  WIN1251_afii10096		254	*/
    KOI8R_afii10097,		/*  WIN1251_afii10097		255	*/
    };

const unsigned char docWIN1251_to_ISO5[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    ISO5_space,			/*  WIN1251_space		 32	*/
    ISO5_exclam,		/*  WIN1251_exclam		 33	*/
    ISO5_quotedbl,		/*  WIN1251_quotedbl		 34	*/
    ISO5_numbersign,		/*  WIN1251_numbersign		 35	*/
    ISO5_dollar,		/*  WIN1251_dollar		 36	*/
    ISO5_percent,		/*  WIN1251_percent		 37	*/
    ISO5_ampersand,		/*  WIN1251_ampersand		 38	*/
    ISO5_quotesingle,		/*  WIN1251_quotesingle		 39	*/
    ISO5_parenleft,		/*  WIN1251_parenleft		 40	*/
    ISO5_parenright,		/*  WIN1251_parenright		 41	*/
    ISO5_asterisk,		/*  WIN1251_asterisk		 42	*/
    ISO5_plus,			/*  WIN1251_plus		 43	*/
    ISO5_comma,			/*  WIN1251_comma		 44	*/
    ISO5_minus,			/*  WIN1251_minus		 45	*/
    ISO5_period,		/*  WIN1251_period		 46	*/
    ISO5_slash,			/*  WIN1251_slash		 47	*/
    ISO5_zero,			/*  WIN1251_zero		 48	*/
    ISO5_one,			/*  WIN1251_one			 49	*/
    ISO5_two,			/*  WIN1251_two			 50	*/
    ISO5_three,			/*  WIN1251_three		 51	*/
    ISO5_four,			/*  WIN1251_four		 52	*/
    ISO5_five,			/*  WIN1251_five		 53	*/
    ISO5_six,			/*  WIN1251_six			 54	*/
    ISO5_seven,			/*  WIN1251_seven		 55	*/
    ISO5_eight,			/*  WIN1251_eight		 56	*/
    ISO5_nine,			/*  WIN1251_nine		 57	*/
    ISO5_colon,			/*  WIN1251_colon		 58	*/
    ISO5_semicolon,		/*  WIN1251_semicolon		 59	*/
    ISO5_less,			/*  WIN1251_less		 60	*/
    ISO5_equal,			/*  WIN1251_equal		 61	*/
    ISO5_greater,		/*  WIN1251_greater		 62	*/
    ISO5_question,		/*  WIN1251_question		 63	*/
    ISO5_at,			/*  WIN1251_at			 64	*/
    ISO5_A,			/*  WIN1251_A			 65	*/
    ISO5_B,			/*  WIN1251_B			 66	*/
    ISO5_C,			/*  WIN1251_C			 67	*/
    ISO5_D,			/*  WIN1251_D			 68	*/
    ISO5_E,			/*  WIN1251_E			 69	*/
    ISO5_F,			/*  WIN1251_F			 70	*/
    ISO5_G,			/*  WIN1251_G			 71	*/
    ISO5_H,			/*  WIN1251_H			 72	*/
    ISO5_I,			/*  WIN1251_I			 73	*/
    ISO5_J,			/*  WIN1251_J			 74	*/
    ISO5_K,			/*  WIN1251_K			 75	*/
    ISO5_L,			/*  WIN1251_L			 76	*/
    ISO5_M,			/*  WIN1251_M			 77	*/
    ISO5_N,			/*  WIN1251_N			 78	*/
    ISO5_O,			/*  WIN1251_O			 79	*/
    ISO5_P,			/*  WIN1251_P			 80	*/
    ISO5_Q,			/*  WIN1251_Q			 81	*/
    ISO5_R,			/*  WIN1251_R			 82	*/
    ISO5_S,			/*  WIN1251_S			 83	*/
    ISO5_T,			/*  WIN1251_T			 84	*/
    ISO5_U,			/*  WIN1251_U			 85	*/
    ISO5_V,			/*  WIN1251_V			 86	*/
    ISO5_W,			/*  WIN1251_W			 87	*/
    ISO5_X,			/*  WIN1251_X			 88	*/
    ISO5_Y,			/*  WIN1251_Y			 89	*/
    ISO5_Z,			/*  WIN1251_Z			 90	*/
    ISO5_bracketleft,		/*  WIN1251_bracketleft		 91	*/
    ISO5_backslash,		/*  WIN1251_backslash		 92	*/
    ISO5_bracketright,		/*  WIN1251_bracketright	 93	*/
    ISO5_asciicircum,		/*  WIN1251_asciicircum		 94	*/
    ISO5_underscore,		/*  WIN1251_underscore		 95	*/
    ISO5_quoteleft,		/*  WIN1251_quoteleft		 96	*/
    ISO5_a,			/*  WIN1251_a			 97	*/
    ISO5_b,			/*  WIN1251_b			 98	*/
    ISO5_c,			/*  WIN1251_c			 99	*/
    ISO5_d,			/*  WIN1251_d			100	*/
    ISO5_e,			/*  WIN1251_e			101	*/
    ISO5_f,			/*  WIN1251_f			102	*/
    ISO5_g,			/*  WIN1251_g			103	*/
    ISO5_h,			/*  WIN1251_h			104	*/
    ISO5_i,			/*  WIN1251_i			105	*/
    ISO5_j,			/*  WIN1251_j			106	*/
    ISO5_k,			/*  WIN1251_k			107	*/
    ISO5_l,			/*  WIN1251_l			108	*/
    ISO5_m,			/*  WIN1251_m			109	*/
    ISO5_n,			/*  WIN1251_n			110	*/
    ISO5_o,			/*  WIN1251_o			111	*/
    ISO5_p,			/*  WIN1251_p			112	*/
    ISO5_q,			/*  WIN1251_q			113	*/
    ISO5_r,			/*  WIN1251_r			114	*/
    ISO5_s,			/*  WIN1251_s			115	*/
    ISO5_t,			/*  WIN1251_t			116	*/
    ISO5_u,			/*  WIN1251_u			117	*/
    ISO5_v,			/*  WIN1251_v			118	*/
    ISO5_w,			/*  WIN1251_w			119	*/
    ISO5_x,			/*  WIN1251_x			120	*/
    ISO5_y,			/*  WIN1251_y			121	*/
    ISO5_z,			/*  WIN1251_z			122	*/
    ISO5_braceleft,		/*  WIN1251_braceleft		123	*/
    ISO5_bar,			/*  WIN1251_bar			124	*/
    ISO5_braceright,		/*  WIN1251_braceright		125	*/
    ISO5_asciitilde,		/*  WIN1251_asciitilde		126	*/
    ISO5_at,			/*  WIN1251__0x7F		127	*//**/
    ISO5_afii10051,		/*  WIN1251_afii10051		128	*/
    ISO5_afii10052,		/*  WIN1251_afii10052		129	*/
    ISO5_quoteleft,		/*  WIN1251_quotesinglbase	130	*//**/
    ISO5_afii10100,		/*  WIN1251_afii10100		131	*/
    ISO5_quotedbl,		/*  WIN1251_quotedblbase	132	*//**/
    ISO5_at,			/*  WIN1251_ellipsis		133	*//**/
    ISO5_at,			/*  WIN1251_dagger		134	*//**/
    ISO5_at,			/*  WIN1251_daggerdbl		135	*//**/
    ISO5_at,			/*  WIN1251__0x88		136	*//**/
    ISO5_at,			/*  WIN1251_perthousand		137	*//**/
    ISO5_afii10058,		/*  WIN1251_afii10058		138	*/
    ISO5_at,			/*  WIN1251_guilsinglleft	139	*//**/
    ISO5_afii10059,		/*  WIN1251_afii10059		140	*/
    ISO5_afii10061,		/*  WIN1251_afii10061		141	*/
    ISO5_afii10060,		/*  WIN1251_afii10060		142	*/
    ISO5_afii10145,		/*  WIN1251_afii10145		143	*/
    ISO5_afii10099,		/*  WIN1251_afii10099		144	*/
    ISO5_quoteleft,		/*  WIN1251_quoteleft		145	*/
    ISO5_quoteleft,		/*  WIN1251_quoteright		146	*//**/
    ISO5_quotedbl,		/*  WIN1251_quotedblleft	147	*//**/
    ISO5_quotedbl,		/*  WIN1251_quotedblright	148	*//**/
    ISO5_at,			/*  WIN1251_bullet		149	*//**/
    ISO5_minus,			/*  WIN1251_endash		150	*//**/
    ISO5_minus,			/*  WIN1251_emdash		151	*//**/
    ISO5_at,			/*  WIN1251__0x98		152	*//**/
    ISO5_at,			/*  WIN1251_trademark		153	*//**/
    ISO5_afii10106,		/*  WIN1251_afii10106		154	*/
    ISO5_at,			/*  WIN1251_guilsinglright	155	*//**/
    ISO5_afii10107,		/*  WIN1251_afii10107		156	*/
    ISO5_afii10109,		/*  WIN1251_afii10109		157	*/
    ISO5_afii10108,		/*  WIN1251_afii10108		158	*/
    ISO5_afii10193,		/*  WIN1251_afii10193		159	*/
    ISO5_nobreakspace,		/*  WIN1251_nobreakspace	160	*/
    ISO5_afii10062,		/*  WIN1251_afii10062		161	*/
    ISO5_afii10110,		/*  WIN1251_afii10110		162	*/
    ISO5_afii10057,		/*  WIN1251_afii10057		163	*/
    ISO5_at,			/*  WIN1251_currency		164	*//**/
    ISO5_at,			/*  WIN1251_afii10050		165	*//**/
    ISO5_bar,			/*  WIN1251_brokenbar		166	*//**/
    ISO5_section,		/*  WIN1251_section		167	*/
    ISO5_afii10023,		/*  WIN1251_afii10023		168	*/
    ISO5_at,			/*  WIN1251_copyright		169	*//**/
    ISO5_afii10053,		/*  WIN1251_afii10053		170	*/
    ISO5_at,			/*  WIN1251_guillemotleft	171	*//**/
    ISO5_at,			/*  WIN1251_logicalnot		172	*//**/
    ISO5_hyphen,		/*  WIN1251_hyphen		173	*/
    ISO5_at,			/*  WIN1251_registered		174	*//**/
    ISO5_afii10056,		/*  WIN1251_afii10056		175	*/
    ISO5_at,			/*  WIN1251_degree		176	*//**/
    ISO5_at,			/*  WIN1251_plusminus		177	*//**/
    ISO5_afii10055,		/*  WIN1251_afii10055		178	*/
    ISO5_afii10103,		/*  WIN1251_afii10103		179	*/
    ISO5_at,			/*  WIN1251_afii10098		180	*//**/
    ISO5_at,			/*  WIN1251_mu			181	*//**/
    ISO5_at,			/*  WIN1251_paragraph		182	*//**/
    ISO5_at,			/*  WIN1251_periodcentered	183	*//**/
    ISO5_afii10071,		/*  WIN1251_afii10071		184	*/
    ISO5_afii61352,		/*  WIN1251_afii61352		185	*/
    ISO5_afii10101,		/*  WIN1251_afii10101		186	*/
    ISO5_at,			/*  WIN1251_guillemotright	187	*//**/
    ISO5_afii10105,		/*  WIN1251_afii10105		188	*/
    ISO5_afii10054,		/*  WIN1251_afii10054		189	*/
    ISO5_afii10102,		/*  WIN1251_afii10102		190	*/
    ISO5_afii10104,		/*  WIN1251_afii10104		191	*/
    ISO5_afii10017,		/*  WIN1251_afii10017		192	*/
    ISO5_afii10018,		/*  WIN1251_afii10018		193	*/
    ISO5_afii10019,		/*  WIN1251_afii10019		194	*/
    ISO5_afii10020,		/*  WIN1251_afii10020		195	*/
    ISO5_afii10021,		/*  WIN1251_afii10021		196	*/
    ISO5_afii10022,		/*  WIN1251_afii10022		197	*/
    ISO5_afii10024,		/*  WIN1251_afii10024		198	*/
    ISO5_afii10025,		/*  WIN1251_afii10025		199	*/
    ISO5_afii10026,		/*  WIN1251_afii10026		200	*/
    ISO5_afii10027,		/*  WIN1251_afii10027		201	*/
    ISO5_afii10028,		/*  WIN1251_afii10028		202	*/
    ISO5_afii10029,		/*  WIN1251_afii10029		203	*/
    ISO5_afii10030,		/*  WIN1251_afii10030		204	*/
    ISO5_afii10031,		/*  WIN1251_afii10031		205	*/
    ISO5_afii10032,		/*  WIN1251_afii10032		206	*/
    ISO5_afii10033,		/*  WIN1251_afii10033		207	*/
    ISO5_afii10034,		/*  WIN1251_afii10034		208	*/
    ISO5_afii10035,		/*  WIN1251_afii10035		209	*/
    ISO5_afii10036,		/*  WIN1251_afii10036		210	*/
    ISO5_afii10037,		/*  WIN1251_afii10037		211	*/
    ISO5_afii10038,		/*  WIN1251_afii10038		212	*/
    ISO5_afii10039,		/*  WIN1251_afii10039		213	*/
    ISO5_afii10040,		/*  WIN1251_afii10040		214	*/
    ISO5_afii10041,		/*  WIN1251_afii10041		215	*/
    ISO5_afii10042,		/*  WIN1251_afii10042		216	*/
    ISO5_afii10043,		/*  WIN1251_afii10043		217	*/
    ISO5_afii10044,		/*  WIN1251_afii10044		218	*/
    ISO5_afii10045,		/*  WIN1251_afii10045		219	*/
    ISO5_afii10046,		/*  WIN1251_afii10046		220	*/
    ISO5_afii10047,		/*  WIN1251_afii10047		221	*/
    ISO5_afii10048,		/*  WIN1251_afii10048		222	*/
    ISO5_afii10049,		/*  WIN1251_afii10049		223	*/
    ISO5_afii10065,		/*  WIN1251_afii10065		224	*/
    ISO5_afii10066,		/*  WIN1251_afii10066		225	*/
    ISO5_afii10067,		/*  WIN1251_afii10067		226	*/
    ISO5_afii10068,		/*  WIN1251_afii10068		227	*/
    ISO5_afii10069,		/*  WIN1251_afii10069		228	*/
    ISO5_afii10070,		/*  WIN1251_afii10070		229	*/
    ISO5_afii10072,		/*  WIN1251_afii10072		230	*/
    ISO5_afii10073,		/*  WIN1251_afii10073		231	*/
    ISO5_afii10074,		/*  WIN1251_afii10074		232	*/
    ISO5_afii10075,		/*  WIN1251_afii10075		233	*/
    ISO5_afii10076,		/*  WIN1251_afii10076		234	*/
    ISO5_afii10077,		/*  WIN1251_afii10077		235	*/
    ISO5_afii10078,		/*  WIN1251_afii10078		236	*/
    ISO5_afii10079,		/*  WIN1251_afii10079		237	*/
    ISO5_afii10080,		/*  WIN1251_afii10080		238	*/
    ISO5_afii10081,		/*  WIN1251_afii10081		239	*/
    ISO5_afii10082,		/*  WIN1251_afii10082		240	*/
    ISO5_afii10083,		/*  WIN1251_afii10083		241	*/
    ISO5_afii10084,		/*  WIN1251_afii10084		242	*/
    ISO5_afii10085,		/*  WIN1251_afii10085		243	*/
    ISO5_afii10086,		/*  WIN1251_afii10086		244	*/
    ISO5_afii10087,		/*  WIN1251_afii10087		245	*/
    ISO5_afii10088,		/*  WIN1251_afii10088		246	*/
    ISO5_afii10089,		/*  WIN1251_afii10089		247	*/
    ISO5_afii10090,		/*  WIN1251_afii10090		248	*/
    ISO5_afii10091,		/*  WIN1251_afii10091		249	*/
    ISO5_afii10092,		/*  WIN1251_afii10092		250	*/
    ISO5_afii10093,		/*  WIN1251_afii10093		251	*/
    ISO5_afii10094,		/*  WIN1251_afii10094		252	*/
    ISO5_afii10095,		/*  WIN1251_afii10095		253	*/
    ISO5_afii10096,		/*  WIN1251_afii10096		254	*/
    ISO5_afii10097,		/*  WIN1251_afii10097		255	*/
    };

const unsigned char docISO5_to_WIN1251[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    WIN1251_space,		/*  ISO5_space			 32	*/
    WIN1251_exclam,		/*  ISO5_exclam			 33	*/
    WIN1251_quotedbl,		/*  ISO5_quotedbl		 34	*/
    WIN1251_numbersign,		/*  ISO5_numbersign		 35	*/
    WIN1251_dollar,		/*  ISO5_dollar			 36	*/
    WIN1251_percent,		/*  ISO5_percent		 37	*/
    WIN1251_ampersand,		/*  ISO5_ampersand		 38	*/
    WIN1251_quotesingle,	/*  ISO5_quotesingle		 39	*/
    WIN1251_parenleft,		/*  ISO5_parenleft		 40	*/
    WIN1251_parenright,		/*  ISO5_parenright		 41	*/
    WIN1251_asterisk,		/*  ISO5_asterisk		 42	*/
    WIN1251_plus,		/*  ISO5_plus			 43	*/
    WIN1251_comma,		/*  ISO5_comma			 44	*/
    WIN1251_minus,		/*  ISO5_minus			 45	*/
    WIN1251_period,		/*  ISO5_period			 46	*/
    WIN1251_slash,		/*  ISO5_slash			 47	*/
    WIN1251_zero,		/*  ISO5_zero			 48	*/
    WIN1251_one,		/*  ISO5_one			 49	*/
    WIN1251_two,		/*  ISO5_two			 50	*/
    WIN1251_three,		/*  ISO5_three			 51	*/
    WIN1251_four,		/*  ISO5_four			 52	*/
    WIN1251_five,		/*  ISO5_five			 53	*/
    WIN1251_six,		/*  ISO5_six			 54	*/
    WIN1251_seven,		/*  ISO5_seven			 55	*/
    WIN1251_eight,		/*  ISO5_eight			 56	*/
    WIN1251_nine,		/*  ISO5_nine			 57	*/
    WIN1251_colon,		/*  ISO5_colon			 58	*/
    WIN1251_semicolon,		/*  ISO5_semicolon		 59	*/
    WIN1251_less,		/*  ISO5_less			 60	*/
    WIN1251_equal,		/*  ISO5_equal			 61	*/
    WIN1251_greater,		/*  ISO5_greater		 62	*/
    WIN1251_question,		/*  ISO5_question		 63	*/
    WIN1251_at,			/*  ISO5_at			 64	*/
    WIN1251_A,			/*  ISO5_A			 65	*/
    WIN1251_B,			/*  ISO5_B			 66	*/
    WIN1251_C,			/*  ISO5_C			 67	*/
    WIN1251_D,			/*  ISO5_D			 68	*/
    WIN1251_E,			/*  ISO5_E			 69	*/
    WIN1251_F,			/*  ISO5_F			 70	*/
    WIN1251_G,			/*  ISO5_G			 71	*/
    WIN1251_H,			/*  ISO5_H			 72	*/
    WIN1251_I,			/*  ISO5_I			 73	*/
    WIN1251_J,			/*  ISO5_J			 74	*/
    WIN1251_K,			/*  ISO5_K			 75	*/
    WIN1251_L,			/*  ISO5_L			 76	*/
    WIN1251_M,			/*  ISO5_M			 77	*/
    WIN1251_N,			/*  ISO5_N			 78	*/
    WIN1251_O,			/*  ISO5_O			 79	*/
    WIN1251_P,			/*  ISO5_P			 80	*/
    WIN1251_Q,			/*  ISO5_Q			 81	*/
    WIN1251_R,			/*  ISO5_R			 82	*/
    WIN1251_S,			/*  ISO5_S			 83	*/
    WIN1251_T,			/*  ISO5_T			 84	*/
    WIN1251_U,			/*  ISO5_U			 85	*/
    WIN1251_V,			/*  ISO5_V			 86	*/
    WIN1251_W,			/*  ISO5_W			 87	*/
    WIN1251_X,			/*  ISO5_X			 88	*/
    WIN1251_Y,			/*  ISO5_Y			 89	*/
    WIN1251_Z,			/*  ISO5_Z			 90	*/
    WIN1251_bracketleft,	/*  ISO5_bracketleft		 91	*/
    WIN1251_backslash,		/*  ISO5_backslash		 92	*/
    WIN1251_bracketright,	/*  ISO5_bracketright		 93	*/
    WIN1251_asciicircum,	/*  ISO5_asciicircum		 94	*/
    WIN1251_underscore,		/*  ISO5_underscore		 95	*/
    WIN1251_quoteleft,		/*  ISO5_quoteleft		 96	*/
    WIN1251_a,			/*  ISO5_a			 97	*/
    WIN1251_b,			/*  ISO5_b			 98	*/
    WIN1251_c,			/*  ISO5_c			 99	*/
    WIN1251_d,			/*  ISO5_d			100	*/
    WIN1251_e,			/*  ISO5_e			101	*/
    WIN1251_f,			/*  ISO5_f			102	*/
    WIN1251_g,			/*  ISO5_g			103	*/
    WIN1251_h,			/*  ISO5_h			104	*/
    WIN1251_i,			/*  ISO5_i			105	*/
    WIN1251_j,			/*  ISO5_j			106	*/
    WIN1251_k,			/*  ISO5_k			107	*/
    WIN1251_l,			/*  ISO5_l			108	*/
    WIN1251_m,			/*  ISO5_m			109	*/
    WIN1251_n,			/*  ISO5_n			110	*/
    WIN1251_o,			/*  ISO5_o			111	*/
    WIN1251_p,			/*  ISO5_p			112	*/
    WIN1251_q,			/*  ISO5_q			113	*/
    WIN1251_r,			/*  ISO5_r			114	*/
    WIN1251_s,			/*  ISO5_s			115	*/
    WIN1251_t,			/*  ISO5_t			116	*/
    WIN1251_u,			/*  ISO5_u			117	*/
    WIN1251_v,			/*  ISO5_v			118	*/
    WIN1251_w,			/*  ISO5_w			119	*/
    WIN1251_x,			/*  ISO5_x			120	*/
    WIN1251_y,			/*  ISO5_y			121	*/
    WIN1251_z,			/*  ISO5_z			122	*/
    WIN1251_braceleft,		/*  ISO5_braceleft		123	*/
    WIN1251_bar,		/*  ISO5_bar			124	*/
    WIN1251_braceright,		/*  ISO5_braceright		125	*/
    WIN1251_asciitilde,		/*  ISO5_asciitilde		126	*/
    WIN1251_currency,		/*  .NOTDEF_127			127	*//**/
    WIN1251_currency,		/*  .NOTDEF_128			128	*//**/
    WIN1251_currency,		/*  .NOTDEF_129			129	*//**/
    WIN1251_currency,		/*  .NOTDEF_130			130	*//**/
    WIN1251_currency,		/*  .NOTDEF_131			131	*//**/
    WIN1251_currency,		/*  .NOTDEF_132			132	*//**/
    WIN1251_currency,		/*  .NOTDEF_133			133	*//**/
    WIN1251_currency,		/*  .NOTDEF_134			134	*//**/
    WIN1251_currency,		/*  .NOTDEF_135			135	*//**/
    WIN1251_currency,		/*  .NOTDEF_136			136	*//**/
    WIN1251_currency,		/*  .NOTDEF_137			137	*//**/
    WIN1251_currency,		/*  .NOTDEF_138			138	*//**/
    WIN1251_currency,		/*  .NOTDEF_139			139	*//**/
    WIN1251_currency,		/*  .NOTDEF_140			140	*//**/
    WIN1251_currency,		/*  .NOTDEF_141			141	*//**/
    WIN1251_currency,		/*  .NOTDEF_142			142	*//**/
    WIN1251_currency,		/*  .NOTDEF_143			143	*//**/
    WIN1251_currency,		/*  .NOTDEF_144			144	*//**/
    WIN1251_currency,		/*  .NOTDEF_145			145	*//**/
    WIN1251_currency,		/*  .NOTDEF_146			146	*//**/
    WIN1251_currency,		/*  .NOTDEF_147			147	*//**/
    WIN1251_currency,		/*  .NOTDEF_148			148	*//**/
    WIN1251_currency,		/*  .NOTDEF_149			149	*//**/
    WIN1251_currency,		/*  .NOTDEF_150			150	*//**/
    WIN1251_currency,		/*  .NOTDEF_151			151	*//**/
    WIN1251_currency,		/*  .NOTDEF_152			152	*//**/
    WIN1251_currency,		/*  .NOTDEF_153			153	*//**/
    WIN1251_currency,		/*  .NOTDEF_154			154	*//**/
    WIN1251_currency,		/*  .NOTDEF_155			155	*//**/
    WIN1251_currency,		/*  .NOTDEF_156			156	*//**/
    WIN1251_currency,		/*  .NOTDEF_157			157	*//**/
    WIN1251_currency,		/*  .NOTDEF_158			158	*//**/
    WIN1251_currency,		/*  .NOTDEF_159			159	*//**/
    WIN1251_nobreakspace,	/*  ISO5_nobreakspace		160	*/
    WIN1251_afii10023,		/*  ISO5_afii10023		161	*/
    WIN1251_afii10051,		/*  ISO5_afii10051		162	*/
    WIN1251_afii10052,		/*  ISO5_afii10052		163	*/
    WIN1251_afii10053,		/*  ISO5_afii10053		164	*/
    WIN1251_afii10054,		/*  ISO5_afii10054		165	*/
    WIN1251_afii10055,		/*  ISO5_afii10055		166	*/
    WIN1251_afii10056,		/*  ISO5_afii10056		167	*/
    WIN1251_afii10057,		/*  ISO5_afii10057		168	*/
    WIN1251_afii10058,		/*  ISO5_afii10058		169	*/
    WIN1251_afii10059,		/*  ISO5_afii10059		170	*/
    WIN1251_afii10060,		/*  ISO5_afii10060		171	*/
    WIN1251_afii10061,		/*  ISO5_afii10061		172	*/
    WIN1251_hyphen,		/*  ISO5_hyphen			173	*/
    WIN1251_afii10062,		/*  ISO5_afii10062		174	*/
    WIN1251_afii10145,		/*  ISO5_afii10145		175	*/
    WIN1251_afii10017,		/*  ISO5_afii10017		176	*/
    WIN1251_afii10018,		/*  ISO5_afii10018		177	*/
    WIN1251_afii10019,		/*  ISO5_afii10019		178	*/
    WIN1251_afii10020,		/*  ISO5_afii10020		179	*/
    WIN1251_afii10021,		/*  ISO5_afii10021		180	*/
    WIN1251_afii10022,		/*  ISO5_afii10022		181	*/
    WIN1251_afii10024,		/*  ISO5_afii10024		182	*/
    WIN1251_afii10025,		/*  ISO5_afii10025		183	*/
    WIN1251_afii10026,		/*  ISO5_afii10026		184	*/
    WIN1251_afii10027,		/*  ISO5_afii10027		185	*/
    WIN1251_afii10028,		/*  ISO5_afii10028		186	*/
    WIN1251_afii10029,		/*  ISO5_afii10029		187	*/
    WIN1251_afii10030,		/*  ISO5_afii10030		188	*/
    WIN1251_afii10031,		/*  ISO5_afii10031		189	*/
    WIN1251_afii10032,		/*  ISO5_afii10032		190	*/
    WIN1251_afii10033,		/*  ISO5_afii10033		191	*/
    WIN1251_afii10034,		/*  ISO5_afii10034		192	*/
    WIN1251_afii10035,		/*  ISO5_afii10035		193	*/
    WIN1251_afii10036,		/*  ISO5_afii10036		194	*/
    WIN1251_afii10037,		/*  ISO5_afii10037		195	*/
    WIN1251_afii10038,		/*  ISO5_afii10038		196	*/
    WIN1251_afii10039,		/*  ISO5_afii10039		197	*/
    WIN1251_afii10040,		/*  ISO5_afii10040		198	*/
    WIN1251_afii10041,		/*  ISO5_afii10041		199	*/
    WIN1251_afii10042,		/*  ISO5_afii10042		200	*/
    WIN1251_afii10043,		/*  ISO5_afii10043		201	*/
    WIN1251_afii10044,		/*  ISO5_afii10044		202	*/
    WIN1251_afii10045,		/*  ISO5_afii10045		203	*/
    WIN1251_afii10046,		/*  ISO5_afii10046		204	*/
    WIN1251_afii10047,		/*  ISO5_afii10047		205	*/
    WIN1251_afii10048,		/*  ISO5_afii10048		206	*/
    WIN1251_afii10049,		/*  ISO5_afii10049		207	*/
    WIN1251_afii10065,		/*  ISO5_afii10065		208	*/
    WIN1251_afii10066,		/*  ISO5_afii10066		209	*/
    WIN1251_afii10067,		/*  ISO5_afii10067		210	*/
    WIN1251_afii10068,		/*  ISO5_afii10068		211	*/
    WIN1251_afii10069,		/*  ISO5_afii10069		212	*/
    WIN1251_afii10070,		/*  ISO5_afii10070		213	*/
    WIN1251_afii10072,		/*  ISO5_afii10072		214	*/
    WIN1251_afii10073,		/*  ISO5_afii10073		215	*/
    WIN1251_afii10074,		/*  ISO5_afii10074		216	*/
    WIN1251_afii10075,		/*  ISO5_afii10075		217	*/
    WIN1251_afii10076,		/*  ISO5_afii10076		218	*/
    WIN1251_afii10077,		/*  ISO5_afii10077		219	*/
    WIN1251_afii10078,		/*  ISO5_afii10078		220	*/
    WIN1251_afii10079,		/*  ISO5_afii10079		221	*/
    WIN1251_afii10080,		/*  ISO5_afii10080		222	*/
    WIN1251_afii10081,		/*  ISO5_afii10081		223	*/
    WIN1251_afii10082,		/*  ISO5_afii10082		224	*/
    WIN1251_afii10083,		/*  ISO5_afii10083		225	*/
    WIN1251_afii10084,		/*  ISO5_afii10084		226	*/
    WIN1251_afii10085,		/*  ISO5_afii10085		227	*/
    WIN1251_afii10086,		/*  ISO5_afii10086		228	*/
    WIN1251_afii10087,		/*  ISO5_afii10087		229	*/
    WIN1251_afii10088,		/*  ISO5_afii10088		230	*/
    WIN1251_afii10089,		/*  ISO5_afii10089		231	*/
    WIN1251_afii10090,		/*  ISO5_afii10090		232	*/
    WIN1251_afii10091,		/*  ISO5_afii10091		233	*/
    WIN1251_afii10092,		/*  ISO5_afii10092		234	*/
    WIN1251_afii10093,		/*  ISO5_afii10093		235	*/
    WIN1251_afii10094,		/*  ISO5_afii10094		236	*/
    WIN1251_afii10095,		/*  ISO5_afii10095		237	*/
    WIN1251_afii10096,		/*  ISO5_afii10096		238	*/
    WIN1251_afii10097,		/*  ISO5_afii10097		239	*/
    WIN1251_afii61352,		/*  ISO5_afii61352		240	*/
    WIN1251_afii10071,		/*  ISO5_afii10071		241	*/
    WIN1251_afii10099,		/*  ISO5_afii10099		242	*/
    WIN1251_afii10100,		/*  ISO5_afii10100		243	*/
    WIN1251_afii10101,		/*  ISO5_afii10101		244	*/
    WIN1251_afii10102,		/*  ISO5_afii10102		245	*/
    WIN1251_afii10103,		/*  ISO5_afii10103		246	*/
    WIN1251_afii10104,		/*  ISO5_afii10104		247	*/
    WIN1251_afii10105,		/*  ISO5_afii10105		248	*/
    WIN1251_afii10106,		/*  ISO5_afii10106		249	*/
    WIN1251_afii10107,		/*  ISO5_afii10107		250	*/
    WIN1251_afii10108,		/*  ISO5_afii10108		251	*/
    WIN1251_afii10109,		/*  ISO5_afii10109		252	*/
    WIN1251_section,		/*  ISO5_section		253	*/
    WIN1251_afii10110,		/*  ISO5_afii10110		254	*/
    WIN1251_afii10193,		/*  ISO5_afii10193		255	*/
    };

const unsigned char docISO7_to_WIN1253[256]=
    {
    ASCII__nul,			/*	ISO7__nul		0	*/
    ASCII__soh,			/*	ISO7__soh		1	*/
    ASCII__stx,			/*	ISO7__stx		2	*/
    ASCII__etx,			/*	ISO7__etx		3	*/
    ASCII__eot,			/*	ISO7__eot		4	*/
    ASCII__enq,			/*	ISO7__enq		5	*/
    ASCII__ack,			/*	ISO7__ack		6	*/
    ASCII__bel,			/*	ISO7__bel		7	*/
    ASCII__bs,			/*	ISO7__bs		8	*/
    ASCII__ht,			/*	ISO7__ht		9	*/
    ASCII__nl,			/*	ISO7__nl		10	*/
    ASCII__vt,			/*	ISO7__vt		11	*/
    ASCII__np,			/*	ISO7__np		12	*/
    ASCII__cr,			/*	ISO7__cr		13	*/
    ASCII__so,			/*	ISO7__so		14	*/
    ASCII__si,			/*	ISO7__si		15	*/
    ASCII__dle,			/*	ISO7__dle		16	*/
    ASCII__dc1,			/*	ISO7__dc1		17	*/
    ASCII__dc2,			/*	ISO7__dc2		18	*/
    ASCII__dc3,			/*	ISO7__dc3		19	*/
    ASCII__dc4,			/*	ISO7__dc4		20	*/
    ASCII__nak,			/*	ISO7__nak		21	*/
    ASCII__syn,			/*	ISO7__syn		22	*/
    ASCII__etb,			/*	ISO7__etb		23	*/
    ASCII__can,			/*	ISO7__can		24	*/
    ASCII__em,			/*	ISO7__em		25	*/
    ASCII__sub,			/*	ISO7__sub		26	*/
    ASCII__esc,			/*	ISO7__esc		27	*/
    ASCII__fs,			/*	ISO7__fs		28	*/
    ASCII__gs,			/*	ISO7__gs		29	*/
    ASCII__rs,			/*	ISO7__rs		30	*/
    ASCII__us,			/*	ISO7__us		31	*/
    WIN1253_space,		/*  ISO7_space			 32	*/
    WIN1253_exclam,		/*  ISO7_exclam			 33	*/
    WIN1253_quotedbl,		/*  ISO7_quotedbl		 34	*/
    WIN1253_numbersign,		/*  ISO7_numbersign		 35	*/
    WIN1253_dollar,		/*  ISO7_dollar			 36	*/
    WIN1253_percent,		/*  ISO7_percent		 37	*/
    WIN1253_ampersand,		/*  ISO7_ampersand		 38	*/
    WIN1253_quotesingle,	/*  ISO7_quotesingle		 39	*/
    WIN1253_parenleft,		/*  ISO7_parenleft		 40	*/
    WIN1253_parenright,		/*  ISO7_parenright		 41	*/
    WIN1253_asterisk,		/*  ISO7_asterisk		 42	*/
    WIN1253_plus,		/*  ISO7_plus			 43	*/
    WIN1253_comma,		/*  ISO7_comma			 44	*/
    WIN1253_hyphen,		/*  ISO7_hyphen			 45	*/
    WIN1253_period,		/*  ISO7_period			 46	*/
    WIN1253_slash,		/*  ISO7_slash			 47	*/
    WIN1253_zero,		/*  ISO7_zero			 48	*/
    WIN1253_one,		/*  ISO7_one			 49	*/
    WIN1253_two,		/*  ISO7_two			 50	*/
    WIN1253_three,		/*  ISO7_three			 51	*/
    WIN1253_four,		/*  ISO7_four			 52	*/
    WIN1253_five,		/*  ISO7_five			 53	*/
    WIN1253_six,		/*  ISO7_six			 54	*/
    WIN1253_seven,		/*  ISO7_seven			 55	*/
    WIN1253_eight,		/*  ISO7_eight			 56	*/
    WIN1253_nine,		/*  ISO7_nine			 57	*/
    WIN1253_colon,		/*  ISO7_colon			 58	*/
    WIN1253_semicolon,		/*  ISO7_semicolon		 59	*/
    WIN1253_less,		/*  ISO7_less			 60	*/
    WIN1253_equal,		/*  ISO7_equal			 61	*/
    WIN1253_greater,		/*  ISO7_greater		 62	*/
    WIN1253_question,		/*  ISO7_question		 63	*/
    WIN1253_at,			/*  ISO7_at			 64	*/
    WIN1253_A,			/*  ISO7_A			 65	*/
    WIN1253_B,			/*  ISO7_B			 66	*/
    WIN1253_C,			/*  ISO7_C			 67	*/
    WIN1253_D,			/*  ISO7_D			 68	*/
    WIN1253_E,			/*  ISO7_E			 69	*/
    WIN1253_F,			/*  ISO7_F			 70	*/
    WIN1253_G,			/*  ISO7_G			 71	*/
    WIN1253_H,			/*  ISO7_H			 72	*/
    WIN1253_I,			/*  ISO7_I			 73	*/
    WIN1253_J,			/*  ISO7_J			 74	*/
    WIN1253_K,			/*  ISO7_K			 75	*/
    WIN1253_L,			/*  ISO7_L			 76	*/
    WIN1253_M,			/*  ISO7_M			 77	*/
    WIN1253_N,			/*  ISO7_N			 78	*/
    WIN1253_O,			/*  ISO7_O			 79	*/
    WIN1253_P,			/*  ISO7_P			 80	*/
    WIN1253_Q,			/*  ISO7_Q			 81	*/
    WIN1253_R,			/*  ISO7_R			 82	*/
    WIN1253_S,			/*  ISO7_S			 83	*/
    WIN1253_T,			/*  ISO7_T			 84	*/
    WIN1253_U,			/*  ISO7_U			 85	*/
    WIN1253_V,			/*  ISO7_V			 86	*/
    WIN1253_W,			/*  ISO7_W			 87	*/
    WIN1253_X,			/*  ISO7_X			 88	*/
    WIN1253_Y,			/*  ISO7_Y			 89	*/
    WIN1253_Z,			/*  ISO7_Z			 90	*/
    WIN1253_bracketleft,	/*  ISO7_bracketleft		 91	*/
    WIN1253_backslash,		/*  ISO7_backslash		 92	*/
    WIN1253_bracketright,	/*  ISO7_bracketright		 93	*/
    WIN1253_asciicircum,	/*  ISO7_asciicircum		 94	*/
    WIN1253_underscore,		/*  ISO7_underscore		 95	*/
    WIN1253_grave,		/*  ISO7_grave			 96	*/
    WIN1253_a,			/*  ISO7_a			 97	*/
    WIN1253_b,			/*  ISO7_b			 98	*/
    WIN1253_c,			/*  ISO7_c			 99	*/
    WIN1253_d,			/*  ISO7_d			100	*/
    WIN1253_e,			/*  ISO7_e			101	*/
    WIN1253_f,			/*  ISO7_f			102	*/
    WIN1253_g,			/*  ISO7_g			103	*/
    WIN1253_h,			/*  ISO7_h			104	*/
    WIN1253_i,			/*  ISO7_i			105	*/
    WIN1253_j,			/*  ISO7_j			106	*/
    WIN1253_k,			/*  ISO7_k			107	*/
    WIN1253_l,			/*  ISO7_l			108	*/
    WIN1253_m,			/*  ISO7_m			109	*/
    WIN1253_n,			/*  ISO7_n			110	*/
    WIN1253_o,			/*  ISO7_o			111	*/
    WIN1253_p,			/*  ISO7_p			112	*/
    WIN1253_q,			/*  ISO7_q			113	*/
    WIN1253_r,			/*  ISO7_r			114	*/
    WIN1253_s,			/*  ISO7_s			115	*/
    WIN1253_t,			/*  ISO7_t			116	*/
    WIN1253_u,			/*  ISO7_u			117	*/
    WIN1253_v,			/*  ISO7_v			118	*/
    WIN1253_w,			/*  ISO7_w			119	*/
    WIN1253_x,			/*  ISO7_x			120	*/
    WIN1253_y,			/*  ISO7_y			121	*/
    WIN1253_z,			/*  ISO7_z			122	*/
    WIN1253_braceleft,		/*  ISO7_braceleft		123	*/
    WIN1253_bar,		/*  ISO7_bar			124	*/
    WIN1253_braceright,		/*  ISO7_braceright		125	*/
    WIN1253_asciitilde,		/*  ISO7_asciitilde		126	*/
    WIN1251_currency,		/*  .NOTDEF_127			127	*//**/
    WIN1251_currency,		/*  .NOTDEF_128			128	*//**/
    WIN1251_currency,		/*  .NOTDEF_129			129	*//**/
    WIN1251_currency,		/*  .NOTDEF_130			130	*//**/
    WIN1251_currency,		/*  .NOTDEF_131			131	*//**/
    WIN1251_currency,		/*  .NOTDEF_132			132	*//**/
    WIN1251_currency,		/*  .NOTDEF_133			133	*//**/
    WIN1251_currency,		/*  .NOTDEF_134			134	*//**/
    WIN1251_currency,		/*  .NOTDEF_135			135	*//**/
    WIN1251_currency,		/*  .NOTDEF_136			136	*//**/
    WIN1251_currency,		/*  .NOTDEF_137			137	*//**/
    WIN1251_currency,		/*  .NOTDEF_138			138	*//**/
    WIN1251_currency,		/*  .NOTDEF_139			139	*//**/
    WIN1251_currency,		/*  .NOTDEF_140			140	*//**/
    WIN1251_currency,		/*  .NOTDEF_141			141	*//**/
    WIN1251_currency,		/*  .NOTDEF_142			142	*//**/
    WIN1251_currency,		/*  .NOTDEF_143			143	*//**/
    WIN1251_currency,		/*  .NOTDEF_144			144	*//**/
    WIN1251_currency,		/*  .NOTDEF_145			145	*//**/
    WIN1251_currency,		/*  .NOTDEF_146			146	*//**/
    WIN1251_currency,		/*  .NOTDEF_147			147	*//**/
    WIN1251_currency,		/*  .NOTDEF_148			148	*//**/
    WIN1251_currency,		/*  .NOTDEF_149			149	*//**/
    WIN1251_currency,		/*  .NOTDEF_150			150	*//**/
    WIN1251_currency,		/*  .NOTDEF_151			151	*//**/
    WIN1251_currency,		/*  .NOTDEF_152			152	*//**/
    WIN1251_currency,		/*  .NOTDEF_153			153	*//**/
    WIN1251_currency,		/*  .NOTDEF_154			154	*//**/
    WIN1251_currency,		/*  .NOTDEF_155			155	*//**/
    WIN1251_currency,		/*  .NOTDEF_156			156	*//**/
    WIN1251_currency,		/*  .NOTDEF_157			157	*//**/
    WIN1251_currency,		/*  .NOTDEF_158			158	*//**/
    WIN1251_currency,		/*  .NOTDEF_159			159	*//**/
    WIN1253_nobreakspace,	/*  ISO7_space			160	*/
    WIN1253_grave,		/*  ISO7_afii64937		161	*//**/
    WIN1253_quotesingle,	/*  ISO7_afii57929		162	*//**/
    WIN1253_sterling,		/*  ISO7_sterling		163	*/
    WIN1251_currency,		/*  .NOTDEF_164			164	*//**/
    WIN1251_currency,		/*  .NOTDEF_165			165	*//**/
    WIN1253_brokenbar,		/*  ISO7_brokenbar		166	*/
    WIN1253_section,		/*  ISO7_section		167	*/
    WIN1253_dieresis,		/*  ISO7_dieresis		168	*/
    WIN1253_copyright,		/*  ISO7_copyright		169	*/
    WIN1251_currency,		/*  .NOTDEF_170			170	*//**/
    WIN1253_guillemotleft,	/*  ISO7_guillemotleft		171	*/
    WIN1253_logicalnot,		/*  ISO7_logicalnot		172	*/
    WIN1253_hyphen,		/*  ISO7_hyphen			173	*/
    WIN1251_currency,		/*  .NOTDEF_174			174	*//**/
    WIN1253_afii00208,		/*  ISO7_afii00208		175	*/
    WIN1253_degree,		/*  ISO7_degree			176	*/
    WIN1253_plusminus,		/*  ISO7_plusminus		177	*/
    WIN1253_twosuperior,	/*  ISO7_twosuperior		178	*/
    WIN1253_threesuperior,	/*  ISO7_threesuperior		179	*/
    WIN1253_tonos,		/*  ISO7_tonos			180	*/
    WIN1253_dieresistonos,	/*  ISO7_dieresistonos		181	*/
    WIN1253_Alphatonos,		/*  ISO7_Alphatonos		182	*/
    WIN1253_periodcentered,	/*  ISO7_periodcentered		183	*/
    WIN1253_Epsilontonos,	/*  ISO7_Epsilontonos		184	*/
    WIN1253_Etatonos,		/*  ISO7_Etatonos		185	*/
    WIN1253_Iotatonos,		/*  ISO7_Iotatonos		186	*/
    WIN1253_guillemotright,	/*  ISO7_guillemotright		187	*/
    WIN1253_Omicrontonos,	/*  ISO7_Omicrontonos		188	*/
    WIN1253_onehalf,		/*  ISO7_onehalf		189	*/
    WIN1253_Upsilontonos,	/*  ISO7_Upsilontonos		190	*/
    WIN1253_Omegatonos,		/*  ISO7_Omegatonos		191	*/
    WIN1253_iotadieresistonos,	/*  ISO7_iotadieresistonos	192	*/
    WIN1253_Alpha,		/*  ISO7_Alpha			193	*/
    WIN1253_Beta,		/*  ISO7_Beta			194	*/
    WIN1253_Gamma,		/*  ISO7_Gamma			195	*/
    WIN1253_Delta,		/*  ISO7_Delta			196	*/
    WIN1253_Epsilon,		/*  ISO7_Epsilon		197	*/
    WIN1253_Zeta,		/*  ISO7_Zeta			198	*/
    WIN1253_Eta,		/*  ISO7_Eta			199	*/
    WIN1253_Theta,		/*  ISO7_Theta			200	*/
    WIN1253_Iota,		/*  ISO7_Iota			201	*/
    WIN1253_Kappa,		/*  ISO7_Kappa			202	*/
    WIN1253_Lambda,		/*  ISO7_Lambda			203	*/
    WIN1253_Mu,			/*  ISO7_Mu			204	*/
    WIN1253_Nu,			/*  ISO7_Nu			205	*/
    WIN1253_Xi,			/*  ISO7_Xi			206	*/
    WIN1253_Omicron,		/*  ISO7_Omicron		207	*/
    WIN1253_Pi,			/*  ISO7_Pi			208	*/
    WIN1253_Rho,		/*  ISO7_Rho			209	*/
    WIN1251_currency,		/*  .NOTDEF_210			210	*//**/
    WIN1253_Sigma,		/*  ISO7_Sigma			211	*/
    WIN1253_Tau,		/*  ISO7_Tau			212	*/
    WIN1253_Upsilon,		/*  ISO7_Upsilon		213	*/
    WIN1253_Phi,		/*  ISO7_Phi			214	*/
    WIN1253_Chi,		/*  ISO7_Chi			215	*/
    WIN1253_Psi,		/*  ISO7_Psi			216	*/
    WIN1253_Omega,		/*  ISO7_Omega			217	*/
    WIN1253_Iotadieresis,	/*  ISO7_Iotadieresis		218	*/
    WIN1253_Upsilondieresis,	/*  ISO7_Upsilondieresis	219	*/
    WIN1253_alphatonos,		/*  ISO7_alphatonos		220	*/
    WIN1253_epsilontonos,	/*  ISO7_epsilontonos		221	*/
    WIN1253_etatonos,		/*  ISO7_etatonos		222	*/
    WIN1253_iotatonos,		/*  ISO7_iotatonos		223	*/
    WIN1253_upsilondieresistonos, /*  ISO7_upsilondieresistonos 224	*/
    WIN1253_alpha,		/*  ISO7_alpha			225	*/
    WIN1253_beta,		/*  ISO7_beta			226	*/
    WIN1253_gamma,		/*  ISO7_gamma			227	*/
    WIN1253_delta,		/*  ISO7_delta			228	*/
    WIN1253_epsilon,		/*  ISO7_epsilon		229	*/
    WIN1253_zeta,		/*  ISO7_zeta			230	*/
    WIN1253_eta,		/*  ISO7_eta			231	*/
    WIN1253_theta,		/*  ISO7_theta			232	*/
    WIN1253_iota,		/*  ISO7_iota			233	*/
    WIN1253_kappa,		/*  ISO7_kappa			234	*/
    WIN1253_lambda,		/*  ISO7_lambda			235	*/
    WIN1253_mu,			/*  ISO7_mu			236	*/
    WIN1253_nu,			/*  ISO7_nu			237	*/
    WIN1253_xi,			/*  ISO7_xi			238	*/
    WIN1253_omicron,		/*  ISO7_omicron		239	*/
    WIN1253_pi,			/*  ISO7_pi			240	*/
    WIN1253_rho,		/*  ISO7_rho			241	*/
    WIN1253_sigma1,		/*  ISO7_sigma1			242	*/
    WIN1253_sigma,		/*  ISO7_sigma			243	*/
    WIN1253_tau,		/*  ISO7_tau			244	*/
    WIN1253_upsilon,		/*  ISO7_upsilon		245	*/
    WIN1253_phi,		/*  ISO7_phi			246	*/
    WIN1253_chi,		/*  ISO7_chi			247	*/
    WIN1253_psi,		/*  ISO7_psi			248	*/
    WIN1253_omega,		/*  ISO7_omega			249	*/
    WIN1253_iotadieresis,	/*  ISO7_iotadieresis		250	*/
    WIN1253_upsilondieresis,	/*  ISO7_upsilondieresis	251	*/
    WIN1253_omicrontonos,	/*  ISO7_omicrontonos		252	*/
    WIN1253_upsilontonos,	/*  ISO7_upsilontonos		253	*/
    WIN1253_omegatonos,		/*  ISO7_omegatonos		254	*/
    0xff,
    };

const unsigned char docWIN1253_to_ISO7[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    ISO7_space,			/*  WIN1253_space		 32	*/
    ISO7_exclam,		/*  WIN1253_exclam		 33	*/
    ISO7_quotedbl,		/*  WIN1253_quotedbl		 34	*/
    ISO7_numbersign,		/*  WIN1253_numbersign		 35	*/
    ISO7_dollar,		/*  WIN1253_dollar		 36	*/
    ISO7_percent,		/*  WIN1253_percent		 37	*/
    ISO7_ampersand,		/*  WIN1253_ampersand		 38	*/
    ISO7_quotesingle,		/*  WIN1253_quotesingle		 39	*/
    ISO7_parenleft,		/*  WIN1253_parenleft		 40	*/
    ISO7_parenright,		/*  WIN1253_parenright		 41	*/
    ISO7_asterisk,		/*  WIN1253_asterisk		 42	*/
    ISO7_plus,			/*  WIN1253_plus		 43	*/
    ISO7_comma,			/*  WIN1253_comma		 44	*/
    ISO7_hyphen,		/*  WIN1253_hyphen		 45	*/
    ISO7_period,		/*  WIN1253_period		 46	*/
    ISO7_slash,			/*  WIN1253_slash		 47	*/
    ISO7_zero,			/*  WIN1253_zero		 48	*/
    ISO7_one,			/*  WIN1253_one			 49	*/
    ISO7_two,			/*  WIN1253_two			 50	*/
    ISO7_three,			/*  WIN1253_three		 51	*/
    ISO7_four,			/*  WIN1253_four		 52	*/
    ISO7_five,			/*  WIN1253_five		 53	*/
    ISO7_six,			/*  WIN1253_six			 54	*/
    ISO7_seven,			/*  WIN1253_seven		 55	*/
    ISO7_eight,			/*  WIN1253_eight		 56	*/
    ISO7_nine,			/*  WIN1253_nine		 57	*/
    ISO7_colon,			/*  WIN1253_colon		 58	*/
    ISO7_semicolon,		/*  WIN1253_semicolon		 59	*/
    ISO7_less,			/*  WIN1253_less		 60	*/
    ISO7_equal,			/*  WIN1253_equal		 61	*/
    ISO7_greater,		/*  WIN1253_greater		 62	*/
    ISO7_question,		/*  WIN1253_question		 63	*/
    ISO7_at,			/*  WIN1253_at			 64	*/
    ISO7_A,			/*  WIN1253_A			 65	*/
    ISO7_B,			/*  WIN1253_B			 66	*/
    ISO7_C,			/*  WIN1253_C			 67	*/
    ISO7_D,			/*  WIN1253_D			 68	*/
    ISO7_E,			/*  WIN1253_E			 69	*/
    ISO7_F,			/*  WIN1253_F			 70	*/
    ISO7_G,			/*  WIN1253_G			 71	*/
    ISO7_H,			/*  WIN1253_H			 72	*/
    ISO7_I,			/*  WIN1253_I			 73	*/
    ISO7_J,			/*  WIN1253_J			 74	*/
    ISO7_K,			/*  WIN1253_K			 75	*/
    ISO7_L,			/*  WIN1253_L			 76	*/
    ISO7_M,			/*  WIN1253_M			 77	*/
    ISO7_N,			/*  WIN1253_N			 78	*/
    ISO7_O,			/*  WIN1253_O			 79	*/
    ISO7_P,			/*  WIN1253_P			 80	*/
    ISO7_Q,			/*  WIN1253_Q			 81	*/
    ISO7_R,			/*  WIN1253_R			 82	*/
    ISO7_S,			/*  WIN1253_S			 83	*/
    ISO7_T,			/*  WIN1253_T			 84	*/
    ISO7_U,			/*  WIN1253_U			 85	*/
    ISO7_V,			/*  WIN1253_V			 86	*/
    ISO7_W,			/*  WIN1253_W			 87	*/
    ISO7_X,			/*  WIN1253_X			 88	*/
    ISO7_Y,			/*  WIN1253_Y			 89	*/
    ISO7_Z,			/*  WIN1253_Z			 90	*/
    ISO7_bracketleft,		/*  WIN1253_bracketleft		 91	*/
    ISO7_backslash,		/*  WIN1253_backslash		 92	*/
    ISO7_bracketright,		/*  WIN1253_bracketright	 93	*/
    ISO7_asciicircum,		/*  WIN1253_asciicircum		 94	*/
    ISO7_underscore,		/*  WIN1253_underscore		 95	*/
    ISO7_grave,			/*  WIN1253_grave		 96	*/
    ISO7_a,			/*  WIN1253_a			 97	*/
    ISO7_b,			/*  WIN1253_b			 98	*/
    ISO7_c,			/*  WIN1253_c			 99	*/
    ISO7_d,			/*  WIN1253_d			100	*/
    ISO7_e,			/*  WIN1253_e			101	*/
    ISO7_f,			/*  WIN1253_f			102	*/
    ISO7_g,			/*  WIN1253_g			103	*/
    ISO7_h,			/*  WIN1253_h			104	*/
    ISO7_i,			/*  WIN1253_i			105	*/
    ISO7_j,			/*  WIN1253_j			106	*/
    ISO7_k,			/*  WIN1253_k			107	*/
    ISO7_l,			/*  WIN1253_l			108	*/
    ISO7_m,			/*  WIN1253_m			109	*/
    ISO7_n,			/*  WIN1253_n			110	*/
    ISO7_o,			/*  WIN1253_o			111	*/
    ISO7_p,			/*  WIN1253_p			112	*/
    ISO7_q,			/*  WIN1253_q			113	*/
    ISO7_r,			/*  WIN1253_r			114	*/
    ISO7_s,			/*  WIN1253_s			115	*/
    ISO7_t,			/*  WIN1253_t			116	*/
    ISO7_u,			/*  WIN1253_u			117	*/
    ISO7_v,			/*  WIN1253_v			118	*/
    ISO7_w,			/*  WIN1253_w			119	*/
    ISO7_x,			/*  WIN1253_x			120	*/
    ISO7_y,			/*  WIN1253_y			121	*/
    ISO7_z,			/*  WIN1253_z			122	*/
    ISO7_braceleft,		/*  WIN1253_braceleft		123	*/
    ISO7_bar,			/*  WIN1253_bar			124	*/
    ISO7_braceright,		/*  WIN1253_braceright		125	*/
    ISO7_asciitilde,		/*  WIN1253_asciitilde		126	*/
    ISO7_copyright,		/*  WIN1253__0x7F		127	*//**/
    ISO7_copyright,		/*  WIN1253__0x80		128	*//**/
    ISO7_copyright,		/*  WIN1253__0x81		129	*//**/
    ISO7_quotesingle,		/*  WIN1253_quotesinglbase	130	*//**/
    ISO7_f,			/*  WIN1253_florin		131	*//**/
    ISO7_quotedbl,		/*  WIN1253_quotedblbase	132	*//**/
    ISO7_copyright,		/*  WIN1253_ellipsis		133	*//**/
    ISO7_copyright,		/*  WIN1253_dagger		134	*//**/
    ISO7_copyright,		/*  WIN1253_daggerdbl		135	*//**/
    ISO7_copyright,		/*  WIN1253__0x88		136	*//**/
    ISO7_copyright,		/*  WIN1253_perthousand		137	*//**/
    ISO7_copyright,		/*  WIN1253__0x8A		138	*//**/
    ISO7_guillemotleft,		/*  WIN1253_guilsinglleft	139	*//**/
    ISO7_copyright,		/*  WIN1253__0x8C		140	*//**/
    ISO7_copyright,		/*  WIN1253__0x8D		141	*//**/
    ISO7_copyright,		/*  WIN1253__0x8E		142	*//**/
    ISO7_copyright,		/*  WIN1253__0x8F		143	*//**/
    ISO7_copyright,		/*  WIN1253__0x90		144	*//**/
    ISO7_quotesingle,		/*  WIN1253_quoteleft		145	*//**/
    ISO7_quotesingle,		/*  WIN1253_quoteright		146	*//**/
    ISO7_quotedbl,		/*  WIN1253_quotedblleft	147	*//**/
    ISO7_quotedbl,		/*  WIN1253_quotedblright	148	*//**/
    ISO7_copyright,		/*  WIN1253_bullet		149	*//**/
    ISO7_minus,			/*  WIN1253_endash		150	*//**/
    ISO7_minus,			/*  WIN1253_emdash		151	*//**/
    ISO7_copyright,		/*  WIN1253__0x98		152	*//**/
    ISO7_copyright,		/*  WIN1253_trademark		153	*//**/
    ISO7_copyright,		/*  WIN1253__0x9A		154	*//**/
    ISO7_guillemotright,	/*  WIN1253_guilsinglright	155	*//**/
    ISO7_copyright,		/*  WIN1253__0x9C		156	*//**/
    ISO7_copyright,		/*  WIN1253__0x9D		157	*//**/
    ISO7_copyright,		/*  WIN1253__0x9E		158	*//**/
    ISO7_copyright,		/*  WIN1253__0x9F		159	*//**/
    ISO7_nobreakspace,		/*  WIN1253_nobreakspace	160	*/
    ISO7_dieresistonos,		/*  WIN1253_dieresistonos	161	*/
    ISO7_Alphatonos,		/*  WIN1253_Alphatonos		162	*/
    ISO7_sterling,		/*  WIN1253_sterling		163	*/
    ISO7_copyright,		/*  WIN1253_currency		164	*//**/
    ISO7_copyright,		/*  WIN1253_yen			165	*//**/
    ISO7_brokenbar,		/*  WIN1253_brokenbar		166	*/
    ISO7_section,		/*  WIN1253_section		167	*/
    ISO7_dieresis,		/*  WIN1253_dieresis		168	*/
    ISO7_copyright,		/*  WIN1253_copyright		169	*/
    ISO7_copyright,		/*  WIN1253__0xAA		170	*//**/
    ISO7_guillemotleft,		/*  WIN1253_guillemotleft	171	*/
    ISO7_logicalnot,		/*  WIN1253_logicalnot		172	*/
    ISO7_hyphen,		/*  WIN1253_hyphen		173	*/
    ISO7_copyright,		/*  WIN1253_registered		174	*//**/
    ISO7_afii00208,		/*  WIN1253_afii00208		175	*/
    ISO7_degree,		/*  WIN1253_degree		176	*/
    ISO7_plusminus,		/*  WIN1253_plusminus		177	*/
    ISO7_twosuperior,		/*  WIN1253_twosuperior		178	*/
    ISO7_threesuperior,		/*  WIN1253_threesuperior	179	*/
    ISO7_tonos,			/*  WIN1253_tonos		180	*/
    ISO7_mu,			/*  WIN1253_mu			181	*/
    ISO7_copyright,		/*  WIN1253_paragraph		182	*//**/
    ISO7_periodcentered,	/*  WIN1253_periodcentered	183	*/
    ISO7_Epsilontonos,		/*  WIN1253_Epsilontonos	184	*/
    ISO7_Etatonos,		/*  WIN1253_Etatonos		185	*/
    ISO7_Iotatonos,		/*  WIN1253_Iotatonos		186	*/
    ISO7_guillemotright,	/*  WIN1253_guillemotright	187	*/
    ISO7_Omicrontonos,		/*  WIN1253_Omicrontonos	188	*/
    ISO7_onehalf,		/*  WIN1253_onehalf		189	*/
    ISO7_Upsilontonos,		/*  WIN1253_Upsilontonos	190	*/
    ISO7_Omegatonos,		/*  WIN1253_Omegatonos		191	*/
    ISO7_iotadieresistonos,	/*  WIN1253_iotadieresistonos	192	*/
    ISO7_Alpha,			/*  WIN1253_Alpha		193	*/
    ISO7_Beta,			/*  WIN1253_Beta		194	*/
    ISO7_Gamma,			/*  WIN1253_Gamma		195	*/
    ISO7_Delta,			/*  WIN1253_Delta		196	*/
    ISO7_Epsilon,		/*  WIN1253_Epsilon		197	*/
    ISO7_Zeta,			/*  WIN1253_Zeta		198	*/
    ISO7_Eta,			/*  WIN1253_Eta			199	*/
    ISO7_Theta,			/*  WIN1253_Theta		200	*/
    ISO7_Iota,			/*  WIN1253_Iota		201	*/
    ISO7_Kappa,			/*  WIN1253_Kappa		202	*/
    ISO7_Lambda,		/*  WIN1253_Lambda		203	*/
    ISO7_Mu,			/*  WIN1253_Mu			204	*/
    ISO7_Nu,			/*  WIN1253_Nu			205	*/
    ISO7_Xi,			/*  WIN1253_Xi			206	*/
    ISO7_Omicron,		/*  WIN1253_Omicron		207	*/
    ISO7_Pi,			/*  WIN1253_Pi			208	*/
    ISO7_Rho,			/*  WIN1253_Rho			209	*/
    ISO7_copyright,		/*  WIN1253__0xD2		210	*//**/
    ISO7_Sigma,			/*  WIN1253_Sigma		211	*/
    ISO7_Tau,			/*  WIN1253_Tau			212	*/
    ISO7_Upsilon,		/*  WIN1253_Upsilon		213	*/
    ISO7_Phi,			/*  WIN1253_Phi			214	*/
    ISO7_Chi,			/*  WIN1253_Chi			215	*/
    ISO7_Psi,			/*  WIN1253_Psi			216	*/
    ISO7_Omega,			/*  WIN1253_Omega		217	*/
    ISO7_Iotadieresis,		/*  WIN1253_Iotadieresis	218	*/
    ISO7_Upsilondieresis,	/*  WIN1253_Upsilondieresis	219	*/
    ISO7_alphatonos,		/*  WIN1253_alphatonos		220	*/
    ISO7_epsilontonos,		/*  WIN1253_epsilontonos	221	*/
    ISO7_etatonos,		/*  WIN1253_etatonos		222	*/
    ISO7_iotatonos,		/*  WIN1253_iotatonos		223	*/
    ISO7_upsilondieresistonos,	/*  WIN1253_upsilondieresistonos 224	*/
    ISO7_alpha,			/*  WIN1253_alpha		225	*/
    ISO7_beta,			/*  WIN1253_beta		226	*/
    ISO7_gamma,			/*  WIN1253_gamma		227	*/
    ISO7_delta,			/*  WIN1253_delta		228	*/
    ISO7_epsilon,		/*  WIN1253_epsilon		229	*/
    ISO7_zeta,			/*  WIN1253_zeta		230	*/
    ISO7_eta,			/*  WIN1253_eta			231	*/
    ISO7_theta,			/*  WIN1253_theta		232	*/
    ISO7_iota,			/*  WIN1253_iota		233	*/
    ISO7_kappa,			/*  WIN1253_kappa		234	*/
    ISO7_lambda,		/*  WIN1253_lambda		235	*/
    ISO7_mu,			/*  WIN1253_mu			236	*/
    ISO7_nu,			/*  WIN1253_nu			237	*/
    ISO7_xi,			/*  WIN1253_xi			238	*/
    ISO7_omicron,		/*  WIN1253_omicron		239	*/
    ISO7_pi,			/*  WIN1253_pi			240	*/
    ISO7_rho,			/*  WIN1253_rho			241	*/
    ISO7_sigma1,		/*  WIN1253_sigma1		242	*/
    ISO7_sigma,			/*  WIN1253_sigma		243	*/
    ISO7_tau,			/*  WIN1253_tau			244	*/
    ISO7_upsilon,		/*  WIN1253_upsilon		245	*/
    ISO7_phi,			/*  WIN1253_phi			246	*/
    ISO7_chi,			/*  WIN1253_chi			247	*/
    ISO7_psi,			/*  WIN1253_psi			248	*/
    ISO7_omega,			/*  WIN1253_omega		249	*/
    ISO7_iotadieresis,		/*  WIN1253_iotadieresis	250	*/
    ISO7_upsilondieresis,	/*  WIN1253_upsilondieresis	251	*/
    ISO7_omicrontonos,		/*  WIN1253_omicrontonos	252	*/
    ISO7_upsilontonos,		/*  WIN1253_upsilontonos	253	*/
    ISO7_omegatonos,		/*  WIN1253_omegatonos		254	*/
    ISO7_copyright,		/*  WIN1253__0xFF		255	*/
    };

