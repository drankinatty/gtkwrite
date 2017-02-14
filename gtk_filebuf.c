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

void source_view_indent_lines (context *app,
                                GtkTextIter *start,
                                GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line;
    gchar *tab_buffer = app->tabstring;
    // guint spaces = 0, tabs = 0;

    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    start_mark = gtk_text_buffer_create_mark (buf, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buf, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    /* TODO: get offset of current pos and take col % tabsize to get
     *  number of chars from last tabstop to get number of spaces to
     *  insert to get to next tabstop for indent
     */

    gtk_text_buffer_begin_user_action (buf);

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter;

        gtk_text_buffer_get_iter_at_line (buf, &iter, i);

        while (gtk_text_iter_get_char (&iter) == '\t')
            gtk_text_iter_forward_char (&iter);

        if (gtk_text_iter_ends_line (&iter))
            continue;

        gtk_text_buffer_insert (buf, &iter, tab_buffer, -1);
    }

    gtk_text_buffer_end_user_action (buf);

    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
                                        gtk_text_buffer_get_insert (buf));

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buf, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buf, end, end_mark);

    gtk_text_buffer_delete_mark (buf, start_mark);
    gtk_text_buffer_delete_mark (buf, end_mark);
}

void source_view_unindent_lines (context *app,
                                GtkTextIter *start,
                                GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, tab_width, indent_width, start_line, end_line;

    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    start_mark = gtk_text_buffer_create_mark (buf, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buf, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    /* TODO: get offset of current pos and take col % tabsize to get
     *  number of chars from last tabstop to get number of spaces to
     *  insert to get to next tabstop for indent
     */

    tab_width = app->tabstop;
    indent_width = app->softtab;

    gtk_text_buffer_begin_user_action (buf);

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter, iter2;
        gint to_delete = 0, to_delete_equiv = 0;

        gtk_text_buffer_get_iter_at_line (buf, &iter, i);

        iter2 = iter;

        while (!gtk_text_iter_ends_line (&iter2) &&
            to_delete_equiv < indent_width)
        {
            gunichar c;
            c = gtk_text_iter_get_char (&iter2);

            if (c == '\t') {
                to_delete_equiv += tab_width - to_delete_equiv % tab_width;
                to_delete++;
            }
            else if (c == ' ') {
                to_delete_equiv++;
                to_delete++;
            }
            else {
                break;
            }

            gtk_text_iter_forward_char (&iter2);
        }

        if (to_delete > 0) {
            gtk_text_iter_set_line_offset (&iter2, to_delete);
            gtk_text_buffer_delete (buf, &iter, &iter2);
        }
    }

    gtk_text_buffer_end_user_action (buf);

    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
                                        gtk_text_buffer_get_insert (buf));

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buf, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buf, end, end_mark);

    gtk_text_buffer_delete_mark (buf, start_mark);
    gtk_text_buffer_delete_mark (buf, end_mark);
}

/** remove all whitespace to prior softtab stop on backspace.
 *  this function will only remove 'spaces', all other backspace
 *  is handled by the default keyboard handler.
 */
gboolean smart_backspace (context *app)
{
    GtkTextMark *cur;
    GtkTextIter beg, end, iter, iter2;
    gunichar c;
    gint cheq = 0, col = 0, ndel = 0;

    /* validate no selection exists */
    if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER (app->buffer),
                                              &beg, &end))
        return FALSE;
    if (!app->col) return FALSE;  /* already at first char */

    /* initialize beg, iter, end, iterators */
    gtk_text_buffer_get_iter_at_line (app->buffer, &beg, app->line);
    iter = iter2 = end = beg;
    gtk_text_iter_set_visible_line_offset (&end, app->col);
    gtk_text_iter_set_visible_line_offset (&iter, app->col - 1);

    /* if last char not ' ', return FALSE for default handling. */
    if ((c = gtk_text_iter_get_char (&iter)) != ' ')
        return FALSE;

    /* iter forward from beg to end and determine char equivalent
     * number of chars in line then set number of chars to delete
     * to next softtab stop. 'c' and 'ndel' are in *addition to*
     * the ' ' above. (they will always be total-1 chars)
     */
    while (gtk_text_iter_forward_char (&iter2) &&
            !gtk_text_iter_equal (&iter2, &end)) {
        c = gtk_text_iter_get_char (&iter2);
        cheq += (c == '\t') ? app->softtab : 1;
    }
    ndel = cheq % app->softtab; /* chars from current 'iter' pos to del */

    /* backup iter at most ndel spaces, setting col flag */
    while (ndel-- && gtk_text_iter_backward_char (&iter) &&
            (c = gtk_text_iter_get_char (&iter)) == ' ') {
        col++;
    }

    if (col) {  /* if col, user_action to delete chars */
        cur = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (app->buffer));
        gtk_text_buffer_begin_user_action (app->buffer);
        gtk_text_buffer_delete (app->buffer, &iter, &end);
        gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (app->buffer),
                                            &iter, cur);
        app->line = gtk_text_iter_get_line (&iter);
        app->col = gtk_text_iter_get_visible_line_offset (&iter);
        status_set_default (app);
        gtk_text_buffer_end_user_action (app->buffer);


        // on_mark_set (GTK_TEXT_BUFFER (app->buffer), &iter, cur, app);
        // app->col -= col + 1;
        // g_signal_emit_by_name (app->buffer, "mark-set");
        return TRUE;    /* return without further handling */
    }

    return FALSE;   /* return FALSE for default handling */
}
