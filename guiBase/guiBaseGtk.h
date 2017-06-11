/************************************************************************/
/*									*/
/*  GTK definitions for guiBase.h. NEVER INCLUDE DIRECTLY, BUT VIA	*/
/*  guiBase.h								*/
/*									*/
/************************************************************************/

# if USE_GTK	/*  { */

# ifndef USE_GDK_DEPRECATED
#   define	GDK_DISABLE_DEPRECATED
# endif
# ifndef USE_GTK_DEPRECATED
#   define	GTK_DISABLE_DEPRECATED
# endif
#   define	DGTK_DISABLE_SINGLE_INCLUDES
#   define	GSEAL_ENABLE


/************************************************************************/
/*									*/
/*  Basic GUI primitives.						*/
/*									*/
/************************************************************************/

#   define KEY_CONTROL_MASK		GDK_CONTROL_MASK
#   define KEY_SHIFT_MASK		GDK_SHIFT_MASK

# ifdef GTK_TYPE_IM_CONTEXT
    typedef	GtkIMContext *		APP_INPUT_CONTEXT;
# else
    typedef	int			APP_INPUT_CONTEXT;
# endif

typedef		int			APP_CONTEXT;

typedef		int			APP_INPUT_METHOD;
typedef		guint			(APP_KEY_VALUE);

					/********************************/
					/*  Widgets and widget types.	*/
					/********************************/
typedef		GtkWidget		(*APP_WIDGET);

					/********************************/
					/*  Drawing related types.	*/
					/********************************/
# if DRAW_GDK
    typedef		GdkGC			(*APP_GC);
    typedef		GdkDrawable		(*APP_DRAWABLE);
    typedef		GdkColor		(APP_COLOR_RGB);
# endif

# if DRAW_CAIRO
    typedef		cairo_pattern_t		(*APP_COLOR_RGB);
# endif

typedef		GdkWindow		(*APP_WINDOW);
typedef		GdkCursor		(*APP_CURSOR);
typedef		GdkAtom			(APP_ATOM);
typedef		GdkPoint		(APP_POINT);

typedef		GCallback		(APP_CALLBACK_FUNC);
typedef		GCallback		(APP_MENU_CALLBACK);
typedef		GCallback		(APP_DRAW_BUTTON_CALLBACK);

typedef		GdkEvent		(APP_EVENT);
# if GTK_MAJOR_VERSION < 3
typedef		GdkEvent		(APP_DRAW_EVENT);
# else
typedef		cairo_t			(APP_DRAW_EVENT);
# endif
typedef		GdkEventButton		(APP_MOUSE_CLICK_EVENT);

typedef		GtkSelectionData	(APP_SELECTION_EVENT);

/************************************************************************/

typedef void (*APP_GIVE_COPY)(		GtkWidget *		w,
					APP_SELECTION_EVENT *	event,
					guint			info,
					guint			tim,
					void *			through );

typedef void (*APP_PASTE_REPLY)(	GtkWidget *		w,
					APP_SELECTION_EVENT *	gsd,
					guint			tim,
					void *			voided );

#define		APP_GIVE_COPY(n,w,e,t)					\
		void n(			GtkWidget *		w,	\
					APP_SELECTION_EVENT *	e,	\
					guint			info,	\
					guint			tim,	\
					void *			t	)

#define		APP_PASTE_REPLY(n,w,e,t)				\
		void n(			GtkWidget *		w,	\
					APP_SELECTION_EVENT *	e,	\
					guint			tim,	\
					void *			t	)

/****/
#define		APP_TOGGLE_CALLBACK_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TOGGLE_CALLBACK_H ((*APP_TOGGLE_CALLBACK_T),w,t,e);

/****/
#define		APP_SCROLLBAR_CALLBACK_H(n,w,t,e)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_SCROLLBAR_CALLBACK_H ((*APP_SCROLLBAR_CALLBACK_T),w,t,e);

/****/
#define		APP_SLIDER_CALLBACK_H(n,w,t,e)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_SLIDER_CALLBACK_H ((*APP_SLIDER_CALLBACK_T),w,t,e);

/****/
#define		APP_MENU_CALLBACK_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_MENU_CALLBACK_H ((*APP_MENU_CALLBACK_T),w,t,e);

# define guiGetToggleStateFromCallback( w, e ) \
			guiGetToggleStateFromCallbackGtk( (w) )

extern int guiGetToggleStateFromCallbackGtk( GtkWidget * w );

/****/
#define		APP_BUTTON_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_BUTTON_CALLBACK_H ((*APP_BUTTON_CALLBACK_T),w,t);

/****/
#define		APP_EVENT_HANDLER_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)
# if GTK_MAJOR_VERSION < 3
#define		APP_REDRAW_HANDLER_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)
# else
#define		APP_REDRAW_HANDLER_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					cairo_t *		e,	\
					void *			t	)
# endif

