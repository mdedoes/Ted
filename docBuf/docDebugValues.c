#   include	"docBufConfig.h"

#   include	<stdio.h>

#   include	"docField.h"
#   include	<docTreeType.h>
#   include	<docObjectProperties.h>
#   include	<docTextParticule.h>
#   include	<textAttribute.h>
#   include	<docBreakKind.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

const char * docKindStr( int kind )
    {
    static char	scratch[12];

    switch( kind )
	{
	case TPkindSPAN:	return "SPAN";
	case TPkindTAB:		return "TAB ";
	case TPkindOBJECT:	return "OBJ ";
	case TPkindFIELDHEAD:	return "<FLD";
	case TPkindFIELDTAIL:	return "FLD>";
	case TPkindLINEBREAK:	return "LINE";
	case TPkindPAGEBREAK:	return "PAGE";
	case TPkindCOLUMNBREAK:	return "COL ";
	case TPkindCHFTNSEP:	return "SEP ";
	case TPkindCHFTNSEPC:	return "SEPC";
	case TPkindOPT_HYPH:	return "HYPH";
	case TPkindLTR_MARK:	return "LTR ";
	case TPkindRTL_MARK:	return "RTL ";

	default:
	    sprintf( scratch, "%-4d", kind );
	    return scratch;
	}
    }

const char * docTreeTypeStr( int treeType )
    {
    static char	scratch[12];

    switch( treeType )
	{
	case DOCinBODY:			return "BODY";

	case DOCinFIRST_HEADER:		return "FIRST_HEADER";
	case DOCinLEFT_HEADER:		return "LEFT_HEADER";
	case DOCinRIGHT_HEADER:		return "RIGHT_HEADER";
	case DOCinLAST_HEADER:		return "LAST_HEADER";

	case DOCinFIRST_FOOTER:		return "FIRST_FOOTER";
	case DOCinLEFT_FOOTER:		return "LEFT_FOOTER";
	case DOCinRIGHT_FOOTER:		return "RIGHT_FOOTER";
	case DOCinLAST_FOOTER:		return "LAST_FOOTER";

	case DOCinFOOTNOTE:		return "FOOTNOTE";
	case DOCinENDNOTE:		return "ENDNOTE";

	case DOCinFTNSEP:		return "FTNSEP";
	case DOCinFTNSEPC:		return "FTNSEPC";
	case DOCinFTNCN:		return "FTNCN";

	case DOCinAFTNSEP:		return "AFTNSEP";
	case DOCinAFTNSEPC:		return "AFTNSEPC";
	case DOCinAFTNCN:		return "AFTNCN";

	case DOCinSHPTXT:		return "SHPTXT";
	case DOCinFIELD_INSTRUCTIONS:	return "FLDINST";

	default:
	    sprintf( scratch, "%d", treeType );
	    return scratch;
	}
    }

const char * docObjectKindStr(	int	objectKind )
    {
    static char	scratch[12];

    switch( objectKind )
	{
	case DOCokUNKNOWN:		return "UNKNOWN";
	case DOCokPICTWMETAFILE:	return "PICTWMETAFILE";
	case DOCokPICTPNGBLIP:		return "PICTPNGBLIP";
	case DOCokPICTJPEGBLIP:		return "PICTJPEGBLIP";
	case DOCokPICTEMFBLIP:		return "PICTEMFBLIP";
	case DOCokMACPICT:		return "MACPICT";
	case DOCokPMMETAFILE:		return "PMMETAFILE";
	case DOCokDIBITMAP:		return "DIBITMAP";
	case DOCokWBITMAP:		return "WBITMAP";
	case DOCokOLEOBJECT:		return "OLEOBJECT";
	case DOCokEPS_FILE:		return "EPS_FILE";
	case DOCokDRAWING_SHAPE:	return "DRAWING_SHAPE";

	default:
	    sprintf( scratch, "%d", objectKind );
	    return scratch;
	}
    }

const char * docFieldKindStr( int kind )
    {
    static char	scratch[12];

    if  ( kind >= 0 && kind < DOC_FieldKindCount )
	{ return DOC_FieldKinds[kind].fkiLabel;	}

    sprintf( scratch, "%4d", kind );
    return scratch;
    }

const char * docAttributeStr(	const TextAttribute *		ta )
    {
    static char	scratch[5];
    char *	to= scratch;

    if  ( ta->taFontIsBold )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}
    if  ( ta->taFontIsSlanted )
	{ *(to++)= 'I';	}
    else{ *(to++)= 'i';	}
    if  ( ta->taTextIsUnderlined )
	{ *(to++)= 'U';	}
    else{ *(to++)= 'u';	}

    *(to)= '\0';
    return scratch;
    }

const char * docParticuleFlagsStr(	int	flags )
    {
    static char	scratch[7];
    char *	to= scratch;

    if  ( flags & TPflagXMATCH_LEFT )
	{ *(to++)= '[';	}
    else{ *(to++)= '.';	}

    if  ( flags & TPflagLEFT_BORDER )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}

    if  ( flags & TPflagRIGHT_BORDER )
	{ *(to++)= 'B';	}
    else{ *(to++)= 'b';	}

    if  ( flags & TPflagXMATCH_RIGHT )
	{ *(to++)= ']';	}
    else{ *(to++)= '.';	}

    *(to)= '\0';
    return scratch;
    }

const char * docBreakKindStr( int kind )
    {
    static char scratch[20];

    switch( kind )
	{
	case DOCibkNONE:	return "NONE";
	case DOCibkCOL:		return "COL";
	case DOCibkPAGE:	return "PAGE";
	case DOCibkEVEN:	return "EVEN";
	case DOCibkODD:		return "ODD";

	default:
	    sprintf( scratch, "%d", kind );
	    return scratch;
	}
    }
