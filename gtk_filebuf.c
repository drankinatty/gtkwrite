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

#ifdef HAVESOURCEVIEW

/** set sourceview syntax scheme based on laststyle used.
 *  set the current sourceview buffer contents to the saved scheme.
 */
void sourceview_set_syntax_laststyle (gpointer data)
{
    kwinst *app = (kwinst *)data;
    GtkSourceStyleSchemeManager *sm;
    GtkSourceStyleScheme *scheme;

    sm = gtk_source_style_scheme_manager_get_default();
    scheme = gtk_source_style_scheme_manager_get_scheme (sm, app->laststyle);
    gtk_source_buffer_set_style_scheme (app->buffer, scheme);
}

void sourceview_guess_language (kwinst *app)
{
    if (app->language) return;  /* prevent changing manually applied language */

    gboolean result_uncertain;
    gchar *content_type;

    app->langmgr = gtk_source_language_manager_get_default();

    content_type = g_content_type_guess (app->filename, NULL,
                                        0, &result_uncertain);
    if (result_uncertain)
    {
        g_free (content_type);
        content_type = NULL;
    }

    app->language = gtk_source_language_manager_guess_language (app->langmgr,
                                        app->filename, content_type);
    gtk_source_buffer_set_language (app->buffer, app->language);

    if (app->laststyle)
        sourceview_set_syntax_laststyle (app);

    gtk_source_buffer_set_highlight_syntax (app->buffer, app->highlight);

    g_free (content_type);

}

void sourceview_get_languange_info (kwinst *app)
{
    if (!app->language) return;

    const gchar *name, *style_id, *section, *metadata, *style_name;
    const gchar * const *lang_ids;
    gchar **mime_types, **globs;

    /* lang_ids is owned by lm and must not be modified */
    lang_ids = gtk_source_language_manager_get_language_ids (app->langmgr);

    printf ("\nlang_ids:\n");
    for (gint i = 0; lang_ids[i]; i++)
        g_print ("  %2d : %s\n", i, lang_ids[i]);

    /* after getting lang_ids, to get language use */
    /* const char *lang_id = lang_ids[foo];
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language (app->langmgr,
                                                                        lang_id);
     */
    /* then apply with */
    /*
    gtk_source_buffer_set_language (buffer, lang);
     */

    style_id   = gtk_source_language_get_id (app->language);
    name       = gtk_source_language_get_name (app->language);
    section    = gtk_source_language_get_section (app->language);
    metadata   = gtk_source_language_get_metadata (app->language, name);
    style_name = gtk_source_language_get_style_name (app->language, style_id);

    g_print ("language style_id   : %s\n"
             "language name       : %s\n"
             "language section    : %s\n"
             "language metadata   : %s\n"
             "language style name : %s\n",
             style_id, name, section, metadata, style_name);

    /* get mime_types */
    mime_types = gtk_source_language_get_mime_types (app->language);

    printf ("\nmime_types:\n");
    for (gint i = 0; mime_types[i]; i++)
        g_print ("  %2d : %s\n", i, mime_types[i]);

    /* get globs */
    globs = gtk_source_language_get_globs (app->language);

    printf ("\nglobs:\n");
    for (gint i = 0; globs[i]; i++)
        g_print ("  %2d : %s\n", i, globs[i]);

    /* free pointer arrays */
    g_strfreev (mime_types);
    g_strfreev (globs);
}

