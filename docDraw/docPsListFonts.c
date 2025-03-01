/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*  Manage resources for layout.					*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<drawMetafilePsList.h>
#   include	"docPsPrintImpl.h"
#   include	<docObjectProperties.h>
#   include	<sioGeneral.h>
#   include	<sioHexedMemory.h>
#   include	<docObject.h>
#   include	<docMetafileObject.h>
#   include	<psFace.h>
#   include	<layoutContext.h>
#   include	<docLayout.h>
#   include	<docTextRun.h>
#   include	<docTreeScanner.h>
#   include	<docParaScanner.h>
#   include	<docScanner.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of PostScriptFonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Find the fonts of an image.						*/
/*									*/
/************************************************************************/

int docPsListImageFonts(	struct PostScriptTypeList *	pstl,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const LayoutContext *		lc,
				const char *			prefix )
    {
    int				rval= 0;
    MetafileWriteListPs		listFontsPs;

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    MetafilePlayer		mp;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	    listFontsPs= appWmfListFontsPs;
	    break;

	case DOCokMACPICT:
	    listFontsPs= appMacPictListFontsPs;
	    break;

	case DOCokPICTEMFBLIP:
	    listFontsPs= appEmfListFontsPs;
	    break;

	default:
	    LDEB(pip->pipType); goto ready;
	}

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisData, lc, pip, 0, 0 );

    if  ( (*listFontsPs)( pstl, &mp, prefix ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

/************************************************************************/

typedef struct GetDocumentFonts
    {
    const LayoutContext *		gdfLayoutContext;
    struct PostScriptTypeList *		gdfPostScriptTypeList;
    } GetDocumentFonts;

/************************************************************************/

static int docPsListObjectFonts( const InsertedObject *		io,
				const char *			prefix,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const LayoutContext *	lc= gdf->gdfLayoutContext;
    struct PostScriptTypeList *	pstl= gdf->gdfPostScriptTypeList;

    switch( io->ioKind )
	{
	case DOCokMACPICT:
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	    return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioObjectData), lc, prefix );
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		||
		  io->ioResultKind == DOCokMACPICT		)
		{
		return docPsListImageFonts( pstl,
				&(io->ioPictureProperties),
				&(io->ioResultData), lc, prefix );
		}
	    else{ /*LDEB(io->ioResultKind);*/ return 0;	}

	case DOCokEPS_FILE:
	    LDEB(io->ioKind);
	    return 0;

	case DOCokDRAWING_SHAPE:
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docPsPrintGotSpan(	const TextRun *			tr,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const LayoutContext *	lc= gdf->gdfLayoutContext;
    struct PostScriptTypeList *	pstl= gdf->gdfPostScriptTypeList;

    const struct AfmFontInfo *	afi;

    const int			appearsInText= 1;

    afi= docDocLayoutGetFontInfo( lc, tr->trTextAttribute );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    if  ( psRememberPostsciptFace( pstl, afi, tr->trTextAttribute,
						    "f", appearsInText ) )
	{ LDEB(1); return -1;	}

    return SCANadviceOK;
    }

static int docPsPrintGotObject(
			const struct VisitParticule *		vp,
			struct InsertedObject *			io,
			void *					through )
    {
    if  ( docPsListObjectFonts( io, "f", through ) )
	{ LDEB(1); return -1;	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docPsPrintGetDocumentFonts(	struct PostScriptTypeList *	pstl,
				const LayoutContext *		lc )
    {
    int				ret;
    GetDocumentFonts		gdf;

    const int			treeFlags=
					FLAGtsSCAN_SECTION_HEADERS_FOOTERS|
					FLAGtsSCAN_BODY_SEPARATORS|
					FLAGtsSCAN_FOOT_END_NOTES;

    const int			paraFlags= FLAGpsSCAN_COMBINE_LINES|
					FLAGpsSCAN_EMPTY_SPANS|
					FLAGpsSCAN_SHAPE_TEXTS;

    gdf.gdfPostScriptTypeList= pstl;
    gdf.gdfLayoutContext= lc;

    /*  a  */
    ret= docScanParagraphsLogicalOrder( lc->lcDocument,
			    (const struct DocumentSelection *)0,
			    treeFlags, paraFlags,
			    (ParticuleVisitor)0, (ParaFieldVisitor)0,
			    docPsPrintGotSpan, docPsPrintGotObject,
			    (TabVisitor)0, (void *)&gdf );
    if  ( ret != SCANadviceOK )
	{ LLDEB(ret,SCANadviceOK); return -1;	}

    return 0;
    }

