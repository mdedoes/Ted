/************************************************************************/
/*									*/
/*  Switch between inspector subjects and a color chooser.		*/
/*									*/
/************************************************************************/

#   include	"appToolsConfig.h"

#   include	<stddef.h>

#   include	"appInspector.h"
#   include	"appInspectorSubject.h"
#   include	"appRgbChooserPage.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Callback of a possible RGB chooser to an inspector.			*/
/*									*/
/************************************************************************/

void appInspectorGotColor(	AppInspector *			ai,
				int				subjectPage,
				int				property,
				const RGB8Color *		rgb8Set )
    {
    RgbChooser *	rcp;
    InspectorSubject *	is;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooser *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ subjectPage;

    if  ( subjectPage < 0 || subjectPage >= ai->aiSubjectCount )
	{ LLDEB(subjectPage,ai->aiSubjectCount); return;	}

    /**/
    if  ( rgb8Set )
	{
	if  ( ! is->isSubjectType || ! is->isSubjectType->istGotColor )
	    { XDEB(is->isSubjectType);					}
	else{
	    (*is->isSubjectType->istGotColor)(
				    is->isTool, property, rgb8Set );
	    }
	}

    /**/
    appEnableInspectorSubject( ai, subjectPage, 1 );
    appInspectorSelectSubject( ai, subjectPage );

    /**/
    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 0 );
    appRgbChooserSetContext( rcp, -1, -1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the RGB color chooser page.				*/
/*									*/
/************************************************************************/

void appInspectorSetRgbPage(		AppInspector *		ai,
					void *			vrcp,
					int			subjectNumber )
    {
    ai->aiRgbChooser= vrcp;
    ai->aiRgbSubjectNumber= subjectNumber;

    return;
    }

void appInspectorShowRgbPage(		AppInspector *		ai,
					int			fromSubject,
					int			fromProperty,
					const RGB8Color *	rgb8 )
    {
    RgbChooser *	rcp;
    InspectorSubject *	is;

    int			enabled= 1;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooser *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ ai->aiRgbSubjectNumber;

    appRgbChooserRefresh( rcp, &enabled, is, rgb8 );

    appRgbChooserSetContext( rcp, fromSubject, fromProperty );

    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 1 );

    appInspectorSelectSubject( ai, ai->aiRgbSubjectNumber );

    return;
    }

