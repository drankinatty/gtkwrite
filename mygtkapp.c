#include "gtk_windef.h"
#include <glib.h>           /* g_fprintf */
#include <glib/gprintf.h>

int main (int argc, char **argv) {

    context *app = NULL;            /* replaced GtkWidget *window */
    app = g_slice_new (context);    /* allocate mem for struct    */
    context_init (app);             /* initialize struct values   */

    // gtk_init (&argc, &argv);        /* terminates if can't initialize */
    if (!gtk_init_check (&argc, &argv)) {   /* same but doesn't exit */
        g_fprintf (stderr, "error: gtk_init failed.\n");
        return 1;
    }

    if (!create_window (app)) {     /* create the main window   */
        g_fprintf (stderr, "error: create_window failed.\n");
        return 1;
    }

    gtk_main ();

    context_destroy (app);          /* free allocated members   */
    g_slice_free (context, app);    /* free struct memory       */

    return 0;
}
