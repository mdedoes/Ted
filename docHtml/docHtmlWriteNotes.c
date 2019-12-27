/************************************************************************/
/*									*/
/*  Save notes.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<ctype.h>

#   include	<docDocumentNote.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	<docFields.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docHtmlSaveNote(		HtmlWritingContext *	hwc,
					struct DocumentNote *	dn )
    {
    if  ( ! dn->dnDocumentTree.dtRoot )
	{ XDEB(dn->dnDocumentTree.dtRoot);	}
    else{
	if  ( docHtmlSaveSelection( hwc, &(dn->dnDocumentTree),
				    (const struct DocumentSelection *)0 ) )
	    { LDEB(1); return -1; }
	}

    return 0;
    }

static int docHtmlSaveNoteSeparator(	HtmlWritingContext *	hwc,
					int			sepTreeType )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    DocumentTree *	sepTree= docDocumentNoteSeparator( bd, sepTreeType );

    if  ( ! sepTree )
	{ LXDEB(sepTreeType,sepTree); return -1;	}
    if  ( ! sepTree->dtRoot )
	{ LXDEB(sepTreeType,sepTree->dtRoot);	}

    if  ( docHtmlSaveSelection( hwc, sepTree,
				(const struct DocumentSelection *)0 ) )
	{ LDEB(1); return -1; }

    return 0;
    }

int docHtmlSaveEndnotes(	int			sect,
				HtmlWritingContext *	hwc )
    {
    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    struct DocumentField *	df;
    struct DocumentNote *	dn;

    df= docGetFirstNoteOfSection( &dn, bd, sect, DOCinENDNOTE );
    if  ( df )
	{
	if  ( docHtmlSaveNoteSeparator( hwc, DOCinAFTNSEP ) )
	    { LDEB(DOCinAFTNSEP); return -1;	}
	}

    while( df )
	{
	if  ( docHtmlSaveNote( hwc, dn ) )
	    { LDEB(1); return -1;	}

	df= docGetNextNoteInSection( &dn, bd, sect, df, DOCinENDNOTE );
	}

    return 0;
    }

int docHtmlSaveFootnotes(	HtmlWritingContext *	hwc )
    {
    const int			sect= -1;

    const HtmlWritingSettings *	hws= hwc->hwcSettings;
    struct BufferDocument *	bd= hws->hwsDocument;

    struct DocumentField *	df;
    struct DocumentNote *	dn;

    df= docGetFirstNoteOfSection( &dn, bd, sect, DOCinFOOTNOTE );
    if  ( df )
	{
	if  ( docHtmlSaveNoteSeparator( hwc, DOCinFTNSEP ) )
	    { LDEB(DOCinAFTNSEP); return -1;	}
	}

    while( df )
	{
	if  ( docHtmlSaveNote( hwc, dn ) )
	    { LDEB(1); return -1;	}

	df= docGetNextNoteInSection( &dn, bd, sect, df, DOCinFOOTNOTE );
	}

    return 0;
    }

typedef struct SaveNotes
    {
    int				snDone;
    HtmlWritingContext *	snHtmlWritingContext;
    } SaveNotes;

static int docHtmlSaveDeferredNote(	int		from,
					int		to,
					void *		vsn )
    {
    SaveNotes *				sn= (SaveNotes *)vsn;
    HtmlWritingContext *		hwc= sn->snHtmlWritingContext;
    const HtmlWritingSettings *		hws= hwc->hwcSettings;
    const struct BufferDocument *	bd= hws->hwsDocument;

    const DocumentField *	df;
    struct DocumentNote *	dn;

    if  ( sn->snDone == 0 )
	{
	/* trbl */
	docHtmlPutString( "<div style=\"padding: 1em 0 1em 40pt;\">\n", hwc );
	}

    df= docGetFieldByNumber( bd, to );
    if  ( df->dfKind != DOCfkCHFTN )
	{ LDEB(df->dfKind); return -1;	}

    dn= docGetNoteOfField( df, bd );
    if  ( ! dn || docHtmlSaveNote( hwc, dn ) )
	{ XDEB(dn); return -1;	}

    sn->snDone++;
    return 0;
    }

int docHtmlSaveDeferredNotes(	HtmlWritingContext *		hwc )
    {
    SaveNotes			sn;

    sn.snDone= 0;
    sn.snHtmlWritingContext= hwc;

    if  ( utilIndexMappingForAll( &(hwc->hwcDeferredNotes),
				    docHtmlSaveDeferredNote, (void *)&sn ) )
	{ LDEB(1); return -1;	}

    if  ( sn.snDone > 0 )
	{ docHtmlWritelnCloseElement( "div", hwc );	}

    utilClearIndexMapping( &(hwc->hwcDeferredNotes) );
    return 0;
    }

int docHtmlStartNote(	const DocumentField *		df,
			HtmlWritingContext *		hwc,
			const struct BufferItem *	node,
			int				attNr )
    {
    int				rval= 0;

    const HtmlWritingSettings *	hws= hwc->hwcSettings;

    const MemoryBuffer *	fileName= (const MemoryBuffer *)0;

    const int			isNote= 1;

    MemoryBuffer		mbTarget;
    MemoryBuffer		mbSource;
    MemoryBuffer		mbTitle;

    utilInitMemoryBuffer( &mbTarget );
    utilInitMemoryBuffer( &mbSource );
    utilInitMemoryBuffer( &mbTitle );

    if  ( node->biTreeType == DOCinBODY )
	{
	DocumentNote *	dn;

	dn= docGetNoteOfField( df, hws->hwsDocument );
	if  ( dn )
	    {
	    if  ( docCollectNoteTitle( &mbTitle, dn, hws->hwsDocument ) )
		{ LDEB(1); rval= -1;	}
	    }
	}

    if  ( docSetNoteLinks( &mbTarget, &mbSource,
					(struct BufferItem *)node, df ) )
	{ LDEB(1); rval= -1;	}

    docHtmlChangeAttributes( hwc, -1 );

    docHtmlStartAnchor( hwc, isNote, fileName, &mbTarget, &mbSource, &mbTitle );

    docHtmlChangeAttributes( hwc, attNr );

    hwc->hwcBytesInLink= 0;

    utilCleanMemoryBuffer( &mbTarget );
    utilCleanMemoryBuffer( &mbSource );
    utilCleanMemoryBuffer( &mbTitle );

    return rval;
    }

