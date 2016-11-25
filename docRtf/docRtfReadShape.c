/************************************************************************/
/*									*/
/*  Read RTF shapes.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"
#   include	<docShapeProp.h>
#   include	<docObjectProperties.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docParaParticules.h>

/************************************************************************/

static int docRtfReadChildShape(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc );

/************************************************************************/
/*									*/
/*  Property control words inside a shape.				*/
/*									*/
/************************************************************************/

int docRtfShpProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeProperties *	sp;

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrcCurrentLine,ds); return 0;	}

    sp= &(ds->dsShapeProperties);

    if  ( rcw->rcwID != SHPpropLID	&&
	  ds->dsIsChildShape		)
	{ SLDEB(rcw->rcwWord,ds->dsIsChildShape);	}

    if  ( docSetShapeProperty( sp, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Content inside a shape.						*/
/*									*/
/************************************************************************/

static int docRtfShpText(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;

    int			treeType= DOCinSHPTXT;
    const int		ignoreEmpty= 0;

    /* No! to deal with faulty documents.
    rrc->rrcDrawingShape= (DrawingShape *)0;
    */

    if  ( docRtfReadDocumentTree( rcw, &(ds->dsDocumentTree), &treeType,
				rrc, ignoreEmpty, &(ds->dsSelectionScope) ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ds->dsSelectionScope.ssTreeType != treeType )
	{
	LLDEB(ds->dsSelectionScope.ssTreeType,treeType);
	ds->dsSelectionScope.ssTreeType= treeType;
	docSetTreeTypeOfNode( ds->dsDocumentTree.dtRoot, treeType );
	}

    /* No! to deal with faulty documents.
    rrc->rrcDrawingShape= ds;
    */
    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an {\sn .... } {\sv .... } pair.				*/
/*									*/
/*  NOTE that the concept of an 'RtfControlWord' is abused for		*/
/*	 the shape property.						*/
/*									*/
/************************************************************************/

static int docRtfShpCollectNumbers(	int *			numbers,
					int			count,
					RtfReader *		rrc )
    {
    int			i;
    SimpleInputStream *	sis= rrc->rrcSis;
    int			c= sioInGetByte( sis );

    for ( i= 0; i < count; i++ )
	{
	int		sign;
	int		val;

	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetByte( sis );
	    }

	if  ( c != ';' )
	    { CDEB(c); return -1;	}
	c= sioInGetByte( sis );

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetByte( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetByte( sis ); }

	numbers[i]= sign* val;

	if  ( c == '}' )
	    { break;	}
	}

    if  ( c != '}' )
	{ CDEB(c); return -1;	}
    sioInUngetLastRead( sis );

    if  ( count > 0 )
	{ numbers[count]= numbers[0]; }

    return 0;
    }

static int docRtfShpCollectVertices(	ShapeVertex *		vertices,
					int			count,
					RtfReader *		rrc )
    {
    int			i;
    SimpleInputStream *	sis= rrc->rrcSis;
    int			c= sioInGetByte( sis );

    for ( i= 0; i < count; i++ )
	{
	int		sign;
	int		val;

	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetByte( sis );
	    }

	if  ( c != ';' )
	    { CDEB(c); return -1;	}
	c= sioInGetByte( sis );
	if  ( c != '(' )
	    { CDEB(c); return -1;	}
	c= sioInGetByte( sis );

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetByte( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetByte( sis ); }
	if  ( c != ',' )
	    { CDEB(c); return -1;	}
	c= sioInGetByte( sis );

	vertices[i].svX= sign* val;

	sign= 1; val= 0;
	if  ( c == '-' )
	    { sign= -1; c= sioInGetByte( sis );	}
	while( isdigit( c ) )
	    { val= 10* val+ c- '0'; c= sioInGetByte( sis ); }

	vertices[i].svY= sign* val;

	if  ( c != ')' )
	    { CDEB(c); return -1;	}
	c= sioInGetByte( sis );
	}

    if  ( c != '}' )
	{ CDEB(c); return -1;	}
    sioInUngetLastRead( sis );

    if  ( count > 0 )
	{ vertices[count]= vertices[0]; }

    return 0;
    }

int docRtfShpArray(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    SimpleInputStream *	sis= rrc->rrcSis;
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    int			rval= 0;

    int			c;
    int			sign;

    int			size;
    int			count;

    ShapeVertex *	vertices= (ShapeVertex *)0;
    int *		numbers= (int *)0;

    c= sioInGetByte( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetByte( sis );
	}

    sign= 1; size= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetByte( sis );	}
    while( isdigit( c ) )
	{ size= 10* size+ c- '0'; c= sioInGetByte( sis ); }
    size *= sign;

    if  ( c != ';' )
	{ CDEB(c); rval= -1; goto ready;	}
    c= sioInGetByte( sis );

    sign= 1; count= 0;
    if  ( c == '-' )
	{ sign= -1; c= sioInGetByte( sis );	}
    while( isdigit( c ) )
	{ count= 10* count+ c- '0'; c= sioInGetByte( sis ); }
    count *= sign;

    sioInUngetLastRead( sis );

    switch( size )
	{
	case 8:
	    vertices= (ShapeVertex *)malloc( (count+ 1)* sizeof( ShapeVertex ) );
	    if  ( ! vertices )
		{ LXDEB(count,vertices); rval= -1; goto ready; }
	    if  ( docRtfShpCollectVertices( vertices, count, rrc ) )
		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	case 2:
	case 4:
	    numbers= (int *)malloc( (count+ 1)* sizeof( int ) );
	    if  ( ! numbers )
		{ LXDEB(count,numbers); rval= -1; goto ready; }
	    if  ( docRtfShpCollectNumbers( numbers, count, rrc ) )

		{ LDEB(count); rval= -1; goto ready;	}
	    break;

	default:
	    LLDEB(count,size); rval= -1; goto ready;
	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); goto ready;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_pVerticies:
	    sd->sdVertices= vertices;
	    vertices= (ShapeVertex *)0;	/* steal */
	    sd->sdVertexCount= count;
	    break;

	case DSHPprop_pWrapPolygonVertices:
	    sd->sdWrapPolygonVertices= vertices;
	    vertices= (ShapeVertex *)0;	/* steal */
	    sd->sdWrapPolygonVertexCount= count;
	    break;

	case DSHPprop_pSegmentInfo:
	    sd->sdSegmentInfos= numbers;
	    numbers= (int *)0;	/* steal */
	    sd->sdSegmentInfoCount= count;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

  ready:
    if  ( vertices )
	{ free( vertices );	}
    if  ( numbers )
	{ free( numbers );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Consume a picture value attached to a shape.			*/
/*									*/
/************************************************************************/

static int docRtfSaveShapeData(	RtfReader *	rrc,
				const char *		text,
				int			len )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( utilMemoryBufferSetBytes( &(ds->dsPictureData),
					(const unsigned char *)text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }


static int docRtfReadShapePicture(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState		internRrs;

    int				rval;

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1, rrc,
			    (const RtfControlWord *)0, docRtfSaveShapeData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    docRtfPopReadingState( rrc );

    return rval;
    }

int docRtfShpPicture(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    int			control;
    char		controlWord[TEDszRTFCONTROL+1];
    int			gotArg;
    int			locArg= -1;
    int			c;
    DrawingShape *	ds= rrc->rrcDrawingShape;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    control= docRtfFindControl( rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    /*
    LSDEB(rrc->rrcCurrentLine,controlWord);
    */

    if  ( strcmp( controlWord, RTFtag_pict ) )
	{ SDEB(controlWord); return -1;	}

    utilPropMaskClear( &(rrc->rrcPicturePropMask) );
    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    if  ( docRtfReadShapePicture( rcw, arg, rrc ) )
	{ SSDEB(rcw->rcwWord,controlWord); return -1;	}

    if  ( ! ds )
	{ XDEB(ds);						}
    else{ ds->dsPictureProperties= rrc->rrcPictureProperties;	}

    return 0;
    }

int docRtfShpString(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    SimpleInputStream *	sis= rrc->rrcSis;
    int			c;

    c= sioInGetByte( sis );
    while( c != '}' )
	{
	if  ( c == EOF )
	    { LDEB(c); return -1;	}

	c= sioInGetByte( sis );
	}

    sioInUngetLastRead( sis );

    return 0;
    }

static int docRtfShpGetNumber(	long *			pVal,
				RtfReader *		rrc )
    {
    SimpleInputStream *	sis= rrc->rrcSis;
    int			c;
    long		val;
    int			sign= 1;

    c= sioInGetByte( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetByte( sis );
	}

    val= 0;

    if  ( c == '-' )
	{ sign= -1; c= sioInGetByte( sis );	}

    while( isdigit( c ) )
	{ val= 10* val+ c- '0'; c= sioInGetByte( sis ); }

    val *= sign;

    sioInUngetLastRead( sis );

    *pVal= val;
    return 0;
    }

int docRtfShpPositionNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_posh:
	    sd->sdXPosition= val;
	    break;
	case DSHPprop_posrelh:
	    sd->sdXReference= val;
	    break;
	case DSHPprop_posv:
	    sd->sdYPosition= val;
	    break;
	case DSHPprop_posrelv:
	    sd->sdYReference= val;
	    break;

	case DSHPprop_fUseShapeAnchor:
	    sd->sd_fUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fLayoutInCell:
	    sd->sd_fLayoutInCell= val != 0;
	    break;
	case DSHPprop_fAllowOverlap:
	    sd->sd_fAllowOverlap= val != 0;
	    break;
	case DSHPprop_fChangePage:
	    sd->sd_fChangePage= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpTypeNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_shapeType:
	    if  ( rcw->rcwID != DSHPprop_shapeType )
		{ SDEB(docShapeTypeString(val));	}

	    sd->sdShapeType= val;
	    break;

	case DSHPprop_rotation:
	    sd->sdRotation= val;
	    break;

	case DSHPprop_dxWrapDistLeft:
	    sd->sdWrapDistLeftEmu= val;
	    break;
	case DSHPprop_dyWrapDistTop:
	    sd->sdWrapDistTopEmu= val;
	    break;
	case DSHPprop_dxWrapDistRight:
	    sd->sdWrapDistRightEmu= val;
	    break;
	case DSHPprop_dyWrapDistBottom:
	    sd->sdWrapDistBottomEmu= val;
	    break;

	case DSHPprop_hspMaster:
	    sd->sdMasterShape= val;
	    break;
	case DSHPprop_hspNext:
	    sd->sdNextShape= val;
	    break;

	case DSHPprop_xLimo:
	    sd->sdXLimo= val;
	    break;
	case DSHPprop_yLimo:
	    sd->sdXLimo= val;
	    break;

	case DSHPprop_fIsBullet:
	    sd->sd_fIsBullet= val != 0;
	    break;
	case DSHPprop_fFlipV:
	    sd->sd_fFlipV= val != 0;
	    break;
	case DSHPprop_fFlipH:
	    sd->sd_fFlipH= val != 0;
	    break;
	case DSHPprop_fBehindDocument:
	    sd->sd_fBehindDocument= val != 0;
	    break;
	case DSHPprop_fIsButton:
	    sd->sd_fIsButton= val != 0;
	    break;
	case DSHPprop_fHidden:
	    sd->sd_fHidden= val != 0;
	    break;
	case DSHPprop_fReallyHidden:
	    sd->sd_fReallyHidden= val != 0;
	    break;
	case DSHPprop_fArrowheadsOK:
	    sd->sd_fArrowheadsOK= val != 0;
	    break;
	case DSHPprop_fBackground:
	    sd->sd_fBackground= val != 0;
	    break;
	case DSHPprop_fDeleteAttachedObject:
	    sd->sd_fDeleteAttachedObject= val != 0;
	    break;
	case DSHPprop_fEditedWrap:
	    sd->sd_fEditedWrap= val != 0;
	    break;
	case DSHPprop_fHitTestFill:
	    sd->sd_fHitTestFill= val != 0;
	    break;
	case DSHPprop_fHitTestLine:
	    sd->sd_fHitTestLine= val != 0;
	    break;
	case DSHPprop_fInitiator:
	    sd->sd_fInitiator= val != 0;
	    break;
	case DSHPprop_fNoFillHitTest:
	    sd->sd_fNoFillHitTest= val != 0;
	    break;
	case DSHPprop_fNoHitTestPicture:
	    sd->sd_fNoHitTestPicture= val != 0;
	    break;
	case DSHPprop_fNoLineDrawDash:
	    sd->sd_fNoLineDrawDash= val != 0;
	    break;
	case DSHPprop_fOleIcon:
	    sd->sd_fOleIcon= val != 0;
	    break;
	case DSHPprop_fOnDblClickNotify:
	    sd->sd_fOnDblClickNotify= val != 0;
	    break;
	case DSHPprop_fOneD:
	    sd->sd_fOneD= val != 0;
	    break;
	case DSHPprop_fPreferRelativeSize:
	    sd->sd_fPreferRelativeSize= val != 0;
	    break;
	case DSHPprop_fPrint:
	    sd->sd_fPrint= val != 0;
	    break;

	case DSHPprop_fPseudoInline:
	    sd->sd_fPseudoInline= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpLockNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_fLockRotation:
	    sd->sd_fLockRotation= val != 0;
	    break;
	case DSHPprop_fLockAspectRatio:
	    sd->sd_fLockAspectRatio= val != 0;
	    break;
	case DSHPprop_fLockAgainstSelect:
	    sd->sd_fLockAgainstSelect= val != 0;
	    break;
	case DSHPprop_fLockCropping:
	    sd->sd_fLockCropping= val != 0;
	    break;
	case DSHPprop_fLockVerticies:
	    sd->sd_fLockVerticies= val != 0;
	    break;
	case DSHPprop_fLockText:
	    sd->sd_fLockText= val != 0;
	    break;
	case DSHPprop_fLockAdjustHandles:
	    sd->sd_fLockAdjustHandles= val != 0;
	    break;
	case DSHPprop_fLockAgainstGrouping:
	    sd->sd_fLockAgainstGrouping= val != 0;
	    break;
	case DSHPprop_fLockShapeType:
	    sd->sd_fLockShapeType= val != 0;
	    break;

	case DSHPprop_fLockPosition:
	    sd->sd_fLockPosition= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpFillNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_fillType:
	    sd->sdFillType= val;
	    break;
	case DSHPprop_fillOpacity:
	    sd->sdFillOpacity= val;
	    break;
	case DSHPprop_fillBackOpacity:
	    sd->sdFillBackOpacity= val;
	    break;

	case DSHPprop_fillblipflags:
	    sd->sdFillblipflags= val;
	    break;

	case DSHPprop_fillWidth:
	    sd->sdFillWidthEmu= val;
	    break;
	case DSHPprop_fillHeight:
	    sd->sdFillHeightEmu= val;
	    break;
	case DSHPprop_fillAngle:
	    sd->sdFillAngle= val;
	    break;
	case DSHPprop_fillFocus:
	    sd->sdFillFocus= val;
	    break;

	case DSHPprop_fillToLeft:
	    sd->sdFillToLeft= val;
	    break;
	case DSHPprop_fillToTop:
	    sd->sdFillToTop= val;
	    break;
	case DSHPprop_fillToRight:
	    sd->sdFillToRight= val;
	    break;
	case DSHPprop_fillToBottom:
	    sd->sdFillToBottom= val;
	    break;

	case DSHPprop_fillOriginX:
	    sd->sdFillOriginX= val;
	    break;
	case DSHPprop_fillOriginY:
	    sd->sdFillOriginY= val;
	    break;
	case DSHPprop_fillShapeOriginX:
	    sd->sdFillShapeOriginX= val;
	    break;
	case DSHPprop_fillShapeOriginY:
	    sd->sdFillShapeOriginY= val;
	    break;

	case DSHPprop_fillDztype:
	    sd->sdFillDztype= val;
	    break;

	case DSHPprop_fillRectLeft:
	    sd->sdFillRectLeftEmu= val;
	    break;
	case DSHPprop_fillRectTop:
	    sd->sdFillRectTopEmu= val;
	    break;
	case DSHPprop_fillRectRight:
	    sd->sdFillRectRightEmu= val;
	    break;
	case DSHPprop_fillRectBottom:
	    sd->sdFillRectBottomEmu= val;
	    break;

	case DSHPprop_fillShadePreset:
	    sd->sdFillShadePreset= val;
	    break;
	case DSHPprop_fillShadeType:
	    sd->sdFillShadeType= val;
	    break;

	case DSHPprop_fFilled:
	    sd->sd_fFilled= val != 0;
	    break;
	case DSHPprop_fillShape:
	    sd->sd_fillShape= val != 0;
	    break;
	case DSHPprop_fillUseRect:
	    sd->sd_fillUseRect= val != 0;
	    break;
	case DSHPprop_fFillOK:
	    sd->sd_fFillOK= val != 0;
	    break;
	case DSHPprop_fFillShadeShapeOK:
	    sd->sd_fFillShadeShapeOK= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpLineNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_lineType:
	    sd->sdLineType= val;
	    break;

	case DSHPprop_lineFillBlipFlags:
	    sd->sdLineFillBlipFlags= val;
	    break;

	case DSHPprop_lineFillWidth:
	    sd->sdLineFillWidthEmu= val;
	    break;
	case DSHPprop_lineFillHeight:
	    sd->sdLineFillHeightEmu= val;
	    break;
	case DSHPprop_lineWidth:
	    sd->sdLineWidthEmu= val;
	    break;

	case DSHPprop_lineStyle:
	    sd->sdLineStyle= val;
	    break;
	case DSHPprop_lineDashing:
	    sd->sdLineDashing= val;
	    break;
	case DSHPprop_lineStartArrowhead:
	    sd->sdLineStartArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineEndArrowhead:
	    sd->sdLineEndArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineStartArrowWidth:
	    sd->sdLineStartArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineStartArrowLength:
	    sd->sdLineStartArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndArrowWidth:
	    sd->sdLineEndArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineEndArrowLength:
	    sd->sdLineEndArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndCapStyle:
	    sd->sdLineEndCapStyle= val;
	    break;
	case DSHPprop_lineFillDztype:
	    sd->sdLineFillDztype= val;
	    break;
	case DSHPprop_lineJoinStyle:
	    sd->sdLineJoinStyle= val;
	    break;

	case DSHPprop_lineMiterLimit:
	    sd->sdLineMiterLimit= val;
	    break;

	case DSHPprop_fLine:
	    sd->sd_fLine= val != 0;
	    break;
	case DSHPprop_fLineOK:
	    sd->sd_fLineOK= val != 0;
	    break;

	case DSHPprop_fLineUseShapeAnchor:
	    sd->sd_fLineUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fColumnLineOK:
	    sd->sd_fColumnLineOK= val != 0;
	    break;
	case DSHPprop_fColumnLine:
	    sd->sd_fColumnLine= val != 0;
	    break;
	case DSHPprop_fLeftLine:
	    sd->sd_fLeftLine= val != 0;
	    break;
	case DSHPprop_fTopLine:
	    sd->sd_fTopLine= val != 0;
	    break;
	case DSHPprop_fRightLine:
	    sd->sd_fRightLine= val != 0;
	    break;
	case DSHPprop_fBottomLine:
	    sd->sd_fBottomLine= val != 0;
	    break;

	case DSHPprop_fColumnHitTestLine:
	    sd->sd_fColumnHitTestLine= val != 0;
	    break;
	case DSHPprop_fLeftHitTestLine:
	    sd->sd_fLeftHitTestLine= val != 0;
	    break;
	case DSHPprop_fTopHitTestLine:
	    sd->sd_fTopHitTestLine= val != 0;
	    break;
	case DSHPprop_fRightHitTestLine:
	    sd->sd_fRightHitTestLine= val != 0;
	    break;
	case DSHPprop_fBottomHitTestLine:
	    sd->sd_fBottomHitTestLine= val != 0;
	    break;

	case DSHPprop_lineFillShape:
	    sd->sd_lineFillShape= val != 0;
	    break;
	case DSHPprop_lineColumnFillShape:
	    sd->sd_lineColumnFillShape= val != 0;
	    break;
	case DSHPprop_lineLeftFillShape:
	    sd->sd_lineLeftFillShape= val != 0;
	    break;
	case DSHPprop_lineTopFillShape:
	    sd->sd_lineTopFillShape= val != 0;
	    break;
	case DSHPprop_lineRightFillShape:
	    sd->sd_lineRightFillShape= val != 0;
	    break;
	case DSHPprop_lineBottomFillShape:
	    sd->sd_lineBottomFillShape= val != 0;
	    break;

	case DSHPprop_fInsetPen:
	    sd->sd_fInsetPen= val != 0;
	    break;
	case DSHPprop_fLeftInsetPen:
	    sd->sd_fLeftInsetPen= val != 0;
	    break;
	case DSHPprop_fTopInsetPen:
	    sd->sd_fTopInsetPen= val != 0;
	    break;
	case DSHPprop_fRightInsetPen:
	    sd->sd_fRightInsetPen= val != 0;
	    break;
	case DSHPprop_fBottomInsetPen:
	    sd->sd_fBottomInsetPen= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpGroupedNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_relLeft:
	    sd->sdRelRect.drX0= val;
	    break;
	case DSHPprop_relTop:
	    sd->sdRelRect.drY0= val;
	    break;
	case DSHPprop_relRight:
	    sd->sdRelRect.drX1= val;
	    break;
	case DSHPprop_relBottom:
	    sd->sdRelRect.drY1= val;
	    break;

	case DSHPprop_relRotation:
	    sd->sdRelRotation= val;
	    break;
	case DSHPprop_lidRegroup:
	    sd->sdRegroupID= val;
	    break;

	case DSHPprop_groupLeft:
	    sd->sdGroupRect.drX0= val;
	    break;
	case DSHPprop_groupTop:
	    sd->sdGroupRect.drY0= val;
	    break;
	case DSHPprop_groupRight:
	    sd->sdGroupRect.drX1= val;
	    break;
	case DSHPprop_groupBottom:
	    sd->sdGroupRect.drY1= val;
	    break;

	case DSHPprop_fRelChangePage:
	    sd->sd_fRelChangePage= val != 0;
	    break;
	case DSHPprop_fRelFlipH:
	    sd->sd_fRelFlipH= val != 0;
	    break;
	case DSHPprop_fRelFlipV:
	    sd->sd_fRelFlipV= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpShadowNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_shadowType:
	    sd->sdShadowType= val;
	    break;
	case DSHPprop_shadowOpacity:
	    sd->sdShadowOpacity= val;
	    break;

	case DSHPprop_shadowOffsetX:
	    sd->sdShadowOffsetXEmu= val;
	    break;
	case DSHPprop_shadowOffsetY:
	    sd->sdShadowOffsetYEmu= val;
	    break;

	case DSHPprop_shadowSecondOffsetX:
	    sd->sdShadowOffset2XEmu= val;
	    break;
	case DSHPprop_shadowSecondOffsetY:
	    sd->sdShadowOffset2YEmu= val;
	    break;

	case DSHPprop_shadowScaleXToX:
	    sd->sdShadowScaleXToX= val;
	    break;
	case DSHPprop_shadowScaleYToX:
	    sd->sdShadowScaleYToX= val;
	    break;
	case DSHPprop_shadowScaleXToY:
	    sd->sdShadowScaleXToY= val;
	    break;
	case DSHPprop_shadowScaleYToY:
	    sd->sdShadowScaleYToY= val;
	    break;

	case DSHPprop_shadowPerspectiveX:
	    sd->sdShadowPerspectiveX= val;
	    break;
	case DSHPprop_shadowPerspectiveY:
	    sd->sdShadowPerspectiveY= val;
	    break;

	case DSHPprop_shadowWeight:
	    sd->sdShadowWeight= val;
	    break;

	case DSHPprop_shadowOriginX:
	    sd->sdShadowOriginX= val;
	    break;
	case DSHPprop_shadowOriginY:
	    sd->sdShadowOriginY= val;
	    break;

	case DSHPprop_fShadow:
	    sd->sd_fShadow= val != 0;
	    break;
	case DSHPprop_fshadowObscured:
	    sd->sd_fshadowObscured= val != 0;
	    break;
	case DSHPprop_fShadowOK:
	    sd->sd_fshadowOK= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpConnectNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_cxk:
	    sd->sdConnectionSite= val;
	    break;
	case DSHPprop_cxstyle:
	    sd->sdConnectionStyle= val;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpGeometryNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_geoLeft:
	    sd->sdGeoRect.drX0= val;
	    break;
	case DSHPprop_geoTop:
	    sd->sdGeoRect.drY0= val;
	    break;
	case DSHPprop_geoRight:
	    sd->sdGeoRect.drX1= val;
	    break;
	case DSHPprop_geoBottom:
	    sd->sdGeoRect.drY1= val;
	    break;

	case DSHPprop_adjustValue:
	    sd->sdAdjustValue= val;
	    break;
	case DSHPprop_adjust2Value:
	    sd->sdAdjust2Value= val;
	    break;
	case DSHPprop_adjust3Value:
	    sd->sdAdjust3Value= val;
	    break;
	case DSHPprop_adjust4Value:
	    sd->sdAdjust4Value= val;
	    break;
	case DSHPprop_adjust5Value:
	    sd->sdAdjust5Value= val;
	    break;
	case DSHPprop_adjust6Value:
	    sd->sdAdjust6Value= val;
	    break;
	case DSHPprop_adjust7Value:
	    sd->sdAdjust7Value= val;
	    break;
	case DSHPprop_adjust8Value:
	    sd->sdAdjust8Value= val;
	    break;
	case DSHPprop_adjust9Value:
	    sd->sdAdjust9Value= val;
	    break;
	case DSHPprop_adjust10Value:
	    sd->sdAdjust10Value= val;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpTxboxNumber(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_dxTextLeft:
	    sd->sd_dxTextLeft= val;
	    break;
	case DSHPprop_dyTextTop:
	    sd->sd_dyTextTop= val;
	    break;
	case DSHPprop_dxTextRight:
	    sd->sd_dxTextRight= val;
	    break;
	case DSHPprop_dyTextBottom:
	    sd->sd_dyTextBottom= val;
	    break;

	case DSHPprop_scaleText:
	    sd->sd_scaleText= val;
	    break;
	case DSHPprop_lTxid:
	    sd->sd_lTxid= val;
	    break;

	case DSHPprop_WrapText:
	    sd->sd_WrapText= val;
	    break;
	case DSHPprop_anchorText:
	    sd->sd_anchorText= val;
	    break;
	case DSHPprop_txflTextFlow:
	    sd->sd_txflTextFlow= val;
	    break;
	case DSHPprop_cdirFont:
	    sd->sd_cdirFont= val;
	    break;

	case DSHPprop_fAutoTextMargin:
	    sd->sd_fAutoTextMargin= val != 0;
	    break;
	case DSHPprop_fRotateText:
	    sd->sd_fRotateText= val != 0;
	    break;
	case DSHPprop_fSelectText:
	    sd->sd_fSelectText= val != 0;
	    break;
	case DSHPprop_fFitShapeToText:
	    sd->sd_fFitShapeToText= val != 0;
	    break;
	case DSHPprop_fFitTextToShape:
	    sd->sd_fFitTextToShape= val != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShpNumber(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    switch( rcw->rcwID )
	{
	case DSHPprop_shapePath:
	    sd->sd_shapePath= val;
	    break;

	case DSHPprop_pictureActive:
	case DSHPprop_pictureId:
	    break;

	case DSHPprop_cropFromLeft:
	    sd->sd_cropFromLeft= val;
	    break;
	case DSHPprop_cropFromRight:
	    sd->sd_cropFromRight= val;
	    break;
	case DSHPprop_cropFromTop:
	    sd->sd_cropFromTop= val;
	    break;
	case DSHPprop_cropFromBottom:
	    sd->sd_cropFromBottom= val;
	    break;

	default:
	    /*SDEB(rcw->rcwWord);*/
	    break;
	}

    return 0;
    }

int docRtfShpColor(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    DrawingShape *	ds= rrc->rrcDrawingShape;
    ShapeDrawing *	sd;
    long		val;
    RGB8Color		rgb8;

    if  ( docRtfShpGetNumber( &val, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( ! ds )
	{ SXDEB(rcw->rcwWord,ds); return 0;	}
    sd= &(ds->dsDrawing);

    utilInitRGB8Color( &rgb8 );
    rgb8.rgb8Red= val & 255; val /= 256;
    rgb8.rgb8Green= val & 255; val /= 256;
    rgb8.rgb8Blue= val & 255; val /= 256;

    switch( rcw->rcwID )
	{
	case DSHPprop_fillColor:
	    sd->sdFillColor= rgb8;
	    break;
	case DSHPprop_fillBackColor:
	    sd->sdFillBackColor= rgb8;
	    break;
	case DSHPprop_fillCrMod:
	    sd->sdFillCrModColor= rgb8;
	    break;

	case DSHPprop_lineColor:
	    sd->sdLineColor= rgb8;
	    break;
	case DSHPprop_lineBackColor:
	    sd->sdLineBackColor= rgb8;
	    break;
	case DSHPprop_lineCrMod:
	    sd->sdLineCrModColor= rgb8;
	    break;

	case DSHPprop_shadowColor:
	    sd->sdShadowColor= rgb8;
	    break;
	case DSHPprop_shadowHighlight:
	    sd->sdShadowHighlightColor= rgb8;
	    break;
	case DSHPprop_shadowCrMod:
	    sd->sdShadowCrModColor= rgb8;
	    break;
	case DSHPprop_c3DExtrusionColor:
	    sd->sdShadowc3DExtrusionColor= rgb8;
	    break;
	case DSHPprop_c3DCrMod:
	    sd->sdShadowc3DCrModColor= rgb8;
	    break;

	case DSHPprop_pictureTransparent:
	    /* ? */
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfShapeProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    SimpleInputStream *		sis= rrc->rrcSis;
    int				control;
    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				locArg= -1;
    int				c;

    int				i;

    const RtfControlWord *	rcwSn;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    control= docRtfFindControl( rrc, &c, controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sn" ) )
	{ SDEB(controlWord); return -1;	}

    i= 0;
    for (;;)
	{
	c= sioInGetByte( sis );
	if  ( c == EOF )
	    { LDEB(c); return -1;	}
	if  ( i >= TEDszRTFCONTROL )
	    {
	    LLDEB(i,TEDszRTFCONTROL);
	    controlWord[i]= '\0';
	    SDEB(controlWord); return -1;
	    }
	if  ( c == '}' )
	    { controlWord[i]= '\0'; break;	}

	controlWord[i++]= c;
	}

    rcwSn= docRtfFindShapePropertyWord( controlWord );
    if  ( ! rcwSn )
	{ SXDEB(controlWord,rcwSn);	}

    control= docRtfFindControl( rrc, &c,
					    controlWord, &gotArg, &locArg );
    if  ( control != RTFfiCTRLGROUP )
	{ SLDEB(rcw->rcwWord,control); return -1; }

    if  ( strcmp( controlWord, "sv" ) )
	{ SDEB(controlWord); return -1;	}

    if  ( rcwSn )
	{
	if  ( (*rcwSn->rcwApply)( rcwSn, 0, rrc ) )
	    {  SDEB(controlWord); return -1;	}

	c= sioInGetByte( sis );
	while( c == '\n' || c == '\r' )
	    {
	    if  ( c == '\n' )
		{ rrc->rrcCurrentLine++;	}
	    c= sioInGetByte( sis );
	    }
	
	if  ( c != '}' )
	    { CDEB(c); return -1;	}
	}
    else{
	if  ( docRtfSkipGroup( rcw, DOClevANY, rrc ) )
	    { LDEB(1); return -1;	}
	}

    c= sioInGetByte( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetByte( sis );
	}
    
    if  ( c != '}' )
	{ CDEB(c); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read shape instructions.						*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShpinstGroups[]=
    {
	RTF_DEST_XX( "shptxt",	DOCinSHPTXT,	docRtfShpText ),
	RTF_DEST_XX( "sp",	1,		docRtfShapeProperty ),
	RTF_DEST_XX( "shpgrp",	SHPtyGROUP,	docRtfReadChildShape ),
	RTF_DEST_XX( "shp",	SHPtyUNKNOWN,	docRtfReadChildShape ),

	{ (char *)0, 0, 0 }
    };

static int docRtfShpinst(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    res= docRtfReadGroup( rcw, 0, 0, rrc,
		    docRtfShpinstGroups, docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read the shape result and immediately discard it.			*/
/*									*/
/************************************************************************/

static int docRtfShprslt(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res= 0;

    BufferItem *	sectBi;
    SelectionScope	ss;
    int			treeType= DOCinSHPTXT;
    const int		ignoreEmpty= 0;

    DocumentTree	dt;

    rrc->rrcInIgnoredGroup++;

    docInitDocumentTree( &dt );
    docInitSelectionScope( &ss );

    sectBi= docGetSectNode( rrc->rrcNode );
    if  ( ! sectBi )
	{ XDEB(sectBi); res= -1; goto ready;	}

    ss.ssTreeType= DOCinSHPTXT;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= sectBi->biNumberInParent;

    if  ( docRtfReadDocumentTree( rcw, &dt, &treeType,
						rrc, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); res= -1; goto ready;	}

    docSetTreeTypeOfNode( dt.dtRoot, treeType );

  ready:

    docCleanDocumentTree( rrc->rrcDocument, &dt );
    rrc->rrcInIgnoredGroup--;

    return res;
    }

/************************************************************************/
/*									*/
/*  Read a shape or a drawing object that is translayed to a shape.	*/
/*									*/
/************************************************************************/

static int docRtfReadShapeIntern(	DrawingShape **		pDs,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc,
					const RtfControlWord *	shapeGroups )
    {
    int			res;
    DrawingShape *	parent= rrc->rrcDrawingShape;
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    if  ( ! rrc->rrcInIgnoredGroup )
	{
	const BufferItem *		bodySectBi;

	ds= docClaimDrawingShape( &(rrc->rrcDocument->bdShapeList) );
	if  ( ! ds )
	    { XDEB(ds); return -1;	}

	ds->dsDrawing.sdShapeType= rcw->rcwID;

	bodySectBi= docGetBodySectNode( rrc->rrcNode, rrc->rrcDocument );
	if  ( ! bodySectBi )
	    { XDEB(bodySectBi); return -1;	}

	ds->dsSelectionScope.ssTreeType= DOCinSHPTXT;
	ds->dsSelectionScope.ssSectNr= 0;
	ds->dsSelectionScope.ssOwnerSectNr= bodySectBi->biNumberInParent;
	ds->dsSelectionScope.ssOwnerNumber= ds->dsShapeNumber;

	rrc->rrcDrawingShape= ds;
	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
			shapeGroups, docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{
	SLDEB(rcw->rcwWord,res);
	if  ( ds )
	    { docDeleteDrawingShape( rrc->rrcDocument, ds );	}
	}
    else{
	if  ( ! rrc->rrcInIgnoredGroup )
	    {
	    BufferItem *	paraBi= rrc->rrcNode;
	    TextParticule *	tp;
	    RtfReadingState *	rrs= rrc->rrcState;

	    if  ( rrs->rrsTextShadingChanged )
		{ docRtfRefreshTextShading( rrc, rrs );	}

	    tp= docAppendObject( rrc->rrcDocument, paraBi,
						    &(rrs->rrsTextAttribute) );
	    if  ( ! tp )
		{
		LDEB(paraBi->biParaParticuleCount);
		docDeleteDrawingShape( rrc->rrcDocument, ds );
		res= -1;
		}
	    else{
		InsertedObject *	io;

		io= docGetObject( rrc->rrcDocument, tp->tpObjectNumber );
		if  ( ! io )
		    { LXDEB(tp->tpObjectNumber,io); res= -1;	}
		else{
		    io->ioKind= DOCokDRAWING_SHAPE;
		    io->ioDrawingShape= ds;
		    io->ioInline= 0;
		    }

		}
	    *pDs= ds;
	    }
	}

    rrc->rrcDrawingShape= parent;
    return res;
    }

/************************************************************************/
/*									*/
/*  The actual shape as opposed to the result in simpler RTF.		*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfShapeGroups[]=
    {
	RTF_DEST_XX( "shptxt",	DOCinSHPTXT,	docRtfShpText ),
	RTF_DEST_XX( "shprslt",	DOCinSHPTXT,	docRtfShprslt ),
	RTF_DEST_XX( "shpgrp",	SHPtyGROUP,	docRtfReadChildShape ),
	RTF_DEST_XX( "shp",	SHPtyUNKNOWN,	docRtfReadChildShape ),
	RTF_DEST_XX( "shpinst",	SHPtyUNKNOWN,	docRtfShpinst ),

	{ (char *)0, 0, 0, }
    };

static int docRtfReadChildShape(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    int			res;
    DrawingShape *	parent= rrc->rrcDrawingShape;
    DrawingShape *	ds;

    if  ( ! rrc->rrcInIgnoredGroup )
	{
	ds= docClaimShapeInParent( &(rrc->rrcDocument->bdShapeList),
						    parent, -1, rcw->rcwID );
	if  ( ! ds )
	    { XDEB(ds); return -1;	}

	ds->dsDrawing.sdShapeType= rcw->rcwID;

	ds->dsSelectionScope= parent->dsSelectionScope;
	ds->dsSelectionScope.ssOwnerNumber= ds->dsShapeNumber;

	rrc->rrcDrawingShape= ds;
	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
		    docRtfShapeGroups, docRtfIgnoreText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcDrawingShape= parent;
    return res;
    }

int docRtfReadShape(			const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    return docRtfReadShapeIntern( &ds, rcw, arg, rrc, docRtfShapeGroups );
    }

/************************************************************************/
/*									*/
/*  Make fixes to a shape that originates from reading a drawing	*/
/*  object.								*/
/*									*/
/*  1)  Word only uses the vertex in a drawing object to determine	*/
/*	what diagonal to draw.						*/
/*  2)  Word swaps the arrow heads. Why?				*/
/*									*/
/************************************************************************/

static int docRtfDrawingObjectToShape(	DrawingShape *	ds )
    {
    ShapeDrawing *		sd= &(ds->dsDrawing);

    if  ( ds->dsDrawing.sdShapeType == SHPtyLINE	&&
	  ! ds->dsIsChildShape				&&
	  sd->sdVertexCount == 2			)
	{
	ShapeArrow		sa;

	/*  1  */
	if  ( sd->sdVertices[0].svX <= sd->sdVertices[1].svX )
	    { sd->sd_fFlipH= 0;	}
	else{ sd->sd_fFlipH= 1;	}
	if  ( sd->sdVertices[0].svY <= sd->sdVertices[1].svY )
	    { sd->sd_fFlipV= 0;	}
	else{ sd->sd_fFlipV= 1;	}

	/*  2  */
	sa= sd->sdLineStartArrow;
	sd->sdLineStartArrow= sd->sdLineEndArrow;
	sd->sdLineEndArrow= sa;
	}

    return 0;
    }

static RtfControlWord	docRtfDoGroups[]=
    {
	RTF_DEST_XX( "dptxbxtext",	DOCinSHPTXT,	docRtfShpText ),

	{ (char *)0, 0, 0 }
    };

int docRtfReadDrawingObject(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    DrawingShape *	ds= (DrawingShape *)0;

    if  ( docRtfReadShapeIntern( &ds, rcw, arg, rrc, docRtfDoGroups ) )
	{ SDEB(rcw->rcwWord);	}

    if  ( ! rrc->rrcInIgnoredGroup )
	{
	if  ( docRtfDrawingObjectToShape( ds ) )
	    { LDEB(1);	}
	}

    return 0;
    }

