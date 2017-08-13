#include "gtk_appdata.h"

/* GError check, free and reset for keyfile read. */
static gboolean chk_key_ok (GError **err)
{
    if (*err != NULL)
    {
        g_print ("key_file err->code: %s\n", (*err)->message);
        g_error_free (*err);
        *err = NULL;
        return FALSE;
    }
    return TRUE;
}

static void set_data_dirs (kwinst *app)
{
    gchar *usrdata = get_posix_filename (g_get_user_data_dir());
    app->usrdatadir = g_strdup_printf ("%s/%s", usrdata, CFGDIR);
    g_free (usrdata);
#ifndef HAVEMSWIN
    app->sysdatadir = g_strdup_printf ("%s", NIXSHARE);
#else
    app->sysdatadir = g_strdup_printf ("%s", WINPRG);
#endif
}

/* set default values for application & initialize variables */
static void context_set_defaults (kwinst *app, char **argv)
{
    app->exename        = get_posix_filename (argv[0]);     /* executable name */
    app->user           = g_get_user_name ();               /* system username */
    set_data_dirs (app);
#ifdef DEBUG
g_print ("app->exename    : %s\n"
         "app->user       : %s\n"
         "app->usrdatadir : %s\n"
         "app->sysdatadir : %s\n",
         app->exename, app->user, app->usrdatadir, app->sysdatadir);
#endif
    app->window         = NULL;     /* main window pointer */
    app->winwidth       = 720;      /* default window width  */
    app->winheight      = 740;      /* default window height */
    app->winrestore     = FALSE;    /* restore window size */

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
    app->pgudmvscsr     = TRUE;     /* PgUp/PgDn keys move cursor */
    app->indentwspc     = TRUE;     /* indent w/spaces not tabs */
    app->indentmixd     = FALSE;    /* Emacs mode indent w/mixed spc/tabs */
    app->indentauto     = TRUE;     /* auto-indent on return */
    app->posixeof       = TRUE;     /* insure POSIX end of line on save */
    app->trimendws      = TRUE;     /* (bug with Quit) remove trailing ws on save */
#ifdef HAVESOURCEVIEW
    app->langmgr        = gtk_source_language_manager_get_default();
    app->language       = NULL;
//     app->schememgr      = gtk_source_style_scheme_manager_get_default();
//     app->schemeids      = gtk_source_style_scheme_manager_get_scheme_ids (app->schememgr);
    app->highlight      = TRUE;     /* show syntax highlight */
    app->lineno         = FALSE;    /* show line numbers (sourceview) */
    app->linehghlt      = TRUE;     /* highlight current line */
    app->laststyle      = NULL;
#endif
#ifndef HAVEMSWIN
    app->eol            = LF;       /* default line end LF */
#else
    app->eol            = CRLF;     /* default line end CRLF */
#endif
    app->oeol           = app->eol; /* original end-of-line (for conversions) */
    app->eolchg         = FALSE;    /* no eol change until file open or user selects */
    app->eolstr[0]      = EOL_LF;   /* eol ending strings */
    app->eolstr[1]      = EOL_CRLF;
    app->eolstr[2]      = EOL_CR;
    app->eolnm[0]       = EOLNM_LF; /* eol string names */
    app->eolnm[1]       = EOLNM_CRLF;
    app->eolnm[2]       = EOLNM_CR;

    app->filename       = NULL;     /* full filename */
    app->fname          = NULL;     /* base filename w/o ext */
    app->fext           = NULL;     /* filename extension */
    app->fpath          = NULL;     /* path components of fname */
    app->modified       = FALSE;    /* need save if not 0  */
    app->fsize          = 0;        /* resulting file size */
    app->filemode       = 0;        /* existing file mode */
    app->fileuid        = 0;        /* existing file UID */
    app->filegid        = 0;        /* existing file GID */
                                    /* initial font name */
    app->fontname       = g_strdup ("monospace 8");
    app->line           = 0;        /* initialize beginning pos line/col  */
    app->lines          = 1;
    app->col            = 0;

    app->indent         = 0;        /* first non-space/tab char in line   */
    app->indentpl       = 0;        /* prev line indent */
    app->indentlevel    = 0;        /* will normally be in initialize fn  */
    app->overwrite = FALSE;         /* ins/overwrite mode flag */

    app->appname        = APPSTR;   /* app name, e.g. "GTKwrite Text Editor" */
    app->appshort       = APPSHORT; /* short name, e.g. "GTKwrite" */
    app->comment        = g_strdup ("// ");

    app->new_pos        = NULL;     /* Goto mark, no sep init */

    app->printsettings  = NULL;     /* initialize print settings */
    app->printpgsetup   = NULL;     /* initialize page setup */
    app->margintop      = 0.5;      /* default margins */
    app->marginbottom   = 0.5;
    app->marginleft     = 1.0;
    app->marginright    = 0.5;

    app->filemon        = NULL;     /* GFileMonitor pointer */
    app->filemonfn      = NULL;     /* filename (full path) being monitored */
    app->mfp_savecmd    = FALSE;    /* flag causing mod events to be ignored */
    app->mfp_handler    = 0;        /* signal_handler_id for foreign mod watch */

    app->cfgdir         = NULL;     /* user config dir */
    app->cfgfile        = NULL;     /* user config file */
    app->keyfile        = NULL;

}

