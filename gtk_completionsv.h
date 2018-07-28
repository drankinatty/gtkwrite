#ifndef __completionsv_h__
#define __completionsv_h__  1

#include "gtk_appdata.h"

#ifdef HAVESOURCEVIEW

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionitem.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>

void create_completion (kwinst *app);

#endif

#endif
