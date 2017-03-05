#include "gtk_appdata.h"

/*
 * general window functions
 */
void context_init (kwinst *app)
{
    app->window         = NULL;     /* initialize struct values */
    app->view           = NULL;     /* text_view widget for app */
    app->menubar        = NULL;     /* menubar widget */
    app->toolbar        = NULL;     /* toolbar widget */
    app->showtoolbar    = TRUE;     /* toolbar is visible */
    app->statusbar      = NULL;     /* statusbar widget */
    app->cid            = 0;        /* kwinst id for statusbar */
    app->tagtable       = NULL;     /* tagtable for text_view */
    app->tabstop        = 8;        /* number of spaces per tab */
    app->softtab        = 4;        /* soft tab stop size */
    app->tabstring      = NULL;     /* tabstring for indent */
    app->expandtab      = TRUE;     /* insert spaces for tab */
    app->smartbs        = TRUE;     /* use smart backspace */
    app->smarthe        = TRUE;     /* smart home & end cursor */
    app->kphome         = FALSE;    /* previous key press was home */
    app->showtabs       = FALSE;    /* display tabulator markers */
    app->dynwrap        = TRUE;     /* use dynamic word wrap */
    app->showdwrap      = FALSE;    /* use dynamic word wrap */
    app->wraptxtcsr     = TRUE;     /* wrap cursor to next line */
    app->pgudmvscsr     = FALSE;    /* PgUp/PgDn keys move cursor */
    app->indentwspc     = TRUE;     /* indent w/spaces not tabs */
    app->indentmixd     = FALSE;    /* Emacs mode indent w/mixed spc/tabs */
    app->indentauto     = TRUE;     /* auto-indent on return */
    app->posixeof       = TRUE;     /* insure POSIX end of line on save */
    app->trimendws      = TRUE;     /* (bug with Quit) remove trailing ws on save */
#ifdef HAVESOURCEVIEW
    app->highlight      = TRUE;     /* show syntax highlight */
    app->lineno         = FALSE;    /* show line numbers (sourceview) */
    app->linehghlt      = TRUE;     /* highlight current line */
#endif
    app->filename       = NULL;     /* full filename */
    app->fname          = NULL;     /* base filename w/o ext */
    app->fext           = NULL;     /* filename extension */
    app->fpath          = NULL;     /* path components of fname */
    app->modified       = FALSE;    /* need save if not 0  */
    app->fsize          = 0;        /* resulting file size */
    app->filemode       = 0;        /* existing file mode */
    app->fileuid        = 0;        /* existing file UID */
    app->filegid        = 0;        /* existing file GID */

    app->fontname       = NULL;     /* initial font name */

    app->appname        = g_strdup ("GTKwrite Text Editor");
    app->appshort       = g_strdup ("GTKwrite");
    app->comment        = g_strdup ("// ");

    app->new_pos        = NULL;     /* Goto mark, no sep init */

    app->printsettings  = NULL;     /* initialize print settings */
    app->printpgsetup   = NULL;     /* initialize page setup */
    app->margintop      = 0.5;      /* default margins */
    app->marginbottom   = 0.5;
    app->marginleft     = 1.0;
    app->marginright    = 0.5;

    findrep_init (app);
}

void findrep_init (kwinst *app)
{
    app->findrepwin     = NULL; /* initialize widgets to NULL */
    app->dlgid          = 0;
    app->entryfind      = NULL;
    app->entryreplace   = NULL;
    app->btnregex       = NULL;
    app->btnplace       = NULL;
    app->chkregex       = NULL;
    app->chkplace       = NULL;
    app->chkcase        = NULL;
    app->chkwhole       = NULL;
    app->chkfrom        = NULL;
    app->chkback        = NULL;
    app->chkselect      = NULL;
    app->chkprompt      = NULL;  /* allocate array of pointers */
    app->findtext       = g_malloc0 (MAXLE * sizeof *(app->findtext));
    app->reptext        = g_malloc0 (MAXLE * sizeof *(app->reptext));
    app->nfentries      = 0;
    app->nrentries      = 0;
    app->fmax           = MAXLE;
    app->rmax           = MAXLE;
    app->optregex       = TRUE;  /* initial checkbox states */
    app->optplace       = FALSE;
    app->optcase        = TRUE;
    app->optwhole       = FALSE;
    app->optfrom        = FALSE;
    app->optback        = FALSE;
    app->optselect      = FALSE;
    app->optprompt      = FALSE;
    app->findcbchgd     = FALSE;
    app->replcbchgd     = FALSE;

    app->txtfound       = FALSE;
    app->last_pos       = NULL;
    app->markfrom       = NULL;
    app->selstart       = NULL;
    app->selend         = NULL;

    if (!(app->findtext && app->reptext)) {
        // err_dialog ("findrep_init()\nvirtual memory exhausted.");
        g_print ("findrep_init()\nvirtual memory exhausted.");
        // findrep_destroy (app);
        // context_destroy (app);
        gtk_main_quit();
    }
}

void context_destroy (kwinst *app)
{   /* free allocated struct values */
    app_free_filename (app);
    if (app->fontname) g_free (app->fontname);

    if (app->appname) g_free (app->appname);
    if (app->appshort) g_free (app->appshort);

    if (app->tabstring) g_free (app->tabstring);

    findrep_destroy (app);
}

void findrep_destroy (kwinst *app)
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
void app_free_filename (kwinst *app)
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

/* cleaner than including filebuf.h */
static gsize g_strlen (const gchar *s)
{
    gsize len = 0;
    for(;;) {
        if (s[0] == 0) return len;
        if (s[1] == 0) return len + 1;
        if (s[2] == 0) return len + 2;
        if (s[3] == 0) return len + 3;
        s += 4, len += 4;
    }
}

/** convert uri to filename.
 *  returns pointer to beginning of filename within uri.
 */
gchar *uri_to_filename (const gchar *uri)
{
    if (!uri || !*uri) return NULL;

    gchar *p = (gchar *)uri;
    gsize len = g_strlen (uri);

    if (len < 4) return NULL;

    for (p += 2; *p; p++) {
        if (*(p - 2) == '/' && *(p - 1) == '/' && *p == '/')
            return p;
    }

    return NULL;
}
