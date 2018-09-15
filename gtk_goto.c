#include "gtk_goto.h"

/* globals to avoid storage in kwinst */
static GtkWidget *gotowin;
static GtkWidget *spinbtn;
static GtkTextMark *new_pos;

/* goto keypress handler */
static gboolean on_goto_keypress (GtkWidget *widget, GdkEventKey *event,
                                  kwinst *app)
{
    switch (event->keyval)
    {
        case GDK_KEY_Escape:
            goto_btnclose (widget, app);
            // return TRUE;    /* return TRUE - no further processing */
            break;
        case GDK_KP_Enter:  /* toggle focus to set value in spinbtn */
            gtk_widget_child_focus (gotowin, GTK_DIR_RIGHT);
            gtk_widget_child_focus (gotowin, GTK_DIR_LEFT);
            goto_btnfind (widget, app);
            return TRUE;
        case GDK_Return:
            gtk_widget_child_focus (gotowin, GTK_DIR_RIGHT);
            gtk_widget_child_focus (gotowin, GTK_DIR_LEFT);
            goto_btnfind (widget, app);
            goto_btnclose (widget, app);
            return TRUE;
    }

    return FALSE;
}

GtkWidget *create_goto_dlg (kwinst *app)
{
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *adjhbox;
    GtkWidget *adjvbox;
    GtkWidget *hbox;
    GtkWidget *image;
    GtkWidget *btnfind;
    GtkWidget *btnclose;
    GtkWidget *label;
    GtkWidget *vscale;

    gint line = app->line;
    // gint line = gtk_text_iter_get_line (iter);
    gint last = gtk_text_buffer_get_line_count (GTK_TEXT_BUFFER(app->buffer));

    /* initialize persistent variables */
    new_pos = NULL;

    /* value, lower, upper, step_increment, page_increment, page_size
     * (as with statusbar, the value is line + 1)
     */
    GtkObject *adj = gtk_adjustment_new (line+1, 0.0, last, 1.0, 1.0, 0.0);
    gchar *str = "\nKeyboard Shortcuts:\n<b>Enter</b> - goto/close dlg.\n<b>KP_Enter</b> - multi-goto.";

    if (last == 1) {
        show_info_bar_markup_ok ("insufficient number of lines in buffer (nowhere to go to).",
                                GTK_MESSAGE_INFO, app);
        // err_dialog ("Error:\n\ninsufficient lines in buffer.");
        return NULL;
    }

    /* create toplevel window */
    if (!(gotowin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        show_info_bar_markup_ok ("create_find_dlg() gtk_window_new failure.",
                                GTK_MESSAGE_ERROR, app);
        // err_dialog ("create_find_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (gotowin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (gotowin), 185, 185);
    gtk_window_set_title (GTK_WINDOW (gotowin), "Goto Line");
    gtk_window_set_modal (GTK_WINDOW(gotowin), TRUE);
    gtk_window_set_transient_for (GTK_WINDOW(gotowin),
                                    GTK_WINDOW(app->window));
    gtk_container_set_border_width (GTK_CONTAINER (gotowin), 5);
    g_signal_connect (gotowin, "destroy",
		      G_CALLBACK (goto_btnclose), app);

    /* main vbox container
     * spacing profided on frames as containers */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (gotowin), vbox);
    gtk_widget_show (vbox);

    adjhbox = gtk_hbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (vbox), adjhbox);

    adjvbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (adjhbox), adjvbox);

    image = gtk_image_new_from_stock (GTK_STOCK_INDEX, GTK_ICON_SIZE_DND);
    gtk_box_pack_start (GTK_BOX (adjvbox), image, FALSE, FALSE, 0);
    gtk_widget_show (image);

    label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL(label), str);
    gtk_label_set_justify (GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start (GTK_BOX (adjvbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    spinbtn = gtk_spin_button_new (GTK_ADJUSTMENT(adj), 1.0, 0);
    // gtk_box_pack_start (GTK_BOX (adjhbox), spinbtn, TRUE, TRUE, 0);
    gtk_box_pack_end (GTK_BOX (adjvbox), spinbtn, TRUE, TRUE, 0);
    gtk_widget_show (spinbtn);
    gtk_widget_show (adjvbox);
    gtk_widget_show (adjhbox);

    vscale = gtk_vscale_new (GTK_ADJUSTMENT (adj));
    gtk_range_set_update_policy (GTK_RANGE (vscale),
                                 GTK_UPDATE_CONTINUOUS);
    gtk_scale_set_digits (GTK_SCALE (vscale), 0);
    gtk_scale_set_value_pos (GTK_SCALE (vscale), GTK_POS_TOP);
    gtk_scale_set_draw_value (GTK_SCALE (vscale), TRUE);
    gtk_box_pack_start (GTK_BOX (adjhbox), vscale, TRUE, TRUE, 0);
    gtk_widget_show (vscale);

    /* hbox button spacing */
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX (hbox), 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);

    /* button sizes 80 x 24 */
    btnfind = gtk_button_new_with_mnemonic ("_Goto");
    gtk_widget_set_size_request (btnfind, 80, 24);
    gtk_widget_set_sensitive (btnfind, (line != app->line));
    gtk_widget_show (btnfind);

    btnclose = gtk_button_new_with_mnemonic ("_Close");
    gtk_widget_set_size_request (btnclose, 80, 24);

    gtk_box_pack_end (GTK_BOX (hbox), btnclose, FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (hbox), btnfind, FALSE, FALSE, 0); // pack on rt-side.
    gtk_widget_show (btnclose);

    gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show (hbox);

    gtk_widget_show (gotowin);

//     g_signal_connect (spinbtn, "preedit-changed",
//                       G_CALLBACK (goto_spinbtn_preedit), app);

    g_signal_connect (spinbtn, "value-changed",
                      G_CALLBACK (goto_spinbtn_changed), btnfind);

    g_signal_connect (btnfind, "clicked",
                      G_CALLBACK (goto_btnfind), app);

    g_signal_connect (btnclose, "clicked",
                      G_CALLBACK (goto_btnclose), app);

    g_signal_connect (gotowin, "key_press_event",
                      G_CALLBACK (on_goto_keypress), app);

    return (gotowin);
}

// void goto_spinbtn_preedit (GtkWidget *widget, gchar *txt, kwinst *app)
// {
//     g_print ("preedit text: %s\n", txt);
//     gtk_widget_set_sensitive (btnfind, TRUE);
//     if (widget) {}
// }

void goto_spinbtn_changed (GtkWidget *widget, gpointer data)
{
    gtk_widget_set_sensitive (data, TRUE);
    if (widget) {}
}

void goto_btnfind (GtkWidget *widget, kwinst *app)
{
    GtkTextIter liter;
    app->line = gtk_spin_button_get_value_as_int (
                    GTK_SPIN_BUTTON(spinbtn)) - 1; /* subtract 1 */
#ifdef DEBUG
    g_print ("goto line: %d\n", app->line);
#endif
    gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(app->buffer), &liter, app->line);
    new_pos = gtk_text_buffer_create_mark (GTK_TEXT_BUFFER(app->buffer),
                                    "new_line", &liter, FALSE);
//     gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (app->view),
//                                         new_pos);
    gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (app->view),
                                new_pos, 0.0, TRUE, 0.95, 0.8);
    /* must place cursor after scroll or statusbar will reflect scroll */
    gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(app->buffer), &liter);

    if (widget) {}
    if (app) {}
}

void goto_btnclose (GtkWidget *widget, kwinst *app)
{
    if (new_pos)
        gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER(app->buffer), new_pos);
    new_pos = NULL;

    gtk_widget_destroy (gotowin);

    if (widget) {}
}
