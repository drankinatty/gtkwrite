#ifndef _gtk_windef_h_
#define _gtk_windef_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"

// typedef struct {
//     GtkWidget       *window;    /* main window      */
//     GtkWidget       *view;      /* text_view        */
//     GtkWidget       *statusbar; /* window statusbar */
//     GtkTextTagTable *tagtable;  /* buffer tagtable  */
//     gchar           *filename;  /* filename w/path  */
//     gchar           *fname;     /* filename only    */
//     gchar           *fext;      /* file extension   */
//     gchar           *fpath;     /* file path        */
//     gsize           fsize;      /* file size        */
//     guint           open;       /* file opened      */
//     guint           modified;   /* buffer modified  */
//     guint           cid;        /* sb context id    */
// } context;

/* general functions */
GtkWidget *create_window (context *app);
void context_init (context *app);
void context_destroy (context *app);
void app_free_filename (context *app);
// void err_dialog (const gchar *errmsg);
void help_about (context *app);
void split_fname (context *app);
void set_tab_size (PangoFontDescription *font_desc, context *app, gint sz);

/* window callbacks */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 context *app);
void on_window_destroy (GtkWidget *widget, context *app);
void gtkwrite_window_set_title (GtkWidget *widget, context *app);

/* menu callbacks */
void menu_file_new_activate (GtkMenuItem *menuitem, context *app);
void menu_file_open_activate (GtkMenuItem *menuitem, context *app);
void menu_file_run_activate (GtkMenuItem *menuitem, context *app);
void menu_file_save_activate (GtkMenuItem *menuitem, context *app);
void menu_file_saveas_activate (GtkMenuItem *menuitem, context *app);
void menu_file_pagesu_activate (GtkMenuItem *menuitem, context *app);
void menu_file_pprev_activate (GtkMenuItem *menuitem, context *app);
void menu_file_print_activate (GtkMenuItem *menuitem, context *app);
void menu_file_close_activate (GtkMenuItem *menuitem, context *app);
void menu_file_quit_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_undo_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_redo_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_copy_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_cut_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_paste_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_delete_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_find_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_replace_activate (GtkMenuItem *menuitem, context *app);
void menu_edit_preferences_activate (GtkMenuItem *menuitem, context *app);
void menu_font_select_activate (GtkMenuItem *menuitem, context *app);
void menu_status_clear_activate (GtkMenuItem *menuitem, context *app);
void menu_status_properties_activate (GtkMenuItem *menuitem, context *app);
void menu_help_about_activate (GtkMenuItem *menuitem, context *app);

/* textview functions */
void buffer_insert_file (context *app, gchar *filename);
void buffer_open_file_dlg (context *app, gchar *filename);
void buffer_write_file (context *app, gchar *filename);
gchar *get_save_filename (context *app);
gboolean buffer_chk_mod (context *app);
gboolean buffer_prompt_on_mod (context *app);

/* statusbar functions */
void status_menuitem_label (GtkMenuItem *menuitem, context *app);
void status_pop (GtkWidget *widget, context *app);
void status_set_default (context *app);
void status_update_str (context *app, gchar *s);

/* misc. callback functions */
gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, context *app);
void on_insmode (GtkWidget *widget, context *app);
void on_mark_set (GtkTextBuffer *buffer, GtkTextIter *iter,
                  GtkTextMark *mark, context *app);
void view_print (GtkWidget *widget, context *app);
void view_print_fmt (GtkWidget *widget, context *app);
void testcb (GtkWidget *widget, context *app);
void propcb (GtkWidget *widget, context *app);

#endif
