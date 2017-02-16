#include "gtk_appdata.h"

/*
 * general window functions
 */
void context_init (context *app)
{
    app->window     = NULL;     /* initialize struct values */
    app->view       = NULL;     /* text_view widget for app */
    app->statusbar  = NULL;     /* statusbar widget */
    app->cid        = 0;        /* context id for statusbar */
    app->tagtable   = NULL;     /* tagtable for text_view */
    app->tabstop    = 8;        /* number of spaces per tab */
    app->softtab    = 4;        /* soft tab stop size */
    app->tabstring  = NULL;     /* tabstring for indent */
    app->expandtab  = TRUE;     /* insert spaces for tab */
    app->smartbs    = TRUE;     /* use smart backspace */
    app->showtabs   = FALSE;    /* display tabulator markers */
    app->dynwrap    = TRUE;     /* use dynamic word wrap */
    app->showdwrap  = FALSE;    /* use dynamic word wrap */
    app->smarthe    = TRUE;     /* smart home & end cursor */
    app->wraptxtcsr = TRUE;     /* wrap cursor to next line */
    app->pgudmvscsr = FALSE;    /* PgUp/PgDn keys move cursor */
    app->indentwspc = TRUE;     /* indent w/spaces not tabs */
    app->indentmixd = FALSE;    /* Emacs mode indent w/mixed spc/tabs */
    app->posixeol   = TRUE;     /* insure POSIX end of line on save */
    app->trimendws  = TRUE;     /* remove trailing ws on save */

    app->filename   = NULL;     /* full filename */
    app->fname      = NULL;     /* base filename w/o ext */
    app->fext       = NULL;     /* filename extension */
    app->fpath      = NULL;     /* path components of fname */
    app->open       = 0;        /* flag - is file open */
    app->modified   = FALSE;    /* need save if not 0  */
    app->fsize      = 0;        /* resulting file size */

    app->fontname   = NULL;     /* initial font name */

    app->appname    = g_strdup ("GTKwrite Text Editor");
    app->appshort   = g_strdup ("GTKwrite");

    app->new_pos    = NULL;     /* Goto mark, no sep init */

    findrep_init (app);
}

void findrep_init (context *app)
{
    app->findrepwin = NULL; /* initialize widgets to NULL */
    app->dlgid      = 0;
    app->entryfind  = NULL;
    app->entryreplace = NULL;
    app->btnregex   = NULL;
    app->btnplace   = NULL;
    app->chkregex   = NULL;
    app->chkplace   = NULL;
    app->chkcase    = NULL;
    app->chkwhole   = NULL;
    app->chkfrom    = NULL;
    app->chkback    = NULL;
    app->chkselect  = NULL;
    app->chkprompt  = NULL;  /* allocate array of pointers */
    app->findtext   = g_malloc0 (MAXLE * sizeof *(app->findtext));
    app->reptext    = g_malloc0 (MAXLE * sizeof *(app->reptext));
    app->nfentries  = 0;
    app->nrentries  = 0;
    app->fmax       = MAXLE;
    app->rmax       = MAXLE;
    app->optregex   = TRUE;  /* initial checkbox states */
    app->optplace   = FALSE;
    app->optcase    = TRUE;
    app->optwhole   = FALSE;
    app->optfrom    = FALSE;
    app->optback    = FALSE;
    app->optselect  = FALSE;
    app->optprompt  = FALSE;
    app->findcbchgd = FALSE;
    app->replcbchgd = FALSE;

    app->txtfound   = FALSE;
    app->last_pos   = NULL;
    app->selstart   = NULL;
    app->selend     = NULL;

    if (!(app->findtext && app->reptext)) {
        // err_dialog ("findrep_init()\nvirtual memory exhausted.");
        g_print ("findrep_init()\nvirtual memory exhausted.");
        // findrep_destroy (app);
        // context_destroy (app);
        gtk_main_quit();
    }
}

void context_destroy (context *app)
{   /* free allocated struct values */
    app_free_filename (app);
    if (app->fontname) g_free (app->fontname);

    if (app->appname) g_free (app->appname);
    if (app->appshort) g_free (app->appshort);

    if (app->tabstring) g_free (app->tabstring);

    findrep_destroy (app);
}

void findrep_destroy (context *app)
{   /* free allocated struct values */
    guint i;    /* free combobox lists */
    for (i = 0; i < app->nfentries; i++) g_free (app->findtext[i]);
    g_free (app->findtext);
    for (i = 0; i < app->nrentries; i++) g_free (app->reptext[i]);
    g_free (app->reptext);

    /* temp dump of options */
#ifdef DEBUGFR
    dumpopts (app);
#endif
}

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

