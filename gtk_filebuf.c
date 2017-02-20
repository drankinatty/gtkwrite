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

/** open file or insert file at cursor.
 *  if filename is given, the file is inserted at the cursor without
 *  changing the current filename within the editor, otherwise,
 *  simply opens the file contained in app->filename.
 */
void buffer_insert_file (context *app, gchar *filename)
{
    /* TODO: fix way filename is passed from argv, use it */
    gchar *filebuf = NULL;
    gchar *status = NULL;
    gchar *fnameok = filename;
    GtkTextView *view = GTK_TEXT_VIEW (app->view);
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);

    if (!filename) {
        filename = app->filename;
        // split_fname (app);
    }

    if (g_file_get_contents (filename, &filebuf, &(app->fsize), NULL)) {
        // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
        gtk_text_buffer_insert_at_cursor (buffer, filebuf, -1);
        /* get iter and check last char and if not '\n', then insert
         * (app->posixeol)
         */
        // gtk_text_buffer_insert_at_cursor (app->buffer, "\n", -1);
        if (filebuf) g_free (filebuf);
        gtk_text_view_scroll_to_mark (view,
                                      gtk_text_buffer_get_insert (buffer),
                                      0.0, TRUE, 0.0, 1.0);

        if (fnameok) { /* inserting file at cursor */
            gtk_text_buffer_set_modified (buffer , TRUE);  /* inserted */
            app->modified = TRUE;
            status = g_strdup_printf ("inserted : '%s'", filename);
        }
        else {  /* opening file */
            gtk_text_buffer_set_modified (buffer , FALSE); /* opened */
            app->modified = FALSE;
            status = g_strdup_printf ("loaded : '%s'", app->fname);
            gtkwrite_window_set_title (NULL, app);  /* set window title */
        }
    }
    else {
        /* TODO: change to error dialog */
        status = g_strdup_printf ("file read failed : '%s'", app->fname);
    }
    status_update_str (app, status);
    g_free (status);
    /* reset values to default */
    status_set_default (app);
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
//     gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(app->buffer), FALSE);
//     app->modified = FALSE;
//     gtkwrite_window_set_title (NULL, app);

    /* clear saving status and restore default */
    status_set_default (app);

    if (filename) {}
}

/** TODO: this is a mess, need to split duplicated functionality
 *  from buffer_save_file. This should be a write only. The rest
 *  of the functionality should be in buffer_save_file or save-as.
 *  Move all 'filename' condiitonals above, just save 'filename'.
 */
void buffer_write_file (context *app, gchar *filename)
{
    GError *err=NULL;
    gchar *text;
    gboolean result;
    GtkWidget *view = app->view;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter start, end;

    while (gtk_events_pending())    /* process all pending events */
        gtk_main_iteration();

    /* disable text view and get contents of buffer */
    gtk_widget_set_sensitive (view, FALSE);
    /* using app->buffer, so call to get buffer from view unneeded */
    // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);

    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    gtk_widget_set_sensitive (view, TRUE);

    /* set the contents of the file to the text from the buffer */
    if (filename)
        result = g_file_set_contents (filename, text, -1, &err);
    else {
        if (!app->filename) {
            err_dialog ("Error: app->filename NULL in buffer_write_file()");
            g_free (text);
            return;
        }
        result = g_file_set_contents (app->filename, text, -1, &err);
    }
    if (result == FALSE) {
        /* error saving file, show message to user */
        err_dialog (err->message);
        g_error_free (err);
    }
    else {
        gtk_text_buffer_set_modified (buffer, FALSE);
        app->modified = FALSE;
        gtkwrite_window_set_title (NULL, app);
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

/** indent current/selected lines to the next softtab stop.
 *  text will be aligned to the next softtab on indent
 *  regardless of the number of spaces before the next
 *  softtab text actually begins.
 */
void buffer_indent_lines (context *app,
                          GtkTextIter *start,
                          GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line, nspaces;
    gchar *ind_buffer = NULL;

    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

#ifdef HAVESOURCEVIEW
    gboolean bracket_hl;
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf), FALSE);
#endif

    start_mark = gtk_text_buffer_create_mark (buf, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buf, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    gtk_text_buffer_begin_user_action (buf);

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter;
        gint offset = 0;

        gtk_text_buffer_get_iter_at_line (buf, &iter, i);

        /* iterate past tabs computing character equivalent */
        for (;;) {
            gunichar c;
            c = gtk_text_iter_get_char (&iter);

            if (c == '\t' || c == ' ')
                offset += (c == '\t') ? app->softtab : 1;
            else
                break;

            gtk_text_iter_forward_char (&iter);
        }

        if (gtk_text_iter_ends_line (&iter))
            continue;

        nspaces = app->softtab - offset % app->softtab;

        ind_buffer = g_strdup_printf ("%*s", nspaces, " ");

        gtk_text_buffer_insert (buf, &iter, ind_buffer, -1);

        g_free (ind_buffer);
    }

    gtk_text_buffer_end_user_action (buf);

#ifdef HAVESOURCEVIEW
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf),
                                                        bracket_hl);
