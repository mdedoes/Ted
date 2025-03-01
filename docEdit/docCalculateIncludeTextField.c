/************************************************************************/
/*									*/
/*  Evaluate INCLUDETEXT fields.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<docIncludeTextField.h>

#   include	<sioFileio.h>

#   include	<docBuf.h>
#   include	<docTreeType.h>
#   include	<docRtfFlags.h>
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	"docEdit.h"
#   include	"docRecalculateIncludeTextField.h"

#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFileExtensions.h>
#   include	<utilFileExtension.h>
#   include	<docFieldScanner.h>
#   include	<appSystem.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"
#   include	<docDocumentProperties.h>
#   include	<sioGeneral.h>
#   include	<docNodeTree.h>
#   include	<docFields.h>
#   include	<docScanner.h>
#   include	<utilTree.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/

static struct BufferDocument * docIncludeTextOpenDocument(
				RecalculateFields *		rf,
				const IncludeTextField *	itf,
				const MemoryBuffer *		refFileName )
    {
    struct BufferDocument *	bdSource= (struct BufferDocument *)0;
    struct SimpleInputStream *	sisFile= (struct SimpleInputStream *)0;
    int				format= -1;

    const int			rtfFlags= RTFflagLENIENT;
    const int			relativeIsFile= 1;

    MemoryBuffer		absolute;
    const char *		absoluteString;

    utilInitMemoryBuffer( &absolute );


    if  ( fileAbsoluteName( &absolute,
		&(itf->itfFilename), relativeIsFile, refFileName ) < 0 )
	{
	SDEB(utilMemoryBufferGetString(refFileName));
	SDEB(utilMemoryBufferGetString(&(itf->itfFilename)));
	goto ready;
	}

    absoluteString= utilMemoryBufferGetString( &absolute );

    if  ( ! rf->rfIncludeDocumentCache )
	{
	rf->rfIncludeDocumentCache= utilTreeMakeTree( 0 );

	if  ( ! rf->rfIncludeDocumentCache )
	    { XDEB(rf->rfIncludeDocumentCache); goto ready;	}
	}
    else{
	bdSource= (struct BufferDocument *)utilTreeGetEQ(
					    rf->rfIncludeDocumentCache,
					    (const char **)0,
					    absoluteString );
	if  ( bdSource )
	    { goto ready;	}
	}

    format= utilDocumentGetOpenFormat( (int *)0,
			DOC_FileExtensions, DOC_FileExtensionCount,
			&absolute, format );

    switch( format )
	{
	case TEDdockindRTF:
	case TEDdockindTEXT_OPEN:
	    sisFile= sioInFileioOpen( &absolute );
	    if  ( ! sisFile )
		{
		SXDEB(utilMemoryBufferGetString(&absolute),sisFile);
		goto ready;
		}
	    break;

	default:
	    SLDEB(utilMemoryBufferGetString(&absolute),format);
	    goto ready;
	}

    switch( format )
	{
	case TEDdockindRTF:
	    bdSource= docRtfReadFile( sisFile, rtfFlags );
	    if  ( ! bdSource )
		{
		SXDEB(utilMemoryBufferGetString(&absolute),bdSource);
		goto ready;
		}
	    break;
	case TEDdockindTEXT_OPEN:
	    bdSource= docPlainReadFile( sisFile, (int *)0,
					(const struct DocumentGeometry *)0 );
	    if  ( ! bdSource )
		{
		SXDEB(utilMemoryBufferGetString(&absolute),bdSource);
		goto ready;
		}
	    break;
	default:
	    LDEB(format); goto ready;
	}

    utilTreeStoreValue( rf->rfIncludeDocumentCache,
					(void **)0, (const char **)0,
					absoluteString, bdSource );

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    utilCleanMemoryBuffer( &absolute );

    return bdSource;
    }

/************************************************************************/

