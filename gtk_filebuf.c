#include "gtk_filebuf.h"

#include <glib.h>
#include <glib/gstdio.h>

// gboolean buffer_chk_mod (kwinst *app)
// {
// //     GtkTextBuffer *buffer;
// //
// //     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//
//     return (app->modified = gtk_text_buffer_get_modified (app->buffer));
// }

void file_get_stats (const gchar *filename, kwinst *app)
{
    struct stat sb;

    app->filemode = 0;
    app->fileuid = 0;
    app->filegid = 0;

    if (!filename) return;  /* validate filename */

    /* validate file exists */
    if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
        g_print ("error: file not found in  file_get_stats() '%s'\n", filename);
        return;
    }

    if (g_stat (filename, &sb)) {   /* get file stats */
        g_print ("error: stat of file failed '%s'\n", filename);
        return;
    }

    app->filemode = sb.st_mode;
    app->fileuid  = sb.st_uid;
    app->filegid  = sb.st_gid;
}

void buffer_clear (kwinst *app)
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
    app->filemode = 0;
    app->fileuid = 0;
    app->filegid = 0;
#ifdef HAVESOURCEVIEW
    app->language = NULL;   /* reset syntax language */
#endif

    /* reset values to default */
    status_set_default (app);
}

/** file_open 'filename' in existing or new editor instance.
 *  'filename' is opened in current instance if buffer is
 *  empty & not modified, otherwise open in new instance.
 */
void file_open (kwinst *app, gchar *filename)
{
    if (!filename) return;

    gint cc = gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(app->buffer));
    gchar *posixfn = get_posix_filename (filename);

    if (!posixfn) {
        dlg_info_win (app, "get_posix_filename() failed.",
                        "Error: filename conversion failure");
        return;
    }

    /* if no chars in buffer and not modified, open in current window */
    if (!cc && !gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(app->buffer))) {
        if (app->filename)              /* free existing filename */
            app_free_filename (app);
        app->filename = posixfn;        /* assign new filename   */
        split_fname (app);              /* split path, name, ext */
        buffer_insert_file (app, NULL); /* insert file in buffer */
        ibar_eol_chk_default (app);     /* check non-default eol */
        // buffer_eol_chk_default (app);   /* check non-default eol */
    }
    else {  /* open in new instance */
        if (!create_new_editor_inst (app, posixfn)) {
            /* clear current file, use current window if spawn fails */
            buffer_clear (app);         /* check for save and clear  */
            status_set_default (app);   /* statusbar default values  */

            /* dialog advising of failure and consequences */
            gchar *msg = g_strdup_printf ("Error: failed to spawn separate\n"
                                        "instance of %s\n", app->exename);
            dlg_info_win (app, msg, "Error - Unable to Create 2nd Instance");
            g_free (msg);
        }
        g_free (posixfn);
    }

    /* final call - return focus to textview */
    gtk_widget_grab_focus (app->view);
}

/** open file or insert file at cursor.
 *  if filename is given, the file is inserted at the cursor without
 *  changing the current filename within the editor, otherwise,
 *  simply opens the file contained in app->filename.
 */
void buffer_insert_file (kwinst *app, gchar *filename)
{
    gchar *filebuf = NULL;
    gchar *fnameok = filename;
    // GtkTextView *view = GTK_TEXT_VIEW (app->view);
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);

    if (!filename) {
        filename = app->filename;
        // split_fname (app);
    }

    if (g_file_get_contents (filename, &filebuf, &(app->fsize), NULL)) {
        // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
        /* TODO: handle BOM here, currently only ASCII/utf-8 supported */
        if (buffer_file_get_bom (filebuf, app) > 1) {
            extern const gchar *bomstr[];
            gchar *errstr = g_strdup_printf ("'%s' contains unsupported %s.",
                            app->fname, bomstr[app->bom]);
            status_set_default (app);
            err_dialog_win ((gpointer *)(app), errstr);
            g_free (errstr);
            if (filebuf) g_free (filebuf);
            return;
        }
        gtk_text_buffer_insert_at_cursor (buffer, filebuf, -1);
        /* get iter and check last char and if not '\n', then insert
         * (app->posixeol)
         */
        // gtk_text_buffer_insert_at_cursor (app->buffer, "\n", -1);
        if (filebuf) g_free (filebuf);

        if (app->poscurend) {
            /* scroll to insert mark */
            gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(app->view),
                                        gtk_text_buffer_get_insert (buffer),
                                        0.0, TRUE, 0.0, 1.0);
        }
        else {
            /* place cursor "insert_mark" at start of file */
            GtkTextIter iter;
            gtk_text_buffer_get_start_iter (buffer, &iter);
            gtk_text_buffer_place_cursor (buffer, &iter);
        }

        if (fnameok) { /* inserting file at cursor */
            gtk_text_buffer_set_modified (buffer , TRUE);  /* inserted */
            app->modified = TRUE;
            // status = g_strdup_printf ("inserted : '%s'", filename); /* remove */
        }
        else {  /* opening file */
            file_get_stats (filename, app); /* save file mode, UID, GID */
            // buffer_get_eol (app);           /* detect EOL, LF, CRLF, CR (moved down) */
            gtk_text_buffer_set_modified (buffer , FALSE);    /* opened */
            app->modified = FALSE;
            buffer_get_eol (app);           /* detect EOL, LF, CRLF, CR */
            // status = g_strdup_printf ("loaded : '%s'", app->fname); /* remove */
            gtkwrite_window_set_title (NULL, app);  /* set window title */
            /* add watch on file */
            if (!app->filemonfn) {
                file_monitor_add (app);
            }
            else if (!g_strcmp0 (app->filemonfn, app->filename) == 0) {
                if (file_monitor_cancel (app))
                    file_monitor_add (app);
            }
        }
    }
    else {
        gchar *errstr = g_strdup_printf ("read of file '%s' failed.", app->fname);
        err_dialog_win ((gpointer *)(app), errstr);
        g_free (errstr);
    }
    // status_update_str (app, status); /* remove */
    // g_free (status);

    /* reset values to default */
    status_set_default (app);

    /* set syntax highlighting language */
#ifdef HAVESOURCEVIEW
    if (app->highlight)
        sourceview_guess_language (app);
#endif
    /* temporary dump of language information */
    // sourceview_get_languange_info (app);
    /*
    if (app->language)
        g_print ("lang_id: %s\n", gtk_source_language_get_id (app->language));
    g_print ("comment single '%s'  blk_beg: '%s'  blk_end: '%s'\n",
            app->comment_single, app->comment_blk_beg, app->comment_blk_end);
    */
}

gboolean buffer_select_all (kwinst *app)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    if (!buffer) return FALSE;

    /* get iter at current insert position */
    gtk_text_buffer_get_iter_at_mark (buffer, &start,
                    gtk_text_buffer_get_insert (buffer));

    if (app->last_pos == NULL) /* set last_pos to restore on cancel */
        app->last_pos = gtk_text_buffer_create_mark (buffer, "last_pos",
                                                    &start, FALSE);
    else
        gtk_text_buffer_move_mark (buffer, app->last_pos, &start);


    /* get start and end iters, verify not the same */
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    if (gtk_text_iter_equal (&start, &end))
        return FALSE;

    /* select range */
    gtk_text_buffer_select_range (buffer, &start, &end);

    return TRUE;
}

gboolean buffer_deselect_all (kwinst *app)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    if (!buffer) return FALSE;

    /* validate existing selection */
    if (!gtk_text_buffer_get_selection_bounds (buffer, &start, &end))
        return FALSE;

    /* get iter at saved insert position, or start if not saved */
    if (app->last_pos) {
        gtk_text_buffer_get_iter_at_mark (buffer, &start, app->last_pos);
        gtk_text_buffer_delete_mark (buffer, app->last_pos);
        app->last_pos = NULL;
    }
    else
        gtk_text_buffer_get_start_iter (buffer, &start);

    /* place cursor at iter */
    gtk_text_buffer_place_cursor (buffer, &start);

    return TRUE;
}

