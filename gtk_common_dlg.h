#ifndef _gtk_common_dlg_h_
#define _gtk_common_dlg_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_filebuf.h"

void err_dialog (const gchar *errmsg);
void font_select_dialog (GtkWidget *widget, kwinst *app);

void buffer_file_insert_dlg (kwinst *app, gchar *filename);
void buffer_file_open_dlg (kwinst *app, gchar *filename);
void dlg_info (const gchar *msg, const gchar *title);
gboolean dlg_yes_no_msg (const gchar *msg, const gchar *title,
                            gboolean default_return);
gboolean buffer_prompt_on_mod (kwinst *app);

gchar *get_open_filename (kwinst *app);
gchar *get_save_filename (kwinst *app);

#endif
