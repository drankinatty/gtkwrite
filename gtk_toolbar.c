#include "gtk_toolbar.h"

void create_toolbar (GtkWidget   **toolbar, GtkAccelGroup *mainaccel, kwinst *app)
{
    GtkToolItem *new;
    GtkToolItem *open;
    GtkToolItem *save;
    GtkToolItem *saveas;
    GtkToolItem *print;
    GtkToolItem *sep;
    GtkToolItem *seprt;
    GtkToolItem *exit;
    GtkToolItem *copy;
    GtkToolItem *cut;
    GtkToolItem *paste;
    GtkToolItem *undo;
    GtkToolItem *redo;
    GtkToolItem *find;
    GtkToolItem *replace;
    GtkToolItem *gotoln;
    GtkToolItem *indent;
    GtkToolItem *unindent;
    GtkToolItem *preferences;

    *toolbar = gtk_toolbar_new ();
    gtk_container_set_border_width(GTK_CONTAINER(*toolbar), 2);
    gtk_toolbar_set_show_arrow (GTK_TOOLBAR(*toolbar), TRUE);
    /* GTK_TOOLBAR_ICONS, GTK_TOOLBAR_TEXT, GTK_TOOLBAR_BOTH, GTK_TOOLBAR_BOTH_HORIZ */
    gtk_toolbar_set_style(GTK_TOOLBAR(*toolbar), GTK_TOOLBAR_ICONS);
    // gtk_toolbar_set_style(GTK_TOOLBAR(*toolbar), GTK_TOOLBAR_BOTH);

    /* Note: accelerators comment out below
     * there is not need to duplicate accelerators between
     * the menu and toolbar.
     */
    if(mainaccel) {}  /* stub to prevent [-Wunused-parameter] */

    new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), new, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(new), "clicked", mainaccel,
    //             GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    /* set_label to change text shown when TEXT, or BOTH style shown */
    // gtk_tool_button_set_label (GTK_TOOL_BUTTON(new), "Clear");
    gtk_widget_set_tooltip_text (GTK_WIDGET(new), "Clear buffer for new text.");

    open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), open, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(open), "clicked", mainaccel,
    //             GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(open), "Open file in TextView buffer.");

    save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), save, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(save), "clicked", mainaccel,
    //             GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(save), "Save TextView buffer in file.");

    saveas = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), saveas, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(saveas), "clicked", mainaccel,
    //             GDK_KEY_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(saveas), "Save as new file.");

    sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), sep, -1);

    print = gtk_tool_button_new_from_stock(GTK_STOCK_PRINT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), print, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(print), "clicked", mainaccel,
    //             GDK_KEY_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(print), "Print buffer contents.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), exit, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(exit), "clicked", mainaccel,
    //             GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(exit), "Quit.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    undo = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
#ifndef HAVESOURCEVIEW
    gtk_widget_set_sensitive (GTK_WIDGET(undo), FALSE);
#endif
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), undo, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(undo), "clicked", mainaccel,
    //             GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(undo), "Undo previous edits.");

    redo = gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
#ifndef HAVESOURCEVIEW
    gtk_widget_set_sensitive (GTK_WIDGET(redo), FALSE);
