#   include	"ucdBidiClass.h"
#   include	"bidiAlgo.h"

#   include	<appDebugon.h>

/************************************************************************/

typedef struct LineState
    {
			/**
			 *  Embedding level of the paragraph as a whole.
			 */
    int			lsParaLevel;
			/**
			 *  The offset of the head of the current segment.
			 */
    int			ls_L1_SegmentHead;
			/**
			 *  The offset of the end of the current segment.
			 */
    int			ls_L1_SegmentUpto;
			/**
			 *  The offset of the white space in the current
			 *  segment.
			 */
    int			ls_L1_SpaceHead;
			/**
			 *  The state of the scanner:
			 *  Going forward:
			 *      BN If the scanner is not in White Space
			 *      WS If the scanner is in White Space
			 *  Going backward:
			 *      BN If the segment is at the end of the line
			 *      S If the segment is terminated by an S
			 */
    unsigned char	ls_L1_SegmentState;

    BidiGotSegmentBody	lsGotSegmentBody;
    BidiGotSegmentTail	lsGotSegmentTail;
    void *		lsThrough;
    } LineState;

/************************************************************************/

static void bidiStartLine(	LineState *		ls,
				int			paraInitiator,
				BidiGotSegmentBody	gotSegmentBody,
				BidiGotSegmentTail	gotSegmentTail,
				void *			through )
    {
    ls->lsParaLevel= 0;
    if  ( paraInitiator == UCDbidi_RLE || paraInitiator == UCDbidi_RLO )
	{ ls->lsParaLevel++;	}

    ls->ls_L1_SegmentHead= -1;
    ls->ls_L1_SegmentUpto= -1;
    ls->ls_L1_SpaceHead= -1;
    ls->ls_L1_SegmentState= UCDbidi_BN;

    ls->lsGotSegmentBody= gotSegmentBody;
    ls->lsGotSegmentTail= gotSegmentTail;
    ls->lsThrough= through;

    return;
    }


/************************************************************************/

static int bidiHandleLineClassesFwd(	LineState *		ls,
					const unsigned char *	classes,
					int			from,
					int			upto )
    {
    int		offset;
    const int	uptoSeparator= 1;

    for ( offset= from; offset < upto; offset++ )
	{
	if  ( ls->ls_L1_SegmentHead < 0 )
	    {
	    ls->ls_L1_SegmentHead= offset;
	    ls->ls_L1_SpaceHead= -1;
	    }

	/*  NOTE: btInputs  */
	switch( classes[offset] )
	    {
	    case UCDbidi_S:
	    /* case UCDbidi_B: Not supported */
		if  ( ls->ls_L1_SpaceHead < 0 )
		    { ls->ls_L1_SpaceHead= offset;	}

		if  ( ls->lsGotSegmentBody				&&
		      (*ls->lsGotSegmentBody)( ls->lsThrough,
			    ls->ls_L1_SegmentHead, ls->ls_L1_SpaceHead ) )
		    { LDEB(offset); return -1;	}

		if  ( ls->lsGotSegmentTail				&&
		      (*ls->lsGotSegmentTail)( ls->lsThrough,
			    ls->ls_L1_SpaceHead, offset, uptoSeparator ) )
		    { LDEB(offset); return -1;	}

		ls->ls_L1_SegmentState= UCDbidi_BN;
		ls->ls_L1_SegmentHead= -1;
		ls->ls_L1_SpaceHead= -1;
		break;

	    case UCDbidi_WS:
		if  ( ls->ls_L1_SpaceHead < 0 )
		    {
		    ls->ls_L1_SegmentState= UCDbidi_WS;
		    ls->ls_L1_SpaceHead= offset;
		    }
		break;

	    default:
		ls->ls_L1_SegmentState= UCDbidi_BN;
		ls->ls_L1_SpaceHead= -1;
		break;

	    /* sanity checks: X9 Removed */
	    case UCDbidi_RLE:
	    case UCDbidi_LRE:
	    case UCDbidi_RLO:
	    case UCDbidi_LRO:
	    case UCDbidi_PDF:
	    case UCDbidi_BN:
		continue;

	    case UCDbidi_B:
		LSDEB(offset,ucdBidiClassStr(classes[offset])); return -1;
	    }
	}

    return 0;
    }