static int docRecalculateIncludeTextFieldIncludeSource(
				EditRange *			erInserted,
				struct BufferDocument *		bdTarget,
				DocumentField *			dfInclude,
				const DocumentSelection *	dsTarget,
				const MemoryBuffer *		refFileName,
				struct BufferDocument *		bdSource,
				const DocumentSelection *	dsSource )
    {
    int				rval= 0;

    EditOperation		eo;
    DocumentCopyJob		dcj;

    const int			forceAttributeTo= -1;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    eo.eoBodySectNode= docGetBodySectNodeOfScope(
				&(dfInclude->dfSelectionScope), bdTarget );

    if  ( docStartEditOperation( &eo, dsTarget, bdTarget, dfInclude ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSet2DocumentCopyJob( &dcj, &eo, bdSource, &(bdSource->bdBody),
					    refFileName, forceAttributeTo) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* ? */
    dcj.dcjCopyHeadParaProperties= 1;
    dcj.dcjCopyTailParaProperties= 1;

    if  ( docSelectionIsSet( dsSource ) )
	{
	if  ( docIncludeSelection( &dcj, dsSource ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( docIncludeDocument( &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    *erInserted= eo.eoSelectedRange;

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

/************************************************************************/

typedef struct RecalculateIncludeTextFields
    {
    RecalculateFields *		ritfRecalculateFields;

				    /**
				     *  The file name of the target document.
				     *  Used to open included files with a 
				     *  relative name.
				     */
    const MemoryBuffer *	ritfRefFileName;

    } RecalculateIncludeTextFields;

static int docRecalculateIncludeTextField(
					DocumentField *		dfInclude,
					RecalculateFields *	rf,
					const MemoryBuffer *	refFileName );

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRecalculateIncludeTextFieldVisitField(
					DocumentField *		df,
					const EditRange *	er,
					void *			vritf )
    {
    RecalculateIncludeTextFields *	ritf;

    ritf= (RecalculateIncludeTextFields *)vritf;

    if  ( df->dfKind == DOCfkINCLUDETEXT		&&
	  df->dfSelectionScope.ssTreeType == DOCinBODY	)
	{
	if  ( docRecalculateIncludeTextField( df,
				    ritf->ritfRecalculateFields, 
				    ritf->ritfRefFileName ) < 0 )
	    { LDEB(1); return -1;	}

	ritf->ritfRecalculateFields->rfFieldsUpdated++;
	return SCANadviceSKIP;
	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/

static int docInsertIncludeSourceInstance(
				DocumentField *			dfInclude,
				RecalculateFields *		rf,
				const MemoryBuffer *		refFileName,
				DocumentSelection *		dsTarget,
				const DocumentSelection *	dsSource,
				BufferDocument *		bdSource,
				int				constantFields )
    {
    struct BufferDocument *	bdTarget= rf->rfDocument;

    EditRange			erInserted;

    docInitEditRange( &erInserted );

    if  ( docRecalculateIncludeTextFieldIncludeSource( &erInserted, bdTarget,
					dfInclude, dsTarget,
					refFileName, bdSource, dsSource ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInEditRange( rf, &erInserted ) )
	{ LDEB(1); return -1;	}

    if  ( ! constantFields )
	{
	RecalculateIncludeTextFields	ritf;

	ritf.ritfRecalculateFields= rf;
	ritf.ritfRefFileName= &(bdSource->bdProperties->dpFilename);

	if  ( docScanFieldsInRange( &(dfInclude->dfResultFields),
				(EditRange *)0,
				docRecalculateIncludeTextFieldVisitField,
				(TreeFieldVisitor)0, &ritf ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docInsertIncludeInstances(
				const IncludeTextField *	itf,
				DocumentField *			dfInclude,
				RecalculateFields *		rf,
				const MemoryBuffer *		refFileName,
				const DocumentSelection *	dsSource,
				BufferDocument *		bdSource )
    {
    int			rval= 0;

    BufferDocument *	bdTarget= rf->rfDocument;

    const char *	xfor= utilMemoryBufferGetString( &(itf->itfFor) );
    const char *	xin= utilMemoryBufferGetString( &(itf->itfIn) );

    InstanceStream *	is= (InstanceStream *)0;

    int			round= 0;

    if  ( ! rf->rfFieldDataProvider )
	{ /*XDEB(rf->rfFieldDataProvider);*/ rval= 1; goto ready;	}
    if  ( utilMemoryBufferIsEmpty( &(itf->itfIn) ) )
	{ SSDEB(xfor,xin); rval= -1; goto ready;		}

    is= docRecalculateFieldsOpenInstanceStream( rf, xfor, xin );
    if  ( ! is )
	{ SSXDEB(xfor,xin,is); rval= -1; goto ready;	}

    for (;;)
	{
	DocumentSelection		dsInsideItf;
	DocumentSelection		dsAroundItf;

	int				res= (*is->isToNext)( is );

	if  ( res < 0 )
	    { SSLDEB(xfor,xin,res); rval= -1; goto ready;	}
	if  ( res > 0 )
	    { break;						}

	if  ( docDelimitFieldInDoc( &dsInsideItf, &dsAroundItf,
				(int *)0, (int *)0, bdTarget, dfInclude ) )
	    {
	    LDEB(1); rval= -1;
	    docListFieldTree(&(bdTarget->bdBody),bdTarget);
	    goto ready;
	    }

	if  ( round > 0 )
	    {
	    DocumentPosition dpTail= dsInsideItf.dsTail;
LDEB(docNumberOfParagraph(dpTail.dpNode));
LDEB(dpTail.dpStroff);
	    docSetIBarSelection( &dsInsideItf, &dpTail );
	    }

	if  ( docInsertIncludeSourceInstance( dfInclude, rf, refFileName,
				    &dsInsideItf, dsSource, bdSource,
				    itf->itfConstantFields ) )
	    { LDEB(1); rval= -1; goto ready;	}

	round++;
	}

# if 0
    /* HACK */
    if  ( round > 0 )
	{
	DocumentSelection		dsInsideItf;
	DocumentSelection		dsAroundItf;

	if  ( docDelimitFieldInDoc( &dsInsideItf, &dsAroundItf,
				(int *)0, (int *)0, bdTarget, dfInclude ) )
	    {
	    LDEB(1); rval= -1;
	    docListFieldTree(&(bdTarget->bdBody),bdTarget);
	    goto ready;
	    }

	if  ( dsAroundItf.dsHead.dpNode->biParaParticuleCount == 1	&&
	      dsAroundItf.dsTail.dpNode->biParaParticuleCount == 1	)
	    {
	    EditOperation	eo;

	    docInitEditOperation( &eo );

	    if  ( docStartEditOperation( &eo, &dsAroundItf, bdTarget,
							(DocumentField *)0 ) )

		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( docDeleteField( (DocumentSelection *)0, &eo, dfInclude ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    docDeleteNode( bdTarget, eo.eoTree, dsAroundItf.dsTail.dpNode );
	    docDeleteNode( bdTarget, eo.eoTree, dsAroundItf.dsHead.dpNode );

	    docCleanEditOperation( &eo );
	    }
	}
# endif

  ready:

    if  ( is )
	{ docRecalculateFieldsCloseInstanceStream( is );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Recalculate one INCLUDETEXT field in a document.			*/
/*									*/
/************************************************************************/

static int docRecalculateIncludeTextField(
					DocumentField *		dfInclude,
					RecalculateFields *	rf,
					const MemoryBuffer *	refFileName )
    {
    int				rval= 0;

    struct BufferDocument *	bdTarget= rf->rfDocument;
    IncludeTextField		itf;
    struct BufferDocument *	bdSource= (struct BufferDocument *)0;

    DocumentSelection		dsInsideSource;

    docInitIncludeTextField( &itf );

    docInitDocumentSelection( &dsInsideSource );

    if  ( docGetIncludeTextField( &itf, dfInclude ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bdSource= docIncludeTextOpenDocument( rf, &itf, refFileName );
    if  ( ! bdSource )
	{
	SXDEB(utilMemoryBufferGetString(&(itf.itfFilename)),bdSource);
	rval= 1; goto ready;
	}

    /*  ?  */
    bdSource->bdProperties->dpHasOpenEnd= 1;

    if  ( ! utilMemoryBufferIsEmpty( &(itf.itfBookmark) ) )
	{
	if  ( docFindBookmarkInDocument( &dsInsideSource,
					bdSource, &(itf.itfBookmark) ) )
	    {
	    SDEB(utilMemoryBufferGetString(&(itf.itfBookmark)));
	    rval= -1; goto ready;
	    }
	}

    if  ( ! utilMemoryBufferIsEmpty( &(itf.itfFor) ) )
	{
	if  ( docInsertIncludeInstances( &itf, dfInclude, rf, refFileName,
					&dsInsideSource, bdSource ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	DocumentSelection		dsInsideItf;
	DocumentSelection		dsAroundItf;

	if  ( docDelimitFieldInDoc( &dsInsideItf, &dsAroundItf,
				(int *)0, (int *)0, bdTarget, dfInclude ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docInsertIncludeSourceInstance( dfInclude, rf, refFileName,
				    &dsInsideItf, &dsInsideSource, bdSource,
				    itf.itfConstantFields ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    /* NO: If is cached on the RecalculateFields
    if  ( bdSource )
	{ docFreeDocument( bdSource );	}
    */

    docCleanIncludeTextField( &itf );

    return rval;
    }

int docRecalculateIncludeTextFields( RecalculateFields *		rf )
    {
    struct BufferDocument *		bdTarget= rf->rfDocument;

    RecalculateIncludeTextFields	ritf;

    ritf.ritfRecalculateFields= rf;
    ritf.ritfRefFileName= &(bdTarget->bdProperties->dpFilename);

    /*  3  */
    if  ( docScanFieldsInRange( &(bdTarget->bdBody.dtRootFields),
				(EditRange *)0,
				docRecalculateIncludeTextFieldVisitField,
				(TreeFieldVisitor)0, &ritf ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

