/************************************************************************/
/*									*/
/*  Printing retaled functionality.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_PRINT_JOB_H
#   define	APP_PRINT_JOB_H

#   include	<stdio.h>

#   include	<drawDrawingSurface.h>

#   include	<psPrint.h>

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Printing..								*/
/*									*/
/************************************************************************/

typedef struct PrintJob
    {
    void *			pjPrivateData;
    int				pjFormat;
    const char *		pjTitle;
    DrawingSurface		pjDrawingSurface;
    struct EditApplication *	pjApplication;
    const PrintGeometry *	pjPrintGeometry;
    } PrintJob;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

# ifdef __cplusplus
    }
# endif

#   endif
