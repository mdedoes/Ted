#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docNotes.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	"docDocumentNote.h"
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	"docSelect.h"
#   include	"docSectHeadersFooters.h"
#   include	"docFields.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

void docListFieldParticule(	int			indent,
				const char *		label,
				int			n,
				const struct BufferItem *	node,
				const TextParticule *	tp,
				const DocumentField *	df )
    {
    appDebug( "%*s%s %3d: [S %4d..%4d] %s",
		    indent, "", label, n,
		    tp->tpStroff, tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ) );

    if  ( tp->tpStrlen > 0 )
	{
	appDebug( " \"%.*s\"",
		    (int)tp->tpStrlen,
		    docParaString( node, tp->tpStroff ) );
	}

    if  ( tp->tpKind == TPkindOBJECT		||
	  tp->tpKind == TPkindFIELDHEAD	||
	  tp->tpKind == TPkindFIELDTAIL	||
	  tp->tpObjectNumber >= 0		)
	{ appDebug( " OBNR=%d", tp->tpObjectNumber );	}

    if  ( df )
	{ appDebug( " FIELD:%s", docFieldKindStr( df->dfKind ) );	}

    appDebug( "\n" );

    return;
    }

static int docCheckFieldHeadParticule(	int			part,
					int			paraNr,
					const struct BufferItem *	paraNode,
					const DocumentField *	df )
    {
    int				wrong= 0;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    if  ( tp->tpKind != TPkindFIELDHEAD )
	{ SLLDEB("####",tp->tpKind,TPkindFIELDHEAD); wrong= 1;	}

    if  ( df->dfFieldNumber != tp->tpObjectNumber )
	{
	LLDEB(df->dfFieldNumber,tp->tpObjectNumber);
	docListFieldParticule( 4, "#FN#", part, paraNode, tp, df );
	wrong= 1;
	}

    if  ( df->dfHeadPosition.epParaNr != paraNr )
	{
	LLDEB(df->dfHeadPosition.epParaNr,paraNr);
	docListFieldParticule( 4, "#PA#", part, paraNode, tp, df );
	wrong= 1;
	}
    if  ( df->dfHeadPosition.epStroff != tp->tpStroff )
	{
	LLDEB(df->dfHeadPosition.epStroff,tp->tpStroff);
	docListFieldParticule( 4, "#ST#", part, paraNode, tp, df );
	wrong= 1;
	}

    return wrong;
    }

static int docCheckFieldTailParticule(	int			part,
					int			paraNr,
					const struct BufferItem *	paraNode,
					const DocumentField *	df )
    {
    int				wrong= 0;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    if  ( tp->tpKind != TPkindFIELDTAIL )
	{ SLLDEB("####",tp->tpKind,TPkindFIELDTAIL); wrong= 1;	}

    if  ( df->dfFieldNumber != tp->tpObjectNumber )
	{
	LLDEB(df->dfFieldNumber,tp->tpObjectNumber);
	docListFieldParticule( 4, "#FN#", part, paraNode, tp, df );
	wrong= 1;
	}

    if  ( df->dfTailPosition.epParaNr != paraNr )
	{
	LLDEB(df->dfTailPosition.epParaNr,paraNr);
	docListFieldParticule( 4, "#PA#", part, paraNode, tp, df );
	wrong= 1;
	}
    if  ( df->dfTailPosition.epStroff != tp->tpStroff )
	{
	LLDEB(df->dfTailPosition.epStroff,tp->tpStroff);
	docListFieldParticule( 4, "#ST#", part, paraNode, tp, df );
	wrong= 1;
	}

    return wrong;
    }

