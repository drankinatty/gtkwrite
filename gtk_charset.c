#include "gtk_charset.h"

/* Byte Order Mark strings and definitions */
const gchar *bomstr[]  =  { "UTF-8 NoBOM",
                            "UTF-8",
                            "UTF-16 (BE)",
                            "UTF-16 (LE)",
                            "UTF-32 (BE)",
                            "UTF-32 (LE)",
                            "UTF-7",
                            "UTF-7",
                            "UTF-7",
                            "UTF-7",
                            "UTF-7",
                            "UTF-1",
                            "UTF-EBCDIC",
                            "SCSU",
                            "BOCU-1",
                            "GB-18030" };

const guchar bomdef[][6] = {{0x3, 0xff, 0xff, 0xff},    /* dummy mark */
                            {0x3, 0xef, 0xbb, 0xbf},
                            {0x2, 0xfe, 0xff},
                            {0x2, 0xff, 0xfe},
                            {0x4, 0x0,  0x0,  0xfe, 0xff},
                            {0x4, 0xff, 0xfe, 0x0,  0x0},
                            {0x4, 0x2b, 0x2f, 0x76, 0x38},
                            {0x4, 0x2b, 0x2f, 0x76, 0x39},
                            {0x4, 0x2b, 0x2f, 0x76, 0x2b},
                            {0x4, 0x2b, 0x2f, 0x76, 0x2f},
                            {0x4, 0x2b, 0x2f, 0x76, 0x38, 0x2d},
                            {0x3, 0xf7, 0x64, 0x4c},
                            {0x4, 0xdd, 0x73, 0x66, 0x73},
                            {0x3, 0xe,  0xfe, 0xff},
                            {0x3, 0xfb, 0xee, 0x28},
                            {0x4, 0x84, 0x31, 0x95, 0x33}};

/** get BOM from file */
gint buffer_file_get_bom (const gchar *buf, kwinst *app)
{
    guchar bomchk[BOMC] = {0};
    gint i, j,
        n = 0,
        bom = 0;

    for (n = 0; buf[n] && n < BOMC; n++)
        bomchk[n] = buf[n];

    for (i = 0; i < NBOM; i++) {
        for (j = 1; j <= n && j <= (gint)bomdef[i][0]; j++)
            if (bomdef[i][j] != bomchk[j-1])
                goto nextbom;
        bom = i;
        goto bomdone;

        nextbom:;
    }
    bomdone:;

    return (app->bom = bom);
}

