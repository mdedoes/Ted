/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReadWrite.h"
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"

int docRtfSaveDocument(		SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				flags )
    {
    int				rval= 0;
    RtfWriter			rwc;

    PropertyMask		dpSaveMask;

    docRtfInitWritingContext( &rwc );

    if  ( docRtfDocPropMask( &dpSaveMask, &(bd->bdProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bd->bdProperties.dpFontList.dflFontCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropFONTTABLE );	}
    if  ( bd->bdProperties.dpColorPalette.cpColorCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropCOLORTABLE );	}
    if  ( bd->bdStyleSheet.dssStyleCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropSTYLESHEET );	}
    if  ( bd->bdProperties.dpListAdmin.laListTable.dltListCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropLISTTABLE );	}
    if  ( bd->bdProperties.dpListAdmin.laListOverrideTable.lotOverrideCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropLISTOVERRIDETABLE );	}

    if  ( ! utilMemoryBufferIsEmpty( &(bd->bdProperties.dpGeneratorWrite) ) )
	{ PROPmaskADD( &dpSaveMask, DPpropGENERATOR );	}

    rwc.rwDocument= bd;
    rwc.rwSosOut= sos;
    rwc.rwSaveFlags= flags;

    if  ( docRtfWriteBuildFontAdmin( &rwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfWriteDestinationBegin( &rwc, "rtf1\\ansi" );
    docRtfWriteNextLine( &rwc );

    if  ( docRtfSaveDocumentProperties( &rwc,
					&dpSaveMask, &(bd->bdProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ds )
	{
	docRtfWriteDestinationBegin( &rwc, RTFtag__SelOpen );
	docRtfWriteDestinationEnd( &rwc );
	}

    if  ( docRtfSaveDocNotesSeparators( &rwc, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRtfWriteSelection( &rwc, ds ) )
	{ LDEB(1); rval= -1; goto ready; }

    docRtfWriteDestinationEnd( &rwc );
    docRtfWriteNextLine( &rwc );

  ready:
    docRtfCleanWritingContext( &rwc );

    return rval;
    }

