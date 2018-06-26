#ifndef _gtk_charset_h_
#define _gtk_charset_h_  1

#include "gtk_appdata.h"

#define BOMC   8    /* number of characters to check for BOM */
#define NBOM  16    /* number of defined BOM sequences (including None) */

/* Byte Order March strings and definitions
 * bomstr[n] holds the name (e.g. "UTF-8") in a string
 * bomdef[0][n] holds number of bytes in definition, and the
 * remaining bytes defing the mark (e.g. 0xef 0xbb 0xbf)
 */
extern const gchar *bomstr[];
extern const guchar bomdef[][6];

gint buffer_file_get_bom (const gchar *buf, kwinst *app);

#endif
