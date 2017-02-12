#include "gtk_settings.h"
#include "gtk_common_dlg.h"

GtkWidget *create_settings_dlg (context *app)
{
    /* variables */
    GtkWidget *vbox;            /* vbox container   */
    GtkWidget *notebook;
    GtkWidget *frame;
    GtkWidget *label;

    /* create toplevel window */
    if (!(app->settingswin = gtk_window_new (GTK_WINDOW_TOPLEVEL))) {
        err_dialog ("create_settings_dlg() gtk_window_new failure.");
        return NULL;
    }
    gtk_window_set_position (GTK_WINDOW (app->settingswin), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (app->settingswin), 640, 540);
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
		      G_CALLBACK (settings_btnclose), app);

    /* main vbox container
     */
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (app->settingswin), vbox);

    /* Create a new notebook, place the position of the tabs */
    notebook = gtk_notebook_new ();
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);

    gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);

    /* TODO: rather than add a frame to the notebook along with
     * the tab label, it would be better to add a vbox and tab,
     * then include the frame(s) within the second vbox
     */
    frame = gtk_frame_new ("Appearance Settings");
    gtk_container_set_border_width (GTK_CONTAINER (frame), 10);
    gtk_widget_set_size_request (frame, 500, 400);
    gtk_widget_show (frame);

    label = gtk_label_new ("Setting Contents");
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_widget_show (label);

    label = gtk_label_new ("Appearance");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

    gtk_widget_show (notebook);
    gtk_widget_show (vbox);
    gtk_widget_show (app->settingswin);

    return (app->settingswin);
}

void settings_btnclose (GtkWidget *widget, context *app)
{
    gtk_widget_destroy (app->settingswin);
    if (app) {}
    if (widget) {}
}

