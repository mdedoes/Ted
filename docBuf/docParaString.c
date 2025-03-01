/************************************************************************/
/*									*/
/*  Manage the string of text bytes in a paragraph.			*/
/*									*/
/*  The text of a paragraph is a UTF-8 encoded string. Size and offsets	*/
/*  are in bytes. (NOT in characters unless the whole text is in US	*/
/*  ASCII)								*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>
#   include	<docTextParticule.h>

#   include	"docTreeNode.h"
#   include	"docParaString.h"
#   include	"docSelect.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

#   define	DEB_PARTICULES	0

/************************************************************************/

int docParaNextWord(	const struct BufferItem *	paraNode,
			int				stroff )
    {
    const char *	from= docParaString( paraNode, stroff );
    int			upto= docParaStrlen( paraNode );

    unsigned short	symbol;
    int			step;

    while( stroff < upto )
	{
	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LLDEB(stroff,step); return -1;	}

	if  ( ucdIsZ( symbol ) )
	    { break;	}

	from += step;
	stroff += step;
	}

    while( stroff < upto )
	{
	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LLDEB(stroff,step); return -1;	}

	if  ( ! ucdIsZ( symbol ) )
	    { break;	}

	from += step;
	stroff += step;
	}

    return stroff;
    }

int docParaPrevWord(	const struct BufferItem *	paraNode,
			int				stroff )
    {
    const char *	from= docParaString( paraNode, stroff );

    unsigned short	symbol;
    int			step;

    while( stroff > 0 )
	{
	int		st;
	const char *	fr;

	st= 1; fr= from- 1;
	while( stroff- st > 0 && UNI_UTF8_INTERN( *fr ) )
	    { st++; fr--; }

	step= uniGetUtf8( &symbol, fr );
	if  ( step < 1 )
	    { LLDEB(stroff-st,step); return -1;	}

	if  ( step != st )
	    { LLDEB(step,st);	}

	if  ( ! ucdIsZ( symbol ) )
	    { break;	}

	from -= step;
	stroff -= step;
	}

    while( stroff > 0 )
	{
	int		st;
	const char *	fr;

	st= 1; fr= from- 1;
	while( stroff- st > 0 && UNI_UTF8_INTERN( *fr ) )
	    { st++; fr--; }

	step= uniGetUtf8( &symbol, fr );
	if  ( step < 1 )
	    { LLDEB(stroff-st,step); return -1;	}

	if  ( step != st )
	    { LLDEB(step,st);	}

	if  ( ucdIsZ( symbol ) )
	    { break;	}

	from -= step;
	stroff -= step;
	}

    return stroff;
    }

int docParaHeadOfWord(		int *				pStroff,
				const struct BufferItem *	paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				stroff= tp->tpStroff;

    while( part > 0						&&
	   tp[-1].tpKind == TPkindSPAN				&&
	   docParaString( paraNode, tp->tpStroff- 1 )[0] != ' '	)
	{
	tp--; part--;
	stroff= tp->tpStroff;
	}

    *pStroff= stroff;
    return part;
    }

int docParaTailOfWord(		int *				pStroff,
				const struct BufferItem *	paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				stroff= tp->tpStroff+ tp->tpStrlen;

    while( part < paraNode->biParaParticuleCount- 1	&&
	   tp[1].tpKind == TPkindSPAN			&&
	   docParaString( paraNode, stroff- 1 )[0] != ' '	)
	{
	tp++; part++;
	stroff= tp->tpStroff+ tp->tpStrlen;
	}

    while( stroff > tp->tpStroff			&&
	   docParaString( paraNode, stroff- 1 )[0] == ' '	)
	{ stroff--; }

    *pStroff= stroff;
    return part;
    }

/************************************************************************/

int docParaPastLastNonBlank(	const struct BufferItem *	paraNode,
				int				from,
				int				upto )
    {
    while( upto > from					&&
	   paraNode->biParaString[upto-1] == ' '	)
	{ upto--;	}

    return upto;
    }

/************************************************************************/
/*									*/
/*  Return the next valid string offset in the paragraph.		*/
/*									*/
/*  Positions inside an UTF-8 sequence are invalid.			*/
/*  Offsets of administrative particules are perfectly acceptable here.	*/
/*									*/
/************************************************************************/

int docParaNextStroff(	const struct BufferItem *	paraNode,
			int				stroff )
    {
    const char *	from= docParaString( paraNode, stroff );

    unsigned short	symbol;
    int			step;

    step= uniGetUtf8( &symbol, from );
    if  ( step < 1 )
	{ LLDEB(stroff,step); return -1;	}

    return stroff+ step;
    }

/************************************************************************/
/*									*/
/*  Return the previous valid string offset in the paragraph.		*/
/*									*/
/*  Positions inside an UTF-8 sequence are invalid.			*/
/*  Offsets of administrative particules are perfectly acceptable here.	*/
/*									*/
/************************************************************************/

int docParaPrevStroff(	const struct BufferItem *	paraNode,
			int				stroff )
    {
    unsigned short		symbol;
    const char *		from;
    int				step;

    if  ( stroff <= 0 )
	{ LDEB(stroff); return -1;	}

    stroff--;
    from= docParaString( paraNode, stroff );

    while( UNI_UTF8_INTERN( *from ) )
	{
	if  ( stroff <= 0 )
	    { LDEB(stroff); return -1;	}

	stroff--; from--;
	}

    step= uniGetUtf8( &symbol, from );
    if  ( step < 1 )
	{ LLDEB(stroff,step); return -1;	}

    return stroff;
    }

