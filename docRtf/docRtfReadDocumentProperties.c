/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<stdlib.h>

#   include	<utilDateTime.h>
#   include	<textOfficeCharset.h>
#   include	<textConverter.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTrace.h"
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Remember Document Properties.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Remember the code page for the document.				*/
/*  = All values from the Word 2003 spec explicitly included.		*/
/*  = String values checked against GNU iconv 'iconv (GNU libc) 2.3.6'	*/
/*  = Check strings against iconv -l					*/
/*									*/
/************************************************************************/

static int docRtfSetAnsicpg(	struct TextConverter *	tc,
				int			ansicpg )
    {
    char	scratch[20];

    switch( ansicpg )
	{
	case 0:
	    textConverterSetNativeEncodingName( tc, DOC_RTF_AnsiCharsetName );
	    return 0;

	case 437: case 819: case 850: case 852:
	case 460: case 862: case 863: case 864:
	case 465: case 866: case 874: case 932:
	case 936: case 949: case 950:
	case 1361:
	    sprintf( scratch, "CP%d", ansicpg );
	    textConverterSetNativeEncodingName( tc, scratch );
	    return 0;

	case 708:
	    textConverterSetNativeEncodingName( tc, "ASMO-708" );
	    return 0;
	case 709:
	    textConverterSetNativeEncodingName( tc, "ASMO_449" );
	    return 0;

	case 1250: case 1251: case 1252: case 1253:
	case 1254: case 1255: case 1256: case 1257:
	case 1258:
	    sprintf( scratch, "CP%d", ansicpg );
	    textConverterSetNativeEncodingName( tc, scratch );
	    return 0;

	case 10000:
	    textConverterSetNativeEncodingName( tc, "MACINTOSH" );
	    return 0;

	case 710:
	case 711:
	case 720:
	default:
	    LDEB(ansicpg);
	    textConverterSetNativeEncodingName( tc, DOC_RTF_AnsiCharsetName );
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Remember the document charset. It is translated to a code page	*/
/*  where possible.							*/
/*									*/
/************************************************************************/

static int docRtfReadSetDocumentCharset(
				RtfReader *		rr,
				int			arg )
    {
    switch( arg )
	{
	case DOCcharsetANSI:
	    if  ( docRtfSetAnsicpg( rr->rrRtfTextConverter, 1252 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetPC:
	    if  ( docRtfSetAnsicpg( rr->rrRtfTextConverter, 437 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetPCA:
	    if  ( docRtfSetAnsicpg( rr->rrRtfTextConverter, 850 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetMAC:
	    if  ( docRtfSetAnsicpg( rr->rrRtfTextConverter, 10000 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	default:
	    LDEB(arg); return -1;
	}
    }

int docRtfRememberDocProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    SectionProperties *		sp= &(rr->rrSectionProperties);

    int				charset= -1;

    switch( rcw->rcwID )
	{
	case DGpropLEFT_MARGIN:
	    sp->spDocumentGeometry.dgMargins.roLeftOffset= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropTOP_MARGIN:
	    sp->spDocumentGeometry.dgMargins.roTopOffset= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropRIGHT_MARGIN:
	    sp->spDocumentGeometry.dgMargins.roRightOffset= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropBOTTOM_MARGIN:
	    sp->spDocumentGeometry.dgMargins.roBottomOffset= arg;
	    break;
	case DGpropPAGE_WIDTH:
	    sp->spDocumentGeometry.dgPageWideTwips= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropPAGE_HEIGHT:
	    sp->spDocumentGeometry.dgPageHighTwips= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropGUTTER:
	    sp->spDocumentGeometry.dgGutterTwips= arg;
	    rr->rrGotDocGeometry= 1;
	    break;
	case DGpropMARGMIR:
	    arg= arg != 0;
	    sp->spDocumentGeometry.dgMirrorMargins= arg;
	    rr->rrGotDocGeometry= 1;
	    break;

	case DPpropFACING_PAGES:
	    arg= arg != 0;
	    break;
	case DPpropGUTTER_HORIZONTAL:
	    arg= arg != 0;
	    break;
	case DPpropWIDOWCTRL:
	    arg= arg != 0;
	    break;
	case DPpropTWO_ON_ONE:
	    arg= arg != 0;
	    break;
	case DPpropDOCTEMP:
	    arg= arg != 0;
	    break;

	case DPpropDEFF:
	    rr->rrDefaultFont= arg;
	    if  ( docRtfReadMapFont( rr, &arg, &charset,
						rr->rrDefaultFont ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFDBCH:
	    rr->rrDefaultFontDbch= arg;
	    if  ( docRtfReadMapFont( rr, &arg, &charset,
						rr->rrDefaultFontDbch ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFLOCH:
	    rr->rrDefaultFontLoch= arg;
	    if  ( docRtfReadMapFont( rr, &arg, &charset,
						rr->rrDefaultFontLoch ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFHICH:
	    rr->rrDefaultFontHich= arg;
	    if  ( docRtfReadMapFont( rr, &arg, &charset,
						rr->rrDefaultFontHich ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFBI:
	    rr->rrDefaultFontBi= arg;
	    if  ( docRtfReadMapFont( rr, &arg, &charset,
						rr->rrDefaultFontBi ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;

	case DPpropDOC_CHARSET:
	    if  ( docRtfReadSetDocumentCharset( rr, arg ) )
		{ SLDEB(rcw->rcwWord,arg); return 0;	}
	    break;
	case DPpropANSICPG:
	    if  ( docRtfSetAnsicpg( rr->rrRtfTextConverter, arg ) )
		{ SLDEB(rcw->rcwWord,arg); return 0;	}
	    break;

	case DPpropTOP_BORDER:
	case DPpropBOTTOM_BORDER:
	case DPpropLEFT_BORDER:
	case DPpropRIGHT_BORDER:
	case DPpropHEAD_BORDER:
	case DPpropFOOT_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;

	case DPpropNOTETYPES:
	    docInitFootEndNotesProperties(
			    &(rr->rrDocument->bdProperties->dpNotesProps) );
	    break;

	case DPprop_IGNORED:
	    return 0;

	default:
	    break;
	}

    if  ( rr->rrTraceReader && rr->rrTraceReader->rtrTraceInProps )
	{
	/*  Working copy for undo */
	if  ( docSetDocumentProperty( &(rr->rrDocumentProperties),
						    rcw->rcwID, arg ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return 0;	}

	PROPmaskADD( &(rr->rrDocPropertyMask), rcw->rcwID );
	}
    else{
	/*  Document */
	if  ( docSetDocumentProperty( rr->rrDocument->bdProperties,
						    rcw->rcwID, arg ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return 0;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember a document property that has a string value.		*/
/*									*/
/************************************************************************/

int docRtfCommitDocPropText(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    int		rval= 0;
    const int	removeSemicolon= rcw->rcwID == DPpropGENERATOR;

    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    if  ( rr->rrTraceReader && rr->rrTraceReader->rtrTraceInProps )
	{
	if  ( docSetDocumentPropertyString( &(rr->rrDocumentProperties),
						    rcw->rcwID, text, size ) )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready;	}

	PROPmaskADD( &(rr->rrDocPropertyMask), rcw->rcwID );
	}
    else{
	if  ( docSetDocumentPropertyString( rr->rrDocument->bdProperties,
						    rcw->rcwID, text, size ) )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready;	}
	}

  ready:

    if  ( text )
	{ free( text );		}

    return rval;
    }

/************************************************************************/
/*									*/
/*  A date/time field in the info group.				*/
/*									*/
/************************************************************************/

int docRtfDocTimeGroup(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    utilInvalidateTime( &(rr->rrTm) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
		(RtfControlWord *)0, docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( rr->rrTraceReader && rr->rrTraceReader->rtrTraceInProps )
	{
	if  ( docSetDocumentPropertyTime( &(rr->rrDocumentProperties),
						rcw->rcwID, &(rr->rrTm) ) )
	    { SDEB(rcw->rcwWord); return -1;	}

	PROPmaskADD( &(rr->rrDocPropertyMask), rcw->rcwID );
	}
    else{
	if  ( docSetDocumentPropertyTime( rr->rrDocument->bdProperties,
						rcw->rcwID, &(rr->rrTm) ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember fields from dates and times.				*/
/*									*/
/************************************************************************/

int docRtfReadTimeField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    if  ( utilDateTimeSetField( &(rr->rrTm), rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember whether the current selection is open or not.		*/
/*									*/
/************************************************************************/

int docRtfSelectionOpen(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    DocumentProperties *	dp= rr->rrDocument->bdProperties;

    dp->dpHasOpenEnd= 1;

    if  ( docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
					(RtfGotText)0, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read 'info' group.							*/
/*									*/
/************************************************************************/

int docRtfReadInfo(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    res= docRtfReadGroup( rcw, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

