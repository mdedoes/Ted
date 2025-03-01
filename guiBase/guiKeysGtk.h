/************************************************************************/
/*									*/
/*  Defines for the key names: Map to GTK names.			*/
/*									*/
/************************************************************************/

# if USE_GTK

#   include	<gtk/gtk.h>
#   include	<gdk/gdkkeysyms.h>

# if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 22

#   define KEY_0		GDK_0
#   define KEY_1		GDK_1
#   define KEY_2		GDK_2
#   define KEY_3		GDK_3
#   define KEY_4		GDK_4
#   define KEY_5		GDK_5
#   define KEY_6		GDK_6
#   define KEY_7		GDK_7
#   define KEY_8		GDK_8
#   define KEY_9		GDK_9

#   define KEY_F1		GDK_F1
#   define KEY_F2		GDK_F2
#   define KEY_F3		GDK_F3
#   define KEY_F4		GDK_F4
#   define KEY_F5		GDK_F5
#   define KEY_F6		GDK_F6
#   define KEY_F7		GDK_F7
#   define KEY_F8		GDK_F8
#   define KEY_F9		GDK_F9
#   define KEY_F10		GDK_F10
#   define KEY_F11		GDK_F11
#   define KEY_F12		GDK_F12

#   define KEY_KP_0		GDK_KP_0
#   define KEY_KP_1		GDK_KP_1
#   define KEY_KP_2		GDK_KP_2
#   define KEY_KP_3		GDK_KP_3
#   define KEY_KP_4		GDK_KP_4
#   define KEY_KP_5		GDK_KP_5
#   define KEY_KP_6		GDK_KP_6
#   define KEY_KP_7		GDK_KP_7
#   define KEY_KP_8		GDK_KP_8
#   define KEY_KP_9		GDK_KP_9

#   define KEY_ISO_Left_Tab	GDK_ISO_Left_Tab
#   define KEY_i		GDK_i
#   define KEY_Tab		GDK_Tab
#   define KEY_j		GDK_j
#   define KEY_m		GDK_m
#   define KEY_KP_Enter		GDK_KP_Enter
#   define KEY_Return		GDK_Return
#   define KEY_KP_Delete	GDK_KP_Delete
#   define KEY_Delete		GDK_Delete
#   define KEY_BackSpace	GDK_BackSpace
#   define KEY_KP_Home		GDK_KP_Home
#   define KEY_Home		GDK_Home
#   define KEY_KP_End		GDK_KP_End
#   define KEY_End		GDK_End
#   define KEY_KP_Left		GDK_KP_Left
#   define KEY_Left		GDK_Left
#   define KEY_KP_Right		GDK_KP_Right
#   define KEY_Right		GDK_Right
#   define KEY_KP_Up		GDK_KP_Up
#   define KEY_Up		GDK_Up
#   define KEY_KP_Down		GDK_KP_Down
#   define KEY_Down		GDK_Down
#   define KEY_KP_Prior		GDK_KP_Prior
#   define KEY_Prior		GDK_Prior
#   define KEY_KP_Next		GDK_KP_Next
#   define KEY_Next		GDK_Next
#   define KEY_c		GDK_c
#   define KEY_x		GDK_x
#   define KEY_v		GDK_v
#   define KEY_y		GDK_y
#   define KEY_Shift_L		GDK_Shift_L
#   define KEY_Shift_R		GDK_Shift_R
#   define KEY_Alt_L		GDK_Alt_L
#   define KEY_Alt_R		GDK_Alt_R
#   define KEY_Control_L	GDK_Control_L
#   define KEY_Control_R	GDK_Control_R
#   define KEY_Caps_Lock	GDK_Caps_Lock
#   define KEY_Insert		GDK_Insert
#   define KEY_KP_Insert	GDK_KP_Insert
#   define KEY_Num_Lock		GDK_Num_Lock
#   define KEY_Hyper_R		GDK_Hyper_R
#   define KEY_Escape		GDK_Escape

# else

#   define KEY_0		GDK_KEY_0
#   define KEY_1		GDK_KEY_1
#   define KEY_2		GDK_KEY_2
#   define KEY_3		GDK_KEY_3
#   define KEY_4		GDK_KEY_4
#   define KEY_5		GDK_KEY_5
#   define KEY_6		GDK_KEY_6
#   define KEY_7		GDK_KEY_7
#   define KEY_8		GDK_KEY_8
#   define KEY_9		GDK_KEY_9

