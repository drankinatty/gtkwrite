#include "gtk_windef.h"
#include "gtk_findreplace.h"

/* create application window & initialize values
 * and connect callback functions. 'app' contains
 * widgets for window, text_view and statusbar.
 */
GtkWidget *create_window (context *app)
{
    GtkAccelGroup *mainaccel;

    GtkWidget *vbox;            /* vbox container   */

    GtkWidget *menubar;         /* menu container   */
    GtkWidget *fileMenu;        /* file menu        */
    GtkWidget *fileMi;
    GtkWidget *sep;
    GtkWidget *newMi;
    GtkWidget *openMi;
    GtkWidget *reloadMi;
    GtkWidget *saveMi;
    GtkWidget *saveasMi;
    GtkWidget *pagesuMi;
    GtkWidget *printMi;
    GtkWidget *pprevMi;
    GtkWidget *closeMi;
    GtkWidget *quitMi;
    GtkWidget *editMenu;        /* edit menu        */
    GtkWidget *editMi;
    GtkWidget *undoMi;
    GtkWidget *redoMi;
    GtkWidget *copyMi;
    GtkWidget *cutMi;
    GtkWidget *pasteMi;
    GtkWidget *deleteMi;
    GtkWidget *findMi;
    GtkWidget *replaceMi;
    GtkWidget *gotoMi;
    GtkWidget *prefsMi;
    GtkWidget *viewMenu;        /* view menu      */
    GtkWidget *viewMi;
    GtkWidget *fontMi;
    GtkWidget *statusMenu;      /* status menu      */
    GtkWidget *statusMi;
    GtkWidget *clearMi;
    GtkWidget *propsMi;
    GtkWidget *helpMenu;        /* help menu        */
    GtkWidget *helpMi;
    GtkWidget *aboutMi;

    GtkWidget *scrolled_window; /* container for text_view */

    // GtkTextBuffer *buffer;      /* text_view values - test write */
    // GtkTextIter iter;
    // GtkTextIter start, end;
    PangoFontDescription *font_desc;
    GtkTextIter iterfirst;
    app->fontname = g_strdup ("DejaVu Sans Mono 8");;
    app->line = 0;          /* initialize beginning pos line/col  */
    app->col = 0;

    app->winwidth = 720;        /* window width x height    */
    app->winheight = 740;

    app->tabspaces = 4;     /* initialize tabspaces & indentlevel */
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
    menubar = gtk_menu_bar_new ();
    fileMenu = gtk_menu_new ();
    editMenu = gtk_menu_new ();
    viewMenu = gtk_menu_new ();
    statusMenu = gtk_menu_new ();
    helpMenu = gtk_menu_new ();

    /* define file menu */
    fileMi = gtk_menu_item_new_with_mnemonic ("_File");
    sep = gtk_separator_menu_item_new ();
    newMi    = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW,
                                                   NULL);
    openMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN,
                                                   NULL);
    saveMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE,
                                                   NULL);
    saveasMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE_AS,
                                                   NULL);
    reloadMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_REFRESH,
                                                   NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (reloadMi), "R_eload Saved File");
    pagesuMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_PAGE_SETUP,
                                                   NULL);
    pprevMi  = gtk_image_menu_item_new_from_stock (GTK_STOCK_PRINT_PREVIEW,
                                                   NULL);
    printMi  = gtk_image_menu_item_new_from_stock (GTK_STOCK_PRINT,
                                                   NULL);
    closeMi  = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLOSE,
                                                   NULL);
    quitMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,
                                                   NULL);

    /* create entries under 'File' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (fileMi), fileMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), newMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), openMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), saveMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), saveasMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), reloadMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), pagesuMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), pprevMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), printMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), closeMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (fileMenu), quitMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), fileMi);

    gtk_widget_add_accelerator (newMi, "activate", mainaccel,
                                GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (openMi, "activate", mainaccel,
                                GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (reloadMi, "activate", mainaccel,
                                GDK_KEY_e, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveMi, "activate", mainaccel,
                                GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveasMi, "activate", mainaccel,
                                GDK_KEY_a, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (pagesuMi, "activate", mainaccel,
                                GDK_KEY_u, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (pprevMi, "activate", mainaccel,
                                GDK_KEY_p, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (printMi, "activate", mainaccel,
                                GDK_KEY_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (closeMi, "activate", mainaccel,
                                GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (quitMi, "activate", mainaccel,
                                GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    /* define edit menu */
    editMi = gtk_menu_item_new_with_mnemonic ("_Edit");
    sep = gtk_separator_menu_item_new ();
    undoMi    = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNDO,
                                                    NULL);
    redoMi    = gtk_image_menu_item_new_from_stock (GTK_STOCK_REDO,
                                                    NULL);
    copyMi    = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY,
                                                    NULL);
    cutMi     = gtk_image_menu_item_new_from_stock (GTK_STOCK_CUT,
                                                    NULL);
    pasteMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_PASTE,
                                                    NULL);
    deleteMi  = gtk_image_menu_item_new_from_stock (GTK_STOCK_DELETE,
                                                    NULL);
    findMi    = gtk_image_menu_item_new_from_stock (GTK_STOCK_FIND,
                                                    NULL);
    replaceMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_FIND_AND_REPLACE,
                                                    NULL);
    gotoMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_INDEX,
                                                    NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (gotoMi), "_Go to Line");

    prefsMi   = gtk_image_menu_item_new_from_stock (GTK_STOCK_PREFERENCES,
                                                    NULL);

    /* create entries under 'Edit' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (editMi), editMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), undoMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), redoMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), copyMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), cutMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), pasteMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), deleteMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), findMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), replaceMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), gotoMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (editMenu), prefsMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), editMi);

    gtk_widget_add_accelerator (undoMi, "activate", mainaccel,
                                GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (redoMi, "activate", mainaccel,
                                GDK_KEY_z, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (copyMi, "activate", mainaccel,
                                GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (cutMi, "activate", mainaccel,
                                GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (pasteMi, "activate", mainaccel,
                                GDK_KEY_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (findMi, "activate", mainaccel,
                                GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (replaceMi, "activate", mainaccel,
                                GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (gotoMi, "activate", mainaccel,
                                GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (prefsMi, "activate", mainaccel,
                                GDK_KEY_p, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    /* define view menu */
    viewMi = gtk_menu_item_new_with_mnemonic ("_View");
    sep = gtk_separator_menu_item_new ();
    fontMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_SELECT_FONT,
                                                  NULL);
    /* create entries under 'Status' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (viewMi), viewMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), fontMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), viewMi);

    gtk_widget_add_accelerator (viewMi, "activate", mainaccel,
                                GDK_KEY_v, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (fontMi, "activate", mainaccel,
                                GDK_KEY_t, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    /* define status menu */
    statusMi = gtk_menu_item_new_with_mnemonic ("_Status");
    sep = gtk_separator_menu_item_new ();
    clearMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLEAR,
                                                  NULL);
    propsMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES,
                                                  NULL);
    /* create entries under 'Status' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (statusMi), statusMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), clearMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), propsMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), statusMi);

    gtk_widget_add_accelerator (clearMi, "activate", mainaccel,
                                GDK_KEY_c, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (propsMi, "activate", mainaccel,
                                GDK_KEY_r, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    /*define help menu */
    helpMi = gtk_menu_item_new_with_mnemonic ("_Help");
    gtk_menu_item_set_right_justified ((GtkMenuItem *)helpMi, TRUE);
    sep = gtk_separator_menu_item_new ();
    aboutMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,
                                                  NULL);

    /* create entries under 'Help' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (helpMi), helpMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (helpMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (helpMenu), aboutMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), helpMi);

    gtk_widget_add_accelerator (aboutMi, "activate", mainaccel,
                                GDK_KEY_a, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

    /* create buffer for text_view, init cursor and iter, line & col */
    app->buffer = gtk_text_buffer_new (NULL);
    app->cursor = gtk_text_buffer_get_insert (app->buffer);
    gtk_text_buffer_get_iter_at_mark (app->buffer, &iterfirst, app->cursor);
    app->line = gtk_text_iter_get_line (&iterfirst);
    app->col = gtk_text_iter_get_line_offset (&iterfirst);

    /* create text_viewview */
    app->view = gtk_text_view_new_with_buffer (app->buffer);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (app->view), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (app->view), 5);

    /* Change default font throughout the widget */
    font_desc = pango_font_description_from_string ("DejaVu Sans Mono 8");
    gtk_widget_modify_font (app->view, font_desc);
    set_tab_size (font_desc, app, 4);
    pango_font_description_free (font_desc);

    /* get buffer for view - quick test write to buffer */
    // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    // gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    // gtk_text_buffer_insert (buffer, &iter, "\nGTK text_view:\n\n", -1);

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
    /* File Menu */
    g_signal_connect (G_OBJECT (newMi), "activate",         /* file New     */
                      G_CALLBACK (menu_file_new_activate), app);

    g_signal_connect (G_OBJECT (openMi), "activate",        /* file Open    */
                      G_CALLBACK (menu_file_open_activate), app);

    g_signal_connect (G_OBJECT (reloadMi), "activate",         /* file Run     */
                      G_CALLBACK (menu_file_reload_activate), app);

    g_signal_connect (G_OBJECT (saveMi), "activate",        /* file Save    */
                      G_CALLBACK (menu_file_save_activate), app);

    g_signal_connect (G_OBJECT (saveasMi), "activate",      /* file SaveAs  */
                      G_CALLBACK (menu_file_saveas_activate), app);

    g_signal_connect (G_OBJECT (pagesuMi), "activate",        /* file Quit    */
                      G_CALLBACK (menu_file_pagesu_activate), app);

    g_signal_connect (G_OBJECT (pprevMi), "activate",        /* file Quit    */
                      G_CALLBACK (menu_file_pprev_activate), app);

    g_signal_connect (G_OBJECT (printMi), "activate",        /* file Quit    */
                      G_CALLBACK (menu_file_print_activate), app);

    g_signal_connect (G_OBJECT (closeMi), "activate",        /* file Quit    */
                      G_CALLBACK (menu_file_close_activate), app);

    g_signal_connect (G_OBJECT (quitMi), "activate",        /* file Quit    */
                      G_CALLBACK (menu_file_quit_activate), app);
    /* Edit Menu */
    g_signal_connect (G_OBJECT (undoMi), "activate",         /* edit Copy   */
                      G_CALLBACK (menu_edit_undo_activate), app);

    g_signal_connect (G_OBJECT (redoMi), "activate",         /* edit Cut     */
                      G_CALLBACK (menu_edit_redo_activate), app);

    g_signal_connect (G_OBJECT (copyMi), "activate",         /* edit Copy   */
                      G_CALLBACK (menu_edit_copy_activate), app);

    g_signal_connect (G_OBJECT (cutMi), "activate",         /* edit Cut     */
                      G_CALLBACK (menu_edit_cut_activate), app);

    g_signal_connect (G_OBJECT (pasteMi), "activate",       /* edit Paste   */
                      G_CALLBACK (menu_edit_paste_activate), app);

    g_signal_connect (G_OBJECT (deleteMi), "activate",      /* edit Delete  */
                      G_CALLBACK (menu_edit_delete_activate), app);

    g_signal_connect (G_OBJECT (findMi), "activate",        /* edit Find    */
                      G_CALLBACK (menu_edit_find_activate), app);

    g_signal_connect (G_OBJECT (replaceMi), "activate",     /* edit Replace */
                      G_CALLBACK (menu_edit_replace_activate), app);

    g_signal_connect (G_OBJECT (gotoMi), "activate",        /* edit Goto */
                      G_CALLBACK (menu_edit_goto_activate), app);

    g_signal_connect (G_OBJECT (prefsMi), "activate",       /* stat Prefs   */
                      G_CALLBACK (menu_edit_preferences_activate), app);
    /* View Menu */
    g_signal_connect (G_OBJECT (fontMi), "activate",        /* font select  */
                      G_CALLBACK (menu_font_select_activate), app);

    /* Status Menu */
    g_signal_connect (G_OBJECT (clearMi), "activate",       /* stat Clear   */
                      G_CALLBACK (menu_status_clear_activate), app);

    g_signal_connect (G_OBJECT (propsMi), "activate",       /* stat Props   */
                      G_CALLBACK (menu_status_properties_activate), app);
    /* Help Menu */
    g_signal_connect (G_OBJECT (aboutMi), "activate",       /* help About   */
                      G_CALLBACK (menu_help_about_activate), app);
    /* general */
    g_signal_connect (G_OBJECT (app->view), "key_press_event",
                      G_CALLBACK (on_keypress), app);
    g_signal_connect (app->buffer, "mark_set",
                      G_CALLBACK (on_mark_set), app);
    g_signal_connect (G_OBJECT (app->view), "toggle-overwrite",
                      G_CALLBACK (on_insmode), app);

    /* showall widgets */
    gtk_widget_show_all (app->window);

    return app->window;
}

