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
#   include	<docTextParticule.h>
#   include	<ucdBidiClass.h>

#   include	"docTreeNode.h"
#   include	"docParaString.h"
#   include	"docParaBuilder.h"
#   include	"docParaBuilderImpl.h"
#   include	"docParaParticuleAdmin.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

#   define	DEB_PARTICULES	0

/************************************************************************/
/*									*/
/*  Delimit a single particule.						*/
/*									*/
/************************************************************************/

static int docDelimitParticule( struct ParagraphBuilder *	pb,
				const char *			from )
    {
    int			done= 0;
    int			res= 0;
    int			bidiClass;

    /*  Visible text  */
    while( pb->pbSubstitutionCurrStroff < pb->pbSubstitutionTailStroff )
	{
	unsigned short	symbol;
	int step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	bidiClass= ucdBidiClass( symbol );
	if  ( pb->pbSubstitutionCurrStroff >= pb->pbSubstitutionHeadStroff )
	    {
	    res= docParagraphBuilderHandleBidiClass( pb, bidiClass );
	    if  ( res < 0 )
		{ LDEB(res); return -1;	}
	    }
	else{ res= 0;	}

	/* space */
	/*
	if  ( ucdIsZ( symbol ) )
	    { break;	}
	*/
	if  ( bidiClass == UCDbidi_WS )
	    { break;	}

	pb->pbSubstitutionCurrStroff += step;
	from += step; done += step;
	}

    /*  Space  */
    while( pb->pbSubstitutionCurrStroff < pb->pbSubstitutionTailStroff )
	{
	unsigned short	symbol;
	int step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	bidiClass= ucdBidiClass( symbol );
	if  ( pb->pbSubstitutionCurrStroff >= pb->pbSubstitutionHeadStroff )
	    {
	    res= docParagraphBuilderHandleBidiClass( pb, bidiClass );
	    if  ( res < 0 )
		{ LDEB(res); return -1;	}
	    }
	else{ res= 0;	}

	/* no space */
	/*
	if  ( ! ucdIsZ( symbol ) )
	    { break;	}
	*/
	if  ( bidiClass != UCDbidi_WS )
	    { break;	}

	pb->pbSubstitutionCurrStroff += step;
	from += step; done += step;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Build a text particule.						*/
/*									*/
/*  1)	BIDI processing can discover a change in implicit level after	*/
/*	particules are closed. Cope with the unfortunate situation here.*/
/*  2)  NOTE that the text particules in the paragraph are in an	*/
/*	ideterminate state. We cannot use binary search. Scan back for	*/
/*	the particule to split.						*/
/*									*/
/************************************************************************/

int docParaBuilderAllocateTextParticule( ParagraphBuilder *	pb,
					int			upto )
    {
    struct BufferItem *	paraNode= pb->pbParaNode;

    /* 1 */
    if  ( upto < pb->pbSubstitutionPartStroff )
	{
	int	part;

	part= pb->pbSubstitutionCurrParticule- 1;
	while( part >= 0 )
	    {
	    TextParticule *	tp= paraNode->biParaParticules+ part;

	    if  ( upto == tp->tpStroff )
		{ return 0;	}
	    if  ( upto == tp->tpStroff+ tp->tpStrlen )
		{ return 0;	}

	    if  ( upto > tp->tpStroff && upto < tp->tpStroff+ tp->tpStrlen )
		{
		if  ( docSplitTextParticule(
				    (TextParticule **)0, (TextParticule **)0,
				    paraNode, part, upto ) )
		    { LLDEB(part,upto); return -1;	}

		pb->pbSubstitutionCurrParticule++;
		pb->pbSubstitutionTailParticule++;
		return 0;
		}

	    part--;
	    }

	LDEB(upto); return -1;
	}

    if  ( upto > pb->pbSubstitutionPartStroff )
	{
#	if DEB_PARTICULES
	const char *	label= "?-?";
#	endif


	if  ( pb->pbSubstitutionCurrParticule <
					pb->pbSubstitutionTailParticule )
	    {
	    TextParticule *	tp;

	    tp= paraNode->biParaParticules+ pb->pbSubstitutionCurrParticule;

	    tp->tpKind= TPkindSPAN;
	    tp->tpStroff= pb->pbSubstitutionPartStroff;
	    tp->tpStrlen= upto- pb->pbSubstitutionPartStroff;
	    tp->tpTextAttrNr= pb->pbSubstitutionTextAttrNr;

	    tp->tpWide= 0;

#	    if DEB_PARTICULES
	    label= "NW.";
#	    endif
	    }
	else{
	    TextParticule *	tp;

	    tp= docInsertTextParticule( paraNode,
		    pb->pbSubstitutionCurrParticule,
		    pb->pbSubstitutionPartStroff,
		    upto- pb->pbSubstitutionPartStroff,
		    TPkindSPAN, pb->pbSubstitutionTextAttrNr );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}

#	    if DEB_PARTICULES
	    label= "NW+";
#	    endif
	    }

#	if DEB_PARTICULES
	{
	const TextParticule *	tp;

	tp= paraNode->biParaParticules+ pb->pbSubstitutionCurrParticule;

	appDebug( "%s %3d: [%4d..%4d] %s \"%.*s\" done= %d\n",
		    label,  pb->pbSubstitutionCurrParticule,
		    tp->tpStroff,
		    tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ),
		    (int)tp->tpStrlen,
		    docParaString( paraNode, tp->tpStroff ),
		    tp->tpStrlen );
	}
#	endif

	pb->pbSubstitutionPartStroff= upto;
	pb->pbSubstitutionCurrParticule++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redivide a piece of a paragraph in particules.			*/
/*									*/
/************************************************************************/

int docRedivideStringInParticules(	ParagraphBuilder *	pb )
    {
    struct BufferItem *	paraNode= pb->pbParaNode;

    while( pb->pbSubstitutionCurrStroff < pb->pbSubstitutionTailStroff )
	{
	int		done;

	done= docDelimitParticule( pb,
				(const char *)paraNode->biParaString+
				pb->pbSubstitutionCurrStroff );
	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	if  ( docParaBuilderAllocateTextParticule( pb,
						pb->pbSubstitutionCurrStroff ) )
	    { LDEB(1); return -1;	}
	}

    if  ( pb->pbSubstitutionCurrParticule < pb->pbSubstitutionTailParticule )
	{
	docDeleteParticules( paraNode, pb->pbSubstitutionCurrParticule,
					pb->pbSubstitutionTailParticule-
					pb->pbSubstitutionCurrParticule );
	}

    return pb->pbSubstitutionCurrParticule- pb->pbSubstitutionHeadParticule;
    }

/************************************************************************/
/*									*/
/*  Replace part of the string of a paragraph with a different string	*/
/*									*/
/************************************************************************/

int docParaStringReplace(		int *			pSizeShift,
					BufferItem *		paraNode,
					int			stroffBegin,
					int			stroffTail,
					const char *		addedString,
					int			addedStrlen )
    {
    int		rval;

    rval= utilMemoryBufferReplaceBytes( &(paraNode->biParaStringBuffer),
			stroffBegin, stroffTail,
			(unsigned char *)addedString, addedStrlen );

    if  ( pSizeShift )
	{
	*pSizeShift= addedStrlen- stroffTail+ stroffBegin;
	}

    return rval;
    }

