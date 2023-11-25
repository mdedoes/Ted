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
	    const struct BufferDocument * bd,
	    const ParagraphProperties *	pp,
	    MemoryBuffer *		structureAttributes )
    {
    struct ListOverride *	lo= (struct ListOverride *)0;
    struct DocumentList *	dl= (struct DocumentList *)0;
    const ListLevel * 		ll= (const struct ListLevel *)0;

    if  ( docGetListLevelOfParagraph( (int *)0, (int *)0,
					    &lo, &dl, &ll, pp, bd ) )
	{ LLDEB(pp->ppListOverride,pp->ppListLevel); return -1;	}

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

const char * docPsParagraphNodeMark(
	    const PrintingState *	ps,
	    const ParagraphProperties *	pp,
	    int *			pCurrentListOverride,
	    int *			pCurrentListLevel,
	    int *			pListLevelsToClose,
	    int *			pListLevelsToOpen )
    {
    if  ( pp->ppListOverride > 0 )
	{
	if  ( ps->psCurrentListOverride != pp->ppListOverride )
	    {
	    if  ( pListLevelsToClose )
		{ *pListLevelsToClose= ps->psCurrentListLevel+ 1;	}
	    if  ( pListLevelsToOpen )
		{ *pListLevelsToOpen= pp->ppListLevel+ 1;	}

	    if  ( pCurrentListOverride )
		{ *pCurrentListOverride= pp->ppListOverride;	}
	    if  ( pCurrentListLevel )
		{ *pCurrentListLevel= pp->ppListLevel;	}
	    }
	else{
	    if  ( pp->ppListLevel != ps->psCurrentListLevel )
		{
		if  ( pp->ppListLevel < ps->psCurrentListLevel && pListLevelsToClose )
		    { *pListLevelsToClose= ps->psCurrentListLevel- pp->ppListLevel;	}
		if  ( pp->ppListLevel > ps->psCurrentListLevel && pListLevelsToOpen )
		    { *pListLevelsToOpen= pp->ppListLevel- ps->psCurrentListLevel;	}

		if  ( pCurrentListLevel )
		    { *pCurrentListLevel= pp->ppListLevel;	}
		}
	    }

	return STRUCTtypeLI;
	}
    else{
	if  ( ps->psCurrentListOverride > 0 )
	    {
	    if  ( pListLevelsToClose )
		{ *pListLevelsToClose= ps->psCurrentListLevel+ 1;	}

	    if  ( pCurrentListOverride )
		{ *pCurrentListOverride= -1;	}
	    if  ( pCurrentListLevel )
		{ *pCurrentListLevel= -1;	}
	    }

	switch( pp->ppOutlineLevel ) {
	    case 0: return STRUCTtypeH1;
	    case 1: return STRUCTtypeH2;
	    case 2: return STRUCTtypeH3;
	    case 3: return STRUCTtypeH4;
	    case 4: return STRUCTtypeH5;
	    case 5: return STRUCTtypeH6;
	    default: return STRUCTtypeP;
	    }
	}
    }