/*
 * window callbacks
 */
gboolean on_window_delete_event (GtkWidget *widget, GdkEvent *event,
                                 context *app)
{
    /* TODO consolidation with 'quit' - new function ? */
    if (buffer_prompt_on_mod (app)) {   /* save on exit?    */
        if (app->filename) {        /* use current filename */
            buffer_write_file (app, NULL);  /* uugh - again */
        }
        else {                      /* prompt for filename  */
            gchar *filename;
            if ((filename = get_save_filename (app))) {
                buffer_write_file (app, filename);
            }
        }
    }
    if (widget) {}
    if (event) {}
    // g_print ("on_window_delete_event\n");
    // return buffer_prompt_on_mod (app);
    return FALSE;
}

void on_window_destroy (GtkWidget *widget, context *app)
{
    // g_print ("on_window_destroy\n");
    gtk_main_quit();
    if (widget) {}
    if (app) {}
}

// void gtkwrite_window_set_title (GtkWidget *widget, context *app)
// {
//     /* TODO: create common set title function for all dialogs */
//     /* (e.g. if (widget == app->window), then window title, else dialog */
//     gchar *title = NULL;
//     if (app->modified) {
//         if (app->fname)
//             title = g_strdup_printf ("%s - %s*", app->appshort, app->fname);
//         else
//             title = g_strdup_printf ("%s - untitled*", app->appshort);
//     }
//     else {
//         if (app->fname)
//             title = g_strdup_printf ("%s - %s", app->appshort, app->fname);
//         else
//             title = g_strdup_printf ("%s - untitled", app->appshort);
//
//     }
//
//     gtk_window_set_title (GTK_WINDOW (app->window), title);
//     g_free (title);
//
//     if (widget) {}
// }

