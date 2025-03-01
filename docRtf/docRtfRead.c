/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	"docRtfReadWrite.h"
#   include	"docRtfTagEnum.h"
#   include	"docRtfFlags.h"
#   include	<docRecalculateFields.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docFieldKind.h>
#   include	<docListAdmin.h>
#   include	<docSelect.h>
#   include	<docBuf.h>
#   include	<docDocument.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/

const char DOC_RTF_LENIENT_MESSAGE[]= "Use lenientRtf setting";

/************************************************************************/
/*									*/
/*  Groups (Most of them handeled at the document level).		*/
/*									*/
/************************************************************************/

RtfControlWord	docRtfDocumentGroups[]=
{
    RTF_DEST_XX( "tc",	 RTCscopePARA_FIELD, DOCfkTC,	docRtfLookupEntry ),
    RTF_DEST_XX( "tcn",	 RTCscopePARA_FIELD, DOCfkTCN,	docRtfLookupEntry ),

    RTF_DEST_XX( "nonesttables", RTCscopeHIERARCHY, 1,	docRtfSkipGroup ),

    RTF_DEST_XX( "trowd", RTCscopeROW, DOClevROW, docRtfReadRowProperties ),

    { (char *)0, 0, 0 }
};

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

static int docRtfReadDoc(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			rval= 0;

    SelectionScope	ss;
    RtfTreeStack	internRts;

    docRtfInitTreeStack( &internRts );
    docInitSelectionScope( &ss );

    ss.ssTreeType= DOCinBODY;

    /* popped in docRtfFinishCurrentTree() */
    docRtfPushTreeStack( rr, &internRts, &ss, &(rr->rrDocument->bdBody) );

    rval= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfDocumentGroups,
				docRtfGotText,
				docRtfFinishCurrentTree );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval); rval= -1; goto ready;	}

  ready:

    docRtfPopTreeStack( rr );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a whole document.						*/
/*									*/
/*  As at least MS-Word 2000 crashes on lists without an override,	*/
/*  and on the other hand staroffice/openoffice creates them under	*/
/*  certain circumstances: Make an empty override for lists without	*/
/*  overrides.								*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfOutsideGroups[]=
    {
	RTF_DEST_XX( "rtf",	RTCscopeANY, 0,	docRtfReadDoc ),

	{ (char *)0, 0, 0 }
    };

struct BufferDocument * docRtfReadFile(	struct SimpleInputStream *	sis,
					unsigned int			flags )
    {
    struct BufferDocument *	rval= (struct BufferDocument *)0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    RtfReader *			rr= (RtfReader *)0;

    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				c;

    const struct DocumentField * dfNote;
    struct DocumentNote *	dn;
    int				changed;

    bd= docNewDocument( (struct BufferDocument *)0 );
    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    rr= docRtfOpenReader( sis, bd, flags );
    if  ( ! rr )
	{ XDEB(rr); goto ready;	}

    res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
    if  ( res != RTFfiCTRLGROUP )
	{ goto ready; }

    rcw= docRtfFindWord( controlWord, docRtfOutsideGroups );
    if  ( ! rcw )
	{ LDEB(1); goto ready; }
    if  ( rcw->rcwType != RTCtypeDEST )
	{ SLDEB(rcw->rcwWord,rcw->rcwType); goto ready;	}

    res= docRtfApplyControlWord( rcw, gotArg, arg, rr );
    if  ( res )
	{ LDEB(1); goto ready; }

    /*  Check against trailing garbage. Spaces are not allowed either, 	*/
    /*  but we do not want to scare the user with invisible things.	*/

    if  ( ! ( rr->rrReadFlags & RTFflagLENIENT )	&&
	  docRtfCheckAtEOF( rr )			)
	{ XDEB(rr->rrReadFlags); goto ready;	}

    if  ( ! rr->rrGotDocGeometry		&&
	  bd->bdBody.dtRoot->biChildCount > 0	)
	{
	bd->bdProperties->dpGeometry=
		    bd->bdBody.dtRoot->biChildren[0]->biSectDocumentGeometry;
	}

    docRenumberNotes( &changed, bd );

    dfNote= docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE );
    if  ( dfNote )
	{
	if  ( docCheckSeparatorExistenceForNoteType( bd, DOCinFOOTNOTE ) )
	    { LDEB(DOCinFTNSEP); goto ready; }
	}

    dfNote= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );
    if  ( dfNote )
	{
	if  ( docCheckSeparatorExistenceForNoteType( bd, DOCinENDNOTE ) )
	    { LDEB(DOCinAFTNSEP); goto ready; }
	}

    if  ( docMakeOverrideForEveryList( bd->bdProperties->dpListAdmin ) )
	{ LDEB(1); goto ready;	}

    if  ( docGetCharsUsed( bd ) )
	{ LDEB(1); goto ready;	}

    if  ( bd->bdProperties->dpDefaultLocaleId > 0 )
	{ bd->bdLocaleId= bd->bdProperties->dpDefaultLocaleId;	}

    rval= bd; bd= (struct BufferDocument *)0; /* steal */

    /* LDEB(1); docListNode(bd,0,rval->bdBody.dtRoot,0); */

  ready:

    if  ( rr && ! rval )
	{
	char		remainingRtf[200+1];
	int		done= sioInReadBytes( sis, (unsigned char *)remainingRtf, sizeof(remainingRtf)-1 );
	RtfTreeStack *  rts= rr->rrTreeStack;

	if  ( done >= 0 )
	    {
	    remainingRtf[done]= '\0';
	    LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,remainingRtf);
	    }

	while( rts )
	    {
	    if  ( rts->rtsTree && rts->rtsTree->dtRoot )
		{
		DocumentPosition	dpLast;

		if  ( ! docTailPosition( &dpLast, rts->rtsTree->dtRoot ) )
		    {
		    const char * lastParagraph= docParaString(dpLast.dpNode,0);
		    SSDEB(docTreeTypeStr(rts->rtsTree->dtRoot->biTreeType),lastParagraph);
		    }
		else{
		    SSDEB(docTreeTypeStr(rts->rtsTree->dtRoot->biTreeType),"empty");
		    }
		}

	    rts= rts->rtsPrev;
	    }
	}

    if  ( rr )
	{ docRtfCloseReader( rr );	}

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remember properties to be used subsequently.			*/
/*									*/
/*  References are to be replaced with feature specific routine		*/
/*  refrerences.							*/
/*									*/
/************************************************************************/

int docRtfRememberProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    switch( rcw->rcwID )
	{
	case RTFidUC:
	    if  ( rr->rrState )
		{ rr->rrState->rrsBytesPerUnicode= arg;	}
	    break;
				/****************************************/
				/*  Unknown				*/
				/****************************************/
	default:
	    /*SLDEB(rcw->rcwWord,rcw->rcwID);*/
	    break;
	}

    return 0;
    }
