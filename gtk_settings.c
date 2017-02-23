#include "gtk_settings.h"
#include "gtk_common_dlg.h"

/* settings keypress handler */
static gboolean on_settings_keypress (GtkWidget *widget, GdkEventKey *event,
                                      context *app)
{
    switch (event->keyval)
    {
        case GDK_KEY_Escape:    /* close dialog if user presses ESC */
            settings_btncancel (widget, app);
            break;
    }

    return FALSE;   /* TRUE - no further processing */
}

GtkWidget *create_settings_dlg (context *app)
{
    /* variables */
    GtkWidget *vbox;            /* vbox - main container   */
    GtkWidget *notebook;        /* notebook in vbox */
    GtkWidget *vboxnb;          /* vboxnb - vbox in each notebook page */
    GtkWidget *frame;           /* frames to section vboxnb */
    GtkWidget *table;           /* table for each frame */
    GtkWidget *hbtweak;         /* hbox to left-justify table element */
    GtkWidget *label;           /* generic label */
    GtkWidget *hbox;            /* hbox container   */
    GtkWidget *btnok;           /* button OK */
    GtkWidget *btncancel;       /* button Cancel */
    GtkWidget *fontbtn;         /* Font Button */
    GtkWidget *chkdynwrap;      /* checkbox - dynamic word wrap */
    GtkWidget *chkshowdwrap;    /* checkbox - show wrap markers */
    GtkWidget *chksmarthe;      /* checkbox - smart home/end */
    GtkWidget *chkwraptxtcsr;   /* checkbox - wrap text cursor */
    GtkWidget *chkpgudmvscsr;   /* checkbox - PgUp/PgDn moves cursor */
    GtkWidget *chkexpandtab;    /* checkbox - insert spaces for tab */
    GtkWidget *chksmartbs;      /* checkbox - smart backspace */
    GtkWidget *chkshowtabs;     /* checkbox - show tab markers */
    GtkWidget *spintab;         /* spinbutton - tabstop */
    GtkWidget *chkindentwspc;   /* checkbox - indent with spaces not tab */
    GtkWidget *chkindentauto;   /* checkbox - auto-indent new line */
    GtkWidget *chkindentmixd;   /* checkbox - Emacs mode mixed spaces/tabs */
    GtkWidget *spinindent;      /* spinbutton - indent width (softtab) */
    GtkWidget *chktrimendws;    /* checkbox - remove trailing whitespace */
    GtkWidget *chkposixeof;     /* checkbox - require POSIX end of file */
#ifdef HAVESOURCEVIEW
    GtkWidget *chklinehghlt;    /* checkbox - show current line highlight */
#endif

    GtkObject *adjtab;          /* adjustment - tab spinbutton */
    GtkObject *adjind;          /* adjustment - indent spinbutton */

    gint wnwidth  = 480;    /* initial dialog width and height */
    gint wnheight = 470;
    gint pgwidth  = 400;    /* initial notebook page width and height */
    gint pgheight = 400;    /* (enforced by vboxnb width/height) */

    /* create toplevel window */
    if (!(app->settingswin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_settings_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->settingswin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->settingswin), wnwidth, wnheight);
    gtk_window_set_title (GTK_WINDOW (app->settingswin), "GtkWrite Settings");
    /* for inclusion in main app set modal and transient_for (to keep on top)
     * gtk_window_set_modal (GtkWindow *window, gboolean modal);
     * gtk_window_set_transient_for(GtkWindow *window, GtkWindow *parent)
     */
    gtk_window_set_modal (GTK_WINDOW(app->settingswin), TRUE);
    gtk_window_set_transient_for (GTK_WINDOW(app->settingswin),
                                    GTK_WINDOW(app->window));
    gtk_container_set_border_width (GTK_CONTAINER (app->settingswin), 5);
    g_signal_connect (app->settingswin, "destroy",
		      G_CALLBACK (settings_btncancel), app);

    /* main vbox container */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->settingswin), vbox);

    /* title over notebook, TODO: save label and change on page sel */
    label = gtk_label_new ("Settings Are Under Construction!");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    /* Create a new notebook, place the position of the tabs left,
     * set tab-hborder and tab-vborder independently, context menu
     * of tabs currently not currently enabled. (see notes below)
     */
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);
    /* set notebook tab border with g_object_set
     * (gtk_notebook_set_tab_[hv]border is deprecated)
     *
     * set both h & v borders to a single value:
     * g_object_set (GTK_NOTEBOOK (notebook), "tab-border", 4, NULL);
     *
     * set hborder and vborder independently:
     */
    g_object_set (GTK_NOTEBOOK (notebook), "tab-vborder", 5,
                                           "tab-hborder", 4, NULL);
    /* enable context menu showing all tabs if user rt-clicks on tab
     * (not currently needed)
     *
     * gtk_notebook_popup_enable (GTK_NOTEBOOK (notebook));
     *
     * if enabled, you must replace:
     *   gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
     * with:
     *   gtk_notebook_append_page_menu (GTK_NOTEBOOK (notebook), vboxnb, label, NULL);
     * (already commented in code below)
     */

    gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);

    /* appearance - setting page */
    vboxnb = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vboxnb, pgwidth, pgheight);

    /* frame within page - font */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Font Selection");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame - font */
    table = gtk_table_new (1, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* select font label is element 1 */
    label = gtk_label_new ("Select Font:");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 0, 1);
    gtk_widget_show (hbtweak);
    gtk_widget_show (label);

    /* font button calling font selection dialog */
    fontbtn = gtk_font_button_new_with_font (app->fontname);
    gtk_font_button_set_show_style (GTK_FONT_BUTTON(fontbtn), TRUE);
    gtk_font_button_set_show_size (GTK_FONT_BUTTON(fontbtn), TRUE);
    gtk_font_button_set_title (GTK_FONT_BUTTON(fontbtn),
                                "Select Editor Font & Size");
    gtk_table_attach_defaults (GTK_TABLE (table), fontbtn, 1, 2, 0, 1);
    gtk_widget_show (fontbtn);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page - word wrap */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Word Wrap");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (2, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chkdynwrap = gtk_check_button_new_with_mnemonic ("_Dynamic word wrap");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkdynwrap), app->dynwrap);
    gtk_table_attach_defaults (GTK_TABLE (table), chkdynwrap, 0, 1, 0, 1);
    gtk_widget_show (chkdynwrap);

    chkshowdwrap = gtk_check_button_new_with_mnemonic ("Show _word wrap indicators");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkshowdwrap), app->showdwrap);
    gtk_table_attach_defaults (GTK_TABLE (table), chkshowdwrap, 0, 1, 1, 2);
    /* disable checkboxs until implemented? */
    // gtk_widget_set_sensitive (chkshowdwrap, FALSE);
    gtk_widget_show (chkshowdwrap);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page - cursor & selection */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Cursor & Selection");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chksmarthe = gtk_check_button_new_with_mnemonic ("_Smart home & end movement");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chksmarthe), app->smarthe);
    gtk_table_attach_defaults (GTK_TABLE (table), chksmarthe, 0, 1, 0, 1);
    gtk_widget_show (chksmarthe);

    chkwraptxtcsr = gtk_check_button_new_with_mnemonic ("Wrap _cursor to next line");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkwraptxtcsr), app->wraptxtcsr);
    gtk_table_attach_defaults (GTK_TABLE (table), chkwraptxtcsr, 0, 1, 1, 2);
    gtk_widget_show (chkwraptxtcsr);

    chkpgudmvscsr = gtk_check_button_new_with_mnemonic ("PgUp/PgDn _moves text cursor");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkpgudmvscsr), app->pgudmvscsr);
    gtk_table_attach_defaults (GTK_TABLE (table), chkpgudmvscsr, 0, 1, 2, 3);
    gtk_widget_show (chkpgudmvscsr);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

