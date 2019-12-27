/************************************************************************/
/*									*/
/*  The format '*' flag of a field.					*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	<ctype.h>

#   include	"docFieldFormat.h"
#   include	"docFieldInstructions.h"

#   include	<appDebugon.h>

int docFieldHasMergeformat(		const FieldInstructions *	fi,
					int				comp )
    {
    const InstructionsComponent *	ic= fi->fiComponents+ comp;

    if  ( comp < fi->fiComponentCount -1			&&
	  docComponentIsFlag( fi, comp, '*' )			&&
	  docComponentEqualsWordNoCase( ic+ 1, "mergeformat", 11 )	)
	{ return 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate the argument of the \\* format flag in the field		*/
/*  instructions to an integer value.					*/
/*									*/
/************************************************************************/

static const char * FieldFormats[]=
    {
    "",			/*  FIELDformatASIS	*/

	    /* Case shifting	*/
    "Lower",
    "Upper",
    "FirstCap",
    "Caps",

	    /* Number formatting */
    "Alphabetic",	/*  FIELDformatALPHABETIC_UPPER	*/
    "Arabic",
    "CardText",
    "DollarText",
    "Hex",		/*  FIELDformatHEX_UPPER		*/
    "Ordinal",
    "OrdText",
    "Roman",		/*  FIELDformatROMAN_UPPER		*/

    "CharFormat",
    "MergeFormat",

    "alphabetic",	/*  FIELDformatALPHABETIC_LOWER	*/
    "hex",		/*  FIELDformatHEX_LOWER		*/
    "roman",		/*  FIELDformatROMAN_LOWER		*/
    };

const int FieldFormatCount= sizeof(FieldFormats)/sizeof(char *);

static int FieldFormatLength[sizeof(FieldFormats)/sizeof(char *)]=
    {
    -1,
    };

int docFieldFormatInt(	const InstructionsComponent *	ic ) 
    {
    const char *	format;
    int			i;

    format= utilMemoryBufferGetString( &(ic->icBuffer) );

    if  ( FieldFormatCount != FIELDformat_COUNT )
	{ LLDEB(FieldFormatCount,FIELDformat_COUNT);	}

    if  ( FieldFormatLength[0] < 0 )
	{
	for ( i= 0; i < FieldFormatCount; i++ )
	    { FieldFormatLength[i]= strlen( FieldFormats[i] ); }
	}

    for ( i= 1; i < FieldFormatCount; i++ )
	{
	int		j;
	const char *	ref= FieldFormats[i];

	if  ( FieldFormatLength[i] != ic->icBuffer.mbSize )
	    { continue;	}

	for ( j= 0; j < ic->icBuffer.mbSize; j++ )
	    {
	    if  ( tolower( format[j] ) != tolower( ref[j] ) )
		{ break;	}
	    }

	if  ( j == ic->icBuffer.mbSize )
	    {
	    if  ( i == FIELDformatROMAN_UPPER	&&
		  islower( format[0] )		)
		{ return FIELDformatROMAN_LOWER;		}

	    if  ( i == FIELDformatALPHABETIC_UPPER	&&
		  islower( format[0] )		)
		{ return FIELDformatALPHABETIC_LOWER;	}

	    if  ( i == FIELDformatHEX_UPPER		&&
		  islower( format[0] )		)
		{ return FIELDformatHEX_LOWER;		}

	    return i;
	    }
	}

    return -1;
    }

int docFieldInstructionsAddFormat(	FieldInstructions *	fi,
					int			format )
    {
    MemoryBuffer	mb;

    if  ( FieldFormatCount != FIELDformat_COUNT )
	{ LLDEB(FieldFormatCount,FIELDformat_COUNT);	}

    if  ( format < 0 || format >= FieldFormatCount )
	{ LLDEB(format,FieldFormatCount);	}

    if  ( format == FIELDformatASIS )
	{ return 0;	}

    mb.mbBytes= (unsigned char *)FieldFormats[format];
    mb.mbSize= strlen( FieldFormats[format] );

    return docFieldInstructionsAddArgFlag( fi, '*', &mb );
    }

/************************************************************************/
/*									*/
/*  Extract an integer value from a component in the field instructions.*/
/*									*/
/************************************************************************/

int docFieldComponentInt(	int *				pVal,
				const InstructionsComponent *	ic ) 
    {
    const char *	format;
    char *		p;
    long		l;

    if  ( ic->icBuffer.mbSize < 1 )
	{ LDEB(ic->icBuffer.mbSize); return -1;	}

    format= (const char *)ic->icBuffer.mbBytes;
    p= (char *)format;

    l= strtol( format, &p, 10 );
    if  ( p == format )
	{ return -1;	}
    else{
	while( p- format < ic->icBuffer.mbSize && p[0] == ' ' )
	    { p++;	}
	if  ( p- format < ic->icBuffer.mbSize )
	    { return -1;	}
	}

    *pVal= l;
    return 0;
    }

/************************************************************************/

int docFieldComponentNumberFormat(
				unsigned char *			pNumberFormat,
				const InstructionsComponent *	ic ) 
    {
    int		f;

    f= docFieldFormatInt( ic );
    switch( f )
	{
	case FIELDformatALPHABETIC_UPPER:
	case FIELDformatARABIC:
	case FIELDformatCARDTEXT:
	case FIELDformatDOLLARTEXT:
	case FIELDformatHEX_UPPER:
	case FIELDformatORDINAL:
	case FIELDformatORDTEXT:
	case FIELDformatROMAN_UPPER:

	case FIELDformatALPHABETIC_LOWER:
	case FIELDformatHEX_LOWER:
	case FIELDformatROMAN_LOWER:
	    *pNumberFormat= f;
	    break;

	case FIELDformatMERGEFORMAT:
	    break;

	default:
	    SLDEB(utilMemoryBufferGetString(&(ic->icBuffer)),f); return -1;
	}

    return 0;
    }


int docFieldComponentCaseShift(	unsigned char *			pCaseShift,
				const InstructionsComponent *	ic ) 
    {
    int		f;

    f= docFieldFormatInt( ic );
    switch( f )
	{
	case FIELDformatMERGEFORMAT:
	    break;

	case FIELDformatLOWER:
	case FIELDformatUPPER:
	case FIELDformatFIRSTCAP:
	case FIELDformatCAPS:
	    *pCaseShift= f;
	    break;

	default:
	    LDEB(f); return -1;
	}

    return 0;
    }

