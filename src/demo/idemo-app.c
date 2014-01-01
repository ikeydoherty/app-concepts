/*
 * idemo-app.c
 * 
 * Copyright 2013 Ikey Doherty <ikey.doherty@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#include <gio/gunixfdlist.h>
#include <fcntl.h>

#include "idemo-app.h"
#include "portal-glue.h"
#include "iportal.h"

G_DEFINE_TYPE(IDemoApp, i_demo_app, GTK_TYPE_WINDOW)

/* Boilerplate GObject code */
static void i_demo_app_class_init(IDemoAppClass *klass);
static void i_demo_app_init(IDemoApp *self);
static void i_demo_app_dispose(GObject *object);

/* In-sandbox file chooser */
static void sandy_file_cb(GtkWidget *widget, gpointer userdata);

/* Outside the sandbox */
static void world_file_cb(GtkWidget *widget, gpointer userdata);

/* Initialisation */
static void i_demo_app_class_init(IDemoAppClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &i_demo_app_dispose;
}


static void i_demo_app_init(IDemoApp *self)
{
        GtkWidget *layout;
        GtkWidget *markup;
        GtkWidget *button;
        GtkWidget *text, *scroll;

        gtk_container_set_border_width(GTK_CONTAINER(self), 15);

        layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(self), layout);

        /* Tell people what we're about. */
        markup = gtk_label_new("<big><b>IDemoApp</b>\n"
                "Demonstration of techniques employed within the app-concepts repo\n"
                "This app should be run within isandbox</big>\n\n"
                "<span color='darkgrey'>Demonstration by Ikey Doherty</span>");
        gtk_label_set_use_markup(GTK_LABEL(markup), TRUE);
        gtk_box_pack_start(GTK_BOX(layout), markup, FALSE, FALSE, 10);

        /* Buttons currently non-functioning */
        button = gtk_button_new_with_label("Open files inside sandbox");
        g_signal_connect(button, "clicked", G_CALLBACK(sandy_file_cb),
                self);
        gtk_box_pack_start(GTK_BOX(layout), button, FALSE, FALSE, 5);

        button = gtk_button_new_with_label("Open files outside sandbox");
        g_signal_connect(button, "clicked", G_CALLBACK(world_file_cb),
                self);
        gtk_box_pack_start(GTK_BOX(layout), button, FALSE, FALSE, 5);

        /* Add the text area to set from text files */
        text = gtk_text_view_new();
        self->text = text;
        gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);

        scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scroll), text);
        gtk_box_pack_start(GTK_BOX(layout), scroll, TRUE, TRUE, 0);

        gtk_window_set_title(GTK_WINDOW(self), "IDemoApp");
        gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(self), 300, 300);
        g_signal_connect(self, "destroy", gtk_main_quit, NULL);
        gtk_widget_show_all(GTK_WIDGET(self));
}

static void i_demo_app_dispose(GObject *object)
{
        /* Destruct */
        G_OBJECT_CLASS (i_demo_app_parent_class)->dispose (object);
}

/* Utility; return a new IDemoApp */
GtkWidget* i_demo_app_new(void)
{
        IDemoApp *self;

        self = g_object_new(I_DEMO_APP_TYPE, NULL);
        return GTK_WIDGET(self);
}


static void sandy_file_cb(GtkWidget *widget, gpointer userdata)
{
        GtkWidget *dialog;
        IDemoApp *self;
        gchar *contents = NULL;
        gsize size;
        GError *error = NULL;
        GtkTextBuffer *buffer;
        gchar *filename = NULL;

        /* Open a file chooser */
        self = I_DEMO_APP(userdata);
        dialog = gtk_file_chooser_dialog_new("Sandboxed",
                GTK_WINDOW(userdata), GTK_FILE_CHOOSER_ACTION_OPEN,
                "Cancel", GTK_RESPONSE_CANCEL,
                "Open", GTK_RESPONSE_ACCEPT,
                NULL);
        if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT)
                goto end;

        /* Retrieve file contents */
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_file_get_contents(filename, &contents, &size, &error);
        if (error) {
                g_printerr("I/O: %s\n", error->message);
                g_error_free(error);
                goto end;
        }
        /* Set textview's text to be that of the file */
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->text));
        gtk_text_buffer_set_text(buffer, contents, size);
        g_free(contents);
end:
        if (filename)
                g_free(filename);
        gtk_widget_destroy(dialog);
}

static void world_file_cb(GtkWidget *widget, gpointer userdata)
{
        IDemoApp *self;
        PortalManagerProxy *manager;
        GError *error = NULL;
        GUnixFDList *fd_list = NULL;
        gint fd;
        GMappedFile *file;
        gchar *contents = NULL;
        GtkTextBuffer *buffer;
        gsize size;
        gint length;

        self = I_DEMO_APP(userdata);

        /* Talk to Portal Manager */
        manager = portal_manager_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
                "org.example.Portal.Manager",
                "/org/example/Portal/Manager/0",
                NULL,
                &error);
        if (error) {
                g_printerr("Unable to connect to d-bus service! %s\n", error->message);
                g_error_free(error);
                g_error("Aborting\n");
                return;
        }

        /* Obtain a file descriptor */
        portal_manager_call_get_portal_fd_sync(manager, I_PORTAL_FILES, NULL, &fd_list, NULL, &error);
        if (fd_list == NULL || (length = g_unix_fd_list_get_length(fd_list)) < 1) {
                printf("No files recieved!\n");
                goto end;
        }
        g_message("Got %d file descriptors", length);

        fd = g_unix_fd_list_get(fd_list, 0, NULL);
        file = g_mapped_file_new_from_fd(fd, FALSE, &error);
        if (error) {
                g_printerr("Unable to open mapped file: %s\n", error->message);
                g_error_free(error);
                return;
        }
        /* Set textview's text to be that of the memory-mapped file */
        contents = g_mapped_file_get_contents(file);
        size = g_mapped_file_get_length(file);
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->text));
        gtk_text_buffer_set_text(buffer, contents, size);

        /* Cleanup */
        g_mapped_file_unref(file);
        close(fd);

end:
        if (fd_list)
                g_object_unref(fd_list);
}
