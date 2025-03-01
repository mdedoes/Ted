/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<docPagerefField.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docRecalculateFields.h"
#   include	<docTreeType.h>
#   include	<docDocumentField.h>
#   include	<docFieldFormat.h>
#   include	<docNumericField.h>
#   include	<docFieldKind.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextLine.h>
#   include	<docSectProperties.h>
#   include	"docFields.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate page number related fields.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Format a page number to be used in a certain paragraph.		*/
/*									*/
/************************************************************************/

static int docGetPageNumberOffset(	const BufferItem *	sectNode )
    {
    if  ( ! sectNode )
	{ XDEB(sectNode); return 0;	}
    if  ( ! sectNode->biParent )
	{ XDEB(sectNode->biParent); return 0;	}

    while( sectNode->biNumberInParent > 0 )
	{
	if  ( sectNode->biSectRestartPageNumbers )
	    { break;	}

	sectNode= sectNode->biParent->biChildren[sectNode->biNumberInParent- 1];
	}

    if  ( sectNode->biSectRestartPageNumbers )
	{
	return sectNode->biTopPosition.lpPage- sectNode->biSectStartPageNumber;
	}
    else{ return sectNode->biTopPosition.lpPage; }
    }

static int docFormatPageNumber(	MemoryBuffer *		mbResult,
				const BufferItem *	bodySectNode,
				int			overrideFormat,
				int			pageNumber )
    {
    int			format= FIELDformatARABIC;

    pageNumber -= docGetPageNumberOffset( bodySectNode );

    if  ( overrideFormat == FIELDformatASIS )
	{
	if  ( ! bodySectNode )
	    { XDEB(bodySectNode); return -1;	}

	switch( bodySectNode->biSectPageNumberStyle )
	    {
	    case DOCpgnDEC:	format= FIELDformatARABIC; break;
	    case DOCpgnUCLTR:	format= FIELDformatALPHABETIC_UPPER; break;
	    case DOCpgnLCLTR:	format= FIELDformatALPHABETIC_LOWER; break;
	    case DOCpgnUCRM:	format= FIELDformatROMAN_UPPER; break;
	    case DOCpgnLCRM:	format= FIELDformatROMAN_LOWER; break;

	    default:
		LDEB(bodySectNode->biSectPageNumberStyle);
		format= FIELDformatARABIC;
		break;
	    }
	}
    else{ format= overrideFormat;	}

    return docFieldFormatInteger( mbResult, format, pageNumber );
    }

/************************************************************************/
/*									*/
/*  Return the value of a pageref field.				*/
/*									*/
/************************************************************************/

int docCalculatePagerefFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		dfRef,
				const RecalculateFields *	rf )
    {
    struct BufferDocument *		bd= rf->rfDocument;
    struct BufferItem *		bodySectNode= (struct BufferItem *)0;
    DocumentField *		dfMark;
    int				pageNumber;

    PagerefField		pf;

    int				n;

    docInitPagerefField( &pf );

    if  ( docGetPagerefField( &pf, dfRef ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    n= docFindBookmarkField( &dfMark, bd, &(pf.pfBookmark) );
    if  ( n < 0 )
	{
	/* SLDEB(utilMemoryBufferGetString(&(pf.pfBookmark)),n); */
	*pCalculated= 0; goto ready;
	}

    if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0,
			&bodySectNode, bd, &(dfMark->dfSelectionScope) ) ||
	  ! bodySectNode						)
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    pageNumber= dfMark->dfPage;

    docFormatPageNumber( mbResult, bodySectNode,
					pf.pfNumberFormat, pageNumber+ 1 );

    *pCalculated= 1;

  ready:

    docCleanPagerefField( &pf );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a page/numpages field.				*/
/*									*/
/************************************************************************/

int docCalculatePageFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		dfPage,
				const RecalculateFields *	rf )
    {
    int			calculated= 0;
    struct BufferItem *	bodySectNode= (struct BufferItem *)rf->rfBodySectNode;
    int			pageNumber= rf->rfBodySectPage;

    const DocumentField *	dfLoc;

    NumericField	nf;

    docInitNumericField( &nf );
    nf.nfNumberFormat= FIELDformatASIS;

    dfLoc= docGetLocationParent( dfPage );
    if  ( ! dfLoc )
	{ XDEB(dfLoc); goto ready;	}

    if  ( docGetNumericField( &nf, DOCfkPAGE, dfPage ) )
	{ LDEB(1); goto ready;	}

    if  ( ! docIsHeaderType( dfLoc->dfSelectionScope.ssTreeType )	&&
	  ! docIsFooterType( dfLoc->dfSelectionScope.ssTreeType )	)
	{
	DocumentPosition		dp;

	if  ( docPositionForEditPosition( &dp, &(dfLoc->dfHeadPosition),
								rf->rfTree ) )
	    { LDEB(dfLoc->dfHeadPosition.epParaNr);	}
	else{
	    int			line;
	    int			flags= 0;

	    if  ( docFindLineOfPosition( &line, &flags, &dp, PARAfindFIRST ) )
		{ LDEB(dp.dpStroff);	}
	    else{
		pageNumber= dp.dpNode->biParaLines[line].tlTopPosition.lpPage;
		}
	    }

	if  ( docGetRootOfSelectionScope( (struct DocumentTree **)0,
				&bodySectNode, rf->rfDocument,
				&(dfLoc->dfSelectionScope) )		||
	      ! bodySectNode						)
	    { XDEB(bodySectNode); goto ready;	}
	}

    docFormatPageNumber( mbResult, bodySectNode,
					nf.nfNumberFormat, pageNumber+ 1 );

    calculated= 1;

  ready:

    docCleanNumericField( &nf );

    *pCalculated= calculated; return 0;
    }

int docCalculateNumpagesFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const DocumentField *		df,
				const RecalculateFields *	rf )
    {
    int			calculated= 0;
    const struct BufferItem *	bodyNode= rf->rfDocument->bdBody.dtRoot;

    NumericField	nf;

    docInitNumericField( &nf );

    if  ( docGetNumericField( &nf, DOCfkNUMPAGES, df ) )
	{ LDEB(1); goto ready;	}

    if  ( docFieldFormatInteger( mbResult, nf.nfNumberFormat,
				bodyNode->biBelowPosition.lpPage+ 1 ) )
	{ LDEB(nf.nfNumberFormat); goto ready;	}

    calculated= 1;

  ready:

    docCleanNumericField( &nf );

    *pCalculated= calculated; return 0;
    }

