/************************************************************************/
/*									*/
/*  Utility routines for making remarks and posing questions.		*/
/*  Implementation for Motif and GTK+ 1 versions.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"
#   include	<guiBase.h>

# if USE_OWN_DIALOGS

#   include	<stdlib.h>
#   include	<stddef.h>

#   include	"appEditApplication.h"
#   include	"appQuestion.h"
#   include	"appGuiResource.h"
#   include	<guiWidgets.h>
#   include	<guiRunDialog.h>
#   include	<utilMemoryBuffer.h>

#   include	<appDebugon.h>

typedef struct QuestionContextResources
    {
    char *		qcrYesText;
    char *		qcrNoText;
    char *		qcrCancelText;
    char *		qcrOkText;
    } QuestionContextResources;

typedef struct QuestionContext
    {
    AppDialog				qcDialog;
    APP_WIDGET				qcPaned;

    APP_WIDGET				qcQuestionWidget;
    APP_WIDGET				qcSubjectWidget;

    APP_WIDGET				qcYesButton;
    APP_WIDGET				qcNoButton;
    APP_WIDGET				qcCancelButton;
    APP_WIDGET				qcOkButton;

    const QuestionContextResources *	qcResources;
    } QuestionContext;

/************************************************************************/
/*									*/
/*  Resource table for various button texts.				*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_QuestionResourceTable[]=
{
    APP_RESOURCE( "messageYesText",
		offsetof(QuestionContextResources,qcrYesText),	"Yes" ),
    APP_RESOURCE( "messageNoText",
		offsetof(QuestionContextResources,qcrNoText),	"No" ),
    APP_RESOURCE( "messageCancelText",
		offsetof(QuestionContextResources,qcrCancelText),"Cancel" ),
    APP_RESOURCE( "messageOkText",
		offsetof(QuestionContextResources,qcrOkText),	"Ok" ),
};

/************************************************************************/
/*									*/
/*  Run a Yes/No/Cancel dialog.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appQuestionOkButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    guiBreakDialog( &(qc->qcDialog), AQDrespOK );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionYesButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    guiBreakDialog( &(qc->qcDialog), AQDrespYES );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionNoButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    guiBreakDialog( &(qc->qcDialog), AQDrespNO );
    return;
    }

static APP_BUTTON_CALLBACK_H( appQuestionCancelButtonPushed, button, voidqc )
    {
    QuestionContext *		qc= (QuestionContext *)voidqc;

    guiBreakDialog( &(qc->qcDialog), AQDrespCANCEL );
    return;
    }

/************************************************************************/
/*									*/
/*  Make a Ok form with one default button.				*/
/*  Make a Yes/No/Cancel form with three buttons.			*/
/*									*/
/************************************************************************/

static APP_WIDGET appQuestionMakeOkRow(	APP_WIDGET		parent,
					QuestionContext *	qc,
					const char *		okText )
    {
    APP_WIDGET		buttonRow;
    const int		heightResizable= 0;
    const int		showAsDefault= 1;
    const int		colspan= 1;

    buttonRow= guiMakeRowInColumn( parent, 1, heightResizable );

    guiMakeButtonInRow( &(qc->qcOkButton), buttonRow, okText,
				appQuestionOkButtonPushed, (void *)qc,
				0, colspan, showAsDefault );

    return buttonRow;
    }

static APP_WIDGET appQuestionMakeYesNoCancelRow( APP_WIDGET	parent,
					QuestionContext *	qc,
					const char *		yesText,
					const char *		noText,
					const char *		cancelText )
    {
    APP_WIDGET		buttonRow;
    const int		heightResizable= 0;
    const int		showYesAsDefault= 1;
    const int		showOtherAsDefault= 0;
    const int		colspan= 1;

    buttonRow= guiMakeRowInColumn( parent, 3, heightResizable );

    guiMakeButtonInRow( &(qc->qcYesButton), buttonRow, yesText,
				appQuestionYesButtonPushed,
				(void *)qc, 0, colspan, showYesAsDefault );

    guiMakeButtonInRow( &(qc->qcNoButton), buttonRow, noText,
				appQuestionNoButtonPushed,
				(void *)qc, 1, colspan, showOtherAsDefault );

    guiMakeButtonInRow( &(qc->qcCancelButton), buttonRow, cancelText,
				appQuestionCancelButtonPushed,
				(void *)qc, 2, colspan, showOtherAsDefault );

    guiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcYesButton );
    guiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcCancelButton );

    return buttonRow;
    }

