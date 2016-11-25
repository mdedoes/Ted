/************************************************************************/
/*									*/
/*  Menu building GTK specific code.					*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	"guiWidgets.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiBase.h"
#   include	"guiMenuItem.h"
#   include	<appDebugon.h>

#   if USE_GTK

#   include	<gdk/gdkkeysyms.h>
#   include	"guiKeysGtk.h"

static int guiGtkMenuParseKey(	const char *		key,
				unsigned int *		pKeyMask,
				unsigned int *		pKeyCode )
    {
    const char *	s= key;
    unsigned int	keyMask= 0;
    unsigned int	keyCode= 0;
    int			got= 0;

    for (;;)
	{
	while( isspace( *s ) )
	    { s++;	}
	if  ( ! *s )
	    { break;	}

	if  ( ! strncmp( s, "Ctrl", 4 ) )
	    { keyMask |= GDK_CONTROL_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Shift", 5 ) )
	    { keyMask |= GDK_SHIFT_MASK; s += 5; continue;	}
	if  ( ! strncmp( s, "Lock", 4 ) )
	    { keyMask |= GDK_LOCK_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod1", 4 ) )
	    { keyMask |= GDK_MOD1_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod2", 4 ) )
	    { keyMask |= GDK_MOD2_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod3", 4 ) )
	    { keyMask |= GDK_MOD3_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod4", 4 ) )
	    { keyMask |= GDK_MOD4_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod5", 4 ) )
	    { keyMask |= GDK_MOD5_MASK; s += 4; continue;	}

	break;
	}

    if  ( strncmp( s, "<Key>", 5 ) )
	{ SDEB(key); return -1;	}
    s += 5;
    while( isspace( *s ) )
	{ s++;	}

    if  ( ! got && strlen( s ) == 1 )
	{ keyCode= s[0]; got= 1;	}

    if  ( ! got && ! strcmp( s, "F1" ) )
	{ keyCode= KEY_F1; got= 1;	}
    if  ( ! got && ! strcmp( s, "F2" ) )
	{ keyCode= KEY_F2; got= 1;	}
    if  ( ! got && ! strcmp( s, "F3" ) )
	{ keyCode= KEY_F3; got= 1;	}
    if  ( ! got && ! strcmp( s, "F4" ) )
	{ keyCode= KEY_F4; got= 1;	}
    if  ( ! got && ! strcmp( s, "F5" ) )
	{ keyCode= KEY_F5; got= 1;	}
    if  ( ! got && ! strcmp( s, "F6" ) )
	{ keyCode= KEY_F6; got= 1;	}
    if  ( ! got && ! strcmp( s, "F7" ) )
	{ keyCode= KEY_F7; got= 1;	}
    if  ( ! got && ! strcmp( s, "F8" ) )
	{ keyCode= KEY_F8; got= 1;	}
    if  ( ! got && ! strcmp( s, "F9" ) )
	{ keyCode= KEY_F9; got= 1;	}
    if  ( ! got && ! strcmp( s, "F10" ) )
	{ keyCode= KEY_F10; got= 1;	}
    if  ( ! got && ! strcmp( s, "F11" ) )
	{ keyCode= KEY_F11; got= 1;	}
    if  ( ! got && ! strcmp( s, "F12" ) )
	{ keyCode= KEY_F12; got= 1;	}

    if  ( ! got )
	{ SDEB(key); return -1;	}

    *pKeyMask= keyMask; *pKeyCode= keyCode; return 0;
    }

APP_WIDGET guiSetMenuItem(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    GtkWidget *	rval=	gtk_menu_item_new_with_label( ami->amiText );

    if  ( ami->amiKey && ami->amiKey[0] )
	{
	unsigned int	keyMask= 0;
	unsigned int	keyCode;

	if  ( guiGtkMenuParseKey( ami->amiKey, &keyMask, &keyCode ) )
	    { SDEB(ami->amiKey);	}
	else{
	    gtk_widget_add_accelerator( rval, "activate", at->atAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
	    }
	}

    if  ( ami->amiCallback )
	{
	g_signal_connect( G_OBJECT( rval ),
			"activate", (GCallback)ami->amiCallback, target );
	}

    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), rval );
    gtk_widget_show( rval );

    return rval;
    }

APP_WIDGET guiSetMenuSeparator(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    APP_WIDGET		rval;

    rval= gtk_menu_item_new();

    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), rval );
    gtk_widget_show( rval );

    return rval;
    }	    

APP_WIDGET guiSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target )
    {
    GtkWidget *	rval=	gtk_check_menu_item_new_with_label( ami->amiText );

    if  ( ami->amiKey )
	{
	unsigned int	keyMask= 0;
	unsigned int	keyCode;

	if  ( guiGtkMenuParseKey( ami->amiKey, &keyMask, &keyCode ) )
	    { SDEB(ami->amiKey);	}
	else{
	    gtk_widget_add_accelerator( rval, "activate", at->atAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
	    }
	}

    gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( rval ),
					ami->amiItemType == ITEMtyTOGGLE_ON );

    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), rval );
    gtk_widget_show( rval );

    if  ( ami->amiCallback )
	{
	g_signal_connect( G_OBJECT( rval ),
			"toggled", (GCallback)ami->amiCallback, target );
	}

    return rval;
    }

APP_WIDGET guiMakeMenuInParent(	APP_WIDGET *		pButton,
				AppToplevel *		at,
				APP_WIDGET		menuBar,
				const AppMenu *		am )
    {
    GtkWidget *			menu;
    GtkWidget *			button;
    char *			scratch= (char *)0;

    if  ( am->amMnemonic && am->amMnemonic[0] )
	{
	char *	p= strchr( am->amText, am->amMnemonic[0] );
	int	l= strlen( am->amText );

	if  ( ! p )
	    { SSDEB(am->amText,am->amMnemonic);	}
	else{
	    scratch= malloc( l+ 2 );
	    if  ( ! scratch )
		{ XDEB(scratch);	}
	    else{
		int	t= p- am->amText;

		strncpy( scratch, am->amText, t );
		scratch[t]= '_';
		strcpy( scratch+ t+ 1, am->amText+ t );
		}
	    }
	}

    menu= gtk_menu_new();

    if  ( scratch )
	{ button= gtk_menu_item_new_with_mnemonic( scratch );	}
    else{ button= gtk_menu_item_new_with_label( am->amText );	}

    gtk_menu_shell_append( GTK_MENU_SHELL( menuBar ), button );

    gtk_menu_item_set_submenu( GTK_MENU_ITEM( button ), menu );

    if  ( am->amIsHelp )
	{
#	if GTK_MAJOR_VERSION < 2
	gtk_menu_item_set_placement( GTK_MENU_ITEM( button ),
						    GTK_DIRECTION_RIGHT );
#	else
#	if GTK_MAJOR_VERSION < 3
	gtk_menu_item_set_right_justified( GTK_MENU_ITEM( button ), TRUE );
#	else
	gtk_widget_set_halign( GTK_WIDGET( button ), GTK_ALIGN_END );
#	endif
#	endif
	}

    gtk_widget_show( button );

    if  ( scratch )
	{ free( scratch );	}

    *pButton= button;

    return menu;
    }

void guiSetToggleItemState(	APP_WIDGET		toggle,
				int			set )
    {
    gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( toggle ), set != 0 );
    }

void guiSetToggleItemLabel(	APP_WIDGET		toggle,
				const char *		label )
    {
    guiGtkSetChildLabel( toggle, label );

    return;
    }

int guiGetMenuToggleStateFromCallbackGtk( APP_WIDGET		toggle )
    {
    GtkCheckMenuItem *	option= GTK_CHECK_MENU_ITEM( toggle );

    return gtk_check_menu_item_get_active( option );
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve the text from menu options.				*/
/*  2)  Free the result obtained in this way. (I.E: Do not free)	*/
/*									*/
/************************************************************************/

/*  1  */
char *	guiGetTextFromMenuOption(	APP_WIDGET		w )
    {
    static char		slabel[]= "<< ? >>";
    const char *	label= slabel;

    if  ( ! guiGtkGetChildLabel( &label, w ) )
	{ SDEB(label);	}

    return (char *)label;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

/*  2  */
void guiFreeTextFromMenuOption(	char *		s )
    { return;	}

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Hide a menu option.							*/
/*									*/
/************************************************************************/

void guiShowMenuOption(		APP_WIDGET		w,
				int			visible )
    {
    if  ( visible )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}
    }

#   endif
