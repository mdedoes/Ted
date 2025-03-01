/************************************************************************/
/*									*/
/*  Save the fields in a BufferDocument to an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	"docRtfWriterImpl.h"
#   include	<docBookmarkField.h>
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFields.h>
#   include	<docIncludePictureField.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

static void docRtfWriteStartField(	RtfWriter *		rw,
					const DocumentField *	df );

/************************************************************************/
/*									*/
/*  Finish/Begin writing a field.					*/
/*									*/
/************************************************************************/

static void docRtfFinishFldrslt(	RtfWriter *	rw )
    {
    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );

    rw->rwcInFldrslt--;

    if  ( docRtfPopAttribute( rw ) )
	{ LDEB(1);	}

    return;
    }

static void docRtfWriteFieldComponent(	RtfWriter *			rw,
					int *				pChild,
					const DocumentField *		df,
					int				comp )
    {
    int					child= *pChild;
    int					from= 0;
    const InstructionsComponent *	ic;

    ic= df->dfInstructions.fiComponents+ comp;
    from= 0;

    if  ( ic->icType == INSTRtypeQUOTED_VALUE )
	{ docRtfWriteDocEncodedString( rw, "\"", 1 );	}

    while( child < df->dfInstructionFields.cfChildCount )
	{
	const DocumentField *	dfCh= df->dfInstructionFields.cfChildren[child];

	if  ( dfCh->dfHeadPosition.epParaNr < comp )
	    { child++; continue;	}
	if  ( dfCh->dfHeadPosition.epParaNr > comp )
	    { break;	}

	docRtfWriteDocEncodedString( rw,
				(const char *)ic->icBuffer.mbBytes+ from,
				dfCh->dfHeadPosition.epStroff- from );

	docRtfWriteStartField( rw, dfCh );
	docRtfWriteDestinationEnd( rw );

	from= dfCh->dfTailPosition.epStroff;
	child++;
	}

    docRtfWriteDocEncodedString( rw,
				(const char *)ic->icBuffer.mbBytes+ from,
				ic->icBuffer.mbSize- from );

    if  ( ic->icType == INSTRtypeQUOTED_VALUE )
	{ docRtfWriteDocEncodedString( rw, "\"", 1 );	}

    *pChild= child;
    return;
    }

static void docRtfWriteFieldInstructions( RtfWriter *			rw,
					const DocumentField *		df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    int					byteCount= 0;
    int					comp;
    int					child= 0;

    for ( comp= 0; comp < fi->fiComponentCount; comp++ )
	{
	const InstructionsComponent *	ic;

	ic= df->dfInstructions.fiComponents+ comp;

	if  ( ic->icType == INSTRtypeQUOTED_VALUE )
	    { byteCount += 2;	}
	ic++;
	}

    if  ( fi->fiComponentCount > 1 )
	{ byteCount += fi->fiComponentCount- 1;	}

    docRtfReserveColumns( rw, byteCount );

    if  ( fi->fiComponentCount > 0 )
	{ docRtfWriteFieldComponent( rw, &child, df, 0 );	}

    for ( comp= 1; comp < fi->fiComponentCount; comp++ )
	{
	docRtfWriteDocEncodedString( rw, " ", 1 );
	docRtfWriteFieldComponent( rw, &child, df, comp );
	}

    return;
    }

static void docRtfWriteStartField(	RtfWriter *		rw,
					const DocumentField *	df )
    {
    docRtfWriteDestinationBegin( rw, "field" );
    docRtfWriteDestinationBegin( rw, "*\\fldinst" );
    docRtfWriteDestinationBegin( rw, "" );

    docRtfWriteFieldInstructions( rw, df );

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );

    return;
    }

static void docRtfWriteStartFieldPRes(	RtfWriter *		rw,
					const DocumentField *	df )
    {
    docRtfWriteStartField( rw, df );

    docRtfWriteDestinationBegin( rw, "fldrslt" );
    docRtfWriteDestinationBegin( rw, "" );

    rw->rwcInFldrslt++;

    if  ( docRtfPushAttribute( rw ) )
	{ LDEB(1);	}

    return;
    }

static void docRtfBookmarkTag(	RtfWriter *		rw,
				const char *		tag,
				const DocumentField *	df )
    {
    const int			addSemicolon= 0;

    const MemoryBuffer *	mbBookmark;

    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	{
	docRtfWriteDocEncodedBufferDestination( rw, tag, mbBookmark,
								addSemicolon );
	}

    return;
    }

