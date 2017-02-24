#include "gtk_menu.h"

GtkWidget *create_menubar (kwinst *app, GtkAccelGroup *mainaccel)
{
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
#ifdef HAVESOURCEVIEW
    GtkWidget *linenoMi;
    GtkWidget *linehlMi;
#endif

    GtkWidget *statusMenu;      /* status menu      */
    GtkWidget *statusMi;
    GtkWidget *clearMi;
    GtkWidget *propsMi;
    GtkWidget *brbMi;   /* (Big Red Button) */

    GtkWidget *toolsMenu;       /* tools menu      */
    GtkWidget *toolsMi;
    GtkWidget *indentMi;
    GtkWidget *unindentMi;
    GtkWidget *insfileMi;
    GtkWidget *toupperMi;
    GtkWidget *tolowerMi;
    GtkWidget *totitleMi;
    GtkWidget *joinMi;
#ifdef HAVESOURCEVIEW
    GtkWidget *syntaxMi;
#endif

    GtkWidget *helpMenu;        /* help menu        */
    GtkWidget *helpMi;
    GtkWidget *aboutMi;

    menubar = gtk_menu_bar_new ();
    fileMenu = gtk_menu_new ();
    editMenu = gtk_menu_new ();
    viewMenu = gtk_menu_new ();
    statusMenu = gtk_menu_new ();
    toolsMenu = gtk_menu_new ();
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
                                GDK_KEY_F5, 0, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveMi, "activate", mainaccel,
                                GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveasMi, "activate", mainaccel,
                                GDK_KEY_a, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (pagesuMi, "activate", mainaccel,
                                GDK_KEY_p, GDK_MOD1_MASK | GDK_SHIFT_MASK,
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
    gtk_menu_item_set_label (GTK_MENU_ITEM (fontMi), "_Font Selection");
#ifdef HAVESOURCEVIEW
    linehlMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_SELECT_COLOR,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (linehlMi), "_Current Line Highlight");
    linenoMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_EDIT,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (linenoMi), "Line _Numbers");
#endif

    /* create entries under 'Status' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (viewMi), viewMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), fontMi);
#ifdef HAVESOURCEVIEW
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), linehlMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (viewMenu), linenoMi);
#endif
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), viewMi);

    gtk_widget_add_accelerator (viewMi, "activate", mainaccel,
                                GDK_KEY_v, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (fontMi, "activate", mainaccel,
                                GDK_KEY_t, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
#ifdef HAVESOURCEVIEW
    gtk_widget_add_accelerator (linehlMi, "activate", mainaccel,
                                GDK_KEY_h, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (linenoMi, "activate", mainaccel,
                                GDK_KEY_F11, 0, GTK_ACCEL_VISIBLE);
#endif

    /* define status menu */
    statusMi = gtk_menu_item_new_with_mnemonic ("_Status");
    sep = gtk_separator_menu_item_new ();
    clearMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_CLEAR,
                                                  NULL);
    propsMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES,
                                                  NULL);
    brbMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_MEDIA_RECORD,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (brbMi), "_Big Red Button...");

    /* create entries under 'Status' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (statusMi), statusMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), clearMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), propsMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (statusMenu), brbMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), statusMi);

    gtk_widget_add_accelerator (clearMi, "activate", mainaccel,
                                GDK_KEY_c, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (propsMi, "activate", mainaccel,
                                GDK_KEY_r, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    /* define tools menu */
    toolsMi = gtk_menu_item_new_with_mnemonic ("_Tools");
    sep = gtk_separator_menu_item_new ();
    indentMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_INDENT,
                                                  NULL);
    unindentMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNINDENT,
                                                  NULL);
#ifdef HAVESOURCEVIEW
    syntaxMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_SELECT_COLOR,
                                                  NULL);
