// #include "gtk_windef.h"
#include "gtk_findreplace.h"

/* find replace keypress handler */
static gboolean on_fr_keypress (GtkWidget *widget, GdkEventKey *event,
                                context *app);

GtkWidget *create_find_dlg (context *app)
{
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *frame1;
    GtkWidget *frame2;
    GtkWidget *table;
    GtkWidget *table2;
    GtkWidget *label;
    GtkWidget *hbtweak;
    GtkWidget *hbox;
    GtkWidget *btnfind;
    GtkWidget *btnclose;

    guint i;

    /* create toplevel window */
    if (!(app->findrepwin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_find_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->findrepwin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->findrepwin), 264, 264);
    gtk_window_set_title (GTK_WINDOW (app->findrepwin), "Find Text");
    gtk_container_set_border_width (GTK_CONTAINER (app->findrepwin), 5);
    g_signal_connect (app->findrepwin, "destroy",
		      G_CALLBACK (btnclose_activate), app);

    app->dlgid = DLGFIND;   /* set dialog id to find dialog */
    /* for inclusion in main app set modal and transient_for (to keep on top)
     * gtk_window_set_modal (GtkWindow *window, gboolean modal);
     * gtk_window_set_transient_for(GtkWindow *window, GtkWindow *parent)
     */
    // gtk_window_set_modal (app->findrepwin, TRUE);
    // gtk_window_set_transient_for (app->findrepwin, mainwin->window)

    /* main vbox container
     * spacing profided on frames as containers */
    vbox = gtk_vbox_new (FALSE, 0);
//     gtk_box_set_spacing (GTK_BOX (vbox), 2);
    gtk_container_add (GTK_CONTAINER (app->findrepwin), vbox);
    gtk_widget_show (vbox);

    /* frame1 - Search Box */
    frame1 = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame1), "Find");
    gtk_frame_set_label_align (GTK_FRAME (frame1), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame1), 5);

    /* table inside frame1 */
    table = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame1), table);

    /* label for combo entry */
    label = gtk_label_new_with_mnemonic ("_Text to Find:");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 0, 1);
    gtk_widget_show (hbtweak);
    gtk_table_set_row_spacing (GTK_TABLE (table), 0, 0);

    /* combo box entry GTK_COMBO_BOX_TEXT */
    app->entryfind = gtk_combo_box_text_new_with_entry();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), app->entryfind);
    gtk_table_attach_defaults (GTK_TABLE (table), app->entryfind, 0, 2, 1, 2);
    gtk_table_set_row_spacing (GTK_TABLE (table), 1, 10);
    for (i = 0; i < app->nfentries; i++)
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind),
                                        app->findtext[i]);

    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), "display text 1");
    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), "display text 2");

    /* regex checkbox */
    app->chkregex = gtk_check_button_new_with_mnemonic ("Regular e_xpression");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkregex), app->optregex);
    gtk_table_attach_defaults (GTK_TABLE (table), app->chkregex, 0, 1, 2, 3);

    /* regex edit button */
    app->btnregex = gtk_button_new_with_mnemonic ("_Edit...");
    gtk_widget_set_size_request (app->btnregex, 80, 24);
    gtk_widget_set_sensitive (app->btnregex, app->optregex);
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbtweak), app->btnregex, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 1, 2, 2, 3);

    // gtk_container_add (GTK_CONTAINER (frame1), app->entryfind);
    gtk_widget_show (app->entryfind);
    gtk_widget_show (label);
    gtk_widget_show (hbtweak);
    gtk_widget_show (app->chkregex);
    gtk_widget_show (app->btnregex);
    gtk_widget_show (table);
    gtk_widget_show (frame1);

    gtk_box_pack_start (GTK_BOX (vbox), frame1, FALSE, FALSE, 0);

    /* frame2 - Options */
    frame2 = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame2), "Options");
    gtk_frame_set_label_align (GTK_FRAME (frame2), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame2), 5);
    gtk_widget_show (frame2);

    gtk_box_pack_start (GTK_BOX (vbox), frame2, FALSE, FALSE, 0);

    /* table in frame2 */
    table2 = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table2), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table2), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table2), 5);
    gtk_container_add (GTK_CONTAINER (frame2), table2);

    /* options checkboxs */
    app->chkcase = gtk_check_button_new_with_mnemonic ("C_ase sensitive");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkcase), app->optcase);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkcase, 0, 1, 0, 1);
    gtk_widget_show (app->chkcase);

    app->chkwhole = gtk_check_button_new_with_mnemonic ("_Whole words only");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkwhole), app->optwhole);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkwhole, 0, 1, 1, 2);
    gtk_widget_show (app->chkwhole);

    app->chkfrom = gtk_check_button_new_with_mnemonic ("From c_ursor");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkfrom), app->optfrom);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkfrom, 0, 1, 2, 3);
    gtk_widget_show (app->chkfrom);

    app->chkback = gtk_check_button_new_with_mnemonic ("Find _backwards");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkback), app->optback);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkback, 1, 2, 0, 1);
    gtk_widget_show (app->chkback);

    chk_existing_selection (app);   /* sets app->optselect, create marks */
    app->chkselect = gtk_check_button_new_with_mnemonic ("_Selected text");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkselect), app->optselect);
    // gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkselect), TRUE);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkselect, 1, 2, 1, 2);
    gtk_widget_show (app->chkselect);

    gtk_widget_show (table2);

    /* hbox (replacing table) button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    btnfind = gtk_button_new_with_mnemonic ("_Find");
    app->btnfind = btnfind;     /* FIXME - remove btnfind? */
    gtk_widget_set_size_request (btnfind, 80, 24);
    // gtk_box_pack_end (GTK_BOX (hbox), btnfind, FALSE, FALSE, 0); // (moved after btnclose)
    gtk_widget_set_sensitive (btnfind, app->findcbchgd);
    gtk_widget_show (btnfind);

    btnclose = gtk_button_new_with_mnemonic ("_Close");
    gtk_widget_set_size_request (btnclose, 80, 24);

    gtk_box_pack_end (GTK_BOX (hbox), btnclose, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), btnfind, FALSE, FALSE, 0); // pack on rt-side.
    gtk_widget_show (btnclose);

    gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    g_signal_connect (app->chkregex, "toggled",
                      G_CALLBACK (chkregex_toggled), app);

    g_signal_connect (app->chkcase, "toggled",
                      G_CALLBACK (chkcase_toggled), app);

    g_signal_connect (app->chkwhole, "toggled",
                      G_CALLBACK (chkwhole_toggled), app);

    g_signal_connect (app->chkfrom, "toggled",
                      G_CALLBACK (chkfrom_toggled), app);

    g_signal_connect (app->chkback, "toggled",
                      G_CALLBACK (chkback_toggled), app);

    g_signal_connect (app->chkselect, "toggled",
                      G_CALLBACK (chkselect_toggled), app);

    g_signal_connect (btnfind, "clicked",
                      G_CALLBACK (btnfind_activate), app);

    g_signal_connect (btnclose, "clicked",
                      G_CALLBACK (btnclose_activate), app);

    g_signal_connect (app->findrepwin, "key_press_event",
                      G_CALLBACK (on_fr_keypress), app);

    g_signal_connect (app->entryfind, "changed",
                      G_CALLBACK (entry_find_activate), app);

    // g_signal_connect_swapped (btnclose, "clicked",
    //                           G_CALLBACK (delete_event),
    //                           window);

    gtk_widget_show (app->findrepwin);
    // gtk_widget_show_all (app->findrepwin);

    return (app->findrepwin);
}

