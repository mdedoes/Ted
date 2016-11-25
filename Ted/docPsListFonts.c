/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*  Manage resources for layout.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appImage.h>
#   include	"docLayout.h"
#   include	"docFind.h"
#   include	<sioHex.h>
#   include	<sioMemory.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Translate text attributes to AfmFontInfo.				*/
/*									*/
/************************************************************************/

AfmFontInfo * docPsPrintGetAfi( int *				pEncoding,
				const AppPhysicalFontList *	apfl,
				int				physf )
    {
    AppFontFamily *			aff;
    int					affCount;
    AppFontTypeface *			aft;

    AppPhysicalFont *			apf;

    if  ( psFontCatalog( apfl->apflAfmDirectory, &aff, &affCount ) )
	{ SDEB(apfl->apflAfmDirectory); return (AfmFontInfo *)0;	}

    if  ( physf < 0 || physf >= apfl->apflCount )
	{ LLDEB(physf,apfl->apflCount); return (AfmFontInfo *)0;	}

    apf= apfl->apflFonts+ physf;

    if  ( apf->apfPsFamilyNumber < 0		||
	  apf->apfPsFamilyNumber >= affCount	)
	{ LLDEB(apf->apfPsFamilyNumber,affCount); return (AfmFontInfo *)0; }

    aff += apf->apfPsFamilyNumber;
    aft= aff->affFaces;

    if  ( apf->apfPsFaceNumber < 0			||
	  apf->apfPsFaceNumber >=  aff->affFaceCount	)
	{
	LLDEB(apf->apfPsFaceNumber,aff->affFaceCount);
	return (AfmFontInfo *)0;
	}

    aft += apf->apfPsFaceNumber;

    if  ( ! aft->aftPrintingData )
	{ XDEB(aft->aftPrintingData);	}

    *pEncoding= apf->apfFontEncoding;
    return (AfmFontInfo *)aft->aftPrintingData;
    }

int docPsClaimParticuleData(	const BufferItem *	bi,
				ParticuleData **	pParticuleData )
    {
    static ParticuleData *	PSPrintGeometry;
    static int			PSPrintGeometryCount;

    if  ( bi->biParaParticuleCount > PSPrintGeometryCount )
	{
	ParticuleData *	fresh;
	unsigned int	size;

	size= ( bi->biParaParticuleCount+ 1 )* sizeof( ParticuleData );

	fresh= (ParticuleData *)realloc( PSPrintGeometry,
			bi->biParaParticuleCount* sizeof( ParticuleData ) );
	if  ( ! fresh )
	    { LXDEB(bi->biParaParticuleCount,fresh); return -1;	}

	PSPrintGeometry= fresh;
	}

    *pParticuleData= PSPrintGeometry; return 0;
    }

/************************************************************************/
/*									*/
/*  Find the fonts of an image.						*/
/*									*/
/************************************************************************/

typedef int (*LIST_FONTS_PS)(	PostScriptFaceList *	psfl,
				SimpleInputStream *	sis,
				const char *		afmDirectory,
				const char *		prefix,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			twipsWide,
				int			twipsHigh );

int docPsListObjectFonts(	PostScriptFaceList *	psfl,
				const InsertedObject *	io,
				const char *		afmDirectory,
				const char *		prefix )
    {
    LIST_FONTS_PS		listFontsPs;

    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisMeta;

    const MemoryBuffer *	mb;
    int				mapMode= 0;

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    mb= &(io->ioObjectData);
	    mapMode= io->ioMapMode;
	    listFontsPs= appMetaListFontsPs;
	    break;

	case DOCokMACPICT:
	    mb= &(io->ioObjectData);
	    listFontsPs= appMacPictListFontsPs;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		mb= &(io->ioResultData);
		mapMode= io->ioResultMapMode;
		listFontsPs= appMetaListFontsPs;
		break;
		}
	    else{ LDEB(io->ioResultKind); return 0;	}

	case DOCokINCLUDEPICTURE:
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); return -1;	}

    if  ( (*listFontsPs)( psfl, sisMeta, afmDirectory, prefix, mapMode,
					io->io_xWinExt, io->io_yWinExt,
					io->ioTwipsWide, io->ioTwipsHigh ) )
	{ SDEB(afmDirectory); return -1;	}

    sioInClose( sisMeta );
    sioInClose( sisMem );

    return 0;
    }


/************************************************************************/
/*									*/
/*  Recursively retrieve the list of PostScriptFonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetDocumentFonts
    {
    const AppPhysicalFontList *		gdfPhysicalFontList;
    PostScriptFaceList *		gdfPostScriptFaceList;
    } GetDocumentFonts;

/*  a  */
static int docPsPrintGetParaFonts(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    GetDocumentFonts *		gdf= (GetDocumentFonts *)through;
    const AppPhysicalFontList *	apfl= gdf->gdfPhysicalFontList;
    PostScriptFaceList *	psfl= gdf->gdfPostScriptFaceList;

    int				privateFont;

    AfmFontInfo *		afi;
    int				encoding;

    InsertedObject *		io;

    const TextParticule *	tp;
    int				part;

    tp= bi->biParaParticules; part= 0;

    while( part < bi->biParaParticuleCount )
	{
	const int	appearsInText= 1;

	switch( tp->tpKind )
	    {
	    case DOCkindTEXT:
	    case DOCkindTAB:
		privateFont= tp->tpPhysicalFont;
		if  ( tp->tpPhysicalFont >= 0 )
		    {
		    TextAttribute	ta;

		    afi= docPsPrintGetAfi( &encoding, apfl,
							tp->tpPhysicalFont );
		    if  ( ! afi )
			{ XDEB(afi); return -1;	}

		    utilGetTextAttributeByNumber( &ta,
					    &(bd->bdTextAttributeList),
					    tp->tpTextAttributeNumber );

		    if  ( utilRememberPostsciptFace( psfl,
				    encoding, afi, ta, "f", appearsInText ) )
			{ LDEB(1); return -1;	}
		    }

		while( part+ 1 < bi->biParaParticuleCount )
		    {
		    if  ( tp[1].tpKind != DOCkindTEXT	&&
			  tp[1].tpKind != DOCkindTAB	)
			{ break;	}
		    if  ( tp[1].tpPhysicalFont != privateFont	)
			{ break;	}

		    part++; tp++;
		    }
		part++; tp++; break;

	    case DOCkindOBJECT:
		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  ( docPsListObjectFonts( psfl, io,
					apfl->apflAfmDirectory, "f" ) )
		    { LDEB(tp->tpKind); return -1;	}

		part++; tp++; break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCHFTNSEP:
		part++; tp++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }
	}

    return 1;
    }


int docPsPrintGetDocumentFonts(	BufferDocument *		bd,
				PostScriptFaceList *		psfl,
				const AppPhysicalFontList *	apfl )
    {
    GetDocumentFonts		gdf;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    gdf.gdfPhysicalFontList= apfl;
    gdf.gdfPostScriptFaceList= psfl;

    /*  a  */
    if  ( docFirstPosition( &dpBeginFrom, &(bd->bdItem) ) )
	{ LDEB(1); return -1;;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bd,
				    docPsPrintGetParaFonts, (void *)&gdf );
    if  ( res != 1 )
	{ LDEB(res); return -1;	}

    return 0;
    }

