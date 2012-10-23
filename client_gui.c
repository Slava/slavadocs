// GUI version of client
// uses GTK+ 3.0
// compile:
// gcc -Wall -g -o client_gui client_gui.c -export-dynamic `pkg-config --cflags --libs gtk+-3.0`

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include "client_functions.h"

// destroy main window
void on_window_destroy(GtkWidget *widget, gpointer user_data);
// destroy dialog box
void on_dialog_startup_destroy(GtkWidget *widget, gpointer user_data);
// buttons of dialog box
void on_button_connect_clicked(GtkWidget *widget, gpointer grid);
void on_button_exit_clicked(GtkWidget *widget, gpointer grid);
// when hit enter on domain or port text field
void on_entry_domain_port_activate(GtkWidget *widget, gpointer user_data);
// when popup window with error shows, we should show appropriate message
void on_popup_error_show(GtkWidget *widget, gpointer user_data);
void on_popup_error_destroy(GtkWidget *widget, gpointer user_data);
// resend text if buffer was modified
void on_text_view_buffer_changed(GtkTextBuffer *buffer, gpointer user_data);

GtkBuilder *builder;
GtkWidget *window, *dialog_startup, *popup_error;
GtkTextBuffer *text_view_buffer;

// error message that should be printed out
char last_error[256];

// socket descriptor, used to connect to server
int sockfd = -1;

int main(int argc, char **argv) {

	// GTK+ init
	gtk_init(&argc, &argv);

	// load layout XML file
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "layout.glade", NULL);
	
	gtk_builder_connect_signals (builder, NULL);
	
	// main window
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	// dialog window with domain/port
	dialog_startup = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_startup"));

	// popup window with error message
	popup_error = GTK_WIDGET(gtk_builder_get_object(builder, "popup_error"));

	// text buffer of main window's text view
	// we will resend whole text if it is changed
	text_view_buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_view"))));
	g_signal_connect (text_view_buffer, "changed", G_CALLBACK (on_text_view_buffer_changed), NULL);

	// we do not show main window untill app connects to server
	gtk_widget_show(dialog_startup);
	
	gtk_main();
	g_object_unref (G_OBJECT (builder));

	return 0;
}

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
	fprintf(stderr, "close socket\n");
	close(sockfd);
	gtk_main_quit();
}

void on_dialog_startup_destroy(GtkWidget *widget, gpointer user_data) {
	gtk_main_quit();
}

void on_button_connect_clicked(GtkWidget *widget, gpointer grid) {
	const gchar *host_name = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entry_domain")));
	const gchar *port_text = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entry_port")));
	
	int portno = -1;
	// check if port_text had valid number	
	if (sscanf(port_text, "%d", &portno) < 1) {
		fprintf(stderr, "portno == %d, text = %s\n", portno, port_text);
		strcpy(last_error, "Please enter valid port number");
		gtk_dialog_run(GTK_DIALOG(popup_error));
		return ;
	}
	
	fprintf(stderr, "trying to connect to %s:%d\n", host_name, portno);
	// check if we could open socket
	if ((sockfd = setup_socket((char *)host_name, portno)) == -1) {
		fprintf(stderr, "could not connect\n");
		strcpy(last_error, "Could not connect to such hostname and port");
		gtk_dialog_run(GTK_DIALOG(popup_error));
		return;
	}

	send_to_server(sockfd, "initiate session");
	// we connected successfully, so now we can show main window
	gtk_widget_hide(dialog_startup);
	gtk_widget_show(window);
}

void on_button_exit_clicked(GtkWidget *widget, gpointer grid) {
	gtk_main_quit();
}

void on_popup_error_show(GtkWidget *widget, gpointer user_data) {
	gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(popup_error), (const gchar *)last_error);
}

void on_popup_error_destroy(GtkWidget *widget, gpointer user_data) {
	gtk_widget_hide(popup_error);
}

void on_entry_domain_port_activate(GtkWidget *widget, gpointer user_data) {
	on_button_connect_clicked(GTK_WIDGET(gtk_builder_get_object(builder, "button_connect")), (gpointer)NULL);
}

void on_text_view_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
	send_to_server(sockfd, "cclear");
	GtkTextIter start;
	GtkTextIter end;

	gchar *text;

	/* Obtain iters for the start and end of points of the buffer */
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);

	/* Get the entire buffer text. */
	text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

	char *tmp = malloc(strlen(text)+2);	// first will be 't' control char, and we need '\0'
	bzero(tmp,sizeof tmp);
	strcat(tmp, "t");
	strcat(tmp, text);
	send_to_server(sockfd, tmp);
}