GtkWidget *create_replace_dlg (context *app)
{
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *frame1;
    GtkWidget *framer;
    GtkWidget *frame2;
    GtkWidget *table;
    GtkWidget *table2;
    GtkWidget *label;
    GtkWidget *hbtweak;
    GtkWidget *hbox;
    GtkWidget *btnreplace;
    GtkWidget *btnclose;

    guint i;

    /* create toplevel window */
    if (!(app->findrepwin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_replace_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->findrepwin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->findrepwin), 264, 264);
    gtk_window_set_title (GTK_WINDOW (app->findrepwin), "Replace Text");
    gtk_container_set_border_width (GTK_CONTAINER (app->findrepwin), 5);
    g_signal_connect (app->findrepwin, "destroy",
		      G_CALLBACK (btnclose_activate), app);

    app->dlgid = DLGREPL;   /* set dialog id to replace dialog */
    /* for inclusion in main app set modal and transient_for (to keep on top)
     * gtk_window_set_modal (GtkWindow *window, gboolean modal);
     * gtk_window_set_transient_for(GtkWindow *window, GtkWindow *parent)
     */
    // gtk_window_set_modal (app->findrepwin, TRUE);
    // gtk_window_set_transient_for (app->findrepwin, mainwin->window)

    /* main vbox container
     * spacing profided on frames as containers */
    vbox = gtk_vbox_new (FALSE, 0);
    // gtk_box_set_spacing (GTK_BOX (vbox), 2);
    gtk_container_add (GTK_CONTAINER (app->findrepwin), vbox);
    gtk_widget_show (vbox);

    /* frame1 - Search Box */
    frame1 = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame1), "Find");
    gtk_frame_set_label_align (GTK_FRAME (frame1), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame1), 5);

    /* table inside frame1 */
    table = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame1), table);

    /* label for combo entry */
    label = gtk_label_new_with_mnemonic ("_Text to find:");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 0, 1);
    gtk_widget_show (hbtweak);
    gtk_table_set_row_spacing (GTK_TABLE (table), 0, 0);

    /* combo box entry (GtkComboBoxText *) */
    app->entryfind = gtk_combo_box_text_new_with_entry();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), app->entryfind);
    gtk_table_attach_defaults (GTK_TABLE (table), app->entryfind, 0, 2, 1, 2);
    gtk_table_set_row_spacing (GTK_TABLE (table), 1, 10);
    for (i = 0; i < app->nfentries; i++)
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind),
                                        app->findtext[i]);

    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), "display text 1");
    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), "display text 2");

    /* regex checkbox */
    app->chkregex = gtk_check_button_new_with_mnemonic ("Regular e_xpression");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkregex), app->optregex);
    gtk_table_attach_defaults (GTK_TABLE (table), app->chkregex, 0, 1, 2, 3);

    /* regex edit button - set active state FALSE - gray Edit... button */
    app->btnregex = gtk_button_new_with_mnemonic ("_Edit...");
    gtk_widget_set_size_request (app->btnregex, 80, 24);
    if (!app->optregex)
        gtk_widget_set_sensitive (app->btnregex, FALSE);
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbtweak), app->btnregex, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 1, 2, 2, 3);

    // gtk_container_add (GTK_CONTAINER (frame1), app->entryfind);
    gtk_widget_show (app->entryfind);
    gtk_widget_show (label);
    gtk_widget_show (hbtweak);
    gtk_widget_show (app->chkregex);
    gtk_widget_show (app->btnregex);
    gtk_widget_show (table);
    gtk_widget_show (frame1);

    gtk_box_pack_start (GTK_BOX (vbox), frame1, FALSE, FALSE, 0);

    /* framer - Replace Box */
    framer = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (framer), "Replace With");
    gtk_frame_set_label_align (GTK_FRAME (framer), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (framer), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (framer), 5);

    /* table inside framer */
    table = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (framer), table);

    /* label for combo entry */
    label = gtk_label_new_with_mnemonic ("Replace_ment text:");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 0, 1);
    gtk_widget_show (hbtweak);
    gtk_table_set_row_spacing (GTK_TABLE (table), 0, 0);

    /* combo box entry (GtkComboBoxText *) */
    app->entryreplace = gtk_combo_box_text_new_with_entry();
    gtk_label_set_mnemonic_widget (GTK_LABEL (label), app->entryreplace);
    gtk_table_attach_defaults (GTK_TABLE (table), app->entryreplace, 0, 2, 1, 2);
    gtk_table_set_row_spacing (GTK_TABLE (table), 1, 10);
    for (i = 0; i < app->nrentries; i++)
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryreplace),
                                        app->reptext[i]);

    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryreplace), "replacement text 1");
    // gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryreplace), "replacement text 2");

    /* regex checkbox */
    app->chkplace = gtk_check_button_new_with_mnemonic ("Use p_laceholders");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkplace), app->optplace);
    gtk_table_attach_defaults (GTK_TABLE (table), app->chkplace, 0, 1, 2, 3);

    /* regex edit button */
    app->btnplace = gtk_button_new_with_mnemonic ("Insert Place_holder");
    gtk_widget_set_size_request (app->btnplace, 110, 24);
    if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkplace)))
        gtk_widget_set_sensitive (app->btnplace, FALSE);
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbtweak), app->btnplace, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 1, 2, 2, 3);

    // gtk_container_add (GTK_CONTAINER (framer), app->entryreplace);
    gtk_widget_show (app->entryreplace);
    gtk_widget_show (label);
    gtk_widget_show (hbtweak);
    gtk_widget_show (app->chkplace);
    gtk_widget_show (app->btnplace);
    gtk_widget_show (table);
    gtk_widget_show (framer);

    gtk_box_pack_start (GTK_BOX (vbox), framer, FALSE, FALSE, 0);

    /* frame2 - Options */
    frame2 = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame2), "Options");
    gtk_frame_set_label_align (GTK_FRAME (frame2), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame2), 5);
    gtk_widget_show (frame2);

    gtk_box_pack_start (GTK_BOX (vbox), frame2, FALSE, FALSE, 0);

    /* table in frame2 */
    table2 = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table2), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table2), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table2), 5);
    gtk_container_add (GTK_CONTAINER (frame2), table2);

    /* options checkboxs */
    app->chkcase = gtk_check_button_new_with_mnemonic ("C_ase sensitive");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkcase), app->optcase);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkcase, 0, 1, 0, 1);
    gtk_widget_show (app->chkcase);

    app->chkwhole = gtk_check_button_new_with_mnemonic ("_Whole words only");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkwhole), app->optwhole);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkwhole, 0, 1, 1, 2);
    gtk_widget_show (app->chkwhole);

    app->chkfrom = gtk_check_button_new_with_mnemonic ("From c_ursor");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkfrom), app->optfrom);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkfrom, 0, 1, 2, 3);
    gtk_widget_show (app->chkfrom);

    app->chkback = gtk_check_button_new_with_mnemonic ("Find _backwards");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkback), app->optback);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkback, 1, 2, 0, 1);
    gtk_widget_show (app->chkback);

    chk_existing_selection (app);   /* sets app->optselect, create marks */
    app->chkselect = gtk_check_button_new_with_mnemonic ("_Selected text");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkselect), app->optselect);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkselect, 1, 2, 1, 2);
    gtk_widget_show (app->chkselect);

    app->chkprompt = gtk_check_button_new_with_mnemonic ("_Prompt on replace");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkprompt), app->optprompt);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkprompt, 1, 2, 2, 3);
    gtk_widget_show (app->chkprompt);

    gtk_widget_show (table2);

    /* hbox (replacing table) button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    btnreplace = gtk_button_new_with_mnemonic ("_Replace");
    app->btnreplace = btnreplace;   /* FIXME - remove btnreplace? */
    gtk_widget_set_size_request (btnreplace, 80, 24);
    // gtk_box_pack_end (GTK_BOX (hbox), btnreplace, FALSE, FALSE, 0); // (moved after btnclose)
    /* TODO only set btnreplace sensitive if btnfind AND btnreplace changed */
    gtk_widget_set_sensitive (btnreplace, app->findcbchgd);
    gtk_widget_show (btnreplace);

    btnclose = gtk_button_new_with_mnemonic ("_Close");
    gtk_widget_set_size_request (btnclose, 80, 24);

    gtk_box_pack_end (GTK_BOX (hbox), btnclose, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), btnreplace, FALSE, FALSE, 0); // pack on rt-side.
    gtk_widget_show (btnclose);

    gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    g_signal_connect (app->chkregex, "toggled",
                      G_CALLBACK (chkregex_toggled), app);

    g_signal_connect (app->chkplace, "toggled",
                      G_CALLBACK (chkplace_toggled), app);

    g_signal_connect (app->chkcase, "toggled",
                      G_CALLBACK (chkcase_toggled), app);

    g_signal_connect (app->chkwhole, "toggled",
                      G_CALLBACK (chkwhole_toggled), app);

    g_signal_connect (app->chkfrom, "toggled",
                      G_CALLBACK (chkfrom_toggled), app);

    g_signal_connect (app->chkback, "toggled",
                      G_CALLBACK (chkback_toggled), app);

    g_signal_connect (app->chkselect, "toggled",
                      G_CALLBACK (chkselect_toggled), app);

    g_signal_connect (app->chkprompt, "toggled",
                      G_CALLBACK (chkprompt_toggled), app);

    g_signal_connect (btnreplace, "clicked",
                      G_CALLBACK (btnreplace_activate), app);

    g_signal_connect (btnclose, "clicked",
                      G_CALLBACK (btnclose_activate), app);

    /* pass keypress to handler */
    g_signal_connect (app->findrepwin, "key_press_event",
                      G_CALLBACK (on_fr_keypress), app);

    /* set replace button sensitive, require entries in both */
    g_signal_connect (app->entryfind, "changed",
                      G_CALLBACK (entry_set_find_sensitive), app);

    g_signal_connect (app->entryfind, "changed",
                      G_CALLBACK (entry_replace_activate), app);

    g_signal_connect (app->entryreplace, "changed",
                      G_CALLBACK (entry_set_repl_sensitive), app);

    g_signal_connect (app->entryreplace, "changed",
                      G_CALLBACK (entry_replace_activate), app);

    // g_signal_connect_swapped (btnclose, "clicked",
    //                           G_CALLBACK (delete_event),
    //                           window);

    gtk_widget_show (app->findrepwin);
    // gtk_widget_show_all (app->findrepwin);

    return (app->findrepwin);
}

