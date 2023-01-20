/************************************************************************/
/*									*/
/*  Pdfmark related PostScript generation.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"psPrint.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Pdfmark functionality around marked content.			*/
/*									*/
/************************************************************************/

int psNewDocContentId(	PrintingState *		ps )
    {
    return ps->psDocContentMarkCount++;
    }

int psNewPageContentId(	PrintingState *		ps )
    {
    return ps->psPageContentMarkCount++;
    }

/************************************************************************/
/*									*/
/*  Mark a position in the document.					*/
/*									*/
/************************************************************************/

int psPdfMarkPosition(		PrintingState *		ps,
				const char *		roleTag,
				int			contentId )
    {
    if  ( contentId < 0 )
	{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s /MP pdfmark\n", roleTag ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s << /DP %d >> /BDC pdfmark\n",
		roleTag, contentId ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Begin/End a piece of marked content.				*/
/*									*/
/************************************************************************/

int psPdfBeginMarkedContent(	PrintingState *		ps,
				const char *		roleTag,
				int			contentId )
    {
    if  ( contentId < 0 )
	{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s /BMC pdfmark\n", roleTag ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s << /MCID %d >> /BDC pdfmark\n",
			roleTag, contentId ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int psPdfBeginArtifact( PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName,
			int				contentId )
    {
    const char * const	roleTag= "Artifact";

    /* The propertList in ISO 32000-1:2008, 14.8.2.2.2 is a dictionary. See 14.6.2 */

    if  ( sioOutPrintf( ps->psSos, "[ /%s << ", roleTag ) < 0 )
	{ LDEB(1); return -1;	}

    if  ( typeName )
	{
	if  ( sioOutPrintf( ps->psSos, " /Type /%s ", typeName ) < 0 )
	    { SDEB(typeName); return -1;	}
	}

    if  ( subtypeName )
	{
	if  ( sioOutPrintf( ps->psSos, " /Subtype /%s ", subtypeName ) < 0 )
	    { SDEB(subtypeName); return -1;	}
	}

    if  ( contentId >= 0 )
	{
	if  ( sioOutPrintf( ps->psSos, " /MCID %d ", contentId ) < 0 )
	    { LDEB(contentId); return -1;	}
	}

    if  ( sioOutPrintf( ps->psSos, " >> /BDC pdfmark\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

int psPdfBeginFigure(	PrintingState *			ps,
			const MemoryBuffer *		altText,
			int				contentId )
    {
    const char * const	roleTag= "Figure";

    /* The propertList in ISO 32000-1:2008, 14.8.2.2.2 is a dictionary. See 14.6.2 */

    if  ( sioOutPrintf( ps->psSos, "[ /%s << ", roleTag ) < 0 )
	{ LDEB(1); return -1;	}

    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	if  ( sioOutPrintf( ps->psSos, " /Alt /%s ", utilMemoryBufferGetString( altText ) ) < 0 )
	    { XDEB(altText); return -1;	}
	}

    if  ( contentId >= 0 )
	{
	if  ( sioOutPrintf( ps->psSos, " /MCID %d ", contentId ) < 0 )
	    { LDEB(contentId); return -1;	}
	}

    if  ( sioOutPrintf( ps->psSos, " >> /BDC pdfmark\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

int psPdfEndMarkedContent(		PrintingState *		ps )
    {
    if  ( sioOutPrintf( ps->psSos, "[ /EMC pdfmark\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

int psPdfmarkMarkedDocumentSetup( PrintingState *		ps,
				const char *			localeTag )
    {
    sioOutPrintf( ps->psSos,
	    "[ {Catalog} << /MarkInfo <</Marked true>> >> /PUT pdfmark\n" );

    if  ( localeTag && psSetCatalogProperty( ps, "Lang", localeTag ) )
	{ SDEB(localeTag);	}


    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ /_objdef {DTREE} /type /array /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ /_objdef {NTREE} /type /dict /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ /_objdef {NTREEA} /type /array /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ /_objdef {DOC} /type /dict /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ /_objdef {STR} /type /dict /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos,
		"[ {STR} <</Type /StructTreeRoot /K {DOC} /ParentTree {NTREE}>> /PUT pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ {DOC} <</S /Document /P {STR} /K {DTREE}>> /PUT pdfmark\n" );

    sioOutPrintf( ps->psSos,
	    "[ {Catalog} <</ViewerPreferences <</DisplayDocTitle true>>>> /PUT pdfmark\n" );

    return 0;
    }

int psPdfmarkMarkedPageSetup(	PrintingState *		ps,
				int			page )
    {
    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ /_objdef {PTREE%d} /type /array /OBJ pdfmark\n", page );
    sioOutPrintf( ps->psSos, "[ /_objdef {PANN%d} /type /array /OBJ pdfmark\n", page );

    ps->psPageFirstMarkId= ps->psDocContentMarkCount;
    ps->psPageContentMarkCount= 0;

    return 0;
    }

int psPdfmarkAppendContentToReadingOrder(
				PrintingState *		ps,
				const char *		roleTag,
				int			page,
				int			docContentId,
				int			pageContentId )
    {
    char	itemDict[50];

    sprintf( itemDict, "TedRo%d", docContentId );

    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
				    itemDict );
    sioOutPrintf( ps->psSos, "[ {%s} <</S /%s /P {DOC} /K %d /Pg {ThisPage}>> /PUT pdfmark\n",
				    itemDict, roleTag, pageContentId );
    sioOutPrintf( ps->psSos, "[ {DTREE} {%s} /APPEND pdfmark\n",
				    itemDict );
    sioOutPrintf( ps->psSos, "[ {PTREE%d} {%s} /APPEND pdfmark\n",
				    page, itemDict );

    return 0;
    }

int psPdfmarkFinishMarkedPage(	PrintingState *		ps,
				int			page )
    {
    sioOutPrintf( ps->psSos, "%% finishing page %d\n", page );

    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ {ThisPage} <</StructParents %d>> /PUT pdfmark\n", page );
    sioOutPrintf( ps->psSos, "[ {ThisPage} <</Annots {PANN%d}>> /PUT pdfmark\n", page );
    sioOutPrintf( ps->psSos, "[ {ThisPage} <</Tabs /S>> /PUT pdfmark\n" );

    sioOutPrintf( ps->psSos, "[ {NTREEA} %d /APPEND pdfmark\n", page );
    sioOutPrintf( ps->psSos, "[ {NTREEA} {PTREE%d} /APPEND pdfmark\n", page );

    return 0;
    }

int psPdfmarkMarkedDocumentTrailer( PrintingState *		ps )
    {

    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ {NTREE} <</Nums {NTREEA}>> /PUT pdfmark\n" );
    sioOutPrintf( ps->psSos, "[ {Catalog} <</StructTreeRoot {STR}>> /PUT pdfmark\n" );

    return 0;
    }

