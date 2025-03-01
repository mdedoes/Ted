/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReadWrite.h"
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	<docNotes.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

int docRtfWriteGetFet(	PropertyMask *		dpSaveMask,
			const struct BufferDocument *	bd )
    {
    struct DocumentNote *	dn;
    int				fet= 0;

    int				hasFootNotes= 0;
    int				hasEndNotes= 0;

    if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
	{
	hasFootNotes= 1;
	PROPmaskADD( dpSaveMask, DPpropFOOTNOTE_JUSTIFICATION );
	}

    if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
	{
	hasEndNotes= 1;
	PROPmaskADD( dpSaveMask, DPpropENDNOTE_JUSTIFICATION );
	}

    if  ( hasEndNotes )
	{
	if  ( hasFootNotes )
	    { fet= 2;	}
	else{ fet= 1;	}
	}
    else{ fet= 0;	}

    return fet;
    }

int docRtfSaveDocument(		struct SimpleOutputStream *		sos,
				struct BufferDocument *			bd,
				const struct DocumentSelection *	ds,
				int					flags )
    {
    int				rval= 0;
    RtfWriter *			rw= (RtfWriter *)0;

    PropertyMask		dpSaveMask;

    int				fet;

    rw= docRtfOpenWriter( sos, bd, flags );
    if  ( ! rw )
	{ XDEB(rw); rval= -1; goto ready;	}

    if  ( docRtfDocPropMask( &dpSaveMask, bd->bdProperties ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bd->bdStyleSheet.dssStyleCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropSTYLESHEET );	}

    if  ( docRtfWriteBuildFontAdmin( rw ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfWriteDestinationBegin( rw, "rtf1\\ansi" );
    docRtfWriteNextLine( rw );

    fet= docRtfWriteGetFet( &dpSaveMask, bd );

    if  ( docRtfSaveDocumentProperties( rw, fet,
					&dpSaveMask, bd->bdProperties ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ds )
	{
	docRtfWriteDestinationBegin( rw, RTFtag__SelOpen );
	docRtfWriteDestinationEnd( rw );
	}

    if  ( docRtfSaveDocNotesSeparators( rw, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRtfWriteSelection( rw, ds ) )
	{ LDEB(1); rval= -1; goto ready; }

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

  ready:

    if  ( rw )
	{ docRtfCloseWriter( rw );	}

    return rval;
    }

