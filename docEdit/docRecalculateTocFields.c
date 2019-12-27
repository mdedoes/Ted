/************************************************************************/
/*									*/
/*  Recalculate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	"docTocBookmarks.h"
#   include	"docRecalculateTocField.h"
#   include	<docTreeType.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFields.h>
#   include	<docRecalculateFields.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recalculate TOC fields.						*/
/*									*/
/*  2)  Make sure that all TOC candidates have valid bookmarks.		*/
/*  3)  Calculate the table(s) of contents.				*/
/*									*/
/************************************************************************/

int docRecalculateTocFields( RecalculateFields *		rf )
    {
    struct BufferDocument *	bdDoc= rf->rfDocument;
    DocumentFieldList *		dfl= &(bdDoc->bdFieldList);
    int				fieldNr;

    /*  2  */
    docRemoveUnbalancedTocBookmarks( bdDoc );

    if  ( docSetTocBookmarks( bdDoc ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    for ( fieldNr= 0; fieldNr < dfl->dflPagedList.plItemCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( bdDoc, fieldNr );

	if  ( ! df						||
	      df->dfKind != DOCfkTOC				||
	      df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	if  ( docRecalculateOneTocField( bdDoc, df ) )
	    { LDEB(fieldNr); return -1;	}

	rf->rfFieldsUpdated++;
	}

    return 0;
    }

