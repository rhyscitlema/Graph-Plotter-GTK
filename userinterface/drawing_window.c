/*
    drawing_window.c
*/

#define USE_GTK_LIBRARY
#include "main.h"
#include <drawing_window.h>
#include <userinterface.h>
#include <mouse.h>
#include <_malloc.h>

#include <gdk/gdkkeysyms.h>

typedef struct _DrawWindow
{
    GtkWidget* window;
    GtkWidget* drawing_area;
    cairo_surface_t* cairo_surface;
} DrawWindow;



static gboolean draw_callback_v3 (GtkWidget *widget, cairo_t *cr, DrawWindow *dw)
{
    cairo_set_source_surface (cr, dw->cairo_surface, 0, 0);
    cairo_paint(cr);
    draw_request_count--;
    return false;
}

#ifdef GTK2
static gboolean draw_callback_v2 (GtkWidget *widget, void *none, DrawWindow *dw)
{
    gboolean r;
    cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(widget));
    r = draw_callback_v3 (widget, cr, dw);
    cairo_destroy(cr);
    return r;
}
#endif



static gboolean mouse_event (GtkWidget* widget, GdkEvent *event, DrawWindow *dw)
{
    int px=0, py=0, dz=0, button=0;
    enum BUTTON_STATE state = BUTTON_SAME;

    if(event->type == GDK_BUTTON_PRESS || event->type == GDK_BUTTON_RELEASE)
    {
        state = (event->type == GDK_BUTTON_PRESS) ? BUTTON_PRESS : BUTTON_RELEASE;

        if(((GdkEventButton*)event)->button==1) button |= 0x1;
        if(((GdkEventButton*)event)->button==2) button |= 0x2;
        if(((GdkEventButton*)event)->button==3) button |= 0x4;

        px = ((GdkEventButton*)event)->x;
        py = ((GdkEventButton*)event)->y;
    }
    else if(event->type == GDK_SCROLL)
    {
        if(((GdkEventScroll*)event)->direction == GDK_SCROLL_UP)   dz = +1;
        if(((GdkEventScroll*)event)->direction == GDK_SCROLL_DOWN) dz = -1;

        px = ((GdkEventScroll*)event)->x;
        py = ((GdkEventScroll*)event)->y;
    }
    else if(event->type == GDK_MOTION_NOTIFY)
    {
        px = ((GdkEventMotion*)event)->x;
        py = ((GdkEventMotion*)event)->y;
    }
    return on_mouse_event(px, py, dz, button, state, dw);
}



static void drawing_window_close (DrawWindow *dw)
{ drawing_window_close_do(dw); }

static void drawing_window_resize (DrawWindow *dw)
{
    int XSize, YSize;
    Camera *cmr = findCameraFromDW(dw);
    if(cmr==NULL) return;

    gtk_window_get_size (GTK_WINDOW(dw->window), &XSize, &YSize);
    cmr->dXSize = XSize - cmr->XSize;
    cmr->dYSize = YSize - cmr->YSize;

    if(cmr->dXSize==0 && cmr->dYSize==0) return;
    drawing_window_resize_do(dw);
}



DrawingWindow drawing_window_new ()
{
    DrawWindow *dw = (DrawWindow*) _malloc (sizeof(DrawWindow));
    memory_alloc("DrawingWindow");

    /* set drawing window */
    dw->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    if(dw->window == NULL) { _free(dw); return NULL; }

    /* set drawing area */
    dw->drawing_area = gtk_drawing_area_new();
    if(dw->drawing_area == NULL) { gtk_widget_destroy(dw->window); _free(dw); return NULL; }

    g_signal_connect_swapped (G_OBJECT(dw->window), "destroy", G_CALLBACK(drawing_window_close), dw);
    g_signal_connect_swapped (G_OBJECT(dw->window), "configure-event", G_CALLBACK(drawing_window_resize), dw);

    gtk_window_set_position (GTK_WINDOW(dw->window), GTK_WIN_POS_CENTER);

    gtk_widget_add_events (dw->drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (dw->drawing_area, GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events (dw->drawing_area, GDK_POINTER_MOTION_MASK);
    gtk_widget_add_events (dw->drawing_area, GDK_SCROLL_MASK);

    g_signal_connect (G_OBJECT(dw->drawing_area), "button-press-event"  , G_CALLBACK(mouse_event), dw);
    g_signal_connect (G_OBJECT(dw->drawing_area), "button-release-event", G_CALLBACK(mouse_event), dw);
    g_signal_connect (G_OBJECT(dw->drawing_area), "motion-notify-event" , G_CALLBACK(mouse_event), dw);
    g_signal_connect (G_OBJECT(dw->drawing_area), "scroll-event"        , G_CALLBACK(mouse_event), dw);

    g_signal_connect (G_OBJECT(dw->window), "key-press-event"  , G_CALLBACK(keyboard_input_event), NULL);
    g_signal_connect (G_OBJECT(dw->window), "key-release-event", G_CALLBACK(keyboard_input_event), NULL);

    #ifdef GTK2
    g_signal_connect (G_OBJECT (dw->drawing_area), "expose-event", G_CALLBACK (draw_callback_v2), dw);
    #else
    g_signal_connect (G_OBJECT (dw->drawing_area), "draw", G_CALLBACK (draw_callback_v3), dw);
    #endif

    /* finally */
    gtk_window_set_icon(GTK_WINDOW(dw->window), gdk_pixbuf_new_from_file("icon.png", NULL));
    gtk_container_add (GTK_CONTAINER(dw->window), dw->drawing_area);
    dw->cairo_surface = NULL;
    return dw;
}



void drawing_window_name (DrawingWindow dW, const wchar* name)
{
    DrawWindow* dw = (DrawWindow*)dW;
    char title[300];
    strcpy12 (title, name);
    gtk_window_set_title (GTK_WINDOW(dw->window), title);
    gtk_widget_show_all (dw->window);
}



void drawing_window_move (DrawingWindow dW)
{
    DrawWindow* dw = (DrawWindow*)dW;
    Camera* cmr = findCameraFromDW(dW);
    if(cmr==NULL) return;
    //gtk_window_set_position (GTK_WINDOW(dw->window), GTK_WIN_POS_CENTER);
    gtk_window_resize (GTK_WINDOW(dw->window), cmr->XSize, cmr->YSize);
    gtk_widget_show_all (dw->window);
}



void drawing_window_draw (DrawingWindow dW)
{
    static int *p=0, w=0, h=0;

    DrawWindow* dw = (DrawWindow*)dW;
    Camera* cmr = findCameraFromDW(dW);
    if(cmr==NULL) return ;

    if(p != cmr->pixelColour || w != cmr->XSize || h != cmr->YSize || dw->cairo_surface == NULL)
    {
        p = cmr->pixelColour;
        w = cmr->XSize;
        h = cmr->YSize;
        cairo_surface_destroy (dw->cairo_surface);
        dw->cairo_surface = cairo_image_surface_create_for_data (
            (unsigned char*)p, CAIRO_FORMAT_RGB24, w, h, w*4);
    }
    gtk_widget_queue_draw (dw->drawing_area);
}



void drawing_window_remove (DrawingWindow dW)
{
    DrawWindow* dw = (DrawWindow*)dW;
    if(dw==NULL || dw->window==NULL) return;
    cairo_surface_destroy(dw->cairo_surface);
    gtk_widget_destroy(dw->window);
    dw->cairo_surface = NULL;
    dw->drawing_area = NULL;
    dw->window = NULL;
    _free(dw);
    memory_freed("DrawingWindow");
}