#endif
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), redo, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(redo), "clicked", mainaccel,
    //             GDK_KEY_z, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(redo), "Redo previos edits.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    cut = gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), cut, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(cut), "clicked", mainaccel,
    //             GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(cut), "Cut selected text to clipboard.");

    copy = gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), copy, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(copy), "clicked", mainaccel,
    //             GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(copy), "Copy selected text to clipboard.");

    paste = gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), paste, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(paste), "clicked", mainaccel,
    //             GDK_KEY_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(paste), "Paste text from clipboard.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    find = gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), find, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(find), "clicked", mainaccel,
    //             GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(find), "Find text.");

    replace = gtk_tool_button_new_from_stock(GTK_STOCK_FIND_AND_REPLACE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), replace, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(replace), "clicked", mainaccel,
    //             GDK_KEY_r, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(replace), "Find and replace text.");

    gotoln = gtk_tool_button_new_from_stock(GTK_STOCK_INDEX);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gotoln, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(gotoln), "clicked", mainaccel,
    //             GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(gotoln), "Goto line in text.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    indent = gtk_tool_button_new_from_stock(GTK_STOCK_INDENT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), indent, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(indent), "clicked", mainaccel,
    //             GDK_KEY_i, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(indent), "Increase indent.");

    unindent = gtk_tool_button_new_from_stock(GTK_STOCK_UNINDENT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), unindent, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(unindent), "clicked", mainaccel,
    //             GDK_KEY_i, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(unindent), "Decrease indent.");

    /* separator that will force remaining items to right of toolbar */
    seprt = gtk_separator_tool_item_new();
    gtk_separator_tool_item_set_draw (GTK_SEPARATOR_TOOL_ITEM(seprt), FALSE);
    gtk_tool_item_set_expand (GTK_TOOL_ITEM(seprt), TRUE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), seprt, -1);

    preferences = gtk_tool_button_new_from_stock(GTK_STOCK_PREFERENCES);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), preferences, -1);
    // gtk_widget_add_accelerator (GTK_WIDGET(preferences), "clicked", mainaccel,
    //             GDK_KEY_p, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_tooltip_text (GTK_WIDGET(preferences), "Preferences.");

    /* Toolbar */
    g_signal_connect (G_OBJECT (new), "clicked",            /* file New     */
                      G_CALLBACK (toolbar_new_activate), app);

    g_signal_connect (G_OBJECT (open), "clicked",           /* file Open    */
                      G_CALLBACK (toolbar_open_activate), app);

    g_signal_connect (G_OBJECT (save), "clicked",           /* file Save    */
                      G_CALLBACK (toolbar_save_activate), app);

    g_signal_connect (G_OBJECT (saveas), "clicked",         /* file Save as */
                      G_CALLBACK (toolbar_saveas_activate), app);

    g_signal_connect (G_OBJECT (print), "clicked",          /* file Print   */
                      G_CALLBACK (toolbar_print_activate), app);

    g_signal_connect (G_OBJECT (exit), "clicked",           /* file Quit    */
                      G_CALLBACK (toolbar_quit_activate), app);

    g_signal_connect (G_OBJECT (undo), "clicked",           /* edit Undo    */
                      G_CALLBACK (toolbar_undo_activate), app);

    g_signal_connect (G_OBJECT (redo), "clicked",           /* edit Redo    */
                      G_CALLBACK (toolbar_redo_activate), app);

    g_signal_connect (G_OBJECT (cut), "clicked",            /* edit Cut     */
                      G_CALLBACK (toolbar_cut_activate), app);

    g_signal_connect (G_OBJECT (copy), "clicked",           /* edit Copy    */
                      G_CALLBACK (toolbar_copy_activate), app);

    g_signal_connect (G_OBJECT (paste), "clicked",          /* edit Paste   */
                      G_CALLBACK (toolbar_paste_activate), app);

    g_signal_connect (G_OBJECT (find), "clicked",           /* edit Find    */
                      G_CALLBACK (toolbar_find_activate), app);

    g_signal_connect (G_OBJECT (replace), "clicked",        /* edit Replace */
                      G_CALLBACK (toolbar_replace_activate), app);

    g_signal_connect (G_OBJECT (gotoln), "clicked",         /* edit Goto    */
                      G_CALLBACK (toolbar_goto_activate), app);

    g_signal_connect (G_OBJECT (indent), "clicked",      /* tools Ind+   */
                      G_CALLBACK (toolbar_indent_activate), app);

    g_signal_connect (G_OBJECT (unindent), "clicked",      /* tools Ind-   */
                      G_CALLBACK (toolbar_unindent_activate), app);

    g_signal_connect (G_OBJECT (preferences), "clicked",    /* edit Prefs   */
                      G_CALLBACK (toolbar_preferences_activate), app);

}

void toolbar_new_activate (GtkMenuItem *menuitem, kwinst *app)
{
    buffer_clear (app);
    status_set_default (app);
    if (menuitem) {}
}

