#include "gtk_filemon.h"

/* prototype from filebuf.c */
void gtkwrite_window_set_title (GtkWidget *widget, kwinst *app);
void buffer_save_file (kwinst *app, gchar *filename);
void file_get_stats (const gchar *filename, kwinst *app);

void buffer_reload_file (kwinst *app)
{
    /* clear exising buffer, insert saved file, set modified to FALSE
     * set title.
     */
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER(app->buffer), "", -1);

    /* insert saved file into buffer */
    buffer_insert_file (app, NULL);
    gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), FALSE);
    gtkwrite_window_set_title (NULL, app);
}

/* For changes, rename in same dir (mv) and restore after delete,
 * move and delete monitoring:
 *   G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT
 * is all that is required, as that is generated after each individual event
 * takes place. Attribute changes are ignored.
 * normal save operations are ignored by setting app->mfp_savecmd flag
 * to TRUE on save. The flag is reset FALSE here on
 * G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT.
 * update file stat information on attribute change.
 */

void file_monitor_on_changed (GFileMonitor *mon,
                                GFile *file, GFile *other,
                                GFileMonitorEvent evtype,
                                gpointer data)
{
    kwinst *app = (kwinst *)data;

#ifdef DEBUGFM
    g_print ("Monitor Event: File = %s\n", g_file_get_parse_name (file));
#endif

    switch (evtype)
    {
        case G_FILE_MONITOR_EVENT_CHANGED:
            /* will fire CHANGES_DONE_HINT */
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_CHANGED\n");
#endif
            break;
        case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
#ifdef DEBUGFM
            /* monitor for mod or create after restored from delete */
            g_print ("G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT\n");
#endif
            /* app->mfp_savecmd is set TRUE for normal save and reset here.
             * in the event of a foreign modification after initiating save,
             * the events will be triggered again after reset occurs.
             */
            if (app->mfp_savecmd) { /* ignore normal saves, reset flag */
#ifdef DEBUGFM
                g_print ("  mfp_savecmd TRUE, normal save, resetting...\n");
#endif
                app->mfp_savecmd = FALSE;
                return;
            }
            /* set modified state and update window title */
            gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), TRUE);
            gtkwrite_window_set_title (NULL, app);
            /* prompt to overwrite or reload */
            if (dlg_yes_no_msg (app, "Current file modified by a foreign process\n\n"
                                "Save File Overwriting Foreign Change? -- Yes\n\n"
                                "Reload File Incorporating Foreign Change? -- NO",
                                "File Modified by Foreign Process",
                                TRUE))
                buffer_save_file (app, NULL);
            else
                buffer_reload_file (app);
                // buffer_insert_file (app, NULL);

            /* set unmodified state and update window title */
            gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), FALSE);
            gtkwrite_window_set_title (NULL, app);

            break;
        case G_FILE_MONITOR_EVENT_DELETED:
            /* monitor for file mv or delete */
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_DELETED\n");
#endif
            if (app->mfp_savecmd)
                return;

            /* set modified state and update window title */
            gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), TRUE);
            gtkwrite_window_set_title (NULL, app);

            /* save into existing filename creating new file
             * TODO: update dialog to present 'Save/Save As' or 'Ignore' options.
             * on ignore, must set unmodified and update window title, handled by
             * Save/Save As otherwise.
             */
            dlg_info_win (app, "Current file deleted by a foreign process\n\n"
                                "Save File Under Original or New Filename.",
                                "File Deleted by Foreign Process");

            break;
        case G_FILE_MONITOR_EVENT_CREATED:
            /* will fire CHANGES_DONE_HINT */
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_CREATED\n");
#endif
            break;
        case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
            /* update attributes */
            file_get_stats (app->filename, app);
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED\n");
#endif
            break;
        case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_PRE_UNMOUNT\n");
#endif
            break;
        case G_FILE_MONITOR_EVENT_UNMOUNTED:
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_UNMOUNTED\n");
#endif
            break;
        case G_FILE_MONITOR_EVENT_MOVED:
#ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_MOVED\n");
#endif
            break;
#ifdef GLIB246
        case G_FILE_MONITOR_EVENT_RENAMED:
 #ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_RENAMED\n");
 #endif
            break;
        case G_FILE_MONITOR_EVENT_MOVED_IN:
 #ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_MOVED_IN\n");
 #endif
            break;
        case G_FILE_MONITOR_EVENT_MOVED_OUT:
 #ifdef DEBUGFM
            g_print ("G_FILE_MONITOR_EVENT_MOVED_OUT\n");
 #endif
            break;
#endif
        default:
#ifdef DEBUGFM
            g_print ("unknown EVENT on changed signal.\n");
#endif
            break;
    }

    if (mon || file || other || data) {}
}

GFileMonitor *file_monitor_add (gpointer data)
{
    kwinst *app = (kwinst *)data;
    GFile *gfile = g_file_new_for_path (app->filename);
    GCancellable *cancellable = g_cancellable_new();
    GError *err=NULL;

    if (app->filemon) {
        /* handle error
         * remove monitor, then add under new filename
         */
        // return NULL;
        file_monitor_cancel (data);
    }

    /* create monitor for app->filename */
    app->filemon = g_file_monitor_file (gfile,
                            G_FILE_MONITOR_NONE,
                            cancellable, &err);
    if (!app->filemon) {
        err_dialog (err->message);
        g_error_free (err);
        g_object_unref (gfile);
        return NULL;
    }

    /* store name of file being monitored */
    app->filemonfn = g_strdup (app->filename);

    /* connect changed signal to monitored file saving ID */
    app->mfp_handler = g_signal_connect (G_OBJECT(app->filemon), "changed",
                            G_CALLBACK (file_monitor_on_changed), data);

#ifdef DEBUGFM
    g_print ("adding app->mfp_handler: %lu\n", app->mfp_handler);
#endif

    g_object_unref (gfile);

    return app->filemon;
}

gboolean file_monitor_cancel (gpointer data)
{
    kwinst *app = (kwinst *)data;

    if (!app->filemon) return FALSE;

#ifdef DEBUGFM
    g_print ("cancelling app->mfp_handler: %lu\n", app->mfp_handler);
#endif

    if (app->mfp_handler) {
        g_signal_handler_disconnect (G_OBJECT(app->filemon), app->mfp_handler);
        app->mfp_handler = 0;
    }

    if (g_file_monitor_cancel (app->filemon)) {
        g_free (app->filemonfn);
        g_object_unref (app->filemon);
        app->filemonfn = NULL;
        app->filemon = NULL;
    }

    return TRUE;
}

/* block firing of changed signal */
void file_monitor_block_changed (gpointer data)
{
    kwinst *app = (kwinst *)data;

    if (!app->filemon || !app->mfp_handler) return;

    g_print ("blocking changed (%lu)\n", app->mfp_handler);

    g_signal_handler_block (app->filemon, app->mfp_handler);
}

/* unblock firing of changed signal */
void file_monitor_unblock_changed (gpointer data)
{
    kwinst *app = (kwinst *)data;

    if (!app->filemon || !app->mfp_handler) return;

    g_print ("unblocking changed (%lu)\n", app->mfp_handler);

    g_signal_handler_unblock (app->filemon, app->mfp_handler);
}