static APP_WIDGET appQuestionMakeOkCancelRow( APP_WIDGET	parent,
					QuestionContext *	qc,
					const char *		okText,
					const char *		cancelText )
    {
    APP_WIDGET		buttonRow;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;
    const int		colspan= 1;

    buttonRow= guiMakeRowInColumn( parent, 2, heightResizable );

    guiMakeButtonInRow( &(qc->qcOkButton), buttonRow, okText,
				appQuestionOkButtonPushed,
				(void *)qc, 0, colspan, showAsDefault );

    guiMakeButtonInRow( &(qc->qcCancelButton), buttonRow, cancelText,
				appQuestionCancelButtonPushed,
				(void *)qc, 1, colspan, showAsDefault );

    guiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcOkButton );
    guiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcCancelButton );

    return buttonRow;
    }


static APP_WIDGET appQuestionMakeYesNoRow( APP_WIDGET		parent,
					QuestionContext *	qc,
					const char *		yesText,
					const char *		noText )
    {
    APP_WIDGET		buttonRow;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;
    const int		colspan= 1;

    buttonRow= guiMakeRowInColumn( parent, 2, heightResizable );

    guiMakeButtonInRow( &(qc->qcYesButton), buttonRow, yesText,
				    appQuestionYesButtonPushed,
				    (void *)qc, 0, colspan, showAsDefault );

    guiMakeButtonInRow( &(qc->qcNoButton), buttonRow, noText,
				    appQuestionNoButtonPushed,
				    (void *)qc, 1, colspan, showAsDefault );

    guiSetDefaultButtonForDialog( &(qc->qcDialog), qc->qcYesButton );
    guiSetCancelButtonForDialog( &(qc->qcDialog), qc->qcNoButton );

    return buttonRow;
    }

/************************************************************************/
/*									*/
/*  Make the framework of a Question Dialog.				*/
/*									*/
/************************************************************************/

