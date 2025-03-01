/************************************************************************/
/*									*/
/*  Keep track of the stack of trees on the reader.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<textOfficeCharset.h>
#   include	<docShape.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docTreeNode.h>
#   include	<docFieldStack.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reset the attributes in an RtfReader to the defaults.	*/
/*									*/
/************************************************************************/

void docRtfInitTreeStack(	RtfTreeStack *	rts )
    {
    rts->rtsLevel= DOClevOUT;
    rts->rtsTree= (struct DocumentTree *)0;
    rts->rtsNode= (struct BufferItem *)0;
    docInitSelectionScope( &(rts->rtsSelectionScope) );

    docInitRowProperties( &(rts->rtsRowProperties) );
    rts->rtsRowCellX= rts->rtsRowProperties.rpLeftIndentTwips;
    utilInitMemoryBuffer( &(rts->rtsRowSummary) );

    rts->rtsFieldStack= (struct FieldStackLevel *)0;
    rts->rtsFieldPiece= FSpieceFLDRSLT;
    rts->rtsLastFieldNumber= -1;

    rts->rtsParagraphBuilder= (struct ParagraphBuilder *)0;

    rts->rtsPrev= (RtfTreeStack *)0;

    return;
    }

void docRtfPopTreeStack(	RtfReader *	rr )
    {
    RtfTreeStack *	rts= rr->rrTreeStack;

    rr->rrTreeStack= rts->rtsPrev;

    docCleanRowProperties( &(rts->rtsRowProperties) );
    utilCleanMemoryBuffer( &(rts->rtsRowSummary) );
    docCleanFieldStack( rts->rtsFieldStack );

    if  ( rts->rtsParagraphBuilder )
	{ docCloseParagraphBuilder( rts->rtsParagraphBuilder );	}

    return;
    }

int docRtfPushTreeStack(	RtfReader *		rr,
				RtfTreeStack *		rts,
				const SelectionScope *	ss,
				struct DocumentTree *	tree )
    {
    rts->rtsTree= tree;
    rts->rtsNode= tree->dtRoot;
    rts->rtsLevel= tree->dtRoot->biLevel;
    rts->rtsSelectionScope= *ss;
    rts->rtsFieldStack= (struct FieldStackLevel *)0;
    rts->rtsFieldPiece= FSpieceFLDRSLT;
    rts->rtsLastFieldNumber= -1;

    rts->rtsParagraphBuilder= docOpenParagraphBuilder( rr->rrDocument,
								ss, tree );
    if  ( ! rts->rtsParagraphBuilder )
	{ PDEB(rts->rtsParagraphBuilder); return -1;	}

    rts->rtsPrev= rr->rrTreeStack;
    rr->rrTreeStack= rts;

    return 0;
    }
