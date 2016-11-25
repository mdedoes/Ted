/************************************************************************/
/*									*/
/*  Printing retaled functionality.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_PRINT_JOB_H
#   define	APP_PRINT_JOB_H

#   include	<drawDrawingSurface.h>

# ifdef __cplusplus
extern "C"
    {
# endif

struct PrintGeometry;
struct SimpleOutputStream;

/************************************************************************/
/*									*/
/*  Printing..								*/
/*									*/
/************************************************************************/

typedef struct PrintJob
    {
    void *				pjPrivateData;
    int					pjFormat;
    const char *			pjTitle;
    DrawingSurface			pjDrawingSurface;
    struct EditApplication *		pjApplication;
    const struct PrintGeometry *	pjPrintGeometry;
    } PrintJob;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appPrintDocument(	int				printer,
				const struct PrintJob *		pj );

extern int appCallPrintFunction( struct SimpleOutputStream *	sos,
				const struct PrintJob *		pj );

# ifdef __cplusplus
    }
# endif

#   endif
