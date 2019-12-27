/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  struct BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<ctype.h>

#   include	<textOfficeCharset.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"
#   include	<docBuf.h>
#   include	<fontDocFontList.h>
#   include	<fontDocFontListImpl.h>

#   include	<appDebugon.h>

/************************************************************************/

void docRtfRestartFont(		RtfReader *	rr )
    {
    fontCleanDocumentFont( &(rr->rrCurrentFont) );
    fontInitDocumentFont( &(rr->rrCurrentFont) );
    fontInitEncodedFont( &(rr->rrCurrentEncodedFont) );
    if  ( rr->rrDocument )
	{
	rr->rrCurrentEncodedFont.ecCharset=
			    rr->rrDocument->bdProperties->dpDocumentCharset;
	}

    return ;
    }

/************************************************************************/
/*									*/
/*  Consume a font table.						*/
/*									*/
/************************************************************************/

int docRtfCommitFontDest(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    const int		removeSemicolon= 0;
    RtfReadingState *	rrs= rr->rrState;

    switch( rcw->rcwID )
	{
	case DFpropALT_NAME:
	    if  ( docRtfMemoryBufferSetText( &(rr->rrCurrentFont.dfAltName),
						    rr, removeSemicolon ) )
		{ LDEB(1); return -1; }
	    return 0;

	case DFpropPANOSE:
	    if  ( rrs->rrsSavedText.mbSize != FONTlenPANOSE )
		{
		LLDEB(rrs->rrsSavedText.mbSize,FONTlenPANOSE);
		utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );
		return 0;
		}

	    memcpy( rr->rrCurrentFont.dfPanose,
				rrs->rrsSavedText.mbBytes,
				rrs->rrsSavedText.mbSize );
	    utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );

	    rr->rrCurrentFont.dfPanose[FONTlenPANOSE]= '\0';
	    return 0;

	case DFpropFONT_FILE:
	    if  ( docRtfMemoryBufferSetText( &(rr->rrCurrentFont.dfFontFile),
						    rr, removeSemicolon ) )
		{ LDEB(1); return -1; }
	    return 0;

	case DFpropFONT_BYTES:
	    if  ( docRtfMemoryBufferSetText( &(rr->rrCurrentFont.dfFontBytes),
						    rr, removeSemicolon ) )
		{ LDEB(1); return -1; }
	    return 0;

	default:
	    LSDEB(rcw->rcwID,rcw->rcwWord);
	    return -1;
	}

    }

