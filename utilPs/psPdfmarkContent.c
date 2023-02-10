/************************************************************************/
/*									*/
/*  Pdfmarks for structured PDF related functionality.			*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"psPrint.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

			/**
			 * The name of the StructTreeRoot dictionary of the
			 * document as a whole.
			 */
static const char PS_DOC_STRUCT_TREE_ROOT[]= "STR";

			/**
			 * The name of the StructItem dictionary that is
			 * the root item of the document as a whole.
			 */
static const char PS_DOC_STRUCT_ITEM[]= "DOC";

			/**
			 * The name of the /K child array of the PS_DOC_STRUCT_ITEM
			 * StructItem for the document as a whole.
			 */
static const char PS_DOC_STRUCT_ITEM_KIDS[]= "DTREE";

			/**
			 * The name of the number tree root in the document
			 * StructTreeRoot that only holds the leaf
			 * StructItems that refer to a marked content or other
			 * objects. See NTREEA and PTREEA%d.
			 */
static const char PS_DOC_NUMBER_TREE_ROOT[]= "NTREE";

			/**
			 * The name of the Nums array in the number tree root
			 * in the document.
			 * The array is a sequence of tuples consisting of a number and an object.
			 * The number is a unique counter starting from 0.
			 * In case of a page content, the object is an array of only the leaf StructItems
			 * that refer to that page. (/K equals to a ContentId)
			 * In case of an annotation, the object is the reference to the annotation.
			 */
static const char PS_DOC_NUMBER_TREE_NUMS[]= "NTREEA";

			/**
			 * The name, unique per page, of the array that contains only the leaf StuctItems
			 * of a page marked content. To be used at the end of the page for populating NTREEA.
			 *
			 * Often, all the StuctItems (/P, /Link, /Figure /Span) are leaves.
			 * So for each marked content the same StructItem is put in DTREE and PTREEA%d
			 *
			 * The ParentTree is the only way to find the corresponding StuctItem starting with a contentId.
			 * A pdf reader accesses the Structure Tree using the value of Page -> StructParents as the number
			 * and contentId as an index in PTREE%d.
			 *
			 * In case of an annotation the number is taken from Annotation -> StructParent
			 * and the object of the tuple is the Annotation.
			 *
			 * See 14.7.4.4 for the Adobe description.
			 *
			 * This is used as an sprintf format for the page number.
			 */

static const char PS_DOC_PAGE_PARENT_TREE[]= "PTREEA%d";

			/**
			 * The name, unique per page, of the array that contains the annotations
			 * of the page.
			 * To be used at the end of the page for populating {ThisPage} -> Annotations.
			 *
			 * This is used as an sprintf format for the page number.
			 */
static const char PS_DOC_PAGE_ANNOTATIONS[]= "PANN%d";

/************************************************************************/
/*									*/
/*  Pdfmark functionality around marked content.			*/
/*									*/
/************************************************************************/

/**
 * Return a new contentId. ISO 32000-1:2008, 14.7.4.2 requires this to
 * be unique in the content stream. As we have one content stream per
 * page, we distribute unique numbers per page.
 */
int psNewContentId(	PrintingState *		ps )
    {
    return ps->psPageContentMarkCount++;
    }

StructItem * psPdfLeafStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine,
				int			contentId )
    {
    const int		uniqueDictId= ps->psDocContentMarkCount++;
    StructItem *	structItem= malloc( sizeof(StructItem) );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName), "TedRo%d", uniqueDictId );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 1;
    structItem->siIsInline= inLine;

    return structItem;
    }

StructItem * psPdfAnnotatedStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			inLine,
				int			contentId )
    {
    StructItem *	structItem= malloc( sizeof(StructItem) );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName), "TedRo%d", ps->psDocContentMarkCount++ );
    utilMemoryBufferPrintf( &(structItem->siAnnotationDictionaryName), "TedAn%d", ps->psDocContentMarkCount++ );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 1;
    structItem->siIsInline= inLine;

    return structItem;
    }

StructItem * psPdfGroupStructItem(
				PrintingState *		ps,
				const char *		structureType,
				int			contentId )
    {
    const int		uniqueDictId= ps->psDocContentMarkCount++;
    StructItem *	structItem= malloc( sizeof(StructItem) );

    if  ( ! structItem )
	{ XDEB(structItem); return structItem;	}

    psPdfInitStructItem( structItem );

    utilMemoryBufferPrintf( &(structItem->siDictionaryName), "TedGr%d", uniqueDictId );
    utilMemoryBufferPrintf( &(structItem->siChildArrayName), "TedGr%dK", uniqueDictId );
    structItem->siStructureType= structureType;
    structItem->siContentId= contentId;
    structItem->siIsLeaf= 0;
    structItem->siIsInline= 0;

    return structItem;
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
			const char *			subtypeName )
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

/**
 *  Create and populate the common members of a simple leaf StructItem
 *  as per table 323 in ISO 32000-1:2008 (14.7.4)
 *  As a leaf refers to actually marked content, /K receives the contentId.
 */
