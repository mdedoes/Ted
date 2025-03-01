/************************************************************************/
/*									*/
/*  Save a BufferDocument into a plain text file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTreeType.h>
#   include	<docTreeScanner.h>
#   include	<docScanner.h>
#   include	"docPlainReadWrite.h"
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<sioGeneral.h>
#   include	<docSelect.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<docTextRun.h>
#   include	<docParaScanner.h>

#   include	<appDebugon.h>

typedef struct PlainDocumentWriter
    {
    SimpleOutputStream *	pdwSos;
    struct BufferDocument *	pdwDocument;

    int				pdwFold;
    int				pdwHasOpenEnd;
    int				pdwNoteRefCount;
    int				pdwNoteDefCount;
    } PlainDocumentWriter;

static void docInitPlainWritingContext(	PlainDocumentWriter *	pdw )
    {
    pdw->pdwSos= (SimpleOutputStream *)0;
    pdw->pdwDocument= (struct BufferDocument *)0;

    pdw->pdwFold= 0;
    pdw->pdwHasOpenEnd= 0;
    pdw->pdwNoteRefCount= 0;
    pdw->pdwNoteDefCount= 0;
    }

typedef struct PlainParagraphWriter
    {
    PlainDocumentWriter *	ppwPlainWriter;
    int				ppwPos;
    int				ppwLineCount;
    unsigned char		ppwClose;
    } PlainParagraphWriter;

static int docPlainSaveParticule( const VisitParticule *	vp,
				void *				vppw )
    {
    PlainParagraphWriter *	ppw= (PlainParagraphWriter *)vppw;
    PlainDocumentWriter *	pdw= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pdw->pdwSos;

    switch( vp->vpTextParticule->tpKind )
	{
	case TPkindTAB:
	    if  ( sioOutPutByte( '\t', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    ppw->ppwPos= 8* ( ppw->ppwPos+ 8 )/ 8;
	    break;

	case TPkindLINEBREAK:
	    if  ( sioOutPutByte( '\n', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    ppw->ppwPos= 0;
	    break;

	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    if  ( sioOutPutByte( '\n', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    if  ( sioOutPutByte( '\f', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    ppw->ppwPos= 0;
	    break;

	case TPkindFIELDHEAD:
	    LDEB(vp->vpTextParticule->tpKind); return SCANadviceOK;

	default:
	    LDEB(vp->vpTextParticule->tpKind);
	    /*FALLTHROUGH*/
	case TPkindOBJECT:
	case TPkindFIELDTAIL:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    break;
	}

    return SCANadviceOK;
    }

static int docPlainSaveField(	const VisitParticule *	vp,
				DocumentField *		df,
				void *			vppw )
    {
    PlainParagraphWriter *	ppw= (PlainParagraphWriter *)vppw;
    PlainDocumentWriter *	pdw= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pdw->pdwSos;

    if  ( df->dfKind == DOCfkPAGEREF )
	{ return SCANadviceSKIP;	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	char	scratch[20+1];

	if  ( vp->vpParaNode->biTreeType == DOCinBODY )
	    {
	    sprintf( scratch, "[%d]", pdw->pdwNoteRefCount+ 1 );
	    pdw->pdwNoteRefCount++;
	    }
	else{
	    sprintf( scratch, "[%d]", pdw->pdwNoteDefCount+ 1 );
	    pdw->pdwNoteDefCount++;
	    }

	sioOutPutString( scratch, sos );
	ppw->ppwPos += strlen( scratch );

	return SCANadviceSKIP;
	}

    return SCANadviceOK;
    }

