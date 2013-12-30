/*
 * idemo-app.h
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
#ifndef i_demo_app_h
#define i_demo_app_h

#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct _IDemoApp IDemoApp;
typedef struct _IDemoAppClass   IDemoAppClass;

#define I_DEMO_APP_TYPE (i_demo_app_get_type())
#define I_DEMO_APP(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), I_DEMO_APP_TYPE, IDemoApp))
#define IS_I_DEMO_APP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), I_DEMO_APP_TYPE))
#define I_DEMO_APP_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), I_DEMO_APP_TYPE, IDemoAppClass))
#define IS_I_DEMO_APP_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), I_DEMO_APP_TYPE))
#define I_DEMO_APP_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), I_DEMO_APP_TYPE, IDemoAppClass))

/* IDemoApp object */
struct _IDemoApp {
        GtkWindow parent;
};

/* IDemoApp class definition */
struct _IDemoAppClass {
        GtkWindowClass parent_class;
};

GType i_demo_app_get_type(void);

/* IDemoApp methods */

/**
 * Construct a new IDemoApp
 * @return A new IDemoApp
 */
GtkWidget* i_demo_app_new(void);

#endif /* i_demo_app_h */