#   define KEY_F1		GDK_KEY_F1
#   define KEY_F2		GDK_KEY_F2
#   define KEY_F3		GDK_KEY_F3
#   define KEY_F4		GDK_KEY_F4
#   define KEY_F5		GDK_KEY_F5
#   define KEY_F6		GDK_KEY_F6
#   define KEY_F7		GDK_KEY_F7
#   define KEY_F8		GDK_KEY_F8
#   define KEY_F9		GDK_KEY_F9
#   define KEY_F10		GDK_KEY_F10
#   define KEY_F11		GDK_KEY_F11
#   define KEY_F12		GDK_KEY_F12

#   define KEY_KP_0		GDK_KEY_KP_0
#   define KEY_KP_1		GDK_KEY_KP_1
#   define KEY_KP_2		GDK_KEY_KP_2
#   define KEY_KP_3		GDK_KEY_KP_3
#   define KEY_KP_4		GDK_KEY_KP_4
#   define KEY_KP_5		GDK_KEY_KP_5
#   define KEY_KP_6		GDK_KEY_KP_6
#   define KEY_KP_7		GDK_KEY_KP_7
#   define KEY_KP_8		GDK_KEY_KP_8
#   define KEY_KP_9		GDK_KEY_KP_9

#   define KEY_ISO_Left_Tab	GDK_KEY_ISO_Left_Tab
#   define KEY_i		GDK_KEY_i
#   define KEY_Tab		GDK_KEY_Tab
#   define KEY_j		GDK_KEY_j
#   define KEY_m		GDK_KEY_m
#   define KEY_KP_Enter		GDK_KEY_KP_Enter
#   define KEY_Return		GDK_KEY_Return
#   define KEY_KP_Delete	GDK_KEY_KP_Delete
#   define KEY_Delete		GDK_KEY_Delete
#   define KEY_BackSpace	GDK_KEY_BackSpace
#   define KEY_KP_Home		GDK_KEY_KP_Home
#   define KEY_Home		GDK_KEY_Home
#   define KEY_KP_End		GDK_KEY_KP_End
#   define KEY_End		GDK_KEY_End
#   define KEY_KP_Left		GDK_KEY_KP_Left
#   define KEY_Left		GDK_KEY_Left
#   define KEY_KP_Right		GDK_KEY_KP_Right
#   define KEY_Right		GDK_KEY_Right
#   define KEY_KP_Up		GDK_KEY_KP_Up
#   define KEY_Up		GDK_KEY_Up
#   define KEY_KP_Down		GDK_KEY_KP_Down
#   define KEY_Down		GDK_KEY_Down
#   define KEY_KP_Prior		GDK_KEY_KP_Prior
#   define KEY_Prior		GDK_KEY_Prior
#   define KEY_KP_Next		GDK_KEY_KP_Next
#   define KEY_Next		GDK_KEY_Next
#   define KEY_c		GDK_KEY_c
#   define KEY_x		GDK_KEY_x
#   define KEY_v		GDK_KEY_v
#   define KEY_y		GDK_KEY_y
#   define KEY_Shift_L		GDK_KEY_Shift_L
#   define KEY_Shift_R		GDK_KEY_Shift_R
#   define KEY_Alt_L		GDK_KEY_Alt_L
#   define KEY_Alt_R		GDK_KEY_Alt_R
#   define KEY_Control_L	GDK_KEY_Control_L
#   define KEY_Control_R	GDK_KEY_Control_R
#   define KEY_Caps_Lock	GDK_KEY_Caps_Lock
#   define KEY_Insert		GDK_KEY_Insert
#   define KEY_KP_Insert	GDK_KEY_KP_Insert
#   define KEY_Num_Lock		GDK_KEY_Num_Lock
#   define KEY_Hyper_R		GDK_KEY_Hyper_R
#   define KEY_Escape		GDK_KEY_Escape

# endif

#   define MOUSE_BUTTON_FROM_EVENT(e) (e)->button.button
#   define MOUSE_BUTTON_1	1
#   define MOUSE_BUTTON_2	2
#   define MOUSE_BUTTON_3	3

# if GTK_MAJOR_VERSION >= 2
#   define SCROLL_DIRECTION_FROM_EVENT(e) (e)->scroll.direction
#   define SCROLL_UP		GDK_SCROLL_UP
#   define SCROLL_DOWN		GDK_SCROLL_DOWN
# else
#   define SCROLL_DIRECTION_FROM_EVENT(e) (e)->button.button
#   define SCROLL_UP		(4)
#   define SCROLL_DOWN		(5)
# endif

# endif