/** buffer_select_to_next_char selects from cursor to next non-ws char.
 *  this corrects the gtk_text_view default selection of whitespace
 *  plus the next word on the ctrl+shift+right-arrow combination.
 *
 *  the keypress handler must keep track of the number of left or right
 *  ctrl+shift select keypress sequences so using the arrow-key in the
 *  opposite direction will undo in the correct direction. the current
 *  bstack will only keep track of 128 simultaneous ctrl+shift keypress
 *  events in one direction at a time (or 256 if undoing 128 in the
 *  opposite direction. increase STKMAX if you need more.
 */
gboolean buffer_select_to_next_char (kwinst *app)
{
    GtkTextBuffer *buf = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter start, end;
    gunichar c;

    /* check existing selection, set start = end, otherwise
     * get iter at cursor position ("Insert" mark), set end = start
     */
    if (!gtk_text_buffer_get_selection_bounds (buf, &start, &end)) {
        gtk_text_buffer_get_iter_at_mark (buf, &start,
                                gtk_text_buffer_get_insert (buf));
        end = start;
        bstack_clear (app);
    }

    /* check if prev ctrl+shift keypress & if it was RIGHT */
    if (!app->bindex || bstack_last (app) == RIGHT) {
        /* get char and check if whitespace or non-whitespace */
        c = gtk_text_iter_get_char (&end);
        if (c == ' ' || c == '\t') {
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_forward_char (&end))
                    break;
                c = gtk_text_iter_get_char (&end);
            }
        }
        else {
            /* read contiguous non-whitespace */
            while (c != ' ' && c != '\t') {
                if (!gtk_text_iter_forward_char (&end))
                    break;
                c = gtk_text_iter_get_char (&end);
            }
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_forward_char (&end))
                    break;
                c = gtk_text_iter_get_char (&end);
            }
        }
    }
    else {
        c = gtk_text_iter_get_char (&start);
        if (c == ' ' || c == '\t') {
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_forward_char (&start) ||
                    gtk_text_iter_equal (&start, &end))
                    goto nextrdlt;
                c = gtk_text_iter_get_char (&start);
            }
            /* read contiguous non-whitespace */
            while (c != ' ' && c != '\t') {
                if (!gtk_text_iter_forward_char (&start) ||
                    gtk_text_iter_equal (&start, &end))
                    break;
                c = gtk_text_iter_get_char (&start);
            }
            nextrdlt:;
        }
        else {
            /* read contiguous non-whitespace */
            while (c != ' ' && c != '\t') {
                if (!gtk_text_iter_forward_char (&start) ||
                    gtk_text_iter_equal (&start, &end))
                    break;
                c = gtk_text_iter_get_char (&start);
            }
        }
        if (gtk_text_iter_equal (&start, &end)) {
            gtk_text_buffer_place_cursor (buf, &start);
            bstack_clear (app);
            return TRUE;
        }
    }

    /* select range */
    gtk_text_buffer_select_range (buf, &start, &end);

    return TRUE;
}

/** buffer_select_to_next_char selects from cursor to next non-ws char.
 *  this corrects the gtk_text_view default selection of whitespace
 *  plus the next word on the ctrl+shift+right-arrow combination.
 *
 *  (see extended note on function above for bstack tracking of
 *   ctrl+shift keypress events.
 */
gboolean buffer_select_to_prev_char (kwinst *app)
{
    GtkTextBuffer *buf = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter start, end;
    gunichar c;

    /* check existing selection, set start = end, otherwise
     * get iter at cursor position ("Insert" mark), set end = start
     */
    if (!gtk_text_buffer_get_selection_bounds (buf, &start, &end)) {
        gtk_text_buffer_get_iter_at_mark (buf, &start,
                                gtk_text_buffer_get_insert (buf));
        end = start;
        bstack_clear (app);
    }

    /* check if prev ctrl+shift keypress & if it was LEFT */
    if (!app->bindex || bstack_last (app) == LEFT) {

        gtk_text_iter_backward_char (&start);

        /* get char and check if whitespace or non-whitespace */
        c = gtk_text_iter_get_char (&start);
        if (c == ' ' || c == '\t') {
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_backward_char (&start))
                    break;
                c = gtk_text_iter_get_char (&start);
            }
        }
        else {
            /* read contiguous non-whitespace */
            while (c != ' ' && c != '\t') {
                if (!gtk_text_iter_backward_char (&start))
                    goto prevrdlt;
                c = gtk_text_iter_get_char (&start);
            }
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_backward_char (&start))
                    break;
                c = gtk_text_iter_get_char (&start);
            }
            prevrdlt:;
        }
        if (gtk_text_iter_equal (&start, &end)) {
            gtk_text_buffer_place_cursor (buf, &start);
            bstack_clear (app);
            return TRUE;
        }
        else
            gtk_text_iter_forward_char (&start);
    }
    else {

        gtk_text_iter_backward_char (&end);

        /* get char and check if whitespace or non-whitespace */
        c = gtk_text_iter_get_char (&end);
        if (c == ' ' || c == '\t') {
            /* read contiguous whitespace to next word */
            while (c == ' ' || c == '\t') {
                if (!gtk_text_iter_backward_char (&end) ||
                    gtk_text_iter_equal (&start, &end))
                    goto prevrdrt;
                c = gtk_text_iter_get_char (&end);
            }
            /* read contiguous non-whitespace */
            while (c != ' ' && c != '\t') {
                if (!gtk_text_iter_backward_char (&end) ||
                    gtk_text_iter_equal (&start, &end))
                    break;
                c = gtk_text_iter_get_char (&end);
            }
            prevrdrt:;
        }
        else {
            /* read contiguous whitespace to next word */
            while (c != ' ' || c != '\t') {
                if (!gtk_text_iter_backward_char (&end) ||
                    gtk_text_iter_equal (&start, &end))
                    break;
                c = gtk_text_iter_get_char (&end);
            }
        }
        if (gtk_text_iter_equal (&start, &end)) {
            gtk_text_buffer_place_cursor (buf, &start);
            bstack_clear (app);
            return TRUE;
        }
        else
            gtk_text_iter_forward_char (&end);
    }

    /* select range */
    gtk_text_buffer_select_range (buf, &start, &end);

    return TRUE;
}

/** buffer_find_words() in buffer, add to ternary tree for word completion.
 *  TODO - add wcomplen to app.
 */
void buffer_find_words (kwinst *app)
{
    gint idx = 0, wcomplen = 3;
    GtkTextIter start, end;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

    gtk_text_buffer_get_start_iter (buffer, &start);

    while (!gtk_text_iter_starts_word (&start))
        gtk_text_iter_forward_char (&start);

    end = start;

    while (gtk_text_iter_forward_word_end (&end)) {
        const gchar *word = gtk_text_buffer_get_text (buffer,
                                            &start, &end, FALSE);
        const gchar *p = word;

        for (; *p; p++) {}

        if (p - word > wcomplen)
            g_print ("[%4d] : %s\n", idx++, word);

        start = end;

        for (;;) {
            if (!gtk_text_iter_forward_char (&start))
                goto wordseldone;
            if (gtk_text_iter_starts_word (&start))
                break;
        }

        end = start;    /* not 100% needed, fwd_word_end will jumpt to next end */
    }
    wordseldone:;

    gtk_text_buffer_end_user_action (buffer);   /* end user action */
}

