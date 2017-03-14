#include "gtk_sourceview.h"

enum { SCTSZ = 16, MESZ = 2048 };

#ifdef HAVESOURCEVIEW

#include <gtksourceview/gtksourcestyleschememanager.h>

GtkWidget *sectMenu;

typedef struct {
    gchar *styleid;
    gchar *name;
    GtkSourceLanguage *lang;
} langinfo;

typedef struct {
    gchar *section;
    GPtrArray *lang;
} sectmenu;

/** free function for each element of langinfo PTrArray */
void freelanginfo (gpointer data)
{
    if (!data) return;

    g_free (((langinfo *)data)->styleid);
    g_free (((langinfo *)data)->name);

    g_free (data);
}

/** free unction for each element of sectmenu PTrArray */
void freemnu (gpointer data)
{
    if (!data) return;

    g_free (((sectmenu *)data)->section);

    GPtrArray *p  = data;
    GPtrArray *lp = ((sectmenu *)p)->lang;

    g_ptr_array_free (lp, TRUE);

    g_free (data);
}

/* glib string sort comparison */
gint gcmpstrings (gconstpointer a, gconstpointer b)
{
    return g_strcmp0 (*(gchar * const *)a, *(gchar * const *)b);
}

/** sectmenu sort on section compare function */
gint gcmpmenu (gconstpointer a, gconstpointer b)
{
    return gcmpstrings (((sectmenu *)a)->section,
                        ((sectmenu *)b)->section);
}

/** langinfo sort on styleid compare function */
gint gcmplang (gconstpointer a, gconstpointer b)
{
    return gcmpstrings (((langinfo *)a)->styleid,
                        ((langinfo *)b)->styleid);
}

/** foreach sort of langinfo in section menu */
void sortlangs (gpointer menu, gpointer data)
{
    g_ptr_array_sort ((GPtrArray *)(((sectmenu *)menu)->lang), gcmplang);
    if (data) {}
}

/* temp foreach dump of langinfo for each section */
void prnsectlangs (gconstpointer l)
{
//     g_print ("  language->styleid: %20s     language->name: %s\n",
    g_print ("  %20s:%s\n",
            ((langinfo *)l)->styleid, ((langinfo *)l)->name);
}

/* temp print of each section and langinfo under each */
void prnmenu (gconstpointer m)
{
    g_print ("\nsection: %s\n", ((sectmenu *)m)->section);
    GPtrArray *lm = ((sectmenu *)m)->lang;
    g_ptr_array_foreach (lm, (GFunc) prnsectlangs, NULL);
}

/** lookup sourceview sourcelanguage given name.
 *  sets and returns app->language allowing the syntax highlighting
 *  to be set on the sourceview buffer.
 */
GtkSourceLanguage *sourcelanguage_lookup (const gchar *name, gpointer data)
{
    kwinst *app = (kwinst *)data;
    const gchar * const *lang_ids;
    GtkSourceLanguage *lang;

    /* lang_ids is owned by lm and must not be modified */
    lang_ids = gtk_source_language_manager_get_language_ids (app->langmgr);

    for (gint i = 0; lang_ids[i]; i++)
    {
        const gchar *lmname;
        lang = gtk_source_language_manager_get_language (app->langmgr,
                                                         lang_ids[i]);
        lmname = gtk_source_language_get_name (lang);

        if (*lmname == *name && g_strcmp0 (lmname, name) == 0)
            return ((app->language = lang));
    }

    return NULL;
}

/** set source buffer syntax highlight none.
 */
void hghltmenu_set_none (GtkMenuItem *menuitem, gpointer data)
{
    kwinst *app = (kwinst *)data;
    gtk_source_buffer_set_language (app->buffer, NULL);
    if (menuitem) {}
}

/** set syntax highlight for sourceview buffer by name.
 *  validates name is a valid language name, sets syntax highlighting.
 */
void sourceview_buffer_set_language (const gchar *name, gpointer data)
{
    kwinst *app = (kwinst *)data;
    GtkSourceLanguage *lang;

    if ((lang = sourcelanguage_lookup (name, data)))
        gtk_source_buffer_set_language (app->buffer, app->language);
}

/** lookup sourceview language based on language name.
 *  based on menuitem.
 */
void hghltmenu_get_name (GtkMenuItem *menuitem, gpointer data)
{
    const gchar *name = gtk_menu_item_get_label (menuitem);

    sourceview_buffer_set_language (name, data);
}