void findrep_init (context *app)
{
    app->findrepwin = NULL; /* initialize widgets to NULL */
    app->dlgid      = 0;
    app->entryfind  = NULL;
    app->entryreplace = NULL;
    app->btnregex   = NULL;
    app->btnplace   = NULL;
    app->chkregex   = NULL;
    app->chkplace   = NULL;
    app->chkcase    = NULL;
    app->chkwhole   = NULL;
    app->chkfrom    = NULL;
    app->chkback    = NULL;
    app->chkselect  = NULL;
    app->chkprompt  = NULL;  /* allocate array of pointers */
    app->findtext   = g_malloc0 (MAXLE * sizeof *(app->findtext));
    app->reptext    = g_malloc0 (MAXLE * sizeof *(app->reptext));
    app->nfentries  = 0;
    app->nrentries  = 0;
    app->fmax       = MAXLE;
    app->rmax       = MAXLE;
    app->optregex   = TRUE;  /* initial checkbox states */
    app->optplace   = FALSE;
    app->optcase    = TRUE;
    app->optwhole   = FALSE;
    app->optfrom    = FALSE;
    app->optback    = FALSE;
    app->optselect  = FALSE;
    app->optprompt  = TRUE;
    app->findcbchgd = FALSE;
    app->replcbchgd = FALSE;

    app->txtfound   = FALSE;
    app->last_pos   = NULL;
    app->selstart   = NULL;
    app->selend     = NULL;

    if (!(app->findtext && app->reptext)) {
        err_dialog ("findrep_init()\nvirtual memory exhausted.");
        // findrep_destroy (app);
        // context_destroy (app);
        gtk_main_quit();
    }
}