/*
 * menu callback functions
 *
 *  _File menu
 */
void menu_file_new_activate (GtkMenuItem *menuitem, context *app)
{
    // GtkTextBuffer *buffer; /* using app->buffer throughout */

    /* if buffer changed, prompt for save */
    if (buffer_prompt_on_mod (app) == TRUE)
        menu_file_save_activate (NULL, app);

    /* free existing filename, set NULL */
    if (app->filename)
        app_free_filename (app);
//         g_free (app->filename);
//     app->filename = NULL;

    /* clear exising buffer, set modified to FALSE */
    gtk_text_buffer_set_text (app->buffer, "", -1);
    gtk_text_buffer_set_modified (app->buffer, FALSE);
    gtkwrite_window_set_title (NULL, app);

    /* reset values to default */
    status_set_default (app);
    // status_update_str (app, "New File");

    if (menuitem) {}
}

void menu_file_open_activate (GtkMenuItem *menuitem, context *app)
{
    /* TODO - clear buffer before open, currently reads file into
     * buffer at cursor. Create menu_file_insert_activate.
     */
    buffer_file_open_dlg (app, NULL);
    if (menuitem) {}
}

void menu_file_reload_activate (GtkMenuItem *menuitem, context *app)
{
    if (!app->filename) {
        /* TODO: error dialog */
        return;
    }

    /* clear exising buffer, set modified to FALSE */
    gtk_text_buffer_set_text (app->buffer, "", -1);
    gtk_text_buffer_set_modified (app->buffer, FALSE);

    /* insert saved file into buffer */
    buffer_insert_file (app, NULL);
    status_menuitem_label (menuitem, app);
}

