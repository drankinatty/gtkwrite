TODO:

[x]  Create gtk_common_dlg.h /.c to hold the common dialogs common to all files like
     err_dialog. E.g.:

         err_dialog ("create_replace_dlg() gtk_window_new failure.");

[x]  Look at separating menu from windef.c. Look at gtk/tas/tst_app for version
     with menu separated to tst_menu.c & tst_menu.h. Will require moving
     common functions to gtk_common_dlg.

Search Files:

tmp/searchline*
tmp/srch_words_*
tmp/debug/tosearchin3.c
tmp/str_srchbuf_offset.c

Compile:

    g mygtkapp mygtkapp.c gtk_windef.c gtk_findreplace.c gtk_common_dlg.c

Todo

  gboolean on_keypress (GtkWidget *widget, GdkEventKey *event, context *app)

  fix indent (reset when current col < indent col) or something like that.
  use
    gtk_text_buffer_get_iter_at_line () /* get iter at start of line x */
    (app->line)
    gtk_text_buffer_get_iter_at_mark () /* get iter a current possiiton */
    (should correspond to app->col)
    get text in between iters and check if whitespace
    if whitespace, then gtk_text_buffer_create_tag () /* create indent tag */
        (use indent property and calculate based on indentlevel)
    gtk_text_buffer_apply_tag ()

  font example files:

    dialog_fontsel.c
    gtk-dialog-font.c

  make window size and common settings persistent in ~/.config/gtkedit

  add screen-width (ruler) possibly in separate child window at top of textview
  or new window under menu. Maybe second textview to move an insertion point caret
  dynamically along with the main textview.

  fix/update line/col after paste into buffer. currently it's not updated until
  there is cursor movement and then it appears there are marks or tags that remain
  that will cause an incorrect display. It has to do with the rectangle of the buffer
  shown within the window. If there is no specific line/col set it will default to
  the first line visible in the top of the text veiw window regardless of where the
  insertion point is. It looks like the mouse-wheel is causing the repositioning of
  the line/col as the window is scrolled up.

  rework to simplify status bar update. Decide where filename will go (if at all).

  implement find/replace options, reverse, case insensitive, etc.

  implement replace code.

  prevent 2nd instance of a find/replace window opening.
