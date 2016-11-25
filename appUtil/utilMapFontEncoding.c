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

