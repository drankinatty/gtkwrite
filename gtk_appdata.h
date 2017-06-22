#ifndef _gtk_appdata_h_
#define _gtk_appdata_h_  1

#define MAXLE 32

#include <glib.h>
#include <gtk/gtk.h>

#if GLIB_MAJOR_VERSION >= 2
#if GLIB_MINOR_VERSION >= 40
 #define GLIB240 1
 #define HAVEKEYFILE 1
#endif
#if GLIB_MINOR_VERSION >= 36
 #define GLIB236 1
#endif
#if GLIB_MINOR_VERSION >= 30
 #define GLIB230 1
#endif
#endif

#ifdef WGTKSOURCEVIEW2
 #include <gtksourceview/gtksourceview.h>
 #include <gtksourceview/gtksourcelanguagemanager.h>
#elif defined WGTKSOURCEVIEW3
 #include <gtksourceview/gtksource.h>
#endif

#if defined (WGTKSOURCEVIEW2) || defined (WGTKSOURCEVIEW3)
 #define HAVESOURCEVIEW 1
 #define APPSTR "GTKwrite Text Editor"
 #define APPSHORT "GTKwrite"
#else
 #define APPSTR "GTKedit Text Editor"
 #define APPSHORT "GTKedit"
#endif

#if defined (_WIN32) || defined (_WIN64)
 #define HAVEMSWIN 1
#endif

#define VER "0.1.3"
#define SITE "https://www.rankinlawfirm.com"
#define CFGDIR  "gtkwrite"
#define CFGFILE "gtkwrite.cfg"

/* TODO:
 *  look at adding app->status to remove include gtk_statusbar.h
 *  dependency in gtk_filebuf.h.
 */

typedef struct {
    GtkWidget           *window;        /* main window      */
    GtkWidget           *view;          /* text_view        */
    GtkWidget           *menubar;       /* menubar          */
    GtkWidget           *toolbar;       /* toolbar          */
    GtkWidget           *tbappearMi;    /* toolbar Menu vis */
    GtkWidget           *statusbar;     /* window statusbar */
    GtkTextTagTable     *tagtable;      /* buffer tagtable  */
    gchar               *appname;       /* application name */
    gchar               *appshort;      /* short app name   */
    gchar               *filename;      /* filename w/path  */
    gchar               *fname;         /* filename only    */
    gchar               *fext;          /* file extension   */
    gchar               *fpath;         /* file path        */
    gsize               fsize;          /* file size        */
    guint               filemode;       /* file mode        */
    guint               fileuid;        /* file UID         */
    guint               filegid;        /* file GID         */
    gboolean            modified;       /* buffer modified  */
    gboolean            showtoolbar;    /* toolbar visible  */
    guint               cid;            /* sb kwinst id     */
    guint               dlgid;          /* dialog id        */

    gint                winwidth;       /* window width     */
    gint                winheight;      /* window height    */
    gboolean            winrestore;     /* restore win size */

    /* text view settings */
    /*
    gint            indent;
    PangoTabArray   *tabs;
    */
    gchar           *fontname;

#ifdef HAVESOURCEVIEW
    GtkSourceBuffer *buffer;
    GtkSourceLanguageManager    *langmgr;
    GtkSourceLanguage           *language;
//     GtkSourceStyleSchemeManager *schememgr;
//     const gchar * const *schemeids;
    GtkWidget       *stylelist;
    GtkWidget       *hghltmenu;
    gboolean        highlight;
    gboolean        lineno;
    gboolean        linehghlt;
    gchar           *laststyle;
#else
    GtkTextBuffer   *buffer;
#endif
    GtkTextMark     *cursor;
    gint            line;
    gint            col;
    gint            indent;
    gint            indentpl;
    gint            indentlevel;
    gint            tabstop;
    gint            softtab;
    gchar           *tabstring;
    gboolean        expandtab;
    gboolean        overwrite;
    gboolean        smartbs;
    gboolean        smarthe;
    gboolean        kphome;
    gboolean        showtabs;
    gboolean        dynwrap;
    gboolean        showdwrap;
    gboolean        wraptxtcsr;
    gboolean        pgudmvscsr;
    gboolean        indentwspc;
    gboolean        indentmixd;
    gboolean        indentauto;
    gboolean        posixeof;
    gboolean        trimendws;
    // gboolean tabkeyindt;    /* TODO: tab key indents */

    /* text view status */
    GtkTextMark     *markfrom,      /* operation from mark */
                    *selstart,      /* selection start/end */
                    *selend;

    gchar           *comment;       /* comment string */
    GtkWidget       *cmtentry;      /* comment entry */

    /* find replace dailog data */
    GtkWidget       *findrepwin;    /* main window      */
    GtkWidget       *entryfind;     /* find entry combo */
    GtkWidget       *entryreplace;  /* replace combo    */
    GtkWidget       *btnregex;
    GtkWidget       *btnplace;
    GtkWidget       *btnfind;       /* to allow set sensitive */
    GtkWidget       *btnreplace;
    GtkWidget       *chkregex;
    GtkWidget       *chkplace;
    GtkWidget       *chkcase;
    GtkWidget       *chkwhole;
    GtkWidget       *chkfrom;
    GtkWidget       *chkback;
    GtkWidget       *chkselect;
    GtkWidget       *chkprompt;
    gchar           **findtext;     /* search term      */
    gchar           **reptext;      /* replace term     */
    guint           nfentries;
    guint           nrentries;
    guint           fmax;
    guint           rmax;
    gboolean        optregex;
    gboolean        optplace;
    gboolean        optcase;
    gboolean        optwhole;
    gboolean        optfrom;
    gboolean        optback;
    gboolean        optselect;
    gboolean        optprompt;
    gboolean        findcbchgd;      /* find combo box changed */
    gboolean        replcbchgd;      /* replace combo box changed */

    /* find replace results */
    gboolean        txtfound;       /* prev search found text */
    GtkTextMark     *last_pos;      /* position of last match in buf */

    /* goto dialog data */
    GtkWidget       *gotowin;       /* main window      */
    GtkWidget       *vscale;        /* vertical scale   */
    GtkWidget       *spinbtn;       /* spin button      */
    GtkTextMark     *new_pos;       /* goto position    */

    /* settings dialog */
    GtkWidget       *settingswin;   /* settings notebook window */

    /* print data */
    GtkPrintSettings    *printsettings;
    GtkPageSetup        *printpgsetup;
    gdouble             margintop;      /* top    print margin */
    gdouble             marginbottom;   /* bottom print margin */
    gdouble             marginleft;     /* left   print margin */
    gdouble             marginright;    /* right  print margin */

    /* config data */
    gchar               *cfgdir;        /* user config dir */
    gchar               *cfgfile;       /* user config file */
    GKeyFile            *keyfile;       /* key_file for config */

} kwinst;

void context_init (kwinst *app);
void findrep_init (kwinst *app);
void context_destroy (kwinst *app);
void findrep_destroy (kwinst *app);
void app_free_filename (kwinst *app);
gchar *uri_to_filename (const gchar *uri);
char *get_user_cfgfile (kwinst *app);

#endif