#ifdef HAVESOURCEVIEW
    /* frame within page - cursor & selection */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Visual Aids");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (1, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chklinehghlt = gtk_check_button_new_with_mnemonic ("_Highlight current line");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chklinehghlt), app->linehghlt);
    gtk_table_attach_defaults (GTK_TABLE (table), chklinehghlt, 0, 1, 0, 1);
    gtk_widget_show (chklinehghlt);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);
#endif

    /* label for tab */
    /* TODO: set tab borders (hborder/vborder, etc..) */
    label = gtk_label_new ("Appearance");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
    /* use if gtk_notebook_popup_enable () set above */
    // gtk_notebook_append_page_menu (GTK_NOTEBOOK (notebook), vboxnb, label, NULL);
    gtk_widget_show (vboxnb);

    /* editing - setting page */
    vboxnb = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vboxnb, pgwidth, pgheight);

    /* frame within page */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Tabulators");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (4, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chkexpandtab = gtk_check_button_new_with_mnemonic ("_Insert spaces instead of tab");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkexpandtab), app->expandtab);
    gtk_table_attach_defaults (GTK_TABLE (table), chkexpandtab, 0, 1, 0, 1);
    gtk_widget_show (chkexpandtab);

    chksmartbs = gtk_check_button_new_with_mnemonic ("Smart _backspace (to tab)");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chksmartbs), app->smartbs);
    gtk_table_attach_defaults (GTK_TABLE (table), chksmartbs, 0, 1, 1, 2);
    gtk_widget_show (chksmartbs);

    chkshowtabs = gtk_check_button_new_with_mnemonic ("_Show tab markers");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkshowtabs), app->showtabs);
    gtk_table_attach_defaults (GTK_TABLE (table), chkshowtabs, 0, 1, 2, 3);
    gtk_widget_show (chkshowtabs);

    label = gtk_label_new ("Set tab size (spaces):");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 3, 4);
    gtk_widget_show (hbtweak);
    gtk_widget_show (label);

    /* value, lower, upper, step_increment, page_increment, page_size
     * (as with statusbar, the value is line + 1)
     */
    adjtab = gtk_adjustment_new (app->tabstop, 1.0, 80.0, 1.0, 4.0, 0.0);
    spintab = gtk_spin_button_new (GTK_ADJUSTMENT(adjtab), 1.0, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), spintab, 1, 2, 3, 4);
    gtk_widget_show (spintab);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Indentation Rules");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (4, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chkindentwspc = gtk_check_button_new_with_mnemonic ("_Use spaces to indent");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkindentwspc), app->indentwspc);
    gtk_table_attach_defaults (GTK_TABLE (table), chkindentwspc, 0, 1, 0, 1);
    gtk_widget_show (chkindentwspc);

    chkindentauto = gtk_check_button_new_with_mnemonic ("_Auto-indent new lines");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkindentauto), app->indentauto);
    gtk_table_attach_defaults (GTK_TABLE (table), chkindentauto, 0, 1, 1, 2);
    gtk_widget_show (chkindentauto);

    chkindentmixd = gtk_check_button_new_with_mnemonic ("_Emacs style mixed mode");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkindentmixd), app->indentmixd);
    gtk_table_attach_defaults (GTK_TABLE (table), chkindentmixd, 0, 1, 2, 3);
    gtk_widget_show (chkindentmixd);

    label = gtk_label_new ("Set indent size (spaces):");
    hbtweak = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbtweak), label, FALSE, FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), hbtweak, 0, 1, 3, 4);
    gtk_widget_show (hbtweak);
    gtk_widget_show (label);

    /* value, lower, upper, step_increment, page_increment, page_size
     * (page_size other than 0.0 is deprecated)
     */
    adjind = gtk_adjustment_new (app->softtab, 1.0, 80.0, 1.0, 2.0, 0.0);
    spinindent = gtk_spin_button_new (GTK_ADJUSTMENT(adjind), 1.0, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), spinindent, 1, 2, 3, 4);
    gtk_widget_show (spinindent);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Automatic Cleanups on Load/Save");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (2, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chktrimendws = gtk_check_button_new_with_mnemonic ("_Remove trailing spaces");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chktrimendws), app->trimendws);
    gtk_table_attach_defaults (GTK_TABLE (table), chktrimendws, 0, 1, 0, 1);
    gtk_widget_show (chktrimendws);

    chkposixeof = gtk_check_button_new_with_mnemonic ("Require _POSIX end-of-file");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkposixeof), app->posixeof);
    gtk_table_attach_defaults (GTK_TABLE (table), chkposixeof, 0, 1, 1, 2);
    gtk_widget_show (chkposixeof);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* label for tab */
    /* TODO: set tab borders (hborder/vborder, etc..) */
    label = gtk_label_new ("Editing");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
    /* use if gtk_notebook_popup_enable () set above */
    // gtk_notebook_append_page_menu (GTK_NOTEBOOK (notebook), vboxnb, label, NULL);
    gtk_widget_show (vboxnb);

    gtk_widget_show (notebook);     /* show the notebook within vbox */

    /* hbox (replacing table) button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    btnok = gtk_button_new_with_mnemonic ("_Ok");
    gtk_widget_set_size_request (btnok, 80, 24);
    gtk_widget_show (btnok);

    btncancel = gtk_button_new_with_mnemonic ("_Cancel");
    gtk_widget_set_size_request (btncancel, 80, 24);

    gtk_box_pack_end (GTK_BOX (hbox), btncancel, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), btnok, FALSE, FALSE, 0); // pack on rt-side.
    gtk_widget_show (btncancel);

    gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    gtk_widget_show (vbox);         /* make all widgets visible */

    /* settings callbacks - TODO: add remaining callbacks */
    g_signal_connect (btncancel, "clicked",
                      G_CALLBACK (settings_btncancel), app);

    g_signal_connect (btnok, "clicked",
                      G_CALLBACK (settings_btnok), app);

    g_signal_connect (fontbtn, "font-set",
                      G_CALLBACK (settings_fontbtn), app);

    g_signal_connect (chkdynwrap, "toggled",
                      G_CALLBACK (chkdynwrap_toggled), app);

    g_signal_connect (chkshowdwrap, "toggled",
                      G_CALLBACK (chkshowdwrap_toggled), app);

    g_signal_connect (chksmarthe, "toggled",
                      G_CALLBACK (chksmarthe_toggled), app);

    g_signal_connect (chkwraptxtcsr, "toggled",
                      G_CALLBACK (chkwraptxtcsr_toggled), app);

    g_signal_connect (chkpgudmvscsr, "toggled",
                      G_CALLBACK (chkpgudmvscsr_toggled), app);

