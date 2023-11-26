/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<docParaProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docListLevel.h>
#   include	<docTreeNode.h>
#   include	<psPrint.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Block level ones
 */
static const char STRUCTtypeP[]= "P";
/*static const char STRUCTtypeDIV[]= "Div";*/

static const char STRUCTtypeH1[]= "H1";
static const char STRUCTtypeH2[]= "H2";
static const char STRUCTtypeH3[]= "H3";
static const char STRUCTtypeH4[]= "H4";
static const char STRUCTtypeH5[]= "H5";
static const char STRUCTtypeH6[]= "H6";

static const char STRUCTtypeLI[]= "LI";

/************************************************************************/

/* See 14.8.5.5 List Attributes (p608) */
int docPsSaveListStructureAttributes(
	    const struct BufferDocument *	bd,
	    int					listOverride,
	    int					listLevel,
	    MemoryBuffer *			structureAttributes )
    {
    struct ListOverride *	lo= (struct ListOverride *)0;
    struct DocumentList *	dl= (struct DocumentList *)0;
    const ListLevel * 		ll= (const struct ListLevel *)0;

    if  ( docGetListLevel( (int *)0, (int *)0,
				&lo, &dl, &ll, listOverride, listLevel, bd ) )
	{ LLDEB(listOverride,listLevel); return -1;	}

    SimpleOutputStream * sosAttributes= sioOutMemoryOpen( structureAttributes );

    sioOutPrintf( sosAttributes, "/O/List " );

    switch( ll->llNumberStyle )
	{
	case DOCpnDEC:
	    sioOutPrintf( sosAttributes, "/ListNumbering/Decimal " );
	    break;
	case DOCpnUCRM:
	    sioOutPrintf( sosAttributes, "/ListNumbering/UpperRoman " );
	    break;
	case DOCpnLCRM:
	    sioOutPrintf( sosAttributes, "/ListNumbering/LowerRoman " );
	    break;
	case DOCpnUCLTR:
	    sioOutPrintf( sosAttributes, "/ListNumbering/UpperAlpha " );
	    break;
	case DOCpnLCLTR:
	    sioOutPrintf( sosAttributes, "/ListNumbering/LowerAlpha " );
	    break;
	default:
	    LDEB(ll->llNumberStyle);
	    break;
	}

    sioOutClose( sosAttributes );

    return 0;
    }

static const char * docPsNoListParagraphMark( int outlineLevel )
    {
    switch( outlineLevel )
	{
	case 0: return STRUCTtypeH1;
	case 1: return STRUCTtypeH2;
	case 2: return STRUCTtypeH3;
	case 3: return STRUCTtypeH4;
	case 4: return STRUCTtypeH5;
	case 5: return STRUCTtypeH6;
	default: return STRUCTtypeP;
	}
    }

static int docPsListNodeDeeper(
	    const BufferItem *		paraNode,
	    const BufferItem *		prevNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	prevPp;

    if  ( prevNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    prevPp= prevNode->biParaProperties;
    if  ( prevPp->ppListOverride != pp->ppListOverride )
	{ return pp->ppListLevel+ 1;	}

    if  ( pp->ppListLevel > prevPp->ppListLevel )
	{ return pp->ppListLevel- prevPp->ppListLevel;	}

    return 0;
    }

static int docPsListNodeShallower(
	    const BufferItem *		paraNode,
	    const BufferItem *		nextNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	nextPp;

    if  ( nextNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    nextPp= nextNode->biParaProperties;
    if  ( nextPp->ppListOverride != pp->ppListOverride )
	{ return pp->ppListLevel+ 1;	}

    if  ( pp->ppListLevel > nextPp->ppListLevel )
	{ return pp->ppListLevel- nextPp->ppListLevel;	}

    return 0;
    }

/* See Annex H.8.3 in the PDF 202 Spec about hierarchical lists */
const char * docPsParagraphNodeStartMark(
	    const PrintingState *	ps,
	    const BufferItem *		paraNode,
	    int *			pListLevelsToOpen )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( pp->ppListOverride > 0 )
	{
	const int	prev= paraNode->biNumberInParent- 1;

	if  ( prev >= 0 )
	    {
	    *pListLevelsToOpen= docPsListNodeDeeper( paraNode, parentNode->biChildren[prev] );
	    }
	else{ *pListLevelsToOpen= pp->ppListLevel+ 1;	}

	return STRUCTtypeLI;
	}
    else{
	return docPsNoListParagraphMark( pp->ppOutlineLevel );
	}
    }


/* See Annex H.8.3 in the PDF 202 Spec about hierarchical lists */
const char * docPsParagraphNodeEndMark(
	    const PrintingState *	ps,
	    const BufferItem *		paraNode,
	    int *			pListLevelsToClose )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const BufferItem *		parentNode= paraNode->biParent;

    if  ( pp->ppListOverride > 0 )
	{
	const int	next= paraNode->biNumberInParent+ 1;
	int		nextDeeper= 0;

	if  ( next < paraNode->biParent->biChildCount )
	    {
	    *pListLevelsToClose= docPsListNodeShallower( paraNode, parentNode->biChildren[next] );
	    /* If the next paragraph is at a deeper level, we want it to go to a nested list:
	       So, do not close the item. */
	    if  ( ! *pListLevelsToClose )
		{ nextDeeper= docPsListNodeDeeper( parentNode->biChildren[next], paraNode ); }
	    }
	else{ *pListLevelsToClose= pp->ppListLevel+ 1;	}

	if  ( nextDeeper )
	    { return (char *)0;		}
	else{ return STRUCTtypeLI;	}
	}
    else{
	return docPsNoListParagraphMark( pp->ppOutlineLevel );
	}
    }