#endif

    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
                                        gtk_text_buffer_get_insert (buf));

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buf, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buf, end, end_mark);

    gtk_text_buffer_delete_mark (buf, start_mark);
    gtk_text_buffer_delete_mark (buf, end_mark);
}

/** unindent current/selected lines to the preceding softtab.
 *  text will be aligned to the prior softtab on unindent
 *  regardless of the number of spaces beyond the previous
 *  softtab text actually begins.
 */
void buffer_unindent_lines (context *app,
                            GtkTextIter *start,
                            GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, indent_width, start_line, end_line;

    buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

#ifdef HAVESOURCEVIEW
    gboolean bracket_hl;
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf), FALSE);
#endif

    start_mark = gtk_text_buffer_create_mark (buf, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buf, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    indent_width = app->softtab;

    gtk_text_buffer_begin_user_action (buf);

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter_start, iter;
        gint offset = 0, ndelete = 0;

        gtk_text_buffer_get_iter_at_line (buf, &iter_start, i);

        iter = iter_start;

        while (!gtk_text_iter_ends_line (&iter))
        {
            gunichar c;
            c = gtk_text_iter_get_char (&iter);

            if (c == '\t' || c == ' ')
                offset += (c == '\t') ? indent_width : 1;
            else
                break;

            gtk_text_iter_forward_char (&iter);
        }

        ndelete = offset % indent_width;

        /* TODO: add logic to handle '\t' instead of ' ' delete */
        if (offset <= indent_width) {
            gtk_text_buffer_delete (buf, &iter_start, &iter);
        }
        else if (!ndelete) {
            gtk_text_iter_set_line_offset (&iter_start, offset - indent_width);
            gtk_text_buffer_delete (buf, &iter_start, &iter);
        }
        else {
            gtk_text_iter_set_line_offset (&iter_start, offset - ndelete);
            gtk_text_buffer_delete (buf, &iter_start, &iter);
        }
    }

    gtk_text_buffer_end_user_action (buf);

#ifdef HAVESOURCEVIEW
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buf),
                                                        bracket_hl);
