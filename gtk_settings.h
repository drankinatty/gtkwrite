#ifndef _gtk_settings_h_
#define _gtk_settings_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"

/* settings functions */
GtkWidget *create_settings_dlg (context *app);
void settings_btncancel (GtkWidget *widget, context *app);

#endif
