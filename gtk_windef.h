#ifndef _gtk_windef_h_
#define _gtk_windef_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"
#include "gtk_filebuf.h"
#include "gtk_goto.h"
#include "gtk_print.h"
#include "gtk_statusbar.h"
#include "gtk_settings.h"

/* TODO:
 *  move all indent functions to gtk_indent.[ch]
 *  finish implementation of titlebar modified indication
 */

/* general functions */
GtkWidget *create_window (kwinst *app);
void help_about (kwinst *app);

void set_tab_size (PangoFontDescription *font_desc, kwinst *app, gint sz);

/* window callbacks */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 kwinst *app);
void on_window_destroy (GtkWidget *widget, kwinst *app);

/* menu callbacks */
void menu_file_new_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_open_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_reload_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_save_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_saveas_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_pagesu_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_pprev_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_print_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_close_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_file_quit_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_undo_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_redo_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_copy_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_cut_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_paste_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_delete_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_find_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_replace_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_goto_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_edit_preferences_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_font_select_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_status_clear_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_status_properties_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_status_bigredbtn_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_indent_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_unindent_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_insfile_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_toupper_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_tolower_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_totitle_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_join_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_help_about_activate (GtkMenuItem *menuitem, kwinst *app);
#ifdef HAVESOURCEVIEW
void menu_view_linehl_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_view_lineno_activate (GtkMenuItem *menuitem, kwinst *app);
void menu_tools_syntax_activate (GtkMenuItem *menuitem, kwinst *app);
#endif

/* misc. callback functions */
gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, kwinst *app);
void on_insmode (GtkWidget *widget, kwinst *app);
void on_mark_set (GtkTextBuffer *buffer, GtkTextIter *iter,
                  GtkTextMark *mark, kwinst *app);
void on_buffer_changed (GtkTextBuffer *buffer,
                        kwinst *app);
void view_print (GtkWidget *widget, kwinst *app);
void view_print_fmt (GtkWidget *widget, kwinst *app);
void testcb (GtkWidget *widget, kwinst *app);
void propcb (GtkWidget *widget, kwinst *app);

#endif