#endif

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
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    gunichar c;
    gint cheq = 0, col = 0, ndel = 0;

    /* validate no selection exists */
    if (gtk_text_buffer_get_selection_bounds (buffer, &beg, &end))
        return FALSE;

    /* get "insert" mark, then current line/column, set end iter */
    cur = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &end, cur);
    app->line = gtk_text_iter_get_line (&end);
    app->col = gtk_text_iter_get_visible_line_offset (&end);

    if (!app->col) return FALSE;  /* already at first char */

    /* initialize iterators, set 'iter' 1-char before insert */
    gtk_text_buffer_get_iter_at_line (buffer, &beg, app->line);
    iter = iter2 = beg;
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
        if (c != ' ' && c != '\t')
            return FALSE;
        cheq += (c == '\t') ? app->softtab : 1;
    }
    ndel = cheq % app->softtab; /* chars from current 'iter' pos to del */

    /* backup iter at most ndel spaces, setting col flag */
    while (ndel-- && gtk_text_iter_backward_char (&iter) &&
            (c = gtk_text_iter_get_char (&iter)) == ' ') {
        col++;
    }

    if (col) {  /* if col, user_action to delete chars */

        gtk_text_buffer_begin_user_action (buffer);

        /* delete characters to prior tab stop from buffer */
        gtk_text_buffer_delete (buffer, &iter, &end);

        /* update line/col with current "insert" mark */
        gtk_text_buffer_get_iter_at_mark (buffer, &iter, cur);

        app->line = gtk_text_iter_get_line (&iter);
        app->col = gtk_text_iter_get_visible_line_offset (&iter);
        status_set_default (app);

        gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER (app->buffer));

        return TRUE;    /* return without further handling */
    }

    return FALSE;   /* return FALSE for default handling */
}

void buffer_remove_trailing_ws (GtkTextBuffer *buffer)
{
    GtkTextIter iter, iter_from, iter_end;
    gunichar c;

    if (!buffer) {
        err_dialog ("Error: Invalid 'buffer' passed to function\n"
                    "buffer_remove_trailing_ws (GtkTextBuffer *buffer)");
        return;
    }

    /* get iter at start of buffer */
    gtk_text_buffer_get_start_iter (buffer, &iter);

    while (gtk_text_iter_forward_to_line_end (&iter)) {

        gint line;
        iter_from = iter_end = iter;

        /* iterate over all trailing whitespace */
        while (gtk_text_iter_backward_char (&iter)) {

            c = gtk_text_iter_get_char (&iter);

            if ((c == ' ' || c == '\t') && c != 0xFFFC)
                iter_from = iter;
            else
                break;
        }

        /* save line to re-validate iter after delete */
        line = gtk_text_iter_get_line (&iter);

        /* remove trailing whitespace up to newline or end */
        if (!gtk_text_iter_equal (&iter_from, &iter_end))
            gtk_text_buffer_delete (buffer, &iter_from, &iter_end);

        /* re-validate iter */
        gtk_text_buffer_get_iter_at_line (buffer, &iter_end, line);
        gtk_text_iter_forward_to_line_end (&iter_end);
        iter = iter_end;
    }

    /* handle last line with trailing whitespace */
    if (!(c = gtk_text_iter_get_char (&iter))) {

        iter_from = iter_end = iter;

        while (gtk_text_iter_backward_char (&iter)) {

            c = gtk_text_iter_get_char (&iter);

            if ((c == ' ' || c == '\t') && c != 0xFFFC)
                iter_from = iter;
            else
                break;
        }

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
#ifdef DEBUGBUF
        else
            g_print ("end char is '\\n'\n");
#endif
    }
}

gsize g_strlen (const gchar *s)
{
    gsize len = 0;
    for(;;) {
        if (s[0] == 0) return len;
        if (s[1] == 0) return len + 1;
        if (s[2] == 0) return len + 2;
        if (s[3] == 0) return len + 3;
        s += 4, len += 4;
    }
}

gboolean str2lower (gchar *str)
{
    if (!str) return FALSE;

    gchar *p = str;
    gboolean changed = FALSE;

    for (; *p; p++)
        if ('A' <= *p && *p <= 'Z') {
            *p ^= 32;
            changed = TRUE;
        }

    return changed;
}

gboolean str2upper (gchar *str)
{
    if (!str) return FALSE;

    gchar *p = str;
    gboolean changed = FALSE;

    for (; *p; p++)
        if ('a' <= *p && *p <= 'z') {
            *p ^= 32;
            changed = TRUE;
        }

    return changed;
}

