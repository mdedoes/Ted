/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilPropMask.h>
#   include	<appDebugon.h>

#   include	"docDocumentProperties.h"

/************************************************************************/
/*									*/
/*  Initialise Document Properties.					*/
/*									*/
/************************************************************************/

void docInitDocumentProperties(	DocumentProperties *	dp )
    {
    dp->dpContainsTables= 0;
    dp->dpTabIntervalTwips= 720;
    dp->dpDefaultColor= 0;
    dp->dpDefaultFont= -1;
    dp->dpAnsiCodepage= -1;

    dp->dpStartPageNumber= 1;
    dp->dpFootEndNoteType= DPfetFOOT_ONLY;

    dp->dpFootnoteProperties.npStartNumber= 1;
    dp->dpFootnoteProperties.npPosition= DPftnPOS_PAGE_BOTTOM;
    dp->dpFootnoteProperties.npRestart= DPftnRST_CONTINUOUS;
    dp->dpFootnoteProperties.npNumberStyle= DPftnNAR;

    dp->dpEndnoteProperties.npStartNumber= 1;
    dp->dpEndnoteProperties.npPosition= DPftnPOS_DOC_END;
    dp->dpEndnoteProperties.npRestart= DPftnRST_CONTINUOUS;
    dp->dpEndnoteProperties.npNumberStyle= DPftnNRLC;

    dp->dpHasFacingPages= 0;
    dp->dpWidowControl= 0;

    appInitDocumentGeometry( &(dp->dpGeometry) );

    docInitFontList( &(dp->dpFontList) );
    docInitListTable( &(dp->dpListTable) );
    docInitListOverrideTable( &(dp->dpListOverrideTable) );

    dp->dpColors= (RGB8Color *)0;
    dp->dpColorCount= 0;

    dp->dpAuthors= (unsigned char **)0;
    dp->dpAuthorCount= 0;

    dp->dpTitle= (unsigned char *)0;
    dp->dpSubject= (unsigned char *)0;
    dp->dpKeywords= (unsigned char *)0;
    dp->dpComment= (unsigned char *)0;
    dp->dpDoccomm= (unsigned char *)0;
    dp->dpAuthor= (unsigned char *)0;
    dp->dpHlinkbase= (unsigned char *)0;
    dp->dpFilename= (unsigned char *)0;

    appInvalidateTime( &(dp->dpCreatim) );
    appInvalidateTime( &(dp->dpRevtim) );
    appInvalidateTime( &(dp->dpPrintim) );
    }

void docCleanDocumentProperties(	DocumentProperties *	dp )
    {
    int		i;

    docCleanFontList( &(dp->dpFontList) );
    docCleanListTable( &(dp->dpListTable) );
    docCleanListOverrideTable( &(dp->dpListOverrideTable) );

    if  ( dp->dpColors )
	{ free( dp->dpColors );	}

    for ( i= 0; i < dp->dpAuthorCount; i++ )
	{ free( dp->dpAuthors[i] );	}
    if  ( dp->dpAuthors )
	{ free( dp->dpAuthors );	}

    appInvalidateTime( &(dp->dpCreatim) );
    appInvalidateTime( &(dp->dpRevtim) );
    appInvalidateTime( &(dp->dpPrintim) );

    if  ( dp->dpTitle )
	{ free( dp->dpTitle );	}
    if  ( dp->dpSubject )
	{ free( dp->dpSubject );	}
    if  ( dp->dpKeywords )
	{ free( dp->dpKeywords );	}
    if  ( dp->dpComment )
	{ free( dp->dpComment );	}
    if  ( dp->dpDoccomm )
	{ free( dp->dpDoccomm );	}
    if  ( dp->dpAuthor )
	{ free( dp->dpAuthor );	}
    if  ( dp->dpHlinkbase )
	{ free( dp->dpHlinkbase );	}
    if  ( dp->dpFilename )
	{ free( dp->dpFilename );	}

    return;
    }

