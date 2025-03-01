/************************************************************************/
/*									*/
/*  Format the note reference.						*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	"docFieldFormat.h"
#   include	"docDocumentField.h"
#   include	"docChftnField.h"
#   include	<docNotesProperties.h>
#   include	<utilMemoryBuffer.h>
#   include	<textChicago.h>
#   include	<docNoteProperties.h>
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format the value of a 'chftn' field.				*/
/*									*/
/*  I.E. the text of the reference to the note in the body of the text,	*/
/*	or the identical sign at the begnning of the note.		*/
/*									*/
/************************************************************************/

static int docFormatNoteNumber(	MemoryBuffer *			mbResult,
				const NotesProperties *		nsp,
				int				noteNumber )
    {
    int		format= FIELDformatASIS;

    if  ( nsp->npRestart == FTN_RST_CONTINUOUS )
	{ noteNumber += nsp->npStartNumber- 1;	}

    switch( nsp->npNumberStyle )
	{
	case FTNstyleNAR:	format= FIELDformatARABIC; break;
	case FTNstyleNALC:	format= FIELDformatALPHABETIC_LOWER; break;
	case FTNstyleNAUC:	format= FIELDformatALPHABETIC_UPPER; break;
	case FTNstyleNRLC:	format= FIELDformatROMAN_LOWER; break;
	case FTNstyleNRUC:	format= FIELDformatROMAN_UPPER; break;

	case FTNstyleNCHI:
	    return textChicagoString( mbResult, noteNumber+ 1 );

	default:
	    LDEB(nsp->npNumberStyle);
	    return docFieldFormatInteger( mbResult,
					FIELDformatARABIC, noteNumber+ 1 );
	}

    return docFieldFormatInteger( mbResult, format, noteNumber+ 1 );
    }

int docFormatChftnField(	MemoryBuffer *			mbResult,
				const FootEndNotesProperties *	fep,
				const NoteProperties *		np,
				int				noteNumber )
    {
    utilEmptyMemoryBuffer( mbResult );

    if  ( ! np->npAutoNumber )
	{
	if  ( utilCopyMemoryBuffer( mbResult, &(np->npFixedText) ) )
	    { LDEB(1); return -1;	}
	return 0;
	}

    switch( np->npTreeType )
	{
	case DOCinFOOTNOTE:
	    return docFormatNoteNumber( mbResult,
					&(fep->fepFootnotesProps), noteNumber );
	    break;

	case DOCinENDNOTE:
	    return docFormatNoteNumber( mbResult,
					&(fep->fepEndnotesProps), noteNumber );
	    break;

	default:
	    SDEB(docTreeTypeStr(np->npTreeType)); return -1;
	}
    }

