// #include "gtk_windef.h"
#include "gtk_findreplace.h"

GtkWidget *create_find_dlg (context *app/*, context *mainwin*/)
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
    // gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkregex), TRUE);
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

    app->chkselect = gtk_check_button_new_with_mnemonic ("_Selected text");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkselect), app->optselect);
    gtk_table_attach_defaults (GTK_TABLE (table2), app->chkselect, 1, 2, 1, 2);
    gtk_widget_show (app->chkselect);

    gtk_widget_show (table2);

    /* hbox (replacing table) button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    btnfind = gtk_button_new_with_mnemonic ("_Find");
    gtk_widget_set_size_request (btnfind, 80, 24);
    // gtk_box_pack_end (GTK_BOX (hbox), btnfind, FALSE, FALSE, 0); // (moved after btnclose)
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
    // gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkregex), FALSE);
    gtk_table_attach_defaults (GTK_TABLE (table), app->chkregex, 0, 1, 2, 3);

    /* regex edit button - set active state FALSE - gray Edit... button */
    app->btnregex = gtk_button_new_with_mnemonic ("_Edit...");
    gtk_widget_set_size_request (app->btnregex, 80, 24);
    // if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkregex)))
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
//     if (app->optwhole)
//         gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (app->chkwhole), TRUE);
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
    gtk_widget_set_size_request (btnreplace, 80, 24);
    // gtk_box_pack_end (GTK_BOX (hbox), btnreplace, FALSE, FALSE, 0); // (moved after btnclose)
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

    app->txtfound   = FALSE;
    app->last_pos   = NULL;

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

/* entry comboboxes */
void entry_find_activate (GtkWidget *widget, context *app) {
    if (app) {}
    if (widget) {}
}

void entry_replace_activate (GtkWidget *widget, context *app) {
    if (app) {}
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

/* scrolling to mark */
/*

void gtk_text_view_scroll_to_mark( GtkTextView *text_view,
                                   GtkTextMark *mark,
                                   gdouble within_margin,
                                   gboolean use_align,
                                   gdouble xalign,
                                   gdouble yalign );

*/
void find (context *app, const gchar *text, GtkTextIter *iter)
{
    GtkTextIter mstart, mend;
    gboolean found;
    GtkTextBuffer *buffer;
    // GtkTextMark *last_pos;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));
    found = gtk_text_iter_forward_search (iter, text, 0,
                                          &mstart, &mend, NULL);

    if (found)
    {
        app->txtfound = TRUE;
        gtk_text_buffer_select_range (buffer, &mstart, &mend);
        app->last_pos = gtk_text_buffer_create_mark (buffer, "last_pos",
                                                    &mend, FALSE);
#ifdef TOMARK
        /* TODO add within_margin and xalign, yalign as config settings? */
        gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                      app->last_pos, 0.0, TRUE, 0.95, 0.8);
        // gtk_text_view_scroll_to_mark (text_view, last_pos, 0.2, FALSE, 0.95, 0.8);
#else
        gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
                                            app->last_pos);
#endif
    }
    else {
        app->txtfound = FALSE;
        /* need flag for last-term, e.g. no new app->last_pos
         * (looks like as is may work)
         */
    }
}

void btnfind_activate (GtkWidget *widget, context *app)
{
    GtkTextBuffer *buffer;
    GtkTextIter iter;

    guint i;
/* options to make use of
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkregex))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkplace))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkcase))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkwhole))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkfrom))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkback))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkselect))) {}
*/
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

    /* TEST dump findtext values to stdout */
    // g_print ("\n findtext[%2u]     : %s\n", app->nfentries, findtext);

    chk_realloc_ent (app);  /* check/realloc find/rep text */

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    if (!app->last_pos)         /* find first occurrence */
        gtk_text_buffer_get_start_iter (buffer, &iter);
    else if (app->txtfound)     /* find next occurrence  */
        gtk_text_buffer_get_iter_at_mark (buffer, &iter, app->last_pos);
    else            /* not found or at last term, for now, just return */
        return;     /* (you could reset here (app->last_pos) and start over) */

    find (app, findtext, &iter);

    if (widget) {}
}

void btnreplace_activate (GtkWidget *widget, context *app)
{

    guint i;
/*
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkregex))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkplace))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkcase))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkwhole))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkfrom))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkback))) {}
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (app->chkselect))) {}
*/
    /* get find & replace entries */
    gchar *findtext    = gtk_combo_box_text_get_active_text (
                            GTK_COMBO_BOX_TEXT(app->entryfind));
    gchar *replacetext = gtk_combo_box_text_get_active_text (
                            GTK_COMBO_BOX_TEXT(app->entryreplace));

    /* add new terms to find/replace lists (no dups) */
    for (i = 0; i < app->nfentries; i++)
        if (g_strcmp0 (app->findtext[i], findtext) == 0) goto fdup;

    /* add to array of entries  & increment indexes */
    app->findtext[app->nfentries++] = findtext;
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryfind), findtext);

  fdup:
    for (i = 0; i < app->nrentries; i++)
        if (g_strcmp0 (app->reptext[i], replacetext) == 0) goto rdup;
    /* add to array of entries  & increment indexes */
    app->reptext[app->nrentries++] = replacetext;
    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->entryreplace), replacetext);

  rdup:

    /* TEST print */
    g_print ("\n findtext[%2u]     : %s\n", app->nfentries, findtext);
    g_print (" replacetext[%2u]  : %s\n", app->nrentries, replacetext);

    chk_realloc_ent (app);  /* check/realloc find/rep text */

    if (widget) {}
}

void btnclose_activate   (GtkWidget *widget, context *app)
{
    /* free app mem - destruct here */
    // gtk_main_quit();
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
}