/** read saved values from key_file, if it exists. */
static void context_read_keyfile (kwinst *app)
{
    get_user_cfgfile (app);

    if (!g_file_test (app->cfgfile, G_FILE_TEST_EXISTS))
        return;

    if (!g_key_file_load_from_file (app->keyfile, app->cfgfile,
                            G_KEY_FILE_NONE, NULL))
        return;

    gchar       *sv;
    gboolean    bv;
    gint        iv;
    gdouble     dv;
    GError *err = NULL;

    /** initialize "appearance" values from keyfile */
    bv = g_key_file_get_boolean (app->keyfile, "appearance",
                                        "winrestore", &err);
    if (chk_key_ok (&err)) app->winrestore = bv;

    if (app->winrestore) {
        iv = g_key_file_get_integer (app->keyfile, "appearance",
                                            "winwidth", &err);
        if (chk_key_ok (&err)) app->winwidth = iv;

        iv = g_key_file_get_integer (app->keyfile, "appearance",
                                            "winheight", &err);
        if (chk_key_ok (&err)) app->winheight = iv;
    }

    bv = g_key_file_get_boolean (app->keyfile, "appearance",
                                        "showtoolbar", &err);
    if (chk_key_ok (&err)) app->showtoolbar = bv;

    bv = g_key_file_get_boolean (app->keyfile, "appearance",
                                        "showtabs", &err);
    if (chk_key_ok (&err)) app->showtabs = bv;

    bv = g_key_file_get_boolean (app->keyfile, "appearance",
                                        "showdwrap", &err);
    if (chk_key_ok (&err)) app->showdwrap = bv;

    sv = g_key_file_get_string (app->keyfile, "appearance",
                                        "fontname", &err);
    if (chk_key_ok (&err)) {
        if (app->fontname)
            g_free (app->fontname);
        app->fontname = sv;
    }

    /** initialize "editor" values from keyfile */
    iv = g_key_file_get_integer (app->keyfile, "editor",
                                        "tabstop", &err);
    if (chk_key_ok (&err)) app->tabstop = iv;

    iv = g_key_file_get_integer (app->keyfile, "editor",
                                        "softtab", &err);
    if (chk_key_ok (&err)) app->softtab = iv;

    sv = g_key_file_get_string (app->keyfile, "editor",
                                        "tabstring", &err);
    if (chk_key_ok (&err)) app->tabstring = sv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "expandtab", &err);
    if (chk_key_ok (&err)) app->expandtab = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "smartbs", &err);
    if (chk_key_ok (&err)) app->smartbs = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "smarthe", &err);
    if (chk_key_ok (&err)) app->smarthe = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "dynwrap", &err);
    if (chk_key_ok (&err)) app->dynwrap = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "wraptxtcsr", &err);
    if (chk_key_ok (&err)) app->wraptxtcsr = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "pgudmvscsr", &err);
    if (chk_key_ok (&err)) app->pgudmvscsr = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "indentwspc", &err);
    if (chk_key_ok (&err)) app->indentwspc = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "indentmixd", &err);
    if (chk_key_ok (&err)) app->indentmixd = bv;

    bv = g_key_file_get_boolean (app->keyfile, "editor",
                                        "indentauto", &err);
    if (chk_key_ok (&err)) app->indentauto = bv;

    /** initialize "cleanup" values from keyfile */
    bv = g_key_file_get_boolean (app->keyfile, "cleanup",
                                        "posixeof", &err);
    if (chk_key_ok (&err)) app->posixeof = bv;

    bv = g_key_file_get_boolean (app->keyfile, "cleanup",
                                        "trimendws", &err);
    if (chk_key_ok (&err)) app->trimendws = bv;

