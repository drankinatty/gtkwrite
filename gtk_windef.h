#ifndef _gtk_windef_h_
#define _gtk_windef_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"
// #include "gtk_config.h"
#include "gtk_filebuf.h"
#include "gtk_goto.h"
#include "gtk_menu.h"
#include "gtk_print.h"
#include "gtk_statusbar.h"
#include "gtk_settings.h"
#include "gtk_toolbar.h"

/* TODO:
 *  move all indent functions to gtk_indent.[ch]
 *  finish implementation of titlebar modified indication
 */

/* general functions */
GtkWidget *create_window (kwinst *app);
// void help_about (kwinst *app);

void set_tab_size (PangoFontDescription *font_desc, kwinst *app, gint sz);

/* window callbacks */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 kwinst *app);
void on_window_destroy (GtkWidget *widget, kwinst *app);

/* misc. callback functions */
gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, kwinst *app);
void on_insmode (GtkWidget *widget, kwinst *app);
void on_mark_set (GtkTextBuffer *buffer, GtkTextIter *iter,
                  GtkTextMark *mark, kwinst *app);
void on_buffer_changed (GtkTextBuffer *buffer,
                        kwinst *app);

#endif
