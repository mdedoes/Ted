/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*  Read form fields.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfFindProperty.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Consume a form field.						*/
/*									*/
/************************************************************************/

int docRtfReadFormField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;

    res= docRtfReadGroup( rcw, 0, 0, rr,
				docRtfDocumentGroups,
				docRtfGotText, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfCommitDataField(	const RtfControlWord *	rcw,
					RtfReader *		rr )
    {
    int			rval= 0;

    char *		text= (char *)0;
    int			size;
    const int		removeSemicolon= 0;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); rval= -1; goto ready;	}

#   if 0

    /*
    Simply holds the binary bytes of the FormField structure.
    As we also have the \\ffsomething tags, there is no need to decode.
    */

    {
    int	i;

    appDebug( "DATAFIELD=\"" );

    for ( i= 0; i < size; i += 2 )
	{
	unsigned	c;

	sscanf( text+ i, "%2x", &c );

	if  ( isprint( c ) )
	    { appDebug( "%c", c );	}
	else{ appDebug( "\\x%02x", c );	}
	}

    appDebug( "\"\n" );
    }

#   endif

  ready:

    if  ( text )
	{ free( text );		}

    return rval;
    }

int docRtfReadDataField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    int			res;

    res= docRtfReadGroup( rcw, 0, 0, rr, (RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						docRtfCommitDataField );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

int docRtfRememberFormFieldProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    if  ( docSetFormFieldProperty( &(rr->rrFormField), rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember a form field property that has a string value.		*/
/*									*/
/************************************************************************/

int docRtfCommitFormFieldText(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    int		rval= 0;
    const int	removeSemicolon= 0;

    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    if  ( docSetFormFieldString( &(rr->rrFormField),
						rcw->rcwID, text, size ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

  ready:

    if  ( text )
	{ free( text );		}

    return rval;
    }
