/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>

#   include	<uniUtf8.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	<docStyle.h>
#   include	<fontDocFontList.h>
#   include	"docTreeNode.h"
#   include	"docBuf.h"
#   include	<docDocumentProperties.h>
#   include	<docListAdmin.h>
#   include	<fontDocFont.h>
#   include	"docAttributes.h"
#   include	"docTextRun.h"
#   include	"docTreeScanner.h"
#   include	"docParaScanner.h"
#   include	<docScanner.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of Document Fonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetCharsUsed
    {
    struct BufferDocument *		gcuDocument;
    struct DocumentFontList *		gcuDocumentFontList;
    } GetCharsUsed;

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docCharsUsedGotSpan(	const TextRun *			tr,
				void *				through )
    {
    GetCharsUsed *		gcu= (GetCharsUsed *)through;
    struct DocumentFontList *	dfl= gcu->gcuDocumentFontList;
    const char *		s;

    DocumentFont *		df;
    int				fontNumber= tr->trTextAttribute->taFontNumber;

    int				from;
    int				upto;

    if  ( fontNumber < 0 )
	{ fontNumber= docGetDefaultFont( gcu->gcuDocument );	}

    df= fontFontListGetFontByNumber( dfl, fontNumber );
    if  ( ! df )
	{
	LXDEB(fontNumber,df);
	fontNumber= docGetDefaultFont( gcu->gcuDocument );
	df= fontFontListGetFontByNumber( dfl, fontNumber );
	if  ( ! df )
	    { LXDEB(fontNumber,df); return -1;	}
	}

    if  ( tr->trStrlen == 0 )
	{
	if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	    { XDEB(0x20); return -1;	}
	}

    from= tr->trStroff;
    upto= tr->trStroff+ tr->trStrlen;
    s= docParaString( tr->trParaNode, from );
    while( from < upto )
	{
	unsigned short			symbol;
	int				step;

	step= uniGetUtf8( &symbol, s );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	if  ( symbol == 0xa0 )
	    { symbol= 0x20;	}

	if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), symbol ) )
	    { LDEB(symbol); return -1;	}

	s += step; from += step;
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static int docInludeFontOfAttribute(	const struct BufferDocument *	bd,
					const TextAttribute *		ta )
    {
    const struct DocumentFontList *	dfl= bd->bdProperties->dpFontList;
    DocumentFont *			df;

    df= fontFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return 0;	}

    if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	{ XDEB(0x20); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docCharsListObject(	const VisitParticule *		vp,
				struct InsertedObject *		io,
				void *				through )
    {
    GetCharsUsed *		gcu= (GetCharsUsed *)through;
    struct DocumentFontList *	dfl= gcu->gcuDocumentFontList;
    DocumentFont *		df;

    df= fontFontListGetFontByNumber( dfl, vp->vpTextAttribute->taFontNumber );
    if  ( ! df )
	{ LXDEB(vp->vpTextAttribute->taFontNumber,df); return -1;	}

    if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	{ XDEB(0x20); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docGetCharsUsed(	struct BufferDocument *		bd )
    {
    int				i;
    struct DocumentFontList *	dfl= bd->bdProperties->dpFontList;

    const DocumentProperties *	dp= bd->bdProperties;
    const ListAdmin *		la= dp->dpListAdmin;
    const DocumentListTable *	dlt= &(la->laListTable);
    const ListOverrideTable *	lot= &(la->laListOverrideTable);

    GetCharsUsed		gcu;

    const DocumentList *	dl;
    const ListOverride *	lo;

    int				ret;

    const int			treeFlags=
					FLAGtsSCAN_SECTION_HEADERS_FOOTERS|
					FLAGtsSCAN_BODY_SEPARATORS|
					FLAGtsSCAN_FOOT_END_NOTES;

    const int			paraFlags= FLAGpsSCAN_COMBINE_LINES|
					FLAGpsSCAN_EMPTY_SPANS|
					FLAGpsSCAN_SHAPE_TEXTS;

    fontListClearCharsUsed( dfl );

    gcu.gcuDocument= bd;
    gcu.gcuDocumentFontList= dfl;

    /*  a  */
    ret= docScanParagraphsLogicalOrder( bd,
			    (const struct DocumentSelection *)0,
			    treeFlags, paraFlags,
			    (ParticuleVisitor)0, (ParaFieldVisitor)0,
			    docCharsUsedGotSpan, docCharsListObject,
			    (TabVisitor)0, (void *)&gcu );
    if  ( ret != SCANadviceOK )
	{ LLDEB(ret,SCANadviceOK); return -1;	}

    for ( i= 0; i < bd->bdStyleSheet.dssStyleCount; i++ )
	{
	const DocumentStyle *	ds= bd->bdStyleSheet.dssStyles[i];

	if  ( ! ds							||
	      ! PROPmaskISSET( &(ds->dsTextMask), TApropFONT_NUMBER ) )
	    { continue;	}

	/*  Include the font.. If this fails lets hope the style is not used */
	if  ( docInludeFontOfAttribute( bd, &(ds->dsTextAttribute) ) )
	    { XDEB(0x20); /* return -1; */	}
	}

    lo= lot->lotOverrides;
    for ( i= 0; i < lot->lotOverrideCount; lo++, i++ )
	{
	int				level;
	const ListOverrideLevel *	lol;

	lol= lo->loLevels;
	for ( level= 0; level < lo->loLevelCount; lol++, level++ )
	    {
	    const ListLevel *	ll= &(lol->lolListLevel);

	    if  ( ! lol->lolOverrideFormat				||
		  ! PROPmaskISSET( &(ll->llTextAttributeMask),
						TApropFONT_NUMBER )	)
		{ continue;	}

	    if  ( docInludeFontOfAttribute( bd, &(ll->llTextAttribute) ) )
		{ XDEB(0x20); return -1;	}
	    }
	}

    dl= dlt->dltLists;
    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	int			level;
	const ListLevel *	ll= dl->dlLevels;

	for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
	    {
	    if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
						    TApropFONT_NUMBER ) )
		{ continue;	}

	    if  ( ll->llTextAttribute.taFontNumber < 0 )
		{
		LLLDEB(i,level,ll->llTextAttribute.taFontNumber);
		continue;
		}

	    if  ( docInludeFontOfAttribute( bd, &(ll->llTextAttribute) ) )
		{ XDEB(0x20); return -1;	}
	    }
	}

    return 0;
    }

