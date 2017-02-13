#include "gtk_settings.h"
#include "gtk_common_dlg.h"

GtkWidget *create_settings_dlg (context *app)
{
    /* variables */
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *notebook;        /* notebook in vbox */
    GtkWidget *vboxnb;          /* vboxnb in each notebook page */
    GtkWidget *frame;           /* frames to section vboxnb */
    GtkWidget *table;           /* table for each frame */
    GtkWidget *hbtweak;
    GtkWidget *label;
    GtkWidget *hbox;            /* hbox container   */
    GtkWidget *btnok;
    GtkWidget *btncancel;
    GtkWidget *fontbtn;
    GtkWidget *chkdynwrap;
    GtkWidget *chkshowdwrap;
    GtkWidget *chkbox;

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

    /* Create a new notebook, place the position of the tabs left */
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);

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
    // gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
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
    // gtk_container_add (GTK_CONTAINER (frame), label);
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
    // gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
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
    gtk_widget_show (chkshowdwrap);

    /* pack frame into notebook vboxnb */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page - cursor & selection */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Cursor & Selection");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    // gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
    gtk_widget_show (frame);

    /* table inside frame */
    table = gtk_table_new (3, 2, TRUE);
    gtk_table_set_row_spacings (GTK_TABLE (table), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table), 3);
    gtk_container_set_border_width (GTK_CONTAINER (table), 5);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_widget_show (table);

    /* options checkboxs */
    chkbox = gtk_check_button_new_with_mnemonic ("_Smart home & end movement");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkbox), app->smarthe);
    gtk_table_attach_defaults (GTK_TABLE (table), chkbox, 0, 1, 0, 1);
    gtk_widget_show (chkbox);

    chkbox = gtk_check_button_new_with_mnemonic ("Wrap _cursor to next line");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkbox), app->wraptxtcsr);
    gtk_table_attach_defaults (GTK_TABLE (table), chkbox, 0, 1, 1, 2);
    gtk_widget_show (chkbox);

    chkbox = gtk_check_button_new_with_mnemonic ("PgUp/PgDn _moves text cursor");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chkbox), app->pgudmvscsr);
    gtk_table_attach_defaults (GTK_TABLE (table), chkbox, 0, 1, 2, 3);
    gtk_widget_show (chkbox);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* label for tab */
    label = gtk_label_new ("Appearance");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
    gtk_widget_show (vboxnb);

    /* X - setting page */
    vboxnb = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vboxnb, pgwidth, pgheight);

    /* frame within page */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Tabulators");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    // gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
    gtk_widget_show (frame);

    /* stuff in frame */
    label = gtk_label_new ("Insert spaces snstead of tabs");
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_widget_show (label);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page */
    frame = gtk_frame_new ("Indentation Rules");
    gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
    gtk_widget_show (frame);

    /* stuff in frame */
    label = gtk_label_new ("Insert spaces instead of tabs to indent");
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_widget_show (label);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* label for tab */
    label = gtk_label_new ("Editing");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
    gtk_widget_show (vboxnb);

    gtk_widget_show (notebook);     /* show the notebook within vbox */

    /* hbox (replacing table) button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    btnok = gtk_button_new_with_mnemonic ("_Ok");
    gtk_widget_set_size_request (btnok, 80, 24);
    // gtk_box_pack_end (GTK_BOX (hbox), btnok, FALSE, FALSE, 0); // (moved after btncancel)
    // gtk_widget_set_sensitive (btnok, app->findcbchgd);
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


    gtk_widget_show (app->settingswin); /* show the window */

    return (app->settingswin);
}

void settings_btncancel (GtkWidget *widget, context *app)
{
    gtk_widget_destroy (app->settingswin);
    if (app) {}
    if (widget) {}
}

