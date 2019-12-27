/************************************************************************/
/*									*/
/*  Management of bookmarks related to TOC fields.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<docBookmarkField.h>
#   include	<docFieldInstructions.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	"docEditOperation.h"
#   include	"docEdit.h"
#   include	"docTocBookmarks.h"
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docFields.h>
#   include	<docNodeTree.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recalculate TOC fields in a document.				*/
/*									*/
/************************************************************************/

static long docFindFreeTocBookmarkId(	struct BufferDocument *	bd )
    {
    DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    long		id0= 0;
    long		id1= 0;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( bd, fieldNr );
	long			id;

	if  ( ! df )
	    { continue;	}
	if  ( df->dfKind != DOCfkBOOKMARK	||
	      ! docIsTocBookmark( &id, df )	)
	    { continue;	}

	if  ( id0 <= 0 || id0 > id )
	    { id0= id;	}
	if  ( id1 <= 0 || id1 < id )
	    { id1= id;	}
	}

    if  ( id0 > 1 )
	{ return id0- 1;	}
    else{ return id1+ 1;	}
    }

static int docSetTocBookmark(	FieldInstructions *	fiBookmark,
				struct BufferDocument *	bd )
    {
    int			rval= 0;

    long		id= docFindFreeTocBookmarkId( bd );

    MemoryBuffer	mbBookmark;

    utilInitMemoryBuffer( &mbBookmark );

    if  ( utilMemoryBufferPrintf( &mbBookmark, "_Toc%ld", id ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docSetBookmarkField( fiBookmark, &mbBookmark ) )
	{ LDEB(id); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &mbBookmark );

    return rval;
    }

void docRemoveUnbalancedTocBookmarks(	struct BufferDocument *	bdDoc )
    {
    DocumentFieldList *	dfl= &(bdDoc->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    EditOperation	eo;

    docInitEditOperation( &eo );

    eo.eoTree= &(bdDoc->bdBody);
    eo.eoDocument= bdDoc;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	struct DocumentField *	df= docGetFieldByNumber( bdDoc, fieldNr );

	if  ( ! df )
	    { continue;		}
	if  ( df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	if  ( df->dfKind == DOCfkBOOKMARK				&&
	      df->dfHeadPosition.epParaNr !=
				      df->dfTailPosition.epParaNr	&&
	      docIsTocBookmark( (long *)0, df )				)
	    {
	    DocumentSelection	dsExInside;
	    DocumentSelection	dsInside;
	    DocumentSelection	dsAround;
	    int			headPart;
	    int			tailPart;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					    &headPart, &tailPart, bdDoc, df ) )
		{ LDEB(fieldNr); continue; }

	    if  ( docStartEditOperation( &eo, &dsInside, bdDoc, df ) )
		{ LDEB(fieldNr); 	}

	    if  ( docDeleteField( &dsExInside, &eo, df ) )
		{ LDEB(fieldNr);	}
	    }
	}

    docCleanEditOperation( &eo );
    }

/************************************************************************/
/*									*/
/*  Find a bookmark field that could be used as a TOC bookmark.		*/
/*  If no field can be found.. fill a selection that can be used to set	*/
/*  a TOC bookmark.							*/
/*									*/
/************************************************************************/

