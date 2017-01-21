#ifndef _gtk_filebuf_h_
#define _gtk_filebuf_h_  1

#include <gtk/gtk.h>

#include "gtk_appdata.h"
#include "gtk_windef.h"

/* TODO: finish sorting functions in proper source files, remove
 * dependency of this file on gtk_windef.h
 */

void buffer_insert_file (context *app, gchar *filename);

#endif
