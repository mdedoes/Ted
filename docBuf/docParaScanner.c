/************************************************************************/
/*									*/
/*  Traverse a selection of a tree of nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	"docParaScanner.h"
#   include	<docScanner.h>
#   include	<docLayoutPosition.h>
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	"docTextRun.h"
#   include	"docSelect.h"
#   include	"docParaParticules.h"
#   include	<bidiTree.h>
#   include	"docFields.h"
#   include	"docAttributes.h"
#   include	<textAttribute.h>
#   include	<docTabStopList.h>
#   include	<docTabStop.h>
#   include	"docParaNodeProperties.h"
#   include	"docObjects.h"
#   include	<docObject.h>
#   include	<docObjectProperties.h>
#   include	"docTreeScanner.h"
#   include	<bidiScanner.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/

typedef struct ParagraphScanner
    {
    const struct DocumentSelection *	psSelection;
    struct BufferDocument *		psDocument;
    struct BufferItem *			psParaNode;
    void *				psThrough;
    int					psFlags;

    ParticuleVisitor			psVisitParticule;
    ParaFieldVisitor			psVisitField;
    RunVisitor				psVisitRun;
    ObjectVisitor			psVisitObject;
    TabVisitor				psVisitTab;
    } ParagraphScanner;

static void docRunAttribute(
			TextRun *		tr,
			const VisitParticule *	vp,
			int			flags )
    {
    tr->trTextAttributeNr= docGetEffectiveTextAttributes(
			    &(tr->trTextAttribute),
			    vp->vpDocument, vp->vpParaNode, vp->vpParticule );

    tr->trHasUnderline= tr->trTextAttribute->taTextIsUnderlined != 0;
    tr->trHasStrikethrough= tr->trTextAttribute->taHasStrikethrough != 0;

    if  ( ( flags & FLAGpsSCAN_COMBINE_LINES )			&&
	  ( tr->trHasUnderline || tr->trHasStrikethrough )	)
	{
	TextAttribute	taPlain= *tr->trTextAttribute;

	taPlain.taTextIsUnderlined= 0;
	taPlain.taHasStrikethrough= 0;

	tr->trTextAttributeNr= docTextAttributeNumber(
						    vp->vpDocument, &taPlain );
	tr->trTextAttribute= docGetTextAttributeByNumber( vp->vpDocument,
						    tr->trTextAttributeNr );
	}

    return;
    }

static int docStepAttributeNumber(
			int *				pHaveUnderline,
			int *				pHaveStriketrough,
			struct BufferDocument *		bd,
			int				flags,
			const struct BufferItem *	paraNode,
			int				part )
    {
    const TextAttribute *	taStep;
    int				stepNr;
    int				haveUnderline;
    int				haveStriketrough;

    stepNr= docGetEffectiveTextAttributes( &taStep, bd, paraNode, part );

    haveUnderline= taStep->taTextIsUnderlined != 0;
    haveStriketrough= taStep->taHasStrikethrough != 0;

    if  ( ( flags & FLAGpsSCAN_COMBINE_LINES )	&&
	  ( haveUnderline || haveStriketrough )	)
	{
	TextAttribute	taPlain= *taStep;

	taPlain.taTextIsUnderlined= 0;
	taPlain.taHasStrikethrough= 0;

	stepNr= docTextAttributeNumber( bd, &taPlain );
	}

    *pHaveUnderline= haveUnderline;
    *pHaveStriketrough= haveStriketrough;
    return stepNr;
    }

static int docDelimitRunBwd(
			TextRun *			tr,
			const VisitParticule *		vp,
			const DocumentSelection *	ds,
			int				flags )
    {
    const TextParticule *	tpLast= vp->vpTextParticule;
    int				stroffUpto= tpLast->tpStroff+ tpLast->tpStrlen;
    int				stroffFrom;
    int				part;

    if  ( flags & FLAGpsSCAN_RUN_SEPARATELY )
	{ stroffFrom= tpLast->tpStroff;	}
    else{ stroffFrom= 0;		}

    if  ( ds && ds->dsHead.dpNode == vp->vpParaNode )
	{
	if  ( stroffFrom < ds->dsHead.dpStroff )
	    { stroffFrom=  ds->dsHead.dpStroff;	}
	}

    if  ( ds && ds->dsTail.dpNode == vp->vpParaNode )
	{
	if  ( stroffUpto > ds->dsTail.dpStroff )
	    { stroffUpto=  ds->dsTail.dpStroff;	}
	}

    docRunAttribute( tr, vp, flags );

    part= vp->vpParticule;

    if  ( stroffFrom <= tpLast->tpStroff		&&
	  ! ( flags & FLAGpsSCAN_RUN_SEPARATELY )	)
	{
	while( part > 0 )
	    {
	    const TextParticule *	tpPr;
	    int				pr;
	    int				haveUnderline;
	    int				haveStriketrough;

	    tpPr= vp->vpParaNode->biParaParticules+ part- 1;

	    if  ( tpPr->tpKind != TPkindSPAN )
		{ stroffFrom= tpPr->tpStroff+ tpPr->tpStrlen; break; }
	    if  ( tpPr->tpStroff+ tpPr->tpStrlen <= stroffFrom )
		{ break;	}

	    pr= docStepAttributeNumber( &haveUnderline, &haveStriketrough,
			    vp->vpDocument, flags, vp->vpParaNode, part- 1 );

	    if  ( pr != tr->trTextAttributeNr )
		{ stroffFrom= tpPr->tpStroff+ tpPr->tpStrlen; break; }

	    tr->trHasUnderline= tr->trHasUnderline || haveUnderline;
	    tr->trHasStrikethrough= tr->trHasStrikethrough || haveStriketrough;

	    part--;

	    if  ( tpPr->tpStroff <= stroffFrom )
		{ break;	}
	    }
	}

    tr->trDocument= vp->vpDocument;
    tr->trParaNode= vp->vpParaNode;

    tr->trStroff= stroffFrom;
    tr->trStrlen= stroffUpto- stroffFrom;
    tr->trPartFrom= part;
    tr->trPartUpto= vp->vpParticule+ 1;
    tr->trDirection= 1;

    return vp->vpParticule- part+ 1;
    }

static int docDelimitRunFwd(
			TextRun *			tr,
			const VisitParticule *		vp,
			const DocumentSelection *	ds,
			int				flags )
    {
    const TextParticule * const	tpFrom= vp->vpTextParticule;

    int				stroffFrom= tpFrom->tpStroff;
    int				stroffUpto;
    int				part;

    if  ( flags & FLAGpsSCAN_RUN_SEPARATELY )
	{ stroffUpto= tpFrom->tpStroff+ tpFrom->tpStrlen;	}
    else{ stroffUpto= docParaStrlen( vp->vpParaNode );		}

    if  ( ds && ds->dsHead.dpNode == vp->vpParaNode )
	{
	if  ( stroffFrom < ds->dsHead.dpStroff )
	    { stroffFrom=  ds->dsHead.dpStroff;	}
	}

    if  ( ds && ds->dsTail.dpNode == vp->vpParaNode )
	{
	if  ( stroffUpto > ds->dsTail.dpStroff )
	    { stroffUpto=  ds->dsTail.dpStroff;	}
	}

    docRunAttribute( tr, vp, flags );

    part= vp->vpParticule+ 1;

    if  ( stroffUpto >= tpFrom->tpStroff+ tpFrom->tpStrlen	&&
	  ! ( flags & FLAGpsSCAN_RUN_SEPARATELY )		)
	{
	while( part < vp->vpParaNode->biParaParticuleCount )
	    {
	    const TextParticule *	tpNx;
	    int				nx;
	    int				haveUnderline;
	    int				haveStriketrough;

	    tpNx= vp->vpParaNode->biParaParticules+ part;

	    if  ( tpNx->tpKind != TPkindSPAN )
		{ stroffUpto= tpNx->tpStroff; break; }
	    if  ( tpNx->tpStroff >= stroffUpto )
		{ break;	}

	    nx= docStepAttributeNumber( &haveUnderline, &haveStriketrough,
				vp->vpDocument, flags, vp->vpParaNode, part );

	    if  ( nx != tr->trTextAttributeNr )
		{ stroffUpto= tpNx->tpStroff; break; }

	    tr->trHasUnderline= tr->trHasUnderline || haveUnderline;
	    tr->trHasStrikethrough= tr->trHasStrikethrough || haveStriketrough;

	    part++;

	    if  ( tpNx->tpStroff+ tpNx->tpStrlen >= stroffUpto )
		{ break;	}
	    }
	}

    tr->trDocument= vp->vpDocument;
    tr->trParaNode= vp->vpParaNode;

    tr->trStroff= stroffFrom;
    tr->trStrlen= stroffUpto- stroffFrom;
    tr->trPartFrom= vp->vpParticule;
    tr->trPartUpto= part;
    tr->trDirection= 0;

    return part- vp->vpParticule;
    }

/************************************************************************/