/** comment line based on comment syntax determined from SourceView language.
 *  start and end iterators indicating the start and end of the block (or single
 *  line) to be commented. Block comments are used if text is selected,
 *  otherwise single-line comments are used to comment entire line.
 *  TODO:
 *    provide setting to prefer single/multi-line comemnt, and update settings
 *    dialog to display current document comment syntax strings.
 */
void buffer_comment_lines (kwinst *app,
                          GtkTextIter *start,
                          GtkTextIter *end)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line;
    const gchar *commentstr = app->comment;  /* set comment from settings */

#ifdef HAVESOURCEVIEW
    gboolean bracket_hl;
    gint start_pos, end_pos;
    if (app->comment_single)                /* if comment based on lang_id */
        commentstr = app->comment_single;   /* update commentstr from lang_id */
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer), FALSE);
#endif

    /* preserve marks for start/end to revalidate iterators before return
     * using left gravity on start to mark stays left of inserted text.
     */
    start_mark = gtk_text_buffer_create_mark (buffer, NULL, start, TRUE);
    end_mark   = gtk_text_buffer_create_mark (buffer, NULL, end, FALSE);

    /* get lines indicated by start/end iters passed based on selection */
    start_line = gtk_text_iter_get_line (start);
    end_line   = gtk_text_iter_get_line (end);

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

#ifdef HAVESOURCEVIEW
    /* get position within line for start/end iters */
    start_pos = gtk_text_iter_get_line_offset (start);
    end_pos   = gtk_text_iter_get_line_offset (end);

    /* insert multi-line block comments */
    if (app->comment_blk_beg && app->comment_blk_end &&
        !app->cmtusesingle &&
        (end_line > start_line || end_pos > start_pos)) {

        /* insert beginning comment for block */
        gtk_text_buffer_insert (buffer, start, app->comment_blk_beg, -1);

        /* revalidate end iter from saved end_mark */
        gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);
        /* insert ending comment for block */
        gtk_text_buffer_insert (buffer, end, app->comment_blk_end, -1);
    }
    else {  /* insert single-line comments */
#endif
        GtkTextIter iter;
        /* check if end is at beginning of multi-line - subtract 1-line */
        if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
            (end_line > start_line)) {
                end_line--;
        }
        /* loop over each line inserting comment at beginning */
        for (i = start_line; i <= end_line; i++) {
            gtk_text_buffer_get_iter_at_line (buffer, &iter, i);
            gtk_text_buffer_insert (buffer, &iter, commentstr, -1);
        }
#ifdef HAVESOURCEVIEW
    }

    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer),
                                                        bracket_hl);
#endif

    gtk_text_buffer_end_user_action (buffer);   /* end user action */

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);

    /* extend selection to cover new comment syntax (for uncomment) */
    gtk_text_buffer_select_range (buffer, end, start);

    gtk_text_buffer_delete_mark (buffer, start_mark);
    gtk_text_buffer_delete_mark (buffer, end_mark);
}

/** TODO add to selection bounds at end of block at the end so block comments
 *  can be enabled and removed without having to reselect the additional end
 *  comment in range.
 */
void buffer_uncomment_lines (kwinst *app,
                          GtkTextIter *start,
                          GtkTextIter *end)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextMark *start_mark, *end_mark;
    GtkTextIter iter = *start;
    gint i, start_line, end_line;
    const gchar *commentstr = app->comment;  /* set comment from settings */

#ifdef HAVESOURCEVIEW
    gboolean blockcmt = FALSE;
    gboolean bracket_hl;
    if (app->comment_single)                /* if comment based on lang_id */
        commentstr = app->comment_single;   /* update commentstr from lang_id */
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer), FALSE);
#endif

    /* preserve marks for start/end to revalidate iterators before return */
    start_mark = gtk_text_buffer_create_mark (buffer, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buffer, NULL, end, FALSE);

    /* get lines indicated by start/end iters passed based on selection */
    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    /* check if end is at beginning of multi-line - subtract 1-line */
    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

#ifdef HAVESOURCEVIEW
    /* if block comment strings are set */
    if (app->comment_blk_beg && app->comment_blk_end) {

        const gchar *p = app->comment_blk_beg;
        gunichar c = gtk_text_iter_get_char (&iter);

        while (*p && c == (gunichar)*p) {   /* compare with chars in buffer */
            if (!gtk_text_iter_forward_char (&iter))
                break;

            c = gtk_text_iter_get_char (&iter);
            p++;
        }
        if (*p == '\0') /* all chars matched to nul-character, block comment */
            blockcmt = TRUE;
    }

    if (blockcmt) { /* handle removal of block comments, which can be sin same line */

        const gchar *p = app->comment_blk_end; /* pointer to opening string */

        gunichar c;

        /* iter and start properly set from above block, just delete */
        gtk_text_buffer_delete (buffer, start, &iter);
        /* revalidate end iterator */
        gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);
        iter = *end;    /* set iter to end */

        while (*p)  /* move to end char of comment_blk_end */
            p++;
        p--;

        /* backup to last char before end iter */
        if (!gtk_text_iter_backward_char (&iter)) {
            /* handle error */
            goto unmatchedblk;
        }
        c = gtk_text_iter_get_char (&iter); /* get last char before end */

        /* iterate backwards until the beginning char in comment found */
        while (p > app->comment_blk_end && (gunichar)*p == c) {
            if (!gtk_text_iter_backward_char (&iter)) {
                /* handle error */
                goto unmatchedblk;
            }
            c = gtk_text_iter_get_char (&iter);
            p--;
        }
        if (p == app->comment_blk_end)  /* all chars matched as comment */
            gtk_text_buffer_delete (buffer, &iter, end);    /* delete comment */
        else {
            /* handle error */
            goto unmatchedblk;
        }
    }
    else {  /* remove single-line comment from all lines in selection */
#endif
        for (i = start_line; i <= end_line; i++) {

            GtkTextIter iterstart;
            gunichar c;
            gchar *p = (gchar *)commentstr;

            gtk_text_buffer_get_iter_at_line (buffer, &iterstart, i);
            iter = iterstart;

            c = gtk_text_iter_get_char (&iter);

            /* move to start of comment string (displaced comment) */
            while (c != (gunichar)*p && (c == ' ' || c == '\t')) {

                if (!gtk_text_iter_forward_char (&iter))
                    goto nxtuncomment;

                c = gtk_text_iter_get_char (&iter);
            }
            iterstart = iter;   /* set start iter */

            for (; *p; p++) {   /* set characters to delete */

                if ((gunichar)*p != gtk_text_iter_get_char (&iter) ||
                        !gtk_text_iter_forward_char (&iter))
                    break;
            }

            /* delete comment */
            gtk_text_buffer_delete (buffer, &iterstart, &iter);

            nxtuncomment:;
        }
#ifdef HAVESOURCEVIEW
    }
    unmatchedblk:;

    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer),
                                                        bracket_hl);
#endif

    gtk_text_buffer_end_user_action (buffer);   /* end user action */

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);

    gtk_text_buffer_delete_mark (buffer, start_mark);
    gtk_text_buffer_delete_mark (buffer, end_mark);
}

