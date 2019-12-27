#   include	"docFieldConfig.h"

#   include	<stdio.h>

#   include	"docFieldInstructions.h"

#   include	<appDebugon.h>

static const char * docInstructionComponentTypeStr(	int	type )
    {
    static char scratch[20];

    switch( type )
	{
	case INSTRtypeFLAG:		return "F";
	case INSTRtypeVALUE:		return "V";
	case INSTRtypeQUOTED_VALUE:	return "QV";

	default:
	    sprintf( scratch, "%d", type );
	    return scratch;
	}
    }

void docListFieldInstructions(		const FieldInstructions *	fi )
    {
    const InstructionsComponent *	ic= fi->fiComponents;
    int					i;

    for ( i= 0; i < fi->fiComponentCount; ic++, i++ )
	{
	appDebug( " %s\"%s\"",
		    docInstructionComponentTypeStr( ic->icType ),
		    utilMemoryBufferGetString( &(ic->icBuffer) ) );
	}

    return;
    }
