#   include	"docBaseConfig.h"

#   include	<uniUtf8.h>
#   include	"docDebugList.h"
#   include	"docListNumberTree.h"
#   include	"docDocumentList.h"
#   include	"docListOverride.h"
#   include	"docListOverrideTable.h"
#   include	"docListTable.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Debugging:								*/
/*									*/
/************************************************************************/

static void docDebugListNumberTreeNode(	int				indent,
					const ListNumberTreeNode *	lntn,
					int				before,
					int				after )
    {
    const char *	bb= "";
    const char *	aa= "";

    if  ( before >= 0				&&
	  lntn->lntnParaNr >= 0	&&
	  lntn->lntnParaNr <= before	)
	{ bb= "#+#"; }
    if  ( after >= 0 && lntn->lntnParaNr >= after )
	{ aa= "#-#"; }

    appDebug( "%*sLEAVES= %d PARA= %d%s%s%s\n", 4* indent, "",
				    lntn->lntnLeafCount,
				    lntn->lntnParaNr, bb, aa,
				    lntn->lntnIsLeaf?"*":"" );

    if  ( lntn->lntnParaNr >= 0 )
	{ before= lntn->lntnParaNr;	}

    if  ( lntn->lntnChildCount > 0 )
	{
	int		i;

	if  ( ! lntn->lntnChildren )
	    { LDEB(lntn->lntnChildren); return;	}

	for ( i= 0; i < lntn->lntnChildCount- 1; i++ )
	    {
	    int		aft;

	    aft= lntn->lntnChildren[i+ 1].lntnParaNr;

	    docDebugListNumberTreeNode( indent+ 1, &(lntn->lntnChildren[i]),
								before, aft );

	    if  ( lntn->lntnChildren[i].lntnParaNr >= 0 )
		{ before= lntn->lntnChildren[i].lntnParaNr;	}
	    }

	if  ( i < lntn->lntnChildCount )
	    {
	    docDebugListNumberTreeNode( indent+ 1, &(lntn->lntnChildren[i]),
								before, after );
	    }
	}

    return;
    }

void docListListNumberNode(	int				indent,
				const ListNumberTreeNode *	lntn )
    {
#   if 0
    int		i;

    appDebug( "%*s PARA=%d %s\n",
	indent, "",
	lntn->lntnParaNr,
	lntn->lntnIsLeaf?"LEAF":"TREE" );

    for ( i= 0; i < lntn->lntnChildCount; i++ )
	{ docListListNumberNode( indent+ 1, &(lntn->lntnChildren[i]) );	}
#   else
    docDebugListNumberTreeNode( indent, lntn, -1, -1 );
#   endif
    }

static int docListDocumentListLevelConst( int *			pLevel,
					const ListLevel *	ll,
					int			field )
    {
    int			constOff;
    int			constLen;
    int			constChars;
    int			level;

    int			chars= 0;

    if  ( docListLevelGetField( &constOff, &constLen, &constChars, &level,
								ll, field ) )
	{ LDEB(field); return -1;	}

    if  ( constLen > 0 )
	{
	const char *	from= utilMemoryBufferGetString( &(ll->llFormatString) )+ constOff;
	int		todo= constLen;

	appDebug( "%.*s", constLen, from );

	while( todo > 0 )
	    {
	    unsigned short	symbol;
	    int			step;

	    step= uniGetUtf8( &symbol, (unsigned char *)from );
	    if  ( step < 1 )
		{ SLDEB("###",step); break;	}

	    chars++; from += step; todo -= step;
	    }
	}

    if  ( chars != constChars )
	{ SLLLDEB("###",field,chars,constChars);	}

    *pLevel= level;
    return 0;
    }

void docListDocumentListLevel(	int			n,
				const ListLevel *	ll )
    {
    int			field;
    int			level;

    appDebug( "\t%d: format: \"%s\"\n", n,
				utilMemoryBufferGetString( &(ll->llFormatString) ) );
    appDebug( "\t%d: %d fields: \"", n, ll->llLevelNumberCount );

    for ( field= 0; field < ll->llLevelNumberCount; field++ )
	{
	if  ( docListDocumentListLevelConst( &level, ll, field ) )
	    { SDEB("###"); break;	}

	appDebug( "<%d>", level );
	if  ( level < 0 || level >= DLmaxLEVELS ||
	      ( n >= 0 && level > n )		)
	    { appDebug( "###" );	}
	}

    if  ( docListDocumentListLevelConst( &level, ll, field ) )
	{ SDEB("###");	}
    else{
	if  ( level != -1 )
	    {
	    appDebug( "<%d>", level );
	    appDebug( "###" );
	    }
	}

    appDebug( "\"\n" );
    }

void docListDocumentList(	int			n,
				const DocumentList *	dl )
    {
    int		l;

    appDebug( "LIST %3d:\n", n );

    for ( l= 0; l < dl->dlLevelCount; l++ )
	{
	docListDocumentListLevel( l, dl->dlLevels+ l );
	}
    }

void docListListOverrideLevel(	int				n,
				const ListOverrideLevel *	lol )
    {
    docListDocumentListLevel( n, &(lol->lolListLevel) );
    }

void docListListOverride(	int				n,
				const ListOverride *		lo )
    {
    int		l;

    for ( l= 0; l < lo->loLevelCount; l++ )
	{
	docListListOverrideLevel( l, lo->loLevels+ l );
	}
    }

void docListListOverrideTable(	int				n,
				const ListOverrideTable *	lot )
    {
    int		o;

    for ( o= 0; o < lot->lotOverrideCount; o++ )
	{
	docListListOverride( o, lot->lotOverrides+ o );
	}
    }

void docListDocumentListTable( const DocumentListTable *	dlt )
    {
    int		n;

    for ( n= 0; n < dlt->dltListCount; n++ )
	{
	docListDocumentList( n, dlt->dltLists+ n );
	}
    }