#ifdef HAVESOURCEVIEW
    /** initialize "sourceview" values from keyfile */
    bv = g_key_file_get_boolean (app->keyfile, "sourceview",
                                        "highlight", &err);
    if (chk_key_ok (&err)) app->highlight = bv;

    bv = g_key_file_get_boolean (app->keyfile, "sourceview",
                                        "lineno", &err);
    if (chk_key_ok (&err)) app->lineno = bv;

    bv = g_key_file_get_boolean (app->keyfile, "sourceview",
                                        "linehghlt", &err);
    if (chk_key_ok (&err)) app->linehghlt = bv;

    if (g_key_file_has_key (app->keyfile, "sourceview",
                            "laststyle", &err))
    {
        sv = g_key_file_get_string (app->keyfile, "sourceview",
                                            "laststyle", &err);
        if (chk_key_ok (&err)) app->laststyle = sv;
    }
#endif
    /** initialize "print" values from keyfile */
    dv = g_key_file_get_double (app->keyfile, "print",
                                        "margintop", &err);
    if (chk_key_ok (&err)) app->margintop = dv;

    dv = g_key_file_get_double (app->keyfile, "print",
                                        "marginbottom", &err);
    if (chk_key_ok (&err)) app->marginbottom = dv;

    dv = g_key_file_get_double (app->keyfile, "print",
                                        "marginleft", &err);
    if (chk_key_ok (&err)) app->marginleft = dv;

    dv = g_key_file_get_double (app->keyfile, "print",
                                        "marginright", &err);
    if (chk_key_ok (&err)) app->marginright = dv;

}

