#include "gtk_print.h"

#define KW_INST(v) (kwinst *)(v)

typedef struct {
    gint fontwidth;
    gint line;
    gint col;
    gint nlines;
    gint npages;
    gint linesperpage;
    gint linenomargin;
    gboolean lineno;
    gdouble lineheight;

    PangoLayout *layout;
} kwprint;


static GtkPrintSettings *settings = NULL;
static GtkPageSetup *page_setup = NULL;
static kwprint *prn;

void kwprint_init (kwprint *s)
{
    s->fontwidth = 0;
    s->line = 0;
    s->col = 0;
    s->nlines = 0;
    s->npages = 0;
    s->linesperpage = 0;
    s->linenomargin = 0;
    s->lineno = FALSE;
    s->lineheight = 0.0;
    s->layout = NULL;
}

void begin_print (GtkPrintOperation *operation,
                    GtkPrintContext *context,
                    gpointer          data)
{
    /* g_print ("begin_print...\n"); */

    kwinst *app = KW_INST(data);
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextMark *insert;
    GtkTextIter iter;

    PangoContext *pango_ctx;
    PangoFontMetrics *metrics;
    PangoFontDescription *desc;
    gint nlines, layout_height, remain;
    gdouble width, height;
    const gchar *fontname = (const gchar *)app->fontname;

    prn->lineno = FALSE;    /* just set to false for now / TODO: add to settings */

    insert = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, insert);

    prn->line = gtk_text_iter_get_line (&iter);
    prn->col = gtk_text_iter_get_line_offset (&iter);
    prn->nlines = nlines = gtk_text_buffer_get_line_count (buffer);
    if (prn->nlines <= 0) { /* nothing to print */
        g_print ("  nothing in buffer to print.\n");
        return;
    }

    prn->linenomargin = 0;
    while (nlines /= 10) prn->linenomargin++;   /* set max char for line no */

    desc = pango_font_description_from_string (fontname);
    pango_ctx = gtk_print_context_create_pango_context (context);

    metrics = pango_context_get_metrics (pango_ctx, desc,
                        pango_context_get_language (pango_ctx));
    prn->fontwidth = pango_font_metrics_get_approximate_digit_width (metrics) / PANGO_SCALE;

    pango_font_metrics_unref (metrics);
    g_object_unref (pango_ctx);

    /* create pango layout from print_context & set options */
    prn->layout = gtk_print_context_create_pango_layout (context);
    pango_layout_set_wrap (prn->layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_spacing (prn->layout, 0);
    pango_layout_set_attributes (prn->layout, NULL);
    pango_layout_set_font_description (prn->layout, desc);

    /* get page width and height */
    width = gtk_print_context_get_width (context);
    height = gtk_print_context_get_height (context);

    if (prn->lineno) /* subtract line num margin from width */
        width -= prn->linenomargin * prn->fontwidth;

    /* get page width and height in characters based on font
     * and calculate lines per page.
     */
    pango_layout_set_width (prn->layout, width * PANGO_SCALE);
    pango_layout_set_text (prn->layout, "Test For Height", -1);
    pango_layout_get_size (prn->layout, NULL, &layout_height);
    if (layout_height <= 0)
        layout_height = 100 & PANGO_SCALE;
    prn->lineheight = (gdouble)layout_height / PANGO_SCALE;
    prn->linesperpage = (height - prn->lineheight) / prn->lineheight + 0.5;
    /* adjust lpp as needed for header (-3) page numbers (-2) */

    remain = prn->linesperpage; /* remaining lines in page */

    prn->npages = 0;

    /* get start iter, iterate over each line in buffer, getting
     * number of chars in line to set number of pages.
     */
    gtk_text_buffer_get_start_iter (buffer, &iter);
    while (gtk_text_iter_forward_to_line_end (&iter)) {
        gint linesoftext = 1;
        gint nchars = gtk_text_iter_get_chars_in_line (&iter);
        if (nchars > width)
            linesoftext = (nchars / width) + 0.5;
#ifdef DPAGES
        if (linesoftext > remain) {
            remain = prn->linesperpage - linesoftext + remain;
            prn->npages++;
        }
        else if (linesoftext == remain) {
            remain = prn->linesperpage;
            prn->npages++;
        }
#else
        while (remain < linesoftext) {
            linesoftext -= remain;
            remain = prn->linesperpage;
            prn->npages++;
        }
#endif
        remain -= linesoftext;
    }
    prn->npages++;
    // g_print ("number of pages: %d\n", prn->npages);

    /* set the number of pages to print */
    gtk_print_operation_set_n_pages (operation, prn->npages);

    pango_font_description_free (desc); /* free font description */

    if (operation || context || app) {}
}

static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext   *context,
           gint               page_nr,
           kwinst            *app)
{
    /* g_print ("draw_page... %d\n", page_nr); */

    cairo_t *cr;
    PangoLayout *layout;
    gdouble width;
    gint line_start = 0, line_end = 0, hdr_lines;
    PangoFontDescription *desc;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);
    GtkTextIter start, end;
    gchar *text;

    cr = gtk_print_context_get_cairo_context (context);
    width = gtk_print_context_get_width (context);

    cairo_rectangle (cr, 0, 0, width, prn->lineheight * 2);

    cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
    // cairo_fill (cr);
    cairo_stroke (cr);

    hdr_lines = 3; /* two above plus margin between header and start */

    /* select text for page_nr from text buffer */
    if (prn->npages == 1) { /* one page -- select whole buffer */
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        // g_print ("  printing entire buffer.\n");
    }
    else {  /* select only the lines per page offset by page_nr pages */
        line_start = (prn->linesperpage - hdr_lines) * page_nr;
        line_end = line_start + (prn->linesperpage - hdr_lines - 1);
        if (line_end > prn->nlines)
            line_end = prn->nlines;
        // g_print ("  printing lines: %2d - %2d\n", line_start, line_end);
        gtk_text_buffer_get_iter_at_line (buffer, &start, line_start);
        gtk_text_buffer_get_iter_at_line (buffer, &end, line_end);

        /* check not at end of line before moving to end of line */
        if (gtk_text_iter_get_char (&end) != '\n') {
            gtk_text_iter_forward_to_line_end (&end);
        }
    }   /* check range isn't empty */
    if (gtk_text_iter_equal (&start, &end)) {
        err_dialog ("Error: Print Range Selected is EMPTY.");
        return;
    }

    /* get text to print for page_nr */
    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);   /* black text */

    /* create the pango layout for text */
    layout = gtk_print_context_create_pango_layout (context);

    desc = pango_font_description_from_string (app->fontname);
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);

    pango_layout_set_text (layout, text, -1);   /* options */
    pango_layout_set_width (layout, width * PANGO_SCALE);
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);

    // cairo_move_to (cr, 0, HEADER_HEIGHT + prn->lineheight);
    cairo_move_to (cr, 0, prn->lineheight * hdr_lines);
    pango_cairo_show_layout (cr, layout);

    g_object_unref (layout);

    g_free (text);

    if (operation) {}
}

void do_print (kwinst *app)
{
    GtkPrintOperation *print;
    GtkPrintOperationResult res;
    GError *error = NULL;

    if (!(prn = g_malloc0 (sizeof *prn)))    /* allocate prn struct */
        return;
    kwprint_init (prn);

    print = gtk_print_operation_new ();

    if (settings != NULL)
        gtk_print_operation_set_print_settings (print, settings);

    if (page_setup != NULL)
        gtk_print_operation_set_default_page_setup (print, page_setup);

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

    g_free (prn);   /* free prn struct */
}
