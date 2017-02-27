#include "gtk_windef.h"
#include "gtk_findreplace.h"

/* TODO:
 *  move all indent implementation to gtk_indent.[ch]
 */

/** create application window & initialize values
 *  and connect callback functions. 'app' contains
 *  widgets for window, text_view and statusbar.
 */
GtkWidget *create_window (kwinst *app)
{
    GtkAccelGroup *mainaccel;
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *menubar;         /* menu container   */
    GtkWidget *scrolled_window; /* container for text_view */

    PangoFontDescription *font_desc;
    GtkTextIter iterfirst;
    /* TODO: initialize all values in gtk_appdata.c */
    app->fontname = g_strdup ("DejaVu Sans Mono 8");
    app->line = 0;          /* initialize beginning pos line/col  */
    app->col = 0;

    app->winwidth = 720;        /* window width x height    */
    app->winheight = 740;

    app->indent = 0;        /* first non-space/tab char in line   */
    app->indentpl = 0;      /* prev line indent */
    app->indentlevel = 0;   /* will normally be in initialize fn  */
    app->overwrite = FALSE; /* ins/overwrite mode flag */

    GtkWidget *sbalign;         /* alignment for statusbar  */
    guint ptop;                 /* padding, top, bot, l, r  */
    guint pbot;
    guint pleft;
    guint pright;

    /* create toplevel window */
    if (!(app->window = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_window() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->window), app->winwidth, app->winheight);
    // gtk_window_set_title (GTK_WINDOW (app->window), "GTKwrite Text Editor");
    gtkwrite_window_set_title (NULL, app); /* uugh - again - fixed */

    /* create & attach accelerator group */
    mainaccel = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (app->window), mainaccel);

    /* create vbox to hold menu, scrolled_window & statusbar
     * and add contaier to main window
     */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->window), vbox);

    /* create menubar and menus to add */
    menubar = create_menubar (app, mainaccel);
    gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

    /* create toolbar
     * GTK_TOOLBAR_ICONS, GTK_TOOLBAR_TEXT, GTK_TOOLBAR_BOTH, GTK_TOOLBAR_BOTH_HORIZ
     */
    create_toolbar (&(app->toolbar), mainaccel, app);
    gtk_box_pack_start(GTK_BOX(vbox), app->toolbar, FALSE, FALSE, 5);
    gtk_widget_show (app->toolbar);

#ifdef HAVESOURCEVIEW
    /* create buffer for text_view, init cursor and iter, line & col */
    app->buffer = gtk_source_buffer_new (NULL);
    app->cursor = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(app->buffer));
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(app->buffer), &iterfirst, app->cursor);
    app->line = gtk_text_iter_get_line (&iterfirst);
    app->col = gtk_text_iter_get_line_offset (&iterfirst);

    /* create text_viewview */
    app->view = gtk_source_view_new_with_buffer (app->buffer);
    gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(app->view), app->lineno);
    gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW(app->view), app->linehghlt);
    gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW(app->view), app->indentauto);
    /* set_smart_backspace available in sourceview 3 */
    // gtk_source_view_set_smart_backspace (GTK_SOURCE_VIEW(app->view), TRUE);
    gtk_source_view_set_smart_home_end (GTK_SOURCE_VIEW(app->view),
                                        GTK_SOURCE_SMART_HOME_END_BEFORE);

    if (app->filename) {
        app->langmgr = gtk_source_language_manager_get_default ();
        app->language = gtk_source_language_manager_guess_language (app->langmgr,
                                                            app->filename, NULL);
        gtk_source_buffer_set_language (app->buffer, app->language);
        gtk_source_buffer_set_highlight_syntax (app->buffer, TRUE);
    }
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (app->view), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (app->view), 5);
#else
    /* create buffer for text_view, init cursor and iter, line & col */
    app->buffer = gtk_text_buffer_new (NULL);
    app->cursor = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(app->buffer));
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(app->buffer), &iterfirst,
                                        app->cursor);
    app->line = gtk_text_iter_get_line (&iterfirst);
    app->col = gtk_text_iter_get_line_offset (&iterfirst);

    /* create text_viewview */
    app->view = gtk_text_view_new_with_buffer (app->buffer);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (app->view), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (app->view), 5);