void findrep_destroy (context *app)
{   /* free allocated struct values */
    guint i;    /* free combobox lists */
    for (i = 0; i < app->nfentries; i++) g_free (app->findtext[i]);
    g_free (app->findtext);
    for (i = 0; i < app->nrentries; i++) g_free (app->reptext[i]);
    g_free (app->reptext);

    /* temp dump of options */
#ifdef DEBUGFR
    dumpopts (app);
#endif
}

/* entry helper callbacks for replace button set sensitive */
void entry_set_find_sensitive (GtkWidget *widget, context *app) {

    app->findcbchgd = TRUE;
    if (widget) {}
}

void entry_set_repl_sensitive (GtkWidget *widget, context *app) {

    app->replcbchgd = TRUE;
    if (widget) {}
}

/* entry comboboxes */
void entry_find_activate (GtkWidget *widget, context *app) {

    app->findcbchgd = TRUE; /* just set, or add set callback to find dlg */
                            /* and change to if (app->findcbchgd) */
    gtk_widget_set_sensitive (app->btnfind, app->findcbchgd);

    if (widget) {}
}

void entry_replace_activate (GtkWidget *widget, context *app) {
    // app->findcbchgd = TRUE;
    if (app->findcbchgd && app->replcbchgd) /* require both for replace */
        gtk_widget_set_sensitive (app->btnreplace, app->findcbchgd);

    if (widget) {}
}

