#ifndef _gtk_filemonitor_h_
#define _gtk_filemonitor_h_  1

#include <gio/gio.h>

#include "gtk_appdata.h"
#include "gtk_common_dlg.h"

void file_monitor_on_changed (GFileMonitor *mon,
                            GFile *file, GFile *other,
                            GFileMonitorEvent evtype,
                            gpointer data);
GFileMonitor *file_monitor_add (gpointer data);
gboolean file_monitor_cancel (gpointer data);
void file_monitor_block_changed (gpointer data);
void file_monitor_unblock_changed (gpointer data);

#endif