void menu_file_save_activate (GtkMenuItem *menuitem, context *app)
{
    // gchar *filename;

    if (app->filename == NULL)
    {
        app->filename = get_save_filename (app);
        if (app->filename != NULL) buffer_write_file (app, NULL);
    }
    else buffer_write_file (app, NULL);
    // status_update_str (app, "File : Save");
    if (menuitem) {}
    // if (app) {}
}

void menu_file_saveas_activate (GtkMenuItem *menuitem, context *app)
{
    // gchar *filename;

    app->filename = get_save_filename (app);
    if (app->filename != NULL) buffer_write_file (app, NULL);

    if (menuitem) {}
    if (app) {}
}

void menu_file_pagesu_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_file_pprev_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_file_print_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_file_close_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_file_quit_activate (GtkMenuItem *menuitem, context *app)
{
    /* TODO - consolidate with on-delete-event */
    if (buffer_prompt_on_mod (app)) {   /* save on exit?    */
        if (app->filename) {        /* use current filename */
            buffer_write_file (app, NULL); /* uugh */
        }
        else {                      /* prompt for filename  */
            // gchar *filename;
            if ((app->filename = get_save_filename (app))) {
                buffer_write_file (app, NULL); /* uugh */
            }
        }
    }

    gtk_main_quit ();
    if (menuitem) {}
}

/*
 *  _Edit menu
 */
void menu_edit_undo_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_edit_redo_activate (GtkMenuItem *menuitem, context *app)
{
    if (menuitem) {}
    if (app) {}
}

void menu_edit_copy_activate (GtkMenuItem *menuitem, context *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_copy_clipboard (buffer, clipboard);

    if (menuitem) {}
    // if (app) {}
}