void docCheckFieldOffsets(	const struct BufferDocument *	bd,
				const struct BufferItem *	node )
    {
    switch( node->biLevel )
	{
	int			i;
	int			part;
	const TextParticule *	tp;
	int			paraNr;
	int			wrong;

	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < node->biChildCount; i++ )
		{ docCheckFieldOffsets( bd, node->biChildren[i] );	}
	    break;

	case DOClevPARA:
	    paraNr= docNumberOfParagraph( node );
	    tp=  node->biParaParticules;
	    wrong= 0;
	    for ( part= 0; part < node->biParaParticuleCount; tp++, part++ )
		{
		const DocumentField *	df;

		if  ( tp->tpKind != TPkindFIELDTAIL	&&
		      tp->tpKind != TPkindFIELDHEAD	)
		    { continue;	}

		df= docGetFieldByNumber( bd, tp->tpObjectNumber );
		if  ( ! df )
		    { SLXDEB("####",tp->tpObjectNumber,df); continue;	}

		if  ( tp->tpKind == TPkindFIELDHEAD )
		    {
		    if  ( docCheckFieldHeadParticule( part, paraNr, node, df ) )
			{ wrong= 1;	}
		    }
		if  ( tp->tpKind == TPkindFIELDTAIL )
		    {
		    if  ( docCheckFieldTailParticule( part, paraNr, node, df ) )
			{ wrong= 1;	}
		    }
		}
	    if  ( wrong )
		{ docListNode(bd,0,node,0);	}
	    break;

	case DOClevOUT:
	default:
	    break;
	}

    return;
    }

typedef struct FieldPosition
    {
    EditPosition	fpPosition;
    int			fpPage;
    int			fpColumn;
    } FieldPosition;

static void docCheckNoteOfField(	int			indent,
					FieldPosition *		fp,
					const DocumentField *	df,
					const struct BufferDocument *	bd )
    {
    const DocumentNote *	dn= docGetNoteOfField( df, bd );

    if  ( ! dn )
	{ SXDEB("##",dn); return;	}

    if  ( dn->dnReferringPage >= 0 || fp->fpPage >= 0 )
	{
	if  ( dn->dnReferringPage < fp->fpPage			||
	      ( dn->dnReferringPage == fp->fpPage	&&
		dn->dnReferringColumn < fp->fpColumn	)	)
	    {
	    appDebug( "%*s## NOTE %d:%d PG/COL BEFORE PREVIOUS %d:%d\n",
		    indent+ 15, "",
		    dn->dnReferringPage, dn->dnReferringColumn,
		    fp->fpPage, fp->fpColumn );
	    }
	}

    if  ( ! dn->dnDocumentTree.dtRoot )
	{ SXDEB("##",dn->dnDocumentTree.dtRoot);	}
    else{
	/*
	const int	noteIndent= 4;
	*/

	const SelectionScope *	ssRoot= &(dn->dnDocumentTree.dtRoot->biSectSelectionScope);

	if  ( ssRoot->ssSectNr != 0 )
	    { SLDEB("##",ssRoot->ssSectNr); }
	if  ( ssRoot->ssOwnerNumber != df->dfFieldNumber )
	    { SLLDEB("##",ssRoot->ssOwnerNumber,df->dfFieldNumber); }

	/*
	docListNode(bd,noteIndent,dn->dnDocumentTree.dtRoot,0);
	*/

	fp->fpPage= dn->dnReferringPage;
	fp->fpColumn= dn->dnReferringColumn;
	}

    return;
    }

static void docListFieldHead(	int			indent,
				const DocumentField *	df,
				const char *		headString,
				const char *		tailString )
    {
    int				fix= 12- indent;

    if  ( fix < 0 )
	{ fix= 0;	}

    appDebug( "%*s %s%*s%4d:%-4d .. %4d:%-4d %s #%d:%s",
		indent, "",
		headString,
		fix, "",
		df->dfHeadPosition.epParaNr,
		df->dfHeadPosition.epStroff,
		df->dfTailPosition.epParaNr,
		df->dfTailPosition.epStroff,
		tailString,
		df->dfFieldNumber, docFieldKindStr( df->dfKind ) );

    docListFieldInstructions( &(df->dfInstructions ) );
    appDebug( "\n" );

    return;
    }

static void docListFieldTail(	int			indent,
				const DocumentField *	df,
				const char *		tailString )
    {
    int			fix= 25- indent;

    if  ( fix < 0 )
	{ fix= 0;	}

    appDebug( "%*s %s%*s%4d:%-4d   #%d:%s\n",
		indent, "",
		tailString,
		fix, "",
		df->dfTailPosition.epParaNr,
		df->dfTailPosition.epStroff,
		df->dfFieldNumber, docFieldKindStr( df->dfKind ) );
    }

