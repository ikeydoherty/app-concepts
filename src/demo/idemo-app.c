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
