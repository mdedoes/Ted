/************************************************************************/
/*									*/
/*  Save the colors in a struct BufferDocument to an RTF file.		*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	"docRtfWriterImpl.h"
#   include	<docDocumentProperties.h>
#   include	<utilPalette.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write a color table.						*/
/*									*/
/************************************************************************/

int docRtfWriteColorTable(	RtfWriter *			rw,
				const DocumentProperties *	dp )
    {
    int				i;
    const RGB8Color *		rgb8= dp->dpColorPalette->cpColors;

    docRtfWriteDestinationBegin( rw, "colortbl" );
    docRtfWriteNextLine( rw );

    for ( i= 0; i < dp->dpColorPalette->cpColorCount; rgb8++, i++ )
	{
	if  ( i != dp->dpDefaultColor )
	    {
	    docRtfWriteArgTag( rw, "red", rgb8->rgb8Red );
	    docRtfWriteArgTag( rw, "green", rgb8->rgb8Green );
	    docRtfWriteArgTag( rw, "blue", rgb8->rgb8Blue );
	    }

	if  ( docRtfPutByte( ';', rw ) < 0 )
	    { LDEB(1); return -1;	}

	rw->rwCol += 1;
	docRtfWriteNextLine( rw );
	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    return 0;
    }

