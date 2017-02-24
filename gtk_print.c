#include "gtk_print.h"

#define HEADER_HEIGHT 20

static GtkPrintSettings *settings = NULL;

void begin_print (GtkPrintOperation *operation,
                    GtkPrintContext   *prcontext,
                    context            *app)
{
//     GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
//     GtkTextIter start, end;
//
//     gtk_text_buffer_get_bounds (buffer, &start, &end);

    if (operation || prcontext || app) {}
    g_print ("begin_print...\n");
}

static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext   *prcontext,
           gint               page_nr,
           context           *app)
{
    g_print ("draw_page...\n");

    cairo_t *cr;
    PangoLayout *layout;
    gdouble width, text_height;
    gint layout_height;
    PangoFontDescription *desc;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_bounds (buffer, &start, &end);
    if (gtk_text_iter_equal (&start, &end)) {
        err_dialog ("Error: GTK_TEXT_BUFFER is EMPTY.");
        return;
    }

    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    cr = gtk_print_context_get_cairo_context (prcontext);
    width = gtk_print_context_get_width (prcontext);

    cairo_rectangle (cr, 0, 0, width, HEADER_HEIGHT);

    cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
    cairo_fill (cr);

    layout = gtk_print_context_create_pango_layout (prcontext);

    desc = pango_font_description_from_string (app->fontname);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    pango_layout_set_text (layout, text, -1);
    pango_layout_set_width (layout, width * PANGO_SCALE);
    pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);

    pango_layout_get_size (layout, NULL, &layout_height);
    text_height = (gdouble)layout_height / PANGO_SCALE;

    cairo_move_to (cr, width / 2,  (HEADER_HEIGHT - text_height) / 2);
    pango_cairo_show_layout (cr, layout);

    g_object_unref (layout);

    g_free (text);

    if (operation || prcontext || page_nr || app) {}
}



void do_print (context *app)
{
    GtkPrintOperation *print;
    GtkPrintOperationResult res;
    GError *error = NULL;

    print = gtk_print_operation_new ();

    if (app->printsettings != NULL)
        gtk_print_operation_set_print_settings (print, app->printsettings);

    if (app->printpgsetup != NULL)
        gtk_print_operation_set_default_page_setup (print, app->printpgsetup);

    g_signal_connect (print, "begin_print", G_CALLBACK (begin_print), app);
    g_signal_connect (print, "draw_page", G_CALLBACK (draw_page), app);

    res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
                                    GTK_WINDOW (app->window), &error);

    if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
    {
        GtkWidget *error_dialog;

        error_dialog = gtk_message_dialog_new (GTK_WINDOW (app->window),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_CLOSE,
                                "Error printing file:\n%s",
                                error->message);
        g_signal_connect (error_dialog, "response",
                            G_CALLBACK (gtk_widget_destroy), NULL);
        gtk_widget_show (error_dialog);
        g_error_free (error);
    }
    else if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
    {
        if (settings != NULL)
            g_object_unref (settings);
        settings = g_object_ref (gtk_print_operation_get_print_settings (print));
    }
    g_object_unref (print);
}