/** callback for ibar_handle_quit, handling save, exit or cancel. */
void ib_handle_quit (GtkInfoBar *bar, gint response_id, kwinst *app)
{
    switch (response_id) {
        case GTK_RESPONSE_CANCEL:   /* return to editor */
            gtk_widget_hide (GTK_WIDGET(bar));
            break;
        case GTK_RESPONSE_NO:       /* quit */
            gtk_main_quit ();
            break;
        case GTK_RESPONSE_YES:      /* check modified, prompt for save */
            gtk_widget_hide (GTK_WIDGET(bar));
            if (!app->filename) {
                gchar *filename;
                while (!(filename = get_save_filename (app))) {
                    if (dlg_yes_no_msg (app, "Warning: Do you want to cancel save?",
                                        "Warning - Save Canceled", FALSE))
                        goto cancel_save;   /* return to editor */
                }
                /* on exit cleanups */
                if (app->trimendws)
                    buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
                if (app->posixeof)
                    buffer_require_posix_eof (app);
                buffer_write_file (app, filename);
                g_free (filename);
            }
            else {
                /* on exit cleanups */
                if (app->trimendws)
                    buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
                if (app->posixeof)
                    buffer_require_posix_eof (app);
                buffer_write_file (app, NULL);
            }
            gtk_main_quit ();
            break;
    }
   cancel_save:;

    /* set text_view sensitive TRUE */
    if (!gtk_widget_get_sensitive (app->view))
        gtk_widget_set_sensitive (app->view, TRUE);

    /* grab focus for textview */
    gtk_widget_grab_focus (app->view);

    /* reset flags */
    app->ibflags = 0;
}

/** ibar_handle_quit catches File->Close and on_window_delete_event.
 *  checks gtk_text_buffer_get_modified and presents an infobar
 *  prompting for save (Save, Discard, Cancel). callback ib_handle_quit
 *  receives the response_id from the user choice and acts accordingly.
 */
void ibar_handle_quit (kwinst *app)
{
    /* create button definitions for infobar */
    ibbtndef btndef[] = { { .btntext = "_Save",    .response_id = GTK_RESPONSE_YES },
                          { .btntext = "_Discard", .response_id = GTK_RESPONSE_NO },
                          { .btntext = "_Cancel",  .response_id = GTK_RESPONSE_CANCEL },
                          { .btntext = "",         .response_id = 0 } };

    if (app->ibflags & IBAR_VISIBLE)        /* prevent duplicate infobars */
        return;

    app->ibflags |= IBAR_VIEW_SENSITIVE;    /* use |= to preserve other flags */

    if (gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(app->buffer)) == TRUE)
    {
        /* create message with markup */
        gchar *msg = g_markup_printf_escaped (
                        "<span font_weight=\"bold\" font_size='large'>"
                        "File Modified!</span>\n\n"
                        "Do you want to save the changes you have made?");

        /* show message and buttons in infobar */
        show_info_bar_choice (msg, GTK_MESSAGE_WARNING, btndef, ib_handle_quit, app);

        g_free (msg);   /* free msg */
    }
    else if (app->filename) /* not modified, apply on-exit cleanups */
    {
        if (app->trimendws &&
            gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(app->buffer)))
            buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
        if (app->posixeof)
            buffer_require_posix_eof (app);
        if (gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(app->buffer)))
            buffer_write_file (app, NULL);
        gtk_main_quit ();
    }
    else
        gtk_main_quit ();

}

/** presents dialog on quit if file modified
 *  (replaced by ibar_handle_quit)
 */
gboolean buffer_chk_save_on_exit (GtkTextBuffer *buffer)
{
    if (!buffer) return FALSE;

    if (gtk_text_buffer_get_modified (buffer) == TRUE)
        return dlg_yes_no_msg (NULL, "Do you want to save the changes you have made?",
                                "Save File?", TRUE);

    return FALSE;
}

/** presents dialog on quit (or WM_CLOSE) if file modified
 *  (replaced by ibar_handle_quit)
 */
void buffer_handle_quit (kwinst *app)
{
    /* check changed, prompt yes/no */
    if (buffer_chk_save_on_exit (GTK_TEXT_BUFFER(app->buffer)))
    {
        if (!app->filename) {
            gchar *filename;
            while (!(filename = get_save_filename (app))) {
                if (dlg_yes_no_msg (app, "Warning: Do you want to cancel save?",
                                    "Warning - Save Canceled", FALSE))
                    goto cancel_save;
            }
            if (app->trimendws)
                buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
            if (app->posixeof)
                buffer_require_posix_eof (app);
            buffer_write_file (app, filename);
            g_free (filename);
            return;
        } /*
        else {
            if (app->trimendws)
                buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
            if (app->posixeof)
                buffer_require_posix_eof (app);
            buffer_write_file (app, app->filename);
        } */
    }
    else if (app->filename)
    {
        if (app->trimendws &&
            gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(app->buffer)))
            buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
        if (app->posixeof)
            buffer_require_posix_eof (app);
        if (gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(app->buffer)))
            buffer_write_file (app, NULL);
    }

    cancel_save:;
}

/** buffer_save_file in new 'filename' or in existing if NULL.
 *  if 'filename' is provided, Save-As 'filename', otherwise if
 *  'filename' is NULL, Save in existing app->filename or prompt
 *  with file_save dialog. cancel and add file monitoring as
 *  required. save file stats for existing file, update status.
 *  if HAVESOURCEVIEW, guess language syntax.
 */
void buffer_save_file (kwinst *app, gchar *filename)
{
    if (filename) {                         /* file_save_as new filename */
        if (app->filename) {                /* if existing file, free filename */
            app_free_filename (app);
            if (!file_monitor_cancel (app)) /* cancel existing monitor */
            { /* handle error */ }
        }
        app->filename = filename;   /* assign to app->filename */
        split_fname (app);
    }
    else {
        if (!app->filename) { /* fix critical error: if -> while, remove return */
            while (!(app->filename = get_save_filename (app))) {
                if (dlg_yes_no_msg (app, "Error: Get save filename canceled!\n"
                                    "Do you want to cancel save?",
                                    "Warning - Save Canceled", FALSE))
                    return;
            }
            split_fname (app);
        }
    }
    status_save_filename (app, NULL);       /* update statusbar Saving....*/

    app->mfp_savecmd = TRUE;                /* set flag TRUE - save in progress */

    buffer_write_file (app, filename);      /* write to file app->filename */

    if (!app->mfp_handler)                  /* if not monitoring file */
        file_monitor_add (app);             /* setup monitoring on new name */

    file_get_stats (app->filename, app);    /* check/save file stats */

    status_set_default (app);               /* restore statusbar */

#ifdef HAVESOURCEVIEW
    if (app->highlight)                     /* if syntax highlight enabled */
        sourceview_guess_language (app);    /* guess source language */
#endif
}

/** TODO: this is a mess, need to split duplicated functionality
 *  from buffer_save_file. This should be a write only. The rest
 *  of the functionality should be in buffer_save_file or save-as.
 *  Move all 'filename' condiitonals above, just save 'filename'.
 */
void buffer_write_file (kwinst *app, gchar *filename)
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

    /* get start and end iters for buffer */
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);

    /* get text from buffer */
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
#ifndef HAVEMSWIN
        if (app->filemode)      /* restore file mode, UID, GID */
            g_chmod (app->filename, app->filemode);
        if (app->fileuid && app->filegid)
            chown ((const char *)app->filename, (uid_t)app->fileuid,
                    (gid_t)app->filegid);
#endif
    }

    /* don't forget to free that memory! */
    g_free (text);

    if (filename) {}
}

void gtkwrite_window_set_title (GtkWidget *widget, kwinst *app)
{
    /* TODO: create common set title function for all dialogs
     * (e.g. if (widget == app->window), then window title, else dialog
     *
     * FIXME: tooltip commented below activates *within* window, not titlebar
     * need to find way to set tooltip for titlebar only.
     */
    gchar *title = NULL;
    // gchar *longtitle = NULL;
    if ((app->modified = gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(app->buffer)))) {
        if (app->fname) {
            title = g_strdup_printf ("%s - %s [modified]", app->appshort, app->fname);
            // longtitle = g_strdup_printf ("%s - %s [modified]", app->appshort,
            //                             app->filename);
        }
        else
            title = g_strdup_printf ("%s - untitled*", app->appshort);
    }
    else {
        if (app->fname) {
            title = g_strdup_printf ("%s - %s", app->appshort, app->fname);
            // longtitle = g_strdup_printf ("%s - %s", app->appshort, app->filename);
        }
        else
            title = g_strdup_printf ("%s - untitled", app->appshort);

    }

    gtk_window_set_title (GTK_WINDOW (app->window), title);
    // if (longtitle)
    //     gtk_widget_set_tooltip_text (app->window, longtitle);

    g_free (title);
    // g_free (longtitle);

    if (widget) {}
}

