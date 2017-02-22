#ifndef _gtk_findreplace_h_
#define _gtk_findreplace_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#ifdef WGTKSOURCEVIEW2
//  #include <gtksourceview/gtksourceview.h>
//  #include <gtksourceview/gtksourcelanguagemanager.h>
 #include <gtksourceview/gtksourceiter.h>
#elif defined WGTKSOURCEVIEW3
 #include <gtksourceview/gtksource.h>
#endif

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"

#define DLGFIND 1
#define DLGREPL 2

/* general functions */
GtkWidget *create_find_dlg (context *app);
GtkWidget *create_replace_dlg (context *app);

void dlg_replace_replace (GtkWidget *widget, gpointer *data);
void chk_realloc_ent (context *app);
void dumpopts (context *app);
void chk_existing_selection (context *app);
void find (context *app, const gchar *text);
void buffer_replace_selection (context *app, const gchar *replacetext);
// void find (context *app, const gchar *text, GtkTextIter *iter);

/* dialog callbacks */
void on_window_show             (GtkWidget *widget, context *app);
void entry_set_find_sensitive   (GtkWidget *widget, context *app);
void entry_set_repl_sensitive   (GtkWidget *widget, context *app);
void entry_find_activate        (GtkWidget *widget, context *app);
void entry_replace_activate     (GtkWidget *widget, context *app);
void chkregex_toggled           (GtkWidget *widget, context *app);
void chkplace_toggled           (GtkWidget *widget, context *app);
void chkcase_toggled            (GtkWidget *widget, context *app);
void chkwhole_toggled           (GtkWidget *widget, context *app);
void chkfrom_toggled            (GtkWidget *widget, context *app);
void chkback_toggled            (GtkWidget *widget, context *app);
void chkselect_toggled          (GtkWidget *widget, context *app);
void chkprompt_toggled          (GtkWidget *widget, context *app);
void btnregex_activate          (GtkWidget *widget, context *app);
void btnplace_activate          (GtkWidget *widget, context *app);
void btnfind_activate           (GtkWidget *widget, context *app);
void btnreplace_activate        (GtkWidget *widget, context *app);
void btnclose_activate          (GtkWidget *widget, context *app);

#endif
