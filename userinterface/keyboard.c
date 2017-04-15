/*
    keyboard.c
*/
#include "main.h"
#include <mouse.h>


static int GDK_KEY_TO_MFET_KEY (int KEY)
{
    if(('0'<=KEY && KEY<='9')
    || ('A'<=KEY && KEY<='Z')
    || ('a'<=KEY && KEY<='z'))
        return KEY;

    switch(KEY)
    {
    case GDK_KEY_Return:    return Key_Enter;
    case GDK_KEY_Escape:    return Key_Escape;
    case GDK_KEY_Delete:    return Key_Delete;
    case GDK_KEY_space:     return Key_Space;
    case GDK_KEY_BackSpace: return Key_Backspace;

    case GDK_KEY_Print:     return Key_Print;
    case GDK_KEY_Break:     return Key_Break; // Key_Pause
    case GDK_KEY_Insert:    return Key_Insert;
    case GDK_KEY_Home:      return Key_Home;
    case GDK_KEY_Page_Up:   return Key_PageUp;
    case GDK_KEY_Page_Down: return Key_PageDown;
    case GDK_KEY_End:       return Key_End;
    case GDK_KEY_Tab:       return Key_Tab;
    case GDK_KEY_Menu:      return Key_Menu;

    case GDK_KEY_Num_Lock:    return Key_NumLock;
    case GDK_KEY_Scroll_Lock: return Key_ScrollLock;
    case GDK_KEY_Caps_Lock:   return Key_CapsLock;

    case GDK_KEY_Control_L: return Key_Ctrl_Left;
    case GDK_KEY_Control_R: return Key_Ctrl_Right;
    case GDK_KEY_Alt_L:     return Key_Alt_Left;
    case GDK_KEY_Alt_R:     return Key_Alt_Right;
    case GDK_KEY_Shift_L:   return Key_Shift_Left;
    case GDK_KEY_Shift_R:   return Key_Shift_Right;

    case GDK_KEY_Up:        return Key_Up;
    case GDK_KEY_Down:      return Key_Down;
    case GDK_KEY_Left:      return Key_Left;
    case GDK_KEY_Right:     return Key_Right;

    case GDK_KEY_F1:        return Key_F1;
    case GDK_KEY_F2:        return Key_F2;
    case GDK_KEY_F3:        return Key_F3;
    case GDK_KEY_F4:        return Key_F4;
    case GDK_KEY_F5:        return Key_F5;
    case GDK_KEY_F6:        return Key_F6;
    case GDK_KEY_F7:        return Key_F7;
    case GDK_KEY_F8:        return Key_F8;
    case GDK_KEY_F9:        return Key_F9;
    case GDK_KEY_F10:       return Key_F10;
    case GDK_KEY_F11:       return Key_F11;
    case GDK_KEY_F12:       return Key_F12;

    default: return 0;
    }
}


gboolean keyboard_input_event (GtkWidget* widget, GdkEvent *event, void* notused)
{
    int key = GDK_KEY_TO_MFET_KEY(((GdkEventKey*)event)->keyval);
    bool pressed = event->type == GDK_KEY_PRESS;
    return on_key_event(key, pressed);
}