static int psPdfmarkOpenLeafStructItem(
				PrintingState *		ps,
				StructItem *		structItem )
    {
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentDict= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siDictionaryName) ) :
				PS_DOC_STRUCT_ITEM;

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	itemDict );

    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K %d /Pg {ThisPage} ",
	itemDict,
	structItem->siStructureType,
	parentDict,
	structItem->siContentId );

    return 0;
    }

/**
 *  Create and populate the common menbers of a StructItem that is meant
 *  to receive children.  See table 323 in ISO 32000-1:2008 (14.7.4)
 *  As a group refers to multiple children, /K refers to an array that
 *  receives the children.
 *  We can safely include a reference to the current page as well: It
 *  is only required that some of the content referenced by /K is on
 *  the page referenced by /Pg.
 */
static int psPdfmarkOpenGroupStructItem(
				PrintingState *		ps,
				StructItem *		structItem )
    {
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentDict= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siDictionaryName) ) :
				PS_DOC_STRUCT_ITEM;
    const char * const	childArray= utilMemoryBufferGetString( &(structItem->siChildArrayName) );

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	itemDict );
    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /array /OBJ pdfmark\n",
	childArray );

    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K {%s} /Pg {ThisPage} ",
	itemDict,
	structItem->siStructureType,
	parentDict,
	childArray );

    return 0;
    }

