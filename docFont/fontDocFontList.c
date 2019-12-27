/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<stdlib.h>

#   include	"fontDocFontList.h"
#   include	"fontDocFontListImpl.h"
#   include	"fontDocFont.h"
#   include	<textOfficeCharset.h>
#   include	<psDocumentFontStyle.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a Font List.						*/
/*  Clean a Font List.							*/
/*  Copy a Font List.							*/
/*									*/
/************************************************************************/

void fontInitDocFontList(	DocumentFontList *	dfl )
    {
    dfl->dflFontCount= 0;
    dfl->dflFontPages= (DocumentFont **)0;
    dfl->dflFontPageCount= 0;
    dfl->dflFontSortIndex= (DocFontSort *)0;
    }

void fontCleanDocFontList(	DocumentFontList *	dfl )
    {
    int		page;
    int		i;

    for ( page = 0; page < dfl->dflFontPageCount; page++ )
	{
	DocumentFont *	fontPage= dfl->dflFontPages[page];

	if  ( fontPage )
	    {
	    for ( i= 0; i < DFL_PGSZ; i++ )
		{ fontCleanDocumentFont( fontPage+ i );	}

	    free( fontPage );
	    }
	}

    if  ( dfl->dflFontPages )
	{ free( dfl->dflFontPages );	}

    if  ( dfl->dflFontSortIndex )
	{ free( dfl->dflFontSortIndex );	}

    return;
    }

DocumentFont * fontFontListGetFontByNumber(
				    const DocumentFontList *	dfl,
				    int				n )
    {
    int			page= n/ DFL_PGSZ;
    DocumentFont *	fontPage;

    if  ( n < 0 || page >= dfl->dflFontPageCount )
	{ return (DocumentFont *)0;	}

    fontPage= dfl->dflFontPages[page];
    if  ( ! fontPage )
	{ return (DocumentFont *)0;	}

    if  ( fontPage[n%DFL_PGSZ].dfDocFontNumber != n )
	{ return (DocumentFont *)0;	}

    return &(fontPage[n%DFL_PGSZ]);
    }

static DocumentFont *	docFontListClaimFont(
					DocumentFontList *		dfl,
					int				n )
    {
    int			page= n/ DFL_PGSZ;
    DocumentFont *	fontPage;

    if  ( dfl->dflFontSortIndex )
	{ free( dfl->dflFontSortIndex );	}
    dfl->dflFontSortIndex= (DocFontSort *)0;

    if  ( page >= dfl->dflFontPageCount )
	{
	int	newPageCount= page+ 1;

	DocumentFont **	fresh= (DocumentFont **)realloc( dfl->dflFontPages,
				    newPageCount* sizeof(DocumentFont *) );

	if  ( ! fresh )
	    { LXDEB(n,fresh); return (DocumentFont *)0;	}
	dfl->dflFontPages= fresh;

	while( dfl->dflFontPageCount < newPageCount )
	    { fresh[dfl->dflFontPageCount++]= (DocumentFont *)0;	}
	}

    fontPage= dfl->dflFontPages[page];
    if  ( ! fontPage )
	{
	int		i;

	dfl->dflFontPages[page]= fontPage=
		    (DocumentFont *)malloc( DFL_PGSZ* sizeof(DocumentFont) );
	if  ( ! fontPage )
	    { XDEB(fontPage); return (DocumentFont *)0;	}

	for ( i= 0; i < DFL_PGSZ; i++ )
	    { fontInitDocumentFont( fontPage+ i );	}
	}

    if  ( dfl->dflFontCount <= n )
	{ dfl->dflFontCount= n+ 1;	}

    return fontPage+ ( n % DFL_PGSZ );
    }


