#include "gtk_completionsv.h"

gchar stub (void)   /* NOP to prevent empty compilation unit */
{ return 0; }       /* when compiling without GtkSourceView */

#ifdef HAVESOURCEVIEW

/* GtkSourceCompletionProvider class interface implementation, for basic
 * word-completion for existing words within the current document.
 */

typedef struct _cmplprovider cmplprovider;
typedef struct _cmplproviderClass cmplproviderClass;

struct _cmplprovider
{
    GObject parent;

    GList *proposals;
    gint priority;
    gchar *name;

    GdkPixbuf *icon;
};

struct _cmplproviderClass
{
    GObjectClass parent_class;
};

/* prototype of '..._iface_init' function to define provider '...' */
static void cmpl_provider_iface_init (GtkSourceCompletionProviderIface *iface);

/* struct type definition */
G_DEFINE_TYPE_WITH_CODE (cmplprovider, cmpl_provider, G_TYPE_OBJECT,
                    G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
                                            cmpl_provider_iface_init))

/* member function - _get_name */
static gchar *cmpl_provider_get_name (GtkSourceCompletionProvider *provider)
{
    return g_strdup (((cmplprovider *)provider)->name);
}

/* member funciton - _get_priority */
static gint cmpl_provider_get_priority (GtkSourceCompletionProvider *provider)
{
    return ((cmplprovider *)provider)->priority;
}

/* member function - _match */
static gboolean cmpl_provider_match (GtkSourceCompletionProvider *provider,
                                    GtkSourceCompletionContext  *context)
{
    return TRUE;

    if (provider || context) {}
}

/* member function - _populate */
static void cmpl_provider_populate (GtkSourceCompletionProvider *provider,
                                    GtkSourceCompletionContext  *context)
{
    gtk_source_completion_context_add_proposals (context, provider,
                                    ((cmplprovider *)provider)->proposals,
                                    TRUE);
}

/* initialize provider interface with member functions */
static void cmpl_provider_iface_init (GtkSourceCompletionProviderIface *iface)
{
    iface->get_name = cmpl_provider_get_name;

    iface->populate = cmpl_provider_populate;
    iface->match = cmpl_provider_match;
    iface->get_priority = cmpl_provider_get_priority;

    // iface->get_icon = cmpl_provider_get_icon;
    iface->get_icon = NULL;
}

/* struct init */
static void cmpl_provider_class_init (cmplproviderClass *klass)
{
    if (klass) {}
}

/* provider member init */
static void cmpl_provider_init (cmplprovider *self)
{
    GList *proposals = NULL;
    self->proposals = proposals;
}

/* create the word completion object for the source_view */
void create_completion (kwinst *app)
{
    GtkSourceCompletionWords *prov_words;

    app->completion = gtk_source_view_get_completion (GTK_SOURCE_VIEW (app->view));

    prov_words = gtk_source_completion_words_new (NULL, NULL);

    gtk_source_completion_words_register (prov_words,
                                            gtk_text_view_get_buffer (GTK_TEXT_VIEW (app->view)));

    gtk_source_completion_add_provider (app->completion,
                                        GTK_SOURCE_COMPLETION_PROVIDER (prov_words),
                                        NULL);

    g_object_set (prov_words, "minimum-word-size", 3, NULL);
//     g_object_set (prov_words, "priority", 10, NULL);
}

#endif