#ifdef HAVESOURCEVIEW
    g_signal_connect (chklinehghlt, "toggled",
                      G_CALLBACK (chklinehghlt_toggled), app);

#endif
    g_signal_connect (chkexpandtab, "toggled",
                      G_CALLBACK (chkexpandtab_toggled), app);

    g_signal_connect (chksmartbs, "toggled",
                      G_CALLBACK (chksmartbs_toggled), app);

    g_signal_connect (chkshowtabs, "toggled",
                      G_CALLBACK (chkshowtabs_toggled), app);

    g_signal_connect (spintab, "value-changed",
                      G_CALLBACK (spintab_changed), app);

    g_signal_connect (chkindentwspc, "toggled",
                      G_CALLBACK (chkindentwspc_toggled), app);

    g_signal_connect (chkindentauto, "toggled",
                      G_CALLBACK (chkindentauto_toggled), app);

    g_signal_connect (chkindentmixd, "toggled",
                      G_CALLBACK (chkindentmixd_toggled), app);

    g_signal_connect (spinindent, "value-changed",
                      G_CALLBACK (spinindent_changed), app);

    g_signal_connect (chktrimendws, "toggled",
                      G_CALLBACK (chktrimendws_toggled), app);

    g_signal_connect (chkposixeof, "toggled",
                      G_CALLBACK (chkposixeof_toggled), app);

    g_signal_connect (app->settingswin, "key_press_event",
                      G_CALLBACK (on_settings_keypress), app);

    gtk_widget_show (app->settingswin); /* show the window */

    return (app->settingswin);
}

