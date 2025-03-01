/************************************************************************/
/*									*/
/*  Translation of GUI Resource primitives to the actual implementing	*/
/*  GUI system. As we do not have the pretentence to introduce our own	*/
/*  GUI layer, as much as possible is done through macros. (#defines)	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_RESOURCE_H
#   define	APP_GUI_RESOURCE_H

#   if USE_GTK
#	define USE_OWN_RESOURCE	1
#   else
#	define USE_OWN_RESOURCE	1
#   endif

#   if USE_OWN_RESOURCE == 0
#   include	<X11/Xlib.h>
#   include	<X11/Intrinsic.h>
#   endif

struct EditApplication;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Resource table entries. In order no to complicate things, all	*/
/*  values are strings. No attempt to make a fully generalised		*/
/*  abstracted universal solution for calling strtol() or strtod()	*/
/*  occasionally is made.						*/
/*									*/
/*  NO LONGER USE THE ARCHAIC X11 RESOURCE MECHANISM			*/
/*									*/
/************************************************************************/

#   if USE_OWN_RESOURCE == 1

typedef struct AppConfigurableResource
    {
    const char *	acrResourceName;
    unsigned int	acrStructOffset;
    const char *	acrDefaultValue;
    } AppConfigurableResource;

#   define	APP_RESOURCE( x, o, d )					\
				{ (x), (o), (d) }

#   define	APP_SET_RESOURCE( acr, x, o, d )			\
				{					\
				(acr)->acrResourceName= (x);		\
				(acr)->acrStructOffset= (o);		\
				(acr)->acrDefaultValue= (d);		\
				}

#   endif

#   if USE_OWN_RESOURCE == 0

    typedef XtResource AppConfigurableResource;

#   define	acrResourceName	resource_name
#   define	acrStructOffset	resource_offset
#   define	acrDefaultValue	default_addr

#   define	APP_RESOURCE( x, o, d )					\
				{					\
				(char *)(x),(char *)(x),		\
				XtRString, sizeof(char *),		\
				(o),					\
				XtRString, (char *)(d),			\
				}

#   define	APP_SET_RESOURCE( acr, x, o, d )			\
				{					\
				(acr)->resource_name= (char *)(x);	\
				(acr)->resource_class= (char *)(x);	\
				(acr)->resource_type= XtRString;	\
				(acr)->resource_size= sizeof(char *);	\
				(acr)->resource_offset= (o);		\
				(acr)->default_type= XtRString;		\
				(acr)->default_addr= (char *)(d);	\
				}

#   endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char * appGetProperty(
				struct EditApplication *	ea,
				const char *			name );

extern void appGuiGetResourceValues(
				int *				pGotResources,
				struct EditApplication *	ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount );

extern void appSetResourceDefaults(
				struct EditApplication *	ea,
				AppConfigurableResource *	acr,
				int				acrCount );

extern int appReadSystemProperties(
				struct EditApplication *	ea );

extern int appReadUserProperties(
				struct EditApplication *	ea );

extern int appSetUserProperty(	struct EditApplication *	ea,
				const char *			name,
				const char *			value );

extern int appSetSystemProperty( struct EditApplication *	ea,
				const char *			name,
				const char *			value );

extern int appReadOverrideProperties(
				struct EditApplication *	ea,
				const struct MemoryBuffer *	filename );

/************************************************************************/
/*									*/
#   endif	/* APP_GUI_RESOURCE_H */
