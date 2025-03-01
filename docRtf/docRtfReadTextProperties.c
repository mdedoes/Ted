/************************************************************************/
/*									*/
/*  Exchange text properties with an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfFlags.h"
#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a text property when reading RTF.				*/
/*									*/
/************************************************************************/

static int docRtfRememberTextPropertyImpl(	TextAttribute *		ta,
						const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case TAprop_NONE:
	    docPlainTextAttribute( ta, rr->rrDocument );
	    return 0;

	case TApropFONTSIZE:
	    if  ( arg < 1 || arg > 2000 )
		{ LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,arg); arg= 20; }
	    break;

	case TApropBORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;

	default:
	    break;
	}

    if  ( textSetTextProperty( ta, rcw->rcwID, arg ) < 0 )
	{ LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,arg); return -1;	}

    return 0;
    }

int docRtfRememberTextShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( docSetShadingProperty( &(rrs->rrsTextShading),
						    rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    rrs->rrsTextShadingChanged= 1;

    return 0;
    }

int docRtfRememberTextProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( rcw->rcwID == TApropTEXT_STYLE )
	{
	const	DocumentStyleSheet *	dss= &(rr->rrDocument->bdStyleSheet);

	rr->rrStyle.dsLevel= DOClevSPAN;

	if  ( arg >= 0 && arg < dss->dssStyleCount )
	    {
	    const DocumentStyle *	ds= dss->dssStyles[arg];

	    if  ( ds && ds->dsLevel == DOClevSPAN )
		{
		PropertyMask	doneMask;

		if  ( rrs->rrsTextShadingChanged )
		    { docRtfRefreshTextShading( rr, rrs );	}

		textUpdateTextAttribute( &doneMask, &(rrs->rrsTextAttribute),
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
		}
	    }
	}

    /* Also used to start a font in the font list		*/
    if  ( rcw->rcwID == TApropFONT_NUMBER )
	{
	docRtfRestartFont( rr );

	rr->rrCurrentEncodedFont.efFileFontNumber= arg;

	if  ( ! ( rr->rrReadFlags & RTFflagUNENCODED ) )
	    {
	    if  ( ! docRtfReadMapFont( rr,
			    &(rr->rrCurrentEncodedFont.ecBufFontNumber),
			    &(rr->rrCurrentEncodedFont.ecCharset),
			    rr->rrCurrentEncodedFont.efFileFontNumber ) )
		{
		rr->rrCurrentFont.dfDocFontNumber=
				    rr->rrCurrentEncodedFont.ecBufFontNumber;
		rrs->rrsTextCharset= rr->rrCurrentEncodedFont.ecCharset;

		arg= rr->rrCurrentEncodedFont.ecBufFontNumber;
		}
	    }
	else{
	    rr->rrCurrentEncodedFont.ecBufFontNumber=
				rr->rrCurrentEncodedFont.efFileFontNumber;
	    }
	}

    if  ( rcw->rcwID == TAprop_NONE )
	{
	docInitItemShading( &(rrs->rrsTextShading) );
	rrs->rrsTextShadingChanged= 0;
	}

    if  ( docRtfRememberTextPropertyImpl( &(rrs->rrsTextAttribute),
							    rcw, arg, rr ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( rcw->rcwID != TAprop_NONE && rcw->rcwID < TAprop_COUNT )
	{ PROPmaskADD( &(rr->rrStyle.dsTextMask), rcw->rcwID ); }

    return 0;
    }

void docRtfResetTextAttribute(	RtfReadingState *	rrs,
				struct BufferDocument *	bd )
    {
    docPlainTextAttribute( &(rrs->rrsTextAttribute), bd );
    docInitItemShading( &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate the separate text shading properties that we have		*/
/*  collected to a shading number.					*/
/*									*/
/************************************************************************/

void docRtfRefreshTextShading(	RtfReader *		rr,
				RtfReadingState *	rrs )
    {
    rrs->rrsTextAttribute.taShadingNumber= docItemShadingNumber(
				    rr->rrDocument, &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    return;
    }
