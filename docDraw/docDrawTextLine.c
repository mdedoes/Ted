#   include	"docDrawConfig.h"

#   include	<stdlib.h>

#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<docLayoutLine.h>
#   include	<docLayout.h>
#   include	<layoutContext.h>
#   include	<docTextLine.h>
#   include	<docTabStop.h>

#   include	<docTreeNode.h>
#   include	<docTextRun.h>
#   include	<docTextParticule.h>
#   include	<docBorderProperties.h>
#   include	<docBlockOrnaments.h>
#   include	<docParticuleData.h>
#   include	<docStripFrame.h>
#   include	<docScanner.h>
#   include	<docParaScanner.h>
#   include	<docParaProperties.h>
#   include	<docPropVal.h>
#   include	<docAttributes.h>
#   include	<docObjectProperties.h>
#   include	<docObject.h>
#   include	<docShape.h>
#   include	<docLayoutObject.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start drawing a text line.						*/
/*									*/
/************************************************************************/

static void docSetDrawTextLine(
			DrawTextLine *			dtl,
			void *				through,
			DrawingContext *		dc,
			TextLine *			tl,
			struct BufferItem *		paraNode,
			const BlockOrigin *		bo,
			const struct ParagraphFrame *	pf,
			const DocumentRectangle *	drLineFramePixels )
    {
    const LayoutContext *	lc= dc->dcLayoutContext;

    dtl->dtlThrough= through;
    dtl->dtlDrawingContext= dc;
    dtl->dtlDocument= lc->lcDocument;
    dtl->dtlTree= dc->dcTree;

    dtl->dtlTextLine= tl;
    dtl->dtlPartUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    dtl->dtlParaNode= paraNode;
    dtl->dtlXShift= bo?bo->boXShift:0;
    dtl->dtlYShift= bo?bo->boYShift:0;
    dtl->dtlParagraphFrame= pf;
    dtl->dtlLineFrameRectanglePixels= *drLineFramePixels;

    docTextLineGetShiftedTop(
			&(dtl->dtlShiftedTopPosition), bo, tl );
    docTextLineGetShiftedBaseline(
			&(dtl->dtlShiftedBaselinePosition), bo, tl );
    docTextLineGetShiftedBelowLine(
			&(dtl->dtlShiftedBelowLinePosition), bo, tl );

    dtl->dtlLineHeight= dtl->dtlShiftedBelowLinePosition.lpPageYTwips-
				dtl->dtlShiftedTopPosition.lpPageYTwips;

    docLineTextRectangleTwips( &(dtl->dtlTextRectangle),
				&(dtl->dtlShiftedTopPosition),
				&(dtl->dtlShiftedBelowLinePosition), tl, pf );

    dtl->dtlShiftedBaselinePixels= docLayoutYPixels( lc,
					&(dtl->dtlShiftedBaselinePosition) );

    dtl->dtlShiftedLineCenterPixels= (
	dtl->dtlShiftedBaselinePixels+
	docLayoutYPixels( lc, &(dtl->dtlShiftedTopPosition) )+
	docLayoutYPixels( lc, &(dtl->dtlShiftedBelowLinePosition) ) ) / 3;

    dtl->dtlX= dtl->dtlTextRectangle.drX0;

    return;
    }

/************************************************************************/

