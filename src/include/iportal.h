/*
 * iportal.h
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

#include <glib.h>

/**
 * Represents a remote portal
 */
typedef struct IPortal {
        GError *error; /**<Error to be set when issues are encountered */
} IPortal;

/**
 * Contains the data from a successful launch operation
 */
typedef union IPortalData {
        gchar *string; /**<String data */
        int fd; /**<Open file descriptor */
} IPortalData;

/**
 * Retrieve a Portal by name
 * @param name Requested portal name
 * @return the requested IPortal, or NULL if not found
 */
IPortal *i_portal_lookup(const gchar *name);

/**
 * Launch the portal and return associated data
 * Blocks until the operation is complete
 * @param portal Initialised portal
 * @param ... Parameters to pass to the portal provider
 * @return The data returned by the Portals process
 */
IPortalData *i_portal_launch(IPortal *portal, ...);
