#include "gtk_common_dlg.h"

/** general use err_dialog, just pass errmsg. */
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

/** general use err_dialog, just pass errmsg. */
void err_dialog_win (gpointer *data, const gchar *errmsg)
{
    kwinst *app = (kwinst *)data;
    GtkWidget *dialog;

    g_warning (errmsg); /* log to terminal window */

    /* create an error dialog and display modally to the user */
    dialog = gtk_message_dialog_new (GTK_WINDOW (app->window),
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_OK,
                                    errmsg);

    gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

/** font_select_dialog used to set textview font.
 *  create a new pango font description and calls
 *  gtk_widget_modify_font to set textview font.
 */
void font_select_dialog (GtkWidget *widget, kwinst *app)
{
    GtkResponseType result;

    GtkWidget *dialog = gtk_font_selection_dialog_new ("Select Font");

    /* set initial font name if (!wanted) use default */
    if (!gtk_font_selection_dialog_set_font_name (
            GTK_FONT_SELECTION_DIALOG (dialog), app->fontname))
        gtk_font_selection_dialog_set_font_name (
            GTK_FONT_SELECTION_DIALOG (dialog), "Monospace 8");

    result = gtk_dialog_run (GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY) {

        if (app->fontname) g_free (app->fontname);

        PangoFontDescription *font_desc;
        app->fontname = gtk_font_selection_dialog_get_font_name (
                                GTK_FONT_SELECTION_DIALOG (dialog));

        if (!app->fontname) {
            err_dialog ("error: invalid font returned.");
            return;
        }

        font_desc = pango_font_description_from_string (app->fontname);

        gtk_widget_modify_font (app->view, font_desc);
        pango_font_description_free (font_desc);
    }
    gtk_widget_destroy (dialog);

    if (widget) {}  /* stub */
}

/** ib_response_ok - default callback for show_info_bar functions.
 *  callback for infobar with "_Ok" button to hide infobar.
 */
void ib_response_ok (GtkInfoBar *bar, gint response_id, kwinst *app)
{
    gtk_widget_hide (GTK_WIDGET(bar));

    /* if called by default from show_info_bar_choice, check and
     * restore sensitive setting for textview.
     */
    if (!gtk_widget_get_sensitive (app->view))
        gtk_widget_set_sensitive (app->view, TRUE);

    /* grab focus for textview */
    gtk_widget_grab_focus (app->view);

    /* reset infobar flags */
    app->ibflags = 0;

    if (response_id) {}
}

void show_info_bar_ok (const gchar *msg, gint msgtype, kwinst *app)
{
    GtkWidget *infobar;             /* the infobar widget */
    GtkInfoBar *bar;                /* a GtkInfoBar* reference */

    GtkWidget *message_label;       /* test to display in infobar */
    GtkWidget *content_area;        /* content_area of infobar */
    GtkWidget *hbox;                /* hbox for content_area */

    infobar = gtk_info_bar_new ();  /* create new infobar */
    gtk_widget_set_no_show_all (infobar, TRUE); /* set no show all */
    bar = GTK_INFO_BAR (infobar);   /* create reference for convenience */
    content_area = gtk_info_bar_get_content_area (bar); /* get content_area */

    hbox = gtk_hbox_new (FALSE, 0); /* create hbox for content w/border */
    gtk_container_set_border_width (GTK_CONTAINER(hbox), 5);

    /* set label text, add hbox to content_area, add label to hbox */
    message_label = gtk_label_new (msg);
    gtk_container_add (GTK_CONTAINER (content_area), hbox);
    gtk_box_pack_start (GTK_BOX(hbox), message_label, FALSE, FALSE, 0);
    gtk_widget_show (message_label);
    gtk_widget_show (hbox);

    /* change message foreground color as needed */
    if (msgtype < GTK_MESSAGE_ERROR) {
        GdkColor color;
        gdk_color_parse ("black", &color);
        gtk_widget_modify_fg (message_label, GTK_STATE_NORMAL, &color);
    }

    /* add button to close infobar */
    gtk_info_bar_add_button (bar, "_OK", GTK_RESPONSE_OK);

    /* choose type of infobar */
    gtk_info_bar_set_message_type (bar, msgtype);

    /* pack infobar into vbox in parent (passed a pointer data) */
    gtk_box_pack_start(GTK_BOX(app->ibarvbox), infobar, FALSE, TRUE, 0);

    /* connect response handler */
    g_signal_connect (bar, "response", G_CALLBACK (ib_response_ok), app);

    /* set label in infobar selectable */
    if ((app->ibflags >> (IBAR_LABEL_SELECT - 1)) & 1)
        gtk_label_set_selectable (GTK_LABEL(message_label), TRUE);

    /* set text_view sensitive FALSE */
    if ((app->ibflags >> (IBAR_VIEW_SENSITIVE - 1)) & 1)
        gtk_widget_set_sensitive (app->view, FALSE);

    gtk_widget_show (infobar);  /* show the infobar */
}

gchar *set_ib_content_markup (gint msgtype, const gchar *msg)
{
    gchar *title;                   /* message title */
    gchar *content;                 /* combined title + msg */

    /* set label text, add hbox to content_area, add label to hbox */
    switch (msgtype) {
        case GTK_MESSAGE_WARNING:
            title = g_markup_printf_escaped ("<span font_weight=\"bold\">%s</span>",
                                            "Warning:");
            break;
        case GTK_MESSAGE_ERROR:
            title = g_markup_printf_escaped ("<span font_weight=\"bold\">%s</span>",
                                            "Error:");
            break;
        default:
            title = g_markup_printf_escaped ("<span font_weight=\"bold\">%s</span>",
                                            "Note:");
            break;
    }
    content = g_strdup_printf ("%s  %s", title, msg);

    g_free (title);     /* free title memory */

    return content;
}

void show_info_bar_markup_ok (const gchar *msg, gint msgtype, kwinst *app)
{
    GtkWidget *infobar;             /* the infobar widget */
    GtkInfoBar *bar;                /* a GtkInfoBar* reference */

    GtkWidget *message_label;       /* test to display in infobar */
    GtkWidget *content_area;        /* content_area of infobar */
    GtkWidget *hbox;                /* hbox for content_area */

    gchar *content;                 /* combined title + msg */

    infobar = gtk_info_bar_new ();  /* create new infobar */
    gtk_widget_set_no_show_all (infobar, TRUE); /* set no show all */
    bar = GTK_INFO_BAR (infobar);   /* create reference for convenience */
    content_area = gtk_info_bar_get_content_area (bar); /* get content_area */

    hbox = gtk_hbox_new (FALSE, 0); /* create hbox for content w/border */
    gtk_container_set_border_width (GTK_CONTAINER(hbox), 5);

    /* set label text, add hbox to content_area, add label to hbox */
    message_label = gtk_label_new (NULL);
    content = set_ib_content_markup (msgtype, msg);
    gtk_label_set_markup (GTK_LABEL(message_label), content);

    gtk_container_add (GTK_CONTAINER (content_area), hbox);
    gtk_box_pack_start (GTK_BOX(hbox), message_label, FALSE, FALSE, 0);
    gtk_widget_show (message_label);
    gtk_widget_show (hbox);

    g_free (content);   /* free content memory */

    /* change message foreground color as needed */
    if (msgtype < GTK_MESSAGE_ERROR) {
        GdkColor color;
        gdk_color_parse ("black", &color);
        gtk_widget_modify_fg (message_label, GTK_STATE_NORMAL, &color);
    }

    /* add button to close infobar */
    gtk_info_bar_add_button (bar, "_OK", GTK_RESPONSE_OK);

    /* choose type of infobar */
    gtk_info_bar_set_message_type (bar, msgtype);

    /* pack infobar into vbox in parent (passed a pointer data) */
    gtk_box_pack_start(GTK_BOX(app->ibarvbox), infobar, FALSE, TRUE, 0);

    /* connect response handler */
    g_signal_connect (bar, "response", G_CALLBACK (ib_response_ok), app);

    /* set label in infobar selectable */
    if ((app->ibflags >> (IBAR_LABEL_SELECT - 1)) & 1)
        gtk_label_set_selectable (GTK_LABEL(message_label), TRUE);

    /* set text_view sensitive FALSE */
    if ((app->ibflags >> (IBAR_VIEW_SENSITIVE - 1)) & 1)
        gtk_widget_set_sensitive (app->view, FALSE);

    gtk_widget_show (infobar);  /* show the infobar */
}

/** example ibbtndef button definition for info_bar, with Yes, No, Cancel
 *  (note: the last btntext member must be the empty-string)
 */
/*
    ibbtndef btndef[] = { { .btntext = "_Yes",    .response_id = GTK_RESPONSE_YES },
                          { .btntext = "_No",     .response_id = GTK_RESPONSE_NO },
                          { .btntext = "_Cancel", .response_id = GTK_RESPONSE_CANCEL },
                          { .btntext = "",        .response_id = 0 } };
*/

/** example show_info_bar_choice() callback.
 *  (must customize for each infobar and ibbtndef array)
 *
 *  functions implementing the show_info_bar_xxx should have a prototype:
 *  void ibar_descriptive-name (....)
 *
 *  functions providing the response_id handler for ibar_descriptive-name
 *  should have a similarly named prototype:
 *  void ib_descriptive-name (GtkInfoBar *bar, gint response_id, kwinst *app)
 *
 */
/*
void ib_response (GtkInfoBar *bar, gint response_id, kwinst *app)
{
    switch (response_id) {
        case GTK_RESPONSE_APPLY:
            g_print ("ib_response: GTK_RESPONSE_APPLY\n");
            break;
        case GTK_RESPONSE_CANCEL:
            g_print ("ib_response: GTK_RESPONSE_CANCEL\n");
            break;
        case GTK_RESPONSE_CLOSE:
            g_print ("ib_response: GTK_RESPONSE_CLOSE\n");
            break;
        case GTK_RESPONSE_NO:
            g_print ("ib_response: GTK_RESPONSE_NO\n");
            break;
        case GTK_RESPONSE_OK:
            g_print ("ib_response: GTK_RESPONSE_OK\n");
            break;
        case GTK_RESPONSE_YES:
            g_print ("ib_response: GTK_RESPONSE_YES\n");
            break;
    }

    gtk_widget_hide (GTK_WIDGET(bar));

    // set text_view sensitive TRUE
    if (!gtk_widget_get_sensitive (app->view))
        gtk_widget_set_sensitive (app->view, TRUE);

    // grab focus for textview
    gtk_widget_grab_focus (app->view);

    // reset flags
    app->ibflags = 0;
}
*/

/* TODO - add gunchar bitfiled for label-selectable, textview-sensitive
 * and any other needed values, rather than adding multiple parameters.
 * declare enum GTK_IBAR_LABEL_SELECT, GTK_IBAR_VIEW_SENSITIVE, etc.
 * can add as app->ibflags and avoid additional parameter altogether.
 */
/** show infobar with msg, msgtype, btndef, and callback fn_response.
 *  btndef and fn_response can both be NULL, if btndef is NULL,
 *  "_OK" is used by default. btndef passes a pointer to array of
 *  ibbtndef containing btntext and resource_id pairs, the last
 *  btntext must be an empty-string sentinel. the fn_response must
 *  handle the response_id returned from the infobar.
 */
void show_info_bar_choice (const gchar *msg, gint msgtype,
                            ibbtndef *btndef,
                            void (*fn_response)(GtkInfoBar *bar,
                                    gint response_id,
                                    kwinst *app),
                            kwinst *app)
{
    GtkWidget *infobar;             /* the infobar widget */
    GtkInfoBar *bar;                /* a GtkInfoBar* reference */

    GtkWidget *message_label;       /* test to display in infobar */
    GtkWidget *content_area;        /* content_area of infobar */
    GtkWidget *hbox;                /* hbox for content_area */

    infobar = gtk_info_bar_new ();  /* create new infobar */
    gtk_widget_set_no_show_all (infobar, TRUE); /* set no show all */
    bar = GTK_INFO_BAR (infobar);   /* create reference for convenience */
    content_area = gtk_info_bar_get_content_area (bar); /* get content_area */

    hbox = gtk_hbox_new (FALSE, 0); /* create hbox for content w/border */
    gtk_container_set_border_width (GTK_CONTAINER(hbox), 5);

    /* set label text, add hbox to content_area, add label to hbox */
    // message_label = gtk_label_new (msg);
    message_label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL(message_label), msg);
    gtk_container_add (GTK_CONTAINER (content_area), hbox);
    gtk_box_pack_start (GTK_BOX(hbox), message_label, FALSE, FALSE, 0);
    gtk_widget_show (message_label);
    gtk_widget_show (hbox);

    /* change message foreground color as needed */
    if (msgtype < GTK_MESSAGE_ERROR) {
        GdkColor color;
        gdk_color_parse ("black", &color);
        gtk_widget_modify_fg (message_label, GTK_STATE_NORMAL, &color);
    }

    /* add buttons to infobar */
    if (btndef && *(btndef->btntext)) {
        for (; *(btndef->btntext); btndef++)
            gtk_info_bar_add_button (bar, btndef->btntext, btndef->response_id);
    }
    else
        gtk_info_bar_add_button (bar, "_OK", GTK_RESPONSE_OK);

    /* choose type of infobar */
    gtk_info_bar_set_message_type (bar, msgtype);

    /* pack infobar into vbox in parent (passed a pointer data) */
    gtk_box_pack_start(GTK_BOX(app->ibarvbox), infobar, FALSE, TRUE, 0);

    /* connect response handler */
    if (fn_response)                /* connect custom handler  */
        g_signal_connect (bar, "response", G_CALLBACK (fn_response), app);
    else                            /* connect default handler */
        g_signal_connect (bar, "response", G_CALLBACK (ib_response_ok), app);

    /* set label in infobar selectable */
    if ((app->ibflags >> (IBAR_LABEL_SELECT - 1)) & 1)
        gtk_label_set_selectable (GTK_LABEL(message_label), TRUE);

    /* set text_view sensitive */
    if ((app->ibflags >> (IBAR_VIEW_SENSITIVE - 1)) & 1)
        gtk_widget_set_sensitive (app->view, FALSE);

    gtk_widget_show (infobar);  /* show the infobar */
}

