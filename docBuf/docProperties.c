/************************************************************************/
/*									*/
/*  Document Properties administration routines.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<docTextParticule.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>

#   include	"docBuf.h"
#   include	"docFind.h"
#   include	"docTreeNode.h"
#   include	"docSelect.h"
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<utilPalette.h>
#   include	<docListAdmin.h>
#   include	<fontDocFontList.h>
#   include	"docAttributes.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Merge the font list of one document into that of another one.	*/
/*  Update the references accordingly.					*/
/*									*/
/*  a)  Scan all paragraphs to remember text attributes. This is done	*/
/*	with a 'Find' routine thet finds nothing.			*/
/*									*/
/*  1)  NOTE that the default color 0 is not really a color.		*/
/*  2)  As we do not want to look for colors everywhere, we also map	*/
/*	unused colors. This differs from the approach to fonts.		*/
/*									*/
/************************************************************************/

typedef struct ResourceUsed
    {
    unsigned char *	ruFontsUsed;
    unsigned char *	ruListStyleUsed;
    } ResourceUsed;

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

/*  a  */
static int docMergeScanParaAttributes(
				DocumentSelection *		ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    ResourceUsed *		ru= (ResourceUsed *)through;

    TextParticule *		tp;
    int				part;
    int				errors= 0;
    int				ls;

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; part++, tp++ )
	{
	const TextAttribute *	ta;

	if  ( tp->tpTextAttrNr < 0 )
	    { errors++; tp->tpTextAttrNr= 0;	}

	ta= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

	ru->ruFontsUsed[ta->taFontNumber]= 1;
	}

    if  ( errors > 0 )
	{ LDEB(errors); docListNode(bd,0,paraNode,0);	}

    ls= paraNode->biParaProperties->ppListOverride;
    if  ( ls > 0 )
	{ ru->ruListStyleUsed[ls]= 1;	}

    return 1;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int docMergeListTables(	struct BufferDocument *		bdTo,
				struct BufferDocument *		bdFrom,
				const unsigned char *		lsUsed,
				int *				lsMap,
				const unsigned char *		listUsed,
				int *				listIndexMap,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    DocumentProperties *	dpTo= bdTo->bdProperties;
    const DocumentProperties *	dpFrom= bdFrom->bdProperties;
    ListAdmin *			laTo= dpTo->dpListAdmin;
    const ListAdmin *		laFrom= dpFrom->dpListAdmin;

    int				listStylesAdded;

    listStylesAdded= docMergeListAdmins( laTo, laFrom,
					    lsUsed, lsMap,
					    listUsed, listIndexMap,
					    fontMap, colorMap, rulerMap );
    if  ( listStylesAdded < 0 )
	{ LDEB(listStylesAdded); return -1;	}

    if  ( listStylesAdded > 0						&&
	  docClaimListNumberTrees( &(bdTo->bdBody.dtListNumberTrees),
			   laTo->laListOverrideTable.lotOverrideCount )	)
	{ LDEB(listStylesAdded); return -1;	}

    return 0;
    }

static void docCountListFontsUsed(	struct BufferDocument *	bdFrom,
					const unsigned char *	lsUsed,
					unsigned char *		listUsed,
					unsigned char *		fontUsed )
    {
    const DocumentProperties *	dpFrom= bdFrom->bdProperties;
    const ListAdmin *		laFrom= dpFrom->dpListAdmin;
    const DocumentListTable *	dltFrom= &(laFrom->laListTable);
    const ListOverrideTable *	lotFrom= &(laFrom->laListOverrideTable);

    int				from;

    if  ( lsUsed && listUsed ) /* set if lotFrom->lotOverrideCount > 0 */
	{
	ListOverride *		lo= lotFrom->lotOverrides;

	for ( from= 0; from < lotFrom->lotOverrideCount; lo++, from++ )
	    {
	    int				listIndex;

	    int				level;
	    const ListOverrideLevel *	lol;

	    if  ( ! lsUsed[from] )
		{ continue;	}

	    listIndex= docGetListOfOverride( lo, dltFrom );
	    if  ( listIndex < 0 )
		{ LDEB(listIndex);		}
	    else{ listUsed[listIndex]= 1;	}

	    lol= lo->loLevels;
	    for ( level= 0; level < lo->loLevelCount; lol++, level++ )
		{
		const ListLevel *	ll= &(lol->lolListLevel);

		if  ( ! lol->lolOverrideFormat )
		    { continue;	}
		if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
							TApropFONT_NUMBER ) )
		    { continue;	}

		fontUsed[ll->llTextAttribute.taFontNumber]= 1;
		}
	    }
	}

    if  ( listUsed ) /* set if dltFrom->dltListCount > 0 */
	{
	const DocumentList *	dl= dltFrom->dltLists;

	for ( from= 0; from < dltFrom->dltListCount; dl++, from++ )
	    {
	    int			level;
	    const ListLevel *	ll= dl->dlLevels;

	    if  ( ! listUsed[from] )
		{ continue;	}

	    for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
		{
		if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
							TApropFONT_NUMBER ) )
		    { continue;	}

		fontUsed[ll->llTextAttribute.taFontNumber]= 1;
		}
	    }
	}

    return;
    }