void menu_edit_cut_activate (GtkMenuItem *menuitem, context *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);

    if (menuitem) {}
    // if (app) {}
}

void menu_edit_paste_activate (GtkMenuItem *menuitem, context *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);

    if (menuitem) {}
    // if (app) {}
}

void menu_edit_delete_activate (GtkMenuItem *menuitem, context *app)
{
    GtkTextBuffer *buffer;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);

    if (menuitem) {}
    // if (app) {}
}

void menu_edit_find_activate (GtkMenuItem *menuitem, context *app)
{
    create_find_dlg (app);
    if (menuitem) {}
    if (app) {}
}

/* error: can't pass both 'context' and 'cntxfindrep' structs in callback,
 * must combine data structs into a single struct
 */
void menu_edit_replace_activate (GtkMenuItem *menuitem, context *app)
{
    create_replace_dlg (app);
    if (menuitem) {}
    if (app) {}
}

void menu_edit_goto_activate (GtkMenuItem *menuitem, context *app)
{
    g_print ("menu_edit_goto_activate callback\n");
    if (menuitem) {}
    if (app) {}
}

void menu_edit_preferences_activate (GtkMenuItem *menuitem, context *app)
{
    status_update_str (app, "Edit : Preferences");
    if (menuitem) {}
    if (app) {}
}

/*
 *  _View menu
 */
void menu_font_select_activate (GtkMenuItem *menuitem, context *app)
{
    font_select_dialog (GTK_WIDGET (menuitem), app);

    if (menuitem) {}
    if (app) {}
}

/*
 *  _Status menu
 */
void menu_status_clear_activate (GtkMenuItem *menuitem, context *app)
{
    status_pop (GTK_WIDGET (menuitem), app);
}

void menu_status_properties_activate (GtkMenuItem *menuitem, context *app)
{
    propcb (GTK_WIDGET (menuitem), app);
    if (menuitem) {}
    if (app) {}
}

/*
 *  _Help menu
 */
void menu_help_about_activate (GtkMenuItem *menuitem, context *app)
{
    help_about (app);
    if (menuitem) {}
}

/*
 * general window functions
 */
void context_init (context *app)
{
    app->window     = NULL; /* initialize struct values */
    app->view       = NULL; /* text_view widget for app */
    app->statusbar  = NULL; /* statusbar widget */
    app->cid        = 0;    /* context id for statusbar */
    app->tagtable   = NULL; /* tagtable for text_view */

    app->filename   = NULL; /* full filename */
    app->fname      = NULL; /* base filename w/o ext */
    app->fext       = NULL; /* filename extension */
    app->fpath      = NULL; /* path components of fname */
    app->open       = 0;    /* flag - is file open */
    app->modified   = 0;    /* need save if not 0  */
    app->fsize      = 0;    /* resulting file size */

    app->fontname   = NULL; /* initial font name */

    app->appname    = g_strdup ("GTKwrite Text Editor");
    app->appshort   = g_strdup ("GTKwrite");

    findrep_init (app);
}

void context_destroy (context *app)
{   /* free allocated struct values */
    app_free_filename (app);
    if (app->fontname) g_free (app->fontname);

    if (app->appname) g_free (app->appname);
    if (app->appshort) g_free (app->appshort);

    findrep_destroy (app);
}

// void app_free_filename (context *app)
// {
//     if (app->filename) g_free (app->filename);
//     if (app->fname) g_free (app->fname);
//     if (app->fext) g_free (app->fext);
//     if (app->fpath) g_free (app->fpath);
//     app->filename = NULL;
//     app->fname = NULL;
//     app->fext = NULL;
//     app->fpath = NULL;
// }

void help_about (context *app)
{
    static const gchar *const authors[] = {
            "David C. Rankin, J.D,P.E. <drankinatty@gmail.com>",
            NULL
    };

    static const gchar copyright[] = \
            "Copyright \xc2\xa9 2017 David C. Rankin";

    static const gchar comments[] = "GTKwrite Text Editor";

    gtk_show_about_dialog (GTK_WINDOW (app->window),
                           "authors", authors,
                           "comments", comments,
                           "copyright", copyright,
                           "version", "0.2",
                           "website", "http://www.rankinlawfirm.com",
                           "program-name", "GTKwrite Text Editor",
                           "logo-icon-name", GTK_STOCK_EDIT,
                           NULL);
}

// void split_fname (context *app)
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

