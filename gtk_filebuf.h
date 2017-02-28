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

// gboolean buffer_chk_mod (kwinst *app);
void buffer_clear (kwinst *app);
void buffer_insert_file (kwinst *app, gchar *filename);
gboolean buffer_chk_save_on_exit (GtkTextBuffer *buffer);
void buffer_handle_quit (kwinst *app);
void buffer_save_file (kwinst *app, gchar *filename);
void buffer_write_file (kwinst *app, gchar *filename);

void gtkwrite_window_set_title (GtkWidget *widget, kwinst *app);
void split_fname (kwinst *app);

void buffer_indent_lines (kwinst *app,
                          GtkTextIter *start,
                          GtkTextIter *end);
void buffer_unindent_lines (kwinst *app,
                            GtkTextIter *start,
                            GtkTextIter *end);
gboolean buffer_indent_auto (kwinst *app);
gboolean smart_backspace (kwinst *app);
gboolean smart_tab (kwinst *app);
void buffer_remove_trailing_ws (GtkTextBuffer *buffer);
void buffer_require_posix_eof (GtkTextBuffer *buffer);

gsize g_strlen (const gchar *s);
gboolean str2lower (gchar *str);
gboolean str2upper (gchar *str);
gboolean str2title (gchar *str);
gboolean joinlines (gchar *s);
void selection_for_each_char (GtkTextBuffer *buffer, gboolean (*fn) (gchar *));

/* temp dump functions */
gboolean dump2lower (gchar *str);
void selection_dump (GtkTextBuffer *buffer, gboolean (*fn) (gchar *));

#endif
