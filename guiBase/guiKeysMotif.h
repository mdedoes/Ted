/************************************************************************/
/*									*/
/*  Defines for the key names: Map to plain X11 names.			*/
/*									*/
/************************************************************************/

# if USE_MOTIF

#    include	<X11/keysym.h>

#  ifdef XK_ISO_Left_Tab
#   define KEY_ISO_Left_Tab	XK_ISO_Left_Tab
#  endif
#   define KEY_i		XK_i
#   define KEY_Tab		XK_Tab
#   define KEY_j		XK_j
#   define KEY_m		XK_m
#   define KEY_KP_Enter		XK_KP_Enter
#   define KEY_Return		XK_Return
#   define KEY_KP_Delete	XK_KP_Delete

#   define KEY_Delete		XK_Delete
#   define KEY_BackSpace	XK_BackSpace

#   define KEY_0		XK_0
#   define KEY_1		XK_1
#   define KEY_2		XK_2
#   define KEY_3		XK_3
#   define KEY_4		XK_4
#   define KEY_5		XK_5
#   define KEY_6		XK_6
#   define KEY_7		XK_7
#   define KEY_8		XK_8
#   define KEY_9		XK_9

#   define KEY_KP_0		XK_KP_0
#   define KEY_KP_1		XK_KP_1
#   define KEY_KP_2		XK_KP_2
#   define KEY_KP_3		XK_KP_3
#   define KEY_KP_4		XK_KP_4
#   define KEY_KP_5		XK_KP_5
#   define KEY_KP_6		XK_KP_6
#   define KEY_KP_7		XK_KP_7
#   define KEY_KP_8		XK_KP_8
#   define KEY_KP_9		XK_KP_9

#   define KEY_KP_Home		XK_KP_Home
#   define KEY_Home		XK_Home
#   define KEY_KP_End		XK_KP_End
#   define KEY_End		XK_End
#   define KEY_KP_Left		XK_KP_Left
#   define KEY_Left		XK_Left
#   define KEY_KP_Right		XK_KP_Right
#   define KEY_Right		XK_Right
#   define KEY_KP_Up		XK_KP_Up
#   define KEY_Up		XK_Up
#   define KEY_KP_Down		XK_KP_Down
#   define KEY_Down		XK_Down
#   define KEY_KP_Prior		XK_KP_Prior
#   define KEY_Prior		XK_Prior
#   define KEY_KP_Next		XK_KP_Next
#   define KEY_Next		XK_Next
#   define KEY_c		XK_c
#   define KEY_x		XK_x
#   define KEY_v		XK_v
#   define KEY_y		XK_y
#   define KEY_Shift_L		XK_Shift_L
#   define KEY_Shift_R		XK_Shift_R
#   define KEY_Alt_L		XK_Alt_L
#   define KEY_Alt_R		XK_Alt_R
#   define KEY_Control_L	XK_Control_L
#   define KEY_Control_R	XK_Control_R
#   define KEY_Caps_Lock	XK_Caps_Lock
#   define KEY_Insert		XK_Insert
#   define KEY_KP_Insert	XK_KP_Insert
#   define KEY_Num_Lock		XK_Num_Lock

#   define MOUSE_BUTTON_FROM_EVENT(e) (e)->xbutton.button
#   define MOUSE_BUTTON_1	Button1
#   define MOUSE_BUTTON_2	Button2
#   define MOUSE_BUTTON_3	Button3
#   define MOUSE_BUTTON_4	Button4
#   define MOUSE_BUTTON_5	Button5

#   define SCROLL_DIRECTION_FROM_EVENT(e) (e)->xbutton.button
#   define SCROLL_UP		Button4
#   define SCROLL_DOWN		Button5

# endif