static void docRtfStartTc(	RtfWriter *		rw,
				const DocumentField *	df )
    {
    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    docRtfWriteDestinationBegin( rw, fki->fkiRtfTag );

    docRtfWriteFieldInstructions( rw, df );

    /* The field instructions actually contain tags */
    rw->rwAfter= RTFafterTAG;

    return;
    }

/************************************************************************/
/*									*/
/*  Save a note.							*/
/*									*/
/************************************************************************/

static int docRtfSaveNoteField(	RtfWriter *			rw,
				int *				pCount,
				const struct BufferItem *	paraNode,
				const int			part,
				const TextParticule *		tp,
				const DocumentField *		df )
    {
    DocumentNote *		dn;
    const NoteProperties *	np;
    int				count;
    int				closed;

    const char *		first= (const char *)0;
    int				attrNr= -1;
    const int			paralen= docParaStrlen( paraNode );
    int				stroff= paralen;

    count= docCountParticulesInFieldFwd( paraNode, &closed, part,
					    paraNode->biParaParticuleCount );

    /*  Prefer the attributes of the field result */
    if  ( count > 0 && tp[1].tpKind == TPkindSPAN )
	{
	stroff= tp[1].tpStroff;
	attrNr= tp[1].tpTextAttrNr;
	}
    else{
	stroff= tp[0].tpStroff+ tp[0].tpStrlen;
	attrNr= tp[0].tpTextAttrNr;
	}

    if  ( stroff < paralen )
	{ first= docParaString( paraNode, stroff );	}

    docRtfWriteSwitchTextAttributes( rw, attrNr, first );

    dn= docGetNoteOfField( df, rw->rwDocument );
    if  ( ! dn )
	{ XDEB(dn); return -1;	}
    np= &(dn->dnNoteProperties);

    if  ( np->npAutoNumber )
	{
	docRtfWriteTag( rw, RTFtag_chftn );

	if  ( paraNode->biTreeType == DOCinBODY			&&
	      rw->rwTextAttribute.taSuperSub != TEXTvaREGULAR	)
	    {
	    docRtfWriteTag( rw, RTFtag_nosupersub );
	    rw->rwTextAttribute.taSuperSub= TEXTvaREGULAR;
	    }
	}
    else{
	docRtfWriteFontEncodedString( rw,
				(const char *)np->npFixedText.mbBytes,
				np->npFixedText.mbSize );
	}

    if  ( df->dfNoteIndex >= 0 && dn->dnDocumentTree.dtRoot )
	{
	const int		evenIfAbsent= 0;
	const int		forcePar= 0;

	if  ( paraNode->biTreeType == DOCinFOOTNOTE	||
	      paraNode->biTreeType == DOCinENDNOTE	)
	    { SDEB(docTreeTypeStr(paraNode->biTreeType));	}
	else{
	    switch( np->npTreeType )
		{
		case DOCinFOOTNOTE:
		    if  ( docRtfSaveDocumentTree( rw, RTFtag_footnote,
			    &(dn->dnDocumentTree), evenIfAbsent, forcePar ) )
			{ LDEB(np->npTreeType); return -1;	}
		    break;

		case DOCinENDNOTE:
		    if  ( docRtfSaveDocumentTree( rw, "footnote\\ftnalt",
			    &(dn->dnDocumentTree), evenIfAbsent, forcePar ) )
			{ LDEB(np->npTreeType); return -1;	}
		    break;

		default:
		    LDEB(np->npTreeType);
		    break;
		}
	    }
	}

    *pCount= count;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a NeXTGraphic field.						*/
/*									*/
/************************************************************************/

static int docRtfSaveNeXTGraphicField(	RtfWriter *		rw,
					const DocumentField *	df )
    {
    int			rval= 0;
    IncludePictureField	ipf;

    docInitIncludePictureField( &ipf );

    docRtfWriteDestinationBegin( rw, "" );
    docRtfWriteDestinationBegin( rw, "NeXTGraphic" );

    if  ( docGetIncludePictureField( &ipf, df ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfWriteDocEncodedString( rw,
		(char *)ipf.ipfFilename.mbBytes, ipf.ipfFilename.mbSize );
    docRtfWriteDocEncodedString( rw, " ", 1 );

    if  ( ipf.ipfTwipsWide > 0 )
	{ docRtfWriteArgTag( rw, "width", ipf.ipfTwipsWide );	}

    if  ( ipf.ipfTwipsHigh > 0 )
	{ docRtfWriteArgTag( rw, "height", ipf.ipfTwipsHigh );	}

  ready:

    docCleanIncludePictureField( &ipf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save a field head particule						*/
/*									*/
/************************************************************************/

int docRtfSaveFieldHead(	RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				int				part )
    {
    const DocumentField *		df;
    const FieldKindInformation *	fki;
    const TextParticule *		tp= paraNode->biParaParticules+ part;

    df= docGetFieldByNumber( rw->rwDocument, tp->tpObjectNumber );
    if  ( ! df )
	{ LPDEB(tp->tpObjectNumber,df); return 0;	}

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( fki->fkiIsFieldInRtf ||
	  ( df->dfKind == DOCfkUNKNOWN && df->dfInstructions.fiComponentCount > 0 ) )
	{
	if  ( df->dfKind != DOCfkMERGEFIELD				||
	      ! ( rw->rwSaveFlags & RTFflagNO_MERGEFIELDS )		)
	    { docRtfWriteStartFieldPRes( rw, df );	}
	}

    if  ( df->dfKind == DOCfkBOOKMARK			&&
	  ! ( rw->rwSaveFlags & RTFflagNO_BOOKMARKS )	)
	{ docRtfBookmarkTag( rw, "*\\bkmkstart", df );	}

    if  ( df->dfKind == DOCfkTC || df->dfKind == DOCfkTCN )
	{ docRtfStartTc( rw, df ); }

    if  ( df->dfKind == DOCfkCHFTN )
	{
	int		count= 0;

	if  ( docRtfSaveNoteField( rw, &count, paraNode, part, tp, df ) )
	    { LDEB(df->dfKind); return -1;	}

	return count;
	}

    if  ( df->dfKind == DOCfkNEXTGRAPHIC )
	{
	if  ( docRtfSaveNeXTGraphicField( rw, df ) )
	    { LDEB(df->dfKind); return -1;	}

	return 0;
	}

    if  ( df->dfKind == DOCfkLISTTEXT				&&
	  paraNode->biParaProperties->ppListOverride > 0	)
	{
	if  ( docRtfPushAttribute( rw ) )
	    { LDEB(1);	}

	docRtfWriteDestinationBegin( rw, "listtext" );
	}

    if  ( fki->fkiIsDestinationInRtf	&&
	  fki->fkiRtfTag		)
	{
	docRtfWriteDestinationBegin( rw, fki->fkiRtfTag );

	docRtfWriteDocEncodedString( rw,
		(char *)df->dfData.mbBytes, df->dfData.mbSize );

	docRtfWriteDestinationBegin( rw, "" );

	docRtfWriteFieldInstructions( rw, df );

	docRtfWriteDestinationEnd( rw );
	docRtfWriteDestinationEnd( rw );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a field tail particule						*/
/*									*/
/************************************************************************/

int docRtfSaveFieldTail(	RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				int				part )
    {
    const DocumentField *		df;
    const FieldKindInformation *	fki;
    const TextParticule *		tp= paraNode->biParaParticules+ part;

    if  ( tp->tpObjectNumber < 0 )
	{ LDEB(tp->tpObjectNumber); return 0; }

    df= docGetFieldByNumber( rw->rwDocument, tp->tpObjectNumber );
    if  ( ! df )
	{ LPDEB(tp->tpObjectNumber,df); return 0;	}

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( df->dfKind == DOCfkBOOKMARK			&&
	  ! ( rw->rwSaveFlags & RTFflagNO_BOOKMARKS )	)
	{ docRtfBookmarkTag( rw, "*\\bkmkend", df ); }

    if  ( df->dfKind == DOCfkTC || df->dfKind == DOCfkTCN )
	{ docRtfWriteDestinationEnd( rw ); }

    if  ( fki->fkiIsFieldInRtf ||
	  ( df->dfKind == DOCfkUNKNOWN && df->dfInstructions.fiComponentCount > 0 ) )
	{
	if  ( df->dfKind != DOCfkMERGEFIELD		    	||
	      ! ( rw->rwSaveFlags & RTFflagNO_MERGEFIELDS )	)
	    {
	    if  ( rw->rwcInFldrslt )
		{ docRtfFinishFldrslt( rw );	}
	    }
	}

    if  ( df->dfKind == DOCfkLISTTEXT				&&
	  paraNode->biParaProperties->ppListOverride > 0	)
	{
	docRtfWriteDestinationEnd( rw );

	if  ( docRtfPopAttribute( rw ) )
	    { LDEB(1);	}
	}

    if  ( df->dfKind == DOCfkNEXTGRAPHIC )
	{
	docRtfWriteDestinationEnd( rw );

	if  ( docRtfPutByte( 0xac, rw ) < 0 )
	    { LDEB(1); return -1;	}

	docRtfWriteDestinationEnd( rw );
	return 0;
	}

    return 0;
    }

