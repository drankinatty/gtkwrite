#include "gtk_statusbar.h"

void status_save_filename (context *app, gchar *filename)
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

void status_update_str (context *app, gchar *s)
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

void status_menuitem_label (GtkMenuItem *menuitem, context *app)
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

void status_pop (GtkWidget *widget, context *app)
{
    if (app->cid)
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
    if (widget) {}
}

void status_set_default (context *app)
{
    gchar *status;

    status = g_strdup_printf (" line:%5d :%4d  |  %s",
                              app->line + 1, app->col + 1,
                              app->overwrite ? "OVR" : "INS");

    if (app->cid)               /* pop previous statusbar entry */
        gtk_statusbar_pop (GTK_STATUSBAR (app->statusbar), app->cid);
    else
        app->cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (app->statusbar),
                                                 status);
    gtk_statusbar_push (GTK_STATUSBAR (app->statusbar), app->cid, status);

    g_free (status);
}