/* option checkboxs */
void chkregex_toggled    (GtkWidget *widget, context *app)
{
    app->optregex = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
    gtk_widget_set_sensitive (app->btnregex, app->optregex);
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
//         gtk_widget_set_sensitive (app->btnregex, TRUE);
//         app->optregex = TRUE;
//     }
//     else {
//         gtk_widget_set_sensitive (app->btnregex, FALSE);
//         app->optregex = FALSE;
//     }
    // if (app) {}
    // if (widget) {}
}

void chkplace_toggled   (GtkWidget *widget, context *app)
{
    app->optplace = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
    gtk_widget_set_sensitive (app->btnplace, app->optplace);
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
//         gtk_widget_set_sensitive (app->btnplace, TRUE);
//         app->optplace = TRUE;
//     }
//     else {
//         gtk_widget_set_sensitive (app->btnplace, FALSE);
//         app->optplace = FALSE;
//     }
    // if (app) {}
    // if (widget) {}
}

void chkcase_toggled    (GtkWidget *widget, context *app)
{
    app->optcase = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optcase = TRUE;
//     else
//         app->optcase = FALSE;
    // if (app) {}
    // if (widget) {}
}

void chkwhole_toggled   (GtkWidget *widget, context *app)
{
    app->optwhole = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optwhole = TRUE;
//     else
//         app->optwhole = FALSE;
    // if (app) {}
    // if (widget) {}
}

