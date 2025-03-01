/************************************************************************/
/*									*/
/*  Manage Form Fields.							*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	"docFormField.h"

#   include	<appDebugon.h>

void docInitFormField(	FormField *	ff )
    {
    ff->ffType= 0;
    ff->ffOwnHelp= 0;
    ff->ffOwnStatus= 0;
    ff->ffProtected= 0;
    ff->ffSize= 0;
    ff->ffTextType= 0;
    ff->ffRecalculate= 0;
    ff->ffHasListbox= 0;

    ff->ffMaxlen= 0;
    ff->ffCheckboxSize= 0;
    ff->ffDefaultIndex= 0;

    utilInitMemoryBuffer( &(ff->ffName) );
    utilInitMemoryBuffer( &(ff->ffDefaultText) );
    utilInitMemoryBuffer( &(ff->ffFormat) );
    utilInitMemoryBuffer( &(ff->ffHelpText) );
    utilInitMemoryBuffer( &(ff->ffStatusText) );
    utilInitMemoryBuffer( &(ff->ffEnterMacro) );
    utilInitMemoryBuffer( &(ff->ffExitMacro) );

    return;
    }

void docCleanFormField(	FormField *	ff )
    {
    utilCleanMemoryBuffer( &(ff->ffName) );
    utilCleanMemoryBuffer( &(ff->ffDefaultText) );
    utilCleanMemoryBuffer( &(ff->ffFormat) );
    utilCleanMemoryBuffer( &(ff->ffHelpText) );
    utilCleanMemoryBuffer( &(ff->ffStatusText) );
    utilCleanMemoryBuffer( &(ff->ffEnterMacro) );
    utilCleanMemoryBuffer( &(ff->ffExitMacro) );

    return;
    }

int docSetFormFieldProperty(	FormField *		ff,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case FFpropTYPE:
	    ff->ffType= val;
	    return 0;
	case FFpropOWN_HELP:
	    ff->ffOwnHelp= val != 0;
	    return 0;
	case FFpropOWN_STAT:
	    ff->ffOwnStatus= val != 0;
	    return 0;
	case FFpropPROT:
	    ff->ffProtected= val != 0;
	    return 0;
	case FFpropSIZE:
	    ff->ffSize= val;
	    return 0;
	case FFpropTYPE_TXT:
	    ff->ffTextType= val;
	    return 0;
	case FFpropRECALC:
	    ff->ffRecalculate= val;
	    return 0;
	case FFpropHAS_LISTBOX:
	    ff->ffHasListbox= val;
	    return 0;

	case FFpropMAX_LEN:
	    ff->ffMaxlen= val;
	    return 0;
	case FFpropCHECKBOX_SIZE:
	    ff->ffCheckboxSize= val;
	    return 0;
	case FFpropDEF_INDEX:
	    ff->ffDefaultIndex= val;
	    return 0;

	default:
	    LDEB(prop); return -1;
	}
    }

int docSetFormFieldString(		FormField *		ff,
					int			prop,
					const char *		val,
					int			vallen )
    {
    switch( prop )
	{
	case FFpropNAME:
	    return utilMemoryBufferSetBytes( &(ff->ffName),
					(const unsigned char *)val, vallen );
	case FFpropDEF_TEXT:
	    return utilMemoryBufferSetBytes( &(ff->ffDefaultText),
					(const unsigned char *)val, vallen );
	case FFpropFORMAT:
	    return utilMemoryBufferSetBytes( &(ff->ffFormat),
					(const unsigned char *)val, vallen );
	case FFpropHELP_TEXT:
	    return utilMemoryBufferSetBytes( &(ff->ffHelpText),
					(const unsigned char *)val, vallen );
	case FFpropSTAT_TEXT:
	    return utilMemoryBufferSetBytes( &(ff->ffStatusText),
					(const unsigned char *)val, vallen );
	case FFpropENTRY_MACRO:
	    return utilMemoryBufferSetBytes( &(ff->ffEnterMacro),
					(const unsigned char *)val, vallen );
	case FFpropEXIT_MACRO:
	    return utilMemoryBufferSetBytes( &(ff->ffExitMacro),
					(const unsigned char *)val, vallen );

	default:
	    LDEB(prop); return -1;
	}
    }

