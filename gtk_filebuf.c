#include "gtk_filebuf.h"

// gboolean buffer_chk_mod (context *app)
// {
// //     GtkTextBuffer *buffer;
// //
// //     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//
//     return (app->modified = gtk_text_buffer_get_modified (app->buffer));
// }

void buffer_clear (context *app)
{
    /* if buffer changed, prompt for save */
    if (buffer_prompt_on_mod (app) == TRUE)
        buffer_save_file (app, NULL);

    /* free existing filename, set NULL */
    if (app->filename)
        app_free_filename (app);

    /* clear exising buffer, set modified to FALSE */
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER(app->buffer), "", -1);
    gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), FALSE);
    app->modified = FALSE;
    gtkwrite_window_set_title (NULL, app);

    /* reset values to default */
    status_set_default (app);
}

void buffer_insert_file (context *app, gchar *filename)
{
    /* TODO: fix way filename is passed from argv, use it */
    gchar *filebuf = NULL;
    gchar *status = NULL;
    gchar *fnameok = filename;

    if (!filename) {
        filename = app->filename;
        // split_fname (app);
    }

    if (g_file_get_contents (filename, &filebuf, &(app->fsize), NULL)) {
        // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
        gtk_text_buffer_insert_at_cursor (app->buffer, filebuf, -1);
        /* get iter and check last char and if not '\n', then insert
         * (app->posixeol)
         */
        // gtk_text_buffer_insert_at_cursor (app->buffer, "\n", -1);
        if (filebuf) g_free (filebuf);
        gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                        gtk_text_buffer_get_insert (app->buffer),
                                        0.0, TRUE, 0.0, 1.0);
        status = g_strdup_printf ("loaded : '%s'", app->fname);
        if (fnameok) { /* inserting file at cursor */
            gtk_text_buffer_set_modified (app->buffer , TRUE);  /* inserted */
            app->modified = TRUE;
        }
        else {  /* opening file */
            gtk_text_buffer_set_modified (app->buffer , FALSE); /* opened */
            app->modified = FALSE;
        }

        /* TODO: set window title */
        gtkwrite_window_set_title (NULL, app);
    }
    else {
        /* TODO: change to error dialog */
        status = g_strdup_printf ("file read failed : '%s'", app->fname);
    }
    status_update_str (app, status);
    g_free (status);
    /* reset values to default */
    status_set_default (app);

    if (filename) {}
}

gboolean buffer_chk_save_on_exit (GtkTextBuffer *buffer)
{
    if (!buffer) return FALSE;

    if (gtk_text_buffer_get_modified (buffer) == TRUE)
        return dlg_yes_no_msg ("Do you want to save the changes you have made?",
                                "Save File?", TRUE);

    return FALSE;
}

void buffer_save_file (context *app, gchar *filename)
{
    if (filename) {                 /* file_save_as new filename */
        if (app->filename)          /* if existing file, free */
            app_free_filename (app);
        app->filename = filename;   /* assign to app->filename */
        split_fname (app);
    }
    else {
        if (!app->filename) { /* fix critical error: if -> while, remove return */
            while (!(app->filename = get_save_filename (app))) {
                if (!dlg_yes_no_msg ("Error: Get save filename canceled!\n"
                                    "Do you want to cancel save?",
                                    "Warning - Save Canceled", FALSE))
                    return;
                //err_dialog ("error: filename not set/invalid!\nfile not saved.");
                // return;
            }
            split_fname (app);
        }
    }
    status_save_filename (app, NULL);       /* update statusbar Saving....*/

    buffer_write_file (app, filename);  /* write to file app->filename */
    gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), FALSE);
    app->modified = FALSE;
    gtkwrite_window_set_title (NULL, app);

    /* clear saving status and restore default */
    status_set_default (app);

    if (filename) {}
}

/** TODO: this is a mess, need to split duplicated functionality
 *  from buffer_save_file. This should be a write only. The rest
 *  of the functionality should be in buffer_save_file or save-as.
 *  Move all 'filename' condiitonals above, just safe 'filename'.
 */
