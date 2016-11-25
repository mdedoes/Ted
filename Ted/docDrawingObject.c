/************************************************************************/
/*									*/
/*  Exchange text properties with an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docDrawingObject.h"

/************************************************************************/
/*									*/
/*  Initialize a drawing object.					*/
/*									*/
/************************************************************************/

void docInitDrawingObject(		DrawingObject *		wdo )
    {
    wdo->doAnchorLocked= 0;

    wdo->doXAttach= DOxaCOLUMN;
    wdo->doYAttach= DOyaPAGE;
    wdo->doKind= DOkindRECT;
    wdo->doLineStyle= DOlineHOLLOW;
    wdo->doFillPattern= DOfillCLEAR;

    wdo->doLineColorGray= 0;
    wdo->doFillForeColorGray= 0;
    wdo->doFillBackColorGray= 0;

    wdo->doArcFlipX= 0;
    wdo->doArcFlipY= 0;

    wdo->doX= 0;
    wdo->doY= 0;
    wdo->doZ= 16000;
    wdo->doWide= 1;
    wdo->doHigh= 1;

    wdo->doLineWidth= 1;

    bmInitRGB8Color( &(wdo->doLineColor) );
    bmInitRGB8Color( &(wdo->doFillForeColor) );
    bmInitRGB8Color( &(wdo->doFillBackColor) );

    wdo->doPoints= (APP_POINT *)0;
    wdo->doPointCount= 0;
    wdo->doNextPoint= 0;

    return;
    }

void docCleanDrawingObject(		DrawingObject *		wdo )
    {
    if  ( wdo->doPoints )
	{ free( wdo->doPoints ); }

    return;
    }