#endif
    insfileMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_EDIT,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (insfileMi), "_Insert File at Cursor...");
    toupperMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_GO_UP,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (toupperMi), "_Uppercase");
    tolowerMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_GO_DOWN,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (tolowerMi), "_Lowercase");
    totitleMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_GO_FORWARD,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (totitleMi), "_Titlecase");
    joinMi = gtk_image_menu_item_new_from_stock (GTK_STOCK_ADD,
                                                  NULL);
    gtk_menu_item_set_label (GTK_MENU_ITEM (joinMi), "_Join Lines");

    /* create entries under 'Tools' then add to menubar */
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (toolsMi), toolsMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), sep);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), indentMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), unindentMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu),
                           gtk_separator_menu_item_new());
#ifdef HAVESOURCEVIEW
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), syntaxMi);
    gtk_menu_item_set_label (GTK_MENU_ITEM (syntaxMi), "Syntax _Highlight");
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu),
                           gtk_separator_menu_item_new());
#endif
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), insfileMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), toupperMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), tolowerMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), totitleMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu),
                           gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (toolsMenu), joinMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), toolsMi);

    gtk_widget_add_accelerator (indentMi, "activate", mainaccel,
                                GDK_KEY_i, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (unindentMi, "activate", mainaccel,
                                GDK_KEY_i, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
#ifdef HAVESOURCEVIEW
    gtk_widget_add_accelerator (syntaxMi, "activate", mainaccel,
                                GDK_KEY_h, GDK_MOD1_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
#endif
    gtk_widget_add_accelerator (toupperMi, "activate", mainaccel,
                                GDK_KEY_u, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (tolowerMi, "activate", mainaccel,
                                GDK_KEY_u, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (totitleMi, "activate", mainaccel,
                                GDK_KEY_u, GDK_MOD1_MASK | GDK_SHIFT_MASK,
                                GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (joinMi, "activate", mainaccel,
                                GDK_KEY_j, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

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
#ifdef HAVESOURCEVIEW
    g_signal_connect (G_OBJECT (linehlMi), "activate",      /* line hglight */
                      G_CALLBACK (menu_view_linehl_activate), app);

    g_signal_connect (G_OBJECT (linenoMi), "activate",      /* line numbers */
                      G_CALLBACK (menu_view_lineno_activate), app);
#endif

    /* Status Menu */
    g_signal_connect (G_OBJECT (clearMi), "activate",       /* stat Clear   */
                      G_CALLBACK (menu_status_clear_activate), app);

    g_signal_connect (G_OBJECT (propsMi), "activate",       /* stat Props   */
                      G_CALLBACK (menu_status_properties_activate), app);

    g_signal_connect (G_OBJECT (brbMi), "activate",       /* stat Props   */
                      G_CALLBACK (menu_status_bigredbtn_activate), app);

    /* Tools Menu */
    g_signal_connect (G_OBJECT (indentMi), "activate",      /* tools indent */
                      G_CALLBACK (menu_tools_indent_activate), app);

    g_signal_connect (G_OBJECT (unindentMi), "activate",    /* unindent     */
                      G_CALLBACK (menu_tools_unindent_activate), app);

#ifdef HAVESOURCEVIEW
    g_signal_connect (G_OBJECT (syntaxMi), "activate",    /* unindent     */
                      G_CALLBACK (menu_tools_syntax_activate), app);
#endif

    g_signal_connect (G_OBJECT (insfileMi), "activate",     /* insert file  */
                      G_CALLBACK (menu_tools_insfile_activate), app);

    g_signal_connect (G_OBJECT (toupperMi), "activate",     /* to uppercase */
                      G_CALLBACK (menu_tools_toupper_activate), app);

    g_signal_connect (G_OBJECT (tolowerMi), "activate",     /* to lowercase */
                      G_CALLBACK (menu_tools_tolower_activate), app);

    g_signal_connect (G_OBJECT (totitleMi), "activate",     /* to titlecase */
                      G_CALLBACK (menu_tools_totitle_activate), app);

    g_signal_connect (G_OBJECT (joinMi), "activate",     /* to titlecase */
                      G_CALLBACK (menu_tools_join_activate), app);

    /* Help Menu */
    g_signal_connect (G_OBJECT (aboutMi), "activate",       /* help About   */
                      G_CALLBACK (menu_help_about_activate), app);

    return (app->menubar = menubar);
}

