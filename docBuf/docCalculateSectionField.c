/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"

#   include	<docNumericField.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the number of the current section.				*/
/*									*/
/************************************************************************/

int docCalculateSectionFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	dfSection,
				const RecalculateFields *	rf )
    {
    int			calculated= 0;
    struct BufferItem *	bodySectNode= (struct BufferItem *)0;

    const DocumentField *	dfLoc;

    NumericField	nf;

    docInitNumericField( &nf );

    dfLoc= docGetLocationParent( dfSection );
    if  ( ! dfLoc )
	{ XDEB(dfLoc); goto ready;	}

    if  ( docGetNumericField( &nf, DOCfkSECTION, dfSection ) )
	{ LDEB(1); goto ready;	}

    if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0, &bodySectNode,
					(struct BufferDocument *)rf->rfDocument,
					&(dfLoc->dfSelectionScope) )	||
	  ! bodySectNode						)
	{ XDEB(bodySectNode); *pCalculated= 0; return 0;	}

    if  ( docFieldFormatInteger( mbResult, nf.nfNumberFormat,
				bodySectNode->biNumberInParent+ 1 ) )
	{ LDEB(nf.nfNumberFormat); goto ready;	}

    calculated= 1;

  ready:

    docCleanNumericField( &nf );

    *pCalculated= calculated; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the number of pages in the current section.			*/
/*  Strangely enough, the field returns exactly that: So in a document	*/
/*  with continuous page numbers you can get 'page 200 of 4'.		*/
/*									*/
/************************************************************************/

int docCalculateSectionPagesFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		dfSectPages,
				const RecalculateFields *	rf )
    {
    int			calculated= 0;
    struct BufferItem *	bodySectNode= (struct BufferItem *)0;

    const DocumentField *	dfLoc;

    NumericField	nf;

    docInitNumericField( &nf );

    dfLoc= docGetLocationParent( dfSectPages );
    if  ( ! dfLoc )
	{ XDEB(dfLoc); goto ready;	}

    if  ( docGetNumericField( &nf, DOCfkSECTIONPAGES, dfSectPages ) )
	{ LDEB(1); goto ready;	}

    if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0, &bodySectNode,
					(struct BufferDocument *)rf->rfDocument,
					&(dfLoc->dfSelectionScope) )	||
	  ! bodySectNode						)
	{ XDEB(bodySectNode); goto ready;	}

    if  ( docFieldFormatInteger( mbResult, nf.nfNumberFormat,
				bodySectNode->biBelowPosition.lpPage-
				bodySectNode->biTopPosition.lpPage+ 1 ) )
	{ LDEB(nf.nfNumberFormat); goto ready;	}

    calculated= 1;

  ready:

    docCleanNumericField( &nf );

    *pCalculated= calculated; return 0;
    }

