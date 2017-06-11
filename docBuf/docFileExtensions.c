/************************************************************************/
/*									*/
/*  Document Extensions that we support in one way or another.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docFileExtensions.h"
#   include	<utilFileExtension.h>

AppFileExtension	DOC_FileExtensionsArray[TEDdockind_COUNT]=
{
    { "rtfFiles",	"*.rtf",
			"Rich Text Files ( *.rtf )",
			"rtf"	 ,
			( APPFILE_CAN_OPEN|APPFILE_CAN_SAVE ),
			},
    { "txtFiles",	"*.txt",
			"Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_OPEN ),
			},
    { "txtFilesWide",	"*.txt",
			"Wide Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "txtFilesFolded",	"*.txt",
			"Folded Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "markdownFiles",	"*.md",
			"Markdown Files ( *.md )",
			"md",
			( APPFILE_CAN_SAVE ),
    },
    { "htmlFiles",	"*.html",
			"HTML Files ( *.html )",
			"html",
			( APPFILE_CAN_SAVE ),
    },
    { "emlFiles",	"*.eml",
			"HTML Mail message (rfc 2557) ( *.eml )",
			"eml",
			( APPFILE_CAN_SAVE ),
    },
    { "epubFiles",	"*.epub",
			"EPUB e-book ( *.epub )",
			"epub",
			( APPFILE_CAN_SAVE ),
    },
    { "psFiles",	"*.ps",
			"PostScript ( *.ps )",
			"ps",
			( APPFILE_CAN_SAVE ),
    },
    { "svgFiles",	"*.svg",
			"Scalable Vector Graphics ( *.svg )",
			"svg",
			( APPFILE_CAN_SAVE ),
    },
    { "pdfFiles",	"*.pdf",
			"Portable Document Format ( *.pdf )",
			"pdf",
			( APPFILE_CAN_SAVE ),
    },
    { "traceFiles",	"*.Ted",
			"Ted Recovery File ( *.Ted )",
			"Ted",
			( APPFILE_CAN_OPEN ),
    },
    { "allFiles",	"*",
			"All Files ( *.* )",
			(char *)0,
			( APPFILE_CAN_OPEN ),
    },
};

const int DOC_FileExtensionCount=
		    sizeof(DOC_FileExtensionsArray)/ sizeof(AppFileExtension);
AppFileExtension * const DOC_FileExtensions= DOC_FileExtensionsArray;
