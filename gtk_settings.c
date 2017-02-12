#include "gtk_settings.h"
#include "gtk_common_dlg.h"

GtkWidget *create_settings_dlg (context *app)
{
    /* variables */
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *notebook;        /* notebook in vbox */
    GtkWidget *vboxnb;          /* vboxnb in each notebook page */
    GtkWidget *frame;           /* frames to section vboxnb */
    GtkWidget *label;
    GtkWidget *hbox;            /* hbox container   */
    GtkWidget *btnok;
    GtkWidget *btncancel;

    /* create toplevel window */
    if (!(app->settingswin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_settings_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->settingswin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->settingswin), 640, 470);
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

    /* main vbox container
     */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->settingswin), vbox);

    /* temp reminder */
    label = gtk_label_new ("Settings Are Under Construction!");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    /* Create a new notebook, place the position of the tabs left */
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);

    gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);

    /* TODO: rather than add a frame to the notebook along with
     * the tab label, it would be better to add a vbox and tab,
     * then include the frame(s) within the second vbox
     */
    /* X - setting page */
    vboxnb = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vboxnb, 500, 400);

    /* frame within page */
    frame = gtk_frame_new (NULL);
    gtk_frame_set_label (GTK_FRAME (frame), "Appearance Settings");
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_OUT);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
    // gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
    gtk_widget_show (frame);

    /* stuff in frame */
    label = gtk_label_new ("Setting Contents");
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_widget_show (label);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* frame within page */
    frame = gtk_frame_new ("Cursor & Selection");
    gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    // gtk_widget_set_size_request (frame, 500, 200);
    gtk_widget_show (frame);

    /* stuff in frame */
    label = gtk_label_new ("Smart home and smart end");
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_widget_show (label);

    /* pack frame into notebook vbox */
    gtk_box_pack_start (GTK_BOX (vboxnb), frame, FALSE, FALSE, 0);

    /* label for tab */
    label = gtk_label_new ("Appearance");

    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vboxnb, label);
    gtk_widget_show (vboxnb);

    /* X - setting page */
    vboxnb = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vboxnb, 500, 400);

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

    /* settings callbacks */
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