typedef		APP_EVENT_HANDLER_H ((*APP_EVENT_HANDLER_T),w,t,e);
typedef		APP_REDRAW_HANDLER_H ((*APP_REDRAW_HANDLER_T),w,t,e);

/****/
#define		APP_CLOSE_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)

typedef		APP_CLOSE_CALLBACK_H ((*APP_CLOSE_CALLBACK_T),w,t);

/****/
#define		APP_TXTYPING_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TXTYPING_CALLBACK_H ((*APP_TXTYPING_CALLBACK_T),w,t);

/****/
#define		APP_TXACTIVATE_CALLBACK_H(n,w,t)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TXACTIVATE_CALLBACK_H ((*APP_TXACTIVATE_CALLBACK_T),w,t);

/****/

typedef		GtkTreeSelection	APP_LIST_CHOICE;

#define		APP_LIST_CALLBACK_H(n,w,t,c)				\
		void n(			GtkWidget *		w,	\
					APP_LIST_CHOICE *	c,	\
					void *			t	)

typedef		APP_LIST_CALLBACK_H ((*APP_LIST_CALLBACK_T),w,t,c);

/****/
# if 0
For "destroy-event" that does not arrive
#define		APP_DESTROY_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)

# else
#define		APP_DESTROY_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_DESTROY_CALLBACK_H ((*APP_DESTROY_CALLBACK_T),w,t);
# endif


typedef struct AppDialog
    {
    GtkWidget *		adTopWidget;
    int			adResponse;
    guint		adCloseId;
    GtkAccelGroup *	adAccelGroup;
    } AppDialog;

/**
 *  Top level widget: (A window)
 */
typedef struct AppToplevel
    {
    GtkWidget *		atTopWidget;
    GtkAccelGroup *	atAccelGroup;
    } AppToplevel;

/*
#   define	COLUMN_SPACING_GTK	1
#   define	WINDOW_BORDER_GTK	4
#   define	FRAME_BORDER_GTK	4
#   define	ROW_XPADDING_GTK	3
#   define	ROW_YPADDING_GTK	2
*/

#   define	COLUMN_SPACING_GTK	1
#   define	WINDOW_BORDER_GTK	4
#   define	FRAME_BORDER_GTK	4
#   define	ROW_XPADDING_GTK	1
#   define	ROW_YPADDING_GTK	0

/************************************************************************/
/*									*/
/*  Line attributes.							*/
/*									*/
/************************************************************************/

#   define	LINEstyleSOLID		GDK_LINE_SOLID
#   define	LINEstyleON_OFF_DASH	GDK_LINE_ON_OFF_DASH
#   define	LINEstyleDOUBLE_DASH	GDK_LINE_DOUBLE_DASH

#   define	LINEcapNOT_LAST		GDK_CAP_NOT_LAST
#   define	LINEcapBUTT		GDK_CAP_BUTT
#   define	LINEcapROUND		GDK_CAP_ROUND
#   define	LINEcapPROJECTING	GDK_CAP_PROJECTING

#   define	LINEjoinMITER		GDK_JOIN_MITER
#   define	LINEjoinROUND		GDK_JOIN_ROUND
#   define	LINEjoinBEVEL		GDK_JOIN_BEVEL

/************************************************************************/
/*									*/
/*  Compatibility nuisance						*/
/*									*/
/************************************************************************/

#if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
#   define gtk_widget_get_window(w) ((w)->window)
#endif

#if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
#   define gtk_widget_get_allocation( w, pa ) *(pa)=((w)->allocation)
#   define gtk_widget_set_allocation( w, pa ) ((w)->allocation)=*(pa)
#endif

#if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
#   define gtk_selection_data_get_data( gsd ) (gsd)->data
#   define gtk_selection_data_get_length( gsd ) (gsd)->length
#   define gtk_selection_data_get_target( gsd ) (gsd)->target
#   define gtk_selection_data_get_selection( gsd ) (gsd)->selection
#   define gtk_selection_data_get_data_type( gsd ) (gsd)->type
#endif

#if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
#   define gtk_adjustment_set_lower( ga, v ) (ga)->lower= (v)
#   define gtk_adjustment_set_upper( ga, v ) (ga)->upper= (v)
#   define gtk_adjustment_set_value( ga, v ) (ga)->value= (v)
#   define gtk_adjustment_get_value( ga ) ((ga)->value)
#   define gtk_adjustment_set_page_size( ga, v ) (ga)->page_size= (v)
#   define gtk_adjustment_get_page_size( ga ) ((ga)->page_size)
#   define gtk_adjustment_set_step_increment( ga, v ) (ga)->step_increment= (v)
#   define gtk_adjustment_set_page_increment( ga, v ) (ga)->page_increment= (v)
#endif

#if (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 12)
#   define gtk_widget_has_grab( w ) GTK_WIDGET_HAS_GRAB( (w) )
#endif

# endif		/*  } USE_GTK */

