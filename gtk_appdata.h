#ifndef _gtk_appdata_h_
#define _gtk_appdata_h_  1

#define MAXLE 32

#include <glib.h>
#include <gtk/gtk.h>

#if GLIB_MAJOR_VERSION >= 2
#if GLIB_MINOR_VERSION >= 46
 #define GLIB246 1
#endif
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

#ifndef CHAR_BIT
# define CHAR_BIT  8
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

#if defined (_WIN32) || defined (_WIN64) || defined (_WINNT)
 #define HAVEMSWIN 1
#endif

#define VER        "0.2.0"
#define SITE       "https://www.rankinlawfirm.com"
#define LICENSE    "gpl-2.0.txt"
#define CFGDIR     "gtkwrite"
#define CFGFILE    "gtkwrite.cfg"
#define IMGDIR     "img"
#define LOGO       "gtkwrite.png"
#define ICON       "gtkwrite.ico"
#define RCFILE     "gtkrc-2.0_gtkwrite"

/* TODO: dynamically strip 'Program Files' from from app->exename
 * and replace with progra~1 or progra~2 below to preserve correct
 * logo display for 'portable' installs.
 */
#define WINPRG     "c:/progra~1/gtkwrite"
#define WINPRG86   "c:/progra~2/gtkwrite"
#define NIXSHARE   "/usr/share/gtkwrite"

#define EOL_LF          "\n"
#define EOL_CR          "\r"
#define EOL_CRLF        "\r\n"
#define EOL_NO          3
#define EOLNM_LF        "LF"
#define EOLNM_CR        "CR"
#define EOLNM_CRLF      "CRLF"
/* added for settings dialog */
#define EOLTXT_LF       "Linux / Unix / OSX"
#define EOLTXT_CRLF     "DOS / Windows"
#define EOLTXT_CR       "Macintosh (pre-OSX)"
#define EOLTXT_FILE     "Use EOL from File"
#define EOLTXT_OS       "Use OS Default"
#define EOLTXT_NO       5

enum eolorder { LF, CRLF, CR, FILE_EOL, OS_EOL };
enum {  IBAR_VISIBLE = 0x1,
        IBAR_LABEL_SELECT = 0x2,
        IBAR_VIEW_SENSITIVE = 0x4 };    /* infobar flags */
enum { LEFT, RIGHT, STKMAX  = 0x4 };    /* boolean stack constants */

/* TODO:
 *  look at adding app->status to remove include gtk_statusbar.h
 *  dependency in gtk_filebuf.h.
 */

typedef struct {
    gchar               *exename;       /* executable name  */
    const gchar         *user;          /* current username */
    gchar               *usrdatadir;    /* user data dir    */
    gchar               *sysdatadir;    /* system data dir  */
    gchar               *imgdir;        /* image directory  */

    GtkWidget           *window;        /* main window      */
    GtkWidget           *view;          /* text_view        */
    GtkWidget           *menubar;       /* menubar          */
    GtkWidget           *toolbar;       /* toolbar          */
    GtkWidget           *tbappearMi;    /* toolbar Menu vis */
    GtkWidget           *ibarvbox;      /* vbox for infobar */
    GtkWidget           *statusbar;     /* window statusbar */
    GtkTextTagTable     *tagtable;      /* buffer tagtable  */
    guchar              ibflags;        /* infobar bitfield */
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
    gboolean            winszsaved;     /* win size saved   */

    gint                nrecent;        /* n recent files   */

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
    const gchar     *comment_single;    /* single line comment */
    const gchar     *comment_blk_beg;   /* blobk comment begin */
    const gchar     *comment_blk_end;   /* block comment end */
#else
    GtkTextBuffer   *buffer;
#endif
    GtkTextMark     *cursor;
    GtkWidget       *eolLFMi;           /* radio button references */
    GtkWidget       *eolCRLFMi;         /*   for EOL tools-menu    */
    GtkWidget       *eolCRMi;
    gint            eol;                /* end-of-line */
    gint            oeol;               /* original eol */
    gint            eolos;              /* operating system default eol */
    gint            eoldefault;         /* default to override OS or file eol */
    gboolean        eolchg;             /* flag to prevent firing during UI init */
    gchar           *eolnm[EOL_NO];     /* ptrs to eol names */
    gchar           *eolstr[EOL_NO];    /* ptrs to eol strings */
    gchar           *eoltxt[EOLTXT_NO]; /* ptrs to eol descriptions */

    gint            bom;                /* Byte Order Mark */
        /* UTF-8  bom:  0xEF 0xBB 0xBF     (24-bit)
         * UTF-16 bom:  U+FEFF  FE FF      (16-bit Big Endian)
         *                      FF FE      (14-bit Little Endian)
         * (see definitions in gtk_appdata.c)
         */
    gint            line;
    gint            lines;
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
    gboolean        poscurend;
    // gboolean tabkeyindt;    /* TODO: tab key indents */

    gchar           *comment;       /* comment string */
    GtkWidget       *cmtentry;      /* comment entry */

    /* word completion keycnt, and cmpltnchars setting the number where completion dlg shown */
    gint            cmpltnchars;    /* number of chars typed before completion win activates */
    gint            keycnt;         /* counter tracking no. of keypress to reach cmpltnchars */
    gboolean        wrdcmpltn;      /* settings flag to enable/disable word completion */

    /* custom key handler flags */
    gboolean        ctrl_shift_right_fix;   /* select only whitespace or char */

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

    /* find/replace placeholders - TODO move to find/replace data struct */
    GtkTextMark     *markfrom,      /* operation from mark */
                    *selstart,      /* selection start/end */
                    *selend;

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

    /* variables for monitoring file
     * changed by foreign process
     */
    GFileMonitor        *filemon;       /* a pointer to the file monitor instance */
    gchar               *filemonfn;     /* the filename being monitored */
    gboolean            mfp_savecmd;    /* flag causing change events ignored */
    gulong              mfp_handler;    /* holding the file monitor hadler ID */

    /* config data */
    gchar               *cfgdir;        /* user config dir */
    gchar               *cfgfile;       /* user config file */
    GKeyFile            *keyfile;       /* key_file for config */

    /* boolean stack implementation
     * to provide keypress history.
     */
    guint               bstack[STKMAX]; /* sizeof guint * CHAR_BIT * STKMAX bits */
    guint               bindex;         /* bit index */

} kwinst;

void context_init (kwinst *app, char **argv);
void findrep_init (kwinst *app);
void context_destroy (kwinst *app);
void split_fname (kwinst *app);
void findrep_destroy (kwinst *app);
void app_free_filename (kwinst *app);
gchar *uri_to_filename (const gchar *uri);
char *get_user_cfgfile (kwinst *app);
void delete_mark_last_pos (kwinst *app);
gchar *get_posix_filename (const gchar *fn);
gboolean create_new_editor_inst (kwinst *app, gchar *fn);

/* boolean stack functions */
void bstack_clear (kwinst *app);
int bstack_push (kwinst *app, int v);
int bstack_pop (kwinst *app);
int bstack_last (kwinst *app);

/* date & time functions */
gchar *get_local_datetime (void);


#endif
