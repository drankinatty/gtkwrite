#include "gtk_filebuf.h"

// gboolean buffer_chk_mod (context *app)
// {
// //     GtkTextBuffer *buffer;
// //
// //     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//
//     return (app->modified = gtk_text_buffer_get_modified (app->buffer));
// }

void buffer_insert_file (context *app, gchar *filename)
{
    /* TODO: fix way filename is passed from argv, use it */
    gchar *filebuf = NULL;
    gchar *status = NULL;

    if (app->filename) split_fname (app);

    if (g_file_get_contents (app->filename, &filebuf, &(app->fsize), NULL)) {
        // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
        gtk_text_buffer_insert_at_cursor (app->buffer, filebuf, -1);
        gtk_text_buffer_insert_at_cursor (app->buffer, "\n", -1);
        if (filebuf) g_free (filebuf);
        gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                        gtk_text_buffer_get_insert (app->buffer),
                                        0.0, TRUE, 0.0, 1.0);
        status = g_strdup_printf ("loaded : '%s'", app->fname);
        gtk_text_buffer_set_modified (app->buffer , FALSE); /* set unmod */
        /* TODO: set window title */
        gtkwrite_window_set_title (NULL, app);

    }
    else {
        /* TODO: change to error dialog */
        status = g_strdup_printf ("file read failed : '%s'", app->fname);
    }
    status_update_str (app, status);
    g_free (status);

    if (filename) {}
}

void buffer_write_file (context *app, gchar *filename)
{
    GError *err=NULL;
    gchar *status;
    gchar *text;
    gboolean result;
    GtkTextBuffer *buffer;
    GtkTextIter start, end;

    /* add Saving message to status bar and ensure GUI is current */
    if (filename != NULL)
        status = g_strdup_printf ("Saving %s...", filename);
    else /* add else if (app->filename) -- TODO prompt or error if both null */
        status = g_strdup_printf ("Saving %s...", app->filename);

    status_update_str (app, status);
    g_free (status);
    while (gtk_events_pending()) gtk_main_iteration();

    /* disable text view and get contents of buffer */
    gtk_widget_set_sensitive (app->view, FALSE);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    gtk_text_buffer_set_modified (buffer, FALSE);
    app->modified = 0;
    gtk_widget_set_sensitive (app->view, TRUE);

    /* set the contents of the file to the text from the buffer */
    if (filename != NULL)
        result = g_file_set_contents (filename, text, -1, &err);
    else
        result = g_file_set_contents (app->filename, text, -1, &err);

    if (result == FALSE)
    {
        /* error saving file, show message to user */
        err_dialog (err->message);
        g_error_free (err);
    }

    /* don't forget to free that memory! */
    g_free (text);

    if (filename != NULL)
    {
        /* free memory used by app->filename and set new filename.
           call split_fname to free/update filename components */
        // if (app->filename != NULL) g_free (app->filename);
        if (app->filename != NULL) app_free_filename (app);
        app->filename = filename;
        split_fname (app);
    }

    /* clear saving status and restore default */
    status_set_default (app);
}

void gtkwrite_window_set_title (GtkWidget *widget, context *app)
{
    /* TODO: create common set title function for all dialogs */
    /* (e.g. if (widget == app->window), then window title, else dialog */
    gchar *title = NULL;
    if (app->modified) {
        if (app->fname)
            title = g_strdup_printf ("%s - %s*", app->appshort, app->fname);
        else
            title = g_strdup_printf ("%s - untitled*", app->appshort);
    }
    else {
        if (app->fname)
            title = g_strdup_printf ("%s - %s", app->appshort, app->fname);
        else
            title = g_strdup_printf ("%s - untitled", app->appshort);

    }

    gtk_window_set_title (GTK_WINDOW (app->window), title);
    g_free (title);

    if (widget) {}
}

void split_fname (context *app)
{
    if (!app->filename) return;
    gchar *ep = app->filename;
    gchar *sp = ep;
    gchar *p = ep;

    /* free memory for existing components */
    if (app->fname) g_free (app->fname);
    if (app->fext)  g_free (app->fext);
    if (app->fpath) g_free (app->fpath);
    app->fname = app->fext = app->fpath = NULL;

    for (; *ep; ep++);  /* end of string */

    /* separate extension */
    p = ep;
    while (*sp == '.') sp++;            /* handle dot files */
    for (; p > sp && *p != '.'; p--);   /* find last '.' */

    if (p != sp)    /* not dot file with no extension */
        app->fext = g_strdup (p + 1);   /* set fext */

    p = ep; /* separate path */
    for (; p > app->filename && *p != '/'; p--);

    if (p == app->filename) {
        if (*p == '/') {    /* handle root path */
            app->fname = g_strdup (app->filename + 1);
            app->fpath = g_strdup ("/");
        }
        else    /* no path */
            app->fname = g_strdup (app->filename);
        return;
    }

    /* separate normal /path/filename */
    app->fname = g_strdup (p + 1);
    app->fpath = g_strndup (app->filename, p - app->filename);
}

