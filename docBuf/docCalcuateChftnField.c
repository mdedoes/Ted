/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docDocumentNote.h"
#   include	"docNotes.h"
#   include	<textAttribute.h>
#   include	<utilPropMask.h>
#   include	<docSectProperties.h>
#   include	"docAttributes.h"
#   include	"docParaBuilderImpl.h"
#   include	"docSelect.h"
#   include	<docChftnField.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate Note number fields.					*/
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
