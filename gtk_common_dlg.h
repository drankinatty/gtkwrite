#ifndef _gtk_common_dlg_h_
#define _gtk_common_dlg_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_filebuf.h"

void err_dialog (const gchar *errmsg);
void font_select_dialog (GtkWidget *widget, context *app);
void buffer_file_open_dlg (context *app, gchar *filename);
gboolean buffer_prompt_on_mod (context *app);
gchar *get_open_filename (context *app);
gchar *get_save_filename (context *app);

#endif
