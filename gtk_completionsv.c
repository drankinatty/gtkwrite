#include "gtk_completionsv.h"

gchar stub (void)   /* NOP to prevent empty compilation unit */
{ return 0; }       /* when compiling without GtkSourceView */

#ifdef HAVESOURCEVIEW

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

static void cmpl_provider_iface_init (GtkSourceCompletionProviderIface *iface);

G_DEFINE_TYPE_WITH_CODE (cmplprovider, cmpl_provider, G_TYPE_OBJECT,
                    G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_PROVIDER,
                                            cmpl_provider_iface_init))

static gchar *cmpl_provider_get_name (GtkSourceCompletionProvider *provider)
{
    return g_strdup (((cmplprovider *)provider)->name);
}

static gint cmpl_provider_get_priority (GtkSourceCompletionProvider *provider)
{
    return ((cmplprovider *)provider)->priority;
}

static gboolean cmpl_provider_match (GtkSourceCompletionProvider *provider,
                                    GtkSourceCompletionContext  *context)
{
    return TRUE;

    if (provider || context) {}
}

static void cmpl_provider_populate (GtkSourceCompletionProvider *provider,
                                    GtkSourceCompletionContext  *context)
{
    gtk_source_completion_context_add_proposals (context, provider,
                                    ((cmplprovider *)provider)->proposals,
                                    TRUE);
}

static void cmpl_provider_iface_init (GtkSourceCompletionProviderIface *iface)
{
    iface->get_name = cmpl_provider_get_name;

    iface->populate = cmpl_provider_populate;
    iface->match = cmpl_provider_match;
    iface->get_priority = cmpl_provider_get_priority;

    // iface->get_icon = cmpl_provider_get_icon;
    iface->get_icon = NULL;
}

static void cmpl_provider_class_init (cmplproviderClass *klass)
{
    if (klass) {}
}

static void cmpl_provider_init (cmplprovider *self)
{
    GList *proposals = NULL;
    self->proposals = proposals;
}

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

//     g_object_set (prov_words, "priority", 10, NULL);
}

#endif
