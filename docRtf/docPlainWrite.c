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

typedef struct PlainWritingContext
    {
    SimpleOutputStream *	pwcSos;
    struct BufferDocument *	pwcDocument;

    int				pwcFold;
    int				pwcHasOpenEnd;
    int				pwcNoteRefCount;
    int				pwcNoteDefCount;
    } PlainWritingContext;

static void docInitPlainWritingContext(	PlainWritingContext *	pwc )
    {
    pwc->pwcSos= (SimpleOutputStream *)0;
    pwc->pwcDocument= (struct BufferDocument *)0;

    pwc->pwcFold= 0;
    pwc->pwcHasOpenEnd= 0;
    pwc->pwcNoteRefCount= 0;
    pwc->pwcNoteDefCount= 0;
    }

typedef struct PlainParagraphWriter
    {
    PlainWritingContext *	ppwPlainWriter;
    int				ppwPos;
    int				ppwLineCount;
    unsigned char		ppwClose;
    } PlainParagraphWriter;

static int docPlainSaveParticule( const VisitParticule *	vp,
				void *				vppw )
    {
    PlainParagraphWriter *	ppw= (PlainParagraphWriter *)vppw;
    PlainWritingContext *	pwc= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pwc->pwcSos;

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
    PlainWritingContext *	pwc= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pwc->pwcSos;

    if  ( df->dfKind == DOCfkPAGEREF )
	{ return SCANadviceSKIP;	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	char	scratch[20+1];

	if  ( vp->vpParaNode->biTreeType == DOCinBODY )
	    {
	    sprintf( scratch, "[%d]", pwc->pwcNoteRefCount+ 1 );
	    pwc->pwcNoteRefCount++;
	    }
	else{
	    sprintf( scratch, "[%d]", pwc->pwcNoteDefCount+ 1 );
	    pwc->pwcNoteDefCount++;
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
    PlainWritingContext *	pwc= ppw->ppwPlainWriter;
    SimpleOutputStream *	sos= pwc->pwcSos;

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

	if  ( pwc->pwcFold			&&
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

static int docPlainSaveParaNode( PlainWritingContext *		pwc,
				struct BufferItem *		paraNode,
				const DocumentSelection *	ds )
    {
    PlainParagraphWriter	ppw;
    const int			scanFlags= 0;

    ppw.ppwPlainWriter= pwc;
    ppw.ppwPos= 0;
    ppw.ppwClose= 1;

    if  ( ds )
	{
	if  ( ds->dsTail.dpNode == paraNode )
	    { ppw.ppwClose= ! pwc->pwcHasOpenEnd;	}
	}
    else{
	if  ( ! docNextParagraph( (struct BufferItem *)paraNode ) )
	    { ppw.ppwClose= ! pwc->pwcHasOpenEnd;	}
	}

    if  ( docScanParagraphLogicalOrder( pwc->pwcDocument, paraNode,
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
	if  ( sioOutPutByte( '\n', pwc->pwcSos ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docPlainWriteEnterNode( struct BufferItem *		node,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				void *				voidpwc )
    {
    PlainWritingContext *	pwc= (PlainWritingContext *)voidpwc;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    return SCANadviceOK;

	case DOClevPARA:
	    if  ( docPlainSaveParaNode( pwc, node, ds ) )
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
    PlainWritingContext		pwc;

    docInitPlainWritingContext( &pwc );

    pwc.pwcFold= fold;
    pwc.pwcHasOpenEnd= bd->bdProperties->dpHasOpenEnd;
    pwc.pwcDocument= bd;
    pwc.pwcSos= sos;

    if  ( ds )
	{ pwc.pwcHasOpenEnd= 1;	}

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds,
				docPlainWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&pwc ) < 0 )
	    { LDEB(1); return -1; }
	}
    else{
	if  ( docScanTree( bd, &(bd->bdBody),
				docPlainWriteEnterNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)&pwc ) < 0 )
	    { LDEB(1); return -1; }
	}

    if  ( ! ds && pwc.pwcNoteRefCount > 0 )
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
		    flags, (void *)&pwc ) < 0 )
		{ LDEB(1); return -1; }
	    }

	if  ( pwc.pwcNoteDefCount != pwc.pwcNoteRefCount )
	    { LLDEB(pwc.pwcNoteDefCount,pwc.pwcNoteRefCount);	}
	}

    return 0;
    }
