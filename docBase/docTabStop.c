/************************************************************************/
/*									*/
/*  Manage tab stops.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	"docTabStop.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a tab stop.						*/
/*									*/
/************************************************************************/

void docInitTabStop(	TabStop *	ts )
    {
    ts->tsOffset= 0;
    ts->tsAlignment= DOCtaLEFT;
    ts->tsLeader= DOCtlNONE;
    ts->tsFromStyleOrList= 0;
    }

void docCalculatedTabStop(	TabStop *	ts,
				int		offset )
    {
    ts->tsOffset= offset;
    ts->tsAlignment= DOCtaLEFT;
    ts->tsLeader= DOCtlNONE;
    ts->tsFromStyleOrList= 0;
    }

int docTabStopSetProperty(	TabStop *		ts,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case TABpropOFFSET:
	    ts->tsOffset= val;
	    return 0;

	case TABpropALIGN:
	    ts->tsAlignment= val;
	    return 0;

	case TABpropLEADER:
	    ts->tsLeader= val;
	    return 0;

	case TABpropFROM_STYLE:
	    ts->tsFromStyleOrList= val != 0;
	    return 0;

	default:
	    LDEB(prop); return -1;
	}
    }

int docTabStopGetProperty(	const TabStop *		ts,
				int			prop )
    {
    switch( prop )
	{
	case TABpropOFFSET:
	    return ts->tsOffset;

	case TABpropALIGN:
	    return ts->tsAlignment;

	case TABpropLEADER:
	    return ts->tsLeader;

	case TABpropFROM_STYLE:
	    return ts->tsFromStyleOrList;

	default:
	    LDEB(prop); return -1;
	}
    }

const char * docTabAlignmentStr(	int	alignment )
    {
    static char	scratch[20];

    switch( alignment )
	{
	case DOCtaLEFT:		return "LEFT";
	case DOCtaRIGHT:	return "RIGHT";
	case DOCtaCENTER:	return "CENTER";
	case DOCtaDECIMAL:	return "DECIMAL";

	default:
	    sprintf( scratch, "%d", alignment );
	    return scratch;
	}
    }

const char * docTabLeaderStr(	int	leader )
    {
    static char	scratch[20];

    switch( leader )
	{
	case DOCtlNONE:		return "NONE";
	case DOCtlDOTS:		return "DOTS";
	case DOCtlUNDERLINE:	return "UNDERLINE";
	case DOCtlHYPH:		return "HYPH";
	case DOCtlTHICK:	return "THICK";
	case DOCtlEQUAL:	return "EQUAL";

	default:
	    sprintf( scratch, "%d", leader );
	    return scratch;
	}
    }

