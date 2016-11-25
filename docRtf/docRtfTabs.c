/************************************************************************/
/*									*/
/*  Read/Write tab stops from/to rtf.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Save a tab stop.							*/
/*									*/
/************************************************************************/

void docRtfSaveTabStopList( 	RtfWriter *		rwc,
				const TabStopList *		tsl )
    {
    int			i;
    const TabStop *	ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	if  ( rwc->rwCol >= 65 )
	    { docRtfWriteNextLine( rwc );	}

	switch( ts->tsAlignment )
	    {
	    case DOCtaLEFT:
		break;
	    case DOCtaRIGHT:
		docRtfWriteTag( rwc, "tqr" );
		break;
	    case DOCtaCENTER:
		docRtfWriteTag( rwc, "tqc" );
		break;
	    case DOCtaDECIMAL:
		docRtfWriteTag( rwc, "tqdec" );
		break;
	    default:
		LDEB(ts->tsAlignment); break;
	    }

	switch( ts->tsLeader )
	    {
	    case DOCtlNONE:
		break;
	    case DOCtlDOTS:
		docRtfWriteTag( rwc, "tldot" );
		break;
	    case DOCtlUNDERLINE:
		docRtfWriteTag( rwc, "tlul" );
		break;
	    case DOCtlHYPH:
		docRtfWriteTag( rwc, "tlhyph" );
		break;
	    case DOCtlTHICK:
		docRtfWriteTag( rwc, "tlth" );
		break;
	    case DOCtlEQUAL:
		docRtfWriteTag( rwc, "tleq" );
		break;
	    default:
		LDEB(ts->tsLeader); break;
	    }

	if  ( ts->tsFromStyleOrList )
	    { docRtfWriteTag( rwc, "jclisttab" );	}

	docRtfWriteArgTag( rwc, "tx", ts->tsTwips );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a tab stop property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberTabStopProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    TabStop *		ts= &(rrc->rrcTabStop);

    if  ( docTabStopSetProperty( ts, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return 0;	}

    if  ( rcw->rcwID == TABpropX )
	{
	if  ( docAddTabToListTwips( &(rrs->rrsTabStopList), ts ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return -1;	}

	docInitTabStop( ts );

	PROPmaskADD( &(rrc->rrcStyle.dsParaMask), PPpropTAB_STOPS );
	}

    return 0;
    }
