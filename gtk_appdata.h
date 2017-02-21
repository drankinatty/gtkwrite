#ifndef _gtk_appdata_h_
#define _gtk_appdata_h_  1

#define MAXLE 32

#include <gtk/gtk.h>

#ifdef WGTKSOURCEVIEW2
 #include <gtksourceview/gtksourceview.h>
 #include <gtksourceview/gtksourcelanguagemanager.h>
#elif defined WGTKSOURCEVIEW3
 #include <gtksourceview/gtksource.h>
#endif

#if defined (WGTKSOURCEVIEW2) || defined (WGTKSOURCEVIEW3)
 #define HAVESOURCEVIEW
 #define APPSTR "GTKwrite Text Editor"
#else
 #define APPSTR "GTKedit Text Editor"
#endif

#define VER "0.0.4"
#define SITE "https://www.rankinlawfirm.com"

/* TODO:
 *  look at adding app->status to remove include gtk_statusbar.h
 *  dependency in gtk_filebuf.h.
 */

typedef struct {
    GtkWidget       *window;        /* main window      */
    GtkWidget       *view;          /* text_view        */
    GtkWidget       *statusbar;     /* window statusbar */
    GtkTextTagTable *tagtable;      /* buffer tagtable  */
    gchar           *appname;       /* application name */
    gchar           *appshort;      /* short app name   */
    gchar           *filename;      /* filename w/path  */
    gchar           *fname;         /* filename only    */
    gchar           *fext;          /* file extension   */
    gchar           *fpath;         /* file path        */
    gsize           fsize;          /* file size        */
    guint           open;           /* file opened      */
    gboolean        modified;       /* buffer modified  */
    guint           cid;            /* sb context id    */
    guint           dlgid;          /* dialog id        */

    gint winwidth;
    gint winheight;
    /* text view settings */
    /*
    gint            indent;
    PangoTabArray   *tabs;
    */
    gchar           *fontname;

#ifdef HAVESOURCEVIEW
    GtkSourceBuffer *buffer;
    GtkSourceLanguageManager *langmgr;
    GtkSourceLanguage *language;
    gboolean lineno;
#else
    GtkTextBuffer *buffer;
#endif
    GtkTextMark *cursor;
    gint line;
    gint col;
    gint indent;
    gint indentpl;
    gint indentlevel;
    gint tabstop;
    gint softtab;
    gchar *tabstring;
    gboolean expandtab;
    gboolean overwrite;
    gboolean smartbs;
    gboolean showtabs;
    gboolean dynwrap;
    gboolean showdwrap;
    gboolean smarthe;
    gboolean wraptxtcsr;
    gboolean pgudmvscsr;
    gboolean indentwspc;
    gboolean indentmixd;
    gboolean posixeof;
    gboolean trimendws;
    // gboolean tabkeyindt;    /* TODO: tab key indents */

    /* text view status */
    GtkTextMark     *selstart,      /* selection start/end */
                    *selend;
    gboolean iset;

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

} context;

void context_init (context *app);
void findrep_init (context *app);
void context_destroy (context *app);
void findrep_destroy (context *app);
void app_free_filename (context *app);

#endif
