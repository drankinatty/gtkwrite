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
    GtkWidget *vbox;                /* vbox container   */
    GtkWidget *menubar;             /* menu container   */
    GtkWidget *scrolled_textview;   /* scrolled win w/text_view */

    GtkWidget *sbalign;             /* alignment for statusbar  */
    guint ptop;                     /* padding, top, bot, l, r  */
    guint pbot;
    guint pleft;
    guint pright;

    gchar *iconfile;            /* filename to loading icon */

    /* create toplevel window */
    if (!(app->window = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_window() gtk_window_new failure.");
        return NULL;
    }
    // gtk_window_set_position (GTK_WINDOW (app->window), GTK_WIN_POS_CENTER);
    gtk_window_set_position (GTK_WINDOW (app->window), GTK_WIN_POS_NONE);
    gtk_window_set_default_size (GTK_WINDOW (app->window), app->winwidth, app->winheight);
    // gtk_window_move (GTK_WINDOW (app->window), app->winrootx, app->winrooty);

    /* create icon filename and set icon */
    if ((iconfile = g_strdup_printf ("%s/%s", app->imgdir, ICON))) {
        gtk_window_set_icon(GTK_WINDOW(app->window), create_pixbuf_from_file (iconfile));
        g_free (iconfile);
    }

    /* create & attach accelerator group */
    mainaccel = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (app->window), mainaccel);

    /* create vbox to hold menu, toolbar, scrolled_windows, textview & statusbar
     * and add contaier to main window
     */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->window), vbox);
    // gtk_box_set_spacing (GTK_BOX (vbox), 0);

    /* create menubar and menus to add */
    menubar = create_menubar (app, mainaccel);
    app->menubar = menubar;
    gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);
    gtk_widget_show (app->menubar);

    /* create toolbar
     * GTK_TOOLBAR_ICONS, GTK_TOOLBAR_TEXT, GTK_TOOLBAR_BOTH, GTK_TOOLBAR_BOTH_HORIZ
     */
    app->toolbar = create_toolbar (mainaccel, app);
    gtk_box_pack_start(GTK_BOX(vbox), app->toolbar, FALSE, FALSE, 0);
    gtk_widget_show (app->toolbar);

    /* create vbox to display infobar */
    app->ibarvbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), app->ibarvbox, FALSE, FALSE, 0);
    gtk_widget_show (app->ibarvbox);

    /* create scrolled_window and textview */
    scrolled_textview = create_textview_scrolledwindow (app);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_textview, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_textview);

    /* create/pack statusbar at end within gtk_alignment */
    sbalign = gtk_alignment_new (0, .5, 1, 1);
    gtk_alignment_get_padding (GTK_ALIGNMENT (sbalign), &ptop, &pbot, &pleft, &pright);
    gtk_alignment_set_padding (GTK_ALIGNMENT (sbalign), ptop, pbot + 2, pleft + 5, pright);

    app->statusbar = gtk_statusbar_new ();
    status_set_default (app);

    gtk_container_add (GTK_CONTAINER (sbalign), app->statusbar);
    gtk_box_pack_end (GTK_BOX (vbox), sbalign, FALSE, FALSE, 0);

    gtk_widget_show (app->statusbar);
    gtk_widget_show (vbox);

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
    gtk_widget_show (app->window);

    /* showall widgets */
    gtk_widget_show_all (app->window);

    /* TODO: load saved settings */
    gtk_widget_set_visible (app->toolbar, app->showtoolbar);
    gtk_widget_set_sensitive (app->tbappearMi, app->showtoolbar);


    return app->window;
}

/*
 * window callbacks
 */
/** on_window_delete_event called first when WM_CLOSE button clicked.
 *  if return is FALSE, on_window_destroy is called, if TRUE, control
 *  is returned to gtk_main and you are responsible for gtk_main_quit.
 */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 kwinst *app)
{
    /* get window size */
    gtk_window_get_size (GTK_WINDOW (app->window), &(app->winwidth),
                        &(app->winheight));

    /* TODO add gboolean app->on_window_delete_event, and return it */
    /* check changed, prompt yes/no */
    ibar_handle_quit (app);
    return TRUE;
    // buffer_handle_quit (app);
    // return FALSE;
    /* TODO - return app->on_window_delete_event; */

    if (widget) {}
    if (event) {}
}

/** on_window_destroy called after on_window_delete_event processed
 *  if the return from on_window_delete_event is FALSE.
 */