/** build languange names submenus */
void addlangmenu (gconstpointer l, gpointer data)
{
    GtkWidget *langMi;

    /* create menu entry with language name, add to sectMenu */
    langMi = gtk_menu_item_new_with_label (((langinfo *)l)->name);
    gtk_menu_shell_append (GTK_MENU_SHELL (sectMenu), langMi);

    /* connect menuitem callback, where name used to set language */
    g_signal_connect (G_OBJECT (langMi), "activate",
                      G_CALLBACK (hghltmenu_get_name), data);
}

/** build the sections submenu containing submenus
 *  of language names
 */
void addsubmenu (gconstpointer m, gpointer data)
{
    GtkWidget *sectionMi;
    kwinst *app = (kwinst *)data;
    GPtrArray *lm = ((sectmenu *)m)->lang;

    sectMenu = gtk_menu_new ();
    sectionMi = gtk_menu_item_new_with_label (((sectmenu *)m)->section);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (sectionMi), sectMenu);

    g_ptr_array_foreach (lm, (GFunc) addlangmenu, app);
    gtk_menu_shell_append (GTK_MENU_SHELL (app->hghltmenu), sectionMi);
}

/** create sourceview language selection submenu.
 *  the submenu is itself a collection of submenus for the
 *  languages in each section. the section top submenu contains
 *  the sorted list of sourceview language sections, with a
 *  submenu for each section containing all languages within
 *  the section in sort order.
 */
GtkWidget *highlight_build_menu (gpointer data)
{
    kwinst *app = (kwinst *)data;
    GtkWidget *noneMi;

    if (!app->langmgr) return NULL;

    app->hghltmenu = gtk_menu_new ();

    /* PTrArray holding the section name and PTrArray of languages
     * within each section. This will build a menu of sections with each holding
     * the languanges that can be used to set syntax highlight.
     */
    GPtrArray *menu = g_ptr_array_new_full (MESZ, freemnu);
    if (menu) { /* handle error */ }

    const gchar * const *lang_ids;

    /* lang_ids is owned by lm and must not be modified */
    lang_ids = gtk_source_language_manager_get_language_ids (app->langmgr);

    for (gint i = 0; lang_ids[i]; i++)
    {
        GPtrArray *p = NULL;
        gboolean found = FALSE;

        /* create sourceview language for each lang_ids[x] */
        /* TODO: add lang to langinfo struct for activation */
        GtkSourceLanguage *lang =
                gtk_source_language_manager_get_language (app->langmgr,
                                                         lang_ids[i]);
        /* create/allocate sectmenu struct and
         * allocate/fill section name
         */
        sectmenu *smenu = g_malloc0 (sizeof *smenu);
        if (!smenu) { /* handle error */ return NULL; }
        smenu->section = g_strdup (gtk_source_language_get_section (lang));

        /* create/allocate langinfo struct and
         * allocate/fill style_id and name
         */
        langinfo *linfo = g_malloc0 (sizeof *linfo);
        if (!linfo) { /* handle error */ return NULL; }
        linfo->styleid = g_strdup (gtk_source_language_get_id (lang));
        linfo->name    = g_strdup (gtk_source_language_get_name (lang));

        /* iterate over existing section names to
         * determine if the section name already exists
         * in the menu PTrArray, set found flag.
         */
        for (guint j = 0; j < menu->len; j++) {
            p = g_ptr_array_index(menu, j);
            if (g_strcmp0 (((sectmenu *)p)->section,
                            smenu->section) == 0) {
                found = TRUE;
                break;
            }
        }

        if (!found) {
            /* if section name is not contained in menu, create new
             * langinfo PTrArray and assign to smenu->lang pointer, then
             * add the smenu struct as a new element in menu.
             */
            GPtrArray *lmenu = g_ptr_array_new_full (SCTSZ, freelanginfo);
            g_ptr_array_add (lmenu, linfo);
            smenu->lang = lmenu;

            g_ptr_array_add (menu, smenu);
        }
        else {
            /* section name already exists in menu, just add linfo
             * as a new element in its lang PTrArray
             */
            g_ptr_array_add (((sectmenu *)p)->lang, linfo);
        }
    }

    /* sort section headings and source languanges under each */
    g_ptr_array_sort (menu, gcmpmenu);
    g_ptr_array_foreach (menu, (GFunc) sortlangs, NULL);

    /* print the menu (for testing) */
    // g_ptr_array_foreach (menu, (GFunc) prnmenu, NULL);

    /* create menu with submenues, add syntax highlight None
     * menu_entry with separator, and create all submenus
     * with nested g_ptr_array_foreach calls.
     */
    app->hghltmenu = gtk_menu_new ();
    noneMi = gtk_menu_item_new_with_label ("None");

    gtk_menu_shell_append (GTK_MENU_SHELL (app->hghltmenu), noneMi);
    gtk_menu_shell_append (GTK_MENU_SHELL (app->hghltmenu),
                           gtk_separator_menu_item_new());

    g_signal_connect (G_OBJECT (noneMi), "activate",
                      G_CALLBACK (hghltmenu_set_none), app);

    g_ptr_array_foreach (menu, (GFunc) addsubmenu, app);

    /* free all memory associated with nested pointer arrays */
    g_ptr_array_free (menu, TRUE);

    return app->hghltmenu;
}