void chkfrom_toggled    (GtkWidget *widget, context *app)
{
    app->optfrom = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optfrom = TRUE;
//     else
//         app->optfrom = FALSE;
    // if (app) {}
    // if (widget) {}
}

void chkback_toggled    (GtkWidget *widget, context *app)
{
    app->optback = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optback = TRUE;
//     else
//         app->optback = FALSE;
    // if (app) {}
    // if (widget) {}
}

void chkselect_toggled  (GtkWidget *widget, context *app)
{
//     GtkTextIter sel_start, sel_end;
//     gboolean selected = FALSE;
//
//     /* check whether existing selection active */
//     selected = gtk_text_buffer_get_selection_bounds (app->buffer,
//                                             &sel_start, &sel_end);
//
//     if (selected) {
//         app->optselect = TRUE;
//         gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), app->optselect);
//     }

    app->optselect = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));

//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optselect = TRUE;
//     else
//         app->optselect = FALSE;
    // if (app) {}
    // if (widget) {}
}

void chkprompt_toggled  (GtkWidget *widget, context *app)
{
    app->optprompt = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
//     if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//         app->optselect = TRUE;
//     else
//         app->optselect = FALSE;
    // if (app) {}
    // if (widget) {}
}

/* dialog buttons */
void btnregex_activate   (GtkWidget *widget, context *app)
{
    if (app) {}
    if (widget) {}
}

void btnplace_activate   (GtkWidget *widget, context *app)
{
    if (app) {}
    if (widget) {}
}

/** check if selection exists when find/replace dlg called,
 *  create marks for beginning and end to restore after
 *  find/replace operations
 */
void chk_existing_selection (context *app)
{
    GtkTextIter sel_start, sel_end;
    gboolean selected = FALSE;

    // app->iset = FALSE;

    /* check whether existing selection active */
    selected = gtk_text_buffer_get_selection_bounds (app->buffer,
                                            &sel_start, &sel_end);

    if (selected) {
        app->optselect = TRUE;
        app->selstart = gtk_text_buffer_create_mark (app->buffer,
                                    "selstart", &sel_start, FALSE);
        app->selend = gtk_text_buffer_create_mark (app->buffer,
                                    "selend", &sel_end, FALSE);
        // app->iset = TRUE;
    }
    else {
        app->optselect = FALSE;
        app->selstart = app->selend = NULL;
    }
}

/** common find function for both find/replace dialogs, locates text
 *  within app->buffer and sets app->last_pos for next search begin,
 *  test match against dialog options, sets app->txtfound on success.
 */
void find (context *app, const gchar *text)
{
    if (!text || !*text) return;

    GtkTextIter iter, mstart, mend;
    gboolean found = FALSE;

    /* start infinite loop here, loop until all options satisfied or end
     * of buffer reached, then break setting app->txtfound as needed.
     * TODO: watch for change from forward/backward to update app->last_pos
     * to avoid needing to press find twice.
     * TODO: handle if existing selection was made from end->start, requires
     * checking iter from gtk_text_buffer_get_insert and selstart/selend.
     * TODO: activate find/replace on [enter]/[kp_return] from findtext and
     * replacetext combo box entry.
     * TODO: F3 - btnfind_activate (think about Prompt on Replace)
     */
    for (;;) {  /* loop until word found matching search options or end */
        if (!app->last_pos) {               /* find first occurrence */
            if (!app->optback) {            /* forward search  */
                if (app->optfrom)           /* if search from cursor */
                    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter,
                                gtk_text_buffer_get_insert (app->buffer));
                else if (app->optselect)    /* search from select start */
                    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter,
                                                        app->selstart);
                else                        /* otherwise get start iter */
                    gtk_text_buffer_get_start_iter (app->buffer, &iter);
            }
            else {                          /* backwards search */
                if (app->optfrom || app->optselect) /* have same start */
                    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter,
                                gtk_text_buffer_get_insert (app->buffer));
                else                        /* start at end of buffer */
                    gtk_text_buffer_get_end_iter (app->buffer, &iter);
            }
        }
        else if (app->txtfound || found)    /* find next occurrence  */
            gtk_text_buffer_get_iter_at_mark (app->buffer, &iter, app->last_pos);
        else    /* text not found */
            return;

        /* case sensitive forward/reverse search from iter for 'text' setting
         * iters mstart & mend pointing to first & last char in matched text.
         */
        if (app->optback)   /* search backward */
            found = gtk_text_iter_backward_search (&iter, text, 0,
                                                &mstart, &mend, NULL);
        else                /* search forward */
            found = gtk_text_iter_forward_search (&iter, text, 0,
                                                &mstart, &mend, NULL);

        if (found)  /* text found in buffer, now refine match w/options */
        {
            /* if optselect, in both forward/backward search check iter
             * against mend/mstart to determine if search is within
             * original selection range.
             */
            if (app->optselect) {   /* if srch in selected-text */
                if (app->optback) { /* if searching backwards */
                    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter,
                                                        app->selstart);
                    if (gtk_text_iter_compare (&mstart, &iter) < 0 )
                        return;
                }
                else {  /* for forward search in selection */
                    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter,
                                                        app->selend);
                    if (gtk_text_iter_compare (&iter, &mend) < 0 )
                        return;
                }
            }   /* TODO: restore original selection if no find in range */

            /* select range, setting start, end iters, last_pos mark */
            gtk_text_buffer_select_range (app->buffer, &mstart, &mend);
            app->last_pos = gtk_text_buffer_create_mark (app->buffer, "last_pos",
                                          app->optback ? &mstart : &mend, FALSE);

            /* flags checking whether match is at word start/end */
            gboolean word_start = gtk_text_iter_starts_word (&mstart),
                    word_end = gtk_text_iter_ends_word (&mend);

            /* handle whole-word search option */
            if (app->optwhole) {
                if (!word_start || !word_end) { /* not word start/end */
                    /* place cursor at end to cancel select-highlight */
                    gtk_text_buffer_place_cursor (app->buffer, &mend);
                    continue;   /* find next text match */
                }
            }

            app->txtfound = TRUE;   /* match found satisfying options */

            /* scroll window to mark to insure match is visible */
