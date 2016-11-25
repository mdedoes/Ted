/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfTextConverter.h"
#   include	<utilOfficeCharset.h>
#   include	<textConverter.h>

/************************************************************************/

const char DOC_RTF_AnsiCharsetName[]= "CP1252";

/************************************************************************/

void docRtfInitTextConverters( RtfTextConverter * rtc )
    {
    utilInitTextConverter( &(rtc->rtcRtfConverter) );
    utilInitTextConverter( &(rtc->rtcTextConverter) );
    }

void docRtfCleanTextConverters( RtfTextConverter * rtc )
    {
    utilCleanTextConverter( &(rtc->rtcRtfConverter) );
    utilCleanTextConverter( &(rtc->rtcTextConverter) );
    }

/************************************************************************/

void docRtfInitEncodedFont(	EncodedFont *	ef )
    {
    ef->ecFileFontNumber= -1;
    ef->ecBufFontNumber= -1;
    ef->ecCharset= FONTcharsetANSI;
    ef->ecCharsetIdx= -1;
    ef->ecCodepage= -1;
    }

