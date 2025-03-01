/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedEditOperation.h"
#   include	<docRtfTrace.h>
#   include	<docTreeNode.h>
#   include	<appEditDocument.h>
#   include	<docEditCommand.h>
#   include	<docParaParticules.h>
#   include	<docDocumentCopyJob.h>
#   include	<docDocumentProperties.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<docTreeType.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Common functionality for the various places where a document is	*/
/*  included. The calles start the edit operation, do some preliminary	*/
/*  checking or administration and then include a document.		*/
/*									*/
/************************************************************************/

int tedEditIncludeDocument(	TedEditOperation *		teo,
				struct DocumentCopyJob *	dcj,
				int				command,
				int				posWhere )
    {
    EditOperation *		eo= &(teo->teoEo);

    DocumentSelection		dsTraced;

    if  ( tedEditStartReplace( &dsTraced, teo, command, DOClevSPAN, 0 ) )
	{ LDEB(1); return -1;	}

    /*  7  */
    if  ( docEditIncludeDocument( eo, dcj ) )
	{ LDEB(1); return -1;	}

    switch( posWhere )
	{
	case SELposTAIL:
	    tedEditFinishSelectionTail( teo );
	    break;
	case SELposHEAD:
	    tedEditFinishSelectionHead( teo );
	    break;
	case SELposNEXT:
	    tedEditFinishSelectionHeadNext( teo );
	    break;
	case SELposALL:
	    tedEditFinishRange( teo, -1, -1, &(eo->eoAffectedRange) );
	    break;
	default:
	    LDEB(posWhere);
	    tedEditFinishSelectionTail( teo );
	    break;
	}

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, posWhere ) )
	    { LDEB(1); return -1;	}
	}

    tedFinishEditOperation( teo );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a plain text document into the current document.		*/
/*									*/
/*  This is the implementation of 'Include File', but also of a paste	*/
/*  E.G. from a program that does not support pasting RTF such as	*/
/*  Xterm.								*/
/*									*/
/*  1)  Determine the text attributes of the target location.		*/
/*  5)  Make a mapping of the fonts in the source document to those in	*/
/*	the target document.						*/
/*  6)  Allocate memory to keep an administration of the fields that	*/
/*	have been copied from one document to the other.		*/
/*  7)  Actually include the source in the target.			*/
/*									*/
/************************************************************************/

int tedIncludePlainDocument(	EditDocument *		ed,
				struct BufferDocument *	bdFrom,
				int			traced )
    {
    int				rval= 0;

    DocumentCopyJob		dcj;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				command= EDITcmdREPLACE;

    const int			fullWidth= 1;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( bdFrom->bdProperties->dpContainsTables )
	{
	if  ( sd.sdHeadInTable )
	    { goto ready;	}
	if  ( sd.sdTailInTable )
	    { goto ready;	}
	}

    /*  5,6  */
    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
			&(ed->edFilename), eo->eoSavedTextAttributeNumber ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  7  */
    if  ( bdFrom->bdBody.dtRoot->biChildCount > 0 )
	{ command= EDITcmdREPLACE_BODY_LEVEL;	}

    if  ( tedEditIncludeDocument( &teo, &dcj, command, SELposTAIL ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include an RTF document.						*/
/*									*/
/*  1)  Do not include tables inside tables.				*/
/*  2)  If the paragraphs that we include into are completely covered	*/
/*	by the insertion, replace their properties with those of the	*/
/*	insertion. Ik this way it looks like the paragraphs themselves	*/
/*	have been replaced.						*/
/*									*/
/************************************************************************/

int tedIncludeRtfDocument(	EditDocument *		ed,
				struct BufferDocument *	bdFrom,
				int			traced )
    {
    int				rval= 0;

    DocumentCopyJob		dcj;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				command= EDITcmdREPLACE;
    const int			fullWidth= 1;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( sd.sdInTreeType != DOCinBODY		&&
	  bdFrom->bdNotesList.nlNoteCount > 0	)
	{
	SLDEB(docTreeTypeStr(sd.sdInTreeType),bdFrom->bdNotesList.nlNoteCount);
	goto ready;
	}

    /*  1  */
    if  ( bdFrom->bdProperties->dpContainsTables )
	{
	if  ( sd.sdHeadInTable )
	    { goto ready;	}
	if  ( sd.sdTailInTable )
	    { goto ready;	}
	}

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
						    &(ed->edFilename), -1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bdFrom->bdBody.dtRoot->biChildCount > 0 )
	{ command= EDITcmdREPLACE_BODY_LEVEL;	}

    /*  2  */
    dcj.dcjCopyHeadParaProperties=
			( sg.sgHead.pgPositionFlags & POSflagPARA_HEAD ) != 0;
    dcj.dcjCopyTailParaProperties=
			( sg.sgTail.pgPositionFlags & POSflagPARA_TAIL ) != 0;

    if  ( tedEditIncludeDocument( &teo, &dcj, command, SELposTAIL ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    tedCleanEditOperation( &teo );

    return rval;
    }