void buffer_write_file (context *app, gchar *filename)
{
    GError *err=NULL;
    gchar *text;
    gboolean result;
    // GtkTextBuffer *buffer;
    GtkTextIter start, end;

    while (gtk_events_pending())    /* process all pending events */
        gtk_main_iteration();

    /* disable text view and get contents of buffer */
    gtk_widget_set_sensitive (app->view, FALSE);
    /* using app->buffer, so call to get buffer from view unneeded */
    // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_get_start_iter (app->buffer, &start);
    gtk_text_buffer_get_end_iter (app->buffer, &end);

    text = gtk_text_buffer_get_text (app->buffer, &start, &end, FALSE);

    gtk_text_buffer_set_modified (app->buffer, FALSE);
    gtk_widget_set_sensitive (app->view, TRUE);

    /* set the contents of the file to the text from the buffer */
    if (filename)
        result = g_file_set_contents (filename, text, -1, &err);
    else {
        if (!app->filename) {
            err_dialog ("Error: app->filename NULL in buffer_write_file()");
            return;
        }
        result = g_file_set_contents (app->filename, text, -1, &err);
    }
    if (result == FALSE) {
        /* error saving file, show message to user */
        err_dialog (err->message);
        g_error_free (err);
    }

    /* don't forget to free that memory! */
    g_free (text);

    if (filename) {}
}

void gtkwrite_window_set_title (GtkWidget *widget, context *app)
{
    /* TODO: create common set title function for all dialogs */
    /* (e.g. if (widget == app->window), then window title, else dialog */
    gchar *title = NULL;
    if (app->modified) {
        if (app->fname)
            title = g_strdup_printf ("%s - %s [modified]", app->appshort, app->fname);
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

void buffer_remove_trailing_ws (GtkTextBuffer *buffer)
{
    GtkTextIter start, end;
    gint i, start_line, end_line/*, nlines*/;

    if (!buffer) {
        err_dialog ("Error: Invalid 'buffer' passed to function\n"
                    "buffer_remove_trailing_ws (GtkTextBuffer *buffer)");
        return;
    }

    /* get start and end iters, lines, check end line and ... */
    gtk_text_buffer_get_bounds (buffer, &start, &end);

    /* validate chars in buffer */
    if (gtk_text_iter_equal (&start, &end))
        return;

    start_line = gtk_text_iter_get_line (&start);
    end_line = gtk_text_iter_get_line (&end);

    /* check for text following last '\n' */
    if ((gtk_text_iter_get_visible_line_offset (&end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    /* iterate over each line in buffer */
    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter, iter_from, iter_end;
        gunichar c;

        /* set iter before last char in line (before '\n') */
        gtk_text_buffer_get_iter_at_line (buffer, &iter, i);
        gtk_text_iter_set_line_offset (&iter,
                                gtk_text_iter_get_chars_in_line (&iter) - 1);

        iter_from = iter_end = iter;

        /* iterate over all trailing whitespace */
        while (gtk_text_iter_backward_char (&iter)) {

            c = gtk_text_iter_get_char (&iter);

            if ((c == ' ' || c == '\t') && c != 0xFFFC)
                iter_from = iter;
            else
                break;
        }

        /* handle whitespace after last '\n' */
        if (i == end_line && (c = gtk_text_iter_get_char (&iter_end)))
            if (c == ' ' || c == '\t')
                gtk_text_iter_forward_char (&iter_end);

        /* remove trailing whitespace up to newline or end */
        if (!gtk_text_iter_equal (&iter_from, &iter_end))
            gtk_text_buffer_delete (buffer, &iter_from, &iter_end);
    }
}

void buffer_require_posix_eof (GtkTextBuffer *buffer)
{
    GtkTextIter end;

    gtk_text_buffer_get_end_iter (buffer, &end);

    if (gtk_text_iter_backward_char (&end)) {
        if (gtk_text_iter_get_char (&end) != '\n') {
            gtk_text_iter_forward_char (&end);
            gtk_text_buffer_insert (buffer, &end, "\n", -1);
        }
        else
            g_print ("end char is '\\n'\n");
    }
}

