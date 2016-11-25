/************************************************************************/
/*									*/
/*  Exchange text properties with an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Save a text attribute (change).					*/
/*									*/
/************************************************************************/

void docRtfSaveTextAttribute(		SimpleOutputStream *	sos,
					int *			pCol,
					const PropertyMask *	updMask,
					const TextAttribute *	ta )
    {
    if  ( PROPmaskISSET( updMask, TApropFONTFAMILY ) )
	{ docRtfWriteArgTag( "\\f", pCol, ta->taFontNumber, sos ); }

    if  ( PROPmaskISSET( updMask, TApropFONTSIZE ) )
	{ docRtfWriteArgTag( "\\fs", pCol, ta->taFontSizeHalfPoints, sos ); }

    if  ( PROPmaskISSET( updMask, TApropTEXT_COLOR ) )
	{ docRtfWriteArgTag( "\\cf", pCol, ta->taTextColorNumber, sos ); }

    if  ( PROPmaskISSET( updMask, TApropFONTBOLD ) )
	{
	if  ( ta->taFontIsBold )
	    { docRtfWriteTag( "\\b", pCol, sos );			}
	else{ docRtfWriteArgTag( "\\b", pCol, ta->taFontIsBold, sos );	}
	}

    if  ( PROPmaskISSET( updMask, TApropFONTSLANTED ) )
	{
	if  ( ta->taFontIsSlanted )
	    { docRtfWriteTag( "\\i", pCol, sos );			}
	else{ docRtfWriteArgTag( "\\i", pCol, ta->taFontIsSlanted, sos ); }
	}

    if  ( PROPmaskISSET( updMask, TApropTEXTUNDERLINED ) )
	{
	if  ( ta->taTextIsUnderlined )
	    { docRtfWriteTag( "\\ul", pCol, sos );			 }
	else{ docRtfWriteArgTag( "\\ul", pCol, ta->taTextIsUnderlined, sos ); }
	}

    if  ( PROPmaskISSET( updMask, TApropSUPERSUB ) )
	{
	switch( ta->taSuperSub )
	    {
	    case DOCfontREGULAR:
		docRtfWriteTag( "\\nosupersub", pCol, sos );
		break;

	    case DOCfontSUPERSCRIPT:
		docRtfWriteTag( "\\super", pCol, sos );
		break;

	    case DOCfontSUBSCRIPT:
		docRtfWriteTag( "\\sub", pCol, sos );
		break;

	    default:
		LDEB(ta->taSuperSub);
	    }
	}

    if  ( PROPmaskISSET( updMask, TApropSMALLCAPS ) )
	{
	if  ( ta->taSmallCaps )
	    { docRtfWriteTag( "\\scaps", pCol, sos );			 }
	else{ docRtfWriteArgTag( "\\scaps", pCol, ta->taSmallCaps, sos ); }
	}

    if  ( PROPmaskISSET( updMask, TApropCAPITALS ) )
	{
	if  ( ta->taCapitals )
	    { docRtfWriteTag( "\\caps", pCol, sos );			}
	else{ docRtfWriteArgTag( "\\caps", pCol, ta->taCapitals, sos );	}
	}

    if  ( PROPmaskISSET( updMask, TApropSTRIKETHROUGH ) )
	{
	if  ( ta->taHasStrikethrough )
	    { docRtfWriteTag( "\\strike", pCol, sos );			}
	else{
	    docRtfWriteArgTag( "\\strike", pCol, ta->taHasStrikethrough, sos );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a text property when reading RTF.				*/
/*									*/
/************************************************************************/

static int docRtfRememberTextPropertyImpl(	TextAttribute *		ta,
						const BufferDocument *	bd,
						const RtfControlWord *	rcw,
						int			arg )
    {
    switch( rcw->rcwID )
	{
	case TAprop_NONE:
	    utilInitTextAttribute( ta );
	    if  ( bd )
		{ ta->taFontNumber= bd->bdProperties.dpDefaultFont;	}
	    ta->taFontSizeHalfPoints= 24;
	    break;

	case TApropFONTFAMILY:
	    ta->taFontNumber= arg;
	    break;

	case TApropFONTSIZE:
	    ta->taFontSizeHalfPoints= arg;
	    break;

	case TApropTEXT_COLOR:
	    ta->taTextColorNumber= arg;
	    break;

	case TApropFONTBOLD:
	    ta->taFontIsBold= arg != 0;
	    break;

	case TApropFONTSLANTED:
	    ta->taFontIsSlanted= arg != 0;
	    break;

	case TApropTEXTUNDERLINED:
	    ta->taTextIsUnderlined= arg != 0;
	    break;

	case TApropSUPERSUB:
	    ta->taSuperSub= rcw->rcwEnumValue;
	    break;

	case TApropSMALLCAPS:
	    ta->taSmallCaps= arg != 0;
	    break;

	case TApropCAPITALS:
	    ta->taCapitals= arg != 0;
	    break;

	case TApropSTRIKETHROUGH:
	    ta->taHasStrikethrough= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return -1;
	}

    return 0;
    }

int docRtfRememberTextProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( rcw->rcwID == TAprop_NONE )
	{ rrc->rrcInDeletedText= 0;	}

    if  ( docRtfRememberTextPropertyImpl(
		    &(rrs->rrsTextAttribute), rrc->rrcBd, rcw, arg ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( rcw->rcwID != TAprop_NONE && rcw->rcwID < TAprop_COUNT )
	{ PROPmaskADD( &(rrc->rrcDocumentStyle.dsTextMask), rcw->rcwID ); }

    return 0;
    }

int docRtfRememberPntextProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    if  ( docRtfRememberTextPropertyImpl(
				&(rrc->rrcParagraphNumber.pnTextAttribute),
				rrc->rrcBd, rcw, arg ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }
