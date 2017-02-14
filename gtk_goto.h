#ifndef _gtk_goto_h_
#define _gtk_goto_h_  1

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>  /* for GDK key values */

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"

GtkWidget *create_goto_dlg (context *app);
// void goto_spinbtn_preedit (GtkWidget *widget, gchar *txt, context *app);
void goto_spinbtn_changed (GtkWidget *widget, context *app);
void goto_btnfind (GtkWidget *widget, context *app);
void goto_btnclose (GtkWidget *widget, context *app);

#endif
