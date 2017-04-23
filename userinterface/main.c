/*
    main.c
*/

#include "main.h"
#include "files.h"
#include <tools.h>
#include <timer.h>
#include <userinterface.h>
#include <_texts.h>
#include <_stdio.h>

int main_window_width = 500;
int main_window_height = 500;

GtkWidget *gui_main_window;
GtkWidget *gui_main_text;

GtkWidget *gui_eval_button;
GtkWidget *gui_mesg_text;
GtkWidget *gui_lock_button;

GtkWidget *gui_prev_button;
GtkWidget *gui_next_button;
GtkWidget *gui_delete_button;
GtkWidget *gui_clear_button;
GtkWidget *gui_path_text;

GtkWidget *gui_pause_button;
GtkWidget *gui_forward_button;
GtkWidget *gui_lower_button;
GtkWidget *gui_higher_button;
GtkWidget *gui_time_text;

GtkWidget *gui_calc_button;
GtkWidget *gui_calc_input;
GtkWidget *gui_calc_result;

GtkTextBuffer *gui_main_text_buffer;
GtkTextBuffer *gui_mesg_text_buffer;
GtkTextBuffer *gui_path_text_buffer;
GtkTextBuffer *gui_time_text_buffer;
GtkTextBuffer *gui_calc_input_buffer;
GtkTextBuffer *gui_calc_result_buffer;


static void callback_button (GtkButton *button, gpointer data)
{
    const char* name = gtk_button_get_label(button);

    if(0==strcmp(name, "=")) tools_do_eval(NULL);

    else if(0==strcmp(name, TEXT_PREV    )) tools_get_prev();
    else if(0==strcmp(name, TEXT_NEXT    )) tools_get_next();
    else if(0==strcmp(name, TEXT_DELE    )) tools_do_delete();
    else if(0==strcmp(name, TEXT_CLEAR   )) tools_do_clear();

    else if(0==strcmp(name, TEXT_PAUSE   )) tools_do_pause(true);
    else if(0==strcmp(name, TEXT_RESUME  )) tools_do_pause(false);
    else if(0==strcmp(name, TEXT_FORWARD )) tools_go_forward(true);
    else if(0==strcmp(name, TEXT_BACKWARD)) tools_go_forward(false);
    else if(0==strcmp(name, TEXT_LOWER   )) tools_lower_period();
    else if(0==strcmp(name, TEXT_HIGHER  )) tools_higher_period();

    else if(0==strcmp(name, TEXT_CALC)) calculator_evaluate_calc(true);

    else display_message(CST21(" Unrecognised Action Event in User Interface\n"));
}


#ifdef LIBRODT
static gboolean on_path_text (GtkWidget* widget, GdkEvent *event, gpointer data)
{
    int key = ((GdkEventKey*)event)->keyval;
    if(key!=GDK_KEY_Return) return false;
    tools_do_select();
    return true;
}
#endif

static gboolean on_time_text (GtkWidget* widget, GdkEvent *event, gpointer data)
{
    int key = ((GdkEventKey*)event)->keyval;
    if(key!=GDK_KEY_Return) return false;
    tools_set_time(NULL);
    return true;
}


static GtkWidget *gtk_text_view_in_scrolled_window_new (GtkTextBuffer *text_buffer, gboolean editable, gboolean cursor_visible)
{
    GtkWidget *scrolled_window;
    GtkWidget *text_view;

    /* set text view */
    text_view = gtk_text_view_new_with_buffer (text_buffer);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD); 

    /* set scrolled window */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
                                    GTK_POLICY_AUTOMATIC, 
                                    GTK_POLICY_AUTOMATIC); 
    gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 2);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), editable);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), cursor_visible);

    return scrolled_window;
}


static GtkFixed* fixed;

static inline void do_resize (GtkWidget *widget, int L[4])
{
    gtk_widget_set_size_request (widget, L[2], L[3]);
    gtk_fixed_move(fixed, widget, L[0], L[1]);
}

