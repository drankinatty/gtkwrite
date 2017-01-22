#ifndef _gtk_filebuf_h_
#define _gtk_filebuf_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"
#include "gtk_statusbar.h"

/* TODO:
 *  implement buffer_chk_mod in windef.c on changed signal, needs work.
 *  check implementing app->status to remove gtk_statusbar.h dependency.
 */

// gboolean buffer_chk_mod (context *app);
void buffer_insert_file (context *app, gchar *filename);
void buffer_write_file (context *app, gchar *filename);
void gtkwrite_window_set_title (GtkWidget *widget, context *app);
void split_fname (context *app);

#endif
