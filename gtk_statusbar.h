#ifndef _gtk_statusbar_h_
#define _gtk_statusbar_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"

/* statusbar functions */
void status_menuitem_label (GtkMenuItem *menuitem, context *app);
void status_pop (GtkWidget *widget, context *app);
void status_set_default (context *app);
void status_update_str (context *app, gchar *s);


#endif