static int bidiHandleLineClassesBwd(	LineState *		ls,
					const unsigned char *	classes,
					int			from,
					int			upto )
    {
    int		offset;

    for ( offset= upto- 1; offset >= from; offset-- )
	{
	if  ( ls->ls_L1_SegmentUpto < 0 )
	    {
	    ls->ls_L1_SegmentUpto= offset+ 1;
	    ls->ls_L1_SpaceHead= -1;
	    }

	/*  NOTE: btInputs  */
	switch( classes[offset] )
	    {
	    case UCDbidi_S:
	    /* case UCDbidi_B: Not supported */
		if  ( ls->ls_L1_SegmentHead < 0 )
		    { ls->ls_L1_SegmentHead= offset+ 1;	}
		if  ( ls->ls_L1_SpaceHead < 0 )
		    { ls->ls_L1_SpaceHead= offset+ 1;	}

		if  ( ls->lsGotSegmentTail				&&
		      (*ls->lsGotSegmentTail)( ls->lsThrough,
			    ls->ls_L1_SpaceHead, ls->ls_L1_SegmentUpto,
			    ls->ls_L1_SegmentState == UCDbidi_S ) )
		    { LDEB(offset); return -1;	}

		if  ( ls->lsGotSegmentBody				&&
		      (*ls->lsGotSegmentBody)( ls->lsThrough,
			    ls->ls_L1_SegmentHead, ls->ls_L1_SpaceHead ) )
		    { LDEB(offset); return -1;	}

		ls->ls_L1_SegmentUpto= offset;
		ls->ls_L1_SegmentHead= -1;
		ls->ls_L1_SpaceHead= -1;
		ls->ls_L1_SegmentState= UCDbidi_S;
		break;

	    case UCDbidi_WS:
		if  ( ls->ls_L1_SegmentHead < 0		||
		      ls->ls_L1_SegmentHead > offset	)
		    { ls->ls_L1_SegmentHead= offset;	}
		break;

	    default:
		if  ( ls->ls_L1_SegmentHead < 0		||
		      ls->ls_L1_SegmentHead > offset	)
		    { ls->ls_L1_SegmentHead= offset;	}
		if  ( ls->ls_L1_SpaceHead < 0 )
		    { ls->ls_L1_SpaceHead= offset+ 1;	}

		ls->ls_L1_SegmentHead= offset;
		break;

	    /* sanity checks X9 Removed */
	    case UCDbidi_RLE:
	    case UCDbidi_LRE:
	    case UCDbidi_RLO:
	    case UCDbidi_LRO:
	    case UCDbidi_PDF:
	    case UCDbidi_BN:
		if  ( ls->ls_L1_SegmentHead > offset )
		    { ls->ls_L1_SegmentHead= offset;	}
		continue;

	    case UCDbidi_B:
		LSDEB(offset,ucdBidiClassStr(classes[offset])); return -1;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle the classes of a line of text inside a paragraph.		*/
/*  We issue callbacks for every segment in the line.			*/
/*  We process the line as a whole.					*/
/*									*/
/*  1)  Look for the node that holds the head of the line.		*/
/*									*/
/************************************************************************/

int bidiHandleLineSegments(	int			paraEmbedding,
				BidiGotSegmentBody	gotSegmentBody,
				BidiGotSegmentTail	gotSegmentTail,
				void *			through,
				const unsigned char *	classes,
				int			from,
				int			upto )
    {
    LineState	ls;

    bidiStartLine( &ls, paraEmbedding,
				gotSegmentBody, gotSegmentTail, through );

    if  ( ls.lsParaLevel % 2 == 0 )
	{
	const int	uptoSeparator= 0;
	const int	offset= upto;

	if  ( bidiHandleLineClassesFwd( &ls, classes, from, upto ) )
	    { LLDEB(from,upto); return -1;	}

	if  ( ls.ls_L1_SegmentHead < 0 )
	    {
	    ls.ls_L1_SegmentHead= offset;
	    ls.ls_L1_SpaceHead= -1;
	    }

	if  ( ls.ls_L1_SpaceHead < 0 )
	    { ls.ls_L1_SpaceHead= offset;	}

	if  ( ls.lsGotSegmentBody					&&
	      (*ls.lsGotSegmentBody)( ls.lsThrough,
			ls.ls_L1_SegmentHead, ls.ls_L1_SpaceHead )	)
	    { LDEB(offset); return -1;	}

	if  ( ls.lsGotSegmentTail					&&
	      (*ls.lsGotSegmentTail)( ls.lsThrough,
			ls.ls_L1_SpaceHead, offset, uptoSeparator ) )
	    { LDEB(offset); return -1;	}
	}
    else{
	const int	offset= from;

	if  ( bidiHandleLineClassesBwd( &ls, classes, from, upto ) )
	    { LLDEB(from,upto); return -1;	}

	if  ( ls.ls_L1_SegmentUpto < 0 )
	    {
	    ls.ls_L1_SegmentUpto= offset+ 1;
	    ls.ls_L1_SpaceHead= -1;
	    }

	if  ( ls.ls_L1_SegmentHead < 0 )
	    { ls.ls_L1_SegmentHead= offset;	}
	if  ( ls.ls_L1_SpaceHead < 0 )
	    { ls.ls_L1_SpaceHead= ls.ls_L1_SegmentHead;	}

	if  ( ls.lsGotSegmentTail				&&
	      (*ls.lsGotSegmentTail)( ls.lsThrough,
		    ls.ls_L1_SpaceHead, ls.ls_L1_SegmentUpto,
		    ls.ls_L1_SegmentState == UCDbidi_S ) )
	    { LDEB(offset); return -1;	}

	if  ( ls.lsGotSegmentBody				&&
	      (*ls.lsGotSegmentBody)( ls.lsThrough,
		    ls.ls_L1_SegmentHead, ls.ls_L1_SpaceHead ) )
	    { LDEB(offset); return -1;	}
	}

    return 0;
    }