static int docFindParaTocBookmark(	DocumentField **	pDfBookmark,
					DocumentSelection *	dsBookmark,
					struct BufferDocument *	bd,
					struct DocumentTree *	dt,
					struct BufferItem *	paraNode )
    {
    DocumentSelection	ds;
    DocumentField *	dfBookmark;

    if  ( docHeadPosition( &(ds.dsHead), paraNode ) )
	{ LDEB(1); return -1;	}
    docAvoidParaHeadField( &(ds.dsHead), (int *)0, bd );

    {
    int				part;
    const TextParticule *	tp;

    docFindParticuleOfPosition( &part, (int *)0, &(ds.dsHead), PARAfindLAST );

    tp= ds.dsHead.dpNode->biParaParticules+ part;
    while( part < paraNode->biParaParticuleCount	&&
	   ( tp->tpKind == TPkindPAGEBREAK	||
	     tp->tpKind == TPkindLINEBREAK	)	)
	{
	ds.dsHead.dpStroff= tp->tpStrlen+ tp->tpStroff;
	tp++; part++;
	}
    }

    if  ( docTailPosition( &(ds.dsTail), paraNode ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( &ds, &(ds.dsHead), &(ds.dsTail), 1 );

    dfBookmark= docFindTypedFieldInSelection( bd, &ds, DOCfkBOOKMARK, 0 );
    if  ( dfBookmark )
	{
	DocumentSelection	dsInsideBookmark;
	DocumentSelection	dsAroundBookmark;

	if  ( docDelimitFieldInDoc( &dsInsideBookmark, &dsAroundBookmark,
				(int *)0, (int *)0, bd, dfBookmark ) )
	    { LDEB(1); return -1;	}

	if  ( dsAroundBookmark.dsHead.dpStroff > ds.dsHead.dpStroff	||
	      dsAroundBookmark.dsTail.dpStroff < ds.dsTail.dpStroff	)
	    {
	    *dsBookmark= ds;
	    *pDfBookmark= (DocumentField *)0;
	    return 1;
	    }
	else{
	    *dsBookmark= dsAroundBookmark;
	    *pDfBookmark= dfBookmark;
	    return 0;
	    }
	}
    else{
	if  ( ds.dsTail.dpStroff < ds.dsHead.dpStroff )
	    {
	    XXDEB(ds.dsTail.dpNode,ds.dsHead.dpNode);
	    LLDEB(ds.dsTail.dpStroff,ds.dsHead.dpStroff);
	    return -1;
	    }

	*dsBookmark= ds;
	*pDfBookmark= (DocumentField *)0;
	return 1;
	}
    }

/************************************************************************/
/*									*/
/*  Set a TOC bookmark that contains the whole paragraph.		*/
/*									*/
/************************************************************************/

static int docSetParaTocBookmark(	struct DocumentField **	pDfBookmark,
					EditOperation *		eo,
					struct BufferDocument *	bd,
					DocumentSelection *	dsBookmark,
					FieldInstructions *	fiBookmark )
    {
    if  ( docSetTocBookmark( fiBookmark, bd ) )
	{ LDEB(1); return -1;	}

    if  ( docStartEditOperation( eo, dsBookmark, bd,
					(struct DocumentField *)0 ) )
	{ LDEB(1); return -1;	}

    if  ( docEditSurroundTextSelectionByField( eo, pDfBookmark,
		    (DocumentSelection *)0, (DocumentSelection *)0,
		    (int *)0, (int *)0,
		    (struct PropertyMask *)0, (struct TextAttribute *)0,
		    DOCfkBOOKMARK, fiBookmark ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

const DocumentField * docGetParaTocBookmark(
					struct BufferDocument *	bd,
					struct DocumentTree *	dt,
					struct BufferItem *	paraNode )
    {
    int				res;
    DocumentSelection		dsBookmark;
    struct DocumentField *	dfBookmark= (struct DocumentField *)0;

    FieldInstructions		fiBookmark;
    EditOperation		eo;

    docInitFieldInstructions( &fiBookmark );
    docInitEditOperation( &eo );

    res= docFindParaTocBookmark( &dfBookmark, &dsBookmark, bd, dt, paraNode );
    if  ( res < 0 )
	{ LDEB(res); goto ready;	}
    if  ( res > 0 )
	{
	if  ( ! docIsIBarSelection( &dsBookmark ) )
	    {
	    if  ( docSetParaTocBookmark( &dfBookmark, &eo, bd,
						&dsBookmark, &fiBookmark ) )
		{ LDEB(1); goto ready;	}
	    }
	}

  ready:

    docCleanEditOperation( &eo );
    docCleanFieldInstructions( &fiBookmark );

    return dfBookmark;
    }

/************************************************************************/
/*									*/
/*  Make sure that all leaves in the outlinelevel tree have a bookmark	*/
/*  that can be used in the table of contents.				*/
/*									*/
/************************************************************************/

typedef struct SetBookmarkThrough
    {
    int				sbtRval;
    struct BufferDocument *	sbtDocument;
    struct DocumentTree *	sbtTree;
    } SetBookmarkThrough;

static int docSetTocBookmarkForLevel(	int			paraNr,
					void *			vsbt )
    {
    struct BufferItem *		paraNode;
    SetBookmarkThrough *	sbt= (SetBookmarkThrough *)vsbt;

    paraNode= docGetParagraphByNumber( sbt->sbtTree, paraNr );
    if  ( ! paraNode )
	{ LXDEB(paraNr,paraNode); sbt->sbtRval= -1;	}
    else{
	/*  Do not check return value:			*/
	/*  No bookmark is set in an empty paragraph	*/

	docGetParaTocBookmark( sbt->sbtDocument, sbt->sbtTree, paraNode );
	}

    return 0;
    }

int docTocSetOutlineBookmarks(	struct BufferDocument *	bd )
    {
    SetBookmarkThrough	sbt;

    sbt.sbtRval= 0;
    sbt.sbtDocument= bd;
    sbt.sbtTree= &(bd->bdBody);

    if  ( docListNumberTreeForAll( &(sbt.sbtTree->dtOutlineTree),
				docSetTocBookmarkForLevel, (void *)&sbt ) )
	{ LDEB(1); return -1;	}

    return sbt.sbtRval;
    }

int docSetTocBookmarks(	struct BufferDocument *		bd )
    {
    int				rval= 0;

    DocumentFieldList *		dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;
    int				fieldNr;

    FieldInstructions		fiBookmark;
    EditOperation		eo;

    docInitFieldInstructions( &fiBookmark );
    docInitEditOperation( &eo );

    if  ( docTocSetOutlineBookmarks( bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		dfTc= docGetFieldByNumber( bd, fieldNr );
	DocumentField *		dfBookmark;
	DocumentSelection	dsInsideTc;
	DocumentSelection	dsAroundTc;

	if  ( ! dfTc )
	    { continue;	}
	if  ( ( dfTc->dfKind != DOCfkTC && dfTc->dfKind != DOCfkTCN )	||
	      dfTc->dfSelectionScope.ssTreeType != DOCinBODY		)
	    { continue;	}

	if  ( docDelimitFieldInDoc( &dsInsideTc, &dsAroundTc,
					    (int *)0, (int *)0, bd, dfTc ) )
	    { LDEB(1); rval= -1; goto ready;	}

	dfBookmark= docFindTypedFieldForPosition( bd, &(dsInsideTc.dsHead),
							    DOCfkBOOKMARK, 0 );
	if  ( ! dfBookmark )
	    {
	    if  ( docSetParaTocBookmark( &dfBookmark, &eo, bd,
						&dsAroundTc, &fiBookmark ) )
		{ LDEB(1); goto ready;	}
	    }
	}

  ready:

    docCleanEditOperation( &eo );
    docCleanFieldInstructions( &fiBookmark );

    return rval;
    }

