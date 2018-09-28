#ifndef _gtk_common_dlg_h_
#define _gtk_common_dlg_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_filebuf.h"

/*  enum GtkMessageType
     GTK_MESSAGE_INFO
     GTK_MESSAGE_WARNING
     GTK_MESSAGE_QUESTION
     GTK_MESSAGE_ERROR
     GTK_MESSAGE_OTHER
*/

typedef struct {    /* for info_bar btntext & response_id pairs */
    const gchar *btntext;
    gint response_id;
} ibbtndef;

void err_dialog (const gchar *errmsg);
void err_dialog_win (gpointer data, const gchar *errmsg);
void font_select_dialog (GtkWidget *widget, kwinst *app);
void show_info_bar_ok (const gchar *msg, gint msgtype, kwinst *app);
void show_info_bar_markup_ok (const gchar *msg, gint msgtype, kwinst *app);
void show_info_bar_choice (const gchar *msg, gint msgtype,
                            ibbtndef *btndef,
                            void (*fn_response)(GtkInfoBar *bar,
                                    gint response_id,
                                    kwinst *app),
                            kwinst *app);

void buffer_file_insert_dlg (kwinst *app, gchar *filename);
void buffer_file_open_dlg (kwinst *app, gchar *filename);
/* Removed until upstream bug fixed
 * see: https://bugzilla.gnome.org/show_bug.cgi?id=779605 (fixed)
 */
void file_open_recent_dlg (kwinst *app);

void dlg_info (const gchar *msg, const gchar *title);
void dlg_info_win (gpointer data, const gchar *msg, const gchar *title);
gboolean dlg_yes_no_msg (gpointer data, const gchar *msg, const gchar *title,
                            gboolean default_return);
/*
gboolean dlg_yes_no_msg (const gchar *msg, const gchar *title,
                            gboolean default_return);
*/
gboolean buffer_prompt_on_mod (kwinst *app);

gchar *get_open_filename (kwinst *app);
gchar *get_save_filename (kwinst *app);

GdkPixbuf *create_pixbuf_from_file (const gchar *filename);

#endif