static void docInitDrawTextLine(	DrawTextLine *	dtl )
    {
    dtl->dtlThrough= (void *)0;
    dtl->dtlDrawingContext= (DrawingContext *)0;
    dtl->dtlDocument= (struct BufferDocument *)0;
    dtl->dtlTree= (struct DocumentTree *)0;

    geoInitRectangle( &(dtl->dtlLineFrameRectanglePixels) );
    docInitLayoutPosition( &(dtl->dtlShiftedTopPosition) );
    docInitLayoutPosition( &(dtl->dtlShiftedBaselinePosition) );
    docInitLayoutPosition( &(dtl->dtlShiftedBelowLinePosition) );
    dtl->dtlShiftedBaselinePixels= 0;
    dtl->dtlShiftedLineCenterPixels= 0;
    dtl->dtlLineHeight= 0;
    dtl->dtlXShift= 0;
    dtl->dtlYShift= 0;
    dtl->dtlPartUpto= 0;
    dtl->dtlParticuleData= (const struct ParticuleData *)0;
    dtl->dtlTextLine= (struct TextLine *)0;
    dtl->dtlParaNode= (struct BufferItem *)0;
    dtl->dtlParagraphFrame= (struct ParagraphFrame *)0;
    dtl->dtlDrawParticulesSeparately= 0;

    geoInitRectangle( &(dtl->dtlTextRectangle) );

    dtl->dtlX= 0;
    }

/************************************************************************/
/*									*/
/*  Draw the underlines of a text run.					*/
/*									*/
/************************************************************************/

