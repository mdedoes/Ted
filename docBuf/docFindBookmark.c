/************************************************************************/
/*									*/
/*  Find a bookmark in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	"docBuf.h"
#   include	"docSelect.h"
#   include	"docFields.h"

#   include	<appDebugon.h>

int docFindBookmarkInDocument(	struct DocumentSelection *	dsInside,
				int *				pHeadPart,
				int *				pTailPart,
				const struct BufferDocument *	bd,
				const struct MemoryBuffer *	markName )
    {
    int				i;
    struct DocumentField *	df;

    i= docFindBookmarkField( &df, &(bd->bdFieldList), markName );
    if  ( i >= 0 )
	{
	DocumentSelection	dsAround;

	if  ( docDelimitFieldInDoc( dsInside, &dsAround,
					    pHeadPart, pTailPart, bd, df ) )
	    { LDEB(i); return -1;	}

	return 0;
	}

    return 1;
    }

