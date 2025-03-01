/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<psPrint.h>
#   include	<docFields.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

int docPsPrintStartNote(
		    const DrawTextLine *	dtl,
		    int				part,
		    int				x0Twips,
		    const DocumentField *	df,
		    int				lineTop )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    struct BufferItem *		paraNode= dtl->dtlParaNode;


    int				cnt;
    int				closed;

    MemoryBuffer		mbSource;
    MemoryBuffer		mbTarget;

    utilInitMemoryBuffer( &mbSource );
    utilInitMemoryBuffer( &mbTarget );

    cnt= docCountParticulesInFieldFwd( paraNode, &closed, part,
				    paraNode->biParaParticuleCount );
    if  ( cnt < 0 )
	{ LDEB(cnt); }

    if  ( docSetNoteLinks( &mbTarget, &mbSource, paraNode, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* Include destination to jump to from the target */
    psDestPdfmark( ps, lineTop, &mbSource );

    /* Remember what the target is to use that at the end of the link */
    utilEmptyMemoryBuffer( &(ps->psAnnotationDictionaryName) );
    utilEmptyMemoryBuffer( &(ps->psLinkFile) );

    utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
    if  ( paraNode->biTreeType == DOCinBODY )
	{
	struct DocumentNote *	dn;

	dn= docGetNoteOfField( df, dtl->dtlDocument );
	if  ( dn )
	    {
	    if  ( docCollectNoteTitle( &(ps->psLinkTitle),
						    dn, dtl->dtlDocument ) )
		{ LDEB(1); rval= -1;	}
	    }
	}

    if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &mbTarget ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ps->psInsideLink= 1;
    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= x0Twips;

  ready:

    utilCleanMemoryBuffer( &mbSource );
    utilCleanMemoryBuffer( &mbTarget );

    return rval;
    }