#endif

    /* Change default font throughout the widget */
    font_desc = pango_font_description_from_string (app->fontname);
    gtk_widget_modify_font (app->view, font_desc);
    /* set tab to lesser of softtab and tabstop if softtab set */
    set_tab_size (font_desc, app, (app->softtab && (app->softtab < app->tabstop) ?
                                    app->softtab : app->tabstop));
    pango_font_description_free (font_desc);

    /* create scrolled_window for view */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), app->view);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 5);

    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);

    /* create/pack statusbar at end within gtk_alignment */
    sbalign = gtk_alignment_new (0, .5, 1, 1);
    gtk_alignment_get_padding (GTK_ALIGNMENT (sbalign), &ptop, &pbot, &pleft, &pright);
    gtk_alignment_set_padding (GTK_ALIGNMENT (sbalign), ptop, pbot + 2, pleft + 5, pright);

    app->statusbar = gtk_statusbar_new ();
    status_set_default (app);

    gtk_container_add (GTK_CONTAINER (sbalign), app->statusbar);
    gtk_box_pack_end (GTK_BOX (vbox), sbalign, FALSE, FALSE, 0);

    /* connect all signals */
    g_signal_connect (G_OBJECT (app->window), "delete-event", /* window del */
                      G_CALLBACK (on_window_delete_event), app);

    g_signal_connect (G_OBJECT (app->window), "destroy",    /* window dest  */
                      G_CALLBACK (on_window_destroy), NULL);

    /* general */
    g_signal_connect (G_OBJECT (app->view), "key_press_event",
                      G_CALLBACK (on_keypress), app);

    g_signal_connect (app->buffer, "mark_set",
                      G_CALLBACK (on_mark_set), app);

    g_signal_connect (app->buffer, "changed",
                      G_CALLBACK (on_buffer_changed), app);

    g_signal_connect (G_OBJECT (app->view), "toggle-overwrite",
                      G_CALLBACK (on_insmode), app);

    /* set window title */
    gtkwrite_window_set_title (NULL, app);

    /* showall widgets */
    gtk_widget_show_all (app->window);

    return app->window;
}

/*
 * window callbacks
 */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 kwinst *app)
{
    /* TODO consolidation with 'quit' - new function ? */
    /* check changed, prompt yes/no */
    if (buffer_chk_save_on_exit (GTK_TEXT_BUFFER(app->buffer))) {
        if (!app->filename) {
            gchar *filename;
            while (!(filename = get_save_filename (app))) {
                if (dlg_yes_no_msg ("Warning: Do you want to cancel save?",
                                    "Warning - Save Canceled", FALSE))
                    return FALSE;
            }
            if (app->trimendws)
                buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
            if (app->posixeof)
                buffer_require_posix_eof (GTK_TEXT_BUFFER(app->buffer));
            buffer_write_file (app, filename);
            g_free (filename);
        }
        else {
            if (app->trimendws)
                buffer_remove_trailing_ws (GTK_TEXT_BUFFER(app->buffer));
            if (app->posixeof)
                buffer_require_posix_eof (GTK_TEXT_BUFFER(app->buffer));
            buffer_write_file (app, app->filename);
        }
    }

    if (widget) {}
    if (event) {}

    return FALSE;
}

void on_window_destroy (GtkWidget *widget, kwinst *app)
{
    // g_print ("on_window_destroy\n");
    gtk_main_quit();
    if (widget) {}
    if (app) {}
}

