/************************************************************************/
/*									*/
/*  Trace modifications to a document.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioFd.h>

#   include	"docRtfTrace.h"
#   include	"docEditCommand.h"
#   include	"docEditStep.h"
#   include	"docEditTrace.h"

#   include	<docRtfTraceImpl.h>
#   include	<docRtfReaderImpl.h>
#   include	<docRtfFindProperty.h>
#   include	<docTreeType.h>
#   include	<docRtfTags.h>
#   include	<docRtfFlags.h>
#   include	<sioGeneral.h>
#   include	<docAttributes.h>
#   include	<docBuf.h>
#   include	<docRtfReadTreeStack.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docRtfReadVersion(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int		res;

    if  ( rcw->rcwID == rr->rrTraceReadWhat )
	{
	RtfTreeStack *	rts= rr->rrTreeStack;

	if  ( rr->rrTraceCommand != EDITcmdUPD_NOTE )
	    { rts->rtsSelectionScope.ssTreeType= DOCinBODY;	}

	utilPropMaskClear( &(rr->rrPicturePropMask) );
	docInitPictureProperties( &(rr->rrPictureProperties) );

	res= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfDocumentGroups,
				docRtfGotText,
				(RtfCommitGroup)0 );
				/* Why not docRtfFinishCurrentTree ? */
	if  ( res )
	    { SLDEB(rcw->rcwWord,res);	}
	}
    else{
	res= docRtfSkipGroup( rcw, arg, rr );
	if  ( res )
	    { SLDEB(rcw->rcwWord,res);	}
	}

    return res;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRtfCommitNewProps(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    if  ( docRtfStoreStyleProperties( rr ) )
	{ LDEB(1);	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRtfReadNewProps(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    int			res;

    docCleanDocumentStyle( &(rr->rrStyle) );
    docInitDocumentStyle( &(rr->rrStyle) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rr->rrDocument );

    rr->rrTraceInProps++;

    res= docRtfReadGroup( rcw, 0, 0, rr,
			    docRtfDocumentGroups,
			    docRtfRefuseText,
			    docRtfCommitNewProps );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rr->rrTraceInProps--;

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rr->rrDocument );

    return res;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

static RtfControlWord	docRtfTraceDetailGroups[]=
{
    RTF_DEST_XX( RTFtag_NTX, RTCscopeANY, EDITversionNEW, docRtfReadVersion ),
    RTF_DEST_XX( RTFtag_NPR, RTCscopeANY, EDITversionNEW, docRtfReadNewProps ),
    RTF_DEST_XX( RTFtag_OTX, RTCscopeANY, EDITversionOLD, docRtfReadVersion ),

    { (char *)0, 0, 0 }
};

/************************************************************************/
/*									*/
/*  Read a trace step.							*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docRtfReadTraceStep(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;
    RtfTreeStack *	rts= rr->rrTreeStack;

    rr->rrTraceCommand= rcw->rcwID;

    if  ( rr->rrTraceCommand == EDITcmdUPD_NOTE )
	{ rts->rtsSelectionScope.ssTreeType= DOCinFOOTNOTE;	}

    res= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfTraceDetailGroups,
				(RtfGotText)0, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Read a trace item.							*/
/*									*/
/************************************************************************/

# define RTF_TRACE_CMD( word, id ) \
	    RTF_DEST_XX( (word), RTCscopeANY, (id), docRtfReadTraceStep )

static RtfControlWord	docRtfTraceGroups[]=
{
    RTF_TRACE_CMD( RTFtag_SLI,		EDITcmdSET_HYPERLINK ),
    RTF_TRACE_CMD( RTFtag_SBK,		EDITcmdSET_BOOKMARK ),
    RTF_TRACE_CMD( RTFtag_DFL,		EDITcmdDEL_FIELD ),
    RTF_TRACE_CMD( RTFtag_UFL,		EDITcmdUPD_FIELD ),
    RTF_TRACE_CMD( RTFtag_ULI,		EDITcmdUPD_LIST ),
    RTF_TRACE_CMD( RTFtag_NLI,		EDITcmdSET_NEW_LIST ),
    RTF_TRACE_CMD( RTFtag_NNO,		EDITcmdINS_NOTE ),
    RTF_TRACE_CMD( RTFtag_UNO,		EDITcmdUPD_NOTE ),
    RTF_TRACE_CMD( RTFtag_UTA,		EDITcmdUPD_SPAN_PROPS ),
    RTF_TRACE_CMD( RTFtag_UPP,		EDITcmdUPD_PARA_PROPS ),
    RTF_TRACE_CMD( RTFtag_UTP,		EDITcmdUPD_TABLE_PROPS ),
    RTF_TRACE_CMD( RTFtag_URP,		EDITcmdUPD_ROW_PROPS ),
    RTF_TRACE_CMD( RTFtag_UCP,		EDITcmdUPD_COLUMN_PROPS ),
    RTF_TRACE_CMD( RTFtag_UcP,		EDITcmdUPD_CELL_PROPS ),
    RTF_TRACE_CMD( RTFtag_USP,		EDITcmdUPD_SECT_PROPS ),
    RTF_TRACE_CMD( RTFtag_USDP,		EDITcmdUPD_SECTDOC_PROPS ),
    RTF_TRACE_CMD( RTFtag_UDP,		EDITcmdUPD_DOC_PROPS ),
    RTF_TRACE_CMD( RTFtag_REP,		EDITcmdREPLACE ),
    RTF_TRACE_CMD( RTFtag_REPF,		EDITcmdREPLACE_FTNSEP ),
    RTF_TRACE_CMD( RTFtag_REPB,		EDITcmdREPLACE_BODY_LEVEL ),
    RTF_TRACE_CMD( RTFtag_DELS,		EDITcmdDELETE_SELECTION ),
    RTF_TRACE_CMD( RTFtag_DELF,		EDITcmdDELETE_SELECTION_DEL ),
    RTF_TRACE_CMD( RTFtag_DELB,		EDITcmdDELETE_SELECTION_BS ),
    RTF_TRACE_CMD( RTFtag_REX,		EDITcmdEXTEND_REPLACE ),
    RTF_TRACE_CMD( RTFtag_MPA,		EDITcmdMERGE_PARAS ),

    RTF_TRACE_CMD( RTFtag_DPA,		EDITcmdDELETE_PARA ),
    RTF_TRACE_CMD( RTFtag_DSE,		EDITcmdDELETE_SECT ),
    RTF_TRACE_CMD( RTFtag_DTB,		EDITcmdDELETE_TABLE ),
    RTF_TRACE_CMD( RTFtag_DRW,		EDITcmdDELETE_ROW ),
    RTF_TRACE_CMD( RTFtag_DCO,		EDITcmdDELETE_COLUMN ),

    RTF_TRACE_CMD( RTFtag_IPA,		EDITcmdINSERT_PARA ),
    RTF_TRACE_CMD( RTFtag_APA,		EDITcmdAPPEND_PARA ),
    RTF_TRACE_CMD( RTFtag_ITB,		EDITcmdINSERT_TABLE ),
    RTF_TRACE_CMD( RTFtag_ISE,		EDITcmdINSERT_SECT ),
    RTF_TRACE_CMD( RTFtag_ASE,		EDITcmdAPPEND_SECT ),
    RTF_TRACE_CMD( RTFtag_SSE,		EDITcmdSPLIT_SECT ),

    RTF_TRACE_CMD( RTFtag_IRW,		EDITcmdINSERT_ROW ),
    RTF_TRACE_CMD( RTFtag_ARW,		EDITcmdAPPEND_ROW ),
    RTF_TRACE_CMD( RTFtag_ICO,		EDITcmdINSERT_COLUMN ),
    RTF_TRACE_CMD( RTFtag_ACO,		EDITcmdAPPEND_COLUMN ),

    RTF_TRACE_CMD( RTFtag_IHD,		EDITcmdINSERT_HEADER ),
    RTF_TRACE_CMD( RTFtag_IFT,		EDITcmdINSERT_FOOTER ),
    RTF_TRACE_CMD( RTFtag_DHD,		EDITcmdDELETE_HEADER ),
    RTF_TRACE_CMD( RTFtag_DFT,		EDITcmdDELETE_FOOTER ),

    RTF_TRACE_CMD( RTFtag_UOB,		EDITcmdUPD_OBJECT ),

    RTF_TRACE_CMD( RTFtag_SRWD,		EDITcmdSHIFT_ROWS_DOWN ),
    RTF_TRACE_CMD( RTFtag_SRWU,		EDITcmdSHIFT_ROWS_UP ),
    RTF_TRACE_CMD( RTFtag_SPR,		EDITcmdSHIFT_RIGHT ),
    RTF_TRACE_CMD( RTFtag_SPL,		EDITcmdSHIFT_LEFT ),

    RTF_TRACE_CMD( "",			EDITcmd_EDIT_COUNT ),

    RTF_TRACE_CMD( RTFtag_Open,		EDITcmd_OPEN ),
    RTF_TRACE_CMD( RTFtag_Save,		EDITcmd_SAVE ),
    RTF_TRACE_CMD( RTFtag_New,		EDITcmd_NEW ),

    { (char *)0, 0, 0 }
};

const int docRtfTraceGroupCount=
		    sizeof(docRtfTraceGroups)/sizeof(RtfControlWord)- 1;

const char *	docRtfGetTraceTag(	int	command )
    {
    const RtfControlWord *	rcw;

    if  ( docRtfTraceGroupCount != EDITcmd_COUNT		||
	  command < 0						||
	  command >= docRtfTraceGroupCount			)
	{
	LLLDEB(command,docRtfTraceGroupCount,EDITcmd_COUNT);
	return (const char *)0;
	}

    rcw= docRtfTraceGroups+ command;
    if  ( rcw->rcwID != command )
	{ LLSDEB(command,rcw->rcwID,rcw->rcwWord); return (const char *)0; }

    return rcw->rcwWord;
    }

/************************************************************************/

typedef struct ReadEditStep
    {
    RtfReader *			resRtfReader;
    const RtfControlWord *	resControlWord;
    int				resArg;
    int				resGotArg;

    SelectionScope		resSelectionScope;
    RtfTreeStack		resTreeStack;
    } ReadEditStep;

static void docInitReadEditStep(	ReadEditStep *	reds )
    {
    reds->resRtfReader= (RtfReader *)0;
    reds->resControlWord= (RtfControlWord *)0;
    reds->resArg= -1;
    reds->resGotArg= 0;

    docInitSelectionScope( &(reds->resSelectionScope) );
    docRtfInitTreeStack( &(reds->resTreeStack) );
    }

static void docCleanReadEditStep(	ReadEditStep *	reds )
    {
    if  ( reds->resRtfReader )
	{
	if  (  reds->resRtfReader->rrTreeStack )
	    { docRtfPopTreeStack(  reds->resRtfReader );	}

	docRtfCloseReader( reds->resRtfReader );
	}
    }

/************************************************************************/
/*									*/
/*  Consume the body (contents) of an EditStep.				*/
/*									*/
/************************************************************************/

static int docRtfReadEditStepBody(	ReadEditStep *		reds,
					EditStep *		es,
					int			readWhat )
    {
    const int			recursively= 1;
    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    RtfReader *			rr= reds->resRtfReader;
    int				res;

    rr->rrTraceReadWhat= readWhat;

    res= docRtfApplyControlWord( reds->resControlWord, reds->resGotArg,
							reds->resArg,  rr );
    if  ( res )
	{ SDEB(reds->resControlWord->rcwWord); return -1; }

    docDelimitTables( rr->rrDocument->bdBody.dtRoot, recursively );

    es->esCommand= rr->rrTraceCommand;
    es->esSelectionPosition= rr->rrTraceSelectionPosition;
    es->esFieldKind= rr->rrcTraceFieldKind;

    es->esOldSelectionScope= rr->rrcTraceOldSelectionScope;
    es->esOldEditRange= rr->rrcTraceOldRange;
    es->esOldCol0= rr->rrcTraceOldCol0;
    es->esOldCol1= rr->rrcTraceOldCol1;
    es->esOldPage= rr->rrcTraceOldPage;
    es->esOldColumn= rr->rrcTraceOldColumn;

    es->esNewSelectionScope= rr->rrcTraceNewSelectionScope;
    es->esNewEditRange= rr->rrcTraceNewRange;
    es->esNewCol0= rr->rrcTraceNewCol0;
    es->esNewCol1= rr->rrcTraceNewCol1;
    es->esNewPage= rr->rrcTraceNewPage;
    es->esNewColumn= rr->rrcTraceNewColumn;

    es->esPropLevel= rr->rrcTracePropLevel;

    if  ( es->esCommand == EDITcmdUPD_SPAN_PROPS	||
	  es->esCommand == EDITcmdUPD_PARA_PROPS	||
	  es->esCommand == EDITcmdUPD_TABLE_PROPS	||
	  es->esCommand == EDITcmdUPD_ROW_PROPS		||
	  es->esCommand == EDITcmdUPD_COLUMN_PROPS	||
	  es->esCommand == EDITcmdUPD_CELL_PROPS	||
	  es->esCommand == EDITcmdUPD_SECT_PROPS	||
	  es->esCommand == EDITcmdUPD_SECTDOC_PROPS	||
	  es->esCommand == EDITcmdUPD_DOC_PROPS		||
	  es->esCommand == EDITcmdSET_HYPERLINK		||
	  es->esCommand == EDITcmdSET_BOOKMARK		||
	  es->esCommand == EDITcmdDEL_FIELD		||
	  es->esCommand == EDITcmdSET_NEW_LIST		)
	{
	RtfTreeStack *	rts= rr->rrTreeStack;
	PropertyMask	cpAllMask;

	utilPropMaskClear( &cpAllMask );
	utilPropMaskFill( &cpAllMask, CLprop_COUNT );

	rts->rtsRowProperties.rpShadingNumber= docItemShadingNumber(
				rr->rrDocument, &(rr->rrRowShading) );

	if  ( docUpdRowProperties( &(rr->rrStyle.dsRowMask),
		    &(rr->rrStyle.dsCellMask), /* ? */
		    &(rr->rrStyle.dsRowProps),
		    &(rr->rrRowPropertyMask), &cpAllMask,
		    &(rts->rtsRowProperties),
		    0, rts->rtsRowProperties.rpCellCount,
		    (const DocumentAttributeMap *)0 ) )
	    { LDEB(es->esCommand); return -1;	}

	if  ( docCopyStyle( &(es->esNewStyle), &(rr->rrStyle), dam0 )	)
	    { LDEB(es->esCommand); return -1;	}

	/*  Set by the RTF parser, but never the result of a set props */
	PROPmaskUNSET( &(es->esNewStyle.dsRowMask), RPprop_CELL_COUNT );

	if  ( rr->rrStyle.dsRowProps.rpCellCount == 0 )
	    {
	    PROPmaskUNSET( &(es->esNewStyle.dsRowMask), RPprop_CELL_PROPS );
	    }
	}

    if  ( docUpdDocumentProperties(
	    (PropertyMask *)0, &(es->esNewDocProps),
	    &(rr->rrDocPropertyMask), &(rr->rrDocumentProperties), dam0 ) )
	{ LDEB(es->esCommand); return -1;	}
    es->esNewDocPropMask= rr->rrDocPropertyMask;

    es->esPicturePropMask= rr->rrPicturePropMask;

    es->esNotePropMask= rr->rrNotePropertyMask;
    docInitNoteProperties( &(rr->rrNoteProperties) );

    es->esDocumentList= rr->rrcDocumentList;
    docInitDocumentList( &(rr->rrcDocumentList) );

    return 0;
    }

static int docRtfReadEditStepHead(
				ReadEditStep *			reds,
				EditStep *			es,
				struct SimpleInputStream *	sis,
				const struct BufferDocument *	bdRef )
    {
    int				rval= 0;
    int				res;
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    RtfReader *			rr= (RtfReader *)0;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				c;

    bd= docEditStepMakeSourceDocument( es, bdRef );
    if  ( ! bd )
	{ XDEB(bd); rval= -1; goto ready;	}

    rr= docRtfOpenReader( sis, bd, RTFflagUNENCODED );
    if  ( ! rr )
	{ XDEB(rr); rval= -1; goto ready;	}

    bd= (struct BufferDocument *)0; /* stolen by rr */

    docRtfPushTreeStack( rr, &(reds->resTreeStack), &(reds->resSelectionScope),
					    &(rr->rrDocument->bdBody) );

    rr->rrTraceSelectionPosition= SELposTAIL;
    rr->rrcTraceFieldKind= -1;

    if  ( bdRef )
	{
	rr->rrDocument->bdProperties->dpDefaultFont=
				    bdRef->bdProperties->dpDefaultFont;
	}

    res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
    if  ( res != RTFfiCTRLGROUP )
	{ LDEB(res); rval= -1; goto ready; }

    rcw= docRtfFindWord( controlWord, docRtfTraceGroups );
    if  ( ! rcw )
	{ SXDEB(controlWord,rcw); rval= -1; goto ready; }
    if  ( rcw->rcwType != RTCtypeDEST )
	{ SLDEB(rcw->rcwWord,rcw->rcwType); rval= -1; goto ready;	}

    reds->resRtfReader= rr; rr= (RtfReader *)0; /* steal */
    reds->resControlWord= rcw;
    reds->resGotArg= gotArg;
    reds->resArg= arg;

  ready:

    if  ( rr )
	{ docRtfCloseReader( rr );	}
    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

static int docRtfReadEditStep(	EditStep *			es,
				struct SimpleInputStream *	sis,
				int				readWhat,
				const struct BufferDocument *	bdRef )
    {
    int			rval= 0;
    ReadEditStep	reds;

    docInitReadEditStep( &reds );

    if  ( docRtfReadEditStepHead( &reds, es, sis, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRtfReadEditStepBody( &reds, es, readWhat ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanReadEditStep( &reds );

    return rval;
    }

/************************************************************************/

int docRtfSelectionScopeProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    SelectionScope *	ssOld= &(rr->rrcTraceOldSelectionScope);
    SelectionScope *	ssNew= &(rr->rrcTraceNewSelectionScope);

    switch( rcw->rcwID )
	{
	case SSpropTREE_TYPE:
	    ssOld->ssTreeType= ssNew->ssTreeType= arg;
	    return 0;
	case SSpropSECT_NR:
	    ssOld->ssSectNr= ssNew->ssSectNr= arg;
	    return 0;
	case SSpropOWNER_SECT_NR:
	    ssOld->ssOwnerSectNr= ssNew->ssOwnerSectNr= arg;
	    return 0;
	case SSpropOWNER_NR:
	    ssOld->ssOwnerNumber= ssNew->ssOwnerNumber= arg;
	    return 0;

	case SSpropPAGE:
	    rr->rrcTraceOldPage= rr->rrcTraceNewPage= arg;
	    return 0;
	case SSpropCOLUMN:
	    rr->rrcTraceOldColumn= rr->rrcTraceNewColumn= arg;
	    return 0;

	case SSprop_COUNT+ SSpropTREE_TYPE:
	    ssNew->ssTreeType= arg;
	    return 0;
	case SSprop_COUNT+ SSpropSECT_NR:
	    ssNew->ssSectNr= arg;
	    return 0;
	case SSprop_COUNT+ SSpropOWNER_SECT_NR:
	    ssNew->ssOwnerSectNr= arg;
	    return 0;
	case SSprop_COUNT+ SSpropOWNER_NR:
	    ssNew->ssOwnerNumber= arg;
	    return 0;

	case SSprop_COUNT+ SSpropPAGE:
	    rr->rrcTraceNewPage= arg;
	    return 0;
	case SSprop_COUNT+ SSpropCOLUMN:
	    rr->rrcTraceNewColumn= arg;
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}
    }

int docRtfEditRangeProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case TRACEposOLD_HEAD_COL:
	    rr->rrcTraceOldCol0= arg;
	    return 0;
	case TRACEposOLD_HEAD_PARA:
	    rr->rrcTraceOldRange.erHead.epParaNr= arg;
	    return 0;
	case TRACEposOLD_HEAD_STROFF:
	    rr->rrcTraceOldRange.erHead.epStroff= arg;
	    return 0;

	case TRACEposOLD_TAIL_COL:
	    rr->rrcTraceOldCol1= arg;
	    return 0;
	case TRACEposOLD_TAIL_PARA:
	    rr->rrcTraceOldRange.erTail.epParaNr= arg;
	    return 0;
	case TRACEposOLD_TAIL_STROFF:
	    rr->rrcTraceOldRange.erTail.epStroff= arg;
	    return 0;

	case TRACEposNEW_HEAD_COL:
	    rr->rrcTraceNewCol0= arg;
	    return 0;
	case TRACEposNEW_HEAD_PARA:
	    rr->rrcTraceNewRange.erHead.epParaNr= arg;
	    return 0;
	case TRACEposNEW_HEAD_STROFF:
	    rr->rrcTraceNewRange.erHead.epStroff= arg;
	    return 0;

	case TRACEposNEW_TAIL_COL:
	    rr->rrcTraceNewCol1= arg;
	    return 0;
	case TRACEposNEW_TAIL_PARA:
	    rr->rrcTraceNewRange.erTail.epParaNr= arg;
	    return 0;
	case TRACEposNEW_TAIL_STROFF:
	    rr->rrcTraceNewRange.erTail.epStroff= arg;
	    return 0;

	case TRACEposSELECTED:
	    rr->rrTraceSelectionPosition= rcw->rcwEnumValue;
	    return 0;

	case TRACEposPROP_LEVEL:
	    rr->rrcTracePropLevel= arg;
	    return 0;
	case TRACEposFIELD_KIND:
	    rr->rrcTraceFieldKind= arg;
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}
    }

/************************************************************************/

static int docRtfReadTraceStepN( EditStep *			es,
				int				readWhat,
				const EditTrace *		et,
				int				n,
				const struct BufferDocument *	bdRef )
    {
    int				rval= 0;
    struct SimpleInputStream *		sis= (struct SimpleInputStream *)0;
    const TraceStep *		ts;

    ts= (const TraceStep *)utilPagedListGetItemByNumber(
						    &(et->etTraceSteps), n );
    if  ( ! ts )
	{ LXDEB(n,ts); rval= -1; goto ready;	}

    sis= sioInFdOpenAt( et->etTraceFileHandle, ts->tsTraceOffset );
    if  ( ! sis )
	{ LPDEB(et->etTraceFileHandle,sis); rval= -1; goto ready;	}

    if  ( docRtfReadEditStep( es, sis, readWhat, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

static int docRtfReadExtendedReplace(
				EditStep *			es,
				int				readWhat,
				const EditTrace *		et,
				int				n,
				const struct BufferDocument *		bdRef )
    {
    int			rval= 0;
    EditStep		esExt;

    docInitEditStep( &esExt );

    if  ( docRtfReadTraceStepN( es, readWhat, et, n, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( es->esCommand != EDITcmdREPLACE )
	{ LDEB(es->esCommand); rval= -1; goto ready;	}

    if  ( docRtfReadTraceStepN( &esExt, readWhat, et, n+ 1, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( esExt.esCommand != EDITcmdEXTEND_REPLACE )
	{ LDEB(es->esCommand); rval= -1; goto ready;	}

    es->esNewEditRange= esExt.esNewEditRange;

    /* Not used
    es->esNewSelectionScope= esExt.esNewSelectionScope;
    es->esNewCol0= esExt.esNewCol0;
    es->esNewCol1= esExt.esNewCol1;
    es->esNewPage= esExt.esNewPage;
    es->esNewColumn= esExt.esNewColumn;
    */

    if  ( readWhat == EDITversionNEW )
	{
	struct BufferDocument *	swap;
	int			inte;

	swap= es->esSourceDocument;
	      es->esSourceDocument= esExt.esSourceDocument;
			            esExt.esSourceDocument= swap;

	inte= es->esSourceIsIntermediary;
	      es->esSourceIsIntermediary= esExt.esSourceIsIntermediary;
					  esExt.esSourceIsIntermediary= inte;
	}

  ready:

    docCleanEditStep( &esExt );

    return rval;
    }

int docEditReadTraceStep(	EditStep *			es,
				int *				pIsRepeat,
				int				direction,
				const EditTrace *		et,
				const struct BufferDocument *		bdRef )
    {
    int			rval= 0;
    int			n;
    const TraceStep *	ts= (const TraceStep *)0;

    int			isRepeat= 0;

    const int		readWhat= direction > 0?EDITversionNEW:
				(
				direction < 0? EDITversionOLD: EDITversionNONE
				);

    n= docEditGetTraceStep( &ts, &isRepeat, direction, et, et->etIndex );
    if  ( n < 0 )
	{ LDEB(n); rval= -1; goto ready;	}

    switch( ts->tsCommand )
	{
	case EDITcmdEXTEND_REPLACE:
	    if  ( docRtfReadExtendedReplace( es, readWhat, et, n- 1, bdRef ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    es->esTraceIndex= n- 1;
	    es->esStepCount= 2;
	    break;

	case EDITcmdREPLACE:
	    ts= (const TraceStep *)0;
	    if  ( n < et->etCount- 1 )
		{
		ts= (const TraceStep *)utilPagedListGetItemByNumber(
						&(et->etTraceSteps), n+ 1 );
		if  ( ! ts )
		    { LXDEB(n,ts); rval= -1; goto ready;	}
		}

	    if  ( ts && ts->tsCommand == EDITcmdEXTEND_REPLACE )
		{
		if  ( docRtfReadExtendedReplace( es, readWhat, et, n, bdRef ) )
		    { LDEB(1); rval= -1; goto ready;	}
		es->esTraceIndex= n;
		es->esStepCount= 2;
		}
	    else{
		if  ( docRtfReadTraceStepN( es, readWhat, et, n, bdRef ) )
		    { LDEB(1); rval= -1; goto ready;	}
		es->esTraceIndex= n;
		es->esStepCount= 1;
		}
	    break;

	default:
	    if  ( docRtfReadTraceStepN( es, readWhat, et, n, bdRef ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    es->esTraceIndex= n;
	    es->esStepCount= 1;
	    break;
	}

    if  ( pIsRepeat )
	{ *pIsRepeat= isRepeat;	}

  ready:

    return rval;
    }

int docRtfScanEditTrace(	struct SimpleInputStream *	sis,
				HandleEditStep			handleStep,
				void *				through,
				const IndexMapping *		readWhats,
				const struct BufferDocument *	bdRef )
    {
    int			rval= 0;

    TraceStep		ts;
    EditStep		es;
    ReadEditStep	reds;

    int			step= 0;
    int			exhausted= 0;

    docInitTraceStep( &ts );
    docInitEditStep( &es );
    docInitReadEditStep( &reds );

    step= 0;
    ts.tsTraceOffset= 0;

    while( ! exhausted )
	{
	int			res;
	long			endOffset;
	int			readWhat= EDITversionNONE;

	ts.tsTraceOffset= sioInGetBytesRead( sis );

	if  ( docRtfReadEditStepHead( &reds, &es, sis, bdRef ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( readWhats )
	    {
	    readWhat= utilIndexMappingGet( readWhats,
						reds.resControlWord->rcwID );
	    if  ( readWhat < 0 )
		{ readWhat= EDITversionNONE;	}
	    }

	if  ( docRtfReadEditStepBody( &reds, &es, readWhat ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docCleanReadEditStep( &reds );
	docInitReadEditStep( &reds );

	{
	int	c= sioInGetByte( sis );

	while ( c == '\r' || c == '\n' )
	    { c= sioInGetByte( sis );	}

	if  ( c == EOF )
	    { exhausted= 1;			}
	else{ sioInUngetLastRead( sis );	}
	}

	endOffset= sioInGetBytesRead( sis );

	ts.tsCommand= es.esCommand;
	ts.tsFieldKind= es.esFieldKind;
	/* ts.tsTraceOffset= .. set above */
	ts.tsByteCount= endOffset- ts.tsTraceOffset;

	res= (*handleStep)( &ts, &es, step, through );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	if  ( res > 0 )
	    { goto ready;	}

	step++;
	docCleanEditStep( &es ); docInitEditStep( &es );
	}

  ready:

    docCleanReadEditStep( &reds );
    docCleanEditStep( &es );

    return rval;
    }
