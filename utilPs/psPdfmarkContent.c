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

/**
 * Return a new contentId. ISO 32000-1:2008, 14.7.4.2 requires this to
 * be unique in the content stream. As we have one content stream per
 * page, we distribute unique numbers per page. (Why must they be contiguous?)
 */
int psNewContentId(	PrintingState *		ps )
    {
    return ps->psPageContentMarkCount++;
    }

/************************************************************************/
/*									*/
/*  Mark a position in the document.					*/
/*									*/
/************************************************************************/

int psPdfMarkPosition(		PrintingState *		ps,
				const char *		structureType,
				int			contentId )
    {
    if  ( contentId < 0 )
	{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s /MP pdfmark\n", structureType ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s << /DP %d >> /BDC pdfmark\n",
		structureType, contentId ) < 0 )
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
				const char *		structureType,
				int			contentId )
    {
    if  ( contentId < 0 )
	{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s /BMC pdfmark\n", structureType ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s << /MCID %d >> /BDC pdfmark\n",
			structureType, contentId ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int psPdfBeginArtifact( PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName,
			int				contentId )
    {
    const char * const	structureType= "Artifact";

    /* The propertList in ISO 32000-1:2008, 14.8.2.2.2 is a dictionary. See 14.6.2 */

    if  ( sioOutPrintf( ps->psSos, "[ /%s << ", structureType ) < 0 )
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
    const char * const	structureType= "Figure";

    /* The propertList in ISO 32000-1:2008, 14.8.2.2.2 is a dictionary. See 14.6.2 */

    if  ( sioOutPrintf( ps->psSos, "[ /%s << ", structureType ) < 0 )
	{ LDEB(1); return -1;	}

    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	if  ( sioOutPrintf( ps->psSos, " /Alt " ) < 0 )
	    { XDEB(altText); return -1;	}

	if  ( psPrintPdfMarkStringValue( ps, altText ) < 0 )
	    { XDEB(altText); return -1;	}

	if  ( sioOutPrintf( ps->psSos, " " ) < 0 )
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

/************************************************************************/
/*									*/
/*  Populate a StructItem as per table 323 in ISO 32000-1:2008 (14.7.4) */
/*									*/
/************************************************************************/

static int psPdfmarkDefineLeafStructItem(
				PrintingState *		ps,
				const char *		itemDict,
				const char *		parentDict,
				const char *		structureType,
				int			pageContentId )
    {
    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	itemDict );

    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K %d /Pg {ThisPage}>> /PUT pdfmark\n",
	itemDict,
	structureType,
	parentDict,
	pageContentId );

    return 0;
    }

static int psPdfmarkPopulateParentStructItem(
				PrintingState *		ps,
				const char *		itemDict,
				const char *		parentDict,
				const char *		structureType,
				const char *		childObjectName )
    {
    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K {%s}>> /PUT pdfmark\n",
	itemDict,
	structureType,
	parentDict,
	childObjectName );

    return 0;
    }

/**
 * Populate the StructTreeRoot of the document.
 * See table 3.2.2 in ISO 32000-1:2008, (14.7.2)
 */
static int psPdfmarkPopulateStructTreeRoot(
				PrintingState *		ps,
				const char *		rootDict,
				const char *		parentTree,
				const char *		childObjectName )
    {
    sioOutPrintf( ps->psSos,
	"[ {%s} <</Type /StructTreeRoot /K {%s} /ParentTree {%s}>> /PUT pdfmark\n",
	rootDict,
	childObjectName,
	parentTree );

    return 0;
    }


			/**
			 * The name of the StructItem dictionary that is
			 * the root item of the document as a whole.
			 */
static const char PS_DOC_STRUCT_ITEM[]= "DOC";

			/**
			 * The name of the StructTreeRoot dictionary of the
			 * document as a whole.
			 */
static const char PS_DOC_STRUCT_TREE_ROOT[]= "STR";

			/**
			 * The name of the number tree root in the document
			 * StructTreeRoot that holds the struct items that
			 * correspond to the pages.  (Correct?)
			 */
static const char PS_DOC_NUMBER_TREE_ROOT[]= "NTREE";

			/**
			 * The name of the Nums array in the number tree root
			 * in the document .
			 */
static const char PS_DOC_NUMBER_TREE_NUMS[]= "NTREEA";

int psPdfmarkMarkedDocumentSetup( PrintingState *		ps,
				const char *			localeTag )
    {
    sioOutPrintf( ps->psSos,
	    "[ {Catalog} << /MarkInfo <</Marked true>> >> /PUT pdfmark\n" );

    if  ( localeTag && psSetCatalogProperty( ps, "Lang", localeTag ) )
	{ SDEB(localeTag);	}


    /* God knows what this means */
    sioOutPrintf( ps->psSos, "[ /_objdef {DTREE} /type /array /OBJ pdfmark\n" );
    sioOutPrintf( ps->psSos,
    	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	PS_DOC_NUMBER_TREE_ROOT );
    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /array /OBJ pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS );

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	PS_DOC_STRUCT_ITEM );

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	PS_DOC_STRUCT_TREE_ROOT );

    psPdfmarkPopulateStructTreeRoot( ps,
	PS_DOC_STRUCT_TREE_ROOT, PS_DOC_NUMBER_TREE_ROOT, PS_DOC_STRUCT_ITEM );

    psPdfmarkPopulateParentStructItem( ps,
	PS_DOC_STRUCT_ITEM, PS_DOC_STRUCT_TREE_ROOT,
	"Document", "DTREE" );

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
				const char *		structureType,
				int			page,
				int			pageContentId )
    {
    int			docContentId= ps->psDocContentMarkCount++;
    char		itemDict[50];

    sprintf( itemDict, "TedRo%d", docContentId );

    /* Define and populate a StructItem */
    psPdfmarkDefineLeafStructItem( ps,
		    itemDict, PS_DOC_STRUCT_ITEM,
		    structureType, pageContentId );

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

    sioOutPrintf( ps->psSos,
	"[ {%s} %d /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, page );
    sioOutPrintf( ps->psSos,
    	"[ {%s} {PTREE%d} /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, page );

    return 0;
    }

/**
 * Finish the document structure
 * 1) Insert the Nums array in the root of the structure number tree
 * 2) Insert the structure number tree in the StructTreeRoot of the document.
 */
int psPdfmarkMarkedDocumentTrailer( PrintingState *		ps )
    {

    /* 1 */
    sioOutPrintf( ps->psSos,
	    "[ {%s} <</Nums {%s}>> /PUT pdfmark\n",
	    PS_DOC_NUMBER_TREE_ROOT, PS_DOC_NUMBER_TREE_NUMS );
    /* 2 */
    sioOutPrintf( ps->psSos, "[ {Catalog} <</StructTreeRoot {%s}>> /PUT pdfmark\n",
	    PS_DOC_STRUCT_TREE_ROOT );

    return 0;
    }