bool main_window_resize ()
{
    int i=0, layout[20][4];
    if(!tools_uidt_eval(layout, NULL))
    { display_message(errorMessage()); return false; }

    if(!gui_main_text) return true;
    do_resize (gui_main_text       , layout[i++]);

    do_resize (gui_eval_button     , layout[i++]);
    do_resize (gui_mesg_text       , layout[i++]);
    do_resize (gui_lock_button     , layout[i++]);

    #ifdef LIBRODT
    do_resize (gui_prev_button     , layout[i++]);
    do_resize (gui_next_button     , layout[i++]);
    do_resize (gui_delete_button   , layout[i++]);
    do_resize (gui_clear_button    , layout[i++]);
    do_resize (gui_path_text       , layout[i++]);
    #endif

    do_resize (gui_pause_button    , layout[i++]);
    do_resize (gui_forward_button  , layout[i++]);
    do_resize (gui_lower_button    , layout[i++]);
    do_resize (gui_higher_button   , layout[i++]);
    do_resize (gui_time_text       , layout[i++]);

    #ifdef LIBRODT
    do_resize (gui_calc_button     , layout[i++]);
    do_resize (gui_calc_input      , layout[i++]);
    do_resize (gui_calc_result     , layout[i++]);
    #endif

  //do_resize (gui_status_bar      , layout[i++]);
    return true;
}

static void on_main_window_resize (GtkWindow* window)
{
    int XSize, YSize, dx, dy;
    gtk_window_get_size (GTK_WINDOW(gui_main_window), &XSize, &YSize);
    dx = XSize - main_window_width;
    dy = YSize - main_window_height;
    if(dx==0 && dy==0) return;
    main_window_width = XSize;
    main_window_height = YSize;
    main_window_resize();
}