static int appMakeQuestionDialog( 	EditApplication *	ea,
					QuestionContext **	pQc,
					APP_WIDGET *		pPaned )
    {
    APP_WIDGET		paned;

    QuestionContext *	qc;

    static int				gotResources= 0;
    static QuestionContextResources	qcr;

    qc= (QuestionContext *)malloc( sizeof(QuestionContext) );
    if  ( ! qc )
	{ XDEB(qc); return -1;	}

    qc->qcQuestionWidget= (APP_WIDGET)0;
    qc->qcSubjectWidget= (APP_WIDGET)0;
    qc->qcYesButton= (APP_WIDGET)0;
    qc->qcNoButton= (APP_WIDGET)0;
    qc->qcCancelButton= (APP_WIDGET)0;
    qc->qcOkButton= (APP_WIDGET)0;
    qc->qcResources= &qcr;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&qcr,
				    APP_QuestionResourceTable,
				    sizeof(APP_QuestionResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    guiMakeVerticalDialog( &(qc->qcDialog), &paned, ea->eaToplevel.atTopWidget,
						(APP_CLOSE_CALLBACK_T)0,
						(APP_DESTROY_CALLBACK_T)0,
						(void *)qc );

    *pQc= qc;
    *pPaned= paned;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a subject and a question in a form.				*/
/*									*/
/************************************************************************/

static void appQuestionMakeSubjectQuestion( QuestionContext *	qc,
					APP_WIDGET		paned,
					const char *		subject,
					const char *		question )
    {
    guiMakeLabelInColumn( &(qc->qcSubjectWidget), paned, subject );
    guiMakeLabelInColumn( &(qc->qcQuestionWidget), paned, question );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a subject and a question in a form.				*/
/*									*/
/************************************************************************/

static void appQuestionMakeQuestion(	QuestionContext *	qc,
					APP_WIDGET		paned,
					const char *		question )
    {
    guiMakeLabelInColumn( &(qc->qcQuestionWidget), paned, question );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the question/subject of a question dialog.			*/
/*									*/
/************************************************************************/

static void appQuestionChangeSubject(	QuestionContext *	qc,
					const char *		subject )
    {
    if  ( ! qc->qcSubjectWidget )
	{ XDEB(qc->qcSubjectWidget); return;	}

    guiChangeLabelText( qc->qcSubjectWidget, subject );
    }

static void appQuestionChangeQuestion(	QuestionContext *	qc,
					const char *		question )
    {
    if  ( ! qc->qcQuestionWidget )
	{ XDEB(qc->qcQuestionWidget); return;	}

    guiChangeLabelText( qc->qcQuestionWidget, question );
    }

static void appQuestionChangeOkText(	QuestionContext *	qc,
					const char *		okText )
    { guiChangeButtonText( qc->qcOkButton, okText );	}

static void appQuestionChangeYesText(	QuestionContext *	qc,
					const char *		yesText )
    { guiChangeButtonText( qc->qcYesButton, yesText );	}

static void appQuestionChangeNoText(	QuestionContext *	qc,
					const char *		noText )
    { guiChangeButtonText( qc->qcNoButton, noText );	}

static void appQuestionChangeCancelText( QuestionContext *	qc,
					const char *		cancelText )
    { guiChangeButtonText( qc->qcCancelButton, cancelText );	}


static void appQuestionRunDialog(	EditApplication *	ea,
					QuestionContext *	qc,
					APP_WIDGET		relative,
					APP_WIDGET		option )
    {
    guiSetDialogTitle( qc->qcDialog.adTopWidget,
					option, ea->eaApplicationName );

    guiShowDialog( ea->eaContext, &(qc->qcDialog), relative );

    guiRunDialog( &(qc->qcDialog), AQDrespNONE, ea->eaContext );

    guiHideDialog( &(qc->qcDialog) );

    return;
    }

static int appQuestionRunOkDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					QuestionContext *	qc )
    {
    if  ( ! qc->qcOkButton )
	{ XDEB(qc->qcOkButton); return -1;	}

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyYesNoCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					QuestionContext *	qc )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeYesText( qc, qcr->qcrYesText );
    appQuestionChangeNoText( qc, qcr->qcrNoText );
    appQuestionChangeCancelText( qc, qcr->qcrCancelText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyOkCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					QuestionContext *	qc )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeOkText( qc, qcr->qcrOkText );
    appQuestionChangeCancelText( qc, qcr->qcrCancelText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

static int appQuestionRunAnyYesNoDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					QuestionContext *	qc )
    {
    const QuestionContextResources *	qcr= qc->qcResources;

    appQuestionChangeYesText( qc, qcr->qcrYesText );
    appQuestionChangeNoText( qc, qcr->qcrNoText );

    appQuestionRunDialog( ea, qc, relative, option );

    return qc->qcDialog.adResponse;
    }

int appQuestionRunSubjectYesNoCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question )
    {
    int				rval;

    APP_WIDGET				paned;
    APP_WIDGET				sep;
    const QuestionContextResources *	qcr;


    QuestionContext *		subjectYesNoCancelContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, question );
	return AQDrespCANCEL;
	}

    if  ( appMakeQuestionDialog( ea, &subjectYesNoCancelContext, &paned ) )
	{ LDEB(1); return AQDrespFAILURE;	}

    qcr= subjectYesNoCancelContext->qcResources;

    appQuestionMakeSubjectQuestion( subjectYesNoCancelContext, paned,
						    subject, question );

    guiInsertSeparatorInColumn( &sep, paned );

    (void) /* buttonRow= */ appQuestionMakeYesNoCancelRow( paned,
				subjectYesNoCancelContext,
				qcr->qcrYesText,
				qcr->qcrNoText,
				qcr->qcrCancelText );

    rval= appQuestionRunAnyYesNoCancelDialog( ea, relative, option,
						subjectYesNoCancelContext );

    guiDestroyShellWidget( subjectYesNoCancelContext->qcDialog.adTopWidget );

    if  ( subjectYesNoCancelContext )
	{ free( subjectYesNoCancelContext );	}

    return rval;
    }

int appQuestionRunYesNoCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question )
    {
    int			rval;

    QuestionContext *	yesNoCancelContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    if  ( ! yesNoCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &yesNoCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= yesNoCancelContext->qcResources;

	appQuestionMakeQuestion( yesNoCancelContext, paned, question );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeYesNoCancelRow( paned,
					yesNoCancelContext,
					qcr->qcrYesText,
					qcr->qcrNoText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( yesNoCancelContext, question );
	}

    rval= appQuestionRunAnyYesNoCancelDialog( ea, relative, option,
					yesNoCancelContext );

    guiDestroyShellWidget( yesNoCancelContext->qcDialog.adTopWidget );
    if  ( yesNoCancelContext )
	{ free( yesNoCancelContext );	}

    return rval;
    }

void appQuestionRunSubjectErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    QuestionContext *	subjectErrorContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return;
	}

    if  ( ! subjectErrorContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectErrorContext, &paned ) )
	    { LDEB(1); return;	}

	qcr= subjectErrorContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectErrorContext, paned,
							subject, message );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeOkRow( paned, subjectErrorContext,
							qcr->qcrOkText );
	}
    else{
	const QuestionContextResources *	qcr= subjectErrorContext->qcResources;

	appQuestionChangeSubject( subjectErrorContext, subject );
	appQuestionChangeQuestion( subjectErrorContext, message );
	appQuestionChangeOkText( subjectErrorContext,
					    qcr->qcrOkText );
	}

    appQuestionRunOkDialog( ea, relative, option, subjectErrorContext );

    guiDestroyShellWidget( subjectErrorContext->qcDialog.adTopWidget );
    if  ( subjectErrorContext )
	{ free( subjectErrorContext );	}
    }

int appQuestionRunOkCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question )
    {
    int			rval;

    QuestionContext *	okCancelContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    if  ( ! okCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &okCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= okCancelContext->qcResources;

	appQuestionMakeQuestion( okCancelContext, paned, question );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeOkCancelRow( paned,
					okCancelContext,
					qcr->qcrOkText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( okCancelContext, question );
	}

    rval= appQuestionRunAnyOkCancelDialog( ea, relative, option,
							    okCancelContext );

    guiDestroyShellWidget( okCancelContext->qcDialog.adTopWidget );
    if  ( okCancelContext )
	{ free( okCancelContext );	}

    return rval;
    }

int appQuestionRunSubjectOkCancelDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    int			rval;

    QuestionContext *	subjectOkCancelContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return AQDrespCANCEL;
	}

    if  ( ! subjectOkCancelContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectOkCancelContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= subjectOkCancelContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectOkCancelContext, paned,
							subject, message );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeOkCancelRow( paned,
					subjectOkCancelContext,
					qcr->qcrOkText,
					qcr->qcrCancelText );
	}
    else{
	appQuestionChangeQuestion( subjectOkCancelContext, message );
	appQuestionChangeSubject( subjectOkCancelContext, subject );
	}

    rval= appQuestionRunAnyOkCancelDialog( ea, relative, option,
						    subjectOkCancelContext );

    guiDestroyShellWidget( subjectOkCancelContext->qcDialog.adTopWidget );
    if  ( subjectOkCancelContext )
	{ free( subjectOkCancelContext );	}

    return rval;
    }

int appQuestionRunSubjectYesNoDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	subject,
					const char *		message )
    {
    int			rval;

    QuestionContext *	subjectYesNoContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, utilMemoryBufferGetString( subject ), message );
	return AQDrespFAILURE;
	}

    if  ( ! subjectYesNoContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &subjectYesNoContext, &paned ) )
	    { LDEB(1); return AQDrespFAILURE;	}

	qcr= subjectYesNoContext->qcResources;

	appQuestionMakeSubjectQuestion( subjectYesNoContext, paned,
							utilMemoryBufferGetString( subject ), message );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeYesNoRow( paned,
					subjectYesNoContext,
					qcr->qcrYesText,
					qcr->qcrNoText );
	}
    else{
	appQuestionChangeQuestion( subjectYesNoContext, message );
	appQuestionChangeSubject( subjectYesNoContext, utilMemoryBufferGetString( subject ) );
	}

    rval= appQuestionRunAnyYesNoDialog( ea, relative, option,
							subjectYesNoContext );

    guiDestroyShellWidget( subjectYesNoContext->qcDialog.adTopWidget );
    if  ( subjectYesNoContext )
	{ free( subjectYesNoContext );	}

    return rval;
    }

void appQuestionRunErrorDialog(	EditApplication *	ea,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const char *		message )
    {
    QuestionContext *	errorContext= (QuestionContext *)0;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, message );
	return;
	}

    if  ( ! errorContext )
	{
	APP_WIDGET				paned;
	APP_WIDGET				sep;
	const QuestionContextResources *	qcr;

	if  ( appMakeQuestionDialog( ea, &errorContext, &paned ) )
	    { LDEB(1); return;	}

	qcr= errorContext->qcResources;

	appQuestionMakeQuestion( errorContext, paned, message );

	guiInsertSeparatorInColumn( &sep, paned );

	(void) /* buttonRow= */ appQuestionMakeOkRow( paned,
				    errorContext, qcr->qcrOkText );
	}
    else{
	const QuestionContextResources *	qcr= errorContext->qcResources;

	appQuestionChangeQuestion( errorContext, message );
	appQuestionChangeOkText( errorContext, qcr->qcrOkText );
	}

    appQuestionRunOkDialog( ea, relative, option, errorContext );

    guiDestroyShellWidget( errorContext->qcDialog.adTopWidget );
    if  ( errorContext )
	{ free( errorContext );	}

    return;
    }

# endif