void buffer_file_insert_dlg (kwinst *app, gchar *filename)
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
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        buffer_insert_file (app, filename);
    }

    gtk_widget_destroy (dialog);
}

void buffer_file_open_dlg (kwinst *app, gchar *filename)
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
        split_fname (app);
        buffer_insert_file (app, NULL);
    }

    gtk_widget_destroy (dialog);
    if (filename) {}
}

/* Removed until upstream bug fixed
 * see: https://bugzilla.gnome.org/show_bug.cgi?id=779605
 */
void file_open_recent_dlg (kwinst *app)
{
    GtkWidget *dialog;
    GtkRecentManager *manager;
    GtkRecentFilter *filter;

    manager = gtk_recent_manager_get_default ();
    dialog = gtk_recent_chooser_dialog_new_for_manager ("Recent Documents",
                                            GTK_WINDOW (app->window),
                                            manager,
                                            GTK_STOCK_CANCEL,
                                            GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_OPEN,
                                            GTK_RESPONSE_ACCEPT,
                                            NULL);

    /* Add a filter that will only display plain text files.
     * note: the first filter defined is displayed by default.
     */
#ifndef HAVEMSWIN
    filter = gtk_recent_filter_new ();
    gtk_recent_filter_set_name (filter, "Plain Text");
    gtk_recent_filter_add_mime_type (filter, "text/plain");
    gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (dialog), filter);
#endif
    /* Add a filter that will display all of the files in the dialog. */
    filter = gtk_recent_filter_new ();
    gtk_recent_filter_set_name (filter, "All Files");
    gtk_recent_filter_add_pattern (filter, "*");
    gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (dialog), filter);

    /* set to choose most recently used files */
    gtk_recent_chooser_set_show_not_found (GTK_RECENT_CHOOSER (dialog),
                                           FALSE);
    gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (dialog),
                                      GTK_RECENT_SORT_MRU);
    gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER (dialog), app->nrecent);
    gtk_recent_chooser_set_show_tips (GTK_RECENT_CHOOSER(dialog), TRUE);
    gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (dialog), TRUE);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        GtkRecentInfo *info;
        const gchar *uri;
        gchar *filename;

        info = gtk_recent_chooser_get_current_item (GTK_RECENT_CHOOSER (dialog));
        /* compare with gtk_recent_chooser_get_current_uri
         * (you can use uri_to_filename here, but g_filename_from_uri is fine )
         */
        uri = gtk_recent_info_get_uri (info);
        // filename = uri_to_filename (gtk_recent_info_get_uri (info));
        if (uri) {
            filename = g_filename_from_uri (uri, NULL, NULL);
            file_open (app, filename);
            g_free (filename);
        }
        else
            err_dialog_win ((gpointer *)(app), "uri_to_filename () returned NULL");
        gtk_recent_info_unref (info);
    }
    gtk_widget_destroy (dialog);
}