/* function to set the tab width to sz spaces */
void set_tab_size (PangoFontDescription *font_desc, kwinst *app, gint sz)
{
    PangoTabArray *tab_array;
    PangoLayout *layout;
    // gchar *tab_string;
    gint width, i;

    // tab_string = g_strdup_printf ("%*s", sz, " ");
    if (app->tabstring) g_free (app->tabstring);
    app->tabstring = g_strdup_printf ("%*s", sz, " ");

    // layout = gtk_widget_create_pango_layout (app->view, tab_string);
    layout = gtk_widget_create_pango_layout (app->view, app->tabstring);
    pango_layout_set_font_description (layout, font_desc);
    pango_layout_get_pixel_size (layout, &width, NULL);
    if (width) {
        tab_array = pango_tab_array_new (app->winwidth/width, TRUE);
        for (i = 0; i * width < app->winwidth; i++)
            pango_tab_array_set_tab (tab_array, i, PANGO_TAB_LEFT, i * width);

        gtk_text_view_set_tabs (GTK_TEXT_VIEW(app->view), tab_array);
        pango_tab_array_free (tab_array);
    }

    // g_free (tab_string);
}

void on_insmode (GtkWidget *widget, kwinst *app)
{
    if (app->overwrite) {
        app->overwrite = FALSE;
#ifdef DEBUG
        g_print (" toggle-overwrite : overwrite -> insert mode\n");
#endif
    }
    else {
        app->overwrite = TRUE;
#ifdef DEBUG
        g_print (" toggle-overwrite : insert -> overwrite mode\n");
#endif
    }

    status_set_default (app);   /* update the status bar */

    if (widget) {}
    if (app) {}
}

void on_mark_set (GtkTextBuffer *buffer, GtkTextIter *iter,
                  GtkTextMark *mark, kwinst *app)
{
    gint line, col;
    gchar *status;

    line = gtk_text_iter_get_line (iter);
    col = gtk_text_iter_get_line_offset (iter);

    // if (line == app->line && col == app->col) return;

    app->line = line;
    app->col = col;

    status = g_strdup_printf (" line:%5d :%4d  |  %s",
                              app->line + 1, app->col + 1,
                              app->overwrite ? "OVR" : "INS");
    status_update_str (app, status);

    g_free (status);

    if (buffer) {}
    if (mark) {}
}

void on_buffer_changed (GtkTextBuffer *buffer,
                        kwinst *app)
{
    app->modified = TRUE;
    gtkwrite_window_set_title (NULL, app);

    if (buffer) {}
}

gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, kwinst *app)
{
    if (gtk_text_view_im_context_filter_keypress (GTK_TEXT_VIEW (app->view),
                                                    event)) {
        return TRUE;
    }

    /* limited to key_press_event only */
//   if ((event->type == GDK_KEY_PRESS) &&
//      (event->state & GDK_CONTROL_MASK)) {

    // GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);

    switch (event->keyval)
    {
        case GDK_KEY_BackSpace:
#ifdef DEBUGKP
            g_print ("  GDK_KEY_BackSpace - caught\n");
            g_print ("  app->smartbs: %s\n", app->smartbs ? "TRUE" : "FALSE");
#endif
            if (app->smartbs)   /* smart_backspace in filebuf.c */
                return smart_backspace (app);
            break;              /* or just return FALSE; */
        case GDK_KEY_Tab:;      /* catch tab, replace with softtab spaces */
                return smart_tab (app);
        case GDK_KEY_Return:
#ifndef HAVESOURCEVIEW
                if (app->indentauto)
                    return buffer_indent_auto (app);
#endif
                break;
        case GDK_KEY_KP_Enter:
#ifndef HAVESOURCEVIEW
                if (app->indentauto)
                    return buffer_indent_auto (app);
#endif
                break;
            /* if (app->indent) {  // set indent based on prior line
                GtkTextBuffer *buffer;
                gchar *indentstr;
                buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
                indentstr = g_strdup_printf ("\n%s", app->indent, " ");
                gtk_text_buffer_insert_at_cursor (buffer, indentstr, -1);
                g_free (indentstr);

            } */
//             if (app->indentlevel) { /* set indent to tab following return */
//                 GtkTextBuffer *buffer;
//                 GtkTextIter iter;
//                 buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
/*                gchar *tab_string;
                buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
                tab_string = g_strdup_printf ("\n%*s",
                                                app->softtab * app->indentlevel,
                                                " ");
                gtk_text_buffer_insert_at_cursor (buffer, tab_string, -1);
                g_free (tab_string);
*/
                // temporary newline input while working on indent
//                 gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
//                 /* get end iterator and get chars in line */
//                 gtk_text_buffer_get_end_iter (buffer, &iter);
                // gtk_text_iter_set_line (&iter, -1);
#ifdef DEBUG
//                 g_print (" chars in line: %d\n",
//                             gtk_text_iter_get_chars_in_line (&iter));
#endif
//                 return TRUE;    /* return TRUE - no further processing */
//             }
        // default:
        //     return FALSE;
    }
//   }
    return FALSE;

    if (widget) {}
    if (app) {}
}

