#include "gtk_statusbar.h"

void status_save_filename (kwinst *app, gchar *filename)
{   /* filename parameter not used in this implementation */
    gchar *status;

    /* if (filename)
        status = g_strdup_printf ("Saving %s...", filename);
    else */
        status = g_strdup_printf ("Saving %s...", app->filename);
    status_update_str (app, status);

    g_free (status);

    if (filename) {}
}

void status_update_str (kwinst *app, gchar *s)
{
    if (app->cid)               /* pop previous statusbar entry */
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);

    gchar *str;
    if (s && *s)
        str = g_strdup (s);
        // str = g_strdup_printf ("Operation : %s", s);
    else
        str = g_strdup_printf ("Error : statusbar update failed 'NULL'");

    app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar), str);
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, str);

    g_free (str);
}

void status_menuitem_label (GtkMenuItem *menuitem, kwinst *app)
{
    if (app->cid)               /* pop previous statusbar entry */
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);

    gchar *str;                 /* create string from menu item */
    str = g_strdup_printf ("menuitem : %s",
                           gtk_menu_item_get_label (menuitem));
//                            gtk_menu_item_get_label (GTK_MENU_ITEM (widget)));
    app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar), str);

    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, str);

    g_free (str);
}

void status_pop (GtkWidget *widget, kwinst *app)
{
    if (app->cid)
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
    if (widget) {}
}

void status_set_default (kwinst *app)
{
    extern const gchar *bomstr[];
    gchar *status;

    /* Fri Jul 13 2018 18:57:03 CDT moved line/col update here from
     * buffer_update_pos () to prevent line number jumping on scrolled-
     * window scrolling when bottom of screen reached.
     */
    GtkTextIter iter;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER (app->buffer);
    GtkTextMark *ins = gtk_text_buffer_get_insert (buffer);

    /* get iter at 'insert' mark */
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, ins);

    /* update line, lines & col */
    app->line = gtk_text_iter_get_line (&iter);
    app->lines = gtk_text_buffer_get_line_count (buffer);
    app->col = gtk_text_iter_get_line_offset (&iter);

#ifdef HAVESOURCEVIEW
    status = g_strdup_printf (" Line:%5d / %d  Col:%4d  |  %s  |  "
                            "%s  |  %s  |  %s  |  %s",
                            app->line + 1, app->lines, app->col + 1,
                            app->overwrite ? "OVR" : "INS",
                            app->eolnm[app->eol], bomstr[app->bom],
                            app->langname ? app->langname : "Plain Text",
                            app->dynwrap ? "Word Wrap" : "Wrap Off");
#else
    status = g_strdup_printf (" Line:%5d / %d  Col:%4d  |  %s  |  %s  |  %s  |  %s",
                            app->line + 1, app->lines, app->col + 1,
                            app->overwrite ? "OVR" : "INS",
                            app->eolnm[app->eol], bomstr[app->bom],
                            app->dynwrap ? "Word Wrap" : "Wrap Off");
#endif

    if (app->cid)               /* pop previous statusbar entry */
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
    else
        app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar),
                                                 status);
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, status);

    g_free (status);
}
