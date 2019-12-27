#   include	"textEncodingConfig.h"

#   include	"ucdBidiClass.h"
#   include	"bidiScanner.h"
#   include	"bidiAlgo.h"
#   include	"bidiAlgoImplicit.h"
#   include	"bidiNoDebug.h"

#   include	<appDebugon.h>

static int bidiEmitImplicitLevel(	const BidiScanner *	bs,
					int			initiator,
					int			level,
					int			from,
					int			upto )
    {
    if  ( from < 0 || from >= upto )
	{ LLDEB(from,upto); return -1;	}

    switch( initiator )
	{
	case UCDbidi_L:
	case UCDbidi_R:
	case UCDbidi_ON:
	    if  ( bs->bsGotRun					&&
		  (*bs->bsGotRun)( bs->bsThrough,
					initiator, level, from, upto )	)
		{
		SLLLDEB(ucdBidiClassStr(initiator),level,from,upto);
		return -1;
		}
	    break;

	default:
	    CSDEB(initiator,ucdBidiClassStr(initiator)); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Implementation of the actions in the algorithm.			*/
/*									*/
/*  The actions are only driven by the state along the _run_. So we	*/
/*  never consult the actual input.					*/
/*  Two exceptions are made:						*/
/*  -   To leave positions that are controled by rule X9 alone.		*/
/*  -   For sanity checks: Refuse to perform actions that are against	*/
/*	the logic of the algorithm. (Only where checks are easy.)	*/
/*									*/
/************************************************************************/

void bidiInitImplicitState(	ImplicitState *	is )
    {
    is->is_I_initiator= UCDbidi_BN;
    is->is_I_level= UCDbidi_BN;

    is->is_W1_replacement= UCDbidi_EN;
    is->is_W2_replacement= UCDbidi_EN;

    is->is_W4_prevNum= UCDbidi_BN;
    is->is_W4_prevSep= UCDbidi_BN;
    is->is_W4_sepPos= -1;

    is->is_W5_prevNum= UCDbidi_BN;
    is->is_W5_termHead= -1;

    is->is_W7_replacement= UCDbidi_BN;
    is->is_W4_W7_replacement= UCDbidi_BN;
    is->is_W5_W7_replacement= UCDbidi_BN;

    is->is_N1_prevStrong= UCDbidi_BN;
    is->is_N1_neutralHead= -1;

    is->is_N2_e= UCDbidi_BN;

    is->is_WNI_head= -1;
    is->is_WNI_upto= -1;

    is->is_WNI_LevelHead= -1;

    is->is_WNI_CurrentInitiator= -1;
    is->is_WNI_CurrentLevel= -1;
    }

void bidiStartImplicitRun(	ImplicitState *		is,
				int			sos,
				int			iInitiator,
				int			xLevel )
    {
    /*TODO check*/
    is->is_I_initiator= iInitiator;
    is->is_I_level= xLevel;

    is->is_WNI_head= -1;
    is->is_WNI_upto= -1;

    is->is_W1_replacement= sos;
    is->is_W2_replacement= UCDbidi_EN;
    is->is_W4_prevNum= UCDbidi_BN;
    is->is_W4_prevSep= UCDbidi_BN;
    is->is_W4_sepPos= -1;
    is->is_W5_prevNum= UCDbidi_BN;
    is->is_W5_termHead= -1;
    is->is_W7_replacement= sos;
    is->is_W4_W7_replacement= is->is_W7_replacement;
    is->is_W5_W7_replacement= is->is_W7_replacement;
    is->is_N1_prevStrong= sos;
    is->is_N1_neutralHead= -1;

    /* No need to wait for ps_I_level: The values only differ in empty runs */
    is->is_N2_e= ( xLevel % 2 == 0 )?UCDbidi_L:UCDbidi_R;

    is->is_WNI_CurrentInitiator= is->is_I_initiator;

    return;
    }

static int bidi_WNI_FlushLevel(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				upto )
    {
    if  ( is->is_WNI_LevelHead < upto )
	{
	const int 	ci= is->is_WNI_CurrentInitiator;
	const int	cl= is->is_WNI_CurrentLevel;

	EMIT_DEB(ci,cl,is->is_WNI_LevelHead,upto);
	if  ( bidiEmitImplicitLevel( bs,
				    ci, cl, is->is_WNI_LevelHead, upto ) )
	    { LLDEB(is->is_WNI_LevelHead,upto); return -1;	}
	}

    is->is_WNI_LevelHead= -1;
    is->is_WNI_CurrentInitiator= -1;
    is->is_WNI_CurrentLevel= -1;

    return 0;
    }

static int bidi_WNI_SetLevel(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				initiator,
				int				level,
				int				offset )
    {
    if  ( initiator == UCDbidi_R && ( level % 2 ) == 0 )
	{ SLDEB(ucdBidiClassStr(initiator),level); return -1;	}
    if  ( initiator == UCDbidi_L && ( level % 2 ) != 0 )
	{ SLDEB(ucdBidiClassStr(initiator),level); return -1;	}

    if  ( is->is_WNI_LevelHead >= 0				&&
	  ( is->is_WNI_CurrentInitiator != initiator	||
	    is->is_WNI_CurrentLevel != level		)	&&
	  bidi_WNI_FlushLevel( bs, is, offset )			)
	{
	SLLDEB(ucdBidiClassStr(initiator),level,offset);
	LLDEB(is->is_WNI_LevelHead,offset);
	return -1;
	}

    if  ( is->is_WNI_LevelHead < 0 )
	{
	is->is_WNI_LevelHead= offset;
	is->is_WNI_CurrentInitiator= initiator;
	is->is_WNI_CurrentLevel= level;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  We got a strong symbol.						*/
/*									*/
/*  Now we can determine the direction of a potential run of neutrals	*/
/*  ending here. (Rules N1 and N2).					*/
/*									*/
/*  Also determine the level of the neutrals with rules I1 and I2.	*/
/*  Finally, flush any postponed explicit controls.			*/
/*									*/
/************************************************************************/

static int bidi_N1_N2_I_GotStrong(	const struct BidiScanner *	bs,
					ImplicitState *			is,
					int				strong,
					int				upto )
    {
    if  ( is->is_N1_neutralHead >= 0 )
	{
	int	offset= is->is_N1_neutralHead;
	int	replacement;
	int	neutralLevel;

	if  ( is->is_N1_prevStrong == strong )
	    { replacement= strong;	}
	else{ replacement= is->is_N2_e;	}

	if  ( is->is_I_initiator != UCDbidi_RLO	&&
	      is->is_I_initiator != UCDbidi_LRO	)
	    {
	    if  ( replacement == UCDbidi_L )
		{
		if  ( is->is_I_level % 2 != 0 )	/*  I2  */
		    { neutralLevel= is->is_I_level+ 1;	}
		else{ neutralLevel= is->is_I_level;	}
		}
	    else{
		if  ( replacement == UCDbidi_R )
		    {
		    if  ( is->is_I_level % 2 == 0 )	/*  I1  */
			{ neutralLevel= is->is_I_level+ 1;	}
		    else{ neutralLevel= is->is_I_level;	}
		    }
		else{ SDEB(ucdBidiClassStr(replacement)); return -1;	}
		}
	    }
	else{ neutralLevel= is->is_I_level;	}	/*  X4,X5  */

	while( offset < upto )
	    {
	    CLS_DEB("N*",offset,replacement);

	    if  ( bidi_WNI_SetLevel( bs, is,
				    replacement, neutralLevel, offset ) )
		{ LLDEB(offset,neutralLevel); return -1;	}
	    LEV_DEB("I*",offset,neutralLevel);

	    offset++;
	    }
	}

    is->is_N1_prevStrong= strong;
    is->is_N1_neutralHead= -1;

    return 0;
    }

static int bidi_N1_N2_I_GotNumber(	const struct BidiScanner *	bs,
					ImplicitState *			is,
					int				offset )
    {
    int		initiator;
    int		level;

    if  ( is->is_I_initiator != UCDbidi_RLO	&&
	  is->is_I_initiator != UCDbidi_LRO	)
	{
	if  ( is->is_I_level % 2 == 0 )
	    { level= is->is_I_level+ 2;	}	/* I1 */
	else{ level= is->is_I_level+ 1;	}	/* I2 */
	}
    else{ level= is->is_I_level;	}	/* X4,X5 */

    if  ( bidi_N1_N2_I_GotStrong( bs, is, UCDbidi_R, offset ) )
	{ LSDEB(offset,ucdBidiClassStr(UCDbidi_R)); return -1; }

    if  ( level % 2 )
	{ initiator= UCDbidi_R;	}
    else{ initiator= UCDbidi_L;	}

    if  ( bidi_WNI_SetLevel( bs, is, initiator, level, offset ) )
	{ LLDEB(offset,level); return -1;	}
    LEV_DEB("I*",offset,level);

    return 0;
    }

static int bidi_N1_N2_I_Got_L(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    int		level;

    if  ( is->is_I_level % 2 != 0		&&
	  is->is_I_initiator != UCDbidi_RLO	&&
	  is->is_I_initiator != UCDbidi_LRO	)
	{ level= is->is_I_level+ 1;	}	/* I2 */
    else{ level= is->is_I_level;	}

    if  ( bidi_N1_N2_I_GotStrong( bs, is, UCDbidi_L, offset ) )
	{ LSDEB(offset,ucdBidiClassStr(UCDbidi_L)); return -1; }

#   if 0
    if  ( level == ps->ps_I_level )
	{ initiator= UCDbidi_EMBEDDING;	}
    else{ initiator= UCDbidi_IMPLICIT;		}
#   endif

    if  ( bidi_WNI_SetLevel( bs, is, UCDbidi_L, level, offset ) )
	{ LLDEB(offset,level); return -1;	}
    LEV_DEB("I*",offset,level);

    return 0;
    }

static int bidi_N1_N2_I_Got_R(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    int		level;

    if  ( is->is_I_level % 2 == 0		&&
	  is->is_I_initiator != UCDbidi_RLO	&&
	  is->is_I_initiator != UCDbidi_LRO	)
	{ level= is->is_I_level+ 1;	}	/* I1 */
    else{ level= is->is_I_level;	}

    if  ( bidi_N1_N2_I_GotStrong( bs, is, UCDbidi_R, offset ) )
	{ LSDEB(offset,ucdBidiClassStr(UCDbidi_R)); return -1; }

    if  ( bidi_WNI_SetLevel( bs, is, UCDbidi_R, level, offset ) )
	{ LLDEB(offset,level); return -1;	}
    LEV_DEB("I*",offset,level);

    return 0;
    }

static int bidi_N1_N2_I_GotNeutral(	ImplicitState *		is,
					int			offset )
    {
    if  ( offset < is->is_N1_neutralHead )
	{ LLDEB(offset,is->is_N1_neutralHead); return -1;	}

    if  ( is->is_N1_neutralHead < 0 )
	{ is->is_N1_neutralHead= offset;	}

    return 0;
    }

static int bidi_W4_reset(	ImplicitState *			is )
    {
    int			offset;

    offset= is->is_W4_sepPos;
    if  ( offset >= 0 )
	{
	CLS_DEB("W6",offset,UCDbidi_ON);

	if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
	    { LDEB(offset); return -1;	}
	}

    is->is_W4_prevNum= UCDbidi_BN;
    is->is_W4_prevSep= UCDbidi_BN;
    is->is_W4_sepPos= -1;

    return 0;
    }

static int bidi_W4_Replace(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				unsigned char			replacement )
    {
    int			offset= is->is_W4_sepPos;

    CLS_DEB("W4",offset,replacement);

    if  ( replacement == UCDbidi_EN		&&
	  is->is_W4_W7_replacement == UCDbidi_L	)
	{
	CLS_DEB("W7",offset,UCDbidi_L);

	if  ( bidi_N1_N2_I_Got_L( bs, is, offset ) )
	    { LDEB(offset); return -1;	}
	}
    else{
	if  ( bidi_N1_N2_I_GotNumber( bs, is, offset ) )
	    { LDEB(offset); return -1;	}
	}

    is->is_W4_sepPos= -1;

    return 0;
    }

static int bidi_W5_W6_Replace(	ImplicitState *			is,
				int				upto )
    {
    int			offset= is->is_W5_termHead;

    offset= is->is_W5_termHead;
    if  ( offset >= 0 )
	{
	while( offset < upto )
	    {
	    CLS_DEB("W6",offset,UCDbidi_ON);

	    if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
		{ LDEB(offset); return -1;	}

	    offset++;
	    }
	}

    return 0;
    }

static int bidi_W5_reset(	ImplicitState *			is,
				int				offset )
    {
    if  ( bidi_W5_W6_Replace( is, offset ) )
	{ LDEB(1); return -1;	}

    is->is_W5_prevNum= UCDbidi_BN;
    is->is_W5_termHead= -1;

    return 0;
    }

static int bidi_W5_Replace(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				upto )
    {
    int			offset= is->is_W5_termHead;

    while( offset < upto )
	{
	CLS_DEB("W5",offset,UCDbidi_EN);

	if  ( is->is_W5_W7_replacement == UCDbidi_L )
	    {
	    CLS_DEB("W7",offset,UCDbidi_L);

	    if  ( bidi_N1_N2_I_Got_L( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    }
	else{
	    if  ( bidi_N1_N2_I_GotNumber( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    }

	offset++;
	}

    return 0;
    }

static int bidi_W4_W5_W7_Got_EN(	const struct BidiScanner *	bs,
					ImplicitState *			is,
					int				offset )
    {
    /*  W4  */
    if  ( is->is_W4_prevSep == UCDbidi_ES	&&
	  is->is_W4_prevNum == UCDbidi_EN	)
	{
	if  ( bidi_W4_Replace( bs, is, UCDbidi_EN ) )
	    { LLDEB(is->is_W4_sepPos,offset); return -1;	}
	}
    if  ( is->is_W4_prevSep == UCDbidi_CS		&&
	  is->is_W4_prevNum == UCDbidi_EN	)
	{
	if  ( bidi_W4_Replace( bs, is, UCDbidi_EN ) )
	    { LLDEB(is->is_W4_sepPos,offset); return -1;	}
	}

    if  ( bidi_W4_reset( is ) )
	{ LDEB(offset); return -1;	}

    is->is_W4_prevNum= UCDbidi_EN;
    is->is_W4_prevSep= UCDbidi_BN;
    is->is_W4_sepPos= -1;

    /*  W5  */
    if  ( is->is_W5_termHead >= 0 )
	{
	if  ( bidi_W5_Replace( bs, is, offset ) )
	    { LLDEB(is->is_W4_sepPos,offset); return -1;	}
	}

    is->is_W5_prevNum= UCDbidi_EN;
    is->is_W5_termHead= -1;

    /*  W7  */
    if  ( is->is_W7_replacement == UCDbidi_L )
	{
	CLS_DEB("W7",offset,UCDbidi_L);

	if  ( bidi_N1_N2_I_Got_L( bs, is, offset ) )
	    { LDEB(offset); return -1;	}
	}
    else{
	if  ( bidi_N1_N2_I_GotNumber( bs, is, offset ) )
	    { LDEB(offset); return -1; }
	}

    return 0;
    }

static int bidi_W4_W5_Got_AN(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    /*  W4  */
    if  ( is->is_W4_prevSep == UCDbidi_CS	&&
	  is->is_W4_prevNum == UCDbidi_AN	)
	{
	if  ( bidi_W4_Replace( bs, is, UCDbidi_AN ) )
	    { LLDEB(is->is_W4_sepPos,offset); return -1;	}
	}

    if  ( bidi_W4_reset( is ) )
	{ LDEB(offset); return -1;	}

    is->is_W4_prevNum= UCDbidi_AN;
    is->is_W4_prevSep= UCDbidi_BN;
    is->is_W4_sepPos= -1;

    /*  W5  */
    if  ( bidi_W5_reset( is, offset ) )
	{ LDEB(offset); return -1;	}

    if  ( bidi_N1_N2_I_GotNumber( bs, is, offset ) )
	{ LDEB(offset); return -1; }

    return 0;
    }

static int bidi_W4_Got_ES(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    if  ( is->is_W4_prevSep == UCDbidi_BN	&&
	  is->is_W4_prevNum == UCDbidi_EN	)
	{
	is->is_W4_prevSep= UCDbidi_ES;
	is->is_W4_sepPos= offset;
	is->is_W4_W7_replacement= is->is_W7_replacement;
	}
    else{
	if  ( bidi_W4_reset( is ) )
	    { LDEB(offset); return -1;	}
	if  ( bidi_W5_reset( is, offset ) )
	    { LDEB(offset); return -1;	}

	CLS_DEB("W6",offset,UCDbidi_ON);

	if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
	    { LDEB(offset); return -1;	}
	}

    return 0;
    }

static int bidi_W4_Got_CS(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    if  ( is->is_W4_prevSep == UCDbidi_BN		&&
	  ( is->is_W4_prevNum == UCDbidi_EN	||
	    is->is_W4_prevNum == UCDbidi_AN	)	)
	{
	is->is_W4_prevSep= UCDbidi_CS;
	is->is_W4_sepPos= offset;
	is->is_W4_W7_replacement= is->is_W7_replacement;
	}
    else{
	if  ( bidi_W4_reset( is ) )
	    { LDEB(offset); return -1;	}
	if  ( bidi_W5_reset( is, offset ) )
	    { LDEB(offset); return -1;	}

	CLS_DEB("W6",offset,UCDbidi_ON);

	if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
	    { LDEB(offset); return -1;	}
	}

    return 0;
    }

static int bidi_W5_Got_ET(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				offset )
    {
    if  ( is->is_W5_prevNum == UCDbidi_EN )
	{
	CLS_DEB("W5",offset,UCDbidi_EN);
	is->is_W5_termHead= -1;

	/*  W7  */
	if  ( is->is_W7_replacement == UCDbidi_L )
	    {
	    CLS_DEB("W7",offset,UCDbidi_L);

	    if  ( bidi_N1_N2_I_Got_L( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    }
	else{
	    if  ( bidi_N1_N2_I_GotNumber( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    }
	}
    else{
	if  ( is->is_W5_termHead < 0 )
	    {
	    is->is_W5_termHead= offset;
	    is->is_W5_W7_replacement= is->is_W7_replacement;
	    }
	}

    return 0;
    }

int bidiHandleImplicitClass(	const struct BidiScanner * bs,
				ImplicitState *		is,
				int			klass,
				int			offset )
    {
    switch( klass )
	{
	case UCDbidi_EN:
	    /*  W2  */
	    if  ( is->is_W2_replacement == UCDbidi_AN )
		{
		klass= UCDbidi_AN;
		CLS_DEB("W2",offset,klass);

		if  ( bidi_W4_W5_Got_AN( bs, is, offset ) )
		    { LDEB(offset); return -1;	}
		}
	    else{
		if  ( bidi_W4_W5_W7_Got_EN( bs, is, offset ) )
		    { LDEB(offset); return -1;	}
		}
	    break;

	case UCDbidi_ET:
	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_Got_ET( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    break;

	case UCDbidi_AN:
	    if  ( bidi_W4_W5_Got_AN( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    break;

	case UCDbidi_ES:
	    if  ( bidi_W4_Got_ES( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    break;

	case UCDbidi_CS:
	    if  ( bidi_W4_Got_CS( bs, is, offset ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    break;

	case UCDbidi_S:
	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    klass= UCDbidi_ON;
	    CLS_DEB("W6",offset,klass);

	    if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
		{ LDEB(offset); return -1;	}

	    break;

	case UCDbidi_AL:
	    /*  W2  */
	    is->is_W2_replacement= UCDbidi_AN;

	    /*  W3  */
	    klass= UCDbidi_R;
	    CLS_DEB("W3",offset,klass);

	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    is->is_W7_replacement= UCDbidi_R;

	    if  ( bidi_N1_N2_I_Got_R( bs, is, offset ) )
		{ LSDEB(offset,ucdBidiClassStr(klass)); return -1; }
	    break;

	case UCDbidi_L:
	    /*  W2  */
	    is->is_W2_replacement= UCDbidi_EN;

	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    is->is_W7_replacement= UCDbidi_L;

	    if  ( bidi_N1_N2_I_Got_L( bs, is, offset ) )
		{ LSDEB(offset,ucdBidiClassStr(klass)); return -1; }
	    break;

	case UCDbidi_R:
	    /*  W2  */
	    is->is_W2_replacement= UCDbidi_EN;

	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}
	    is->is_W7_replacement= UCDbidi_R;

	    if  ( bidi_N1_N2_I_Got_R( bs, is, offset ) )
		{ LSDEB(offset,ucdBidiClassStr(klass)); return -1; }
	    break;

	case UCDbidi_WS:
	case UCDbidi_ON:
	    if  ( bidi_W4_reset( is ) )
		{ LDEB(offset); return -1;	}
	    if  ( bidi_W5_reset( is, offset ) )
		{ LDEB(offset); return -1;	}

	    if  ( bidi_N1_N2_I_GotNeutral( is, offset ) )
		{ LDEB(offset); return -1;	}
	    break;

	/* sanity checks: X9 Retained */
	case UCDbidi_LRI:
	case UCDbidi_RLI:
	case UCDbidi_FSI:
	case UCDbidi_PDI:

	/* sanity checks: X9 Removed */
	case UCDbidi_RLE:
	case UCDbidi_LRE:
	case UCDbidi_RLO:
	case UCDbidi_LRO:
	case UCDbidi_PDF:
	case UCDbidi_BN:

	    return 0;

	default:
	    LSDEB(offset,ucdBidiClassStr(klass)); return 0;
	}

    return 0;
    }

/**
 *  Flush the W4, W5, W6 administration: We do not expect any 
 *  numerical symbols after the end of a segment, so now we know that 
 *  the postponed separators and terminators can be handled in 
 *  the default way.
 */
static int bidi_W4_W5_W6_Flush(	const struct BidiScanner *	bs,
				ImplicitState *			is,
				int				upto )
    {
    if  ( bidi_W4_reset( is ) )
	{ LDEB(is->is_W4_sepPos);	}

    if  ( bidi_W5_W6_Replace( is, upto ) )
	{ LDEB(upto); return -1;	}

    return 0;
    }

int bidiFlushRun(		const struct BidiScanner *	bs,
				struct ImplicitState *		is,
				int				eor,
				int				upto )
    {
    if  ( is->is_WNI_head < is->is_WNI_upto )
	{
	if  ( bidi_W4_W5_W6_Flush( bs, is, upto ) )
	    { LDEB(upto); return -1;	}

	if  ( bidi_N1_N2_I_GotStrong( bs, is, eor, upto ) )
	    { LDEB(upto); return -1;	}
	}

    if  ( is->is_WNI_LevelHead >= 0 && is->is_WNI_LevelHead < upto )
	{
	if  ( bidi_WNI_FlushLevel( bs, is, upto ) )
	    { LDEB(upto); return -1;	}
	}

    return 0;
    }
