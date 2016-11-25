/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<numbersBase26.h>
#   include	<numbersRoman.h>
#   include	<utilNumberFormat.h>

#   include	"docDocumentField.h"
#   include	"docFieldFormat.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format values following the arguments of the \\* or \\# format	*/
/*  flags in the fieldinstructions.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Format an integer following the arguments of the \\* format flags	*/
/*  in the fieldinstructions.						*/
/*									*/
/************************************************************************/

int docFieldFormatInteger(	MemoryBuffer *			mbResult,
				int				format,
				int				value )
    {
    char			scratch[100+1];

    switch( format )
	{
	case FIELDformatARABIC:
	    sprintf( scratch, "%d", value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatALPHABETIC_UPPER:
	    if  ( numbersBase26String( scratch, sizeof(scratch)-1, value, 1 ) )
		{ sprintf( scratch, "ALPHA:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatALPHABETIC_LOWER:
	    if  ( numbersBase26String( scratch, sizeof(scratch)-1, value, 0 ) )
		{ sprintf( scratch, "alpha:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatROMAN_UPPER:
	    if  ( numbersRomanString( scratch, sizeof(scratch)-1, value, 1 ) )
		{ sprintf( scratch, "ROMAN:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatROMAN_LOWER:
	    if  ( numbersRomanString( scratch, sizeof(scratch)-1, value, 0 ) )
		{ sprintf( scratch, "roman:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatHEX_UPPER:
	    sprintf( scratch, "%X", (unsigned)value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatHEX_LOWER:
	    sprintf( scratch, "%x", (unsigned)value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case FIELDformatCARDTEXT:
	case FIELDformatDOLLARTEXT:
	case FIELDformatORDINAL:
	case FIELDformatORDTEXT:
	default:
	    LDEB(format);
	    sprintf( scratch, "(%d)", value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Format a double following the arguments of the \\# format flags	*/
/*  in the fieldinstructions.						*/
/*									*/
/************************************************************************/

int docFieldFormatNumber(	MemoryBuffer *			mbResult,
				const MemoryBuffer *		picture,
				double				value,
				const struct SimpleLocale *	sl )
    {
    char			scratch[100+1];

    const char *		format= utilMemoryBufferGetString( picture );

    if  ( utilFormatNumber( scratch, sizeof(scratch)-1, format, value, sl ) )
	{ SFDEB(format,value); return -1;	}

    utilMemoryBufferAppendBytes( mbResult,
			(unsigned char *)scratch, strlen( scratch ) );

    return 0;
    }