// void split_fname (kwinst *app)
// {
//     if (!app->filename) return;
//     gchar *ep = app->filename;
//     gchar *sp = ep;
//     gchar *p = ep;
//
//     /* free memory for existing components */
//     if (app->fname) g_free (app->fname);
//     if (app->fext)  g_free (app->fext);
//     if (app->fpath) g_free (app->fpath);
//     app->fname = app->fext = app->fpath = NULL;
//
//     for (; *ep; ep++);  /* end of string */
//
//     /* separate extension */
//     p = ep;
//     while (*sp == '.') sp++;            /* handle dot files */
//     for (; p > sp && *p != '.'; p--);   /* find last '.' */
//
//     if (p != sp)    /* not dot file with no extension */
//         app->fext = g_strdup (p + 1);   /* set fext */
//
//     p = ep; /* separate path */
//     for (; p > app->filename && *p != '/'; p--);
//
//     if (p == app->filename) {
//         if (*p == '/') {    /* handle root path */
//             app->fname = g_strdup (app->filename + 1);
//             app->fpath = g_strdup ("/");
//         }
//         else    /* no path */
//             app->fname = g_strdup (app->filename);
//         return;
//     }
//
//     /* separate normal /path/filename */
//     app->fname = g_strdup (p + 1);
//     app->fpath = g_strndup (app->filename, p - app->filename);
// }

/** indent current/selected lines to the next softtab stop.
 *  text will be aligned to the next softtab on indent
 *  regardless of the number of spaces before the next
 *  softtab.
 */
void buffer_indent_lines (kwinst *app,
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

        nspaces = app->softtab - offset % app->softtab;

        ind_buffer = g_strdup_printf ("%*s", nspaces, " ");

        gtk_text_buffer_insert (buf, &iter, ind_buffer, -1);

        g_free (ind_buffer);

        /* reset start iter and start mark to beginning of line
         * so that selection continues to encompass entire first line.
         */
        if (start_line != end_line && i == start_line &&
            !gtk_text_iter_starts_line (&iter)) {
            gtk_text_iter_set_line_offset (&iter, 0);
            gtk_text_buffer_move_mark (buf, start_mark, &iter);
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

    /* adjust selection to cover entire first line */
    gtk_text_buffer_select_range (buf, end, start);

    /* delete GtkTextMark's */
    gtk_text_buffer_delete_mark (buf, start_mark);
    gtk_text_buffer_delete_mark (buf, end_mark);
}

/** unindent current/selected lines to the preceding softtab.
 *  text will be aligned to the prior softtab on unindent
 *  regardless of the number of spaces beyond the previous
 *  softtab text actually begins.
 */
void buffer_unindent_lines (kwinst *app,
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
        gboolean hastabs = FALSE;       /* special handling for tab chars */

        gtk_text_buffer_get_iter_at_line (buf, &iter_start, i);

        iter = iter_start;

        while (!gtk_text_iter_ends_line (&iter))
        {
            gunichar c;
            c = gtk_text_iter_get_char (&iter);

            if (c == '\t') {
                offset += indent_width;
                hastabs = TRUE;
            }
            else if (c == ' ')
                offset += 1;
            else
                break;

            gtk_text_iter_forward_char (&iter);
        }

        ndelete = offset % indent_width;

        /* tabs - del leading ws & insert correct spaces for unindent */
        if (hastabs) {
            char *ws = g_strdup_printf ("%*s", offset - ndelete, " ");
            if (!ws) {      /* handle error */
                g_print ("buffer_unindent_lines() error: g_strdup_printf.\n");
            }

            gtk_text_buffer_delete (buf, &iter_start, &iter);   /* del whitespace/tabs */
            gtk_text_buffer_get_iter_at_line (buf, &iter, i);   /* revalidate at start */
            gtk_text_buffer_insert (buf, &iter, ws, -1);        /* ins correct indent  */

            if (ws)
                g_free (ws);
        }
        else {  /* no tabs - all spaces */
            if (offset <= indent_width) {   /* within first softtab */
                gtk_text_buffer_delete (buf, &iter_start, &iter);
            }
            else if (!ndelete) {    /* backup full softtab width */
                gtk_text_iter_set_line_offset (&iter_start, offset - indent_width);
                gtk_text_buffer_delete (buf, &iter_start, &iter);
            }
            else {  /* bring indention to nex prior softtab */
                gtk_text_iter_set_line_offset (&iter_start, offset - ndelete);
                gtk_text_buffer_delete (buf, &iter_start, &iter);
            }
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

/** indent current/selected lines by softtab spaces.
 *  text will be indented by softtab spaces on indent
 *  preserving the existing number of space in line.
 */
void buffer_indent_lines_fixed (kwinst *app,
                                GtkTextIter *start,
                                GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line;
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

        gtk_text_buffer_get_iter_at_line (buf, &iter, i);

        ind_buffer = g_strdup_printf ("%*s", app->softtab, " ");

        gtk_text_buffer_insert (buf, &iter, ind_buffer, -1);

        g_free (ind_buffer);

        /* reset start iter and start mark to beginning of line
         * so that selection continues to encompass entire first line.
         */
        if (start_line != end_line && i == start_line &&
            !gtk_text_iter_starts_line (&iter)) {
            gtk_text_iter_set_line_offset (&iter, 0);
            gtk_text_buffer_move_mark (buf, start_mark, &iter);
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

    /* adjust selection to cover entire first line */
    gtk_text_buffer_select_range (buf, end, start);

    gtk_text_buffer_delete_mark (buf, start_mark);
    gtk_text_buffer_delete_mark (buf, end_mark);
}

/** Unindent current/selected lines by softtab spaces.
 *  text will be unindented by softtab spaces on unindent
 *  preserving the existing number of space in line.
 */
void buffer_unindent_lines_fixed (kwinst *app,
                                GtkTextIter *start,
                                GtkTextIter *end)
{
    GtkTextBuffer *buf;
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line, nspaces;

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

        GtkTextIter iter_start, iter;
        gint offset = 0;

        gtk_text_buffer_get_iter_at_line (buf, &iter, i);
        iter_start = iter;

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

        nspaces = offset >= app->softtab ? app->softtab : offset;

        /* set iter at nspaces and delete between iter_start - iter */
        gtk_text_iter_set_line_offset (&iter, nspaces);
        gtk_text_buffer_delete (buf, &iter_start, &iter);
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

/** callback handling eol_chk_default infobar */
void ib_eol_chk_default (GtkInfoBar *bar, gint response_id, kwinst *app)
{
    switch (response_id) {
        case GTK_RESPONSE_NO:       /* take no action */
            gtk_widget_hide (GTK_WIDGET(bar));
            break;
        case GTK_RESPONSE_YES:      /* convert eol */
            gtk_widget_hide (GTK_WIDGET(bar));
            if (app->eoldefault < FILE_EOL) {   /* default is LF, CRLF, or CR */
                if (app->eoldefault == LF)
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolLFMi),
                                                    TRUE);
                else if (app->eoldefault == CRLF)
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRLFMi),
                                                    TRUE);
                else if (app->eoldefault == CR)
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRMi),
                                                    TRUE);
                app->eol = app->eoldefault;
                app->oeol = app->eol;
            }
            else {  /* default is OS_EOL */
                if (app->eolos == LF)
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolLFMi),
                                                    TRUE);
                else if (app->eolos == CRLF)
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRLFMi),
                                                    TRUE);
                else
                    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRMi),
                                                    TRUE);
                app->eol = app->eolos;
                app->oeol = app->eol;
            }
            break;
    }

    /* set text_view sensitive TRUE */
    if (!gtk_widget_get_sensitive (app->view))
        gtk_widget_set_sensitive (app->view, TRUE);

    /* grab focus for textview */
    gtk_widget_grab_focus (app->view);

    /* reset flags */
    app->ibflags = 0;
}