static int docRtfAddCurrentFontToList(	RtfReader *	rr )
    {
    int		rval= 0;
    const int	removeSemicolon= 1;

    if  ( docRtfMemoryBufferSetText( &(rr->rrCurrentFont.dfName),
						    rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( utilMemoryBufferIndexOf( &(rr->rrCurrentFont.dfName), '@' ) == 0 )
	{
	utilMemoryBufferReplaceBytes( &(rr->rrCurrentFont.dfName), 0, 1,
						(const unsigned char *)0, 0 );
	}

    if  ( ! utilMemoryBufferIsEmpty( &(rr->rrCurrentFont.dfName) ) )
	{
	DocumentProperties *	dp= rr->rrDocument->bdProperties;
	DocumentFontList *	dfl= dp->dpFontList;

	if  ( fontAddEncodedFontToList( dfl,
					&(rr->rrEncodedFontList),
					&(rr->rrCurrentFont),
					&(rr->rrCurrentEncodedFont) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docRtfRestartFont( rr );

    return rval;
    }

static int docRtfFontName(	RtfReader *		rr,
				const char *		name,
				int			len )
    {
    if  ( docRtfSaveDocEncodedText( rr, name, len ) )
	{ LDEB(len); return -1;	}

    if  ( rr->rrCurrentEncodedFont.efFileFontNumber < 0 )
	{ LDEB(rr->rrCurrentEncodedFont.efFileFontNumber); return 0;	}

    if  ( docRtfAddCurrentFontToList( rr ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfCommitCurrentFont(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    { return docRtfAddCurrentFontToList( rr );	}

int docRtfFontProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case DFpropFAMILY_STYLE:
	    rr->rrCurrentFont.dfStyleInt= arg;
	    break;
	case DFpropCHARSET:
	    rr->rrCurrentEncodedFont.ecCharset= arg;
	    break;
	case DFpropCODEPAGE:
	    rr->rrCurrentEncodedFont.ecCodepage= arg;
	    break;
	case DFpropPITCH:
	    rr->rrCurrentFont.dfPitch= arg;
	    break;
	case DFpropFONT_TYPE:
	    rr->rrCurrentFont.dfEmbeddedType= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,arg);
	}

    return 0;
    }

static int docRtfFontGroup(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    docRtfRestartFont( rr );

    rr->rrCurrentEncodedFont.efFileFontNumber= arg;

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				    (RtfControlWord *)0,
				    docRtfSaveDocEncodedText,
				    docRtfCommitCurrentFont ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfThemeFontGroup(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    docRtfRestartFont( rr );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				    (RtfControlWord *)0,
				    docRtfSaveDocEncodedText,
				    docRtfCommitCurrentFont ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static RtfControlWord	docRtfFontTableGroups[]=
    {
	RTF_DEST_XX( "f",	RTCscopeFONT, RTFidF,	docRtfFontGroup ),

	RTF_DEST_XX( "flomajor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fhimajor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fdbmajor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fbimajor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "flominor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fhiminor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fdbminor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),
	RTF_DEST_XX( "fbiminor",RTCscopeTHEME_FONT, RTFidF,	docRtfThemeFontGroup ),

	{ (char *)0, 0, 0 }
    };

static int docRtfCommitFontTable(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    struct BufferDocument *		bd= rr->rrDocument;
    DocumentProperties *	dp= bd->bdProperties;

    int				charset;

    if  ( rr->rrDefaultFont >= 0 )
	{
	if  ( docRtfReadMapFont( rr, &(dp->dpDefaultFont), &charset,
					    rr->rrDefaultFont ) < 0 )
	    { LDEB(rr->rrDefaultFont);	}
	}

    if  ( rr->rrDefaultFontDbch >= 0 )
	{
	if  ( docRtfReadMapFont( rr, &(dp->dpDefaultFontDbch), &charset,
					    rr->rrDefaultFontDbch ) < 0 )
	    { LDEB(rr->rrDefaultFontDbch);	}
	}

    if  ( rr->rrDefaultFontLoch >= 0 )
	{
	if  ( docRtfReadMapFont( rr, &(dp->dpDefaultFontLoch), &charset,
					    rr->rrDefaultFontLoch ) < 0 )
	    { LDEB(rr->rrDefaultFontLoch);	}
	}

    if  ( rr->rrDefaultFontHich >= 0 )
	{
	if  ( docRtfReadMapFont( rr, &(dp->dpDefaultFontHich), &charset,
					    rr->rrDefaultFontHich ) < 0 )
	    { LDEB(rr->rrDefaultFontHich);	}
	}

    if  ( rr->rrDefaultFontBi >= 0 )
	{
	if  ( docRtfReadMapFont( rr, &(dp->dpDefaultFontBi), &charset,
						rr->rrDefaultFontBi ) < 0 )
	    { LDEB(rr->rrDefaultFontBi);	}
	}

    return 0;
    }

int docRtfFontTable(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    DocumentProperties *	dp= rr->rrDocument->bdProperties;
    DocumentFontList *		dfl= dp->dpFontList;
    int				f;

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
	    docRtfFontTableGroups, docRtfFontName, docRtfCommitFontTable ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    for ( f= 0; f < dfl->dflFontCount; f++ )
	{
	DocumentFont *	df= fontFontListGetFontByNumber( dfl, f );

	if  ( ! df || df->dfDocFontNumber < 0 )
	    { continue;	}

	fontDetermineEncoding( df );
	}

    return 0;
    }

/************************************************************************/

int docRtfReadMapFont(	const RtfReader *		rr,
			int *				pDocFontNum,
			int *				pCharset,
			int				fileFontNum )
    {
    return fontMapFileFont( &(rr->rrEncodedFontList),
					pDocFontNum, pCharset, fileFontNum );
    }