/* misc callback functions
 * TODO: move all print to gtk_print.[ch]
 */
void view_print (GtkWidget *widget, kwinst *app)
{
    gchar *str;
    GtkTextBuffer *buffer;

    str = g_strdup_printf ("Current Operation : %s\n",
                           gtk_menu_item_get_label (GTK_MENU_ITEM (widget)));
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    gtk_text_buffer_insert_at_cursor (buffer, str, -1);

    g_free (str);
}

void view_print_fmt (GtkWidget *widget, kwinst *app)
{
    gchar *entry;
    gchar *str;
    GtkTextBuffer *buffer;
    GtkTextIter iter;

    entry = g_strdup_printf ("%s",
                           gtk_menu_item_get_label (GTK_MENU_ITEM (widget)));

    str = g_strdup_printf ("Current Operation : ");
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    /* retrieve & check GtkTextTagTable for existing tag */
    if (!app->tagtable)
        app->tagtable = gtk_text_buffer_get_tag_table (buffer);

    if (!gtk_text_tag_table_lookup (app->tagtable, "fg_green"))
        gtk_text_buffer_create_tag (buffer, "fg_green",
                                    "foreground", "green", NULL);

    gtk_text_buffer_insert_at_cursor (buffer, str, -1);
    gtk_text_buffer_get_end_iter (buffer, &iter);
    gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
            entry, -1, "fg_green",  NULL);
    gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);

    g_free (str);
    g_free (entry);
}

void testcb (GtkWidget *widget, kwinst *app)
{
    err_dialog ("It all went to hell in a handbasket!");
    if (app) {}
    if (widget) {}
}

void propcb (GtkWidget *widget, kwinst *app)
{
    gchar *str;
    GtkTextBuffer *buffer;

    str = g_strdup_printf ("Current Operation : %s\n\n",
                           gtk_menu_item_get_label ((GtkMenuItem *) widget));
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    gtk_text_buffer_insert_at_cursor (buffer, str, -1);

    g_free (str);
    str = NULL;

    str  = g_strdup_printf ("  filename  : %s\n", app->filename);
    gtk_text_buffer_insert_at_cursor (buffer, str, -1);
    g_free (str);
    str = NULL;

    str  = g_strdup_printf ("  file      : %s\n", app->fname);
    gtk_text_buffer_insert_at_cursor (buffer, str, -1);
    g_free (str);
    str = NULL;

    str  = g_strdup_printf ("  extension : %s\n", app->fext);
    gtk_text_buffer_insert_at_cursor (buffer, str, -1);
    g_free (str);
    str = NULL;

    str  = g_strdup_printf ("  path      : %s\n", app->fpath);
    gtk_text_buffer_insert_at_cursor (buffer, str, -1);
    g_free (str);

}