gboolean str2title (gchar *str)
{
    if (!str) return FALSE;

    gchar *p = str;
    gboolean changed = FALSE;

    for (; *p; p++) {
        if (p == str) {
            if ('a' <= *p && *p <= 'z') {
                *p ^= 32;
                changed = TRUE;
            }
        }
        else if (*(p - 1) == ' ') {
            if ('a' <= *p && *p <= 'z') {
                *p ^= 32;
                changed = TRUE;
            }
        }
        else {
            if ('A' <= *p && *p <= 'Z') {
                *p ^= 32;
                changed = TRUE;
            }
        }
    }

    return changed;
}

/** remove leading, interleaved and trailing whitespace, in place.
 *  the original start address is preserved but due to reindexing,
 *  the contents of the original are not preserved. newline chars
 *  are removed. returns TRUE if changes made, FALSE otherwise.
 */
gboolean joinlines (gchar *s)
{
    if (!s || !*s) return FALSE;

    gchar *p = s, *wp = s;      /* pointer and write-pointer */
    gboolean changed = FALSE, xnl = FALSE;

    while (*p) {
        while (*p == '\n') {    /* check for '\n' */
            while (wp > s && (*(wp - 1) == ' ' || *(wp - 1) == '\t'))
                wp--;           /* remove preceeding whitespace */
            *wp++ = ' ';        /* replace '\n' with ' ' */
            p++;
            changed = TRUE;
            xnl = TRUE;     /* set replaced '\n' flag */
        }
        if (p > s && xnl) { /* if not at beginning & just replaced */
            while (*p && (*p == ' ' || *p == '\t')) {
                p++;        /* replace following whitespace */
                changed = TRUE;
            }
            if (!*p)        /* bail on nul-byte */
                break;
            xnl = FALSE;    /* unset replaced   */
        }
        *wp++ = *p;         /* copy non-ws char */
        p++;                /* advance to next  */
    }
    while (wp > s && (*(wp - 1) == ' ' || *(wp - 1) == '\t')) {
        wp--;               /* trim trailing ws */
        changed = TRUE;
    }
    *wp = 0;    /* nul-terminate */

    return changed;
}

void selection_for_each_char (GtkTextBuffer *buffer, gboolean (*fn) (gchar *))
{
    GtkTextIter start, end;
    gchar *text = NULL;

    /* validate selection exists */
    if (!gtk_text_buffer_get_selection_bounds (buffer, &start, &end)) {
        err_dialog ("Error: Selection Required.\n\n"
                    "gtk_text_buffer_get_selection_bounds()\n"
                    "requires selected text before being called.");
        return;
    }

    if (!(text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE))) {
        err_dialog ("Error: gtk_text_buffer_get_text()\n\n"
                    "Failed to return pointer to alloced\n"
                    "block of memory containing selection.");
        return;
    }

    if (fn (text)) {
        gtk_text_buffer_begin_user_action (buffer);

        if (gtk_text_buffer_delete_selection (buffer, FALSE, TRUE))
            gtk_text_buffer_insert_at_cursor (buffer, text, -1);

        gtk_text_buffer_end_user_action (buffer);
    }

    g_free (text);
}

/* temp dump functions */
gboolean dump2lower (gchar *str)
{
    if (!str) return FALSE;

    gchar *p = str;
    gboolean changed = FALSE;

    for (; *p; p++)
        if ('A' <= *p && *p <= 'Z') {
            *p = g_unichar_tolower(*p);
            changed = TRUE;
        }

    return changed;
}

void selection_dump (GtkTextBuffer *buffer, gboolean (*fn) (gchar *))
{
    GtkTextIter start, end;
    gchar *text = NULL;

    /* validate selection exists */
    if (!gtk_text_buffer_get_selection_bounds (buffer, &start, &end)) {
        err_dialog ("Error: Selection Required.\n\n"
                    "gtk_text_buffer_get_selection_bounds()\n"
                    "requires selected text before being called.");
        return;
    }

    if (!(text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE))) {
        err_dialog ("Error: gtk_text_buffer_get_text()\n\n"
                    "Failed to return pointer to alloced\n"
                    "block of memory containing selection.");
        return;
    }

    if (fn (text)) {
        g_print ("<text>\n%s\n</text>\n", text);
    }

    g_free (text);
}