static void docDrawRunUnderlines(	const DrawTextLine *	dtl,
					int			x0Twips,
					int			direction,
					int			partFrom,
					int			stroffUpto,
					const LayoutPosition *	baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    struct BufferDocument *	bd= dtl->dtlDocument;

    const struct BufferItem *	paraNode= dtl->dtlParaNode;
    int				part= partFrom;
    const TextParticule *	tp= paraNode->biParaParticules;

    while( part < paraNode->biParaParticuleCount )
	{
	int			textAttrNr;
	const TextAttribute *	ta;

	int		head;
	int		past;
	int		x1Twips;

	if  ( docNextUnderlinedRun( &head, &past, &x0Twips, &x1Twips,
					&textAttrNr, &ta,
					bd, paraNode, part, stroffUpto ) )
	    { break;	}

	(*dc->dcDrawUnderline)( dtl, head, past, direction,
				textAttrNr, ta, x0Twips, x1Twips, baseLine );

	part= past;
	x0Twips= x1Twips;

	if  ( part >= paraNode->biParaParticuleCount	||
	      tp[part].tpStroff >= stroffUpto		)
	    { break;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the strikethrough of a text run.				*/
/*									*/
/************************************************************************/

static void docDrawRunStrikethrough(	const DrawTextLine *	dtl,
					int			x0Twips,
					int			direction,
					int			partFrom,
					int			stroffUpto,
					const LayoutPosition *	baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    struct BufferDocument *	bd= dtl->dtlDocument;

    const struct BufferItem *	paraNode= dtl->dtlParaNode;
    int				part= partFrom;
    const TextParticule *	tp= paraNode->biParaParticules;

    while( part < paraNode->biParaParticuleCount )
	{
	int			textAttrNr;
	const TextAttribute *	ta;

	int		head;
	int		past;
	int		x1Twips;

	if  ( docNextStrikethroughRun( &head, &past, &x0Twips, &x1Twips,
					&textAttrNr, &ta,
					bd, paraNode, part, stroffUpto ) )
	    { break;	}

	(*dc->dcDrawStrikethrough)( dtl, head, past, direction,
				textAttrNr, ta, x0Twips, x1Twips, baseLine );

	part= past;
	x0Twips= x1Twips;

	if  ( part >= paraNode->biParaParticuleCount	||
	      tp[part].tpStroff >= stroffUpto		)
	    { break;	}
	}

    return;
    }

/************************************************************************/

static int docDrawTextBorders(
			const DrawTextLine *		dtl,
			const DocumentRectangle *	drLine,
			int				xTwips )
    {
    DrawingContext *			dc= dtl->dtlDrawingContext;
    struct BufferDocument *		bd= dtl->dtlDocument;
    const struct TextLine *		tl= dtl->dtlTextLine;

    BlockOrnaments			ornaments;

    int					part;
    int					partUpto;

    docInitBlockOrnaments( &ornaments );

    part= tl->tlFirstParticule;
    partUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    while( part < partUpto )
	{
	int			borderNumber;
	int			ignored;
	int			next;

	DocumentRectangle	drText;

	part= docDelimitBorderStretch( &borderNumber, &xTwips,
				(DocumentRectangle *)0,
				0, bd, dtl->dtlParaNode, part, partUpto );

	if  ( part >= partUpto )
	    { break;	}

	drText= *drLine;

	next= docDelimitBorderStretch( &ignored, &xTwips, &drText,
			borderNumber, bd, dtl->dtlParaNode, part, partUpto );

	if  ( next > part && xTwips > drText.drX0 )
	    {
	    ornaments.boTopBorder= docGetBorderPropertiesByNumber(
							    bd, borderNumber );
	    ornaments.boTopBorderNumber= borderNumber;

	    if  ( DOCisBORDER( ornaments.boTopBorder ) )
		{
		int			thick;
		int			space;
		DocumentRectangle	drInside;

		PROPmaskADD( &(ornaments.boPropMask), ORNdrawTOP_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawLEFT_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawRIGHT_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawBOTTOM_BORDER );

		ornaments.boLeftBorder= ornaments.boTopBorder;
		ornaments.boRightBorder= ornaments.boTopBorder;
		ornaments.boBottomBorder= ornaments.boTopBorder;

		ornaments.boLeftBorderNumber= ornaments.boTopBorderNumber;
		ornaments.boRightBorderNumber= ornaments.boTopBorderNumber;
		ornaments.boBottomBorderNumber= ornaments.boTopBorderNumber;

		geoIntersectRectangle( &drText, &drText, drLine );

		thick= docBorderThick( &space, ornaments.boTopBorder );
		drInside= drText;

		drInside.drX0 += thick;
		drInside.drX1 -= thick;
		drInside.drY0 += thick;
		drInside.drY1 -= thick;

		if  ( (*dc->dcDrawOrnaments)( &ornaments,
				dtl->dtlShiftedTopPosition.lpPage,
				&drText, &drInside, dtl->dtlThrough, dc ) )
		    { LDEB(1); return -1;	}
		}
	    }

	part= next;
	}

    return 0;
    }

/************************************************************************/

static int docDrawTextShading(
			const DrawTextLine *		dtl,
			const DocumentRectangle *	drLine,
			int				xTwips )
    {
    DrawingContext *			dc= dtl->dtlDrawingContext;
    struct BufferDocument *		bd= dtl->dtlDocument;
    const struct TextLine *		tl= dtl->dtlTextLine;

    BlockOrnaments			ornaments;

    int					part;
    int					partUpto;

    docInitBlockOrnaments( &ornaments );
    PROPmaskADD( &(ornaments.boPropMask), ORNdrawSHADE );

    part= tl->tlFirstParticule;
    partUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    while( part < partUpto )
	{
	int			shadingNumber;
	int			ignored;
	int			next;
	DocumentRectangle	drText;

	part= docDelimitShadingStretch( &shadingNumber, &xTwips,
				    (DocumentRectangle *)0,
				    0, bd, dtl->dtlParaNode, part, partUpto );

	if  ( part >= partUpto )
	    { break;	}

	drText= *drLine;

	next= docDelimitShadingStretch( &ignored, &xTwips, &drText,
			shadingNumber, bd, dtl->dtlParaNode, part, partUpto );

	if  ( next > part && xTwips > drText.drX0 )
	    {
	    ornaments.boShading= docGetItemShadingByNumber( 
							bd, shadingNumber );

	    geoIntersectRectangle( &drText, &drText, drLine );

	    if  ( (*dc->dcDrawOrnaments)( &ornaments,
				    dtl->dtlShiftedTopPosition.lpPage,
				    &drText, &drText, dtl->dtlThrough, dc ) )
		{ LDEB(1); return -1;	}
	    }

	part= next;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw borders and shading below the text of a line.			*/
/*									*/
/*  Experimentation with MS-Word reveals:				*/
/*  1)  That MS-Word draws the border over the text.			*/
/*  2)  The the border is subtracted from the rectangle.		*/
/*  3)  That the same rectangle is used for borders and shading. This	*/
/*	same rectangle is shown in reverse when text is selected.	*/
/*									*/
/************************************************************************/

static int docDrawLineBackground(	const DrawTextLine *	dtl )
    {
    DocumentRectangle		drLine;
    int				xTwips;

    drLine= dtl->dtlParagraphFrame->pfParaContentRect;
    drLine.drY0= dtl->dtlTextRectangle.drY0;
    drLine.drY1= dtl->dtlTextRectangle.drY1;

    xTwips= dtl->dtlTextRectangle.drX0;

    if  ( dtl->dtlTextLine->tlFlags & TLflagSHADING )
	{
	if  ( docDrawTextShading( dtl, &drLine, xTwips ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dtl->dtlTextLine->tlFlags & TLflagBORDER )
	{
	if  ( docDrawTextBorders( dtl, &drLine, xTwips ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/

static int docDrawLineField(	const VisitParticule *		vp,
				struct DocumentField *		df,
				void *				vdtl )
    {
    DrawTextLine *		dtl= (DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				res= SCANadviceOK;

    const TextParticule *	tp= vp->vpTextParticule;
    int				x0Twips= dtl->dtlX+ dtl->dtlXShift;
    int				x1Twips= x0Twips+ tp->tpWide;

    switch( tp->tpKind )
	{
	case TPkindFIELDHEAD:
	    if  ( vp->vpDirection == 0 )
		{
		if  ( dc->dcStartField )
		    {
		    res= (*dc->dcStartField)( dtl, vp->vpParticule,
								x0Twips, df );
		    if  ( res < 0 )
			{ LLDEB(vp->vpParticule,res); return -1;	}
		    }
		}
	    else{
		if  ( dc->dcFinishField )
		    {
		    res= (*dc->dcFinishField)( dtl, vp->vpParticule,
								x1Twips, df );
		    if  ( res < 0 )
			{ LLDEB(vp->vpParticule,res); return -1;	}
		    }
		}

	    if  ( tp->tpWide != 0 )
		{ LDEB(tp->tpWide);	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindFIELDTAIL:
	    if  ( vp->vpDirection == 0 )
		{
		if  ( dc->dcFinishField )
		    {
		    res= (*dc->dcFinishField)( dtl, vp->vpParticule,
								x1Twips, df );
		    if  ( res < 0 )
			{ LLDEB(vp->vpParticule,res); return -1;	}
		    }
		}
	    else{
		if  ( dc->dcStartField )
		    {
		    res= (*dc->dcStartField)( dtl, vp->vpParticule,
								x0Twips, df );
		    if  ( res < 0 )
			{ LLDEB(vp->vpParticule,res); return -1;	}
		    }
		}

	    if  ( tp->tpWide != 0 )
		{ LDEB(tp->tpWide);	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	default:
	    SDEB(docFieldKindStr(tp->tpKind));
	    return -1;
	}
    }

static int docDrawLineTab(	const VisitParticule *	vp,
				const TabStop *		ts,
				void *			vdtl )
    {
    DrawTextLine *		dtl= (DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				res= SCANadviceOK;

    const TextParticule *	tp= vp->vpTextParticule;
    int				x0Twips= dtl->dtlX+ dtl->dtlXShift;
    int				x1Twips= x0Twips+ tp->tpWide;
    LayoutPosition		spanBaseline;

    int				leader= DOCtlNONE;

    if  ( ! dc->dcDrawTab )
	{ XDEB(dc->dcDrawTab); return -1;	}

    spanBaseline= dtl->dtlShiftedBaselinePosition;
    spanBaseline.lpPageYTwips += 10* vp->vpTextAttribute->taBaselineShiftHalfPoints;

    if  ( ts )
	{ leader= ts->tsLeader;	}

    res= (*dc->dcDrawTab)( dtl, vp->vpParticule,
			vp->vpTextAttributeNr, vp->vpTextAttribute,
			leader, x0Twips, x1Twips, &spanBaseline );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    if  ( vp->vpTextAttribute->taTextIsUnderlined	&&
	  dc->dcDrawUnderline				)
	{
	docDrawRunUnderlines( dtl, x0Twips, vp->vpDirection,
		vp->vpParticule, tp->tpStroff+ tp->tpStrlen, &spanBaseline );
	}
    if  ( vp->vpTextAttribute->taHasStrikethrough	&&
	  dc->dcDrawStrikethrough			)
	{
	docDrawRunStrikethrough( dtl, x0Twips, vp->vpDirection,
		vp->vpParticule, tp->tpStroff+ tp->tpStrlen, &spanBaseline );
	}

    dtl->dtlX += tp->tpWide;
    return res;
    }

static int docDrawLineObject(	const VisitParticule *	vp,
				struct InsertedObject *	io,
				void *			vdtl )
    {
    DrawTextLine *		dtl= (DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				res= SCANadviceOK;

    const TextParticule *	tp= vp->vpTextParticule;
    int				x0Twips= dtl->dtlX+ dtl->dtlXShift;
    LayoutPosition		spanBaseline;

    DocumentRectangle		drOutside;

    if  ( ! dc->dcDrawInlineObject )
	{ XDEB(dc->dcDrawInlineObject); return -1;	}

    spanBaseline= dtl->dtlShiftedBaselinePosition;
    spanBaseline.lpPageYTwips +=
		10* vp->vpTextAttribute->taBaselineShiftHalfPoints;

    docObjectGetPageRect( &drOutside, io,
				x0Twips, spanBaseline.lpPageYTwips );

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	DrawingShape *	ds= io->ioDrawingShape;
	int		inLine= ds->dsDrawing.sd_fPseudoInline;

	if  ( inLine )
	    {
	    int		high= drOutside.drY1- drOutside.drY0;

	    /* Coordinates are relative to the top, rather than the base line */
	    drOutside.drY0 -= high;
	    drOutside.drY1 -= high;

	    res= docDrawShape( dc, dtl->dtlThrough, dc->dcBodySectNode,
							    &drOutside, io );
	    if  ( res < 0 )
		{ LDEB(res); res= SCANadviceOK;	}
	    dtl->dtlX += tp->tpWide;
	    }
	}
    else{
	res= (*dc->dcDrawInlineObject)( dtl, vp->vpParticule, io,
						    &drOutside, &spanBaseline );
	if  ( res < 0 )
	    { LDEB(res); res= SCANadviceOK;	}
	dtl->dtlX += tp->tpWide;
	}

    return res;
    }

static int docDrawLineParticule( const VisitParticule *	vp,
				void *			vdtl )
    {
    DrawTextLine *		dtl= (DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				res= SCANadviceOK;

    const TextParticule *	tp= vp->vpTextParticule;
    LayoutPosition		spanBaseline;

    spanBaseline= dtl->dtlShiftedBaselinePosition;
    spanBaseline.lpPageYTwips +=
		10* vp->vpTextAttribute->taBaselineShiftHalfPoints;

    /*  1  */
    switch( tp->tpKind )
	{
	case TPkindTAB:
	    if  ( dc->dcDrawTab )
		{ XDEB(dc->dcDrawTab); return -1;	}

	    dtl->dtlX += tp->tpWide;

	    return res;

	case TPkindFIELDHEAD:
	    /* NO! If we are not skipping the field, you get here (Bwd)
	    if  ( dc->dcStartField )
		{ XXDEB(dc->dcStartField,dc->dcFinishField); return -1; }
	    */

	    if  ( tp->tpWide != 0 )
		{ LDEB(tp->tpWide);	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindFIELDTAIL:
	    /* NO! If we are not skipping the field, you get here (Fwd)
	    if  ( dc->dcFinishField )
		{ XXDEB(dc->dcStartField,dc->dcFinishField); return -1; }
	    */

	    if  ( tp->tpWide != 0 )
		{ LDEB(tp->tpWide);	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindLINEBREAK:
	case TPkindPAGEBREAK:
	case TPkindCOLUMNBREAK:
	    if  ( tp->tpWide != 0 )
		{ LDEB(tp->tpWide);	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindOBJECT:
	    if  ( dc->dcDrawInlineObject )
		{ XDEB(dc->dcDrawInlineObject); return -1;	}

	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindCHFTNSEP:
	case TPkindCHFTNSEPC:
	    if  ( dc->dcDrawFtnsep )
		{
		res= (*dc->dcDrawFtnsep)( dtl, vp->vpParticule,
			    vp->vpTextAttributeNr, vp->vpTextAttribute,
			    dtl->dtlX, dtl->dtlX+ tp->tpWide,
			    &spanBaseline );
		}
	    dtl->dtlX += tp->tpWide;
	    return res;

	case TPkindOPT_HYPH:
	case TPkindLTR_MARK:
	case TPkindRTL_MARK:
	    if  ( tp->tpWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpWide);	}
	    dtl->dtlX += tp->tpWide;
	    return res;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    return res;
    }

static int docDrawLineRun(	const TextRun *			tr,
				void *				vdtl )
    {
    DrawTextLine *		dtl= (DrawTextLine *)vdtl;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    int				res= SCANadviceOK;

    int				twipsWide= docTextRunTwipsWide( tr );

    if  ( tr->trStrlen > 0 )
	{
	int			x0Twips= dtl->dtlX+ dtl->dtlXShift;
	int			x1Twips= x0Twips+ twipsWide;
	LayoutPosition		spanBaseline;

	spanBaseline= dtl->dtlShiftedBaselinePosition;
	spanBaseline.lpPageYTwips +=
			10* tr->trTextAttribute->taBaselineShiftHalfPoints;

	if  ( tr->trHasUnderline && dc->dcDrawUnderline )
	    {
	    docDrawRunUnderlines( dtl, x0Twips, tr->trDirection,
		tr->trPartFrom, tr->trStroff+ tr->trStrlen, &spanBaseline );
	    }
	if  ( tr->trHasStrikethrough && dc->dcDrawStrikethrough )
	    {
	    docDrawRunStrikethrough( dtl, x0Twips, tr->trDirection,
		tr->trPartFrom, tr->trStroff+ tr->trStrlen, &spanBaseline );
	    }

	if  ( dc->dcDrawTextRun )
	    {
	    const char *		outputString;

	    outputString= docParaString( tr->trParaNode, tr->trStroff );

	    res= (*dc->dcDrawTextRun)( tr, x0Twips, x1Twips,
					    dtl, &spanBaseline, outputString );
	    if  ( res < 0 )
		{ LLDEB(tr->trStrlen,res); return -1;	}
	    }
	}

    dtl->dtlX += twipsWide;
    return res;
    }

/************************************************************************/
/*									*/
/*  Draw the text particles on a line.					*/
/*									*/
/*  NOTE: As the scan order determines the position of the text on the	*/
/*  line, we do not pass the selection to the scanner: Only scanning	*/
/*  part of the line would cause the text to end up in a different	*/
/*  horizontal position. We do not call the actual drawing routines	*/
/*  for text outside the selection in the visitor callbacks however.	*/
/*									*/
/************************************************************************/

static int docDrawLineParticules(	DrawTextLine *			dtl )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;

    int				res;

    int				flags;

    ParaFieldVisitor		fieldVisitor= (ParaFieldVisitor)0;
    TabVisitor			tabVisitor= (TabVisitor)0;
    ObjectVisitor		objectVisitor= (ObjectVisitor)0;

    if  ( dtl->dtlDrawParticulesSeparately )
	{ flags= FLAGpsSCAN_RUN_SEPARATELY;	}
    else{ flags= FLAGpsSCAN_COMBINE_LINES;	}

    if  ( dc->dcStartField || dc->dcFinishField )
	{ fieldVisitor= docDrawLineField;	}
    if  ( dc->dcDrawTab )
	{ tabVisitor= docDrawLineTab;	}
    if  ( dc->dcDrawInlineObject )
	{ objectVisitor= docDrawLineObject;	}

    res= docScanLineDisplayOrder( dtl->dtlDocument,
				dtl->dtlParaNode, dtl->dtlTextLine,
				(struct DocumentSelection *)0, flags,
				docDrawLineParticule,
				fieldVisitor,
				docDrawLineRun,
				objectVisitor,
				tabVisitor,
				(void *)dtl );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Generic Document drawing machinery.					*/
/*									*/
/************************************************************************/

static int docDrawLayoutTextLine(
				const LayoutContext *		lc,
				struct BufferItem *		paraNode,
				int				part,
				const ParagraphFrame *		pf,
				ParticuleData *			pd )
    {
    int				accepted;

    LineRun			blr;
    LineLayoutJob		llj;

    docInitLineLayoutJob( &llj );

    docStartLayoutLine( &blr, &llj, paraNode, part, lc, pd+ part, pf );

    accepted= docLayoutLineBox( &blr, &llj );
    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    return 0;
    }

int docDrawTextLine(	struct BufferItem *		paraNode,
			TextLine *			tl,
			const ParagraphFrame *		pf,
			const DocumentRectangle *	drLineFramePixels,
			void *				through,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    int				rval= 0;
    const LayoutContext *	lc= dc->dcLayoutContext;

    ParticuleData *		pd= (ParticuleData *)0;
    int				part= tl->tlFirstParticule;
    const int			isLast= tl- paraNode->biParaLines == paraNode->biParaLineCount- 1;

    DrawTextLine		dtl;

    docInitDrawTextLine( &dtl );

    docSetDrawTextLine( &dtl, through, dc, tl, paraNode, bo, pf,
							drLineFramePixels );

    dtl.dtlDrawParticulesSeparately= dc->dcDrawParticulesSeparately ||
		( paraNode->biParaProperties->ppAlignment == DOCthaJUSTIFIED && ! isLast );

    if  ( dc->dcDrawOrnaments				&&
	  ( ( tl->tlFlags & TLflagSHADING )	||
	    ( tl->tlFlags & TLflagBORDER )	)	)
	{ docDrawLineBackground( &dtl );	}

    rval= (*dc->dcStartTextLine)( &dtl, dtl.dtlX );
    if  ( rval < 0 )
	{ LDEB(rval); goto ready;	}
    if  ( rval > 0 )
	{ goto ready;	}

    pd= malloc( paraNode->biParaParticuleCount* sizeof(ParticuleData) );
    if  ( ! pd )
	{ LXDEB(paraNode->biParaParticuleCount,pd); rval= -1; goto ready; }

    if  ( docDrawLayoutTextLine( lc, paraNode, part, pf, pd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dtl.dtlParticuleData= pd;

    rval= docDrawLineParticules( &dtl );
    if  ( rval < 0 )
	{ LDEB(rval); goto ready;	}
    if  ( rval > 0 )
	{ goto ready;	}

    if  ( dc->dcFinishTextLine )
	{
	rval= (*dc->dcFinishTextLine)( &dtl, dtl.dtlX );
	if  ( rval < 0 )
	    { LDEB(rval); goto ready;	}
	if  ( rval > 0 )
	    { goto ready;	}
	}

  ready:

    if  ( pd )
	{ free( pd );	}

    return rval;
    }