int fontCopyDocFontList(	DocumentFontList *		to,
				const DocumentFontList *	from )
    {
    int				rval= 0;

    int				i;
    DocumentFontList		copy;

    fontInitDocFontList( &copy );

    for ( i = 0; i < from->dflFontCount; i++ )
	{
	DocumentFont *		dfTo;
	const DocumentFont *	dfFrom;

	dfFrom= fontFontListGetFontByNumber( from, i );
	if  ( ! dfFrom )
	    { continue;	}
	dfTo= docFontListClaimFont( &copy, i );
	if  ( ! dfTo )
	    { XDEB(dfTo); rval= -1; goto ready;	}

	if  ( fontCopyDocumentFont( dfTo, dfFrom ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    fontCleanDocFontList( to );
    *to= copy; fontInitDocFontList( &copy ); /* steal contents */

  ready:
    fontCleanDocFontList( &copy );

    return rval;
    }

/************************************************************************/

static int fontListFindOpenSlot(	const DocumentFontList *	dfl )
    {
    int		page;

    for ( page = 0; page < dfl->dflFontPageCount; page++ )
	{
	int		i;
	DocumentFont *	fontPage= dfl->dflFontPages[page];

	if  ( ! fontPage )
	    { return DFL_PGSZ* page;	}

	for ( i= 0; i < DFL_PGSZ; i++ )
	    {
	    if  ( fontPage[i].dfDocFontNumber < 0 )
		{ return DFL_PGSZ* page+ i;	}
	    }
	}

    return dfl->dflFontCount;
    }

/************************************************************************/
/*									*/
/*  Insert a font. The font comes from the document as it is read.	*/
/*									*/
/************************************************************************/

static DocumentFont * fontInsertDocFontInList(	
					DocumentFontList *	dfl,
					const DocumentFont *	dfSet )
    {
    DocumentFont *	dfTo= (DocumentFont *)0;
    int			n= fontListFindOpenSlot( dfl );

    dfTo= docFontListClaimFont( dfl, n );
    if  ( ! dfTo )
	{ XDEB(dfTo); goto failed;	}

    if  ( fontCopyDocumentFont( dfTo, dfSet ) )
	{ LDEB(1); goto failed;	}

    dfTo->dfDocFontNumber= n;

    return dfTo;

  failed:

    if  ( dfTo )
	{ fontCleanDocumentFont( dfTo );	}

    return (DocumentFont *)0;
    }

/************************************************************************/

static int docFindFontByName(	DocumentFontList *	dflTo,
				const char *		fontFamilyName )
    {
    int		i;

    for ( i= 0; i < dflTo->dflFontCount; i++ )
	{
	const DocumentFont *	dfTo= fontFontListGetFontByNumber( dflTo, i );
	if  ( ! dfTo || dfTo->dfDocFontNumber < 0 )
	    { continue;	} 

	if  ( utilMemoryBufferEqualsString( &(dfTo->dfName), fontFamilyName ) )
	    { return i;	}
	}

    return -1;
    }

static int docMakeFontByName(	DocumentFontList *	dflTo,
				const char *		fontFamilyName )
    {
    int			fontFamilyStyle;
    DocumentFont	dfNew;
    DocumentFont *	dfTo= (DocumentFont *)0;
    int			rval= -1;

    fontFamilyStyle= psFontFamilyStyle( fontFamilyName );

    fontInitDocumentFont( &dfNew );

    if  ( fontSetFamilyStyle( &dfNew, fontFamilyStyle ) )
	{ LDEB(fontFamilyStyle); goto ready;	}
    if  ( fontSetFamilyName( &dfNew, fontFamilyName ) )
	{ SDEB(fontFamilyName); goto ready;	}

    dfTo= fontInsertDocFontInList( dflTo, &dfNew );
    if  ( ! dfTo )
	{ LDEB(dflTo->dflFontCount); goto ready;	}
    rval= dfTo->dfDocFontNumber;

  ready:

    fontCleanDocumentFont( &dfNew );

    return rval;
    }

int fontListGetFontByName(	DocumentFontList *	dflTo,
			const char *		fontFamilyName )
    {
    int			rval= docFindFontByName( dflTo, fontFamilyName );

    if  ( rval < 0 )
	{
	rval= docMakeFontByName( dflTo, fontFamilyName );
	if  ( rval < 0 )
	    { SLDEB(fontFamilyName,rval); return rval;	}
	}

    return rval;
    }

int fontMergeFontIntoList(	DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom )
    {
    DocumentFont *	dfTo;
    int			rval= -1;

    rval= docFindFontByName( dflTo, utilMemoryBufferGetString( &(dfFrom->dfName) ) );
    if  ( rval >= 0 )
	{ return rval;	}

    dfTo= fontInsertDocFontInList( dflTo, dfFrom );
    if  ( ! dfTo )
	{ XDEB(dfTo); return -1;	}

    fontCopyFaceMatches( dfTo, dfFrom );

    return dfTo->dfDocFontNumber;
    }

/************************************************************************/
/*									*/
/*  Add a font from a PostScript font list to the document font list.	*/
/*									*/
/************************************************************************/

DocumentFont * fontAddDocFontToList(
				DocumentFontList *		dfl,
				const char *			name,
				int				styleInt,
				int				pitch )
    {
    int				fontNumber;
    DocumentFont *		df;

    fontNumber= fontListGetFontByName( dfl, name );
    if  ( fontNumber < 0 )
	{ SLDEB(name,fontNumber); return (DocumentFont *)0; }

    df= fontFontListGetFontByNumber( dfl, fontNumber );
    if  ( ! df )
	{ XDEB(df); return df;	}

    df->dfStyleInt= styleInt;
    df->dfPitch= pitch;

    return df;
    }

static const DocumentFontList * dflsort;
static int utilFontCompareFontIdx(	const void * vdfs1,
					const void * vdfs2 )
    {
    const DocFontSort *		dfs1= (const DocFontSort *)vdfs1;
    const DocFontSort *		dfs2= (const DocFontSort *)vdfs2;

    const DocumentFont *	df1;
    const DocumentFont *	df2;

    df1= fontFontListGetFontByNumber( dflsort, dfs1->arrayIndex );
    df2= fontFontListGetFontByNumber( dflsort, dfs2->arrayIndex );

    if  ( ! df1 && ! df2 )
	{ return  0;	}
    if  (   df1 && ! df2 )
	{ return -1;	}
    if  ( ! df1 &&   df2 )
	{ return +1;	}

    return utilMemoryCompareBuffers( &(df1->dfName), &(df2->dfName) );
    }

static int utilDocumentFontListBuildSortIndex(	DocumentFontList *	dfl )
    {
    int		idx;

    dfl->dflFontSortIndex= (DocFontSort *)malloc(
				    dfl->dflFontCount* sizeof(DocFontSort) );
    if  ( ! dfl->dflFontSortIndex )
	{ LXDEB(dfl->dflFontCount,dfl->dflFontSortIndex); return -1;	}

    for ( idx= 0; idx < dfl->dflFontCount; idx++ )
	{
	dfl->dflFontSortIndex[idx].arrayIndex= idx;
	dfl->dflFontSortIndex[idx].sortIndex= idx;
	}

    dflsort= dfl;
    qsort( dfl->dflFontSortIndex, dfl->dflFontCount,
			sizeof(DocFontSort), utilFontCompareFontIdx );
    dflsort= (const DocumentFontList *)0;

    for ( idx= 0; idx < dfl->dflFontCount; idx++ )
	{
	int		aidx= dfl->dflFontSortIndex[idx].arrayIndex;

	dfl->dflFontSortIndex[aidx].sortIndex= idx;
	}

    return 0;
    }

const DocumentFont * fontListGetFontBySortIndex(
						DocumentFontList *	dfl,
						int			idx )
    {
    if  ( idx < 0 || idx >= dfl->dflFontCount )
	{ LLDEB(idx,dfl->dflFontCount); return (const DocumentFont *)0;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return (const DocumentFont *)0;	}

    return fontFontListGetFontByNumber( dfl,
				    dfl->dflFontSortIndex[idx].arrayIndex );
    }

int fontListGetSortIndex(	DocumentFontList *	dfl,
				int			aidx )
    {
    if  ( aidx < 0 || aidx >= dfl->dflFontCount )
	{ LLDEB(aidx,dfl->dflFontCount); return -1;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return -1;	}

    return dfl->dflFontSortIndex[aidx].sortIndex;
    }

int fontListGetArrayIndex(	DocumentFontList *	dfl,
				int			sidx )
    {
    if  ( sidx < 0 || sidx >= dfl->dflFontCount )
	{ LLDEB(sidx,dfl->dflFontCount); return -1;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return -1;	}

    return dfl->dflFontSortIndex[sidx].arrayIndex;
    }

void fontListClearCharsUsed(	DocumentFontList *	dfl )
    {
    int				i;

    for ( i= 0; i < dfl->dflFontCount; i++ )
	{
	DocumentFont *	df= fontFontListGetFontByNumber( dfl, i );

	if  ( ! df )
	    { continue;	}

	utilCleanIndexSet( &(df->dfUnicodesUsed) );
	utilInitIndexSet( &(df->dfUnicodesUsed) );

	utilCleanIndexMapping( &(df->dfUnicodeToCharset) );
	utilInitIndexMapping( &(df->dfUnicodeToCharset) );
	}
    }

int fontListMergeLists(		DocumentFontList *		dflTo,
				const DocumentFontList *	dflFrom,
				int *				fontMap,
				const unsigned char *		fontUsed )
    {
    int		from;

    for ( from= 0; from < dflFrom->dflFontCount; from++ )
	{
	const DocumentFont *	dfFrom;
	int			to;
	DocumentFont *		dfTo;

	if  ( ! fontUsed || ! fontUsed[from] )
	    { continue;	}

	dfFrom= fontFontListGetFontByNumber( dflFrom, from );
	if  ( ! dfFrom )
	    { continue;	}
	if  ( utilMemoryBufferIsEmpty( &(dfFrom->dfName) ) )
	    { LDEB(from); continue;	}

	to= fontMergeFontIntoList( dflTo, dfFrom );
	if  ( to < 0 )
	    { LDEB(to); return -1;	}

	if  ( fontMap )
	    { fontMap[from]= to;	}

	dfTo= fontFontListGetFontByNumber( dflTo, to );
	if  ( ! dfTo )
	    { XDEB(dfTo); continue;	}
	}

    return 0;
    }

int fontListAllocateMergeAdmin(	int **				pFontMap,
				unsigned char **		pFontUsed,
				const DocumentFontList *	dflFrom )
    {
    int			rval= 0;

    int			from;

    int *		fontMap= (int *)0;
    unsigned char *	fontUsed= (unsigned char *)0;

    if  ( dflFrom->dflFontCount > 0 )
	{
	fontMap= (int *)malloc( dflFrom->dflFontCount* sizeof( int ) );
	if  ( ! fontMap )
	    { LXDEB(dflFrom->dflFontCount,fontMap); rval= -1; goto ready; }
	fontUsed= (unsigned char *)malloc(
			    dflFrom->dflFontCount* sizeof( unsigned char ) );
	if  ( ! fontUsed )
	    { LXDEB(dflFrom->dflFontCount,fontUsed); rval= -1; goto ready; }

	for ( from= 0; from < dflFrom->dflFontCount; from++ )
	    { fontMap[from]= -1; fontUsed[from]= 0; }
	}

    *pFontMap= fontMap; fontMap= (int *)0; /*  steal */
    *pFontUsed= fontUsed; fontUsed= (unsigned char *)0; /*  steal */

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( fontUsed )
	{ free( fontUsed );	}

    return rval;
    }

void fontListFontList( const DocumentFontList *	dfl )
    {
    int		i;

    appDebug( "FONTLIST: %d fonts\n", dfl->dflFontCount );
    for ( i= 0; i < dfl->dflFontCount; i++ )
	{
	const DocumentFont *	df= fontFontListGetFontByNumber( dfl, i );

	if  ( ! df )
	    { continue;	}

	appDebug( "FONT %6d: \"%s\"\n", df->dfDocFontNumber,
				utilMemoryBufferGetString( &(df->dfName) ) );
	}
    }

