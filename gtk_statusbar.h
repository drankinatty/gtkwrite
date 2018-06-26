#ifndef _gtk_statusbar_h_
#define _gtk_statusbar_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_charset.h"

/* TODO:
 *  complete rewrite of statusbar functions needed. Nothing
 *  has been done since originally implemented as a test.
 */

/* statusbar functions */
void status_save_filename (kwinst *app, gchar *filename);
void status_menuitem_label (GtkMenuItem *menuitem, kwinst *app);
void status_pop (GtkWidget *widget, kwinst *app);
void status_set_default (kwinst *app);
void status_update_str (kwinst *app, gchar *s);


#endif