void settings_btncancel (GtkWidget *widget, context *app)
{
    gtk_widget_destroy (app->settingswin);
    if (app) {}
    if (widget) {}
}

void settings_btnok (GtkWidget *widget, context *app)
{
    gtk_widget_destroy (app->settingswin);
    if (app) {}
    if (widget) {}
}

void settings_fontbtn (GtkWidget *widget, context *app)
{
    const gchar *newfont = gtk_font_button_get_font_name (GTK_FONT_BUTTON(widget));
    PangoFontDescription *font_desc;

    if (!newfont) {
        err_dialog ("error: invalid font returned.");
        return;
    }

    if (app->fontname) g_free (app->fontname);

    if (!(app->fontname = g_strdup (newfont))) {
        err_dialog ("error:\ncopy of newfont to app->fontname failed.");
        return;
    }

    font_desc = pango_font_description_from_string (app->fontname);

    gtk_widget_modify_font (app->view, font_desc);
    pango_font_description_free (font_desc);
}

void chkdynwrap_toggled (GtkWidget *widget, context *app)
{
    app->dynwrap = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chkshowdwrap_toggled (GtkWidget *widget, context *app)
{
    app->showdwrap = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chksmarthe_toggled (GtkWidget *widget, context *app)
{
    app->smarthe = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
#ifdef HAVESOURCEVIEW
    if (app->smarthe)
        gtk_source_view_set_smart_home_end (GTK_SOURCE_VIEW(app->view),
                                        GTK_SOURCE_SMART_HOME_END_BEFORE);
    else
        gtk_source_view_set_smart_home_end (GTK_SOURCE_VIEW(app->view),
                                        GTK_SOURCE_SMART_HOME_END_DISABLED);
#endif
}

void chkwraptxtcsr_toggled (GtkWidget *widget, context *app)
{
    app->wraptxtcsr = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chkpgudmvscsr_toggled (GtkWidget *widget, context *app)
{
    app->pgudmvscsr = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

#ifdef HAVESOURCEVIEW
void chklinehghlt_toggled (GtkWidget *widget, context *app)
{
    app->linehghlt = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
    gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW(app->view),
                                                app->linehghlt);
}
#endif

void chkexpandtab_toggled (GtkWidget *widget, context *app)
{
    app->expandtab = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chksmartbs_toggled (GtkWidget *widget, context *app)
{
    app->smartbs = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chkshowtabs_toggled (GtkWidget *widget, context *app)
{
    app->showtabs = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void spintab_changed (GtkWidget *widget, context *app)
{
    app->tabstop = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget));
}

void chkindentwspc_toggled (GtkWidget *widget, context *app)
{
    app->indentwspc = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chkindentauto_toggled (GtkWidget *widget, context *app)
{
    app->indentauto = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
#ifdef HAVESOURCEVIEW
    if (gtk_source_view_get_auto_indent (GTK_SOURCE_VIEW(app->view)) &&
        !app->indentauto)
        gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW(app->view), app->indentauto);
    else if (!gtk_source_view_get_auto_indent (GTK_SOURCE_VIEW(app->view)) &&
        app->indentauto)
        gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW(app->view), app->indentauto);
#endif
}

void chkindentmixd_toggled (GtkWidget *widget, context *app)
{
    app->indentmixd = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void spinindent_changed (GtkWidget *widget, context *app)
{
    app->softtab = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget));
}

void chktrimendws_toggled (GtkWidget *widget, context *app)
{
    app->trimendws = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

void chkposixeof_toggled (GtkWidget *widget, context *app)
{
    app->posixeof = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}