void toolbar_open_activate (GtkMenuItem *menuitem, kwinst *app)
{
    buffer_clear (app);
    buffer_file_open_dlg (app, NULL);

#ifdef HAVESOURCEVIEW
    app->langmgr = gtk_source_language_manager_get_default ();
    app->language = gtk_source_language_manager_guess_language (app->langmgr,
                                                        app->filename, NULL);
    gtk_source_buffer_set_language (app->buffer, app->language);
    gtk_source_buffer_set_highlight_syntax (app->buffer, TRUE);
#endif

    if (menuitem) {}
}

void toolbar_save_activate (GtkMenuItem *menuitem, kwinst *app)
{
    buffer_save_file (app, NULL);
    if (menuitem) {}
}

void toolbar_saveas_activate (GtkMenuItem *menuitem, kwinst *app)
{
    gchar *filename = NULL;

    filename = get_save_filename (app);
    if (filename)
        buffer_save_file (app, filename);
    else
        dlg_info ("Warning: Save of File Canceled!", "SAVE CANCELED!");

    if (menuitem) {}
}

void print_close (GtkWidget *window, gpointer data)
{
    gtk_widget_destroy (window);
    if (data) {}
}

void toolbar_print_activate (GtkMenuItem *menuitem, kwinst *app)
{
    do_print (app);
    if (menuitem) {}
}

void toolbar_quit_activate (GtkMenuItem *menuitem, kwinst *app)
{
    /* check changed, prompt yes/no */
    if (buffer_chk_save_on_exit (GTK_TEXT_BUFFER(app->buffer))) {
        if (!app->filename) {
            gchar *filename;
            while (!(filename = get_save_filename (app))) {
                if (dlg_yes_no_msg ("Warning: Do you want to cancel save?",
                                    "Warning - Save Canceled", FALSE))
                    goto cancel_save;
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
    cancel_save:

    gtk_main_quit ();
    if (menuitem || app) {}
}

void toolbar_undo_activate (GtkMenuItem *menuitem, kwinst *app)
{
#ifdef HAVESOURCEVIEW
    if (gtk_source_buffer_can_undo (app->buffer))
        gtk_source_buffer_undo (app->buffer);
    else
        err_dialog ("Error:\n\nUnable to undo later operation.");
#else
    if (app) {}
#endif

    if (menuitem || app) {}
}

void toolbar_redo_activate (GtkMenuItem *menuitem, kwinst *app)
{
#ifdef HAVESOURCEVIEW
    if (gtk_source_buffer_can_redo (app->buffer))
        gtk_source_buffer_redo (app->buffer);
    else
        err_dialog ("Error:\n\nUnable to redo later operation.");
#else
    if (app) {}
#endif

    if (menuitem || app) {}
}

void toolbar_cut_activate (GtkMenuItem *menuitem, kwinst *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);

    if (menuitem || app) {}
}

void toolbar_copy_activate (GtkMenuItem *menuitem, kwinst *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_copy_clipboard (buffer, clipboard);

    if (menuitem || app) {}
}

void toolbar_paste_activate (GtkMenuItem *menuitem, kwinst *app)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);

    if (menuitem || app) {}
}

void toolbar_find_activate (GtkMenuItem *menuitem, kwinst *app)
{
    create_find_dlg (app);
    if (menuitem || app) {}
}

void toolbar_replace_activate (GtkMenuItem *menuitem, kwinst *app)
{
    create_replace_dlg (app);
    if (menuitem || app) {}
}

void toolbar_goto_activate (GtkMenuItem *menuitem, kwinst *app)
{
    create_goto_dlg (app);
    if (menuitem || app) {}
}

void toolbar_indent_activate (GtkMenuItem *menuitem, kwinst *app)
{
    status_pop (GTK_WIDGET (menuitem), app);
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(app->buffer), &start, &end);
    buffer_indent_lines (app, &start, &end);

    if (menuitem || app) {}
}

void toolbar_unindent_activate (GtkMenuItem *menuitem, kwinst *app)
{
    status_pop (GTK_WIDGET (menuitem), app);
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(app->buffer), &start, &end);
    buffer_unindent_lines (app, &start, &end);
    if (menuitem || app) {}
}

void toolbar_preferences_activate (GtkMenuItem *menuitem, kwinst *app)
{
    status_update_str (app, "Edit : Preferences");
    create_settings_dlg (app);

    if (menuitem || app) {}
}

