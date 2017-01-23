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

/* goto keypress handler */
/* TODO: add handling by combobox and skip handler? (see snip below) */
static gboolean on_goto_keypress (GtkWidget *widget, GdkEventKey *event,
                                  context *app)
{
    switch (event->keyval)
    {
        case GDK_KEY_Escape:
            goto_btnclose (widget, app);
            // return TRUE;    /* return TRUE - no further processing */
            break;
        case GDK_KP_Enter:  /* toggle focus to set value in spinbtn */
            gtk_widget_child_focus (app->gotowin, GTK_DIR_RIGHT);
            gtk_widget_child_focus (app->gotowin, GTK_DIR_LEFT);
            goto_btnfind (widget, app);
            return TRUE;
        case GDK_Return:
            gtk_widget_child_focus (app->gotowin, GTK_DIR_RIGHT);
            gtk_widget_child_focus (app->gotowin, GTK_DIR_LEFT);
            goto_btnfind (widget, app);
            goto_btnclose (widget, app);
            return TRUE;
    }

    return FALSE;
}

// static void scale_set_default_values( GtkScale *scale )
// {
//     gtk_range_set_update_policy (GTK_RANGE (scale),
//                                  GTK_UPDATE_CONTINUOUS);
//     gtk_scale_set_digits (scale, 0);
//     gtk_scale_set_value_pos (scale, GTK_POS_TOP);
//     gtk_scale_set_draw_value (scale, TRUE);
// }

GtkWidget *create_goto_dlg (context *app)
{
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *adjhbox;
    GtkWidget *hbox;
    // GtkWidget *image;   /* nix image for now - use cairo for own */
    // GtkWidget *btnfind;
    GtkWidget *btnclose;    /* TODO: confirm reuse is no problem */

    gint line = app->line;
    // gint line = gtk_text_iter_get_line (iter);
    gint last = gtk_text_buffer_get_line_count (app->buffer);

    /* initialize persistent variables */
    app->new_pos = NULL;

    /* value, lower, upper, step_increment, page_increment, page_size */
    GtkObject *adj = gtk_adjustment_new (line, 1.0, last, 1.0, 1.0, 0.0);

    if (last == 1) {
        err_dialog ("Error:\n\ninsufficient lines in buffer.");
        return NULL;
    }

    /* create toplevel window */
    if (!(app->gotowin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_find_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->gotowin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->gotowin), 185, 185);
    gtk_window_set_title (GTK_WINDOW (app->gotowin), "Goto Line");
    gtk_container_set_border_width (GTK_CONTAINER (app->gotowin), 5);
    g_signal_connect (app->gotowin, "destroy",
		      G_CALLBACK (goto_btnclose), app);

    /* main vbox container
     * spacing profided on frames as containers */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->gotowin), vbox);
    gtk_widget_show (vbox);

//     image = gtk_image_new_from_stock (GTK_STOCK_INDEX, GTK_ICON_SIZE_DND);
//     gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, FALSE, 0);
//     gtk_widget_show (image);

    adjhbox = gtk_hbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (vbox), adjhbox);

    app->spinbtn = gtk_spin_button_new (GTK_ADJUSTMENT(adj), 1.0, 0);
    gtk_box_pack_start (GTK_BOX (adjhbox), app->spinbtn, TRUE, TRUE, 0);
    gtk_widget_show (app->spinbtn);
    gtk_widget_show (adjhbox);

    app->vscale = gtk_vscale_new (GTK_ADJUSTMENT (adj));
    // scale_set_default_values (GTK_SCALE (app->vscale));
    gtk_range_set_update_policy (GTK_RANGE (app->vscale),
                                 GTK_UPDATE_CONTINUOUS);
    gtk_scale_set_digits (GTK_SCALE (app->vscale), 0);
    gtk_scale_set_value_pos (GTK_SCALE (app->vscale), GTK_POS_TOP);
    gtk_scale_set_draw_value (GTK_SCALE (app->vscale), TRUE);
    gtk_box_pack_start (GTK_BOX (adjhbox), app->vscale, TRUE, TRUE, 0);
    gtk_widget_show (app->vscale);

    /* hbox button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    app->btnfind = gtk_button_new_with_mnemonic ("_Goto");
    gtk_widget_set_size_request (app->btnfind, 80, 24);
    gtk_widget_set_sensitive (app->btnfind, (line != app->line));
    gtk_widget_show (app->btnfind);

    btnclose = gtk_button_new_with_mnemonic ("_Close");
    gtk_widget_set_size_request (btnclose, 80, 24);

    gtk_box_pack_end (GTK_BOX (hbox), btnclose, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), app->btnfind, FALSE, FALSE, 0); // pack on rt-side.
    gtk_widget_show (btnclose);

    gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    gtk_widget_show (app->gotowin);

//     g_signal_connect (app->spinbtn, "preedit-changed",
//                       G_CALLBACK (goto_spinbtn_preedit), app);

    g_signal_connect (app->spinbtn, "value-changed",
                      G_CALLBACK (goto_spinbtn_changed), app);

    g_signal_connect (app->btnfind, "clicked",
                      G_CALLBACK (goto_btnfind), app);

    g_signal_connect (btnclose, "clicked",
                      G_CALLBACK (goto_btnclose), app);

    g_signal_connect (app->gotowin, "key_press_event",
                      G_CALLBACK (on_goto_keypress), app);

    return (app->gotowin);
}

// void goto_spinbtn_preedit (GtkWidget *widget, gchar *txt, context *app)
// {
//     g_print ("preedit text: %s\n", txt);
//     gtk_widget_set_sensitive (app->btnfind, TRUE);
//     if (widget) {}
// }

void goto_spinbtn_changed (GtkWidget *widget, context *app)
{
    gtk_widget_set_sensitive (app->btnfind, TRUE);
    if (widget) {}
}

void goto_btnfind (GtkWidget *widget, context *app)
{
    GtkTextIter liter;
    app->line = gtk_spin_button_get_value_as_int (
                    GTK_SPIN_BUTTON(app->spinbtn)) - 1; /* subtract 1 */
#ifdef DEBUG
    g_print ("goto line: %d\n", app->line);
#endif
    gtk_text_buffer_get_iter_at_line (app->buffer, &liter, app->line);
    app->new_pos = gtk_text_buffer_create_mark (app->buffer,
                                    "new_line", &liter, FALSE);
//     gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
//                                         app->new_pos);
    gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                app->new_pos, 0.0, TRUE, 0.95, 0.8);
    /* must place cursor after scroll or statusbar will reflect scroll */
    gtk_text_buffer_place_cursor (app->buffer, &liter);

    if (widget) {}
    if (app) {}
}

void goto_btnclose (GtkWidget *widget, context *app)
{
    if (app->new_pos)
        gtk_text_buffer_delete_mark (app->buffer, app->new_pos);
    app->new_pos = NULL;

    gtk_widget_destroy (app->gotowin);

    if (widget) {}
}