static void docCheckInstructionsFieldPosition(
				int			indent,
				EditPosition *		ep,
				const DocumentField *	df )
    {
    const char *		tailString= "]";

    if  ( df->dfResultFields.cfChildCount > 0		||
	  df->dfInstructionFields.cfChildCount > 0	)
	{ tailString= " "; }

    docListFieldHead( indent, df, "[", tailString );

    if  ( docCompareEditPositions( ep, &(df->dfHeadPosition) ) > 0 )
	{ appDebug( "%*s## START BEFORE PREV\n", indent+ 15, "" ); }

    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					&(df->dfTailPosition) ) > 0 )
	{ appDebug( "%*s## START AFTER END\n", indent+ 15, "" ); }

    *ep= df->dfTailPosition;

    return;
    }

static void docCheckResultFieldPosition(
				int			indent,
				FieldPosition *		fp,
				const DocumentField *	df,
				const struct BufferDocument *	bd )
    {
    const NotesList *		nl= &(bd->bdNotesList);
    const char *		tailString= "}";

    if  ( df->dfResultFields.cfChildCount > 0		||
	  df->dfInstructionFields.cfChildCount > 0	)
	{ tailString= " "; }

    docListFieldHead( indent, df, "{", tailString );

    if  ( docCompareEditPositions( &(fp->fpPosition),
					    &(df->dfHeadPosition) ) > 0 )
	{ appDebug( "%*s## START BEFORE PREV or PARENT\n", indent+ 15, "" ); }

    if  ( docCompareEditPositions( &(df->dfHeadPosition),
					&(df->dfTailPosition) ) > 0 )
	{ appDebug( "%*s## START AFTER END\n", indent+ 15, "" ); }

    if  ( df->dfSelectionScope.ssTreeType == DOCinBODY	&&
	  docFieldHasNote( df->dfKind )			&&
	  nl						)
	{
	if  ( df->dfNoteIndex < 0 || df->dfNoteIndex >= nl->nlNoteCount )
	    {
	    appDebug( "%*s## Note %d/%d\n", indent+ 15, "",
				df->dfNoteIndex, nl->nlNoteCount );
	    }
	else{ docCheckNoteOfField( indent, fp, df, bd );	}
	}

    {
    struct DocumentTree  *	dt;
    struct BufferItem *	bodySectNode= (struct BufferItem *)0;

    if  ( docGetRootOfSelectionScope( &dt, &bodySectNode,
			(struct BufferDocument *)bd, &(df->dfSelectionScope) )	||
	  ! bodySectNode						)
	{
	SXDEB("##",bodySectNode);
	SSDEB("##",docTreeTypeStr(df->dfSelectionScope.ssTreeType));
	SLDEB("##",df->dfSelectionScope.ssSectNr);
	SLDEB("##",df->dfSelectionScope.ssOwnerSectNr);
	SLDEB("##",df->dfSelectionScope.ssOwnerNumber);
	}
    }

    return;
    }

static void docCheckFieldHierarchy(	int			indent,
					int			pos,
					const DocumentField *	dfPa,
					const ChildFields *	cf,
					const DocumentField *	dfCh )
    {
    if  ( dfCh->dfParent != dfPa )
	{ appDebug( "%*s## WRONG PARENT\n", indent+ 15, "" ); }
    if  ( dfCh->dfNumberInParent != pos )
	{
	appDebug( "%*s## WRONG INDEX %d@%d\n", indent+ 15, "",
					    dfCh->dfNumberInParent, pos );
	}

    if  ( pos > 0 && dfCh == cf->cfChildren[pos-1] )
	{ appDebug( "%*s## FIELD APPEARS TWICE\n", indent+ 15, "" ); }

    return;
    }