/** EOL check against default setting on file open to warn of mismatch (infobar) */
void ibar_eol_chk_default (kwinst *app)
{
    ibbtndef btndef[] = { { .btntext = "_Yes",    .response_id = GTK_RESPONSE_YES },
                          { .btntext = "_No",     .response_id = GTK_RESPONSE_NO },
                          { .btntext = "",        .response_id = 0 } };

    app->ibflags = IBAR_VIEW_SENSITIVE;     /* make text_view insensitive on display */

    /* check against eoldefault, and if eoldefault != FILE_EOL or if
     * it differes from OS_EOL, warn of mismatch.
     */
    if (app->eoldefault != FILE_EOL && app->eol != app->eoldefault) {
        if (app->eoldefault != OS_EOL || app->eol != app->eolos) {
            gchar *msg = g_markup_printf_escaped ("File contains <span font_weight=\"bold\">"
                "'%s'</span> line ends.\n"
                "Selected default line end is: <span font_weight=\"bold\">'%s'</span>\n"
                "Operating-System default is: <span font_weight=\"bold\">'%s'</span>\n\n"
                "<span font_weight=\"bold\">Tools->End-of-Line Selection</span> -- "
                "to convert between line ends at any time.\n"
                "<span font_weight=\"bold\">Settings->File Load/Save->End-of-Line "
                "Handling</span> -- to change default setting.\n\n"
                "<span font_weight=\"bold\">Convert File to '%s' line ends?</span>",
                app->eolnm[app->eol], app->eoltxt[app->eoldefault],
                app->eolnm[app->eolos], app->eolnm[app->eolos]);
//             app->ibflags |= IBAR_VISIBLE;
            show_info_bar_choice (msg, GTK_MESSAGE_WARNING, btndef, ib_eol_chk_default, app);
            g_free (msg);
        }
    }
}

/** EOL check against default setting on file open to warn of mismatch */
void buffer_eol_chk_default (kwinst *app)
{
    /* check against eoldefault, and if eoldefault != FILE_EOL or if
     * it differes from OS_EOL, warn of mismatch.
     */
    if (app->eoldefault != FILE_EOL && app->eol != app->eoldefault) {
        if (app->eoldefault != OS_EOL || app->eol != app->eolos) {
            gchar *msg = g_strdup_printf ("File contains '%s' line ends.\n"
                "Selected default line end is: '%s'\n"
                "Operating-System default is: '%s'\n\n"
                "Tools->End-of-Line Selection to convert.\n\n"
                "Settings->File Load/Save->End-of-Line Handling\n"
                "to change default setting.\n\n"
                "Convert File to '%s' line ends?", app->eolnm[app->eol],
                app->eoltxt[app->eoldefault], app->eolnm[app->eolos],
                app->eolnm[app->eolos]);
            // dlg_info_win (app->window, msg, "End-of-Line Differs from Selection");
            if (dlg_yes_no_msg (app, msg, "End-of-Line Differs from Selection",
                                TRUE)) {
                if (app->eoldefault < FILE_EOL) {   /* default is LF, CRLF, of CR */
                    if (app->eoldefault == LF)
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolLFMi),
                                                        TRUE);
                    else if (app->eoldefault == CRLF)
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRLFMi),
                                                        TRUE);
                    else if (app->eoldefault == CR)
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRMi),
                                                        TRUE);
                    app->eol = app->eoldefault;
                    app->oeol = app->eol;
                }
                else {  /* default is OS_EOL */
                    if (app->eolos == LF)
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolLFMi),
                                                        TRUE);
                    else if (app->eolos == CRLF)
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRLFMi),
                                                        TRUE);
                    else
                        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRMi),
                                                        TRUE);
                    app->eol = app->eolos;
                    app->oeol = app->eol;
                }
            }
            g_free (msg);
        }
    }
}

/** determine current EOL by scanning buffer content */
void buffer_get_eol (kwinst *app)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    gint cnt = 0;

    /* check buffer character count */
    if (gtk_text_buffer_get_char_count (buffer) < 1)
        return;

    /* save current position */
    GtkTextMark *ins = gtk_text_buffer_get_insert (buffer);
    GtkTextIter iter;

    /* get start iterator in buffer */
    gtk_text_buffer_get_start_iter (buffer, &iter);

    /* disable text view and during EOL detection */
    gtk_widget_set_sensitive (app->view, FALSE);

    while (gtk_text_iter_forward_to_line_end (&iter)) {
        gunichar c = gtk_text_iter_get_char (&iter);

        if (c == '\n') {                /* do we have a linefeed */
            if (cnt && app->eol == LF)  /* if second - confirmed */
                break;
            else {
                app->eol = LF;          /* otherwise, set, check again */
                cnt = 1;
            }
        }
        else if (c == '\r') {           /* do we have a carriage-return */
            if (!gtk_text_iter_forward_char (&iter)) {  /* advance */
                app->eol = CR;
                break;
            }
            c = gtk_text_iter_get_char (&iter); /* get next char */
            if (c == '\n') {
                if (cnt && app->eol == CRLF)    /* we have CRLF */
                    break;
                else {
                    app->eol = CRLF;    /* set CRLF, check again */
                    cnt = 1;
                }
            }
            else if (cnt && app->eol == CR) {   /* we have a CR */
                break;
            }
            else {
                app->eol = CR;          /* set CR, check again */
                cnt = 1;
            }
        }
    }
#ifdef DEBUGEOL
    g_print ("buffer_get_eol() - before menu_item app->eol: '%s' (orig: '%s')\n",
            app->eolnm[app->eol], app->eolnm[app->oeol]);
#endif

//     /* check against eoldefault, and if eoldefault != FILE_EOL or if
//      * it differes from OS_EOL, warn of mismatch.
//      */
//     if (app->eoldefault != FILE_EOL && app->eol != app->eoldefault) {
//         if (app->eoldefault != OS_EOL || app->eol != app->eolos) {
//             /* TODO - use infobar instead of dialog */
//             gchar *msg = g_strdup_printf ("File contains '%s' line ends.\n"
//                 "Selected default line end is: '%s'\n"
//                 "Operating-System default is: '%s'\n\n"
//                 "Tools->End-of-Line Selection to convert.\n\n"
//                 "Settings->File Load/Save->End-of-Line Handling\n"
//                 "to change default setting.", app->eolnm[app->eol],
//                 app->eoltxt[app->eoldefault], app->eolnm[app->eolos]);
//             dlg_info_win (app->window, msg, "End-of-Line Differs from Selection");
//             g_free (msg);
//         }
//     }

    // buffer_eol_chk_default (app);

    /* update tools menu active EOL radio button
     * (do not set app->eolchg before to prevent
     *  firing on gtk_check_menu_item_set_active)
     */
    if (app->eol == LF)
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolLFMi), TRUE);
    else if (app->eol == CRLF)
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRLFMi), TRUE);
    else
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (app->eolCRMi), TRUE);

    if (app->eol != app->oeol) {
        app->oeol = app->eol;   /* set orignal to current */
    }
    app->eolchg = TRUE;         /* eol changed */

    /* restore original insert position */
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, ins);
    gtk_text_buffer_place_cursor (buffer, &iter);

    /* enable text view and after EOL detection */
    gtk_widget_set_sensitive (app->view, TRUE);

