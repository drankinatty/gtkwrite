#ifndef _gtk_toolbar_h_
#define _gtk_toolbar_h_  1

#include "gtk_menu.h"

void create_toolbar (GtkWidget **toolbar, GtkAccelGroup *mainaccel, kwinst *app);
/* Removed until upstream bug fixed
 * see: https://bugzilla.gnome.org/show_bug.cgi?id=779605
 */

void toolbar_file_open_recent (GtkWidget *widget, kwinst *app);


#endif
