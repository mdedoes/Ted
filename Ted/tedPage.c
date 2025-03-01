/************************************************************************/
/*  Ted: interaction with the page layout tool.				*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	"tedSelect.h"
#   include	<tedDocFront.h>
#   include	"tedLayout.h"
#   include	"docScreenObjects.h"
#   include	"tedDocument.h"
#   include	<docScreenLayout.h>
#   include	<guiDrawingWidget.h>
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<appEditApplication.h>
#   include	<appEditDocument.h>
#   include	<layoutContext.h>
#   include	<docLayout.h>
#   include	<docBuf.h>
#   include	<appPageTool.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reformat and redisplay the document because of a major format	*/
/*  change. etc.							*/
/*									*/
/************************************************************************/

void tedRedoDocumentLayout(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    int				hasSelection= tedHasSelection( ed );
    int				reachedBottom= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    tedLayoutDocumentBody( &reachedBottom, &lc );

    if  ( docOpenTreeObjects( &(bd->bdBody), &lc ) )
	{ LDEB(1); 	}

    if  ( reachedBottom )
	{
	struct BufferItem *	rootNode= bd->bdBody.dtRoot;

	docGetPixelRectangleForPages( &(ed->edFullRect), &lc,
					    rootNode->biTopPosition.lpPage,
					    rootNode->biBelowPosition.lpPage );
	}

    if  ( hasSelection )
	{ tedDelimitCurrentSelection( ed );	}

    appDocSetScrollbarValues( ed );

    if  ( hasSelection )
	{
	DocumentSelection		ds;
	SelectionGeometry		sg;
	SelectionDescription		sd;
	struct BufferItem *		bodySectNode;

	tedDescribeSelection( ed );

	if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0, &bodySectNode, ed ) )
	    { LDEB(1); return;	}

	tedDocAdaptTopRuler( ed, &ds, &sg, &sd, bodySectNode );

	tedScrollToSelection( ed, (int *)0, (int *)0 );

	if  ( td->tdSelectionDescription.sdIsObjectSelection )
	    { tedMoveObjectWindows( ed );	}
	}

    guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the page tool to a document.					*/
/*									*/
/************************************************************************/

void tedAdaptPageToolToDocument(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= bd->bdProperties;

    if  ( ! ea->eaPageTool )
	{ return;	}

    appPageToolSetProperties( ea->eaPageTool, &(dp->dpGeometry) );

    appEnablePageTool( ea->eaPageTool, ! ed->edIsReadonly );

    return;
    }

/************************************************************************/
/*									*/
/*  (re)Calculate the layout of a whole document.			*/
/*									*/
/************************************************************************/

int tedLayoutDocumentBody(	int *				pReachedBottom,
				const LayoutContext *		lc )
    {
    return docScreenLayoutDocumentBody( pReachedBottom, lc->lcDocument, lc );
    }

