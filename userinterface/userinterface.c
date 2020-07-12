/*
	userinterface.c
*/

#include "main.h"
#include <_stdio.h>
#include <userinterface.h>

static wchar* buffer = NULL;


#ifndef CLI_DIALOG

void user_alert (const wchar* title, const wchar* message)
{
	char msg[strlen12(message)+1];
	strcpy12(msg, message);

	GtkWidget *dialog =
		gtk_message_dialog_new(
			GTK_WINDOW(gui_main_window),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			msg,
			NULL);
	gtk_window_set_title (GTK_WINDOW(dialog), C12(title));
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}


bool user_confirm (const wchar* title, const wchar* message)
{
	char msg[strlen12(message)+1];
	strcpy12(msg, message);

	GtkWidget *dialog =
		gtk_message_dialog_new(
			GTK_WINDOW(gui_main_window),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK_CANCEL,
			msg,
			NULL);
	gtk_window_set_title (GTK_WINDOW(dialog), C12(title));
	bool ret;
	switch(gtk_dialog_run(GTK_DIALOG(dialog))) // TODO: how does this possibly sets errorMessage()[0]=0 and mainStack()[0]=... ???
	{
		case -5: ret = true; break; // TODO: remove hard-coding of OK == -5
		default: ret = false; break;
	}
	gtk_widget_destroy (dialog);
	return ret;
}

#else

void user_alert (const wchar* title, const wchar* message)
{
	printf("\r\n>>> "); puts2(title); puts2(message);
	printf("Press Enter to continue..."); getchar();
}

bool user_confirm (const wchar* title, const wchar* message)
{
	char buffer[10];
	printf("\r\n>>> "); puts2(title); puts2(message);
	printf("Enter ok / <anything> : ");
	if(!fgets(buffer, sizeof(buffer), stdin))
		return false;
	else return 0==strcmp(buffer, "ok\n");
}

#endif


const wchar* user_prompt (const wchar* title, const wchar* message, const wchar* entry)
{
	buffer = wchar_alloc (buffer, 100000);
	strcpy22(buffer, entry);
	return buffer;
}


bool setMenuItemTextOfConvertText (int item, const wchar* text) { return false; }



static GtkTextBuffer* get_gui_text (enum UI_ITEM ui_item)
{
	GtkTextBuffer* text_buffer;
	switch(ui_item)
	{
		case UI_MAIN_TEXT:   text_buffer = gui_main_text_buffer; break;
		case UI_MESG_TEXT:   text_buffer = gui_mesg_text_buffer; break;
		case UI_PATH_TEXT:   text_buffer = gui_path_text_buffer; break;
		case UI_TIME_TEXT:   text_buffer = gui_time_text_buffer; break;
		case UI_CALC_INPUT:  text_buffer = gui_calc_input_buffer; break;
		case UI_CALC_RESULT: text_buffer = gui_calc_result_buffer; break;
		default: text_buffer = NULL; break;
	}
	return text_buffer;
}

const wchar* userinterface_get_text (enum UI_ITEM ui_item)
{
	const char* str=NULL;
	GtkTextIter start, stop;

	     if(ui_item==UI_PAUSE_BUTTON)   str = gtk_button_get_label((GtkButton*)gui_pause_button);
	else if(ui_item==UI_FORWARD_BUTTON) str = gtk_button_get_label((GtkButton*)gui_forward_button);
	else{
		GtkTextBuffer* gui_text = get_gui_text(ui_item);
		if(gui_text)
		{
			gtk_text_buffer_get_bounds(gui_text, &start, &stop);
			str = gtk_text_buffer_get_text(gui_text, &start, &stop, false);
		}
	}
	astrcpy21(&buffer, str);
	return buffer;
}

void userinterface_set_text (enum UI_ITEM ui_item, const wchar* text)
{
	const char* str = C12(text);
	     if(ui_item==UI_PAUSE_BUTTON)   gtk_button_set_label((GtkButton*)gui_pause_button  , str);
	else if(ui_item==UI_FORWARD_BUTTON) gtk_button_set_label((GtkButton*)gui_forward_button, str);
	else{
		GtkTextBuffer* gui_text = get_gui_text(ui_item);
		if(gui_text) gtk_text_buffer_set_text(gui_text, str, -1);
	}
}

void userinterface_clean ()
{
	buffer = wchar_free(buffer);
}

