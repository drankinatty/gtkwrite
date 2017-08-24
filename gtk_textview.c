#include "gtk_textview.h"

/** create text_view or source_view, set font and tab size */
GtkWidget *create_textview_scrolledwindow (kwinst *app)
{
    GtkWidget *scrolled_window;         /* container for text_view */
    PangoFontDescription *font_desc;    /* font description */

#ifdef HAVESOURCEVIEW
    /* create buffer for text_view */
    app->buffer = gtk_source_buffer_new (NULL);

    /* create text_view */
    app->view = gtk_source_view_new_with_buffer (app->buffer);
    gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW(app->view), app->lineno);
    gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW(app->view), app->linehghlt);
    gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW(app->view), app->indentauto);

    /* set_smart_backspace available in sourceview 3 */
    // gtk_source_view_set_smart_backspace (GTK_SOURCE_VIEW(app->view), TRUE);
    gtk_source_view_set_smart_home_end (GTK_SOURCE_VIEW(app->view),
                                        GTK_SOURCE_SMART_HOME_END_BEFORE);
#else
    /* create buffer for text_view, init cursor and iter, line & col */
    app->buffer = gtk_text_buffer_new (NULL);

    /* create text_viewview */
    app->view = gtk_text_view_new_with_buffer (app->buffer);
#endif

    /* set wrap mode and show text_view */
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (app->view), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (app->view), 5);
    gtk_widget_show (app->view);

    /* Change default font throughout the widget */
    font_desc = pango_font_description_from_string (app->fontname);
    gtk_widget_modify_font (app->view, font_desc);

    /* set tab to lesser of softtab and tabstop if softtab set */
    set_tab_size (font_desc, app, (app->softtab && (app->softtab < app->tabstop) ?
                                    app->softtab : app->tabstop));
    pango_font_description_free (font_desc);

    /* create scrolled_window for view */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), app->view);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 5);

    return scrolled_window;
}

/** function to set the tab width to sz spaces based on font description */
void set_tab_size (PangoFontDescription *font_desc, kwinst *app, gint sz)
{
    PangoTabArray *tab_array;
    PangoLayout *layout;
    gint width, i;

    if (app->tabstring)
        g_free (app->tabstring);
    app->tabstring = g_strdup_printf ("%*s", sz, " ");

    layout = gtk_widget_create_pango_layout (app->view, app->tabstring);
    pango_layout_set_font_description (layout, font_desc);
    pango_layout_get_pixel_size (layout, &width, NULL);
    if (width) {
        tab_array = pango_tab_array_new (app->winwidth/width, TRUE);
        for (i = 0; i * width < app->winwidth; i++)
            pango_tab_array_set_tab (tab_array, i, PANGO_TAB_LEFT, i * width);

        gtk_text_view_set_tabs (GTK_TEXT_VIEW(app->view), tab_array);
        pango_tab_array_free (tab_array);
    }
}
