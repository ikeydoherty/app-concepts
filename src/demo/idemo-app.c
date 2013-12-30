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

#include "idemo-app.h"

G_DEFINE_TYPE(IDemoApp, i_demo_app, GTK_TYPE_WINDOW)

/* Boilerplate GObject code */
static void i_demo_app_class_init(IDemoAppClass *klass);
static void i_demo_app_init(IDemoApp *self);
static void i_demo_app_dispose(GObject *object);

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

        gtk_container_set_border_width(GTK_CONTAINER(self), 15);

        layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(self), layout);

        /* Tell people what we're about. */
        markup = gtk_label_new("<big><b>IDemoApp</b>\n"
                "Demonstration of techniques employed within the app-concepts repo\n"
                "This app should be run within isandbox</big>\n\n"
                "<span color='darkgrey'>Demonstration by Ikey Doherty</span>");
        gtk_label_set_use_markup(GTK_LABEL(markup), TRUE);
        gtk_box_pack_start(GTK_BOX(layout), markup, TRUE, TRUE, 10);

        /* Buttons currently non-functioning */
        button = gtk_button_new_with_label("Open files inside sandbox");
        gtk_box_pack_start(GTK_BOX(layout), button, FALSE, FALSE, 5);

        button = gtk_button_new_with_label("Open files outside sandbox");
        gtk_box_pack_start(GTK_BOX(layout), button, FALSE, FALSE, 5);

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
