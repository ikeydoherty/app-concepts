/*
 * iportal.c
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

#include <stdlib.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <gio/gunixfdlist.h>

#include "portal-glue.h"
#include "iportal.h"

static GDBusObjectManagerServer *manager = NULL;

static gboolean on_get_portal_fd(PortalManager *portals,
                                 GDBusMethodInvocation  *invocation,
                                 GUnixFDList *unused,
                                 gchar *portal,
                                 gpointer user_data)
{
        GtkWidget *dialog;
        GUnixFDList *list;
        GError *error = NULL;

        if (g_str_equal(portal, I_PORTAL_FILES)) {
                gchar *filename;
                int fd;
                dialog = gtk_file_chooser_dialog_new("Not Sandboxed",
                        NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
                        "Cancel", GTK_RESPONSE_CANCEL,
                        "Open", GTK_RESPONSE_ACCEPT,
                        NULL);
                if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_ACCEPT) {
                        g_dbus_method_invocation_return_dbus_error(invocation,
                                "org.example.Portal.Manager.Error.NoFile",
                                "Unable to complete without a valid file");
                        goto end;
                }
                list = g_unix_fd_list_new();
                filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
                fd = open(filename, O_RDONLY);
                g_unix_fd_list_append(list, fd, &error);
                if (error) {
                        g_message("Error in copying fd: %s", error->message);
                        g_error_free(error);
                }
                g_free(filename);
                portal_manager_complete_get_portal_fd(portals, invocation, list);
end:
                gtk_widget_destroy(dialog);
        } else {
                g_dbus_method_invocation_return_dbus_error(invocation,
                        "org.example.Portal.Manager.Unknown",
                        "Unknown portal");
        }
        return TRUE;
}

static void on_bus_acquired(GDBusConnection *connection,
                            const gchar *name,
                            gpointer userdata)
{
        GDBusObjectSkeleton *object;
        PortalManager *portal;

        /* TL;DR: Export manager on /org/example/Portal/Manager/0 */

        manager = g_dbus_object_manager_server_new("/org/example/Portal/Manager");
        portal = portal_manager_skeleton_new();
        object = g_dbus_object_skeleton_new("/org/example/Portal/Manager/0");
        g_dbus_object_skeleton_add_interface(object, G_DBUS_INTERFACE_SKELETON(portal));
        g_signal_connect(portal, "handle-get-portal-fd", G_CALLBACK(on_get_portal_fd), NULL);
        g_dbus_object_manager_server_export(manager, G_DBUS_OBJECT_SKELETON(object));
        g_object_unref(object);

        g_dbus_object_manager_server_set_connection(manager, connection);
}

static void on_name_acquired(GDBusConnection *connection,
                             const gchar *name,
                             gpointer userdata)
{
}

static void on_name_lost(GDBusConnection *connection,
                         const gchar *name,
                         gpointer userdata)
{
}

int main(int argc, char **argv)
{
        GMainLoop *loop;
        gtk_init(&argc, &argv);
        guint id;

        loop = g_main_loop_new(NULL, FALSE);
        id = g_bus_own_name(G_BUS_TYPE_SESSION,
                "org.example.Portal.Manager",
                G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT |
                G_BUS_NAME_OWNER_FLAGS_REPLACE,
                on_bus_acquired,
                on_name_acquired,
                on_name_lost,
                loop,
                NULL);

        g_main_loop_run(loop);

        g_bus_unown_name(id);
        g_main_loop_unref(loop);

        return EXIT_SUCCESS;
}
