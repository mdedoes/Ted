/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>
#   include	<time.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfControlWord.h"
#   include	<utilMemoryBuffer.h>
#   include	<docDocumentProperties.h>
#   include	<docBuf.h>
#   include	<utilPropMask.h>
#   include	<docListAdmin.h>
#   include	<fontDocFontListImpl.h>
#   include	<utilPalette.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save Document Properties to RTF.					*/
/*									*/
/************************************************************************/

static int docRtfSaveInfo(	RtfWriter *		rw,
				const char *		tag,
				const MemoryBuffer *	info )
    {
    const int		addSemicolon= 0;

    docRtfWriteDocEncodedBufferDestination( rw, tag, info, addSemicolon );
    docRtfWriteNextLine( rw );

    return 0;
    }

int docRtfSaveDate(	RtfWriter *		rw,
			const char *		tag,
			const struct tm *	tm )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( tm->tm_mday != 0 )
	{
	docRtfWriteArgTag( rw, "yr",	tm->tm_year+ 1900 );
	docRtfWriteArgTag( rw, "mo",	tm->tm_mon+ 1 );
	docRtfWriteArgTag( rw, "dy",	tm->tm_mday );
	docRtfWriteArgTag( rw, "hr",	tm->tm_hour );
	docRtfWriteArgTag( rw, "min",	tm->tm_min );
	docRtfWriteArgTag( rw, "sec",	tm->tm_sec );
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

static const int DocRegularProperties[]=
{
    DGpropPAGE_WIDTH,
    DGpropPAGE_HEIGHT,
    DGpropLEFT_MARGIN,
    DGpropRIGHT_MARGIN,
    DGpropTOP_MARGIN,
    DGpropBOTTOM_MARGIN,
    DGpropGUTTER,
    DGpropMARGMIR,
    DPpropFACING_PAGES,
    DPpropGUTTER_HORIZONTAL,
    DPpropWIDOWCTRL,
    DPpropTWO_ON_ONE,
    DPpropDOCTEMP,
};

static const int DocRegularPropertyCount=
			sizeof(DocRegularProperties)/sizeof(int);

static const int DocRegularBorders[]=
{
    DPpropTOP_BORDER,
    DPpropBOTTOM_BORDER,
    DPpropLEFT_BORDER,
    DPpropRIGHT_BORDER,
    DPpropHEAD_BORDER,
    DPpropFOOT_BORDER,
};

static const int DocRegularBorderCount=
			sizeof(DocRegularBorders)/sizeof(int);

int docRtfSaveDocumentProperties( RtfWriter *			rw,
				int				fet,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet )
    {
    const int			scope= RTCscopeDOC;

    /* NO! We always write ansi 1252 documents
    int				ansicpg= dpSet->dpAnsiCodepage;

    if  ( ansicpg >= 0 )
	{
	if  ( docRtfSetAnsicpg( rw->rwRtfTextConverter, ansicpg ) )
	    { LDEB(ansicpg);	}
	}
    */

    if  ( PROPmaskISSET( dpSetMask, DPpropDEFF ) )
	{
	int	deff= docRtfWriteGetDefaultFont( rw, dpSet->dpDefaultFont );

	if  ( deff >= 0 )
	    { docRtfWriteArgTag( rw, "deff", deff );	}
	}

    if  ( PROPmaskISSET( dpSetMask, DPpropDEFLANG ) )
	{ docRtfWriteArgTag( rw, "deflang", dpSet->dpDefaultLocaleId );	}

    if  ( PROPmaskISSET( dpSetMask, DPpropRTOL ) )
	{
	docRtfWriteAltTag( rw, "rtldoc", "ltrdoc", dpSet->dpRToL );
	}

    /* Strictly spoken, this is not a document property */
    docRtfWriteArgTag( rw, "uc", rw->rwUnicodeSubstituteCount );

    if  ( PROPmaskISSET( dpSetMask, DPpropFONTTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteFontTable( rw );
	}

    if  ( PROPmaskISSET( dpSetMask, DPpropCOLORTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteColorTable( rw, dpSet );
	}

    if  ( PROPmaskISSET( dpSetMask, DPpropSTYLESHEET ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteStyleSheet( rw, &(rw->rwDocument->bdStyleSheet) );
	}

    if  ( PROPmaskISSET( dpSetMask, DPpropLISTTABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteListTable( rw, &(dpSet->dpListAdmin->laListTable) );
	}

    if  ( PROPmaskISSET( dpSetMask, DPpropLISTOVERRIDETABLE ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteListOverrideTable( rw,
				&(dpSet->dpListAdmin->laListOverrideTable) );
	}

    /* Not ready yet
    if  ( PROPmaskISSET( dpSetMask, DPpropUSERPROPS ) )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteUserProperties( rw, &(rw->rwDocument->bdUserProperties) );
	}
    */

    if  ( PROPmaskISSET( dpSetMask, DPpropGENERATOR ) )
	{ docRtfSaveInfo( rw, RTFtag__generator, &(dpSet->dpGeneratorWrite) ); }

    if  ( PROPmaskISSET( dpSetMask, DPpropTITLE )	||
	  PROPmaskISSET( dpSetMask, DPpropAUTHOR )	||
	  PROPmaskISSET( dpSetMask, DPpropCOMPANY )	||
	  PROPmaskISSET( dpSetMask, DPpropSUBJECT )	||
	  PROPmaskISSET( dpSetMask, DPpropKEYWORDS )	||
	  PROPmaskISSET( dpSetMask, DPpropDOCCOMM )	||
	  PROPmaskISSET( dpSetMask, DPpropHLINKBASE )	||
	  PROPmaskISSET( dpSetMask, DPpropCREATIM )	||
	  PROPmaskISSET( dpSetMask, DPpropREVTIM )	||
	  PROPmaskISSET( dpSetMask, DPpropPRINTIM )	)
	{
	docRtfWriteNextLine( rw );
	docRtfWriteDestinationBegin( rw, "info" );
	docRtfWriteNextLine( rw );

	if  ( PROPmaskISSET( dpSetMask, DPpropTITLE ) )
	    { docRtfSaveInfo( rw, "title", &(dpSet->dpTitle) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropAUTHOR ) )
	    { docRtfSaveInfo( rw, "author", &(dpSet->dpAuthor) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropCOMPANY ) )
	    { docRtfSaveInfo( rw, "company", &(dpSet->dpCompany) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropSUBJECT ) )
	    { docRtfSaveInfo( rw, "subject", &(dpSet->dpSubject) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropKEYWORDS ) )
	    { docRtfSaveInfo( rw, "keywords", &(dpSet->dpKeywords) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropDOCCOMM ) )
	    { docRtfSaveInfo( rw, "doccomm", &(dpSet->dpDoccomm) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropHLINKBASE ) )
	    { docRtfSaveInfo( rw, RTFtag_hlinkbase, &(dpSet->dpHlinkbase) );	}

	if  ( PROPmaskISSET( dpSetMask, DPpropCREATIM ) )
	    { docRtfSaveDate( rw, "creatim", &(dpSet->dpCreatim) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropREVTIM ) )
	    { docRtfSaveDate( rw, RTFtag_revtim, &(dpSet->dpRevtim) );	}
	if  ( PROPmaskISSET( dpSetMask, DPpropPRINTIM ) )
	    { docRtfSaveDate( rw, "printim", &(dpSet->dpPrintim) );	}

	docRtfWriteDestinationEnd( rw );
	}

    docRtfWriteNextLine( rw );

    if  ( docRtfWriteItemProperties( rw, scope, dpSet,
			(RtfGetIntProperty)docGetDocumentProperty, dpSetMask,
			DocRegularProperties, DocRegularPropertyCount ) )
	{ LLDEB(scope,DocRegularPropertyCount); return -1;	}

    if  ( dpSet->dpTabIntervalTwips > 0 && dpSet->dpTabIntervalTwips != 720 )
	{ docRtfWriteArgTag( rw, "deftab", dpSet->dpTabIntervalTwips ); }

    docRtfWriteNextLine( rw );

    if  ( fet >= 0 )
	{ docRtfWriteArgTag( rw, RTFtag_fet, fet );	}

    docRtfSaveNotesProperties( rw, dpSetMask,
			    &(dpSet->dpNotesProps.fepFootnotesProps),
			    DOCdocFOOTNOTE_PROP_MAP, "ftnstart",
			    DOCrtf_DocFootNotesJustificationTags,
			    DOCrtf_DocFootNotesJustificationTagCount,
			    DOCrtf_DocFootNotesPlacementTags,
			    DOCrtf_DocFootNotesPlacementTagCount,
			    DOCrtf_DocFootNotesRestartTags,
			    DOCrtf_DocFootNotesRestartTagCount,
			    DOCrtf_DocFootNotesNumberStyleTags,
			    DOCrtf_DocFootNotesNumberStyleTagCount );

    docRtfSaveNotesProperties( rw, dpSetMask,
			    &(dpSet->dpNotesProps.fepEndnotesProps),
			    DOCdocENDNOTE_PROP_MAP, "aftnstart",
			    DOCrtf_DocEndNotesJustificationTags,
			    DOCrtf_DocEndNotesJustificationTagCount,
			    DOCrtf_DocEndNotesPlacementTags,
			    DOCrtf_DocEndNotesPlacementTagCount,
			    DOCrtf_DocEndNotesRestartTags,
			    DOCrtf_DocEndNotesRestartTagCount,
			    DOCrtf_DocEndNotesNumberStyleTags,
			    DOCrtf_DocEndNotesNumberStyleTagCount );

    if  ( docRtfWriteItemProperties( rw, scope, dpSet,
			(RtfGetIntProperty)docGetDocumentProperty, dpSetMask,
			DocRegularBorders, DocRegularBorderCount ) )
	{ LLDEB(scope,DocRegularBorderCount); return -1;	}

    docRtfWriteNextLine( rw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the various separators relating to notes.			*/
/*									*/
/************************************************************************/

int docRtfSaveDocNotesSeparators(	RtfWriter *		rw,
					struct BufferDocument *	bd )
    {
    const int			evenIfAbsent= 0;
    const int			forcePar= 0;

    if  ( docRtfSaveDocumentTree( rw, "ftnsep", &(bd->bdFtnsep),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "ftnsepc", &(bd->bdFtnsepc),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "ftncn", &(bd->bdFtncn),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    /******/

    if  ( docRtfSaveDocumentTree( rw, "aftnsep", &(bd->bdAftnsep),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "aftnsepc", &(bd->bdAftnsepc),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveDocumentTree( rw, "aftncn", &(bd->bdAftncn),
						    evenIfAbsent, forcePar ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the mask of scalar properties that differ from the	*/
/*  defaults.								*/
/*									*/
/************************************************************************/

int docRtfDocPropMask(	PropertyMask *			dpSaveMask,
			const DocumentProperties *	dpDoc )
    {
    int				rval= 0;

    PropertyMask		dpSetMask;
    DocumentProperties		dpDef;

    const struct DocumentAttributeMap * const dam0=
				(const struct DocumentAttributeMap *)0;

    docInitDocumentProperties( &dpDef );

    utilPropMaskClear( &dpSetMask );
    utilPropMaskFill( &dpSetMask, DPprop_COUNT );
    utilPropMaskClear( dpSaveMask );

    if  ( docUpdDocumentProperties( dpSaveMask, &dpDef,
						    &dpSetMask, dpDoc, dam0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dpDoc->dpFontList->dflFontCount > 0 )
	{ PROPmaskADD( dpSaveMask, DPpropFONTTABLE );	}
    if  ( dpDoc->dpColorPalette->cpColorCount > 0 )
	{ PROPmaskADD( dpSaveMask, DPpropCOLORTABLE );	}
    if  ( dpDoc->dpListAdmin->laListTable.dltListCount > 0 )
	{ PROPmaskADD( dpSaveMask, DPpropLISTTABLE );	}
    if  ( dpDoc->dpListAdmin->laListOverrideTable.lotOverrideCount > 0 )
	{ PROPmaskADD( dpSaveMask, DPpropLISTOVERRIDETABLE );	}

    if  ( ! utilMemoryBufferIsEmpty( &(dpDoc->dpGeneratorWrite) ) )
	{ PROPmaskADD( dpSaveMask, DPpropGENERATOR );	}

  ready:

    docCleanDocumentProperties( &dpDef );

    return rval;
    }