/* function to set the tab width to sz spaces */
void set_tab_size (PangoFontDescription *font_desc, context *app, gint sz)
{
    PangoTabArray *tab_array;
    PangoLayout *layout;
    gchar *tab_string;
    gint width, i;

    tab_string = g_strdup_printf ("%*s", sz, " ");

    layout = gtk_widget_create_pango_layout (app->view, tab_string);
    pango_layout_set_font_description (layout, font_desc);
    pango_layout_get_pixel_size (layout, &width, NULL);
    if (width) {
        tab_array = pango_tab_array_new (app->winwidth/width, TRUE);
        for (i = 0; i * width < app->winwidth; i++)
            pango_tab_array_set_tab (tab_array, i, PANGO_TAB_LEFT, i * width);

        gtk_text_view_set_tabs (GTK_TEXT_VIEW(app->view), tab_array);
        pango_tab_array_free (tab_array);
    }

    g_free (tab_string);
}

// gboolean buffer_chk_mod (context *app)
// {
//     GtkTextBuffer *buffer;
//
//     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//
//     return (app->modified = gtk_text_buffer_get_modified (buffer));
// }

// /* TODO: Check on exit if user says "yes" save then cancels the
//  *   filesave dialog (current GLib-CRITICAL **: g_file_set_contents:
//  *   assertion 'filename != NULL' failed (segfault)
//  */
// gboolean buffer_prompt_on_mod (context *app)
// {
//     gboolean ret = FALSE;
//     GtkTextBuffer *buffer;
//
//     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//
//     if (gtk_text_buffer_get_modified (buffer) == TRUE)
//     {
//         GtkWidget *dialog;
//
//         const gchar *msg  = "Do you want to save the changes you have made?";
//
//         dialog = gtk_message_dialog_new (NULL,
//                                          GTK_DIALOG_MODAL |
//                                          GTK_DIALOG_DESTROY_WITH_PARENT,
//                                          GTK_MESSAGE_QUESTION,
//                                          GTK_BUTTONS_YES_NO,
//                                          msg);
//
//         gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
//         if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
//         {
//             ret = FALSE;    /* don't save */
//         }
//         else ret = TRUE;    /* save */
//
//         gtk_widget_destroy (dialog);
//     }
//
//     return ret;
// }

// void status_update_str (context *app, gchar *s)
// {
//     if (app->cid)               /* pop previous statusbar entry */
//         gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
//
//     gchar *str;
//     if (s && *s)
//         str = g_strdup (s);
//         // str = g_strdup_printf ("Operation : %s", s);
//     else
//         str = g_strdup_printf ("Error : statusbar update failed 'NULL'");
//
//     app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar), str);
//     gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, str);
//
//     g_free (str);
// }
//
// void status_menuitem_label (GtkMenuItem *menuitem, context *app)
// {
//     if (app->cid)               /* pop previous statusbar entry */
//         gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
//
//     gchar *str;                 /* create string from menu item */
//     str = g_strdup_printf ("menuitem : %s",
//                            gtk_menu_item_get_label (menuitem));
// //                            gtk_menu_item_get_label (GTK_MENU_ITEM (widget)));
//     app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar), str);
//
//     gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, str);
//
//     g_free (str);
// }
//
// void status_pop (GtkWidget *widget, context *app)
// {
//     if (app->cid)
//         gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
//     if (widget) {}
// }
//
// void status_set_default (context *app)
// {
//     gchar *file;
//     gchar *status;
//
//     if (app->modified) {
//         if (app->filename)
//             file = g_strdup_printf ("*%s", g_path_get_basename (app->filename));
//         else
//             file = g_strdup ("*(Untitled)");
//     }
//     else {
//         if (app->filename)
//             file = g_path_get_basename (app->filename);
//         else
//             file = g_strdup ("(Untitled)");
//     }
//
//     // status = g_strdup_printf ("File : %s", file);
//     status = g_strdup_printf (" line:%5d :%4d  |  %s",
//                               app->line + 1, app->col + 1,
//                               app->overwrite ? "OVR" : "INS");
//
//     if (app->cid)               /* pop previous statusbar entry */
//         gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
//     else
//         app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar),
//                                                  status);
//     gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, status);
//     g_free (status);
//     g_free (file);
// }