static int docVisitParticule(	const ParagraphScanner *	ps,
				VisitParticule *		vp )
    {
    int				res;

    vp->vpTextAttributeNr= docGetEffectiveTextAttributes(
			&(vp->vpTextAttribute),
			vp->vpDocument, vp->vpParaNode, vp->vpParticule );

    res= (*ps->psVisitParticule)( vp, ps->psThrough );

    if  ( res == SCANadviceSKIP )
	{ LDEB(res); return -1;	}

    return res;
    }

static int docVisitField(	const ParagraphScanner *	ps,
				VisitParticule *		vp )
    {
    int				res;

    if  ( vp->vpTextParticule->tpObjectNumber < 0 )
	{
	LDEB(vp->vpTextParticule->tpObjectNumber);
	res= 0;
	}
    else{
	struct DocumentField *	df;

	df= docGetFieldByNumber( vp->vpDocument,
					vp->vpTextParticule->tpObjectNumber );
	if  ( ! df )
	    { LXDEB(vp->vpTextParticule->tpObjectNumber,df); return -1; }

	vp->vpTextAttributeNr= docGetEffectiveTextAttributes(
			&(vp->vpTextAttribute),
			vp->vpDocument, vp->vpParaNode, vp->vpParticule );

	res= (*ps->psVisitField)( vp, df, ps->psThrough );
	}

    return res;
    }