#ifdef TOMARK
            gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                        app->last_pos, 0.0, TRUE, 0.95, 0.8);
#else
            gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
                                                app->last_pos);
#endif
            break;
        }
        else {  /* 'text' not found in buffer */
            app->txtfound = FALSE;
            /* TODO: handle no match/last-term, e.g. no new app->last_pos
             * (dialog "End reached, continue at beginning?")
             * (restore existing selection if no match, selstart/selend)
             */
            break;
        }
    }
}

void btnfind_activate (GtkWidget *widget, context *app)
{
    guint i;

    /* get find & replace entries */
    gchar *findtext    = gtk_combo_box_text_get_active_text (
                            GTK_COMBO_BOX_TEXT(app->entryfind));

    /* add new terms to find/replace lists (no dups) */
    for (i = 0; i < app->nfentries; i++)
        if (g_strcmp0 (app->findtext[i], findtext) == 0) goto fdup;

    /* add to array of entries  & increment indexes */
    app->findtext[app->nfentries++] = findtext;
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), findtext);

  fdup:

    chk_realloc_ent (app);  /* check/realloc find/rep text */

    find (app, findtext);

    if (widget) {}
}

void btnreplace_activate (GtkWidget *widget, context *app)
{
    guint i;

    /* get find & replace entries */
    gchar *findtext    = gtk_combo_box_text_get_active_text (
                            GTK_COMBO_BOX_TEXT(app->entryfind));
    gchar *replacetext = gtk_combo_box_text_get_active_text (
                            GTK_COMBO_BOX_TEXT(app->entryreplace));

    /* add new terms to find/replace lists (no dups) */
    for (i = 0; i < app->nfentries; i++)
        if (g_strcmp0 (app->findtext[i], findtext) == 0) goto fdup;

    /* add to array of entries  & increment indexes */
    app->findtext[app->nfentries++] = findtext;     /* TODO skip dups */
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), findtext);

  fdup:
    for (i = 0; i < app->nrentries; i++)
        if (g_strcmp0 (app->reptext[i], replacetext) == 0) goto rdup;
    /* add to array of entries  & increment indexes */
    app->reptext[app->nrentries++] = replacetext;   /* TODO skip dups */
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryreplace), replacetext);

  rdup:

    chk_realloc_ent (app);  /* check/realloc find/rep text */

    if (app->optprompt) {   /* if option prompt_on_replace do once */
        find (app, findtext);

        /* delete text between input/select marks, replace with replacetext */
        if (app->txtfound) {

            /* TODO: add dialog prompting for replace, or maybe separate
             * [find] [replace] buttons on dialog
             */
            buffer_replace_selection (app, replacetext);
        }
    }
    else {                  /* otherwise, replace all occurrences */
        for (;;) {
            find (app, findtext);
            if (app->txtfound)
                buffer_replace_selection (app, replacetext);
            else
                break;
        }
    }

    if (widget) {}
}