/** set sourceview syntax scheme based on menuitem passed to function.
 *  set the current sourceview buffer contents to the chosen scheme.
 */
void sourceview_set_syntax_style (GtkMenuItem *menuitem, gpointer data)
{
    kwinst *app = (kwinst *)data;
    GtkSourceStyleSchemeManager *sm;
    GtkSourceStyleScheme *scheme;
    const gchar *name = gtk_menu_item_get_label (menuitem);

    if (app->laststyle)
        g_free (app->laststyle);

    app->laststyle = g_strdup (name);

    sm = gtk_source_style_scheme_manager_get_default();
    scheme = gtk_source_style_scheme_manager_get_scheme (sm, name);
    gtk_source_buffer_set_style_scheme (app->buffer, scheme);
}

/** set sourceview syntax scheme based on combobox entry selection.
 *  set the current sourceview buffer contents to the chosen scheme.
 */
void sourceview_list_set_syntax_style (GtkWidget *widget, gpointer data) {

    kwinst *app = (kwinst *)data;
    GtkSourceStyleSchemeManager *sm;
    GtkSourceStyleScheme *scheme;
    gchar *name = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(widget));

    if (app->laststyle)
        g_free (app->laststyle);

    app->laststyle = name;

    sm = gtk_source_style_scheme_manager_get_default();
    scheme = gtk_source_style_scheme_manager_get_scheme (sm, name);
    gtk_source_buffer_set_style_scheme (app->buffer, scheme);

    // g_free (name); /* not needed as saving in app->laststyle */
}

/** build systax scheme submenu and fill combobox with entries.
 *  returns submenu, combobox is preserves as part of data structure.
 */
GtkWidget *sourceview_syntax_styles_menu (GtkWidget *menu, gpointer data)
{
    GtkSourceStyleSchemeManager *sm;
    kwinst *app = (kwinst *)data;
    const gchar * const *scheme_ids = NULL;
    gchar **sorted = NULL;
    gint n = 0;

    sm = gtk_source_style_scheme_manager_get_default();
    scheme_ids = gtk_source_style_scheme_manager_get_scheme_ids (sm);

    if (!scheme_ids) return NULL;

    /* get the number of entries */
    for (n = 0; scheme_ids[n]; n++) {}

    /* allocate n pointers */
    sorted = g_malloc0 (n * sizeof *sorted);

    /* assign pointers to scheme_ids to array for sorting */
    for (gint i = 0; i < n; i++)
        sorted[i] = (gchar *)scheme_ids[i];

    /* if more than 1 style, sort, the number of elements is small
     * so there is nothing to be gained from a more efficient sort
     * routine, and this saves all function call overhead.
     */
    if (n > 1)
        for (gint i = 0; i < n; i++) {
            for (gint j = i; j < n; j++) {
                const gchar *tmp;
                if (g_strcmp0 (sorted[j], sorted[i]) < 0) {
                    tmp = sorted[i];
                    sorted[i] = sorted[j];
                    sorted[j] = (gchar *)tmp;
                }
            }
        }

    /* create new menu for syntax styles */
    menu = gtk_menu_new();

    /* create menu entries for each style_id and connect callback
     * that uses the menuitem to set the syntax style.
     */
    for (gint i = 0; i < n; i++) {

        GtkWidget *synschemeMi;

        /* define entry based on scheme_id and append to menu */
        synschemeMi = gtk_menu_item_new_with_label (sorted[i]);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), synschemeMi);

        /* connect menuitem callback, where menuitem used to set scheme */
        g_signal_connect (G_OBJECT (synschemeMi), "activate",
                        G_CALLBACK (sourceview_set_syntax_style), data);
    }

    /* create syntaxt styles combobox for toolbar */
    app->stylelist = gtk_combo_box_text_new ();

    /* create entries for combobox */
    for (gint i = 0; i < n; i++) {

        /* append sorted entry to combo box */
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(app->stylelist),
                                        sorted[i]);
        /* connect callback */
        g_signal_connect (app->stylelist, "changed",
                        G_CALLBACK (sourceview_list_set_syntax_style), app);
    }

    g_free (sorted);    /* free sorted list of styles */

    return menu;
}

#endif