int docMergeDocumentLists(	int **				pFontMap,
				int **				pListStyleMap,
				struct BufferDocument *		bdTo,
				struct BufferDocument *		bdFrom,
				const int *			colorMap,
				const int *			rulerMap )
    {
    int				rval= 0;

    DocumentProperties *	dpTo= bdTo->bdProperties;
    struct DocumentFontList *	dflTo= dpTo->dpFontList;

    const DocumentProperties *	dpFrom= bdFrom->bdProperties;
    const struct DocumentFontList *	dflFrom= dpFrom->dpFontList;

    const ListAdmin *		laFrom= dpFrom->dpListAdmin;
    const DocumentListTable *	dltFrom= &(laFrom->laListTable);
    const ListOverrideTable *	lotFrom= &(laFrom->laListOverrideTable);

    int *			fontMap= (int *)0;
    unsigned char *		fontUsed= (unsigned char *)0;

    unsigned char *		lsUsed= (unsigned char *)0;
    int *			lsMap= (int *)0;
    unsigned char *		listUsed= (unsigned char *)0;
    int *			listIndexMap= (int *)0;

    int				from;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    ResourceUsed		ru;

    /*****/

    if  ( fontListAllocateMergeAdmin( &fontMap, &fontUsed, dflFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( lotFrom->lotOverrideCount > 0 )
	{
	lsUsed= (unsigned char *)malloc(
			    lotFrom->lotOverrideCount* sizeof(unsigned char) );
	if  ( ! lsUsed )
	    { LXDEB(lotFrom->lotOverrideCount,lsUsed); rval= -1; goto ready; }
	lsMap= (int *)malloc( lotFrom->lotOverrideCount* sizeof(int) );
	if  ( ! lsMap )
	    { LXDEB(lotFrom->lotOverrideCount,lsMap); rval= -1; goto ready; }

	for ( from= 0; from < lotFrom->lotOverrideCount; from++ )
	    { lsMap[from]= -1; lsUsed[from]= 0; }

	lsMap[0]= 0;
	}

    if  ( dltFrom->dltListCount > 0 )
	{
	listUsed= (unsigned char *)malloc(
				dltFrom->dltListCount* sizeof(unsigned char) );
	if  ( ! listUsed )
	    { LXDEB(dltFrom->dltListCount,listUsed); rval= -1; goto ready; }

	listIndexMap= (int *)malloc( dltFrom->dltListCount* sizeof(int) );
	if  ( ! listIndexMap )
	    { LXDEB(dltFrom->dltListCount,listIndexMap); rval= -1; goto ready; }

	for ( from= 0; from < dltFrom->dltListCount; from++ )
	    { listUsed[from]= 0; listIndexMap[from]= -1; }
	}

    /*****/

    /*  a  */
    ru.ruFontsUsed= fontUsed;
    ru.ruListStyleUsed= lsUsed;
    if  ( docDocumentHead( &dpBeginFrom, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bdFrom,
				    docMergeScanParaAttributes, (void *)&ru );
    if  ( res != 1 )
	{ LDEB(res); rval= -1; goto ready;	}

    docCountListFontsUsed( bdFrom, lsUsed, listUsed, fontUsed );

    /*****/

    if  ( fontListMergeLists( dflTo, dflFrom, fontMap, fontUsed ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*****/

    if  ( docMergeListTables( bdTo, bdFrom, lsUsed, lsMap,
						listUsed, listIndexMap,
						fontMap, colorMap, rulerMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pFontMap= fontMap; fontMap= (int *)0; /*  steal */
    *pListStyleMap= lsMap; lsMap= (int *)0; /*  steal */

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( fontUsed )
	{ free( fontUsed );	}

    if  ( lsMap )
	{ free( lsMap );	}
    if  ( lsUsed )
	{ free( lsUsed );	}

    if  ( listUsed )
	{ free( listUsed );	}
    if  ( listIndexMap )
	{ free( listIndexMap );	}

    return rval;
    }

int docMergeColorTables(	int **				pColorMap,
				struct BufferDocument *		bdTo,
				const struct BufferDocument *		bdFrom )
    {
    const int			avoidZero= 1;
    const int			maxColors= 256;

    DocumentProperties *	dpTo= bdTo->bdProperties;
    ColorPalette *		cpTo= dpTo->dpColorPalette;

    const DocumentProperties *	dpFrom= bdFrom->bdProperties;
    const ColorPalette *	cpFrom= dpFrom->dpColorPalette;

    return utilMergeColorPalettes( pColorMap, cpTo, cpFrom,
							avoidZero, maxColors );
    }

