/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	"docParaNodeProperties.h"
#   include	"docParaBuilderImpl.h"
#   include	"docParaParticuleAdmin.h"
#   include	<docListLevel.h>
#   include	<docListDepth.h>
#   include	<docTextParticule.h>
#   include	<docParaProperties.h>
#   include	<docTreeType.h>
#   include	"docParaBuilder.h"
#   include	<docSectProperties.h>
#   include	"docSelect.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the string for a listtext field.				*/
/*  The caller (below) makes the particules.				*/
/*									*/
/*  1)  The actual number						*/
/*  2)  The space for the separator.					*/
/*									*/
/************************************************************************/

static int docCalculateListtextParaString( char *		target,
					int			maxsize,
					int			ilvl,
					const int *		numberPath,
					const int *		startPath,
					const int *		formatPath,
					const ListLevel *	ll )
    {
    int		done;

    /*  1  */
    done= docListLevelFormatParagraphNumber( target, maxsize, (int *)0, 0,
			    ilvl, numberPath, startPath, formatPath, ll );
    if  ( done < 0 )
	{ LDEB(done); return done;	}

    target += done;

    /*  2  */
    if  ( ll->llFollow == DOCllfTAB || ll->llFollow == DOCllfSPACE )
	{
	if  ( done+ 1 >= maxsize )
	    { LLDEB(done,maxsize); }

	*(target++)= ' '; done++;
	target[0]= '\0';
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Evaluate Listtext fields for bulleted and numbered lists &c.	*/
/*									*/
/*  1)  Paranoia: check list number.					*/
/*  2)  Get override and number tree.					*/
/*  3)  Link override to list if nexessary.				*/
/*  4)  Paranoia again: Is it an existing list?				*/
/*  5)  Obtain the path in the number tree. ( E.G: 13.1.18.11 )		*/
/*  6)  Calculate number or bullet text.				*/
/*  7)  Replace old field contents with the new string.			*/
/*  8)  Calculate attribute number for added text.			*/
/*  9)  Insert text particule.						*/
/*  10) Insert tab.							*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docRecalculateParaListtextTextParticules(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				struct DocumentField *		dfListtext,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int				partTail= partHead+ partCount;
    BufferItem *		paraNode= pb->pbParaNode;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    DocumentTree *		tree= rf->rfTree;

    int				ilvl= pp->ppListLevel;

    struct ListOverride *	lo;
    struct DocumentList *	dl;

    int				paraNr= docNumberOfParagraph( paraNode );

    int				startPath[DLmaxLEVELS+1];
    int				formatPath[DLmaxLEVELS+1];
    const ListLevel *		ll= (const ListLevel *)0;
    int				numberPath[DLmaxLEVELS+1];

    char			addedString[200];
    int				addedStrlen;

    int				stroff;
    int				stroffShift;
    int				textAttrNr= -1;

    int				makeSep= 0;
    int				sepKind= TPkindTAB;

    struct TextParticule *	tp;

    if  ( pp->ppListOverride <= 0 )
	{ /*LDEB(pp->ppListOverride);*/ *pCalculated= 0; goto ready; }

    /*  1,2,3,4  */
    if  ( docGetListLevelOfParagraph( startPath, formatPath,
				&lo, &dl, &ll, paraNode, rf->rfDocument ) )
	{
	LLDEB(pp->ppListOverride,pp->ppListLevel);
	*pCalculated= 0; goto ready;
	}

    /*  5  */
    if  ( docListNumberTreesGetNumberPath( numberPath, &(tree->dtListNumberTrees),
							pp->ppListOverride, ilvl, paraNr ) )
	{
	LLDEB(pp->ppListOverride,ilvl);
	SDEB(docTreeTypeStr(paraNode->biTreeType));
	*pCalculated= 0; goto ready;
	}

    if  ( tree->dtRoot->biTreeType == DOCinBODY )
	{
	const BufferItem *	sectNode= docGetSectNode( paraNode );
	const BufferItem *	parentNode= sectNode->biParent;
	int			sect= sectNode->biNumberInParent;

	while( sect > 0 )
	    {
	    BufferItem *	node= parentNode->biChildren[sect];

	    if  ( node->biSectRestartPageNumbers )
		{
		DocumentPosition	dpOffset;

		if  ( ! docHeadPosition( &dpOffset, node ) )
		    {
		    int		offsetParaNr= docNumberOfParagraph( dpOffset.dpNode );
		    int		rootOffset= 0;

		    if  ( docListNumberTreesGetRootOffset( &rootOffset, &(tree->dtListNumberTrees),
							pp->ppListOverride, ilvl, offsetParaNr ) )
			{
			LLDEB(pp->ppListOverride,ilvl);
			SDEB(docTreeTypeStr(paraNode->biTreeType));
			*pCalculated= 0; goto ready;
			}

		    numberPath[0] -= rootOffset;
		    }

		break;
		}

	    sect--;
	    }
	}

    /*  6  */
    addedStrlen= docCalculateListtextParaString(
				    addedString, sizeof(addedString)- 1, ilvl,
				    numberPath, startPath, formatPath, ll );
    if  ( addedStrlen < 0 )
	{ LLDEB(ilvl,addedStrlen); *pCalculated= 0; goto ready;	}

    /*  7  */
    if  ( docFieldReplaceContents( &stroff, &stroffShift, &textAttrNr,
			    paraNode, partHead, partCount,
			    *pStroffShift, addedString, addedStrlen, rf ) )
	{ LDEB(1); partTail= -1; goto ready;	}

    /*  9  */
    if  ( ll->llFollow == DOCllfTAB )
	{ makeSep= 1; sepKind= TPkindTAB;	}

    if  ( ll->llFollow == DOCllfSPACE					&&
	  ( PROPmaskISSET( &(ll->llTextAttributeMask),
					TApropTEXTUNDERLINED )	||
	    PROPmaskISSET( &(ll->llTextAttributeMask),
					TApropSTRIKETHROUGH )	)	)
	{ makeSep= 1; sepKind= TPkindSPAN;	}

    tp= docParaGraphBuilderInsertSpanParticule( pb,
		    partHead+ 1, stroff, addedStrlen- makeSep, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); *pCalculated= 0; partTail= -1; goto ready;	}

    partTail= partHead+ 1;

    /*  10  */
    if  ( makeSep )
	{
	tp= docInsertTextParticule( paraNode, partHead+ 2,
					    stroff+ addedStrlen- makeSep, 1,
					    sepKind, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); *pCalculated= 0; partTail= -1; goto ready;	}

	partTail= partHead+ 2;
	}

    *pCalculated= 1;
    *pStroffShift= stroffShift;

  ready:

    return partTail;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif
