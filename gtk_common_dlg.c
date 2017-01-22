#include "gtk_common_dlg.h"

void err_dialog (const gchar *errmsg)
{
        GtkWidget *dialog;

        g_warning (errmsg); /* log to terminal window */

        /* create an error dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         errmsg);

        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}

void font_select_dialog (GtkWidget *widget, context *app)
{
    GtkResponseType result;

    GtkWidget *dialog = gtk_font_selection_dialog_new ("Select Font");

    /* set initial font name if (!wanted) use default */
    if (!gtk_font_selection_dialog_set_font_name ((GtkFontSelectionDialog *)dialog,
                                                    app->fontname))
        gtk_font_selection_dialog_set_font_name ((GtkFontSelectionDialog *)dialog,
                                                "Monospace 8");

    result = gtk_dialog_run (GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY) {

        if (app->fontname) g_free (app->fontname);

        PangoFontDescription *font_desc;
        app->fontname = gtk_font_selection_dialog_get_font_name (
                                GTK_FONT_SELECTION_DIALOG (dialog));

        font_desc = pango_font_description_from_string (app->fontname);

        gtk_widget_modify_font (app->view, font_desc);
        pango_font_description_free (font_desc);
    }
    gtk_widget_destroy (dialog);

    if (widget) {}  /* stub */
}

void buffer_open_file_dlg (context *app, gchar *filename)
{
    GtkWidget *dialog;

    /* Create a new file chooser widget */
    dialog = gtk_file_chooser_dialog_new ("Select a file for editing",
					  // parent_window,
					  GTK_WINDOW (app->window),
					  GTK_FILE_CHOOSER_ACTION_OPEN,
					  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					  NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        app->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        buffer_insert_file (app, NULL); /* uugh passing ptr to member - fixed */
    }

    gtk_widget_destroy (dialog);
    if (filename) {}
}

/* TODO: Check on exit if user says "yes" save then cancels the
 *   filesave dialog (current GLib-CRITICAL **: g_file_set_contents:
 *   assertion 'filename != NULL' failed (segfault)
 */
gboolean buffer_prompt_on_mod (context *app)
{
    gboolean ret = FALSE;
    GtkTextBuffer *buffer;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view));

    if (gtk_text_buffer_get_modified (buffer) == TRUE)
    {
        GtkWidget *dialog;

        const gchar *msg  = "Do you want to save the changes you have made?";

        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO,
                                         msg);

        gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
        if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
        {
            ret = FALSE;    /* don't save */
        }
        else ret = TRUE;    /* save */

        gtk_widget_destroy (dialog);
    }

    return ret;
}

/* not currently in use, buffer_open_file_dlg instead */
gchar *get_open_filename (context *app)
{
    GtkWidget *chooser;
    gchar *filename=NULL;

    chooser = gtk_file_chooser_dialog_new ("Open File...",
                                            GTK_WINDOW (app->window),
                                            GTK_FILE_CHOOSER_ACTION_OPEN,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                            NULL);

    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    gtk_widget_destroy (chooser);

    return filename;
}

gchar *get_save_filename (context *app)
{
    GtkWidget *chooser;
    gchar *filename = NULL;

    chooser = gtk_file_chooser_dialog_new ("Save File...",
                                            GTK_WINDOW (app->window),
                                            GTK_FILE_CHOOSER_ACTION_SAVE,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                                            NULL);

    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    gtk_widget_destroy (chooser);

    return filename;
}