void on_insmode (GtkWidget *widget, context *app)
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
                  GtkTextMark *mark, context *app)
{
    gint line, col;
    gchar *status;

    line = gtk_text_iter_get_line (iter);
    col = gtk_text_iter_get_line_offset (iter);

    if (line == app->line && col == app->col) return;

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

void on_indent_inc (GtkWidget *widget, context *app)
{
//   fix indent (reset when current col < indent col) or something like that.
//   use
//     gtk_text_buffer_get_iter_at_line () /* get iter at start of line x */
//     (app->line)
//     gtk_text_buffer_get_iter_at_mark () /* get iter a current possiiton */
//     (should correspond to app->col)
//     get text in between iters and check if whitespace
//     if whitespace, then gtk_text_buffer_create_tag () /* create indent tag */
//         (use indent property and calculate based on indentlevel)
//     gtk_text_buffer_apply_tag ()

    GtkTextIter istr, imrk;
    GtkTextBuffer *buffer;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view)); /* get buffer */
    gtk_text_buffer_get_iter_at_line (buffer, &istr, app->line);   /* get start */
    gtk_text_buffer_get_iter_at_mark (buffer, &imrk, app->cursor); /* get mark */
    /* now get text of buffer between iters istr, imrk and test */

    if (widget) {}
    if (app) {}
}

gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, context *app)
{
    if (gtk_text_view_im_context_filter_keypress (GTK_TEXT_VIEW (app->view),
                                                    event)) {
        return TRUE;
    }

    /* limited to key_press_event only */
//   if ((event->type == GDK_KEY_PRESS) &&
//      (event->state & GDK_CONTROL_MASK)) {

    switch (event->keyval)
    {
        case GDK_KEY_BackSpace:
            // g_print ("  GDK_KEY_BackSpace - caught\n");
            /* if in leading whitespace and app->indentlevel > 0
             * then app->indentlevel--;
             */
            break;
        case GDK_KEY_Tab:;   /* catch tab, replace with 4 spaces */
            GtkTextBuffer *buffer;
            gchar *tab_string;
            buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
            /* get line and check if in leading whitespace before
             * incrementing indent level
             * must condition adjust of indent level on change in indent level, if pressed in a column
             * before current indentlevel*tabspaces, do not indent.
             */
            // app->indentlevel++; /* TODO: set backspace at beginning to reduce */
//             tab_string = g_strdup_printf ("%*s", app->tabspaces * app->indentlevel,
//                                           " ");
            tab_string = g_strdup_printf ("%*s", app->tabspaces,
                                          " ");
            gtk_text_buffer_insert_at_cursor (buffer, tab_string, -1);
            g_free (tab_string);
            /* TODO: if (at beginning)
             * You MUST also use your PangoTabArray not just spaces
             */
            return TRUE;    /* return TRUE - no further processing */
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
            /* if (app->indent) {  // set indent based on prior line
                GtkTextBuffer *buffer;
                gchar *indentstr;
                buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
                indentstr = g_strdup_printf ("\n%s", app->indent, " ");
                gtk_text_buffer_insert_at_cursor (buffer, indentstr, -1);
                g_free (indentstr);

            } */
            if (app->indentlevel) { /* set indent to tab following return */
                GtkTextBuffer *buffer;
                GtkTextIter iter;
                buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
/*                gchar *tab_string;
                buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
                tab_string = g_strdup_printf ("\n%*s",
                                                app->tabspaces * app->indentlevel,
                                                " ");
                gtk_text_buffer_insert_at_cursor (buffer, tab_string, -1);
                g_free (tab_string);
*/
                // temporary newline input while working on indent
                gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
                /* get end iterator and get chars in line */
                gtk_text_buffer_get_end_iter (buffer, &iter);
                // gtk_text_iter_set_line (&iter, -1);
#ifdef DEBUG
                g_print (" chars in line: %d\n",
                            gtk_text_iter_get_chars_in_line (&iter));
#endif
                return TRUE;    /* return TRUE - no further processing */
            }
        // default:
        //     return FALSE;
    }
//   }
    return FALSE;

    if (widget) {}
    if (app) {}
}

// gchar *get_open_filename (context *app)
// {
//     GtkWidget *chooser;
//     gchar *filename=NULL;
//
//     chooser = gtk_file_chooser_dialog_new ("Open File...",
//                                             GTK_WINDOW (app->window),
//                                             GTK_FILE_CHOOSER_ACTION_OPEN,
//                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//                                             GTK_STOCK_OPEN, GTK_RESPONSE_OK,
//                                             NULL);
//
//     if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
//     {
//         filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
//     }
//     gtk_widget_destroy (chooser);
//
//     return filename;
// }

// gchar *get_save_filename (context *app)
// {
//     GtkWidget *chooser;
//     gchar *filename = NULL;
//
//     chooser = gtk_file_chooser_dialog_new ("Save File...",
//                                             GTK_WINDOW (app->window),
//                                             GTK_FILE_CHOOSER_ACTION_SAVE,
//                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//                                             GTK_STOCK_SAVE, GTK_RESPONSE_OK,
//                                             NULL);
//
//     if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
//     {
//         filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
//     }
//     gtk_widget_destroy (chooser);
//
//     return filename;
// }

