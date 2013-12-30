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

#include "portal-glue.h"

static void on_bus_acquired(GDBusConnection *connection,
                            const gchar *name,
                            gpointer userdata)
{
        /* TODO: Implement */
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