void btnclose_activate (GtkWidget *widget, context *app)
{
    /* free app mem - destruct here */
    app->txtfound = FALSE;  /* reset found & last_pos */
    if (app->last_pos)
        gtk_text_buffer_delete_mark (app->buffer, app->last_pos);
    app->last_pos = NULL;
    app->findcbchgd = FALSE;
    app->replcbchgd = FALSE;

    if (app->selstart) {    /* delete marks used during search */
        gtk_text_buffer_delete_mark (app->buffer, app->selstart);
        app->selstart = NULL;   /* must be set NULL for next chk */
    }
    if (app->selend) {
        gtk_text_buffer_delete_mark (app->buffer, app->selend);
        app->selend = NULL;
    }

    app->dlgid = 0; /* reset dialog id to default */

    /* call common gtk_widget_destroy (could move all there) */
    gtk_widget_destroy (app->findrepwin);
    if (app) {}
    if (widget) {}
}

void chk_realloc_ent (context *app)
{
    /* check/realloc find/rep text */
    if (app->nfentries == app->fmax) {
        gchar **tmp = g_realloc (app->findtext,
                                 2 * app->nfentries * sizeof *(app->findtext));
        if (!tmp) {
            err_dialog ("btnreplace_activate()\nvirtual memory exhausted.");
            // findrep_destroy (app); /* TODO: graceful exit */
            // context_destroy (app);
            gtk_main_quit();
        }
        app->fmax += MAXLE;
    }

    if (app->nrentries == app->rmax) {
        gchar **tmp = g_realloc (app->reptext,
                                 2 * app->nrentries * sizeof *(app->reptext));
        if (!tmp) {
            err_dialog ("btnreplace_activate()\nvirtual memory exhausted.");
            // findrep_destroy (app); /* TODO: graceful exit */
            // context_destroy (app);
            gtk_main_quit();
        }
        app->rmax += MAXLE;
    }
}

void dumpopts (context *app)
{
    g_print ("\n checkbox states:\n\n");
    g_print ("  optregex  : %s\n", app->optregex  ? "true" : "false");
    g_print ("  optplace  : %s\n", app->optplace  ? "true" : "false");
    g_print ("  optcase   : %s\n", app->optcase   ? "true" : "false");
    g_print ("  optwhole  : %s\n", app->optwhole  ? "true" : "false");
    g_print ("  optfrom   : %s\n", app->optfrom   ? "true" : "false");
    g_print ("  optback   : %s\n", app->optback   ? "true" : "false");
    g_print ("  optselect : %s\n", app->optselect ? "true" : "false");
    g_print ("  optprompt : %s\n", app->optprompt ? "true" : "false");
}

/* find replace keypress handler */
/* TODO: add handling by combobox and skip handler? (see snip below) */
static gboolean on_fr_keypress (GtkWidget *widget, GdkEventKey *event,
                                context *app)
{
    switch (event->keyval)
    {
        case GDK_KEY_Escape:
            btnclose_activate (widget, app);
            // return TRUE;    /* return TRUE - no further processing */
            break;
        case GDK_KP_Enter:
            if (app->dlgid == DLGFIND && app->findcbchgd)
                btnfind_activate (widget, app);
            else if (app->dlgid == DLGREPL && app->findcbchgd && app->replcbchgd)
                btnreplace_activate (widget, app);
            return TRUE;
        case GDK_Return:
            if (app->dlgid == DLGFIND && app->findcbchgd)
                btnfind_activate (widget, app);
            else if (app->dlgid == DLGREPL && app->findcbchgd && app->replcbchgd)
                btnreplace_activate (widget, app);
            return TRUE;
    }

    return FALSE;
}

/* snip - example for text-view handling, that may be something we can do
   for combobox

static gboolean
gtk_foo_bar_key_press_event (GtkWidget   *widget,
                             GdkEventKey *event)
{
  if ((key->keyval == GDK_Return || key->keyval == GDK_KP_Enter))
    {
      if (gtk_text_view_im_context_filter_keypress (GTK_TEXT_VIEW (view), event))
        return TRUE;
    }
    // Do some stuff

  return GTK_WIDGET_CLASS (gtk_foo_bar_parent_class)->key_press_event (widget, event);
}

*/

/** delete text between input/select marks, making last_pos the same as
 *  insert_mark using last_pos to create iter, then insert replacetext.
 */
void buffer_replace_selection (context *app, const gchar *replacetext)
{
    GtkTextIter iter;

    gtk_text_buffer_delete_selection (app->buffer, FALSE, TRUE);
    gtk_text_buffer_get_iter_at_mark (app->buffer, &iter, app->last_pos);
    gtk_text_buffer_insert (app->buffer, &iter, replacetext, -1);
}