void sourceview_get_scheme_info (kwinst *app)
{
    GtkSourceStyleSchemeManager *sm;
    // GtkSourceStyle style;
    const gchar * const *srchpath = NULL;
    const gchar * const *scheme_ids = NULL;

    /* get default SchemeManager and related info */
    sm = gtk_source_style_scheme_manager_get_default();
    srchpath = gtk_source_style_scheme_manager_get_search_path (sm);
    scheme_ids = gtk_source_style_scheme_manager_get_scheme_ids (sm);

    g_print ("\nThe SchemeManager search paths:\n");
    for (gint i = 0; srchpath[i]; i++)
        g_print (" %2d.  %s\n", i, srchpath[i]);

    g_print ("\nThe SchemeManager scheme_ids:\n");
    /* get scheme info from scheme_ids */
    for (gint i = 0; scheme_ids[i]; i++)
    {
        GtkSourceStyleScheme *scheme;
        const gchar *scheme_id, *scheme_name, *scheme_desc, *scheme_fn;

        scheme_id = scheme_ids[i];

        scheme = gtk_source_style_scheme_manager_get_scheme (sm, scheme_id);

        scheme_id = gtk_source_style_scheme_get_id (scheme);
        scheme_name = gtk_source_style_scheme_get_name (scheme);
        scheme_desc = gtk_source_style_scheme_get_description (scheme);
        scheme_fn = gtk_source_style_scheme_get_filename (scheme);

        g_print ("\n  scheme_id  : %s\n"
                 "  scheme_name: %s\n"
                 "  scheme_desc: %s\n"
                 "  scheme_fn  : %s\n",
                 scheme_id, scheme_name, scheme_desc, scheme_fn);

        /* you can load a style within a scheme with its 'style_id' */
//         GtkSourceStyle *style;
//         const gchar *style_id = "c"; /* TEST: hardcoded to "c" */
//         style = gtk_source_style_scheme_get_style (scheme, style_id);
//         if (style) {
//             /* make a copy of the style for modification */
//             GtkSourceStyle *copy;
//             copy = gtk_source_style_copy (style);
//             /* modify and set properties */
//             g_print ("  SUCCESS 'c' within scheme.\n");
//             g_object_unref (copy);
//         }
    }

    /* All style schemes will be reloaded next time the manager is accessed. */
    // gtk_source_style_scheme_manager_force_rescan (sm);

    if (app) {}
}
#endif

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

/** open file or insert file at cursor.
 *  if filename is given, the file is inserted at the cursor without
 *  changing the current filename within the editor, otherwise,
 *  simply opens the file contained in app->filename.
 */
void buffer_insert_file (kwinst *app, gchar *filename)
{
    /* TODO: fix way filename is passed from argv, use it */
    gchar *filebuf = NULL;
    gchar *status = NULL;
    gchar *fnameok = filename;
    // GtkTextView *view = GTK_TEXT_VIEW (app->view);
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    // GtkTextIter iter;
    // GtkTextMark *sof;

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

        /*
         * scroll to insert mark
         */
        gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(app->view),
                                      gtk_text_buffer_get_insert (buffer),
                                      0.0, TRUE, 0.0, 1.0);

        if (fnameok) { /* inserting file at cursor */
            gtk_text_buffer_set_modified (buffer , TRUE);  /* inserted */
            app->modified = TRUE;
            status = g_strdup_printf ("inserted : '%s'", filename);
        }
        else {  /* opening file */
            file_get_stats (filename, app); /* save file mode, UID, GID */
            // buffer_get_eol (app);           /* detect EOL, LF, CRLF, CR (moved down) */
            gtk_text_buffer_set_modified (buffer , FALSE);    /* opened */
            app->modified = FALSE;
            buffer_get_eol (app);           /* detect EOL, LF, CRLF, CR */
            status = g_strdup_printf ("loaded : '%s'", app->fname);
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
        /* TODO: change to error dialog */
        status = g_strdup_printf ("file read failed : '%s'", app->fname);
    }
    status_update_str (app, status);
    g_free (status);
    /* reset values to default */
    status_set_default (app);

    /* set syntax highlighting language */
#ifdef HAVESOURCEVIEW
    if (app->highlight)
        sourceview_guess_language (app);
#endif
    /* temporary dump of language information */
    // sourceview_get_languange_info (app);
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

void buffer_comment_lines (kwinst *app,
                          GtkTextIter *start,
                          GtkTextIter *end)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line;
    const gchar *commentstr = app->comment;
//     const gchar *commentstr = "// ";


#ifdef HAVESOURCEVIEW
    gboolean bracket_hl;
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer), FALSE);
#endif

    start_mark = gtk_text_buffer_create_mark (buffer, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buffer, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iter;

        gtk_text_buffer_get_iter_at_line (buffer, &iter, i);

        gtk_text_buffer_insert (buffer, &iter, commentstr, -1);
    }

    gtk_text_buffer_end_user_action (buffer);   /* end user action */

#ifdef HAVESOURCEVIEW
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer),
                                                        bracket_hl);
#endif

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);

    gtk_text_buffer_delete_mark (buffer, start_mark);
    gtk_text_buffer_delete_mark (buffer, end_mark);
}

void buffer_uncomment_lines (kwinst *app,
                          GtkTextIter *start,
                          GtkTextIter *end)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextMark *start_mark, *end_mark;
    gint i, start_line, end_line;
    const gchar *commentstr = app->comment;