static int docPlainSaveRun(	const TextRun *		tr,
				void *			vppw )
    {
    PlainParagraphWriter *	ppw= (PlainParagraphWriter *)vppw;
    PlainDocumentWriter *	pdw= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pdw->pdwSos;

    const BufferItem *		paraNode= tr->trParaNode;
    const char *		s= docParaString( paraNode, tr->trStroff );
    int				from= 0;
    int				past= from;

    while( from < tr->trStrlen )
	{
	while( past < tr->trStrlen && s[past] != ' ' )
	    {  past++;	}
	while( past < tr->trStrlen && s[past] == ' ' )
	    {  past++;	}

	if  ( pdw->pdwFold			&&
	      ppw->ppwPos > 8			&&
	      ppw->ppwPos+ past- from >= 72	)
	    {
	    if  ( sioOutPutByte( '\n', sos ) < 0 )
		{ LDEB(1); return -1;	}
	    ppw->ppwPos= 0;

	    if  ( paraNode->biParaProperties->ppFirstIndentTwips < -9 &&
		  paraNode->biParaProperties->ppLeftIndentTwips  >  9 )
		{
		if  ( sioOutPutByte( '\t', sos ) < 0 )
		    { LDEB(1); return -1;	}
		ppw->ppwPos= 8;
		}
	    }

	while( from < past )
	    {
	    if  ( sioOutPutByte( s[from], sos ) < 0 )
		{ LDEB(1); return -1;	}

	    from++; ppw->ppwPos++;
	    }
	}

    return SCANadviceOK;
    }

static int docPlainSaveParaNode( PlainDocumentWriter *		pdw,
				struct BufferItem *		paraNode,
				const DocumentSelection *	ds )
    {
    PlainParagraphWriter	ppw;
    const int			scanFlags= 0;

    ppw.ppwPlainWriter= pdw;
    ppw.ppwPos= 0;
    ppw.ppwClose= 1;

    if  ( ds )
	{
	if  ( ds->dsTail.dpNode == paraNode )
	    { ppw.ppwClose= ! pdw->pdwHasOpenEnd;	}
	}
    else{
	if  ( ! docNextParagraph( (struct BufferItem *)paraNode ) )
	    { ppw.ppwClose= ! pdw->pdwHasOpenEnd;	}
	}

    if  ( docScanParagraphLogicalOrder( pdw->pdwDocument, paraNode,
				ds, scanFlags,
				docPlainSaveParticule,
				docPlainSaveField,
				docPlainSaveRun,
				(ObjectVisitor)0,
				(TabVisitor)0,
				&ppw ) < 0 )
	{ LDEB(1); return -1;	}

    if  ( ppw.ppwClose )
	{
	if  ( sioOutPutByte( '\n', pdw->pdwSos ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docPlainWriteEnterNode( struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voidpwc )
    {
    PlainDocumentWriter *	pdw= (PlainDocumentWriter *)voidpwc;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    return SCANadviceOK;

	case DOClevPARA:
	    if  ( docPlainSaveParaNode( pdw, node, ds ) )
		{ LDEB(1); return -1;	}
	    return SCANadviceOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }


int docPlainSaveDocument(	SimpleOutputStream *		sos,
				struct BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				fold )
    {
    const int			flags= 0;
    PlainDocumentWriter		pdw;

    docInitPlainWritingContext( &pdw );

    pdw.pdwFold= fold;
    pdw.pdwHasOpenEnd= bd->bdProperties->dpHasOpenEnd;
    pdw.pdwDocument= bd;
    pdw.pdwSos= sos;

    if  ( ds )
	{ pdw.pdwHasOpenEnd= 1;	}

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
				docPlainWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&pdw ) < 0 )
	    { LDEB(1); return -1; }
	}
    else{
	if  ( docScanTree( bd, &(bd->bdBody),
				docPlainWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&pdw ) < 0 )
	    { LDEB(1); return -1; }
	}

    if  ( ! ds && pdw.pdwNoteRefCount > 0 )
	{
	DocumentField *	dfNote;
	DocumentNote *	dn;

	sioOutPutString( "\n-----\n\n", sos );

	for ( dfNote= docGetFirstNoteOfDocument( &dn, bd, -1 );
	      dfNote;
	      dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 ) )
	    {
	    if  ( docScanTree( bd, &(dn->dnDocumentTree),
		    docPlainWriteEnterNode, (NodeVisitor)0,
		    (TreeVisitor)0, (TreeVisitor)0, 
		    flags, (void *)&pdw ) < 0 )
		{ LDEB(1); return -1; }
	    }

	if  ( pdw.pdwNoteDefCount != pdw.pdwNoteRefCount )
	    { LLDEB(pdw.pdwNoteDefCount,pdw.pdwNoteRefCount);	}
	}

    return 0;
    }