#ifdef DEBUGEOL
    g_print ("buffer_get_eol() - after menu_item app->eol: '%s' (orig: '%s')\n",
            app->eolnm[app->eol], app->eolnm[app->oeol]);
#endif

#ifdef DEBUG
    switch (app->eol) {
        case 0: g_print ("app->eol: %d (LF)\n", app->eol); break;
        case 1: g_print ("app->eol: %d (CRLF)\n", app->eol); break;
        case 2: g_print ("app->eol: %d (CR)\n", app->eol); break;
    }
#endif

}

/** convert all end-of-line in file to selected app->eol.
 *  traverse buffer from start to end converting all end-of-line
 *  terminating characters to the user selected app->eol when
 *  a change in the end-of-line setting occurs. (or on save)
 */
void buffer_convert_eol (kwinst *app)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter iter;

#ifdef DEBUGEOL
    g_print ("buffer_convert_eol() - entered.\n");
#endif

    /* if no change -- return */
    if (app->eol == app->oeol || (!app->eolchg && !app->modified))
        return;

    if (!buffer) {  /* validate buffer */
        err_dialog ("Error: Invalid 'buffer' passed to function\n"
                    "buffer_remove_trailing_ws (GtkTextBuffer *buffer)");
        return;
    }

#ifdef DEBUGEOL
    g_print ("buffer_convert_eol() - running: %s -> %s\n",
            app->eolnm[app->oeol], app->eolnm[app->eol]);
#endif

    /* get iter at start of buffer */
    gtk_text_buffer_get_start_iter (buffer, &iter);

    if (app->last_pos) {    /* delete app->last_pos mark, if set */
        gtk_text_buffer_delete_mark (buffer, app->last_pos);
        app->last_pos = NULL;
    }

    /* set app->last_pos Mark to start, and move on each iteration */
    app->last_pos = gtk_text_buffer_create_mark (buffer, "last_pos", &iter, FALSE);

    /* loop, moving to the end of each line, before the EOL chars */
    while (gtk_text_iter_forward_to_line_end (&iter)) {

        gunichar c = gtk_text_iter_get_char (&iter);
        gtk_text_buffer_move_mark (buffer, app->last_pos, &iter);

        if (c == '\n') {        /* if end-of-line begins with LF */
            gtk_text_iter_forward_char (&iter);
            gtk_text_buffer_backspace (buffer, &iter, FALSE, TRUE);
            gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);

            if (app->eol == CRLF)       /* handle change to CRLF */
                gtk_text_buffer_insert (buffer, &iter, app->eolstr[CRLF], -1);
            else if (app->eol == CR)    /* handle change to CR */
                gtk_text_buffer_insert (buffer, &iter, app->eolstr[CR], -1);
        }
        else if (c == '\r') {   /* if end-of-line begins with CR */
            if (app->eol == LF) {   /* handle change to LF */
                gtk_text_iter_forward_char (&iter);
                if (gtk_text_buffer_backspace (buffer, &iter, FALSE, TRUE)) {
                    gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);
                    if (gtk_text_iter_get_char (&iter) != '\n') {
                        gtk_text_buffer_insert (buffer, &iter, app->eolstr[LF], -1);
                    }
                }
            }
            else if (app->eol == CRLF) {    /* handle change to CRLF */
                gtk_text_iter_forward_char (&iter);
                if (gtk_text_iter_get_char (&iter) != '\n') {
                    if (gtk_text_buffer_backspace (buffer, &iter, FALSE, TRUE)) {
                        gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);
                        gtk_text_buffer_insert (buffer, &iter, app->eolstr[CRLF], -1);
                    }
                    /* CRLF a single char and can't be created? */
                    // gtk_text_buffer_insert (buffer, &iter, app->eolstr[LF], -1);
                }
            }
            else {  /* handle change to CR */
                if (gtk_text_iter_forward_char (&iter)) {
                    if (gtk_text_iter_get_char (&iter) == '\n') {
                        gtk_text_iter_forward_char (&iter);
                        gtk_text_buffer_backspace (buffer, &iter, FALSE, TRUE);
                    }
                }
            }
        }
        else {
            /* handle no-eol error */
#ifdef DEBUGEOL
            g_print ("buffer_convert_eol() error: no-eol found.\n");
#endif
        }
        /* revalidate interator with last_pos mark */
        gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);
    }

    if (app->last_pos) {    /* delete app->last_pos mark, if set */
        gtk_text_buffer_delete_mark (buffer, app->last_pos);
        app->last_pos = NULL;
    }

    app->oeol = app->eol;   /* update original eol to current */
}

/** helper function for insert_eol and indent_auto actions after "mark-set".
 *  needed because "mark-set" fires before indent and eol inserted causing
 *  app->{line,col,lines} not to update statusbar on [Enter]
 */
void buffer_update_pos (kwinst *app)
{
    /* Fri Jul 13 2018 18:52:36 CDT
     * moved line/col update to status_set_default to avoid
     * mark_set firing when gtk_text_view_scroll_mark_onscreen
     * is called, causing line number to reset to 1 or jump by
     * the number of lines scrolled in forward direction.
     */
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    GtkTextMark *ins = gtk_text_buffer_get_insert (buffer);

    /* scroll text-view to keep cursor position in window */
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view), ins);
}

/** insert configured EOL at cursor position on Return/Enter. */
gboolean buffer_insert_eol (kwinst *app)
{
    /* validate eolstr[x] not NULL and not empty */
    if (!app->eolstr[app->eol] || !*app->eolstr[app->eol])
        return FALSE;   /* fallback to default keystroke handler */

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);

    /* insert EOL at cursor */
    gtk_text_buffer_insert_at_cursor (buffer, app->eolstr[app->eol], -1);

    /* update 'line/lines col' on statusbar */
    buffer_update_pos (app);

    return TRUE;    /* keypress handled */
}

/** auto-indent on return */
gboolean buffer_indent_auto (kwinst *app)
{
    GtkTextMark *ins;
    GtkTextIter end, iter;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    gchar *indstr = NULL;
    gint line, nspaces = 0;

    ins = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &end, ins);
    line = gtk_text_iter_get_line (&end);
    gtk_text_buffer_get_iter_at_line (buffer, &iter, line);

    for (;;) {
        gunichar c;
        c = gtk_text_iter_get_char (&iter);

        if (c == '\t' || c == ' ')
            nspaces += (c == '\t') ? app->softtab : 1;
        else
            break;

        gtk_text_iter_forward_char (&iter);
    }

    // app->indentpl = nspaces;    /* set previous line indent */

    if (nspaces) {
        // indstr = g_strdup_printf ("\n%*s", nspaces, " ");
        indstr = g_strdup_printf ("%s%*s", app->eolstr[app->eol], nspaces, " ");
        gtk_text_buffer_insert_at_cursor (buffer, indstr, -1);
        g_free (indstr);

        /* update 'line/lines col' on statusbar */
        buffer_update_pos (app);

        return TRUE;
    }
    else
        return buffer_insert_eol (app);

    return FALSE;
}

/** remove all whitespace to prior softtab stop on backspace.
 *  this function will only remove 'spaces', all other backspace
 *  is handled by the default keyboard handler.
 */
gboolean smart_backspace (kwinst *app)
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

        gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER (app->buffer));

        status_set_default (app);

        return TRUE;    /* return without further handling */
    }

    return FALSE;   /* return FALSE for default handling */
}

