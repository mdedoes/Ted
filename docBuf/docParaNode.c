/************************************************************************/
/*									*/
/*  Book keeping on a paragraph node.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docShape.h"
#   include	"docParaParticules.h"
#   include	<docObjectProperties.h>
#   include	<docDocumentField.h>
#   include	<docObject.h>
#   include	<docTextParticule.h>
#   include	"docTreeNode.h"
#   include	"docObjects.h"
#   include	"docFields.h"
#   include	<bidiTree.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Free a Paragraph level node.					*/
/*									*/
/************************************************************************/

void docCleanParaNode(	struct BufferDocument *	bd,
			struct DocumentTree *	dt,
			BufferItem *		paraNode )
    {
    int				i;
    TextParticule *		tp;

    tp= paraNode->biParaParticules;
    for ( i= 0; i < paraNode->biParaParticuleCount; tp++, i++ )
	{
	switch( tp->tpKind )
	    {
	    case TPkindFIELDHEAD:
	    case TPkindFIELDTAIL:
		{
		DocumentField *	df;

		if  ( tp->tpObjectNumber < 0 )
		    { continue;	}
		df= docGetFieldByNumber( bd, tp->tpObjectNumber );
		if  ( df )
		    {
		    if  ( docDeleteFieldFromParent( dt, df ) )
			{ LDEB(df->dfFieldNumber);	}

		    docDeleteFieldFromDocument( bd, df );
		    }
		else{ /*LXDEB(tp->tpObjectNumber,df);*/ } /*  Is a normal situation as we try for HEAD and TAIL */
		}
		break;

	    case TPkindOBJECT:
		{
		InsertedObject *	io;

		io=  docGetObject( bd, tp->tpObjectNumber );
		if  ( ! io )
		    { LPDEB(tp->tpObjectNumber,io);	}
		else{
		    if  ( io->ioKind == DOCokDRAWING_SHAPE )
			{ docDeleteDrawingShape( bd, io->ioDrawingShape ); }
		    }
		}
		break;
	    }
	}

    utilCleanMemoryBuffer( &(paraNode->biParaStringBuffer) );
    if  ( paraNode->biParaParticules )
	{ free( paraNode->biParaParticules );	}
    if  ( paraNode->biParaLines )
	{ free( paraNode->biParaLines );	}

    if  ( paraNode->biParaBidiRoot )
	{
	bidiCleanTreeNode( paraNode->biParaBidiRoot );
	free( paraNode->biParaBidiRoot );
	}

    return;
    }

void docInitParaNode(	BufferItem *		paraNode )
    {
    utilInitMemoryBuffer( &(paraNode->biParaStringBuffer) );

    paraNode->biParaParticuleCount= 0;
    paraNode->biParaParticules= (TextParticule *)0;

    paraNode->biParaBidiRoot= (struct BidiNode *)0;

    paraNode->biParaLineCount= 0;
    paraNode->biParaLines= (struct TextLine *)0;

    paraNode->biParaMajorityFontSize= 0;
    paraNode->biParaMajorityFontAscY0= 0;
    paraNode->biParaMajorityFontDescY1= 0;

    paraNode->biParaTopInset= 0;
    paraNode->biParaBottomInset= 0;

    paraNode->biParaTocLevel= 0;

    paraNode->biParaProperties= (const struct ParagraphProperties *)0;
    paraNode->BIU.biuPara.btParaPropertyNumber= 0;
    }

int docParagraphIsEmpty(	const BufferItem *	paraNode )
    {
    return paraNode->biParaParticuleCount == 0			||
	  (   paraNode->biParaParticuleCount == 1			&&
	      paraNode->biParaParticules[0].tpKind == TPkindSPAN	&&
	      paraNode->biParaParticules[0].tpStrlen == 0		);
    }