static int docVisitTab(		const ParagraphScanner *	ps,
				VisitParticule *		vp )
    {
    int				res;
    const TabStopList *		tsl;
    const struct TabStop *	ts= (const struct TabStop *)0;

    tsl= docParaNodeGetTabStopList( vp->vpParaNode, vp->vpDocument );

    if  ( vp->vpTextParticule->tpObjectNumber >= 0			&&
	  vp->vpTextParticule->tpObjectNumber < tsl->tslTabStopCount	)
	{ ts= tsl->tslTabStops+ vp->vpTextParticule->tpObjectNumber;	}

    vp->vpTextAttributeNr= docGetEffectiveTextAttributes(
			&(vp->vpTextAttribute),
			vp->vpDocument, vp->vpParaNode, vp->vpParticule );

    res= (*ps->psVisitTab)( vp, ts, ps->psThrough );

    if  ( res == SCANadviceSKIP )
	{ LDEB(res); return -1;	}

    return res;
    }

static int docVisitObject(	const ParagraphScanner *	ps,
				VisitParticule *		vp )
    {
    int				res;
    struct InsertedObject *	io= (struct InsertedObject *)0;

    io= docGetObject( vp->vpDocument, vp->vpTextParticule->tpObjectNumber );
    if  ( ! io )
	{
	LPDEB(vp->vpTextParticule->tpObjectNumber,io);
	return -1;
	}

    vp->vpTextAttributeNr= docGetEffectiveTextAttributes(
			&(vp->vpTextAttribute),
			vp->vpDocument, vp->vpParaNode, vp->vpParticule );

    res= (*ps->psVisitObject)( vp, io, ps->psThrough );
    switch( res )
	{
	case SCANadviceOK:
	    break;
	case SCANadviceSKIP:
	    return SCANadviceOK;
	case SCANadviceSTOP:
	    return res;
	default:
	    LDEB(res); return -1;
	}

    if  ( ( ps->psFlags & FLAGpsSCAN_SHAPE_TEXTS )	&&
	  io->ioKind == DOCokDRAWING_SHAPE		&&
	  io->ioDrawingShape				)
	{
	const int	treeFlags= FLAGtsSCAN_FOOT_END_NOTES;

	res= docScanShapeParagraphsLogicalOrder( ps->psDocument,
				io->ioDrawingShape, treeFlags, ps->psFlags,
				ps->psVisitParticule, ps->psVisitField,
				ps->psVisitRun, ps->psVisitObject,
				ps->psVisitTab, ps->psThrough );
	switch( res )
	    {
	    case SCANadviceOK:
	    case SCANadviceSTOP:
		break;
	    case SCANadviceSKIP:
	    default:
		LDEB(res); return -1;
	    }
	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Scan a range of particules in a paragraph moving forward.		*/
/*									*/
/************************************************************************/

static int docScanParticulesFwd( const ParagraphScanner *	ps,
				int				partFrom,
				const DocumentSelection *	ds )
    {
    VisitParticule		vp;

    vp.vpDocument= ps->psDocument;
    vp.vpParaNode= ps->psParaNode;
    vp.vpParticule= partFrom;
    vp.vpDirection= 0;

    while( vp.vpParticule < vp.vpParaNode->biParaParticuleCount )
	{
	int			res;
	int			step;
	TextRun			tr;

	vp.vpTextParticule= vp.vpParaNode->biParaParticules+ vp.vpParticule;

	if  ( ds && ds->dsTail.dpNode == vp.vpParaNode )
	    {
	    if  ( vp.vpTextParticule->tpStroff >= ds->dsTail.dpStroff )
		{ break;	}
	    }

	switch( vp.vpTextParticule->tpKind )
	    {
	    case TPkindSPAN:
		res= SCANadviceOK;
		if  ( ps->psVisitRun )
		    {
		    docInitTextRun( &tr );

		    step= docDelimitRunFwd( &tr, &vp, ds, ps->psFlags );
		    if  ( step < 1 )
			{ LDEB(step); return -1;	}

		    res= (*ps->psVisitRun)( &tr, ps->psThrough );
		    }
		else{
		    step= 1;

		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule += step; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindTAB:
		res= SCANadviceOK;
		if  ( ps->psVisitTab )
		    { res= docVisitTab( ps, &vp );	}
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule++; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindOBJECT:
		res= SCANadviceOK;
		if  ( ps->psVisitObject )
		    { res= docVisitObject( ps, &vp );	}
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule++; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindFIELDHEAD:
		res= SCANadviceOK;
		if  ( ps->psVisitField )
		    {
		    res= docVisitField( ps, &vp );

		    if  ( res == SCANadviceSKIP )
			{
			int		closed= 0;

			step= docCountParticulesInFieldFwd( vp.vpParaNode,
					&closed, vp.vpParticule,
					vp.vpParaNode->biParaParticuleCount );
			if  ( step < 0 )
			    { LDEB(step); return -1;	}

			step += closed;
			vp.vpParticule += 1+ step;
			continue;
			}
		    }
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule++; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindFIELDTAIL:
		res= SCANadviceOK;
		if  ( ps->psVisitField )
		    {
		    res= docVisitField( ps, &vp );

		    if  ( res == SCANadviceSKIP )
			{
			LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
			return -1;
			}
		    }
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule++; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    default:
		res= SCANadviceOK;
		if  ( ps->psVisitParticule )
		    { res= docVisitParticule( ps, &vp );	}

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule++; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scan a range of particules in a paragraph moving backward.		*/
/*									*/
/************************************************************************/

static int docScanParticulesBwd( const ParagraphScanner *	ps,
				int				partLast,
				const DocumentSelection *	ds )
    {
    VisitParticule		vp;

    vp.vpDocument= ps->psDocument;
    vp.vpParaNode= ps->psParaNode;
    vp.vpParticule= partLast;
    vp.vpDirection= 1;

    while( vp.vpParticule >= 0 )
	{
	int			res;
	int			step;
	TextRun			tr;

	vp.vpTextParticule= vp.vpParaNode->biParaParticules+ vp.vpParticule;

	if  ( ds && ds->dsHead.dpNode == vp.vpParaNode )
	    {
	    if  ( vp.vpTextParticule->tpStroff+ vp.vpTextParticule->tpStrlen <=
							ds->dsHead.dpStroff )
		{ break;	}
	    }

	switch( vp.vpTextParticule->tpKind )
	    {
	    case TPkindSPAN:
		res= SCANadviceOK;
		if  ( ps->psVisitRun )
		    {
		    docInitTextRun( &tr );

		    step= docDelimitRunBwd( &tr, &vp, ds, ps->psFlags );
		    if  ( step < 1 )
			{ LDEB(step); return -1;	}

		    res= (*ps->psVisitRun)( &tr, ps->psThrough );
		    }
		else{
		    step= 1;

		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule -= step; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindTAB:
		res= SCANadviceOK;
		if  ( ps->psVisitTab )
		    { res= docVisitTab( ps, &vp );	}
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule--; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindOBJECT:
		res= SCANadviceOK;
		if  ( ps->psVisitObject )
		    { res= docVisitObject( ps, &vp );	}
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule--; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindFIELDTAIL:
		res= SCANadviceOK;
		if  ( ps->psVisitField )
		    {
		    res= docVisitField( ps, &vp );

		    if  ( res == SCANadviceSKIP )
			{
			int		closed= 0;

			step= docCountParticulesInFieldBwd( vp.vpParaNode,
					&closed, vp.vpParticule,
					vp.vpParaNode->biParaParticuleCount );
			if  ( step < 0 )
			    { LDEB(step); return -1;	}

			step += closed;
			vp.vpParticule -= 1+ step;
			continue;
			}
		    }
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }
		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule--; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    case TPkindFIELDHEAD:
		res= SCANadviceOK;
		if  ( ps->psVisitField )
		    {
		    res= docVisitField( ps, &vp );

		    if  ( res == SCANadviceSKIP )
			{
			LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
			return -1;
			}
		    }
		else{
		    if  ( ps->psVisitParticule )
			{ res= docVisitParticule( ps, &vp );	}
		    }

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule--; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;

	    default:
		res= SCANadviceOK;
		if  ( ps->psVisitParticule )
		    { res= docVisitParticule( ps, &vp );	}

		if  ( res == SCANadviceSTOP )
		    { return SCANadviceSTOP;	}
		if  ( res == SCANadviceOK )
		    { vp.vpParticule--; continue;	}

		LSLDEB(vp.vpParticule,
				docKindStr(vp.vpTextParticule->tpKind),res);
		return -1;
	    }
	}

    return 0;
    }

static void docInitParagraphScanner(	ParagraphScanner *		ps )
    {
    ps->psSelection= (const struct DocumentSelection *)0;
    ps->psDocument= (struct BufferDocument *)0;
    ps->psParaNode= (struct BufferItem *)0;
    ps->psThrough= (void *)0;
    ps->psFlags= 0;

    ps->psVisitParticule= (ParticuleVisitor)0;
    ps->psVisitField= (ParaFieldVisitor)0;
    ps->psVisitRun= (RunVisitor)0;
    ps->psVisitObject= (ObjectVisitor)0;
    ps->psVisitTab= (TabVisitor)0;

    return;
    }

int docScanParagraphLogicalOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    ParagraphScanner		ps;
    int				from;

    docInitParagraphScanner( &ps );

    ps.psSelection= ds;
    ps.psDocument= bd;
    ps.psParaNode= paraNode;
    ps.psThrough= through;
    ps.psFlags= flags;

    ps.psVisitParticule= visitParticule;
    ps.psVisitField= visitField;
    ps.psVisitRun= visitRun;
    ps.psVisitObject= visitObject;
    ps.psVisitTab= visitTab;

    from= 0;
    if  ( ds && ds->dsHead.dpNode == paraNode )
	{
	if  ( docFindParticuleOfPosition( &from, (int *)0,
					    &(ds->dsHead), PARAfindLAST ) )
	    { LDEB(1); return -1;	}
	}

    return docScanParticulesFwd( &ps, from, ds );
    }

int docScanLineLogicalOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			TextLine *			tl,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    ParagraphScanner		ps;
    DocumentSelection		dsLine;
    int				from;

    docInitParagraphScanner( &ps );
    docInitDocumentSelection( &dsLine );

    if  ( ds &&
	  ( ds->dsHead.dpNode == paraNode || ds->dsTail.dpNode == paraNode ) )
	{ docIntersectLineSelection( &dsLine, paraNode, tl, ds );	}
    else{ docTextLineSelection( &dsLine, paraNode, tl, tl );	}

    ps.psSelection= &dsLine;
    ps.psDocument= bd;
    ps.psParaNode= paraNode;
    ps.psThrough= through;
    ps.psFlags= flags;

    ps.psVisitParticule= visitParticule;
    ps.psVisitField= visitField;
    ps.psVisitRun= visitRun;
    ps.psVisitObject= visitObject;
    ps.psVisitTab= visitTab;

    if  ( docFindParticuleOfPosition( &from, (int *)0,
					    &(dsLine.dsHead), PARAfindLAST ) )
	{ LDEB(1); return -1;	}

    return docScanParticulesFwd( &ps, from, &dsLine );
    }

static int docScanLineGotLevelRun(	void *		vps,
					int		initiator,
					int		level,
					int		stroffFrom,
					int		stroffUpto )
    {
    ParagraphScanner *	ps= (ParagraphScanner *)vps;
    DocumentSelection	dsLevelRun;

    if  ( ! ps->psSelection )
	{ XDEB(ps->psSelection); return -1;	}

    dsLevelRun= *(ps->psSelection);
    dsLevelRun.dsHead.dpStroff= stroffFrom;
    dsLevelRun.dsTail.dpStroff= stroffUpto;

    if  ( level % 2 == 0 )
	{
	int			partFrom;

	if  ( docFindParticuleOfPosition( &partFrom, (int *)0,
				    &(dsLevelRun.dsHead), PARAfindLAST ) )
	    { LDEB(1); return -1;	}

	return docScanParticulesFwd( ps, partFrom, &dsLevelRun );
	}
    else{
	int			partLast;

	if  ( docFindParticuleOfPosition( &partLast, (int *)0,
				    &(dsLevelRun.dsTail), PARAfindFIRST ) )
	    { LDEB(1); return -1;	}

	return docScanParticulesBwd( ps, partLast, &dsLevelRun );
	}

    return 0;
    }

int docScanLineDisplayOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			struct TextLine *		tl,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    ParagraphScanner		ps;
    DocumentSelection		dsLine;

    docInitParagraphScanner( &ps );
    docInitDocumentSelection( &dsLine );

    if  ( ds &&
	  ( ds->dsHead.dpNode == paraNode || ds->dsTail.dpNode == paraNode ) )
	{ docIntersectLineSelection( &dsLine, paraNode, tl, ds );	}
    else{ docTextLineSelection( &dsLine, paraNode, tl, tl );	}

    ps.psSelection= &dsLine;
    ps.psDocument= bd;
    ps.psParaNode= paraNode;
    ps.psThrough= through;
    ps.psFlags= flags;

    ps.psVisitParticule= visitParticule;
    ps.psVisitField= visitField;
    ps.psVisitRun= visitRun;
    ps.psVisitObject= visitObject;
    ps.psVisitTab= visitTab;

    if  ( paraNode->biParaBidiRoot )
	{
	BidiScanner	bs;
	int		res;

	bidiInitScanner( &bs );
	bs.bsThrough= (void *)&ps;
	bs.bsGotRun= docScanLineGotLevelRun;

	res= bidiTraverseNodeDisplayOrder( paraNode->biParaBidiRoot, &bs,
			dsLine.dsHead.dpStroff, dsLine.dsTail.dpStroff );
	if  ( res < 0 )
	    {
	    LDEB(res);
	    bidiListNode(paraNode->biParaBidiRoot,
					    &(paraNode->biParaStringBuffer));
	    docListNode(bd,0,paraNode,0);
	    return -1;
	    }

	return res;
	}
    else{
	int		from;

	if  ( docFindParticuleOfPosition( &from, (int *)0,
					    &(dsLine.dsHead), PARAfindLAST ) )
	    { LDEB(1); return -1;	}

	return docScanParticulesFwd( &ps, from, &dsLine );
	}
    }

