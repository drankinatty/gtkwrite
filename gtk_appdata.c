#include "gtk_appdata.h"

/** app_free_filename, free all filename components. */
void app_free_filename (context *app)
{
    if (app->filename) g_free (app->filename);
    if (app->fname) g_free (app->fname);
    if (app->fext) g_free (app->fext);
    if (app->fpath) g_free (app->fpath);
    app->filename = NULL;
    app->fname = NULL;
    app->fext = NULL;
    app->fpath = NULL;
}

