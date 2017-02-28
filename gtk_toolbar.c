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

    /* Note: accelerators commented out below
     * there is not need to duplicate accelerators between
     * the menu and toolbar.
     */
    if(mainaccel) {}  /* stub to prevent [-Wunused-parameter] */

    new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), new, -1);
    /* set_label to change text shown when TEXT, or BOTH style shown
     * // gtk_tool_button_set_label (GTK_TOOL_BUTTON(new), "Clear");
     */
    gtk_widget_set_tooltip_text (GTK_WIDGET(new), "New file.");

    open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), open, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(open), "Open existing file.");

    save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), save, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(save), "Save file.");

    saveas = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), saveas, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(saveas), "Save as new file name.");

    sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), sep, -1);

    print = gtk_tool_button_new_from_stock(GTK_STOCK_PRINT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), print, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(print), "Print file.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), exit, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(exit), "Quit.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    undo = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
#ifndef HAVESOURCEVIEW
    gtk_widget_set_sensitive (GTK_WIDGET(undo), FALSE);
#endif
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), undo, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(undo), "Undo previous edits.");

    redo = gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
#ifndef HAVESOURCEVIEW
    gtk_widget_set_sensitive (GTK_WIDGET(redo), FALSE);
#endif
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), redo, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(redo), "Redo previos edits.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    cut = gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), cut, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(cut), "Cut selected text to clipboard.");

    copy = gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), copy, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(copy), "Copy selected text to clipboard.");

    paste = gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), paste, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(paste), "Paste text from clipboard.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    find = gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), find, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(find), "Find text.");

    replace = gtk_tool_button_new_from_stock(GTK_STOCK_FIND_AND_REPLACE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), replace, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(replace), "Find and replace text.");

    gotoln = gtk_tool_button_new_from_stock(GTK_STOCK_INDEX);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gotoln, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(gotoln), "Goto line in text.");

    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), gtk_separator_tool_item_new(), -1);

    indent = gtk_tool_button_new_from_stock(GTK_STOCK_INDENT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), indent, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(indent), "Increase indent.");

    unindent = gtk_tool_button_new_from_stock(GTK_STOCK_UNINDENT);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), unindent, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(unindent), "Decrease indent.");

    /* separator that will force remaining items to right of toolbar */
    seprt = gtk_separator_tool_item_new();
    gtk_separator_tool_item_set_draw (GTK_SEPARATOR_TOOL_ITEM(seprt), FALSE);
    gtk_tool_item_set_expand (GTK_TOOL_ITEM(seprt), TRUE);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), seprt, -1);

    preferences = gtk_tool_button_new_from_stock(GTK_STOCK_PREFERENCES);
    gtk_toolbar_insert(GTK_TOOLBAR(*toolbar), preferences, -1);
    gtk_widget_set_tooltip_text (GTK_WIDGET(preferences), "Preferences.");

    /*
     * Toolbar callbacks - simply connect to menu functions
     */
    g_signal_connect (G_OBJECT (new), "clicked",            /* file New     */
                      G_CALLBACK (menu_file_new_activate), app);

    g_signal_connect (G_OBJECT (open), "clicked",           /* file Open    */
                      G_CALLBACK (menu_file_open_activate), app);

    g_signal_connect (G_OBJECT (save), "clicked",           /* file Save    */
                      G_CALLBACK (menu_file_save_activate), app);

    g_signal_connect (G_OBJECT (saveas), "clicked",         /* file Save as */
                      G_CALLBACK (menu_file_saveas_activate), app);

    g_signal_connect (G_OBJECT (print), "clicked",          /* file Print   */
                      G_CALLBACK (menu_file_print_activate), app);

    g_signal_connect (G_OBJECT (exit), "clicked",           /* file Quit    */
                      G_CALLBACK (menu_file_quit_activate), app);

    g_signal_connect (G_OBJECT (undo), "clicked",           /* edit Undo    */
                      G_CALLBACK (menu_edit_undo_activate), app);

    g_signal_connect (G_OBJECT (redo), "clicked",           /* edit Redo    */
                      G_CALLBACK (menu_edit_redo_activate), app);

    g_signal_connect (G_OBJECT (cut), "clicked",            /* edit Cut     */
                      G_CALLBACK (menu_edit_cut_activate), app);

    g_signal_connect (G_OBJECT (copy), "clicked",           /* edit Copy    */
                      G_CALLBACK (menu_edit_copy_activate), app);

    g_signal_connect (G_OBJECT (paste), "clicked",          /* edit Paste   */
                      G_CALLBACK (menu_edit_paste_activate), app);

    g_signal_connect (G_OBJECT (find), "clicked",           /* edit Find    */
                      G_CALLBACK (menu_edit_find_activate), app);

    g_signal_connect (G_OBJECT (replace), "clicked",        /* edit Replace */
                      G_CALLBACK (menu_edit_replace_activate), app);

    g_signal_connect (G_OBJECT (gotoln), "clicked",         /* edit Goto    */
                      G_CALLBACK (menu_edit_goto_activate), app);

    g_signal_connect (G_OBJECT (indent), "clicked",         /* tools Ind+   */
                      G_CALLBACK (menu_tools_indent_activate), app);

    g_signal_connect (G_OBJECT (unindent), "clicked",       /* tools Ind-   */
                      G_CALLBACK (menu_tools_unindent_activate), app);

    g_signal_connect (G_OBJECT (preferences), "clicked",    /* edit Prefs   */
                      G_CALLBACK (menu_edit_preferences_activate), app);
}
