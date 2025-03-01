/************************************************************************/
/*									*/
/*  Pdfmarks for structured PDF related functionality.			*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"psPrint.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>

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
			 * The name of the Nums array in the number tree root in the document.
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
			 * The ParentTree is the only way to find the corresponding StuctItem starting with a
			 * contentId. A pdf reader accesses the Structure Tree using the value of
			 * Page -> StructParents as the number and contentId as an index in PTREE%d.
			 *
			 * In case of an annotation the number is taken from Annotation -> StructParent
			 * and the object of the tuple is the Annotation.
			 *
			 * See 14.7.4.4 for the Adobe description.
			 *
			 * This is used as an sprintf format for the page number.
			 */

static const char PS_PAGE_LEAF_ARRAY_NAME[]= "PTREEA%d";

			/**
			 * The name, unique per page, of the array that contains the annotations
			 * of the page.
			 * To be used at the end of the page for populating {ThisPage} -> Annotations.
			 *
			 * This is used as an sprintf format for the page number.
			 */
static const char PS_PAGE_ANNOTATION_ARRAY_NAME[]= "PANN%d";

/************************************************************************/
/*									*/
/*  Mark a position in the document.					*/
/*									*/
/************************************************************************/

int psPdfMarkPosition(		PrintingState *		ps,
				const char *		structureType,
				int			contentId )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( contentId < 0 )
	{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s /MP pdfmark\n", structureType ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
		"[ /%s << /MCID %d >> /DP pdfmark\n",
		structureType, contentId ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/

static int psPdfmarkSetText(
			PrintingState *			ps,
			const char *			key,
			const MemoryBuffer *		text )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( text && ! utilMemoryBufferIsEmpty( text )		&&
	  psPrintPdfmarkTextEntry( ps->psSos, key, text )	)
	{ SDEB(key); return -1;	}

    return 0;
    }

static int psPdfmarkSetAltText(
			PrintingState *			ps,
			const MemoryBuffer *		altText )
    { return psPdfmarkSetText( ps, "Alt", altText );	}

static int psPdfmarkSetActualText(
			PrintingState *			ps,
			const MemoryBuffer *		actualText )
    { return psPdfmarkSetText( ps, "ActualText", actualText );	}

/************************************************************************/
/*									*/
/*  Begin/End a piece of marked content.				*/
/*									*/
/************************************************************************/

