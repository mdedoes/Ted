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

#   include	<appDebugon.h>

/************************************************************************/

static struct BufferDocument * docIncludeTextOpenDocument(
				const IncludeTextField *	itf,
				const MemoryBuffer *		refFileName )
    {
    struct BufferDocument *		bdFrom= (struct BufferDocument *)0;
    struct SimpleInputStream *		sisFile= (struct SimpleInputStream *)0;
    int				format= -1;

    const int			rtfFlags= RTFflagLENIENT;
    const int			relativeIsFile= 1;

    MemoryBuffer		absolute;

    utilInitMemoryBuffer( &absolute );

    if  ( fileAbsoluteName( &absolute,
		&(itf->itfFilename), relativeIsFile, refFileName ) < 0 )
	{
	SDEB(utilMemoryBufferGetString(refFileName));
	SDEB(utilMemoryBufferGetString(&(itf->itfFilename)));
	goto ready;
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
	    bdFrom= docRtfReadFile( sisFile, rtfFlags );
	    if  ( ! bdFrom )
		{
		SXDEB(utilMemoryBufferGetString(&absolute),bdFrom);
		goto ready;
		}
	    break;
	case TEDdockindTEXT_OPEN:
	    bdFrom= docPlainReadFile( sisFile, (int *)0,
					(const struct DocumentGeometry *)0 );
	    if  ( ! bdFrom )
		{
		SXDEB(utilMemoryBufferGetString(&absolute),bdFrom);
		goto ready;
		}
	    break;
	default:
	    LDEB(format); goto ready;
	}

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    utilCleanMemoryBuffer( &absolute );

    return bdFrom;
    }

/************************************************************************/

static int docRecalculateIncludeTextFieldIncludeSource(
					struct BufferDocument *	bdTo,
					DocumentField *		dfInclude,
					const MemoryBuffer *	refFileName,
					struct BufferDocument *	bdFrom )
    {
    int				rval= 0;

    DocumentSelection		dsInsideItf;
    DocumentSelection		dsAroundItf;
    int				part0;
    int				part1;

    EditOperation		eo;
    DocumentCopyJob		dcj;

    const int			forceAttributeTo= -1;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    if  ( docDelimitFieldInDoc( &dsInsideItf, &dsAroundItf,
					    &part0, &part1, bdTo, dfInclude ) )
	{ LDEB(1); rval= -1; goto ready;	}

    eo.eoBodySectNode= docGetBodySectNodeOfScope(
					&(dfInclude->dfSelectionScope), bdTo );

    if  ( docStartEditOperation( &eo, &dsInsideItf, bdTo, dfInclude ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSet2DocumentCopyJob( &dcj, &eo, bdFrom, &(bdFrom->bdBody),
					    refFileName, forceAttributeTo) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* ? */
    dcj.dcjCopyHeadParaProperties= 1;
    dcj.dcjCopyTailParaProperties= 1;

    if  ( docIncludeDocument( &dcj ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

/************************************************************************/

typedef struct RecalculateIncludeTextFields
    {
    RecalculateFields *		ritfRecalculateFields;
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

	return SCANadviceSKIP;
	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

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
    struct BufferDocument *		bdTo= rf->rfDocument;
    int				rval= 0;
    IncludeTextField		itf;
    struct BufferDocument *		bdFrom= (struct BufferDocument *)0;

    docInitIncludeTextField( &itf );

    if  ( docGetIncludeTextField( &itf, dfInclude ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bdFrom= docIncludeTextOpenDocument( &itf, refFileName );
    if  ( ! bdFrom )
	{
	SXDEB(utilMemoryBufferGetString(&(itf.itfFilename)),bdFrom);
	rval= 1; goto ready;
	}

    /*  ?  */
    bdFrom->bdProperties->dpHasOpenEnd= 1;

    if  ( docRecalculateIncludeTextFieldIncludeSource( bdTo,
					    dfInclude, refFileName, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! itf.itfConstantFields )
	{
	RecalculateIncludeTextFields	ritf;

	ritf.ritfRecalculateFields= rf;
	ritf.ritfRefFileName= &(bdFrom->bdProperties->dpFilename);

	if  ( docScanFieldsInRange( &(dfInclude->dfResultFields),
				(EditRange *)0,
				docRecalculateIncludeTextFieldVisitField,
				(TreeFieldVisitor)0, &ritf ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    if  ( bdFrom )
	{ docFreeDocument( bdFrom );	}

    docCleanIncludeTextField( &itf );

    return rval;
    }

int docRecalculateIncludeTextFields( RecalculateFields *		rf )
    {
    struct BufferDocument *	bdDoc= rf->rfDocument;

    RecalculateIncludeTextFields	ritf;

    ritf.ritfRecalculateFields= rf;
    ritf.ritfRefFileName= &(bdDoc->bdProperties->dpFilename);

    /*  3  */
    if  ( docScanFieldsInRange( &(bdDoc->bdBody.dtRootFields),
				(EditRange *)0,
				docRecalculateIncludeTextFieldVisitField,
				(TreeFieldVisitor)0, &ritf ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

