#include "gtk_windef.h"

int main (int argc, char **argv) {

    context *app = NULL;            /* replaced GtkWidget *window */
    app = g_slice_new (context);    /* allocate mem for struct    */
    context_init (app);             /* initialize struct values   */

    gtk_init (&argc, &argv);

    if (!create_window (app)) return 1;

    gtk_main ();

    context_destroy (app);          /* free allocated members   */
    g_slice_free (context, app);    /* free struct memory       */

    return 0;
}