int psPdfBeginMarkedContent(	PrintingState *		ps,
				const char *		structureType,
				int			contentId,
				const MemoryBuffer *	extraProperties )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( extraProperties && utilMemoryBufferIsEmpty( extraProperties ) )
	{ extraProperties= (MemoryBuffer *)0;	}

    if  ( contentId < 0 && ! extraProperties )
	{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s /BMC pdfmark %% {\n", structureType ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( sioOutPrintf( ps->psSos,
			"[ /%s << ", structureType ) < 0 )
	    { SDEB(structureType); return -1;	}

	if  ( contentId >= 0 && sioOutPrintf( ps->psSos,
			"/MCID %d ", contentId ) < 0 )
	    { LDEB(contentId); return -1;	}

	if  ( extraProperties )
	    {
	    sioOutPrintf( ps->psSos,
		"%s", utilMemoryBufferGetString( extraProperties ) );
	    }

	if  ( sioOutPrintf( ps->psSos,
			">> /BDC pdfmark %% {\n" ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int psPdfBeginMarkedContentActualText(
				PrintingState *		ps,
				const char *		structureType,
				int			contentId,
				const MemoryBuffer *	actualText )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( sioOutPrintf( ps->psSos,
		    "[ /%s << /MCID %d ",
		    structureType, contentId ) < 0 )
	{ LDEB(1); return -1;	}

    /* Studioform extension. Does no harm for others */
    if  ( psPdfmarkSetActualText( ps, actualText ) )
	{ LDEB(1); return -1;	}

    if  ( sioOutPrintf( ps->psSos,
		    " >> /BDC pdfmark %% {\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

int psPdfBeginMarkedFigure(
			PrintingState *				ps,
			const char *				structureType,
			int					contentId,
			const struct DocumentRectangle *	drTwips,
			const struct MemoryBuffer *		altText )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( sioOutPrintf( ps->psSos,
		    "[ /%s << /MCID %d ",
		    structureType, contentId ) < 0 )
	{ LDEB(1); return -1;	}

    /* Studioform extension. Does no harm for others */
    if  ( psPdfmarkSetAltText( ps, altText ) )
	{ LDEB(1); return -1;	}

    /* Studioform extension. Does no harm for others */
    if  ( drTwips )
	{
	DocumentRectangle	dr;

	geoTransformRectangle( &dr, drTwips, &(ps->psCurrentTransform) );

	if  ( sioOutPrintf( ps->psSos,
		"/BBox [%d %d %d %d] ",
		dr.drX0, dr.drY0, dr.drX1, dr.drY1 ) < 0 )
	    { XDEB(drTwips); return -1;	}
	}

    if  ( sioOutPrintf( ps->psSos,
		    " >> /BDC pdfmark %% {\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

/**
 * Begin an artifact.
 * The propertList in ISO 32000-1:2008, 14.8.2.2.2 is a dictionary. See 14.6.2
 */
int psPdfBeginTypedArtifact(
			PrintingState *			ps,
			const char *			typeName,
			const char *			subtypeName )
    {
    const char * const	structureType= "Artifact";

    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

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

    if  ( sioOutPrintf( ps->psSos, " >> /BDC pdfmark %% {\n" ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

/**
 *  End a piece of marked content.
 *  @param structureType The type of structure that we end. (For debugging)
 */
int psPdfEndMarkedContent(
	    PrintingState *		ps,
	    const char *		structureType )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( sioOutPrintf( ps->psSos, "[ /EMC pdfmark %% %s }\n", structureType ) < 0 )
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
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
    {
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentDict= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siDictionaryName) ) :
				PS_DOC_STRUCT_ITEM;

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	itemDict );

    sioOutPrintf( ps->psSos,
	"[ {%s} <</Type /StructElem /S /%s /P {%s} /K %d /Pg {ThisPage} ",
	itemDict,
	structItem->siStructureType,
	parentDict,
	structItem->siContentId );

    if  ( structureAttributes && ! utilMemoryBufferIsEmpty( structureAttributes ) )
	{
	sioOutPrintf( ps->psSos,
		"/A << %s >> ", utilMemoryBufferGetString( structureAttributes ) );
	}

    return 0;
    }

/**
 *  Create and populate the common members of a StructItem that is meant
 *  to receive children.  See table 323 in ISO 32000-1:2008 (14.7.4)
 *  As a group refers to multiple children, /K refers to an array that
 *  receives the children.
 *  We can safely include a reference to the current page as well: It
 *  is only required that some of the content referenced by /K is on
 *  the page referenced by /Pg.
 */
static int psPdfmarkOpenGroupStructItem(
				PrintingState *		ps,
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
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
	"[ {%s} <</Type /StructElem /S /%s /P {%s} /K {%s} /Pg {ThisPage} ",
	itemDict,
	structItem->siStructureType,
	parentDict,
	childArray );

    if  ( structureAttributes && ! utilMemoryBufferIsEmpty( structureAttributes ) )
	{
	sioOutPrintf( ps->psSos,
		"/A << %s >> ", utilMemoryBufferGetString( structureAttributes ) );
	}

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
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
    {
    if  ( ! ps->psOmitContentMarks )
	{
	if  ( psPdfmarkOpenLeafStructItem( ps, structItem, structureAttributes ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

	if  ( psPdfmarkCloseStructItem( ps ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}
	}

    return 0;
    }

/**
 *  Populate a StructItem as per table 323 in ISO 32000-1:2008 (14.7.4)
 *  The StructItem is a group. It is intended to receive children.
 */
static int psPdfmarkDefineGroupStructItem(
				PrintingState *		ps,
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
    {
    if  ( ! ps->psOmitContentMarks )
	{
	if  ( psPdfmarkOpenGroupStructItem( ps, structItem, structureAttributes ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

	if  ( psPdfmarkCloseStructItem( ps ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}
	}

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


/**
 *  Prepare a document to conform to be a structured PDF document.
 *  See https://ghostscript.com/blog/zugferd.html Look for Ext_Metadata to see how to extend the XMP metadata
 *  See https://taggedpdf.com/xmp/pdfUA-ID.xmp For a slice to include
 */
int psPdfmarkMarkedDocumentSetup( PrintingState *		ps,
				const char *			localeTag )
    {
    if  ( ps->psDeclareUACompliant )
	{
	if  ( ps->psOmitContentMarks )
	    { LLDEB(ps->psDeclareUACompliant,ps->psOmitContentMarks);	}

	/* Insert metadata stating that this is a level 1 PDF-UA document */
	sioOutPrintf( ps->psSos, "[ /XML " );
	psPrintStringValue( ps->psSos, PS_PDF_UA_XMP, strlen( PS_PDF_UA_XMP ), 0 );
	sioOutPrintf( ps->psSos, " /Ext_Metadata pdfmark\n" );
	}

    sioOutPrintf( ps->psSos,
	    "[ {Catalog} << /MarkInfo <</Marked true>> >> /PUT pdfmark\n" );

    if  ( localeTag && psSetCatalogProperty( ps, "Lang", localeTag ) )
	{ SDEB(localeTag);	}

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

    if  ( ! ps->psOmitContentMarks )
	{
	psPdfmarkPopulateGroupStructItem( ps,
	    PS_DOC_STRUCT_ITEM, PS_DOC_STRUCT_TREE_ROOT,
	    "Document", PS_DOC_STRUCT_ITEM_KIDS );
	}

    sioOutPrintf( ps->psSos,
	    "[ {Catalog} <</ViewerPreferences <</DisplayDocTitle true>>>> /PUT pdfmark\n" );

    return 0;
    }

int psPdfmarkMarkedPageSetup(	PrintingState *		ps,
				int			page )
    {
    char	pageParentTreeName[50];
    char	pageAnnotationArrayName[50];

    sprintf( pageParentTreeName, PS_PAGE_LEAF_ARRAY_NAME, page );
    sprintf( pageAnnotationArrayName, PS_PAGE_ANNOTATION_ARRAY_NAME, page );

    sioOutPrintf( ps->psSos,
	    "[ /_objdef {%s} /type /array /OBJ pdfmark\n", pageParentTreeName );
    sioOutPrintf( ps->psSos,
	    "[ /_objdef {%s} /type /array /OBJ pdfmark\n", pageAnnotationArrayName );

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
    const int		page= ps->psSheetsPrinted;
    const char * const	itemDict= utilMemoryBufferGetString( &(structItem->siDictionaryName) );
    const char * const	parentArray= structItem->siParent?
				utilMemoryBufferGetString( &(structItem->siParent->siChildArrayName) ) :
				PS_DOC_STRUCT_ITEM_KIDS;

    sioOutPrintf( ps->psSos, "[ {%s} {%s} /APPEND pdfmark\n",
				    parentArray, itemDict );

    if  ( structItem->siIsLeaf )
	{
	char	pageParentTreeName[50];

	sprintf( pageParentTreeName, PS_PAGE_LEAF_ARRAY_NAME, page );

	sioOutPrintf( ps->psSos, "[ {%s} {%s} /APPEND pdfmark\n",
				    pageParentTreeName, itemDict );
	}

    return 0;
    }

/**
 *  Define and populate a simple StructItem object in the output.
 */
int psPdfmarkAppendMarkedLeaf(	PrintingState *		ps,
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
    {
    if  ( ps->psOmitContentMarks )
	{ return 0;	}
    else{
	if  ( ps->psInArtifact )
	    { LDEB(ps->psInArtifact); return -1;	}

	psPdfmarkDefineLeafStructItem( ps, structItem, structureAttributes );

	return psPdfmarkAppendDefinedItem( ps, structItem );
	}
    }

/**
 *  Define and populate StructItem object that potentially
 *  receives children in the output.
 */
int psPdfmarkAppendMarkedGroup(	PrintingState *		ps,
				StructItem *		structItem,
				const MemoryBuffer *	structureAttributes )
    {
    if  ( ps->psOmitContentMarks )
	{ return 0;	}
    else{
	if  ( ps->psInArtifact )
	    { LDEB(ps->psInArtifact); return -1;	}

	psPdfmarkDefineGroupStructItem( ps, structItem, structureAttributes );

	return psPdfmarkAppendDefinedItem( ps, structItem );
	}
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
    if  ( ps->psOmitContentMarks )
	{ return 0;	}
    else{
	if  ( ps->psInArtifact )
	    { LDEB(ps->psInArtifact); return -1;	}

	if  ( psPdfmarkOpenLeafStructItem( ps, structItem, (MemoryBuffer *)0 ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}

	if  ( psPdfmarkSetAltText( ps, altText ) )
	    { LDEB(1); return -1;	}

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
    }

static int addItemToDocumentNumberTree(	PrintingState *	ps,
					int		itemIndex,
					const char *	itemName )
    {
    sioOutPrintf( ps->psSos,
	"[ {%s} %d /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, itemIndex );
    sioOutPrintf( ps->psSos,
    	"[ {%s} {%s} /APPEND pdfmark\n",
	PS_DOC_NUMBER_TREE_NUMS, itemName );

    return 0;
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

    const int		page= ps->psSheetsPrinted;
    char		pageAnnotationArrayName[50];
    int 		pageAnnotationReference;

    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    pageAnnotationReference= psPdfmarkDefineAnnotationDictionary( ps,
				    &(ps->psLinkFile), &(ps->psLinkMark), &(ps->psLinkTitle),
				    annotationDictionaryName );
    if  ( pageAnnotationReference < 0 )
	{ LDEB(pageAnnotationReference); return -1;	}

    if  ( ! ps->psOmitContentMarks )
	{
	sioOutPrintf( ps->psSos,
	    "[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	    itemDict );

	sioOutPrintf( ps->psSos,
	    "[ {%s} <</Type /StructElem /S /%s /P {%s} /K [ <</Obj {%s} /Pg {ThisPage} /Type /OBJR>> %d ] /Pg {ThisPage} ",
	    itemDict,
	    structItem->siStructureType,
	    parentDict,
	    annotationDictionaryName,
	    structItem->siContentId );

	if  ( psPdfmarkCloseStructItem( ps ) )
	    { SLDEB(structItem->siStructureType,structItem->siContentId); return -1;	}
	}

    sprintf( pageAnnotationArrayName, PS_PAGE_ANNOTATION_ARRAY_NAME, page );

    sioOutPrintf( ps->psSos,
	"[ {%s} {%s} /APPEND pdfmark\n",
	pageAnnotationArrayName, annotationDictionaryName );

    /* Add the annotation to the number tree of the document */
    /* Strangely enough, it is the structItem (itemDict), rather */
    /* than the annotation(annotationDictionaryName) */
    if  ( ! ps->psOmitContentMarks				&&
	  addItemToDocumentNumberTree( ps, ps->psDocNumberTreeItemCount++, itemDict ) )
	{ LDEB(1); return -1;	}

    if  ( ps->psOmitContentMarks )
	{ return 0;						}
    else{ return psPdfmarkAppendDefinedItem( ps, structItem );	}
    }

int psPdfmarkFinishMarkedPage(	PrintingState *		ps,
				int			page )
    {
    char	pageParentTreeName[50];
    char	pageAnnotationArrayName[50];

    const int	numberTreeItemCount= ps->psDocNumberTreeItemCount++;

    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    sprintf( pageParentTreeName, PS_PAGE_LEAF_ARRAY_NAME, page );
    sprintf( pageAnnotationArrayName, PS_PAGE_ANNOTATION_ARRAY_NAME, page );

    sioOutPrintf( ps->psSos, "%% finishing page %d\n", page );

    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</StructParents %d>> /PUT pdfmark\n",
	    numberTreeItemCount );
    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</Annots {%s}>> /PUT pdfmark\n",
	    pageAnnotationArrayName );
    sioOutPrintf( ps->psSos,
	    "[ {ThisPage} <</Tabs /S>> /PUT pdfmark\n" );

    /* Add the page to the number tree of the document */
    if  ( ! ps->psOmitContentMarks				&&
	  addItemToDocumentNumberTree( ps, numberTreeItemCount, pageParentTreeName ) )
	{ LDEB(numberTreeItemCount); return -1;	}

    return 0;
    }

/**
 * Finish the document structure
 * 1) Insert the Nums array in the root of the structure number tree
 * 2) Insert the structure number tree in the StructTreeRoot of the document.
 */
int psPdfmarkMarkedDocumentTrailer( PrintingState *		ps )
    {
    if  ( ! ps->psOmitContentMarks )
	{
	/* 1 */
	sioOutPrintf( ps->psSos,
		"[ {%s} <</Nums {%s}>> /PUT pdfmark\n",
		PS_DOC_NUMBER_TREE_ROOT, PS_DOC_NUMBER_TREE_NUMS );
	/* 2 */
	sioOutPrintf( ps->psSos, "[ {Catalog} <</StructTreeRoot {%s}>> /PUT pdfmark\n",
		PS_DOC_STRUCT_TREE_ROOT );
	}

    return 0;
    }


int psPdfMarkSetActualText(	PrintingState *		ps,
				struct StructItem *	structItem,
				const struct MemoryBuffer * actualText )
    {
    if  ( ps->psInArtifact )
	{ LDEB(ps->psInArtifact); return -1;	}

    if  ( ! ps->psOmitContentMarks )
	{
	sioOutPrintf( ps->psSos,
	    "[ {%s} <<",
	    utilMemoryBufferGetString( &(structItem->siDictionaryName) ) );

	psPdfmarkSetActualText( ps, actualText );

	sioOutPrintf( ps->psSos, ">> /PUT pdfmark\n" );
	}

    return 0;
    }