void on_window_destroy (GtkWidget *widget, kwinst *app)
{
    // g_print ("on_window_destroy\n");
    gtk_main_quit();
    if (widget) {}
    if (app) {}
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

/** on cursor position change (insert mark_set), update line, lines, col. */
void on_mark_set (GtkTextBuffer *buffer, GtkTextIter *iter,
                  GtkTextMark *mark, kwinst *app)
{
    /* update current line, total lines and current column */
    app->line = gtk_text_iter_get_line (iter);
    app->lines = gtk_text_buffer_get_line_count (buffer);
    app->col = gtk_text_iter_get_line_offset (iter);

    /* update status bar */
    status_set_default (app);

    if (mark) {}
}

void on_buffer_changed (GtkTextBuffer *buffer,
                        kwinst *app)
{
    if (!app->modified) /* app->modified set in set_title */
        gtkwrite_window_set_title (NULL, app);

    /* TODO fix with central gtk_text_buffer_get_modified()
     * but you want to make sure set_modified is FALSE after
     * all saves, open w/o edits, etc..
     */
    /* set app->eolchg if buffer changed so conversion runs on eol change */
    if (app->modified && app->eol != app->oeol)
        app->eolchg = TRUE;

    if (buffer) {}
}

gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, kwinst *app)
{
    if (gtk_text_view_im_context_filter_keypress (GTK_TEXT_VIEW (app->view), event)) {
        return TRUE;
    }

    /* handle control + shift key combinations */
    if (event->type == GDK_KEY_PRESS &&
        event->state & GDK_CONTROL_MASK &&
        event->state & GDK_SHIFT_MASK) {
        switch (event->keyval) {
            case GDK_KEY_Left:
                if (app->ctrl_shift_right_fix) {
                    gboolean rtn = buffer_select_to_prev_char (app);
                    /* if no prev ctrl+shift keypress or last was to left
                     * push another left keypress on stack.
                     */
                    if (!app->bindex || bstack_last (app) == LEFT)
                        bstack_push (app, LEFT);    /* record LEFT selection */
                    else
                        bstack_pop (app);           /* pop left from stack */
                    return rtn;
                }
                break;
            case GDK_KEY_Right:
                if (app->ctrl_shift_right_fix) {
                    gboolean rtn = buffer_select_to_next_char (app);
                    /* if no prev ctrl+shift keypress or last was to right
                     * push another right keypress on stack.
                     */
                    if (!app->bindex || bstack_last (app) == RIGHT)
                        bstack_push (app, RIGHT);   /* record RIGHT selection */
                    else
                        bstack_pop (app);           /* pop right from stack   */
                    return rtn;
                }
                break;
        }
        return FALSE;   /* return - only process ctrl + shift events */
    }

    /* handle control key combinations */
    /*
    if (event->type == GDK_KEY_PRESS &&
        event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            case GDK_KEY_Right:
                g_print ("key pressed: %s\n", "ctrl + Right->Arrow");
                break;
        }
        return FALSE;
    }
    */

    /* handle shift key combinations */
    /*
    if (event->type == GDK_KEY_PRESS &&
        event->state & GDK_SHIFT_MASK) {
        switch (event->keyval) {
            case GDK_KEY_Right:
                g_print ("key pressed: %s\n", "shift + Right->Arrow");
                break;
        }
        return FALSE;
    }
    */

    /* handle normal keypress events */
    if (event->type == GDK_KEY_PRESS) {
        switch (event->keyval) {
            case GDK_KEY_BackSpace:
                if (app->smartbs)       /* smart_backspace in filebuf.c */
                    return smart_backspace (app);
                break;                  /* or just return FALSE; */
            case GDK_KEY_Tab:;          /* catch tab, replace with softtab spaces */
                    return smart_tab (app);
            case GDK_KEY_Return:
// #ifndef HAVESOURCEVIEW   /* using my auto-indent and EOL handling for SV & non-SV */
                if (app->indentauto)
                    return buffer_indent_auto (app);
                else
                    return buffer_insert_eol (app);
// #endif
                break;
            case GDK_KEY_KP_Enter:
// #ifndef HAVESOURCEVIEW
                if (app->indentauto)
                    return buffer_indent_auto (app);
                else
                    return buffer_insert_eol (app);
// #endif
                break;
            case GDK_KEY_Home:
#ifndef HAVESOURCEVIEW
                if (app->smarthe)
                    return ((app->kphome = smart_home (app)));
#endif
                break;
        }
    }

    app->kphome = FALSE;    /* reset kphome - return above protects needed TRUE */

    return FALSE;

    if (widget) {}
    if (app) {}
}
