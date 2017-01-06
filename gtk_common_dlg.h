#ifndef _gtk_common_dlg_h_
#define _gtk_common_dlg_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"

void err_dialog (const gchar *errmsg);
void font_select_dialog (GtkWidget *widget, context *app);

#endif