gboolean smart_tab (kwinst *app)
{
    GtkTextMark *ins;
    GtkTextIter beg, end, iter;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    gchar *tabstr = NULL;
    gint nspaces = 0;

    if (!app->expandtab) return FALSE;  /* default handler */

    /* validate no selection exists */
    if (gtk_text_buffer_get_selection_bounds (buffer, &beg, &end))
        return FALSE;

    /* get "insert" mark, then current line/column, set end iter */
    ins = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, ins);
    app->col = gtk_text_iter_get_visible_line_offset (&iter);

    /* TODO: iterate to beginning to insert to check if all leading
     * whitespace, if so, if app->indontab, then call indent function
     * for line and return TRUE
     */

    gtk_text_buffer_begin_user_action (buffer);

    nspaces = app->softtab - app->col % app->softtab;
    tabstr = g_strdup_printf ("%*s", nspaces, " ");
    gtk_text_buffer_insert (buffer, &iter, tabstr, -1);

    gtk_text_buffer_end_user_action (buffer);

    gtk_text_buffer_get_iter_at_mark (buffer, &iter, ins);
    app->line = gtk_text_iter_get_line (&iter);
    app->col = gtk_text_iter_get_visible_line_offset (&iter);

    g_free (tabstr);

    return TRUE;
}

/** move cursor to beginning text on first keypress, line start on next.
 *  sequential GDK_KEY_Home keypress events are stored in app->kphome.
 *  if app->kphome is not set, move to text start, else move to start
 *  of line.
 */
gboolean smart_home (kwinst *app)
{
#ifdef SMARTHEDEBUG
    g_print ("on smart_home entry, app->kphome: %s, app->smarthe: %s\n",
            app->kphome ? "TRUE" : "FALSE", app->smarthe ? "TRUE" : "FALSE");
#endif
    GtkTextMark *ins;
    GtkTextIter start, insiter, iter;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    gunichar c = 0;

    if (app->kphome)
        return ((app->kphome = FALSE));

    /* get "insert" mark, then iter at beginning of line */
    ins = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &insiter, ins);
    iter = insiter;
    gtk_text_iter_set_line_offset (&iter, 0);
    start = iter;

    /* iter forward to first non-whitespace or end */
    while (!gtk_text_iter_ends_line (&iter))
    {
        c = gtk_text_iter_get_char (&iter);

        if (c != '\t' && c != ' ' && c != 0xFFFC)
            break;

        gtk_text_iter_forward_char (&iter);
    }

    /* place cursor */
    if (c == ' ' || c == '\t')
        gtk_text_buffer_place_cursor (buffer, &start);
    else
        gtk_text_buffer_place_cursor (buffer, &iter);

    return ((app->kphome = TRUE));
}

/** remove all trailing whitespace from buffer */
void buffer_remove_trailing_ws (GtkTextBuffer *buffer)
{
    GtkTextIter iter, iter_from, iter_end;
    gunichar c;
    gint line_end;

    if (!buffer) {
        err_dialog ("Error: Invalid 'buffer' passed to function\n"
                    "buffer_remove_trailing_ws (GtkTextBuffer *buffer)");
        return;
    }

    /* get iter at start of buffer */
    gtk_text_buffer_get_start_iter (buffer, &iter);
    gtk_text_buffer_get_end_iter (buffer, &iter_end);

    line_end =  gtk_text_iter_get_line (&iter_end);

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
        if (line == line_end) {
            gtk_text_buffer_get_iter_at_line (buffer, &iter, line);
            gtk_text_iter_forward_to_line_end (&iter);
            break;
        }
        gtk_text_buffer_get_iter_at_line (buffer, &iter, line + 1);
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

void buffer_require_posix_eof (kwinst *app)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter end;

    gtk_text_buffer_get_end_iter (buffer, &end);

    if (gtk_text_iter_backward_char (&end)) {
        gunichar c = gtk_text_iter_get_char (&end);
        if (c != '\n' && c != '\r') {
            gtk_text_iter_forward_char (&end);
            gtk_text_buffer_insert (buffer, &end, app->eolstr[app->eol], -1);
        }
#ifdef DEBUGBUF
        else
            g_print ("end char is '%0x%02x'\n", c);
#endif
    }
}

/** gather buffer character, word and line statistics.
 *  traverse the buffer, gathering buffer statistics, including the
 *  number of whitespace and non-whitespace characters, the total,
 *  the number of words and the number of lines. present in a simple
 *  dialog.
 *  TODO: present in a formatted dialog with option to save.
 */
// void buffer_content_stats (GtkTextBuffer *buffer)
void buffer_content_stats (kwinst *app)
{
    GtkTextIter iter;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    gint ws = 0, wsc = 0, nws = 0, nwrd = 0, other = 0, lines = 1;
    gboolean ldws = FALSE, lws = FALSE, havechars = FALSE;
    gunichar c;


    if (!buffer) {
        err_dialog ("Error: Invalid 'buffer' passed to function\n"
                    "buffer_remove_trailing_ws (GtkTextBuffer *buffer)");
        return;
    }

    /* get iter at start of buffer */
    gtk_text_buffer_get_start_iter (buffer, &iter);

    do {
        c = gtk_text_iter_get_char (&iter);

        /* test if iter at end of line */
        if (gtk_text_iter_ends_line (&iter)) {
            if (c == '\r' || c == '\n') { /* loop over all */
                while (c == '\r' || c == '\n') {
                    gchar current = c;
                    wsc++;              /* increment whitespace */
                    if (!gtk_text_iter_forward_char (&iter)) {
                        lines++;        /* end, add line */
                        goto wsdone;    /* goto done */
                    }
                    if (c == '\n')      /* if newline found */
                        lines++;        /* increment lines */
                    c = gtk_text_iter_get_char (&iter); /* get next char */
                    if (current == '\r' && c != '\n')   /* if CR alone   */
                        lines++;        /* increment lines Max (pre-OSX) */
                }
                /* not line-end, backup for next iteration */
                gtk_text_iter_backward_char (&iter);
            }
            if (havechars)              /* if have chars in line */
                nwrd += ws + 1;         /* number of words */
            ws = 0;                     /* word sep per line */
            lws = FALSE;                /* reset last was ws */
            havechars = FALSE;          /* reset havechars */
        }
        else {  /* checking chars in line */

            if (c == ' ' || c == '\t') {
                wsc++;                  /* add whitespace char */
                lws = TRUE;             /* set last ws TRUE */
                if (!havechars)         /* if no chars */
                    ldws = TRUE;        /* set leading whitespace */
            }
            else if (c == 0xFFFC)       /* other/tag char */
                other++;
            else {
                havechars = TRUE;       /* chars in line */
                nws++;                  /* add to non-whitespace */
                if (lws) {              /* if last flag set */
                    lws = FALSE;        /* unset */
                    if (!ldws)          /* if not leading whitespace */
                        ws++;           /* increment word-sep */
                }
                ldws = FALSE;           /* reset leading whitespace */
            }
        }

    } while (gtk_text_iter_forward_char (&iter));
    wsdone:;

    if (havechars)
        nwrd += ws + 1;

    if (!gtk_text_iter_is_end (&iter))
        g_print ("error: not end iter after exiting loop.\n");

    gchar *stats;

    stats = g_strdup_printf ("whitespace characters: %d\n"
                            "non-whitespace chars: %d\n"
                            "other characters : %d\n"
                            "total characters : %d\n"
                            "\nnumber of words: %d\n"
                            "number of lines: %d\n",
                            wsc, nws, other,
                            wsc + nws + other, nwrd, --lines);

    // dlg_info (stats, "Buffer Content Statistics");
    app->ibflags = IBAR_LABEL_SELECT;
    show_info_bar_ok (stats, GTK_MESSAGE_INFO, app);

    g_free (stats);

#ifdef DEBUG
    g_printf ("\nws  : %d\nnws : %d\noth : %d\nchr : %d\nwrd : %d\nlns : %d\n\n",
            wsc, nws, other, wsc + nws + other, nwrd, lines);
    g_printf ("line count: %d\n", gtk_text_buffer_get_line_count (buffer));
    g_printf ("char count: %d\n", gtk_text_buffer_get_char_count (buffer));
#endif
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
