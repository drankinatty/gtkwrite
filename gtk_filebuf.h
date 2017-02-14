#ifndef _gtk_filebuf_h_
#define _gtk_filebuf_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

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

// GtkWidget *create_goto_dlg (context *app);
// void goto_spinbtn_changed (GtkWidget *widget, context *app);
// void goto_btnfind (GtkWidget *widget, context *app);
// void goto_btnclose (GtkWidget *widget, context *app);
void source_view_indent_lines (context *app,
                                GtkTextIter *start,
                                GtkTextIter *end);
void source_view_unindent_lines (context *app,
                                GtkTextIter *start,
                                GtkTextIter *end);
gboolean smart_backspace (context *app);

#endif