static void getMainWindow()
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Rhyscitlema Graph Plotter 3D");
    gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_file("userinterface/icon.png", NULL));
    //gtk_container_set_border_width(GTK_CONTAINER(window), 2);
    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));
    g_signal_connect_swapped(G_OBJECT(window), "configure-event", G_CALLBACK(on_main_window_resize), window);
    gui_main_window = window;


    fixed = (GtkFixed*)gtk_fixed_new();

    gui_main_text_buffer = gtk_text_buffer_new(NULL);
    gui_main_text = gtk_text_view_in_scrolled_window_new (gui_main_text_buffer, true, true);
    gtk_widget_set_hexpand (gui_main_text, true);
    gtk_widget_set_vexpand (gui_main_text, true);
    gtk_fixed_put(fixed, gui_main_text, 0,0);


    gui_eval_button = gtk_button_new_with_label("=");
    g_signal_connect (G_OBJECT(gui_eval_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_eval_button, 0,0);

    gui_mesg_text_buffer = gtk_text_buffer_new(NULL);
    gui_mesg_text = gtk_text_view_in_scrolled_window_new (gui_mesg_text_buffer, false, false);
    gtk_fixed_put(fixed, gui_mesg_text, 0,0);

    gui_lock_button = gtk_button_new();
    g_signal_connect (G_OBJECT(gui_lock_button), "key-press-event"  , G_CALLBACK(keyboard_input_event), NULL);
    g_signal_connect (G_OBJECT(gui_lock_button), "key-release-event", G_CALLBACK(keyboard_input_event), NULL);
    gtk_fixed_put(fixed, gui_lock_button, 0,0);


    #ifdef LIBRODT
    gui_prev_button = gtk_button_new_with_label(TEXT_PREV);
    g_signal_connect (G_OBJECT(gui_prev_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_prev_button, 0,0);

    gui_next_button = gtk_button_new_with_label(TEXT_NEXT);
    g_signal_connect (G_OBJECT(gui_next_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_next_button, 0,0);

    gui_delete_button = gtk_button_new_with_label(TEXT_DELE);
    g_signal_connect (G_OBJECT(gui_delete_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_delete_button, 0,0);

    gui_clear_button = gtk_button_new_with_label(TEXT_CLEAR);
    g_signal_connect (G_OBJECT(gui_clear_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_clear_button, 0,0);

    gui_path_text_buffer = gtk_text_buffer_new(NULL);
    gui_path_text = gtk_text_view_new_with_buffer (gui_path_text_buffer);
    g_signal_connect (G_OBJECT(gui_path_text), "key-press-event", G_CALLBACK(on_path_text), NULL);
    gtk_fixed_put(fixed, gui_path_text, 0,0);
    #endif


    gui_pause_button = gtk_button_new_with_label(TEXT_RESUME);
    g_signal_connect (G_OBJECT(gui_pause_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_pause_button, 0,0);

    gui_forward_button = gtk_button_new_with_label(TEXT_BACKWARD);
    g_signal_connect (G_OBJECT(gui_forward_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_forward_button, 0,0);

    gui_lower_button = gtk_button_new_with_label(TEXT_LOWER);
    g_signal_connect (G_OBJECT(gui_lower_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_lower_button, 0,0);

    gui_higher_button = gtk_button_new_with_label(TEXT_HIGHER);
    g_signal_connect (G_OBJECT(gui_higher_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_higher_button, 0,0);

    gui_time_text_buffer = gtk_text_buffer_new(NULL);
    gui_time_text = gtk_text_view_new_with_buffer (gui_time_text_buffer);
    g_signal_connect (G_OBJECT(gui_time_text), "key-press-event", G_CALLBACK(on_time_text), NULL);
    gtk_fixed_put(fixed, gui_time_text, 0,0);


    #ifdef LIBRODT
    gui_calc_button = gtk_button_new_with_label(TEXT_CALC);
    g_signal_connect (G_OBJECT(gui_calc_button), "clicked", G_CALLBACK(callback_button), NULL);
    gtk_fixed_put(fixed, gui_calc_button, 0,0);

    gui_calc_input_buffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text (gui_calc_input_buffer, " LocalPointedPoint", -1);
    gui_calc_input = gtk_text_view_in_scrolled_window_new (gui_calc_input_buffer, true, true);
    gtk_fixed_put(fixed, gui_calc_input, 0,0);

    gui_calc_result_buffer = gtk_text_buffer_new(NULL);
    gui_calc_result = gtk_text_view_new_with_buffer (gui_calc_result_buffer);
    gtk_text_view_set_editable (GTK_TEXT_VIEW(gui_calc_result), false);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW(gui_calc_result), false);
    gtk_fixed_put(fixed, gui_calc_result, 0,0);
    #endif

    /* done */
    main_window_resize();
    gtk_container_add(GTK_CONTAINER(window), (GtkWidget*)fixed);
    gtk_widget_show_all(window);
}


static bool on_launch_or_drop_file (const mchar* fileName)
{
    const mchar* extension;
    if(!open_file(fileName)) { display_message(errorMessage()); return false; }
    get_path_from_path_name (default_file_path, fileName);
    extension = get_extension_from_name(NULL, fileName);
    if(0==strcmp21(extension, "rodt")
    || 0==strcmp21(extension, "mfet"))
        tools_do_eval(get_name_from_path_name(NULL,fileName));
    return true;
}

static void load_launched_file (int argc, char** argv)
{
    mchar str[MAX_PATH_SIZE];
    int i;
    for(i=1; i<argc; i++)
    {
        strcpy21(str, argv[i]);
        on_launch_or_drop_file(str);
    }
    if(argc<=1)
    {
        #ifdef LIBRODT
        strcpy21(str, "\r\n To get started:\r\n");
        strcat21(str, "\r\n Drag-and-drop to open an MFET or RODT File, or,\r\n");
        strcat21(str, "\r\n Launch the software from an MFET or RODT file, or,\r\n");
        strcat21(str, "\r\n Go to Menu -> File -> Open... then do Evaluate.\r\n");
        display_main_text(str);
        #endif
    }
    #ifdef LIBRODT
    calculator_evaluate_calc(true);
    #endif
    userinterface_update();
}


void timer_pause_do() {}

void timer_set_period_do (int period)
{
    g_timeout_add ( period,
            (GSourceFunc) timer_handler_do,
            (gpointer) gui_main_window );
}


int main (int argc, char** argv)
{
    gtk_init(&argc, &argv);

    tools_init(10000000,NULL);

    getMainWindow();

    timer_install_do();

    load_launched_file(argc, argv);

    gtk_main();

    tools_clean();
    memory_print();
    return 0;
}