int docCopyDocumentProperties(	DocumentProperties *		to,
				const DocumentProperties *	from )
    {
    int			i;
    DocumentProperties	copy;

    copy= *from;

    docInitFontList( &(copy.dpFontList) );
    docInitListTable( &(copy.dpListTable) );
    docInitListOverrideTable( &(copy.dpListOverrideTable) );

    copy.dpColorCount= 0;
    copy.dpColors= (RGB8Color *)0;
    copy.dpAuthorCount= 0;
    copy.dpAuthors= (unsigned char **)0;

    copy.dpTitle= (unsigned char *)0;
    copy.dpSubject= (unsigned char *)0;
    copy.dpKeywords= (unsigned char *)0;
    copy.dpComment= (unsigned char *)0;
    copy.dpDoccomm= (unsigned char *)0;
    copy.dpAuthor= (unsigned char *)0;
    copy.dpHlinkbase= (unsigned char *)0;
    copy.dpFilename= (unsigned char *)0;

    if  ( docCopyFontList( &(copy.dpFontList), &(from->dpFontList) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    if  ( docCopyListTable( &(copy.dpListTable), &(from->dpListTable) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    if  ( docCopyListOverrideTable( &(copy.dpListOverrideTable),
					    &(from->dpListOverrideTable) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    /**/
    copy.dpColors= (RGB8Color *)
			malloc( from->dpColorCount* sizeof(RGB8Color) );
    if  ( ! copy.dpColors )
	{
	XDEB(copy.dpColors);
	docCleanDocumentProperties( &copy ); return -1;
	}
    for ( i= 0; i < from->dpColorCount; i++ )
	{ copy.dpColors[i]= from->dpColors[i]; }
    copy.dpColorCount= from->dpColorCount;

    /**/
    copy.dpAuthors= (unsigned char **)
		    malloc( from->dpAuthorCount* sizeof(unsigned char *) );
    if  ( ! copy.dpAuthors )
	{
	XDEB(copy.dpAuthors);
	docCleanDocumentProperties( &copy ); return -1;
	}
    for ( i= 0; i < from->dpAuthorCount; i++ )
	{ copy.dpAuthors[i]= (unsigned char *)0; }
    copy.dpAuthorCount= from->dpAuthorCount;
    for ( i= 0; i < from->dpAuthorCount; i++ )
	{
	copy.dpAuthors[i]= (unsigned char *)
				    strdup( (char *)from->dpAuthors[i] );
	if  ( ! copy.dpAuthors[i] )
	    {
	    XDEB(copy.dpAuthors[i]);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpTitle )
	{
	copy.dpTitle= (unsigned char *)strdup( (char *)from->dpTitle );
	if  ( ! copy.dpTitle )
	    {
	    XDEB(copy.dpTitle);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpSubject )
	{
	copy.dpSubject= (unsigned char *)strdup( (char *)from->dpSubject );
	if  ( ! copy.dpSubject )
	    {
	    XDEB(copy.dpSubject);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpKeywords )
	{
	copy.dpKeywords= (unsigned char *)strdup( (char *)from->dpKeywords );
	if  ( ! copy.dpKeywords )
	    {
	    XDEB(copy.dpKeywords);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpComment )
	{
	copy.dpComment= (unsigned char *)strdup( (char *)from->dpComment );
	if  ( ! copy.dpComment )
	    {
	    XDEB(copy.dpComment);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpDoccomm )
	{
	copy.dpComment= (unsigned char *)strdup( (char *)from->dpDoccomm );
	if  ( ! copy.dpDoccomm )
	    {
	    XDEB(copy.dpDoccomm);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpAuthor )
	{
	copy.dpAuthor= (unsigned char *)strdup( (char *)from->dpAuthor );
	if  ( ! copy.dpAuthor )
	    {
	    XDEB(copy.dpAuthor);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpHlinkbase )
	{
	copy.dpHlinkbase= (unsigned char *)strdup( (char *)from->dpHlinkbase );
	if  ( ! copy.dpHlinkbase )
	    {
	    XDEB(copy.dpHlinkbase);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpFilename )
	{
	copy.dpFilename= (unsigned char *)strdup( (char *)from->dpFilename );
	if  ( ! copy.dpFilename )
	    {
	    XDEB(copy.dpFilename);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    docCleanDocumentProperties( to );
    *to= copy;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the file name in the document properties.			*/
/*									*/
/************************************************************************/

int docPropertiesSetFilename(	DocumentProperties *	dp,
				const char *		filename )
    {
    char *	saved= (char *)0;

    if  ( filename )
	{
	saved= strdup( filename );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( dp->dpFilename )
	{ free( dp->dpFilename );	}

    dp->dpFilename= (unsigned char *)saved;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Change document properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

static void docUpdNotesProperties(
			PropertyMask *			pChgMask,
			NotesProperties *		np,
			const PropertyMask *		updMask,
			const NotesProperties *		npNew,
			int				propStartNr,
			int				propPosition,
			int				propRestart,
			int				propStyle )
    {
    if  ( PROPmaskISSET( updMask, propStartNr ) )
	{
	if  ( np->npStartNumber != npNew->npStartNumber )
	    {
	    np->npStartNumber= npNew->npStartNumber;
	    PROPmaskADD( pChgMask, propStartNr );
	    }
	}

    if  ( PROPmaskISSET( updMask, propPosition ) )
	{
	if  ( np->npPosition != npNew->npPosition )
	    {
	    np->npPosition= npNew->npPosition;
	    PROPmaskADD( pChgMask, propPosition );
	    }
	}

    if  ( PROPmaskISSET( updMask, propRestart ) )
	{
	if  ( np->npRestart != npNew->npRestart )
	    {
	    np->npRestart= npNew->npRestart;
	    PROPmaskADD( pChgMask, propRestart );
	    }
	}

    if  ( PROPmaskISSET( updMask, propStyle ) )
	{
	if  ( np->npNumberStyle != npNew->npNumberStyle )
	    {
	    np->npNumberStyle= npNew->npNumberStyle;
	    PROPmaskADD( pChgMask, propStyle );
	    }
	}

    return;
    }

int docUpdDocumentProperties(	PropertyMask *			pChgMask,
				DocumentProperties *		dp,
				const PropertyMask *		updMask,
				const DocumentProperties *	dpNew )
    {
    PropertyMask	chgMask;
    PropertyMask	dgMask;

    PROPmaskCLEAR( &chgMask );

    PROPmaskCLEAR( &dgMask );
    PROPmaskFILL( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, updMask );

    appSetDocumentGeometry( &(dp->dpGeometry), &(dpNew->dpGeometry),
							&chgMask, &dgMask );

    if  ( PROPmaskISSET( updMask, DPpropSTART_PAGE ) )
	{
	if  ( dp->dpStartPageNumber != dpNew->dpStartPageNumber )
	    {
	    dp->dpStartPageNumber= dpNew->dpStartPageNumber;
	    PROPmaskADD( &chgMask, DPpropSTART_PAGE );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropFACING_PAGES ) )
	{
	if  ( dp->dpHasFacingPages != dpNew->dpHasFacingPages )
	    {
	    dp->dpHasFacingPages= dpNew->dpHasFacingPages;
	    PROPmaskADD( &chgMask, DPpropFACING_PAGES );
	    }
	}

    docUpdNotesProperties( &chgMask, &(dp->dpFootnoteProperties),
				updMask, &(dpNew->dpFootnoteProperties),
				DPpropFOOTNOTE_STARTNR,
				DPpropFOOTNOTE_POSITION,
				DPpropFOOTNOTE_RESTART,
				DPpropFOOTNOTE_STYLE );

    docUpdNotesProperties( &chgMask, &(dp->dpEndnoteProperties),
				updMask, &(dpNew->dpEndnoteProperties),
				DPpropENDNOTE_STARTNR,
				DPpropENDNOTE_POSITION,
				DPpropENDNOTE_RESTART,
				DPpropENDNOTE_STYLE );

    if  ( PROPmaskISSET( updMask, DPpropDEFTAB ) )
	{
	if  ( dp->dpTabIntervalTwips != dpNew->dpTabIntervalTwips )
	    {
	    dp->dpTabIntervalTwips= dpNew->dpTabIntervalTwips;
	    PROPmaskADD( &chgMask, DPpropDEFTAB );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropANSICPG ) )
	{
	if  ( dp->dpAnsiCodepage != dpNew->dpAnsiCodepage )
	    {
	    dp->dpAnsiCodepage= dpNew->dpAnsiCodepage;
	    PROPmaskADD( &chgMask, DPpropANSICPG );
	    }
	}

    *pChgMask= chgMask;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a color.							*/
/*									*/
/************************************************************************/

int docAllocateDocumentColor(		DocumentProperties *	dp,
					const RGB8Color *	rgb8 )
    {
    const int			maxColors= 256;
    const int			avoidZero= 1;
    int				color;

    color= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
						avoidZero, maxColors, rgb8 );
    if  ( color < 0 )
	{ LDEB(color); return -1;	}

    return color;
    }

