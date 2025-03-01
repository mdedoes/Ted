/************************************************************************/
/*									*/
/*  Management of a document window.					*/
/*  (Internal use)							*/
/*									*/
/************************************************************************/

# ifndef	APP_DOCUMENT_H
# define	APP_DOCUMENT_H

# ifdef __cplusplus
extern "C"
    {
# endif

struct EditDocument;

#   define	SCROLL_WHEEL_STEP	10
#   define	SCROLL_BAR_STEP		20

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMakeDocumentWidget(
				struct EditDocument *		ed );

extern int appFinishDocumentSetup(
				struct EditDocument *		ed );

extern int appFinishDocumentWindow(
				struct EditDocument *		ed );

extern void appDocumentRulerWidth(
				struct EditDocument *		ed );

extern void appCloseDocument(	struct EditDocument *		ed );

extern void appDocSetHorizontalScrollbarValues(
				struct EditDocument *		ed,
				int				minimum,
				int				maximum,
				int				value,
				int				sliderSize );

extern void appDocSetVerticalScrollbarValues(
				struct EditDocument *		ed,
				int				minimum,
				int				maximum,
				int				value,
				int				sliderSize );

# ifdef __cplusplus
    }
# endif

# endif	/*	APP_DOCUMENT_H	*/