static void docCheckInstructionFieldTree( int			indent,
					const DocumentField *	dfPa )
    {
    int			i;
    const ChildFields *	cfI= &(dfPa->dfInstructionFields);

    EditPosition	ep;

    docInitEditPosition( &ep );
    if  ( cfI->cfChildCount > 0 )
	{ ep= cfI->cfChildren[0]->dfHeadPosition;	}

    for ( i= 0; i < cfI->cfChildCount; i++ )
	{
	const DocumentField *	dfCh= cfI->cfChildren[i];

	docCheckInstructionsFieldPosition( indent, &ep, dfCh );

	docCheckFieldHierarchy( indent, i, dfPa, cfI, dfCh );

	if  ( dfCh->dfSelectionScope.ssTreeType != DOCinFIELD_INSTRUCTIONS )
	    { SLDEB("###",dfCh->dfSelectionScope.ssTreeType);	}

	if  ( dfCh->dfResultFields.cfChildCount > 0 )
	    {
	    const ChildFields *	cfR= &(dfCh->dfResultFields);
	    int			j;

	    appDebug( "%*s## INSTRUCTION FIELD HAS RESULTS\n", indent+ 15, "" );

	    for ( j= 0; j < cfR->cfChildCount; j++ )
		{
		docListFieldHead( indent+ 1, cfR->cfChildren[j], "<", ">" );
		}
	    }

	if  ( dfCh->dfInstructionFields.cfChildCount > 0 )
	    { docCheckInstructionFieldTree( indent+ 1, dfCh );	}

	if  ( dfCh->dfResultFields.cfChildCount > 0		||
	      dfCh->dfInstructionFields.cfChildCount > 0	)
	    { docListFieldTail( indent, dfCh, "]" );	}
	}

    return;
    }

static void docCheckResultFieldTree(	int			indent,
					const struct BufferDocument *	bd,
					const struct DocumentTree *	dt,
					FieldPosition *		fp,
					const DocumentField *	dfPa,
					const ChildFields *	cf )
    {
    int			i;

    for ( i= 0; i < cf->cfChildCount; i++ )
	{
	const DocumentField *	dfCh= cf->cfChildren[i];

	docCheckResultFieldPosition( indent, fp, dfCh, bd );

	docCheckFieldHierarchy( indent, i, dfPa, cf, dfCh );

	fp->fpPosition= dfCh->dfHeadPosition;

	if  ( dfCh->dfResultFields.cfChildCount > 0 )
	    {
	    docCheckResultFieldTree( indent+ 1, bd, dt,
					fp, dfCh, &(dfCh->dfResultFields) );

	    if  ( docCompareEditPositions( &(fp->fpPosition),
					    &(dfCh->dfTailPosition ) ) > 0 )
		{ appDebug( "%*s## CHILD PAST END\n", indent+ 15, "" ); }
	    }

	if  ( dfCh->dfInstructionFields.cfChildCount > 0 )
	    {
	    docCheckInstructionFieldTree( indent+ 1, dfCh );
	    }

	if  ( dfCh->dfResultFields.cfChildCount > 0		||
	      dfCh->dfInstructionFields.cfChildCount > 0	)
	    { docListFieldTail( indent, dfCh, "}" );	}

	if  ( dt )
	    {
	    DocumentSelection	dsInside;
	    DocumentSelection	dsAround;
	    int			part0;
	    int			part1;

	    if  ( docDelimitFieldInTree( &dsInside, &dsAround,
					    &part0, &part1, dt, dfCh ) )
		{
		appDebug( "### '%s' Field not found\n",
					docFieldKindStr( dfCh->dfKind ) );
		LLDEB(dfCh->dfHeadPosition.epParaNr,dfCh->dfHeadPosition.epStroff);
		LLDEB(dfCh->dfTailPosition.epParaNr,dfCh->dfTailPosition.epStroff);
		}
	    else{
		docCheckFieldHeadParticule(
			    part0, dfCh->dfHeadPosition.epParaNr,
			    dsAround.dsHead.dpNode, dfCh );

		docCheckFieldTailParticule(
			    part1, dfCh->dfTailPosition.epParaNr,
			    dsInside.dsTail.dpNode, dfCh );
		}
	    }

	fp->fpPosition= dfCh->dfTailPosition;
	}
    }

void docListFieldTree(	const struct DocumentTree *	dt,
			const struct BufferDocument *	bd )
    {
    const ChildFields *	cf= &(dt->dtRootFields);
    DocumentField *	dfPa= (DocumentField *)0;

    if  ( cf->cfChildCount > 0 )
	{
	FieldPosition	fp;

	fp.fpPosition= cf->cfChildren[0]->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	docCheckResultFieldTree( 0, bd, dt, &fp, dfPa, cf );
	}

    return;
    }