#ifdef HAVESOURCEVIEW
    gboolean bracket_hl;
    bracket_hl = gtk_source_buffer_get_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer));
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer), FALSE);
#endif

    start_mark = gtk_text_buffer_create_mark (buffer, NULL, start, FALSE);
    end_mark = gtk_text_buffer_create_mark (buffer, NULL, end, FALSE);

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if ((gtk_text_iter_get_visible_line_offset (end) == 0) &&
        (end_line > start_line)) {
            end_line--;
    }

    gtk_text_buffer_begin_user_action (buffer); /* begin user action */

    for (i = start_line; i <= end_line; i++) {

        GtkTextIter iterstart, iter;
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

    gtk_text_buffer_end_user_action (buffer);   /* end user action */

#ifdef HAVESOURCEVIEW
    gtk_source_buffer_set_highlight_matching_brackets (GTK_SOURCE_BUFFER (buffer),
                                                        bracket_hl);
#endif

    /* revalidate iters */
    gtk_text_buffer_get_iter_at_mark (buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);

    gtk_text_buffer_delete_mark (buffer, start_mark);
    gtk_text_buffer_delete_mark (buffer, end_mark);
}

gboolean buffer_chk_save_on_exit (GtkTextBuffer *buffer)
{
    if (!buffer) return FALSE;

    if (gtk_text_buffer_get_modified (buffer) == TRUE)
        return dlg_yes_no_msg (NULL, "Do you want to save the changes you have made?",
                                "Save File?", TRUE);

    return FALSE;
}

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
    GtkTextIter start, end/*, last*/;

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
    /* TODO: create common set title function for all dialogs */
    /* (e.g. if (widget == app->window), then window title, else dialog */
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
            // longtitle = g_strdup_printf ("%s - %s", app->appshort, app->filename);;

        }
        else
            title = g_strdup_printf ("%s - untitled", app->appshort);

    }

    gtk_window_set_title (GTK_WINDOW (app->window), title);
    // gtk_widget_set_tooltip_text (app->window, longtitle);

    g_free (title);
    // g_free (longtitle);

    if (widget) {}
}

void split_fname (kwinst *app)
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

        if (c == '\n') {            /* if end-of-line begins with LF */
            if (app->eol == CRLF) { /* handle change to CRLF */
                gtk_text_buffer_insert (buffer, &iter, app->eolstr[CR], -1);
            }
            else if (app->eol == CR) {  /* handle change to CR */
                gtk_text_iter_forward_char (&iter);
                if (gtk_text_buffer_backspace (buffer, &iter, FALSE, TRUE)) {
                    gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);
                    gtk_text_buffer_insert (buffer, &iter, app->eolstr[CR], -1);
                }
            }
        }
        else if (c == '\r') {       /* if end-of-line begins with CR */
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

/** insert configured EOL at cursor position on Return/Enter. */
gboolean buffer_insert_eol (kwinst *app)
{
    /* validate eolstr[x] not NULL and not empty */
    if (!app->eolstr[app->eol] || !*app->eolstr[app->eol])
        return FALSE;   /* fallback to default keystroke handler */

    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);

    /* insert EOL at cursor */
    gtk_text_buffer_insert_at_cursor (buffer, app->eolstr[app->eol], -1);

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
void buffer_content_stats (GtkTextBuffer *buffer)
{
    GtkTextIter iter;
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
                    wsc++;          /* increment whitespace */
                    if (!gtk_text_iter_forward_char (&iter)) {
                        lines++;        /* end, add line */
                        goto wsdone;    /* goto done */
                    }
                    c = gtk_text_iter_get_char (&iter);
                }
                lines++;            /* incriment lines */
                gtk_text_iter_backward_char (&iter);
            }
            if (havechars)          /* if have chars in line */
                nwrd += ws + 1;     /* number of words */
            ws = 0;                 /* word sep per line */
            lws = FALSE;            /* reset last was ws */
            havechars = FALSE;      /* reset havechars */
        }
        else {  /* checking chars in line */

            if (c == ' ' || c == '\t') {
                wsc++;              /* add whitespace char */
                lws = TRUE;         /* set last ws TRUE */
                if (!havechars)     /* if no chars */
                    ldws = TRUE;    /* set leading whitespace */
            }
            else if (c == 0xFFFC)   /* other/tag char */
                other++;
            else {
                havechars = TRUE;   /* chars in line */
                nws++;              /* add to non-whitespace */
                if (lws) {          /* if last flag set */
                    lws = FALSE;    /* unset */
                    if (!ldws)      /* if not leading whitespace */
                        ws++;       /* increment word-sep */
                }
                ldws = FALSE;       /* reset leading whitespace */
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

    dlg_info (stats, "Buffer Content Statistics");

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
