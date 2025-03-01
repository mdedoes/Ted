/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout inside lies of text.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<math.h>

#   include	"docLayoutLine.h"
#   include	<docTabStop.h>
#   include	<docTabStop.h>
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	"docParticuleData.h"
#   include	<docSelect.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Justify the particules in a line of text.				*/
/*									*/
/*  1)  Start justification after the last tab of the line. Justifying	*/
/*	after anything else than a left tab is ridiculous. Simply	*/
/*	refuse to.							*/
/*  2)  Ignore organisational particules such as the delimitation of	*/
/*	links and bookmarks at the end of the line. The formatter did	*/
/*	allocate any space for them.					*/
/*									*/
/*  See http://www.tug.org/TUGboat/tb27-2/tb87benatia.pdf on Arabic	*/
/*  justification.							*/
/*  See http://rishida.net/blog/?p=1059					*/
/*  See http://quod.lib.umich.edu/j/jep/3336451.0013.105/--justify-just-or-just-justify?rgn=main;view=fulltext						*/
/*									*/
/************************************************************************/

static int docJustifyLineFindRange(
				int *				pFrom,
				int *				pUpto,
				const TextParticule *		tpFrom,
				const ParticuleData *		pdFrom,
				int				accepted )
    {
    int				from= 0;
    int				upto= accepted;

    int				part;
    const TextParticule *	tp;

    /*  1  */
    from= 0;
    tp= tpFrom;
    for ( part= 0; part < accepted- 1; tp++, part++ )
	{
	if  ( tp->tpKind == TPkindTAB )
	    { from= part+ 1;	}
	}

    if  ( from > 0 )
	{
	if  ( pdFrom[from-1].pdTabKind != DOCtaLEFT )
	    { LDEB(pdFrom[from-1].pdTabKind); return -1;	}
	}

    /*  2  */
    upto= accepted;
    while( upto > 0				&&
	   pdFrom[upto- 1].pdTwipsWide == 0	)
	{ upto--;	}

    if  ( upto- from < 2 )
	{ /* LDEB(upto); */ return 1;	}

    *pFrom= from;
    *pUpto= upto;
    return 0;
    }

static int docJustifyFindWhite(	const struct BufferItem *	paraNode,
				int				from,
				int				upto,
				const TextParticule *		tpFrom,
				ParticuleData *			pdFrom )
    {
    int				part;
    int				totalWhite= 0;

    const TextParticule *	tp= tpFrom+ from;
    ParticuleData *		pd= pdFrom+ from;

    for ( part= from; part < upto- 1; tp++, pd++, part++ )
	{
	pd->pdWhiteUnits= 0;
	pd->pdCorrectBy= 0;

	if  ( docParaPastLastNonBlank( paraNode, tp->tpStroff,
					    tp->tpStroff+ tp->tpStrlen ) <
					    tp->tpStroff+ tp->tpStrlen )
	    {
	    int		visibleWidth= pd[1].pdVisibleBBox.drX1;

	    pd->pdWhiteUnits=
		sqrt( (double)pd[0].pdTwipsWide+ (double)visibleWidth )+ 0.4999;

	    totalWhite += pd->pdWhiteUnits;
	    }
	}

    if  ( totalWhite == 0 )
	{ totalWhite= 1;	}

    return totalWhite;
    }

static int docDistributeProportionally(
				int				from,
				int				upto,
				const TextParticule *		tpFrom,
				ParticuleData *			pdFrom,
				int				extra,
				int				totalWhite )
    {
    int				part;
    int				left= extra;

    const TextParticule *	tp= tpFrom+ from;
    ParticuleData *		pd= pdFrom+ from;

    for ( part= from; part < upto- 1 && left > 0; tp++, pd++, part++ )
	{
	int	step;

	if  ( pd->pdWhiteUnits == 0 )
	    { continue;	}

	step= ( extra* pd->pdWhiteUnits )/ totalWhite;
	if  ( step > left )
	    { step= left;	}

	pd->pdCorrectBy += step;
	left -= step;
	}

    return left;
    }

static int docDistributeLeftOver(
				int				from,
				int				upto,
				const TextParticule *		tpFrom,
				ParticuleData *			pdFrom,
				int				extra )
    {
    int				part;
    int				left= extra;

    const TextParticule *	tp= tpFrom+ from;
    ParticuleData *		pd= pdFrom+ from;

    for ( part= from; part < upto- 1 && left > 0; tp++, pd++, part++ )
	{
	int	step;

	if  ( pd->pdWhiteUnits == 0 )
	    { continue;	}

	step= 1;
	if  ( step > left )
	    { step= left;	}

	pd->pdCorrectBy += step;
	left -= step;
	}

    return left;
    }

static int docExpandWidthsAndShiftOffsets(
				int				from,
				int				upto,
				const TextParticule *		tpFrom,
				ParticuleData *			pdFrom )
    {
    int				step= 0;
    int				part;

    const TextParticule *	tp= tpFrom+ from;
    ParticuleData *		pd= pdFrom+ from;

    for ( part= from; part < upto- 1; tp++, pd++, part++ )
	{
	pd->pdX0 += step;

	pd->pdTwipsWide += pd->pdCorrectBy;
	step += pd->pdCorrectBy;
	}

    pd->pdX0 += step;

    return step;
    }

void docJustifyParticules(	const struct BufferItem *	paraNode,
				const TextParticule *		tpFrom,
				ParticuleData *			pdFrom,
				int				accepted,
				const int			visibleX1,
				int				contentX1 )
    {
    int				res;
    int				from;
    int				upto;

    /*
    int				x0;
    */

    int				extra;
    int				totalWhite;

    int				left;

    /*  1,2  */
    res= docJustifyLineFindRange( &from, &upto, tpFrom, pdFrom, accepted );
    if  ( res < 0 )
	{ LDEB(res); return;	}
    if  ( res > 0 )
	{ return;		}

    /*
    x0= pdFrom[from].pdX0;
    */

    extra= contentX1- visibleX1;

    if  ( extra < 0 )
	{
	int			part;
	const ParticuleData *	pd= pdFrom;
	const TextParticule *	tp= tpFrom;

	LLLDEB(contentX1,visibleX1,extra);

	for ( part= 0; part < accepted; tp++, pd++, part++ )
	    {
	    appDebug( "%-4s: %5d..%5d \"%*.*s\"\n",
		    docKindStr( tp->tpKind ),
		    pd->pdX0, pd->pdX0+ pd->pdVisibleBBox.drX1,
		    tp->tpStrlen, tp->tpStrlen,
		    docParaString( paraNode, tp->tpStroff ) );
	    }

	extra= 0;
	return;
	}

    totalWhite= docJustifyFindWhite( paraNode, from, upto, tpFrom, pdFrom );

    left= docDistributeProportionally( from, upto, tpFrom, pdFrom,
							extra, totalWhite );

    left= docDistributeLeftOver( from, upto, tpFrom, pdFrom, left );

    /* step= */ docExpandWidthsAndShiftOffsets( from, upto, tpFrom, pdFrom );

    return;
    }

