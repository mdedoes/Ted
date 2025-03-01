#   include	"docBufConfig.h"

#   include	<string.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	<docTreeType.h>
#   include	"docDocumentNote.h"

#   include	<appDebugon.h>

#   define	LIST_LINE_PARTICULES	0

void docListParticule(	int			indent,
			const char *		label,
			int			n,
			const struct BufferItem *	node,
			const TextParticule *	tp )
    {
    appDebug( "%*s%s %3d: [S %4d..%4d] A=%-3d %s %s",
		    indent, "", label, n,
		    tp->tpStroff, tp->tpStroff+ tp->tpStrlen,
		    tp->tpTextAttrNr,
		    docParticuleFlagsStr( tp->tpFlags ),
		    docKindStr( tp->tpKind ) );

    if  ( tp->tpStrlen > 0 )
	{
	appDebug( " \"%.*s\"",
		    (int)tp->tpStrlen,
		    docParaString( node, tp->tpStroff ) );
	}

    if  ( tp->tpKind == TPkindOBJECT	||
	  tp->tpKind == TPkindFIELDHEAD	||
	  tp->tpKind == TPkindFIELDTAIL	||
	  tp->tpObjectNumber >= 0		)
	{ appDebug( " OBNR=%d", tp->tpObjectNumber );	}

    appDebug( "\n" );

    return;
    }

void docListTextLine(	int			indent,
			const char *		label,
			int			n,
			const struct BufferItem *	paraNode,
			const TextLine *	tl )
    {
#   if LIST_LINE_PARTICULES
    TextParticule *	tp;
    int			part;
#   endif

    char		flags[100];
    const char *	sep= " ";

    if  ( ! tl )
	{ LDEB(tl); return;	}

    flags[0]= '\0';
    if  ( tl->tlFlags & TLflagBLOCKBREAK )
	{
	strcat( flags, sep ); sep= "|";
	strcat( flags, "BB" );
	}
    if  ( tl->tlFlags & TLflagINLINECONTENT )
	{
	strcat( flags, sep ); sep= "|";
	strcat( flags, "IC" );
	}
    if  ( tl->tlFlags & TLflagSHADING )
	{
	strcat( flags, sep ); sep= "|";
	strcat( flags, "SH" );
	}
    if  ( tl->tlFlags & TLflagBORDER )
	{
	strcat( flags, sep ); sep= "|";
	strcat( flags, "BD" );
	}

    appDebug( "%*s%s %3d: PG=%2d:%d Y=%d H=%d S:[%4d..%4d] P:[%3d..%3d]%s ",
	    indent, "", label, n,
	    tl->tlTopPosition.lpPage,tl->tlTopPosition.lpColumn,
	    tl->tlTopPosition.lpPageYTwips, tl->tlLineStride,
	    tl->tlStroff, tl->tlStroff+ tl->tlStrlen,
	    tl->tlFirstParticule, tl->tlFirstParticule+ tl->tlParticuleCount,
	    flags );

    appDebug( "\"%.*s\"\n",
		    (int)tl->tlStrlen,
		    docParaString( paraNode, tl->tlStroff ) );

#   if LIST_LINE_PARTICULES

    tp= paraNode->biParaParticules+ tl->tlFirstParticule;
    for ( part= tl->tlFirstParticule;
          part < tl->tlFirstParticule+ tl->tlParticuleCount;
	  tp++, part++ )
	{ docListParticule( indent+ 4, "PART", part, paraNode, tp ); }

#   endif

    return;
    }

void docListNotes(	const struct BufferDocument *	bd )
    {
    int				i;

    for ( i= 0; i < bd->bdNotesList.nlNoteCount; i++ )
	{
	const DocumentNote *	dn= bd->bdNotesList.nlNotes[i];

	if  ( ! dn )
	    {
	    appDebug( "%4d DELETED\n", i );
	    continue;
	    }

	appDebug(
	    "%4d %-8s %3d: NR=%3d PG=%3d:%d\n",
	    i, docTreeTypeStr( dn->dnNoteProperties.npTreeType ),
	    i, dn->dnNoteNumber,
	    dn->dnReferringPage,
	    dn->dnReferringColumn );
	}

    return;
    }

