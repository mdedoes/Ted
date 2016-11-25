/************************************************************************/
/*									*/
/*  Manage the string of text bytes in a paragraph.			*/
/*  (Try not to be too clever, this will be changed to unicode once.)	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Make sure there is enough space in the string of a paragraph.	*/
/*									*/
/*  NOTE that bi->biParaStrlen is not updated. We just make sure that	*/
/*	there is enough memory.						*/
/*									*/
/************************************************************************/

int docInflateTextString(	BufferItem *    bi,
				int		by )
    {
    unsigned char *	freshString;
    int			newSize;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    if  ( bi->biParaStrlen % 50 )
	{ newSize= bi->biParaStrlen+ by+  1; }
    else{ newSize= bi->biParaStrlen+ by+ 51; }

    freshString= (unsigned char *)realloc( bi->biParaString, newSize );
    if  ( ! freshString )
	{ LXDEB(bi->biParaStrlen,freshString); return -1; }

    bi->biParaString= freshString;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace part of the string of a paragraph with a different string	*/
/*									*/
/************************************************************************/

int docParaStringReplace(		int *			pStroffShift,
					BufferItem *		bi,
					int			stroffBegin,
					int			stroffEnd,
					const unsigned char *	addedString,
					int			addedStrlen )
    {
    int		stroffShift;

    stroffShift= addedStrlen- stroffEnd+ stroffBegin;

    if  ( stroffShift > 0				&&
	  docInflateTextString( bi, stroffShift )	)
	{ LLDEB(bi->biParaStrlen,stroffShift); return -1;	}

    if  ( bi->biParaStrlen > 0 )
	{
	memmove( bi->biParaString+ stroffEnd+ stroffShift,
		bi->biParaString+ stroffEnd, bi->biParaStrlen- stroffEnd+ 1 );
	}

    memcpy( bi->biParaString+ stroffBegin, addedString, addedStrlen );

    if  ( stroffShift != 0 )
	{
	bi->biParaStrlen += stroffShift;
	bi->biParaString[bi->biParaStrlen]= '\0';
	}

    *pStroffShift= stroffShift;
    return 0;
    }