void dlg_info (const gchar *msg, const gchar *title)
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new (NULL,
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_CLOSE,
                                    msg);

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void dlg_info_win (gpointer data, const gchar *msg, const gchar *title)
{
    kwinst *app = (kwinst *)data;
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new (GTK_WINDOW (app->window),
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_CLOSE,
                                    msg);

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

gboolean dlg_yes_no_msg (gpointer data, const gchar *msg, const gchar *title,
                            gboolean default_return)
{
    GtkWidget *window = data ? ((kwinst *)data)->window : NULL;
    gboolean ret = default_return ? TRUE : FALSE;
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    msg);

    gtk_window_set_title (GTK_WINDOW (dialog), title);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
    {
        ret = FALSE;    /* don't save */
    }
    else ret = TRUE;    /* save */

    gtk_widget_destroy (dialog);

    return ret;
}

gboolean buffer_prompt_on_mod (kwinst *app)
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

gchar *get_open_filename (kwinst *app)
{
    GtkWidget *chooser;
    gchar *filename=NULL;

    chooser = gtk_file_chooser_dialog_new ("Open File...",
                                            GTK_WINDOW (app->window),
                                            GTK_FILE_CHOOSER_ACTION_OPEN,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                            NULL);

    if (app->filename) {
#ifdef DEBUG
        g_print ("get_open_filename() app->fpath: %s\napp->filename: %s\n",
                app->fpath, app->filename);
#endif
        /* set current file path beginning choice */
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(chooser),
                                            app->fpath);
        /* set current filename beginning choice */
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(chooser),
                                        app->filename);
    }

    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    gtk_widget_destroy (chooser);

    return filename;
}

gchar *get_save_filename (kwinst *app)
{
    GtkWidget *chooser;
    gchar *filename = NULL;

    chooser = gtk_file_chooser_dialog_new ("Save File...",
                                            GTK_WINDOW (app->window),
                                            GTK_FILE_CHOOSER_ACTION_SAVE,
                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                            GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                                            NULL);

    gtk_file_chooser_set_create_folders (GTK_FILE_CHOOSER(chooser), TRUE);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(chooser),
                                                    TRUE);
    if (app->filename) {
        /* set current file path beginning choice */
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(chooser),
                                            app->fpath);
        /* set current filename beginning choice */
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(chooser),
                                        app->filename);
    }

    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    gtk_widget_destroy (chooser);

    return filename;
}

/** creates a new pixbuf from filename.
 *  you are responsible for calling g_object_unref() on
 *  the pixbuf when done.
 */
GdkPixbuf *create_pixbuf_from_file (const gchar *filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file (filename, &error);

    if (!pixbuf) {
        g_warning (error->message); /* log to terminal window */
        g_error_free (error);
    }

    return pixbuf;
}