#ifndef HAVEKEYFILE
/** general use err_dialog, just pass errmsg. */
static void err_dialog (const gchar *errmsg)
{
        GtkWidget *dialog;

        g_warning (errmsg); /* log to terminal window */

        /* create an error dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL,
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         errmsg);

        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}
#endif

/** write settings to key_file, key_file created if it doesn't exist.
 *  (default location $HOME/.config/gtkwrite/gtkwrite.cfg)
 */
static void context_write_keyfile (kwinst *app)
{
    if (!app->keyfile || !app->cfgfile) return;

    if (g_mkdir_with_parents (app->cfgdir, 0755)) {
        g_print ("error: directory creation failed '%s'\n", app->cfgdir);
        return;
    }

    g_key_file_set_boolean (app->keyfile, "appearance", "winrestore", app->winrestore);
    g_key_file_set_integer (app->keyfile, "appearance", "winwidth", app->winwidth);
    g_key_file_set_integer (app->keyfile, "appearance", "winheight", app->winheight);
    g_key_file_set_boolean (app->keyfile, "appearance", "showtoolbar", app->showtoolbar);
    g_key_file_set_boolean (app->keyfile, "appearance", "showtabs", app->showtabs);
    g_key_file_set_boolean (app->keyfile, "appearance", "showdwrap", app->showdwrap);
    g_key_file_set_string  (app->keyfile, "appearance", "fontname", app->fontname);
    g_key_file_set_integer (app->keyfile, "editor", "tabstop", app->tabstop);
    g_key_file_set_integer (app->keyfile, "editor", "softtab", app->softtab);
    g_key_file_set_string  (app->keyfile, "editor", "tabstring", app->tabstring);
    g_key_file_set_boolean (app->keyfile, "editor", "expandtab", app->expandtab);
    g_key_file_set_boolean (app->keyfile, "editor", "smartbs", app->smartbs);
    g_key_file_set_boolean (app->keyfile, "editor", "smarthe", app->smarthe);
    g_key_file_set_boolean (app->keyfile, "editor", "dynwrap", app->dynwrap);
    g_key_file_set_boolean (app->keyfile, "editor", "wraptxtcsr", app->wraptxtcsr);
    g_key_file_set_boolean (app->keyfile, "editor", "pgudmvscsr", app->pgudmvscsr);
    g_key_file_set_boolean (app->keyfile, "editor", "indentwspc", app->indentwspc);
    g_key_file_set_boolean (app->keyfile, "editor", "indentmixd", app->indentmixd);
    g_key_file_set_boolean (app->keyfile, "editor", "indentauto", app->indentauto);
    g_key_file_set_boolean (app->keyfile, "cleanup", "posixeof", app->posixeof);
    g_key_file_set_boolean (app->keyfile, "cleanup", "trimendws", app->trimendws);
#ifdef HAVESOURCEVIEW
    g_key_file_set_boolean (app->keyfile, "sourceview", "highlight", app->highlight);
    g_key_file_set_boolean (app->keyfile, "sourceview", "lineno", app->lineno);
    g_key_file_set_boolean (app->keyfile, "sourceview", "linehghlt", app->linehghlt);
    if (app->laststyle)
        g_key_file_set_string  (app->keyfile, "sourceview", "laststyle",
                                app->laststyle);
#endif
    g_key_file_set_double (app->keyfile, "print", "margintop", app->margintop);
    g_key_file_set_double (app->keyfile, "print", "marginbottom", app->marginbottom);
    g_key_file_set_double (app->keyfile, "print", "marginleft", app->marginleft);
    g_key_file_set_double (app->keyfile, "print", "marginright", app->marginright);

#ifdef HAVEKEYFILE
    if (!g_key_file_save_to_file (app->keyfile, app->cfgfile, NULL))
        g_print ("error: saving keyfile failed '%s'\n", app->cfgfile);
#else
    GError *err=NULL;
    gchar *kfd;
    gsize len;

    kfd = g_key_file_to_data (app->keyfile, &len, NULL);

    if (!g_file_set_contents (app->cfgfile, kfd, -1, &err)) {
        err_dialog (err->message);
        g_error_free (err);
    }

    if (kfd) g_free (kfd);
#endif

}

/** set default values and read save values from key_file.
 *  (wrapper for functions above)
 */
void context_init (kwinst *app, char **argv)
{
    /* load default values */
    context_set_defaults (app, argv);

    /* create an empty key_file */
    app->keyfile = g_key_file_new();

    /* read key_file values from file
     * (default $HOME/.config/gtkwrite/gtkwrite.cfg)
     */
    context_read_keyfile (app);

    /* initialize find/replace values */
    findrep_init (app);
}

/* initialize find/replace variables and values. */
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

/** free allocated memory on exit */
void context_destroy (kwinst *app)
{
    /* save settings to keyfile */
    context_write_keyfile (app);

    /* free allocated struct values */
    app_free_filename (app);
    if (app->exename)       g_free (app->exename);
    if (app->usrdatadir)    g_free (app->usrdatadir);
    if (app->sysdatadir)    g_free (app->sysdatadir);
    if (app->fontname)      g_free (app->fontname);

    // if (app->appname) g_free (app->appname);
    // if (app->appshort) g_free (app->appshort);

    if (app->tabstring)     g_free (app->tabstring);

    if (app->cfgdir)        g_free (app->cfgdir);
    if (app->cfgfile)       g_free (app->cfgfile);

    if (app->keyfile)       g_key_file_free (app->keyfile);

    /* free find/replace GList memory */
    findrep_destroy (app);
}

/** free memory allocated to find/replace on exit */
void findrep_destroy (kwinst *app)
{
    guint i;

    /* free combobox lists */
    for (i = 0; i < app->nfentries; i++) g_free (app->findtext[i]);
    g_free (app->findtext);

    for (i = 0; i < app->nrentries; i++) g_free (app->reptext[i]);
    g_free (app->reptext);

}

/* cleaner than including filebuf.h
 * TODO: move all common utility functions here.
 */
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

/** split app->filename into path, filename, extension.
 *  this can be called numerous times during editing, so
 *  the struct is passed and all components are updated.
 */
void split_fname (kwinst *app)
{
    if (!app->filename) return;
    gchar *ep = app->filename;
    gchar *sp = ep;
    gchar *p = ep;

    /* free memory for existing components */
    if (app->fname) g_free (app->fname);
    if (app->fext)  g_free (app->fext);
    if (app->fpath) g_free (app->fpath);
    app->fname = app->fext = app->fpath = NULL;

    for (; *ep; ep++);  /* end of string */

    /* separate extension */
    p = ep;
    while (*sp == '.') sp++;            /* handle dot files */
    for (; p > sp && *p != '.'; p--);   /* find last '.' */

    if (p != sp)    /* not dot file with no extension */
        app->fext = g_strdup (p + 1);   /* set fext */

    p = ep; /* separate path */
    for (; p > app->filename && *p != '/'; p--);

    if (p == app->filename) {
        if (*p == '/') {    /* handle root path */
            app->fname = g_strdup (app->filename + 1);
            app->fpath = g_strdup ("/");
        }
        else    /* no path */
            app->fname = g_strdup (app->filename);
        return;
    }

    /* separate normal /path/filename */
    app->fname = g_strdup (p + 1);
    app->fpath = g_strndup (app->filename, p - app->filename);
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

/* TODO - win32 check URI conversion */
/** convert uri to filename.
 *  returns pointer to beginning of filename within uri.
 */
gchar *uri_to_filename (const gchar *uri)
{
    if (!uri || !*uri) return NULL;

    gchar *p = (gchar *)uri;
    gsize len = g_strlen (uri);

    if (len < 4) return NULL;
    /* TODO - check beginning of URI and adjust action based on content */
    for (p += 2; *p; p++) {
        if (*(p - 2) == '/' && *(p - 1) == '/' && *p == '/')
            return p;
    }

    return NULL;
}

/* TODO - get_user_cfgfile() add error checks */
/**  form key_file filename from config_dir and defines
 */
char *get_user_cfgfile (kwinst *app)
{
    gchar *cfgdir;

    if ((cfgdir = (gchar *)g_get_user_config_dir())) {
        app->cfgdir = g_strdup_printf ("%s/%s", cfgdir, CFGDIR);
        if (!app->cfgdir)
            return NULL;
        return (app->cfgfile =
                g_strdup_printf ("%s/%s", app->cfgdir, CFGFILE));
    }

    return NULL;
}

/** delete the last_pos mark */
void delete_mark_last_pos (kwinst *app)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(app->buffer);

    if (app->last_pos)
        gtk_text_buffer_delete_mark (buffer, app->last_pos);

    app->last_pos = NULL;
}

/** get_posix_filename returns a pointer to an allocated POSIX filename.
 *  the user is responsible for calling g_free on the return.
 */
gchar *get_posix_filename (const gchar *fn)
{
    if (!fn) return NULL;

    gsize len = g_strlen (fn);
    gchar *posixfn = g_malloc0 (len * 2),
        *wp = posixfn;

    if (!wp) {
        /* handle error */
        return NULL;
    }

    /* convert fn to POSIX filename format in posixfn */
    for (const gchar *p = fn; *p; p++) {
        if (*p == '\\')
            *wp++ = '/';        /* replace backslash with forward slash */
        else if (*p == ' ') {
            *wp++ = '\\';       /* escape before all spaces */
            *wp++ = *p;
        }
        else
            *wp++ = *p;         /* copy remaining chars unchanged */
    }
    *wp = 0;

    return posixfn;
}

/** create new instance of editor window with file 'fn'.
 *  check filename in argv[0] and reformat to POSIX filename
 *  by replaceing forwardslash with backslash and escaping
 *  any spaces before calling g_spawn_command_line_async.
 *  if 'fn' NULL, create with empty buffer, otherwise open
 *  'fn' in new instance.
 */
gboolean create_new_editor_inst (kwinst *app, gchar *fn)
{
    GError *err = NULL;
//     gchar *exename = get_posix_filename (app->exename);
    gboolean result;

    if (fn) {   /* form cmdline with 'posixfn' as 1st argument */
        gchar *cmdline = g_strdup_printf ("%s %s", app->exename, fn);

        /* spawn a new instance with cmdline */
        result = g_spawn_command_line_async (cmdline, &err);
        g_free (cmdline);
    }
    else        /* open new instance with empty buffer */
        result = g_spawn_command_line_async (app->exename, &err);

//     g_free (exename);

    return result;
}