// void buffer_insert_file (context *app, gchar *filename)
// {
//     /* TODO: fix way filename is passed from argv, use it */
//     gchar *filebuf = NULL;
//     gchar *status = NULL;
//
//     if (app->filename) split_fname (app);
//
//     if (g_file_get_contents (app->filename, &filebuf, &(app->fsize), NULL)) {
//         // buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//         gtk_text_buffer_insert_at_cursor (app->buffer, filebuf, -1);
//         gtk_text_buffer_insert_at_cursor (app->buffer, "\n", -1);
//         if (filebuf) g_free (filebuf);
//         gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
//                                         gtk_text_buffer_get_insert (app->buffer),
//                                         0.0, TRUE, 0.0, 1.0);
//         status = g_strdup_printf ("loaded : '%s'", app->fname);
//         gtk_text_buffer_set_modified (app->buffer , FALSE); /* set unmod */
//         /* TODO: set window title */
//         gtkwrite_window_set_title (NULL, app);
//
//     }
//     else {
//         /* TODO: change to error dialog */
//         status = g_strdup_printf ("file read failed : '%s'", app->fname);
//     }
//     status_update_str (app, status);
//     g_free (status);
//
//     if (filename) {}
// }

// void buffer_file_open_dlg (context *app, gchar *filename)
// {
//     GtkWidget *dialog;
//
//     /* Create a new file chooser widget */
//     dialog = gtk_file_chooser_dialog_new ("Select a file for editing",
// 					  // parent_window,
// 					  GTK_WINDOW (app->window),
// 					  GTK_FILE_CHOOSER_ACTION_OPEN,
// 					  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
// 					  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
// 					  NULL);
//
//     if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
//         app->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
//         buffer_insert_file (app, NULL); /* uugh passing ptr to member - fixed */
//     }
//
//     gtk_widget_destroy (dialog);
//     if (filename) {}
// }

// void buffer_write_file (context *app, gchar *filename)
// {
//     GError *err=NULL;
//     gchar *status;
//     gchar *text;
//     gboolean result;
//     GtkTextBuffer *buffer;
//     GtkTextIter start, end;
//
//     /* add Saving message to status bar and ensure GUI is current */
//     if (filename != NULL)
//         status = g_strdup_printf ("Saving %s...", filename);
//     else /* add else if (app->filename) -- TODO prompt or error if both null */
//         status = g_strdup_printf ("Saving %s...", app->filename);
//
//     status_update_str (app, status);
//     g_free (status);
//     while (gtk_events_pending()) gtk_main_iteration();
//
//     /* disable text view and get contents of buffer */
//     gtk_widget_set_sensitive (app->view, FALSE);
//     buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
//     gtk_text_buffer_get_start_iter (buffer, &start);
//     gtk_text_buffer_get_end_iter (buffer, &end);
//     text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
//     gtk_text_buffer_set_modified (buffer, FALSE);
//     app->modified = 0;
//     gtk_widget_set_sensitive (app->view, TRUE);
//
//     /* set the contents of the file to the text from the buffer */
//     if (filename != NULL)
//         result = g_file_set_contents (filename, text, -1, &err);
//     else
//         result = g_file_set_contents (app->filename, text, -1, &err);
//
//     if (result == FALSE)
//     {
//         /* error saving file, show message to user */
//         err_dialog (err->message);
//         g_error_free (err);
//     }
//
//     /* don't forget to free that memory! */
//     g_free (text);
//
//     if (filename != NULL)
//     {
//         /* free memory used by app->filename and set new filename.
//            call split_fname to free/update filename components */
//         // if (app->filename != NULL) g_free (app->filename);
//         if (app->filename != NULL) app_free_filename (app);
//         app->filename = filename;
//         split_fname (app);
//     }
//
//     /* clear saving status and restore default */
//     status_set_default (app);
// }

/* misc callback functions */
void view_print (GtkWidget *widget, context *app)
{
    gchar *str;
    GtkTextBuffer *buffer;

    str = g_strdup_printf ("Current Operation : %s\n",
                           gtk_menu_item_get_label (GTK_MENU_ITEM (widget)));
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    gtk_text_buffer_insert_at_cursor (buffer, str, -1);

    g_free (str);
}

void view_print_fmt (GtkWidget *widget, context *app)
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

void testcb (GtkWidget *widget, context *app)
{
    err_dialog ("It all went to hell in a handbasket!");
    if (app) {}
    if (widget) {}
}

void propcb (GtkWidget *widget, context *app)
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
