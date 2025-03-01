/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"psPrintShape.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a rounded rectangle.						*/
/*									*/
/************************************************************************/

void psDrawRoundRectPath(	SimpleOutputStream *		sos,
				const DocumentRectangle *	drAround,
				int				r,
				const char *			psOperator )
    {
    int		d;

    d= ( drAround->drX1- drAround->drX0 )/ 2;
    if  ( r > d )
	{ LLDEB(r,d); r= d;	}

    d= ( drAround->drY1- drAround->drY0 )/ 2;
    if  ( r > d )
	{ LLDEB(r,d); r= d;	}

    if  ( r < 0 )
	{ LDEB(r); r= -r;	}

    sioOutPrintf( sos, "%d %d bp\n", drAround->drX0+ r, drAround->drY0 );

    if  ( r > 0 )
	{
	sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    drAround->drX1, drAround->drY0,
				    drAround->drX1, drAround->drY0+ r, r );
	sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    drAround->drX1, drAround->drY1,
				    drAround->drX1- r, drAround->drY1, r );
	sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    drAround->drX0, drAround->drY1,
				    drAround->drX0, drAround->drY1- r, r );
	sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    drAround->drX0, drAround->drY0,
				    drAround->drX0+ r, drAround->drY0, r );
	}
    else{
	sioOutPrintf( sos, "%d %d rl\n", drAround->drX1- drAround->drX0, 0 );
	sioOutPrintf( sos, "%d %d rl\n", 0, drAround->drY1- drAround->drY0 );
	sioOutPrintf( sos, "%d %d rl\n", drAround->drX0- drAround->drX1, 0 );
	sioOutPrintf( sos, "%d %d rl\n", 0, drAround->drY0- drAround->drY1 );
	}

    if  ( psOperator )
	{ sioOutPrintf( sos, "%s", psOperator );	}

    return;
    }