static int psPdfmarkCloseStructItem(
				PrintingState *		ps )
    {
    if  ( sioOutPrintf( ps->psSos, " >> /PUT pdfmark\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

/**
 *  Populate a StructItem as per table 323 in ISO 32000-1:2008 (14.7.4)
 *  The StructItem is a simple leaf in the structure hierarchy. It will
 *  not receive children.
 */
static int psPdfmarkDefineLeafStructItem(
				PrintingState *		ps,
				StructItem *		structItem )
    {
    if  ( psPdfmarkOpenLeafStructItem( ps, structItem ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    if  ( psPdfmarkCloseStructItem( ps ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    return 0;
    }

/**
 *  Populate a StructItem as per table 323 in ISO 32000-1:2008 (14.7.4)
 *  The StructItem is a group. It is intended to receive children.
 */
static int psPdfmarkDefineGroupStructItem(
				PrintingState *		ps,
				StructItem *		structItem )
    {
    if  ( psPdfmarkOpenGroupStructItem( ps, structItem ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    if  ( psPdfmarkCloseStructItem( ps ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    return 0;
    }

static int psPdfmarkPopulateGroupStructItem(
				PrintingState *		ps,
				const char *		itemDict,
				const char *		parentDict,
				const char *		structureType,
				const char *		childArrayName )
    {
    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K {%s}>> /PUT pdfmark\n",
	itemDict,
	structureType,
	parentDict,
	childArrayName );

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


int psPdfmarkMarkedDocumentSetup( PrintingState *		ps,
				const char *			localeTag )
    {
    sioOutPrintf( ps->psSos,
	    "[ {Catalog} << /MarkInfo <</Marked true>> >> /PUT pdfmark\n" );

    if  ( localeTag && psSetCatalogProperty( ps, "Lang", localeTag ) )
	{ SDEB(localeTag);	}


    /* God knows what this means */
    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /array /OBJ pdfmark\n", PS_DOC_STRUCT_ITEM_KIDS );
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

    psPdfmarkPopulateGroupStructItem( ps,
	PS_DOC_STRUCT_ITEM, PS_DOC_STRUCT_TREE_ROOT,
	"Document", PS_DOC_STRUCT_ITEM_KIDS );

    sioOutPrintf( ps->psSos,
	    "[ {Catalog} <</ViewerPreferences <</DisplayDocTitle true>>>> /PUT pdfmark\n" );

    return 0;
    }

int psPdfmarkMarkedPageSetup(	PrintingState *		ps,
				int			page )
    {
    char	pageParentTreeName[50];
    char	annotationArrayName[50];

    sprintf( pageParentTreeName, PS_DOC_PAGE_PARENT_TREE, page );
    sprintf( annotationArrayName, PS_DOC_PAGE_ANNOTATIONS, page );

    sioOutPrintf( ps->psSos,
	    "[ /_objdef {%s} /type /array /OBJ pdfmark\n", pageParentTreeName );
    sioOutPrintf( ps->psSos,
	    "[ /_objdef {%s} /type /array /OBJ pdfmark\n", annotationArrayName );

    ps->psPageFirstMarkId= ps->psDocContentMarkCount;
    ps->psPageContentMarkCount= 0;
    ps->psPageAnnotationCount= 0;

    return 0;
    }

/**
 *  Insert a StructItem in the hierarchy. It already has been
 *  defined. So all names are known.
 */
static int psPdfmarkAppendDefinedItem(
				PrintingState *		ps,
				StructItem *		structItem )
    {
    char	pageParentTreeName[50];

    const int		page= ps->psSheetsPrinted;
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentArray= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siChildArrayName) ) :
				PS_DOC_STRUCT_ITEM_KIDS;

    sprintf( pageParentTreeName, PS_DOC_PAGE_PARENT_TREE, page );

    sioOutPrintf( ps->psSos, "[ {%s} {%s} /APPEND pdfmark\n",
				    parentArray, itemDict );
    sioOutPrintf( ps->psSos, "[ {%s} {%s} /APPEND pdfmark\n",
				    pageParentTreeName, itemDict );

    return 0;
    }

/**
 *  Define and populate a simple StructItem object in the output.
 */
int psPdfmarkAppendMarkedLeaf(	PrintingState *		ps,
				StructItem *		structItem )
    {
    psPdfmarkDefineLeafStructItem( ps, structItem );

    return psPdfmarkAppendDefinedItem( ps, structItem );
    }

/**
 *  Define and populate StructItem object that potentially
 *  receives children in the output.
 */
int psPdfmarkAppendMarkedGroup(	PrintingState *		ps,
				StructItem *		structItem )
    {
    psPdfmarkDefineGroupStructItem( ps, structItem );

    return psPdfmarkAppendDefinedItem( ps, structItem );
    }

/**
 *  Define and populate an illustration simple StructItem object in the output.
 *  See ISO 32000-1:2008, 14.8.4.5
 */
int psPdfmarkAppendMarkedIllustration(
			PrintingState *			ps,
			StructItem *			structItem,
			const DocumentRectangle *	drTwips,
			const MemoryBuffer *		altText )
    {
    if  ( psPdfmarkOpenLeafStructItem( ps, structItem ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    if  ( altText && ! utilMemoryBufferIsEmpty( altText ) )
	{
	if  ( sioOutPrintf( ps->psSos, " /Alt " ) < 0 )
	    { XDEB(altText); return -1;	}

	if  ( psPrintPdfMarkStringValue( ps, altText ) < 0 )
	    { XDEB(altText); return -1;	}

	if  ( sioOutPrintf( ps->psSos, " " ) < 0 )
	    { XDEB(altText); return -1;	}
	}

    if  ( drTwips )
	{
	DocumentRectangle	dr;

	geoTransformRectangle( &dr, drTwips, &(ps->psCurrentTransform) );

	if  ( sioOutPrintf( ps->psSos,
		"/A <</BBox [%d %d %d %d] /Placement /Block /O /Layout>>",
		dr.drX0, dr.drY0, dr.drX1, dr.drY1 ) < 0 )
	    { XDEB(drTwips); return -1;	}
	}

    if  ( psPdfmarkCloseStructItem( ps ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    return psPdfmarkAppendDefinedItem( ps, structItem );
    }

int psPdfmarkAppendMarkedLink(	PrintingState *		ps,
				StructItem *		structItem )
    {
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentDict= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siDictionaryName) ) :
				PS_DOC_STRUCT_ITEM;
    const char * const	annotationDictionaryName=
				utilMemoryBufferGetString( &(structItem->siAnnotationDictionaryName) );

    const int	page= ps->psSheetsPrinted;
    char	annotationArrayName[50];
    int 	pageAnnotationReference;

    pageAnnotationReference= psPdfmarkDefineAnnotationDictionary( ps,
				    &(ps->psLinkFile), &(ps->psLinkMark),
				    annotationDictionaryName );
    if  ( pageAnnotationReference < 0 )
	{ LDEB(pageAnnotationReference); return -1;	}

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	itemDict );

    sioOutPrintf( ps->psSos,
	"[ {%s} <</S /%s /P {%s} /K [ <</Obj {%s} /Pg {ThisPage} /Type /OBJR>> %d ] /Pg {ThisPage} ",
	itemDict,
	structItem->siStructureType,
	parentDict,
	utilMemoryBufferGetString( &(structItem->siAnnotationDictionaryName) ),
	structItem->siContentId );

    if  ( psPdfmarkCloseStructItem( ps ) )
	{ SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

    sprintf( annotationArrayName, PS_DOC_PAGE_ANNOTATIONS, page );

    sioOutPrintf( ps->psSos,
	"[ {%s} {%s} /APPEND pdfmark\n",
	annotationArrayName, annotationDictionaryName );

    return psPdfmarkAppendDefinedItem( ps, structItem );
    }

int psPdfmarkFinishMarkedPage(	PrintingState *		ps,
				int			page )
    {
    char	pageParentTreeName[50];
    char	annotationArrayName[50];

    sprintf( pageParentTreeName, PS_DOC_PAGE_PARENT_TREE, page );
    sprintf( annotationArrayName, PS_DOC_PAGE_ANNOTATIONS, page );

    sioOutPrintf( ps->psSos, "%% finishing page %d\n", page );

    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</StructParents %d>> /PUT pdfmark\n", page );
    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</Annots {%s}>> /PUT pdfmark\n",
	    annotationArrayName );
    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</Tabs /S>> /PUT pdfmark\n" );

    sioOutPrintf( ps->psSos,
	"[ {%s} %d /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, page );
    sioOutPrintf( ps->psSos,
    	"[ {%s} {%s} /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, pageParentTreeName );

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

