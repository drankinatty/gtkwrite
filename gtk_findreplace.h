#ifndef _gtk_findreplace_h_
#define _gtk_findreplace_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"

/* general functions */
GtkWidget *create_find_dlg (context *app);
GtkWidget *create_replace_dlg (context *app);
void findrep_init (context *app);
void findrep_destroy (context *app);
// void err_dialog (const gchar *errmsg);
void dlg_replace_replace (GtkWidget *widget, gpointer *data);
void chk_realloc_ent (context *app);
void dumpopts (context *app);

/* dialog callbacks */
void entry_find_activate    (GtkWidget *widget, context *app);
void entry_replace_activate (GtkWidget *widget, context *app);
void chkregex_toggled       (GtkWidget *widget, context *app);
void chkplace_toggled       (GtkWidget *widget, context *app);
void chkcase_toggled        (GtkWidget *widget, context *app);
void chkwhole_toggled       (GtkWidget *widget, context *app);
void chkfrom_toggled        (GtkWidget *widget, context *app);
void chkback_toggled        (GtkWidget *widget, context *app);
void chkselect_toggled      (GtkWidget *widget, context *app);
void btnregex_activate      (GtkWidget *widget, context *app);
void btnplace_activate      (GtkWidget *widget, context *app);
void btnfind_activate       (GtkWidget *widget, context *app);
void btnreplace_activate    (GtkWidget *widget, context *app);
void btnclose_activate      (GtkWidget *widget, context *app);

#endif