void docListFields(	const ChildFields *	cf,
			const struct BufferDocument *	bd )
    {
    DocumentField *	df;

    df= docGetFirstField( cf );
    if  ( df )
	{
	FieldPosition	fp;

	fp.fpPosition= df->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	while( df )
	    {
	    const int		indent= 0;
	    DocumentField *	odf= df;

	    docCheckResultFieldPosition( indent, &fp, df, bd );

	    df= docGetNextField( cf, df );
	    if  ( df == odf )
		{ XXDEB(df,odf); /*abort();*/ break;	}
	    }
	}
    }

void docListFieldsOfDocument(	const struct BufferDocument *	bd )
    {
    appDebug( "BODY:\n" );
    docListFieldTree( &(bd->bdBody), bd );

    if  ( bd->bdBody.dtRoot )
	{
	int		sect;

	for ( sect= 0; sect < bd->bdBody.dtRoot->biChildCount; sect++ )
	    {
	    const struct BufferItem *	sectNode= bd->bdBody.dtRoot->biChildren[sect];
	    SectHeadersFooters *	shf;

	    shf= sectNode->biSectHeadersFooters;

	    if  ( shf && shf->shfFirstPageHeader.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d First Page Header:\n", sect );
		docListFieldTree( &(shf->shfFirstPageHeader), bd );
		}
	    if  ( shf && shf->shfLeftPageHeader.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Left Page Header:\n", sect );
		docListFieldTree( &(shf->shfLeftPageHeader), bd );
		}
	    if  ( shf && shf->shfRightPageHeader.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Right Page Header:\n", sect );
		docListFieldTree( &(shf->shfRightPageHeader), bd );
		}
	    if  ( shf && shf->shfLastPageHeader.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Last Page Header:\n", sect );
		docListFieldTree( &(shf->shfLastPageHeader), bd );
		}
	    if  ( shf && shf->shfFirstPageFooter.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d First Page Footer:\n", sect );
		docListFieldTree( &(shf->shfFirstPageFooter), bd );
		}
	    if  ( shf && shf->shfLeftPageFooter.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Left Page Footer:\n", sect );
		docListFieldTree( &(shf->shfLeftPageFooter), bd );
		}
	    if  ( shf && shf->shfRightPageFooter.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Right Page Footer:\n", sect );
		docListFieldTree( &(shf->shfRightPageFooter), bd );
		}
	    if  ( shf && shf->shfLastPageFooter.dtRootFields.cfChildCount > 0 )
		{
		appDebug( "SECT %d Last Page Footer:\n", sect );
		docListFieldTree( &(shf->shfLastPageHeader), bd );
		}
	    }
	}
    }

static void docScanNotes(	const struct BufferDocument *	bd,
				int				treeType )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    appDebug("-- %s:\n", docTreeTypeStr(treeType) );
    dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
    if  ( dfNote )
	{
	FieldPosition	fp;

	fp.fpPosition= dfNote->dfHeadPosition;
	fp.fpPage= -1;
	fp.fpColumn= 0;

	while( dfNote )
	    {
	    const int		indent= 0;

	    docCheckResultFieldPosition( indent, &fp, dfNote, bd );

	    dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, treeType );
	    }
	}
    }

void docScanNotesOfDocument(	const struct BufferDocument *	bd )
    {
    docScanNotes( bd, DOCinFOOTNOTE );
    docScanNotes( bd, DOCinENDNOTE );
    }

void docListField(		int			indent,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);

    appDebug( "%*s %4d:%-4d .. %4d:%-4d #%d:%s ",
		indent, "",
		df->dfHeadPosition.epParaNr,
		df->dfHeadPosition.epStroff,
		df->dfTailPosition.epParaNr,
		df->dfTailPosition.epStroff,
		df->dfFieldNumber, docFieldKindStr( df->dfKind ) );

    docListFieldInstructions( fi );
    appDebug( "\n" );

    return;
    }
