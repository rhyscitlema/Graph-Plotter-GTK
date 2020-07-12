#ifndef _USER_MAIN_GUI_H
#define _USER_MAIN_GUI_H
/*
	main.h
*/

#include <gtk/gtk.h>

extern GtkWidget     *gui_main_window;

extern GtkTextBuffer *gui_main_text_buffer;
extern GtkTextBuffer *gui_mesg_text_buffer;

extern GtkTextBuffer *gui_path_text_buffer;
extern GtkTextBuffer *gui_time_text_buffer;

extern GtkWidget     *gui_pause_button;
extern GtkWidget     *gui_forward_button;

extern GtkTextBuffer *gui_calc_input_buffer;
extern GtkTextBuffer *gui_calc_result_buffer;

gboolean keyboard_input_event (GtkWidget* widget, GdkEvent *event, void* notused);

#endif
