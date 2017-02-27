#ifndef _gtk_toolbar_h_
#define _gtk_toolbar_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"
#include "gtk_filebuf.h"
#include "gtk_findreplace.h"
#include "gtk_goto.h"
#include "gtk_menu.h"
#include "gtk_print.h"
#include "gtk_statusbar.h"
#include "gtk_settings.h"

void create_toolbar (GtkWidget **toolbar, GtkAccelGroup *mainaccel, kwinst *app);

void toolbar_new_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_open_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_save_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_saveas_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_print_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_quit_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_undo_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_redo_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_cut_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_copy_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_paste_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_find_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_replace_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_goto_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_indent_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_unindent_activate (GtkMenuItem *menuitem, kwinst *app);
void toolbar_preferences_activate (GtkMenuItem *menuitem, kwinst *app);

#endif
