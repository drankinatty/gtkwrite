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
