#ifndef _gtk_appdata_h_
#define _gtk_appdata_h_  1

#define MAXLE 32

#include <gtk/gtk.h>

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
    guint           modified;       /* buffer modified  */
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

    GtkTextBuffer *buffer;
    GtkTextMark *cursor;
    gint line;
    gint col;
    gint indent;
    gint indentpl;
    gint indentlevel;
    gint tabspaces;
    gboolean overwrite;

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

} context;

void app_free_filename (context *app);

#endif
