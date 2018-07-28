#ifndef _gtk_textview_h_
#define _gtk_textview_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_completionsv.h"

GtkWidget *create_textview_scrolledwindow (kwinst *app);
void set_tab_size (PangoFontDescription *font_desc, kwinst *app, gint sz);

#endif
