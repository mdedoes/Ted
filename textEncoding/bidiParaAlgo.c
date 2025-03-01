#   include	<stdlib.h>
#   include	<appDebugon.h>
#   include	"ucdBidiClass.h"
#   include	"bidiAlgoImpl.h"
#   include	"bidiAlgo.h"
#   include	"bidiDebug.h"

#   if 0

Possible Substitutions
u: Uses in the same position
<: Uses in a later position
>: Uses in an earlier position
+: Produces
-: Removes (Sometimes)
x: Removes (Always)
!: Previously removed
s: Strong
w: Weak
n: Neutral

    L    R    AL   EN   ES   ET   AN   CS   NSM  S    WS   ON   sor  eor
    s    s    s    w    w    w    w    w    .    n    n    n    .    .
W1  <+   <+   <+   <+   <+   <+   <+   <+   x    <+   <+   <+   <    .
W2  <    <    <    -    .    .    +    .    !    .    .    .    <    .
W3  .    +    x    .    .    .    .    .    !    .    .    .    .    .
W4  .    .    !    <+>  -    .    <+>  -    !    .    .    .    .    .
W5  .    .    !    <+>  .    -    .    .    !    .    .    .    .    .
W6  .    .    !    .    x    x    .    x    !    .    .    +    .    .
W7  <+   <    !    u-   !    !    .    !    !    .    .    .    <    .
N1  <+>  <+>  !    <>   !    !    <>   !    !    u-   u-   u-   <    >
N2  +    +    !    .    !    !    .    !    !    x    x    x    .    .
I   u    u    !    u    !    !    u    !    !    !    !    !    .    .

NOTE:
    (Ignoring W1)
    That W2, W4, W5, W7, N1 use classes in a previous position.
	That W2 changes an EN to an AN.
	    Uses an AL in a previous position that will disappear through W3
	That W4 changes an ES to an EN or a CS to AN/EN.
	    Uses an EN in a previous position that might disappear through W7
	That W5 changes an ET to an EN.
	    W5 can never pick up ENs produced by W4 as those are between 
		ENs. For that reason, they have no adjacent ETs.
	    Uses an EN in a previous position that might disappear through W7
	That W7 changes an EN to an L.
	    Replaces an EN that might have been produced by W4 or W5
	That N1 changes neutrals to an L or an R.
	    The fact that W2 changes some ENs to AN is irrelevant. (same efect)
	    Uses Rs that are produced by W3
	    Must avoid to use ENs that are removed by W7
	That N2 changes all neutrals to an L or an R.

NOTE:
    That L1 cannot be combined with the paragraph level approach for two
    reasons:
    -   It applies to the line rather than to the paragraph. But even if 
	we decide to ignore the theoretical argument:
    -   It cannot be applied in a forward-only way. If you just keep 
        local state, you cannot know the levels of the neutrals without 
	looking back in the input. In the following example, you must look 
	back, to find the positions of the Ss and the ONs once the direction 
	of the ONs is determined by the final strong symbol using N1 or N2.
	=   L ON S ON S ON L or
	=   L ON S ON S ON R

#   endif

/************************************************************************/
/*									*/
/*  Feed an individual class to the tree builder to handle.		*/
/*									*/
/************************************************************************/

static int bidiHandleParagraphClass(	ParaState *		ps,
					unsigned char		klass,
					int			offset,
					int			upto )
    {
SLLSDEB(">>",offset,upto,ucdBidiClassStr(klass));

    switch( klass )
	{
	case UCDbidi_RLE:	/*  X2  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_LRE:	/*  X3  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_RLO:	/*  X4  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_LRO:	/*  X5  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_PDF:	/*  X7  */
	    if  ( bidiFinishEmbedding( ps, offset, upto ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_LRI:	/*  X5b  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_RLI:	/*  X5a  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_FSI:	/*  X5c  */
	    if  ( bidiStartLevel( ps, offset, upto, klass ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_PDI:	/*  X6a  */
	    if  ( bidiFinishIsolate( ps, offset, upto ) )
		{ LLDEB(offset,upto); return -1;	}
	    break;

	case UCDbidi_NSM: /* Removed */
	    if  ( bidi_X9_keep( ps, offset, upto ) )
		{ LLDEB(offset,upto); return -1;	}

	    klass= ps->ps_ImplicitState.is_W1_replacement;
	    CLS_DEB("W1",offset,klass);
	    break;

	case UCDbidi_BN:
	    if  ( bidi_X9_keep( ps, offset, upto ) )
		{ LLDEB(offset,upto); return -1;	}

	    klass= ps->ps_ImplicitState.is_W1_replacement;
	    CLS_DEB("W1",offset,klass);
	    break;

	default:
	    if  ( bidi_X9_keep( ps, offset, upto ) )
		{ LLDEB(offset,upto); return -1;	}

	    break;
	}

    if  ( bidiHandleImplicitClass( ps->psScanner,
				    &(ps->ps_ImplicitState), klass, offset ) )
	{ LSDEB(offset,ucdBidiClassStr(klass)); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Traverse an array of classifications of the symbols in a		*/
/*  text and invoke the callback in a way that makes it possible to	*/
/*  build a tree structure that represents the tree structure of the	*/
/*  embedding of the runs in the text.					*/
/*									*/
/************************************************************************/

int bidiHandleParagraphClasses(	ParaState *		ps,
				const unsigned char *	classes,
				int			from,
				int			upto )
    {
    int			offset;

    /*  X1; W1  */
    for ( offset= from; offset < upto; offset++ )
	{
	if  ( bidiHandleParagraphClass( ps, classes[offset],
						    offset, offset+ 1 ) )
	    { LDEB(offset); return -1;	}
	}

    return 0;
    }

void bidiCloseParagraph(	ParaState *	ps )
    {
    if  ( ps->ps_QueuedExplicits )
	{ free( ps->ps_QueuedExplicits );	}

    free( ps );
    }

