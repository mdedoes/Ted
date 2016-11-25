/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docTreeType.h>
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docDocumentNote.h"
#   include	"docNotes.h"
#   include	<docFieldFormat.h>
#   include	<docDocumentField.h>
#   include	<textChicago.h>
#   include	<textAttribute.h>
#   include	<utilPropMask.h>
#   include	<docSectProperties.h>
#   include	"docAttributes.h"
#   include	"docParaBuilderImpl.h"
#   include	"docSelect.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format the value of a 'chftn' field.				*/
/*									*/
/*  I.E. the text of the reference to the note in the body of the text,	*/
/*	or the identical sign at the begnning of the note.		*/
/*									*/
/************************************************************************/

int docFormatChftnField(	MemoryBuffer *			mbResult,
				const FootEndNotesProperties *	fep,
				const NoteProperties *		np,
				int				noteNumber )
    {
    const NotesProperties *	nsp;
    int				format= FIELDformatASIS;

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
	    nsp= &(fep->fepFootnotesProps);
	    break;

	case DOCinENDNOTE:
	    nsp= &(fep->fepEndnotesProps);
	    break;

	default:
	    SDEB(docTreeTypeStr(np->npTreeType)); return -1;
	}

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
	    format= FIELDformatARABIC; break;
	}

    return docFieldFormatInteger( mbResult, format, noteNumber+ 1 );
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docCalculateChftnFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	dfChftn,
				const RecalculateFields *	rf )
    {
    DocumentNote *		dn;

    dn= docGetNoteOfField( dfChftn, rf->rfDocument );
    if  ( dn )
	{
	const struct BufferItem *	bodySectNode;

	bodySectNode= docGetBodySectNode( dn->dnDocumentTree.dtRoot,
							    rf->rfDocument );

	if  ( docFormatChftnField( mbResult,
			    &(bodySectNode->biSectNotesProperties),
			    &(dn->dnNoteProperties),
			    dn->dnNoteNumber ) )
	    { LDEB(dn->dnNoteNumber); *pCalculated= 0; return 0; }

	*pCalculated= 1;
	return 0;
	}

    XDEB(dn);
    *pCalculated= 0; return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docRecalculateParaChftnTextParticules(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				struct DocumentField *		df,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int			partTail= partHead+ partCount;
    BufferItem *	paraNode= pb->pbParaNode;

    DocumentPosition	dp;

    TextAttribute	taSet;
    PropertyMask	taSetMask;
    int			taChanged= 0;

    docInitDocumentPosition( &dp );

    partTail= docRecalculateParaStringTextParticules( pCalculated, pStroffShift,
					    pb, df, rf, partHead, partCount );

    if  ( partTail <= partHead )
	{ LLDEB(partHead,partTail); return -1;	}

    docPlainTextAttribute( &taSet, rf->rfDocument );
    utilPropMaskClear( &taSetMask );

    taSet.taSuperSub= TEXTvaSUPERSCRIPT;
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    if  ( docChangeParticuleAttributes( &taChanged, (PropertyMask *)0,
					rf->rfDocument,
					paraNode, partHead+ 1, partTail,
					&taSet, &taSetMask ) )
	{ LLDEB(partHead,partTail);	}

    if  ( taChanged )
	{ *pCalculated= 1;	}

    return partTail;
    }
